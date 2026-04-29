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
// Source: cpp/atf_comp/coverage.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

// Set up coverage mode: force cfg, clear and create covdir
void atf_comp::CoverageSetup() {
    _db.cmdline.cfg = dev_Cfg_cfg_coverage;
    tempstr covdir("temp/atf_comp/cov.d");
    algo::RemDirRecurse(covdir, true);
    algo::CreateDirRecurse(covdir);
}

// Run final coverage merge pass after all tests complete
void atf_comp::CoverageFinalize() {
    u8 mode = _db.cmdline.mode;
    tempstr covdir("temp/atf_comp/cov.d");
    command::atf_cov_proc atf_cov;
    atf_cov.cmd.covdir = covdir;
    atf_cov.cmd.logfile = DirFileJoin(covdir, "atf_cov.log");
    atf_cov.cmd.mergepath = covdir;
    atf_cov.cmd.gcov = true;
    atf_cov.cmd.ssim = true;
    atf_cov.cmd.report = true;
    atf_cov.cmd.check = mode == command_atf_comp_mode_covcheck;
    atf_cov.cmd.capture = mode == command_atf_comp_mode_covcapture;
    prlog("atf_comp.coverage"<<Keyval("cmd", atf_cov_ToCmdline(atf_cov)));
    int rc = atf_cov_Exec(atf_cov);
    SysCmd(tempstr() << "cat " << DirFileJoin(covdir, "summary.txt"));
    SysCmd(tempstr() << "grep success:N " << atf_cov.cmd.logfile);
    if (rc != 0) {
        algo_lib::_db.exit_code = 1;
    }
}
