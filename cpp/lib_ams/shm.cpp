// Copyright (C) 2023-2026 AlgoRND
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
// Source: cpp/lib_ams/shm.cpp
//

#include "include/lib_ams.h"

#ifdef WIN32
#include <windows.h>
#endif

// -----------------------------------------------------------------------------

// Scan /dev/shm for stale shm files and delete them
// A file is stale if it's flockable (i.e. no process has locked it for writing)
// and it's at least 15 seconds old
void lib_ams::CleanOldShmFiles() {
#ifndef WIN32 // not needed on Windows as named shm segments are not persistent
    ind_beg(algo::Dir_curs,entry,"/dev/shm/*.ams") {
        ams::ShmId shm_id;
        int fd=open(Zeroterm(entry.pathname), O_RDONLY);
        if (fd != -1 && algo::ModTime(entry.pathname).value + algo::UNTIME_PER_SEC*15 < algo::CurrUnTime().value) {
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

// return TRUE if shared memory region is attached to shm SHM.
bool lib_ams::ShmemFdOpenQ(lib_ams::FShm &shm) {
#ifdef WIN32
    bool ret = shm.shm_handle;
#else
    bool ret = ValidQ(shm.shm_file.fd);
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// Open shared memory for reading/writing (as specified in FLAGS)
// and return success status
// Fields initialized:
// - shm.filename
// - shm.shm_handle (windows)
// - shm.shm_file (linux)
// - shm.shm_region, if mapped successfully
// If the segment is opened for writing, it is flocked.
bool lib_ams::ShmemOpenFile(lib_ams::FShm &shm, ams::ShmFlags flags) {
    i64 size = 0;
    bool ok = false;
#ifdef WIN32
    // create or open named shared memory segment
    shm.filename = tempstr() << "Global\\" << _db.file_prefix << "-" << shm.shm_id <<".ams";
    int access = write_Get(flags) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
    bool inherit = false;
    // try opening first
    shm.shm_handle = (u8*)OpenFileMapping(access,inherit,Zeroterm(shm.filename));
    // in write mode, try creating if needed
    if (!shm.shm_handle && write_Get(flags)) {
        shm.shm_handle = (u8*)CreateFileMapping(
                                                INVALID_HANDLE_VALUE,        // use paging file
                                                NULL,                        // default security
                                                PAGE_READWRITE,              // read/write access
                                                0,                           // maximum object size (high-order DWORD)
                                                lib_ams::_db.shmem_size,     // maximum object size (low-order DWORD)
                                                Zeroterm(shm.filename));  // name of mapping object (section name, not a file)
    }
    // if the object has been already created CreateFileMapping() returns existing handle
    // and does not change size, GetLastError() indicates ERROR_ALREADY_EXISTS.
    // This helps us to avoid locks.
    ok = bool(shm.shm_handle) && (!write_Get(flags) || !GetLastError());
    // map section to virtual memory region
    if (ok) {
        shm.shm_region.elems = (u8*)MapViewOfFile(shm.shm_handle,    // handle of shm section
                                                  access,           // read/write permission
                                                  0,                                                         // file offset high
                                                  0,                                                         // file offset low
                                                  0);                                                        // length - 0 means up to end of file
        ok = shm.shm_region.elems != NULL;
    }
    // determine region size
    if (ok) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(shm.shm_region.elems,&mbi,sizeof mbi)==sizeof mbi) {
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
    if (!ok && shm.shm_region.elems) {
        UnmapViewOfFile(shm.shm_region.elems);
        shm.shm_region.elems = NULL;
    }
    if (!ok && shm.shm_handle) {
        CloseHandle(shm.shm_handle);
        shm.shm_handle = NULL;
    }
#else // linux
    int create_flags = (write_Get(flags) ? (O_RDWR|O_CREAT) : O_RDONLY);
    int mode = write_Get(flags) ? S_IRUSR | S_IWUSR : S_IRUSR;
    nonblock_Set(shm.flags,nonblock_Get(flags));
    // POSIX says / character in argument to shm_open is implementation-defined
    // Practically, shm_open fails on Linux if / is used.
    shm.filename = tempstr() << _db.file_prefix  << (_db.file_prefix=="" ? "" : "-") << shm.shm_id << ".ams";
    if (_db.file_prefix == "") {
        size = lib_ams::_db.shmem_size;
        shm.shm_region = algo::memptr((u8*)algo_lib::lpool_AllocMem(lib_ams::_db.shmem_size),size);
        shm.created=true;
    } else {
        // create file
        shm.shm_file.fd.value = shm_open(Zeroterm(shm.filename), create_flags, mode);
        ok = ValidQ(shm.shm_file.fd);
        // determine shm size
        if (ok) {
            size = algo::GetFileSize(shm.shm_file.fd);
            if (size == 0) {
                shm.created=true;
                size = lib_ams::_db.shmem_size;
                ok = ok && ftruncate(shm.shm_file.fd.value, size)==0;
            }
        }
        ok = ok && i32(size) >= lib_ams::_db.max_msg_size*2;
        // map the region
        if (ok) {
            int prot = write_Get(flags) ? PROT_READ | PROT_WRITE : PROT_READ;
            shm.shm_region = algo::memptr((u8*)mmap(NULL, size, prot, MAP_SHARED, shm.shm_file.fd.value, 0), size);
        }
    }
    if (shm.created) {
        memset(shm.shm_region.elems,0,size);// touch all bytes
    }
    // 1<<mask is a region of size that's a power of two, and has room for at least 2 messages beyond it.
    // next message is written at offset `offset & mask` to memory
    shm.offset_mask = (1 << algo::FloorLog2(u32(size) - lib_ams::_db.max_msg_size*2)) - 1;
    ok = shm.shm_region.elems != NULL;
#endif // win/linux
    return ok;
}

// -----------------------------------------------------------------------------

// Open shm for reading or writing (or both)
// The file creator sets shm size.
// Writer creates & locks the file.
// Reader determines file from file size
// When opening for writing:
//   The next message written to the shm will have OFFSET as determined by POS
// Initial value of offset is 0
// (However a newly spawned child process can be provided a known sequence & offset by the parent.)
// When opening for reading:
//   begin hot-polling for new messages on the shm
bool lib_ams::OpenShm(lib_ams::FShm &shm, ams::ShmFlags flags, u64 pos DFLTVAL(0)) {
    bool ok =true;
    if (!ShmemFdOpenQ(shm)) {
        ok = ShmemOpenFile(shm,flags);
    }
    // attach read member
    if (ok && read_Get(flags)) {
        OpenRead(shm,pos);
    }
    // attach write member
    if (ok && write_Get(flags)) {
        OpenWrite(shm,pos);
    }
    nonblock_Set(shm.flags,nonblock_Get(flags));// copy
    UpdateWriteLimit(shm);
    ok = ok && ((shm.flags.value & flags.value) == flags.value);
    verblog(lib_ams::_db.proc_id
            <<"  openshm"
            <<Keyval("shm_id",shm.shm_id)
            <<Keyval("flags",shm.flags)
            <<Keyval("origflags",flags)
            <<Keyval("size",shm.shm_region.n_elems)
            <<Keyval("filename",shm.filename)
            <<Keyval("success",Bool(ok)));
    return ok;
}

// Update budget for shm SHM
// Return TRUE if the shm is potentially unblocked for
// writing due to WRITELIMIT being increased.
// (WRITELIMIT is the point beyond which no message can be written
// because doing so would overwrite data not yet consumed by one of the read members.)
bool lib_ams::UpdateWriteLimit(lib_ams::FShm &shm) {
    bool ret=false;
    if (shm.c_write) {
        u64 min_roff=ULLONG_MAX/2; // some huge number for which you can still do arithmetic
        ind_beg(lib_ams::shm_c_shmember_curs,shmember,shm) {
            if (r_Get(shmember.shmember.flags)) {
                u64_UpdateMin(min_roff,shmember.off);
            }
        }ind_end;
        u64 new_offset = min_roff + (shm.offset_mask+1) - lib_ams::_db.max_msg_size*2;
        ret = u64_Update(shm.writelimit, new_offset);
    }
    return ret;
}

// Register a bad, unskippable incoming message
// on SHM, report it, and stop reading.
void lib_ams::ShmError(lib_ams::FShm &shm, ams::Shmmsg *msg, const char *text) {
    if (bool_Update(shm.error,true)) {
        prlog(lib_ams::_db.proc_id
              <<"  lib_ams.read_error"
              <<Keyval("expected",tempstr()<<shm.shm_id<<"@"<<shm.c_read->off)
              <<Keyval("shm_id",msg->shm_id)
              <<Keyval("off",msg->off)
              <<Keyval("type",msg->type)
              <<Keyval("length",msg->length)
              <<Keyval("comment",tempstr()<<"Bad message. Reading stopped: "<<text));
        StopReading(shm);
    }
}

// If SHM is out of write space, and shm is set to blocking mode,
// poll indefinitely all control shms (zd_ctlin list) for heartbeats
// that indicate that the shm write space is finally available
// (This happens when all shm readers read enough data so that some can be discarded,
// or all readers are eliminated.)
// Return TRUE if writing can proceed
inline static bool WaitBudget(lib_ams::FShm &shm) {
    bool ret = true;
    if (shm.c_write) {
        ret = shm.c_write->off < shm.writelimit;
        if (!ret && !nonblock_Get(shm.flags)) {
            do {
                lib_ams::_db.trace.n_write_block_spin++;
                lib_ams::PollCtlIn();
                ret = shm.c_write->off < shm.writelimit;
                shm.n_write_block++;
                if (algo_lib_logcat_slowness.enabled) {
                    if ((shm.n_write_block & ((1<<20)-1))==0) {
                        prcat(slowness, lib_ams::_db.proc_id
                              <<": writing to "<<shm.shm_id
                              <<": apparent deadlock");
                    }
                }
            } while (!ret);
        }
    }
    return ret;
}

void lib_ams::PollCtlIn() {
    ind_beg(lib_ams::_db_c_ctlin_curs,ctlin,lib_ams::_db) {
        if (ams::Shmmsg *shmmsg = PeekMsg(ctlin)) {
            if (ams::MsgHeader *payload = payload_Get(*shmmsg)) {
                h_amsmsg_Call(*ctlin.p_shm,*payload);
            }
            SkipMsg(ctlin);
        }
    }ind_end;
}

// Begin writing message of length LENGTH
// Used by WriteMsg and with amc's pnew (acr pnew) for zero-copy sends.
// If successful, return pointer to region of size LENGTH whre the mssage can be written,
// otherwise return NULL.
void *lib_ams::BeginWrite(lib_ams::FShm &shm, int length) {
    void *ret = NULL;
    int L = ssizeof(ams::Shmmsg) + length;
    if (L <= _db.max_msg_size) {
        if (WaitBudget(shm)) {
            ams::Shmmsg *seqmsg = MsgAtOffset(shm,shm.c_write->off);
            ret = seqmsg+1;
        }
    }
    return ret;
}

// Finish writing message of length LENGTH,
// and send the result.
void lib_ams::EndWrite(lib_ams::FShm &shm, void *ptr, int len) {
    ams::Shmmsg *shmmsg = (ams::Shmmsg*)((u8*)ptr - ssizeof(ams::Shmmsg));
    int L = ssizeof(ams::Shmmsg) + len;
    shmmsg->shm_id = shm.shm_id;
    shmmsg->off = shm.c_write->off;
    shmmsg->tstamp = algo::CurrSchedTime().value;
    shmmsg->type = ams_MsgHeader_type_ams_Shmmsg;
    shm.c_write->off = AddOffset(shm.c_write->off, L);
    ams::Shmmsg *next = MsgAtOffset(shm,shm.c_write->off);
    next->length = 0;// length 0 means the message isn't ready
    sfence();
    shmmsg->length = L;// mark message as ready!
    // if shm is being both written and read by this process
    // trigger reading
    if (shm.c_read) {
        cd_poll_read_Insert(*shm.c_read);
    }
}

// If the shm is attached to a writable shared memory segment,
// write sequenced message to shm SHM, and increment sequence number
// and shm offset.
// Message must be within max. message size
// If the shm is not configured for writingIf an output fd is associated with the shm, hthe shm, write
bool lib_ams::WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    bool ret=false;
    int len = msg.length;
    if (void *ptr = lib_ams::BeginWrite(shm,msg.length)) {
        memcpy(ptr, &msg, msg.length);
        lib_ams::EndWrite(shm,ptr,len);
        ret=true;
    } else if (algo_lib_logcat_slowness.enabled) {
        lib_ams::FShmember *slowest=NULL;
        ind_beg(shm_c_shmember_curs,shmember,shm) {
            if (slowest==NULL  || shmember.off < slowest->off) {
                slowest=&shmember;
            }
        }ind_end;
        prcat(slowness,lib_ams::_db.proc_id<<": writing to "<<shm.shm_id
              <<": dropping"
              <<Keyval("sizelimit",_db.max_msg_size)
              <<Keyval("writesize",len+ssizeof(ams::Shmmsg))
              <<Keyval("budget",lib_ams::GetWriteBudget(shm))
              <<Keyval("maxbudget",shm.shm_region.n_elems)
              <<Keyval("slowest_shmember",(slowest ? slowest->shmember : ams::ShmemberId()))
              <<Keyval("behind_by",slowest ? shm.c_write->off-slowest->off : 0)
              <<" msg "<<lib_ams::ToDbgString(msg)
              );
    }
    return ret;
}

// Initialize shm library
// FILE_PREFIX: unique filesystem prefix for all subsequently created shms.
// This is copied to lib_ams::_db.file_prefix.
// if FILE_PREFIX is empty, we are running in NON-SHMEM mode. In this mode,
// the library begins reading STDIN and posting any input messages to the default
// input shm.
// PROC_ID: process ID for this app. This is copied to lib_ams::_db.proc_id.
// The PROC_ID is <proctype>.<nodeindex>-<index> -- enough to identify any number of processes within
// a cluster.
// If the variable _db.shm_files_cleaned is FALSE (which is the default),
// clean any stale (unlocked) shm files in /dev/shm/*.ams
// In SHMEM mode, all current and future memory is locked using mlockall.
bool lib_ams::Init(algo::strptr file_prefix, ams::ProcId proc_id) {
    bool shmem_mode = file_prefix != "";
    lib_ams::_db.file_prefix = file_prefix;
    lib_ams::_db.proc_id = proc_id;
    _db.c_thisproc = &lib_ams::ind_proc_GetOrCreate(lib_ams::_db.proc_id);// so it exists
    if (shmem_mode && bool_Update(lib_ams::_db.shm_files_cleaned,true)) {
        lib_ams::CleanOldShmFiles();
    }
    lib_ams::_db.shmem_mode = shmem_mode;
    if (shmem_mode) {
    } else {
        BeginReadStdin();
    }
    bool ok = true;
    //bool lock_ok = mlockall(MCL_CURRENT | MCL_FUTURE)==0;
    // if (!lock_ok) {
    //     prlog(lib_ams::_db.proc_id<<"  WARNING mlockall failed; expect high latency");
    // }
    return ok;
}

void lib_ams::Uninit() {
    lib_ams::_db.c_shm_ctl=NULL;
    shm_RemoveAll();
}

void lib_ams::shm_file_Cleanup(lib_ams::FShm &shm) {// fcleanup:lib_ams.FShm.shm_file
#ifdef WIN32
    // unmap section from process address space for reader and writer as well
    if (shm.shm_region.elems) {
        UnmapViewOfFile(shm.shm_region.elems);
        Refurbish(shm.shm_region);
    }
    // close named section
    // note that section will be actually removed along with last reference to it
    if (shm.shm_handle) {
        CloseHandle(shm.shm_handle);
        shm.shm_handle = NULL;
    }
#else
    if (shm.created) {
        if (ShmemFdOpenQ(shm)) {
            (void)flock(shm.shm_file.fd.value, LOCK_UN);
        }
        if (shm.filename != "") {
            shm_unlink(Zeroterm(shm.filename));
            shm.filename = "";
        }
    }
#endif
}

// -----------------------------------------------------------------------------

void lib_ams::CtlMsg_ShmHbMsg(ams::ShmHbMsg &msg) {
    lib_ams::FShmember *shmember = lib_ams::ind_shmember_Find(msg.shmember);
    if (!shmember) {
        lib_ams::ind_proc_GetOrCreate(msg.shmember.proc_id);
        lib_ams::ind_proc_GetOrCreate(msg.shmember.shm_id.proc_id);
        lib_ams::ind_shm_GetOrCreate(msg.shmember.shm_id);
        shmember=&lib_ams::shmember_Alloc();
        shmember->shmember=msg.shmember;
        if (!shmember_XrefMaybe(*shmember)) {
            shmember_Delete(*shmember);
            shmember=NULL;
        }
    }
    if (shmember) {
        shmember->off  = msg.off;
        shmember->budget = msg.wbudget;
        shmember->last_hb = algo_lib::_db.clock;
        if (UpdateWriteLimit(*shmember->p_shm)) {
            ind_beg(_db_fdin_curs,fdin,lib_ams::_db) {
                if (!cd_fdin_read_InLlistQ(fdin) && ch_N(_db.expect_str)==0) {
                    cd_fdin_read_Insert(fdin);
                }
            }ind_end;
        }
    }
}

void lib_ams::ReadCtlMsg(lib_ams::FShm &, ams::MsgHeader &msg) {
    if (!lib_ams::CtlMsgDispatch(msg)) {
    }
}

// Read next input line from stdin
// The line is parsed as an AMS message. It could be a Shmmsg or any other msg.
// If the line doesn't look like anything, it is converted to ams.InputLineMsg
// Then, look up a shm where to post the message.
// If we see a Shmmsg, find shm where this message is intended to go and write the message there.
// Otherwise, post the message to the default shm (lib_ams::_db.dflt_shm_id)
// If the shm where we are posting the message is full (won't accept the message)
// then reading of fdin is stopped and will resume after the shm has room.
// If there is nowhere to post the message because no target shm is found, the counter
//  trace.n_fdin_drop_notgt is incremented and a message is printed in verbose mode.
void lib_ams::cd_fdin_read_Step() {
    lib_ams::FFdin &fdin = *cd_fdin_read_RotateFirst();
    algo::strptr msgstr = in_GetMsg(fdin);
    if (msgstr.elems) {
        bool processed=true;
        lib_ams::FShm *tgt_shm = NULL;
        ams::MsgHeader *tgt_msg = NULL;
        ary_RemoveAll(_db.fdin_buf);
        (void)ams::MsgHeaderMsgs_ReadStrptr(msgstr, _db.fdin_buf);
        if (!_db.fdin_buf.ary_n) {
            lib_ams::InputLineMsg_FmtByteAry(_db.fdin_buf, lib_ams::_db.proc_id, msgstr);
        }
        ams::Shmmsg *shmmsg = ams::Shmmsg_Castdown(*(ams::MsgHeader*)_db.fdin_buf.ary_elems);
        if (shmmsg) {
            tgt_shm = lib_ams::ind_shm_Find(shmmsg->shm_id);
            if (tgt_shm) {
                if (shmmsg->off == 0) {// allow user not to guess
                    shmmsg->off = tgt_shm->c_write->off;
                }
                if (shmmsg->off == tgt_shm->c_write->off) {
                    tgt_msg = payload_Get(*shmmsg);
                }
            }
        } else {
            tgt_shm = lib_ams::ind_shm_Find(lib_ams::_db.dflt_shm_id);
            tgt_msg = (ams::MsgHeader*)_db.fdin_buf.ary_elems;
        }
        if (!tgt_msg) {
            // message wasn't parsed
        } else if (!tgt_shm) {
            // no target shm found
        } else if (!(tgt_shm->c_write && tgt_shm->c_read)) {
            // must be readable & writable
        } else if (WriteMsg(*tgt_shm,*tgt_msg)) {
            // success
            _db.trace.n_fdin_posted++;
        } else {
            // stop reading stdin - no progress is possible
            _db.trace.n_fdin_pushback++;
            verblog(lib_ams::_db.proc_id<<" shmblocked "<<tgt_shm->shm_id<<", write budget "<<GetWriteBudget(*tgt_shm));
            cd_fdin_read_Remove(fdin);
            processed=false;
        }
        // #AL# if input message has been dropped, help debug this situation
        if (!tgt_shm) {
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
    cd_hb_RemoveAll();
    cd_poll_read_RemoveAll();
    // and force exit since above are not the only steps present....
    algo_lib::ReqExitMainLoop();
}

// Begin reading ams control messages from stdin
void lib_ams::BeginReadStdin() {
    lib_ams::FFdin &fdin = fdin_Alloc();
    fdin_XrefMaybe(fdin);
    algo::Fildes fd(0);
    algo::SetBlockingMode(fd,false);
    in_BeginRead(fdin,fd);
}

lib_ams::FShm &lib_ams::ind_shm_GetOrCreate(ams::ShmId shm_id) {
    lib_ams::FShm* ret = lib_ams::ind_shm_Find(shm_id);
    if (!ret) {
        ret         = &shm_Alloc();
        (*ret).shm_id = shm_id;
        bool good = shm_XrefMaybe(*ret);
        if (!good) {
            shm_RemoveLast(); // delete offending row, any existin xrefs are cleared
            ret = NULL;
        }
    }
    vrfy(ret,tempstr()<<"lib_ams.create_error  table:ind_shm  key:'"<<shm_id<<"'  comment:'bad xref'");
    return *ret;
}

// -----------------------------------------------------------------------------

// Close shm.
// If shm is opened for writing, remove its file.
// BUG: I think if the shm is opened for reading, the mmap() leaks
void lib_ams::Close(lib_ams::FShm &shm) {
    shm_file_Cleanup(shm);
    algo::Refurbish(shm.shm_region);
    algo::Refurbish(shm.shm_file);
    read_Set(shm.flags,false);
    write_Set(shm.flags,false);
    ind_beg(shm_c_shmember_curs,shmember,shm) {
        cd_hb_Remove(shmember);
        cd_poll_read_Remove(shmember);
    }ind_end;
}

// -----------------------------------------------------------------------------

void lib_ams::SetDfltShmSize(u32 size) {
    size = i32_Max(size, lib_ams::_db.max_msg_size*2);
    size = 1 << algo::CeilingLog2(size);
    lib_ams::_db.shmem_size = size + lib_ams::_db.max_msg_size*2;
}

// -----------------------------------------------------------------------------

// Register PROC_ID as reader of shm SHM_ID starting at offset 0
// This immediately limits the write budget of SHM_ID
//    (Subsequent attempt to overwrite data not yet consumed by PROC_ID causes
//     either reject or blocking wait)
void lib_ams::AddReadShmember(ams::ShmId shm_id, ams::ProcId proc_id) {
    ams::ShmHbMsg msg(ams::ShmemberId(shm_id,proc_id,ams_ShmemberFlags_r),0,0);
    lib_ams::CtlMsg_ShmHbMsg(msg);
}

// Open shm SHM for reading and add it to the list of shms
// which lib_ams scans for messages (i.e. control shms).
// Attach lib_ams message dispatch to the shm
bool lib_ams::AddCtlIn(lib_ams::FShm &shm) {
    bool ret = OpenShm(shm,ams_ShmFlags_read);
    h_amsmsg_Set2(shm,shm,lib_ams::ReadCtlMsg);
    if (read_Get(shm.flags)) {
        c_ctlin_Insert(*shm.c_read);
    }
    return ret;
}

// -----------------------------------------------------------------------------

void lib_ams::CloseAllShms() {
    ind_beg(_db_shm_curs,shm,_db) {
        Close(shm);
    }ind_end;
    shm_RemoveAll();
    _db.c_shm_ctl=NULL;
}

// -----------------------------------------------------------------------------

bool lib_ams::EnsureOutput(lib_ams::FShm &outshm, int budget) {
    bool ret = GetWriteBudget(outshm) > u64(budget);
    return ret;
}
