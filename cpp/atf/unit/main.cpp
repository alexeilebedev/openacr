// (C) AlgoEngineering LLC 2008-2013
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
// Target: atf_unit (exe) -- Algo Test Framework: unit tests
// Exceptions: yes
// Source: cpp/atf/unit/main.cpp
//
// Created By: alexei.lebedev alexey.polovinkin
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev jeffrey.wang
//

#include "include/atf_unit.h"


// -----------------------------------------------------------------------------

static void Main_Debug() {
    tempstr single_case;
    tempstr single_case_cpp;
    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select && !ch_N(single_case)) {
        single_case=unittest.unittest;
    }ind_end;
    single_case_cpp << "atf_unit::unittest_" << Pathcomp(single_case,".LL") << "_" << Pathcomp(single_case,".LR");

    command::mdbg mdbg;
    mdbg.target = "atf_unit";
    mdbg.args = tempstr() << " -nofork " << single_case;
    mdbg.b = tempstr() << single_case_cpp<<",Testcmpfail";
    SysCmd(mdbg_ToCmdline(mdbg), FailokQ(true), DryrunQ(false));
}

// -----------------------------------------------------------------------------

static void Main_Test(atf_unit::FUnittest &test, int temp_fd) {
    atf_unit::_db.c_test = &test;
    bool do_exit = false;// used in case of forking
    bool do_output = true;
    bool do_test = true;// only child runs test if forking
    bool do_fork = !atf_unit::_db.cmdline.nofork;
    int child_pid = 0;
    if (do_test && do_fork) {
        child_pid = fork();
        errno_vrfy(child_pid!=-1, "fork");
        do_test = child_pid == 0;
        do_exit = child_pid == 0;
    }
    if (do_test) {
        try {
            if (do_fork) {
                alarm(atf_unit::_db.cmdline.pertest_timeout);// reasonable timeout
            }
            value_SetEnum(test.c_testrun->testresult, atf_Testresult_PASSED);
            (*test.step)();
        }catch(algo_lib::ErrorX &x) {
            prerr("error "<<x);
            value_SetEnum(test.c_testrun->testresult,atf_Testresult_FAILED);
            test.c_testrun->comment <<x; // algo.Comment may be too small to handle big exception
        }
    }
    // parent: wait for child
    if (do_fork && child_pid != 0) {
        int status = 0;
        int ret    = waitpid(0,&status,0);
        vrfy(ret == child_pid, "waitpid");
        if (status != 0) {
            value_SetEnum(test.c_testrun->testresult,atf_Testresult_FAILED);
            test.c_testrun->comment << "Subprocess exited with code "<<status;
            if (WIFSIGNALED(status)) {
                test.c_testrun->comment << " ("<<strsignal(WTERMSIG(status))<<")";
            }
        }
        do_output = false;// parent doesn't print testrun record, child will!
    }
    // child atf runs in a separate process, so it must use file i/o
    // to report test results.
    if (do_output) {
        tempstr out;
        atf::Testrun testrun;
        atf_unit::testrun_CopyOut(*test.c_testrun, testrun);
        out << testrun << eol;
        write(temp_fd,out.ch_elems,out.ch_n);
    }
    if (do_exit) {
        close(temp_fd);
        // MUST call exit, not _exit, to ensure coverage files get written out
        exit(atf_unit::_db.report.n_err);
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_amc_Unit() {
    command::atf_amc_proc atf_amc;
    atf_amc_ExecX(atf_amc);
}

// -----------------------------------------------------------------------------

void atf_unit::Main() {
    atf_unit::_db.perf_cycle_budget = ToSchedTime(atf_unit::_db.cmdline.perf_secs);

    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) {
        atf_unit::_db.report.n_test_total++;
        bool match = Regx_Match(atf_unit::_db.cmdline.unittest, unittest.unittest);
        unittest.select = match;
        atf_unit::_db.report.n_test_run += unittest.select;
    }ind_end;

    // Useful warning
    if (atf_unit::_db.report.n_test_run==0) {
        prerr("atf_uniq.regex_warning"
              <<Keyval("regex", atf_unit::_db.cmdline.unittest)
              <<Keyval("comment", "no tests match regx. See 'acr unittest' for a complete list of runnable tests"));
    }

    if (atf_unit::_db.cmdline.debug) {
        Main_Debug();
        _exit(0);
    }

    // create testrun objects -- one for selected test, always from scratch.
    atf_unit::testrun_RemoveAll();
    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select) {
        atf_unit::FTestrun &testrun = atf_unit::testrun_Alloc();
        testrun.testrun = unittest.unittest;
        vrfy(testrun_XrefMaybe(testrun), algo_lib::_db.errtext);
    }ind_end;

    // a temp. file is used to write test results from child subprocesses.
    // it'd be nice to use a table, but a table cannot be shared between processes.
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"atf_unit.out");

    ind_beg(atf_unit::_db_unittest_curs,unittest, atf_unit::_db) if (unittest.select) {// loop over selected tests
        Main_Test(unittest, tempfile.fildes.fd.value);
    }ind_end;

    // merge testrun results as written by child processes
    cstring tempdata(FileToString(tempfile.filename));
    ind_beg(Line_curs,line,tempdata) {
        atf::Testrun testrun;
        if (Testrun_ReadStrptrMaybe(testrun,line)) {
            atf_unit::FTestrun *ftestrun = atf_unit::ind_testrun_Find(testrun.testrun);
            if (ftestrun) {
                // worsen testresult.
                // child may say the test is a success, but if we know the child exited with nonzero status,
                // we must declare it a failure.
                if (ftestrun->testresult == atf_Testresult_UNTESTED) {
                    ftestrun->testresult = testrun.testresult;
                }
                ftestrun->n_step += testrun.n_step;
                ftestrun->n_cmp += testrun.n_cmp;
                ftestrun->comment = tempstr() << ftestrun->comment << testrun.comment;
                // all other attributes are already known here in the parent process
            }
        }
    }ind_end;
    Refurbish(tempfile);

    // update error counter
    ind_beg(atf_unit::_db_testrun_curs, testrun, atf_unit::_db) {
        atf_unit::_db.report.n_test_total += 1;
        atf_unit::_db.report.n_test_run   += testrun.p_test->select;
        atf_unit::_db.report.n_test_step  += testrun.n_step;
        atf_unit::_db.report.n_cmp        += testrun.n_cmp;
        atf_unit::_db.report.n_err        += testrun.testresult != atf_Testresult_PASSED && testrun.testresult != atf_Testresult_UNTESTED;
    }ind_end;

    // print all tests
    ind_beg(atf_unit::_db_testrun_curs, testrun, atf_unit::_db) if (testrun.testresult != atf_Testresult_FAILED) {
        atf::Testrun _testrun;
        atf_unit::testrun_CopyOut(testrun, _testrun);
        prlog(_testrun);
    }ind_end;

    // printed failed tests at the end
    ind_beg(atf_unit::_db_testrun_curs, testrun, atf_unit::_db) if (testrun.testresult == atf_Testresult_FAILED) {
        atf::Testrun _testrun;
        atf_unit::testrun_CopyOut(testrun, _testrun);
        prlog(_testrun);
    }ind_end;

    // print report
    // report.n_test_step is reported incorrectly, because the value is not collected
    // from child processes!
    prlog(atf_unit::_db.report);
    algo_lib::_db.exit_code = atf_unit::_db.report.n_err;// zero errors = zero exit code
}

// -----------------------------------------------------------------------------

// usage:
// DO_PERF_TEST("Testing XYZ",xyz());
// The expression will be evaluated for 2 seconds, after which average speed will be printed.
void atf_unit::PrintPerfSample(const strptr& action, u64 nloops, u64 clocks) {
    UnDiff time_per_loop(clocks * (1e9/get_cpu_hz()) / nloops);
    prlog("atf_unit.perf"
          <<Keyval("action",action)
          <<Keyval("nloops",nloops)
          <<Keyval("time_per_loop",time_per_loop));
}

// -----------------------------------------------------------------------------

void atf_unit::Testcmp(const char *file, int line, strptr value, strptr expect, bool eq) {
    atf_unit::_db.c_test->c_testrun->n_cmp++;
    if (!eq) {
        prerr(file<<":"<<line<<": atf_unit.fail"
              <<Keyval("unittest", atf_unit::_db.c_test->unittest)
              <<Keyval("expression",value)
              <<Keyval("expected_value",value)
              <<Keyval("comment", "Expression doesn't match expected value"));
        atf_unit::FUnittest &test = *atf_unit::_db.c_test;
        if (!ch_N(test.c_testrun->comment)) {
            test.c_testrun->comment
                <<Keyval("value",value)
                <<Keyval("expect",expect);
        }
        value_SetEnum(test.c_testrun->testresult,atf_Testresult_FAILED);
        atf_unit::_db.report.n_err++;
    }
}

// -----------------------------------------------------------------------------

void atf_unit::Testcmp(const char *file, int line, const char *value, const char *expect, bool eq) {
    Testcmp(file, line, strptr(value), strptr(expect), eq);
}
