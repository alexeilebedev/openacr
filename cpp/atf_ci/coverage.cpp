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
