// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/msgcurs.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

static void CheckBuf(algo::ByteAry &buf, strptr expect) {
    cstring out;
    ind_beg(atf_amc::MsgHeader_curs,hdr,ary_Getary(buf)) {
        if (atf_amc::Text *msg = atf_amc::Text_Castdown(*hdr)) {
            out << text_Getary(*msg);
        }
    }ind_end;
    prlog(out);
    vrfy(out == expect,
         tempstr("failure")
         <<Keyval("expect",expect)
         <<Keyval("got",out));
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

    // Now do the same thing with GetAlloc / GetAllocAppend
    ary_RemoveAll(bigbuf);
    Text_FmtAlloc(ary_GetAlloc(bigbuf), "msg1;");
    Text_FmtAlloc(ary_GetAllocAppend(bigbuf), "msg2;");
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
