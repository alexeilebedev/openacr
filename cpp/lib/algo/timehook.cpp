// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/timehook.cpp
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/algo.h"

// Initialize time hook TH as non-recurrent, with delay DELAY.
// Usage:
// ThInit(th, SchedTime());     // schedule at current time
// hook_Set0(th, myfunction);   // set callback
// bh_timehook_Reheap(th);      // insert into timehook heap
// ... eventually algo_lib::Step() will call the hook
void algo_lib::ThInit(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW {
    th.recurrent = false;
    th.delay     = delay;
    th.time      = algo_lib::_db.clock;
}

// Similar to the above, but recurrent.
void algo_lib::ThInitRecur(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW {
    th.recurrent = true;
    th.delay     = delay;
    th.time      = algo_lib::_db.clock;
}
