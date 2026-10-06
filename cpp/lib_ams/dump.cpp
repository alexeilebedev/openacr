// Copyright (C) 2025-2026 AlgoX2 Corp
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
// Source: cpp/lib_ams/dump.cpp
//

#include "include/algo.h"
#include "include/lib_ams.h"
#include "include/gen/lib_prot_gen.h"
#include "include/gen/lib_prot_gen.inl.h"

// Add the group table's columns to TBL: the group, its type, and one column per
// process of this node, which holds that process's membership of the group.
void lib_ams::GrpTableCols(algo_lib::FTxttbl &tbl) {
    AddCols(tbl,"grp,type");
    ind_beg(lib_ams::_db_zd_proc_curs,proc,lib_ams::_db) {
        AddCol(tbl,tempstr()<<proc.proc_id);
    }ind_end;
}

// Add one row per mapped ring matching REGX to TBL, of type ring: for each
// process of the node, R when it reads the ring -- green while its heartbeat
// is fresh, red once it is stale -- with the bytes it lags the writer by when
// that is more than a few, and W when it writes the ring.
void lib_ams::GrpTableRings(algo_lib::FTxttbl &tbl, algo_lib::Regx &regx) {
    ind_beg(lib_ams::_db_shm_curs,shm,lib_ams::_db) {
        if (Regx_Match(regx,tempstr()<<shm.grp_id) && shm.shm_region.elems) {
            algo_lib::FTxtrow &row= AddRow(tbl);
            AddCol(tbl,tempstr()<<shm.grp_id);
            AddCol(tbl,"ring");
            ind_beg(lib_ams::_db_zd_proc_curs,proc,lib_ams::_db) {
                algo_lib::FTxtcell &cell = algo_lib::AddCell(row,"",algo_TextJust_j_left);
                algo::ListSep ls(",");
                algo::SchedTime last_hb;
                u64 roff=0;
                u64 woff=shm.c_shmhdr->woff;
                ams::Shmember *read_shmember = FindReadShmember(shm, proc.proc_id);
                if (read_shmember) {
                    roff = read_shmember->offset;
                    last_hb = read_shmember->last_hb;
                    cell.text << ls<<"R";
                    double hbbehind = algo::ElapsedSecs(last_hb, algo_lib::_db.clock);
                    bool online = hbbehind < 2.0;
                    cell.style = online ? algo_TermStyle_green : algo_TermStyle_red;
                }
                if (proc.pid == shm.c_shmhdr->writer_pid) {
                    cell.text <<"W";
                    cell.style = algo_TermStyle_blue;
                }
                if (read_shmember) {
                    u64 behind = algo::u64_SubClip(woff,roff);
                    if (behind>10) {
                        cell.text << " "<<behind;
                    }
                }
            }ind_end;
        }
    }ind_end;
}

// Add one row to TBL per shm channel of each mapped ring matching REGX: the
// ring, the channel's key and reader, the bytes written and read on it, its
// limit and the window it follows the read count by, the room the writer has
// left, and the bytes written and not yet read.
void lib_ams::ChannelTable(algo_lib::FTxttbl &tbl, algo_lib::Regx &regx) {
    AddCols(tbl,"grp,key,reader,nwrite,nread,wlim,window,room,unread");
    ind_beg(lib_ams::_db_shm_curs,shm,lib_ams::_db) {
        if (Regx_Match(regx,tempstr()<<shm.grp_id) && shm.shm_region.elems) {
            ind_beg(lib_ams::shm_c_channel_curs,channel,shm) {
                AddRow(tbl);
                AddCol(tbl,tempstr()<<shm.grp_id);
                AddCol(tbl,tempstr()<<channel.key);
                AddCol(tbl,tempstr()<<channel.reader);
                AddCol(tbl,tempstr()<<channel.nwrite);
                AddCol(tbl,tempstr()<<channel.nread);
                AddCol(tbl,tempstr()<<channel.wlim);
                AddCol(tbl,tempstr()<<channel.window);
                AddCol(tbl,tempstr()<<ChannelRoom(channel));
                AddCol(tbl,tempstr()<<algo::u64_SubClip(channel.nwrite,channel.nread));
            }ind_end;
        }
    }ind_end;
}

// Print the shm channels of this node's rings matching REGX as a table, when
// there are any.  Both group-table dumps end with it.
void lib_ams::PrintChannelTable(algo_lib::Regx &regx) {
    algo_lib::FTxttbl tbl;
    ChannelTable(tbl, regx);
    if (algo_lib::c_txtrow_N(tbl) > 1) {
        prlog(tbl);
    }
}

// Print the group table of this node's rings matching REGX, for a process
// that knows no fabric group; lib_x2::DumpGrpTableVisual adds those.  The shm
// channels of those rings follow in a table of their own, when there are any.
void lib_ams::DumpGrpTableVisual(algo_lib::Regx &regx) {
    algo_lib::FTxttbl tbl;
    tbl.style = true;// output rides a term-flagged cmdout to a remote terminal
    GrpTableCols(tbl);
    GrpTableRings(tbl, regx);
    prlog(tbl);
    PrintChannelTable(regx);
}

// Print message MSG to string OUT according to format FMT.
// FMT.STRIP outer layers are stripped.  An envelope, whose msgtype says
// strip:Always, yields what it nests in every format.  A message whose msgtype
// says strip:Decode yields what it nests or the bytes it carries only under
// FMT.PRETTY; ssim and bin output print
// it whole, so a reader of the tuple sees the message.  With FMT.FORMAT bin, the
// message left after stripping is printed whole as binary; otherwise it prints
// as text.  With
// FMT.PRETTY, each nested message and byte payload prints on a line of its own,
// indented, cut to the payload limit.  With FMT.SHOWLEN, the message length is
// included.  amc generates the printer from the schema, so a new message prints
// this way with no code here.
void lib_ams::PrintMsg(algo::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out) {
    ams::MsgHeaderMsgs_PrintFmt(fmt, msg, out);
}

// -----------------------------------------------------------------------------

// This function should be called if the ams logcat is enabled
// It prints the given MSG to ams logcat using pretty format.
// Heartbeats (msgtype heartbeat:Y) are skipped unless verbose 2 is on
void lib_ams::TraceMsg(algo_lib::FLogcat *logcat, lib_ams::FShm &shm, ams::MsgHeader *payload) {
    bool trace = algo_lib_logcat_verbose2.enabled || !ams::MsgHeaderMsgs_HeartbeatQ(*payload);
    if (trace) {
        tempstr out;
        out << lib_ams::_db.proc_id<<"/"<<shm.grp_id<<": ";
        algo::MsgFmt fmt;
        lib_ams::PrintMsg(fmt, *payload, out);
        out << eol;
        algo_lib::_db.Prlog(logcat, algo::CurrSchedTime(), out);
    }
}

// -----------------------------------------------------------------------------

tempstr lib_ams::ToString(ams::MsgHeader &msg) {
    tempstr out;
    algo::MsgFmt fmt;
    fmt.pretty=false;
    lib_ams::PrintMsg(fmt, msg, out);
    return out;
}

// -----------------------------------------------------------------------------

// Convert message MSG to string in a way suitable for debugging
// (some information is lost in exchange for readability)
tempstr lib_ams::ToDbgString(ams::MsgHeader &msg) {
    tempstr out;
    algo::MsgFmt fmt;
    fmt.showlen=true;
    fmt.pretty=false;
    lib_ams::PrintMsg(fmt, msg, out);
    return algo::LimitLengthEllipsis(out, 120);
}
