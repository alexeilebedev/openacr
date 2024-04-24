// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/lib_ams/expect.cpp -- Expect implementation
//

#include "include/lib_ams.h"

// -----------------------------------------------------------------------------

// In-process `expect` implementation allows pausing reading of inputs until
// a certain prlog output is produced
// Motivating example:
//   InputMsg1
//   InputMsg2
//   Expect text:blah
//   InputMsg3
// InputMsg3 is not processed until blah is printed.
// Implementation:
// When reading an input message:
//   clear output buffer
//   if read an ExpectMsg:
//      set expect string to the input data
//      check output buffer (match any outputs since last input against expect string)
//      set expect timeout
// Expect timeout:
//    exit process with error
// When printing:
//    add to output buffer (limit: 100K).
//   if expect mode, check output buffer
void lib_ams::ExpectEnable(strptr str) {
    _db.expect_str=str;
    hook_Set0(_db.th_expect, ExpectTimeout);
    _db.th_expect.delay = algo::ToSchedTime(_db.expect_timeout);
    _db.th_expect.time = algo_lib::_db.clock + _db.th_expect.delay;
    bh_timehook_Insert(_db.th_expect);
    lib_ams::cd_fdin_read_RemoveAll();
    prlog_cat(algo_lib_logcat_expect
              ,"lib_ams.expect_begin"
              <<Keyval("str",str)
              <<Keyval("timeout",_db.expect_timeout));
    ExpectCheck();
}

// Replace input reading function with one that pauses output
// to support ExpectMsg functionality
void lib_ams::ExpectAttach(lib_ams::FStream &stream) {
    if (stream.h_msg_orig == NULL) {
        stream.h_msg_orig = stream.h_msg;
        stream.h_msg_orig_ctx = stream.h_msg_ctx;
        h_msg_Set2(stream,stream, lib_ams::ExpectHook);
        prlog_cat(algo_lib_logcat_expect
                  ,"lib_ams.expect_attach"
                  <<Keyval("stream",stream.stream_id));
    }
}

// Function that replaces uses-provided message hook for a given stream
// Having no cost until the first call to ExpectEnable, the purpose
// is to block processing of any messages until expect_Str is empty
void lib_ams::ExpectHook(lib_ams::FStream &stream, ams::MsgHeader &msg) {
    if (ch_N(lib_ams::_db.expect_str)) {
        StopReading(stream);
    } else {
        if (ams::ExpectMsg *expect = ams::ExpectMsg_Castdown(msg)) {
            lib_ams::ExpectEnable(text_Getary(*expect));
        } else {
            h_msg_orig_Call(stream,msg);
        }
    }
}

// Save output to possibly check against expect string later
// Keep last 50k-100k of output
void lib_ams::ExpectSaveOutput(algo::strptr text) {
    static int lock=0;
    _db.expect_buf << text;
    if (lock++ == 0) {
        ExpectCheck();
        lock--;
    }
}

void lib_ams::ExpectTimeout() {
    prlog("lib_ams.expect_timeout"
          <<Keyval("success","N")
          <<Keyval("expect_str",_db.expect_str)
          <<Keyval("next_output",algo::LimitLengthEllipsis(ch_RestFrom(_db.expect_buf,_db.expect_pos),500))
          <<Keyval("comment","Timed output waiting for expected output"));
    abort();
}

// Check saved output buffer against expect string
// WARNING: calling prlog() from this function will cause an infinite loop
void lib_ams::ExpectCheck() {
    int buf_len = ch_N(_db.expect_buf);
    int str_len = ch_N(_db.expect_str);
    if (str_len) {
        // have something to look for?
        // searching inside the expect buffer..
        int loc = FindFrom(_db.expect_buf, _db.expect_str, _db.expect_pos, true);
        if (loc >= 0) {
            // resume reading messages from all inputs
            ind_beg(_db_fdin_curs,fdin,_db) {
                lib_ams::cd_fdin_read_Insert(fdin);
            }ind_end;
            // resume reading messages from stream
            if (lib_ams::FStream *dflt_stream = lib_ams::ind_stream_Find(lib_ams::_db.dflt_stream_id)) {
                prlog_cat(algo_lib_logcat_expect
                          ,"lib_ams.expect_end"
                          <<Keyval("str",_db.expect_str)
                          <<Keyval("comment","string matched, resume reading from default stream"));
                lib_ams::cd_poll_read_Insert(*dflt_stream);
            }
            bh_timehook_Remove(_db.th_expect);
            ch_RemoveAll(_db.expect_str);
        }
        // update reading position
        _db.expect_pos = loc==-1 ? buf_len : loc+str_len;
    }
    int maxbuf=50*1024;
    if (buf_len > maxbuf) {
        int skip = maxbuf / 2;
        memmove(_db.expect_buf.ch_elems, _db.expect_buf.ch_elems + skip, buf_len-skip);
        _db.expect_buf.ch_n -= skip;
        _db.expect_pos = i32_Max(_db.expect_pos-skip,0);
    }
}
