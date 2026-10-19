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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/coverageA.cpp
//
// Coverage is measured by two jobs that run side by side, coverageA and
// coverageB, and one job that joins them, covfinal.  Each half builds the
// instrumented binaries, runs its citests (each citest marked gcov writes its
// profile data into its own temp/cov/<citest>.d, via the GCC_PROFILE_DIR that
// RunCiTest sets), and turns that data into line counts in temp/cov/<cijob>.
// gcov needs the profile data and the instrumented build together, so this
// last step runs on the host that ran the tests.  The line counts are plain
// ssim, which covfinal merges on a host of its own.

#include "include/algo.h"
#include "include/atf_ci.h"

// -----------------------------------------------------------------------------

// Build every target with the coverage cfg, as the first citest of a coverage
// job.  atf_ci is the single place that builds the coverage binaries; the test
// tools never build.  No -install, so bin/ keeps pointing at the release build
// -- the citests reach the instrumented binaries through -bindir:build/coverage
// instead.
void atf_ci::BuildCoverage() {
    command::abt_proc abt;
    Regx_ReadSql(abt.cmd.target, "%", true);
    abt.cmd.cfg.expr = "coverage";
    abt.cmd.install = false;
    abt_ExecX(abt);
    // An instrumented object is half of a measurement: it counts the lines it
    // executes, and the program graph the compiler wrote beside it says which
    // lines those are.  gcov needs both, and it reports an object whose graph
    // is missing the way it reports one that never ran -- as no data.  A
    // machine that cannot produce the graphs therefore spends an hour running
    // the suite and fails at the end, naming coverage targets rather than
    // itself.  The graphs are countable the moment the build ends, so this is
    // where a build that cannot be measured says so.
    u32 n_obj = 0;
    u32 n_gcno = 0;
    ind_beg(algo::Dir_curs,ent,"build/coverage/*.o") {
        n_obj += 1;
        n_gcno += FileQ(ReplaceExt(ent.pathname,".gcno"));
    }ind_end;
    bool success = n_gcno == n_obj;
    prlog("atf_ci.cov_prep"
          <<Keyval("n_obj",n_obj)
          <<Keyval("n_gcno",n_gcno)
          <<Keyval("success",success ? "Y" : "N")
          <<Keyval("comment",success ? "" : "coverage build wrote no program graph for these objects; gcov can measure nothing from them"));
    if (!success) {
        algo_lib::_db.exit_code = 1;
    }
}

// Directory that holds the line counts of coverage job CIJOB: what its last
// citest writes, what CI uploads, and what covfinal merges.
tempstr atf_ci::CovJobDir(strptr cijob) {
    return tempstr() << "temp/cov/" << cijob;
}

// Turn the profile data of the running citest's job into line counts, as the
// last citest of a coverage job.  Every citest of the job marked gcov left its
// data in temp/cov/<citest>.d; atf_cov runs gcov over each of those dirs, adds
// the counts up, and writes one .cov.ssim per source into CovJobDir.  Beside
// them goes the commit the counts measure, which covfinal compares with its
// own checkout before it merges anything.  The floors are not checked here,
// since a half measures a part of the suite and every target reads low.
void atf_ci::RunCovGcov() {
    strptr cijob = atf_ci::_db.c_citest->cijob;
    tempstr covdir = atf_ci::CovJobDir(cijob);
    tempstr mergepath;
    ind_beg(atf_ci::_db_citest_curs, citest, atf_ci::_db) {
        if (citest.cijob == cijob && citest.gcov) {
            if (ch_N(mergepath)) {
                mergepath << ":";
            }
            mergepath << "temp/cov/" << citest.citest << ".d";
        }
    }ind_end;
    algo::RemDirRecurse(covdir, true);
    algo::CreateDirRecurse(covdir);
    command::atf_cov_proc atf_cov;
    atf_cov.cmd.covdir = covdir;
    atf_cov.cmd.logfile = tempstr() << covdir << ".log";
    atf_cov.cmd.mergepath = mergepath;
    atf_cov.cmd.gcov = true;
    atf_cov.cmd.ssim = true;
    atf_cov.cmd.summary = false;
    prlog("atf_ci.coverage"<<Keyval("cmd", atf_cov_ToCmdline(atf_cov)));
    int rc = atf_cov_Exec(atf_cov);
    StringToFile(Trimmed(SysEval("git rev-parse HEAD", FailokQ(false), 1024)), DirFileJoin(covdir,"commit"));
    if (rc != 0) {
        algo_lib::_db.exit_code = 1;
    }
}

// -----------------------------------------------------------------------------

// First citest of the coverageA job: build the coverage binaries.
void atf_ci::citest_cov_prepA() {
    atf_ci::BuildCoverage();
}

// Run the C++ unit-test suite against the coverage build so unit-tested
// library functions are credited.  atf_unit spawns no children, so naming
// the instrumented binary directly is enough; GCC_PROFILE_DIR routes its
// gcda.  perf_secs:0 skips the timing benchmarks.
void atf_ci::citest_atf_unit_cov() {
    command::atf_unit_proc atf_unit;
    atf_unit.path = "build/coverage/atf_unit";
    atf_unit.cmd.perf_secs = 0;
    atf_unit_ExecX(atf_unit);
}

// Run every comptest against the coverage build: cfg:coverage makes
// atf_comp's $bindir = build/coverage, and the gcda land in the covdir
// RunCiTest exported via GCC_PROFILE_DIR.
void atf_ci::citest_atf_comp_cov() {
    command::atf_comp_proc atf_comp;
    atf_comp.cmd.cfg = dev_Cfg_cfg_coverage;
    atf_comp.cmd.maxerr = 3;
    atf_comp_ExecX(atf_comp);
}

// Last citest of the coverageA job: its line counts, for covfinal.
void atf_ci::citest_cov_gcovA() {
    atf_ci::RunCovGcov();
}
