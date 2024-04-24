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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf_amc/thash.cpp
//

#include "include/atf_amc.h"

// THASH SLL

void atf_amc::amctest_ThashEmpty() {
    // predicates
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_Find(0),NULL);
    vrfyeq_(ind_thash_elem_N(),0);
    // internals
    vrfyeq_(_db.ind_thash_elem_buckets_n,4);
    vrfy_(_db.ind_thash_elem_buckets_elems);
    frep_(i,_db.ind_thash_elem_buckets_n) {
        vrfyeq_(_db.ind_thash_elem_buckets_elems[i],NULL);
    }
    // elem no-ops
    FThashElem &elem = thash_elem_Alloc();
    vrfyeq_(elem.ind_thash_elem_next,(FThashElem*)-1);
    // remove
    ind_thash_elem_Remove(elem);
    // predicates
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_Find(0),NULL);
    vrfyeq_(ind_thash_elem_N(),0);
    // cleanup
    thash_elem_RemoveAll();
}

void atf_amc::amctest_ThashInsertMaybe() {
    constexpr int N=100;
    vrfyeq_(thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_N(),0);
    frep_(i,N) {
        thash_elem_Alloc().key = i;
    }
    frep_(i,N) {
        vrfy_(ind_thash_elem_InsertMaybe(thash_elem_qFind(i)));
        vrfyeq_(ind_thash_elem_EmptyQ(),false);
        vrfyeq_(ind_thash_elem_N(),i+1);
        frep_(j,N) {
            vrfyeq_(ind_thash_elem_Find(j),j<=i?&thash_elem_qFind(j):NULL);
            vrfyeq_(thash_elem_qFind(j).key,u64(j));
            vrfyeq_(thash_elem_qFind(j).ind_thash_elem_next==(FThashElem*)-1,j>i);
        }
        // idempotent
        vrfy_(ind_thash_elem_InsertMaybe(thash_elem_qFind(i)));
        vrfyeq_(ind_thash_elem_EmptyQ(),false);
        vrfyeq_(ind_thash_elem_N(),i+1);
    }
    // cleanup
    thash_elem_RemoveAll();
    // predicates
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_Find(0),NULL);
    vrfyeq_(ind_thash_elem_N(),0);
}

void atf_amc::amctest_ThashRemove() {
    constexpr int N=100;
    vrfyeq_(thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_N(),0);
    frep_(i,N) {
        thash_elem_Alloc().key = i;
        vrfy_(ind_thash_elem_InsertMaybe(*thash_elem_Last()));
    }
    frep_(i,N) {
        ind_thash_elem_Remove(thash_elem_qFind(i));
        vrfyeq_(ind_thash_elem_EmptyQ(),i==N-1);
        vrfyeq_(ind_thash_elem_N(),N-1-i);
        frep_(j,N) {
            vrfyeq_(ind_thash_elem_Find(j),j>i?&thash_elem_qFind(j):NULL);
            vrfyeq_(thash_elem_qFind(j).key,u64(j));
            vrfyeq_(thash_elem_qFind(j).ind_thash_elem_next==(FThashElem*)-1,j<=i);
        }
        // idempotent
        ind_thash_elem_Remove(thash_elem_qFind(i));
        vrfyeq_(ind_thash_elem_EmptyQ(),i==N-1);
        vrfyeq_(ind_thash_elem_N(),N-1-i);
    }
}

void atf_amc::amctest_ThashFindRemove() {
    constexpr int N=100;
    vrfyeq_(thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_N(),0);
    frep_(i,N) {
        thash_elem_Alloc().key = i;
        vrfy_(ind_thash_elem_InsertMaybe(*thash_elem_Last()));
    }
    frep_(i,N) {
        vrfyeq_(ind_thash_elem_FindRemove(i),&thash_elem_qFind(i));
        vrfyeq_(ind_thash_elem_EmptyQ(),i==N-1);
        vrfyeq_(ind_thash_elem_N(),N-1-i);
        frep_(j,N) {
            vrfyeq_(ind_thash_elem_Find(j),j>i?&thash_elem_qFind(j):NULL);
            vrfyeq_(thash_elem_qFind(j).key,u64(j));
            vrfyeq_(thash_elem_qFind(j).ind_thash_elem_next==(FThashElem*)-1,j<=i);
        }
        // idempotent
        vrfyeq_(ind_thash_elem_FindRemove(i),NULL);
        vrfyeq_(ind_thash_elem_EmptyQ(),i==N-1);
        vrfyeq_(ind_thash_elem_N(),N-1-i);
    }
}

void atf_amc::amctest_ThashGetOrCreate() {
    constexpr int N=100;
    vrfyeq_(thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_N(),0);
    frep_(i,N) {
        auto &elem = ind_thash_elem_GetOrCreate(i);
        vrfyeq_(elem.key,u64(i));
        vrfyeq_(&elem,thash_elem_Last());
    }
    vrfyeq_(ind_thash_elem_EmptyQ(),false);
    vrfyeq_(ind_thash_elem_N(),100);
    vrfyeq_(thash_elem_N(),100);
    frep_(i,N) {
        auto &elem = ind_thash_elem_GetOrCreate(i);
        vrfyeq_(elem.key,u64(i));
        vrfyeq_(&elem,&thash_elem_qFind(i));
    }
    vrfyeq_(ind_thash_elem_EmptyQ(),false);
    vrfyeq_(ind_thash_elem_N(),100);
    vrfyeq_(thash_elem_N(),100);
    thash_elem_RemoveAll();
}

void atf_amc::amctest_ThashXref() {
    constexpr int N=100;
    vrfyeq_(thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_EmptyQ(),true);
    vrfyeq_(ind_thash_elem_N(),0);
    frep_(i,N) {
        auto &elem = thash_elem_Alloc();
        elem.key = i;
        vrfy_(thash_elem_XrefMaybe(elem));
    }
    vrfyeq_(ind_thash_elem_EmptyQ(),false);
    vrfyeq_(ind_thash_elem_N(),100);
    vrfyeq_(thash_elem_N(),100);
    frep_(i,N) {
        auto *elem = ind_thash_elem_Find(i);
        vrfy_(elem);
        vrfyeq_(elem->key,u64(i));
        vrfyeq_(elem,&thash_elem_qFind(i));
    }
    vrfyeq_(ind_thash_elem_EmptyQ(),false);
    vrfyeq_(ind_thash_elem_N(),100);
    vrfyeq_(thash_elem_N(),100);
    thash_elem_RemoveAll();
}

// THASH DLL

void atf_amc::amctest_PerfThashRemove() {
    // used for generation Modulo-length
    // pseudo-random sequence
    constexpr u64 Modulo = 1000000;
    constexpr u64 Multiplier = 50001;
    constexpr int N = Modulo;

    // benchmark random deletion - singly linked
    frep_(i,N) {
        ind_thash_elem_GetOrCreate(i);
    }
    u64 seq(0);
    u64 s_begin = algo::get_cycles();
    frep_(i,N) {
        ind_thash_elem_Remove(thash_elem_qFind(seq));
        seq = (seq*Multiplier+1)%Modulo;
    }
    vrfyeq_(ind_thash_elem_N(),0);
    u64 s_end = algo::get_cycles();
    double sllist_cycles_per_elem = double(s_end-s_begin)/N;

    prlog("atf_amc.PerfThashRemove"
          << Keyval("sllist_cycles_per_elem",sllist_cycles_per_elem));
}
