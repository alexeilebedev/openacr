// Copyright (C) 2024-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
