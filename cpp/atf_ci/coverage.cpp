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
// Source: cpp/atf_ci/coverage.cpp
//
// Coverage cijob: cov_prep builds the instrumented binaries and clears the
// covdir tree, the middle citests exercise them (each into its own
// temp/cov/<citest>.d via the GCC_PROFILE_DIR that RunCiTest sets), and
// cov_finalize gcov-merges every per-citest dir into dev.tgtcov.

#include "include/algo.h"
#include "include/atf_ci.h"

// First coverage citest: clear stale per-citest covdirs, then build every
// target with the coverage cfg.  atf_ci is the single place that builds the
// coverage binaries; the test tools never build.  No -install, so bin/ keeps
// pointing at the release build -- the citests reach the instrumented
// binaries through -bindir:build/coverage instead.  cov_prep runs no
// instrumented binary, so it needs no covdir of its own.
void atf_ci::citest_cov_prep() {
    algo::RemDirRecurse("temp/cov", false);
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

// Last coverage citest: gcov + merge every per-citest covdir, write the
// reports, and check against dev.tgtcov (or, with -capture, rebaseline
// dev.tgtcov + dev.uncovfunc).
void atf_ci::citest_cov_finalize() {
    tempstr mergepath;
    ind_beg(algo::Dir_curs, ent, "temp/cov/*.d") {
        if (ent.is_dir) {
            if (ch_N(mergepath)) {
                mergepath << ":";
            }
            mergepath << ent.pathname;
        }
    }ind_end;
    command::atf_cov_proc atf_cov;
    atf_cov.cmd.covdir = "temp/cov";
    atf_cov.cmd.logfile = "temp/cov/atf_cov.log";
    atf_cov.cmd.mergepath = mergepath;
    atf_cov.cmd.gcov = true;
    atf_cov.cmd.ssim = true;
    atf_cov.cmd.report = true;
    atf_cov.cmd.check = !CaptureQ();
    atf_cov.cmd.capture = CaptureQ();
    prlog("atf_ci.coverage"<<Keyval("cmd", atf_cov_ToCmdline(atf_cov)));
    int rc = atf_cov_Exec(atf_cov);
    SysCmd(tempstr() << "cat temp/cov/summary.txt");
    if (rc != 0) {
        algo_lib::_db.exit_code = 1;
    }
}
