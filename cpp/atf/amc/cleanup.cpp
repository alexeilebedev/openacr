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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/cleanup.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

// --------------------------------------------------------------------------------

void atf_amc::field1_Cleanup(atf_amc::AmcCleanup2 &cleanup2) {// gets called second
    vrfyeq_(cleanup2.field1, 100);
    vrfyeq_(cleanup2.field2, 0);
    cleanup2.field1 = 0;
}

void atf_amc::field2_Cleanup(atf_amc::AmcCleanup2 &cleanup2) {// gets called first
    vrfyeq_(cleanup2.field1, 100);
    vrfyeq_(cleanup2.field2, 100);
    cleanup2.field2 = 0;
}

void atf_amc::amctest_CleanupOrder() {
    atf_amc::AmcCleanup2 cleanup2;
    vrfyeq_(cleanup2.field1, 0);
    vrfyeq_(cleanup2.field2, 0);
    cleanup2.field1 = 100;
    cleanup2.field2 = 100;
    vrfyeq_(cleanup2.field1, 100);
    vrfyeq_(cleanup2.field2, 100);
    AmcCleanup2_Uninit(cleanup2);

    // check that fields got cleaned up
    vrfyeq_(cleanup2.field1, 0);
    vrfyeq_(cleanup2.field2, 0);

    // initialize again because Uninit is getting called a second time!
    cleanup2.field1 = 100;
    cleanup2.field2 = 100;
}
