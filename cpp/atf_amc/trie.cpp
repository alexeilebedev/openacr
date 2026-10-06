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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/trie.cpp
//
// Tests of the Trie reftype, a tree of fixed nodes holding values keyed by a
// dense integer.  TrieU64 branches 256 ways, TrieNarrow 4 ways so that a
// handful of keys reaches every level, and TrieStr holds values with a
// destructor.  TrieBench measures the operations a stream's record index performs
// against the Tary it replaces.

#include "include/algo.h"
#include "include/atf_amc.h"

// Return KEY as the Trie key type.
static algo::SeqType MakeTriekey(u64 key) {
    return algo::SeqType(key);
}

// Allocate, find and re-allocate values at scattered keys: each key finds its
// own value, a slot never allocated finds nothing, and a second Alloc returns
// the value already there.
void atf_amc::amctest_TrieFind() {
    atf_amc::TrieU64 trie;
    u64 key[] = {0, 1, 255, 256, 1000, 70000, u64(1) << 40};
    frep_(i, 7) {
        u64 &val = trie_Alloc(trie, MakeTriekey(key[i]));
        vrfyeq_(val, u64(44));
        val = key[i] * 3;
    }
    vrfyeq_(trie_N(trie), 7);
    frep_(i, 7) {
        u64 *val = trie_Find(trie, MakeTriekey(key[i]));
        vrfy_(val && *val == key[i] * 3);
    }
    vrfy_(!trie_Find(trie, MakeTriekey(2)));
    vrfy_(!trie_Find(trie, MakeTriekey(257)));
    vrfy_(!trie_Find(trie, MakeTriekey(u64(1) << 39)));
    vrfy_(!trie_Find(trie, MakeTriekey(u64(1) << 50)));
    vrfyeq_(trie_Alloc(trie, MakeTriekey(1000)), u64(3000));
    vrfyeq_(trie_N(trie), 7);
    vrfy_(!trie_EmptyQ(trie));
}

// Grow a 4-way trie from one key to a key near the top of u64: the root rises a
// level at a time, and every value placed before survives each rise.
void atf_amc::amctest_TrieGrow() {
    atf_amc::TrieNarrow trie;
    frep_(i, 1000) {
        trie_Alloc(trie, MakeTriekey(i)) = i + 1;
    }
    u32 height = trie.trie_height;
    vrfyeq_(height, u32(4));
    trie_Alloc(trie, MakeTriekey(u64(1) << 62)) = 7;
    vrfyeq_(trie.trie_height, u32(31));
    frep_(i, 1000) {
        vrfyeq_(*trie_Find(trie, MakeTriekey(i)), u64(i + 1));
    }
    vrfyeq_(*trie_Find(trie, MakeTriekey(u64(1) << 62)), u64(7));
    vrfyeq_(trie_N(trie), 1001);
    trie_RemoveAll(trie);
    vrfyeq_(trie_N(trie), 0);
    vrfyeq_(trie.trie_nleaf, 0);
    vrfyeq_(trie.trie_nnode, 0);
    vrfy_(!trie_Find(trie, MakeTriekey(5)));
}

// Remove ranges from a filled trie: whole leaves inside the range are freed,
// the edge leaves keep what lies outside it, and a trie emptied by removal
// frees every node and drops to height 0.  Remove takes the largest key too.
void atf_amc::amctest_TrieRemoveRange() {
    atf_amc::TrieU64 trie;
    frep_(i, 10000) {
        trie_Alloc(trie, MakeTriekey(i)) = i;
    }
    vrfyeq_(trie.trie_nleaf, 40);
    trie_RemoveRange(trie, MakeTriekey(100), MakeTriekey(9000));
    vrfyeq_(trie_N(trie), 1100);
    vrfyeq_(trie.trie_nleaf, 6);
    vrfy_(trie_Find(trie, MakeTriekey(99)));
    vrfy_(!trie_Find(trie, MakeTriekey(100)));
    vrfy_(!trie_Find(trie, MakeTriekey(8999)));
    vrfy_(trie_Find(trie, MakeTriekey(9000)));
    trie_RemoveRange(trie, MakeTriekey(0), MakeTriekey(u64(1) << 40));
    vrfyeq_(trie_N(trie), 0);
    vrfyeq_(trie.trie_nleaf, 0);
    vrfyeq_(trie.trie_nnode, 0);
    vrfyeq_(trie.trie_height, u32(0));
    trie_Alloc(trie, MakeTriekey(5)) = 5;
    trie_Remove(trie, MakeTriekey(5));
    vrfy_(trie_EmptyQ(trie));
    vrfyeq_(trie.trie_nleaf, 0);

    // a narrow trie frees interior nodes on the way back up
    atf_amc::TrieNarrow narrow;
    frep_(i, 300) {
        trie_Alloc(narrow, MakeTriekey(i * 7)) = i;
    }
    trie_RemoveRange(narrow, MakeTriekey(0), MakeTriekey(1050));
    vrfyeq_(trie_N(narrow), 150);
    vrfy_(!trie_Find(narrow, MakeTriekey(1043)));
    vrfy_(trie_Find(narrow, MakeTriekey(1050)));
    trie_RemoveRange(narrow, MakeTriekey(1050), MakeTriekey(2100));
    vrfyeq_(trie_N(narrow), 0);
    vrfyeq_(narrow.trie_nnode, 0);
    vrfyeq_(narrow.trie_nleaf, 0);

    // the largest key, one past which wraps to 0, is removed like any other
    trie_Alloc(narrow, MakeTriekey(~u64(0))) = 1;
    trie_Alloc(narrow, MakeTriekey(3)) = 2;
    trie_Remove(narrow, MakeTriekey(~u64(0)));
    vrfy_(!trie_Find(narrow, MakeTriekey(~u64(0))));
    vrfyeq_(trie_N(narrow), 1);
    trie_Remove(narrow, MakeTriekey(3));
    vrfy_(trie_EmptyQ(narrow));
    vrfyeq_(narrow.trie_nnode, 0);
}

// Walk sparse keys with the cursor: every value is visited once, in ascending
// key order.  Each value is its own key, so the order can be read off them.
void atf_amc::amctest_TrieCurs() {
    atf_amc::TrieNarrow trie;
    frep_(i, 500) {
        u64 key = (u64(i) * 7919) % 100003;
        trie_Alloc(trie, MakeTriekey(key)) = key;
    }
    trie_Alloc(trie, MakeTriekey(u64(1) << 40)) = u64(1) << 40;
    i64 n = 0;
    u64 prev = 0;
    ind_beg(atf_amc::TrieNarrow_trie_curs, val, trie) {
        vrfy_(n == 0 || val > prev);
        prev = val;
        n++;
    }ind_end;
    vrfyeq_(n, trie_N(trie));
    vrfyeq_(prev, u64(1) << 40);
    atf_amc::TrieU64 empty;
    ind_beg(atf_amc::TrieU64_trie_curs, val, empty) {
        vrfy_(val == 0 && false);
    }ind_end;
}

// Fill a trie of strings long enough to live on the heap, and take them out by
// Remove, RemoveRange and RemoveAll.  Each value is destroyed exactly once,
// which memcheck confirms: a value left behind leaks, and one destroyed twice
// frees its buffer twice.
void atf_amc::amctest_TrieDtor() {
    atf_amc::TrieStr trie;
    frep_(i, 200) {
        trie_Alloc(trie, MakeTriekey(i)) << "a string longer than the inline buffer, key " << i;
    }
    trie_Remove(trie, MakeTriekey(3));
    trie_RemoveRange(trie, MakeTriekey(10), MakeTriekey(150));
    vrfyeq_(trie_N(trie), 59);
    vrfy_(ch_N(*trie_Find(trie, MakeTriekey(150))) > 0);
    trie_RemoveAll(trie);
    vrfyeq_(trie_N(trie), 0);
    frep_(i, 50) {
        trie_Alloc(trie, MakeTriekey(i)) << "left for Uninit " << i;
    }
}

// Compare a Trie with the Tary it replaces, on the operations a stream's record
// index performs, at 2M keys: the worst single append, a lookup a reader makes
// behind the tip, the trim of the oldest half, and a random seek.  The numbers
// are reported and not asserted, since a loaded machine moves them.
void atf_amc::amctest_TrieBench() {
    i64 nkey = i64(1) << 21;
    double ns = algo_lib::_db.clocks_to_ns;
    algo::U64Ary tary;
    u64 tary_worst = 0;
    frep_(i, nkey) {
        u64 c = algo::get_cycles();
        ary_Alloc(tary) = i;
        u64_UpdateMax(tary_worst, algo::get_cycles() - c);
    }
    atf_amc::TrieU64 trie;
    u64 trie_worst = 0;
    frep_(i, nkey) {
        u64 c = algo::get_cycles();
        trie_Alloc(trie, MakeTriekey(i)) = i;
        u64_UpdateMax(trie_worst, algo::get_cycles() - c);
    }
    // a reader 1000 keys behind the tip; the Tary answers with the bisection
    // its index used, the Trie with Find
    u64 sum = 0;
    u64 c = algo::get_cycles();
    frep_(i, nkey) {
        u64 key = u64(i);
        i64 lo = 0;
        i64 hi = ary_N(tary);
        while (lo < hi) {
            i64 mid = lo + (hi - lo) / 2;
            if (ary_qFind(tary, mid) <= key) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        sum += ary_qFind(tary, lo - 1);
    }
    u64 tary_lag = (algo::get_cycles() - c) / u64(nkey);
    c = algo::get_cycles();
    frep_(i, nkey) {
        sum += *trie_Find(trie, MakeTriekey(i));
    }
    u64 trie_lag = (algo::get_cycles() - c) / u64(nkey);
    c = algo::get_cycles();
    frep_(i, 1000000) {
        u64 key = (u64(i) * 1000003) % u64(nkey);
        sum += *trie_Find(trie, MakeTriekey(key));
    }
    u64 trie_seek = (algo::get_cycles() - c) / 1000000;
    c = algo::get_cycles();
    ary_RemRegion(tary, 0, nkey / 2);
    u64 tary_trim = algo::get_cycles() - c;
    c = algo::get_cycles();
    trie_RemoveRange(trie, MakeTriekey(0), MakeTriekey(u64(nkey / 2)));
    u64 trie_trim = algo::get_cycles() - c;
    vrfyeq_(i64(ary_N(tary)), trie_N(trie));
    vrfy_(sum > 0);
    prlog("atf_amc.TrieBench"
          <<Keyval("nkey",nkey)
          <<Keyval("tary_worst_append_us",u64(double(tary_worst) * ns / 1000))
          <<Keyval("trie_worst_append_us",u64(double(trie_worst) * ns / 1000))
          <<Keyval("tary_lag_lookup_ns",u64(double(tary_lag) * ns))
          <<Keyval("trie_lag_lookup_ns",u64(double(trie_lag) * ns))
          <<Keyval("trie_random_seek_ns",u64(double(trie_seek) * ns))
          <<Keyval("tary_trim_half_us",u64(double(tary_trim) * ns / 1000))
          <<Keyval("trie_trim_half_us",u64(double(trie_trim) * ns / 1000))
          <<Keyval("trie_nleaf",trie.trie_nleaf)
          <<Keyval("trie_nnode",trie.trie_nnode));
}

// Place keys with holes of every size, in a 4-way trie so the holes span leaves
// and levels, and check that NextKey and PrevKey land on the nearest held key
// from every starting point, and report none past either end.
void atf_amc::amctest_TrieNextPrev() {
    atf_amc::TrieNarrow trie;
    u64 key[] = {3, 4, 17, 64, 65, 1000, 1001, 70000, u64(1) << 33};
    frep_(i, 9) {
        trie_Alloc(trie, MakeTriekey(key[i])) = key[i];
    }
    algo::SeqType out;
    vrfy_(trie_NextKey(trie, MakeTriekey(0), out) && out.value == 3);
    vrfy_(!trie_PrevKey(trie, MakeTriekey(2), out));
    vrfy_(!trie_NextKey(trie, MakeTriekey((u64(1) << 33) + 1), out));
    vrfy_(trie_PrevKey(trie, MakeTriekey(~u64(0) >> 1), out) && out.value == u64(1) << 33);
    frep_(i, 9) {
        vrfy_(trie_NextKey(trie, MakeTriekey(key[i]), out) && out.value == key[i]);
        vrfy_(trie_PrevKey(trie, MakeTriekey(key[i]), out) && out.value == key[i]);
        if (i + 1 < 9) {
            vrfy_(trie_NextKey(trie, MakeTriekey(key[i] + 1), out) && out.value == key[i + 1]);
        }
        if (i > 0) {
            vrfy_(trie_PrevKey(trie, MakeTriekey(key[i] - 1), out) && out.value == key[i - 1]);
        }
    }
    // a walk by NextKey visits every key once
    i64 n = 0;
    algo::SeqType pos(0);
    while (trie_NextKey(trie, pos, out)) {
        vrfyeq_(out.value, key[n]);
        n++;
        pos = algo::SeqType(out.value + 1);
    }
    vrfyeq_(n, 9);
    atf_amc::TrieU64 empty;
    vrfy_(!trie_NextKey(empty, MakeTriekey(0), out));
    vrfy_(!trie_PrevKey(empty, MakeTriekey(100), out));
}
