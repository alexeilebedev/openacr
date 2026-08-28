// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/fbuf.cpp
//

#include "include/atf_amc.h"

#include <openssl/err.h>

void atf_amc::cd_in_msg_Step() {
    // do nothing
}

void atf_amc::amctest_msgbuf_test0() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg == NULL);
}

void atf_amc::amctest_msgbuf_test1() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    in_buf_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg->type == hdr.type && msg->length == hdr.length);
    in_buf_SkipMsg(msgbuf);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg == NULL);
}

void atf_amc::amctest_msgbuf_test1_1() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(3));
    in_buf_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg == NULL);
}

void atf_amc::amctest_msgbuf_test1_2() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    in_buf_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    msg = in_buf_GetMsg(msgbuf);
    vrfyeq_((msg==NULL), false);
    in_buf_RemoveAll(msgbuf);
    msg = in_buf_GetMsg(msgbuf);
    vrfyeq_((msg==NULL), true);
}

void atf_amc::amctest_msgbuf_test2() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    in_buf_WriteAll(msgbuf, (u8*)&hdr, hdr.length);
    in_buf_WriteAll(msgbuf, (u8*)&hdr, hdr.length-1);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg->type == hdr.type && msg->length == hdr.length);
    in_buf_SkipMsg(msgbuf);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg == NULL);
    msgbuf.in_buf_eof = true;
    in_buf_GetMsg(msgbuf);
    vrfy_(msgbuf.in_buf_eof && !msgbuf.in_buf_msgvalid);
}

void atf_amc::amctest_msgbuf_test3() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHeader *msg;
    atf_amc::MsgHeader hdr1(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    atf_amc::MsgHeader hdr2(atf_amc::MsgType(2), atf_amc::MsgLength(4));
    atf_amc::MsgHeader hdr3(atf_amc::MsgType(3), atf_amc::MsgLength(4));
    in_buf_WriteAll(msgbuf, (u8*)&hdr1, hdr1.length);
    in_buf_WriteAll(msgbuf, (u8*)&hdr2, hdr2.length);
    in_buf_WriteAll(msgbuf, (u8*)&hdr3, hdr3.length);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg->type == hdr1.type && msg->length == hdr1.length);
    in_buf_SkipMsg(msgbuf);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg->type == hdr2.type && msg->length == hdr2.length);
    in_buf_SkipMsg(msgbuf);
    msg = in_buf_GetMsg(msgbuf);
    vrfy_(msg->type == hdr3.type && msg->length == hdr3.length);
    in_buf_SkipMsg(msgbuf);
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
    in_buf_WriteAll(msgbuf, (u8*)hdr, 4); // write first message in full
    in_buf_WriteAll(msgbuf, (u8*)hdr2, 60); // write second message partially
    //
    msg = in_buf_GetMsg(msgbuf); // get first message
    vrfy_(msg);
    vrfyeq_(msg->type, hdr->type); //it should be OK
    vrfyeq_(msg->length, hdr->length); //it should be OK
    vrfyeq_(memcmp(msg, hdr, hdr->length), 0);
    in_buf_SkipMsg(msgbuf);//skip first message

    msg = in_buf_GetMsg(msgbuf);// try to access next message
    vrfyeq_((u64)msg, (u64)0);// message should not be there

    vrfy_(in_buf_WriteAll(msgbuf, ((u8*)hdr2)+60, 4));// write remainder of second message
    msg = in_buf_GetMsg(msgbuf);// try to access second message
    vrfy_(msg);
    vrfyeq_(msg->type, hdr2->type);// message is there
    vrfyeq_(msg->length, hdr2->length);// message is there
    vrfyeq_(memcmp(msg, hdr2, hdr2->length), 0);
    in_buf_SkipMsg(msgbuf);// skip second message
    msg = in_buf_GetMsg(msgbuf);// try to access third (non-existent) message
    vrfyeq_((u64)msg, (u64)0);// it shouldn't be there
}

void atf_amc::amctest_msgbuf_test5() {
    atf_amc::Msgbuf msgbuf;

    atf_amc::MsgHeader hdr;
    hdr.length.value = sizeof(hdr);
    in_buf_WriteAll(msgbuf, (u8*)&hdr, sizeof(hdr));// write a message into the buffer
    cd_in_msg_Insert(msgbuf);
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), true);
    atf_amc::MsgHeader *msg = in_buf_GetMsg(msgbuf);// access message
    vrfyeq_(msg !=NULL, true);// message must be there
    in_buf_SkipMsg(msgbuf);// skip it
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), true);// still readable
    msg = in_buf_GetMsg(msgbuf);// access message
    vrfyeq_(msg !=NULL, false);// message is not there
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), false);// must not be readable
}

void atf_amc::amctest_msgbuf_test6() {
    atf_amc::Msgbuf msgbuf;

    atf_amc::MsgHeader hdr;
    hdr.length.value = 1;
    in_buf_WriteAll(msgbuf, (u8*)&hdr, sizeof(hdr));// write a message into the buffer
    atf_amc::MsgHeader *msg = in_buf_GetMsg(msgbuf);// access message
    vrfyeq_(msg==NULL, true);// message must NOT be there
    vrfyeq_(msgbuf.in_buf_eof, true);// eof flag must be set
    vrfyeq_(msgbuf.in_buf_err.value != 0, true);// error flag must be set
}

void atf_amc::amctest_msgbuf_test7() {
    atf_amc::Msgbuf msgbuf;

    atf_amc::MsgHeader hdr;
    hdr.length.value = 65;
    in_buf_WriteAll(msgbuf, (u8*)&hdr, sizeof(hdr));// write a message into the buffer
    atf_amc::MsgHeader *msg = in_buf_GetMsg(msgbuf);// access message
    vrfyeq_(msg==NULL, true);// message must NOT be there
    vrfyeq_(msgbuf.in_buf_eof, true);// eof flag must be set
    vrfyeq_(msgbuf.in_buf_err.value != 0, true);// error flag must be set
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
    in_buf_BeginRead(msgbuf, read_fd);// set up iohook
    // on some platforms (Windows) IoHookAdd marks the pipe readable up
    // front; normalize to the not-readable state the asserts below expect
    cd_in_msg_Remove(msgbuf);
    msgbuf.in_buf_iohook.nodelete=true;

    atf_amc::MsgHeader hdr;
    hdr.length.value = sizeof(hdr);
    vrfyeq_((u64)write(write_fd.value, &hdr, sizeof(hdr)), sizeof(hdr));// write to the write end of the pipe
    prlog("1");
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), false); // the buffer is not readable
    callback_Call(msgbuf.in_buf_iohook, msgbuf.in_buf_iohook);// call the iohook callback
    prlog("2");
    vrfyeq_(cd_in_msg_InLlistQ(msgbuf), true);// the buffer should have become readable

    atf_amc::MsgHeader *msg = in_buf_GetMsg(msgbuf);// access message
    vrfyeq_(msg !=NULL, true);// message must be there
    in_buf_SkipMsg(msgbuf);// skip it
    vrfyeq_((u64)in_buf_GetMsg(msgbuf), (u64)0);// access message -- this should return 0
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
    in_buf_BeginRead(msgbuf, read_fd);// set up iohook
    msgbuf.in_buf_iohook.nodelete=true;

    char buf[64+4];
    frep_(i,sizeof(buf)) buf[i] = i; // some pattern
    new (buf) atf_amc::MsgHeader(atf_amc::MsgType(1), atf_amc::MsgLength(4)); // first message - 4 bytes
    new (buf+4) atf_amc::MsgHeader(atf_amc::MsgType(2), atf_amc::MsgLength(64)); // second message - 64 bytes
    int nmsg_read=0;
    for (int i=0; i<ssizeof(buf); i++) {
        vrfyeq_((u64)write(write_fd.value, buf+i, 1), (u64)1);// write 1 byte
        nmsg_read += in_buf_GetMsg(msgbuf) != NULL;
        in_buf_SkipMsg(msgbuf);
    }
    vrfyeq_(msgbuf.in_buf_eof, false);
    vrfyeq_(msgbuf.in_buf_err.value, (u32)0); //  no error
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
    in_buf_BeginRead(msgbuf, read_fd);// set up iohook
    msgbuf.in_buf_iohook.nodelete=true;

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
    srandom(12346);// fixed seed: the walk below is deterministic
    while (nmsg_written < 10000 || nmsg_read < nmsg_written) {
        // rarely switch phases -- this forces system to visit different states
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
            callback_Call(msgbuf.in_buf_iohook, msgbuf.in_buf_iohook);
        }

        // phase3: read messages from buffer
        if (phase3 && cd_in_msg_InLlistQ(msgbuf)) {
            atf_amc::MsgHeader *hdr = in_buf_GetMsg(msgbuf);
            if (hdr) {
                vrfyeq_(hdr->type, nmsg_read % 65536);
            }
            vrfy_(!msgbuf.in_buf_eof);
            nmsg_read += hdr != NULL;
            in_buf_SkipMsg(msgbuf);
        }
    }

    vrfyeq_(msgbuf.in_buf_eof, false);

    char drain;
    while (read(read_fd.value,&drain,1) > 0) {// read any remaining bytes out of the pipe
    }
}

void atf_amc::amctest_msgbuf_extra_test() {
    atf_amc::Msgbuf msgbuf;
    atf_amc::MsgHdrLT hdr1(1,'A');
    atf_amc::MsgHdrLT hdr2(1,'B');
    atf_amc::MsgHdrLT hdr3(1,'O');
    out_extra_WriteMsg(msgbuf, hdr1);
    out_extra_WriteMsg(msgbuf, hdr2);
    out_extra_WriteMsg(msgbuf, hdr3);
    algo::memptr msgs(msgbuf.out_extra_elems, msgbuf.out_extra_end);
    // test the curs
    i32 cnt = 0;
    ind_beg(atf_amc::MsgHdrLT_curs,msg,msgs) {
        ++cnt;
        switch (cnt) {
            break; case 1: vrfyeq(msg->type,hdr1.type,"msg.1"); vrfyeq(msg->len,hdr1.len,"msg.1");
            break; case 2: vrfyeq(msg->type,hdr2.type,"msg.2"); vrfyeq(msg->len,hdr2.len,"msg.2");
            break; case 3: vrfyeq(msg->type,hdr3.type,"msg.3"); vrfyeq(msg->len,hdr3.len,"msg.3");
        }
    }ind_end;
    vrfyeq_(cnt, 3);
    in_extra_WriteAll(msgbuf, msgbuf.out_extra_elems, msgbuf.out_extra_end);
    atf_amc::MsgHdrLT *msg;
    msg = in_extra_GetMsg(msgbuf);
    vrfy(msg,"msg.1");
    vrfyeq(msg->type,hdr1.type,"msg.1");
    vrfyeq(msg->len,hdr1.len,"msg.1");
    in_extra_SkipMsg(msgbuf);
    msg = in_extra_GetMsg(msgbuf);
    vrfy(msg,"msg.2");
    vrfyeq(msg->type,hdr2.type,"msg.2");
    vrfyeq(msg->len,hdr2.len,"msg.2");
    in_extra_SkipMsg(msgbuf);
    msg = in_extra_GetMsg(msgbuf);
    vrfy(msg,"msg.3");
    vrfyeq(msg->type,hdr3.type,"msg.3");
    vrfyeq(msg->len,hdr3.len,"msg.3");
    in_extra_SkipMsg(msgbuf);
    msg = in_extra_GetMsg(msgbuf);
    vrfy_(msg == nullptr);
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

void atf_amc::amctest_bytebuf_dyn_test1() {
    atf_amc::BytebufDyn bytebuf;
    vrfy_(in_Max(bytebuf)==0);
    vrfy_(in_WriteAll(bytebuf, (u8*)"abcd", 4)==false);// doesn't have a buffer

    in_Realloc(bytebuf, 10);
    vrfy_(in_Max(bytebuf)==10);// added buffer space

    vrfy_(in_WriteAll(bytebuf, (u8*)"abcd", 4));

    in_Realloc(bytebuf, 3);
    vrfy_(in_Max(bytebuf)==4);// won't lose data

    strptr line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("abcd",4));
    in_SkipBytes(bytebuf, 1);// skip 1
    line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("bcd",3));
    in_SkipBytes(bytebuf, 5);// skip too many
    line = in_GetMsg(bytebuf);
    vrfyeq_(line, strptr("",0));
}

// custom framer -- 4 bytes at a time
void atf_amc::in_custom_ScanMsg(atf_amc::Msgbuf &msgbuf) {
    i32 avail = in_custom_N(msgbuf);
    msgbuf.in_custom_msgvalid = avail>=4;
    msgbuf.in_custom_msglen=4;
}

void atf_amc::amctest_msgbuf_custom() {
    atf_amc::Msgbuf msgbuf;
    // try custom framer
    in_custom_WriteAll(msgbuf,(u8*)"abcdefghij",10);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("abcd"));
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("abcd"));
    in_custom_SkipMsg(msgbuf);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("efgh"));
    in_custom_SkipMsg(msgbuf);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr(""));
    in_custom_WriteAll(msgbuf,(u8*)"kl",2);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("ijkl"));
    in_custom_SkipMsg(msgbuf);

    // try SkipBytes instead of SkipMsg
    in_custom_WriteAll(msgbuf,(u8*)"abcdefghij",10);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("abcd"));
    in_custom_SkipBytes(msgbuf,1);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("bcde"));
    in_custom_SkipBytes(msgbuf,1);
    vrfyeq_(in_custom_GetMsg(msgbuf), algo::strptr("cdef"));
}

// --------------------------------------------------------------------------------

// Read-direction fbuf on a global (FDb) ctype:
// buffer state and trace counters live on _db, and every
// generated member access must go through it.
void atf_amc::amctest_FbufGlobalRead() {
    atf_amc::in_fb_RemoveAll();
    u64 byte0 = atf_amc::_db.in_fb_n_read_byte;
    u64 msg0  = atf_amc::_db.in_fb_n_read_msg;
    vrfy_(atf_amc::in_fb_WriteAll((u8*)"hello", 5));
    algo::aryptr<char> msg = atf_amc::in_fb_GetMsg();
    vrfyeq_(msg.n_elems, 5);
    atf_amc::in_fb_SkipMsg();
    vrfyeq_(atf_amc::_db.in_fb_n_read_byte - byte0, u64(5));
    vrfyeq_(atf_amc::_db.in_fb_n_read_msg - msg0, u64(1));
    vrfy_(atf_amc::in_fb_GetMsg().elems == NULL);
}

// --------------------------------------------------------------------------------

// Write-direction fbuf on a global (FDb) ctype:
// WriteAll counts written bytes and messages on _db.
void atf_amc::amctest_FbufGlobalWrite() {
    atf_amc::out_fb_RemoveAll();
    u64 byte0 = atf_amc::_db.out_fb_n_write_byte;
    u64 msg0  = atf_amc::_db.out_fb_n_write_msg;
    vrfy_(atf_amc::out_fb_WriteAll((u8*)"hello", 5));
    vrfyeq_(atf_amc::out_fb_N(), 5);
    vrfyeq_(atf_amc::_db.out_fb_n_write_byte - byte0, u64(5));
    vrfyeq_(atf_amc::_db.out_fb_n_write_msg - msg0, u64(1));
}

// --------------------------------------------------------------------------------

// Out-direction flow control: the space condition.
// A producer that keeps writing into a full out buffer has no event to wait
// for -- it can only re-try and be refused, and a loop that re-judges a
// condition it cannot bring about is what keeps a process awake for the whole
// of a congestion episode.  The insight is that the room a producer waits for
// can appear in exactly one place, the drain, so the drain is what announces
// it.  An out fbuf therefore latches a congested flag on the write side and
// arms the space condition once the buffer falls back to its low-water mark.
// The marks are fractions of the buffer's own capacity: congestion latches at
// three quarters full, and the wake comes at one quarter.
void atf_amc::amctest_FbufSpaceDrain() {
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    u8 fill[64];// the buffer's own capacity: a structural bound, not a guess
    memset(fill, 'x', sizeof(fill));
    i32 max = atf_amc::out_buf_Max(condtest);
    vrfyeq_(max, 64);
    vrfy_(!atf_amc::cd_condtest_space_InLlistQ(condtest));
    // below the high-water mark: no congestion, so nothing to wake
    vrfy_(atf_amc::out_buf_WriteAll(condtest, fill, 40));
    vrfyeq_(atf_amc::out_buf_N(condtest), 40);
    vrfy_(!condtest.out_buf_congested);
    // crossing three quarters latches congestion; the space condition is the
    // drain's to announce, so membership does not follow the write
    vrfy_(atf_amc::out_buf_WriteAll(condtest, fill, 10));
    vrfy_(condtest.out_buf_congested);
    vrfy_(!atf_amc::cd_condtest_space_InLlistQ(condtest));
    // a partial drain that stays above one quarter is not yet room
    atf_amc::out_buf_SkipBytes(condtest, 20);
    vrfyeq_(atf_amc::out_buf_N(condtest), 30);
    vrfy_(condtest.out_buf_congested);
    vrfy_(!atf_amc::cd_condtest_space_InLlistQ(condtest));
    // falling to the low-water mark clears the latch and arms the producer
    atf_amc::out_buf_SkipBytes(condtest, 20);
    vrfyeq_(atf_amc::out_buf_N(condtest), 10);
    vrfy_(!condtest.out_buf_congested);
    vrfy_(atf_amc::cd_condtest_space_InLlistQ(condtest));
    vrfyeq_(atf_amc::cd_condtest_space_N(), 1);
    // the queue is armed on the rising edge only (rem:N), so draining further
    // neither re-arms nor releases it -- the consumer owns the removal
    atf_amc::out_buf_SkipBytes(condtest, 10);
    vrfyeq_(atf_amc::out_buf_N(condtest), 0);
    vrfy_(atf_amc::cd_condtest_space_InLlistQ(condtest));
    atf_amc::cd_condtest_space_Remove(condtest);
    // a write refused outright latches too: the message fits the buffer's
    // capacity, so the producer has room to wait for rather than an
    // impossible request
    vrfy_(atf_amc::out_buf_WriteAll(condtest, fill, 64));
    vrfy_(!atf_amc::out_buf_WriteAll(condtest, fill, 8));
    vrfy_(condtest.out_buf_congested);
    atf_amc::out_buf_SkipBytes(condtest, 64);
    vrfy_(atf_amc::cd_condtest_space_InLlistQ(condtest));
    atf_amc::cd_condtest_space_Remove(condtest);
    atf_amc::condtest_Delete(condtest);
}

// --------------------------------------------------------------------------------

// Discarding a congested out buffer is a drain like any other: the room
// appears all at once, so the producer parked on the space condition is woken
// rather than left waiting for a byte-by-byte drain that will never come.
void atf_amc::amctest_FbufSpaceRemoveAll() {
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    u8 fill[64];// the buffer's own capacity: a structural bound, not a guess
    memset(fill, 'x', sizeof(fill));
    vrfy_(atf_amc::out_buf_WriteAll(condtest, fill, 60));
    vrfy_(condtest.out_buf_congested);
    vrfy_(!atf_amc::cd_condtest_space_InLlistQ(condtest));
    atf_amc::out_buf_RemoveAll(condtest);
    vrfyeq_(atf_amc::out_buf_N(condtest), 0);
    vrfy_(!condtest.out_buf_congested);
    vrfy_(atf_amc::cd_condtest_space_InLlistQ(condtest));
    // an uncongested buffer has no producer waiting, so discarding it arms
    // nothing
    atf_amc::cd_condtest_space_Remove(condtest);
    vrfy_(atf_amc::out_buf_WriteAll(condtest, fill, 10));
    vrfy_(!condtest.out_buf_congested);
    atf_amc::out_buf_RemoveAll(condtest);
    vrfy_(!atf_amc::cd_condtest_space_InLlistQ(condtest));
    atf_amc::condtest_Delete(condtest);
}

// --------------------------------------------------------------------------------

// Write-direction fbuf with iotype:openssl:
// a TLS hard error (here: SSL_write on an SSL object with no connect/accept
// role, which fails with SSL_ERROR_SSL) must record the error code and
// unschedule the buffer from the outflow ready list, exactly like a plain
// write() hard error does; a buffer left on the list would be re-run by the
// scheduler forever.
void atf_amc::amctest_sslbuf_outflow_error() {
    atf_amc::Sslbuf sslbuf;
    SSL_CTX *ctx = SSL_CTX_new(TLS_method());
    SSL *ssl = ctx ? SSL_new(ctx) : NULL;
    // observations are collected first and asserted after the teardown, so a
    // regression this test catches does not also leak the SSL objects; each
    // starts at the value its assertion expects, so an observation the ssl
    // check preempts adds no second failure
    bool wrote = true;
    bool sched = true;
    bool flushed = false;
    bool errset = true;
    bool unsched = false;
    if (ssl) {
        sslbuf.out_ssl = ssl;
        wrote   = out_WriteAll(sslbuf, (u8*)"x", 1);
        sched   = cd_sslbuf_out_InLlistQ(sslbuf);
        flushed = out_Outflow(sslbuf);
        errset  = sslbuf.out_err.value != 0;
        unsched = cd_sslbuf_out_InLlistQ(sslbuf);
    }
    // the provoked SSL_ERROR_SSL entry stays on the thread's OpenSSL error
    // queue; whatever runs next in the same process would read it as its own
    ERR_clear_error();
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    vrfy_(ctx != NULL);
    vrfy_(ssl != NULL);
    vrfy_(wrote);
    vrfyeq_(sched, true); // WriteAll schedules outflow
    vrfyeq_(flushed, false); // hard error: nothing written
    vrfy_(errset); // error code recorded
    vrfyeq_(unsched, false); // buffer unscheduled
}

// --------------------------------------------------------------------------------

// Write-direction fbuf with iotype:openssl and nothing buffered:
// a TLS connection is scheduled for outflow as soon as its file descriptor is
// writable, which on a fresh connection happens before anything has been
// buffered, so Outflow runs with a byte count of zero. OpenSSL documents
// SSL_write with num=0 as an error, and its return of zero as a failed write.
// Outflow must therefore not reach SSL_write at all with an empty buffer: the
// call would report a failure that did not happen, and an empty buffer's
// Outflow has nothing to report. The empty buffer is unscheduled, exactly as a
// fully drained one is.
void atf_amc::amctest_sslbuf_outflow_zero() {
    atf_amc::Sslbuf sslbuf;
    SSL_CTX *ctx = SSL_CTX_new(TLS_method());
    SSL *ssl = ctx ? SSL_new(ctx) : NULL;
    // observations are collected first and asserted after the teardown, so a
    // regression this test catches does not also leak the SSL objects; each
    // starts at the value its assertion expects, so an observation the ssl
    // check preempts adds no second failure
    bool flushed = false;
    bool errset = false;
    bool sched = false;
    if (ssl) {
        sslbuf.out_ssl = ssl;
        cd_sslbuf_out_Insert(sslbuf);// schedule an empty buffer
        flushed = out_Outflow(sslbuf);
        errset  = sslbuf.out_err.value != 0;
        sched   = cd_sslbuf_out_InLlistQ(sslbuf);
    }
    ERR_clear_error();
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    vrfy_(ctx != NULL);
    vrfy_(ssl != NULL);
    vrfyeq_(flushed, false); // nothing was written
    vrfyeq_(errset, false); // SSL_write was not called, so no error was recorded
    vrfyeq_(sched, false); // empty buffer unscheduled
}

// --------------------------------------------------------------------------------

// Fbuf backed by a private lpool:
// Realloc allocates the buffer as plain bytes, and Uninit must return it to
// the pool with that same byte size. The lpool files a freed block on a
// freelist keyed by the free size, so an inflated size (sizeof(arg)*max)
// would park the 8K record on the 32K freelist and a later 32K request
// would be served only 8K of memory.
void atf_amc::amctest_fbuf_lpool_free() {
    void *elems = NULL;
    {
        atf_amc::Lpoolbuf lpoolbuf;
        elems = lpoolbuf.in_elems;
        vrfy_(elems != NULL);
        vrfyeq_(lpoolbuf.in_max, u32(8192));
    }
    void *blk32 = atf_amc::lpool_AllocMem(sizeof(atf_amc::MsgHeader) * 8192);
    vrfy_(blk32 != elems); // the 32K freelist must not hold the 8K record
    void *rec8 = atf_amc::lpool_AllocMem(8192);
    vrfy_(rec8 == elems); // the record is recycled within its own size class
    atf_amc::lpool_FreeMem(rec8, 8192);
    atf_amc::lpool_FreeMem(blk32, sizeof(atf_amc::MsgHeader) * 8192);
}

// A datagram buffer walks the whole messages one datagram packed, in order.
void atf_amc::amctest_dgrambuf_walk() {
    atf_amc::Dgrambuf dgrambuf;
    atf_amc::MsgHeader first(atf_amc::MsgType(1), atf_amc::MsgLength(4));
    atf_amc::MsgHeader second(atf_amc::MsgType(2), atf_amc::MsgLength(4));
    in_WriteAll(dgrambuf, (u8*)&first, first.length);
    in_WriteAll(dgrambuf, (u8*)&second, second.length);
    atf_amc::MsgHeader *msg = in_GetMsg(dgrambuf);
    vrfy_(msg != NULL && msg->type == first.type);
    in_SkipMsg(dgrambuf);
    msg = in_GetMsg(dgrambuf);
    vrfy_(msg != NULL && msg->type == second.type);
    in_SkipMsg(dgrambuf);
    vrfy_(in_GetMsg(dgrambuf) == NULL);
}

// A length the datagram cannot satisfy ends the datagram and raises no eof.
//
// This is the property the whole datagram framing exists for.  A stream framer
// answers an unframeable length by setting eof, which for a datagram socket is
// a lie -- there is no end of input -- and it strands the bad header at the
// buffer's start, where it is re-scanned forever.  A datagram buffer reports no
// message and leaves eof alone, so the next refill drops the bad bytes with the
// datagram they came in and the interface keeps serving.
void atf_amc::amctest_dgrambuf_badlen() {
    atf_amc::Dgrambuf dgrambuf;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(3));
    in_WriteAll(dgrambuf, (u8*)&hdr, ssizeof(hdr));
    vrfy_(in_GetMsg(dgrambuf) == NULL);
    vrfyeq_(dgrambuf.in_eof, false);
}

// A message whose declared length runs past the datagram is not returned, and
// it too raises no eof: the bytes are a truncated tail, not an error.
void atf_amc::amctest_dgrambuf_overrun() {
    atf_amc::Dgrambuf dgrambuf;
    atf_amc::MsgHeader hdr(atf_amc::MsgType(1), atf_amc::MsgLength(32));
    in_WriteAll(dgrambuf, (u8*)&hdr, ssizeof(hdr));
    vrfy_(in_GetMsg(dgrambuf) == NULL);
    vrfyeq_(dgrambuf.in_eof, false);
}
