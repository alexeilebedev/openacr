// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/disp/print.cpp -- Dispatch print
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Generate Dispatch_Print function
void amc::Disp_Print(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$typefld", FieldvalExpr(disp.p_ctype_hdr, *disp.p_ctype_hdr->c_typefld->p_field, "msg"));

    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..Print"));
    func.glob = true;
    func.ret = "bool";
    Ins(&R, func.comment, "Print message to STR. If message is too short for MSG_LEN, print nothing.");
    Ins(&R, func.comment, "MSG.LENGTH must have already been validated against msg_len.");
    Ins(&R, func.comment, "This function will additionally validate that sizeof(Msg) <= msg_len");
    Ins(&R, func.proto, "$Dname_Print(algo::cstring &str, $Hdrtype &msg, u32 msg_len)", false);
    Ins(&R, func.body, "switch($typefld) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,disp) {
        vrfy(msg.p_ctype->c_msgtype, tempstr()<<"amc.Disp_Print  ctype:"<<ctype_Get(msg)<<"  error:'No msgtype defined for ctype'");
        Set(R, "$Msgtype", msg.p_ctype->c_msgtype->type.value);
        Set(R, "$Msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(msg)));
        Ins(&R, func.body, "case $Msgtype: {");
        Ins(&R, func.body, "    if (sizeof($Ctype) > msg_len) { return false; }");
        Ins(&R, func.body, "    $Msgname_Print(($Ctype&)(msg), str);");
        Ins(&R, func.body, "    return true;");
        Ins(&R, func.body, "}");
    }ind_end;
    Ins(&R, func.body, "default:\n");
    Ins(&R, func.body, "    return false;");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

// Return true if FIELD is a varlen field: Varlen or Opt.
static bool VarlenQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Varlen || field.reftype == dmmeta_Reftype_reftype_Opt;
}

// -----------------------------------------------------------------------------

// Return true if message CTYPE has a varlen field.
static bool HasVarlenQ(amc::FCtype &ctype) {
    bool ret = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        ret = ret || VarlenQ(field);
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return true if FIELD, a varlen field of a message under dispatch DISP, holds
// messages framed by the dispatch's header.
static bool NestedQ(amc::FDispatch &disp, amc::FField &field) {
    return field.p_arg == disp.p_ctype_hdr || amc::UltimateBaseType(field.p_arg, NULL) == disp.p_ctype_hdr;
}

// -----------------------------------------------------------------------------

// Return true if FIELD is a varlen field of bytes: char or u8.
static bool BytesQ(amc::FField &field) {
    return field.arg == "char" || field.arg == "u8";
}

// -----------------------------------------------------------------------------

// Append to the body of FUNC, the generated <Dname>_PrintFmt, the case of
// DISPATCH_MSG.  The case is laid out from the message's schema: its fixed fields
// print as the message's line, each varlen field after them by what it holds.  A
// field of messages framed by the dispatch's header makes the message a layer,
// a byte field prints inline when it is short, and on a line of its own when it
// is not; a field of rows prints a row per line.  The msgtype's strip says what
// header stripping does.  Always removes the message in every format, so an
// envelope yields what it nests even in ssim and bin output.  Decode removes it
// only when the output decodes (pretty, not bin): a layer yields what it nests
// and a leaf the bytes it carries, while ssim and bin print it whole, which is
// what a reader parsing the ssim tuple relies on.  Extern hands the decoded
// stripped form to a hand-written <Msg>_PrintStripped.  N prints it whole.  R
// carries $ns, $Dname, $Hdrtype and $msglen.
static void GenPrintFmtCase(algo_lib::Replscope &R, amc::FDispatch &disp, amc::FDispatchmsg &dispatch_msg, amc::FFunc &func) {
    algo::cstring &body = func.body;
    amc::FCtype &ctype = *dispatch_msg.p_ctype;
    amc::FMsgtype &msgtype = *ctype.c_msgtype;
    Set(R, "$Msgtype", msgtype.type.value);
    Set(R, "$Msgname", StripNs("",ctype.ctype));
    Set(R, "$Ctype", amc::NsToCpp(ctype.ctype));
    Set(R, "$Fns", ns_Get(ctype));
    amc::FField *nested = NULL;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (VarlenQ(field) && !nested && NestedQ(disp,field)) {
            nested = &field;
        }
    }ind_end;
    bool always = msgtype.strip == dmmeta_Msgstrip_msgstrip_Always;
    bool strippable = nested && (always || msgtype.strip == dmmeta_Msgstrip_msgstrip_Decode);
    bool strip_extern = msgtype.strip == dmmeta_Msgstrip_msgstrip_Extern;
    bool keepwhole = msgtype.strip == dmmeta_Msgstrip_msgstrip_N;
    if (strip_extern) {
        amc::FFunc &stripped = amc::ind_func_GetOrCreate(Subst(R, "$Fns.$Msgname..PrintStripped"));
        stripped.glob = true;
        stripped.extrn = true;
        stripped.ret = "void";
        Ins(&R, stripped.comment, "Print ROW to OUT in the form header stripping gives it (msgtype strip:Extern).");
        Ins(&R, stripped.proto, "$Msgname_PrintStripped(algo::MsgFmt &fmt, $Ctype &row, algo::cstring &out)", false);
    }
    Ins(&R, body, "case $Msgtype: {");
    Ins(&R, body, "    if ($Ctype *castrow = $Fns::$Msgname_Castdown(msg)) {");
    Ins(&R, body, "        $Ctype &row = *castrow;");
    if (strippable) {
        Set(R, "$nested", name_Get(*nested));
        Set(R, "$stripcond", always ? "fmt.strip > 0" : "fmt.strip > 0 && fmt.pretty && fmt.format != algo_MsgFmt_format_bin");
        Ins(&R, body, "        if ($stripcond) {");
        bool before = true;
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            before = before && &field != nested;
            if (before && field.reftype == dmmeta_Reftype_reftype_Varlen && BytesQ(field)) {
                Set(R, "$fld", name_Get(field));
                Set(R, "$bytes", field.arg == "u8" ? "algo::memptr_ToStrptr($Fns::$fld_Getary(row))" : "$Fns::$fld_Getary(row)");
                Ins(&R, body, "            algo_lib::MsgFmt_PrintPrefix(fmt, \"$fld\", $bytes, out);");
            }
        }ind_end;
        Ins(&R, body, "            fmt.strip--;");
        if (nested->reftype == dmmeta_Reftype_reftype_Opt) {
            Ins(&R, body, "            if ($Hdrtype *sub = $Fns::$nested_Get(row)) {");
            Ins(&R, body, "                $Dname_PrintFmt(fmt, *sub, out);");
            Ins(&R, body, "            } else {");
            Ins(&R, body, "                out << \"[MALFORMED] \";");
            Ins(&R, body, "            }");
        } else {
            Ins(&R, body, "            if ($Fns::$nested_N(row) == 0) {");
            Ins(&R, body, "                out << \"[MALFORMED] \";");
            Ins(&R, body, "            }");
            Ins(&R, body, "            ind_beg($Fns::$Msgname_$nested_curs, sub, row) {");
            Ins(&R, body, "                if (ind_curs(sub).index > 0 && fmt.format != algo_MsgFmt_format_bin) {");
            Ins(&R, body, "                    out << eol;");
            Ins(&R, body, "                }");
            Ins(&R, body, "                $Dname_PrintFmt(fmt, sub, out);");
            Ins(&R, body, "            }ind_end;");
        }
        Ins(&R, body, "            fmt.strip++;");
        Ins(&R, body, "        } else if (fmt.format == algo_MsgFmt_format_bin) {");
    } else if (strip_extern) {
        Ins(&R, body, "        if (fmt.strip > 0 && fmt.pretty && fmt.format != algo_MsgFmt_format_bin) {");
        Ins(&R, body, "            $Fns::$Msgname_PrintStripped(fmt, row, out);");
        Ins(&R, body, "        } else if (fmt.format == algo_MsgFmt_format_bin) {");
    } else {
        Ins(&R, body, "        if (fmt.format == algo_MsgFmt_format_bin) {");
    }
    Ins(&R, body, "            out << algo::strptr((char*)&msg, $msglen);");
    Ins(&R, body, "        } else if (!fmt.pretty) {");
    Ins(&R, body, "            $Fns::$Msgname_Print(row, out);");
    Ins(&R, body, "        } else {");
    if (keepwhole) {
        Ins(&R, body, "            i32 strip = fmt.strip;");
        Ins(&R, body, "            fmt.strip = 0;");
    }
    Ins(&R, body, "            algo_lib::MsgFmt_Indent(fmt, out);");
    Ins(&R, body, "            if (fmt.strip == 0) {");
    Ins(&R, body, "                algo::cstring &str = out;");
    if (!amc::GenPrintFixedTuple(ctype, func)) {
        Ins(&R, body, "                $Fns::$Msgname_Print(row, str);");
    }
    Ins(&R, body, "                algo_lib::MsgFmt_Showlen(fmt, $msglen, out);");
    Ins(&R, body, "            }");
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        Set(R, "$fld", name_Get(field));
        if (field.reftype == dmmeta_Reftype_reftype_Varlen && BytesQ(field)) {
            Set(R, "$bytes", field.arg == "u8" ? "algo::memptr_ToStrptr($Fns::$fld_Getary(row))" : "$Fns::$fld_Getary(row)");
            Ins(&R, body, "            algo_lib::MsgFmt_PrintBytes(fmt, \"$fld\", $bytes, out);");
        } else if (VarlenQ(field) && NestedQ(disp,field) && field.reftype == dmmeta_Reftype_reftype_Opt) {
            Ins(&R, body, "            if ($Hdrtype *sub = $Fns::$fld_Get(row)) {");
            Ins(&R, body, "                out << \"  $fld:\\\\\" << eol;");
            Ins(&R, body, "                fmt.indent++;");
            Ins(&R, body, "                $Dname_PrintFmt(fmt, *sub, out);");
            Ins(&R, body, "                fmt.indent--;");
            Ins(&R, body, "            }");
        } else if (VarlenQ(field) && NestedQ(disp,field)) {
            Ins(&R, body, "            if ($Fns::$fld_N(row) > 0) {");
            Ins(&R, body, "                out << \"  $fld:\\\\\";");
            Ins(&R, body, "                ind_beg($Fns::$Msgname_$fld_curs, sub, row) {");
            Ins(&R, body, "                    out << eol;");
            Ins(&R, body, "                    fmt.indent++;");
            Ins(&R, body, "                    $Dname_PrintFmt(fmt, sub, out);");
            Ins(&R, body, "                    fmt.indent--;");
            Ins(&R, body, "                }ind_end;");
            Ins(&R, body, "            }");
        } else if (field.reftype == dmmeta_Reftype_reftype_Varlen && amc::HasStringPrintQ(*field.p_arg)) {
            Set(R, "$Elemprint", tempstr() << ns_Get(*field.p_arg) << "::" << StripNs("",field.arg) << "_Print");
            Ins(&R, body, "            ind_beg($Fns::$Msgname_$fld_curs, elem, row) {");
            Ins(&R, body, "                if (fmt.strip == 0 || ind_curs(elem).index > 0) {");
            Ins(&R, body, "                    out << eol;");
            Ins(&R, body, "                    algo::char_PrintNTimes(' ', out, (fmt.indent + (fmt.strip == 0))*2);");
            Ins(&R, body, "                }");
            Ins(&R, body, "                $Elemprint(elem, out);");
            Ins(&R, body, "            }ind_end;");
        }
    }ind_end;
    if (keepwhole) {
        Ins(&R, body, "            fmt.strip = strip;");
    }
    Ins(&R, body, "        }");
    Ins(&R, body, "        printed = true;");
    Ins(&R, body, "    }");
    Ins(&R, body, "    break;");
    Ins(&R, body, "}");
}

// -----------------------------------------------------------------------------

// Generate <Dname>_PrintFmt for DISP, the dispatch of every message framed by one
// header with a length field: print a message under an algo::MsgFmt -- stripped of FMT.STRIP outer
// layers, as binary, as one ssim tuple, or pretty, with nested messages and byte
// payloads on lines of their own.  A message with no varlen field needs no case
// of its own: it prints as its tuple, indented when pretty.  A message of a type
// the dispatch does not know prints as its quoted bytes.
void amc::Disp_PrintFmt(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$typefld", FieldvalExpr(disp.p_ctype_hdr, *disp.p_ctype_hdr->c_typefld->p_field, "msg"));
    Set(R, "$msglen", amc::LengthExpr(*disp.p_ctype_hdr, "msg"));
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..PrintFmt"));
    func.glob = true;
    func.ret = "void";
    Ins(&R, func.comment, "Print message MSG to OUT under format FMT: FMT.STRIP outer layers stripped,");
    Ins(&R, func.comment, "as binary, as an ssim tuple, or pretty, with nested messages and byte payloads");
    Ins(&R, func.comment, "on lines of their own.  Each case is laid out from the message's schema.");
    Ins(&R, func.proto, "$Dname_PrintFmt(algo::MsgFmt &fmt, $Hdrtype &msg, algo::cstring &out)", false);
    Ins(&R, func.body, "bool printed = false;");
    Ins(&R, func.body, "switch($typefld) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg, disp) {
        if (HasVarlenQ(*dispatch_msg.p_ctype) && dispatch_msg.p_ctype->c_msgtype) {
            GenPrintFmtCase(R, disp, dispatch_msg, func);
        }
    }ind_end;
    Ins(&R, func.body, "default:");
    Ins(&R, func.body, "    break;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (!printed && fmt.format == algo_MsgFmt_format_bin) {");
    Ins(&R, func.body, "    out << algo::strptr((char*)&msg, $msglen);");
    Ins(&R, func.body, "} else if (!printed) {");
    Ins(&R, func.body, "    if (fmt.pretty) {");
    Ins(&R, func.body, "        algo_lib::MsgFmt_Indent(fmt, out);");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (!$Dname_Print(out, msg, $msglen)) {");
    Ins(&R, func.body, "        out << algo::strptr_ToSsim(algo::strptr((char*)&msg, $msglen));");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

// Generate <Dname>_HeartbeatQ for DISP: whether a message is a heartbeat, which
// its msgtype says with heartbeat:Y.  A trace leaves heartbeats out unless asked,
// since they arrive many times a second and say only that a peer is alive.
void amc::Disp_HeartbeatQ(amc::FDispatch &disp) {
    algo_lib::Replscope R;
    R.strict=2;
    Set(R, "$ns", ns_Get(disp));
    Set(R, "$Hdrtype", amc::NsToCpp(disp.p_ctype_hdr->ctype));
    Set(R, "$Dname", name_Get(disp));
    Set(R, "$typefld", FieldvalExpr(disp.p_ctype_hdr, *disp.p_ctype_hdr->c_typefld->p_field, "msg"));
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Dname..HeartbeatQ"));
    func.glob = true;
    func.ret = "bool";
    Ins(&R, func.comment, "Return true if MSG is a heartbeat: a periodic liveness message.");
    Ins(&R, func.proto, "$Dname_HeartbeatQ($Hdrtype &msg)", false);
    Ins(&R, func.body, "bool ret = false;");
    Ins(&R, func.body, "switch($typefld) {");
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg, disp) {
        if (dispatch_msg.p_ctype->c_msgtype && dispatch_msg.p_ctype->c_msgtype->heartbeat) {
            Set(R, "$Msgtype", dispatch_msg.p_ctype->c_msgtype->type.value);
            Ins(&R, func.body, "case $Msgtype:");
            Ins(&R, func.body, "    ret = true;");
            Ins(&R, func.body, "    break;");
        }
    }ind_end;
    Ins(&R, func.body, "default:");
    Ins(&R, func.body, "    break;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");
}
