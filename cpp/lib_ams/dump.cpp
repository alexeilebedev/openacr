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
// Source: cpp/lib_ams/dump.cpp
//

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

// Print table of shms in lib_ams, using ssim format
// if MEMBER is specified, print member information as well
void lib_ams::DumpShmTableDflt(algo_lib::Regx &regx, bool member) {
    ind_beg(lib_ams::_db_shm_curs,shm,lib_ams::_db) {
        if (Regx_Match(regx,tempstr()<<shm.shm_id) && shm.shm_region.elems) {
            prlog(lib_ams::_db.proc_id
                  <<"  shm"
                  <<Keyval("shm_id",shm.shm_id)
                  <<Keyval("roff",(shm.c_read ? shm.c_read->off : 0))
                  <<Keyval("woff",(shm.c_write ? shm.c_write->off : 0))
                  <<Keyval("writelimit",shm.writelimit)
                  <<Keyval("flags",shm.flags)
                  <<Keyval("filename",shm.filename)
                  <<Keyval("fd",shm.shm_file.fd.value)
                  <<Keyval("handle",shm.shm_handle)
                  <<Keyval("size",shm.shm_region.n_elems)
                  <<Keyval("n_write_block",shm.n_write_block)
                  <<Keyval("nbits",algo::FloorLog2(u64(shm.offset_mask+1)))
                  );
            if (member) {
                ind_beg(lib_ams::shm_c_shmember_curs,shmember,shm) {
                    prlog(lib_ams::_db.proc_id
                          <<"  shmember"
                          <<Keyval("shmember",shmember.shmember)
                          <<Keyval("off",shmember.off)
                          <<Keyval("budget",shmember.budget));
                }ind_end;
            }
        }
    }ind_end;
}

// Print table of shms in lib_ams, using a more readable layout
void lib_ams::DumpShmTableVisual(algo_lib::Regx &regx) {
    algo_lib::FTxttbl tbl;
    AddCols(tbl,"shm,size,budget");
    // list of processes reading the shm
    ind_beg(lib_ams::_db_zd_proc_curs,proc,lib_ams::_db) {
        AddCol(tbl,tempstr()<<proc.proc_id);
    }ind_end;
    ind_beg(lib_ams::_db_shm_curs,shm,lib_ams::_db) {
        if (Regx_Match(regx,tempstr()<<shm.shm_id) && shm.shm_region.elems) {
            algo_lib::FTxtrow &row= AddRow(tbl);
            u64 budget=0;
            ind_beg(lib_ams::shm_c_shmember_curs,shmember,shm) {
                if (w_Get(shmember.shmember.flags)) {
                    budget=u64_Max(budget,shmember.budget);
                }
            }ind_end;
            AddCol(tbl,tempstr()<<shm.shm_id);
            AddCol(tbl,tempstr()<<shm.shm_region.n_elems);
            AddCol(tbl,tempstr()<<budget);
            ind_beg(lib_ams::_db_zd_proc_curs,proc,lib_ams::_db) {
                algo_lib::FTxtcell &cell = algo_lib::AddCell(row,"",algo_TextJust_j_left);
                algo::ListSep ls(",");
                algo::SchedTime last_hb;
                u64 roff=0;
                u64 woff=0;
                lib_ams::FShmember *read_shmember = ind_shmember_Find(ams::ShmemberId(shm.shm_id,proc.proc_id,ams_ShmemberFlags_r));
                lib_ams::FShmember *write_shmember = ind_shmember_Find(ams::ShmemberId(shm.shm_id,proc.proc_id,ams_ShmemberFlags_w));
                if (read_shmember) {
                    roff = read_shmember->off;
                    last_hb = read_shmember->last_hb;
                    cell.text << ls<<"R";
                    double hbbehind = algo::ElapsedSecs(last_hb, algo_lib::_db.clock);
                    bool online = hbbehind < 2.0;
                    cell.style = online ? algo_TermStyle_green : algo_TermStyle_red;
                }
                if (write_shmember) {
                    woff = write_shmember->off;
                    cell.text <<"W";
                    cell.style = algo_TermStyle_blue;
                    if (shm.shm_id == lib_ams::_db.dflt_shm_id) {
                        cell.text << "*";
                    }
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

static void Showlen(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out) {
    if (fmt.showlen) {
        out << "  msglen:" << msg.length;
    }
}

// Print message MSG to string OUT according to format FMT
// if FMT.STRIP > 0, strip this many outer "layers";
// if FMT.BIN, the message is printed as pure binary; otherwise, convert to text
// if FMT.PRETTY, every next layer / payload is printed on a new line with indent
// for readability.
// FMT.PAYLOAD_LEN limits maximum printed payload length, allowing to fit one messge per
// screen even if payload is 10MB
// Finally, if FMT.SHOWLEN is true, message length is included in output.
void lib_ams::PrintMsg(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out) {
    bool printed=false;
    if (fmt.strip>0) {
        // strip headers
    } else if (fmt.format==lib_ams_MsgFmt_format_bin) {
        // just dump the binary part
        out << algo::strptr((char*)&msg,msg.length);
        printed=true;
    }
    if (!printed && fmt.pretty) {
        algo::char_PrintNTimes(' ', out, fmt.indent*2);
        if (ams::LogMsg *logmsg = ams::LogMsg_Castdown(msg)) {
            if (fmt.strip == 0) {
                out << "ams.LogMsg";
                Showlen(fmt,msg,out);
                out << Keyval("proc_id",logmsg->proc_id)
                    << Keyval("tstamp",logmsg->tstamp)
                    << Keyval("logcat",logcat_Getary(*logmsg));
                out << "  text:\\" << eol;
                char_PrintNTimes(' ', out, (fmt.indent+1)*2);
                out << LimitLengthCRC(text_Getary(*logmsg), fmt.payload_lim);
            } else {
                out << logmsg->proc_id
                    <<" "<<logcat_Getary(*logmsg)
                    <<": "
                    <<text_Getary(*logmsg);
            }
            printed=true;
        }
    }
    if (!printed) {
        MsgHeaderMsgs_Print(out,msg,msg.length);
    }
}

// -----------------------------------------------------------------------------

// This function should be called if the ams logcat is enabled
// It prints the given MSG to ams logcat using pretty format.
// Heartbeats (ShmHb, MemberHb, PubMetric) are skipped unless verbose is on
void lib_ams::TraceMsg(ams::Shmmsg *msg, ams::MsgHeader *payload) {
    bool trace = algo_lib_logcat_verbose2.enabled
        || !(payload->type == ams_MsgHeader_type_ams_ShmHbMsg);
    if (trace) {
        tempstr out;
        ams::MsgHeader &what=msg ? Castbase(*msg) : *payload;
        lib_ams::MsgFmt fmt;
        lib_ams::PrintMsg(fmt, what, out);
        if (algo_lib::LogcatFilterQ(algo_lib_logcat_ams,out)) {
            prlog(lib_ams::_db.proc_id<<" "<<out);
        }
    }
}

// -----------------------------------------------------------------------------

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
    fmt.payload_lim=80;// limit output length
    lib_ams::PrintMsg(fmt, msg, out);
    return out;
}
