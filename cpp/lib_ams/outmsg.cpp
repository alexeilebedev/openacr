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
// preserves the order the caller wrote them in.  BeginWrite calls this before
// any reservation, so a direct writer never overtakes the queue; the
// reservation here is therefore the ring's own, ReserveWrite.
void lib_ams::OutmsgFlush(lib_ams::FShm &shm) {
    bool room = true;
    while (room) {
        lib_ams::FOutmsg *outmsg = lib_ams::zd_outmsg_First(shm);
        if (!outmsg) {
            room = false;
        } else {
            algo::aryptr<u8> bytes = ary_Getary(outmsg->data);
            int len = int(elems_N(bytes));
            void *dst = lib_ams::ReserveWrite(shm, len);
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

// Reserve LENGTH bytes for a message on SHM and return where to build it,
// taking the ring itself when it has room and a record when it does not.  The
// answer is never NULL, which is what lets a caller format without testing.
// EndWriteQueue must follow, and the two communicate through
// _db.c_cur_outmsg, so a format call may not begin another before it ends.
//
// A message longer than the ring's largest is always built in a record,
// because the ring can never take it: queued, it would stand at the head of
// the ring's queue forever, and every message written after it would wait
// behind it.  EndWriteQueue sends such a message by the board instead.
void *lib_ams::BeginWriteQueue(lib_ams::FShm &shm, int length) {
    // BeginWrite refuses a ring that still holds a queue, so a message behind
    // one goes through the queue too and never overtakes what is waiting
    void *ret = length > shm.max_msg_size ? NULL : lib_ams::BeginWrite(shm, length);
    lib_ams::_db.c_cur_outmsg = NULL;
    if (!ret) {
        lib_ams::FOutmsg &outmsg = lib_ams::outmsg_Alloc();
        outmsg.p_shm = &shm;
        ret = ary_AllocN(outmsg.data, length).elems;
        lib_ams::_db.c_cur_outmsg = &outmsg;
    }
    return ret;
}

// Finish the message BeginWriteQueue started at PTR, LEN bytes long: publish
// it to SHM when it was built there, send it by the board when it is longer
// than the ring's largest message, and otherwise put it at the back of the
// ring's queue and arm the step that will write it.  Return false when the
// message is oversize and the board cannot take it now, in which case it is
// dropped and counted in n_outmsg_oversize_drop.
bool lib_ams::EndWriteQueue(lib_ams::FShm &shm, void *ptr, int len) {
    bool ret = true;
    lib_ams::FOutmsg *outmsg = lib_ams::_db.c_cur_outmsg;
    lib_ams::_db.c_cur_outmsg = NULL;
    if (outmsg && len > shm.max_msg_size) {
        ret = lib_ams::WriteMsg(shm, *(ams::MsgHeader*)outmsg->data.ary_elems);
        if (!ret) {
            lib_ams::_db.trace.n_outmsg_oversize_drop++;
        }
        lib_ams::outmsg_Delete(*outmsg);
    } else if (outmsg) {
        lib_ams::zd_outmsg_Insert(shm, *outmsg);
        lib_ams::zd_outshm_Insert(shm);
    } else {
        lib_ams::EndWrite(shm, ptr, len);
    }
    return ret;
}

// Write MSG to SHM, queued: into the ring when it has room and nothing waits
// ahead of it, otherwise onto the ring's queue, which zd_outshm writes as
// budget appears.  The message WriteMsg would drop is the one this keeps, so
// this is the write for a message with no retry of its own behind it -- a
// command, its answer -- where a drop is a requester waiting out its whole
// deadline for output that was thrown away.  Return false when MSG is
// oversize and the board cannot take it now, which EndWriteQueue counts.
bool lib_ams::WriteMsgQueue(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    int len = msg.length;
    void *ptr = lib_ams::BeginWriteQueue(shm, len);
    memcpy(ptr, &msg, size_t(len));
    return lib_ams::EndWriteQueue(shm, ptr, len);
}
