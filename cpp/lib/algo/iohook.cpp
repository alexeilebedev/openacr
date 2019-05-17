// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/iohook.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/algo.h"
#if defined(__MACH__) || __FreeBSD__>0
// kqueue.h
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#endif

// -----------------------------------------------------------------------------

void algo_lib::IohookInit() {
#if defined(__MACH__) || __FreeBSD__>0
    algo_lib::_db.epoll_fd = kqueue();
    if (algo_lib::_db.epoll_fd == -1) {
        FatalErrorExit("kqueue");
    }
#else
    algo_lib::_db.epoll_fd = epoll_create(1);
    if (algo_lib::_db.epoll_fd == -1) {
        FatalErrorExit("epoll_create");
    }
#endif
}

// -----------------------------------------------------------------------------

// Register IOHOOK to be called whenever an IO operation is possible.
// OK to add an fd twice with different flags. Subsequent calls override previous ones.
// Add iohook to epoll in read, write or read/write mode
// Optionally, add as edge triggered
void algo_lib::IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) NOTHROW {
    bool isnew = !iohook.in_epoll;
    iohook.evt_flags = inflags;// flags we're now subscribed to
#if defined(__MACH__) || __FreeBSD__>0
    struct kevent ev;
    int flags=0;
    if (read_Get(inflags)) {
        EV_SET(&ev, iohook.fildes.value, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, &iohook);
        (void)kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL);
    }
    if (write_Get(inflags)) {
        EV_SET(&ev, iohook.fildes.value, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, &iohook);
        (void)kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL);
    }
#else
    struct epoll_event ev;
    ev.events                     = 0;
    ev.events |= EPOLLET;// edge-trigger only
    if (read_Get(inflags)) ev.events  |= EPOLLIN;
    if (write_Get(inflags)) ev.events |= EPOLLOUT;
    ev.data.ptr = (void*)&iohook;
    int req     = isnew ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    int rc      = epoll_ctl(algo_lib::_db.epoll_fd, req, iohook.fildes.value, &ev);
    int err     = errno;
    (void)rc;// coverity
    // file descriptors cannot be added to epoll, linux returns EPERM.
    // we support such use, and immediately mark the file descriptor as "ready" -- why not?
    if (err == EPERM) {
        callback_Call(iohook,iohook);// immediate callback
    }
#endif
    iohook.in_epoll = true;
    algo_lib::_db.n_iohook += isnew;
}

// -----------------------------------------------------------------------------

// De-register interest in iohook
void algo_lib::IohookRemove(algo_lib::FIohook& iohook) NOTHROW {
    if (iohook.in_epoll) {
        algo_lib::_db.n_iohook -= 1;
        iohook.in_epoll = false;
#if defined(__MACH__) || __FreeBSD__>0
        struct kevent ev;
        int flags=0;
        if (read_Get(iohook.evt_flags)) {
            EV_SET(&ev, iohook.fildes.value, EVFILT_READ, EV_DELETE, 0, 0, &iohook);
            (void)kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL);
        }
        if (write_Get(iohook.evt_flags)) {
            EV_SET(&ev, iohook.fildes.value, EVFILT_WRITE, EV_DELETE, 0, 0, &iohook);
            (void)kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL);
        }
#else
        // epoll_ctl:
        // passing NULL as last argument causes crash in DBL2.0
        // but is valid on Linux >2.6.9
        struct epoll_event ev;
        ev.events = 0;
        ev.data.ptr = (void*)&iohook;
        // iohook may not be in epoll at all, if
        // it is a file descriptor that was successfully ignored
        // in IohookAdd.
        int rc = epoll_ctl(algo_lib::_db.epoll_fd,EPOLL_CTL_DEL,iohook.fildes.value,&ev);
        (void)rc;// coverity
#endif
    }
}

// -----------------------------------------------------------------------------

// Give up CPU time for up to CLOCKS clocks,
// with no expectation of I/O (since no FDs are open)
// Small sleeps (less than a few microseconds) are converted to spinning.
static void SleepClocks(u64 clocks) {
    if (clocks > 10000) {
        u64 sleep_nsec = u64(clocks * algo_lib::_db.clocks_to_ns);
        struct timespec t;
        t.tv_sec = sleep_nsec / 1000000000;
        t.tv_nsec = sleep_nsec % 1000000000;
        nanosleep(&t,&t);
    }
}

// -----------------------------------------------------------------------------

#if defined(__MACH__) || __FreeBSD__>0
static inline void IohookWaitClocks_Kqueue(u64 wait_clocks) {
    struct kevent events[20];
    u64 wait_nano = wait_clocks * algo_lib::_db.clocks_to_ns;
    const i64 billion = 1000000000;
    struct timespec timeout;
    timeout.tv_sec = wait_nano / billion;
    timeout.tv_nsec = wait_nano % billion;
    int n = kevent(algo_lib::_db.epoll_fd
                   , NULL, 0
                   , events, _array_count(events)
                   , &timeout);
    for (int i=0; i<n; i++) {
        struct kevent &ev = events[i];
        algo_lib::FIohook *iohook = (algo_lib::FIohook*)ev.udata;
        iohook->flags.value = 0;
        if (ev.flags & EV_EOF) {
            eof_Set(iohook->flags, true);
        }
        if (ev.filter == EVFILT_READ) {
            read_Set(iohook->flags, true);
        }
        if (ev.filter == EVFILT_WRITE) {
            write_Set(iohook->flags, true);
        }
        callback_Call(*iohook,*iohook);// immediate callback
    }
}
#endif

// -----------------------------------------------------------------------------

#ifdef __linux__
static inline void IohookWaitClocks_Epoll(u64 wait_clocks) {
    i32 wait_ms = i32(i64_Min(wait_clocks * algo_lib::_db.clocks_to_ms, 60000));
    // For non-realtime processes,
    // avoid taking 100% cpu when wait is < 1 msec.
    // Convert all sub-millisecond waits to 1-millisecond waits
    if (UNLIKELY(algo_lib::_db.sleep_roundup)) {
        wait_ms += wait_clocks > 0 && wait_ms == 0;
    }
    epoll_event events[20];
    int n = epoll_wait(algo_lib::_db.epoll_fd, events, _array_count(events), wait_ms);
    for (int i=0; i<n; i++) {
        epoll_event &ev = events[i];
        algo_lib::FIohook *iohook = (algo_lib::FIohook*)ev.data.ptr;
        iohook->flags.value = 0;
        if (ev.events & EPOLLIN) read_Set(iohook->flags, true);
        if (ev.events & EPOLLOUT) write_Set(iohook->flags, true);
        if (ev.events & EPOLLHUP) eof_Set(iohook->flags, true);
        if (ev.events & EPOLLERR) err_Set(iohook->flags, true);
        callback_Call(*iohook,*iohook);// immediate callback
    }
}
#endif

// -----------------------------------------------------------------------------

static void _IohookWaitClocks(u64 wait_clocks) {
    // this is a heuristic -- do not call epoll on every cycle.
    // epolls are expensive (650 cycles/call) with vma_lib, while main steps are cheap
    // (200 cycles/step).
    if ((wait_clocks > 0) || (algo_lib::_db.giveup_count & 1) == 0) {
#if defined(__MACH__) || __FreeBSD__>0
        IohookWaitClocks_Kqueue(wait_clocks);
#else
        IohookWaitClocks_Epoll(wait_clocks);
#endif
    }
}

// -----------------------------------------------------------------------------

// give up unused time to the OS.
// Compute number of clocks to sleep before next scheduling cycle
// If there was no sleep on the previous cycle, the sleep is zero.
//   This last bit is important because it prevents deadlocks
//   when one step implicitly creates work for another step
//   that occurs before it in the main loop.
// Sleep will not extend beyond algo_lib::_db.limit
void algo_lib::giveup_time_Step() {
    u64 limit = algo_lib::_db.limit;
    u64 slack = u64_SubClip(u64_Min(algo_lib::_db.next_loop, limit), algo_lib::_db.clock);
    u64 wait_clocks = algo_lib::_db.last_sleep_clocks == 0 ? 0 : slack;
    if (algo_lib::_db.n_iohook > 0) {
        algo_lib::_db.giveup_count++;
        _IohookWaitClocks(wait_clocks);
        // at this point, next_loop could be equal to _db.limit
        // which would cause the main loop to exit; since we just executed
        // an io wait, prevent that
        algo_lib::_db.next_loop = algo_lib::_db.clock;
    } else {
        // no i/o is in progress, just give up time using nanosleep.
        // however if next_loop is equal to limit, the loop is about to exit,
        // so do nothing.
        if (algo_lib::_db.next_loop < limit) {
            SleepClocks(wait_clocks);
        }
    }
    algo_lib::_db.last_sleep_clocks = slack;
    algo_lib::giveup_time_UpdateCycles();
}
