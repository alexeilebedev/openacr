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
// Target: ams_sendtest (exe) -- Algo Messaging System test tool
// Exceptions: yes
// Source: cpp/ams_sendtest.cpp
//
// Streams:
// -  amstest-0.out-0
// -  amstest-{0..N}.ctl-0
// Processes:
// -  amstest-0: parent
// -  amstest-{1..N}: child processes
// Parent: spawn N children
// -  Write amstest-0.out-0, write amstest-0.ctl-0, read amstest-{1..N}.ctl-0
// Each child #K:
// -  Write amstest-K.ctl-0, read amstest-0.ctl-0, read amstest-0.out-0
// Parent: write N messages to output stream
// -  Each child: read messages and note latency; once a fixed number
// -  of messages are read, report average latency and exit

#include "include/algo.h"
#include "include/ams_sendtest.h"
#include "include/lib_ams.h"

// child reads parent messsage
static void ReadParentMsg(lib_ams::FStream &stream, ams::MsgHeader &) {
    ams_sendtest::AmsSendTest &frame = ams_sendtest::_db.ams_send_test;
    if (ams_sendtest::_db.cmdline.recvdelay_ns>0) {
        u64 clock=algo::get_cycles();
        u64 limit=clock + ams_sendtest::_db.cmdline.recvdelay_ns / algo_lib::_db.clocks_to_ns;
        while (clock < limit) {
            sfence();
            clock=algo::get_cycles();
        }
    }
    frame.n_msg_recv++;
    // zero out first message latency
    u64 tsc = algo::get_cycles();
    u64 msgtsc = stream.cur_msg->tsc;
    frame.sum_recv_latency += tsc - msgtsc;
    if (frame.n_msg_recv >= frame.n_msg_limit) {
        prlog("child: received all messages");
        algo_lib::ReqExitMainLoop();
    }
}

static void SendMsg(ams_sendtest::AmsSendTest &frame) {
    if (frame.n_msg_send < frame.n_msg_limit) {
        tempstr text;
        text << "parent message #"<<frame.n_msg_send<<" ";
        // compose random length message
        int msglen = ams_sendtest::_db.cmdline.msgsize_min
            + i32_WeakRandom(ams_sendtest::_db.cmdline.msgsize_max - ams_sendtest::_db.cmdline.msgsize_min);
        while (ch_N(text) < msglen) {
            text << 'z';
        }
        if (ams::LogMsg *msg = lib_ams::LogMsg_FmtAmsStream(*ams_sendtest::_db.c_out, /*logcat*/"", algo::SchedTime(), text)) {
            ams::Seqmsg *seqmsg = (ams::Seqmsg*)((u8*)msg - sizeof(ams::Seqmsg));
            verblog(lib_ams::_db.proc_id<<" send "<<*seqmsg<<"  len:"<<seqmsg->length<<" offset "<<u64(seqmsg)-u64(ams_sendtest::_db.c_out->shm_region.elems));
            vrfy_(ams::text_Getary(*msg) == text);
            frame.n_msg_send++;
            if (frame.n_msg_send == frame.n_msg_limit) {
                bh_timehook_Remove(frame.h_write);
                // artificial: stop reading streams to exit main loop
                lib_ams::cd_poll_read_RemoveAll();
                lib_ams::cd_stream_hb_RemoveAll();
                prlog("parent: wrote all "<<frame.n_msg_limit<<" messages");
            }
        } else {
            frame.n_write_wait++;
        }
    }
}

inline ams::Seqmsg *MsgAtOffset(lib_ams::FStream &stream, u64 offset) {
    return (ams::Seqmsg*)(stream.shm_region.elems + (offset & stream.offset_mask));
}

// -----------------------------------------------------------------------------

void ams_sendtest::Main() {
    ams::ProcId parent_proc(ams_ProcType_amstest,0);
    ams::ProcId my_id(ams_ProcType_amstest,_db.cmdline.id);
    prlog("start of process id "<<my_id);
    i32_UpdateMax(_db.cmdline.msgsize_max, _db.cmdline.msgsize_min+1);
    // set up test context
    AmsSendTest &frame = ams_sendtest::_db.ams_send_test;
    frame.n_msg_limit = _db.cmdline.nmsg;
    bool isparent = my_id.procidx == 0;
    Regx_ReadSql(lib_ams::_db.regx_trace,_db.cmdline.trace.expr,true);
    if (isparent) {// parent
        if (ams_sendtest::_db.cmdline.file_prefix == "") {
            ams_sendtest::_db.cmdline.file_prefix << "ams_sendtest_" << getpid();
        }
    } else {
        vrfy(ams_sendtest::_db.cmdline.file_prefix != "", "file_prefix must be specified in child mode");
    }
    lib_ams::SetDfltShmSize(_db.cmdline.bufsize);

    lib_ams::Init(ams_sendtest::_db.cmdline.file_prefix, my_id);

    // setup:
    // ctl0: output ctl stream for parent (all global events from root process are published here)
    // ctlK: output ctl stream for child
    // out0: output for root process
    lib_ams::FStream &ctl0=lib_ams::ind_stream_GetOrCreate(ams::StreamId(parent_proc, ams_StreamType_ctl,0));
    lib_ams::FStream &out0=lib_ams::ind_stream_GetOrCreate(ams::StreamId(parent_proc, ams_StreamType_out,0));

    lib_ams::FStream &ctlout=lib_ams::ind_stream_GetOrCreate(ams::StreamId(my_id, ams_StreamType_ctl,0));
    lib_ams::OpenStream(ctlout,ams::StreamFlags(ams_StreamFlags_write|ams_StreamFlags_nonblock));
    lib_ams::_db.c_stream_ctl=&ctlout;// this is our event output

    // open streams
    if (isparent) {
        ams_sendtest::_db.c_out=&out0;

        lib_ams::OpenStream(out0,ams_StreamFlags_write);
        nonblock_Set(out0.flags, !_db.cmdline.blocking);

        // add readers
        for (int i=0; i<_db.cmdline.nchild; i++) {
            ams_sendtest::FChild &child = ams_sendtest::child_Alloc();
            child.proc_id = ams::ProcId(ams_ProcType_amstest, i+1);
            lib_ams::AddReadMember(child.proc_id,ctl0.stream_id);
            lib_ams::AddReadMember(child.proc_id,out0.stream_id);

            vrfy_(algo_lib::_db.argc);
            vrfy_(algo_lib::_db.argv[0]);
            child.child_path = algo_lib::_db.argv[0];
            child.child_cmd.file_prefix = ams_sendtest::_db.cmdline.file_prefix;
            child.child_cmd.id = child.proc_id.procidx;
            child.child_cmd.nmsg = _db.cmdline.nmsg;
            child.child_cmd.trace.expr = _db.cmdline.trace.expr;
            child.child_cmd.recvdelay_ns = _db.cmdline.recvdelay_ns;
            child.child_cmd.senddelay_ns = _db.cmdline.senddelay_ns;
            child.child_cmd.timeout = _db.cmdline.timeout;
            child.child_cmd.bufsize = _db.cmdline.bufsize;
            child.child_cmd.msgsize_max = _db.cmdline.msgsize_max;
            child.child_cmd.msgsize_min = _db.cmdline.msgsize_min;
            prlog("spawning child "<<i+1);
            vrfy_(child_Start(child)==0);
        }
        for (int retry=0; retry<10; retry++) {
            int ngood=0;
            ind_beg(_db_child_curs,child,_db) {
                lib_ams::FStream &ctlK=lib_ams::ind_stream_GetOrCreate(ams::StreamId(child.proc_id , ams_StreamType_ctl,0));
                if (!read_Get(ctlK.flags)) {
                    lib_ams::AddCtlIn(ctlK);
                }
                ngood += read_Get(ctlK.flags);
            }ind_end;
            if (ngood == child_N()) {
                break;
            }
            prlog("waiting for child to open output stream (attempt "<<retry<<" of "<<10<<")");
            sleep(1);
        }
        ind_beg(_db_child_curs,child,_db) {
            lib_ams::FStream &ctlK=lib_ams::ind_stream_GetOrCreate(ams::StreamId(child.proc_id, ams_StreamType_ctl,0));
            vrfy(read_Get(ctlK.flags), tempstr()<<"child "<<child.proc_id<<" didn't open stream for writing");
        }ind_end;
    } else {
        vrfy_(lib_ams::AddCtlIn(ctl0));
        vrfy_(lib_ams::OpenStream(out0,ams_StreamFlags_read));
        h_msg_Set2(out0,out0,ReadParentMsg);

        lib_ams::AddReadMember(parent_proc,ctlout.stream_id);
        vrfy(read_Get(ctl0.flags), "can't open ctl0 for reading");
        vrfy(read_Get(out0.flags), "can't open out0 for reading");
        vrfy(write_Get(ctlout.flags), "can't open ctlN for writing");
    }
    // set time limit
    algo_lib::_db.limit = algo_lib::_db.clock + algo::ToSchedTime(_db.cmdline.timeout);
    // separate creation of stream file from
    if (isparent) {
        hook_Set1(frame.h_write, frame, SendMsg);
        ThInitRecur(frame.h_write, algo::SchedTime(ams_sendtest::_db.cmdline.senddelay_ns / algo_lib::_db.clocks_to_ns));
        bh_timehook_Insert(frame.h_write);
    }
    prlog(lib_ams::_db.proc_id<<": entering main loop");
    if (!isparent) {
        lib_ams::DumpStreamTable();
    }
    ams_sendtest::MainLoop();// process events
    prlog(lib_ams::_db.proc_id<<": exit main loop");
    prlog(lib_ams::_db.proc_id<<": "<<frame);
    if (algo_lib::_db.cmdline.verbose) {
        lib_ams::DumpStreamTable();
    }
    bool ok = false;
    if (isparent) {
        ind_beg(_db_child_curs,child,_db) {
            prlog("waiting for child "<<ind_curs(child).index+1);
            child_Wait(child);
            vrfy_(child.child_pid==0);
            prlog("waiting for child "<<ind_curs(child).index+1<<": done");
        }ind_end;
        ok = out0.wpos.seq == frame.n_msg_limit+1;
        lib_ams::Close(out0);
    } else {
        double avg_clocks = frame.sum_recv_latency / u64_Max(frame.n_msg_recv,1);
        double avg_ns = avg_clocks * algo_lib::_db.clocks_to_ns;
        prlog("Child: avg recv latency "<< avg_ns<<" ns");
        ok = out0.rpos.seq == frame.n_msg_limit+1;
    }
    if (!ok) {
        lib_ams::DumpStreamTable();
        algo_lib::_db.exit_code=1;
    }
    prlog(lib_ams::_db.proc_id
          << "  ams_sendtest.end"
          <<Keyval("wpos",out0.wpos)
          <<Keyval("rpos",out0.rpos)
          <<Keyval("success",ok));
    lib_ams::Close(ctlout);
}
