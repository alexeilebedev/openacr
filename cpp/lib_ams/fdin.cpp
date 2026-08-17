// Copyright (C) 2026 AlgoRND
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
// Exceptions: NO
// Source: cpp/lib_ams/fdin.cpp
//

#include "include/algo.h"
#include "include/lib_ams.h"

// Read next input line from stdin, parse as ams message, and write to target shm.
// If the target shm is full, stop reading (backpressure).
// If the shm where we are posting the message is full (won't accept the message)
// then reading of fdin is stopped and will resume after the shm has room.
// If there is nowhere to post the message because no target shm is found, the counter
//  trace.n_fdin_drop_notgt is incremented and a message is printed in verbose mode.
void lib_ams::cd_fdin_read_Step() {
    lib_ams::FFdin &fdin = *cd_fdin_read_RotateFirst();
    algo::strptr msgstr = in_GetMsg(fdin);
    if (msgstr.elems) {
        bool processed=true;
        ary_RemoveAll(_db.fdin_buf);
        (void)ams::MsgHeaderMsgs_ReadStrptr(msgstr, _db.fdin_buf);
        if (!_db.fdin_buf.ary_n) {
            lib_ams::InputLineMsg_FmtByteAry(_db.fdin_buf, lib_ams::_db.proc_id, msgstr);
        }
        ams::MsgHeader *tgt_msg = (ams::MsgHeader*)_db.fdin_buf.ary_elems;
        if (!tgt_msg) {
            // message wasn't parsed
        } else if (!_db.c_shm_stdin) {
            _db.trace.n_fdin_drop_notgt++;
        } else if (!write_Get(_db.c_shm_stdin->flags)) {
            // must be writable
        } else if (WriteMsg(*_db.c_shm_stdin,*tgt_msg)) {
            _db.trace.n_fdin_posted++;
        } else {
            _db.trace.n_fdin_pushback++;
            verblog(lib_ams::_db.proc_id<<" shmblocked "<<_db.c_shm_stdin->grp_id<<", write budget "<<lib_ams::GetBudget(*_db.c_shm_stdin));
            cd_fdin_read_Remove(fdin);
            processed=false;
        }
        if (processed) {
            in_SkipMsg(fdin);
        }
    }
}

// Stop reading stdin and drop the stdio-mode loopback shm. Once both are
// gone, MainLoop has no input source from the stdio path — if the app has
// no other shm peers either, it will exit naturally. Apps that keep peers
// alive past stdin EOF (e.g. ams_bridge waiting on shm_in echoes) need not
// do anything special: their other shms keep the loop running until eof or
// peer death drops them too.
void lib_ams::cd_fdin_eof_Step() {
    lib_ams::FFdin &fdin = *cd_fdin_eof_RemoveFirst();
    fdin_Delete(fdin);
    // A proc spawned over a pipe its parent holds open (a driver, or a worker
    // behind an ssh shell) treats stdin EOF as "my parent is
    // gone" and exits immediately.  Its MainLoop is otherwise kept alive by the
    // gateway connection, so the graceful path below would not take it down.
    if (lib_ams::_db.exit_on_fdin_eof) {
        verblog(lib_ams::_db.proc_id<<": stdin closed, exiting");
        _exit(0);
    }
    // Translate stdin EOF into an ams.TerminateMsg on the outbound stream
    // so the standard shutdown path runs.  For stdio-mode apps that's the
    // loopback shm and the message routes back to the app's own OnShmMsg;
    // for a supervisor it is the supervisor ring and every peer receives it.  Apps with
    // no outbound stream (no c_shm_stdin) can rely on the loopback drop
    // below to make MainLoop exit naturally.
    if (_db.c_shm_stdin && write_Get(_db.c_shm_stdin->flags)) {
        // The reader record is deleted right below, so this send happens once
        // and there is no later pass to repeat it.  Dropped on a full ring,
        // the ordinary shutdown never starts and the process stays up with its
        // input gone.  Queue it.
        ams::TerminateMsg_FmtAlloc(lib_ams::GetAllocQueue(*_db.c_shm_stdin), ams::ProcId());
    }
    if (_db.c_loopback_shm) {
        cd_poll_read_Remove(*_db.c_loopback_shm);
        _db.c_loopback_shm = NULL;
    }
}

// Begin reading ams control messages from stdin.
//
// Two paths ask for this and either may come first: lib_ams::Init takes it for
// a stdio peer, and an interactive process takes it when it opens its shms.
// Stdin is one descriptor and a descriptor carries one epoll registration, so
// the second caller joins the reader the first one made -- two readers would
// leave one of them subscribed to nothing and its messages unread.
void lib_ams::BeginReadStdin() {
    if (!_db.c_fdin_stdin) {
        lib_ams::FFdin &fdin = fdin_Alloc();
        fdin_XrefMaybe(fdin);
        algo::Fildes fd(0);
        algo::SetBlockingMode(fd,false);
        in_BeginRead(fdin,fd);
        _db.c_fdin_stdin = &fdin;
    }
}
