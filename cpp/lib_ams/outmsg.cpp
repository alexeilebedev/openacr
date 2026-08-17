// Copyright (C) 2026 AlgoRND
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
// Source: cpp/lib_ams/outmsg.cpp
//
// The queued write: a message that must reach its ring even when the ring is
// momentarily full.
// A caller that formats straight into a ring discovers, at the moment it wants
// to send, whether there is room -- and those are two different times.  A
// command's answer is built when the command finishes, and the control lane
// has room whenever its reader last drained it; nothing ties one to the other.
// The plain write resolves that by dropping the message and reporting nothing,
// so the requester waits out a timeout for output that was thrown away.
// The two times are bridged by a record.  A message that finds no room is
// formatted into an lib_ams.FOutmsg instead, queued on the ring it was meant
// for, and written by zd_outshm_Step as budget appears.  The ring keeps its
// own queue rather than one global list, so a blocked ring delays nothing but
// itself, and a ring that already holds a queued message takes every later one
// through the queue as well -- which is what keeps a message from overtaking
// the one before it.
// Everything here is reached through GetAllocQueue, so a caller writes
// `Msg_FmtAlloc(lib_ams::GetAllocQueue(shm), ...)` and is done: the fast path
// still formats in place in the ring, and the slow path costs one copy.

#include "include/algo.h"
#include "include/lib_ams.h"

// Move SHM's queued messages into the ring, oldest first, stopping at the
// first one the ring has no room for.  Stopping rather than skipping is what
// preserves the order the caller wrote them in.
static void OutmsgFlush(lib_ams::FShm &shm) {
    bool room = true;
    while (room) {
        lib_ams::FOutmsg *outmsg = lib_ams::zd_outmsg_First(shm);
        if (!outmsg) {
            room = false;
        } else {
            algo::aryptr<u8> bytes = ary_Getary(outmsg->data);
            int len = int(elems_N(bytes));
            void *dst = lib_ams::BeginWrite(shm, len);
            if (!dst) {
                room = false;
            } else {
                memcpy(dst, bytes.elems, size_t(len));
                lib_ams::EndWrite(shm, dst, len);
                lib_ams::outmsg_Delete(*outmsg);
            }
        }
    }
}

// Write what the queued rings will take, and keep the rest for the next pass.
// A ring that empties leaves the list; one that is still blocked goes to the
// back of it, so no ring can starve another.  The list length is sampled at
// entry, so a ring rotated to the back is not visited twice in one pass.
void lib_ams::zd_outshm_Step() {
    int todo = lib_ams::zd_outshm_N();
    while (todo > 0) {
        todo--;
        lib_ams::FShm *shm = lib_ams::zd_outshm_RemoveFirst();
        if (shm) {
            OutmsgFlush(*shm);
            if (lib_ams::zd_outmsg_N(*shm) > 0) {
                lib_ams::zd_outshm_Insert(*shm);
            }
        }
    }
}

// Reserve LEN bytes for a message on SHM and return where to build it, taking
// the ring itself when it has room and a queued record when it does not.  The
// answer is never NULL for a length the ring could ever carry, which is what
// lets a caller format without testing.  EndWriteQueue must follow, and the
// two communicate through _db.c_cur_outmsg, so a format call may not begin
// another before it ends.
void *lib_ams::BeginWriteQueue(lib_ams::FShm &shm, int length) {
    void *ret = NULL;
    lib_ams::_db.c_cur_outmsg = NULL;
    // a ring with a queue takes this message through the queue too, or this
    // one would overtake the messages already waiting
    if (lib_ams::zd_outmsg_N(shm) == 0) {
        ret = lib_ams::BeginWrite(shm, length);
    }
    if (!ret) {
        lib_ams::FOutmsg &outmsg = lib_ams::outmsg_Alloc();
        outmsg.p_shm = &shm;
        ret = ary_AllocN(outmsg.data, length).elems;
        lib_ams::_db.c_cur_outmsg = &outmsg;
    }
    return ret;
}

// Finish the message BeginWriteQueue started: publish it to the ring when it
// was built there, and otherwise put it at the back of the ring's queue and
// arm the step that will write it.
void lib_ams::EndWriteQueue(lib_ams::FShm &shm, void *ptr, int len) {
    lib_ams::FOutmsg *outmsg = lib_ams::_db.c_cur_outmsg;
    if (outmsg) {
        lib_ams::_db.c_cur_outmsg = NULL;
        lib_ams::zd_outmsg_Insert(shm, *outmsg);
        lib_ams::zd_outshm_Insert(shm);
    } else {
        lib_ams::EndWrite(shm, ptr, len);
    }
}
