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
// Exceptions: NO
// Source: cpp/lib_ams/dump.cpp
//
// Rendering an AMS message as text, and the shm table as a picture.
// A message arrives as bytes with a type tag, and the reader wants to see what
// it is.  amc generates a printer for every message type the tree declares, so
// the general case is one call; what this file adds is the presentation around
// it -- a payload cut down to a readable length, the log message rendered as a
// line rather than as a tuple, and the width-limited forms the trace and the
// debugger use.

#include "include/algo.h"
#include "include/lib_ams.h"

// Return string STR limited to ~LIM chars
// Either the original string is returned, or a string in the form
// abcabcabcabc [CRC:12345678] xyzxyzxyzxyz
// original characters are unquoted -- result must be passed through Keyval or strptr_ToSsim
// the final string may be longer than lim chars by about ~20 bytes.
static tempstr LimitLengthCRC(algo::strptr str, int lim) {
    tempstr ret;
    if (str.n_elems > lim) {
        int ncut = str.n_elems - lim;
        int cut1 = (str.n_elems - ncut) / 2;
        int cut2 = (str.n_elems + ncut) / 2;
        vrfy_(cut1 >= 0 && cut1 <= str.n_elems && cut2 >= 0 && cut2 <= str.n_elems);
        if (cut1 > 0) {
            ret << ch_FirstN(str,cut1) << " [";
        }
        ret << "CRC:"<<algo::CRC32Step(0, (u8*)str.elems + cut1, cut2 - cut1);
        if (cut2 < str.n_elems) {
            ret << "] " << ch_RestFrom(str,cut2);
        }
    } else {
        ret << str;
    }
    return ret;
}

// Print table of shms in lib_ams, using a more readable layout
void lib_ams::DumpShmTableVisual(algo_lib::Regx &regx) {
    algo_lib::FTxttbl tbl;
    tbl.style = true;// output rides a term-flagged cmdout to a remote terminal
    AddCols(tbl,"shm,size");
    // list of processes reading the shm
    ind_beg(lib_ams::_db_zd_proc_curs,proc,lib_ams::_db) {
        AddCol(tbl,tempstr()<<proc.proc_id);
    }ind_end;
    ind_beg(lib_ams::_db_shm_curs,shm,lib_ams::_db) {
        if (Regx_Match(regx,tempstr()<<shm.grp_id) && shm.shm_region.elems) {
            algo_lib::FTxtrow &row= AddRow(tbl);
            AddCol(tbl,tempstr()<<shm.grp_id);
            AddCol(tbl,tempstr()<<shm.shm_region.n_elems);
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
    prlog(tbl);
}

// Print a log message MSG in the long form: the tuple's fields, then the text
// on its own indented line so a multi-line log entry stays readable.
static void ShowLogMsg(lib_ams::MsgFmt &fmt, ams::LogMsg &logmsg, cstring &out) {
    out << "ams.LogMsg";
    if (fmt.showlen) {
        out << "  msglen:" << logmsg.length;
    }
    out << Keyval("proc_id",logmsg.proc_id)
        << Keyval("tstamp",logmsg.tstamp)
        << Keyval("logcat",logcat_Getary(logmsg));
    out << "  text:\\" << eol;
    char_PrintNTimes(' ', out, (fmt.indent+1)*2);
    out << LimitLengthCRC(text_Getary(logmsg), fmt.payload_lim);
}

// Print a log message MSG the way a log file reads: who said it, under which
// category, and what they said.
static void ShowLogLine(ams::LogMsg &logmsg, cstring &out) {
    out << logmsg.proc_id
        <<" "<<logcat_Getary(logmsg)
        <<": "
        <<text_Getary(logmsg);
}

// Print message MSG to string OUT according to format FMT
// if FMT.STRIP > 0, strip this many outer "layers" -- a message this tree
// knows no wrapper for has none to strip and renders in full, except for a
// log message, which drops to its one-line form.
// if FMT.FORMAT is bin, the message is printed as pure binary; otherwise, convert to text
// if FMT.PRETTY, every next layer / payload is printed on a new line with indent
// for readability.
// FMT.PAYLOAD_LIM limits maximum printed payload length, allowing to fit one message per
// screen even if payload is 10MB
// Finally, if FMT.SHOWLEN is true, message length is included in output.
void lib_ams::PrintMsg(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out) {
    bool printed=false;
    ams::LogMsg *logmsg = ams::LogMsg_Castdown(msg);
    if (fmt.format==lib_ams_MsgFmt_format_bin) {
        // just dump the binary part
        out << algo::strptr((char*)&msg,msg.length);
        printed=true;
    } else if (fmt.pretty && logmsg) {
        algo::char_PrintNTimes(' ', out, fmt.indent*2);
        if (fmt.strip == 0) {
            ShowLogMsg(fmt, *logmsg, out);
        } else {
            ShowLogLine(*logmsg, out);
        }
        printed=true;
    }
    if (!printed) {
        MsgHeaderMsgs_Print(out,msg,msg.length);
    }
}

// -----------------------------------------------------------------------------

// This function should be called if the ams logcat is enabled
// It prints the given MSG to ams logcat using pretty format.
// The shm heartbeat is skipped unless verbose is on -- it arrives once a
// second per member and says nothing a reader of the trace is looking for.
void lib_ams::TraceMsg(algo_lib::FLogcat *logcat, lib_ams::FShm &shm, ams::MsgHeader *payload) {
    bool trace = algo_lib_logcat_verbose2.enabled
        || payload->type != ams_MsgHeader_type_ams_ShmHbMsg;
    if (trace) {
        tempstr out;
        out << lib_ams::_db.proc_id<<"/"<<shm.grp_id<<": ";
        lib_ams::MsgFmt fmt;
        lib_ams::PrintMsg(fmt, *payload, out);
        out << eol;
        algo_lib::_db.Prlog(logcat, algo::CurrSchedTime(), out);
    }
}

// -----------------------------------------------------------------------------

// Convert message MSG to a single-line string carrying every field.
tempstr lib_ams::ToString(ams::MsgHeader &msg) {
    tempstr out;
    lib_ams::MsgFmt fmt;
    fmt.pretty=false;
    lib_ams::PrintMsg(fmt, msg, out);
    return out;
}

// -----------------------------------------------------------------------------

// Convert message MSG to string in a way suitable for debugging
// (some information is lost in exchange for readability)
tempstr lib_ams::ToDbgString(ams::MsgHeader &msg) {
    tempstr out;
    lib_ams::MsgFmt fmt;
    fmt.showlen=true;
    fmt.pretty=false;
    fmt.payload_lim=60;// limit output length
    lib_ams::PrintMsg(fmt, msg, out);
    return algo::LimitLengthEllipsis(out, 120);
}
