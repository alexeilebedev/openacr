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

#endif
