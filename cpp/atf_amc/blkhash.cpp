// Copyright (C) 2026 AlgoRND
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/blkhash.cpp
//
// The element pool blkhash_elem carries two indexes on the same key:
// ind_blkhash_elem (Blkhash, linbits:12) and ind_blkhash_elem_thash (a Thash
// twin, inscond:false).  Functional tests exercise the Blkhash index through
// both the manual InsertMaybe/Remove calls and the XrefMaybe path; the perf
// test drives each index in turn through the same rolling-window scenario.

#include "include/atf_amc.h"

// Build a Blkhash test key from a stream-like id and a dense seqno
static atf_amc::BlkKey Key(u32 id, u64 seq) {
    atf_amc::BlkKey ret;
    ret.id = id;
    ret.seq.value = seq;
    return ret;
}

// -----------------------------------------------------------------------------

// Insert/Find semantics: exact lookup, idempotent re-insert, duplicate-key
// rejection, block accounting (one block per (id, 4096-aligned seq region))
void atf_amc::amctest_BlkhashInsertMaybe() {
    i32 n0 = ind_blkhash_elem_N();
    i32 nblk0 = _db.ind_blkhash_elem_nblk;
    atf_amc::FBlkhashElem &a = blkhash_elem_Alloc();
    a.key = Key(1,100);
    a.val = 100;
    vrfy_(ind_blkhash_elem_InsertMaybe(a));
    atf_amc::FBlkhashElem &b = blkhash_elem_Alloc();
    b.key = Key(1,101);
    b.val = 101;
    vrfy_(ind_blkhash_elem_InsertMaybe(b));
    atf_amc::FBlkhashElem &c = blkhash_elem_Alloc();
    c.key = Key(2,100);
    c.val = 200;
    vrfy_(ind_blkhash_elem_InsertMaybe(c));
    vrfyeq_(ind_blkhash_elem_N(), n0+3);
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0+2); // (1,0..4095) and (2,0..4095)
    // exact lookup
    vrfyeq_(ind_blkhash_elem_Find(Key(1,100)), &a);
    vrfyeq_(ind_blkhash_elem_Find(Key(1,101)), &b);
    vrfyeq_(ind_blkhash_elem_Find(Key(2,100)), &c);
    vrfyeq_(ind_blkhash_elem_Find(Key(1,102)), NULL); // empty slot in a resident block
    vrfyeq_(ind_blkhash_elem_Find(Key(3,100)), NULL); // no such block
    // re-insert of a member row: true, no change
    vrfy_(ind_blkhash_elem_InsertMaybe(a));
    vrfyeq_(ind_blkhash_elem_N(), n0+3);
    // different row with an occupied key: rejected
    atf_amc::FBlkhashElem &dup = blkhash_elem_Alloc();
    dup.key = Key(1,100);
    vrfyeq_(ind_blkhash_elem_InsertMaybe(dup), false);
    blkhash_elem_Delete(dup);
    vrfyeq_(ind_blkhash_elem_N(), n0+3);
    vrfyeq_(ind_blkhash_elem_Find(Key(1,100)), &a); // original row undisturbed
    // FindX: reference on hit, throw on miss
    vrfyeq_(&ind_blkhash_elem_FindX(Key(1,101)), &b);
    bool thrown = false;
    try {
        ind_blkhash_elem_FindX(Key(3,100));
    } catch (algo_lib::ErrorX&) {
        thrown = true;
    }
    vrfy_(thrown);
    blkhash_elem_Delete(a);
    blkhash_elem_Delete(b);
    blkhash_elem_Delete(c);
    vrfyeq_(ind_blkhash_elem_N(), n0);
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0);
}

// -----------------------------------------------------------------------------

// Remove semantics: no-op on non-member, block freed when its last slot
// clears, freed region usable again, bucket array growth past 4 blocks
void atf_amc::amctest_BlkhashRemove() {
    i32 n0 = ind_blkhash_elem_N();
    i32 nblk0 = _db.ind_blkhash_elem_nblk;
    // seqs span three 4096-slot blocks of one stream
    u64 seq[5] = {0, 1, 4095, 4096, 8192};
    atf_amc::FBlkhashElem *elem[5];
    frep_(i,5) {
        elem[i] = &blkhash_elem_Alloc();
        elem[i]->key = Key(1,seq[i]);
        elem[i]->val = seq[i];
        vrfy_(ind_blkhash_elem_InsertMaybe(*elem[i]));
    }
    vrfyeq_(ind_blkhash_elem_N(), n0+5);
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0+3);
    // remove of a non-member row: no-op
    atf_amc::FBlkhashElem &loner = blkhash_elem_Alloc();
    loner.key = Key(1,4097); // resident block, empty slot
    ind_blkhash_elem_Remove(loner);
    vrfyeq_(ind_blkhash_elem_N(), n0+5);
    loner.key = Key(9,0); // no such block
    ind_blkhash_elem_Remove(loner);
    vrfyeq_(ind_blkhash_elem_N(), n0+5);
    blkhash_elem_Delete(loner);
    // removing the sole member of the middle block frees the block
    ind_blkhash_elem_Remove(*elem[3]); // seq 4096
    vrfyeq_(ind_blkhash_elem_N(), n0+4);
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0+2);
    vrfyeq_(ind_blkhash_elem_Find(Key(1,4096)), NULL);
    // the freed region works again
    elem[3]->key = Key(1,4100);
    vrfy_(ind_blkhash_elem_InsertMaybe(*elem[3]));
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0+3);
    vrfyeq_(ind_blkhash_elem_Find(Key(1,4100)), elem[3]);
    // growth: 8 more streams, one row each, pushes block count past 4 buckets
    atf_amc::FBlkhashElem *grow[8];
    frep_(i,8) {
        grow[i] = &blkhash_elem_Alloc();
        grow[i]->key = Key(100+i, 7);
        grow[i]->val = i;
        vrfy_(ind_blkhash_elem_InsertMaybe(*grow[i]));
    }
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0+11);
    frep_(i,5) {
        vrfyeq_(ind_blkhash_elem_Find(elem[i]->key), elem[i]); // reachable after re-chain
    }
    frep_(i,8) {
        vrfyeq_(ind_blkhash_elem_Find(Key(100+i, 7)), grow[i]);
        blkhash_elem_Delete(*grow[i]);
    }
    frep_(i,5) {
        blkhash_elem_Delete(*elem[i]);
    }
    vrfyeq_(ind_blkhash_elem_N(), n0);
    vrfyeq_(_db.ind_blkhash_elem_nblk, nblk0);
}

// -----------------------------------------------------------------------------

// Cursor visits every member exactly once
void atf_amc::amctest_BlkhashCurs() {
    u64 want_sum = 0;
    i32 want_n = 0;
    frep_(id,3) {
        frep_(k,5) {
            atf_amc::FBlkhashElem &elem = blkhash_elem_Alloc();
            u64 seq = k*3000; // spread across blocks
            elem.key = Key(20+id, seq);
            elem.val = (20+id)*100000 + seq;
            vrfy_(ind_blkhash_elem_InsertMaybe(elem));
            want_sum += elem.val;
            want_n++;
        }
    }
    u64 sum = 0;
    i32 count = 0;
    ind_beg(atf_amc::_db_ind_blkhash_elem_curs, elem, atf_amc::_db) {
        sum += elem.val;
        count++;
    }ind_end;
    vrfyeq_(count, want_n);
    vrfyeq_(sum, want_sum);
    frep_(id,3) {
        frep_(k,5) {
            blkhash_elem_Delete(*ind_blkhash_elem_Find(Key(20+id, k*3000)));
        }
    }
    vrfyeq_(ind_blkhash_elem_N(), 0);
}

// -----------------------------------------------------------------------------

// XrefMaybe inserts into the Blkhash index and reports duplicate keys
void atf_amc::amctest_BlkhashXref() {
    atf_amc::FBlkhashElem &a = blkhash_elem_Alloc();
    a.key = Key(5,77);
    a.val = 77;
    vrfy_(blkhash_elem_XrefMaybe(a));
    vrfyeq_(ind_blkhash_elem_Find(Key(5,77)), &a);
    // second row with the same key: xref fails, errtext names the index
    atf_amc::FBlkhashElem &dup = blkhash_elem_Alloc();
    dup.key = Key(5,77);
    vrfyeq_(blkhash_elem_XrefMaybe(dup), false);
    blkhash_elem_Delete(dup);
    vrfyeq_(ind_blkhash_elem_Find(Key(5,77)), &a);
    blkhash_elem_Delete(a);
    vrfyeq_(ind_blkhash_elem_Find(Key(5,77)), NULL);
}

// -----------------------------------------------------------------------------

// One rolling-window pass driven against the Blkhash index: TOTAL appends of
// a WINDOW-sized set (insert at tail, remove at head) plus LOOKUP random
// finds near the tail per append.  Returns elapsed cycles for the timed loop.
static u64 BlkhashPass(u32 id, i32 total, i32 window, u64 &checksum) {
    u64 rnd = 12345;
    u64 t0 = algo::get_cycles();
    frep_(i, total) {
        atf_amc::FBlkhashElem *row;
        if (i < window) {
            row = &atf_amc::blkhash_elem_Alloc();
        } else {
            row = atf_amc::ind_blkhash_elem_Find(Key(id, i-window)); // retire window head
            atf_amc::ind_blkhash_elem_Remove(*row);
        }
        row->key = Key(id, i);
        row->val = i;
        atf_amc::ind_blkhash_elem_InsertMaybe(*row);
        frep_(k, 3) {
            rnd = rnd*6364136223846793005ULL + 1442695040888963407ULL;
            u64 back = (rnd>>33) & 65535;
            u64 seq = u64(i) - u64_Min(back, u64(i));
            atf_amc::FBlkhashElem *hit = atf_amc::ind_blkhash_elem_Find(Key(id, seq));
            checksum += hit->val;
        }
    }
    u64 t1 = algo::get_cycles();
    frep_(i, window) {
        atf_amc::blkhash_elem_Delete(*atf_amc::ind_blkhash_elem_Find(Key(id, total-window+i)));
    }
    return t1-t0;
}

// -----------------------------------------------------------------------------

// The same rolling-window pass driven against the Thash twin index
static u64 ThashPass(u32 id, i32 total, i32 window, u64 &checksum) {
    u64 rnd = 12345;
    u64 t0 = algo::get_cycles();
    frep_(i, total) {
        atf_amc::FBlkhashElem *row;
        if (i < window) {
            row = &atf_amc::blkhash_elem_Alloc();
        } else {
            row = atf_amc::ind_blkhash_elem_thash_Find(Key(id, i-window)); // retire window head
            atf_amc::ind_blkhash_elem_thash_Remove(*row);
        }
        row->key = Key(id, i);
        row->val = i;
        atf_amc::ind_blkhash_elem_thash_InsertMaybe(*row);
        frep_(k, 3) {
            rnd = rnd*6364136223846793005ULL + 1442695040888963407ULL;
            u64 back = (rnd>>33) & 65535;
            u64 seq = u64(i) - u64_Min(back, u64(i));
            atf_amc::FBlkhashElem *hit = atf_amc::ind_blkhash_elem_thash_Find(Key(id, seq));
            checksum += hit->val;
        }
    }
    u64 t1 = algo::get_cycles();
    frep_(i, window) {
        atf_amc::blkhash_elem_Delete(*atf_amc::ind_blkhash_elem_thash_Find(Key(id, total-window+i)));
    }
    return t1-t0;
}

// -----------------------------------------------------------------------------

// Rolling-window benchmark: 10M appends over a 1M-element window with 3
// near-tail lookups per append, run once against the Thash twin and once
// against the Blkhash index.  Per-append cost includes one insert, one
// head lookup + removal (once the window is full), and the 3 lookups.
void atf_amc::amctest_PerfBlkhashRolling() {
    const i32 total  = 10000000;
    const i32 window = 1000000;
    u64 checksum_thash = 0;
    u64 checksum_blkhash = 0;
    u64 cycles_thash = ThashPass(7, total, window, checksum_thash);
    u64 cycles_blkhash = BlkhashPass(7, total, window, checksum_blkhash);
    vrfyeq_(checksum_blkhash, checksum_thash); // identical scenario, identical hits
    vrfyeq_(ind_blkhash_elem_N(), 0);
    vrfyeq_(ind_blkhash_elem_thash_N(), 0);
    double hz = algo::get_cpu_hz_int();
    prlog("atf_amc.PerfBlkhashRolling"
          << Keyval("total", total)
          << Keyval("window", window)
          << Keyval("thash_cycles_per_append", double(cycles_thash)/total)
          << Keyval("blkhash_cycles_per_append", double(cycles_blkhash)/total)
          << Keyval("thash_sec", double(cycles_thash)/hz)
          << Keyval("blkhash_sec", double(cycles_blkhash)/hz)
          << Keyval("speedup", double(cycles_thash)/double(cycles_blkhash)));
}
