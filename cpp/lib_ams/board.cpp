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
// Source: cpp/lib_ams/board.cpp
//
// A lane ring holds whole messages, so a message that reaches N readers on N
// lanes is copied N times, and every lane is sized for the largest message it may
// carry.  The message board takes the copies out.  A writer keeps one board, a
// segment under the `board` grptype whose body is an arena of fixed-size chunks.
// It reserves message space in the current chunk with BoardAlloc, formats the
// message there, and sends it to a lane with BoardSend, which writes a 64-byte
// ams::BoardrefMsg naming the payload's board offset and length into the lane
// ring.  The reader's poll step resolves the reference to the payload and hands
// the payload to the lane's hook, so nothing above the transport can tell which
// way a message traveled.  A reader maps the board with BoardOpen and writes
// nothing into it: its lane offset is the one number the writer already reads.
// The chunk is the unit of reuse, and the writer alone keeps the books, in its
// own memory.  Each FChunk carries `nref`, the count of references sent into it
// that no reader has yet passed, plus any holds a module takes with ChunkHold.
// Per lane the writer keeps up to board_max_chunkref Chunkref rows, one per
// chunk the lane's readers may still be reading: the chunk, how many references
// into it went down the lane, and the lane offset past the last of them.  A send
// to a chunk the lane already references adds to that row, so a run of messages
// out of one chunk costs one row however long it is.  BoardReap reads the lane's
// slowest reader offset and, for every row it has passed, subtracts the row's
// count from the chunk and drops the row; a chunk that reaches zero and is not
// the one being filled returns to the board's free list.  A send that needs a
// row the lane has no room for reaps first and then refuses, so a lane
// references at most board_max_chunkref chunks at once and a reader that stops
// costs the board that many chunks, never its ability to reuse the rest.  That
// is what a ring cannot offer: space here is reclaimed by chunk, in whatever
// order the readers let go of it.

#include "include/algo.h"
#include "include/lib_ams.h"
#include <sys/mman.h>

// The board of lane SHM's writer, or NULL when that process keeps none.
// A process keeps one board, so the lookup is by the writer's proc id, and the
// answer is cached on the lane so a lane opened before the board existed finds
// it on a later call.
//
// A reader learns of a board by the first reference that reaches it, so on a
// lane this process reads and does not write the board is opened here when it
// is not mapped yet.  The open fails while the writer has not created its board,
// which costs one shm_open per reference until it has; a writer sends no
// reference before its board exists, so in practice the first one succeeds.
lib_ams::FShm *lib_ams::BoardOf(lib_ams::FShm &shm) {
    if (!shm.p_board && !lib_ams::BoardQ(shm)) {
        shm.p_board = lib_ams::ind_shm_Find(ams::GrpId(shm.grp_id.proc_id, ams::Grptype(ams_Grptype_board), 0));
    }
    bool mapped = shm.p_board && shm.p_board->c_shmhdr;
    if (!mapped && read_Get(shm.flags) && !write_Get(shm.flags) && !lib_ams::BoardQ(shm)) {
        shm.p_board = lib_ams::BoardOpen(shm.grp_id.proc_id);
    }
    return shm.p_board;
}

// The chunk of BOARD whose bytes include address PTR, or NULL when PTR lies
// outside the board's body.
lib_ams::FChunk *lib_ams::ChunkOf(lib_ams::FShm &board, const void *ptr) {
    lib_ams::FChunk *ret = NULL;
    i64 off = (const u8*)ptr - board.shm_region.elems;
    if (board.c_shmhdr && board.chunk_size > 0 && off >= i64(board.c_shmhdr->datastart)) {
        ret = lib_ams::c_chunk_Find(board, u64(off - board.c_shmhdr->datastart) / board.chunk_size);
    }
    return ret;
}

// Put CHUNK on its board's free list once nothing references it and the board
// is not filling it.  The one site that frees a chunk, reached from every path
// that lowers `nref` and from the retirement of the chunk being filled.
static void ChunkFreeMaybe(lib_ams::FChunk &chunk) {
    if (chunk.nref == 0 && chunk.p_board->c_chunk_cur != &chunk) {
        lib_ams::zd_chunk_free_Insert(*chunk.p_board, chunk);
    }
}

// Release N references to CHUNK.
static void ChunkUnref(lib_ams::FChunk &chunk, u32 n) {
    chunk.nref -= u32_Min(n, chunk.nref);
    ChunkFreeMaybe(chunk);
}

// Give back one hold ChunkHold took on CHUNK.  A chunk nobody holds or
// references any longer goes back to the board's free list.
void lib_ams::ChunkRelease(lib_ams::FChunk &chunk) {
    ChunkUnref(chunk, 1);
}

// Describe BOARD's mapped body as chunks of its chunk size, every one free, and
// make it a board this process fills.  The count follows from the segment, so
// a board that came from the process pool at the default size, or one the
// topology sized, is described as it is rather than as it was asked for.  A
// process may fill several boards, and the chunks of each join the process's
// chunk pool after those of the boards described before it; a board already
// described is left as it is.
void lib_ams::BoardInitChunks(lib_ams::FShm &board) {
    board.c_chunk_cur = NULL;
    u64 nchunk = lib_ams::c_chunk_N(board) == 0 ? (u64(board.shm_region.n_elems) - board.c_shmhdr->datastart) / board.chunk_size : 0;
    for (u64 i = 0; i < nchunk; i++) {
        lib_ams::FChunk *chunk = lib_ams::chunk_AllocMaybe();
        if (chunk) {
            chunk->p_board = &board;
            chunk->offset = board.c_shmhdr->datastart + i * board.chunk_size;
            (void)lib_ams::chunk_XrefMaybe(*chunk);
            lib_ams::zd_chunk_free_Insert(board, *chunk);
        }
    }
}

// Create this process's message board, a segment whose body holds at least BODY
// bytes as chunks of CHUNK_SIZE bytes, and open it for writing.  NULL if the
// segment cannot be made or the tmpfs cannot hold it.
//
// A chunk is the largest message the board carries and the unit a reader's
// lag is charged in: a lane references at most board_max_chunkref chunks, so a
// reader that stops holds that many chunks and the rest of the board keeps
// turning over.  Size the body for the burst every reader together may be
// behind by, plus the chunks the stopped readers you are willing to carry hold.
//
// The segment is created as any board segment is (ShmCreate): its pages are
// committed and locked when it is made, so the tmpfs answers here, where the
// caller can fall back, and never with a fault under a store minutes into the
// run.  A process in a topology does not call this: its board is a segment the
// topology declares and the supervisor creates, which it opens with the rest.
lib_ams::FShm *lib_ams::BoardCreate(u64 body, u32 chunk_size) {
    lib_ams::FShm *ret = lib_ams::ind_shm_GetOrCreate(ams::GrpId(lib_ams::_db.proc_id, ams::Grptype(ams_Grptype_board), 0));
    if (ret) {
        ret->max_msg_size = i32(chunk_size);
        ret->size = i64(lib_ams::BoardSize(body, chunk_size));
        if (lib_ams::ShmOpen(*ret, ams_ShmFlags_write)) {
            lib_ams::BoardInitChunks(*ret);
        } else {
            ret = NULL;
        }
    }
    return ret;
}

// Bytes a board segment takes to hold a body of BODY bytes as chunks of
// CHUNK_SIZE: the header page and whole chunks, at least one.
u64 lib_ams::BoardSize(u64 body, u32 chunk_size) {
    u64 nchunk = u64_Max((body + chunk_size - 1) / chunk_size, 1);
    return 4096 + nchunk * chunk_size;
}

// Create a message board of this process as a private mapping of this process
// alone -- BODY bytes as chunks of CHUNK_SIZE, anonymous memory, no file under
// /dev/shm -- and open it for writing.  INDEX is the board's index among this
// process's boards: index 0 is the one readers open (BoardOpen), so a private
// board kept beside a shared one takes another index.  NULL when the memory
// cannot be mapped.
//
// This is the board for memory no reader needs to open: the process keeps the
// chunk arena, the allocation and the eviction it was written against, and
// gives up only what a segment would buy it -- no other process can open the
// board, so a reference sent into it would resolve to nothing.  The board says
// so on `privateq`, and a writer tests that before sending a reference.
lib_ams::FShm *lib_ams::BoardCreatePrivate(u64 body, u32 chunk_size, u32 index DFLTVAL(0)) {
    lib_ams::FShm *ret = lib_ams::ind_shm_GetOrCreate(ams::GrpId(lib_ams::_db.proc_id, ams::Grptype(ams_Grptype_board), u8(index)));
    i64 size = i64(lib_ams::BoardSize(body, chunk_size));
    void *mem = ret && !ret->c_shmhdr ? mmap(NULL, size_t(size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) : MAP_FAILED;
    if (mem == MAP_FAILED) {
        ret = NULL;
    } else {
        ret->shm_region = algo::memptr((u8*)mem, size);
        ret->created = true;
        ret->privateq = true;
        ret->chunk_size = chunk_size;
        ret->max_msg_size = i32(chunk_size);
        ret->size = size;
        ret->offset_mask = 0;
        ret->c_shmhdr = (ams::Shmhdr*)mem;
        new (ret->c_shmhdr) ams::Shmhdr;// defaults
        ret->c_shmhdr->grp_id = ret->grp_id;
        ret->c_shmhdr->tot_size = u64(size);
        ret->c_shmhdr->datastart = 4096;
        ret->c_shmhdr->max_msg_size = ret->max_msg_size;
        ret->c_shmhdr->creator_pid = getpid();
        ret->c_shmhdr->writer_pid = getpid();
        ret->c_data = ret->shm_region.elems + 4096;
        write_Set(ret->flags, true);
        lib_ams::BoardInitChunks(*ret);
    }
    return ret;
}

// Open process WRITER's message board for reading, so references arriving on
// that writer's lanes resolve; open it before the first reference arrives.
// NULL when the segment is missing.
//
// Holding it costs a mapping and nothing else: the board takes no member slot,
// joins no poll list, and is never written by the reader.  The position that
// matters is the one the reader already keeps on the lane the reference arrived
// on, and the writer reads it there.
lib_ams::FShm *lib_ams::BoardOpen(ams::ProcId writer) {
    lib_ams::FShm *ret = lib_ams::ind_shm_GetOrCreate(ams::GrpId(writer, ams::Grptype(ams_Grptype_board), 0));
    if (ret && !lib_ams::ShmOpen(*ret, ams_ShmFlags_read)) {
        ret = NULL;
    }
    return ret;
}

// Offset of lane SHM's slowest reader: the least consume offset over its
// members, and the ring start when it has none.  A message at or below this
// offset has been read by everyone the lane delivers to.
u64 lib_ams::SlowestReaderOffset(lib_ams::FShm &shm) {
    u64 ret = shm.c_shmhdr->n_shmember ? (ULLONG_MAX/2) : 0;
    ind_beg(lib_ams::shm_c_shmember_curs,shmember,shm) {
        u64_UpdateMin(ret,shmember.offset);
    }ind_end;
    return ret;
}

// Release what lane SHM's readers have finished with: every Chunkref whose
// offset the slowest reader has reached gives its count back to its chunk and
// leaves the table.  Cheap enough to call before every send, and BoardSend
// calls it itself when it needs a row the table has no room for.
void lib_ams::BoardReap(lib_ams::FShm &shm) {
    if (shm.c_shmhdr) {
        u64 roff = lib_ams::SlowestReaderOffset(shm);
        for (i64 i = lib_ams::chunkref_N(shm) - 1; i >= 0; i--) {
            lib_ams::Chunkref &chunkref = *lib_ams::chunkref_Find(shm, u64(i));
            if (chunkref.woffset <= roff) {
                ChunkUnref(*chunkref.p_chunk, chunkref.nmsg);
                lib_ams::chunkref_Remove(shm, u64(i));
            }
        }
    }
}

// Release every reference lane SHM holds on any chunk, and forget them.  Call
// when the lane closes: its readers' offsets stop moving, so nothing else would
// give those chunks back.
void lib_ams::ChunkrefReleaseAll(lib_ams::FShm &shm) {
    ind_beg(lib_ams::shm_chunkref_curs, chunkref, shm) {
        ChunkUnref(*chunkref.p_chunk, chunkref.nmsg);
    }ind_end;
    lib_ams::chunkref_RemoveAll(shm);
}

// Forget BOARD's lanes and every lane's references into it, and the chunks of
// every board this process fills.  Call when the board closes; the chunks name
// bytes of a mapping that is going away.  The chunk pool is one pool across the
// process's boards, so a process that fills several closes them together, as it
// does when it exits.
void lib_ams::BoardReset(lib_ams::FShm &board) {
    ind_beg(lib_ams::shm_c_lane_curs, lane, board) {
        lib_ams::chunkref_RemoveAll(lane);
    }ind_end;
    lib_ams::c_lane_RemoveAll(board);
    board.c_chunk_cur = NULL;
    lib_ams::chunk_RemoveAll();
}

// Reap every lane that references a chunk of BOARD, so each chunk whose last
// reference its reader has passed returns to the free list.  A board's owner
// that judges its chunks by how many are free calls this first: a lane is
// otherwise reaped only when something is sent on it or the free list runs dry,
// and a chunk a quiet lane's reader passed long ago still reads as held.
void lib_ams::BoardReapAll(lib_ams::FShm &board) {
    ind_beg(lib_ams::shm_c_lane_curs, lane, board) {
        lib_ams::BoardReap(lane);
    }ind_end;
}

// Stop filling CHUNK, the chunk BOARD is allocating from.  A chunk nothing
// referenced while it was being filled is free at once.
static void ChunkRetire(lib_ams::FShm &board, lib_ams::FChunk &chunk) {
    board.c_chunk_cur = NULL;
    ChunkFreeMaybe(chunk);
}

// Take a free chunk of BOARD to fill, off the free list and emptied, or NULL
// when none is free -- after reaping every lane that references the board, since
// a reader may have moved past the last reference into some chunk.  The caller
// holds the chunk by filling it; when it has moved on, ChunkRelease with no
// references outstanding is what frees it.  A module that fills several chunks
// at once -- one per class of message -- takes them here and reserves with
// ChunkAlloc; BoardAlloc is the one-chunk form over the two.
lib_ams::FChunk *lib_ams::ChunkTake(lib_ams::FShm &board) {
    lib_ams::FChunk *ret = board.c_shmhdr ? lib_ams::zd_chunk_free_First(board) : NULL;
    if (!ret && board.c_shmhdr) {
        lib_ams::BoardReapAll(board);
        ret = lib_ams::zd_chunk_free_First(board);
    }
    if (ret) {
        lib_ams::zd_chunk_free_Remove(board, *ret);
        ret->used = 0;
    } else {
        lib_ams::_db.trace.n_board_nochunk++;
    }
    return ret;
}

// Reserve LEN bytes at the end of CHUNK and return where to write them, or NULL
// when the chunk has no room for them.  Nothing is published by the reservation.
// A reservation is exactly LEN bytes, so messages lie in a chunk back to back the
// way they lie in a datagram, and a datagram read into a chunk is a run of
// messages the board can reference without moving a byte.
void *lib_ams::ChunkAlloc(lib_ams::FChunk &chunk, int len) {
    void *ret = NULL;
    if (chunk.used + u32(len) <= chunk.p_board->chunk_size) {
        ret = lib_ams::ChunkAddr(chunk) + chunk.used;
        chunk.used += u32(len);
    }
    return ret;
}

// Reserve LEN bytes of message space in BOARD and return where to write them,
// or NULL when no chunk can take them.  The space is the next run of the chunk
// being filled; a message that does not fit there retires that chunk and starts
// a free one (ChunkTake).  A message longer than a chunk is refused outright.
//
// Nothing is published by the reservation.  The caller formats the message in
// place and then sends it with BoardSend, or gives the space back with BoardTrim.
void *lib_ams::BoardAlloc(lib_ams::FShm &board, int len) {
    void *ret = NULL;
    lib_ams::FChunk *chunk = board.c_chunk_cur;
    if (chunk && chunk->used + u32(len) > board.chunk_size) {
        ChunkRetire(board, *chunk);
        chunk = NULL;
    }
    if (!chunk && u32(len) <= board.chunk_size) {
        chunk = lib_ams::ChunkTake(board);
        board.c_chunk_cur = chunk;
    }
    if (chunk) {
        ret = lib_ams::ChunkAlloc(*chunk, len);
    }
    return ret;
}

// Shorten the most recent reservation of BOARD, at PTR, to LEN bytes: ChunkTrim
// on the chunk being filled, when PTR lies in it.
void lib_ams::BoardTrim(lib_ams::FShm &board, void *ptr, int len) {
    lib_ams::FChunk *chunk = board.c_chunk_cur;
    if (chunk && chunk == lib_ams::ChunkOf(board, ptr)) {
        lib_ams::ChunkTrim(*chunk, ptr, len);
    }
}

// The Chunkref of lane SHM for CHUNK, or NULL when the lane holds none.  The
// last row is tested first, because a writer sends runs of messages out of one
// chunk and the row it wants is almost always the one it used last.
static lib_ams::Chunkref *ChunkrefFind(lib_ams::FShm &shm, lib_ams::FChunk &chunk) {
    lib_ams::Chunkref *ret = lib_ams::chunkref_Last(shm);
    if (ret && ret->p_chunk != &chunk) {
        ret = NULL;
        ind_beg(lib_ams::shm_chunkref_curs, chunkref, shm) {
            if (chunkref.p_chunk == &chunk) {
                ret = &chunkref;
            }
        }ind_end;
    }
    return ret;
}

// TRUE when lane SHM can take a reference to a message in CHUNK as things
// stand: it holds a Chunkref row for CHUNK or has room for one, and the ring
// has room for the reference with nothing queued ahead of it.
bool lib_ams::BoardRoomQ(lib_ams::FShm &shm, lib_ams::FChunk &chunk) {
    bool ret = ChunkrefFind(shm, chunk) != NULL || lib_ams::chunkref_N(shm) < lib_ams::_db.board_max_chunkref;
    return ret && lib_ams::zd_outmsg_N(shm) == 0 && lib_ams::HasBudgetQ(shm, ssizeof(ams::BoardrefMsg));
}

// Make room on lane SHM for a reference to a message in CHUNK, and say whether
// there is any: BoardRoomQ as things stand, and after one reap when there is
// not.  The reap runs only when it is needed, so a run of messages out of one
// chunk pays for none.  Ask this of every lane a message goes to before sending
// it to any of them: a reader that missed one message of a numbered stream has
// a gap it cannot ask to have filled.
bool lib_ams::BoardMakeRoom(lib_ams::FShm &shm, lib_ams::FChunk &chunk) {
    bool ret = lib_ams::BoardRoomQ(shm, chunk);
    if (!ret) {
        lib_ams::BoardReap(shm);
        ret = lib_ams::BoardRoomQ(shm, chunk);
    }
    return ret;
}

// Begin sending MSG, a message in a chunk of this process's board, to lane SHM
// by a reference message of REFLEN bytes: the ring slot the caller formats the
// reference into, or NULL when the send cannot happen now -- MSG is not in the
// board, the board is a private mapping no reader can open, the lane references
// board_max_chunkref chunks already and reaping frees none of them
// (n_board_nochunkref), or the ring has no room.  The caller writes
// its reference into the slot and finishes with BoardEndSend; the pair exists so
// a module may carry a reference inside a message of its own, with the fields
// its reader needs beside the coordinates.
void *lib_ams::BoardBeginSend(lib_ams::FShm &shm, ams::MsgHeader &msg, int reflen) {
    void *ret = NULL;
    lib_ams::FShm *board = lib_ams::BoardOf(shm);
    lib_ams::FChunk *chunk = board && !board->privateq ? lib_ams::ChunkOf(*board, &msg) : NULL;
    bool room = chunk && lib_ams::BoardMakeRoom(shm, *chunk);
    if (room) {
        ret = lib_ams::BeginWrite(shm, reflen);
    } else if (chunk) {
        lib_ams::_db.trace.n_board_nochunkref++;
    }
    return ret;
}

// Publish the reference that BoardBeginSend reserved PTR for on lane SHM,
// REFLEN bytes long, and count it against CHUNK, the chunk the referenced
// message lies in.
//
// The chunk is charged after the reference is published and before anything
// else runs, so no reader sees a reference to a chunk the writer thinks free.
// A reader is done with the payload when its lane offset passes the reference,
// which is what the row's `woffset` records.
void lib_ams::BoardEndSend(lib_ams::FShm &shm, lib_ams::FChunk &chunk, void *ptr, int reflen) {
    lib_ams::FShm &board = *chunk.p_board;
    lib_ams::EndWrite(shm, ptr, reflen);
    lib_ams::Chunkref *chunkref = ChunkrefFind(shm, chunk);
    if (!chunkref) {
        chunkref = &lib_ams::chunkref_Alloc(shm);
        chunkref->p_chunk = &chunk;
        // the lane joins the set the board reaps when it runs out of chunks
        lib_ams::c_lane_InsertMaybe(board, shm);
    }
    chunkref->nmsg++;
    chunkref->woffset = shm.c_shmhdr->woff;
    chunk.nref++;
    lib_ams::_db.trace.n_board_send++;
}

// Send MSG, a message formatted into a chunk of this process's board, to lane
// SHM: write an ams::BoardrefMsg naming it into the ring and count it against
// its chunk.  TRUE when the reference was published.  FALSE when MSG is not in
// the board, when the lane references board_max_chunkref chunks already and
// reaping frees none of them, or when the ring has no room; the message stays in
// the board and the caller retries or gives its space back with BoardTrim.
bool lib_ams::BoardSend(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    bool ret = false;
    if (void *ptr = lib_ams::BoardBeginSend(shm, msg, ssizeof(ams::BoardrefMsg))) {
        lib_ams::FChunk &chunk = *lib_ams::ChunkOf(*lib_ams::BoardOf(shm), &msg);
        ams::BoardrefMsg boardref;
        boardref.offset = lib_ams::BoardOffset(chunk, msg);
        boardref.payload_length = msg.length;
        memcpy(ptr, &boardref, sizeof(boardref));
        lib_ams::BoardEndSend(shm, chunk, ptr, ssizeof(ams::BoardrefMsg));
        ret = true;
    }
    return ret;
}

// The LENGTH bytes at board OFFSET on lane SHM's writer's board, or NULL when
// they do not lie inside the board's body.  A module that carries several
// messages under one reference walks them from here; a single message is
// BoardResolve, which also checks the header at the offset.
u8 *lib_ams::BoardSpan(lib_ams::FShm &shm, u64 offset, u32 length) {
    u8 *ret = NULL;
    lib_ams::FShm *board = lib_ams::BoardOf(shm);
    if (board && board->c_shmhdr) {
        u64 end = offset + u64(length);
        bool inbound = offset >= board->c_shmhdr->datastart && end <= u64(board->shm_region.n_elems);
        ret = inbound ? board->shm_region.elems + offset : NULL;
    }
    return ret;
}
