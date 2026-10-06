// Copyright (C) 2026 AlgoX2 Corp
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
// Target: lib_prot (lib) -- Library covering all protocols
// Exceptions: NO
// Source: cpp/lib_prot/ams.cpp
//

#include "include/algo.h"
#include "include/gen/ams_gen.h"
#include "include/gen/ams_gen.inl.h"
#include "include/gen/ws_gen.h"
#include "include/gen/ws_gen.inl.h"

u32 ws::payload_N(const ws::Frame& parent) {
    return payload_len_Get(parent);
}

u32 ws::payload_N(const ws::FrameMasked& parent) {
    return payload_len_Get(parent);
}

// I64Price8: fixed-point price with 8 decimal places
void ams::I64Price8_Print(ams::I64Price8 row, algo::cstring &str) {
    i64 whole = row.value / 100000000LL;
    i64 frac  = labs(row.value % 100000000LL);
    str << whole;
    if (frac) {
        str << ".";
        // print up to 8 digits, trimming trailing zeros
        tempstr fracstr;
        i64_PrintPadLeft(frac, fracstr, 8);
        int end = ch_N(fracstr);
        while (end > 0 && ch_qFind(fracstr, end-1) == '0') {
            --end;
        }
        str << ch_FirstN(fracstr, end);
    }
}

bool ams::I64Price8_ReadStrptrMaybe(ams::I64Price8 &row, algo::strptr str) {
    double dval = 0;
    bool ok = double_ReadStrptrMaybe(dval, str);
    if (ok) {
        row.value = i64(dval * 100000000.0 + (dval >= 0 ? 0.5 : -0.5));
    }
    return ok;
}

// SampMengSymbol extern functions are not needed (Raw printfmt generates them)

// Read IN_STR into PARENT and return whether every token of it parsed.  IN_STR
// is the value of a process's -proc argument, a comma-separated list of
// key:value tokens: id, prefix, in, out, nick, gw and shadow.  A token without
// a colon is the id, so `-proc:dev1.ams_sendtest-0-0`, the form every module takes,
// names the id alone.  An empty token is passed over.  The whole string is
// refused when a key is one this function does not know, when the id is named
// twice, bare or as id:, or when one ring of the in and out pair is named without
// the other.  So a misspelled key, a stray bare word, or a comma that split one
// value in two is an error and never a silent default.
bool ams::Procspec_ReadStrptrMaybe(ams::Procspec &parent, algo::strptr in_str) {
    bool ok = true;
    bool has_id = false;
    algo::StringIter it(in_str);
    while (ok && !it.EofQ()) {
        algo::strptr token = algo::GetTokenChar(it, ',');
        bool named = algo::FindChar(token, ':') != -1;
        algo::strptr key = named ? algo::Pathcomp(token, ":LL") : algo::strptr();
        algo::strptr value = named ? algo::Pathcomp(token, ":LR") : token;
        if (ch_N(token) == 0) {
            // a trailing comma names nothing
        } else if (!named || key == "id") {
            ok = !has_id;
            has_id = true;
            parent.id = value;
        } else if (key == "prefix") {
            parent.prefix = value;
        } else if (key == "in") {
            ok = ams::GrpId_ReadStrptrMaybe(parent.in, value);
        } else if (key == "out") {
            ok = ams::GrpId_ReadStrptrMaybe(parent.out, value);
        } else if (key == "nick") {
            parent.nick = value;
        } else if (key == "gw") {
            parent.gw = value;
        } else if (key == "shadow") {
            ok = bool_ReadStrptrMaybe(parent.shadow, value);
        } else {
            ok = false;
        }
    }
    ok = ok && (parent.in == ams::GrpId()) == (parent.out == ams::GrpId());
    return ok;
}

// Append to STR the -proc value that ROW reads back from: one key:value token
// for each field ROW sets, in the order id, prefix, in, out, nick, gw, shadow,
// joined by commas.
void ams::Procspec_Print(ams::Procspec &row, algo::cstring &str) {
    algo::ListSep ls(",");
    if (ch_N(row.id) > 0) {
        str << ls << "id:" << row.id;
    }
    if (ch_N(row.prefix) > 0) {
        str << ls << "prefix:" << row.prefix;
    }
    if (!(row.in == ams::GrpId())) {
        str << ls << "in:" << row.in;
    }
    if (!(row.out == ams::GrpId())) {
        str << ls << "out:" << row.out;
    }
    if (ch_N(row.nick) > 0) {
        str << ls << "nick:" << row.nick;
    }
    if (ch_N(row.gw) > 0) {
        str << ls << "gw:" << row.gw;
    }
    if (row.shadow) {
        str << ls << "shadow:Y";
    }
}

// -----------------------------------------------------------------------------

// Print log message ROW to OUT as header stripping shows it: the process, the
// log category and the text, as in "ams_sendtest-0-0 : VOX PER TUBUM".  A reader that
// strips headers off a process's output stream reads what each process said,
// and the timestamp and length are left out.  FMT supplies the indentation.
void ams::LogMsg_PrintStripped(algo::MsgFmt &fmt, ams::LogMsg &row, algo::cstring &out) {
    if (fmt.pretty) {
        algo_lib::MsgFmt_Indent(fmt, out);
    }
    out << row.proc_id << " " << logcat_Getary(row) << ": " << text_Getary(row);
}
