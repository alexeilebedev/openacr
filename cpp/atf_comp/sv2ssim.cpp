// Copyright (C) 2026 AlgoRND
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
// Target: atf_comp (exe) -- Algo Test Framework - Component test execution
// Exceptions: yes
// Source: cpp/atf_comp/sv2ssim.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_sv2ssim_Convert1() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/2.csv -ctype a.B -schema -data");
}

void atf_comp::comptest_sv2ssim_Convert1Signed() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/2.csv -ctype a.B -schema -data -prefer_signed");
}

void atf_comp::comptest_sv2ssim_Convert2() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/1.csv -ctype a.B -schema -data -outseparator:, -header:N");
}

void atf_comp::comptest_sv2ssim_Convert2Tsv() {
    atf_comp::ProcStart("$bindir/sv2ssim test/csv/1.csv -ctype a.B -schema -data -outseparator:$'\\t' -header:N");
}

void atf_comp::comptest_sv2ssim_UniqueFieldName() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/sv2ssim - -ctype a.B -schema -data");
    atf_comp::ProcWrite(proc, "1,1,1,1,,");
    atf_comp::ProcWrite(proc, "2,2,2,2,2");
}
