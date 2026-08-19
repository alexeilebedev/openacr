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
// Target: amsspy (exe) -- List ams sessions and monitor traffic on host
// Exceptions: yes
// Source: cpp/amsspy/amsspy.cpp
//

#include "include/algo.h"
#include "include/amsspy.h"
#include "include/lib_ams.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Scan /dev/shm for *.ams files, build FSession/FShm tables
static void ScanSessions() {
    ind_beg(algo::Dir_curs, entry, "/dev/shm/*.ams") {
        algo::strptr shm = Pathcomp(entry.filename, ".RL");
        amsspy::FSession &session = amsspy::ind_session_GetOrCreate(Pathcomp(entry.filename, ".LL-RL-RL-RL"));
        if (!amsspy::ind_shm_Find(shm)) {
            amsspy::FShm &fshm = amsspy::shm_Alloc();
            fshm.shm       = shm;
            fshm.p_session = &session;
            shm_XrefMaybe(fshm);
        }
    }ind_end;
}

static void ListSessions() {
    ind_beg(amsspy::_db_session_curs, session, amsspy::_db) {
        prlog(session.session);
        if (amsspy::_db.cmdline.shm) {
            ind_beg(amsspy::session_c_shm_curs, shm, session) {
                prlog("    " << shm.shm);
            }ind_end;
        }
    }ind_end;
}

// For each FShm matching the -spy regex: open the segment, initialise state fields,
// pre-allocate the copy buffer, and add the entry to the cd_shm_poll list.
static void SpySession() {
    ind_beg(amsspy::_db_shm_curs, fshm, amsspy::_db) {
        if (Regx_Match(amsspy::_db.cmdline.spy, fshm.shm)) {
            tempstr filename;
            filename << "/dev/shm/" << fshm.shm << ".ams";
            int fd = open(Zeroterm(filename), O_RDONLY);
            if (fd < 0) {
                prerr("amsspy.spy  cannot open" << Keyval("shm", filename));
            } else {
                struct stat st;
                fstat(fd, &st);
                u64 size = st.st_size;
                void *mem = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
                close(fd);
                if (size <= sizeof(ams::Shmhdr) || mem == MAP_FAILED) {
                    prerr("amsspy.spy  mmap failed" << Keyval("shm", filename));
                } else {
                    ams::Shmhdr *hdr = (ams::Shmhdr*)mem;
                    ams::Shmhdr dflt;
                    if (hdr->magic != dflt.magic) {
                        prerr("amsspy.spy  invalid magic" << Keyval("shm", filename));
                        munmap(mem, size);
                    } else {
                        fshm.hdr      = (u64)hdr;
                        fshm.mem_size = size;
                        fshm.mask     = hdr->offset_mask;
                        fshm.maxlen   = hdr->max_msg_size;
                        fshm.roff     = hdr->woff;
                        ch_Reserve(fshm.buf, fshm.maxlen);
                        amsspy::cd_shm_poll_Insert(fshm);
                    }
                }
            }
        }
    }ind_end;
}

// Print a decoded message from buf, prefixed with shm/offset if verbose
static void PrintMsg(amsspy::FShm &fshm, u64 roff) {
    ams::MsgHeader *pmsg = (ams::MsgHeader*)strptr(fshm.buf).elems;
    tempstr str;
    if (algo_lib::_db.cmdline.verbose) {
        str << fshm.shm << "@" << roff << " ";
    }
    if (amsspy::_db.cmdline.f == command_amsspy_f_decode) {
        str << lib_ams::ToString(*pmsg);
        prlog(str);
    } else if (amsspy::_db.cmdline.f == command_amsspy_f_text) {
        str << pmsg->type << " " << pmsg->length << " " << strptr_ToSsim(strptr((char*)pmsg, pmsg->length));
        prlog(str);
    } else if (amsspy::_db.cmdline.f == command_amsspy_f_raw) {
        int rc = write(1, pmsg, pmsg->length);
        (void)rc;
    }
}

// Inline step: called each main-loop iteration.
// Uses RotateFirst for round-robin polling across all watched segments.
void amsspy::cd_shm_poll_Step() {
    amsspy::FShm *fshm = amsspy::cd_shm_poll_RotateFirst();
    ams::Shmhdr *hdr = (ams::Shmhdr*)fshm->hdr;
    lfence();
    u64 woff = hdr->woff;
    if (fshm->roff + sizeof(ams::MsgHeader) <= woff) {
        u8 *data     = (u8*)hdr + hdr->datastart;
        ams::MsgHeader *msg_ptr = (ams::MsgHeader*)(data + (fshm->roff & fshm->mask));
        u32 len      = msg_ptr->length;
        bool valid   = len >= (u32)sizeof(ams::MsgHeader) && len <= fshm->maxlen && fshm->roff + len <= woff;
        if (valid) {
            // Copy message to local buffer, then fence and re-check for overrun
            ch_RemoveAll(fshm->buf);
            ch_Addary(fshm->buf, algo::strptr((char*)msg_ptr, len));
            lfence();
            u64 woff2 = hdr->woff;
            if (woff2 - fshm->roff > (u64)(fshm->mask + 1)) {
                prlog("amsspy.spy_overrun" << Keyval("shm", fshm->shm) << Keyval("skipped", len));
                fshm->roff = woff2;
            } else {
                u64 msg_roff = fshm->roff;
                fshm->roff   = lib_ams::AddOffset(fshm->roff, len);
                PrintMsg(*fshm, msg_roff);
            }
        }
    }
}

// Count *.ams segments currently in /dev/shm.
static int NShmSegment() {
    int ret = 0;
    ind_beg(algo::Dir_curs, entry, "/dev/shm/*.ams") {
        if (ch_N(entry.filename)) {
            ret++;
        }
    }ind_end;
    return ret;
}

// -clean: unlink orphaned /dev/shm ams segments (their writers are gone --
// see lib_ams::CleanOldShmFiles) and report how many were reclaimed.  Used as
// the first step of a CI job to reclaim segments leaked by a preempted or
// kill -9'd predecessor before its shm exhausts the host.
static void Clean() {
    int before = NShmSegment();
    lib_ams::CleanOldShmFiles();
    int after = NShmSegment();
    prlog("amsspy.clean"
          <<Keyval("before", before)
          <<Keyval("after", after)
          <<Keyval("removed", before - after));
}

// -dump: out-of-band snapshot of every segment's flow-control state, read
// straight from /dev/shm without touching the cluster's command path (which
// is exactly what a produce-path wedge stalls).  For each segment matching the
// -session regex: print woff, the writer's budget (computed as the live
// UpdateBudget does -- min reader offset + ring - 2*max_msg - woff), nnobudget,
// and one indented line per reader with its pid, offset, lag, and sleeping bit.
// The slowest reader (smallest offset below woff) is the one pinning the ring.
static void DumpSegments() {
    ind_beg(amsspy::_db_shm_curs, fshm, amsspy::_db) {
        if (Regx_Match(amsspy::_db.cmdline.session, fshm.p_session->session)) {
            tempstr filename;
            filename << "/dev/shm/" << fshm.shm << ".ams";
            int fd = open(Zeroterm(filename), O_RDONLY);
            struct stat st;
            void *mem = MAP_FAILED;
            if (fd >= 0) {
                fstat(fd, &st);
                mem = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                close(fd);
            }
            ams::Shmhdr dflt;
            if (mem == MAP_FAILED || st.st_size <= (off_t)sizeof(ams::Shmhdr) || ((ams::Shmhdr*)mem)->magic != dflt.magic) {
                prerr("amsspy.dump  unreadable" << Keyval("shm", fshm.shm));
            } else {
                ams::Shmhdr *hdr = (ams::Shmhdr*)mem;
                ams::Shmember *member = (ams::Shmember*)(hdr+1);
                u64 woff = hdr->woff;
                u64 min_roff = woff;
                for (u32 i = 0; i < hdr->n_shmember; i++) {
                    u64_UpdateMin(min_roff, member[i].offset);
                }
                // A message board is an array of slots rather than a ring, so it
                // has no write budget: the ring arithmetic below would read its
                // zero offset mask and report an underflowed u64.  What it has
                // instead is a slot count and a slot size, and how many of those
                // slots are in use is not visible from here -- the free list is
                // the writer's own, and nothing about it is published to the
                // segment.
                bool board = hdr->grp_id.grptype == ams_Grptype_board;
                u64 nslot = board ? (u64(hdr->tot_size) - hdr->datastart) / u64(hdr->max_msg_size) : 0;
                u64 writelimit = min_roff + u64(hdr->offset_mask)+1 - u64(hdr->max_msg_size)*2;
                tempstr budget;
                if (board) {
                    budget << "n/a";
                } else {
                    budget << algo::u64_SubClip(writelimit, woff);
                }
                prlog("amsspy.dump"
                      <<Keyval("shm", fshm.shm)
                      <<Keyval("woff", woff)
                      <<Keyval("budget", budget)
                      <<Keyval("nslot", nslot)
                      <<Keyval("slot_size", board ? hdr->max_msg_size : 0)
                      <<Keyval("nnobudget", hdr->nnobudget)
                      <<Keyval("nblock", hdr->nblock)
                      <<Keyval("writer_pid", hdr->writer_pid)
                      <<Keyval("eof", hdr->eof)
                      <<Keyval("n_reader", hdr->n_shmember));
                for (u32 i = 0; i < hdr->n_shmember; i++) {
                    prlog("    reader"
                          <<Keyval("grpmember", member[i].grpmember_id)
                          <<Keyval("pid", member[i].pid)
                          <<Keyval("offset", member[i].offset)
                          <<Keyval("lag", algo::u64_SubClip(woff, member[i].offset))
                          <<Keyval("sleeping", member[i].sleeping));
                }
                munmap(mem, st.st_size);
            }
        }
    }ind_end;
}

void amsspy::Main() {
    if (_db.cmdline.clean) {
        Clean();
    } else if (_db.cmdline.dump) {
        ScanSessions();
        DumpSegments();
    } else {
        ScanSessions();
        SpySession();
        if (cd_shm_poll_EmptyQ()) {
            _db.cmdline.list = true;
        }
        if (_db.cmdline.list) {
            ListSessions();
        }
        if (!cd_shm_poll_EmptyQ()) {
            amsspy::MainLoop();
        }
    }
}
