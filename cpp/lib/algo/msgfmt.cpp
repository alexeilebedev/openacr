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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/msgfmt.cpp
//
// amc generates <Header>Msgs_PrintFmt for every message header: one case per
// message, laid out from the schema, which prints a message under an
// algo::MsgFmt.  The pieces every case shares are here: indentation, the
// length, the byte fields, and the h_convert hook that may rewrite a byte field
// before it prints.

#include "include/algo.h"



// -----------------------------------------------------------------------------

// Return STR cut to about LIM characters, the middle replaced by its CRC32:
// abcabcabcabc [CRC:12345678] xyzxyzxyzxyz
// The characters are unquoted, so the caller passes the result through Keyval
// or strptr_ToSsim.  The result may run about 20 characters past LIM.
algo::tempstr algo_lib::LimitLengthCRC(algo::strptr str, int lim) {
    algo::tempstr ret;
    if (str.n_elems > lim) {
        int ncut = str.n_elems - lim;
        int cut1 = (str.n_elems - ncut) / 2;
        int cut2 = (str.n_elems + ncut) / 2;
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

// -----------------------------------------------------------------------------

// Append to OUT the indentation of the message FMT is printing, two spaces per
// level of nesting.
void algo_lib::MsgFmt_Indent(algo::MsgFmt &fmt, algo::cstring &out) {
    algo::char_PrintNTimes(' ', out, fmt.indent*2);
}

// -----------------------------------------------------------------------------

// Append LENGTH, the length of the message being printed, to OUT when FMT asks
// for lengths.
void algo_lib::MsgFmt_Showlen(algo::MsgFmt &fmt, u32 length, algo::cstring &out) {
    if (fmt.showlen) {
        out << "  msglen:" << length;
    }
}

// -----------------------------------------------------------------------------

// Return the printed form of DATA, the bytes of the field named NAME.
// With an h_convert hook installed on FMT, the hook receives the name and the
// bytes in convert_field and convert_val and leaves the printed form in
// convert_val, so a reader can decode a field whose bytes are a struct.  With
// none, the bytes print as they are.
algo::strptr algo_lib::MsgFmt_Convert(algo::MsgFmt &fmt, algo::strptr name, algo::strptr data) {
    algo::strptr ret = data;
    if (fmt.h_convert) {
        fmt.convert_field = name;
        fmt.convert_val = data;
        h_convert_Call(fmt,fmt);
        ret = fmt.convert_val;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Append DATA, the bytes of a stripped layer's field named NAME, to OUT followed
// by a colon, as the prefix of what the layer nests.  A stripped keyed record
// thus reads key:value, the form a tuple reader parses back.
void algo_lib::MsgFmt_PrintPrefix(algo::MsgFmt &fmt, algo::strptr name, algo::strptr data, algo::cstring &out) {
    out << MsgFmt_Convert(fmt,name,data) << ":";
}

// -----------------------------------------------------------------------------

// Append DATA, a byte field named NAME, to OUT.
// A value that is short and fits on a line prints as an attribute of the
// message's line; a longer one prints on a line of its own below the message,
// quoted and cut to the payload limit.  So one message type prints either way,
// as its content dictates: a ten-byte record reads data:hello, and a kilobyte
// one takes a second line.  Stripped (FMT.STRIP > 0), the bytes print whole and
// unquoted, which is what a reader asking for the payload alone wants.
void algo_lib::MsgFmt_PrintBytes(algo::MsgFmt &fmt, algo::strptr name, algo::strptr data, algo::cstring &out) {
    algo::strptr text = MsgFmt_Convert(fmt,name,data);
    if (fmt.strip > 0) {
        out << text;
    } else if (text.n_elems <= 64 && algo::FindChar(text,'\n') < 0) {
        out << Keyval(name, text);
    } else {
        out << "  " << name << ":\\" << eol;
        algo::char_PrintNTimes(' ', out, (fmt.indent+1)*2);
        out << algo::strptr_ToSsim(LimitLengthCRC(text, algo_lib::_db.payload_lim));
    }
}
