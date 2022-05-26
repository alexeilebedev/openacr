// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/fbuf.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

void atf_amc::cd_in_msg_Step() {
    // do nothing
}

void atf_amc::amctest_msgbuf_test0() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg = in_GetMsg(msgbuf);
    vrfy_(msg == NULL);
}

void atf_amc::amctest_msgbuf_test1() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    in_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg->type == hdr.type && msg->length == hdr.length);
    in_SkipMsg(msgbuf);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg == NULL);
}

void atf_amc::amctest_msgbuf_test1_1() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(3));
    in_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg == NULL);
}

void atf_amc::amctest_msgbuf_test1_2() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    in_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    msg = in_GetMsg(msgbuf);
    vrfyeq_((msg==NULL), false);
    in_RemoveAll(msgbuf);
    msg = in_GetMsg(msgbuf);
    vrfyeq_((msg==NULL), true);
}

void atf_amc::amctest_msgbuf_test2() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    in_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    in_WriteAll(msgbuf, (u8*)&hdr, hdr.length-1);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg->type == hdr.type && msg->length == hdr.length);
    in_SkipMsg(msgbuf);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg == NULL);
    msgbuf.in_eof = true;
    in_GetMsg(msgbuf);
    vrfy_(msgbuf.in_eof && !msgbuf.in_msgvalid);
}

void atf_amc::amctest_msgbuf_test3() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr1(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    atf_amc::MsgHeader hdr2(atf_amc::MsgType(2), atf_amc::MsgLength(4));
    atf_amc::MsgHeader hdr3(atf_amc::MsgType(3), atf_amc::MsgLength(4));
    in_WriteAll(msgbuf, (u8*)&hdr1, hdr1.length);
    in_WriteAll(msgbuf, (u8*)&hdr2, hdr2.length);
    in_WriteAll(msgbuf, (u8*)&hdr3, hdr3.length);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg->type == hdr1.type && msg->length == hdr1.length);
    in_SkipMsg(msgbuf);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg->type == hdr2.type && msg->length == hdr2.length);
    in_SkipMsg(msgbuf);
    msg = in_GetMsg(msgbuf);
    vrfy_(msg->type == hdr3.type && msg->length == hdr3.length);
    in_SkipMsg(msgbuf);
}

void atf_amc::amctest_msgbuf_test4() {
    atf_amc::Msgbuf msgbuf;
    char buf[64+4];
    frep_(i,sizeof(buf)) buf[i] = i; // some pattern
    atf_amc::MsgHeader *msg;
    // first message - 4 bytes
    atf_amc::MsgHeader *hdr=new (buf) atf_amc::MsgHeader(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    // second message - 64 bytes
    atf_amc::MsgHeader *hdr2=new (buf+4) atf_amc::MsgHeader(atf_amc::MsgType(2), atf_amc::MsgLength(64));
    in_WriteAll(msgbuf, (u8*)hdr, 4); // write first message in full
    in_WriteAll(msgbuf, (u8*)hdr2, 60); // write second message partially
    //
    msg = in_GetMsg(msgbuf); // get first message
    vrfyeq_(msg->type, hdr->type); //it should be OK
    vrfyeq_(msg->length, hdr->length); //it should be OK
    vrfyeq_(memcmp(msg, hdr, hdr->length), 0);
    in_SkipMsg(msgbuf);//skip first message

    msg = in_GetMsg(msgbuf);// try to access next message
    vrfyeq_((u64)msg, (u64)0);// message should not be there

    in_WriteAll(msgbuf, ((u8*)hdr2)+60, 4);// write remainder of second message
    msg = in_GetMsg(msgbuf);// try to access second message
    vrfyeq_(msg->type, hdr2->type);// message is there
    vrfyeq_(msg->length, hdr2->length);// message is there
    vrfyeq_(memcmp(msg, hdr2, hdr2->length), 0);
    in_SkipMsg(msgbuf);// skip second message
    msg = in_GetMsg(msgbuf);// try to access third (non-existent) message
    vrfyeq_((u64)msg, (u64)0);// it shouldn't be there
}

void atf_amc::amctest_msgbuf_test5() {
    atf_amc::Msgbuf msgbuf;

    atf_amc::MsgHeader hdr;
    hdr.length.value = sizeof(hdr);
    in_WriteAll(msgbuf, (u8*)&hdr, sizeof(hdr));// write a message into the buffer
    cd_in_msg_Insert(msgbuf);
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), true);
    atf_amc::MsgHeader *msg = in_GetMsg(msgbuf);// access message
    vrfyeq_(msg !=NULL, true);// message must be there
    in_SkipMsg(msgbuf);// skip it
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), true);// still readable
    msg = in_GetMsg(msgbuf);// access message
    vrfyeq_(msg !=NULL, false);// message is not there
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), false);// must not be readable
}

void atf_amc::amctest_msgbuf_test6() {
    atf_amc::Msgbuf msgbuf;

    atf_amc::MsgHeader hdr;
    hdr.length.value = 1;
    in_WriteAll(msgbuf, (u8*)&hdr, sizeof(hdr));// write a message into the buffer
    atf_amc::MsgHeader *msg = in_GetMsg(msgbuf);// access message
    vrfyeq_(msg==NULL, true);// message must NOT be there
    vrfyeq_(msgbuf.in_eof, true);// eof flag must be set
    vrfyeq_(msgbuf.in_err.value != 0, true);// error flag must be set
}

void atf_amc::amctest_msgbuf_test7() {
    atf_amc::Msgbuf msgbuf;

    atf_amc::MsgHeader hdr;
    hdr.length.value = 65;
    in_WriteAll(msgbuf, (u8*)&hdr, sizeof(hdr));// write a message into the buffer
    atf_amc::MsgHeader *msg = in_GetMsg(msgbuf);// access message
    vrfyeq_(msg==NULL, true);// message must NOT be there
    vrfyeq_(msgbuf.in_eof, true);// eof flag must be set
    vrfyeq_(msgbuf.in_err.value != 0, true);// error flag must be set
}

void atf_amc::amctest_msgbuf_test8() {
    // set up a couple of pipes for testing msgbuf.
    // they will be used from here on to the end
    int pipefd[2];
    vrfy(pipe(pipefd) == 0, "pipe");
    algo::Fildes read_fd(pipefd[0]);
    algo::Fildes write_fd(pipefd[1]);
    (void)algo::SetBlockingMode(read_fd, false);
    (void)algo::SetBlockingMode(write_fd, false);

    atf_amc::Msgbuf msgbuf;
    in_BeginRead(msgbuf, read_fd);// set up iohook
    // #AL# on platforms like Windows, the pipe may be
    // market readable in IoHookAdd. until this is fixed, normalize desired state.
    // it doesn't affect correctness
    cd_in_msg_Remove(msgbuf);
    msgbuf.in_iohook.nodelete=true;

    atf_amc::MsgHeader hdr;
    hdr.length.value = sizeof(hdr);
    vrfyeq_((u64)write(write_fd.value, &hdr, sizeof(hdr)), sizeof(hdr));// write to the write end of the pipe
    prlog("1");
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), false); // the buffer is not readable
    callback_Call(msgbuf.in_iohook, msgbuf.in_iohook);// call the iohook callback
    prlog("2");
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), true);// the buffer should have become readable

    atf_amc::MsgHeader *msg = in_GetMsg(msgbuf);// access message
    vrfyeq_(msg !=NULL, true);// message must be there
    in_SkipMsg(msgbuf);// skip it
    vrfyeq_((u64)in_GetMsg(msgbuf), (u64)0);// access message -- this should return 0
    prlog("3");
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), false);// the buffer should have become NOT readable

    char drain;
    while (read(read_fd.value,&drain,1) > 0) {// read any remaining bytes out of the pipe
    }
}

void atf_amc::amctest_msgbuf_test9() {
    // set up a couple of pipes for testing msgbuf.
    // they will be used from here on to the end
    int pipefd[2];
    vrfy(pipe(pipefd) == 0, "pipe");
    algo::Fildes read_fd(pipefd[0]);
    algo::Fildes write_fd(pipefd[1]);
    (void)algo::SetBlockingMode(read_fd, false);
    (void)algo::SetBlockingMode(write_fd, false);

    atf_amc::Msgbuf msgbuf;
    in_BeginRead(msgbuf, read_fd);// set up iohook
    msgbuf.in_iohook.nodelete=true;

    char buf[64+4];
    frep_(i,sizeof(buf)) buf[i] = i; // some pattern
    new (buf) atf_amc::MsgHeader(atf_amc::MsgType(1), atf_amc::MsgLength(4)); // first message - 4 bytes
    new (buf+4) atf_amc::MsgHeader(atf_amc::MsgType(2), atf_amc::MsgLength(64)); // second message - 64 bytes
    int nmsg_read=0;
    for (int i=0; i<ssizeof(buf); i++) {
        vrfyeq_((u64)write(write_fd.value, buf+i, 1), (u64)1);// write 1 byte
        nmsg_read += in_GetMsg(msgbuf) != NULL;
        in_SkipMsg(msgbuf);
    }
    vrfyeq_(msgbuf.in_eof, false);
    vrfyeq_(msgbuf.in_err.value, (u32)0); //  no error
    vrfyeq_(nmsg_read, 2); //  two messages should have been read
    char drain;
    while (read(read_fd.value,&drain,1) > 0) {// read any remaining bytes out of the pipe
    }
}

void atf_amc::amctest_msgbuf_test10() {
    // set up a couple of pipes for testing msgbuf.
    // they will be used from here on to the end
    int pipefd[2];
    vrfy(pipe(pipefd) == 0, "pipe");
    algo::Fildes read_fd(pipefd[0]);
    algo::Fildes write_fd(pipefd[1]);
    (void)algo::SetBlockingMode(read_fd, false);
    (void)algo::SetBlockingMode(write_fd, false);

    atf_amc::Msgbuf msgbuf;
    in_BeginRead(msgbuf, read_fd);// set up iohook
    msgbuf.in_iohook.nodelete=true;

    char buf[64];
    memset(buf,0,sizeof(buf));
    bool msg_valid = false;
    int msg_size = 0;
    int nmsg_written = 0;
    int nmsg_read = 0;
    int msg_offset = 0;
    bool phase1 = true;
    bool phase2 = true;
    bool phase3 = true;
    while (nmsg_written < 10000 || nmsg_read < nmsg_written) {
        // rarely switch phases -- this forces system to visit different states
        srandom(12346);
        if (algo::i32_WeakRandom(100) < 5) phase1 = !phase1;
        if (algo::i32_WeakRandom(100) < 5) phase2 = !phase2;
        if (algo::i32_WeakRandom(100) < 5) phase3 = !phase3;
        phase1 &= nmsg_written < 10000;
        // phase 1: stuff messages into pipe
        if (phase1) {
            if (!msg_valid) {
                msg_size = i32_Max(algo::i32_WeakRandom(sizeof(buf)-1), 4);
                // randomly sized message
                new (buf) atf_amc::MsgHeader(atf_amc::MsgType(nmsg_written), atf_amc::MsgLength(msg_size));
                msg_valid = true;
                msg_offset=0;
            }
            if (msg_valid) {
                int nwrite = i32_Min(msg_size-msg_offset, algo::i32_WeakRandom(sizeof(buf)-1));
                int written=write(write_fd.value, buf+msg_offset, nwrite);
                if (written>0) {
                    msg_offset += written;
                    msg_valid = msg_offset==msg_size ? false : msg_valid;
                    nmsg_written += msg_offset == msg_size;
                }
            }
        }
        // phase2: read messages into buffer
        if (phase2) {
            callback_Call(msgbuf.in_iohook, msgbuf.in_iohook);
        }

        // phase3: read messages from buffer
        if (phase3 && cd_in_msg_InLlistQ(msgbuf)) {
            atf_amc::MsgHeader *hdr = in_GetMsg(msgbuf);
            if (hdr) {
                vrfyeq_(hdr->type, nmsg_read % 65536);
            }
            vrfy_(!msgbuf.in_eof);
            nmsg_read += hdr != NULL;
            in_SkipMsg(msgbuf);
        }
    }

    vrfyeq_(msgbuf.in_eof, false);

    char drain;
    while (read(read_fd.value,&drain,1) > 0) {// read any remaining bytes out of the pipe
    }
}

void atf_amc::amctest_linebuf_test1() {
    atf_amc::Linebuf linebuf;
    strptr line = in_GetMsg(linebuf);
    vrfyeq_((u64)line.elems, (u64)NULL);
}

void atf_amc::amctest_linebuf_test2() {
    atf_amc::Linebuf linebuf;
    in_WriteAll(linebuf, (u8*)"abcd\n", 5);
    strptr line = in_GetMsg(linebuf);
    vrfyeq_(line, strptr("abcd"));
    in_SkipMsg(linebuf);
}

void atf_amc::amctest_linebuf_test3() {
    atf_amc::Linebuf linebuf;
    in_WriteAll(linebuf, (u8*)"abcd", 4);
    strptr line = in_GetMsg(linebuf);
    vrfyeq_((u64)line.elems, (u64)NULL);
    in_SkipMsg(linebuf);
    in_WriteAll(linebuf, (u8*)"\n", 1);
    line = in_GetMsg(linebuf);
    vrfyeq_(line, strptr("abcd"));
    in_SkipMsg(linebuf);
}

void atf_amc::amctest_linebuf_test4() {
    atf_amc::Linebuf linebuf;
    in_WriteAll(linebuf,(u8*)"\n",1);
    strptr line = in_GetMsg(linebuf);
    vrfyeq_(line, strptr(""));
    in_SkipMsg(linebuf);
}

void atf_amc::amctest_linebuf_test5() {
    atf_amc::Linebuf linebuf;
    for(int i = 0; i < 1000; i++) {
        in_WriteAll(linebuf,(u8*)" ",1);
    }
    strptr line = in_GetMsg(linebuf);
    vrfyeq_(line.elems==NULL, true);
    vrfyeq_(linebuf.in_err.value!=0, true);
    vrfyeq_(linebuf.in_eof, true);
}

void atf_amc::amctest_bytebuf_test1() {
    atf_amc::Bytebuf bytebuf;
    strptr line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("",0));
}

void atf_amc::amctest_bytebuf_test2() {
    atf_amc::Bytebuf bytebuf;
    in_WriteAll(bytebuf, (u8*)"abcd", 4);
    strptr line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("abcd",4));
    in_SkipBytes(bytebuf, 1);// skip 1
    line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("bcd",3));
    in_SkipBytes(bytebuf, 5);// skip too many
    line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("",0));
}
