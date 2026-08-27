## lib_ams - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>
<!-- abt_md.toc_end -->

### Description
<a href="#description"></a>
for usage, see [lib_ams - Library for AMS middleware, supporting file format & messaging](/txt/lib/lib_ams/README.md)

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
inline u64 lib_ams::AddOffset(u64 offset, int n)
```

```c++
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
ams::Shmember *lib_ams::AddReadShmember(lib_ams::FShm &shm, ams::ProcId proc_id)
```

```c++
// Begin reading ams control messages from stdin.
//
// Two paths ask for this and either may come first: lib_ams::Init takes it for
// a stdio peer, and an interactive process takes it when it opens its shms.
// Stdin is one descriptor and a descriptor carries one epoll registration, so
// the second caller joins the reader the first one made -- two readers would
// leave one of them subscribed to nothing and its messages unread.
void lib_ams::BeginReadStdin()
```

```c++
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
void *lib_ams::BeginWrite(lib_ams::FShm &shm, int length)
```

```c++
// Begin writing message of length LENGTH, blocking until the ring has room.
// WaitBudget busy-waits for a max_msg_size slot; BeginWrite then returns the
// write pointer (or NULL for a too-big message, which it rejects outright).
void *lib_ams::BeginWriteBlock(lib_ams::FShm &shm, int length)
```

```c++
// Reserve LEN bytes for a message on SHM and return where to build it, taking
// the ring itself when it has room and a queued record when it does not.  The
// answer is never NULL for a length the ring could ever carry, which is what
// lets a caller format without testing.  EndWriteQueue must follow, and the
// two communicate through _db.c_cur_outmsg, so a format call may not begin
// another before it ends.
void *lib_ams::BeginWriteQueue(lib_ams::FShm &shm, int length)
```

```c++
// Create this process's message board with NSLOT slots of board_slot_size bytes
// and open it for writing.  NULL if the segment cannot be made.
//
// Choose NSLOT so the board is at least the sum of its readers' pin allowances
// (board_max_pin each): that is what makes a placement unable to fail, and with
// fan-out it is generous, since a slot reaching several readers is charged to
// each allowance but occupies the board once.
lib_ams::FShm *lib_ams::BoardCreate(u32 nslot)
```

```c++
// Stock board BOARD's slot bookkeeping: every slot free and none referenced.
// The slot count follows from the segment, so a reader that maps a board sized
// by someone else agrees with its creator without being told.
void lib_ams::BoardInit(lib_ams::FShm &board)
```

```c++
// The board serving lane SHM's writer, or NULL when that process keeps none.
// A process has exactly one board, so the lookup is by the writer's proc id and
// the answer is cached on the lane -- a lane opened before the board existed
// finds it on a later call.
lib_ams::FShm *lib_ams::BoardOf(lib_ams::FShm &shm)
```

```c++
// Open process WRITER's message board for reading, so references arriving on
// that writer's lanes can be resolved.  A reader holds the board before the
// first reference arrives.
//
// Holding it costs a mapping and nothing else: the board takes no member slot,
// joins no poll list, and is never written by the reader.  There is nothing for
// a board reader to say -- it advances no position, and the position that
// matters is the one it already keeps on the lane the reference arrived on.
lib_ams::FShm *lib_ams::BoardOpen(ams::ProcId writer)
```

```c++
// Write MSG to the board serving lane SHM and post a reference to it in SHM's
// ring, so every reader of the lane receives the message at the cost of one copy.
// The one-lane case of BoardPostSet, which is where the work is described.
bool lib_ams::BoardPost(lib_ams::FShm &shm, ams::MsgHeader &msg)
```

```c++
// Add lane SHM to the set the next BoardPostSet reaches.  The set is scratch,
// rebuilt for every message and emptied by the post.
void lib_ams::BoardPostLane(lib_ams::FShm &shm)
```

```c++
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
bool lib_ams::BoardPostSet(ams::MsgHeader &msg)
```

```c++
// TRUE when SHM is a message board rather than a lane ring.
bool lib_ams::BoardQ(lib_ams::FShm &shm)
```

```c++
// Release every slot reader MEMBERIDX of lane SHM holds, and forget its queue.
// Call when the reader is gone: its ring position stops advancing at the moment
// it dies, so nothing else would ever release what it was holding.
void lib_ams::BoardRelease(lib_ams::FShm &shm, u32 memberidx)
```

```c++
// The payload BOARDREF names, or NULL when the reference does not describe a
// message this process can see.
//
// The bounds test and the length cross-check are not ceremony: one board slot
// serves every recipient of the message, so a reference that has gone stale --
// through a sender accounting error, or a slot reused before a reader was done
// with it -- would hand the same wrong bytes to every reader at once.  A
// reference that fails either test is refused rather than dispatched.
ams::MsgHeader *lib_ams::BoardResolve(lib_ams::FShm &shm, ams::BoardrefMsg &boardref)
```

```c++
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
bool lib_ams::BoardRoomQ(lib_ams::FShm &shm)
```

```c++
// Bring every reader queue of lane SHM up to date, creating a queue for a reader
// that has none yet.  Run before the lane is asked to take a message, so the
// room test sees what readers have actually consumed rather than what they held
// when the last message went out.
void lib_ams::BoardSweep(lib_ams::FShm &shm)
```

```c++
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
lib_ams::FBoardq *lib_ams::BoardqGetOrCreate(lib_ams::FShm &shm, u32 memberidx)
```

```c++
// Release the slots reader ROFF of BOARDQ has consumed: every entry at the head
// whose ring position the reader has passed.  Entries are appended in ring
// order, so the walk stops at the first one still outstanding.
void lib_ams::BoardqSweep(lib_ams::FBoardq &boardq, u64 roff)
```

```c++
// Inbound shm group for a bridged user process: child writes, parent reads.
// GRPIDX disambiguates multiple bridges between the same proc pair.
inline ams::GrpId lib_ams::BridgeInGrp(ams::ProcId child_proc_id, int grpidx)
```

```c++
// Outbound shm group for a bridged user process: parent writes, child reads.
// GRPIDX disambiguates multiple bridges between the same proc pair.
inline ams::GrpId lib_ams::BridgeOutGrp(ams::ProcId child_proc_id, int grpidx)
```

```c++
// Format the value of a `-proc:` argument that initializes a bridged
// child with CHILD_PROC_ID at GRPIDX.  Format:
// <child_proc_id>,<prefix>,<in_grp>,<out_grp>[,<nickname>]
// where the child's perspective is encoded:
// in_grp  = parent's BridgeOutGrp (parent writes, child reads),
// out_grp = parent's BridgeInGrp (child writes, parent reads).
// NICKNAME is the child's human-facing name (the userproc name); the
// child prefixes its published metrics with it in place of the proc id.
tempstr lib_ams::ChildProcStr(ams::ProcId child_proc_id, int grpidx, algo::strptr nickname = algo::strptr())
```

```c++
// Scan /dev/shm for orphaned ams segments and unlink them.  A segment is an
// orphan when no process holds its write lock and it is no longer being created
// (see OrphanSegmentQ) -- i.e. its writer crashed or was kill -9'd without
// unlinking, or it was created but never claimed by a writer.  Orphans are
// collected during the walk and unlinked after it, so the unlink never mutates
// the directory Dir_curs is iterating.
void lib_ams::CleanOldShmFiles()
```

```c++
void lib_ams::CloseAllShms()
```

```c++
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
bool lib_ams::CreateBridgeShms(ams::ProcId child_proc_id, int grpidx, ams::ProcId reader_proc_id, lib_ams::FShm *&shm_in, lib_ams::FShm *&shm_out, i64 size = 0, i32 maxmsg = 0)
```

```c++
// Print table of shms in lib_ams, using a more readable layout
void lib_ams::DumpShmTableVisual(algo_lib::Regx &regx)
```

```c++
// Emit message. In shm mode, write to output shm.
// In stdio mode, print as text.
void lib_ams::EmitMsg(ams::MsgHeader &msg)
```

```c++
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
void lib_ams::EndWrite(lib_ams::FShm &shm, void *ptr, int len)
```

```c++
// Finish the message BeginWriteQueue started: publish it to the ring when it
// was built there, and otherwise put it at the back of the ring's queue and
// arm the step that will write it.
void lib_ams::EndWriteQueue(lib_ams::FShm &shm, void *ptr, int len)
```

```c++
ams::Shmember *lib_ams::FindReadShmember(lib_ams::FShm &shm, ams::ProcId proc_id)
```

```c++
inline algo::Alloc lib_ams::GetAlloc(lib_ams::FShm &shm)
```

```c++
// Like GetAlloc, but the begin hook blocks (busy-waits for a max_msg_size
// slot) instead of returning NULL when the ring is full -- so a *_FmtShm
// built on it never drops, it backpressures the writer.
inline algo::Alloc lib_ams::GetAllocBlock(lib_ams::FShm &shm)
```

```c++
// Like GetAlloc, but a message the ring has no room for is queued on the ring
// and written by lib_ams's own step as budget appears -- so a *_FmtAlloc built
// on it neither drops nor blocks, and the caller has nothing to test.  This is
// what a message with no other retry behind it is written through; see
// cpp/lib_ams/outmsg.cpp.
inline algo::Alloc lib_ams::GetAllocQueue(lib_ams::FShm &shm)
```

```c++
// Evaluate current budget.  A ring that cannot be written has no budget: zero,
// rather than a read through its released header.
u64 lib_ams::GetBudget(lib_ams::FShm &shm)
```

```c++
// Free bytes on the tmpfs backing /dev/shm.  INT64_MAX on statvfs
// failure (treated as "no limit" by callers comparing against a need),
// so chroots or platforms without /dev/shm don't hard-fail callers.
i64 lib_ams::GetShmAvail()
```

```c++
// Check if thre is room in SHM to write at least 2 messages, plus EXTRA.
// The function re-samples current budget if needed.  A ring that cannot be
// written has no room, and no counter to charge the miss to.
bool lib_ams::HasBudgetQ(lib_ams::FShm &shm, u32 extra = 0)
```

```c++
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
bool lib_ams::Init(algo::strptr proc_str, lib_ams::MsgCb msg_cb = NULL)
```

```c++
inline ams::ProcId lib_ams::MakeProcId(ams::Proctype proctype, int node, int index)
```

```c++
inline ams::MsgHeader *lib_ams::MsgAtOffset(lib_ams::FShm &shm, u64 offset)
```

```c++
inline algo::memptr lib_ams::MsgBytes(ams::MsgHeader &msg)
```

```c++
// Find the lowest slot N for a new bridged process of PROCTYPE on NODEIDX.
// The slot is used as both the process index in the new ProcId and the
// grpidx of the bridge shms.  Skipped: the caller's own slot (avoids
// colliding with our own proc_id when proctype/nodeidx happen to match)
// and any slot whose shm pair already exists locally (avoids re-using a
// slot owned by a still-live bridge).
int lib_ams::NextBridgeSlot(ams::Proctype proctype, int nodeidx)
```

```c++
// Park the reader on SHM: set its sleeping flag, then under a full barrier
// re-check for a message that raced in after the empty peek -- the writer's
// EndWrite may already have read sleeping==0 and skipped the SIGRTMIN.  Return
// true if parked (no data); false if a message is present, in which case the
// flag is cleared and the caller keeps polling.
bool lib_ams::ParkReader(lib_ams::FShm &shm)
```

```c++
// Park the writer on SHM waiting for budget: set its writer_sleeping flag, then
// under a full barrier re-sample the budget -- a reader may free it between the
// store and the load.  Return true if budget appeared (the caller writes), in
// which case the flag is cleared; false if parked (a reader's WakeWriter signals
// it).
bool lib_ams::ParkWriter(lib_ams::FShm &shm)
```

```c++
// If the shm is open for reading, check to see if a message
// is available. If it is available, return pointer to message.
ams::MsgHeader *lib_ams::PeekMsg(lib_ams::FShm &shm)
```

```c++
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
void lib_ams::PrintMsg(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out)
```

```c++
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
lib_ams::FProc *lib_ams::ProcExit(int pid, int status)
```

```c++
// About to sleep (the poll loop has drained empty): under a full barrier re-scan
// every parked reader and re-arm any whose shm now has data.  Catches a writer
// that published and read sleeping==0 before the reader's park landed -- the
// lost wakeup the per-shm ParkReader re-check cannot recover for a shm parked in
// an earlier pass.
void lib_ams::RecoverWakeup()
```

```c++
// Set the segment size a ring this process creates gets by default: a body of
// at least SIZE bytes for messages up to the process ceiling.
void lib_ams::SetDfltShmSize(u32 size)
```

```c++
// Enter or leave signaled mode.  Entering blocks SIGRTMIN and arms an
// always-armed signalfd registered with the iohook, so a peer's SIGRTMIN wakes
// the epoll_wait.  Leaving removes the hook, closes the signalfd, and moves
// every parked reader back into the poll loop; the SIGRTMIN block stays in
// place for the rest of the process lifetime.
void lib_ams::SetSignaledMode(bool enable)
```

```c++
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
void lib_ams::SetupTerminateSignal()
```

```c++
// Close shm: unmap the region, drop the fd, and unlink the file if this
// process created it.  Clear the sleeping flag if a reader was sleeping.
// The record itself stays in the shm table so the next incarnation under the
// same grp reuses it.
void lib_ams::ShmClose(lib_ams::FShm &shm)
```

```c++
// Create (or open) shared memory for reading/writing (as specified in FLAGS)
// and return success status
bool lib_ams::ShmCreate(lib_ams::FShm &shm, ams::ShmFlags flags)
```

```c++
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
i64 lib_ams::ShmExistingSize(ams::GrpId grp_id)
```

```c++
// return TRUE if shared memory region is attached to shm SHM.
bool lib_ams::ShmFdOpenQ(lib_ams::FShm &shm)
```

```c++
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
bool lib_ams::ShmOpen(lib_ams::FShm &shm, ams::ShmFlags flags)
```

```c++
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
u32 lib_ams::ShmSize(u32 body, u32 maxmsg)
```

```c++
// Drain the signalfd (coalesced SIGRTMIN wakeups read as one event) and move
// every parked reader back into the poll loop.
void lib_ams::SignalReadStep()
```

```c++
// Called by the client
// to avoid reading current message
void lib_ams::StopReading(lib_ams::FShm &shm)
```

```c++
// This process was asked to stop.  One definition, reached by every spelling of
// the request: an inbound ams.TerminateMsg addressed to this proc, the stdin EOF
// that a parent's exit closes, and the SIGTERM or SIGINT a stop sends.
//
// What stopping means depends on the role, and h_terminate is where a role says
// so.  The default is the only thing a process can do about its own stop -- end
// its main loop -- and it is what every process wants except a supervisor, whose
// stop is the orderly shutdown of the node it runs: its own exit is the last
// step of that, not the first.
void lib_ams::Terminate()
```

```c++
// Convert message MSG to string in a way suitable for debugging
// (some information is lost in exchange for readability)
tempstr lib_ams::ToDbgString(ams::MsgHeader &msg)
```

```c++
// Convert message MSG to a single-line string carrying every field.
tempstr lib_ams::ToString(ams::MsgHeader &msg)
```

```c++
// This function should be called if the ams logcat is enabled
// It prints the given MSG to ams logcat using pretty format.
// The shm heartbeat is skipped unless verbose is on -- it arrives once a
// second per member and says nothing a reader of the trace is looking for.
void lib_ams::TraceMsg(algo_lib::FLogcat *logcat, lib_ams::FShm &shm, ams::MsgHeader *payload)
```

```c++
void lib_ams::Uninit()
```

```c++
// Move SHM's parked reader back into the poll loop: clear the sleeping flag its
// writer reads, take it off the park list, and re-arm it for polling.  Every
// wake goes through here, so the two lists stay a partition of the open
// readers.
void lib_ams::UnparkReader(lib_ams::FShm &shm)
```

```c++
// Wake every parked reader.  A SIGRTMIN names no stream -- the signal says only
// that some peer freed something -- and leaving signaled mode ends parking
// altogether, so both hand the whole parked set back to the poll loop and let
// the next cd_poll_read_Step re-check each for data.
void lib_ams::UnparkReaderSet()
```

```c++
void lib_ams::UnreadMsg()
```

```c++
// Update budget for SHM
// Return TRUE if the WRITELIMIT was updated.
// (WRITELIMIT is the point beyond which no message can be written
// because doing so would overwrite data not yet consumed by one of the read members.)
// A board has no write budget to update.  It is not a ring, so it has no
// writelimit and no member offsets to derive one from; its space is tracked by
// the slot free list instead.
bool lib_ams::UpdateBudget(lib_ams::FShm &shm)
```

```c++
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
bool lib_ams::WaitBudget(lib_ams::FShm &shm, bool block)
```

```c++
// After publishing data to SHM, wake any reader parked on it.  Full barrier
// between the woff store (the caller's EndWrite) and the sleeping load: the
// reader sets sleeping and then re-reads woff under its own mfence, so a
// store-load fence on both sides is required -- without it each can miss the
// other and the wakeup is lost.
void lib_ams::WakeReader(lib_ams::FShm &shm)
```

```c++
// After draining SHM, wake a writer parked on its budget -- but only once the
// ring is at least half-drained, so the writer refills about half the ring per
// wake instead of one message per wake.  Full barrier first: the reader's
// offset store must be globally visible before this re-read, else the writer's
// own re-check and this one can both miss.
void lib_ams::WakeWriter(lib_ams::FShm &shm)
```

```c++
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
bool lib_ams::WritableQ(lib_ams::FShm &shm)
```

```c++
// Write message MSG to SHM, non-blocking.  Return TRUE on success; FALSE
// when the ring has no budget (the message is dropped -- the caller decides
// whether to retry, unread, or discard).
//
// A message the ring cannot hold goes to the writer's message board instead,
// and the ring carries a reference to it.  The board path answers the same way
// the ring does -- FALSE when it cannot take the message -- so a caller sees one
// contract whichever way the message travels, and a writer with no board keeps
// rejecting an oversize message as before.
bool lib_ams::WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg)
```

```c++
// Write message MSG to SHM, blocking until the ring has room.  WaitBudget
// busy-waits for a max_msg_size slot, then WriteMsg performs the write.
// Always succeeds except for a too-big message, or a ring that is closed and
// therefore never gains room.  Use only where dropping would be incorrect --
// correctness, not config, picks this variant.
bool lib_ams::WriteMsgBlock(lib_ams::FShm &shm, ams::MsgHeader &msg)
```

```c++
// Stop reading stdin and drop the stdio-mode loopback shm. Once both are
// gone, MainLoop has no input source from the stdio path — if the app has
// no other shm peers either, it will exit naturally. Apps that keep peers
// alive past stdin EOF (e.g. ams_bridge waiting on shm_in echoes) need not
// do anything special: their other shms keep the loop running until eof or
// peer death drops them too.
void lib_ams::cd_fdin_eof_Step()
```

```c++
// Read next input line from stdin, parse as ams message, and write to target shm.
// If the target shm is full, stop reading (backpressure).
// If the shm where we are posting the message is full (won't accept the message)
// then reading of fdin is stopped and will resume after the shm has room.
// If there is nowhere to post the message because no target shm is found, the counter
// trace.n_fdin_drop_notgt is incremented and a message is printed in verbose mode.
void lib_ams::cd_fdin_read_Step()
```

```c++
// Check all shms (that are not already readable) for readability and
// transfer readable shms to the read heap with correct sort key.
// In signaled mode, idle shms are removed from the poll loop; the reader
// sets sleeping=1 on the shmember so that the writer can wake it via kill().
void lib_ams::cd_poll_read_Step()
```

```c++
ams::Shmember& lib_ams::shm_c_shmember_curs_Access(shm_c_shmember_curs &curs)
```

```c++
void lib_ams::shm_c_shmember_curs_Next(shm_c_shmember_curs &curs)
```

```c++
void lib_ams::shm_c_shmember_curs_Reset(shm_c_shmember_curs &curs, lib_ams::FShm &parent)
```

```c++
bool lib_ams::shm_c_shmember_curs_ValidQ(shm_c_shmember_curs &curs)
```

```c++
void lib_ams::shm_file_Cleanup(lib_ams::FShm &shm)
```

```c++
ams::Shmember *lib_ams::shmember_Find(lib_ams::FShm &shm, int i)
```

```c++
// Write what the queued rings will take, and keep the rest for the next pass.
// A ring that empties leaves the list; one that is still blocked goes to the
// back of it, so no ring can starve another.  The list length is sampled at
// entry, so a ring rotated to the back is not visited twice in one pass.
void lib_ams::zd_outshm_Step()
```

### Inputs
<a href="#inputs"></a>
`lib_ams` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_ams_gen.cpp](/cpp/gen/lib_ams_gen.cpp)||
|[cpp/lib_ams/board.cpp](/cpp/lib_ams/board.cpp)||
|[cpp/lib_ams/bridge.cpp](/cpp/lib_ams/bridge.cpp)||
|[cpp/lib_ams/dump.cpp](/cpp/lib_ams/dump.cpp)||
|[cpp/lib_ams/fdin.cpp](/cpp/lib_ams/fdin.cpp)||
|[cpp/lib_ams/lib.cpp](/cpp/lib_ams/lib.cpp)||
|[cpp/lib_ams/outmsg.cpp](/cpp/lib_ams/outmsg.cpp)||
|[cpp/lib_ams/shm.cpp](/cpp/lib_ams/shm.cpp)||
|[cpp/lib_ams/signal.cpp](/cpp/lib_ams/signal.cpp)||
|[include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)||
|[include/gen/lib_ams_gen.inl.h](/include/gen/lib_ams_gen.inl.h)||
|[include/lib_ams.h](/include/lib_ams.h)||
|[include/lib_ams.inl.h](/include/lib_ams.inl.h)||

### Dependencies
<a href="#dependencies"></a>
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

### In Memory DB
<a href="#in-memory-db"></a>
`lib_ams` generated code creates the tables below.
All allocations are done through global `lib_ams::_db` [lib_ams.FDb](#lib_ams-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_ams.Boardent](#lib_ams-boardent)||FBoardq.boardent (Tary)|
|[lib_ams.FBoardq](#lib_ams-fboardq)||FDb.boardq (Lary)|boardq (Lary, by rowid)|
||||FShm.c_boardq (Ptrary)|
|[lib_ams.FDb](#lib_ams-fdb)||FDb._db (Global)|
|[lib_ams.FFdin](#lib_ams-ffdin)||FDb.fdin (Tpool)|cd_fdin_eof (Llist)|cd_fdin_read (Llist)|zd_fdin (Llist)|c_fdin_stdin (Ptr)|
|[lib_ams.FGrptype](#lib_ams-fgrptype)|[amsdb.grptype](/txt/ssimdb/amsdb/grptype.md)|FDb.grptype (Lary)|**static**|grptype (Lary, by rowid)|ind_grptype (Thash, hash field id)|
||||FShm.p_grptype (Upptr)|
|[lib_ams.FOutmsg](#lib_ams-foutmsg)||FDb.outmsg (Tpool)|c_cur_outmsg (Ptr)|
||||FShm.zd_outmsg (Llist)|
|[lib_ams.FProc](#lib_ams-fproc)||FDb.proc (Tpool)|ind_proc (Thash, hash field proc_id)|zd_proc (Llist)|c_thisproc (Ptr)|
|[lib_ams.FProctype](#lib_ams-fproctype)|[amsdb.proctype](/txt/ssimdb/amsdb/proctype.md)|FDb.proctype (Lary)|**static**|proctype (Lary, by rowid)|ind_proctype (Thash, hash field proctype)|
|[lib_ams.FShm](#lib_ams-fshm)||FDb.shm (Lary)|shm (Lary, by rowid)|ind_shm (Thash, hash field grp_id)|cd_poll_read (Llist)|c_cur_shm (Ptr)|c_shm_stdout (Ptr)|c_shm_stdin (Ptr)|c_loopback_shm (Ptr)|zd_park_read (Llist)|c_postlane (Ptrary)|zd_outshm (Llist)|
||||FBoardq.p_shm (Upptr)|
||||FOutmsg.p_shm (Upptr)|
||||FProc.c_shm (Ptrary)|
||||FShm.p_board (Upptr)|
|[lib_ams.MsgFmt](#lib_ams-msgfmt)||
||||MsgFmt.h_convert (Hook)|

#### lib_ams.Boardent - One outstanding board reference posted to one reader
<a href="#lib_ams-boardent"></a>

#### lib_ams.Boardent Fields
<a href="#lib_ams-boardent-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.Boardent.ring_pos|u64|[Val](/txt/exe/amc/reftype/Val.md)||Lane ring position the reader must pass to release the slot|
|lib_ams.Boardent.slot|u32|[Val](/txt/exe/amc/reftype/Val.md)||Board slot index holding the payload|

#### Struct Boardent
<a href="#struct-boardent"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct Boardent { // lib_ams.Boardent: One outstanding board reference posted to one reader
    u64   ring_pos;   //   0  Lane ring position the reader must pass to release the slot
    u32   slot;       //   0  Board slot index holding the payload
    // func:lib_ams.Boardent..Ctor
    inline               Boardent() __attribute__((nothrow));
};
```

#### lib_ams.FBoardq - Board references outstanding to one reader of one lane
<a href="#lib_ams-fboardq"></a>

#### lib_ams.FBoardq Fields
<a href="#lib_ams-fboardq-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FBoardq.memberidx|u32|[Val](/txt/exe/amc/reftype/Val.md)||Index of this reader in the lane segment member table|
|lib_ams.FBoardq.head|u64|[Val](/txt/exe/amc/reftype/Val.md)||Absolute position of the oldest entry not yet released|
|lib_ams.FBoardq.tail|u64|[Val](/txt/exe/amc/reftype/Val.md)||Absolute position one past the newest entry|
|lib_ams.FBoardq.p_shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Upptr](/txt/exe/amc/reftype/Upptr.md)||Lane whose reader this queue tracks|
|lib_ams.FBoardq.boardent|[lib_ams.Boardent](/txt/gen/lib_ams/lib_ams.md#lib_ams-boardent)|[Tary](/txt/exe/amc/reftype/Tary.md)||Fixed power-of-two ring of outstanding references|

#### Struct FBoardq
<a href="#struct-fboardq"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FBoardq { // lib_ams.FBoardq: Board references outstanding to one reader of one lane
    u32                  memberidx;             //   0  Index of this reader in the lane segment member table
    u64                  head;                  //   0  Absolute position of the oldest entry not yet released
    u64                  tail;                  //   0  Absolute position one past the newest entry
    lib_ams::FShm*       p_shm;                 // reference to parent row
    lib_ams::Boardent*   boardent_elems;        // pointer to elements
    u64                  boardent_n;            // number of elements in array
    u64                  boardent_max;          // max. capacity of array before realloc
    bool                 shm_c_boardq_in_ary;   //   false  membership flag
    // func:lib_ams.FBoardq..AssignOp
    lib_ams::FBoardq&    operator =(const lib_ams::FBoardq &rhs) = delete;
    // func:lib_ams.FBoardq..CopyCtor
    FBoardq(const lib_ams::FBoardq &rhs) = delete;
private:
    // func:lib_ams.FBoardq..Ctor
    inline               FBoardq() __attribute__((nothrow));
    // func:lib_ams.FBoardq..Dtor
    inline               ~FBoardq() __attribute__((nothrow));
    friend lib_ams::FBoardq&    boardq_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FBoardq*    boardq_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 boardq_RemoveAll() __attribute__((nothrow));
    friend void                 boardq_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FDb - In-memory database for lib_ams
<a href="#lib_ams-fdb"></a>

#### lib_ams.FDb Fields
<a href="#lib_ams-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FDb.fdin|[lib_ams.FFdin](/txt/gen/lib_ams/lib_ams.md#lib_ams-ffdin)|[Tpool](/txt/exe/amc/reftype/Tpool.md)||ams control messages on stdin|
|lib_ams.FDb.cd_fdin_eof|[lib_ams.FFdin](/txt/gen/lib_ams/lib_ams.md#lib_ams-ffdin)|[Llist](/txt/exe/amc/reftype/Llist.md)||End reading from stdin|
|lib_ams.FDb.cd_fdin_read|[lib_ams.FFdin](/txt/gen/lib_ams/lib_ams.md#lib_ams-ffdin)|[Llist](/txt/exe/amc/reftype/Llist.md)||Read next message from file descriptor|
|lib_ams.FDb._db|[lib_ams.FDb](/txt/gen/lib_ams/lib_ams.md#lib_ams-fdb)|[Global](/txt/exe/amc/reftype/Global.md)|||
|lib_ams.FDb.shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Lary](/txt/exe/amc/reftype/Lary.md)||Table of streams|
|lib_ams.FDb.ind_shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Thash](/txt/exe/amc/reftype/Thash.md)||Index of streams by stream id|
|lib_ams.FDb.proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftype/Val.md)||Process id, e.g. amstest-0|
|lib_ams.FDb.shmem_size|i32|[Val](/txt/exe/amc/reftype/Val.md)|32768|Default stream shared memory size|
|lib_ams.FDb.max_msg_size|i32|[Val](/txt/exe/amc/reftype/Val.md)|4096|Frame size: largest message this process builds for itself and carries whole|
|lib_ams.FDb.cd_poll_read|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Llist](/txt/exe/amc/reftype/Llist.md)||Hot poll input stream for reading|
|lib_ams.FDb.file_prefix|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|""|File prefix for all streams|
|lib_ams.FDb.proc|[lib_ams.FProc](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproc)|[Tpool](/txt/exe/amc/reftype/Tpool.md)|||
|lib_ams.FDb.ind_proc|[lib_ams.FProc](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproc)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|lib_ams.FDb.grptype|[lib_ams.FGrptype](/txt/gen/lib_ams/lib_ams.md#lib_ams-fgrptype)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|lib_ams.FDb.ind_grptype|[lib_ams.FGrptype](/txt/gen/lib_ams/lib_ams.md#lib_ams-fgrptype)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|lib_ams.FDb.shm_files_cleaned|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FDb.fdin_buf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FDb.zd_proc|[lib_ams.FProc](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproc)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|lib_ams.FDb.expect_buf|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Buffer containing outputs produced since last input|
|lib_ams.FDb.expect_str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||If non-empty, pause reading inputs until this string is non-empty|
|lib_ams.FDb.expect_pos|i32|[Val](/txt/exe/amc/reftype/Val.md)||Match position within expect buf|
|lib_ams.FDb.expect_timeout|double|[Val](/txt/exe/amc/reftype/Val.md)|10.0|Default expect timeout|
|lib_ams.FDb.fmt_buf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FDb.c_thisproc|[lib_ams.FProc](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproc)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||
|lib_ams.FDb.c_cur_shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||
|lib_ams.FDb.signaled|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Signaled mode enabled|
|lib_ams.FDb.signal_fd|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftype/Val.md)||signalfd file descriptor|
|lib_ams.FDb.signal_iohook|[algo_lib.FIohook](/txt/lib/algo_lib/FIohook.md)|[Val](/txt/exe/amc/reftype/Val.md)||Iohook for signalfd, registered with epoll|
|lib_ams.FDb.io_print|u64|[Val](/txt/exe/amc/reftype/Val.md)||IO print callback (lib_ams::PrintCb cast to u64)|
|lib_ams.FDb.c_shm_stdout|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Output shm for IO pub (set by InitIO in shm mode)|
|lib_ams.FDb.c_shm_stdin|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Loopback input shm for IO (stdio mode)|
|lib_ams.FDb.h_amsmsg|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Hook](/txt/exe/amc/reftype/Hook.md)||Default message callback|
|lib_ams.FDb.c_loopback_shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||In-process shm carrying messages parsed from stdin in stdio mode|
|lib_ams.FDb.zd_fdin|[lib_ams.FFdin](/txt/gen/lib_ams/lib_ams.md#lib_ams-ffdin)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|lib_ams.FDb.exit_on_fdin_eof|bool|[Val](/txt/exe/amc/reftype/Val.md)||force process exit when stdin (FFdin) reaches EOF; set by procs spawned over a parent-held pipe|
|lib_ams.FDb.nickname|[algo.Smallstr30](/txt/protocol/algo/README.md#algo-smallstr30)|[Val](/txt/exe/amc/reftype/Val.md)|""|Human-facing name for this process (userproc name); prefixes published metrics when set|
|lib_ams.FDb.proctype|[lib_ams.FProctype](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproctype)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|lib_ams.FDb.ind_proctype|[lib_ams.FProctype](/txt/gen/lib_ams/lib_ams.md#lib_ams-fproctype)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|lib_ams.FDb.zd_park_read|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Llist](/txt/exe/amc/reftype/Llist.md)||Streams whose reader is parked awaiting a wakeup|
|lib_ams.FDb.h_terminate||[Hook](/txt/exe/amc/reftype/Hook.md)||What a stop request means for this process; default exits the main loop, a supervisor shuts its node down|
|lib_ams.FDb.c_fdin_stdin|[lib_ams.FFdin](/txt/gen/lib_ams/lib_ams.md#lib_ams-ffdin)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||The reader of stdin, when stdin is being read|
|lib_ams.FDb.boardq|[lib_ams.FBoardq](/txt/gen/lib_ams/lib_ams.md#lib_ams-fboardq)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|lib_ams.FDb.board_slot_size|i32|[Val](/txt/exe/amc/reftype/Val.md)|1048576|Slot size of a board this process creates; the largest message it carries|
|lib_ams.FDb.board_max_pin|i64|[Val](/txt/exe/amc/reftype/Val.md)|8388608|Board bytes one reader may pin before the sender withholds from it|
|lib_ams.FDb.c_postlane|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)||Scratch: lanes the next board post reaches|
|lib_ams.FDb.outmsg|[lib_ams.FOutmsg](/txt/gen/lib_ams/lib_ams.md#lib_ams-foutmsg)|[Tpool](/txt/exe/amc/reftype/Tpool.md)||Pool of messages queued for a ring|
|lib_ams.FDb.zd_outshm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Llist](/txt/exe/amc/reftype/Llist.md)||Rings holding a queued message; the step writes them as budget appears|
|lib_ams.FDb.c_cur_outmsg|[lib_ams.FOutmsg](/txt/gen/lib_ams/lib_ams.md#lib_ams-foutmsg)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Scratch: the queued message the current format call is building|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FDb { // lib_ams.FDb: In-memory database for lib_ams
    u64                             fdin_blocksize;               // # bytes per block
    lib_ams::FFdin*                 fdin_free;                    //
    lib_ams::FFdin*                 cd_fdin_eof_head;             // zero-terminated doubly linked list
    i32                             cd_fdin_eof_n;                // zero-terminated doubly linked list
    lib_ams::FFdin*                 cd_fdin_read_head;            // zero-terminated doubly linked list
    i32                             cd_fdin_read_n;               // zero-terminated doubly linked list
    lib_ams::FShm*                  shm_lary[36];                 // level array
    i64                             shm_n;                        // number of elements in array
    lib_ams::FShm**                 ind_shm_buckets_elems;        // pointer to bucket array
    i32                             ind_shm_buckets_n;            // number of elements in bucket array
    i32                             ind_shm_n;                    // number of elements in the hash table
    ams::ProcId                     proc_id;                      // Process id, e.g. amstest-0
    i32                             shmem_size;                   //   32768  Default stream shared memory size
    i32                             max_msg_size;                 //   4096  Frame size: largest message this process builds for itself and carries whole
    lib_ams::FShm*                  cd_poll_read_head;            // zero-terminated doubly linked list
    i32                             cd_poll_read_n;               // zero-terminated doubly linked list
    algo::cstring                   file_prefix;                  //   ""  File prefix for all streams
    u64                             proc_blocksize;               // # bytes per block
    lib_ams::FProc*                 proc_free;                    //
    lib_ams::FProc**                ind_proc_buckets_elems;       // pointer to bucket array
    i32                             ind_proc_buckets_n;           // number of elements in bucket array
    i32                             ind_proc_n;                   // number of elements in the hash table
    lib_ams::FGrptype*              grptype_lary[36];             // level array
    i64                             grptype_n;                    // number of elements in array
    lib_ams::FGrptype**             ind_grptype_buckets_elems;    // pointer to bucket array
    i32                             ind_grptype_buckets_n;        // number of elements in bucket array
    i32                             ind_grptype_n;                // number of elements in the hash table
    bool                            shm_files_cleaned;            //   false
    algo::ByteAry                   fdin_buf;                     //
    lib_ams::FProc*                 zd_proc_head;                 // zero-terminated doubly linked list
    i32                             zd_proc_n;                    // zero-terminated doubly linked list
    lib_ams::FProc*                 zd_proc_tail;                 // pointer to last element
    algo::cstring                   expect_buf;                   // Buffer containing outputs produced since last input
    algo::cstring                   expect_str;                   // If non-empty, pause reading inputs until this string is non-empty
    i32                             expect_pos;                   //   0  Match position within expect buf
    double                          expect_timeout;               //   10.0  Default expect timeout
    algo::ByteAry                   fmt_buf;                      //
    lib_ams::FProc*                 c_thisproc;                   // optional pointer
    lib_ams::FShm*                  c_cur_shm;                    // optional pointer
    bool                            signaled;                     //   false  Signaled mode enabled
    algo::Fildes                    signal_fd;                    // signalfd file descriptor
    algo_lib::FIohook               signal_iohook;                // Iohook for signalfd, registered with epoll
    u64                             io_print;                     //   0  IO print callback (lib_ams::PrintCb cast to u64)
    lib_ams::FShm*                  c_shm_stdout;                 // Output shm for IO pub (set by InitIO in shm mode). optional pointer
    lib_ams::FShm*                  c_shm_stdin;                  // Loopback input shm for IO (stdio mode). optional pointer
    lib_ams::_db_h_amsmsg_hook      h_amsmsg;                     //   NULL  Pointer to a function
    u64                             h_amsmsg_ctx;                 //   0  Callback context
    lib_ams::FShm*                  c_loopback_shm;               // In-process shm carrying messages parsed from stdin in stdio mode. optional pointer
    lib_ams::FFdin*                 zd_fdin_head;                 // zero-terminated doubly linked list
    i32                             zd_fdin_n;                    // zero-terminated doubly linked list
    lib_ams::FFdin*                 zd_fdin_tail;                 // pointer to last element
    bool                            exit_on_fdin_eof;             //   false  force process exit when stdin (FFdin) reaches EOF; set by procs spawned over a parent-held pipe
    algo::Smallstr30                nickname;                     //   ""  Human-facing name for this process (userproc name); prefixes published metrics when set
    lib_ams::FProctype*             proctype_lary[36];            // level array
    i64                             proctype_n;                   // number of elements in array
    lib_ams::FProctype**            ind_proctype_buckets_elems;   // pointer to bucket array
    i32                             ind_proctype_buckets_n;       // number of elements in bucket array
    i32                             ind_proctype_n;               // number of elements in the hash table
    lib_ams::FShm*                  zd_park_read_head;            // zero-terminated doubly linked list
    i32                             zd_park_read_n;               // zero-terminated doubly linked list
    lib_ams::FShm*                  zd_park_read_tail;            // pointer to last element
    lib_ams::_db_h_terminate_hook   h_terminate;                  //   NULL  Pointer to a function
    u64                             h_terminate_ctx;              //   0  Callback context
    lib_ams::FFdin*                 c_fdin_stdin;                 // The reader of stdin, when stdin is being read. optional pointer
    lib_ams::FBoardq*               boardq_lary[36];              // level array
    i64                             boardq_n;                     // number of elements in array
    i32                             board_slot_size;              //   1048576  Slot size of a board this process creates; the largest message it carries
    i64                             board_max_pin;                //   8388608  Board bytes one reader may pin before the sender withholds from it
    lib_ams::FShm**                 c_postlane_elems;             // array of pointers
    u64                             c_postlane_n;                 // current size
    u64                             c_postlane_max;               // capacity of allocated array
    u64                             outmsg_blocksize;             // # bytes per block
    lib_ams::FOutmsg*               outmsg_free;                  //
    lib_ams::FShm*                  zd_outshm_head;               // zero-terminated doubly linked list
    i32                             zd_outshm_n;                  // zero-terminated doubly linked list
    lib_ams::FShm*                  zd_outshm_tail;               // pointer to last element
    lib_ams::FOutmsg*               c_cur_outmsg;                 // Scratch: the queued message the current format call is building. optional pointer
    lib_ams::trace                  trace;                        //
};
```

#### lib_ams.FFdin - FD input (normally stdin)
<a href="#lib_ams-ffdin"></a>

#### lib_ams.FFdin Fields
<a href="#lib_ams-ffdin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FFdin.iohook|[algo_lib.FIohook](/txt/lib/algo_lib/FIohook.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FFdin.in|char|[Fbuf](/txt/exe/amc/reftype/Fbuf.md)|'\n'||

#### Struct FFdin
<a href="#struct-ffdin"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FFdin { // lib_ams.FFdin: FD input (normally stdin)
    lib_ams::FFdin*     fdin_next;           // Pointer to next free element int tpool
    lib_ams::FFdin*     cd_fdin_eof_next;    // zslist link; -1 means not-in-list
    lib_ams::FFdin*     cd_fdin_eof_prev;    // previous element
    lib_ams::FFdin*     cd_fdin_read_next;   // zslist link; -1 means not-in-list
    lib_ams::FFdin*     cd_fdin_read_prev;   // previous element
    lib_ams::FFdin*     zd_fdin_next;        // zslist link; -1 means not-in-list
    lib_ams::FFdin*     zd_fdin_prev;        // previous element
    algo_lib::FIohook   iohook;              //
    u8*                 in_elems;            //   NULL  pointer to elements of indirect array
    u32                 in_max;              //   0  current length of allocated array
    i32                 in_start;            // beginning of valid bytes (in bytes)
    i32                 in_end;              // end of valid bytes (in bytes)
    i32                 in_msglen;           // current message length
    algo::Errcode       in_err;              // system error code
    algo_lib::FIohook   in_iohook;           // edge-triggered hook for the buffer
    bool                in_eof;              // no more data will be written to buffer
    bool                in_msgvalid;         // current message is valid
    bool                in_epoll_enable;     // use epoll?
    // value field lib_ams.FFdin.iohook is not copiable
    // field lib_ams.FFdin.in prevents copy
    // func:lib_ams.FFdin..AssignOp
    inline lib_ams::FFdin& operator =(const lib_ams::FFdin &rhs) = delete;
    // value field lib_ams.FFdin.iohook is not copiable
    // field lib_ams.FFdin.in prevents copy
    // func:lib_ams.FFdin..CopyCtor
    inline               FFdin(const lib_ams::FFdin &rhs) = delete;
private:
    // func:lib_ams.FFdin..Ctor
    inline               FFdin() __attribute__((nothrow));
    // func:lib_ams.FFdin..Dtor
    inline               ~FFdin() __attribute__((nothrow));
    friend lib_ams::FFdin&      fdin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FFdin*      fdin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fdin_Delete(lib_ams::FFdin &row) __attribute__((nothrow));
};
```

#### lib_ams.FGrptype
<a href="#lib_ams-fgrptype"></a>

#### lib_ams.FGrptype Fields
<a href="#lib_ams-fgrptype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FGrptype.base|[amsdb.Grptype](/txt/ssimdb/amsdb/grptype.md)|[Base](/txt/ssimdb/amsdb/grptype.md)|||

#### Struct FGrptype
<a href="#struct-fgrptype"></a>
*Note:* field ``lib_ams.FGrptype.base`` has reftype ``base`` so the fields of [amsdb.Grptype](/txt/ssimdb/amsdb/grptype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FGrptype { // lib_ams.FGrptype
    lib_ams::FGrptype*   ind_grptype_next;      // hash next
    u32                  ind_grptype_hashval;   // hash value
    algo::Smallstr50     grptype;               //
    ams::Grptype         id;                    //
    algo::cstring        comment;               //
    // func:lib_ams.FGrptype..AssignOp
    inline lib_ams::FGrptype& operator =(const lib_ams::FGrptype &rhs) = delete;
    // func:lib_ams.FGrptype..CopyCtor
    inline               FGrptype(const lib_ams::FGrptype &rhs) = delete;
private:
    // func:lib_ams.FGrptype..Ctor
    inline               FGrptype() __attribute__((nothrow));
    // func:lib_ams.FGrptype..Dtor
    inline               ~FGrptype() __attribute__((nothrow));
    friend lib_ams::FGrptype&   grptype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FGrptype*   grptype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 grptype_RemoveAll() __attribute__((nothrow));
    friend void                 grptype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FOutmsg - Outbound message waiting for room in its ring
<a href="#lib_ams-foutmsg"></a>

#### lib_ams.FOutmsg Fields
<a href="#lib_ams-foutmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FOutmsg.data|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftype/Val.md)||The formatted message, byte for byte as it will enter the ring|
|lib_ams.FOutmsg.p_shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Upptr](/txt/exe/amc/reftype/Upptr.md)||Ring this message is queued for|

#### Struct FOutmsg
<a href="#struct-foutmsg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FOutmsg { // lib_ams.FOutmsg: Outbound message waiting for room in its ring
    lib_ams::FOutmsg*   outmsg_next;          // Pointer to next free element int tpool
    algo::ByteAry       data;                 // The formatted message, byte for byte as it will enter the ring
    lib_ams::FShm*      p_shm;                // reference to parent row
    lib_ams::FOutmsg*   shm_zd_outmsg_next;   // zslist link; -1 means not-in-list
    lib_ams::FOutmsg*   shm_zd_outmsg_prev;   // previous element
    // func:lib_ams.FOutmsg..AssignOp
    inline lib_ams::FOutmsg& operator =(const lib_ams::FOutmsg &rhs) = delete;
    // func:lib_ams.FOutmsg..CopyCtor
    inline               FOutmsg(const lib_ams::FOutmsg &rhs) = delete;
private:
    // func:lib_ams.FOutmsg..Ctor
    inline               FOutmsg() __attribute__((nothrow));
    // func:lib_ams.FOutmsg..Dtor
    inline               ~FOutmsg() __attribute__((nothrow));
    friend lib_ams::FOutmsg&    outmsg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FOutmsg*    outmsg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 outmsg_Delete(lib_ams::FOutmsg &row) __attribute__((nothrow));
};
```

#### lib_ams.FProc
<a href="#lib_ams-fproc"></a>

#### lib_ams.FProc Fields
<a href="#lib_ams-fproc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FProc.proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftype/Val.md)||Unique ID|
|lib_ams.FProc.c_shm|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|lib_ams.FProc.pid|i32|[Val](/txt/exe/amc/reftype/Val.md)||Current PID|
|lib_ams.FProc.status|i32|[Val](/txt/exe/amc/reftype/Val.md)||Exit status|
|lib_ams.FProc.critical|bool|[Val](/txt/exe/amc/reftype/Val.md)||Exit if process dies?|

#### Struct FProc
<a href="#struct-fproc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FProc { // lib_ams.FProc
    lib_ams::FProc*   proc_next;          // Pointer to next free element int tpool
    lib_ams::FProc*   ind_proc_next;      // hash next
    u32               ind_proc_hashval;   // hash value
    lib_ams::FProc*   zd_proc_next;       // zslist link; -1 means not-in-list
    lib_ams::FProc*   zd_proc_prev;       // previous element
    ams::ProcId       proc_id;            // Unique ID
    lib_ams::FShm**   c_shm_elems;        // array of pointers
    u64               c_shm_n;            // current size
    u64               c_shm_max;          // capacity of allocated array
    i32               pid;                //   0  Current PID
    i32               status;             //   0  Exit status
    bool              critical;           //   false  Exit if process dies?
    // reftype Ptrary of lib_ams.FProc.c_shm prohibits copy
    // func:lib_ams.FProc..AssignOp
    inline lib_ams::FProc& operator =(const lib_ams::FProc &rhs) = delete;
    // reftype Ptrary of lib_ams.FProc.c_shm prohibits copy
    // func:lib_ams.FProc..CopyCtor
    inline               FProc(const lib_ams::FProc &rhs) = delete;
private:
    // func:lib_ams.FProc..Ctor
    inline               FProc() __attribute__((nothrow));
    // func:lib_ams.FProc..Dtor
    inline               ~FProc() __attribute__((nothrow));
    friend lib_ams::FProc&      proc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FProc*      proc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 proc_Delete(lib_ams::FProc &row) __attribute__((nothrow));
};
```

#### lib_ams.FProctype
<a href="#lib_ams-fproctype"></a>

#### lib_ams.FProctype Fields
<a href="#lib_ams-fproctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FProctype.base|[amsdb.Proctype](/txt/ssimdb/amsdb/proctype.md)|[Base](/txt/ssimdb/amsdb/proctype.md)|||

#### Struct FProctype
<a href="#struct-fproctype"></a>
*Note:* field ``lib_ams.FProctype.base`` has reftype ``base`` so the fields of [amsdb.Proctype](/txt/ssimdb/amsdb/proctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FProctype { // lib_ams.FProctype
    lib_ams::FProctype*   ind_proctype_next;      // hash next
    u32                   ind_proctype_hashval;   // hash value
    algo::Smallstr50      proctype;               //
    u32                   id;                     //   0
    algo::cstring         ns;                     // dmmeta.ns can be missing if the module is external, don't set it as xref
    u32                   overheadmb;             //   0  Measured base memory overhead MB (10^6) beyond topo budgets; 0=no derived proc limit
    u32                   hugemb;                 //   0  Huge-page heap ceiling MB (10^6); 0 = maps none, as a forking supervisor must
    i32                   hbtimeout;              //   30  Heartbeat timeout sec: the supervisor kills a module silent this long
    algo::cstring         comment;                //
    // func:lib_ams.FProctype..AssignOp
    inline lib_ams::FProctype& operator =(const lib_ams::FProctype &rhs) = delete;
    // func:lib_ams.FProctype..CopyCtor
    inline               FProctype(const lib_ams::FProctype &rhs) = delete;
private:
    // func:lib_ams.FProctype..Ctor
    inline               FProctype() __attribute__((nothrow));
    // func:lib_ams.FProctype..Dtor
    inline               ~FProctype() __attribute__((nothrow));
    friend lib_ams::FProctype&  proctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FProctype*  proctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 proctype_RemoveAll() __attribute__((nothrow));
    friend void                 proctype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FShm - Shm record
<a href="#lib_ams-fshm"></a>

#### lib_ams.FShm Fields
<a href="#lib_ams-fshm-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FShm.grp_id|[ams.GrpId](/txt/protocol/ams/README.md#ams-grpid)|[Val](/txt/exe/amc/reftype/Val.md)||Stream ID (primary key)|
|lib_ams.FShm.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Name of the file (if opened)|
|lib_ams.FShm.shm_file|[algo_lib.FFildes](/txt/gen/algo_lib/algo_lib.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftype/Val.md)||Associated file|
|lib_ams.FShm.flags|[ams.ShmFlags](/txt/protocol/ams/README.md#ams-shmflags)|[Val](/txt/exe/amc/reftype/Val.md)||Shim flags (r, w, etc)|
|lib_ams.FShm.shm_handle|u8|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Associated memory segment handle (windows only)|
|lib_ams.FShm.c_cur_msg|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Current message|
|lib_ams.FShm.c_shmhdr|[ams.Shmhdr](/txt/protocol/ams/README.md#ams-shmhdr)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Control block for this segment|
|lib_ams.FShm.c_data|u8|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Pointer to data|
|lib_ams.FShm.c_reader|[ams.Shmember](/txt/protocol/ams/README.md#ams-shmember)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||If we are reading this shm, heartbeat area|
|lib_ams.FShm.shm_region|[algo.memptr](/txt/protocol/algo/memptr.md)|[Val](/txt/exe/amc/reftype/Val.md)||Shared memory region|
|lib_ams.FShm.writelimit|u64|[Val](/txt/exe/amc/reftype/Val.md)||Write limit, based on all read members|
|lib_ams.FShm.cached_woff|u64|[Val](/txt/exe/amc/reftype/Val.md)||Cached woff from previous poll|
|lib_ams.FShm.offset_mask|u64|[Val](/txt/exe/amc/reftype/Val.md)||Mask for byte eof|
|lib_ams.FShm.h_amsmsg|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Hook](/txt/exe/amc/reftype/Hook.md)||Message processing hook|
|lib_ams.FShm.p_grptype|[lib_ams.FGrptype](/txt/gen/lib_ams/lib_ams.md#lib_ams-fgrptype)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|lib_ams.FShm.error|bool|[Val](/txt/exe/amc/reftype/Val.md)||Error was detected|
|lib_ams.FShm.created|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FShm.locked|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FShm.n_unread|u64|[Val](/txt/exe/amc/reftype/Val.md)||Number of messages unread with UnreadMsg|
|lib_ams.FShm.burst|i32|[Val](/txt/exe/amc/reftype/Val.md)|10|Read this many messages at a time|
|lib_ams.FShm.lowbudget|i32|[Val](/txt/exe/amc/reftype/Val.md)|0|Low budget counter|
|lib_ams.FShm.max_msg_size|i32|[Val](/txt/exe/amc/reftype/Val.md)|0|Largest message this ring carries; 0 until declared, when the process ceiling applies|
|lib_ams.FShm.n_wlim_update|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|lib_ams.FShm.size|i64|[Val](/txt/exe/amc/reftype/Val.md)||Segment size in bytes; 0 = the process-wide default|
|lib_ams.FShm.c_boardq|[lib_ams.FBoardq](/txt/gen/lib_ams/lib_ams.md#lib_ams-fboardq)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)||Per-reader board queues of this lane|
|lib_ams.FShm.p_board|[lib_ams.FShm](/txt/gen/lib_ams/lib_ams.md#lib_ams-fshm)|[Upptr](/txt/exe/amc/reftype/Upptr.md)||Board this lane posts large payloads to|
|lib_ams.FShm.free_slot|u32|[Tary](/txt/exe/amc/reftype/Tary.md)||Stack of free board slot indices, board segment only|
|lib_ams.FShm.slot_nref|u32|[Tary](/txt/exe/amc/reftype/Tary.md)||Per-slot count of readers yet to release it, board only|
|lib_ams.FShm.zd_outmsg|[lib_ams.FOutmsg](/txt/gen/lib_ams/lib_ams.md#lib_ams-foutmsg)|[Llist](/txt/exe/amc/reftype/Llist.md)||This ring's queued messages, oldest first|

#### Struct FShm
<a href="#struct-fshm"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FShm { // lib_ams.FShm: Shm record
    lib_ams::FShm*               ind_shm_next;        // hash next
    u32                          ind_shm_hashval;     // hash value
    lib_ams::FShm*               cd_poll_read_next;   // zslist link; -1 means not-in-list
    lib_ams::FShm*               cd_poll_read_prev;   // previous element
    lib_ams::FShm*               zd_park_read_next;   // zslist link; -1 means not-in-list
    lib_ams::FShm*               zd_park_read_prev;   // previous element
    lib_ams::FShm*               zd_outshm_next;      // zslist link; -1 means not-in-list
    lib_ams::FShm*               zd_outshm_prev;      // previous element
    ams::GrpId                   grp_id;              // Stream ID (primary key)
    algo::cstring                filename;            // Name of the file (if opened)
    algo_lib::FFildes            shm_file;            // Associated file
    ams::ShmFlags                flags;               // Shim flags (r, w, etc)
    u8*                          shm_handle;          // Associated memory segment handle (windows only). optional pointer
    ams::MsgHeader*              c_cur_msg;           // Current message. optional pointer
    ams::Shmhdr*                 c_shmhdr;            // Control block for this segment. optional pointer
    u8*                          c_data;              // Pointer to data. optional pointer
    ams::Shmember*               c_reader;            // If we are reading this shm, heartbeat area. optional pointer
    algo::memptr                 shm_region;          // Shared memory region
    u64                          writelimit;          //   0  Write limit, based on all read members
    u64                          cached_woff;         //   0  Cached woff from previous poll
    u64                          offset_mask;         //   0  Mask for byte eof
    lib_ams::shm_h_amsmsg_hook   h_amsmsg;            //   NULL  Pointer to a function
    u64                          h_amsmsg_ctx;        //   0  Callback context
    lib_ams::FGrptype*           p_grptype;           // reference to parent row
    bool                         error;               //   false  Error was detected
    bool                         created;             //   false
    bool                         locked;              //   false
    u64                          n_unread;            //   0  Number of messages unread with UnreadMsg
    i32                          burst;               //   10  Read this many messages at a time
    i32                          lowbudget;           //   0  Low budget counter
    i32                          max_msg_size;        //   0  Largest message this ring carries; 0 until declared, when the process ceiling applies
    u64                          n_wlim_update;       //   0
    i64                          size;                //   0  Segment size in bytes; 0 = the process-wide default
    lib_ams::FBoardq**           c_boardq_elems;      // array of pointers
    u64                          c_boardq_n;          // current size
    u64                          c_boardq_max;        // capacity of allocated array
    lib_ams::FShm*               p_board;             // reference to parent row
    u32*                         free_slot_elems;     // pointer to elements
    u64                          free_slot_n;         // number of elements in array
    u64                          free_slot_max;       // max. capacity of array before realloc
    u32*                         slot_nref_elems;     // pointer to elements
    u64                          slot_nref_n;         // number of elements in array
    u64                          slot_nref_max;       // max. capacity of array before realloc
    lib_ams::FOutmsg*            zd_outmsg_head;      // zero-terminated doubly linked list
    i32                          zd_outmsg_n;         // zero-terminated doubly linked list
    lib_ams::FOutmsg*            zd_outmsg_tail;      // pointer to last element
    i32                          proc_c_shm_idx;      //   -1  Array index (-1 = not in array)
    // reftype Hook of lib_ams.FShm.h_amsmsg prohibits copy
    // x-reference on lib_ams.FShm.p_grptype prevents copy
    // reftype Ptrary of lib_ams.FShm.c_boardq prohibits copy
    // reftype Llist of lib_ams.FShm.zd_outmsg prohibits copy
    // func:lib_ams.FShm..AssignOp
    lib_ams::FShm&       operator =(const lib_ams::FShm &rhs) = delete;
    // reftype Hook of lib_ams.FShm.h_amsmsg prohibits copy
    // x-reference on lib_ams.FShm.p_grptype prevents copy
    // reftype Ptrary of lib_ams.FShm.c_boardq prohibits copy
    // reftype Llist of lib_ams.FShm.zd_outmsg prohibits copy
    // func:lib_ams.FShm..CopyCtor
    FShm(const lib_ams::FShm &rhs) = delete;
private:
    // func:lib_ams.FShm..Ctor
    inline               FShm() __attribute__((nothrow));
    // func:lib_ams.FShm..Dtor
    inline               ~FShm() __attribute__((nothrow));
    friend lib_ams::FShm&       shm_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FShm*       shm_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 shm_RemoveAll() __attribute__((nothrow));
    friend void                 shm_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.MsgFmt
<a href="#lib_ams-msgfmt"></a>

#### lib_ams.MsgFmt Fields
<a href="#lib_ams-msgfmt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.MsgFmt.payload_lim|i32|[Val](/txt/exe/amc/reftype/Val.md)|1024000|Print up to this many chars from payload|
|lib_ams.MsgFmt.indent|i32|[Val](/txt/exe/amc/reftype/Val.md)|0|Print indentation (with -pretty)|
|lib_ams.MsgFmt.format|u8|[Val](/txt/exe/amc/reftype/Val.md)|0|Output format|
|lib_ams.MsgFmt.strip|i32|[Val](/txt/exe/amc/reftype/Val.md)||Number of headers to strip|
|lib_ams.MsgFmt.pretty|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Use structured and indented printing|
|lib_ams.MsgFmt.showlen|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Show message length|
|lib_ams.MsgFmt.h_convert|[lib_ams.MsgFmt](/txt/gen/lib_ams/lib_ams.md#lib_ams-msgfmt)|[Hook](/txt/exe/amc/reftype/Hook.md)|||
|lib_ams.MsgFmt.convert_key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||conversion key|
|lib_ams.MsgFmt.convert_val|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||conversion value - overwritten by the return|

#### Struct MsgFmt
<a href="#struct-msgfmt"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct MsgFmt { // lib_ams.MsgFmt
    i32                               payload_lim;     //   1024000  Print up to this many chars from payload
    i32                               indent;          //   0  Print indentation (with -pretty)
    u8                                format;          //   0  Output format
    i32                               strip;           //   0  Number of headers to strip
    bool                              pretty;          //   true  Use structured and indented printing
    bool                              showlen;         //   false  Show message length
    lib_ams::msg_fmt_h_convert_hook   h_convert;       //   NULL  Pointer to a function
    u64                               h_convert_ctx;   //   0  Callback context
    algo::cstring                     convert_key;     // conversion key
    algo::cstring                     convert_val;     // conversion value - overwritten by the return
    // func:lib_ams.MsgFmt..Ctor
    inline               MsgFmt() __attribute__((nothrow));
};
```
