// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/iohook.cpp
//

#include "include/algo.h"
#if defined(__MACH__) || __FreeBSD__>0
// kqueue.h
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#elif defined(__CYGWIN__)
#include <sys/types.h>
#include <sys/time.h>
//#include <Windows.h>
#endif

// -----------------------------------------------------------------------------

// Watch PARENT on this process's queue, so that the wait loop kills this process
// with SIGKILL when PARENT exits, as Linux's PR_SET_PDEATHSIG would.  Return
// false when the queue refuses: PARENT is already gone, or the process is a
// child between fork and exec, whose parent's kqueue did not survive the fork.
// Only macOS watches; elsewhere the call returns false.
bool algo_lib::DieWithWatchQ(i32 parent) {
    bool ret = false;
#if defined(__MACH__)
    struct kevent ev;
    EV_SET(&ev, pid_t(parent), EVFILT_PROC, EV_ADD, NOTE_EXIT, 0, NULL);
    ret = algo_lib::_db.epoll_fd >= 0 && kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL) == 0;
    if (ret) {
        algo_lib::_db.die_with = parent;
    }
#else
    (void)parent;
#endif
    return ret;
}

void algo_lib::IohookInit() {
    // a forked child that opens its own queue closes the one it inherited: an
    // epoll survives fork and would stay open beside the new one, and a kqueue
    // does not, so there the close finds nothing.  A die-with watch lived on the
    // old queue and ends with it, as a fork clears PR_SET_PDEATHSIG on Linux
    if (algo_lib::_db.epoll_fd >= 0) {
        (void)close(algo_lib::_db.epoll_fd);
        algo_lib::_db.epoll_fd = -1;
    }
    algo_lib::_db.die_with = 0;
#if defined(__MACH__) || __FreeBSD__>0
    algo_lib::_db.epoll_fd = kqueue();
    if (algo_lib::_db.epoll_fd == -1) {
        FatalErrorExit("kqueue");
    }
#if defined(__MACH__)
    // ALGO_DIE_WITH is <child>:<parent>, set by DieWithParent in a forked child.
    // The watch belongs to the child alone, as PR_SET_PDEATHSIG does on Linux: it
    // holds across an exec, which keeps the pid, and a process with another pid
    // inherited the variable through a fork and ignores it.  So a shell that
    // execs its one command passes the watch on, and a detached descendant runs
    // normally.  A parent already gone kills this process now.  The variable is
    // cleared, so this process's own children start without it
    const char *die_with = getenv(algo_lib::dev_envvar_ALGO_DIE_WITH);
    if (die_with) {
        char *rest = NULL;
        i32 child = i32(strtol(die_with, &rest, 10));
        i32 parent = *rest == ':' ? i32(strtol(rest + 1, NULL, 10)) : 0;
        (void)unsetenv(algo_lib::dev_envvar_ALGO_DIE_WITH);
        if (child == i32(getpid()) && !DieWithWatchQ(parent)) {
            (void)kill(getpid(), SIGKILL);
        }
    }
#endif
#elif defined(__CYGWIN__)
#elif defined(WIN32)
#else
    algo_lib::_db.epoll_fd = epoll_create(1);
    if (algo_lib::_db.epoll_fd == -1) {
        FatalErrorExit("epoll_create");
    }
#endif
}

// -----------------------------------------------------------------------------

// Register IOHOOK to be called whenever an IO operation is possible.
// Add iohook to epoll in read, write or read/write mode, edge triggered.
// Re-registering the same iohook replaces its flag set, so a caller may widen
// or narrow one hook's subscription at any time; a descriptor carries exactly
// one registration, so two hooks may never claim the same fd.
void algo_lib::IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) NOTHROW {
    bool isnew = !iohook.in_epoll;
    iohook.evt_flags = inflags;// flags we're now subscribed to
#if defined(__MACH__) || __FreeBSD__>0
    // a kqueue keeps one filter per direction, so the new flag set adds each
    // direction it names and deletes each it drops, as EPOLL_CTL_MOD replaces
    // epoll's whole set.  Deleting a filter that is not there fails harmlessly
    //
    // A regular file never blocks, and epoll refuses one, so it is marked ready
    // at once and never reaches the queue.  kqueue would accept it and fire a read
    // only while the offset is short of the end, so a hook on an empty file, or
    // on one read to its end, would wait forever.
    struct stat st;
    bool regular = fstat(iohook.fildes.value, &st) == 0 && S_ISREG(st.st_mode);
    bool refused = false;
    int filter[2] = {EVFILT_READ, EVFILT_WRITE};
    bool want[2] = {read_Get(inflags), write_Get(inflags)};
    frep_(i, 2) {
        if (!regular) {
            struct kevent ev;
            EV_SET(&ev, iohook.fildes.value, filter[i], want[i] ? EV_ADD | EV_CLEAR : EV_DELETE, 0, 0, &iohook);
            refused = (kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL) < 0 && want[i] && errno == EINVAL) || refused;
        }
    }
    // kqueue refuses a device it cannot watch, such as /dev/null, with EINVAL.
    // Such a device never blocks either, so it too is marked ready at once.
    if (refused || regular) {
        callback_Call(iohook,iohook);// immediate callback
    }
#elif defined(WIN32)
    // TODO: windows
    // always active
    callback_Call(iohook,iohook);
#elif defined(__CYGWIN__)
    callback_Call(iohook,iohook);
#else
    struct epoll_event ev;
    ev.events = 0;
    ev.events |= EPOLLET;// edge-trigger only
    if (read_Get(inflags)) {
        ev.events |= EPOLLIN;
    }
    if (write_Get(inflags)) {
        ev.events |= EPOLLOUT;
    }
    ev.data.ptr = (void*)&iohook;
    int req = isnew ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    int rc = epoll_ctl(algo_lib::_db.epoll_fd, req, iohook.fildes.value, &ev);
    int err = rc < 0 ? errno : 0;
    // file descriptors cannot be added to epoll, linux returns EPERM.
    // we support such use, and immediately mark the file descriptor as "ready" -- why not?
    if (err == EPERM) {
        callback_Call(iohook,iohook);// immediate callback
    } else if (err == EEXIST) {
        // Consider a connection whose reader and writer each own an iohook and
        // hand both the same socket.  The reader registers, the writer's
        // registration is refused, and the writer's callback is never invoked
        // again -- so its buffer fills and the connection stalls with no error
        // anywhere, since the only report was the return value of this call.
        //
        // An epoll registration is keyed by descriptor, not by watcher: one fd
        // admits one set of flags and one callback pointer, and a second claim
        // cannot be honored under any flags.  Two hooks on one fd is therefore
        // a construction error in the caller, not a condition to absorb.
        //
        // Report it here, where the offending fd is still known.  The check
        // fires at registration -- connection setup or startup -- so it is
        // deterministic rather than a load-dependent surprise, and a wedge
        // that would have to be diagnosed from a hang becomes one line.
        FatalErrorExit(Zeroterm(tempstr()<<"algo_lib.iohook_taken"
                                <<Keyval("fd",iohook.fildes.value)
                                <<Keyval("comment","fd already registered by another iohook; one fd carries one registration")));
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
        // both directions, whatever the flags last named, so no filter is left
        // pointing at a hook that may be freed
        int filter[2] = {EVFILT_READ, EVFILT_WRITE};
        frep_(i, 2) {
            struct kevent ev;
            EV_SET(&ev, iohook.fildes.value, filter[i], EV_DELETE, 0, 0, &iohook);
            (void)kevent(algo_lib::_db.epoll_fd, &ev, 1, NULL, 0, NULL);
        }
#elif defined(WIN32)
        // TODO: fix
#elif defined(__CYGWIN__)

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
#if defined(WIN32) || defined(__CYGWIN__)
        u64 msec = clocks * algo_lib::_db.clocks_to_ns / 1000000;
        if (msec > 0) {
            algo::SleepMsec(msec);
        }
#else
        // #AL# on some cygwin implementations, nanosleep doesn't work
        // (delays by 20x the requested amount)
        u64 sleep_nsec = u64(clocks * algo_lib::_db.clocks_to_ns);
        struct timespec t;
        t.tv_sec = sleep_nsec / 1000000000;
        t.tv_nsec = sleep_nsec % 1000000000;
        nanosleep(&t,&t);
#endif
    }
}

// -----------------------------------------------------------------------------

// Return the time to wait for i/o, in nanoseconds, for a wait of WAIT_CLOCKS
// clocks, at most a minute.
//
// The wait is the time to the next scheduled step, and a process with no timer
// has none before the loop's limit, which is effectively forever.  The product
// with the clock rate then exceeds any integer, and converting it is undefined:
// on Darwin arm64 it saturates to a timeout kevent refuses, so every wait
// returns at once with no events, and an idle daemon spins without ever seeing
// a connection.  The product is capped while it is still a double, and a
// minute is long enough that the loop waking once a minute costs nothing.
static u64 GetWaitNsec(u64 wait_clocks) {
    double wait_ns = double(wait_clocks) * algo_lib::_db.clocks_to_ns;
    double cap = 60.0 * algo::UNTIME_PER_SEC;
    return u64(wait_ns < cap ? wait_ns : cap);
}

// -----------------------------------------------------------------------------

#if defined(__MACH__) || __FreeBSD__>0
static inline void IohookWaitClocks_Kqueue(u64 wait_clocks) {
    struct kevent events[20];
    u64 wait_nano = GetWaitNsec(wait_clocks);
    const i64 billion = 1000000000;
    struct timespec timeout;
    timeout.tv_sec = wait_nano / billion;
    timeout.tv_nsec = wait_nano % billion;
    int n = kevent(algo_lib::_db.epoll_fd
                   , NULL, 0
                   , events, _array_count(events)
                   , &timeout);
    // a queue this process did not open, such as one inherited through a fork
    // without IohookInit, fails every wait at once, and the loop would spin
    if (n < 0 && errno == EBADF) {
        FatalErrorExit("algo_lib.kqueue_closed  comment:\"the event queue is not open in this process; a forked child calls IohookInit\"");
    }
    for (int i=0; i<n; i++) {
        struct kevent &ev = events[i];
        if (ev.filter == EVFILT_PROC) {
            // the parent DieWithParent named has exited (IohookInit)
            (void)kill(getpid(), SIGKILL);
        } else {
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
}
#endif

// -----------------------------------------------------------------------------

#ifdef __linux__
static inline void IohookWaitClocks_Epoll(u64 wait_clocks) {
    epoll_event events[20];
    int n = 0;
    if (algo_lib::_db.use_epoll_pwait2) {
        // #AL# if epoll_pwait2 is not available, then sleep time might need
        // to be updated to at least 1 msec to avoid taking up 100% cpu for any process
        // operating at >1KHz. for now assuming that epoll_pwait2 is always there.
        struct timespec twait;
        u64 sleep_nsec = GetWaitNsec(wait_clocks);
        twait.tv_sec  = sleep_nsec / algo::UNTIME_PER_SEC;
        twait.tv_nsec = sleep_nsec % algo::UNTIME_PER_SEC;
        n = epoll_pwait2(algo_lib::_db.epoll_fd, events, _array_count(events), &twait, NULL);
    } else {
        i32 wait_ms = i32(GetWaitNsec(wait_clocks) / 1000000);
        // WARNING: Any sleep under 1 msec will cause hot-spinning in the process.
        n = epoll_wait(algo_lib::_db.epoll_fd, events, _array_count(events), wait_ms);
    }
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
#elif defined(__CYGWIN__)

#elif defined(WIN32)
        // use WaitForSingleObjectEx
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
    // A process with i/o in progress waits on the queue.  So does one that
    // watches the process it dies with (DieWithParent), or it would never read
    // that exit; its wait leaves the loop free to end at its limit.  Any other
    // process gives up time with nanosleep, unless next_loop equals limit and
    // the loop is about to exit.
    bool io = algo_lib::_db.n_iohook > 0;
    bool waitq = io || (algo_lib::_db.next_loop < limit && algo_lib::_db.die_with != 0);
    if (waitq) {
        algo_lib::_db.giveup_count++;
        _IohookWaitClocks(wait_clocks);
    } else if (algo_lib::_db.next_loop < limit) {
        SleepClocks(wait_clocks);
    }
    if (io) {
        // at this point, next_loop could be equal to _db.limit
        // which would cause the main loop to exit; since we just executed
        // an io wait, prevent that
        algo_lib::_db.next_loop = algo_lib::_db.clock;
    }
    algo_lib::_db.last_sleep_clocks = slack;
    algo_lib::giveup_time_UpdateCycles();
}
