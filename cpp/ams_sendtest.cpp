// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
    // -slowreader confines the delay to the first child, so one reader lags
    // while the others keep up.
    if (ams_sendtest::_db.cmdline.recvdelay_ns>0 && (!_db.cmdline.slowreader || _db.cmdline.id == 1)) {
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
    // In channel mode message n belongs to child n % nchild + 1, and that child
    // counts it on its channel, whose limit follows the count by the window.
    ams::LogMsg *owned = _db.c_channel ? ams::LogMsg_Castdown(msg) : NULL;
    if (owned && i32((_db.test.n_msg_recv - 1) % u64(_db.cmdline.nchild)) + 1 == _db.cmdline.id) {
        lib_ams::ChannelRead(*_db.c_channel, u64(msg.length));
    }
    // zero out first message latency
    if (ams::LogMsg *logmsg = ams::LogMsg_Castdown(msg)) {
        // Check the contents, not just the count.  The parent numbers every
        // message and pads it with one repeated character, and messages arrive
        // in order, so the n-th message read says which one it is.  In board
        // mode the bytes come from a chunk the ring never held: a reference
        // resolved to the wrong place, or to a chunk already reused, would
        // deliver a well-formed message carrying another message's contents,
        // and a count would report that as a clean run.
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

// Return the room the next message needs on a lane: a reference in board mode,
// and otherwise the largest message plus its header's rounding.
static u32 GetMsgNeed() {
    return ams_sendtest::_db.cmdline.board ? u32(sizeof(ams::BoardrefMsg)) : u32(ams_sendtest::_db.cmdline.msgsize_max) + 64;
}

// TRUE when CHILD's lane is one the parent writes: every child's with one lane
// per reader, and the first child's, which is the shared lane, otherwise.
static bool LaneQ(ams_sendtest::FChild &child) {
    return ams_sendtest::_db.cmdline.uc || &child == ams_sendtest::child_Find(0);
}

// TRUE when every lane the next message goes to has ring room for it.  Asked
// before the message is built, so a send refused for lack of room costs neither
// the padding nor the format; in board mode the lane needs room for a reference,
// and whether it may reference the chunk is known only once the message has a
// chunk, so BoardSendAll asks that half.
bool ams_sendtest::RoomQ() {
    bool ret = true;
    u32 need = GetMsgNeed();
    if (ams_sendtest::_db.cmdline.channel) {
        // The message's budget on each lane is its channel's: the lane's own
        // budget, and on its child's lane the channel's room as well.
        ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
            if (LaneQ(child)) {
                ret = ret && lib_ams::HasBudgetQ(ams_sendtest::LaneChannel(child), need);
            }
        }ind_end;
    } else if (ams_sendtest::_db.cmdline.uc) {
        ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
            ret = ret && lib_ams::HasBudgetQ(*child.p_shm, need);
        }ind_end;
    } else {
        ret = lib_ams::HasBudgetQ(*ams_sendtest::_db.c_out, need);
    }
    return ret;
}

// TRUE when the next message is refused by its owner's channel limit alone:
// every lane it goes to has ring room for it, and the owner's channel has no room
// for it under the write limit.  A send the ring refuses is not the limit's.
bool ams_sendtest::LimitRefusedQ() {
    bool ring = true;
    u32 need = GetMsgNeed();
    ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
        if (LaneQ(child)) {
            ring = ring && lib_ams::HasBudgetQ(*child.p_shm, need);
        }
    }ind_end;
    ams_sendtest::FChild &owner = ams_sendtest::MsgChild();
    return ring && !lib_ams::SlotRoomQ(owner.c_channel->c_slot, need);
}

// Send MSG, formatted into the board, to every lane the run delivers on; TRUE
// when every lane took a reference.  Every lane is asked for room first,
// because a reader that missed one message of a numbered stream has a gap it
// cannot ask to have filled, so a message reaches every lane or none.  A
// refused message stays in the board for the caller to give back.
bool ams_sendtest::BoardSendAll(ams::MsgHeader &msg) {
    lib_ams::FShm &board = *lib_ams::BoardOf(*ams_sendtest::_db.c_out);
    lib_ams::FChunk *chunk = lib_ams::ChunkOf(board, &msg);
    bool ret = chunk != NULL;
    // children are registered in order and share a lane only when all of them
    // do, so a child whose lane differs from the previous child's names a new lane
    lib_ams::FShm *prev = NULL;
    ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
        if (child.p_shm != prev) {
            ret = ret && lib_ams::BoardMakeRoom(*child.p_shm, *chunk);
        }
        prev = child.p_shm;
    }ind_end;
    prev = NULL;
    ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
        if (ret && child.p_shm != prev) {
            ret = lib_ams::BoardSend(*child.p_shm, msg);
        }
        prev = child.p_shm;
    }ind_end;
    return ret;
}

// Compose TEXT as a LogMsg and deliver it to every reader; TRUE when all of them
// got it.  Delivery is all or nothing, because a reader that missed one message
// of a numbered stream has a gap it cannot ask to have filled; RoomQ has said
// the lanes have room, so a refusal here is a race with a reader, or the board
// out of chunks, and is retried.
//
// Two independent choices decide the shape.  The message goes to one ring every
// reader shares, or to a ring per reader; and it travels inline in those rings
// or as a reference to a payload formatted once into the board.  Inline
// delivery to N rings is N copies of the payload -- that is the cost the board
// exists to remove, and the only arrangement in which the two paths can be told
// apart, since a shared ring is one write however many readers consume it.
bool ams_sendtest::SendText(algo::strptr text) {
    bool ret = false;
    algo::SchedTime now = algo::CurrSchedTime();
    if (ams_sendtest::_db.cmdline.channel) {
        ret = true;
        ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
            if (LaneQ(child)) {
                lib_ams::FChannel &channel = ams_sendtest::LaneChannel(child);
                ret = ret && lib_ams::LogMsg_FmtAlloc(lib_ams::GetAlloc(channel), lib_ams::_db.proc_id, now, "", text) != NULL;
            }
        }ind_end;
    } else if (ams_sendtest::_db.cmdline.board) {
        lib_ams::FShm &board = *lib_ams::BoardOf(*ams_sendtest::_db.c_out);
        ams::LogMsg *msg = lib_ams::LogMsg_FmtAlloc(lib_ams::BoardGetAlloc(board), lib_ams::_db.proc_id, now, "", text);
        if (msg) {
            ret = ams_sendtest::BoardSendAll(ams::Castbase(*msg));
            if (!ret) {
                lib_ams::BoardTrim(board, msg, 0);
            }
        }
    } else if (ams_sendtest::_db.cmdline.uc) {
        ret = true;
        ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
            ret = ret && lib_ams::LogMsg_FmtShm(*child.p_shm, lib_ams::_db.proc_id, now, "", text) != NULL;
        }ind_end;
    } else {
        ret = lib_ams::LogMsg_FmtShm(*ams_sendtest::_db.c_out, lib_ams::_db.proc_id, now, "", text) != NULL;
    }
    return ret;
}

// Return the child the next message belongs to in channel mode: message n goes
// to child n % nchild + 1, whose channel paces it.
ams_sendtest::FChild &ams_sendtest::MsgChild() {
    return ams_sendtest::child_qFind(u64(ams_sendtest::_db.test.n_msg_send % u64(ams_sendtest::child_N())));
}

// Return the channel the next message goes on in the lane of CHILD, in channel
// mode: the channel of the child it belongs to on that child's lane, and the
// lane's base channel on any other lane, since a message belongs to one channel.
lib_ams::FChannel &ams_sendtest::LaneChannel(ams_sendtest::FChild &child) {
    ams_sendtest::FChild &owner = ams_sendtest::MsgChild();
    lib_ams::FChannel *ret = owner.c_channel->p_shm == child.p_shm ? owner.c_channel : child.c_base;
    return *ret;
}

// Park the writer on every target the next message has no room on, and return
// true when one of them had room after all, so the next pass should try again.
// The targets are the ones RoomQ asks: the message's channel on each lane in
// channel mode, each child's lane with one lane per reader, and the shared lane
// otherwise.  A park raises the ring's writer_sleeping flag and re-checks under a
// barrier, so a reader freeing room either sees the flag and signals, or the
// re-check sees the room.
bool ams_sendtest::ParkRefused() {
    bool ret = false;
    u32 need = GetMsgNeed();
    ind_beg(ams_sendtest::_db_child_curs, child, ams_sendtest::_db) {
        bool lane = LaneQ(child);
        if (lane && ams_sendtest::_db.cmdline.channel) {
            lib_ams::FChannel &channel = ams_sendtest::LaneChannel(child);
            if (!lib_ams::HasBudgetQ(channel, need)) {
                ret = lib_ams::ParkWriter(channel, need) || ret;
            }
        } else if (lane && !lib_ams::HasBudgetQ(*child.p_shm, need)) {
            ret = lib_ams::ParkWriter(*child.p_shm, need) || ret;
        }
    }ind_end;
    return ret;
}

// Send messages until one is refused, in park mode.  A pass that sent something,
// or is still waiting for the children to start, keeps the loop awake.  A pass
// refused for room parks the writer on what refused it and lets the loop sleep,
// so the next pass runs only when a reader's wake signal arrives: a wake that is
// never sent leaves the parent asleep until its time limit, and the run fails.
void ams_sendtest::send_Step() {
    u64 nsend = _db.test.n_msg_send;
    u64 nwait = _db.test.n_write_wait;
    for (int i = 0; i < 64 && _db.send && _db.test.n_write_wait == nwait; i++) {
        ams_sendtest::SendMsg();
    }
    bool syncing = _db.test.n_msg_send == 1 && i32(_db.nsync) < child_N();
    bool refused = _db.test.n_write_wait != nwait;
    bool again = _db.test.n_msg_send != nsend || syncing;
    if (refused && !again) {
        again = ams_sendtest::ParkRefused();
        _db.test.n_writer_park += !again;
    }
    if (again) {
        algo_lib::_db.next_loop = algo_lib::_db.clock;
    }
    ams_sendtest::send_UpdateCycles();
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
                // The rate is measured from here, so a child's startup is not
                // charged to the send.
                _db.test.send_begin_tsc = algo::get_cycles();
            }
        } else if (!(_db.cmdline.blocking || ams_sendtest::RoomQ())) {
            // no room on a lane: the message is not built, and the next pass asks again
            _db.test.n_write_wait++;
            if (_db.cmdline.channel && ams_sendtest::LimitRefusedQ()) {
                _db.test.n_limit_wait++;
            }
        } else {
            tempstr text;
            text << "parent message #"<<_db.test.n_msg_send<<" ";
            // The length is a hash of the message number, so a refused send
            // retries the same message.  A fresh draw on each try would let the
            // lane's length-dependent room test admit short messages and refuse
            // long ones, and a run under backpressure would then move shorter
            // messages than a run with room and report the smaller byte rate as
            // the transport's.
            int range = ams_sendtest::_db.cmdline.msgsize_max - ams_sendtest::_db.cmdline.msgsize_min;
            int msglen = ams_sendtest::_db.cmdline.msgsize_min + i32((_db.test.n_msg_send * u64(2654435761u)) % u64(range));
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
                _db.test.n_byte_send += u64(msglen);
                if (_db.test.n_msg_send == _db.test.n_msg_limit) {
                    _db.test.send_end_tsc = algo::get_cycles();
                    bh_timehook_Remove(_db.test.h_write);
                    _db.send = false;
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
    report.n_limit_wait = test.n_limit_wait;
    report.n_writer_park = test.n_writer_park;
    report.woff = shm.c_shmhdr ? shm.c_shmhdr->woff : 0;
    report.roff = shm.c_reader ? shm.c_reader->offset : 0;
    double avg_clock = double(test.sum_recv_latency) / double(u64_Max(test.n_msg_recv,1));
    report.latency_ns = avg_clock * algo_lib::_db.clocks_to_ns;
    if (isparent) {
        report.success = child_ok && test.n_msg_send == test.n_msg_limit;
        // The send rate, measured from the moment every child had consumed the
        // first message to the last send.  One copy of each payload is counted,
        // whatever the lane shape did with it.
        report.send_s = double(test.send_end_tsc - test.send_begin_tsc) * algo_lib::_db.clocks_to_ns / 1e9;
        if (report.send_s > 0) {
            report.msg_per_s = double(test.n_msg_send) / report.send_s;
            report.mb_per_s = double(test.n_byte_send) / report.send_s / 1e6;
        }
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
    // them.  The board's body is the lag its readers may build before the
    // writer waits, which is what a ring's write budget is on the other path,
    // so -board_bufsize and -bufsize play the same part; -board_chunkref bounds
    // what one stopped reader can hold of it.
    lib_ams::_db.board_max_chunkref = _db.cmdline.board_chunkref;
    lib_ams::_db.max_msg_size = _db.cmdline.board ? 4096 : _db.cmdline.msgsize_max + 64;
    _db.test.n_msg_limit = _db.cmdline.nmsg;
    // A board message travels as a reference sent by the board's own path, which
    // writes on no channel, so the two modes do not combine.
    vrfy(!(_db.cmdline.channel && _db.cmdline.board), "ams_sendtest: -channel does not combine with -board");
    // A window no larger than a message would refuse that message forever, so
    // the run is refused before any process starts.
    vrfy(!_db.cmdline.channel || u64(_db.cmdline.channel_window) > u64(lib_ams::_db.max_msg_size)
         , tempstr() << "ams_sendtest.badwindow"
         << Keyval("channel_window", _db.cmdline.channel_window)
         << Keyval("max_msg_size", lib_ams::_db.max_msg_size)
         << Keyval("comment", "a channel window must exceed the ring's largest message"));
    // A parked writer is woken by a signal, and in polling mode nothing sends one;
    // a blocking send waits in place and never parks.
    vrfy(!_db.cmdline.parkwrite || (_db.cmdline.signaled && !_db.cmdline.blocking && !_db.cmdline.board)
         , "ams_sendtest: -parkwrite needs -signaled, and does not combine with -blocking or -board");
    bool isparent = procidx_Get(my_id) == 0;
    if (isparent) {// parent
        if (ams_sendtest::_db.cmdline.file_prefix == "") {
            ams_sendtest::_db.cmdline.file_prefix << "ams_sendtest_" << getpid();
        }
    } else {
        vrfy(ams_sendtest::_db.cmdline.file_prefix != "", "file_prefix must be specified in child mode");
    }
    lib_ams::SetDfltShmSize(_db.cmdline.bufsize);

    ams::Procspec spec;
    spec.id << my_id;
    spec.prefix = ams_sendtest::_db.cmdline.file_prefix;
    lib_ams::InitProcspec(spec);
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

        // The board exists before the first child is spawned, so a child never
        // races to open a segment its parent has not made yet.
        lib_ams::FShm *board = _db.cmdline.board ? lib_ams::BoardCreate(u64(_db.cmdline.board_bufsize), u32(_db.cmdline.chunk_size)) : NULL;
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
            // The parent opens each child's channel before the child exists, and
            // the child finds the same slot by its key when it starts.
            if (_db.cmdline.channel) {
                child.c_channel = lib_ams::ChannelOpen(*child.p_shm, u64(procidx_Get(child.proc_id)));
                child.c_base = lib_ams::ChannelOpen(*child.p_shm, 0);
                vrfy(child.c_channel, tempstr() << "ams_sendtest: no free channel slot" << Keyval("shm", child.p_shm->grp_id));
            }

            vrfy_(algo_lib::_db.argc);
            vrfy_(algo_lib::_db.argv[0]);
            child.child_path = algo_lib::_db.argv[0];
            child.child_cmd.file_prefix = ams_sendtest::_db.cmdline.file_prefix;
            child.child_cmd.id = procidx_Get(child.proc_id);
            child.child_cmd.nmsg = _db.cmdline.nmsg;
            child.child_cmd.recvdelay_ns = _db.cmdline.recvdelay_ns;
            child.child_cmd.senddelay_ns = _db.cmdline.senddelay_ns;
            child.child_cmd.timeout = _db.cmdline.timeout;
            child.child_cmd.bufsize = _db.cmdline.bufsize;
            child.child_cmd.msgsize_max = _db.cmdline.msgsize_max;
            child.child_cmd.msgsize_min = _db.cmdline.msgsize_min;
            child.child_cmd.signaled = _db.cmdline.signaled;
            child.child_cmd.board = _db.cmdline.board;
            child.child_cmd.board_chunkref = _db.cmdline.board_chunkref;
            child.child_cmd.board_bufsize = _db.cmdline.board_bufsize;
            child.child_cmd.chunk_size = _db.cmdline.chunk_size;
            child.child_cmd.slowreader = _db.cmdline.slowreader;
            child.child_cmd.uc = _db.cmdline.uc;
            child.child_cmd.nchild = _db.cmdline.nchild;
            child.child_cmd.channel = _db.cmdline.channel;
            child.child_cmd.channel_window = _db.cmdline.channel_window;
            child.child_cmd.parkwrite = _db.cmdline.parkwrite;
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
        // A child sets its channel's window before it reads anything, so the
        // parent's first message on the channel has room to go, and from then
        // on the limit follows what the child has read.
        if (_db.cmdline.channel) {
            _db.c_channel = lib_ams::ChannelOpen(log0, u64(_db.cmdline.id));
            vrfy(_db.c_channel, tempstr() << "ams_sendtest: cannot open channel" << Keyval("key", _db.cmdline.id));
            vrfy(lib_ams::ChannelSetWindow(*_db.c_channel, u64(_db.cmdline.channel_window))
                 , tempstr() << "ams_sendtest.badwindow"
                 << Keyval("channel_window", _db.cmdline.channel_window)
                 << Keyval("max_msg_size", log0.max_msg_size)
                 << Keyval("comment", "a channel window must exceed the ring's largest message"));
        }
        h_amsmsg_Set2(log0,log0,ReadParentMsg);
        vrfy(read_Get(log0.flags), "can't open log0 for reading");
    }
    // set time limit
    algo_lib::_db.limit = algo_lib::_db.clock + algo::ToSchedTime(_db.cmdline.timeout);
    // separate creation of shm file from
    // In park mode the parent sends from its step and nothing else wakes its
    // loop; otherwise a recurring timer sends one message per firing.
    if (isparent && _db.cmdline.parkwrite) {
        _db.send = true;
    } else if (isparent) {
        hook_Set0(_db.test.h_write, SendMsg);
        ThScheduleRecur(_db.test.h_write, algo::SchedTime(ams_sendtest::_db.cmdline.senddelay_ns / algo_lib::_db.clocks_to_ns));
    }
    prlog(lib_ams::_db.proc_id<<": entering main loop");
    ams_sendtest::MainLoop();// process events
    prlog(lib_ams::_db.proc_id<<": exit main loop");
    if (algo_lib::_db.cmdline.verbose) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::DumpGrpTableVisual(regx);
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
    // Every channel must end with what the parent wrote on it read by its child,
    // and never with more written than its child granted.
    ind_beg(_db_child_curs,child,_db) {
        if (child.c_channel) {
            ams::Shmchannel &channel = *child.c_channel->c_slot;
            bool channel_ok = channel.nwrite == channel.nread && channel.nwrite <= channel.wlim
                && channel.reader == child.proc_id;
            if (!channel_ok) {
                prerr("ams_sendtest.channel_mismatch" << Keyval("channel", channel));
            }
            child_ok = child_ok && channel_ok;
        }
    }ind_end;
    if (isparent && _db.cmdline.channel) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::PrintChannelTable(regx);
    }
    FillReport(log0,isparent,child_ok);
    if (!_db.report.success) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::DumpGrpTableVisual(regx);
        algo_lib::_db.exit_code=1;
    }
    prlog(_db.report);
    // the parent created every segment of the run, the board and a lane per
    // child included, and unlinks them all; a child closes only its mapping
    if (isparent) {
        lib_ams::CloseAllShms();
    }
}
