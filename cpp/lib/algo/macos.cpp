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

#include <sys/sysctl.h>
#include <mach/mach.h>

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


// Allocate backing store for FD up to END bytes from the start of the file, without
// changing its length.  F_PREALLOCATE in F_PEOFPOSMODE allocates past the file's
// physical end, the space it already holds, and its length is the number of new
// bytes wanted, so the call asks for the gap between END and that space and asks
// nothing of a file that already holds it.  It takes contiguous space first and
// falls back to whatever the volume has.  Returns 0, or the error number.
static int PreallocateTo(int fd, off_t end) {
    int ret = 0;
    struct stat st;
    if (fstat(fd, &st) == -1) {
        ret = errno;
    } else if (end > off_t(st.st_blocks) * 512) {
        fstore_t store;
        store.fst_flags = F_ALLOCATECONTIG;
        store.fst_posmode = F_PEOFPOSMODE;
        store.fst_offset = 0;
        store.fst_length = end - off_t(st.st_blocks) * 512;
        store.fst_bytesalloc = 0;
        if (fcntl(fd, F_PREALLOCATE, &store) == -1) {
            store.fst_flags = F_ALLOCATEALL;
            if (fcntl(fd, F_PREALLOCATE, &store) == -1) {
                ret = errno;
            }
        }
    }
    return ret;
}

// Take LEN bytes of backing store for FD from OFFSET, the way Linux's
// posix_fallocate does: the file is extended to at least OFFSET+LEN and the
// space is allocated, so a later store into it cannot fail for lack of room.
// Returns 0, or the error number, as the Linux call does rather than through
// errno.  The truncate after the allocation gives a shorter file its length,
// and a file already longer keeps its length, as on Linux.
//
// A POSIX shared memory object is memory with no filesystem under it, so it has
// no space to run out of.  F_PREALLOCATE refuses it with EBADF, and Darwin lets
// its length be set only once, by the ftruncate that made it.  An object that
// is already OFFSET+LEN long therefore has its space, and the call succeeds
// without touching it.
int posix_fallocate(int fd, off_t offset, off_t len) {
    int ret = PreallocateTo(fd, offset + len);
    struct stat st;
    bool sized = ret == EBADF && fstat(fd, &st) == 0 && st.st_size >= offset + len;
    if (sized) {
        ret = 0;
    } else if (ret == 0 && fstat(fd, &st) == -1) {
        ret = errno;
    } else if (ret == 0 && st.st_size < offset + len && ftruncate(fd, offset + len) == -1) {
        ret = errno;
    }
    return ret;
}

// Allocate LEN bytes of backing store for FD from OFFSET, the way Linux's
// fallocate does.  MODE is 0, which also extends the file to OFFSET+LEN, or
// FALLOC_FL_KEEP_SIZE, which leaves the file's length where it was so the space
// sits beneath a region the file does not claim yet.  Any other MODE bit is
// refused with EOPNOTSUPP.  Returns 0, or -1 with errno set, as the Linux call does.
int fallocate(int fd, int mode, off_t offset, off_t len) {
    int err = 0;
    if ((mode & ~FALLOC_FL_KEEP_SIZE) != 0) {
        err = EOPNOTSUPP;
    } else if ((mode & FALLOC_FL_KEEP_SIZE) != 0) {
        err = PreallocateTo(fd, offset + len);
    } else {
        err = posix_fallocate(fd, offset, len);
    }
    if (err != 0) {
        errno = err;
    }
    return err == 0 ? 0 : -1;
}

// Write FD's data to the device and wait until it is there, the way Linux's
// fdatasync does.  Darwin's fsync hands the data to the drive and returns before
// the drive's cache has written it, so a power loss can still take it.
// F_FULLFSYNC waits for the drive.  A descriptor it does not support gets the
// fsync, and the fsync's errno is the answer: F_FULLFSYNC answers EBADF for a
// pipe, which is a valid descriptor that fsync refuses with EINVAL, as Linux's
// fdatasync does.  Any other error is the device's, such as EIO from a failed
// flush, and reaches the caller as it is, since an fsync after it may find the
// buffers already marked written and report success.
// Returns 0, or -1 with errno set.
int fdatasync(int fd) {
    int ret = fcntl(fd, F_FULLFSYNC);
    if (ret == -1 && (errno == EBADF || errno == EINVAL || errno == ENOTSUP || errno == ENOTTY)) {
        ret = fsync(fd);
    }
    return ret;
}

// Start writeback of NBYTES of FD from OFFSET, which Linux does without waiting
// for it.  Darwin has no call that starts writeback and returns, and every form
// of flush it offers blocks until the data is on the device.  A caller uses this
// call to spread writeback ahead of a sync barrier, and the barrier writes the same
// data later, so the call does nothing here and returns 0 whatever FLAGS asks.
int sync_file_range(int fd, off64_t offset, off64_t nbytes, unsigned int flags) {
    (void)fd;
    (void)offset;
    (void)nbytes;
    (void)flags;
    return 0;
}

// Fill INFO with the host's memory, the way Linux's sysinfo does for the fields
// Darwin declares: the physical memory from sysctl hw.memsize, and the free pages
// from the Mach VM statistics, all in bytes, so mem_unit is 1.  Returns 0, or -1
// with errno set when the host refuses either question.
int sysinfo(struct sysinfo *info) {
    int ret = -1;
    u64 memsize = 0;
    size_t len = sizeof(memsize);
    vm_size_t pagesize = 0;
    vm_statistics64_data_t vm;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) != 0) {
        // sysctlbyname set errno
    } else if (host_page_size(mach_host_self(), &pagesize) != KERN_SUCCESS) {
        errno = EIO;
    } else if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vm, &count) != KERN_SUCCESS) {
        errno = EIO;
    } else {
        info->totalram = memsize;
        info->freeram = u64(vm.free_count) * pagesize;
        info->mem_unit = 1;
        ret = 0;
    }
    return ret;
}

// Read the CPUs PID may run on into MASK, of SIZE bytes.  Darwin has no CPU
// affinity, so the call refuses with ENOSYS and returns -1.
int sched_getaffinity(int pid, size_t size, cpu_set_t *mask) {
    (void)pid;
    (void)size;
    (void)mask;
    errno = ENOSYS;
    return -1;
}

// Restrict PID to the CPUs in MASK, of SIZE bytes.  Darwin has no CPU affinity,
// so the call refuses with ENOSYS and returns -1.
int sched_setaffinity(int pid, size_t size, const cpu_set_t *mask) {
    (void)pid;
    (void)size;
    (void)mask;
    errno = ENOSYS;
    return -1;
}

#endif
