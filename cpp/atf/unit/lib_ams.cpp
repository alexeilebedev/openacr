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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/lib_ams.cpp
//

#include "include/atf_unit.h"
#include "include/lib_ams.h"

void atf_unit::unittest_lib_ams_Test1() {
    cstring filename("temp/atf.ams_test2.bin");
    DeleteFile(filename);

    algo::UnTime ts1_in(1234567890*algo::UNTIME_PER_SEC-1);
    algo::UnTime ts2_in(2234567890*algo::UNTIME_PER_SEC);
    algo::UnTime ts3_in(3234567890*algo::UNTIME_PER_SEC+1);

    ietf::Ipv4 saddr1_in;
    ietf::Ipv4 saddr2_in;
    ietf::Ipv4 saddr3_in;

    ietf::Ipv4 daddr1_in;
    ietf::Ipv4 daddr2_in;
    ietf::Ipv4 daddr3_in;

    vrfy_(Ipv4_ReadStrptrMaybe(saddr1_in,"4.3.2.1"));
    vrfy_(Ipv4_ReadStrptrMaybe(saddr2_in,"4.3.2.2"));
    vrfy_(Ipv4_ReadStrptrMaybe(saddr3_in,"4.3.2.3"));

    vrfy_(Ipv4_ReadStrptrMaybe(daddr1_in,"240.3.2.1"));
    vrfy_(Ipv4_ReadStrptrMaybe(daddr2_in,"240.3.2.2"));
    vrfy_(Ipv4_ReadStrptrMaybe(daddr3_in,"240.3.2.3"));

    u16 sport1_in(12341);
    u16 sport2_in(12342);
    u16 sport3_in(12343);

    u16 dport1_in(59001);
    u16 dport2_in(59002);
    u16 dport3_in(59003);

    cstring msg1_payload_in("test_msg_1");
    cstring msg2_payload_in("test_msg_22");
    cstring msg3_payload_in("test_msg_3");

    algo::ByteAry msgbuf1;
    algo::ByteAry msgbuf2;
    algo::ByteAry msgbuf3;

    ams::UdpFrame &msg1_in = *ams::UdpFrame_FmtByteAry(msgbuf1,
                                                       ts1_in,
                                                       saddr1_in,
                                                       daddr1_in,
                                                       sport1_in,
                                                       dport1_in,
                                                       strptr_ToMemptr(msg1_payload_in));
    ams::UdpFrame &msg2_in = *ams::UdpFrame_FmtByteAry(msgbuf2,
                                                       ts2_in,
                                                       saddr2_in,
                                                       daddr2_in,
                                                       sport2_in,
                                                       dport2_in,
                                                       strptr_ToMemptr(msg2_payload_in));
    ams::UdpFrame &msg3_in = *ams::UdpFrame_FmtByteAry(msgbuf3,
                                                       ts3_in,
                                                       saddr3_in,
                                                       daddr3_in,
                                                       sport3_in,
                                                       dport3_in,
                                                       strptr_ToMemptr(msg3_payload_in));

    // write one message
    {
        lib_ams::FWritefile wf;
        lib_ams::OpenAppend(wf,filename);
        vrfy_(OkQ(wf));
        vrfy_(!FailQ(wf));

        WriteMsg(wf,Castbase(msg1_in));
        vrfy_(OkQ(wf));
        vrfy_(!FailQ(wf));

        vrfyeq_(wf.block.first_seqno,0);
        vrfyeq_(wf.block.n_messages,1);

        Close(wf);
        vrfy_(!OkQ(wf));
        vrfy_(!FailQ(wf));
    }

    // append two messages
    {
        lib_ams::FWritefile wf;
        lib_ams::OpenAppend(wf,filename);
        vrfy_(OkQ(wf));
        vrfy_(!FailQ(wf));

        vrfyeq_(wf.block.first_seqno,1);
        vrfyeq_(wf.block.n_messages,0);

        WriteMsg(wf,Castbase(msg2_in));
        vrfy_(OkQ(wf));
        vrfy_(!FailQ(wf));

        vrfyeq_(wf.block.first_seqno,1);
        vrfyeq_(wf.block.n_messages,1);

        WriteMsg(wf,Castbase(msg3_in));
        vrfy_(OkQ(wf));
        vrfy_(!FailQ(wf));

        vrfyeq_(wf.block.first_seqno,1);
        vrfyeq_(wf.block.n_messages,2);

        Close(wf);
        vrfy_(!OkQ(wf));
        vrfy_(!FailQ(wf));
    }

    // read messages
    {
        lib_ams::FReadfile rf;
        lib_ams::OpenRead(rf,filename);
        vrfy_(OkQ(rf));

        ams::MsgHeader *msg = ReadMsg(rf,0);
        vrfy_(OkQ(rf));
        vrfy_(msg);
        ams::UdpFrame *frame = UdpFrame_Castdown(*msg);
        vrfy_(frame);
        vrfyeq_(frame->timestamp,ts1_in);
        vrfyeq_(frame->src_ip,saddr1_in);
        vrfyeq_(frame->dst_ip,daddr1_in);
        vrfyeq_(frame->src_port,sport1_in);
        vrfyeq_(frame->dst_port,dport1_in);
        algo::memptr msg_payload_out = payload_Getary(*frame);
        vrfyeq_(ToStrPtr(msg_payload_out),msg1_payload_in);

        msg = ReadMsg(rf,1);
        vrfy_(OkQ(rf));
        vrfy_(msg);
        frame = UdpFrame_Castdown(*msg);
        vrfy_(frame);
        vrfyeq_(frame->timestamp,ts2_in);
        vrfyeq_(frame->src_ip,saddr2_in);
        vrfyeq_(frame->dst_ip,daddr2_in);
        vrfyeq_(frame->src_port,sport2_in);
        vrfyeq_(frame->dst_port,dport2_in);
        msg_payload_out = payload_Getary(*frame);
        vrfyeq_(ToStrPtr(msg_payload_out),msg2_payload_in);

        msg = ReadMsg(rf,2);
        vrfy_(OkQ(rf));
        vrfy_(msg);
        frame = UdpFrame_Castdown(*msg);
        vrfy_(frame);
        vrfyeq_(frame->timestamp,ts3_in);
        vrfyeq_(frame->src_ip,saddr3_in);
        vrfyeq_(frame->dst_ip,daddr3_in);
        vrfyeq_(frame->src_port,sport3_in);
        vrfyeq_(frame->dst_port,dport3_in);
        msg_payload_out = payload_Getary(*frame);
        vrfyeq_(ToStrPtr(msg_payload_out),msg3_payload_in);

        msg = ReadMsg(rf,3);
        vrfy_(OkQ(rf));
        vrfy_(!msg);

        msg = ReadMsg(rf,0);
        vrfy_(OkQ(rf));
        vrfy_(msg);
        frame = UdpFrame_Castdown(*msg);
        vrfy_(frame);
        vrfyeq_(frame->timestamp,ts1_in);
        msg_payload_out = payload_Getary(*frame);
        vrfyeq_(ToStrPtr(msg_payload_out),msg1_payload_in);

        Close(rf);
    }
}


void atf_unit::unittest_ams_sendtest() {
    command::ams_sendtest_proc ams_sendtest;
    ams_sendtest.cmd.timeout = 30;
    ams_sendtest.cmd.nchild = 1;
    ams_sendtest.cmd.recvdelay_ns = 100;
    ams_sendtest.cmd.msgsize_max = 1000;
    ams_sendtest.cmd.nmsg = 1000000;
    AdjustDebugPath(ams_sendtest.path);
    vrfy_(ams_sendtest_Exec(ams_sendtest) == 0);
}
