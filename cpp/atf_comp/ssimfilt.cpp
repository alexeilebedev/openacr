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
// Source: cpp/atf_comp/ssimfilt.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_ssimfilt_Csv() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -format:csv");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe       comment:Executable");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:lib       comment:Library");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:none      comment:\"Skip building\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:objlist   comment:\"Object files are added to dependents' link line\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:pch       comment:\"Precompiled header\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb    comment:\"Ssim database (not a target)\"");
}

void atf_comp::comptest_ssimfilt_CsvField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -format:csv -field:comment");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe       comment:Executable");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:lib       comment:Library");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:none      comment:\"Skip building\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:objlist   comment:\"Object files are added to dependents' link line\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:pch       comment:\"Precompiled header\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb    comment:\"Ssim database (not a target)\"");
}

void atf_comp::comptest_ssimfilt_FirstTag() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt ^");
    atf_comp::ProcWrite(proc, "dmmeta.ns ns:blah");
    atf_comp::ProcWrite(proc, "dmmeta.ctype ctype:blah.Y");
}

void atf_comp::comptest_ssimfilt_Json() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -format:json");
    atf_comp::ProcWrite(proc, "report.acr  n_select:4741  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0");
}

void atf_comp::comptest_ssimfilt_JsonRecursive() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -format json");
    atf_comp::ProcWrite(proc, "atf_amc.TypeTVal j:33 types:'atf_amc.TypeS types:1 comment:\"lkjljk\"'");
    atf_comp::ProcWrite(proc, "atf_amc.TypeTVal j:22 types:'atf_amc.TypeS types:2 comment:\"eryerty\"'");
    atf_comp::ProcWrite(proc, "atf_amc.TypeTVal j:11 types:'atf_amc.TypeS types:3 comment:\"asdf\"'");
}

void atf_comp::comptest_ssimfilt_MatchField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt % ns:b%");
    atf_comp::ProcWrite(proc, "dmmeta.ns ns:blah");
    atf_comp::ProcWrite(proc, "dmmeta.ctype ctype:blah.Y");
    atf_comp::ProcWrite(proc, "dev.target target:ssimfilt");
}

void atf_comp::comptest_ssimfilt_MatchTag() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt dmmeta.%");
    atf_comp::ProcWrite(proc, "dmmeta.ns ns:blah");
    atf_comp::ProcWrite(proc, "dmmeta.ctype ctype:blah.Y");
    atf_comp::ProcWrite(proc, "dev.target target:ssimfilt");
}

void atf_comp::comptest_ssimfilt_SelectField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -field:nstype -format:field");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe       comment:Executable");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:lib       comment:Library");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:none      comment:\"Skip building\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:objlist   comment:\"Object files are added to dependents' link line\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:pch       comment:\"Precompiled header\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb    comment:\"Ssim database (not a target)\"");
}

void atf_comp::comptest_ssimfilt_Stable() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -format:stablefld");
    atf_comp::ProcWrite(proc, "report.acr  n_select:0  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0");
    atf_comp::ProcWrite(proc, "dev.uname  uname:Linux    comment:\"Ubuntu 17, CentOS 7.6, Debian\"");
}

void atf_comp::comptest_ssimfilt_Table() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/ssimfilt -t");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:exe       comment:Executable");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:lib       comment:Library");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:none      comment:\"Skip building\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:objlist   comment:\"Object files are added to dependents' link line\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:pch       comment:\"Precompiled header\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:ssimdb    comment:\"Ssim database (not a target)\"");
}
