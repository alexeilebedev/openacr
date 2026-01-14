// Copyright (C) 2025-2026 AlgoRND
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
// Exceptions: yes
// Source: cpp/lib_ams/shmember.cpp
//

#include "include/algo.h"
#include "include/lib_ams.h"

// If the shm is open for reading, check to see if a message
// is available. If it is available, return pointer to message.
ams::Shmmsg *lib_ams::PeekMsg(lib_ams::FShmember &shmember) {
    lib_ams::FShm &shm = *shmember.p_shm;
    ams::Shmmsg *msg = MsgAtOffset(shm,shmember.off);
    ams::Shmmsg *ret = NULL;
    int L = msg->length;
    if (L >= ssizeof(ams::Shmmsg)) {
        lfence();
        if (msg->type == ams_MsgHeader_type_ams_Shmmsg && msg->shm_id == shm.shm_id && msg->off == shmember.off) {
            ret = msg;
        } else {
            ShmError(shm,msg,"unexpected message id");
        }
    } else if (L > 0) {
        ShmError(shm,msg,"message too short");
    }
    return ret;
}

// Called by the client
// to avoid reading current message
void lib_ams::StopReading(lib_ams::FShm &shm) {
    UnreadMsg();
    if (shm.c_read) {
        cd_poll_read_Remove(*shm.c_read);
    }
}

// -----------------------------------------------------------------------------

// Read up to N messages from the message heap
// Messages are processed in the order in which they were posted to the shms.
void lib_ams::bh_shmember_read_Step() {
    for (int i=0; i<20 && bh_shmember_read_N()>0; i++) {
        lib_ams::FShmember *shmember = bh_shmember_read_First();
        lib_ams::FShm *shm=shmember->p_shm;
        _db.c_cur_shmember=shmember;
        if (ams::Shmmsg *shmmsg = shmember->c_cur_msg) {
            if (ams::MsgHeader *payload = payload_Get(*shmmsg)) {
                if (algo_lib_logcat_ams.enabled) {
                    TraceMsg(shmmsg,payload);
                }
                h_amsmsg_Call(*shm,*payload);
            }
        }
        if (shmember->c_cur_msg) {
            SkipMsg(*shmember);// sets c_curmsg back to NULL
            ams::Shmmsg *shmmsg = PeekMsg(*shmember);
            shmember->c_cur_msg = shmmsg;
            if (shmmsg) {
                shmember->sortkey=shmmsg->tstamp + shm->schedbias;
                bh_shmember_read_Reheap(*shmember);
            } else {
                bh_shmember_read_Remove(*shmember);
                if (!write_Get(shm->flags)) {
                    cd_poll_read_Insert(*shmember);
                }
            }
        } else {
            // message was not processed.
            // stop reading and go back to polling
            // is there a better method?
            bh_shmember_read_Remove(*shmember);
            cd_poll_read_Insert(*shmember);
        }
    }
    _db.c_cur_shmember = NULL;
}

// -----------------------------------------------------------------------------

// Check all shms (that are not already readable) for readability and
// transfer readable shms to the read heap with correct sort key.
// If powersave is enabled, then non-readable shms
// are transferred to the slow_poll_read list where they are polled less frequently
void lib_ams::cd_poll_read_Step() {
    int n = cd_poll_read_N();
    for (int i=0; i<n; i++) {
        lib_ams::FShmember *shmember=cd_poll_read_First();
        ams::Shmmsg *shmmsg = PeekMsg(*shmember);
        shmember->c_cur_msg = shmmsg;
        if (shmmsg) {
            shmember->sortkey=shmmsg->tstamp;
            bh_shmember_read_Reheap(*shmember);
            cd_poll_read_Remove(*shmember);
        } else if (write_Get(shmember->p_shm->flags)) {
            // remove reader from poll loop if this process is the writer, i.e.
            // an unsolicited message cannot show up
            cd_poll_read_Remove(*shmember);
        } else if (lib_ams::_db.powersave) {
            cd_poll_read_Remove(*shmember);
            cd_slow_poll_read_Insert(*shmember);
        } else {
            cd_poll_read_RotateFirst();
        }
    }
}

// -----------------------------------------------------------------------------

void lib_ams::cd_slow_poll_read_Step() {
    int n = cd_slow_poll_read_N();
    for (int i=0; i<n; i++) {
        lib_ams::FShmember *shmember=cd_slow_poll_read_First();
        if (PeekMsg(*shmember)) {
            cd_slow_poll_read_Remove(*shmember);
            cd_poll_read_Insert(*shmember);
        } else {
            cd_slow_poll_read_RotateFirst();
        }
    }
    _db.cd_slow_poll_read_delay.value = cd_poll_read_N()>0 ? 0 : algo_lib::_db.cpu_hz/1024;
}

// -----------------------------------------------------------------------------

// Mark current message as read and move to the next message.
void lib_ams::SkipMsg(lib_ams::FShmember &shmember) {
    lib_ams::FShm &shm=*shmember.p_shm;
    if (ams::Shmmsg *msg = shmember.c_cur_msg) {
        shmember.off = AddOffset(shmember.off, msg->length);
        shmember.c_cur_msg = NULL;
        // Force a report (ShmHb) every N messages
        if (shmember.off >= shm.next_ackoff) {
            UpdateAckOffset(shm,shmember);
            SendHb(shmember);
        }
    }
}

// -----------------------------------------------------------------------------

// Send heartbeat to control shm
void lib_ams::SendHb(lib_ams::FShmember &shmember) {
    if (lib_ams::_db.c_shm_ctl && lib_ams::_db.shmem_mode) {
        lib_ams::FShm &shm = *shmember.p_shm;
        if (w_Get(shmember.shmember.flags)) {
            shmember.budget = algo::u64_SubClip(shm.writelimit, shmember.off);
        }
        shmember.last_hb = algo_lib::_db.clock;
        lib_ams::ShmHbMsg_FmtShm(*lib_ams::_db.c_shm_ctl,shmember.shmember,shmember.off,shmember.budget);
    }
}

// Publish shm heartbeats to c_shm_ctl shm
void lib_ams::cd_hb_Step() {
    lib_ams::FShmember &shmember = *cd_hb_RotateFirst();
    SendHb(shmember);
}

// -----------------------------------------------------------------------------

// Create a read shmember for shm SHM
// Begin hot-polling the shm
bool lib_ams::OpenRead(lib_ams::FShm &shm, u64 off) {
    bool ok = true;
    if (!shm.c_read) {
        ams::ShmemberId key(shm.shm_id,lib_ams::_db.proc_id,ams_ShmemberFlags_r);
        //prlog(lib_ams::_db.proc_id<<": begin read "<<key);
        shm.c_read = ind_shmember_Find(key);
        if (!shm.c_read) {
            shm.c_read = &lib_ams::shmember_Alloc();
            shm.c_read->shmember=key;
            if (!shmember_XrefMaybe(*shm.c_read)) {
                ok=false;
            }
        }
        shm.c_read->off = off;
        shm.c_read->last_hb=algo_lib::_db.clock;
        UpdateAckOffset(shm,*shm.c_read);
        read_Set(shm.flags,true);
        cd_poll_read_Insert(*shm.c_read);
        cd_hb_Insert(*shm.c_read);
    }
    return ok;
}

// -----------------------------------------------------------------------------

// Create a write shmember for shm SHM
bool lib_ams::OpenWrite(lib_ams::FShm &shm, u64 off) {
    bool ok = true;
    // lock file.
    // the file may already exist and be locked -- this is OK, AMS root
    // process usually pre-creates all shm segments and locks them.
    if (!shm.c_write) {
        if (ShmemFdOpenQ(shm)) {
            ok = flock(shm.shm_file.fd.value, LOCK_EX|LOCK_NB)==0;
            if (!ok) {
                prerr(lib_ams::_db.proc_id<<": opening "<<shm.shm_id<<" for writing: failed to acquire lock");
            }
        }
        if (ok) {
            ams::ShmemberId key(shm.shm_id,lib_ams::_db.proc_id,ams_ShmemberFlags_w);
            shm.c_write = ind_shmember_Find(key);
            if (!shm.c_write) {
                shm.c_write = &lib_ams::shmember_Alloc();
                shm.c_write->shmember=key;
                if (!shmember_XrefMaybe(*shm.c_write)) {
                    ok=false;
                }
            }
            shm.c_write->last_hb=algo_lib::_db.clock;
            shm.c_write->off = off;
            write_Set(shm.flags,true);
            cd_hb_Insert(*shm.c_write);
            UpdateWriteLimit(shm);
        }
    }
    return ok;
}

void lib_ams::UnreadMsg() {
    if (_db.c_cur_shmember) {
        _db.c_cur_shmember->c_cur_msg = NULL;
    }
}
