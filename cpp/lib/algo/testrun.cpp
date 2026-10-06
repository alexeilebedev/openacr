// Copyright (C) 2026 AlgoRND
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/testrun.cpp -- Progress and separator lines a test runner prints
//

#include "include/algo.h"


// Log the line a test runner prints before running test INDEX of N, where INDEX
// counts from 1, with the time passed since the run began at START.  atf_x2 and
// atf_comp both print it, so a reader sees how far either run has come.
void algo::PrlogTestProgress(i32 index, i32 n, algo::UnTime start) {
    tempstr elapsed;
    algo::UnDiff_PrintSpec(algo::CurrUnTime() - start, elapsed, "%H:%M:%S");
    prlog("test " << index << "/" << n << " (" << (n > 0 ? index * 100 / n : 0) << "%)"
          << ", elapsed:" << elapsed);
}

// Log the separator a test runner prints after each test: a rule, then a blank
// line.
void algo::PrlogTestSeparator() {
    tempstr rule;
    algo::char_PrintNTimes('-', rule, 80);
    prlog(rule);
    prlog("");
}
