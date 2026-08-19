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
// Source: cpp/lib_ams/lib.cpp
//

#include "include/algo.h"
#include "include/lib_ams.h"


static lib_ams::FShm *CreateLoopbackShm(lib_ams::MsgCb cb) {
    ams::GrpId grp_id;
    grp_id.proc_id = lib_ams::_db.proc_id;
    grp_id.grptype = ams::Grptype(ams_Grptype_prsup);
    lib_ams::ind_proc_GetOrCreate(grp_id.proc_id);// FShm xref requires parent FProc
    lib_ams::FShm *shm = lib_ams::ind_shm_GetOrCreate(grp_id);
    bool ok = shm != NULL;
    if (ok) {
        ams::ShmFlags flags;
        read_Set(flags, true);
        write_Set(flags, true);
        algo::cstring saved_prefix = lib_ams::_db.file_prefix;
        lib_ams::_db.file_prefix = "";// force lpool allocation (no file)
        ok = lib_ams::ShmCreate(*shm, flags);
        lib_ams::_db.file_prefix = saved_prefix;
    }
    if (ok) {
        // ShmCreate with empty prefix allocates in-process memory and initializes the header.
        // Manually set up read/write state (ShmOpen expects a file-backed shm).
        shm->c_reader = lib_ams::AddReadShmember(*shm, lib_ams::_db.proc_id);
        shm->c_reader->pid = getpid();
        read_Set(shm->flags, true);
        write_Set(shm->flags, true);
        shm->locked = true;
        shm->c_shmhdr->writer_pid = getpid();
        cd_poll_read_Insert(*shm);
        h_amsmsg_Set2(*shm, *shm, cb);
        // Track loopback so cd_fdin_eof_Step can drop it when stdin EOFs.
        lib_ams::_db.c_loopback_shm = shm;
    }
    if (!ok) {
        prerr("lib_ams: failed to create loopback shm");
        shm = NULL;
    }
    return shm;
}

// Initialize library.  PROC_STR carries everything in one string:
//   <proc_id>                                stdio peer (loopback + stdin
//                                            reader if MSG_CB is set);
//                                            no shm namespace owned
//   <proc_id>,<prefix>                       server: PREFIX is the
//                                            filename prefix for shm
//                                            files we create
//   <proc_id>,<prefix>,<in_grp>,<out_grp>    bridged child: open the
//                                            named existing shm pair
//                                            (PREFIX is informational —
//                                            it identifies the server
//                                            that owns the namespace)
//   <proc_id>,<prefix>,<in_grp>,<out_grp>,<nickname>
//                                            bridged child with a
//                                            human-facing name; the name
//                                            prefixes published metrics
//                                            in place of the proc id
// Comma is the separator; PROC_ID, GRPIDs, PREFIX, and NICKNAME must not
// contain commas.  Stale unlocked /dev/shm/*.ams files are cleaned on
// first call when PREFIX is set.
bool lib_ams::Init(algo::strptr proc_str, lib_ams::MsgCb msg_cb DFLTVAL(NULL)) {
    lib_ams::_db.h_amsmsg = (lib_ams::_db_h_amsmsg_hook)msg_cb;
    lib_ams::_db.h_amsmsg_ctx = 0;
    algo::StringIter it(proc_str);
    algo::strptr proc_s   = algo::GetTokenChar(it, ',');
    algo::strptr prefix_s = algo::GetTokenChar(it, ',');
    algo::strptr in_s     = algo::GetTokenChar(it, ',');
    algo::strptr out_s    = algo::GetTokenChar(it, ',');
    algo::strptr nick_s   = it.Rest();
    ams::ProcId proc_id;
    algo::strptr comment;
    lib_ams::SetupTerminateSignal();
    bool ok = ams::ProcId_ReadStrptrMaybe(proc_id, proc_s);
    if (!ok) {
        comment = "cannot parse the proc id";
    }
    if (ok) {
        lib_ams::_db.proc_id     = proc_id;
        lib_ams::_db.file_prefix = prefix_s;
        lib_ams::_db.nickname    = nick_s;
        lib_ams::_db.c_thisproc  = &lib_ams::ind_proc_GetOrCreate(proc_id);
    }
    if (ok && ch_N(in_s)) {
        // Bridged-child path: open the existing shm pair.
        ams::GrpId in_grp;
        ams::GrpId out_grp;
        ok = ams::GrpId_ReadStrptrMaybe(in_grp,  in_s)
            && ams::GrpId_ReadStrptrMaybe(out_grp, out_s);
        if (!ok) {
            comment = "cannot parse the bridge grp ids";
        }
        if (ok) {
            lib_ams::FShm *in_shm  = lib_ams::ind_shm_GetOrCreate(in_grp);
            lib_ams::FShm *out_shm = lib_ams::ind_shm_GetOrCreate(out_grp);
            ams::ShmFlags rflags;
            read_Set(rflags, true);
            ams::ShmFlags wflags;
            write_Set(wflags, true);
            // A failure to attach is reported by ShmOpen, which names the ring
            // and the reason; what is left for this report is the direction --
            // which of the pair the child could not take -- and the case where
            // the shm table itself had no room for the rows.
            bool row_ok = in_shm && out_shm;
            bool in_ok = row_ok && lib_ams::ShmOpen(*in_shm, rflags);
            ok = in_ok && lib_ams::ShmOpen(*out_shm, wflags);
            if (!row_ok) {
                comment = "no room in the shm table for the bridge pair";
            } else if (!in_ok) {
                comment = "cannot read the inbound ring";
            } else if (!ok) {
                comment = "cannot write the outbound ring";
            }
            if (ok && msg_cb) {
                h_amsmsg_Set2(*in_shm, *in_shm, msg_cb);
            }
            if (ok) {
                lib_ams::_db.c_shm_stdout = out_shm;
                // If the writer of either shm is in signaled mode (a
                // signaled cluster whose netstack allows it), match that here
                // so SIGRTMIN wakeups reach us too.
                if ((in_shm->c_shmhdr  && in_shm->c_shmhdr->signaled)
                    || (out_shm->c_shmhdr && out_shm->c_shmhdr->signaled)) {
                    SetSignaledMode(true);
                }
            }
        }
    } else if (ok && ch_N(prefix_s)) {
        // Server path: own the shm namespace.
        if (bool_Update(lib_ams::_db.shm_files_cleaned, true)) {
            lib_ams::CleanOldShmFiles();
        }
    } else if (ok) {
        // Stdio peer: route stdin-parsed messages through a loopback shm
        // to MSG_CB.
        if (msg_cb) {
            lib_ams::_db.c_shm_stdin = CreateLoopbackShm(msg_cb);
        }
        BeginReadStdin();
    }
    if (!ok) {
        prerr("lib_ams.init_error"<<Keyval("proc_str",proc_str)<<Keyval("comment",comment));
    }
    return ok;
}

void lib_ams::Uninit() {
    SetSignaledMode(false);
    shm_RemoveAll();
}

// Emit message. In shm mode, write to output shm.
// In stdio mode, print as text.
void lib_ams::EmitMsg(ams::MsgHeader &msg) {
    amscat(verbose,"emit "<<msg);
    if (lib_ams::_db.c_shm_stdout) {
        lib_ams::WriteMsgBlock(*lib_ams::_db.c_shm_stdout, msg);
    } else {
        prlog(lib_ams::ToString(msg));
    }
}

// Notify lib_ams that process PID has exited with STATUS.
// Clear the pid in any FProc record matching PID and return that record (if any).
// For each shm in the database:
//   - If PID was the writer, clear writer_pid. If we are reading from this shm,
//     also set the eof flag on its shmhdr so cd_poll_read_Step can permanently
//     remove it from the poll loop once any remaining messages are drained.
//   - If PID was a reader, clear that shmember's pid so the writer's budget
//     is no longer constrained by it.
//
// The unlink is the part that needs care, because a writer's death is not the
// segment's death.  Consider a userproc bridged to a gateway: its supervisor
// creates the parent->child ring, the gateway writes it, and the child reads
// it.  When the gateway exits, the supervisor collects the death and reaches
// this walk, where the ring matches on writer_pid -- so unlinking every
// segment the dead pid wrote would take that name away while the child still
// owns it.  A child that has not yet opened the ring would then get ENOENT
// out of its own shm_open and report a segment it never had the chance to
// map, which is a startup failure invented by someone else's exit.
//
// A segment's name belongs to the proc its grp id names, and only that proc's
// death retires the name.  So the unlink is scoped to the segments the dead
// proc owns; a writer of somebody else's ring is only a writer, and lets go by
// clearing writer_pid.  Nothing leaks by leaving the file: the owner's own
// teardown unlinks it, and a segment whose writer is gone and whose lock is
// free is already reclaimable by the orphan sweep.
lib_ams::FProc *lib_ams::ProcExit(int pid, int status) {
    lib_ams::FProc *ret=NULL;
    ind_beg(lib_ams::_db_zd_proc_curs,proc,lib_ams::_db) {
        if (proc.pid == pid) {
            proc.pid = -1;
            proc.status = status;
            ret = &proc;
            break;
        }
    }ind_end;
    // Mark every read-side shm tied to the dead proc as eof, even when
    // writer_pid never matched (e.g. proc died before claiming the shm).
    // cd_poll_read drains and drops eof shms, so MainLoop sees the input
    // source disappear and exits naturally without an explicit ReqExit.
    if (ret) {
        ind_beg(lib_ams::proc_c_shm_curs, shm, *ret) {
            if (read_Get(shm.flags) && shm.c_shmhdr) {
                shm.c_shmhdr->eof = true;
            }
        }ind_end;
    }
    ind_beg(_db_shm_curs,shm,_db) {
        bool own = ret && shm.grp_id.proc_id == ret->proc_id;
        // Skip closed shms (c_shmhdr cleared by ShmClose).
        if (!shm.c_shmhdr) {
        } else if (shm.c_shmhdr->writer_pid == pid) {
            shm.c_shmhdr->writer_pid=0;
            if (read_Get(shm.flags)) {
                shm.c_shmhdr->eof = true;
            }
            // The owner is gone — unlink the /dev/shm file now so it doesn't
            // leak when it died without running its own cleanup.
            // Existing mappings stay valid until the last munmap.
            if (own && shm.filename != "") {
                shm_unlink(Zeroterm(shm.filename));
                shm.filename = "";
            }
        }
        ind_beg(lib_ams::shm_c_shmember_curs,shmember,shm) {
            if (shmember.pid == pid) {
                shmember.pid = 0;
                // A dead reader's ring position stops advancing where it died,
                // so the board slots it was holding would be released by
                // nothing.  Its allowance bounds the loss, but the slots are
                // still the board's to reuse, and only this moment says so.
                lib_ams::BoardRelease(shm, ind_curs(shmember).index);
            }
        }ind_end;
    }ind_end;
    return ret;
}

void lib_ams::UnreadMsg() {
    if (lib_ams::FShm *shm=cd_poll_read_First()) {
        shm->c_cur_msg=NULL;
        shm->n_unread++;
        _db.trace.n_unread++;
    }
}

// -----------------------------------------------------------------------------

// Set the segment size a ring this process creates gets by default: a body of
// at least SIZE bytes for messages up to the process ceiling.
void lib_ams::SetDfltShmSize(u32 size) {
    lib_ams::_db.shmem_size = i32(lib_ams::ShmSize(size, u32(lib_ams::_db.max_msg_size)));
}
