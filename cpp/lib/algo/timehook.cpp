// Copyright (C) 2023-2026 AlgoRND
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/timehook.cpp
//

#include "include/algo.h"

// Initialize a recurrent time hook TH to execute on the next scheduling cycle,
// and after that, every DELAY clocks
// NOTE: 'delay' field of a recurrent timehook is used when automatically rescheduling it.
void algo_lib::ThInitRecur(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW {
    th.recurrent = true;
    th.delay     = delay;
    th.time      = algo_lib::_db.clock;
}

// Schedule a time hook TH to execute on the next scheduling cycle,
// and after that, every DELAY clocks
void algo_lib::ThScheduleRecur(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW {
    th.recurrent = true;
    th.delay     = delay;
    th.time      = algo_lib::_db.clock;
    bh_timehook_Reheap(th);
}

// Initialize a non-recurrent time hook TH to execute after DELAY clock cycles with
// respect to current time
// NOTE: 'delay' field of non-recurrent timehook is ignored
// NOTE: this function updates scheduling clock to the most current value
void algo_lib::ThScheduleIn(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW {
    algo_lib::_db.clock = algo::CurrSchedTime();
    th.recurrent = false;
    th.time = algo_lib::_db.clock + delay;
    bh_timehook_Reheap(th);
}
