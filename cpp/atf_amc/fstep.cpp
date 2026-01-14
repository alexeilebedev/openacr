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
// Source: cpp/atf_amc/fstep.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

void atf_amc::amctest_fstep_Inline() {
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_fstep_InlineOnce() {
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_fstep_InlineRecur() {
}

// -----------------------------------------------------------------------------

static void IncrNum() {
    atf_amc::_db.timehook_recur_counter++;
}

void atf_amc::amctest_fstep_TimeHookRecur() {
    // check the TimeHookRecur is scheduling things repeatedly
    bool success=false;
    for (int retry=0; retry<60 && !success; retry++) {
        if (retry>0) {
            atf_amc::_db.timehook_recur_counter=0;
            sleep(1);
        }
        algo_lib::FTimehook th;
        hook_Set0(th, IncrNum);
        ThScheduleRecur(th,algo::ToSchedTime(0.01));
        algo_lib::_db.limit = algo::CurrSchedTime() + algo::ToSchedTime(0.1);
        MainLoop();
        prlog(atf_amc::_db.timehook_recur_counter<<" <=> "<<7);
        success = atf_amc::_db.timehook_recur_counter>=7 && atf_amc::_db.timehook_recur_counter<=10;
    }
    vrfy_(success);
}

// -----------------------------------------------------------------------------

void atf_amc::bh_time_entry_Step() {
    time_entry_Delete(*bh_time_entry_RemoveFirst());
    atf_amc::_db.timehook_once_counter++;
}

void atf_amc::amctest_fstep_TimeHookOnce() {
    atf_amc::FTimeEntry &a=time_entry_Alloc();
    time_entry_XrefMaybe(a);
    // initially it's not in the heap
    vrfy_(!bh_time_entry_InBheapQ(a));
    vrfy_(!bh_timehook_InBheapQ(_db.th_bh_time_entry));
    // now schedule it
    a.time = algo::SchedTime(3);
    bh_time_entry_Reheap(a);
    // now the time hook has been scheduled
    vrfy_(bh_timehook_InBheapQ(_db.th_bh_time_entry));

    // add an entry that preempts it
    atf_amc::FTimeEntry &b=time_entry_Alloc();
    time_entry_XrefMaybe(b);
    b.time = algo::SchedTime(2);
    bh_time_entry_Reheap(b);
    vrfy_(bh_timehook_InBheapQ(_db.th_bh_time_entry));
    // b must be scheduled
    vrfy_(_db.th_bh_time_entry.time == b.time);

    // add an entry that doesn't preempts anyone
    atf_amc::FTimeEntry &c=time_entry_Alloc();
    time_entry_XrefMaybe(c);
    c.time = algo::SchedTime(10);
    bh_time_entry_Reheap(c);
    vrfy_(bh_timehook_InBheapQ(_db.th_bh_time_entry));
    // b is still scheduled
    vrfy_(_db.th_bh_time_entry.time == b.time);

    // delete b
    time_entry_Delete(b);
    // a becomes first
    vrfy_(_db.th_bh_time_entry.time == a.time);
    time_entry_Delete(a);
    // c becomes first
    vrfy_(_db.th_bh_time_entry.time == c.time);
    time_entry_Delete(c);
    // timehook heap must be empty now
    vrfy_(algo_lib::bh_timehook_N()==0);

    for (int i=0; i<10; i++) {
        atf_amc::FTimeEntry &e =time_entry_Alloc();
        e.time = algo_lib::_db.clock + algo::ToSchedTime(i*0.01);
        time_entry_XrefMaybe(e);
        bh_time_entry_Reheap(e);
    }
    atf_amc::MainLoop();
    vrfy_(atf_amc::_db.timehook_once_counter == 10);
}
