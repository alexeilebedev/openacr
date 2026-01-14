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
// -  amstest-{0..N}.ctl-0
// Processes:
// -  amstest-0: parent
// -  amstest-{1..N}: child processes
// Parent: spawn N children
// -  Write amstest-0.out-0, write amstest-0.ctl-0, read amstest-{1..N}.ctl-0
// Each child #K:
// -  Write amstest-K.ctl-0, read amstest-0.ctl-0, read amstest-0.out-0
// Parent: write N messages to output shm
// -  Each child: read messages and note latency; once a fixed number
// -  of messages are read, report average latency and exit

#include "include/algo.h"
#include "include/ams_sendtest.h"
#include "include/lib_ams.h"

lib_ams::FShm &ams_sendtest::GetOrCreateShm(ams::ShmId shm_id) {
    lib_ams::ind_proc_GetOrCreate(shm_id.proc_id);
    return lib_ams::ind_shm_GetOrCreate(shm_id);
}

// child reads parent messsage
static void ReadParentMsg(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    (void)shm;
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
    frame.off_recv = lib_ams::AddOffset(frame.off_recv, lib_ams::_db.c_cur_shmember->c_cur_msg->length);
    // zero out first message latency
    if (ams::LogMsg *logmsg = ams::LogMsg_Castdown(msg)) {
        u64 tsc = algo::get_cycles();
        u64 msgtsc = logmsg->tstamp.value;
        frame.sum_recv_latency += tsc - msgtsc;
        if (frame.n_msg_recv >= frame.n_msg_limit) {
            prlog("child: received all messages, offset "<<frame.off_recv);
            algo_lib::ReqExitMainLoop();
        }
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
        if (ams::LogMsg *msg = lib_ams::LogMsg_FmtShm(*ams_sendtest::_db.c_out, lib_ams::_db.proc_id, algo::CurrSchedTime(), "", text)) {
            ams::Shmmsg *seqmsg = (ams::Shmmsg*)((u8*)msg - sizeof(ams::Shmmsg));
            verblog(lib_ams::_db.proc_id<<" send "<<*seqmsg<<"  len:"<<seqmsg->length<<" offset "<<u64(seqmsg)-u64(ams_sendtest::_db.c_out->shm_region.elems));
            vrfy_(ams::text_Getary(*msg) == text);
            frame.n_msg_send++;
            frame.off_send = lib_ams::AddOffset(frame.off_send, seqmsg->length);
            if (frame.n_msg_send == frame.n_msg_limit) {
                bh_timehook_Remove(frame.h_write);
                // artificial: stop reading shms to exit main loop
                lib_ams::cd_poll_read_RemoveAll();
                lib_ams::cd_hb_RemoveAll();
                prlog("parent: wrote all "<<frame.n_msg_limit<<" messages, off "<<frame.off_send);
            }
        } else {
            frame.n_write_wait++;
        }
    }
}

inline ams::Shmmsg *MsgAtOffset(lib_ams::FShm &shm, u64 offset) {
    return (ams::Shmmsg*)(shm.shm_region.elems + (offset & shm.offset_mask));
}

// -----------------------------------------------------------------------------

void ams_sendtest::Main() {
    ams::ProcId parent_proc = lib_ams::MakeProcId(ams_Proctype_ams_sendtest,0,0);
    ams::ProcId my_id = lib_ams::MakeProcId(ams_Proctype_ams_sendtest,0,_db.cmdline.id);
    prlog("start of process id "<<my_id);
    i32_UpdateMax(_db.cmdline.msgsize_max, _db.cmdline.msgsize_min+1);
    lib_ams::_db.max_msg_size = _db.cmdline.msgsize_max + 64;
    // set up test context
    AmsSendTest &frame = ams_sendtest::_db.ams_send_test;
    frame.n_msg_limit = _db.cmdline.nmsg;
    bool isparent = procidx_Get(my_id) == 0;
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
    // ctl0: output ctl shm for parent (all global events from root process are published here)
    // ctlK: output ctl shm for child
    // log0: output for root process
    lib_ams::FShm &ctl0=GetOrCreateShm(ams::ShmId(parent_proc, ams_Shmtype_ctl,0));
    lib_ams::FShm &log0=GetOrCreateShm(ams::ShmId(parent_proc, ams_Shmtype_log,0));

    lib_ams::FShm &ctlout=GetOrCreateShm(ams::ShmId(my_id, ams_Shmtype_ctl,0));
    lib_ams::OpenShm(ctlout,ams::ShmFlags(ams_ShmFlags_write|ams_ShmFlags_nonblock));
    lib_ams::_db.c_shm_ctl=&ctlout;// this is our event output

    // open shms
    if (isparent) {
        ams_sendtest::_db.c_out=&log0;

        lib_ams::OpenShm(log0,ams_ShmFlags_write);
        nonblock_Set(log0.flags, !_db.cmdline.blocking);
        algo_lib::ApplyTrace(_db.cmdline.trace.expr);

        // add readers
        for (int i=0; i<_db.cmdline.nchild; i++) {
            ams_sendtest::FChild &child = ams_sendtest::child_Alloc();
            child.proc_id = lib_ams::MakeProcId(ams_Proctype_ams_sendtest, 0,i+1);
            lib_ams::AddReadShmember(ctl0.shm_id,child.proc_id);
            lib_ams::AddReadShmember(log0.shm_id,child.proc_id);

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
            prlog("spawning child "<<i+1);
            vrfy_(child_Start(child)==0);
        }
        int ngood=0;
        for (int retry=0; retry<10 && ngood < child_N(); retry++) {
            if (retry>0) {
                prlog("waiting for child to open output shm (attempt "<<retry<<" of "<<10<<")");
                sleep(1);
            }
            ind_beg(_db_child_curs,child,_db) {
                lib_ams::FShm &ctlK=GetOrCreateShm(ams::ShmId(child.proc_id , ams_Shmtype_ctl,0));
                if (!read_Get(ctlK.flags)) {
                    // this function tries to open stream for reading
                    lib_ams::AddCtlIn(ctlK);
                }
                ngood += read_Get(ctlK.flags);
            }ind_end;
        }
        vrfy(ngood==child_N(), "not all children started");
    } else {
        vrfy_(lib_ams::AddCtlIn(ctl0));
        vrfy_(lib_ams::OpenShm(log0,ams_ShmFlags_read));
        h_amsmsg_Set2(log0,log0,ReadParentMsg);
        algo_lib::ApplyTrace(_db.cmdline.trace.expr);

        lib_ams::AddReadShmember(ctlout.shm_id,parent_proc);
        vrfy(read_Get(ctl0.flags), "can't open ctl0 for reading");
        vrfy(read_Get(log0.flags), "can't open log0 for reading");
        vrfy(write_Get(ctlout.flags), "can't open ctlN for writing");
    }
    // set time limit
    algo_lib::_db.limit = algo_lib::_db.clock + algo::ToSchedTime(_db.cmdline.timeout);
    // separate creation of shm file from
    if (isparent) {
        hook_Set1(frame.h_write, frame, SendMsg);
        ThScheduleRecur(frame.h_write, algo::SchedTime(ams_sendtest::_db.cmdline.senddelay_ns / algo_lib::_db.clocks_to_ns));
    }
    prlog(lib_ams::_db.proc_id<<": entering main loop");
    ams_sendtest::MainLoop();// process events
    prlog(lib_ams::_db.proc_id<<": exit main loop");
    prlog(lib_ams::_db.proc_id<<": "<<frame);
    if (algo_lib::_db.cmdline.verbose) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::DumpShmTableVisual(regx);
    }
    bool ok = false;
    if (isparent) {
        ind_beg(_db_child_curs,child,_db) {
            prlog("waiting for child "<<ind_curs(child).index+1);
            child_Wait(child);
            vrfy_(child.child_pid==0);
            prlog("waiting for child "<<ind_curs(child).index+1<<": done");
        }ind_end;
        ok = log0.c_write->off == frame.off_send;
        lib_ams::Close(log0);
    } else {
        double avg_clocks = frame.sum_recv_latency / u64_Max(frame.n_msg_recv,1);
        double avg_ns = avg_clocks * algo_lib::_db.clocks_to_ns;
        prlog("Child: avg recv latency "<< avg_ns<<" ns, read off "<<log0.c_read->off<<" off_recv "<<frame.off_recv);
        ok = log0.c_read->off == frame.off_recv;
    }
    if (!ok) {
        algo_lib::Regx regx;
        Regx_ReadSql(regx,"%",true);
        lib_ams::DumpShmTableVisual(regx);
        algo_lib::_db.exit_code=1;
    }
    prlog(lib_ams::_db.proc_id
          << "  ams_sendtest.end"
          <<Keyval("wpos",(log0.c_write ? log0.c_write->off : 0))
          <<Keyval("rpos",(log0.c_read ? log0.c_read->off : 0))
          <<Keyval("success",ok));
    lib_ams::Close(ctlout);
}
