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
// Source: cpp/lib_ams/shm.cpp
//

#include "include/lib_ams.h"
#ifndef WIN32
#include <sys/statvfs.h>
#ifdef __APPLE__
#include <sys/posix_shm.h>
#endif
#endif

// -----------------------------------------------------------------------------

#ifndef WIN32
// Derive the POSIX shared-memory object name for GRP_ID.  Darwin limits these
// names to PSHMNAMLEN (currently 31) characters, while a descriptive AMS name
// can be longer.  Keep the readable spelling when it fits and compact only an
// overlong name, using two different CRC polynomials so the full cluster and
// group identity still has a 64-bit fingerprint.
static tempstr ShmFilename(ams::GrpId grp_id) {
    tempstr ret;
    ret << lib_ams::_db.file_prefix
        << (lib_ams::_db.file_prefix == "" ? "" : "-")
        << grp_id << ".ams";
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
    // the name ShmOpen gives the segment, which is the one on disk
    tempstr filename = tempstr() << "/dev/shm/" << ShmFilename(grp_id);
    struct stat st;
    if (stat(Zeroterm(filename), &st) == 0) {
        ret = i64(st.st_blocks) * 512;
    }
#endif
    return ret;
}

#ifndef WIN32
// TRUE when the segment named PATH (open on FD) is a reclaimable orphan.  For a
// segment this host can attribute to a process, the answer rests on a proof that
// some process is gone, never on the segment's age.
//
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
    shm.filename = ShmFilename(shm.grp_id);
    if (_db.file_prefix == "") {
        size = lib_ams::_db.shmem_size;
        shm.shm_region = algo::memptr((u8*)algo_lib::lpool_AllocMem(lib_ams::_db.shmem_size),size);
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
            size = shm.size > 0 ? shm.size : lib_ams::_db.shmem_size;
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
        // A board is an array of fixed slots, not a ring, so it takes no offset
        // mask and its woff stays at zero for the life of the segment.  What keeps
        // it out of the read path is that it is never put on cd_poll_read and
        // never takes a reader member: PeekMsg reads its reader's offset before it
        // tests anything, so a board is not a segment that reads as empty but one
        // that is never asked.  Only its header is pre-touched: a board is sized
        // for the sum of its readers' allowances, and pre-faulting that would
        // commit at startup what the traffic may never use.
        bool board = lib_ams::BoardQ(shm);
        shm.c_shmhdr = shmhdr;
        // A segment's largest message is the segment's own property, so the
        // creator declares it on the record and an opener reads back what the
        // segment was built with.  Undeclared, a lane carries the largest
        // message this process handles at all, and a board takes the slot size
        // this process gives the boards it creates -- a board's slot size is
        // its largest message, which is what lets one number serve both.
        if (shm.max_msg_size == 0) {
            shm.max_msg_size = board ? lib_ams::_db.board_slot_size : lib_ams::_db.max_msg_size;
        }
        shm.offset_mask = 0;
        if (!board) {
            i64 span = i64(size) - 4096 - shm.max_msg_size;// what is left for the body
            shm.offset_mask = span > 0 ? (1 << algo::FloorLog2(u32(span))) - 1 : 0;
        }
        if (shm.created) {
            memset(shm.shm_region.elems,0,board ? 4096 : size);// touch all bytes
            new (shm.c_shmhdr) ams::Shmhdr; // defaults
            shmhdr->grp_id       = shm.grp_id;
            shmhdr->tot_size     = size;
            shmhdr->offset_mask  = shm.offset_mask;
            shmhdr->max_shmember = board ? 0 : 16;
            shmhdr->datastart    = 4096;
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
        // A board is exempt because it is not a ring.  Nothing wraps inside it:
        // it is an array of fixed slots whose occupancy the writer tracks in a
        // free list, so it has no body for a write limit to land behind and no
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
// the slot free list instead.
bool lib_ams::UpdateBudget(lib_ams::FShm &shm) {
    bool ret=false;
    if (write_Get(shm.flags) && !lib_ams::BoardQ(shm)) {
        // with no readers attached, the slowest reader is conceptually at the ring start
        u64 min_roff = shm.c_shmhdr->n_shmember ? (ULLONG_MAX/2) : 0;
        ind_beg(shm_c_shmember_curs,shmember,shm) {
            u64_UpdateMin(min_roff,shmember.offset);
        }ind_end;
        u64 new_offset = min_roff + (shm.offset_mask+1) - shm.max_msg_size*2;
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
// whether there is room.  When BLOCK is true and there is none, poll all
// control shms (zd_ctlin list) for heartbeats indefinitely until a reader
// drains the ring and only then return -- so a blocking write never drops.
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
void *lib_ams::BeginWrite(lib_ams::FShm &shm, int length) {
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

// Begin writing message of length LENGTH, blocking until the ring has room.
// WaitBudget busy-waits for a max_msg_size slot; BeginWrite then returns the
// write pointer (or NULL for a too-big message, which it rejects outright).
void *lib_ams::BeginWriteBlock(lib_ams::FShm &shm, int length) {
    WaitBudget(shm, true);
    return lib_ams::BeginWrite(shm, length);
}

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
// A message the ring cannot hold goes to the writer's message board instead,
// and the ring carries a reference to it.  The board path answers the same way
// the ring does -- FALSE when it cannot take the message -- so a caller sees one
// contract whichever way the message travels, and a writer with no board keeps
// rejecting an oversize message as before.
bool lib_ams::WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    bool ret = false;
    int len = msg.length;
    if (len > shm.max_msg_size && lib_ams::BoardOf(shm)) {
        ret = lib_ams::BoardPost(shm, msg);
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
// process created it.  Clear the sleeping flag if a reader was sleeping.
// The record itself stays in the shm table so the next incarnation under the
// same grp reuses it.
void lib_ams::ShmClose(lib_ams::FShm &shm) {
    if (shm.c_reader && shm.c_reader->sleeping) {
        shm.c_reader->sleeping = 0;
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
ams::MsgHeader *lib_ams::PeekMsg(lib_ams::FShm &shm) {
    ams::MsgHeader *ret = NULL;
    u64 roff=shm.c_reader->offset;
    u64 woff=shm.cached_woff;
    // reload woff if needed
    if (roff + sizeof(ams::MsgHeader) >= woff) {
        lfence();
        woff = shm.c_shmhdr->woff;
        shm.cached_woff = woff;
    }
    if (roff + sizeof(ams::MsgHeader) <= woff) {
        ams::MsgHeader *msg = MsgAtOffset(shm,roff);
        if (roff + msg->length <= woff) {
            ret = msg;
        }
    }
    return ret;
}

// Called by the client
// to avoid reading current message
void lib_ams::StopReading(lib_ams::FShm &shm) {
    UnreadMsg();
    cd_poll_read_Remove(shm);
}

// -----------------------------------------------------------------------------

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
                if (lib_ams::_db.signaled) {
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
            // Park this shm per-shm, here, not in the drained-list sweep below:
            // a process whose poll list never empties never reaches
            // cd_poll_read_N()==0, so the sweep alone can't recover the miss.
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
    // About to sleep (poll list drained): re-arm any reader that has data but
    // parked before its writer's wakeup landed.
    if (lib_ams::_db.signaled && cd_poll_read_N()==0) {
        RecoverWakeup();
    }
}

// -----------------------------------------------------------------------------

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
