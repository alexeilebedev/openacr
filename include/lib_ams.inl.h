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
// Exceptions: yes
// Header: include/lib_ams.inl.h
//
// -----------------------------------------------------------------------------
// The length field of a message must reside entirely within one cache line,
// or the sfence/lfence instructions won't behave as expected.
// So, we force each message to start at an address that'a a multiple of machine
// cache line size.

inline u64 lib_ams::AddOffset(u64 offset, int n) {
    return (offset + n + 63) & ~63;
}

inline ams::Shmmsg *lib_ams::MsgAtOffset(lib_ams::FShm &shm, u64 offset) {
    offset = offset & shm.offset_mask;
    return (ams::Shmmsg*)(shm.shm_region.elems + offset);
}

// next_ackoff is the read offset at which an unconditional
// Shmhb is sent out. It is enabled when the shm is being read.
inline void lib_ams::UpdateAckOffset(lib_ams::FShm &shm, lib_ams::FShmember &shmember) {
    shm.next_ackoff = shmember.off + shm.offset_mask / 8;
}

inline ams::ProcId lib_ams::MakeProcId(ams::Proctype proctype, int node, int index) {
    ams::ProcId ret;
    proctype_Set(ret,proctype);
    nodeidx_Set(ret,node);
    procidx_Set(ret,index);
    return ret;
}

inline u64 lib_ams::GetWriteBudget(lib_ams::FShm &shm) {
    return algo::u64_SubClip(shm.writelimit, shm.c_write->off);
}
