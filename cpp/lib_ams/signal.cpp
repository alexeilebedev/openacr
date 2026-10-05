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
// Source: cpp/lib_ams/signal.cpp
//
// Signaled-mode wakeup protocol.  In signaled mode a process sleeps in
// epoll_wait when idle and is woken by SIGRTMIN delivered through an
// always-armed signalfd, instead of busy-spinning.  A reader with no data
// parks (sleeping flag, dropped from the poll loop); a writer with no budget
// parks (writer_sleeping flag).  The peer that frees the resource -- a writer
// publishing data, a reader draining the ring -- owes it a wakeup, and sends the
// SIGRTMIN at the end of its pass, from the zd_wake step, just before its own
// loop waits.
// Every park/wake pair re-checks the resource under a full barrier (mfence):
// the handshake is Dekker-shaped (store my flag, load the peer's state), and on
// x86 a store-load reorder lets both sides miss without it, losing the wakeup.
// A reader this process opened is on exactly one of two lists: cd_poll_read
// while it is polled, zd_park_read while it is parked.  The two lists partition
// the open readers, so every wakeup path walks the parked set alone and costs
// what this process is actually waiting on -- not the size of its stream table,
// which holds a record per group id the process has ever opened.
// This file also owns the other direction of the signal boundary: the signals by
// which the outside world asks this process to stop.

#include "include/algo.h"
#include "include/lib_ams.h"
#ifdef __linux__
#include <sys/signalfd.h>
#elif defined(WIN32)
#include <windows.h>
#endif

#ifdef _COVERAGE
extern "C" void __gcov_dump(void);// gcc coverage runtime: write .gcda for counters accumulated so far
extern "C" void __gcov_reset(void);// gcc coverage runtime: zero the counters and re-enable dumping
#endif

// This process was asked to stop.  One definition, reached by every spelling of
// the request: an inbound ams.TerminateMsg addressed to this proc, the stdin EOF
// that a parent's exit closes, and the SIGTERM or SIGINT a stop sends.
//
// What stopping means depends on the role, and h_terminate is where a role says
// so.  The default is the only thing a process can do about its own stop -- end
// its main loop -- and it is what every process wants except a supervisor, whose
// stop is the orderly shutdown of the node it runs: its own exit is the last
// step of that, not the first.
void lib_ams::Terminate() {
#ifdef _COVERAGE
    // A coverage-built process writes its .gcda only from gcc's atexit hook,
    // which the SIGKILL that ends a node (a supervisor's forceful shutdown
    // pkill) skips -- so its coverage is lost or left half-written and the
    // gcov-merge drops.  Flush now, the moment termination is requested, so
    // the coverage survives however the process finally dies.
    //
    // The reset is what keeps the rest of the run measurable.  A dump marks the
    // profile written and every later dump, the atexit one included, then does
    // nothing -- so a flush placed here would be the last word on a process
    // whose whole shutdown is still ahead of it, and every line the stop
    // executes would read as never executed.  Resetting zeroes the counters and
    // restores the right to dump, and gcda records merge by addition, so the
    // two halves add back up to the run.
    __gcov_dump();
    __gcov_reset();
#endif
    if (lib_ams::_db.h_terminate) {
        lib_ams::h_terminate_Call();
    } else {
        algo_lib::ReqExitMainLoop();
    }
}

#ifndef WIN32
// Write SIG's number to the pipe the main loop polls, and do nothing else.
// A handler runs on whatever instruction the process was executing, so it may
// perform only operations that stay correct there.  A write to a non-blocking
// pipe qualifies: it touches no allocator and no structure of this program.
//
// A full pipe is not an error.  The byte means "look again", so one already
// waiting says as much as a second would, and the read step re-examines every
// child however many bytes arrived.
static void StopsigHandler(int sig) {
    // Save and restore errno around the write.  This handler can interrupt code
    // between a failing syscall and that code's own read of errno, and the write
    // sets errno whenever the pipe is full, which is the expected case here
    // rather than an error.  Leaving it changed would give the interrupted
    // caller a verdict about a syscall it never made.
    int save = errno;
    u8 signo = u8(sig);
    ssize_t n = write(lib_ams::_db.stopsig_write.value, &signo, 1);
    (void)n;
    errno = save;
}

static void TerminateSignal(int sig) {
    (void)sig;
    lib_ams::Terminate();
}

// Perform the disposition for each signal that arrived since the last pass.
// The byte the handler writes is the signal number, so one pipe carries every
// routed signal and this step decides what each one means.
static void StopsigReadStep() {
    u8 signo = 0;
    while (read(lib_ams::_db.stopsig_read.value, &signo, 1) == 1) {
        if (signo == SIGCHLD) {
            lib_ams::h_sigchld_Call();
        } else {
            lib_ams::Terminate();
        }
    }
}
#endif

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
void lib_ams::RouteSignal(int sig) {
#ifndef WIN32
    if (!ValidQ(_db.stopsig_read)) {
        int fd[2] = {-1, -1};
        if (pipe(fd) == 0) {
            // set both flags before anything can fork, so no child inherits an
            // end of the pipe and no handler can block on a full one
            (void)fcntl(fd[0], F_SETFD, FD_CLOEXEC);
            (void)fcntl(fd[1], F_SETFD, FD_CLOEXEC);
            (void)fcntl(fd[0], F_SETFL, O_NONBLOCK);
            (void)fcntl(fd[1], F_SETFL, O_NONBLOCK);
            _db.stopsig_read.value = fd[0];
            _db.stopsig_write.value = fd[1];
            _db.stopsig_iohook.fildes = _db.stopsig_read;
            callback_Set0(_db.stopsig_iohook, StopsigReadStep);
            algo::IOEvtFlags flags;
            read_Set(flags, true);
            algo_lib::IohookAdd(_db.stopsig_iohook, flags);
        }
    }
    if (ValidQ(_db.stopsig_read)) {
        struct sigaction sigact;
        sigact.sa_handler = StopsigHandler;
        // the routed signals need not mask each other: every handler installed
        // here is the same single write, so one nesting inside another costs a
        // byte
        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = SA_RESTART;
        (void)sigaction(sig, &sigact, 0);
    }
#else
    (void)sig;
#endif
}

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
void lib_ams::SetupTerminateSignal() {
#ifndef WIN32
    lib_ams::FProctype *proctype = lib_ams::ind_proctype_Find(value_ToCstr(proctype_Get(_db.proc_id)));
    bool module = proctype && proctype->stoprank > 0;
    struct sigaction sigact;
    sigact.sa_handler = module ? SIG_IGN : TerminateSignal;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    (void)sigaction(SIGTERM, &sigact, 0);
    (void)sigaction(SIGINT , &sigact, 0);
#endif
}

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
void lib_ams::UnparkReader(lib_ams::FShm &shm) {
    shm.c_reader->sleeping = 0;
    zd_park_read_Remove(shm);
    cd_poll_read_Insert(shm);
    algo_lib::_db.next_loop = algo_lib::_db.clock;
}

// Wake every parked reader.  A SIGRTMIN names no stream -- the signal says only
// that some peer freed something -- and leaving signaled mode ends parking
// altogether, so both hand the whole parked set back to the poll loop and let
// the next cd_poll_read_Step re-check each for data.
void lib_ams::UnparkReaderSet() {
    while (zd_park_read_N() > 0) {
        UnparkReader(*zd_park_read_First());
    }
}

// Drain the signalfd (coalesced SIGRTMIN wakeups read as one event) and move
// every parked reader back into the poll loop.
void lib_ams::SignalReadStep() {
#ifdef __linux__
    struct signalfd_siginfo info;
    while (read(_db.signal_fd.value, &info, sizeof(info)) == sizeof(info)) {
    }
#endif
    UnparkReaderSet();
}

// Enter or leave signaled mode.  Entering blocks SIGRTMIN and arms an
// always-armed signalfd registered with the iohook, so a peer's SIGRTMIN wakes
// the epoll_wait.  ENABLE picks which.  Leaving first sends the wakeups this
// pass owes its peers, then removes the hook, closes the signalfd, and moves
// every parked reader back into the poll loop; the SIGRTMIN block stays in
// place for the rest of the process lifetime.
void lib_ams::SetSignaledMode(bool enable) {
#ifdef __linux__
    if (enable && !_db.signaled) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGRTMIN);
        // Block SIGRTMIN for the rest of the process lifetime, not only while
        // signaled.  A reader parks by setting its sleeping flag in shared
        // memory; its writer peer, seeing the flag, wakes it with
        // kill(pid, SIGRTMIN).  If the reader has meanwhile left signaled mode,
        // an unblocked SIGRTMIN takes its default disposition and terminates
        // the process -- a stray wakeup at a teardown boundary becomes a silent
        // kill (exit 128+SIGRTMIN).  Keeping the signal blocked past the
        // signalfd's lifetime makes a late wakeup a no-op: it sits pending,
        // undrained and harmless.  Leaving signaled mode therefore stops the
        // drain but never unblocks.
        sigprocmask(SIG_BLOCK, &mask, NULL);
        _db.signal_fd.value = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
        if (ValidQ(_db.signal_fd)) {
            _db.signal_iohook.fildes = _db.signal_fd;
            callback_Set0(_db.signal_iohook, lib_ams::SignalReadStep);
            algo::IOEvtFlags flags;
            read_Set(flags, true);
            algo_lib::IohookAdd(_db.signal_iohook, flags);
            _db.signaled = true;
        }
    }
    if (!enable && _db.signaled) {
        FlushWake();
        algo_lib::IohookRemove(_db.signal_iohook);
        close(_db.signal_fd.value);
        _db.signal_fd = algo::Fildes();
        UnparkReaderSet();
        _db.signaled = false;
    }
#else
    // signalfd and real-time signals are Linux-specific.  Other platforms
    // retain the normal busy-poll mode until a native wakeup backend exists.
    (void)enable;
    UnparkReaderSet();
    _db.signaled = false;
#endif
}

// Owe the readers of SHM a wakeup for a message just published on it.  The ring
// goes on the pass's wake list, whose step signals the parked readers before the
// loop waits.  A pass that writes a thousand messages to one ring then walks its
// member slots once, where waking at each write would walk them a thousand times
// and send a signal for each.
void lib_ams::WakeReader(lib_ams::FShm &shm) {
    shm.wake_reader = true;
    zd_wake_Insert(shm);
}

// Owe the writer of SHM a wakeup for the room this reader freed on it, which the
// pass's wake step sends if the writer is parked when the pass ends.
void lib_ams::WakeWriter(lib_ams::FShm &shm) {
    shm.wake_writer = true;
    zd_wake_Insert(shm);
}

// Park the reader on SHM: set its sleeping flag, then under a full barrier
// re-check for a message that raced in after the empty peek -- the writer's
// zd_wake_Step may already have read sleeping==0 and skipped the SIGRTMIN.  Return
// true if parked (no data); false if a message is present, in which case the
// flag is cleared and the caller keeps polling.
bool lib_ams::ParkReader(lib_ams::FShm &shm) {
    bool parked = true;
    shm.c_reader->sleeping = 1;
    mfence();
    if (PeekMsg(shm)) {
        shm.c_reader->sleeping = 0;
        parked = false;
    } else {
        zd_park_read_Insert(shm);
    }
    return parked;
}

// Park the writer on SHM waiting for budget for EXTRA more bytes: set its
// writer_sleeping flag, then under a full barrier re-sample the budget -- a
// reader may free it between the store and the load.  SLOT is the channel the
// write goes on, NULL for the base channel, and its room under its limit is
// re-checked the same way.  Return true if budget appeared (the caller writes),
// in which case the flag is cleared; false if parked (a reader's zd_wake_Step
// signals it), or if the ring is not writable, which parks nothing.  EXTRA is
// the room test the caller asked HasBudgetQ with, so the re-check agrees with
// the test that refused.
bool lib_ams::ParkWriter(lib_ams::FShm &shm, u32 extra DFLTVAL(0), ams::Shmchannel *slot DFLTVAL(NULL)) {
    bool ok = false;
    if (WritableQ(shm)) {
        shm.c_shmhdr->writer_sleeping = 1;
        mfence();
        UpdateBudget(shm);
        ok = shm.c_shmhdr->woff + extra < shm.writelimit && SlotRoomQ(slot, extra);
        if (ok) {
            shm.c_shmhdr->writer_sleeping = 0;
        }
    }
    return ok;
}


// Send SIGRTMIN to each of the NPID pids in PID.
static void KillPidSet(i32 (&pid)[32], int npid) {
    for (int i = 0; i < npid; i++) {
#ifdef __linux__
        kill(pid[i], SIGRTMIN);
#else
        (void)pid;
#endif
    }
}

// Add VAL to the set of NPID pids in PID and return the new count.  A pid
// already in the set is not added twice.  A full set is signaled and emptied
// first, so a pass with more peers than the set holds signals some of them twice
// and loses none.
static int AddWakePid(i32 (&pid)[32], int npid, i32 val) {
    bool found = false;
    for (int i = 0; i < npid && !found; i++) {
        found = pid[i] == val;
    }
    if (!found && npid == int(sizeof(pid)/sizeof(pid[0]))) {
        KillPidSet(pid, npid);
        npid = 0;
    }
    if (!found) {
        pid[npid] = val;
        npid++;
    }
    return npid;
}

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
void lib_ams::zd_wake_Step() {
    i32 pid[32];
    int npid = 0;
    mfence();
    while (lib_ams::FShm *shm = lib_ams::zd_wake_RemoveFirst()) {
        if (shm->wake_reader) {
            ind_beg(lib_ams::shm_c_shmember_curs, member, *shm) {
                if (member.sleeping && member.pid > 0) {
                    npid = AddWakePid(pid, npid, member.pid);
                }
            }ind_end;
        }
        if (shm->wake_writer && shm->c_shmhdr->writer_sleeping && shm->c_shmhdr->writer_pid > 0) {
            shm->c_shmhdr->writer_sleeping = 0;
            npid = AddWakePid(pid, npid, shm->c_shmhdr->writer_pid);
        }
        shm->wake_reader = false;
        shm->wake_writer = false;
    }
    KillPidSet(pid, npid);
    lib_ams::zd_wake_UpdateCycles();
}
