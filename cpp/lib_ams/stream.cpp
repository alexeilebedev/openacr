// Copyright (C) 2023-2024 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: lib_ams (lib) -- Library for AMS middleware, supporting file format & messaging
// Exceptions: yes
// Source: cpp/lib_ams/stream.cpp
//
// TODOs:
// - Define procedure to:
// spawn a child process
// Child process can be handed an argument, where to start (stream id + seqno + offset).
// register child process as a reading member
// have the child process subscribe to main ctl stream
// have child process open its own ctl stream for writing
// subscribe to child process's ctl stream
// have child process open output file
// subscribe to output file
// After that, we can publish messages to our own CTL stream that cause appropriate effects
// Have the child process subscribe to output. Subscribe to child process's output.
// - Describe message layout in shared memory region
// ---
// WINDOWS NOTE:
// On windows in multi-threaded mode, lib_ams must be accessed from the main thread only.

#include "include/lib_ams.h"

#ifdef WIN32
#include <windows.h>
#endif

// -----------------------------------------------------------------------------

// The length field of a message must reside entirely within one cache line,
// or the sfence/lfence instructions won't behave as expected.
// So, we force each message to start at an address that'a a multiple of machine
// cache line size.
inline u64 AddOffset(u64 offset, int n) {
    return (offset + n + 63) & ~63;
}

inline ams::Seqmsg *MsgAtOffset(lib_ams::FStream &stream, u64 offset) {
    offset = offset & stream.offset_mask;
    return (ams::Seqmsg*)(stream.shm_region.elems + offset);
}

inline void UpdateAckOffset(lib_ams::FStream &stream) {
    stream.next_ackoff = stream.rpos.off + stream.offset_mask / 8;
}

// -----------------------------------------------------------------------------

// Scan /dev/shm for stale stream files and delete them
// A file is stale if it's flockable (i.e. no process has locked it for writing)
// and it's at least 1 hour old
void lib_ams::CleanOldStreamFiles() {
#ifndef WIN32 // not needed on Windows as named shm segments are not persistent
    ind_beg(algo::Dir_curs,entry,"/dev/shm/*.ams") {
        ams::StreamId stream_id;
        int fd=open(Zeroterm(entry.pathname), O_RDONLY);
        if (fd != -1 && algo::ModTime(entry.pathname).value + algo::UNTIME_PER_SEC*3600 < algo::CurrUnTime().value) {
            if (flock(fd,LOCK_EX|LOCK_NB)!=0) {
            } else {
                (void)unlink(Zeroterm(entry.pathname));
            }
        }
        if (fd!=-1) {
            close(fd);
        }
    }ind_end;
#endif
}

// -----------------------------------------------------------------------------

// TODO remove: There is no simple way to do it on Win
// Scan stream/ directory for existing streams
// Create a stream record for each.
void lib_ams::DiscoverStreams() {
#ifndef WIN32
    ind_beg(algo::Dir_curs,entry,"/dev/shm/*.ams") {
        ams::StreamId stream_id;
        // example entry.filename: blah-amstest-0.ctl-0.ams
        algo::strptr prefix = Pathcomp(entry.filename,"-LL");// blah
        algo::strptr name = Pathcomp(entry.filename,"-LR.RL");// amstest-0.ctl-0
        if (prefix == _db.file_prefix && StreamId_ReadStrptrMaybe(stream_id,name)) {
            lib_ams::FStream &stream = lib_ams::ind_stream_GetOrCreate(stream_id);
            (void)stream;
        }
    }ind_end;
#endif
}

// -----------------------------------------------------------------------------

// return TRUE if shared memory region is attached to stream STREAM.
bool lib_ams::ShmemOpenQ(lib_ams::FStream &stream) {
#ifdef WIN32
    bool ret = stream.shm_handle;
#else
    bool ret = ValidQ(stream.shm_file.fd);
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// Open shared memory for reading/writing (as specified in FLAGS)
// and return resulting shared memory region size.
// Fields initialized:
// - stream.filename
// - stream.shm_handle (windows)
// - stream.shm_file (linux)
// - stream.shm_region, if mapped successfully
i64 lib_ams::OpenShmem(lib_ams::FStream &stream, ams::StreamFlags flags) {
    i64 size = 0;
#ifdef WIN32
    // create or open named shared memory segment
    stream.filename = tempstr() << "Global\\" << _db.file_prefix << "-" << stream.stream_id <<".ams";
    int access = write_Get(flags) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
    bool inherit = false;
    // try opening first
    stream.shm_handle = (u8*)OpenFileMapping(access,inherit,Zeroterm(stream.filename));
    // in write mode, try creating if needed
    if (!stream.shm_handle && write_Get(flags)) {
        stream.shm_handle = (u8*)CreateFileMapping(
                                                   INVALID_HANDLE_VALUE,        // use paging file
                                                   NULL,                        // default security
                                                   PAGE_READWRITE,              // read/write access
                                                   0,                           // maximum object size (high-order DWORD)
                                                   lib_ams::_db.shmem_size,     // maximum object size (low-order DWORD)
                                                   Zeroterm(stream.filename));  // name of mapping object (section name, not a file)
    }
    // if the object has been already created CreateFileMapping() returns existing handle
    // and does not change size, GetLastError() indicates ERROR_ALREADY_EXISTS.
    // This helps us to avoid locks.
    bool ok = bool(stream.shm_handle) && (!write_Get(flags) || !GetLastError());
    // map section to virtual memory region
    if (ok) {
        stream.shm_region.elems = (u8*)MapViewOfFile(stream.shm_handle,    // handle of shm section
                                                     access,           // read/write permission
                                                     0,                                                         // file offset high
                                                     0,                                                         // file offset low
                                                     0);                                                        // length - 0 means up to end of file
        ok = stream.shm_region.elems != NULL;
    }
    // determine region size
    if (ok) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(stream.shm_region.elems,&mbi,sizeof mbi)==sizeof mbi) {
            size = mbi.RegionSize;
        }
    }
    // TODO move duplicate code somewhere
    // create a 'working area mask'
    // 1<<mask is a region of size that's a power of two, and has room for at least 2
    // messages beyond it.
    // next message is written at offset `offset & mask` to memory
    ok = ok && i32(size) >= lib_ams::_db.max_msg_size*2;
    // cleanup in case of error - TBD needed?
    if (!ok && stream.shm_region.elems) {
        UnmapViewOfFile(stream.shm_region.elems);
        stream.shm_region.elems = NULL;
    }
    if (!ok && stream.shm_handle) {
        CloseHandle(stream.shm_handle);
        stream.shm_handle = NULL;
    }
#else // linux
    int create_flags = (write_Get(flags) ? (O_RDWR|O_CREAT) : O_RDONLY);
    int mode = write_Get(flags) ? S_IRUSR | S_IWUSR : S_IRUSR;
    // compute filename
    // POSIX says / character in argument to shm_open is implementation-defined
    // Practically, shm_open fails on Linux if / is used.
    // So we treat it as a prefix
    stream.filename = tempstr() << _db.file_prefix << "-" << stream.stream_id <<".ams";
    // create file
    stream.shm_file.fd.value = shm_open(Zeroterm(stream.filename), create_flags, mode);
    bool ok = ValidQ(stream.shm_file.fd);
    // determine stream size
    if (ok) {
        size = algo::GetFileSize(stream.shm_file.fd);
        if (size == 0) {
            size = lib_ams::_db.shmem_size;
            ok = ok && ftruncate(stream.shm_file.fd.value, size)==0;
        }
    }
    if (ok && write_Get(flags)) {
        ok = ok && flock(stream.shm_file.fd.value, LOCK_EX)==0;
    }
    // create a 'working area mask'
    // 1<<mask is a region of size that's a power of two, and has room for at least 2
    // messages beyond it.
    // next message is written at offset `offset & mask` to memory
    ok = ok && i32(size) >= lib_ams::_db.max_msg_size*2;
    // map the region
    if (ok) {
        int prot = write_Get(flags) ? PROT_READ | PROT_WRITE : PROT_READ;
        stream.shm_region.elems = (u8*)mmap(NULL, size, prot, MAP_SHARED, stream.shm_file.fd.value, 0);
        ok = stream.shm_region.elems != NULL;
    }
#endif // win/linux
    return ok ? size : 0;
}

// -----------------------------------------------------------------------------

// Open stream for reading or writing (but not both)
// If shared memory mode (lib_ams::_db.shmem_mode) is set,
//   The file creator sets stream size.
//   Writer creates & locks the file.
//   Reader determines file from file size
// If shared memory mode is not set (stdio mode),
//   the stream is attached to a private memory block of default size.
// When opening for writing:
//   The next message written to the stream will have sequence SEQ and offset OFFSET.
// Default is to use sequence 1, offset 0.
// (However a newly spawned child process can be provided a known sequence & offset by the parent.)
// When opening for reading:
//   begin hot-polling for new messages on the stream
bool lib_ams::OpenStream(lib_ams::FStream &stream, ams::StreamFlags flags, ams::StreamPos pos) {
    // code formatters and scripts do not like flow control interrupted with ifdefs
    if (!ShmemOpenQ(stream)) {
        u64 size = lib_ams::_db.shmem_mode ? OpenShmem(stream,flags) : lib_ams::_db.shmem_size;
        // create a substitute so that messages can always be written
        if (!stream.shm_region.elems) {
            int nadd = size - stream.byteary.ary_n;
            if (nadd) {
                memset(ary_AllocN(stream.byteary, nadd).elems, 0, nadd);
            }
            stream.shm_region.elems = stream.byteary.ary_elems;
        }
        stream.offset_mask = (1 << algo::FloorLog2(size - lib_ams::_db.max_msg_size*2)) - 1;
        stream.shm_region.n_elems = size;
        // ensure the acks are sent frequently enough
        UpdateAckOffset(stream);
        if (size) {
            stream.flags = flags;
            stream.rpos = pos;
            stream.wpos = pos;
        }
        if (read_Get(stream.flags) || write_Get(stream.flags)) {
            cd_stream_hb_Insert(stream); // begin publishing heartbeats for stream
        }
        UpdateBudget(stream);
        if (read_Get(stream.flags) && lib_ams::_db.dflt_stream_id == ams::StreamId()) {
            lib_ams::_db.dflt_stream_id = stream.stream_id;
        }
        if (read_Get(stream.flags)) {
            cd_poll_read_Insert(stream);
        }
        if (ch_N(lib_ams::_db.regx_trace.expr)) {
            ApplyTrace(stream,lib_ams::_db.regx_trace,true);
        }
    }
    verblog(lib_ams::_db.proc_id
            <<"  openstream"
            <<Keyval("stream_id",stream.stream_id)
            <<Keyval("flags",flags)
            <<Keyval("filename",stream.filename)
            <<Keyval("success",Bool(stream.flags.value == flags.value)));
    return stream.flags.value == flags.value;
}

bool lib_ams::OpenStream(lib_ams::FStream &stream, ams::StreamFlags flags) {
    return lib_ams::OpenStream(stream,flags,ams::StreamPos());
}

// TBD not sure if this function is needed in non-fork() environment
bool lib_ams::ReopenForReading(lib_ams::FStream &stream) {
    bool ret=false;
    if (write_Get(stream.flags)) {
        write_Set(stream.flags,false);
        read_Set(stream.flags,true);
        stream.limit=0;
#ifndef WIN32
        flock(stream.shm_file.fd.value,LOCK_UN);
#endif
        // change protection to readonly
#ifdef WIN32
        DWORD old;
        if (!VirtualProtect(stream.shm_region.elems, stream.shm_region.n_elems, FILE_MAP_READ, &old)) {
            int code = GetLastError();
            prlog("VirtualProtect() error, code "<<code);
        }
#else // linux
        if (mprotect(stream.shm_region.elems,stream.shm_region.n_elems,PROT_READ)!=0) {
            prlog("mprotect() error: "<< strerror(errno));
        }
#endif
        cd_poll_read_Insert(stream);
        UpdateBudget(stream);
        ret=true;
    }
    return ret;
}

// Update budget for stream STREAM
// Return TRUE if the stream is potentially unblocked for
// writing due to WRITELIMIT being increased.
// (WRITELIMIT is the point beyond which no message can be written
// because doing so would overwrite data not yet consumed by one of the read members.)
// TODO: do not update budget for a stream that is not opened for writing
bool lib_ams::UpdateBudget(lib_ams::FStream &stream) {
    bool ret=false;
    u64 min_roff=ULLONG_MAX/2; // some huge number for which you can still do arithmetic
    ind_beg(lib_ams::stream_zd_member_bystream_curs,member,stream) {
        if (member.member.mode == ams_Member_mode_r) {
            u64_UpdateMin(min_roff,member.pos.off);
        }
    }ind_end;
    u64 new_offset = min_roff + (stream.offset_mask+1) - lib_ams::_db.max_msg_size*2;
    ret = u64_Update(stream.limit, new_offset);
    return ret;
}

// Update stream budgets
void lib_ams::UpdateBudgets() {
    ind_beg(lib_ams::_db_stream_curs,stream,lib_ams::_db) {
        UpdateBudget(stream);
    }ind_end;
}

// Register a bad, unskippable incoming message
// on STREAM, report it, and stop reading.
void lib_ams::WriterError(lib_ams::FStream &stream, ams::Seqmsg *msg) {
    if (bool_Update(stream.writer_error,true)) {
        prlog(lib_ams::_db.proc_id
              <<"  lib_ams.read_error"
              <<Keyval("expected_seqmsg_id",tempstr()<<stream.stream_id<<"@"<<stream.rpos)
              <<Keyval("seqmsg_id",msg->seqmsg_id)
              <<Keyval("msg_type",msg->type)
              <<Keyval("msg_length",msg->length)
              <<Keyval("msg_offset_in_shm_region",u64(msg)-u64(stream.shm_region.elems))
              <<Keyval("comment","Bad message detected in stream. Reading stopped"));
        StopReading(stream);
    }
}

// If the stream is open for reading, check to see if a message
// is available.
// If it is available, return pointer to message.
ams::Seqmsg *lib_ams::PeekMsg(lib_ams::FStream &stream) {
    if (!stream.cur_msg) {
        ams::Seqmsg *msg = MsgAtOffset(stream,stream.rpos.off);
        int L = msg->length;
        if (L >= ssizeof(ams::Seqmsg)) {
            lfence();
            if (msg->type == ams_MsgHeader_type_ams_Seqmsg && msg->seqmsg_id.stream_id == stream.stream_id && msg->seqmsg_id.seq == stream.rpos.seq) {
                stream.cur_msg = msg;
            } else {
                WriterError(stream,msg);
            }
        } else if (L > 0) {
            WriterError(stream,msg);
        }
    }
    return stream.cur_msg;
}

// Called by the client
// to aoivd reading current message
void lib_ams::StopReading(lib_ams::FStream &stream) {
    stream.cur_msg=NULL;
    cd_poll_read_Remove(stream);
}

// Mark current message as read and move to the next message.
void lib_ams::SkipMsg(lib_ams::FStream &stream) {
    if (ams::Seqmsg *msg=stream.cur_msg) {
        stream.rpos.seq++;
        stream.rpos.off = AddOffset(stream.rpos.off, stream.cur_msg->length);
        stream.cur_msg = NULL;
        // #AL# NOTE: tracing of read messages occurs after any outputs
        // generated by those messages.
        // Moving TraceMsg to pre-dispatch will break expect (since it will match its
        // own trace).
        if (UNLIKELY(trace_read_Get(stream.flags))) {
            lib_ams::TraceMsg(stream,Castbase(*msg),true);
        }
        // Force a report (StreamHb) every N messages
        if (stream.rpos.off >= stream.next_ackoff) {
            UpdateAckOffset(stream);
            SendHb(stream);
        }
    }
}

// Send heartbeat to control stream
// Update
void lib_ams::SendHb(lib_ams::FStream &stream) {
    if (lib_ams::_db.c_stream_ctl && lib_ams::_db.shmem_mode) {
        // report on this stream's reading progress
        if (read_Get(stream.flags)) {
            lib_ams::StreamHbMsg_FmtAmsStream(*lib_ams::_db.c_stream_ctl,ams::Member(_db.proc_id,stream.stream_id,ams_Member_mode_r),stream.rpos,0);
        }
        // report on this stream's writing progress
        if (write_Get(stream.flags)) {
            u32 wbudget = u64_Min(u64_Max(stream.limit, stream.wpos.off) - stream.wpos.off, 0x7fffffff);
            lib_ams::StreamHbMsg_FmtAmsStream(*lib_ams::_db.c_stream_ctl,ams::Member(_db.proc_id,stream.stream_id,ams_Member_mode_w),stream.wpos,wbudget);
        }
    }
}

// Publish stream heartbeats to c_stream_ctl stream
void lib_ams::cd_stream_hb_Step() {
    lib_ams::FStream &stream = *cd_stream_hb_RotateFirst();
    SendHb(stream);
}

// Check all streams (that are not already readable) for readability.
// If readable, call h_msg hook.
void lib_ams::cd_poll_read_Step() {
    int n=cd_poll_read_N();
    for (int i=0; i<n; i++) {
        lib_ams::FStream *stream=cd_poll_read_RotateFirst();
        for (int j=0; j < stream->burst; ++j) {
            if (ams::Seqmsg *seqmsg = PeekMsg(*stream)) {
                if (ams::MsgHeader *payload = payload_Get(*seqmsg)) {
                    h_msg_Call(*stream,*payload);
                }
                if (stream->cur_msg) {
                    SkipMsg(*stream);
                } else {
                    break;// user called StopReading, finish burst early
                }
            } else {
                if (!lib_ams::_db.shmem_mode) {
                    // private (stdio) mode: stop polling stream
                    cd_poll_read_Remove(*stream);
                }
                break;
            }
        }
    }
}

// If STREAM is out of write space, and stream is set to blocking mode,
// poll indefinitely all control streams (zd_ctlin list) for heartbeats
// that indicate that the stream write space is finally available
// (This happens when all stream readers read enough data so that some can be discarded,
// or all readers are eliminated.)
// Return TRUE if writing can proceed
inline static bool WaitBudget(lib_ams::FStream &stream) {
    bool ret = stream.wpos.off < stream.limit;
    if (!ret && !nonblock_Get(stream.flags)) {
        stream.n_write_block++;
        do {
            lib_ams::_db.trace.n_write_block_spin++;
            lib_ams::PollCtlIn();
            ret = stream.wpos.off < stream.limit;
        } while (!ret);
    }
    return ret;
}

void lib_ams::PollCtlIn() {
    ind_beg(lib_ams::_db_zd_ctlin_curs,ctlin,lib_ams::_db) {
        if (ams::Seqmsg *seqmsg = PeekMsg(ctlin)) {
            if (ams::MsgHeader *payload = payload_Get(*seqmsg)) {
                h_msg_Call(ctlin,*payload);
            }
            SkipMsg(ctlin);
        }
    }ind_end;
}

// Begin writing message of length LENGTH
// Used by WriteMsg and with amc's pnew (acr pnew) for zero-copy sends.
// If successful, return pointer to region of size LENGTH whre the mssage can be written,
// otherwise return NULL.
void *lib_ams::BeginWrite(lib_ams::FStream &stream, int length) {
    void *ret = NULL;
    int L = ssizeof(ams::Seqmsg) + length;
    if (L <= _db.max_msg_size) {
        if (WaitBudget(stream)) {
            ams::Seqmsg *seqmsg = MsgAtOffset(stream,stream.wpos.off);
            ret = seqmsg+1;
        }
    }
    return ret;
}

// Finish writing message of length LENGTH,
// and send the result.
void lib_ams::EndWrite(lib_ams::FStream &stream, void *ptr, int len) {
    ams::Seqmsg *seqmsg = (ams::Seqmsg*)((u8*)ptr - ssizeof(ams::Seqmsg));
    int L = ssizeof(ams::Seqmsg) + len;
    seqmsg->seqmsg_id.stream_id = stream.stream_id;
    seqmsg->seqmsg_id.seq = stream.wpos.seq;
    seqmsg->seqmsg_id.off = stream.wpos.off;
    seqmsg->type = ams_MsgHeader_type_ams_Seqmsg;
    seqmsg->tsc = algo::get_cycles();
    stream.wpos.seq++;
    stream.wpos.off = AddOffset(stream.wpos.off, L);
    ams::Seqmsg *next = MsgAtOffset(stream,stream.wpos.off);
    next->length = 0;// length 0 means the message isn't ready
    sfence();
    seqmsg->length = L;// mark message as ready!
    // copy message to output stream
    if (trace_write_Get(stream.flags)) {
        TraceMsg(stream,Castbase(*seqmsg),false);
    }
    // if stream is being both written and read by this process
    // trigger reading
    if (read_Get(stream.flags)) {
        cd_poll_read_Insert(stream);
    }
}

// If the stream is attached to a writable shared memory segment,
// write sequenced message to stream STREAM, and increment sequence number
// and stream offset.
// Message must be within max. message size
// If the stream is not configured for writingIf an output fd is associated with the stream, hthe stream, write
bool lib_ams::WriteMsg(lib_ams::FStream &stream, ams::MsgHeader &msg) {
    bool ret=false;
    int len = msg.length;
    if (void *ptr = lib_ams::BeginWrite(stream,msg.length)) {
        memcpy(ptr, &msg, msg.length);
        lib_ams::EndWrite(stream,ptr,len);
        ret=true;
    }
    return ret;
}

// Write message MSG to output, either in binary or text mode.
// Return success status
// If ISREAD flag is set, this is a copy of a message that has just been read
// from another stream. Otherwise, it's a copy of a message that's just been written.
// In stdin mode, where messages are read from ascii input and posted to the
void lib_ams::TraceMsg(lib_ams::FStream &stream, ams::MsgHeader &msg, bool isread) {
    if (!write_err_Get(stream.flags)) {
        if (trace_text_Get(stream.flags)) {// write as text
            ams::Seqmsg *seqmsg = ams::Seqmsg_Castdown(msg);
            ams::MsgHeader *printmsg = &msg;
            if (seqmsg) {
                ams::SeqmsgTrace *msgtrace
                    = ams::SeqmsgTrace_FmtByteAry(_db.fmt_buf
                                                  ,lib_ams::_db.proc_id
                                                  ,(isread?ams_SeqmsgTrace_mode_r:ams_SeqmsgTrace_mode_w)
                                                  ,seqmsg->seqmsg_id
                                                  ,seqmsg->tsc
                                                  ,payload_Get(*seqmsg));
                printmsg = &ams::Castbase(*msgtrace);
            }
            tempstr out;
            ams::MsgHeaderMsgs_Print(out,*printmsg,printmsg->length);
            out << "\n";
            (void)algo::WriteFile(algo::Fildes(1), (u8*)out.ch_elems, out.ch_n);
            // make sure input messages are not scanned for output strings...
            if (!isread) {
                lib_ams::ExpectSaveOutput(out);
            }
        } else {// write as binary
            (void)algo::WriteFile(algo::Fildes(1), (u8*)&msg, msg.length);
        }
    }
}

// Initialize stream library
// FILE_PREFIX: unique filesystem prefix for all subsequently created streams
// Also clean any stale (unlocked) stream files.
//    (this can be disabled by setting _db.stream_files_cleaned to true)
bool lib_ams::Init(algo::strptr file_prefix, ams::ProcId proc_id) {
    bool shmem_mode = file_prefix != "";
    lib_ams::_db.file_prefix = file_prefix;
    lib_ams::_db.proc_id = proc_id;
    if (bool_Update(lib_ams::_db.stream_files_cleaned,true)) {
        lib_ams::CleanOldStreamFiles();
    }
    lib_ams::_db.shmem_mode = shmem_mode;
    if (shmem_mode) {
    } else {
        BeginReadStdin();
    }
    bool ok = true;
    // in text mode, enable tracing on all streams unless customized by user
    if (!shmem_mode && _db.regx_trace.expr == "") {
        Regx_ReadSql(_db.regx_trace, "%", true);
    }
    // not strictly speaking required but latency will not be good
    // without this succeeding
    // #AL# TODO -- add a flag to allow this
    // bool lock_ok = mlockall(MCL_CURRENT | MCL_FUTURE)==0;
    // if (!lock_ok) {
    //     prlog(lib_ams::_db.proc_id<<"  WARNING mlockall failed; expect high latency");
    // }
    return ok;
}

void lib_ams::Uninit() {
    lib_ams::_db.c_stream_ctl=NULL;
    stream_RemoveAll();
}

void lib_ams::shm_handle_Cleanup(lib_ams::FStream &stream) {// fcleanup:lib_ams.FStream.handle_file
#ifdef WIN32
    // unmap section from process address space for reader and writer as well
    if (stream.shm_region.elems) {
        UnmapViewOfFile(stream.shm_region.elems);
        Refurbish(stream.shm_region);
    }
    // close named section
    // note that section will be actually removed along with last reference to it
    if (stream.shm_handle) {
        CloseHandle(stream.shm_handle);
        stream.shm_handle = NULL;
    }
#else
    (void)stream;
#endif
}

void lib_ams::shm_file_Cleanup(lib_ams::FStream &stream) {// fcleanup:lib_ams.FStream.shm_file
#ifndef WIN32
    if (write_Get(stream.flags)) {
        if (ValidQ(stream.shm_file.fd)) {
            (void)flock(stream.shm_file.fd.value, LOCK_UN);
        }
        if (stream.filename != "") {
            shm_unlink(Zeroterm(stream.filename));
            stream.filename = "";
        }
    }
#else
    (void)stream;
#endif
}

// -----------------------------------------------------------------------------

// (This message could be unnecessary)
void lib_ams::CtlMsg_ProcAddMsg(ams::ProcAddMsg &msg) {
    lib_ams::FProc &proc = ind_proc_GetOrCreate(msg.proc_id);
    (void)proc;
}

// A process has exited
// Remove any read/write members contributed by the process and
// potentially unblock some write streams
void lib_ams::CtlMsg_ProcRemoveMsg(ams::ProcRemoveMsg &msg) {
    lib_ams::FProc *proc = ind_proc_Find(msg.proc_id);
    if (proc) {
        proc_Delete(*proc);// this deletes all members
        UpdateBudgets();
    }
}

void lib_ams::CtlMsg_DumpStreamTableMsg(ams::DumpStreamTableMsg &msg) {
    if (msg.proc_id == ams::ProcId() || msg.proc_id == lib_ams::_db.proc_id) {
        lib_ams::DumpStreamTable();
    }
}

void lib_ams::CtlMsg_StreamHbMsg(ams::StreamHbMsg &msg) {
    lib_ams::FMember *member = lib_ams::ind_member_Find(msg.member);
    if (!member) {
        // TODO: require ProcAdd or create procs implicitly?
        // create pre-requisites
        lib_ams::ind_proc_GetOrCreate(msg.member.proc_id);
        lib_ams::ind_stream_GetOrCreate(msg.member.stream_id);
        member=&lib_ams::member_Alloc();
        member->member=msg.member;
        if (!member_XrefMaybe(*member)) {
            member_Delete(*member);
            member=NULL;
        }
    }
    if (member) {
        member->pos  = msg.pos;
        member->budget = msg.wbudget;
        member->last_hb = algo::CurrUnTime();
        if (UpdateBudget(*member->p_stream)) {
            ind_beg(_db_fdin_curs,fdin,lib_ams::_db) {
                if (!cd_fdin_read_InLlistQ(fdin) && ch_N(_db.expect_str)==0) {
                    cd_fdin_read_Insert(fdin);
                }
            }ind_end;
        }
    }
}

void lib_ams::ReadCtlMsg(lib_ams::FStream &, ams::MsgHeader &msg) {
    lib_ams::CtlMsgDispatch(msg);
}

// Read next input line from stdin
void lib_ams::cd_fdin_read_Step() {
    lib_ams::FFdin &fdin = *cd_fdin_read_RotateFirst();
    algo::strptr msgstr = in_GetMsg(fdin);
    if (msgstr.elems) {
        bool processed=true;
        lib_ams::FStream *tgt_stream = NULL;
        ams::MsgHeader *tgt_msg = NULL;
        ary_RemoveAll(_db.fdin_buf);
        (void)ams::MsgHeaderMsgs_ReadStrptr(msgstr, _db.fdin_buf);
        // Save input line verbatim if it doesn't correspond to any message
        // TODO: check length
        if (!_db.fdin_buf.ary_n) {
            lib_ams::InputLineMsg_FmtByteAry(_db.fdin_buf, msgstr);
        }
        // figure out what kind of messages was provided on input
        // it could be an explicit Seqmsg
        // or, for any other message, we post it to dflt_stream_id
        // (previously set with DfltStream message)
        ams::Seqmsg *seqmsg = ams::Seqmsg_Castdown(*(ams::MsgHeader*)_db.fdin_buf.ary_elems);
        if (seqmsg) {
            tgt_stream = lib_ams::ind_stream_Find(seqmsg->seqmsg_id.stream_id);
            if (tgt_stream) {
                if (seqmsg->seqmsg_id.seq == 0) {// allow user not to guess
                    seqmsg->seqmsg_id.seq = tgt_stream->wpos.seq;
                }
                if (seqmsg->seqmsg_id.seq == tgt_stream->wpos.seq) {
                    tgt_msg = payload_Get(*seqmsg);
                }
            }
        } else {
            tgt_stream = lib_ams::ind_stream_Find(lib_ams::_db.dflt_stream_id);
            tgt_msg = (ams::MsgHeader*)_db.fdin_buf.ary_elems;
        }
        if (tgt_msg && tgt_stream && read_Get(tgt_stream->flags)) {
            if (WriteMsg(*tgt_stream,*tgt_msg)) {
                _db.trace.n_fdin_posted++;
            } else {
                // stop reading stdin - no progress is possible
                _db.trace.n_fdin_pushback++;
                verblog(lib_ams::_db.proc_id<<" streamblocked "<<tgt_stream->stream_id);
                cd_fdin_read_Remove(fdin);
                processed=false;
            }
        }
        // #AL# if input message has been dropped, help debug this situation
        if (!tgt_stream) {
            _db.trace.n_fdin_drop_notgt++;
            verblog("# lib_ams.drop_msg"
                    <<"  "<<lib_ams::_db.proc_id
                    <<Keyval("message",msgstr)
                    <<Keyval("comment","there is nowhere to post it"));
        }
        if (processed) {
            // advance
            in_SkipMsg(fdin);
        }
    }
}

// Stop reading stdin
void lib_ams::cd_fdin_eof_Step() {
    fdin_RemoveAll();
    // remove sources of periodic actions
    // so that the process may exit
    cd_stream_hb_RemoveAll();
    cd_poll_read_RemoveAll();
}

// Begin reading ams control messages from stdin
void lib_ams::BeginReadStdin() {
    lib_ams::FFdin &fdin = fdin_Alloc();
    fdin_XrefMaybe(fdin);
    algo::Fildes fd(0);
    algo::SetBlockingMode(fd,false);
    in_BeginRead(fdin,fd);
}

lib_ams::FStream &lib_ams::ind_stream_GetOrCreate(ams::StreamId stream_id) {
    lib_ams::FStream* ret = lib_ams::ind_stream_Find(stream_id);
    if (!ret) {
        ret         = &stream_Alloc();
        (*ret).stream_id = stream_id;
        bool good = stream_XrefMaybe(*ret);
        if (!good) {
            stream_RemoveLast(); // delete offending row, any existin xrefs are cleared
            ret = NULL;
        }
    }
    vrfy(ret,tempstr()<<"lib_ams.create_error  table:ind_stream  key:'"<<stream_id<<"'  comment:'bad xref'");
    return *ret;
}

void lib_ams::DumpStreamTable() {
    // global counter
    prlog(Keyval("n_write_block_spin",lib_ams::_db.trace.n_write_block_spin));
    ind_beg(lib_ams::_db_stream_curs,stream,lib_ams::_db) {
        prlog(lib_ams::_db.proc_id
              <<"  stream"
              <<Keyval("stream_id",stream.stream_id)
              <<Keyval("rpos",stream.rpos)
              <<Keyval("wpos",stream.wpos)
              <<Keyval("writelimit",stream.limit)
              <<Keyval("flags",stream.flags)
              <<Keyval("filename",stream.filename)
              <<Keyval("fd",stream.shm_file.fd.value)
              <<Keyval("handle",stream.shm_handle)
              <<Keyval("size",stream.shm_region.n_elems)
              <<Keyval("n_write_block",stream.n_write_block)
              <<Keyval("nbits",algo::FloorLog2(u64(stream.offset_mask+1)))
              );
        ind_beg(lib_ams::stream_zd_member_bystream_curs,member,stream) {
            prlog(lib_ams::_db.proc_id
                  <<"  member"
                  <<Keyval("member",member.member)
                  <<Keyval("pos",member.pos)
                  <<Keyval("budget",member.budget));
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Close stream.
// If stream is opened for writing, remove its file.
// BUG: I think if the stream is opened for reading, the mmap() leaks
void lib_ams::Close(lib_ams::FStream &stream) {
    shm_file_Cleanup(stream);
    shm_handle_Cleanup(stream);
    algo::Refurbish(stream.shm_region);
    algo::Refurbish(stream.shm_file);
    read_Set(stream.flags,false);
    write_Set(stream.flags,false);
    cd_stream_hb_Remove(stream);
    cd_poll_read_Remove(stream);
}

// -----------------------------------------------------------------------------

void lib_ams::SetDfltShmSize(u32 size) {
    size = i32_Max(size, lib_ams::_db.max_msg_size*2);
    size = 1 << algo::CeilingLog2(size);
    lib_ams::_db.shmem_size = size + lib_ams::_db.max_msg_size*2;
}

// -----------------------------------------------------------------------------

ams::StreamPos lib_ams::ReadStreamPos(algo::strptr val) {
    ams::StreamPos stream_pos;
    StreamPos_ReadStrptrMaybe(stream_pos, val);
    if (stream_pos.seq==0) {
        stream_pos.seq=1;
        stream_pos.off=0;
    }
    return stream_pos;
}

// Register PROC_ID as reader of stream STREAM_ID starting at offset 0
// This immediately limits the write budget of STREAM_ID
//    (Subsequent attempt to overwrite data not yet consumed by PROC_ID causes
//     either reject or blocking wait)
void lib_ams::AddReadMember(ams::ProcId proc_id, ams::StreamId stream_id) {
    ams::StreamHbMsg msg(ams::Member(proc_id,stream_id,0),ams::StreamPos(),0);
    lib_ams::CtlMsg_StreamHbMsg(msg);
}

// Open stream STREAM for reading and add it to the list of streams
// which lib_ams scans for messages (i.e. control streams).
// Attach lib_ams message dispatch to the stream
bool lib_ams::AddCtlIn(lib_ams::FStream &stream) {
    bool ret = OpenStream(stream,ams_StreamFlags_read);
    h_msg_Set2(stream,stream,lib_ams::ReadCtlMsg);
    if (read_Get(stream.flags)) {
        zd_ctlin_Insert(stream);
    }
    return ret;
}

// -----------------------------------------------------------------------------

void lib_ams::CloseAllStreams() {
    ind_beg(_db_stream_curs,stream,_db) {
        Close(stream);
    }ind_end;
    stream_RemoveAll();
    _db.c_stream_ctl=NULL;
}

// -----------------------------------------------------------------------------

// Match trace expression REGX against stream STREAM.
// If it matches, enable/disable tracing as indicated by ENABLE
// For full description see txt/trace.md
bool lib_ams::ApplyTrace(lib_ams::FStream &stream, algo_lib::Regx &regx, bool enable) {
    bool match=false;
    if (Regx_Match(regx,tempstr() << lib_ams::_db.proc_id << "/" << stream.stream_id)) {
        trace_read_Set(stream.flags,enable);
        trace_write_Set(stream.flags,enable);
        trace_text_Set(stream.flags,enable);
        match=true;
    } else {
        if (Regx_Match(regx,tempstr() << lib_ams::_db.proc_id << "/" << stream.stream_id << ",r")) {
            trace_read_Set(stream.flags,enable);
            trace_text_Set(stream.flags,enable);
            match=true;
        }
        if (Regx_Match(regx,tempstr() << lib_ams::_db.proc_id << "/" << stream.stream_id << ",w")) {
            trace_write_Set(stream.flags,enable);
            trace_text_Set(stream.flags,enable);
            match=true;
        }
        if (Regx_Match(regx,tempstr() << lib_ams::_db.proc_id << "/" << stream.stream_id << ",rb")) {
            trace_read_Set(stream.flags,enable);
            trace_text_Set(stream.flags,false);
            match=enable;
        }
        if (Regx_Match(regx,tempstr() << lib_ams::_db.proc_id << "/" << stream.stream_id << ",wb")) {
            trace_write_Set(stream.flags,enable);
            trace_text_Set(stream.flags,false);
            match=true;
        }
    }
    // input messages get written to dflt stream
    // this causes them to get logged twice
    if (lib_ams::_db.dflt_stream_id == stream.stream_id && trace_read_Get(stream.flags)) {
        trace_write_Set(stream.flags,false);
    }
    return match;
}

// Enable or disable logcat tracing based on regex WHAT
// If TRACE is specified, logcats matching regex are enabled; otherwise
// they are disabled.
// This affects all future `prlog_cat` calls.
// Return number of matches
int lib_ams::ApplyTrace(algo::strptr what, bool enable) {
    algo_lib::Regx name_regx;
    Regx_ReadSql(name_regx,what,true);
    if (enable) {
        Regx_ReadSql(lib_ams::_db.regx_trace, what, true);
    }
    int nmatch=0;
    ind_beg(algo_lib::_db_logcat_curs,logcat,algo_lib::_db) if (!logcat.builtin) {
        if (algo_lib::Regx_Match(name_regx, logcat.logcat)) {
            nmatch++;
            prlog(logcat.logcat << (enable ? " enabled" : " disabled"));
            logcat.enabled = enable;
        }
    }ind_end;
    ind_beg(lib_ams::_db_stream_curs,stream,lib_ams::_db) {
        nmatch += lib_ams::ApplyTrace(stream,name_regx,enable);
    }ind_end;
    return nmatch;
}


// Same as ApplyTrace, but
// If expression doesn't match anything, print a helpful message for the user
void lib_ams::ApplyTraceV(algo::strptr what, bool enable) {
    int nmatch = ApplyTrace(what,enable);
    if (nmatch==0) {
        prlog("Trace expression didn't match anything");
        prlog("Try 'show logcat', 'show stream' or select one of the following categories");
        algo::ListSep ls;
        cstring out;
        int breakat=80;
        ind_beg(algo_lib::_db_logcat_curs,logcat,algo_lib::_db) if (!logcat.builtin) {
            out << ls << logcat.logcat;
            if (ch_N(out) > breakat) {
                out << eol;
                breakat = ch_N(out) + 80;
            }
        }ind_end;
        ind_beg(lib_ams::_db_stream_curs,stream,lib_ams::_db) {
            out << ls << stream.stream_id;
        }ind_end;
        prlog(out);
    }
}

// prlog hook for ams application, could be used for sending log messages to output stream
void lib_ams::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) {
    i32 max_payload_size = lib_ams::_db.max_msg_size - sizeof(ams::Seqmsg) - sizeof(ams::LogMsg);
    if (UNLIKELY(logcat == &algo_lib_logcat_expect || !lib_ams::_db.c_stream_out)) {
        // force output to stdout to avoid interaction between expect implementation
        // and its output
        algo::Prlog(logcat,tstamp,str);
    } else {
        while (ch_N(str)) {
            lib_ams::LogMsg_FmtAmsStream(*lib_ams::_db.c_stream_out
                                         , logcat->logcat
                                         , tstamp
                                         , ch_FirstN(str,max_payload_size));
            str = ch_RestFrom(str,max_payload_size);
        }
    }
}
