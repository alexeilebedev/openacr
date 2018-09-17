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
// Header: include/algo/pcbuf.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once

namespace algo {

    struct PCBuf {
        PageBuf      buf;
        ifmsc(CACHE_ALIGN) volatile u32 write_ptr  ifgcc(CACHE_ALIGN);
        ifmsc(CACHE_ALIGN)          u32 write_next ifgcc(CACHE_ALIGN);
        ifmsc(CACHE_ALIGN) volatile u32 read_ptr   ifgcc(CACHE_ALIGN);
        ifmsc(CACHE_ALIGN)          u32 read_next  ifgcc(CACHE_ALIGN);
        PCBuf();
    };

#ifdef WIN32
#pragma warning(pop)
#endif

    struct PCPLine {
        void* volatile data[8];
    };

    struct PCPBuf {
        ifmsc(CACHE_ALIGN) u32 volatile   write_ptr ifgcc(CACHE_ALIGN);
        int            nmsgs;
        PageBuf        backing;
        PCPBuf();
    };

    namespace pcbuf {
        struct MsgHdr {
            u32 next;
            u32 length;
        } __attribute__((aligned(64)));
    }

    // The writing thread avoids reading write_ptr, while the reading thread
    // avoids reading read_ptr.

    inline PCBuf::PCBuf()
        : write_ptr(0)
        , write_next(0)
        , read_ptr(0)
        , read_next(0)
        {
        }

    // The size of the buffer should be at least twice the size of the largest
    // message + 64.
    void           Init(PCBuf &F, u64 nbytes);
    // Attempt to start new message of size NBYTES and return pointer to it.
    // If the buffer is full, return NULL.
    u8   *TryBeginWrite(PCBuf &F, u32 nbytes);
    // Start new message of size NBYTES and return pointer to it.
    // Spin in infinite loop until enough space is available.
    u8      *BeginWrite(PCBuf &F, u32 nbytes);
    // Finish writing message and allow another thread to read it.
    void       EndWrite(PCBuf &F);
    // Finish writing a smaller message than originally requested.
    // BUF.elems must match pointer returned from BeginWrite. Length may
    // be same or smaller. This function is typically used to read data
    // into a buffer and then immediately send it to another thread. Since the read
    // may be smaller than anticipated, the remaining space can be reclaimed.
    void       EndWrite(PCBuf &F, memptr buf);
    // BeginWrite / memcpy / EndWrite in one
    void          Write(PCBuf &F, memptr bytes);
    // Retrieves next message from buffer.
    // If elems=NULL in the result, no new messages are available
    // TryBeginRead must be balanced with a call to EndRead.
    // Calling TryBeginRead multiple times will return the same pointer, until advanced.
    memptr TryBeginRead(PCBuf &F);
    // Allows space for current message to be overwritten by the writing thread
    void        EndRead(PCBuf &F);
    // Check if there are any messages available for reading.
    bool       AllReadQ(PCBuf &F);

    // Not allowed to write NULL pointers
    void        InitPCPBuf(PCPBuf &B, u32 nbits);
    void*          ReadPtr(PCPBuf &B, u32 &read_index);
    // Any number of producers, but they must block.
    void          WritePtr(PCPBuf &B, void *ptr);
    //
    // WritePtrSP       Write pointer, single-producer case.
    //
    // PTR              Data pointer to be written
    // BLOCK            Returns true if pointer written successfully.
    //                  If flag is false, function spins until success.
    //
    bool        WritePtrSP(PCPBuf &B, void *ptr, bool block=true);

    PCPLine &Access(algo::PCPBuf &B, u32 idx);
    void* TryReadPtr(algo::PCPBuf &B, u32 &read_ptr);

}
