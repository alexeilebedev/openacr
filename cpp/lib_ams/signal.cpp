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
// Source: cpp/lib_ams/signal.cpp
//
// Signaled-mode wakeup protocol.  In signaled mode a process sleeps in
// epoll_wait when idle and is woken by SIGRTMIN delivered through an
// always-armed signalfd, instead of busy-spinning.  A reader with no data
// parks (sleeping flag, dropped from the poll loop); a writer with no budget
// parks (writer_sleeping flag).  The peer that frees the resource -- a writer
// publishing data, a reader draining the ring -- sends SIGRTMIN to wake it.
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
    __gcov_dump();
#endif
    if (lib_ams::_db.h_terminate) {
        lib_ams::h_terminate_Call();
    } else {
        algo_lib::ReqExitMainLoop();
    }
}

#ifndef WIN32
static void TerminateSignal(int sig) {
    (void)sig;
    lib_ams::Terminate();
}
#endif

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
void lib_ams::SetupTerminateSignal() {
#ifndef WIN32
    struct sigaction sigact;
    sigact.sa_handler = TerminateSignal;
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
void lib_ams::UnparkReader(lib_ams::FShm &shm) {
    shm.c_reader->sleeping = 0;
    zd_park_read_Remove(shm);
    cd_poll_read_Insert(shm);
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
// the epoll_wait.  Leaving removes the hook, closes the signalfd, and moves
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

// After publishing data to SHM, wake any reader parked on it.  Full barrier
// between the woff store (the caller's EndWrite) and the sleeping load: the
// reader sets sleeping and then re-reads woff under its own mfence, so a
// store-load fence on both sides is required -- without it each can miss the
// other and the wakeup is lost.
void lib_ams::WakeReader(lib_ams::FShm &shm) {
#ifdef __linux__
    mfence();
    ind_beg(shm_c_shmember_curs, member, shm) {
        if (member.sleeping && member.pid > 0) {
            kill(member.pid, SIGRTMIN);
        }
    }ind_end;
#else
    (void)shm;
#endif
}

// After draining SHM, wake a writer parked on its budget -- but only once the
// ring is at least half-drained, so the writer refills about half the ring per
// wake instead of one message per wake.  Full barrier first: the reader's
// offset store must be globally visible before this re-read, else the writer's
// own re-check and this one can both miss.
void lib_ams::WakeWriter(lib_ams::FShm &shm) {
#ifdef __linux__
    if (shm.c_shmhdr->writer_sleeping) {
        mfence();
        u64 unread = shm.c_shmhdr->woff - shm.c_reader->offset;
        if (shm.c_shmhdr->writer_sleeping && shm.c_shmhdr->writer_pid > 0
            && unread <= u64(shm.offset_mask+1)/2) {
            shm.c_shmhdr->writer_sleeping = 0;
            kill(shm.c_shmhdr->writer_pid, SIGRTMIN);
        }
    }
#else
    (void)shm;
#endif
}

// Park the reader on SHM: set its sleeping flag, then under a full barrier
// re-check for a message that raced in after the empty peek -- the writer's
// EndWrite may already have read sleeping==0 and skipped the SIGRTMIN.  Return
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

// Park the writer on SHM waiting for budget: set its writer_sleeping flag, then
// under a full barrier re-sample the budget -- a reader may free it between the
// store and the load.  Return true if budget appeared (the caller writes), in
// which case the flag is cleared; false if parked (a reader's WakeWriter signals
// it).
bool lib_ams::ParkWriter(lib_ams::FShm &shm) {
    shm.c_shmhdr->writer_sleeping = 1;
    mfence();
    UpdateBudget(shm);
    bool ok = shm.c_shmhdr->woff < shm.writelimit;
    if (ok) {
        shm.c_shmhdr->writer_sleeping = 0;
    }
    return ok;
}

// About to sleep (the poll loop has drained empty): under a full barrier re-scan
// every parked reader and re-arm any whose shm now has data.  Catches a writer
// that published and read sleeping==0 before the reader's park landed -- the
// lost wakeup the per-shm ParkReader re-check cannot recover for a shm parked in
// an earlier pass.
void lib_ams::RecoverWakeup() {
    mfence();
    ind_beg(_db_zd_park_read_delcurs,shm,_db) {
        if (PeekMsg(shm)) {
            UnparkReader(shm);
        }
    }ind_end;
}
