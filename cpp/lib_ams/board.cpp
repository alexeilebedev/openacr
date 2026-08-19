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
// Source: cpp/lib_ams/board.cpp
//
// A lane ring holds whole messages, so it must be sized for the largest message
// it may carry; and because its writable span is its power-of-two body less
// twice that size, a lane costs three to five times the largest message whether
// or not one ever crosses it.  A process therefore keeps one message board: a
// segment of fixed-size slots holding the payloads that do not fit a ring.  The
// payload is written to a slot once and each recipient's ring carries an
// ams::BoardrefMsg in the position the message would have occupied, so a message
// reaching N readers is copied once instead of N times and no ring is sized for
// it at all.
// A slot returns to the free list when the last reader it was posted to has
// consumed the reference.  The sender learns that by watching ring positions:
// every reference is recorded, together with the ring position that releases it,
// in a queue the sender alone owns and readers never see.  A reader writes only
// its own ring position, exactly as it does for an ordinary lane, and maps the
// board without ever polling it.
// Sizing is what keeps one reader's slowness to itself.  A reader's queue holds
// its pin allowance divided by the slot size, so a full queue and a spent
// allowance are one condition; and a board at least as large as the sum of its
// readers' allowances can always place a message, because every reader's pins
// fit at once by construction.  A reader that stops consuming spends its own
// allowance and delays nobody.

#include "include/algo.h"
#include "include/lib_ams.h"

// TRUE when SHM is a message board rather than a lane ring.
bool lib_ams::BoardQ(lib_ams::FShm &shm) {
    return shm.grp_id.grptype == ams_Grptype_board;
}

// Address of slot SLOT within board BOARD.
static u8 *SlotAddr(lib_ams::FShm &board, u32 slot) {
    return board.c_data + u64(slot) * u64(board.max_msg_size);
}

// Offset of slot SLOT from the start of board BOARD's segment -- the form an
// ams::BoardrefMsg carries, so a reader reaches the payload from the mapping
// alone, without knowing how slots are laid out.
static u64 SlotOffset(lib_ams::FShm &board, u32 slot) {
    return board.c_shmhdr->datastart + u64(slot) * u64(board.max_msg_size);
}

// Take a free slot of BOARD into SLOT.  FALSE when none is free, which the
// sizing rule puts out of reach: a reader that has spent its allowance is
// withheld from before a slot is asked for, and the board holds every
// allowance at once.
static bool SlotAllocMaybe(lib_ams::FShm &board, u32 &slot) {
    bool ret = free_slot_N(board) > 0;
    if (ret) {
        slot = *free_slot_Last(board);
        free_slot_RemoveLast(board);
    }
    return ret;
}

// Drop one reader's reference to slot SLOT of BOARD, freeing the slot once the
// last reader has let go.
static void SlotRelease(lib_ams::FShm &board, u32 slot) {
    u32 *nref = slot_nref_Find(board, slot);
    if (nref && *nref > 0) {
        *nref -= 1;
        if (*nref == 0) {
            free_slot_Alloc(board) = slot;
        }
    }
}

// The board serving lane SHM's writer, or NULL when that process keeps none.
// A process has exactly one board, so the lookup is by the writer's proc id and
// the answer is cached on the lane -- a lane opened before the board existed
// finds it on a later call.
lib_ams::FShm *lib_ams::BoardOf(lib_ams::FShm &shm) {
    if (!shm.p_board && !lib_ams::BoardQ(shm)) {
        shm.p_board = lib_ams::ind_shm_Find(ams::GrpId(shm.grp_id.proc_id, ams::Grptype(ams_Grptype_board), 0));
    }
    return shm.p_board;
}

// Stock board BOARD's slot bookkeeping: every slot free and none referenced.
// The slot count follows from the segment, so a reader that maps a board sized
// by someone else agrees with its creator without being told.
void lib_ams::BoardInit(lib_ams::FShm &board) {
    u32 nslot = (board.c_shmhdr->tot_size - board.c_shmhdr->datastart) / board.max_msg_size;
    free_slot_RemoveAll(board);
    slot_nref_RemoveAll(board);
    for (u32 i = 0; i < nslot; i++) {
        slot_nref_Alloc(board) = 0;
        free_slot_Alloc(board) = nslot - 1 - i;
    }
}

// Create this process's message board with NSLOT slots of board_slot_size bytes
// and open it for writing.  NULL if the segment cannot be made.
//
// Choose NSLOT so the board is at least the sum of its readers' pin allowances
// (board_max_pin each): that is what makes a placement unable to fail, and with
// fan-out it is generous, since a slot reaching several readers is charged to
// each allowance but occupies the board once.
lib_ams::FShm *lib_ams::BoardCreate(u32 nslot) {
    lib_ams::FShm *ret = lib_ams::ind_shm_GetOrCreate(ams::GrpId(lib_ams::_db.proc_id, ams::Grptype(ams_Grptype_board), 0));
    if (ret) {
        ret->size = i64(lib_ams::_db.board_slot_size) * nslot + 4096;
        if (lib_ams::ShmOpen(*ret, ams_ShmFlags_write)) {
            lib_ams::BoardInit(*ret);
        } else {
            ret = NULL;
        }
    }
    return ret;
}

// Open process WRITER's message board for reading, so references arriving on
// that writer's lanes can be resolved.  A reader holds the board before the
// first reference arrives.
//
// Holding it costs a mapping and nothing else: the board takes no member slot,
// joins no poll list, and is never written by the reader.  There is nothing for
// a board reader to say -- it advances no position, and the position that
// matters is the one it already keeps on the lane the reference arrived on.
lib_ams::FShm *lib_ams::BoardOpen(ams::ProcId writer) {
    lib_ams::FShm *ret = lib_ams::ind_shm_GetOrCreate(ams::GrpId(writer, ams::Grptype(ams_Grptype_board), 0));
    if (ret && !lib_ams::ShmOpen(*ret, ams_ShmFlags_read)) {
        ret = NULL;
    }
    return ret;
}

// The queue recording what reader MEMBERIDX of lane SHM still holds, created on
// first use.  NULL when the writer keeps no board, or when the queue cannot be
// allocated.
//
// The queue length is the pin allowance divided by the slot size, rounded down
// to a power of two so the position can be masked.  That is also what makes the
// allowance exact rather than approximate: the reader may pin precisely as many
// slots as the queue has entries, so there is one limit to test instead of two
// that disagree at the edges.  An allowance below a single slot would let no
// message through at all and stall the lane silently, so it is reported and
// raised to one slot.
lib_ams::FBoardq *lib_ams::BoardqGetOrCreate(lib_ams::FShm &shm, u32 memberidx) {
    lib_ams::FBoardq *ret = NULL;
    ind_beg(lib_ams::shm_c_boardq_curs, boardq, shm) {
        if (boardq.memberidx == memberidx) {
            ret = &boardq;
        }
    }ind_end;
    lib_ams::FShm *board = ret ? NULL : lib_ams::BoardOf(shm);
    if (board) {
        u64 slot_size = u64(board->max_msg_size);
        u64 nslot_pin = u64(lib_ams::_db.board_max_pin) / slot_size;
        if (nslot_pin == 0) {
            prerr("lib_ams.board_pin_low"
                  <<Keyval("grp",shm.grp_id)
                  <<Keyval("board_max_pin",lib_ams::_db.board_max_pin)
                  <<Keyval("slot_size",slot_size)
                  <<Keyval("comment","allowance below one slot admits no message; raised to one slot"));
            nslot_pin = 1;
        }
        ret = lib_ams::boardq_AllocMaybe();
        if (ret) {
            ret->p_shm = &shm;
            ret->memberidx = memberidx;
            (void)(boardent_AllocN(*ret, i64(u64(1) << algo::u64_BitScanReverse(nslot_pin))).elems);
            (void)lib_ams::boardq_XrefMaybe(*ret);
        }
    }
    return ret;
}

// Release the slots reader ROFF of BOARDQ has consumed: every entry at the head
// whose ring position the reader has passed.  Entries are appended in ring
// order, so the walk stops at the first one still outstanding.
void lib_ams::BoardqSweep(lib_ams::FBoardq &boardq, u64 roff) {
    lib_ams::FShm *board = lib_ams::BoardOf(*boardq.p_shm);
    u64 mask = u64(boardent_N(boardq)) - 1;
    bool more = board != NULL;
    while (more) {
        more = boardq.head < boardq.tail;
        if (more) {
            lib_ams::Boardent &boardent = *boardent_Find(boardq, boardq.head & mask);
            more = boardent.ring_pos <= roff;
            if (more) {
                SlotRelease(*board, boardent.slot);
                boardq.head++;
            }
        }
    }
}

// Bring every reader queue of lane SHM up to date, creating a queue for a reader
// that has none yet.  Run before the lane is asked to take a message, so the
// room test sees what readers have actually consumed rather than what they held
// when the last message went out.
void lib_ams::BoardSweep(lib_ams::FShm &shm) {
    ind_beg(lib_ams::shm_c_shmember_curs, shmember, shm) {
        lib_ams::FBoardq *boardq = lib_ams::BoardqGetOrCreate(shm, ind_curs(shmember).index);
        if (boardq) {
            lib_ams::BoardqSweep(*boardq, shmember.offset);
        }
    }ind_end;
}

// TRUE when every reader of lane SHM can take one more board reference.  Reports
// on the queues as they stand; run BoardSweep first for a current answer.
//
// Every reader must have a queue, not merely one of them.  A slot's reference
// count is how many queues received the reference, so a reader without a queue
// is a reader the count does not know about -- and it still sees the reference
// in the ring and still resolves it, so the slot would be freed and handed to
// the next message while that reader was reading it.  A queue that could not be
// allocated therefore stops board traffic on the lane rather than admitting a
// message whose lifetime nothing tracks.
bool lib_ams::BoardRoomQ(lib_ams::FShm &shm) {
    bool ret = lib_ams::c_boardq_N(shm) > 0 && shm.c_shmhdr && lib_ams::c_boardq_N(shm) == i64(shm.c_shmhdr->n_shmember);
    ind_beg(lib_ams::shm_c_boardq_curs, boardq, shm) {
        ret = ret && boardq.tail - boardq.head < u64(boardent_N(boardq));
    }ind_end;
    return ret;
}

// Record a reference at RING_POS to slot SLOT in the queue of every reader of
// lane SHM, and return how many readers were given it -- which is the slot's
// reference count, the number the sweeps count back down to zero.
static u32 BoardPostEntry(lib_ams::FShm &shm, u64 ring_pos, u32 slot) {
    u32 ret = 0;
    ind_beg(lib_ams::shm_c_boardq_curs, boardq, shm) {
        lib_ams::Boardent &boardent = *boardent_Find(boardq, boardq.tail & (u64(boardent_N(boardq)) - 1));
        boardent.ring_pos = ring_pos;
        boardent.slot = slot;
        boardq.tail++;
        ret++;
    }ind_end;
    return ret;
}

// Add lane SHM to the set the next BoardPostSet reaches.  The set is scratch,
// rebuilt for every message and emptied by the post.
void lib_ams::BoardPostLane(lib_ams::FShm &shm) {
    lib_ams::c_postlane_Insert(shm);
}

// TRUE when every lane collected so far can take a reference: the reader
// allowances are unspent and the ring has room for one.  Asked of all of them
// before a slot is taken, because a message must reach every lane or none --
// a lane that missed one leaves its reader a gap, and a reader consuming a
// stream of references has no way to ask for what it did not get.
static bool PostSetRoomQ() {
    bool ret = lib_ams::c_postlane_N() > 0;
    ind_beg(lib_ams::_db_c_postlane_curs, shm, lib_ams::_db) {
        ret = ret && lib_ams::BoardRoomQ(shm) && lib_ams::HasBudgetQ(shm, ssizeof(ams::BoardrefMsg));
    }ind_end;
    return ret;
}

// Write MSG once to the board and post a reference to it in every lane collected
// with BoardPostLane, so a message reaching readers on N separate lanes is
// copied once rather than N times.  The set is emptied whether or not the post
// succeeds.
//
// FALSE when the set is empty, when MSG is larger than a slot, when any reader
// has spent its pin allowance, or when any ring is full; the caller treats every
// one of those exactly as it treats a full ring.  All-or-nothing is the point of
// asking every lane first: a partial post is a gap in somebody's stream.
//
// The ring position recorded for a reference is the position one past it, which
// is where that reader's own offset lands once it has consumed the reference and
// is done with the payload.  Entries are recorded before the reference is
// published, so the slot is never visible to a reader while unaccounted for, and
// the slot's reference count is the total across every lane -- which is what
// lets one copy serve them all and still be freed at exactly the right moment.
bool lib_ams::BoardPostSet(ams::MsgHeader &msg) {
    bool ret = false;
    lib_ams::FShm *first = lib_ams::c_postlane_N() > 0 ? lib_ams::c_postlane_Find(0) : NULL;
    lib_ams::FShm *board = first ? lib_ams::BoardOf(*first) : NULL;
    if (board && u64(msg.length) <= u64(board->max_msg_size)) {
        ind_beg(lib_ams::_db_c_postlane_curs, shm, lib_ams::_db) {
            lib_ams::BoardSweep(shm);
        }ind_end;
        u32 slot = 0;
        if (PostSetRoomQ() && SlotAllocMaybe(*board, slot)) {
            u32 nref = 0;
            memcpy(SlotAddr(*board, slot), &msg, msg.length);
            ind_beg(lib_ams::_db_c_postlane_curs, shm, lib_ams::_db) {
                ams::BoardrefMsg boardref;
                boardref.offset = SlotOffset(*board, slot);
                boardref.payload_length = msg.length;
                if (void *ptr = lib_ams::BeginWrite(shm, ssizeof(ams::BoardrefMsg))) {
                    memcpy(ptr, &boardref, sizeof(boardref));
                    u64 ring_pos = lib_ams::AddOffset(shm.c_shmhdr->woff, ssizeof(ams::BoardrefMsg));
                    nref += BoardPostEntry(shm, ring_pos, slot);
                    lib_ams::EndWrite(shm, ptr, ssizeof(ams::BoardrefMsg));
                }
            }ind_end;
            *slot_nref_Find(*board, slot) = nref;
            lib_ams::_db.trace.n_board_post++;
            ret = nref > 0;
        }
    }
    lib_ams::c_postlane_RemoveAll();
    return ret;
}

// Write MSG to the board serving lane SHM and post a reference to it in SHM's
// ring, so every reader of the lane receives the message at the cost of one copy.
// The one-lane case of BoardPostSet, which is where the work is described.
bool lib_ams::BoardPost(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    lib_ams::c_postlane_RemoveAll();
    lib_ams::BoardPostLane(shm);
    return lib_ams::BoardPostSet(msg);
}

// The payload BOARDREF names, or NULL when the reference does not describe a
// message this process can see.
//
// The bounds test and the length cross-check are not ceremony: one board slot
// serves every recipient of the message, so a reference that has gone stale --
// through a sender accounting error, or a slot reused before a reader was done
// with it -- would hand the same wrong bytes to every reader at once.  A
// reference that fails either test is refused rather than dispatched.
ams::MsgHeader *lib_ams::BoardResolve(lib_ams::FShm &shm, ams::BoardrefMsg &boardref) {
    ams::MsgHeader *ret = NULL;
    lib_ams::FShm *board = lib_ams::BoardOf(shm);
    if (board && board->c_shmhdr) {
        u64 end = boardref.offset + u64(boardref.payload_length);
        bool inbound = boardref.offset >= board->c_shmhdr->datastart && end <= u64(board->shm_region.n_elems);
        ams::MsgHeader *msg = inbound ? (ams::MsgHeader*)(board->shm_region.elems + boardref.offset) : NULL;
        if (msg && msg->length == boardref.payload_length) {
            ret = msg;
        }
    }
    return ret;
}

// Release every slot reader MEMBERIDX of lane SHM holds, and forget its queue.
// Call when the reader is gone: its ring position stops advancing at the moment
// it dies, so nothing else would ever release what it was holding.
void lib_ams::BoardRelease(lib_ams::FShm &shm, u32 memberidx) {
    lib_ams::FShm *board = lib_ams::BoardOf(shm);
    lib_ams::FBoardq *boardq = NULL;
    ind_beg(lib_ams::shm_c_boardq_curs, cur, shm) {
        if (cur.memberidx == memberidx) {
            boardq = &cur;
        }
    }ind_end;
    if (board && boardq) {
        u64 mask = u64(boardent_N(*boardq)) - 1;
        while (boardq->head < boardq->tail) {
            SlotRelease(*board, boardent_Find(*boardq, boardq->head & mask)->slot);
            boardq->head++;
        }
    }
}
