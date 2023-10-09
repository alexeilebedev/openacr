// Copyright (C) 2023 AlgoRND
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
// Header: include/lib_ams.h
//

#include "include/algo.h"
#include "include/gen/lib_ams_gen.h"
#include "include/gen/lib_ams_gen.inl.h"

namespace lib_ams { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/ams/expect.cpp -- Expect implementation
    //

    // In-process `expect` implementation allows pausing reading of inputs until
    // a certain prlog output is produced
    // Motivating example:
    // InputMsg1
    // InputMsg2
    // Expect text:blah
    // InputMsg3
    // InputMsg3 is not processed until blah is printed.
    // Implementation:
    // When reading an input message:
    // clear output buffer
    // if read an ExpectMsg:
    // set expect string to the input data
    // check output buffer (match any outputs since last input against expect string)
    // set expect timeout
    // Expect timeout:
    // exit process with error
    // When printing:
    // add to output buffer (limit: 100K).
    // if expect mode, check output buffer
    void ExpectEnable(strptr str);

    // Replace input reading function with one that pauses output
    // to support ExpectMsg functionality
    void ExpectAttach(lib_ams::FStream &stream);

    // Function that replaces uses-provided message hook for a given stream
    // Having no cost until the first call to ExpectEnable, the purpose
    // is to block processing of any messages until expect_Str is empty
    void ExpectHook(lib_ams::FStream &stream, ams::MsgHeader &msg);

    // Save output to possibly check against expect string later
    // Keep last 50k-100k of output
    void ExpectSaveOutput(algo::strptr text);
    void ExpectTimeout();

    // Check saved output buffer against expect string
    // WARNING: calling prlog() from this function will cause an infinite loop
    void ExpectCheck();

    // -------------------------------------------------------------------
    // cpp/lib/ams/file.cpp
    //

    // Whether write file is in good state - valid fd and no failure
    bool OkQ(lib_ams::FWritefile &wf);

    // Check for failure. This is intended for checking Close errors.
    // After Close, OkQ could not be used for this purpose,
    // but failure flag is preserved, and could be retrieved with FailQ.
    bool FailQ(lib_ams::FWritefile &wf);

    // Open file for appending messages
    void OpenAppend(lib_ams::FWritefile &wf, algo::strptr filename);

    // Flush data to file
    //     (user-implemented function, prototype is in amc-generated header)
    // void zd_flush_Step();

    // Write message (buffered)
    void WriteMsg(lib_ams::FWritefile &wf, ams::MsgHeader &msg);

    // Write buffer as message block
    void Flush(lib_ams::FWritefile &wf);

    // Compress buffer - form full message in cbuf
    void Compress(lib_ams::FWritefile &wf);

    // Write message block to file
    void WriteMsgBlock(lib_ams::FWritefile &wf);

    // starts new message block - discard buffer, adjust seqno
    void StartNextMsgBlock(lib_ams::FWritefile &wf);

    // Close written file
    void Close(lib_ams::FWritefile &wf);

    // cleanup function
    void fd_Cleanup(lib_ams::FWritefile& parent);

    // Whether read file is in good state - valid fd and no failure.
    // EOF is not a failure.
    bool OkQ(lib_ams::FReadfile &rf);

    // Check for failure after close.
    // EOF is not a failure.
    bool FailQ(lib_ams::FReadfile &rf);

    // Check for EOF
    bool EofQ(lib_ams::FReadfile &rf);

    // Whether can read from file - file is good and no EOF
    bool CanReadQ(lib_ams::FReadfile &rf);

    // Open file for reading messages
    void OpenRead(lib_ams::FReadfile &rf, algo::strptr filename);

    // Read message by sequence number
    ams::MsgHeader *ReadMsg(lib_ams::FReadfile &rf, u64 seqno);

    // Scan file up to end
    void ScanToEnd(lib_ams::FReadfile &rf);

    // Scan up to seqno, rewind if needed
    void ScanToSeqno(lib_ams::FReadfile &rf, u64 seqno);

    // Scan and block load engine.
    // if seqno is NULL, scans to end, fills block with last block.
    // If seqno is present, scans up to given seqno, reads cbuf with payload
    // Note is does only forward scan.
    void Scan(lib_ams::FReadfile &rf,const u64 *seqno);

    // Skip bytes
    void Skip(lib_ams::FReadfile &rf,u32 skip);

    // Read bytes
    bool Read(lib_ams::FReadfile &rf,void *buf,u32 n);

    // Uncompress
    void Uncompress(lib_ams::FReadfile &rf);

    // Reindex
    void Reindex(lib_ams::FReadfile &rf);

    // Close read file
    void Close(lib_ams::FReadfile &wf);

    // cleanup function
    void fd_Cleanup(lib_ams::FReadfile& parent);

    // -------------------------------------------------------------------
    // cpp/lib/ams/stream.cpp
    //

    // Scan /dev/shm for stale stream files and delete them
    // A file is stale if it's flockable (i.e. no process has locked it for writing)
    // and it's at least 1 hour old
    void CleanOldStreamFiles();

    // TODO remove: There is no simple way to do it on Win
    // Scan stream/ directory for existing streams
    // Create a stream record for each.
    void DiscoverStreams();

    // return TRUE if shared memory region is attached to stream STREAM.
    bool ShmemOpenQ(lib_ams::FStream &stream);

    // Open shared memory for reading/writing (as specified in FLAGS)
    // and return resulting shared memory region size.
    // Fields initialized:
    // - stream.filename
    // - stream.shm_handle (windows)
    // - stream.shm_file (linux)
    // - stream.shm_region, if mapped successfully
    i64 OpenShmem(lib_ams::FStream &stream, ams::StreamFlags flags);

    // Open stream for reading or writing (but not both)
    // If shared memory mode (lib_ams::_db.shmem_mode) is set,
    // The file creator sets stream size.
    // Writer creates & locks the file.
    // Reader determines file from file size
    // If shared memory mode is not set (stdio mode),
    // the stream is attached to a private memory block of default size.
    // When opening for writing:
    // The next message written to the stream will have sequence SEQ and offset OFFSET.
    // Default is to use sequence 1, offset 0.
    // (However a newly spawned child process can be provided a known sequence & offset by the parent.)
    // When opening for reading:
    // begin hot-polling for new messages on the stream
    bool OpenStream(lib_ams::FStream &stream, ams::StreamFlags flags, ams::StreamPos pos);
    bool OpenStream(lib_ams::FStream &stream, ams::StreamFlags flags);

    // TBD not sure if this function is needed in non-fork() environment
    bool ReopenForReading(lib_ams::FStream &stream);

    // Update budget for stream STREAM
    // Return TRUE if the stream is potentially unblocked for
    // writing due to WRITELIMIT being increased.
    // (WRITELIMIT is the point beyond which no message can be written
    // because doing so would overwrite data not yet consumed by one of the read members.)
    // TODO: do not update budget for a stream that is not opened for writing
    bool UpdateBudget(lib_ams::FStream &stream);

    // Update stream budgets
    void UpdateBudgets();

    // Register a bad, unskippable incoming message
    // on STREAM, report it, and stop reading.
    void WriterError(lib_ams::FStream &stream, ams::Seqmsg *msg);

    // If the stream is open for reading, check to see if a message
    // is available.
    // If it is available, return pointer to message.
    ams::Seqmsg *PeekMsg(lib_ams::FStream &stream);

    // Called by the client
    // to aoivd reading current message
    void StopReading(lib_ams::FStream &stream);

    // Mark current message as read and move to the next message.
    void SkipMsg(lib_ams::FStream &stream);

    // Send heartbeat to control stream
    // Update
    void SendHb(lib_ams::FStream &stream);

    // Publish stream heartbeats to c_stream_ctl stream
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_stream_hb_Step();

    // Check all streams (that are not already readable) for readability.
    // If readable, call h_msg hook.
    // void cd_poll_read_Step();
    void PollCtlIn();

    // Begin writing message of length LENGTH
    // Used by WriteMsg and with amc's pnew (acr pnew) for zero-copy sends.
    // If successful, return pointer to region of size LENGTH whre the mssage can be written,
    // otherwise return NULL.
    void *BeginWrite(lib_ams::FStream &stream, int length);

    // Finish writing message of length LENGTH,
    // and send the result.
    void EndWrite(lib_ams::FStream &stream, void *ptr, int len);

    // If the stream is attached to a writable shared memory segment,
    // write sequenced message to stream STREAM, and increment sequence number
    // and stream offset.
    // Message must be within max. message size
    // If the stream is not configured for writingIf an output fd is associated with the stream, hthe stream, write
    bool WriteMsg(lib_ams::FStream &stream, ams::MsgHeader &msg);

    // Write message MSG to output, either in binary or text mode.
    // Return success status
    // If ISREAD flag is set, this is a copy of a message that has just been read
    // from another stream. Otherwise, it's a copy of a message that's just been written.
    // In stdin mode, where messages are read from ascii input and posted to the
    void TraceMsg(lib_ams::FStream &stream, ams::MsgHeader &msg, bool isread);

    // Initialize stream library
    // FILE_PREFIX: unique filesystem prefix for all subsequently created streams
    // Also clean any stale (unlocked) stream files.
    // (this can be disabled by setting _db.stream_files_cleaned to true)
    bool Init(algo::strptr file_prefix, ams::ProcId proc_id);
    void Uninit();
    void shm_handle_Cleanup(lib_ams::FStream &stream);
    void shm_file_Cleanup(lib_ams::FStream &stream);

    // (This message could be unnecessary)
    //     (user-implemented function, prototype is in amc-generated header)
    // void CtlMsg_ProcAddMsg(ams::ProcAddMsg &msg);

    // A process has exited
    // Remove any read/write members contributed by the process and
    // potentially unblock some write streams
    // void CtlMsg_ProcRemoveMsg(ams::ProcRemoveMsg &msg);
    // void CtlMsg_DumpStreamTableMsg(ams::DumpStreamTableMsg &msg);
    // void CtlMsg_StreamHbMsg(ams::StreamHbMsg &msg);
    void ReadCtlMsg(lib_ams::FStream &, ams::MsgHeader &msg);

    // Read next input line from stdin
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_fdin_read_Step();

    // Stop reading stdin
    // void cd_fdin_eof_Step();

    // Begin reading ams control messages from stdin
    void BeginReadStdin();
    lib_ams::FStream &ind_stream_GetOrCreate(ams::StreamId stream_id);
    void DumpStreamTable();

    // Close stream.
    // If stream is opened for writing, remove its file.
    // BUG: I think if the stream is opened for reading, the mmap() leaks
    void Close(lib_ams::FStream &stream);
    void SetDfltShmSize(u32 size);
    ams::StreamPos ReadStreamPos(algo::strptr val);

    // Register PROC_ID as reader of stream STREAM_ID starting at offset 0
    // This immediately limits the write budget of STREAM_ID
    // (Subsequent attempt to overwrite data not yet consumed by PROC_ID causes
    // either reject or blocking wait)
    void AddReadMember(ams::ProcId proc_id, ams::StreamId stream_id);

    // Open stream STREAM for reading and add it to the list of streams
    // which lib_ams scans for messages (i.e. control streams).
    // Attach lib_ams message dispatch to the stream
    bool AddCtlIn(lib_ams::FStream &stream);
    void CloseAllStreams();

    // Match trace expression REGX against stream STREAM.
    // If it matches, enable/disable tracing as indicated by ENABLE
    // For full description see txt/trace.md
    bool ApplyTrace(lib_ams::FStream &stream, algo_lib::Regx &regx, bool enable);

    // Enable or disable logcat tracing based on regex WHAT
    // If TRACE is specified, logcats matching regex are enabled; otherwise
    // they are disabled.
    // This affects all future `prlog_cat` calls.
    // Return number of matches
    int ApplyTrace(algo::strptr what, bool enable);

    // Same as ApplyTrace, but
    // If expression doesn't match anything, print a helpful message for the user
    void ApplyTraceV(algo::strptr what, bool enable);

    // prlog hook for ams application, could be used for sending log messages to output stream
    void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str);
}
