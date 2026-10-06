// Copyright (C) 2026 AlgoX2 Corp
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
// Source: cpp/lib_ams/channel.cpp
//
// Shm channels: flow control inside one ring, keyed by the users.
// A ring has one writer and many readers, and its write limit, derived from the
// slowest reader's offset, paces the writer as a whole.  A writer that puts
// several flows onto one ring -- one per partition, say, each drained at its own
// rate by one particular reader -- needs a limit per flow.  A shm channel is
// that flow: a count the writer keeps, a count its reader keeps, and a limit the
// reader sets, in a slot of the ring's control page after the member table.
// A channel is the ring's own flow control applied to one flow, so a writer
// writes on a channel as it writes on a ring.  Its handle is an FChannel, and the
// ring as a whole is the base channel, key 0, whose count is woff and whose
// limit is the write limit.  HasBudgetQ, BeginWrite and EndWrite take a channel
// as they take a ring, a message belongs to one channel, and EndWrite counts it.
// A writer may also charge a channel bytes it wrote elsewhere (ChannelCharge),
// when a message commits a second flow or packs several.
// A writer refused on a channel parks as a writer refused on the ring does, and
// the reader that raises the limit wakes it the same way.  The difference is who
// moves the limit.  A ring's moves in passing as its readers read; a channel's
// follows its reader's read count by a window the reader chose, or is granted by
// hand when the reader answers for something other than what it has read.
// Each field of a slot has one process that stores it: the writer stores nwrite,
// the reader stores nread, wlim, window and reader, and the key is claimed
// once by compare-and-swap.  A slot is never freed, so the claimed slots are a
// prefix of the table.

#include "include/algo.h"
#include "include/lib_ams.h"

// Return slot I of the channel table in the control page HDR, or NULL past the
// table's end.  A tool that maps a segment without opening it reads the table
// through this.  A slot whose key is 0 is free.
ams::Shmchannel *lib_ams::HdrChannelFind(ams::Shmhdr &hdr, int i) {
    ams::Shmchannel *ret = NULL;
    if (i >= 0 && u32(i) < hdr.max_channel) {
        ams::Shmchannel *table = (ams::Shmchannel*)((ams::Shmember*)(&hdr + 1) + hdr.max_shmember);
        ret = table + i;
    }
    return ret;
}

// Return slot I of the channel table of SHM, or NULL past the table's end or when
// the segment is not mapped.
ams::Shmchannel *lib_ams::channel_Find(lib_ams::FShm &shm, int i) {
    return shm.c_shmhdr ? HdrChannelFind(*shm.c_shmhdr, i) : NULL;
}

// Return the slot of SHM keyed KEY, claiming the first free one for it when the
// ring holds none yet, and NULL when every slot is taken.  A slot is claimed by
// compare-and-swap on its key, so two processes claiming the same key at once
// end on the same slot, and two claiming different keys end on two.
static ams::Shmchannel *SlotClaim(lib_ams::FShm &shm, u64 key) {
    ams::Shmchannel *ret = NULL;
    int i = 0;
    ams::Shmchannel *slot = lib_ams::channel_Find(shm, i);
    while (slot && !ret) {
        u64 prev = __sync_val_compare_and_swap(&slot->key, u64(0), key);
        if (prev == 0 || prev == key) {
            ret = slot;
        }
        i++;
        slot = lib_ams::channel_Find(shm, i);
    }
    return ret;
}

// Return this process's handle on the channel of ring SHM keyed KEY, or NULL when
// KEY names a channel the ring has no slot for.  Key 0 is the base channel, the
// ring as a whole, which needs no slot.  A keyed channel the ring holds no slot
// for yet is opened here, and the writer and its reader may each open it first,
// in either order.  A new channel has written nothing, read nothing, and has no
// room until its reader sets a window or grants some.  The handle lives as long
// as the ring stays open in this process: ShmClose deletes it with the mapping.
lib_ams::FChannel *lib_ams::ChannelOpen(lib_ams::FShm &shm, u64 key) {
    lib_ams::FChannel *ret = NULL;
    ind_beg(lib_ams::shm_zd_channel_curs, channel, shm) {
        if (channel.key == key) {
            ret = &channel;
            break;
        }
    }ind_end;
    ams::Shmchannel *slot = !ret && key != 0 ? SlotClaim(shm, key) : NULL;
    if (!ret && (slot || key == 0)) {
        lib_ams::FChannel &channel = lib_ams::channel_Alloc();
        channel.p_shm = &shm;
        channel.key = key;
        channel.c_slot = slot;
        lib_ams::channel_XrefMaybe(channel);
        ret = &channel;
    }
    return ret;
}

// Park the writer on CHANNEL waiting for budget for EXTRA more bytes, as
// ParkWriter parks it on a ring, and return true when the ring's budget and the
// channel's room both have it after all.
bool lib_ams::ParkWriter(lib_ams::FChannel &channel, u32 extra DFLTVAL(0)) {
    return ParkWriter(*channel.p_shm, extra, channel.c_slot);
}

// True when a message of EXTRA more bytes may be written on CHANNEL now: the ring
// has budget for it, and a keyed channel has room for it under its limit.
bool lib_ams::HasBudgetQ(lib_ams::FChannel &channel, u32 extra DFLTVAL(0)) {
    return HasBudgetQ(*channel.p_shm, extra) && lib_ams::SlotRoomQ(channel.c_slot, extra);
}

// Begin writing a message of LENGTH bytes on CHANNEL, non-blocking: the write
// pointer, or NULL when the channel's limit or the ring refuses the message now.
// A channel whose limit refuses it parks the writer in signaled mode, so the
// reader's next raise wakes the process, as a ring's reader wakes a writer that
// ran out of ring budget.  The retry is the caller's, as it is on a ring.
void *lib_ams::BeginWrite(lib_ams::FChannel &channel, i32 length) {
    bool room = lib_ams::SlotRoomQ(channel.c_slot, 0);
    if (!room && lib_ams::_db.signaled) {
        room = ParkWriter(channel);
    }
    return room ? BeginWrite(*channel.p_shm, length) : NULL;
}

// Count NBYTE bytes on CHANNEL as written, as its writer, with no message of
// the channel's own.  A writer charges what a message commits the channel's flow
// to when the message itself travels on another channel or carries several
// flows: a publish that obliges an acknowledgment on a second partition, or a
// datagram packing records of several.  Its reader counts the same bytes with
// ChannelRead, so the two counts agree.  The base channel counts nothing here.
void lib_ams::ChannelCharge(lib_ams::FChannel &channel, u64 nbyte) {
    if (channel.c_slot) {
        channel.c_slot->nwrite += nbyte;
    }
}

// Finish the message of LEN bytes begun at PTR on CHANNEL: publish it to the ring
// and count it on the channel.
void lib_ams::EndWrite(lib_ams::FChannel &channel, void *ptr, i32 len) {
    EndWrite(*channel.p_shm, ptr, len);
    ChannelCharge(channel, u64(len));
}

// Make this process the reader of channel slot SLOT when the slot has none yet,
// and return true when this process is its reader.  A process that already is
// the reader costs one load; the claim itself is a compare-and-swap, as the
// key's is, so two processes racing to be first end with one reader, and the
// other is refused.
static bool ClaimReader(ams::Shmchannel &slot) {
    u32 me = lib_ams::_db.proc_id.value;
    bool ret = slot.reader.value == me;
    if (!ret && slot.reader.value == 0) {
        u32 prev = __sync_val_compare_and_swap(&slot.reader.value, u32(0), me);
        ret = prev == 0 || prev == me;
    }
    return ret;
}

// Raise the limit of CHANNEL to WLIM, as the reader the caller has claimed to
// be, and return true when it rose; one that would lower the limit changes
// nothing.  In signaled mode a raise owes a parked writer a wakeup once the room
// it leaves is at least half the channel's window, or at once on a channel
// granted by hand, which is WakeWriter's hysteresis for a channel; the end of
// the pass sends it.
static bool ChannelRaise(lib_ams::FChannel &channel, u64 wlim) {
    ams::Shmchannel &slot = *channel.c_slot;
    bool ret = wlim > slot.wlim;
    if (ret) {
        sfence();
        slot.wlim = wlim;
        if (lib_ams::_db.signaled && lib_ams::ChannelRoom(slot) >= slot.window / 2) {
            lib_ams::WakeWriter(*channel.p_shm);
        }
    }
    return ret;
}

// Count NBYTE bytes this process read on CHANNEL, as the channel's reader.  On a
// channel with a window the limit follows the count in passing, as a ring's
// write limit follows its readers' offsets: it becomes the count plus the window.
// A process that is not the channel's reader counts nothing.
void lib_ams::ChannelRead(lib_ams::FChannel &channel, u64 nbyte) {
    ams::Shmchannel *slot = channel.c_slot;
    if (slot && ClaimReader(*slot)) {
        slot->nread += nbyte;
        if (slot->window > 0) {
            ChannelRaise(channel, slot->nread + slot->window);
        }
    }
}

// Count NBYTE bytes this process read on the channel of ring SHM keyed KEY, as
// the channel's reader, for a message that names the channel it was charged to.
// KEY 0 is the ring itself, which counts nothing here.  A reader counts by the
// key the message carries, so it counts every charged message, including one
// whose request it no longer holds.
void lib_ams::ChannelReadKey(lib_ams::FShm &shm, u64 key, u64 nbyte) {
    lib_ams::FChannel *channel = key != 0 ? ChannelOpen(shm, key) : NULL;
    if (channel) {
        ChannelRead(*channel, nbyte);
    }
}

// Make CHANNEL's limit follow its read count by WINDOW bytes, as its reader,
// raise the limit to that at once, and return true when the window was set.  A
// reader whose room is its own read position sets a window once and then only
// reads; WINDOW 0 leaves the limit to grants.
//
// A window no larger than the ring's largest message would refuse that message
// forever: with everything read, the room is exactly the window, and a writer
// asking for more than that waits for a read that has nothing left to read.  So
// such a window is refused, as is one from a process that is not the channel's
// reader, and the channel keeps the window it had.
bool lib_ams::ChannelSetWindow(lib_ams::FChannel &channel, u64 window) {
    ams::Shmchannel *slot = channel.c_slot;
    bool ret = false;
    if (slot && (window == 0 || window > u64(channel.p_shm->max_msg_size))) {
        ret = ClaimReader(*slot);
    }
    if (ret) {
        slot->window = window;
        ChannelRaise(channel, slot->nread + window);
    }
    return ret;
}

// Raise CHANNEL's limit to WLIM by hand, as its reader, and return true when
// it rose: for a reader whose room is set by something other than what it has
// read, such as records it holds until others release them.
bool lib_ams::ChannelGrant(lib_ams::FChannel &channel, u64 wlim) {
    bool ret = false;
    if (channel.c_slot && ClaimReader(*channel.c_slot)) {
        ret = ChannelRaise(channel, wlim);
    }
    return ret;
}

// Step the channel cursor CURS to the next slot.
void lib_ams::shm_c_channel_curs_Next(shm_c_channel_curs &curs) {
    curs.index++;
}

// Start the channel cursor CURS at the first slot of the table of ring PARENT.
void lib_ams::shm_c_channel_curs_Reset(shm_c_channel_curs &curs, lib_ams::FShm &parent) {
    curs.shm = &parent;
    curs.index = 0;
}

// True while CURS stands on a claimed slot: the claimed slots are a prefix of the
// table, so the walk ends at the first free slot or at the table's end.
bool lib_ams::shm_c_channel_curs_ValidQ(shm_c_channel_curs &curs) {
    ams::Shmchannel *slot = channel_Find(*curs.shm, curs.index);
    return slot && slot->key != 0;
}

// Return the slot CURS stands on.
ams::Shmchannel& lib_ams::shm_c_channel_curs_Access(shm_c_channel_curs &curs) {
    return *channel_Find(*curs.shm, curs.index);
}
