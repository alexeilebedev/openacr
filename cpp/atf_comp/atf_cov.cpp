// Copyright (C) 2026 AlgoX2 Corp
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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/atf_cov.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"


// -----------------------------------------------------------------------------

// A gcov command that exits non-zero is named in the merge report with the
// lines gcov printed about the file it refused, and a directory whose profile
// data yielded no coverage at all is reported lost.  The directory holds one
// .gcda that is not a profile, so gcov refuses it and the notes file beside
// it, exits 2, and produces no .gcov; atf_cov reports both and exits 1.  The
// report is read back from a file, because the harness drops every output line
// naming a .gcda, which is gcov's own noise in a coverage build.
//
// The gcov that runs is a stand-in that refuses every file the way GNU gcov
// does.  The gcov on a macOS host is LLVM's, which words its refusal
// differently and exits 0 on a file it cannot read, and this test is about
// what atf_cov makes of a refusal.
void atf_comp::comptest_atf_cov_GcovFail() {
    CreateDirRecurse(atf_comp::TempPath("bad.d"));
    CreateDirRecurse(atf_comp::TempPath("bin"));
    StringToFile("garbage", atf_comp::TempPath("bad.d/x.gcda"));
    StringToFile("#!/bin/sh\n"
                 "for f in \"$@\"; do\n"
                 "    case \"$f\" in\n"
                 "    *.gcda) echo \"$f:not a gcov data file\"; echo \"${f%.gcda}.gcno:cannot open notes file\";;\n"
                 "    esac\n"
                 "done\n"
                 "exit 2\n", atf_comp::TempPath("bin/gcov"));
    errno_vrfy_(chmod(algo::Zeroterm(atf_comp::TempPath("bin/gcov")), 0755) == 0);
    atf_comp::FProc &atf_cov = atf_comp::ProcStart("PATH=$tempdir/bin:$$PATH $bindir/atf_cov -covdir:$tempdir/cov -logfile:$tempdir/atf_cov.log -mergepath:$tempdir/bad.d -gcov:Y -summary:N 2> $tempdir/stderr.txt");
    atf_comp::ProcWait(atf_cov, 1);
    tempstr fail_line;
    tempstr merge_line;
    ind_beg(algo::FileLine_curs, line, atf_comp::TempPath("stderr.txt")) {
        if (StartsWithQ(line, "atf_cov.gcov_fail")) {
            fail_line = line;
        } else if (StartsWithQ(line, "atf_cov.merge")) {
            merge_line = line;
        }
    }ind_end;
    atf_comp::Check("gcov_fail_exit", tempstr() << Bool(FindStr(fail_line, "  exit:2  ") != -1));
    atf_comp::Check("gcov_fail_names_gcda", tempstr() << Bool(FindStr(fail_line, "x.gcda:not a gcov data file") != -1));
    atf_comp::Check("gcov_fail_names_gcno", tempstr() << Bool(FindStr(fail_line, "x.gcno:cannot open notes file") != -1));
    atf_comp::Check("merge_lost", tempstr() << Bool(FindStr(merge_line, "n_gcda:1  n_gcov:0  n_fail:1  success:N") != -1));
}
