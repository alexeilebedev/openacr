// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/psp.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/algo/pcbuf.h"
#include "include/algo/pcbuf.inl.h"
#include "include/gen/algo_lib_gen.h"
#include "include/gen/algo_lib_gen.inl.h"

struct ZSLink {
    ZSLink* next;
    ZSLink() :next(NULL){}
};

void* algo::TryReadPtr(algo::PCPBuf &B, u32 &read_ptr) {
    u32 i         = read_ptr;
    u32 limit     = B.nmsgs;
    PCPLine &line = Access(B, i & (limit-1));
    void *data    = line.data[0];
    if (!data) {
        return NULL;
    }
    line.data[0] = NULL;
    read_ptr = i+1;
    return data;
}

void algo::WritePtr(PCPBuf &B, void *ptr) {
    u32 limit     = B.nmsgs;
    u32 i         = IncrMT(&B.write_ptr)-1;
    algo::PCPLine &line = algo::Access(B, i & (limit-1));
    do {
        if (!line.data[0]) {
            line.data[0]  = ptr;
            return;
        }
    } while (true);
}

bool algo::WritePtrSP(PCPBuf &B, void *ptr, bool block) {
    u32 limit     = B.nmsgs;
    u32 i         = B.write_ptr;
    algo::PCPLine &line = algo::Access(B, i & (limit-1));
    do {
        if (!line.data[0]) {
            line.data[0]  = ptr;
            B.write_ptr = i+1;
            return true;
        }
    } while (block);
    return false;
}

u8 *algo::BeginWrite(PCBuf &F, u32 nbytes) {
    using namespace pcbuf;
    u8 *v;
    while ((v = TryBeginWrite(F,nbytes))==NULL) {
    }
    return v;
}

void algo::EndWrite(PCBuf &F, memptr buf) {
    using namespace pcbuf;
    MsgHdr *msg      = (MsgHdr*)(buf.elems - sizeof(MsgHdr));
    u64     ptr      = (u8*)msg - F.buf.elems;
    u64     msg_end  = u64_CeilPow2(ptr + u32(sizeof(MsgHdr)) + elems_N(buf), CACHE_LINE);
    msg->length      = elems_N(buf);
    msg->next        = (u32)msg_end;
    F.write_next     = (u32)msg_end;
    sfence();
    F.write_ptr      = (u32)msg_end;
}


u8 *algo::TryBeginWrite(PCBuf &F, u32 nbytes) {
    using namespace pcbuf;
    u32 w       = F.write_next;
    u32 r       = F.read_ptr;
    u32 buf_end = elems_N(F.buf);
    u32 msg_end = u32_CeilPow2(w + u32(sizeof(ZSLink)) + nbytes, CACHE_LINE);
    if (r > w && msg_end >= r) return NULL;
    MsgHdr *msg = (MsgHdr*)(F.buf.elems +  w);
    if (msg_end + sizeof(ZSLink) > buf_end) {
        msg_end = u32_CeilPow2(u32(sizeof(ZSLink)) + nbytes, CACHE_LINE);
        if (msg_end >= r) {
            return NULL;
        }
        msg->next = 0;
        msg = (MsgHdr*)F.buf.elems;
    }
    msg->next = msg_end;
    msg->length = nbytes;
    F.write_next = msg_end;
    return (u8*)(msg+1);
}

memptr algo::TryBeginRead(PCBuf &F) {
    using namespace pcbuf;
    u32 r = F.read_next;
    u32 w = F.write_ptr;
    if (r != w) {
        // we have a data dependency. do we need an lfence?
        lfence();
        MsgHdr *hdr = (MsgHdr*)(F.buf.elems + r);
        u32 next = hdr->next;
        if (next > r) {
            F.read_next = next;
            return memptr((u8*)(hdr+1),hdr->length);
        }
        r = next;
        F.read_ptr = next;
        if (r != w) {
            hdr = (MsgHdr*)(F.buf.elems + r);
            next = hdr->next;
            F.read_next = next;
            return memptr((u8*)(hdr+1),hdr->length);
        }
    }
    return memptr();
}

void algo::InitPCPBuf(PCPBuf &B, u32 nbits) {
    PageBufInit(B.backing, sizeof(PCPLine) * (u64(1)<<nbits), getpagesize());
    B.nmsgs = 1<<nbits;
    Fill(B.backing, u8(0));
}
