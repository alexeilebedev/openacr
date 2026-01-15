// Copyright (C) 2023,2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/main.cpp
//

#include "include/atf_unit.h"
#include "include/lib_exec.h"

// -----------------------------------------------------------------------------

void atf_unit::AdjustDebugPath(algo::cstring &path) {
    (void)path;
#ifdef _DEBUG
    Replace(path,"release/","debug/");
#endif
}

// -----------------------------------------------------------------------------

// Compare contents of file `outfname` with the reference file.
// Any difference = error
void atf_unit::CompareOutput(strptr outfname) {
    tempstr expect_fname = tempstr() << "test/atf_unit/" << atf_unit::_db.c_curtest->unittest;
    if (_db.cmdline.capture) {
        CopyFileX(outfname, expect_fname, 0644);
    } else {
        vrfy_(SysCmd(tempstr()
                     << "git --no-pager diff --no-index "
                     << " " << expect_fname
                     << " " << outfname) == 0);
    }
}

// -----------------------------------------------------------------------------

static void Main_Debug() {
    tempstr single_case;
    tempstr single_case_cpp;
    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select && !ch_N(single_case)) {
        single_case=unittest.unittest;
    }ind_end;
    single_case_cpp << "atf_unit::unittest_" << Pathcomp(single_case,".LL") << "_" << Pathcomp(single_case,".LR");

    command::mdbg_proc mdbg;
    mdbg.cmd.target = "atf_unit";
    args_Alloc(mdbg.cmd) << "-nofork";
    args_Alloc(mdbg.cmd) << single_case;
    b_Alloc(mdbg.cmd) << single_case_cpp;
    b_Alloc(mdbg.cmd) << "Testcmpfail";
    int rc=mdbg_Exec(mdbg);
    algo_lib::_db.exit_code=rc;
}

// -----------------------------------------------------------------------------

static void CheckUntrackedFiles(cstring &comment) {
    cstring untracked_files=SysEval("git status -u --porcelain",FailokQ(false),1024*1024);
    algo::ListSep ls;
    ind_beg(Line_curs,line,untracked_files) {
        if (StartsWithQ(line,"??")) {
            comment << ls << Pathcomp(line," LR");
        }
    }ind_end;
}

static void Main_CheckUntrackedFiles() {
    cstring untracked_files;
    CheckUntrackedFiles(untracked_files);
    vrfy(untracked_files == ""
         , tempstr()
         <<"atf_unit.untracked_files"
         <<Keyval("files",untracked_files)
         <<Keyval("comment", "Please remove any untracked files before running atf_unit"));
}

// -----------------------------------------------------------------------------

// Run specified test (called both with -nofork and without)
void atf_unit::Main_StartTest(atf_unit::FUnittest &test, lib_exec::FSyscmd *start, lib_exec::FSyscmd *end) {
    atf_unit::_db.c_curtest = &test;
    if (atf_unit::_db.cmdline.nofork) {
        try {
            prlog("atf_unit.begin"
                  <<Keyval("unittest",test.unittest));
            alarm(atf_unit::_db.cmdline.pertest_timeout);// reasonable timeout
            (*test.step)();
        }catch(algo_lib::ErrorX &x) {
            test.error << "exception: "<<x;
        }
        if (test.error=="" && _db.cmdline.check_untracked) {
            // check for untracked files
            tempstr files;
            CheckUntrackedFiles(files);
            if (files != "") {
                test.error << "Test created untracked files: "<<files;
            }
        }
        test.success = test.error == "";
        prlog("atf_unit.unittest"
              <<Keyval("unittest",test.unittest)
              <<Keyval("success",test.success)
              <<Keyval("comment",test.error)
              );
    } else {
        command::atf_unit_proc cmd;
        cmd.path="/proc/self/exe";
        cmd.cmd.unittest.expr   = test.unittest;
        cmd.cmd.capture         = atf_unit::_db.cmdline.capture;
        cmd.cmd.perf_secs       = atf_unit::_db.cmdline.perf_secs;
        cmd.cmd.pertest_timeout = atf_unit::_db.cmdline.pertest_timeout;
        cmd.cmd.data_dir        = atf_unit::_db.cmdline.data_dir;
        cmd.cmd.nofork          = true;
        cmd.cmd.check_untracked = atf_unit::_db.cmdline.check_untracked;
        cmd.cmd.report          = false; // prevent double reporting

        lib_exec::FSyscmd &syscmd = lib_exec::NewCmd(start,end);
        syscmd.redir_out          = true;
        syscmd.show_out           = true;
        atf_unit_ToArgv(cmd,syscmd.args);
        test.c_syscmd=&syscmd;
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_amc_Unit() {
    command::atf_amc_proc atf_amc;
    atf_amc_ExecX(atf_amc);
}

// -----------------------------------------------------------------------------

// usage:
// DO_PERF_TEST("Testing XYZ",xyz());
// The expression will be evaluated for 2 seconds, after which average speed will be printed.
void atf_unit::PrintPerfSample(const strptr& action, u64 nloops, u64 clocks) {
    algo::UnDiff time_per_loop(clocks * (1e9/algo::get_cpu_hz()) / nloops);
    prlog("atf_unit.perf"
          <<Keyval("action",action)
          <<Keyval("nloops",nloops)
          <<Keyval("time_per_loop",time_per_loop));
}

// -----------------------------------------------------------------------------

void atf_unit::Testcmp(const char *file, int line, strptr value, strptr expect, bool eq) {
    if (!eq) {
        prerr(file<<":"<<line<<": atf_unit.fail"
              <<Keyval("unittest", atf_unit::_db.c_curtest->unittest)
              <<Keyval("expression",value)
              <<Keyval("expected_value",value)
              <<Keyval("comment", "Expression doesn't match expected value"));
        vrfy(0,tempstr()<<Keyval("value",value)<<Keyval("expect",expect));
        atf_unit::_db.report.n_err++;
    }
}

// -----------------------------------------------------------------------------

void atf_unit::Testcmp(const char *file, int line, const char *value, const char *expect, bool eq) {
    Testcmp(file, line, strptr(value), strptr(expect), eq);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_atf_unit_Outfile() {
    ind_beg(algo::Dir_curs,entry,"test/atf_unit/*") {
        if (!EndsWithQ(entry.filename,"~")) {
            vrfy(atf_unit::ind_unittest_Find(entry.filename)
                 ,tempstr()<<"foreign file "<<entry.pathname);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_unit::Main() {
    atf_unit::_db.perf_cycle_budget = algo::ToSchedTime(atf_unit::_db.cmdline.perf_secs);
    lib_exec::_db.cmdline.maxjobs = i32_Max(4,sysconf(_SC_NPROCESSORS_ONLN));
    lib_exec::_db.cmdline.q=true;
    lib_exec::_db.cmdline.complooo=false;
    lib_exec::_db.cmdline.merge_output=true;

    int nsel=0;
    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) {
        atf_unit::_db.report.n_test_total++;
        bool match = Regx_Match(atf_unit::_db.cmdline.unittest, unittest.unittest);
        unittest.select = match;
        nsel += unittest.select;
    }ind_end;

    // Useful warning
    if (nsel==0) {
        prerr("atf_uniq.regex_warning"
              <<Keyval("regex", atf_unit::_db.cmdline.unittest)
              <<Keyval("comment", "no tests match regx. See 'acr unittest' for a complete list of runnable tests"));
    }

    // Drop into debugger
    if (atf_unit::_db.cmdline.mdbg) {
        Main_Debug();
        _exit(0);
    }

    if (_db.cmdline.check_untracked) {
        Main_CheckUntrackedFiles();
    }

    atf_unit::_db.report.n_test_total += atf_unit::unittest_N();
    lib_exec::FSyscmd *start=NULL,*end=NULL;
    if (!_db.cmdline.nofork) {
        start=&lib_exec::NewCmd(NULL,NULL);
        end=&lib_exec::NewCmd(start,NULL);
    }
    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select) {
        Main_StartTest(unittest,start,end);
        atf_unit::_db.report.n_test_run++;
    }ind_end;
    // parallel version
    if (!_db.cmdline.nofork) {
        lib_exec::SyscmdExecute();
        ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select) {
            if (unittest.c_syscmd && !CompletedOKQ(*unittest.c_syscmd)) {
                unittest.error << "Subprocess exited with code "<<unittest.c_syscmd->status;
            }
        }ind_end;
    }
    // set success flag and count errors
    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select) {
        unittest.success = unittest.error == "";
        _db.report.n_err += !unittest.success;
    }ind_end;


    atf_unit::_db.report.success = atf_unit::_db.report.n_err==0;
    algo_lib::_db.exit_code = atf_unit::_db.report.n_err;// zero errors = zero exit code

    // show failed tests again
    if (_db.cmdline.report) {
        ind_beg(atf_unit::_db_unittest_curs, unittest, atf_unit::_db) {
            if (unittest.select && !unittest.success) {
                prlog("atf_unit.unittest"
                      <<Keyval("unittest",unittest.unittest)
                      <<Keyval("success",unittest.success)
                      <<Keyval("repro",tempstr()<<"atf_unit "<<unittest.unittest)
                      );
            }
        }ind_end;
        prlog(atf_unit::_db.report);
    }
}
