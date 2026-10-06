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
// Source: cpp/lib_ams/shm.cpp
//

#include "include/lib_ams.h"
#ifndef WIN32
#include <sys/statvfs.h>
#include <fcntl.h>// posix_fallocate
#include <sys/mman.h>// mlock
#ifdef __APPLE__
#include <sys/posix_shm.h>
#endif
#endif

// -----------------------------------------------------------------------------

// Free bytes on the tmpfs backing /dev/shm.  INT64_MAX on statvfs
// failure (treated as "no limit" by callers comparing against a need),
// so chroots or platforms without /dev/shm don't hard-fail callers.
i64 lib_ams::GetShmAvail() {
    i64 ret = INT64_MAX;
#ifndef WIN32
    struct statvfs vfs;
    if (statvfs("/dev/shm", &vfs) == 0) {
        // f_bavail is counted in f_frsize (fragment) units, not f_bsize.
        ret = i64(vfs.f_bavail) * i64(vfs.f_frsize);
    }
#endif
    return ret;
}

// Size in bytes of the tmpfs backing /dev/shm, and 0 when statvfs cannot
// answer, so a reserve derived from it is nothing where the filesystem is
// unknown.
i64 lib_ams::GetShmTotal() {
    i64 ret = 0;
#ifndef WIN32
    struct statvfs vfs;
    if (statvfs("/dev/shm", &vfs) == 0) {
        ret = i64(vfs.f_blocks) * i64(vfs.f_frsize);
    }
#endif
    return ret;
}

// The name shm_open gives the segment of group GRP_ID: the file prefix that
// separates one instance's segments from another's, the group id, and the
// suffix.  shm_open resolves it under /dev/shm, so a caller that reaches the
// file through the filesystem instead puts that directory in front of it.
// Darwin limits these names to PSHMNAMLEN (31) characters, and a descriptive
// name can be longer, so there an overlong name is compacted to a hash.  Two
// different CRC polynomials keep a 64-bit fingerprint of the whole identity.
static algo::tempstr ShmFileName(ams::GrpId grp_id) {
    algo::tempstr ret;
    ret << lib_ams::_db.file_prefix << (lib_ams::_db.file_prefix=="" ? "" : "-") << grp_id << ".ams";
#ifdef __APPLE__
    if (ch_N(ret) > PSHMNAMLEN) {
        algo::memptr bytes = strptr_ToMemptr(ret);
        u64 hash = u64(algo::CRC32Step(0, bytes.elems, bytes.n_elems)) << 32
            | algo::CRC32IEEE(0, bytes.elems, bytes.n_elems);
        ret = "ams-";
        algo::u64_PrintHex(hash, ret, 16, false);
        ret << ".ams";
    }
#endif
    return ret;
}

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
i64 lib_ams::ShmExistingSize(ams::GrpId grp_id) {
    i64 ret = 0;
#ifndef WIN32
    tempstr filename;
    filename << "/dev/shm/" << ShmFileName(grp_id);
    struct stat st;
    if (stat(Zeroterm(filename), &st) == 0) {
        ret = i64(st.st_blocks) * 512;
    }
#endif
    return ret;
}

// TRUE when the segment of group GRP_ID exists on this node's tmpfs, whoever
// made it and whether or not this process has opened it.
bool lib_ams::ShmExistsQ(ams::GrpId grp_id) {
    bool ret = false;
#ifndef WIN32
    tempstr filename;
    filename << "/dev/shm/" << ShmFileName(grp_id);
    struct stat st;
    ret = stat(Zeroterm(filename), &st) == 0;
#endif
    return ret;
}

#ifndef WIN32
// TRUE when the segment named PATH (open on FD) is a reclaimable orphan.  For a
// segment this host can attribute to a process, the answer rests on a proof that
// some process is gone, never on the segment's age.
//
// Take every page of board segment SHM from the tmpfs now, and lock them in
// memory; TRUE when the pages are committed.
//
// A tmpfs file is extended without taking pages and takes them as they are first
// touched, so a board that fit the filesystem's free space when it was made can
// still fault later: three nodes of a development topology on one host each
// make a board sized to their own cache budget, each sees the whole filesystem
// free, and the first store past what the filesystem holds dies of SIGBUS in a
// memcpy, minutes into the run.  Committing the pages makes the filesystem
// answer here, with ENOSPC, and the creator leaves the segment unmade
// (lib_ams.board_too_big), which the txn answers with a board of its
// own memory.  Locking keeps the committed pages from being paged out under a
// reader holding a reference.  A lock the account's memlock limit refuses leaves
// the board committed, since the fault the commitment prevents does not depend
// on it, and is counted (n_board_unlocked) and logged at verbose level only: the
// default limit is 8MB, every topology declares a board larger than that, and a
// line on stderr at every start would report the host's default as a fault.  A segment with no file
// behind it -- the process pool a test's segments come from -- is committed by
// its allocation and needs nothing here.
//
// The board leaves a tenth of the tmpfs free.  A board is the largest segment
// a node makes and the last it makes at startup, and segments are made after
// it for as long as the node runs: a userproc's rings when an operator creates
// one, a bridge's when a peer connects.  A board that fit exactly would leave
// them nothing, and the first of them would then fail against a filesystem the
// node reports full, over a board that could have been a little smaller.  So
// the board is refused when committing it would leave less than the reserve,
// with the same report as one the filesystem cannot hold at all.
static bool BoardCommit(lib_ams::FShm &shm) {
    bool ret = true;
    if (ValidQ(shm.shm_file.fd)) {
        i64 size = i64(shm.shm_region.n_elems);
        i64 avail = lib_ams::GetShmAvail();
        i64 reserve = lib_ams::GetShmTotal() / 10;
        int err = avail - size < reserve ? ENOSPC : posix_fallocate(shm.shm_file.fd.value, 0, off_t(size));
        ret = err == 0;
        if (!ret) {
            prerr("lib_ams.board_too_big"
                  <<Keyval("grp",shm.grp_id)
                  <<Keyval("size",size)
                  <<Keyval("shm_avail",avail)
                  <<Keyval("shm_reserve",reserve)
                  <<Keyval("err",algo::FromErrno(err))
                  <<Keyval("comment","/dev/shm cannot commit the board's pages and keep a tenth free; raise the tmpfs size or shrink the board"));
        } else if (mlock(shm.shm_region.elems, size_t(shm.shm_region.n_elems)) != 0) {
            lib_ams::_db.trace.n_board_unlocked++;
            verblog("lib_ams.board_unlocked"
                    <<Keyval("grp",shm.grp_id)
                    <<Keyval("size",shm.shm_region.n_elems)
                    <<Keyval("err",algo::FromErrno(errno))
                    <<Keyval("comment","the board's pages are committed but not locked; raise the memlock limit (ulimit -l) to lock them"));
        }
    }
    return ret;
}

// Populate this process's page tables over the whole of board SHM, so no
// store or read into the board takes a page fault later.
//
// A board is committed when it is made, but committing fills the tmpfs, not the
// page tables of the processes that map it.  Each of those processes then takes
// a minor fault on its first touch of every 4K page.  A 6 GB board is 1.5M pages,
// and a txn filling one took about 600K faults inside the pass that
// reads the fabric, where it runs near its limit.  The pages already exist, so
// populating them costs this process its page tables and nothing else, and it
// moves the faults to startup.  A board with
// no file behind it is committed by its allocation and needs nothing here.  A
// kernel that refuses the advice leaves the board to fault on first touch, which
// is counted (n_board_unpopulated) and logged at verbose level.
static void BoardPopulate(lib_ams::FShm &shm) {
    int rc = 0;
    if (ValidQ(shm.shm_file.fd)) {
        rc = madvise(shm.shm_region.elems, size_t(shm.shm_region.n_elems), MADV_POPULATE_WRITE);
    }
    if (rc != 0) {
        lib_ams::_db.trace.n_board_unpopulated++;
        verblog("lib_ams.board_unpopulated"
                <<Keyval("grp",shm.grp_id)
                <<Keyval("size",shm.shm_region.n_elems)
                <<Keyval("err",algo::FromErrno(errno))
                <<Keyval("comment","the kernel refused to populate the board's page tables; each page faults on first touch"));
    }
}

// Consider what a sweep sees and how easily it reads a live ring as a dead one.
// Any process starting up scans every segment in /dev/shm, not just its own
// cluster's, and a free write lock looks like an abandoned ring.  But a ring is
// routinely created for a writer in another process -- a userproc's rings are
// made by its supervisor before the fork -- and the lock is taken only when that
// writer opens the ring, so a live segment sits unlocked for the whole handover;
// a metrics ring stays unlocked for the entire life of a child that publishes no
// sample.  Judging those by a timeout unlinks the file underneath a running
// cluster, which then writes into an unlinked inode while the next process to
// attach by name finds nothing there.
//
// The insight is that a segment has two halves of life and a different process
// answers for each.  A writer records its pid only once it holds LOCK_EX, so a
// header naming a writer whose lock is now free names a writer that has since
// exited: that is a proof of absence, and the ring is dead however recently it
// died.  Before any writer claims the ring no such proof exists, and what
// answers for it is its creator -- the process that owns the segment's existence
// and unlinks the file -- which protects the ring while it runs and releases it
// the moment it is gone.
//
// So a claimed ring is freed by its writer's departure whoever created it, and
// an unclaimed one is held by a living creator.  A segment whose header names no
// creator cannot be attributed to anything -- a file another program left under
// this suffix -- and only those fall back to an age, so that a burst of
// short-lived clusters cannot pile up unreclaimable files.
static bool OrphanSegmentQ(int fd, algo::strptr path) {
    bool ret = false;
    ams::Shmhdr hdr;
    ams::Shmhdr dflt;
    bool readable = pread(fd, &hdr, sizeof(hdr), 0) == (ssize_t)sizeof(hdr)
        && hdr.magic == dflt.magic;
    bool claimed = readable && hdr.writer_pid != 0;
    bool named = readable && hdr.creator_pid != 0;
    // ESRCH is the only answer that proves absence: EPERM names a live process
    // owned by another user
    bool creator_gone = named && kill(hdr.creator_pid, 0) == -1 && errno == ESRCH;
    bool unattributed_old = !named
        && algo::ToSecs(algo::CurrUnTime() - algo::ModTime(path)) > 30;
    if ((claimed || creator_gone || unattributed_old) && flock(fd, LOCK_EX|LOCK_NB) == 0) {
        ret = true;
    }
    return ret;
}
#endif

// Scan /dev/shm for orphaned ams segments and unlink them.  A segment is an
// orphan when no process holds its write lock and it is no longer being created
// (see OrphanSegmentQ) -- i.e. its writer crashed or was kill -9'd without
// unlinking, or it was created but never claimed by a writer.  Orphans are
// collected during the walk and unlinked after it, so the unlink never mutates
// the directory Dir_curs is iterating.
void lib_ams::CleanOldShmFiles() {
#ifndef WIN32 // not needed on Windows as named shm segments are not persistent
    algo::cstring orphans;
    int nclean=0;
    ind_beg(algo::Dir_curs,entry,"/dev/shm/*.ams") {
        int fd=open(Zeroterm(entry.pathname), O_RDONLY);
        if (fd != -1) {
            if (OrphanSegmentQ(fd, entry.pathname)) {
                orphans << entry.pathname << eol;
                nclean++;
            }
            close(fd);
        }
    }ind_end;
    ind_beg(algo::Line_curs,path,orphans) {
        (void)unlink(Zeroterm(tempstr() << path));
    }ind_end;
    if (nclean>0) {
        verblog("cleaned "<<nclean<<" orphaned .ams segments from /dev/shm");
    }
#endif
}

// -----------------------------------------------------------------------------

// return TRUE if shared memory region is attached to shm SHM.
bool lib_ams::ShmFdOpenQ(lib_ams::FShm &shm) {
#ifdef WIN32
    bool ret = shm.shm_handle != NULL;
#else
    bool ret = ValidQ(shm.shm_file.fd);
#endif
    return ret;
}

// -----------------------------------------------------------------------------

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
u32 lib_ams::ShmSize(u32 body, u32 maxmsg) {
    u32 ret = 1 << algo::CeilingLog2(u32_Max(body, maxmsg * 4));
    ret += 4096;// control header
    ret += maxmsg;// linear overflow past the body
    return ret;
}

// -----------------------------------------------------------------------------

// Create (or open) shared memory for reading/writing (as specified in FLAGS)
// and return success status
bool lib_ams::ShmCreate(lib_ams::FShm &shm, ams::ShmFlags flags) {
    i64 size = 0;
    bool ok = false;
#ifdef WIN32
    // create or open named shared memory segment
    shm.filename = tempstr() << "Global\\" << _db.file_prefix << "-" << shm.grp_id <<".ams";
    int access = write_Get(flags) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
    bool inherit = false;
    // try opening first
    shm.shm_handle = (u8*)OpenFileMapping(access,inherit,Zeroterm(shm.filename));
    // in write mode, try creating if needed
    if (!shm.shm_handle && write_Get(flags)) {
        shm.shm_handle = (u8*)CreateFileMapping(
                                                INVALID_HANDLE_VALUE,        // use paging file
                                                NULL,                        // default security
                                                PAGE_READWRITE,              // read/write access
                                                0,                           // maximum object size (high-order DWORD)
                                                lib_ams::_db.shmem_size,     // maximum object size (low-order DWORD)
                                                Zeroterm(shm.filename));  // name of mapping object (section name, not a file)
    }
    // if the object has been already created CreateFileMapping() returns existing handle
    // and does not change size, GetLastError() indicates ERROR_ALREADY_EXISTS.
    // This helps us to avoid locks.
    ok = bool(shm.shm_handle) && (!write_Get(flags) || !GetLastError());
    // map section to virtual memory region
    if (ok) {
        shm.shm_region.elems = (u8*)MapViewOfFile(shm.shm_handle,    // handle of shm section
                                                  access,           // read/write permission
                                                  0,                                                         // file offset high
                                                  0,                                                         // file offset low
                                                  0);                                                        // length - 0 means up to end of file
        ok = shm.shm_region.elems != NULL;
    }
    // determine region size
    if (ok) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(shm.shm_region.elems,&mbi,sizeof mbi)==sizeof mbi) {
            size = mbi.RegionSize;
        }
    }
    // TODO move duplicate code somewhere
    // create a 'working area mask'
    // 1<<mask is a region of size that's a power of two, and has room for at least 2
    // messages beyond it.
    // next message is written at offset `offset & mask` to memory
    ok = ok && i32(size) >= lib_ams::_db.max_msg_size*2;
    // cleanup in case of error - TBD needed?
    if (!ok && shm.shm_region.elems) {
        UnmapViewOfFile(shm.shm_region.elems);
        shm.shm_region.elems = NULL;
    }
    if (!ok && shm.shm_handle) {
        CloseHandle(shm.shm_handle);
        shm.shm_handle = NULL;
    }
#else // linux
    //int create_flags = (write_Get(flags) ? (O_RDWR|O_CREAT) : O_RDONLY);
    int create_flags = (write_Get(flags) ? (O_RDWR|O_CREAT) : O_RDWR);
    //int mode = write_Get(flags) ? S_IRUSR | S_IWUSR : S_IRUSR;
    int mode = S_IRUSR | S_IWUSR;
    // POSIX says / character in argument to shm_open is implementation-defined
    // Practically, shm_open fails on Linux if / is used.
    shm.filename = ShmFileName(shm.grp_id);
    // A ring's channel count is declared on the record and nowhere else, so the
    // creator adds the header pages past the first that the channels need to
    // whatever size it was asked for, and the body keeps the size it was planned
    // at.  A size and a count that must agree are then one input each.
    i64 extra = lib_ams::BoardQ(shm) ? 0 : i64(ShmControlSize(shm.max_channel)) - 4096;
    if (_db.file_prefix == "") {
        size = lib_ams::_db.shmem_size + extra;
        shm.shm_region = algo::memptr((u8*)algo_lib::lpool_AllocMem(size),size);
        shm.created=true;
        ok = shm.shm_region.elems != NULL;
    } else {
        shm.shm_file.fd.value = shm_open(Zeroterm(shm.filename), create_flags, mode);
        ok = ValidQ(shm.shm_file.fd);
        if (ok) {
            size = algo::GetFileSize(shm.shm_file.fd);
        }
        // Only the process that brings a segment into being chooses its size;
        // everyone else reads it off the file it opens.  That is what lets a
        // bridge's size be a property of the userproc rather than of the
        // processes that map it: the supervisor states it here, and the
        // attach point that opens the same file afterwards inherits it
        // without being told.  A segment nobody sized takes the process-wide
        // default, which is every segment the topology makes.
        if (ok && size == 0) {
            shm.created=true;
            size = (shm.size > 0 ? shm.size : lib_ams::_db.shmem_size) + extra;
            ok = ok && ftruncate(shm.shm_file.fd.value, size)==0;
        }
        // map the region
        if (ok) {
            //int prot = write_Get(flags) ? PROT_READ | PROT_WRITE : PROT_READ;
            // #AL# map as r/w because of communications header area
            int prot = PROT_READ | PROT_WRITE;
            void *result = mmap(NULL, size, prot, MAP_SHARED, shm.shm_file.fd.value, 0);
            if (result == MAP_FAILED) {
                ok=false;
            } else {
                shm.shm_region = algo::memptr((u8*)result,size);
            }
        }
    }
#endif // win/linux
    ams::Shmhdr *shmhdr = (ams::Shmhdr*)shm.shm_region.elems;
    if (shmhdr) {
        // A board is an arena of chunks, not a ring, so it takes no offset
        // mask and its woff stays at zero for the life of the segment.  What keeps
        // it out of the read path is that it is never put on cd_poll_read and
        // never takes a reader member: PeekMsg reads its reader's offset before it
        // tests anything, so a board is not a segment that reads as empty but one
        // that is never asked.  Its creator writes only the header here, and
        // every process that maps it populates its page tables below.
        bool board = lib_ams::BoardQ(shm);
        shm.c_shmhdr = shmhdr;
        // A segment's largest message is the segment's own property, so the
        // creator declares it on the record and an opener reads back what the
        // segment was built with.  Undeclared, a segment carries the largest
        // message this process handles at all; a board's creator declares its
        // chunk size here, since a chunk is the largest message a board carries.
        if (shm.max_msg_size == 0) {
            shm.max_msg_size = lib_ams::_db.max_msg_size;
        }
        shm.offset_mask = 0;
        u32 control = board ? 4096 : ShmControlSize(shm.max_channel);
        if (!board) {
            i64 span = i64(size) - control - shm.max_msg_size;// what is left for the body
            shm.offset_mask = span > 0 ? (1 << algo::FloorLog2(u32(span))) - 1 : 0;
        }
        if (shm.created && board && !BoardCommit(shm)) {
            ok = false;
        } else if (shm.created) {
            memset(shm.shm_region.elems,0,board ? 4096 : size);// touch all bytes
            new (shm.c_shmhdr) ams::Shmhdr; // defaults
            shmhdr->grp_id       = shm.grp_id;
            shmhdr->tot_size     = size;
            shmhdr->offset_mask  = shm.offset_mask;
            shmhdr->max_shmember = board ? 0 : lib_ams::RingShmemberN();
            shmhdr->datastart    = control;
            // The shm channels take the rest of the control pages after the
            // member table, as many as the creator declared and never fewer
            // than one page holds.  A board has no reader offsets and no channels.
            shmhdr->max_channel  = board ? 0 : (control - RingChannelStart()) / u32(sizeof(ams::Shmchannel));
            shmhdr->max_msg_size = shm.max_msg_size;
            // The creator owns the segment's existence: it is the process that
            // unlinks the file, and while it lives no sweep may reclaim the
            // file underneath it (see OrphanSegmentQ).  A ring is routinely
            // created for a writer in another process -- a userproc's rings are
            // made by its supervisor before the fork -- so the writer's pid
            // cannot answer for the segment during the gap before that writer
            // attaches, and this can.
            shmhdr->creator_pid  = getpid();
            // Persist the writer's signaled bit so future readers know they
            // must run in signaled mode to receive SIGRTMIN wakeups.
            shmhdr->signaled     = signaled_Get(flags);
        } else {
            ams::Shmhdr dflt;
            if (shmhdr->magic != dflt.magic) {
                prerr(shm.grp_id<<": Invalid segment format");
                ok=false;
            }
            shm.max_msg_size = shmhdr->max_msg_size;
            shm.offset_mask = shmhdr->offset_mask;
        }
        // The body must hold four of this ring's messages, whether this process
        // built the segment or inherited it, because a writer that opens an
        // undersized ring corrupts its readers silently: the write limit lands
        // behind the slowest reader, so the writer stops holding anything back
        // and laps that reader inside a body a few kilobytes wide, and every
        // reader then parses bytes overwritten under it.  Refusing to attach
        // turns that into a startup failure naming the ring.
        //
        // A board's chunk is its largest message, and the geometry a reader walks
        // it by, so an opener learns it from the header as the creator declared it.
        if (board) {
            shm.chunk_size = u32(shm.max_msg_size);
        }
        if (ok && board) {
            BoardPopulate(shm);
        }
        // A board is exempt because it is not a ring.  Nothing wraps inside it:
        // it is an arena of chunks whose occupancy the writer tracks in its own
        // memory, so it has no body for a write limit to land behind and no
        // offset for one reader to lap another at.  Measured against the ring
        // rule it would fail every time, its offset mask being zero.
        if (ok && !board && i64(shm.offset_mask) + 1 < i64(shm.max_msg_size) * 4) {
            prerr(shm.grp_id<<": segment too small for the messages it carries"
                  <<Keyval("size",size)<<Keyval("max_msg_size",shm.max_msg_size)
                  <<Keyval("body",i64(shm.offset_mask)+1)
                  <<Keyval("need",lib_ams::ShmSize(0,u32(shm.max_msg_size))));
            ok=false;
        }
        shm.c_data = shm.shm_region.elems + shmhdr->datastart;
        //amslog((shm.created ? "initialized" : "loaded") << " ctl block "<<*shmhdr);
    }
    if (!ok) {
        lib_ams::ShmClose(shm);
    }
    ok = shm.shm_region.elems != NULL;
    return ok;
}

// -----------------------------------------------------------------------------

// Claim this segment's one writer.  Linux shm descriptors are regular tmpfs
// files and support flock.  Darwin's POSIX shm descriptors support neither
// flock nor fcntl record locks, so claim the aligned writer pid in the shared
// header atomically.  A dead owner may be replaced; a live one keeps the claim.
static bool LockWriter(lib_ams::FShm &shm) {
#ifdef __APPLE__
    i32 pid = getpid();
    i32 *owner = &shm.c_shmhdr->writer_pid;
    i32 found = __sync_val_compare_and_swap(owner, 0, pid);
    bool ok = found == 0 || found == pid;
    if (!ok && kill(found, 0) == -1 && errno == ESRCH) {
        ok = __sync_bool_compare_and_swap(owner, found, pid);
    }
    if (!ok) {
        errno = EWOULDBLOCK;
    }
    return ok;
#else
    return flock(shm.shm_file.fd.value, LOCK_EX|LOCK_NB) == 0;
#endif
}

// -----------------------------------------------------------------------------

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
bool lib_ams::ShmOpen(lib_ams::FShm &shm, ams::ShmFlags flags) {
    algo::strptr step;
    algo::strptr comment;
    int err = 0;
    bool ok = ShmFdOpenQ(shm);
    // create if opening for writing
    if (!ok) {
        ok = ShmCreate(shm,flags);
        if (!ok) {
            step = "map";
            err = errno;
            comment = "segment is missing or cannot be mapped";
        }
    }
    // find me in the list of readers.. and open
    if (ok && read_Get(flags) && !read_Get(shm.flags) && lib_ams::BoardQ(shm)) {
        // A board reader takes no slot in the member table, because it has
        // nothing to record there.  A member slot holds one reader's consume
        // position, and a board reader has none: it advances no offset, and the
        // writer learns what it has finished with from the lane the reference
        // arrived on.  So the reader maps the segment, reads it, and registers
        // on its read channel instead -- which is the lane, where its position
        // does mean something.  The board's member table is sized to zero to say
        // so, which also refuses any later attempt to register in it.
        read_Set(shm.flags,true);
    } else if (ok && read_Get(flags) && !read_Get(shm.flags)) {
        shm.c_reader = FindReadShmember(shm,lib_ams::_db.proc_id);
        if (!shm.c_reader && shm.created) {
            // register this reader if we're also the writer
            shm.c_reader=AddReadShmember(shm,lib_ams::_db.proc_id);
        }
        if (shm.c_reader) {
            shm.c_reader->pid=getpid();
            read_Set(shm.flags,true);
            cd_poll_read_Insert(shm);
        } else {
            bool table_full = shm.c_shmhdr && shm.c_shmhdr->n_shmember >= shm.c_shmhdr->max_shmember;
            ok = false;
            step = "read";
            comment = table_full ? "member table is full" : "the writer did not register this proc as a reader";
        }
    }
    // attach write member
    if (ok && write_Get(flags) && !shm.locked) {
        if (ShmFdOpenQ(shm)) {
            ok = LockWriter(shm);
            if (!ok) {
                step = "write";
                err = errno;
                comment = "another process holds the write lock";
            }
        }
        if (ok) {
            shm.locked=true;
            shm.c_shmhdr->writer_pid = getpid();
            write_Set(shm.flags,true);
        }
    }
    UpdateBudget(shm);
    if (!ok) {
        tempstr err_s;
        if (err != 0) {
            err_s << strerror(err);
        }
        prerr("lib_ams.attach_error"
              <<Keyval("proc",lib_ams::_db.proc_id)
              <<Keyval("grp",shm.grp_id)
              <<Keyval("step",step)
              <<Keyval("err",err_s)
              <<Keyval("comment",comment));
    }
    return ok;
}

// Update budget for SHM
// Return TRUE if the WRITELIMIT was updated.
// (WRITELIMIT is the point beyond which no message can be written
// because doing so would overwrite data not yet consumed by one of the read members.)
// A board has no write budget to update.  It is not a ring, so it has no
// writelimit and no member offsets to derive one from; its space is tracked by
// chunk, in the writer's own memory.
bool lib_ams::UpdateBudget(lib_ams::FShm &shm) {
    bool ret=false;
    if (write_Get(shm.flags) && !lib_ams::BoardQ(shm)) {
        u64 new_offset = lib_ams::SlowestReaderOffset(shm) + (shm.offset_mask+1) - shm.max_msg_size*2;
        ret = u64_Update(shm.writelimit, new_offset);
        shm.n_wlim_update += ret;
        if (ret) {
            ind_beg(_db_zd_fdin_curs,fdin,lib_ams::_db) {
                if (!cd_fdin_read_InLlistQ(fdin) && ch_N(_db.expect_str)==0) {
                    cd_fdin_read_Insert(fdin);
                }
            }ind_end;
        }
    }
    return ret;
}

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
bool lib_ams::WritableQ(lib_ams::FShm &shm) {
    return write_Get(shm.flags) && shm.c_shmhdr != NULL;
}

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
bool lib_ams::WaitBudget(lib_ams::FShm &shm, bool block) {
    bool ret = false;
    if (WritableQ(shm)) {
        ret = shm.c_shmhdr->woff < shm.writelimit;
        if (!ret) {
            UpdateBudget(shm);
            ret = shm.c_shmhdr->woff < shm.writelimit;
        }
        if (!ret && block) {
            // The reader this spin waits on may be parked on a message this
            // pass wrote, with its wakeup still owed; send it first, or the
            // spin waits on a reader nobody woke.
            FlushWake();
            shm.c_shmhdr->nblock++;
            u64 i=0;
            do {
                UpdateBudget(shm);
                ret = shm.c_shmhdr->woff < shm.writelimit;
                if ((++i & ((1<<22)-1)) == 0 && algo_lib::LogcatOnQ(algo_lib_logcat_slowness)) {
                    amscat(slowness, "writing to "<<shm.grp_id<<": apparent deadlock, flags "<<shm.flags);
                }
            } while (!ret);
        }
        if (!ret) {
            shm.c_shmhdr->nnobudget++;
        }
    }
    return ret;
}

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
void *lib_ams::ReserveWrite(lib_ams::FShm &shm, int length) {
    void *ret = NULL;
    if (WritableQ(shm) && length <= shm.max_msg_size) {
        bool ok = shm.c_shmhdr->woff < shm.writelimit;
        if (!ok) {
            UpdateBudget(shm);
            ok = shm.c_shmhdr->woff < shm.writelimit;
        }
        if (!ok && lib_ams::_db.signaled) {
            ok = ParkWriter(shm);
        }
        if (ok) {
            ret = MsgAtOffset(shm,shm.c_shmhdr->woff);
        } else {
            shm.c_shmhdr->nnobudget++;
        }
    }
    return ret;
}

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
void *lib_ams::BeginWrite(lib_ams::FShm &shm, int length) {
    void *ret = NULL;
    if (lib_ams::zd_outmsg_N(shm) > 0) {
        lib_ams::OutmsgFlush(shm);
    }
    if (lib_ams::zd_outmsg_N(shm) == 0) {
        ret = lib_ams::ReserveWrite(shm, length);
    }
    return ret;
}

// Begin writing message of length LENGTH, blocking until the ring has room.
// WaitBudget busy-waits for a max_msg_size slot; BeginWrite then returns the
// write pointer (or NULL for a too-big message, which it rejects outright).
void *lib_ams::BeginWriteBlock(lib_ams::FShm &shm, int length) {
    WaitBudget(shm, true);
    return lib_ams::BeginWrite(shm, length);
}

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
void lib_ams::EndWrite(lib_ams::FShm &shm, void *ptr, int len) {
    u64 woff = AddOffset(shm.c_shmhdr->woff, len);
    sfence();
    shm.c_shmhdr->woff = woff;
    if (shm.c_reader) {
        UnparkReader(shm);
    }
    if (lib_ams::_db.signaled) {
        WakeReader(shm);
    }
    if (algo_lib::LogcatOnQ(algo_lib_logcat_amswrite)) {
        lib_ams::TraceMsg(&algo_lib_logcat_amswrite, shm, (ams::MsgHeader*)ptr);
    }
}

// Write message MSG to SHM, non-blocking.  Return TRUE on success; FALSE
// when the ring has no budget (the message is dropped -- the caller decides
// whether to retry, unread, or discard).
//
// A message the ring cannot hold is copied into the writer's message board
// instead, and the ring carries a reference to it.  The board path answers the
// same way the ring does -- FALSE when it cannot take the message, and the
// board space is given back -- so a caller sees one contract whichever way the
// message travels, and a writer with no board rejects an oversize message.
bool lib_ams::WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    bool ret = false;
    int len = msg.length;
    lib_ams::FShm *board = len > shm.max_msg_size ? lib_ams::BoardOf(shm) : NULL;
    if (len > shm.max_msg_size) {
        void *dst = board ? lib_ams::BoardAlloc(*board, len) : NULL;
        if (dst) {
            memcpy(dst, &msg, size_t(len));
            ret = lib_ams::BoardSend(shm, *(ams::MsgHeader*)dst);
            if (!ret) {
                lib_ams::BoardTrim(*board, dst, 0);
            }
        }
    } else if (void *ptr = lib_ams::BeginWrite(shm,len)) {
        memcpy(ptr, &msg, len);
        lib_ams::EndWrite(shm,ptr,len);
        ret = true;
    }
    return ret;
}

// Write message MSG to SHM, blocking until the ring has room.  WaitBudget
// busy-waits for a max_msg_size slot, then WriteMsg performs the write.
// Always succeeds except for a too-big message, or a ring that is closed and
// therefore never gains room.  Use only where dropping would be incorrect --
// correctness, not config, picks this variant.
bool lib_ams::WriteMsgBlock(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    WaitBudget(shm, true);
    return lib_ams::WriteMsg(shm, msg);
}

void lib_ams::shm_file_Cleanup(lib_ams::FShm &shm) {// fcleanup:lib_ams.FShm.shm_file
    if (shm.locked) {
#ifdef __APPLE__
        if (shm.c_shmhdr) {
            (void)__sync_bool_compare_and_swap(&shm.c_shmhdr->writer_pid, i32(getpid()), 0);
        }
#else
        (void)flock(shm.shm_file.fd.value, LOCK_UN);
#endif
        shm.locked=false;
    }
    // unmap section from process address space for reader and writer as well
    if (shm.shm_region.elems) {
        munmap(shm.shm_region.elems, shm.shm_region.n_elems);
        Refurbish(shm.shm_region);
    }
#ifdef WIN32
    // close named section
    // note that section will be actually removed along with last reference to it
    if (shm.shm_handle) {
        CloseHandle(shm.shm_handle);
        shm.shm_handle = NULL;
    }
#else
    // Only the segment's creator unlinks the file; an opener closes its
    // mapping and fd, nothing else.  A shm name is stable across a userproc's
    // incarnations, so a non-creator that unlinks on teardown -- e.g. the
    // gateway tearing down the OLD incarnation's bridge (a /sys/req delete it
    // processes asynchronously) -- would remove the NEW incarnation's freshly
    // created segment by name, and its re-open would then recreate the file
    // racing the child's read of the not-yet-written header
    // ("Invalid segment format").
    if (shm.filename != "") {
        if (shm.created) {
            shm_unlink(Zeroterm(shm.filename));
        }
        shm.filename = "";
    }
#endif
}

// -----------------------------------------------------------------------------

// Close shm: unmap the region, drop the fd, and unlink the file if this
// process created it.  Send the wakeups the pass owes first, and clear the
// sleeping flag if a reader was sleeping.
// The record itself stays in the shm table so the next incarnation under the
// same grp reuses it.
void lib_ams::ShmClose(lib_ams::FShm &shm) {
    // A wakeup owed on this ring is sent while the mapping still stands, since
    // its peer learns of the last messages from nothing else.
    FlushWake();
    if (shm.c_reader && shm.c_reader->sleeping) {
        shm.c_reader->sleeping = 0;
    }
    // A channel handle points into the control page being unmapped, so it goes
    // with the mapping.
    while (lib_ams::FChannel *channel = lib_ams::zd_channel_First(shm)) {
        lib_ams::channel_Delete(*channel);
    }
    // A closing lane's readers stop moving, so the chunks its references hold
    // are given back here; a closing board's chunks name bytes of the mapping
    // that is going away, so they are forgotten along with every lane's
    // references into them.
    if (lib_ams::BoardQ(shm)) {
        lib_ams::BoardReset(shm);
    } else {
        lib_ams::ChunkrefReleaseAll(shm);
    }
    shm_file_Cleanup(shm);
    algo::Refurbish(shm.shm_region);
    algo::Refurbish(shm.shm_file);
    // creator-ship ends with the file: a reused row (a userproc's next
    // incarnation under the same grp) re-derives it at the next ShmCreate
    shm.created=false;
    read_Set(shm.flags,false);
    write_Set(shm.flags,false);
    shm.c_shmhdr=NULL;
    shm.c_data=NULL;
    // c_reader and c_cur_msg point into the just-unmapped shm region; clear them
    // so the signaled-mode sleeping check in cd_poll_read_Step and the read loop
    // don't dereference freed memory.
    shm.c_reader=NULL;
    shm.c_cur_msg=NULL;
    // cached_woff is a read-side cache of the writer offset.  Reset it: if this
    // shm's grp_id is later reopened on the same record (a userproc reusing its
    // proc_id across incarnations, issue #2124), a stale cache would let PeekMsg
    // read past the fresh ring's real write position into uninitialized memory
    // and hand up a garbage message.
    shm.cached_woff=0;
    cd_poll_read_Remove(shm);
    // a parked reader is one this process is still waiting on; the wait ends
    // with the mapping, and the park list must not outlive the c_reader slot
    // its wake path dereferences
    zd_park_read_Remove(shm);
}

// -----------------------------------------------------------------------------

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
ams::Shmember *lib_ams::AddReadShmember(lib_ams::FShm &shm, ams::ProcId proc_id) {
    ams::Shmember *ret=FindReadShmember(shm,proc_id);
    if (!ret && shm.c_shmhdr->n_shmember < shm.c_shmhdr->max_shmember) {
        int i = shm.c_shmhdr->n_shmember++;
        ret=shmember_Find(shm,i);
        ret->grpmember_id.grp_id = shm.grp_id;
        ret->grpmember_id.proc_id = proc_id;
        r_Set(ret->grpmember_id.flags,true);
    } else if (!ret) {
        prerr("lib_ams.shmember_error"
              <<Keyval("proc",lib_ams::_db.proc_id)
              <<Keyval("grp",shm.grp_id)
              <<Keyval("reader",proc_id)
              <<Keyval("n_shmember",shm.c_shmhdr->n_shmember)
              <<Keyval("comment","member table is full; this reader gets no slot and the ring will not deliver to it"));
    }
    UpdateBudget(shm);
    return ret;
}

// -----------------------------------------------------------------------------

ams::Shmember *lib_ams::FindReadShmember(lib_ams::FShm &shm, ams::ProcId proc_id) {
    ams::Shmember *ret=NULL;
    ind_beg(shm_c_shmember_curs,shmember,shm) {
        if (shmember.grpmember_id.proc_id == proc_id) {
            ret=&shmember;
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

void lib_ams::CloseAllShms() {
    ind_beg(_db_shm_curs,shm,_db) {
        ShmClose(shm);
    }ind_end;
    shm_RemoveAll();
}

// -----------------------------------------------------------------------------

// Evaluate current budget.  A ring that cannot be written has no budget: zero,
// rather than a read through its released header.
u64 lib_ams::GetBudget(lib_ams::FShm &shm) {
    return WritableQ(shm) ? algo::u64_SubClip(shm.writelimit, shm.c_shmhdr->woff) : 0;
}

// Check if thre is room in SHM to write at least 2 messages, plus EXTRA.
// The function re-samples current budget if needed.  A ring that cannot be
// written has no room, and no counter to charge the miss to.
bool lib_ams::HasBudgetQ(lib_ams::FShm &shm, u32 extra DFLTVAL(0)) {
    bool ret = false;
    if (WritableQ(shm)) {
        ret = shm.c_shmhdr->woff + extra < shm.writelimit;
        if (!ret) {
            UpdateBudget(shm);
            ret = shm.c_shmhdr->woff + extra < shm.writelimit;
        }
        if (!ret) {
            shm.c_shmhdr->nnobudget++;
        }
    }
    return ret;
}

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
ams::MsgHeader *lib_ams::PeekMsg(lib_ams::FShm &shm) {
    ams::MsgHeader *ret = NULL;
    u64 roff=shm.c_reader->offset;
    u64 woff=shm.cached_woff;
    // reload woff if needed
    if (roff + sizeof(ams::MsgHeader) >= woff) {
        u64 hdr_woff = shm.c_shmhdr->woff;
        if (hdr_woff != woff) {
            lfence();
            shm.cached_woff = hdr_woff;
            woff = hdr_woff;
        }
    }
    if (roff + sizeof(ams::MsgHeader) <= woff) {
        ams::MsgHeader *msg = MsgAtOffset(shm,roff);
        if (roff + msg->length <= woff) {
            ret = msg;
        }
    }
    return ret;
}

// Check all shms (that are not already readable) for readability and
// transfer readable shms to the read heap with correct sort key.
// In signaled mode, idle shms are removed from the poll loop; the reader
// sets sleeping=1 on the shmember so that the writer can wake it via kill().
void lib_ams::cd_poll_read_Step() {
    int n=cd_poll_read_N();
    // A message handler (h_amsmsg_Call) can tear down a bridge conn, which
    // ShmCloses its inbound shm -- clearing c_shmhdr/c_reader and removing shms
    // from cd_poll_read in the middle of this walk.  Re-check the list is
    // non-empty each iteration, and after a handler runs do not touch a shm it
    // just closed (c_shmhdr cleared), so the poll loop never dereferences a
    // freed/unmapped shm.
    for (int i=0; i<n && cd_poll_read_N()>0; i++) {
        lib_ams::FShm &shm = *cd_poll_read_First();
        _db.c_cur_shm=&shm;
        shm.c_cur_msg = PeekMsg(shm);
        if (shm.c_cur_msg) {
            for (int j=0; j<shm.burst && shm.c_cur_msg; j++) {
                if (algo_lib::LogcatOnQ(algo_lib_logcat_amsread)) {
                    TraceMsg(&algo_lib_logcat_amsread,shm,shm.c_cur_msg);
                }
                int len = shm.c_cur_msg->length;
                // A board reference stands in the ring for the message it names,
                // and the handler is given the message rather than the reference,
                // so nothing above this loop can tell which way a message
                // travelled.  The ring still advances by the reference's own
                // length: what occupies the ring is the reference.
                ams::BoardrefMsg *boardref = ams::BoardrefMsg_Castdown(*shm.c_cur_msg);
                ams::MsgHeader *payload = boardref ? lib_ams::BoardResolve(shm,*boardref) : shm.c_cur_msg;
                if (payload) {
                    h_amsmsg_Call(shm,*payload);
                } else {
                    _db.trace.n_board_badref++;
                }
                if (shm.c_shmhdr && shm.c_cur_msg) {
                    shm.c_reader->offset = AddOffset(shm.c_reader->offset, len);
                    shm.c_cur_msg = PeekMsg(shm);
                } else {
                    shm.c_cur_msg = NULL;
                }
            }
            if (shm.c_shmhdr) {
                // A writer woken for one message's room writes one message
                // and parks again, so its wake waits until half the ring is
                // free and the writer can refill half of it.
                if (lib_ams::_db.signaled && HalfDrainedQ(shm)) {
                    WakeWriter(shm);
                }
                cd_poll_read_RotateFirst();
            }
        } else if (shm.c_shmhdr->eof) {
            // writer exited and queue is drained — permanently stop polling
            cd_poll_read_RemoveFirst();
        } else if (write_Get(shm.flags)) {
            // remove reader from poll loop if this process is the writer, i.e.
            // an unsolicited message cannot show up
            cd_poll_read_RemoveFirst();
        } else if (lib_ams::_db.signaled) {
            // Park this ring here, as its own empty poll finds it: ParkReader
            // sets the sleeping flag and re-checks under a barrier, which is the
            // whole defense against a wakeup lost between the peek and the store.
            if (ParkReader(shm)) {
                cd_poll_read_RemoveFirst();
            } else {
                cd_poll_read_RotateFirst();
            }
        } else {
            cd_poll_read_RotateFirst();
        }
        _db.c_cur_shm=NULL;
    }
}


ams::Shmember *lib_ams::shmember_Find(lib_ams::FShm &shm, int i) {
    ams::Shmember *ret=NULL;
    if (u32(i) < shm.c_shmhdr->n_shmember) {
        ret = ((ams::Shmember*)(shm.c_shmhdr+1)) + i;
    }
    return ret;
}

void lib_ams::shm_c_shmember_curs_Next(shm_c_shmember_curs &curs) {
    curs.index++;
}

void lib_ams::shm_c_shmember_curs_Reset(shm_c_shmember_curs &curs, lib_ams::FShm &parent) {
    curs.shm = &parent;
    curs.index = 0;
    curs.limit = curs.shm->c_shmhdr ? curs.shm->c_shmhdr->n_shmember : 0;
}

bool lib_ams::shm_c_shmember_curs_ValidQ(shm_c_shmember_curs &curs) {
    return curs.index < curs.limit;
}

ams::Shmember& lib_ams::shm_c_shmember_curs_Access(shm_c_shmember_curs &curs) {
    return *shmember_Find(*curs.shm,curs.index);
}
