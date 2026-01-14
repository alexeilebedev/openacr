// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Exceptions: yes
// Source: cpp/atf_amc/sort.cpp
//

#include "include/atf_amc.h"
// for sort
#include <algorithm>

// -----------------------------------------------------------------------------

void atf_amc::amctest_AmcSort() {
    // setup suite
    atf_amc::FUnitSort frame;
    tary_Reserve(frame,100);
    frep_(i,100) {
        atf_amc::TypeA &val = tary_Alloc(frame);
        (void)val;
    }

    // insertion sort by value + sortedq test
    {
        // setup
        frep_(i,100) {
            tary_qFind(frame,i).typea = (i*73) % 100;
        }
        vrfy_(!tary_SortedQ(frame)); // check whether not sorted
        // exercise
        tary_InsertionSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(tary_qFind(frame,i).typea == i);
        }
        vrfy_(tary_SortedQ(frame));  // check whether sorted now
        // teardown
    }

    // insertion sort by pointer
    {
        // setup
        frep_(i,100) {
            tary_qFind(frame,i).typea = (i*73) % 100;
            c_ptrary_Insert(frame,tary_qFind(frame,i));
        }
        // exercise
        c_ptrary_InsertionSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(tary_qFind(frame,i).typea == (i*73) % 100);
            vrfy_(c_ptrary_Find(frame,i)->typea == i);
        }
        // teardown
        c_ptrary_RemoveAll(frame);
    }

    // heap sort by value
    {
        // setup
        frep_(i,100) {
            tary_qFind(frame,i).typea = (i*73) % 100;
        }
        // exercise
        tary_HeapSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(tary_qFind(frame,i).typea == i);
        }
        // teardown
    }

    // heap sort by pointer
    {
        // setup
        frep_(i,100) {
            tary_qFind(frame,i).typea = (i*73) % 100;
            c_ptrary_Insert(frame,tary_qFind(frame,i));
        }
        // exercise
        c_ptrary_HeapSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(tary_qFind(frame,i).typea == (i*73) % 100);
            vrfy_(c_ptrary_Find(frame,i)->typea == i);
        }
        // teardown
        c_ptrary_RemoveAll(frame);
    }

    // quick sort by value
    {
        // setup
        frep_(i,100) {
            tary_qFind(frame,i).typea = (i*73) % 100;
        }
        // exercise
        tary_QuickSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(tary_qFind(frame,i).typea == i);
        }
        // teardown
    }

    // quick sort by pointer
    {
        // setup
        frep_(i,100) {
            tary_qFind(frame,i).typea = (i*73) % 100;
            c_ptrary_Insert(frame,tary_qFind(frame,i));
        }
        // exercise
        c_ptrary_QuickSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(tary_qFind(frame,i).typea == (i*73) % 100);
            vrfy_(c_ptrary_Find(frame,i)->typea == i);
        }
        // teardown
        c_ptrary_RemoveAll(frame);
    }

    // quick sort for fixary
    {
        // setup
        frep_(i,100) {
            fixary_qFind(frame,i).typea = (i*73) % 100;
        }
        // exercise
        fixary_QuickSort(frame);
        // verify
        frep_(i,100) {
            vrfy_(fixary_qFind(frame,i).typea == i);
        }
        // teardown
    }

    // quick sort for varlen
    {
        // setup
        u32 length = sizeof(atf_amc::VarlenH)+100*sizeof(atf_amc::TypeH);
        atf_amc::VarlenH &varlenh=*(atf_amc::VarlenH*)malloc(length);
        varlenh.length=length;
        algo::aryptr<atf_amc::TypeH> ary = atf_amc::typeh_Getary(varlenh);
        frep_(i,100) {
            ary[i].typeh = (i*73) % 100;
        }
        // exercise
        atf_amc::typeh_QuickSort(varlenh);
        // verify
        frep_(i,100) {
            vrfy_(ary[i].typeh == i);
        }
        // teardown
        free(&varlenh);
    }
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_PerfSortString() {
    atf_amc::FPerfSortString frame;
    int N = 1000000;
    frep_(i,N) {
        atf_amc::Cstr &str = orig_Alloc(frame);
        frep_(j,60) str.val << "0123456789"[algo::i32_WeakRandom(10)];
    }
    frep_(i,2) {
        prlog("--------------------------------------------------------------------------------");
        {
            sorted1_RemoveAll(frame);
            sorted1_Addary(frame, orig_Getary(frame));
            u64 c = algo::get_cycles();
            std::sort(frame.sorted1_elems, frame.sorted1_elems + frame.sorted1_n);
            c = algo::get_cycles() - c;
            prlog("STL sort of cstrings: "<<c/N<<" cycles/elem");
        }
        {
            sorted1_RemoveAll(frame);
            sorted1_Addary(frame, orig_Getary(frame));
            u64 c = algo::get_cycles();
            // can AMC generate a quick sort for cstring??
            sorted1_QuickSort(frame);
            c = algo::get_cycles() - c;
            prlog("AMC QuickSort of cstrings: "<<c/N<<" cycles/elem");
        }
    }
}
