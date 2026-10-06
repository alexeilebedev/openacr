// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Header: include/lib_ams.h
//

#pragma once
#include "include/algo.h"
#include "include/gen/lib_ams_gen.h"
#include "include/gen/lib_ams_gen.inl.h"

#define amslog(x) prlog(lib_ams::_db.proc_id<<": "<<x)
#define amscat(cat,x) prcat(cat,lib_ams::_db.proc_id<<": "<<x)


namespace lib_ams {
    typedef void (*MsgCb)(lib_ams::FShm &shm, ams::MsgHeader &msg);
    struct shm_c_shmember_curs {
        typedef ams::Shmember ChildType;
        lib_ams::FShm* shm;
        int limit;
        int index;
        shm_c_shmember_curs() { shm=NULL; index=0; }
    };
    struct shm_c_channel_curs {
        typedef ams::Shmchannel ChildType;
        lib_ams::FShm* shm;
        int index;
        shm_c_channel_curs() { shm=NULL; index=0; }
    };
}

// The metric subsystem's prototypes live in include/lib_ams_metric.h, so this
// section names the sources it takes prototypes from rather than taking all of
// them.  A source added to lib_ams and left out of this list gets no
// declaration, and its definition then fails -Werror=missing-declarations.
namespace lib_ams { // update-hdr srcfile:"(cpp/lib_ams/(board|bridge|channel|dump|fdin|lib|outmsg|shm|signal)\.cpp|include/lib_ams\.inl\.h)"
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_ams/board.cpp
    //

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
    lib_ams::FShm *BoardOf(lib_ams::FShm &shm);

    // The chunk of BOARD whose bytes include address PTR, or NULL when PTR lies
    // outside the board's body.
    lib_ams::FChunk *ChunkOf(lib_ams::FShm &board, const void *ptr);

    // Give back one hold ChunkHold took on CHUNK.  A chunk nobody holds or
    // references any longer goes back to the board's free list.
    void ChunkRelease(lib_ams::FChunk &chunk);

    // Describe BOARD's mapped body as chunks of its chunk size, every one free, and
    // make it a board this process fills.  The count follows from the segment, so
    // a board that came from the process pool at the default size, or one the
    // topology sized, is described as it is rather than as it was asked for.  A
    // process may fill several boards, and the chunks of each join the process's
    // chunk pool after those of the boards described before it; a board already
    // described is left as it is.
    void BoardInitChunks(lib_ams::FShm &board);

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
    lib_ams::FShm *BoardCreate(u64 body, u32 chunk_size);

    // Bytes a board segment takes to hold a body of BODY bytes as chunks of
    // CHUNK_SIZE: the header page and whole chunks, at least one.
    u64 BoardSize(u64 body, u32 chunk_size);

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
    lib_ams::FShm *BoardCreatePrivate(u64 body, u32 chunk_size, u32 index = 0);

    // Open process WRITER's message board for reading, so references arriving on
    // that writer's lanes resolve; open it before the first reference arrives.
    // NULL when the segment is missing.
    //
    // Holding it costs a mapping and nothing else: the board takes no member slot,
    // joins no poll list, and is never written by the reader.  The position that
    // matters is the one the reader already keeps on the lane the reference arrived
    // on, and the writer reads it there.
    lib_ams::FShm *BoardOpen(ams::ProcId writer);

    // Offset of lane SHM's slowest reader: the least consume offset over its
    // members, and the ring start when it has none.  A message at or below this
    // offset has been read by everyone the lane delivers to.
    u64 SlowestReaderOffset(lib_ams::FShm &shm);

    // Release what lane SHM's readers have finished with: every Chunkref whose
    // offset the slowest reader has reached gives its count back to its chunk and
    // leaves the table.  Cheap enough to call before every send, and BoardSend
    // calls it itself when it needs a row the table has no room for.
    void BoardReap(lib_ams::FShm &shm);

    // Release every reference lane SHM holds on any chunk, and forget them.  Call
    // when the lane closes: its readers' offsets stop moving, so nothing else would
    // give those chunks back.
    void ChunkrefReleaseAll(lib_ams::FShm &shm);

    // Forget BOARD's lanes and every lane's references into it, and the chunks of
    // every board this process fills.  Call when the board closes; the chunks name
    // bytes of a mapping that is going away.  The chunk pool is one pool across the
    // process's boards, so a process that fills several closes them together, as it
    // does when it exits.
    void BoardReset(lib_ams::FShm &board);

    // Reap every lane that references a chunk of BOARD, so each chunk whose last
    // reference its reader has passed returns to the free list.  A board's owner
    // that judges its chunks by how many are free calls this first: a lane is
    // otherwise reaped only when something is sent on it or the free list runs dry,
    // and a chunk a quiet lane's reader passed long ago still reads as held.
    void BoardReapAll(lib_ams::FShm &board);

    // Take a free chunk of BOARD to fill, off the free list and emptied, or NULL
    // when none is free -- after reaping every lane that references the board, since
    // a reader may have moved past the last reference into some chunk.  The caller
    // holds the chunk by filling it; when it has moved on, ChunkRelease with no
    // references outstanding is what frees it.  A module that fills several chunks
    // at once -- one per class of message -- takes them here and reserves with
    // ChunkAlloc; BoardAlloc is the one-chunk form over the two.
    lib_ams::FChunk *ChunkTake(lib_ams::FShm &board);

    // Reserve LEN bytes at the end of CHUNK and return where to write them, or NULL
    // when the chunk has no room for them.  Nothing is published by the reservation.
    // A reservation is exactly LEN bytes, so messages lie in a chunk back to back the
    // way they lie in a datagram, and a datagram read into a chunk is a run of
    // messages the board can reference without moving a byte.
    void *ChunkAlloc(lib_ams::FChunk &chunk, int len);

    // Reserve LEN bytes of message space in BOARD and return where to write them,
    // or NULL when no chunk can take them.  The space is the next run of the chunk
    // being filled; a message that does not fit there retires that chunk and starts
    // a free one (ChunkTake).  A message longer than a chunk is refused outright.
    //
    // Nothing is published by the reservation.  The caller formats the message in
    // place and then sends it with BoardSend, or gives the space back with BoardTrim.
    void *BoardAlloc(lib_ams::FShm &board, int len);

    // Shorten the most recent reservation of BOARD, at PTR, to LEN bytes: ChunkTrim
    // on the chunk being filled, when PTR lies in it.
    void BoardTrim(lib_ams::FShm &board, void *ptr, int len);

    // TRUE when lane SHM can take a reference to a message in CHUNK as things
    // stand: it holds a Chunkref row for CHUNK or has room for one, and the ring
    // has room for the reference with nothing queued ahead of it.
    bool BoardRoomQ(lib_ams::FShm &shm, lib_ams::FChunk &chunk);

    // Make room on lane SHM for a reference to a message in CHUNK, and say whether
    // there is any: BoardRoomQ as things stand, and after one reap when there is
    // not.  The reap runs only when it is needed, so a run of messages out of one
    // chunk pays for none.  Ask this of every lane a message goes to before sending
    // it to any of them: a reader that missed one message of a numbered stream has
    // a gap it cannot ask to have filled.
    bool BoardMakeRoom(lib_ams::FShm &shm, lib_ams::FChunk &chunk);

    // Begin sending MSG, a message in a chunk of this process's board, to lane SHM
    // by a reference message of REFLEN bytes: the ring slot the caller formats the
    // reference into, or NULL when the send cannot happen now -- MSG is not in the
    // board, the board is a private mapping no reader can open, the lane references
    // board_max_chunkref chunks already and reaping frees none of them
    // (n_board_nochunkref), or the ring has no room.  The caller writes
    // its reference into the slot and finishes with BoardEndSend; the pair exists so
    // a module may carry a reference inside a message of its own, with the fields
    // its reader needs beside the coordinates.
    void *BoardBeginSend(lib_ams::FShm &shm, ams::MsgHeader &msg, int reflen);

    // Publish the reference that BoardBeginSend reserved PTR for on lane SHM,
    // REFLEN bytes long, and count it against CHUNK, the chunk the referenced
    // message lies in.
    //
    // The chunk is charged after the reference is published and before anything
    // else runs, so no reader sees a reference to a chunk the writer thinks free.
    // A reader is done with the payload when its lane offset passes the reference,
    // which is what the row's `woffset` records.
    void BoardEndSend(lib_ams::FShm &shm, lib_ams::FChunk &chunk, void *ptr, int reflen);

    // Send MSG, a message formatted into a chunk of this process's board, to lane
    // SHM: write an ams::BoardrefMsg naming it into the ring and count it against
    // its chunk.  TRUE when the reference was published.  FALSE when MSG is not in
    // the board, when the lane references board_max_chunkref chunks already and
    // reaping frees none of them, or when the ring has no room; the message stays in
    // the board and the caller retries or gives its space back with BoardTrim.
    bool BoardSend(lib_ams::FShm &shm, ams::MsgHeader &msg);

    // The LENGTH bytes at board OFFSET on lane SHM's writer's board, or NULL when
    // they do not lie inside the board's body.  A module that carries several
    // messages under one reference walks them from here; a single message is
    // BoardResolve, which also checks the header at the offset.
    u8 *BoardSpan(lib_ams::FShm &shm, u64 offset, u32 length);

    // -------------------------------------------------------------------
    // cpp/lib_ams/bridge.cpp
    //

    // Find the lowest slot N for a new bridged process of PROCTYPE on NODEIDX.
    // The slot is used as both the process index in the new ProcId and the
    // grpidx of the bridge shms.  Skipped: the caller's own slot (avoids
    // colliding with our own proc_id when proctype/nodeidx happen to match)
    // and any slot whose shm pair already exists locally (avoids re-using a
    // slot owned by a still-live bridge).
    int NextBridgeSlot(ams::Proctype proctype, int nodeidx);

    // Allocate FProc + both shm files (writable) for a bridged user process at
    // GRPIDX (use NextBridgeGrpidx to pick one). On success the inbound and
    // outbound shm files exist on disk so the forked child can find them via
    // shm_open. The child is pre-registered as a reader of shm_out so its
    // ShmOpen(read) succeeds.  READER_PROC_ID is pre-registered as a reader of
    // shm_in (the child->parent ring) so the ring is bounded from the start:
    // a child that writes before the real reader (gateway/txn) opens the ring
    // blocks on backpressure instead of overrunning a reader-less ring (which
    // is treated as unbounded and silently overwrites).  The real reader's own
    // AddReadShmember on open is then a dedup no-op.  Pass proctype_
    // (the null proctype) to skip this when there is no distinct parent reader.
    // Caller is then free to open the shms locally for its own reads/writes
    // (e.g. in a single-process bridge) or leave them for another process to
    // open (e.g. lib_x2).
    // ignore:ptr_byref
    bool CreateBridgeShms(ams::ProcId child_proc_id, int grpidx, ams::ProcId reader_proc_id, lib_ams::FShm *&shm_in, lib_ams::FShm *&shm_out, i64 size = 0, i32 maxmsg = 0);

    // Format the value of a `-proc:` argument that initializes a bridged child with
    // CHILD_PROC_ID at GRPIDX, and return it.  The child's rings are named from its
    // own side: it reads `in`, the parent's BridgeOutGrp, and writes `out`, the
    // parent's BridgeInGrp.  NICKNAME, the child's human-facing name (the userproc
    // name), prefixes its published metrics in place of the proc id.
    tempstr ChildProcStr(ams::ProcId child_proc_id, int grpidx, algo::strptr nickname = algo::strptr());

    // -------------------------------------------------------------------
    // cpp/lib_ams/channel.cpp
    //

    // Return slot I of the channel table in the control page HDR, or NULL past the
    // table's end.  A tool that maps a segment without opening it reads the table
    // through this.  A slot whose key is 0 is free.
    ams::Shmchannel *HdrChannelFind(ams::Shmhdr &hdr, int i);

    // Return slot I of the channel table of SHM, or NULL past the table's end or when
    // the segment is not mapped.
    ams::Shmchannel *channel_Find(lib_ams::FShm &shm, int i);

    // Return this process's handle on the channel of ring SHM keyed KEY, or NULL when
    // KEY names a channel the ring has no slot for.  Key 0 is the base channel, the
    // ring as a whole, which needs no slot.  A keyed channel the ring holds no slot
    // for yet is opened here, and the writer and its reader may each open it first,
    // in either order.  A new channel has written nothing, read nothing, and has no
    // room until its reader sets a window or grants some.  The handle lives as long
    // as the ring stays open in this process: ShmClose deletes it with the mapping.
    lib_ams::FChannel *ChannelOpen(lib_ams::FShm &shm, u64 key);

    // Park the writer on CHANNEL waiting for budget for EXTRA more bytes, as
    // ParkWriter parks it on a ring, and return true when the ring's budget and the
    // channel's room both have it after all.
    bool ParkWriter(lib_ams::FChannel &channel, u32 extra = 0);

    // True when a message of EXTRA more bytes may be written on CHANNEL now: the ring
    // has budget for it, and a keyed channel has room for it under its limit.
    bool HasBudgetQ(lib_ams::FChannel &channel, u32 extra = 0);

    // Begin writing a message of LENGTH bytes on CHANNEL, non-blocking: the write
    // pointer, or NULL when the channel's limit or the ring refuses the message now.
    // A channel whose limit refuses it parks the writer in signaled mode, so the
    // reader's next raise wakes the process, as a ring's reader wakes a writer that
    // ran out of ring budget.  The retry is the caller's, as it is on a ring.
    void *BeginWrite(lib_ams::FChannel &channel, i32 length);

    // Count NBYTE bytes on CHANNEL as written, as its writer, with no message of
    // the channel's own.  A writer charges what a message commits the channel's flow
    // to when the message itself travels on another channel or carries several
    // flows: a publish that obliges an acknowledgment on a second partition, or a
    // datagram packing records of several.  Its reader counts the same bytes with
    // ChannelRead, so the two counts agree.  The base channel counts nothing here.
    void ChannelCharge(lib_ams::FChannel &channel, u64 nbyte);

    // Finish the message of LEN bytes begun at PTR on CHANNEL: publish it to the ring
    // and count it on the channel.
    void EndWrite(lib_ams::FChannel &channel, void *ptr, i32 len);

    // Count NBYTE bytes this process read on CHANNEL, as the channel's reader.  On a
    // channel with a window the limit follows the count in passing, as a ring's
    // write limit follows its readers' offsets: it becomes the count plus the window.
    // A process that is not the channel's reader counts nothing.
    void ChannelRead(lib_ams::FChannel &channel, u64 nbyte);

    // Count NBYTE bytes this process read on the channel of ring SHM keyed KEY, as
    // the channel's reader, for a message that names the channel it was charged to.
    // KEY 0 is the ring itself, which counts nothing here.  A reader counts by the
    // key the message carries, so it counts every charged message, including one
    // whose request it no longer holds.
    void ChannelReadKey(lib_ams::FShm &shm, u64 key, u64 nbyte);

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
    bool ChannelSetWindow(lib_ams::FChannel &channel, u64 window);

    // Raise CHANNEL's limit to WLIM by hand, as its reader, and return true when
    // it rose: for a reader whose room is set by something other than what it has
    // read, such as records it holds until others release them.
    bool ChannelGrant(lib_ams::FChannel &channel, u64 wlim);

    // Step the channel cursor CURS to the next slot.
    void shm_c_channel_curs_Next(shm_c_channel_curs &curs);

    // Start the channel cursor CURS at the first slot of the table of ring PARENT.
    void shm_c_channel_curs_Reset(shm_c_channel_curs &curs, lib_ams::FShm &parent);

    // True while CURS stands on a claimed slot: the claimed slots are a prefix of the
    // table, so the walk ends at the first free slot or at the table's end.
    bool shm_c_channel_curs_ValidQ(shm_c_channel_curs &curs);

    // Return the slot CURS stands on.
    ams::Shmchannel& shm_c_channel_curs_Access(shm_c_channel_curs &curs);

    // -------------------------------------------------------------------
    // cpp/lib_ams/dump.cpp
    //

    // Add the group table's columns to TBL: the group, its type, and one column per
    // process of this node, which holds that process's membership of the group.
    void GrpTableCols(algo_lib::FTxttbl &tbl);

    // Add one row per mapped ring matching REGX to TBL, of type ring: for each
    // process of the node, R when it reads the ring -- green while its heartbeat
    // is fresh, red once it is stale -- with the bytes it lags the writer by when
    // that is more than a few, and W when it writes the ring.
    void GrpTableRings(algo_lib::FTxttbl &tbl, algo_lib::Regx &regx);

    // Add one row to TBL per shm channel of each mapped ring matching REGX: the
    // ring, the channel's key and reader, the bytes written and read on it, its
    // limit and the window it follows the read count by, the room the writer has
    // left, and the bytes written and not yet read.
    void ChannelTable(algo_lib::FTxttbl &tbl, algo_lib::Regx &regx);

    // Print the shm channels of this node's rings matching REGX as a table, when
    // there are any.  Both group-table dumps end with it.
    void PrintChannelTable(algo_lib::Regx &regx);

    // Print the group table of this node's rings matching REGX, for a process
    // that knows no fabric group; lib_x2::DumpGrpTableVisual adds those.  The shm
    // channels of those rings follow in a table of their own, when there are any.
    void DumpGrpTableVisual(algo_lib::Regx &regx);

    // Print message MSG to string OUT according to format FMT.
    // FMT.STRIP outer layers are stripped.  An envelope, whose msgtype says
    // strip:Always, yields what it nests in every format.  A message whose msgtype
    // says strip:Decode yields what it nests or the bytes it carries only under
    // FMT.PRETTY; ssim and bin output print
    // it whole, so a reader of the tuple sees the message.  With FMT.FORMAT bin, the
    // message left after stripping is printed whole as binary; otherwise it prints
    // as text.  With
    // FMT.PRETTY, each nested message and byte payload prints on a line of its own,
    // indented, cut to the payload limit.  With FMT.SHOWLEN, the message length is
    // included.  amc generates the printer from the schema, so a new message prints
    // this way with no code here.
    void PrintMsg(algo::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out);

    // This function should be called if the ams logcat is enabled
    // It prints the given MSG to ams logcat using pretty format.
    // Heartbeats (msgtype heartbeat:Y) are skipped unless verbose 2 is on
    void TraceMsg(algo_lib::FLogcat *logcat, lib_ams::FShm &shm, ams::MsgHeader *payload);
    tempstr ToString(ams::MsgHeader &msg);

    // Convert message MSG to string in a way suitable for debugging
    // (some information is lost in exchange for readability)
    tempstr ToDbgString(ams::MsgHeader &msg);

    // -------------------------------------------------------------------
    // cpp/lib_ams/fdin.cpp
    //

    // Read next input line from stdin, parse as ams message, and write to target shm.
    // If the target shm is full, stop reading (backpressure).
    // If the shm where we are posting the message is full (won't accept the message)
    // then reading of fdin is stopped and will resume after the shm has room.
    // If there is nowhere to post the message because no target shm is found, the counter
    // trace.n_fdin_drop_notgt is incremented and a message is printed in verbose mode.
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_fdin_read_Step(); // dmmeta.fstep:lib_ams.FDb.cd_fdin_read

    // Stop reading stdin and drop the stdio-mode loopback shm. Once both are
    // gone, MainLoop has no input source from the stdio path — if the app has
    // no other shm peers either, it will exit naturally. Apps that keep peers
    // alive past stdin EOF (e.g. ams_bridge waiting on shm_in echoes) need not
    // do anything special: their other shms keep the loop running until eof or
    // peer death drops them too.
    // void cd_fdin_eof_Step(); // dmmeta.fstep:lib_ams.FDb.cd_fdin_eof

    // Begin reading ams control messages from stdin.
    //
    // Two paths ask for this and either may come first: lib_ams::Init takes it for
    // a stdio peer, and an interactive process takes it when it opens its shms.
    // Stdin is one descriptor and a descriptor carries one epoll registration, so
    // the second caller joins the reader the first one made -- two readers would
    // leave one of them subscribed to nothing and its messages unread.
    void BeginReadStdin();

    // -------------------------------------------------------------------
    // cpp/lib_ams/lib.cpp
    //

    // Initialize the library as the process SPEC describes, with MSG_CB receiving
    // every message read, and return whether it could.  SPEC's id is a proc key,
    // `[<cluster>.]<proctype>-<node>-<proc>`, and its last part is the proc id.
    // Which fields SPEC sets decides the role:
    // id alone                a stdio peer: messages parsed from stdin reach
    // MSG_CB through a loopback ring, and the process
    // owns no shm namespace
    // id and prefix           a server: PREFIX names the shm files it creates,
    // and stale unlocked ones are cleaned on first call
    // id, prefix, in and out  a bridged child: open the existing ring pair,
    // reading IN and writing OUT; PREFIX identifies the
    // server that owns the namespace
    // NICK, when set, prefixes published metrics in place of the proc id.  The
    // connection keys, gw and shadow, are the client library's and are not read
    // here.
    bool InitProcspec(ams::Procspec &spec, lib_ams::MsgCb msg_cb = NULL);

    // Initialize the library from PROC_STR, the value of a -proc argument, with
    // MSG_CB receiving every message read, and return whether it could.  PROC_STR
    // is read as an ams.Procspec (InitProcspec says what each field does); a string
    // that does not parse is reported and refused.
    bool Init(algo::strptr proc_str, lib_ams::MsgCb msg_cb = NULL);
    void Uninit();

    // Emit message. In shm mode, write to output shm.
    // In stdio mode, print as text.
    void EmitMsg(ams::MsgHeader &msg);

    // Notify lib_ams that process PID has exited with STATUS.
    // Clear the pid in any FProc record matching PID and return that record (if any).
    // For each shm in the database:
    // - If PID was the writer, clear writer_pid and wake the ring's parked
    // readers, whose wakeup the writer may have died owing. If we are reading
    // from this shm, also set the eof flag on its shmhdr so cd_poll_read_Step
    // can permanently remove it from the poll loop once any remaining messages
    // are drained.
    // - If PID was a reader, clear that shmember's pid so the writer's budget
    // is no longer constrained by it.
    //
    // The unlink is the part that needs care, because a writer's death is not the
    // segment's death.  Consider a userproc bridged to a gateway: its supervisor
    // creates the parent->child ring, the gateway writes it, and the child reads
    // it.  When the gateway exits, the supervisor collects the death and reaches
    // this walk, where the ring matches on writer_pid -- so unlinking every
    // segment the dead pid wrote would take that name away while the child still
    // owns it.  A child that has not yet opened the ring would then get ENOENT
    // out of its own shm_open and report a segment it never had the chance to
    // map, which is a startup failure invented by someone else's exit.
    //
    // A segment's name belongs to the proc its grp id names, and only that proc's
    // death retires the name.  So the unlink is scoped to the segments the dead
    // proc owns; a writer of somebody else's ring is only a writer, and lets go by
    // clearing writer_pid.  Nothing leaks by leaving the file: the owner's own
    // teardown unlinks it, and a segment whose writer is gone and whose lock is
    // free is already reclaimable by the orphan sweep.
    lib_ams::FProc *ProcExit(int pid, int status);
    void UnreadMsg();

    // Set the segment size a ring this process creates gets by default: a body of
    // at least SIZE bytes for messages up to the process ceiling.
    void SetDfltShmSize(u32 size);

    // -------------------------------------------------------------------
    // cpp/lib_ams/outmsg.cpp
    //

    // Move SHM's queued messages into the ring, oldest first, stopping at the
    // first one the ring has no room for.  Stopping rather than skipping is what
    // preserves the order the caller wrote them in.  BeginWrite calls this before
    // any reservation, so a direct writer never overtakes the queue; the
    // reservation here is therefore the ring's own, ReserveWrite.
    void OutmsgFlush(lib_ams::FShm &shm);

    // Write what the queued rings will take, and keep the rest for the next pass.
    // A ring that empties leaves the list; one that is still blocked goes to the
    // back of it, so no ring can starve another.  The list length is sampled at
    // entry, so a ring rotated to the back is not visited twice in one pass.
    //     (user-implemented function, prototype is in amc-generated header)
    // void zd_outshm_Step(); // dmmeta.fstep:lib_ams.FDb.zd_outshm

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
    void *BeginWriteQueue(lib_ams::FShm &shm, int length);

    // Finish the message BeginWriteQueue started at PTR, LEN bytes long: publish
    // it to SHM when it was built there, send it by the board when it is longer
    // than the ring's largest message, and otherwise put it at the back of the
    // ring's queue and arm the step that will write it.  Return false when the
    // message is oversize and the board cannot take it now, in which case it is
    // dropped and counted in n_outmsg_oversize_drop.
    bool EndWriteQueue(lib_ams::FShm &shm, void *ptr, int len);

    // Write MSG to SHM, queued: into the ring when it has room and nothing waits
    // ahead of it, otherwise onto the ring's queue, which zd_outshm writes as
    // budget appears.  The message WriteMsg would drop is the one this keeps, so
    // this is the write for a message with no retry of its own behind it -- a
    // command, its answer -- where a drop is a requester waiting out its whole
    // deadline for output that was thrown away.  Return false when MSG is
    // oversize and the board cannot take it now, which EndWriteQueue counts.
    bool WriteMsgQueue(lib_ams::FShm &shm, ams::MsgHeader &msg);

    // -------------------------------------------------------------------
    // cpp/lib_ams/shm.cpp
    //

    // Free bytes on the tmpfs backing /dev/shm.  INT64_MAX on statvfs
    // failure (treated as "no limit" by callers comparing against a need),
    // so chroots or platforms without /dev/shm don't hard-fail callers.
    i64 GetShmAvail();

    // Size in bytes of the tmpfs backing /dev/shm, and 0 when statvfs cannot
    // answer, so a reserve derived from it is nothing where the filesystem is
    // unknown.
    i64 GetShmTotal();

    // Bytes the segment belonging to GRP_ID already occupies on the tmpfs, and zero
    // when no such segment exists yet.
    //
    // A caller sizing a topology against GetShmAvail needs this to avoid charging
    // the same bytes twice.  Free space is what the filesystem has left after every
    // segment already created has taken its share, so a segment that is on disk is
    // not a future demand on the filesystem -- yet a walk of the topology's rows
    // counts it, because the rows describe the whole set whether or not it has been
    // made.  Summing the rows and comparing against free space therefore demands
    // room for the existing segments a second time, and a node whose segments are
    // created before the comparison runs is refused at a little over half the
    // filesystem it actually fits in.
    //
    // What is credited is the space consumed rather than the size declared.  A
    // segment is created by extending an empty file, and a tmpfs allocates those
    // pages as they are first touched, so a fresh one can stand at its full length
    // while holding almost no blocks.  Its untouched remainder is still a claim on
    // free space, and st_blocks is what leaves that claim in the caller's need.
    i64 ShmExistingSize(ams::GrpId grp_id);

    // TRUE when the segment of group GRP_ID exists on this node's tmpfs, whoever
    // made it and whether or not this process has opened it.
    bool ShmExistsQ(ams::GrpId grp_id);

    // Scan /dev/shm for orphaned ams segments and unlink them.  A segment is an
    // orphan when no process holds its write lock and it is no longer being created
    // (see OrphanSegmentQ) -- i.e. its writer crashed or was kill -9'd without
    // unlinking, or it was created but never claimed by a writer.  Orphans are
    // collected during the walk and unlinked after it, so the unlink never mutates
    // the directory Dir_curs is iterating.
    void CleanOldShmFiles();

    // return TRUE if shared memory region is attached to shm SHM.
    bool ShmFdOpenQ(lib_ams::FShm &shm);

    // Return the segment size that gives a ring a writable body of at least BODY
    // bytes while carrying messages of up to MAXMSG.  A segment is a control header
    // of whole pages, a power-of-two body that a message offset wraps inside, and
    // one message of linear overflow past the body, so a message near the top of
    // the body writes straight into the overflow and never straddles the end.  The
    // size counts one header page; ShmCreate adds the pages a ring's declared shm
    // channels need past the first (ShmControlSize).
    //
    // The body is floored at four messages, and that floor is the whole reason
    // this arithmetic lives in one function.  A writer holds two messages back
    // from the slowest reader so it cannot lap that reader mid-message, so a body
    // of exactly two messages leaves a writable span of zero and every send
    // reports no budget forever, while a body below two makes the write limit sit
    // behind the reader entirely.  Four leaves the ring half its body to write
    // into.
    u32 ShmSize(u32 body, u32 maxmsg);

    // Create (or open) shared memory for reading/writing (as specified in FLAGS)
    // and return success status
    bool ShmCreate(lib_ams::FShm &shm, ams::ShmFlags flags);

    // Open shm for reading or writing (or both)
    // If the shm is being opened for writing and doesn't exist, it's created.
    // Otherwise it must have been created with ShmCreate.
    //
    // A successful return means attached, and on a read that includes holding a
    // slot in the segment's member table.  The slot is where the ring records how
    // far this reader has consumed, so a reader without one is delivered nothing and
    // is invisible to the writer's budget.  Reporting such an open as success hands
    // the caller a ring that will never speak to it: a process that joins a pool,
    // waits for work that cannot arrive, and misses the barrier its owner is
    // counting on, with nothing in any log to say which of the two it was.
    //
    // Three ways to fail to attach, and each one names itself here rather than
    // leaving the caller's own error as the only trace.  The segment can be missing
    // or unmappable -- a read open never creates one, so a child that starts before
    // its ring exists, or after the ring was unlinked, ends up here.  The member
    // table can have no slot for this reader, either because the writer never
    // registered it or because the table is full.  And the write lock can be held
    // by another process, which is a live predecessor rather than a dead one.
    bool ShmOpen(lib_ams::FShm &shm, ams::ShmFlags flags);

    // Update budget for SHM
    // Return TRUE if the WRITELIMIT was updated.
    // (WRITELIMIT is the point beyond which no message can be written
    // because doing so would overwrite data not yet consumed by one of the read members.)
    // A board has no write budget to update.  It is not a ring, so it has no
    // writelimit and no member offsets to derive one from; its space is tracked by
    // chunk, in the writer's own memory.
    bool UpdateBudget(lib_ams::FShm &shm);

    // True when SHM can be written: it is open for writing and its control header
    // is mapped.  Every function that samples or charges the write budget reads
    // that header, so each one asks this first.
    //
    // The two conditions are one fact, because ShmClose clears both -- and the
    // pointer is the one that matters.  A closed ring keeps its record in the shm
    // table (a userproc reuses its grp ids across incarnations) and keeps its
    // max_msg_size, so a stale writer's message still passes a length check and
    // arrives at a budget counter reached through a null header.  A bridge ring
    // released while a connection still points at it is exactly that writer.
    bool WritableQ(lib_ams::FShm &shm);

    // Wait for write space in SHM.  A successful return guarantees room for a
    // full max_msg_size message -- the ring reserves that much slack, so the
    // woff<writelimit test is not length-specific; there is nothing to wait for
    // per-length (WriteMsg/BeginWrite reject any oversize write outright).  When
    // BLOCK is false, sample the budget once (re-running UpdateBudget) and return
    // whether there is room.  When BLOCK is true and there is none, re-sample this
    // ring's own budget in a tight loop until a reader drains it, so a blocking
    // write never drops.  Before it spins it sends the wakeups the pass owes its
    // peers (FlushWake), since the reader it waits on may be asleep on one of them.
    // The loop rides no list and reads no other ring, and nothing else in this
    // process runs while it spins.
    // A closed ring returns FALSE at once, in blocking mode too: no reader can
    // ever drain it, so waiting would be waiting forever.
    // Return TRUE if writing can proceed.  Public so a caller can wait for room
    // ahead of a zero-copy *_FmtShm.
    bool WaitBudget(lib_ams::FShm &shm, bool block);

    // Reserve LENGTH bytes at SHM's write offset and return where to build the
    // message, or NULL when the ring refuses the length or has no room for it now.
    // This is the ring's own admission, and the hot path of every write: the budget
    // check is inlined so a message that fits costs one straight-line sample.  On a
    // miss the budget is re-sampled once through UpdateBudget, and in signaled mode
    // the writer then parks (ParkWriter) so a draining reader wakes it.  Still no
    // room bumps nnobudget.  A too-big message, and a ring that is not writable,
    // return NULL without touching the counters, which live in the ring's own
    // header and are gone once the ring is closed.  BeginWrite is what a writer
    // calls; it puts the ring's queued messages ahead of the reservation.
    void *ReserveWrite(lib_ams::FShm &shm, int length);

    // Begin writing a message of LENGTH bytes to SHM, non-blocking: the write
    // pointer, or NULL when the ring cannot take the message now.
    //
    // A ring may hold messages an earlier writer queued because the ring was full
    // at the time (BeginWriteQueue).  The txn keeps a committer's ring full of
    // records for as long as the committer reads slower than they arrive, and a
    // command relayed onto that ring waits in its queue.  Were the record delivery
    // allowed to reserve each slot the committer frees, the ring would stay full
    // and the command would wait out the whole run -- the requester times out on
    // an answer that was never lost, only never sent.  So the queue is written
    // first, and while any of it remains the reservation is refused: every writer
    // of a ring yields to what was queued on it before, and a queued message is
    // never overtaken.
    void *BeginWrite(lib_ams::FShm &shm, int length);

    // Begin writing message of length LENGTH, blocking until the ring has room.
    // WaitBudget busy-waits for a max_msg_size slot; BeginWrite then returns the
    // write pointer (or NULL for a too-big message, which it rejects outright).
    void *BeginWriteBlock(lib_ams::FShm &shm, int length);

    // Finish writing the message of length LEN and publish it: sfence so the
    // payload is visible before the woff store, then re-arm the reader's poll
    // entry.  In signaled mode, owe any reader parked on the ring a wakeup
    // (WakeReader), which the end of the pass sends.
    //
    // c_reader is this process's own slot in the ring's reader table, so a ring
    // with one is a ring this process both writes and reads -- a loopback.  The
    // publish is then its own wakeup and goes through the local wake path: a
    // loopback ring that had parked would otherwise sit on the park list with its
    // sleeping flag raised while it is being polled, and nothing would put the two
    // lists back in agreement, since a same-process writer sends itself no signal.
    // UnparkReader here also sets next_loop, so the ring is polled before the loop
    // sleeps.
    void EndWrite(lib_ams::FShm &shm, void *ptr, int len);

    // Write message MSG to SHM, non-blocking.  Return TRUE on success; FALSE
    // when the ring has no budget (the message is dropped -- the caller decides
    // whether to retry, unread, or discard).
    //
    // A message the ring cannot hold is copied into the writer's message board
    // instead, and the ring carries a reference to it.  The board path answers the
    // same way the ring does -- FALSE when it cannot take the message, and the
    // board space is given back -- so a caller sees one contract whichever way the
    // message travels, and a writer with no board rejects an oversize message.
    bool WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg);

    // Write message MSG to SHM, blocking until the ring has room.  WaitBudget
    // busy-waits for a max_msg_size slot, then WriteMsg performs the write.
    // Always succeeds except for a too-big message, or a ring that is closed and
    // therefore never gains room.  Use only where dropping would be incorrect --
    // correctness, not config, picks this variant.
    bool WriteMsgBlock(lib_ams::FShm &shm, ams::MsgHeader &msg);
    //     (user-implemented function, prototype is in amc-generated header)
    // void shm_file_Cleanup(lib_ams::FShm &shm); // dmmeta.ffunc:lib_ams.FShm.shm_file.Cleanup

    // Close shm: unmap the region, drop the fd, and unlink the file if this
    // process created it.  Send the wakeups the pass owes first, and clear the
    // sleeping flag if a reader was sleeping.
    // The record itself stays in the shm table so the next incarnation under the
    // same grp reuses it.
    void ShmClose(lib_ams::FShm &shm);

    // Register PROC_ID as reader of shm SHM_ID starting at offset 0
    // This is done by the writer, and the change updates shared memory (shmhdr)
    // and immediately becomes visible by clients.
    //
    // The member table is fixed at creation, so registration can run out of room.
    // Nothing about that is visible to the proc being registered: its own open finds
    // no slot and it reads a ring that never delivers, one caller frame removed from
    // the writer that could not seat it.  So the writer says so at the moment it
    // fails, and names the table's size -- a reader denied a slot is a member of the
    // group as far as the topology is concerned, and only the segment disagrees.
    ams::Shmember *AddReadShmember(lib_ams::FShm &shm, ams::ProcId proc_id);
    ams::Shmember *FindReadShmember(lib_ams::FShm &shm, ams::ProcId proc_id);
    void CloseAllShms();

    // Evaluate current budget.  A ring that cannot be written has no budget: zero,
    // rather than a read through its released header.
    u64 GetBudget(lib_ams::FShm &shm);

    // Check if thre is room in SHM to write at least 2 messages, plus EXTRA.
    // The function re-samples current budget if needed.  A ring that cannot be
    // written has no room, and no counter to charge the miss to.
    bool HasBudgetQ(lib_ams::FShm &shm, u32 extra = 0);

    // If the shm is open for reading, check to see if a message
    // is available. If it is available, return pointer to message.
    //
    // The writer copies a message in, fences, and then stores woff (EndWrite), so
    // a woff this reader sees announces bytes already written.  The reader's half
    // is the mirror: load woff, fence, and only then read the header and body it
    // announces.  With the fence ahead of the load instead, nothing orders the
    // payload read after the woff read, and on a weakly ordered core (aarch64,
    // where lfence is dmb ishld) a reader could see the advanced woff and still
    // read the bytes the slot held before.  A reloaded woff equal to the cached one
    // announces nothing new, and the fence taken when it was cached already orders
    // every read below it, so an idle ring skips the fence.
    ams::MsgHeader *PeekMsg(lib_ams::FShm &shm);

    // Check all shms (that are not already readable) for readability and
    // transfer readable shms to the read heap with correct sort key.
    // In signaled mode, idle shms are removed from the poll loop; the reader
    // sets sleeping=1 on the shmember so that the writer can wake it via kill().
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_poll_read_Step(); // dmmeta.fstep:lib_ams.FDb.cd_poll_read
    ams::Shmember *shmember_Find(lib_ams::FShm &shm, int i);
    void shm_c_shmember_curs_Next(shm_c_shmember_curs &curs);
    void shm_c_shmember_curs_Reset(shm_c_shmember_curs &curs, lib_ams::FShm &parent);
    bool shm_c_shmember_curs_ValidQ(shm_c_shmember_curs &curs);
    ams::Shmember& shm_c_shmember_curs_Access(shm_c_shmember_curs &curs);

    // -------------------------------------------------------------------
    // cpp/lib_ams/signal.cpp
    //

    // This process was asked to stop.  One definition, reached by every spelling of
    // the request: an inbound ams.TerminateMsg addressed to this proc, the stdin EOF
    // that a parent's exit closes, and the SIGTERM or SIGINT a stop sends.
    //
    // What stopping means depends on the role, and h_terminate is where a role says
    // so.  The default is the only thing a process can do about its own stop -- end
    // its main loop -- and it is what every process wants except a supervisor, whose
    // stop is the orderly shutdown of the node it runs: its own exit is the last
    // step of that, not the first.
    void Terminate();

    // Install a handler for SIG that writes to a pipe, so the disposition runs from
    // the main loop instead of inside the handler.
    //
    // A handler runs at an arbitrary instruction of the interrupted code, so it may
    // perform only what stays correct there.  A disposition that allocates, or that
    // moves a structure the main loop also moves, does not qualify: interrupting the
    // loop inside one leaves a container reporting a member it has lost, or an
    // allocator deadlocked against its own lock.  Both fail silently.  Denser
    // signals make the collision likelier, since each arrives while the loop is
    // still handling the previous one.
    //
    // The handler therefore writes one byte and returns.  The pipe's read end is an
    // ordinary iohook, so the disposition runs between two of the loop's actions.
    //
    // No signal is blocked.  A blocked signal's mask survives exec, so a parent that
    // blocked SIGCHLD would hand every child the same mask and break the child's own
    // waits.
    //
    // The first routed signal opens the pipe, and one iohook reads it for all of
    // them.  Routing is idempotent, and a process that routes nothing keeps the
    // dispositions it already had.
    //
    // Routing is requested rather than given to every process.  A registered iohook
    // is work the main loop can be woken by, so `giveup_time_Step` holds next_loop
    // at the clock while any iohook exists.  A process that ends by running out of
    // inputs to poll would never end again, so only a process whose exit is its own
    // decision may route.
    void RouteSignal(int sig);

    // Install this process's answer to SIGTERM and SIGINT, which its proctype
    // decides.  Call it once the proc id is known, since the proctype comes from
    // there.
    //
    // A process that owns its own stop gets the graceful handler, so the signal
    // means what an ams.TerminateMsg means.  A node's drain sends SIGTERM to a
    // userproc group leader as the polite stop, and without a handler the default
    // disposition kills it mid-write, leaving its owner waiting on a barrier nobody
    // will report.  A process whose disposition is too large to run inside a handler
    // asks for RouteSignal instead.
    //
    // A module of a node ignores both.  Ctrl-C signals the whole foreground process
    // group, so every module would exit while its supervisor is still on the first
    // step of the node's stop.  A module's stop is its node's, asked for by an
    // ams.TerminateMsg in stoprank order, so it waits to be asked -- and stoprank
    // names exactly that set, since a client, a supervisor or a one-shot tool
    // carries zero.
    void SetupTerminateSignal();

    // Move SHM's parked reader back into the poll loop: clear the sleeping flag its
    // writer reads, take it off the park list, and re-arm it for polling.  Every
    // wake goes through here, so the two lists stay a partition of the open
    // readers.
    //
    // Keeping the loop awake is part of the same act.  A ring on the poll list is
    // polled only on a pass the loop runs, and giveup_time sleeps unless a step sets
    // next_loop to now; the cd_poll_read step sets it while it runs, but only while
    // its list is already non-empty.  The loopback publish in EndWrite hands a ring
    // back mid-pass, after that gate has been read, so nothing else would keep the
    // loop off epoll_wait and a ring with data waiting would sleep unpolled until an
    // unrelated event.  Setting next_loop here holds the invariant "a pollable ring
    // keeps the loop awake" for that path.  A ring handed back by SignalReadStep
    // instead came from epoll returning, where giveup_time has already set next_loop,
    // so the store is a no-op there and harmless.
    void UnparkReader(lib_ams::FShm &shm);

    // Wake every parked reader.  A SIGRTMIN names no stream -- the signal says only
    // that some peer freed something -- and leaving signaled mode ends parking
    // altogether, so both hand the whole parked set back to the poll loop and let
    // the next cd_poll_read_Step re-check each for data.
    void UnparkReaderSet();

    // Drain the signalfd (coalesced SIGRTMIN wakeups read as one event) and move
    // every parked reader back into the poll loop.
    void SignalReadStep();

    // Enter or leave signaled mode.  Entering blocks SIGRTMIN and arms an
    // always-armed signalfd registered with the iohook, so a peer's SIGRTMIN wakes
    // the epoll_wait.  ENABLE picks which.  Leaving first sends the wakeups this
    // pass owes its peers, then removes the hook, closes the signalfd, and moves
    // every parked reader back into the poll loop; the SIGRTMIN block stays in
    // place for the rest of the process lifetime.
    void SetSignaledMode(bool enable);

    // Owe the readers of SHM a wakeup for a message just published on it.  The ring
    // goes on the pass's wake list, whose step signals the parked readers before the
    // loop waits.  A pass that writes a thousand messages to one ring then walks its
    // member slots once, where waking at each write would walk them a thousand times
    // and send a signal for each.
    void WakeReader(lib_ams::FShm &shm);

    // Owe the writer of SHM a wakeup for the room this reader freed on it, which the
    // pass's wake step sends if the writer is parked when the pass ends.
    void WakeWriter(lib_ams::FShm &shm);

    // Park the reader on SHM: set its sleeping flag, then under a full barrier
    // re-check for a message that raced in after the empty peek -- the writer's
    // zd_wake_Step may already have read sleeping==0 and skipped the SIGRTMIN.  Return
    // true if parked (no data); false if a message is present, in which case the
    // flag is cleared and the caller keeps polling.
    bool ParkReader(lib_ams::FShm &shm);

    // Park the writer on SHM waiting for budget for EXTRA more bytes: set its
    // writer_sleeping flag, then under a full barrier re-sample the budget -- a
    // reader may free it between the store and the load.  SLOT is the channel the
    // write goes on, NULL for the base channel, and its room under its limit is
    // re-checked the same way.  Return true if budget appeared (the caller writes),
    // in which case the flag is cleared; false if parked (a reader's zd_wake_Step
    // signals it), or if the ring is not writable, which parks nothing.  EXTRA is
    // the room test the caller asked HasBudgetQ with, so the re-check agrees with
    // the test that refused.
    bool ParkWriter(lib_ams::FShm &shm, u32 extra = 0, ams::Shmchannel *slot = NULL);

    // Wake the peers every ring on the wake list is owed: the parked readers of a
    // ring this pass wrote, and the parked writer of a ring this pass freed room on.
    // The step runs at the end of the pass, just before the loop waits, and signals
    // each pid once, however many rings it was owed a wakeup on.  A writer's flag is
    // cleared before its signal, so one wake is sent per park; a reader clears its
    // own flag when the signal unparks it.
    //
    // One barrier covers the whole list.  A reader parks by storing its sleeping
    // flag and then, under its own barrier, re-reading woff; the writer that stored
    // woff now loads the flag.  A parked writer stores its flag and re-samples the
    // reader's offset the same way.  Without a barrier between this process's
    // store and its load of the flag, the load may be served first, and the peer
    // sleeps on data or room nobody tells it about.
    //     (user-implemented function, prototype is in amc-generated header)
    // void zd_wake_Step(); // dmmeta.fstep:lib_ams.FDb.zd_wake

    // -------------------------------------------------------------------
    // include/lib_ams.inl.h
    //
    inline u64 AddOffset(u64 offset, int n);
    inline ams::MsgHeader *MsgAtOffset(lib_ams::FShm &shm, u64 offset);
    inline ams::ProcId MakeProcId(ams::Proctype proctype, int node, int index);

    // Inbound shm group for a bridged user process: child writes, parent reads.
    // GRPIDX disambiguates multiple bridges between the same proc pair.
    inline ams::GrpId BridgeInGrp(ams::ProcId child_proc_id, int grpidx);

    // Outbound shm group for a bridged user process: parent writes, child reads.
    // GRPIDX disambiguates multiple bridges between the same proc pair.
    inline ams::GrpId BridgeOutGrp(ams::ProcId child_proc_id, int grpidx);
    inline algo::memptr MsgBytes(ams::MsgHeader &msg);
    inline algo::Alloc GetAlloc(lib_ams::FShm &shm);

    // Like GetAlloc, but the begin hook blocks (busy-waits for a max_msg_size
    // slot) instead of returning NULL when the ring is full -- so a *_FmtShm
    // built on it never drops, it backpressures the writer.
    inline algo::Alloc GetAllocBlock(lib_ams::FShm &shm);

    // An allocator that formats a message straight into BOARD's chunk being
    // filled, so a *_FmtAlloc built on it costs no copy: the begin hook is
    // BoardAlloc and returns NULL when no chunk can take the message, and the end
    // hook trims the reservation to the length the format used.  Nothing is sent
    // by the format; the caller sends the message with BoardSend, or gives the
    // space back with BoardTrim.
    inline algo::Alloc BoardGetAlloc(lib_ams::FShm &board);

    // Like GetAlloc, but a message the ring has no room for is queued on the ring
    // and written by lib_ams's own step as budget appears -- so a *_FmtAlloc built
    // on it neither drops nor blocks, and the caller has nothing to test.  This is
    // what a message with no other retry behind it is written through; see
    // cpp/lib_ams/outmsg.cpp.
    inline algo::Alloc GetAllocQueue(lib_ams::FShm &shm);

    // TRUE when SHM is a message board rather than a lane ring.
    inline bool BoardQ(lib_ams::FShm &shm);

    // Send every wakeup the pass owes now, without waiting for the end of the pass.
    // A caller that is about to wait outside the loop calls it first: WaitBudget
    // before it spins on a peer that may be asleep, ShmClose before the ring's
    // mapping goes, and leaving signaled mode before the lists stop being drained.
    inline void FlushWake();

    // TRUE when this process, a reader of SHM, has at most half the ring left unread.
    inline bool HalfDrainedQ(lib_ams::FShm &shm);

    // Address of CHUNK's first byte in this process's mapping of its board.
    inline u8 *ChunkAddr(lib_ams::FChunk &chunk);

    // Count one hold on CHUNK that no reader releases: a module keeping records
    // that index into the chunk takes one, and the chunk is not reused until
    // ChunkRelease gives it back.
    inline void ChunkHold(lib_ams::FChunk &chunk);

    // The free room at the end of CHUNK: where the next reservation would start, and
    // the bytes left to it.  Nothing is reserved by asking; a caller that reads data
    // into the room reserves what landed with ChunkAlloc, and nothing else may
    // reserve in the chunk in between.
    inline algo::memptr ChunkRoom(lib_ams::FChunk &chunk);

    // Shorten the most recent reservation in CHUNK, at PTR, to LEN bytes, so the
    // next reservation starts right after it; a LEN of zero gives the space back
    // entirely.  Only the last reservation in a chunk can move, and only while no
    // reference to it has been sent.
    inline void ChunkTrim(lib_ams::FChunk &chunk, void *ptr, int len);

    // Board offset of MSG, a message in CHUNK: the coordinate a reference carries,
    // which a reader resolves against its own mapping of the board.
    inline u64 BoardOffset(lib_ams::FChunk &chunk, ams::MsgHeader &msg);

    // The message of LENGTH bytes at board OFFSET on lane SHM's writer's board, or
    // NULL when the coordinates do not describe a message this process can see.
    //
    // The bounds test and the length cross-check are not ceremony: one chunk serves
    // every recipient of the message, so a reference that has gone stale -- through
    // a sender accounting error, or a chunk reused before a reader was done with it
    // -- would hand the same wrong bytes to every reader at once.  A reference that
    // fails either test is refused rather than dispatched.
    inline ams::MsgHeader *BoardResolve(lib_ams::FShm &shm, u64 offset, u32 length);

    // The payload BOARDREF names on lane SHM's writer's board, or NULL when it does
    // not describe a message this process can see; the coordinate form is above.
    inline ams::MsgHeader *BoardResolve(lib_ams::FShm &shm, ams::BoardrefMsg &boardref);

    // True when SPEC names a ring pair, both IN and OUT: the process is a child
    // bridged to a server's namespace, and speaks to it over shared memory.
    inline bool BridgedQ(ams::Procspec &spec);

    // TRUE when the channel of slot SLOT has room for EXTRA more bytes: its count is
    // below its limit by more than EXTRA.  As on a ring, the last message may take
    // the count past the limit, and the one after it is refused.  A NULL SLOT is the
    // base channel, whose room is the ring's alone.
    inline bool SlotRoomQ(ams::Shmchannel *slot, u32 extra);

    // TRUE when CHANNEL's reader has claimed it and set a window, so a writer may
    // count a flow on it.  A reader that never opens the channel leaves it with no
    // room for the life of the ring, and a writer that tests this first keeps such a
    // reader's flow on the ring itself.
    inline bool ChannelClaimedQ(lib_ams::FChannel &channel);

    // Return the bytes the writer may still write on channel slot CHANNEL: the limit
    // its reader set less what the writer has written, and 0 at or past the limit.
    inline u64 ChannelRoom(ams::Shmchannel &channel);

    // Return the bytes the writer may still write on CHANNEL under its limit: the
    // slot's room on a keyed channel, and the ring's budget on the base channel.
    inline u64 ChannelRoom(lib_ams::FChannel &channel);

    // Return the number of member slots in a ring's control header, one per reader
    // the ring may carry.  A board has none.
    inline u32 RingShmemberN();

    // Return the offset of a ring's channel table in its control header: past the
    // header and the member slots.  A segment an older build laid out with other
    // sizes of these records is refused by its magic, and the asserts tie the sizes
    // the magic stands for to the build.
    inline u32 RingChannelStart();

    // Return the bytes of a ring's control header when its creator declares NCHANNEL
    // shm channels: the header, the member slots and the channel table, rounded up
    // to whole pages.  One page is the least, and it holds 62 channels, so a ring
    // declaring no more than that keeps a one-page header.
    inline u32 ShmControlSize(u32 nchannel);

    // Return an allocator that formats a message straight onto CHANNEL: its begin
    // is the channel's BeginWrite and its end the channel's EndWrite, so a generated
    // Msg_FmtAlloc writes on a channel as Msg_FmtShm writes on a ring, and the
    // message counts on the channel.
    inline algo::Alloc GetAlloc(lib_ams::FChannel &channel);
}

#include "include/lib_ams.inl.h"
