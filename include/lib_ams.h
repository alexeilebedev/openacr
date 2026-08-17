// Copyright (C) 2023-2026 AlgoRND
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
}

// The metric subsystem's prototypes live in include/lib_ams_metric.h, so this
// section names the sources it takes prototypes from rather than taking all of
// them.  A source added to lib_ams and left out of this list gets no
// declaration, and its definition then fails -Werror=missing-declarations.
namespace lib_ams { // update-hdr srcfile:"(cpp/lib_ams/(board|bridge|dump|fdin|lib|outmsg|shm|signal)\.cpp|include/lib_ams\.inl\.h)"
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_ams/board.cpp
    //

    // TRUE when SHM is a message board rather than a lane ring.
    bool BoardQ(lib_ams::FShm &shm);

    // The board serving lane SHM's writer, or NULL when that process keeps none.
    // A process has exactly one board, so the lookup is by the writer's proc id and
    // the answer is cached on the lane -- a lane opened before the board existed
    // finds it on a later call.
    lib_ams::FShm *BoardOf(lib_ams::FShm &shm);

    // Stock board BOARD's slot bookkeeping: every slot free and none referenced.
    // The slot count follows from the segment, so a reader that maps a board sized
    // by someone else agrees with its creator without being told.
    void BoardInit(lib_ams::FShm &board);

    // Create this process's message board with NSLOT slots of board_slot_size bytes
    // and open it for writing.  NULL if the segment cannot be made.
    //
    // Choose NSLOT so the board is at least the sum of its readers' pin allowances
    // (board_max_pin each): that is what makes a placement unable to fail, and with
    // fan-out it is generous, since a slot reaching several readers is charged to
    // each allowance but occupies the board once.
    lib_ams::FShm *BoardCreate(u32 nslot);

    // Open process WRITER's message board for reading, so references arriving on
    // that writer's lanes can be resolved.  A reader holds the board before the
    // first reference arrives.
    //
    // Holding it costs a mapping and nothing else: the board takes no member slot,
    // joins no poll list, and is never written by the reader.  There is nothing for
    // a board reader to say -- it advances no position, and the position that
    // matters is the one it already keeps on the lane the reference arrived on.
    lib_ams::FShm *BoardOpen(ams::ProcId writer);

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
    lib_ams::FBoardq *BoardqGetOrCreate(lib_ams::FShm &shm, u32 memberidx);

    // Release the slots reader ROFF of BOARDQ has consumed: every entry at the head
    // whose ring position the reader has passed.  Entries are appended in ring
    // order, so the walk stops at the first one still outstanding.
    void BoardqSweep(lib_ams::FBoardq &boardq, u64 roff);

    // Bring every reader queue of lane SHM up to date, creating a queue for a reader
    // that has none yet.  Run before the lane is asked to take a message, so the
    // room test sees what readers have actually consumed rather than what they held
    // when the last message went out.
    void BoardSweep(lib_ams::FShm &shm);

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
    bool BoardRoomQ(lib_ams::FShm &shm);

    // Add lane SHM to the set the next BoardPostSet reaches.  The set is scratch,
    // rebuilt for every message and emptied by the post.
    void BoardPostLane(lib_ams::FShm &shm);

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
    bool BoardPostSet(ams::MsgHeader &msg);

    // Write MSG to the board serving lane SHM and post a reference to it in SHM's
    // ring, so every reader of the lane receives the message at the cost of one copy.
    // The one-lane case of BoardPostSet, which is where the work is described.
    bool BoardPost(lib_ams::FShm &shm, ams::MsgHeader &msg);

    // The payload BOARDREF names, or NULL when the reference does not describe a
    // message this process can see.
    //
    // The bounds test and the length cross-check are not ceremony: one board slot
    // serves every recipient of the message, so a reference that has gone stale --
    // through a sender accounting error, or a slot reused before a reader was done
    // with it -- would hand the same wrong bytes to every reader at once.  A
    // reference that fails either test is refused rather than dispatched.
    ams::MsgHeader *BoardResolve(lib_ams::FShm &shm, ams::BoardrefMsg &boardref);

    // Release every slot reader MEMBERIDX of lane SHM holds, and forget its queue.
    // Call when the reader is gone: its ring position stops advancing at the moment
    // it dies, so nothing else would ever release what it was holding.
    void BoardRelease(lib_ams::FShm &shm, u32 memberidx);

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

    // Format the value of a `-proc:` argument that initializes a bridged
    // child with CHILD_PROC_ID at GRPIDX.  Format:
    // <child_proc_id>,<prefix>,<in_grp>,<out_grp>[,<nickname>]
    // where the child's perspective is encoded:
    // in_grp  = parent's BridgeOutGrp (parent writes, child reads),
    // out_grp = parent's BridgeInGrp (child writes, parent reads).
    // NICKNAME is the child's human-facing name (the userproc name); the
    // child prefixes its published metrics with it in place of the proc id.
    tempstr ChildProcStr(ams::ProcId child_proc_id, int grpidx, algo::strptr nickname = algo::strptr());

    // -------------------------------------------------------------------
    // cpp/lib_ams/dump.cpp
    //

    // Print table of shms in lib_ams, using a more readable layout
    void DumpShmTableVisual(algo_lib::Regx &regx);

    // Print message MSG to string OUT according to format FMT
    // if FMT.STRIP > 0, strip this many outer "layers" -- a message this tree
    // knows no wrapper for has none to strip and renders in full, except for a
    // log message, which drops to its one-line form.
    // if FMT.FORMAT is bin, the message is printed as pure binary; otherwise, convert to text
    // if FMT.PRETTY, every next layer / payload is printed on a new line with indent
    // for readability.
    // FMT.PAYLOAD_LIM limits maximum printed payload length, allowing to fit one message per
    // screen even if payload is 10MB
    // Finally, if FMT.SHOWLEN is true, message length is included in output.
    void PrintMsg(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out);

    // This function should be called if the ams logcat is enabled
    // It prints the given MSG to ams logcat using pretty format.
    // The shm heartbeat is skipped unless verbose is on -- it arrives once a
    // second per member and says nothing a reader of the trace is looking for.
    void TraceMsg(algo_lib::FLogcat *logcat, lib_ams::FShm &shm, ams::MsgHeader *payload);

    // Convert message MSG to a single-line string carrying every field.
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
    // void cd_fdin_read_Step(); // fstep:lib_ams.FDb.cd_fdin_read

    // Stop reading stdin and drop the stdio-mode loopback shm. Once both are
    // gone, MainLoop has no input source from the stdio path — if the app has
    // no other shm peers either, it will exit naturally. Apps that keep peers
    // alive past stdin EOF (e.g. ams_bridge waiting on shm_in echoes) need not
    // do anything special: their other shms keep the loop running until eof or
    // peer death drops them too.
    // void cd_fdin_eof_Step(); // fstep:lib_ams.FDb.cd_fdin_eof

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

    // Initialize library.  PROC_STR carries everything in one string:
    // <proc_id>                                stdio peer (loopback + stdin
    // reader if MSG_CB is set);
    // no shm namespace owned
    // <proc_id>,<prefix>                       server: PREFIX is the
    // filename prefix for shm
    // files we create
    // <proc_id>,<prefix>,<in_grp>,<out_grp>    bridged child: open the
    // named existing shm pair
    // (PREFIX is informational —
    // it identifies the server
    // that owns the namespace)
    // <proc_id>,<prefix>,<in_grp>,<out_grp>,<nickname>
    // bridged child with a
    // human-facing name; the name
    // prefixes published metrics
    // in place of the proc id
    // Comma is the separator; PROC_ID, GRPIDs, PREFIX, and NICKNAME must not
    // contain commas.  Stale unlocked /dev/shm/*.ams files are cleaned on
    // first call when PREFIX is set.
    bool Init(algo::strptr proc_str, lib_ams::MsgCb msg_cb = NULL);
    void Uninit();

    // Emit message. In shm mode, write to output shm.
    // In stdio mode, print as text.
    void EmitMsg(ams::MsgHeader &msg);

    // Notify lib_ams that process PID has exited with STATUS.
    // Clear the pid in any FProc record matching PID and return that record (if any).
    // For each shm in the database:
    // - If PID was the writer, clear writer_pid. If we are reading from this shm,
    // also set the eof flag on its shmhdr so cd_poll_read_Step can permanently
    // remove it from the poll loop once any remaining messages are drained.
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

    // Write what the queued rings will take, and keep the rest for the next pass.
    // A ring that empties leaves the list; one that is still blocked goes to the
    // back of it, so no ring can starve another.  The list length is sampled at
    // entry, so a ring rotated to the back is not visited twice in one pass.
    //     (user-implemented function, prototype is in amc-generated header)
    // void zd_outshm_Step(); // fstep:lib_ams.FDb.zd_outshm

    // Reserve LEN bytes for a message on SHM and return where to build it, taking
    // the ring itself when it has room and a queued record when it does not.  The
    // answer is never NULL for a length the ring could ever carry, which is what
    // lets a caller format without testing.  EndWriteQueue must follow, and the
    // two communicate through _db.c_cur_outmsg, so a format call may not begin
    // another before it ends.
    void *BeginWriteQueue(lib_ams::FShm &shm, int length);

    // Finish the message BeginWriteQueue started: publish it to the ring when it
    // was built there, and otherwise put it at the back of the ring's queue and
    // arm the step that will write it.
    void EndWriteQueue(lib_ams::FShm &shm, void *ptr, int len);

    // -------------------------------------------------------------------
    // cpp/lib_ams/shm.cpp
    //

    // Free bytes on the tmpfs backing /dev/shm.  INT64_MAX on statvfs
    // failure (treated as "no limit" by callers comparing against a need),
    // so chroots or platforms without /dev/shm don't hard-fail callers.
    i64 GetShmAvail();

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

    // Scan /dev/shm for orphaned ams segments and unlink them.  A segment is an
    // orphan when no process holds its write lock and it is no longer being created
    // (see OrphanSegmentQ) -- i.e. its writer crashed or was kill -9'd without
    // unlinking, or it was created but never claimed by a writer.  Orphans are
    // collected during the walk and unlinked after it, so the unlink never mutates
    // the directory Dir_curs is iterating.
    void CleanOldShmFiles();

    // return TRUE if shared memory region is attached to shm SHM.
    bool ShmFdOpenQ(lib_ams::FShm &shm);

    // Segment size that gives a ring a writable body of at least BODY bytes while
    // carrying messages of up to MAXMSG.  A segment is a 4096-byte control header,
    // a power-of-two body that a message offset wraps inside, and one message of
    // linear overflow past the body, so a message near the top of the body writes
    // straight into the overflow and never straddles the end.
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
    // the slot free list instead.
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
    // whether there is room.  When BLOCK is true and there is none, poll all
    // control shms (zd_ctlin list) for heartbeats indefinitely until a reader
    // drains the ring and only then return -- so a blocking write never drops.
    // A closed ring returns FALSE at once, in blocking mode too: no reader can
    // ever drain it, so waiting would be waiting forever.
    // Return TRUE if writing can proceed.  Public so a caller can wait for room
    // ahead of a zero-copy *_FmtShm.
    bool WaitBudget(lib_ams::FShm &shm, bool block);

    // Begin writing message of length LENGTH, non-blocking -- the hot path for
    // WriteMsg and amc's pnew (acr pnew) zero-copy *_FmtShm.  The budget check is
    // inlined here (rather than calling WaitBudget) so the common case is a
    // straight-line sample with no out-of-line call: if a message already fits,
    // return a pointer to the write region.  On a miss, re-sample once via
    // UpdateBudget; in signaled mode then park as a waiting writer (ParkWriter) so
    // a draining reader wakes us.  Still no room -> bump nnobudget and return NULL.
    // A too-big message, and a ring that is not writable, return NULL without
    // touching the budget counters -- the counters live in the ring's own header,
    // which a closed ring no longer has.
    void *BeginWrite(lib_ams::FShm &shm, int length);

    // Begin writing message of length LENGTH, blocking until the ring has room.
    // WaitBudget busy-waits for a max_msg_size slot; BeginWrite then returns the
    // write pointer (or NULL for a too-big message, which it rejects outright).
    void *BeginWriteBlock(lib_ams::FShm &shm, int length);

    // Finish writing the message of length LEN and publish it: sfence so the
    // payload is visible before the woff store, then re-arm the reader's poll
    // entry.  In signaled mode, wake any reader parked on the ring (WakeReader).
    //
    // c_reader is this process's own slot in the ring's reader table, so a ring
    // with one is a ring this process both writes and reads -- a loopback.  The
    // publish is then its own wakeup and goes through the local wake path: a
    // loopback ring that had parked would otherwise sit on the park list with its
    // sleeping flag raised while it is being polled, and only the next idle
    // recovery pass would put the two lists back in agreement.
    void EndWrite(lib_ams::FShm &shm, void *ptr, int len);

    // Write message MSG to SHM, non-blocking.  Return TRUE on success; FALSE
    // when the ring has no budget (the message is dropped -- the caller decides
    // whether to retry, unread, or discard).
    //
    // A message the ring cannot hold goes to the writer's message board instead,
    // and the ring carries a reference to it.  The board path answers the same way
    // the ring does -- FALSE when it cannot take the message -- so a caller sees one
    // contract whichever way the message travels, and a writer with no board keeps
    // rejecting an oversize message as before.
    bool WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg);

    // Write message MSG to SHM, blocking until the ring has room.  WaitBudget
    // busy-waits for a max_msg_size slot, then WriteMsg performs the write.
    // Always succeeds except for a too-big message, or a ring that is closed and
    // therefore never gains room.  Use only where dropping would be incorrect --
    // correctness, not config, picks this variant.
    bool WriteMsgBlock(lib_ams::FShm &shm, ams::MsgHeader &msg);
    //     (user-implemented function, prototype is in amc-generated header)
    // void shm_file_Cleanup(lib_ams::FShm &shm); // ffunc:lib_ams.FShm.shm_file.Cleanup

    // Close shm: unmap the region, drop the fd, and unlink the file if this
    // process created it.  Clear the sleeping flag if a reader was sleeping.
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
    ams::MsgHeader *PeekMsg(lib_ams::FShm &shm);

    // Called by the client
    // to avoid reading current message
    void StopReading(lib_ams::FShm &shm);

    // Check all shms (that are not already readable) for readability and
    // transfer readable shms to the read heap with correct sort key.
    // In signaled mode, idle shms are removed from the poll loop; the reader
    // sets sleeping=1 on the shmember so that the writer can wake it via kill().
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_poll_read_Step(); // fstep:lib_ams.FDb.cd_poll_read
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

    // Install the graceful stop for SIGTERM and SIGINT, so a signal asking this
    // process to stop means what an ams.TerminateMsg asking it to stop means.
    //
    // The platform already sends that signal expecting to be understood.  A node's
    // shutdown SIGTERMs a userproc group leader as "the polite stop", and the
    // `userproc -del` verb sends the same signal; without a handler both are
    // answered by the default disposition -- immediate death, no drain, no departure
    // record.  A process that dies that way while holding a worker pool's ring, or
    // midway through a store write, leaves its owner waiting on a barrier nobody
    // will ever report.  Installing the handler in Init makes the polite stop
    // actually polite for every process that speaks ams, which is every process
    // here; a process whose stop means something else says so by overriding the
    // handler after Init (ams_bridge, whose pty child dies by SIGHUP to its
    // session).
    void SetupTerminateSignal();

    // Move SHM's parked reader back into the poll loop: clear the sleeping flag its
    // writer reads, take it off the park list, and re-arm it for polling.  Every
    // wake goes through here, so the two lists stay a partition of the open
    // readers.
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
    // the epoll_wait.  Leaving removes the hook, closes the signalfd, and moves
    // every parked reader back into the poll loop; the SIGRTMIN block stays in
    // place for the rest of the process lifetime.
    void SetSignaledMode(bool enable);

    // After publishing data to SHM, wake any reader parked on it.  Full barrier
    // between the woff store (the caller's EndWrite) and the sleeping load: the
    // reader sets sleeping and then re-reads woff under its own mfence, so a
    // store-load fence on both sides is required -- without it each can miss the
    // other and the wakeup is lost.
    void WakeReader(lib_ams::FShm &shm);

    // After draining SHM, wake a writer parked on its budget -- but only once the
    // ring is at least half-drained, so the writer refills about half the ring per
    // wake instead of one message per wake.  Full barrier first: the reader's
    // offset store must be globally visible before this re-read, else the writer's
    // own re-check and this one can both miss.
    void WakeWriter(lib_ams::FShm &shm);

    // Park the reader on SHM: set its sleeping flag, then under a full barrier
    // re-check for a message that raced in after the empty peek -- the writer's
    // EndWrite may already have read sleeping==0 and skipped the SIGRTMIN.  Return
    // true if parked (no data); false if a message is present, in which case the
    // flag is cleared and the caller keeps polling.
    bool ParkReader(lib_ams::FShm &shm);

    // Park the writer on SHM waiting for budget: set its writer_sleeping flag, then
    // under a full barrier re-sample the budget -- a reader may free it between the
    // store and the load.  Return true if budget appeared (the caller writes), in
    // which case the flag is cleared; false if parked (a reader's WakeWriter signals
    // it).
    bool ParkWriter(lib_ams::FShm &shm);

    // About to sleep (the poll loop has drained empty): under a full barrier re-scan
    // every parked reader and re-arm any whose shm now has data.  Catches a writer
    // that published and read sleeping==0 before the reader's park landed -- the
    // lost wakeup the per-shm ParkReader re-check cannot recover for a shm parked in
    // an earlier pass.
    void RecoverWakeup();

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

    // Like GetAlloc, but a message the ring has no room for is queued on the ring
    // and written by lib_ams's own step as budget appears -- so a *_FmtAlloc built
    // on it neither drops nor blocks, and the caller has nothing to test.  This is
    // what a message with no other retry behind it is written through; see
    // cpp/lib_ams/outmsg.cpp.
    inline algo::Alloc GetAllocQueue(lib_ams::FShm &shm);
}

#include "include/lib_ams.inl.h"
