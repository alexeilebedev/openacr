// Copyright (C) 2023-2024,2026 AlgoRND
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

static u32 Lcg100Next(int prev) {
    return (prev*21+1)%100;
}

void atf_amc::amctest_ThashLinear() {
    // fill hash
    vrfyeq_(ind_linear_EmptyQ(),true);
    vrfyeq_(ind_linear_N(),0);
    int x(0);
    frep_(i,100) {
        x = Lcg100Next(x);
        vrfy_(ind_linear_GetOrCreate(algo::U32LinearKey(x)).ind_linear_next!=(FHashableLinear*)-1);
        vrfyeq_(ind_linear_EmptyQ(),false);
        vrfyeq_(ind_linear_N(),i+1);
    }
    // cursor
    int count[100];
    memset(count,0,sizeof(count));
    ind_beg(_db_ind_linear_curs,item,_db) {
        ++count[item.key.value];
    } ind_end;
    frep_(i,100) {
        vrfyeq_(count[i],1);
    }
    // find
    x=0;
    frep_(i,100) {
        x = Lcg100Next(x);
        vrfyeq_(ind_linear_FindX(algo::U32LinearKey(x)).key,u32(x));
    }
    vrfyeq_(ind_linear_Find(algo::U32LinearKey(100)),NULL);
    vrfyeq_(ind_linear_Find(algo::U32LinearKey(UINT_MAX)),NULL);
    // remove by key
    x=0;
    frep_(i,100) {
        x = Lcg100Next(x);
        // leaks but we do not care
        FHashableLinear *elem = ind_linear_FindRemove(algo::U32LinearKey(x));
        vrfyeq_(elem->key,u32(x));
        vrfyeq_(elem->ind_linear_next,(FHashableLinear *)-1);
    }
    vrfyeq_(ind_linear_FindRemove(algo::U32LinearKey(100)),NULL);
    vrfyeq_(ind_linear_FindRemove(algo::U32LinearKey(UINT_MAX)),NULL);
    vrfyeq_(ind_linear_EmptyQ(),true);
    vrfyeq_(ind_linear_N(),0);
    // remove
    x=0;
    frep_(i,100) {
        x = Lcg100Next(x);
        ind_linear_GetOrCreate(algo::U32LinearKey(x));
    }
    vrfyeq_(ind_linear_N(),100);
    frep_(i,100) {
        x = Lcg100Next(x);
        hashable_linear_Delete(ind_linear_FindX(algo::U32LinearKey(x)));
    }
    vrfyeq_(ind_linear_EmptyQ(),true);
    vrfyeq_(ind_linear_N(),0);
}

// Test hash with string keys containing binary chars
void atf_amc::amctest_ThashStrkey() {

    for (int pass=0; pass<2; pass++) {
        for (int i=0; i<1000; i++) {
            char key[6];
            key[0]='\000';
            (i32&)key[1]=i;
            key[4]='z';
            key[5]=i%256;
            if (pass==0) {
                Strkey &elem = strkey_Alloc();
                elem.strkey = algo::strptr(key,6);
                strkey_XrefMaybe(elem);
            } else if (pass==1) {
                Strkey *found=ind_strkey_Find(algo::strptr(key,6));
                vrfy_(found);
            }
        }
    }
    {
        Strkey &elem1 = strkey_Alloc();
        const char key1[]="\000\001\000\017sub-000-zztt01g\000\032test-topic-0000000-7TLEyoo\000\000\000\001";
        algo::strptr strkey1(key1,sizeof(key1)-1);
        prlog(Keyval("key1",strkey1));
        elem1.strkey = strkey1;
        strkey_XrefMaybe(elem1);
        vrfy_(ind_strkey_Find(strkey1)==&elem1);

        Strkey &elem2 = strkey_Alloc();
        const char key2[]="\000\001\000\017sub-000-zztt01g\000\032test-topic-0000000-7TLEyoo\000\000\000\004";
        algo::strptr strkey2(key2,sizeof(key2)-1);
        prlog(Keyval("key2",strkey2));
        elem2.strkey = strkey2;
        strkey_XrefMaybe(elem2);
        vrfy_(ind_strkey_Find(strkey1)==&elem1);
        vrfy_(ind_strkey_Find(strkey2)==&elem2);
    }
}
