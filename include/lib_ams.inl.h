// Copyright (C) 2025-2026 AlgoX2 Corp
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
    return algo::Alloc(shm, lib_ams::BeginWrite, lib_ams::EndWrite);
}

// Like GetAlloc, but the begin hook blocks (busy-waits for a max_msg_size
// slot) instead of returning NULL when the ring is full -- so a *_FmtShm
// built on it never drops, it backpressures the writer.
inline algo::Alloc lib_ams::GetAllocBlock(lib_ams::FShm &shm) {
    return algo::Alloc(shm, lib_ams::BeginWriteBlock, lib_ams::EndWrite);
}

// An allocator that formats a message straight into BOARD's chunk being
// filled, so a *_FmtAlloc built on it costs no copy: the begin hook is
// BoardAlloc and returns NULL when no chunk can take the message, and the end
// hook trims the reservation to the length the format used.  Nothing is sent
// by the format; the caller sends the message with BoardSend, or gives the
// space back with BoardTrim.
inline algo::Alloc lib_ams::BoardGetAlloc(lib_ams::FShm &board) {
    algo::Alloc alloc;
    alloc.ctx = &board;
    alloc.begin = algo::BeginAllocFcn(lib_ams::BoardAlloc);
    alloc.end = algo::EndAllocFcn(lib_ams::BoardTrim);
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

// TRUE when SHM is a message board rather than a lane ring.
inline bool lib_ams::BoardQ(lib_ams::FShm &shm) {
    return shm.grp_id.grptype == ams_Grptype_board;
}

// Send every wakeup the pass owes now, without waiting for the end of the pass.
// A caller that is about to wait outside the loop calls it first: WaitBudget
// before it spins on a peer that may be asleep, ShmClose before the ring's
// mapping goes, and leaving signaled mode before the lists stop being drained.
inline void lib_ams::FlushWake() {
    lib_ams::zd_wake_Call();
}

// TRUE when this process, a reader of SHM, has at most half the ring left unread.
inline bool lib_ams::HalfDrainedQ(lib_ams::FShm &shm) {
    return shm.c_shmhdr->woff - shm.c_reader->offset <= u64(shm.offset_mask+1)/2;
}

// Address of CHUNK's first byte in this process's mapping of its board.
inline u8 *lib_ams::ChunkAddr(lib_ams::FChunk &chunk) {
    return chunk.p_board->shm_region.elems + chunk.offset;
}

// Count one hold on CHUNK that no reader releases: a module keeping records
// that index into the chunk takes one, and the chunk is not reused until
// ChunkRelease gives it back.
inline void lib_ams::ChunkHold(lib_ams::FChunk &chunk) {
    chunk.nref++;
}

// The free room at the end of CHUNK: where the next reservation would start, and
// the bytes left to it.  Nothing is reserved by asking; a caller that reads data
// into the room reserves what landed with ChunkAlloc, and nothing else may
// reserve in the chunk in between.
inline algo::memptr lib_ams::ChunkRoom(lib_ams::FChunk &chunk) {
    return algo::memptr(lib_ams::ChunkAddr(chunk) + chunk.used, i64(chunk.p_board->chunk_size - chunk.used));
}

// Shorten the most recent reservation in CHUNK, at PTR, to LEN bytes, so the
// next reservation starts right after it; a LEN of zero gives the space back
// entirely.  Only the last reservation in a chunk can move, and only while no
// reference to it has been sent.
inline void lib_ams::ChunkTrim(lib_ams::FChunk &chunk, void *ptr, int len) {
    u32 start = u32((u8*)ptr - lib_ams::ChunkAddr(chunk));
    chunk.used = start + u32(len);
}

// Board offset of MSG, a message in CHUNK: the coordinate a reference carries,
// which a reader resolves against its own mapping of the board.
inline u64 lib_ams::BoardOffset(lib_ams::FChunk &chunk, ams::MsgHeader &msg) {
    return chunk.offset + u64((u8*)&msg - lib_ams::ChunkAddr(chunk));
}

// The message of LENGTH bytes at board OFFSET on lane SHM's writer's board, or
// NULL when the coordinates do not describe a message this process can see.
//
// The bounds test and the length cross-check are not ceremony: one chunk serves
// every recipient of the message, so a reference that has gone stale -- through
// a sender accounting error, or a chunk reused before a reader was done with it
// -- would hand the same wrong bytes to every reader at once.  A reference that
// fails either test is refused rather than dispatched.
inline ams::MsgHeader *lib_ams::BoardResolve(lib_ams::FShm &shm, u64 offset, u32 length) {
    ams::MsgHeader *msg = (ams::MsgHeader*)lib_ams::BoardSpan(shm, offset, length);
    return msg && msg->length == length ? msg : NULL;
}

// The payload BOARDREF names on lane SHM's writer's board, or NULL when it does
// not describe a message this process can see; the coordinate form is above.
inline ams::MsgHeader *lib_ams::BoardResolve(lib_ams::FShm &shm, ams::BoardrefMsg &boardref) {
    return lib_ams::BoardResolve(shm, boardref.offset, boardref.payload_length);
}

// True when SPEC names a ring pair, both IN and OUT: the process is a child
// bridged to a server's namespace, and speaks to it over shared memory.
inline bool lib_ams::BridgedQ(ams::Procspec &spec) {
    return !(spec.in == ams::GrpId()) && !(spec.out == ams::GrpId());
}

// TRUE when the channel of slot SLOT has room for EXTRA more bytes: its count is
// below its limit by more than EXTRA.  As on a ring, the last message may take
// the count past the limit, and the one after it is refused.  A NULL SLOT is the
// base channel, whose room is the ring's alone.
inline bool lib_ams::SlotRoomQ(ams::Shmchannel *slot, u32 extra) {
    return !slot || slot->nwrite + extra < slot->wlim;
}

// TRUE when CHANNEL's reader has claimed it and set a window, so a writer may
// count a flow on it.  A reader that never opens the channel leaves it with no
// room for the life of the ring, and a writer that tests this first keeps such a
// reader's flow on the ring itself.
inline bool lib_ams::ChannelClaimedQ(lib_ams::FChannel &channel) {
    return channel.c_slot && channel.c_slot->reader.value != 0 && channel.c_slot->window > 0;
}

// Return the bytes the writer may still write on channel slot CHANNEL: the limit
// its reader set less what the writer has written, and 0 at or past the limit.
inline u64 lib_ams::ChannelRoom(ams::Shmchannel &channel) {
    return algo::u64_SubClip(channel.wlim, channel.nwrite);
}

// Return the bytes the writer may still write on CHANNEL under its limit: the
// slot's room on a keyed channel, and the ring's budget on the base channel.
inline u64 lib_ams::ChannelRoom(lib_ams::FChannel &channel) {
    return channel.c_slot ? ChannelRoom(*channel.c_slot) : GetBudget(*channel.p_shm);
}

// Return the number of member slots in a ring's control header, one per reader
// the ring may carry.  A board has none.
inline u32 lib_ams::RingShmemberN() {
    return 16;
}

// Return the offset of a ring's channel table in its control header: past the
// header and the member slots.  A segment an older build laid out with other
// sizes of these records is refused by its magic, and the asserts tie the sizes
// the magic stands for to the build.
inline u32 lib_ams::RingChannelStart() {
    static_assert(sizeof(ams::Shmember) == 64, "ams.Shmember size is segment layout; bump the ams.Shmhdr magic");
    static_assert(sizeof(ams::Shmchannel) == 48, "ams.Shmchannel size is segment layout; bump the ams.Shmhdr magic");
    return u32(sizeof(ams::Shmhdr) + RingShmemberN() * sizeof(ams::Shmember));
}

// Return the bytes of a ring's control header when its creator declares NCHANNEL
// shm channels: the header, the member slots and the channel table, rounded up
// to whole pages.  One page is the least, and it holds 62 channels, so a ring
// declaring no more than that keeps a one-page header.
inline u32 lib_ams::ShmControlSize(u32 nchannel) {
    u32 need = RingChannelStart() + nchannel * u32(sizeof(ams::Shmchannel));
    return u32_Max(4096, (need + 4095) & ~u32(4095));
}

// Return an allocator that formats a message straight onto CHANNEL: its begin
// is the channel's BeginWrite and its end the channel's EndWrite, so a generated
// Msg_FmtAlloc writes on a channel as Msg_FmtShm writes on a ring, and the
// message counts on the channel.
inline algo::Alloc lib_ams::GetAlloc(lib_ams::FChannel &channel) {
    return algo::Alloc(channel, lib_ams::BeginWrite, lib_ams::EndWrite);
}
