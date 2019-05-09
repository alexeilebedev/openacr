// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: atf_norm (exe) -- Run normalization tests (see normcheck table)
// Exceptions: yes
// Source: cpp/atf/norm/main.cpp
//
// Created By: alexei.lebedev vladimir.parizhsky
// Recent Changes: alexei.lebedev vladimir.parizhsky hayk.mkrtchyan
//

#include "include/atf_norm.h"
#include "include/lib_git.h"

// -----------------------------------------------------------------------------

static strptr CurNormcheck() {
    return atf_norm::_db.c_normcheck ? strptr(atf_norm::_db.c_normcheck->normcheck) : strptr();
}

// -----------------------------------------------------------------------------

// Check that a list of directories specified with DIRS is clean
// Nothing is done if an error exit code is already set, since
// this error might have been caused by the previous error.
void atf_norm::CheckCleanDirs(strptr dirs) {
    if (algo_lib::_db.exit_code==0) {
        tempstr mod = lib_git::GitModifiedFiles(dirs);
        if (mod != "") {
            prerr("atf_norm.modified_files"
                  <<Keyval("during",CurNormcheck())
                  <<Keyval("files",mod)
                  <<Keyval("success","N")
                  <<Keyval("comment","Please resolve modified files and try again"));
            algo_lib::_db.exit_code=1;
        }
    }
}

static void CheckCleanDirsAll() {
    atf_norm::CheckCleanDirs("cpp include data sql gen");
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_testamc() {
    command::atf_amc atf_amc;
    SysCmd(atf_amc_ToCmdline(atf_amc)<< ">/dev/null",FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_unit() {
    command::atf_unit atf_unit;
    atf_unit.perf_secs=0;
    SysCmd(atf_unit_ToCmdline(atf_unit) << ">/dev/null",FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_bootstrap() {
    SysCmd("make bootstrap",FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_iffy_src() {
    command::src_func src_func;
    src_func.iffy = true;
    src_func.check = true;
    src_func.listfunc = true;
    src_func.proto = true;
    SysCmd(src_func_ToCmdline(src_func),FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_normalize_amc_vis() {
    command::amc_vis amc_vis;
    amc_vis.ctype.expr = "%";
    amc_vis.check      = true;
    SysCmd(amc_vis_ToCmdline(amc_vis),FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_gitfile() {
    SysCmd("bin/update-gitfile >/dev/null",FailokQ(false));
    ind_beg(algo::FileLine_curs,line,SsimFname(atf_norm::_db.cmdline.in,dmmeta_Ssimfile_ssimfile_dev_gitfile)) {
        dev::Gitfile gitfile;
        if (Gitfile_ReadStrptrMaybe(gitfile,line)) {
            if (!FileObjectExistsQ(gitfile.gitfile)) {
                prlog("atf_norm.missing_file"
                      <<Keyval("success","N")
                      <<Keyval("gitfile",gitfile.gitfile)
                      <<Keyval("comment","File missing from filesystem"));
                algo_lib::_db.exit_code=1;
            }
        }
    }ind_end;
    CheckCleanDirs("data");
}

// -----------------------------------------------------------------------------

static bool RunCheck(atf_norm::FNormcheck &normcheck) {
    bool success=true;
    UnTime start=CurrUnTime();
    try {
        atf_norm::_db.c_normcheck=&normcheck;
        normcheck.step();
    } catch (algo_lib::ErrorX &x) {
        prlog(x.str);
        success=false;
    }
    if (normcheck.nerr>0) {
        success=false;
    }
    if (success==false) {
        algo_lib::_db.exit_code = 1;
    }
    prlog("atf_norm.normcheck"
          <<Keyval("normcheck",normcheck.normcheck)
          <<Keyval("runtime", CurrUnTime()-start)
          <<Keyval("success", Bool(success))
          <<Keyval("comment",normcheck.comment));
    return success;
}

// -----------------------------------------------------------------------------

void atf_norm::Main() {
    algo_lib::DieWithParent();

    // start with clean state
    CheckCleanDirsAll();

    int n_run  = 0;
    int n_pass = 0;
    ind_beg(atf_norm::_db_normcheck_curs,normcheck,atf_norm::_db) {
        if (Regx_Match(atf_norm::_db.cmdline.normcheck, normcheck.normcheck)) {
            ++n_run;
            n_pass += RunCheck(normcheck);
            CheckCleanDirsAll();
        }
    }ind_end;

    CheckCleanDirsAll();

    prlog("report.atf_norm"
          <<Keyval("n_normcheck", atf_norm::normcheck_N())
          <<Keyval("n_run", n_run)
          <<Keyval("n_pass", n_pass)
          <<Keyval("exit_code", algo_lib::_db.exit_code));

    if (n_run==0) {
        algo_lib::_db.exit_code=1;
        prlog("atf_norm.nomatch"
              <<Keyval("normcheck",atf_norm::_db.cmdline.normcheck)
              <<Keyval("comment","no tests matched specified pattern. run 'acr normcheck' to see the full list"));
    }
}
