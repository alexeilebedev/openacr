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
// Target: atf_norm (exe) -- Normalization tests (see normcheck table)
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
    atf_norm::CheckCleanDirs(".");
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_atf_amc() {
    command::atf_amc_proc atf_amc;
    atf_amc.fstdout = "> temp/atf_amc";
    if (atf_amc_Exec(atf_amc)!=0) {
        SysCmd("cat temp/atf_amc");
        vrfy(0, "atf_amc returned an error");
    }
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_atf_unit() {
    command::atf_unit_proc atf_unit;
    atf_unit.cmd.perf_secs=0;
    atf_unit.fstdout = "> temp/atf_unit";
    if (atf_unit_Exec(atf_unit)!=0) {
        SysCmd("cat temp/atf_unit");
        vrfy(0, "atf_unit returned an error");
    }
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
    algo::UnTime start=algo::CurrUnTime();
    try {
        atf_norm::_db.c_normcheck=&normcheck;
        int code_before = algo_lib::_db.exit_code;
        normcheck.step();
        success = success && !(code_before ==0 && algo_lib::_db.exit_code != 0);
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
          <<Keyval("runtime", algo::CurrUnTime()-start)
          <<Keyval("success", Bool(success))
          <<Keyval("comment",normcheck.comment));
    return success;
}


// -----------------------------------------------------------------------------

void atf_norm::normcheck_bintests() {
    ind_beg(algo::Dir_curs,entry,"bin/test-*") {
        if (!entry.is_dir && !EndsWithQ(entry.pathname,"~")) {
            SysCmd(entry.pathname,FailokQ(false));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// start with clean state
void atf_norm::normcheck_checkclean() {
    CheckCleanDirsAll();
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_lineendings() {
    algo_lib::Regx regx;
    Regx_ReadSql(regx, "(txt/%)",true);
    cstring files(SysEval("git ls-files",FailokQ(true),1024*1024*100));
    ind_beg(Line_curs,fname,files) {
        if (Regx_Match(regx,fname) && FileQ(fname)) {
            SysCmd(tempstr() << "sed -i 's/\\r$//;s/\\r/\\n/g' "<<fname);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_shebang() {
    ind_beg(_db_scriptfile_curs,scriptfile,_db) {
        ind_beg(algo::FileLine_curs,line,scriptfile.gitfile) {
            if (StartsWithQ(line,"#!")) {
                strptr interpreter = Pathcomp(line,"!LR LL");
                if (interpreter != "/bin/sh" && interpreter != "/usr/bin/env") {
                    prerr(scriptfile.gitfile<<":1: Non-portable interpreter "<<interpreter<<" use /usr/bin/env");
                    algo_lib::_db.exit_code=1;
                }
            }
            break;// first line only
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_norm::Main() {
    algo_lib::DieWithParent();

    int n_run  = 0;
    int n_pass = 0;
    ind_beg(atf_norm::_db_normcheck_curs,normcheck,atf_norm::_db) {
        if (Regx_Match(atf_norm::_db.cmdline.normcheck, normcheck.normcheck)) {
            ++n_run;
            n_pass += RunCheck(normcheck);
        }
    }ind_end;

    CheckCleanDirsAll();

    if (n_run==0) {
        algo_lib::_db.exit_code=1;
        prlog("atf_norm.nomatch"
              <<Keyval("normcheck",atf_norm::_db.cmdline.normcheck)
              <<Keyval("comment","no tests matched specified pattern. run 'acr normcheck' to see the full list"));
    }

    if (algo_lib::_db.exit_code != 0) {
        ind_beg(_db_normcheck_curs,normcheck,_db) {
            if (normcheck.nerr) {
                prlog("atf_norm.failed"
                      <<Keyval("normcheck",normcheck.normcheck)
                      <<Keyval("success",Bool(false))
                      <<Keyval("comment","See prior output for more information"));
            }
        }ind_end;
    }

    prlog("report.atf_norm"
          <<Keyval("n_normcheck", atf_norm::normcheck_N())
          <<Keyval("n_run", n_run)
          <<Keyval("n_pass", n_pass)
          <<Keyval("success", Bool(algo_lib::_db.exit_code==0))
          <<Keyval("comment",(algo_lib::_db.exit_code==0
                              ? "The coast is clear. Proceed with caution :-)"
                              : "Some errors occured. Please examine them and try again.")));
}
