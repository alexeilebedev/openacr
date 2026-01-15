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
// Header: include/lib_ams.h
//

#pragma once
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
    // cpp/lib_ams/dump.cpp
    //

    // Print table of shms in lib_ams, using ssim format
    // if MEMBER is specified, print member information as well
    void DumpShmTableDflt(algo_lib::Regx &regx, bool member);

    // Print table of shms in lib_ams, using a more readable layout
    void DumpShmTableVisual(algo_lib::Regx &regx);

    // Print message MSG to string OUT according to format FMT
    // if FMT.STRIP > 0, strip this many outer "layers";
    // if FMT.BIN, the message is printed as pure binary; otherwise, convert to text
    // if FMT.PRETTY, every next layer / payload is printed on a new line with indent
    // for readability.
    // FMT.PAYLOAD_LEN limits maximum printed payload length, allowing to fit one messge per
    // screen even if payload is 10MB
    // Finally, if FMT.SHOWLEN is true, message length is included in output.
    void PrintMsg(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out);

    // This function should be called if the ams logcat is enabled
    // It prints the given MSG to ams logcat using pretty format.
    // Heartbeats (ShmHb, MemberHb, PubMetric) are skipped unless verbose is on
    void TraceMsg(ams::Shmmsg *msg, ams::MsgHeader *payload);
    tempstr ToString(ams::MsgHeader &msg);

    // Convert message MSG to string in a way suitable for debugging
    // (some information is lost in exchange for readability)
    tempstr ToDbgString(ams::MsgHeader &msg);

    // -------------------------------------------------------------------
    // cpp/lib_ams/file.cpp
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
    // void zd_flush_Step(); // fstep:lib_ams.FDb.zd_flush

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void fd_Cleanup(lib_ams::FWritefile& parent); // fcleanup:lib_ams.FWritefile.fd

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void fd_Cleanup(lib_ams::FReadfile& parent); // fcleanup:lib_ams.FWritefile.fd

    // -------------------------------------------------------------------
    // cpp/lib_ams/shm.cpp
    //

    // Scan /dev/shm for stale shm files and delete them
    // A file is stale if it's flockable (i.e. no process has locked it for writing)
    // and it's at least 15 seconds old
    void CleanOldShmFiles();

    // return TRUE if shared memory region is attached to shm SHM.
    bool ShmemFdOpenQ(lib_ams::FShm &shm);

    // Open shared memory for reading/writing (as specified in FLAGS)
    // and return success status
    // Fields initialized:
    // - shm.filename
    // - shm.shm_handle (windows)
    // - shm.shm_file (linux)
    // - shm.shm_region, if mapped successfully
    // If the segment is opened for writing, it is flocked.
    bool ShmemOpenFile(lib_ams::FShm &shm, ams::ShmFlags flags);

    // Open shm for reading or writing (or both)
    // The file creator sets shm size.
    // Writer creates & locks the file.
    // Reader determines file from file size
    // When opening for writing:
    // The next message written to the shm will have OFFSET as determined by POS
    // Initial value of offset is 0
    // (However a newly spawned child process can be provided a known sequence & offset by the parent.)
    // When opening for reading:
    // begin hot-polling for new messages on the shm
    bool OpenShm(lib_ams::FShm &shm, ams::ShmFlags flags, u64 pos = 0);

    // Update budget for shm SHM
    // Return TRUE if the shm is potentially unblocked for
    // writing due to WRITELIMIT being increased.
    // (WRITELIMIT is the point beyond which no message can be written
    // because doing so would overwrite data not yet consumed by one of the read members.)
    bool UpdateWriteLimit(lib_ams::FShm &shm);

    // Register a bad, unskippable incoming message
    // on SHM, report it, and stop reading.
    void ShmError(lib_ams::FShm &shm, ams::Shmmsg *msg, const char *text);
    void PollCtlIn();

    // Begin writing message of length LENGTH
    // Used by WriteMsg and with amc's pnew (acr pnew) for zero-copy sends.
    // If successful, return pointer to region of size LENGTH whre the mssage can be written,
    // otherwise return NULL.
    void *BeginWrite(lib_ams::FShm &shm, int length);

    // Finish writing message of length LENGTH,
    // and send the result.
    void EndWrite(lib_ams::FShm &shm, void *ptr, int len);

    // If the shm is attached to a writable shared memory segment,
    // write sequenced message to shm SHM, and increment sequence number
    // and shm offset.
    // Message must be within max. message size
    // If the shm is not configured for writingIf an output fd is associated with the shm, hthe shm, write
    bool WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg);

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
    bool Init(algo::strptr file_prefix, ams::ProcId proc_id);
    void Uninit();
    //     (user-implemented function, prototype is in amc-generated header)
    // void shm_file_Cleanup(lib_ams::FShm &shm); // fcleanup:lib_ams.FShm.shm_file
    // void CtlMsg_ShmHbMsg(ams::ShmHbMsg &msg); // dispatch_msg:lib_ams.CtlMsg/ams.ShmHbMsg
    void ReadCtlMsg(lib_ams::FShm &, ams::MsgHeader &msg);

    // Read next input line from stdin
    // The line is parsed as an AMS message. It could be a Shmmsg or any other msg.
    // If the line doesn't look like anything, it is converted to ams.InputLineMsg
    // Then, look up a shm where to post the message.
    // If we see a Shmmsg, find shm where this message is intended to go and write the message there.
    // Otherwise, post the message to the default shm (lib_ams::_db.dflt_shm_id)
    // If the shm where we are posting the message is full (won't accept the message)
    // then reading of fdin is stopped and will resume after the shm has room.
    // If there is nowhere to post the message because no target shm is found, the counter
    // trace.n_fdin_drop_notgt is incremented and a message is printed in verbose mode.
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_fdin_read_Step(); // fstep:lib_ams.FDb.cd_fdin_read

    // Stop reading stdin
    // void cd_fdin_eof_Step(); // fstep:lib_ams.FDb.cd_fdin_eof

    // Begin reading ams control messages from stdin
    void BeginReadStdin();
    lib_ams::FShm &ind_shm_GetOrCreate(ams::ShmId shm_id);

    // Close shm.
    // If shm is opened for writing, remove its file.
    // BUG: I think if the shm is opened for reading, the mmap() leaks
    void Close(lib_ams::FShm &shm);
    void SetDfltShmSize(u32 size);

    // Register PROC_ID as reader of shm SHM_ID starting at offset 0
    // This immediately limits the write budget of SHM_ID
    // (Subsequent attempt to overwrite data not yet consumed by PROC_ID causes
    // either reject or blocking wait)
    void AddReadShmember(ams::ShmId shm_id, ams::ProcId proc_id);

    // Open shm SHM for reading and add it to the list of shms
    // which lib_ams scans for messages (i.e. control shms).
    // Attach lib_ams message dispatch to the shm
    bool AddCtlIn(lib_ams::FShm &shm);
    void CloseAllShms();
    bool EnsureOutput(lib_ams::FShm &outshm, int budget);

    // -------------------------------------------------------------------
    // cpp/lib_ams/shmember.cpp
    //

    // If the shm is open for reading, check to see if a message
    // is available. If it is available, return pointer to message.
    ams::Shmmsg *PeekMsg(lib_ams::FShmember &shmember);

    // Called by the client
    // to avoid reading current message
    void StopReading(lib_ams::FShm &shm);

    // Read up to N messages from the message heap
    // Messages are processed in the order in which they were posted to the shms.
    //     (user-implemented function, prototype is in amc-generated header)
    // void bh_shmember_read_Step(); // fstep:lib_ams.FDb.bh_shmember_read

    // Check all shms (that are not already readable) for readability and
    // transfer readable shms to the read heap with correct sort key.
    // If powersave is enabled, then non-readable shms
    // are transferred to the slow_poll_read list where they are polled less frequently
    // void cd_poll_read_Step(); // fstep:lib_ams.FDb.cd_poll_read
    // void cd_slow_poll_read_Step(); // fstep:lib_ams.FDb.cd_slow_poll_read

    // Mark current message as read and move to the next message.
    void SkipMsg(lib_ams::FShmember &shmember);

    // Send heartbeat to control shm
    void SendHb(lib_ams::FShmember &shmember);

    // Publish shm heartbeats to c_shm_ctl shm
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_hb_Step(); // fstep:lib_ams.FDb.cd_hb

    // Create a read shmember for shm SHM
    // Begin hot-polling the shm
    bool OpenRead(lib_ams::FShm &shm, u64 off);

    // Create a write shmember for shm SHM
    bool OpenWrite(lib_ams::FShm &shm, u64 off);
    void UnreadMsg();

    // -------------------------------------------------------------------
    // include/lib_ams.inl.h
    //
    inline u64 AddOffset(u64 offset, int n);
    inline ams::Shmmsg *MsgAtOffset(lib_ams::FShm &shm, u64 offset);

    // next_ackoff is the read offset at which an unconditional
    // Shmhb is sent out. It is enabled when the shm is being read.
    inline void UpdateAckOffset(lib_ams::FShm &shm, lib_ams::FShmember &shmember);
    inline ams::ProcId MakeProcId(ams::Proctype proctype, int node, int index);
    inline u64 GetWriteBudget(lib_ams::FShm &shm);
}

#include "include/lib_ams.inl.h"
