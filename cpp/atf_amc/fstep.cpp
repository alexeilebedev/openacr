// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023 AlgoRND
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

static int _num;
static void IncrNum() {
    _num++;
}

void atf_amc::amctest_fstep_TimeHookRecur() {
    bool success=false;
    for (int retry=0; retry<60 && !success; retry++) {
        if (retry>0) {
            _num=0;
            sleep(1);
        }
        algo_lib::FTimehook th;
        ThInitRecur(th,algo::ToSchedTime(0.01));
        hook_Set0(th, IncrNum);
        bh_timehook_Insert(th);
        algo_lib::_db.limit = algo::CurrSchedTime() + algo::ToSchedTime(0.1);
        MainLoop();
        prlog(_num<<" <=> "<<7);
        success = _num>=7 && _num<=10;
    }
    vrfy_(success);
}
