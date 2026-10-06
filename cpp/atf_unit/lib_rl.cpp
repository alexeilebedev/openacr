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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/lib_rl.cpp
//

#include "include/algo.h"
#include "include/atf_unit.h"
#include "include/lib_rl.h"

// Offer CANDS (a |-separated list) as the answer to a completion query, the
// way the h_compl hook does, and report what a Tab would insert.
static tempstr Complete(strptr cands) {
    lib_rl::cand_RemoveAll();
    ind_beg(algo::Sep_curs, cand, cands, '|') {
        lib_rl::AddCandidate(cand);
    }ind_end;
    tempstr ret(lib_rl::CommonPrefix());
    lib_rl::cand_RemoveAll();
    return ret;
}

// Tab inserts what every candidate agrees on and stops there, so an answer
// naming one word completes it outright and an answer naming several advances
// only as far as they are alike.  An empty answer inserts nothing, which is
// what keeps a mistyped verb on the line instead of being replaced by whatever
// the host's working directory happens to hold.
void atf_unit::unittest_lib_rl_CommonVerbPrefix() {
    // one candidate: the whole word
    vrfyeq_(Complete("path"), "path");
    vrfyeq_(Complete("-ssim"), "-ssim");
    // several: only the part they share
    vrfyeq_(Complete("part|passwd|path"), "pa");
    vrfyeq_(Complete("-ssim|-stop"), "-s");
    // one word that is a prefix of another stops at the shorter
    vrfyeq_(Complete("read|readonly"), "read");
    // an unrelated neighbour drags the shared prefix back to nothing
    vrfyeq_(Complete("read|node"), "");
    // nothing offered, nothing inserted
    vrfyeq_(Complete(""), "");
    lib_rl::cand_RemoveAll();
}


// A line being edited is split the same way whoever answers a completion would
// split it: which word is being typed, and, when that word is a path, where its
// directory ends and its name begins.
void atf_unit::unittest_lib_rl_Word() {
    // the first word is the one nothing but blanks precedes
    vrfyeq_(lib_rl::FirstWordQ("ls /pub", 0), true);
    vrfyeq_(lib_rl::FirstWordQ("  ls", 2), true);
    vrfyeq_(lib_rl::FirstWordQ("ls /pub", 3), false);
    vrfyeq_(lib_rl::FirstWordQ("ls -n /pub", 6), false);
    // the word a completion acts on is the last one
    vrfyeq_(lib_rl::LastWord(""), "");
    vrfyeq_(lib_rl::LastWord("ls"), "ls");
    vrfyeq_(lib_rl::LastWord("ls /pub"), "/pub");
    vrfyeq_(lib_rl::LastWord("ls -l /public/an"), "/public/an");
    vrfyeq_(lib_rl::LastWord("ls "), "");
    // a word with no slash names no directory, so the caller's own applies
    vrfyeq_(lib_rl::WordDir("an"), "");
    vrfyeq_(lib_rl::WordName("an"), "an");
    // one leading slash is the root, which keeps its slash
    vrfyeq_(lib_rl::WordDir("/pub"), "/");
    vrfyeq_(lib_rl::WordName("/pub"), "pub");
    vrfyeq_(lib_rl::WordDir("/"), "/");
    vrfyeq_(lib_rl::WordName("/"), "");
    // deeper, the directory is everything before the last slash
    vrfyeq_(lib_rl::WordDir("/public/an"), "/public");
    vrfyeq_(lib_rl::WordName("/public/an"), "an");
    vrfyeq_(lib_rl::WordDir("/public/"), "/public");
    vrfyeq_(lib_rl::WordName("/public/"), "");
    vrfyeq_(lib_rl::WordDir("a/b/c"), "a/b");
    vrfyeq_(lib_rl::WordName("a/b/c"), "c");
    // the two halves put back together are the word again
    vrfyeq_(lib_rl::WordName(""), "");
    vrfyeq_(lib_rl::WordDir(""), "");
}


// Where a session's history is kept: the app names the file, and it sits in the
// user's home.
void atf_unit::unittest_lib_rl_HistoryFile() {
    tempstr named = lib_rl::HistoryFile("demo");
    vrfyeq_(algo::Pathcomp(named, "/RR"), ".demo_history");
    tempstr anon = lib_rl::HistoryFile("");
    vrfyeq_(algo::Pathcomp(anon, "/RR"), ".history");
    // both land in the same directory
    vrfyeq_(algo::Pathcomp(named, "/RL"), algo::Pathcomp(anon, "/RL"));
}

// -----------------------------------------------------------------------------

// Feed KEYS to the editor one byte at a time, the way GetLine does with what
// it reads from a terminal.
static void Type(strptr keys) {
    for (int i = 0; i < ch_N(keys); i++) {
        lib_rl::ApplyByte(u8(keys.elems[i]));
    }
}

// Offer the verbs `read` and `reset` to the completion query COMPL_ that they
// complete.  CTX is the hook's context, unused.
static void OfferRe(int &ctx, lib_rl::Compl &compl_) {
    (void)ctx;
    if (StartsWithQ("read", compl_.word)) {
        lib_rl::AddCandidate("read");
    }
    if (StartsWithQ("reset", compl_.word)) {
        lib_rl::AddCandidate("reset");
    }
}

// Put the editor in state edit with an empty line and HIST as its history,
// without touching the terminal.
static void EditBegin(strptr hist) {
    lib_rl::_db.state = lib_rl::State(lib_rl_State_edit);
    lib_rl::_db.keystate = lib_rl::Keystate(lib_rl_Keystate_plain);
    lib_rl::hist_RemoveAll();
    ind_beg(algo::Sep_curs, line, hist, '|') {
        if (ch_N(line)) {
            lib_rl::hist_Alloc() = line;
        }
    }ind_end;
    lib_rl::_db.hist_pos = lib_rl::hist_N();
    ch_RemoveAll(lib_rl::_db.edit);
    lib_rl::_db.point = 0;
    lib_rl::SkipLine();
}

// Leave the editor idle and empty, as the other tests expect it.
static void EditEnd() {
    EditBegin("");
    lib_rl::_db.state = lib_rl::State(lib_rl_State_idle);
}

// The keys of the editor change the line and the cursor the way the emacs
// bindings do: motion by character and by word, kill and yank, deletion over a
// multi-byte character, and the escape sequences a terminal sends for the
// arrows, Home, End and Delete.
void atf_unit::unittest_lib_rl_EditKey() {
    EditBegin("");
    Type("hello world");
    Type("\x01X\x05!");
    vrfyeq_(lib_rl::_db.edit, "Xhello world!");
    Type("\x17");
    vrfyeq_(lib_rl::_db.edit, "Xhello ");
    vrfyeq_(lib_rl::_db.killbuf, "world!");
    Type("\x19");
    vrfyeq_(lib_rl::_db.edit, "Xhello world!");
    // meta-b twice reaches the first word, and meta-d kills it
    Type("\033b\033b\033d");
    vrfyeq_(lib_rl::_db.edit, " world!");
    // Home, then ctrl-right to the end of the next word
    Type("\033[H\033[1;5C");
    vrfyeq_(lib_rl::_db.point, 7);
    // a left arrow and Delete, then SS3 Home and Delete
    Type("\033[D\033[3~");
    vrfyeq_(lib_rl::_db.edit, " world");
    Type("\033OH\033[3~");
    vrfyeq_(lib_rl::_db.edit, "world");
    // SS3 End, two left arrows, ^K
    Type("\033OF\033[D\033[D\x0b");
    vrfyeq_(lib_rl::_db.edit, "wor");
    Type("\x15");
    vrfyeq_(lib_rl::_db.edit, "");
    // backspace takes the whole of a multi-byte character
    Type(" \xc3\xa9\xe2\x82\xac\x7f");
    vrfyeq_(lib_rl::_db.edit, " \xc3\xa9");
    Type("\x02\x02\x06");
    vrfyeq_(lib_rl::_db.point, 1);
    // a lone ESC leaves a key half read until ^C cancels it
    Type("\033");
    vrfyeq_(lib_rl::SubstateQ(), true);
    lib_rl::CancelSubstate();
    vrfyeq_(lib_rl::SubstateQ(), false);
    Type("\r");
    vrfyeq_(lib_rl::LineValidQ(), true);
    vrfyeq_(lib_rl::_db.line, " \xc3\xa9");
    vrfyeq_(lib_rl::_db.edit, "");
    EditEnd();
}

// ^P and ^N walk history and come back to the line being typed.  ^R searches
// history backward; ^G restores the line the search began with, and Enter
// accepts the match.
void atf_unit::unittest_lib_rl_EditHistory() {
    EditBegin("ls /a|pwd");
    Type("typed");
    Type("\x10");
    vrfyeq_(lib_rl::_db.edit, "pwd");
    Type("\033[A");
    vrfyeq_(lib_rl::_db.edit, "ls /a");
    Type("\033[A");
    vrfyeq_(lib_rl::_db.edit, "ls /a");
    Type("\x0e\x0e");
    vrfyeq_(lib_rl::_db.edit, "typed");
    Type("\x12l");
    vrfyeq_(lib_rl::_db.state.value, lib_rl_State_search);
    vrfyeq_(lib_rl::_db.edit, "ls /a");
    vrfyeq_(lib_rl::_db.point, 0);
    Type("\x07");
    vrfyeq_(lib_rl::_db.state.value, lib_rl_State_edit);
    vrfyeq_(lib_rl::_db.edit, "typed");
    Type("\x12w\x7fp");
    vrfyeq_(lib_rl::_db.edit, "pwd");
    // a key with no meaning in search ends it on the match
    Type("\x05X");
    vrfyeq_(lib_rl::_db.state.value, lib_rl_State_edit);
    vrfyeq_(lib_rl::_db.edit, "pwdX");
    Type("\x12zzz");
    vrfyeq_(lib_rl::_db.edit, "pwdX");
    Type("\r");
    vrfyeq_(lib_rl::LineValidQ(), true);
    vrfyeq_(lib_rl::_db.line, "pwdX");
    lib_rl::SkipLine();
    // ^N after a search comes back to the new line typed before it
    Type("fresh\x12l\x05");
    vrfyeq_(lib_rl::_db.edit, "ls /a");
    Type("\x0e\x0e");
    vrfyeq_(lib_rl::_db.edit, "fresh");
    EditEnd();
}

// TAB completes the word before the cursor from what the app offers.  A unique
// candidate lands with a blank after it.  Several candidates put in what they
// share, and a second TAB in a row lists them and leaves the line alone.
void atf_unit::unittest_lib_rl_EditComplete() {
    EditBegin("");
    int ctx = 0;
    lib_rl::h_compl_Set2(ctx, OfferRe);
    Type("r\t");
    vrfyeq_(lib_rl::_db.edit, "re");
    Type("\t");
    vrfyeq_(lib_rl::_db.edit, "re");
    Type("a\t");
    vrfyeq_(lib_rl::_db.edit, "read ");
    lib_rl::_db.h_compl = NULL;
    EditEnd();
}
