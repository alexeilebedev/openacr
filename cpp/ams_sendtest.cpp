// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: ams_sendtest (exe) -- Algo Messaging System test tool
// Exceptions: yes
// Source: cpp/ams_sendtest.cpp
//
// Shms:
// -  amstest-0.out-0
// Processes:
// -  amstest-0: parent
// -  amstest-{1..N}: child processes
// Parent: spawn N children
// -  Write amstest-0.out-0
// Each child #K:
// -  read amstest-0.out-0
// Parent: write N messages to output shm
// -  Each child: read messages and note latency; once a fixed number
// -  of messages are read, report average latency and exit

#include "include/algo.h"
#include "include/ams_sendtest.h"
#include "include/lib_ams.h"

lib_ams::FShm &ams_sendtest::GetOrCreateShm(ams::GrpId grp_id) {
    lib_ams::ind_proc_GetOrCreate(grp_id.proc_id);
    lib_ams::FShm *ret = lib_ams::ind_shm_GetOrCreate(grp_id);
    vrfy(ret, tempstr() << "ams_sendtest: cannot create shm " << grp_id);
    return *ret;
}

// child reads parent messsage
void ams_sendtest::ReadParentMsg(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    (void)shm;
    if (ams_sendtest::_db.cmdline.recvdelay_ns>0) {
        u64 clock=algo::get_cycles();
        u64 limit=clock + ams_sendtest::_db.cmdline.recvdelay_ns / algo_lib::_db.clocks_to_ns;
        while (clock < limit) {
            // waste time
            sfence();
            clock=algo::get_cycles();
        }
    }
    ams_sendtest::_db.test.n_msg_recv++;
    ams_sendtest::_db.test.off_recv = lib_ams::AddOffset(_db.test.off_recv, lib_ams::cd_poll_read_First()->c_cur_msg->length);
    // zero out first message latency
    if (ams::LogMsg *logmsg = ams::LogMsg_Castdown(msg)) {
        // Check the contents, not just the count.  The parent numbers every
        // message and pads it with one repeated character, and messages arrive
        // in order, so the n-th message read says which one it is.  In board
        // mode the bytes come from a slot the ring never held: a reference
        // resolved to the wrong slot, or to one already reused, would deliver a
        // well-formed message carrying another message's contents, and a count
        // would report that as a clean run.
        tempstr expect;
        expect << "parent message #" << (_db.test.n_msg_recv - 1) << " ";
        algo::strptr text = ams::text_Getary(*logmsg);
        vrfy(algo::StartsWithQ(text, expect)
             , tempstr() << "ams_sendtest.payload"
             << Keyval("expect", expect)
             << Keyval("got", algo::strptr(text.elems, i32_Min(ch_N(text), ch_N(expect)))));
        vrfy(ch_N(text) == 0 || text[ch_N(text)-1] == 'z'
             , tempstr() << "ams_sendtest.payload_tail" << Keyval("len", ch_N(text))
             << Keyval("tail", algo::strptr(text.elems + i32_Max(0,ch_N(text)-6), i32_Min(6,ch_N(text)))));
        u64 tsc = algo::get_cycles();
        u64 msgtsc = logmsg->tstamp.value;
        _db.test.sum_recv_latency += tsc - msgtsc;
        amscat(verbose,"read "<<*logmsg<<", latency "<<tsc-msgtsc<<" clocks");
        if (_db.test.n_msg_recv >= _db.test.n_msg_limit) {
            prlog("child: received all messages, offset "<<_db.test.off_recv);
            algo_lib::ReqExitMainLoop();
        }
    }
}

// Compose TEXT as a LogMsg and deliver it to every reader; TRUE when all of them
// got it.  Delivery is all or nothing, because a reader that missed one message
// of a numbered stream has a gap it cannot ask to have filled.
//
// Two independent choices decide the shape.  The message goes to one ring every
// reader shares, or to a ring per reader; and it travels inline in those rings
// or as a reference to a board slot.  Inline delivery to N rings is N copies of
// the payload -- that is the cost the board exists to remove, and the only
// arrangement in which the two paths can be told apart, since a shared ring is
// one write however many readers consume it.
bool ams_sendtest::SendText(algo::strptr text) {
    bool ret = false;
    algo::SchedTime now = algo::CurrSchedTime();
    if (ams_sendtest::_db.cmdline.board) {
        ams::LogMsg *msg = ams::LogMsg_FmtByteAry(ams_sendtest::_db.msgbuf, lib_ams::_db.proc_id, now, "", text);
        if (msg) {
            lib_ams::c_postlane_RemoveAll();
            if (ams_sendtest::_db.cmdline.uc) {
                ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
                    lib_ams::BoardPostLane(*child.p_shm);
                }ind_end;
            } else {
                lib_ams::BoardPostLane(*ams_sendtest::_db.c_out);
            }
            ret = lib_ams::BoardPostSet(ams::Castbase(*msg));
        }
    } else if (ams_sendtest::_db.cmdline.uc) {
        ret = true;
        ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
            ret = ret && lib_ams::HasBudgetQ(*child.p_shm, u32(ch_N(text)) + 64);
        }ind_end;
        if (ret) {
            ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
                ret = ret && lib_ams::LogMsg_FmtShm(*child.p_shm, lib_ams::_db.proc_id, now, "", text) != NULL;
            }ind_end;
        }
    } else {
        ret = lib_ams::LogMsg_FmtShm(*ams_sendtest::_db.c_out, lib_ams::_db.proc_id, now, "", text) != NULL;
    }
    return ret;
}

void ams_sendtest::SendMsg() {
    if (_db.test.n_msg_send < _db.test.n_msg_limit) {
        if (_db.test.n_msg_send == 1 && i32(_db.nsync) < child_N()) {
            _db.nsync=0;
            ind_beg(ams_sendtest::_db_child_curs,child,ams_sendtest::_db) {
                // Each child is registered on the lane it actually reads, which
                // is its own in unicast mode and the shared one otherwise.
                ams::Shmember *reader = FindReadShmember(*child.p_shm,child.proc_id);
                vrfy_(reader);
                _db.nsync += reader->offset == child.p_shm->c_shmhdr->woff;
            }ind_end;
            if (i32(_db.nsync) == child_N()) {
                prlog("all children started up");
            }
        } else {
            tempstr text;
            text << "parent message #"<<_db.test.n_msg_send<<" ";
            // compose random length message
            int msglen = ams_sendtest::_db.cmdline.msgsize_min
                + i32_WeakRandom(ams_sendtest::_db.cmdline.msgsize_max - ams_sendtest::_db.cmdline.msgsize_min);
            // Pad in one allocation rather than a character at a time.  At the
            // sizes the board exists for, a per-character append is the most
            // expensive thing in the loop -- an 8KB message is 8192 appends --
            // and a throughput measured this way reports the cost of building
            // the message rather than the cost of moving it.
            int pad = msglen - ch_N(text);
            if (pad > 0) {
                memset(ch_AllocN(text, pad).elems, 'z', size_t(pad));
            }
            // -blocking emulates a blocking send: wait for ring budget, then
            // use the non-blocking zero-copy FmtShm (which now succeeds).
            // Without it, a full ring drops the message and bumps n_write_wait.
            // The send mode is explicit here, not implied by a shm flag.
            if (_db.cmdline.blocking) {
                lib_ams::WaitBudget(*ams_sendtest::_db.c_out, true);
            }
            bool sent = ams_sendtest::SendText(text);
            if (sent) {
                _db.test.n_msg_send++;
                if (_db.test.n_msg_send == _db.test.n_msg_limit) {
                    bh_timehook_Remove(_db.test.h_write);
                    // The parent's work ends with its last message, so it says so
                    // rather than leaving the loop to notice that nothing is left
                    // to do.  Signaled mode keeps a signalfd armed for the life of
                    // the process, and an armed iohook is work: a parent that
                    // relied on an idle loop to fall out therefore ran until its
                    // time limit expired, however early it had finished sending.
                    // Its readers are not waiting on it -- what they have not yet
                    // consumed is already in the lane, and the parent's next act
                    // is to wait for each child to exit.
                    algo_lib::ReqExitMainLoop();
                    UpdateBudget(*ams_sendtest::_db.c_out);
                    prlog("parent: wrote all "<<_db.test.n_msg_limit<<" messages"
                          <<", wlimit updates: "<<ams_sendtest::_db.c_out->n_wlim_update
                          <<", offset "<<ams_sendtest::_db.c_out->c_shmhdr->woff
                          <<", budget "<<GetBudget(*ams_sendtest::_db.c_out));
                }
            } else {
                _db.test.n_write_wait++;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// State this process's verdict as one report row on _db.report: what it was
// asked to move, what it moved, how often the lane refused it, where the lane's
// offsets ended up, and whether it finished.  SHM is the lane this process
// wrote or read, ISPARENT says which of the two it did, and CHILD_OK is whether
// every child exited zero -- meaningless in a child, where there are none.
//
// The verdict has to name the work, not only the consistency of the counters
// that record it.  A reader that runs out of time has consumed exactly what it
// counted, so its read offset agrees with its own tally and the run looks
// finished from outside; a lane that stalled halfway then reports success with
// half the stream undelivered.  So the parent's verdict is that it sent every
// message it was asked for and that every child exited zero, and a child's is
// that it received every message and that the lane's read offset accounts for
// all of them.  A stall becomes a failure rather than a short pass, which is
// what makes the tool's own -timeout a liveness check.
static void FillReport(lib_ams::FShm &shm, bool isparent, bool child_ok) {
    report::ams_sendtest &report = ams_sendtest::_db.report;
    ams_sendtest::FTest &test = ams_sendtest::_db.test;
    report.proc << lib_ams::_db.proc_id;
    report.n_msg = test.n_msg_limit;
    report.n_msg_send = test.n_msg_send;
    report.n_msg_recv = test.n_msg_recv;
    report.n_write_wait = test.n_write_wait;
    report.woff = shm.c_shmhdr ? shm.c_shmhdr->woff : 0;
    report.roff = shm.c_reader ? shm.c_reader->offset : 0;
    double avg_clock = double(test.sum_recv_latency) / double(u64_Max(test.n_msg_recv,1));
    report.latency_ns = avg_clock * algo_lib::_db.clocks_to_ns;
    if (isparent) {
        report.success = child_ok && test.n_msg_send == test.n_msg_limit;
    } else {
        report.success = test.n_msg_recv == test.n_msg_limit && report.roff == test.off_recv;
    }
}

// -----------------------------------------------------------------------------

void ams_sendtest::Main() {
    ams::ProcId parent_proc = lib_ams::MakeProcId(ams_Proctype_ams_sendtest,0,0);
    ams::ProcId my_id = lib_ams::MakeProcId(ams_Proctype_ams_sendtest,0,_db.cmdline.id);
    prlog("start of process id "<<my_id);
    i32_UpdateMax(_db.cmdline.msgsize_max, _db.cmdline.msgsize_min+1);
    // In board mode the ring is deliberately too small for the traffic it
    // carries: the payloads go to the board and the ring carries references to
    // them.  A reader may pin board_pin slots, and the board is built to hold
    // that many for every child at once -- the sizing that keeps a placement
    // from ever failing, and with it one child's slowness from reaching
    // another.  The allowance is what a ring's write budget is on the other
    // path, so setting the two alike is what makes the paths comparable.
    lib_ams::_db.board_slot_size = _db.cmdline.msgsize_max + 64;
    lib_ams::_db.board_max_pin = i64(lib_ams::_db.board_slot_size) * _db.cmdline.board_pin;
    lib_ams::_db.max_msg_size = _db.cmdline.board ? 4096 : _db.cmdline.msgsize_max + 64;
    _db.test.n_msg_limit = _db.cmdline.nmsg;
    bool isparent = procidx_Get(my_id) == 0;
    if (isparent) {// parent
        if (ams_sendtest::_db.cmdline.file_prefix == "") {
            ams_sendtest::_db.cmdline.file_prefix << "ams_sendtest_" << getpid();
        }
    } else {
        vrfy(ams_sendtest::_db.cmdline.file_prefix != "", "file_prefix must be specified in child mode");
    }
    lib_ams::SetDfltShmSize(_db.cmdline.bufsize);

    tempstr proc_str;
    proc_str << my_id << "," << ams_sendtest::_db.cmdline.file_prefix;
    lib_ams::Init(proc_str);
    if (_db.cmdline.signaled) {
        lib_ams::SetSignaledMode(true);
    }

    // setup:
    // log0: output for root process.  In unicast mode every reader gets a lane
    // of its own -- grpidx names the reader -- which is the shape a real fan-out
    // has: the writer holds one ring per recipient rather than one ring they all
    // read.  A shared ring costs one write however many read it, so it is the
    // arrangement in which a board can save nothing.
    int mygrp = _db.cmdline.uc && !isparent ? procidx_Get(my_id) - 1 : 0;
    lib_ams::FShm &log0=GetOrCreateShm(ams::GrpId(parent_proc, ams_Grptype_log,mygrp));

    // open shms
    if (isparent) {
        ams_sendtest::_db.c_out=&log0;

        vrfy_(lib_ams::ShmOpen(log0,ams_ShmFlags_write));
        algo_lib::ApplyTrace(_db.cmdline.trace.expr);

        // The board exists before the first child is spawned, so a child never
        // races to open a segment its parent has not made yet.
        lib_ams::FShm *board = _db.cmdline.board ? lib_ams::BoardCreate(u32(_db.cmdline.nchild) * u32(_db.cmdline.board_pin)) : NULL;
        vrfy(board || !_db.cmdline.board, "ams_sendtest: cannot create the message board");

        // add readers
        for (int i=0; i<_db.cmdline.nchild; i++) {
            ams_sendtest::FChild &child = ams_sendtest::child_Alloc();
            child.proc_id = lib_ams::MakeProcId(ams_Proctype_ams_sendtest, 0,i+1);
            prlog("adding read shmember "<<child.proc_id);
            // Shared mode registers every child on the one lane; unicast mode
            // opens a lane per child and registers that child alone on it.
            child.p_shm = &log0;
            if (_db.cmdline.uc) {
                child.p_shm = &GetOrCreateShm(ams::GrpId(parent_proc, ams_Grptype_log, i));
                vrfy_(lib_ams::ShmOpen(*child.p_shm, ams_ShmFlags_write));
            }
            lib_ams::AddReadShmember(*child.p_shm,child.proc_id);

            vrfy_(algo_lib::_db.argc);
            vrfy_(algo_lib::_db.argv[0]);
            child.child_path = algo_lib::_db.argv[0];
            child.child_cmd.file_prefix = ams_sendtest::_db.cmdline.file_prefix;
            child.child_cmd.id = procidx_Get(child.proc_id);
            child.child_cmd.nmsg = _db.cmdline.nmsg;
            child.child_cmd.trace.expr = _db.cmdline.trace.expr;
            child.child_cmd.recvdelay_ns = _db.cmdline.recvdelay_ns;
            child.child_cmd.senddelay_ns = _db.cmdline.senddelay_ns;
            child.child_cmd.timeout = _db.cmdline.timeout;
            child.child_cmd.bufsize = _db.cmdline.bufsize;
            child.child_cmd.msgsize_max = _db.cmdline.msgsize_max;
            child.child_cmd.msgsize_min = _db.cmdline.msgsize_min;
            child.child_cmd.signaled = _db.cmdline.signaled;
            child.child_cmd.board = _db.cmdline.board;
            child.child_cmd.board_pin = _db.cmdline.board_pin;
            child.child_cmd.uc = _db.cmdline.uc;
            prlog("spawning child "<<i+1);
            vrfy_(child_Start(child)==0);
        }
    } else {
        vrfy_(lib_ams::ShmOpen(log0,ams_ShmFlags_read));
        // Hold the parent's board before entering the loop: a reference that
        // arrives with no board mapped resolves to nothing and is counted as a
        // bad reference rather than delivered.
        if (_db.cmdline.board) {
            vrfy(lib_ams::BoardOpen(parent_proc), "ams_sendtest: cannot open the parent's message board");
        }
        log0.burst=50;
        h_amsmsg_Set2(log0,log0,ReadParentMsg);
        algo_lib::ApplyTrace(_db.cmdline.trace.expr);
        vrfy(read_Get(log0.flags), "can't open log0 for reading");
    }
    // set time limit
    algo_lib::_db.limit = algo_lib::_db.clock + algo::ToSchedTime(_db.cmdline.timeout);
    // separate creation of shm file from
    if (isparent) {
        hook_Set0(_db.test.h_write, SendMsg);
        ThScheduleRecur(_db.test.h_write, algo::SchedTime(ams_sendtest::_db.cmdline.senddelay_ns / algo_lib::_db.clocks_to_ns));
    }
    prlog(lib_ams::_db.proc_id<<": entering main loop");
    ams_sendtest::MainLoop();// process events
    prlog(lib_ams::_db.proc_id<<": exit main loop");
    if (algo_lib::_db.cmdline.verbose) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::DumpShmTableVisual(regx);
    }
    // A child that fails its own payload or offset checks exits non-zero, and
    // the parent's verdict is the whole run's: without reading the status here
    // the parent reports the send it completed and the run ends green with a
    // child that never received a correct message.
    bool child_ok = true;
    ind_beg(_db_child_curs,child,_db) {
        prlog("waiting for child "<<ind_curs(child).index+1);
        child_Wait(child);
        vrfy_(child.child_pid==0);
        child_ok = child_ok && child.child_status == 0;
        prlog("waiting for child "<<ind_curs(child).index+1
              <<": done"<<Keyval("status",child.child_status));
    }ind_end;
    FillReport(log0,isparent,child_ok);
    if (!_db.report.success) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::DumpShmTableVisual(regx);
        algo_lib::_db.exit_code=1;
    }
    prlog(_db.report);
    if (isparent) {
        lib_ams::ShmClose(log0);
    }
}
