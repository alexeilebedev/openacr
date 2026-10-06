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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/macos.cpp -- macOS adaptation layer
//
// The macOS adaptation layer: Darwin implementations of calls that Linux
// supplies and Darwin does not.  One `#if defined(__MACH__)` spans the file, so
// it compiles to nothing everywhere else, which is what lets a single
// dev.targsrc row carry a platform-only source -- the table names a target and a
// source with no uname between them.  Everything macOS needs that is not simply
// a different way of answering the same question reads here, and nowhere else.

#include "include/algo.h"

#if defined(__MACH__)

// Create a pipe whose two ends already carry FLAGS, the way Linux's pipe2 does.
//
// A caller that wants O_CLOEXEC on a pipe has to get it before the next fork,
// and `pipe` followed by `fcntl` leaves a window in between: a thread that forks
// there hands the child a copy of the write end, and the child then holds the
// reader open after the writer is gone, so a read that should have seen eof
// blocks forever.  Darwin has no atomic form of the call, so the window cannot
// be closed here -- what it can do is stop being the caller's problem, and shrink
// to the two fcntl calls below.
//
// FD receives the two descriptors on success, read end first.  FLAGS takes
// O_CLOEXEC and O_NONBLOCK, each applied to both ends; any other bit is refused
// with EINVAL, since silently dropping a flag the caller asked for is how the
// window above reopens.  Returns 0, or -1 with errno set and no descriptor left
// open.
int pipe2(int fd[2], int flags) {
    int ret = -1;
    int fdflags = (flags & O_CLOEXEC) != 0 ? FD_CLOEXEC : 0;
    int stflags = flags & O_NONBLOCK;
    if ((flags & ~(O_CLOEXEC | O_NONBLOCK)) != 0) {
        errno = EINVAL;
    } else if (pipe(fd) == 0) {
        ret = 0;
        for (int i = 0; i < 2 && ret == 0; i++) {
            if (fcntl(fd[i], F_SETFD, fdflags) == -1) {
                ret = -1;
            } else if (stflags != 0 && fcntl(fd[i], F_SETFL, stflags) == -1) {
                ret = -1;
            }
        }
        if (ret == -1) {
            int err = errno;
            (void)close(fd[0]);
            (void)close(fd[1]);
            errno = err;
        }
    }
    return ret;
}


// Take LEN bytes of backing store for FD from OFFSET, the way Linux's
// posix_fallocate does: the file is extended to at least OFFSET+LEN and the
// space is allocated, so a later store into it cannot fail for lack of room.
// Returns 0, or the error number, as the Linux call does rather than through
// errno.  Darwin allocates with F_PREALLOCATE, which takes contiguous space
// first and falls back to whatever the volume has; the truncate after it gives
// the file its length.
//
// A POSIX shared memory object is memory with no filesystem under it, so it has
// no space to run out of.  F_PREALLOCATE refuses it with EBADF, and Darwin lets
// its length be set only once, by the ftruncate that made it.  An object that
// is already OFFSET+LEN long therefore has its space, and the call succeeds
// without touching it.
int posix_fallocate(int fd, off_t offset, off_t len) {
    int ret = 0;
    fstore_t store;
    store.fst_flags = F_ALLOCATECONTIG;
    store.fst_posmode = F_PEOFPOSMODE;
    store.fst_offset = 0;
    store.fst_length = offset + len;
    store.fst_bytesalloc = 0;
    if (fcntl(fd, F_PREALLOCATE, &store) == -1) {
        store.fst_flags = F_ALLOCATEALL;
        if (fcntl(fd, F_PREALLOCATE, &store) == -1) {
            ret = errno;
        }
    }
    struct stat st;
    bool sized = ret == EBADF && fstat(fd, &st) == 0 && st.st_size >= offset + len;
    if (sized) {
        ret = 0;
    } else if (ret == 0 && ftruncate(fd, offset + len) == -1) {
        ret = errno;
    }
    return ret;
}

#endif
