// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf_amc/msgcurs.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

static void CheckBuf(algo::ByteAry &buf, strptr result) {
    cstring out;
    ind_beg(atf_amc::MsgHeader_curs,hdr,ary_Getary(buf)) {
        if (atf_amc::Text *msg = atf_amc::Text_Castdown(*hdr)) {
            out << text_Getary(*msg);
        }
    }ind_end;
    prlog(out);
    vrfy_(out == result);
}

// -----------------------------------------------------------------------------

// Read 2 messages from byteary
void atf_amc::amctest_MsgCurs() {
    algo::ByteAry bigbuf;
    algo::ByteAry buf;
    Text_FmtByteAry(buf, "msg1;");
    ary_Addary(bigbuf, ary_Getary(buf));
    Text_FmtByteAry(buf, "msg2;");
    ary_Addary(bigbuf, ary_Getary(buf));
    CheckBuf(bigbuf,"msg1;msg2;");
}

// -----------------------------------------------------------------------------

// Byte array too small for message
void atf_amc::amctest_MsgCurs2() {
    algo::ByteAry buf;
    Text_FmtByteAry(buf, "msg1;");
    buf.ary_n--;
    CheckBuf(buf, "");
}

// -----------------------------------------------------------------------------

// Message too big for buffer;
void atf_amc::amctest_MsgCurs3() {
    algo::ByteAry buf;
    Text_FmtByteAry(buf, "msg1;")->length.value++;
    CheckBuf(buf, "");
}

// -----------------------------------------------------------------------------

// Byte array too small for even message header
void atf_amc::amctest_MsgCurs4() {
    algo::ByteAry buf;
    Text_FmtByteAry(buf, "msg1;");
    buf.ary_n=0;
    CheckBuf(buf, "");
}
