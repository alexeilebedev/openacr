// Copyright (C) 2025-2026 AlgoRND
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
// Header: include/lib_ams.inl.h
//
// The length field of a message must reside entirely within one cache line,
// or the sfence/lfence instructions won't behave as expected.
// So, we force each message to start at an address that'a a multiple of machine
// cache line size.

inline u64 lib_ams::AddOffset(u64 offset, int n) {
    return (offset + n + 63) & ~63;
}

inline ams::MsgHeader *lib_ams::MsgAtOffset(lib_ams::FShm &shm, u64 offset) {
    return (ams::MsgHeader*)(shm.c_data + (offset & shm.offset_mask));
}

inline ams::ProcId lib_ams::MakeProcId(ams::Proctype proctype, int node, int index) {
    ams::ProcId ret;
    proctype_Set(ret,proctype);
    nodeidx_Set(ret,node);
    procidx_Set(ret,index);
    return ret;
}

// Inbound shm group for a bridged user process: child writes, parent reads.
// GRPIDX disambiguates multiple bridges between the same proc pair.
inline ams::GrpId lib_ams::BridgeInGrp(ams::ProcId child_proc_id, int grpidx) {
    return ams::GrpId(child_proc_id, ams::Grptype(ams_Grptype_userpr), grpidx);
}

// Outbound shm group for a bridged user process: parent writes, child reads.
// GRPIDX disambiguates multiple bridges between the same proc pair.
inline ams::GrpId lib_ams::BridgeOutGrp(ams::ProcId child_proc_id, int grpidx) {
    return ams::GrpId(child_proc_id, ams::Grptype(ams_Grptype_pruser), grpidx);
}

inline algo::memptr lib_ams::MsgBytes(ams::MsgHeader &msg) {
    return algo::memptr((u8*)&msg, msg.length);
}

inline algo::Alloc lib_ams::GetAlloc(lib_ams::FShm &shm) {
    algo::Alloc alloc;
    alloc.ctx = &shm;
    alloc.begin = algo::BeginAllocFcn(lib_ams::BeginWrite);
    alloc.end = algo::EndAllocFcn(lib_ams::EndWrite);
    return alloc;
}

// Like GetAlloc, but the begin hook blocks (busy-waits for a max_msg_size
// slot) instead of returning NULL when the ring is full -- so a *_FmtShm
// built on it never drops, it backpressures the writer.
inline algo::Alloc lib_ams::GetAllocBlock(lib_ams::FShm &shm) {
    algo::Alloc alloc;
    alloc.ctx = &shm;
    alloc.begin = algo::BeginAllocFcn(lib_ams::BeginWriteBlock);
    alloc.end = algo::EndAllocFcn(lib_ams::EndWrite);
    return alloc;
}

// Like GetAlloc, but a message the ring has no room for is queued on the ring
// and written by lib_ams's own step as budget appears -- so a *_FmtAlloc built
// on it neither drops nor blocks, and the caller has nothing to test.  This is
// what a message with no other retry behind it is written through; see
// cpp/lib_ams/outmsg.cpp.
inline algo::Alloc lib_ams::GetAllocQueue(lib_ams::FShm &shm) {
    algo::Alloc alloc;
    alloc.ctx = &shm;
    alloc.begin = algo::BeginAllocFcn(lib_ams::BeginWriteQueue);
    alloc.end = algo::EndAllocFcn(lib_ams::EndWriteQueue);
    return alloc;
}
