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
// Source: cpp/lib_ams/bridge.cpp
//

#include "include/algo.h"
#include "include/lib_ams.h"

// Find the lowest slot N for a new bridged process of PROCTYPE on NODEIDX.
// The slot is used as both the process index in the new ProcId and the
// grpidx of the bridge shms.  Skipped: the caller's own slot (avoids
// colliding with our own proc_id when proctype/nodeidx happen to match)
// and any slot whose shm pair already exists locally (avoids re-using a
// slot owned by a still-live bridge).
int lib_ams::NextBridgeSlot(ams::Proctype proctype, int nodeidx) {
    int self_skip = -1;
    if (proctype == proctype_Get(_db.proc_id) && nodeidx == nodeidx_Get(_db.proc_id)) {
        self_skip = procidx_Get(_db.proc_id);
    }
    int slot = 0;
    while (true) {
        ams::ProcId proc_id = MakeProcId(proctype, nodeidx, slot);
        if (slot != self_skip
            && !ind_shm_Find(BridgeInGrp(proc_id, slot))
            && !ind_shm_Find(BridgeOutGrp(proc_id, slot))) {
            break;
        }
        slot++;
    }
    return slot;
}

// Allocate FProc + both shm files (writable) for a bridged user process at
// GRPIDX (use NextBridgeGrpidx to pick one). On success the inbound and
// outbound shm files exist on disk so the forked child can find them via
// shm_open. The child is pre-registered as a reader of shm_out so its
// ShmOpen(read) succeeds.  READER_PROC_ID is pre-registered as a reader of
// shm_in (the child->parent ring) so the ring is bounded from the start:
// a child that writes before the real reader (gateway/txn) opens the ring
// blocks on backpressure instead of overrunning a reader-less ring (which
// is treated as unbounded and silently overwrites).  The real reader's own
// AddReadShmember on open is then a dedup no-op.  Pass proctype_
// (the null proctype) to skip this when there is no distinct parent reader.
// Caller is then free to open the shms locally for its own reads/writes
// (e.g. in a single-process bridge) or leave them for another process to
// open (e.g. lib_x2).
// ignore:ptr_byref
bool lib_ams::CreateBridgeShms(ams::ProcId child_proc_id, int grpidx, ams::ProcId reader_proc_id, lib_ams::FShm *&shm_in, lib_ams::FShm *&shm_out, i64 size DFLTVAL(0), i32 maxmsg DFLTVAL(0)) {
    ind_proc_GetOrCreate(child_proc_id);
    shm_in  = ind_shm_GetOrCreate(BridgeInGrp(child_proc_id, grpidx));
    shm_out = ind_shm_GetOrCreate(BridgeOutGrp(child_proc_id, grpidx));
    bool ok = shm_in && shm_out;
    if (ok) {
        // Both halves of a pair have the same geometry, and the geometry is the
        // caller's to state: what a bridge has to hold is a property of the
        // child that writes through it, not of the platform.  Zero leaves the
        // process-wide default, which is what every unsized segment takes.
        //
        // The geometry is both numbers, not just the size.  A ring whose
        // largest message goes undeclared falls back to the process frame
        // size, and a stated size that cannot hold four frames is refused
        // outright at create, taking the bridge and the child behind it with
        // it.  So the message size is stated too, and the segment size is
        // derived from it through ShmSize, the one definition of a ring's
        // geometry: a caller then cannot state a pair of numbers that do not
        // fit each other.  A child that writes records rather than frames
        // states the payload allowance here, which is the only place the
        // allowance may reach a ring's geometry.
        shm_in->max_msg_size  = maxmsg;
        shm_out->max_msg_size = maxmsg;
        i64 want = size > 0 ? i64(ShmSize(u32(size), u32(maxmsg > 0 ? maxmsg : _db.max_msg_size))) : 0;
        shm_in->size  = want;
        shm_out->size = want;
        ams::ShmFlags wflags;
        write_Set(wflags, true);
        // Propagate the caller's signaled mode into the shm header so the
        // forked child (which opens the shm via -proc) can match it on
        // open without a separate cmdline flag.
        signaled_Set(wflags, _db.signaled);
        ok = ShmCreate(*shm_out, wflags);
        ok = ShmCreate(*shm_in, wflags) && ok;
    }
    if (ok) {
        AddReadShmember(*shm_out, child_proc_id);
        if (proctype_Get(reader_proc_id) != ams_Proctype_) {
            AddReadShmember(*shm_in, reader_proc_id);
        }
    }
    return ok;
}

// Format the value of a `-proc:` argument that initializes a bridged
// child with CHILD_PROC_ID at GRPIDX.  Format:
//   <child_proc_id>,<prefix>,<in_grp>,<out_grp>[,<nickname>]
// where the child's perspective is encoded:
//   in_grp  = parent's BridgeOutGrp (parent writes, child reads),
//   out_grp = parent's BridgeInGrp (child writes, parent reads).
// NICKNAME is the child's human-facing name (the userproc name); the
// child prefixes its published metrics with it in place of the proc id.
tempstr lib_ams::ChildProcStr(ams::ProcId child_proc_id, int grpidx, algo::strptr nickname DFLTVAL(algo::strptr())) {
    tempstr ret;
    ret << child_proc_id
        << "," << _db.file_prefix
        << "," << BridgeOutGrp(child_proc_id, grpidx)
        << "," << BridgeInGrp(child_proc_id, grpidx);
    if (ch_N(nickname)) {
        ret << "," << nickname;
    }
    return ret;
}
