// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2023 Astra
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
// Source: cpp/atf_comp/main.cpp
//

#include "include/atf_comp.h"
#include "include/lib_ctype.h"
#include "include/gen/dev_gen.h"

// -----------------------------------------------------------------------------

static tempstr GetArgs(atf_comp::FComptest &comptest) {
    tempstr ret;
    if (comptest.c_targs) {
        ret << comptest.c_targs->args;
    }
    return ret;
}

static tempstr GetFilter(atf_comp::FComptest &comptest) {
    tempstr ret;
    if (comptest.c_tfilt) {
        ret << comptest.c_tfilt->filter;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Compute component directory depending on config given on command line
void atf_comp::Main_GuessBindir() {
    atf_comp::_db.bindir = atf_comp::_db.cmdline.bindir;
    if (atf_comp::_db.bindir == "") {
        atf_comp::_db.bindir = tempstr("build/")<<atf_comp::_db.cmdline.cfg;
    }
    vrfy(DirectoryQ(atf_comp::_db.bindir), tempstr()<<"Can't find component directory: "<<atf_comp::_db.bindir);
}

// -----------------------------------------------------------------------------

// Run abt for build or ood check for config given on command line
// -cfg, -build options are passed directly to abt
void atf_comp::Main_Ood() {
    if (atf_comp::_db.cmdline.bindir == "") {
        if (atf_comp::_db.cmdline.build) {
            command::abt_proc abt;
            Regx_ReadSql(abt.cmd.target,"%",true);
            abt.cmd.cfg.expr = atf_comp::_db.cmdline.cfg;
            abt.cmd.build = atf_comp::_db.cmdline.build;
            abt_ExecX(abt);
        }
    } else {
        prlog("Component dir specified manually, build/ood check is not performed");
    }
}

// -----------------------------------------------------------------------------

// Check that no untracked files are located under test/ directory
// (this might make the test succeed locally and fail in CI)
static void CheckUntrackedFiles(cstring &comment) {
    cstring untracked_files=SysEval("git status -u --porcelain test",FailokQ(false),1024*1024);
    algo::ListSep ls;
    ind_beg(Line_curs,line,untracked_files) {
        if (StartsWithQ(line,"??")) {
            comment << ls << Pathcomp(line,"?RR");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Running instrumented executable or under dynamic analyzer
// may slow down execution greatly. Timeout given on the comptest record is
// 3 * typical elapsed time for running release executable without any tool.
// This functios applies known slowdown figures for various configurations and tools.
int atf_comp::CalcTimeout(atf_comp::FComptest &comptest) {
    double timeout(comptest.timeout);
    if (atf_comp::_db.cmdline.cfg == dev_Cfg_cfg_debug) {
        timeout *= 4;
    }
    if (atf_comp::_db.cmdline.cfg == dev_Cfg_cfg_coverage) {
        timeout *= 4;
    }
    if (atf_comp::_db.cmdline.memcheck) {
        timeout *= 30;
    }
    if (atf_comp::_db.cmdline.callgrind) {
        timeout *= 50;
    }
    return timeout;
}

// -----------------------------------------------------------------------------

// Initialize COMPTEST.BASH.CMD to the command that will execute
// If invoked with -memcheck, wrap the command line with valgrind
//   and initialize COMPTEST.FILE_MEMCHECK pathname
// If invoked with -callgrind, wrap the command line with valgrind
//   and initialize COMPTEST.FILE_CALLGRIND_LOG, COMPTEST.FILE_CALLGRIND_OUT pathname
// If invoked with -coverage, wrap the command line with atf_cov
//   and link with covdir
//
void atf_comp::SetupCmdline(atf_comp::FComptest &comptest, algo_lib::Replscope &R) {
    // Compose component path from component directory and target name given
    // on first part of testcase name
    comptest.bash.cmd.c   = DirFileJoin(atf_comp::_db.bindir,target_Get(comptest));
    if (comptest.c_tfilt) {
        comptest.filter_command = Subst(R,comptest.c_tfilt->filter);
    }
    if (comptest.c_targs) {
        comptest.bash.cmd.c << " ";
        Ins(&R,comptest.bash.cmd.c,comptest.c_targs->args,false);
    }
    vrfy_(comptest.dir != "");
    if (atf_comp::_db.cmdline.memcheck) {
        comptest.file_memcheck = DirFileJoin(comptest.dir, "memcheck.log");
        comptest.bash.cmd.c = tempstr()
            << "valgrind --tool=memcheck --log-file=" << comptest.file_memcheck
            << " " << comptest.bash.cmd.c;
        DeleteFile(comptest.file_memcheck);
    }
    if (atf_comp::_db.cmdline.callgrind) {
        comptest.file_callgrind_log = DirFileJoin(comptest.dir, "callgrind.log");
        comptest.file_callgrind_out = DirFileJoin(comptest.dir, "callgrind.out");
        comptest.bash.cmd.c = tempstr()
            << "valgrind --tool=callgrind"
            << " --log-file=" << comptest.file_callgrind_log
            << " --callgrind-out-file=" << comptest.file_callgrind_out
            << " " << comptest.bash.cmd.c;
        DeleteFile(comptest.file_callgrind_log);
        DeleteFile(comptest.file_callgrind_out);
    }
    if (atf_comp::_db.cmdline.cfg == dev_Cfg_cfg_coverage) {
        FCovdir *covdir = zd_covdir_free_RemoveFirst();
        if (!covdir) {
            covdir = &covdir_Alloc();
            covdir->covdir = _db.cmdline.covfast
                ? DirFileJoin(atf_comp::_db.tempdir,"cov.d/") << covdir_N()
                : DirFileJoin(comptest.dir, "cov.d");
        }
        comptest.c_covdir = covdir;
        command::atf_cov_proc atf_cov;
        atf_cov.cmd.covdir = covdir->covdir;
        atf_cov.cmd.incremental = _db.cmdline.covfast;
        atf_cov.cmd.logfile = _db.cmdline.covfast
            ? DirFileJoin(covdir->covdir, comptest.comptest) << ".atf_cov.log"
            : DirFileJoin(covdir->covdir,"atf_cov.log");
        atf_cov.cmd.runcmd = comptest.bash.cmd.c;
        comptest.bash.cmd.c = tempstr() << atf_cov.path << " ";
        atf_cov_PrintArgv(atf_cov.cmd,comptest.bash.cmd.c);
    }
}

// -----------------------------------------------------------------------------

// Initialize COMPTEST.FILE_TEST_IN
// Write input messages to file
void atf_comp::PrepareInput(atf_comp::FComptest &comptest) {
    comptest.file_test_in  = DirFileJoin(comptest.dir,"in");
    tempstr teststr;
    ind_beg(comptest_zd_tmsg_curs,tmsg,comptest) {
        if (dir_Get(tmsg) == atfdb_Msgdir_msgdir_in) {
            teststr << tmsg.msg << eol;
        }
    }ind_end;
    StringToFile(teststr, comptest.file_test_in);
}

// -----------------------------------------------------------------------------

void atf_comp::Comptest_Start(atf_comp::FComptest &comptest) {
    if (comptest.nrun < i32_Max(_db.cmdline.minrepeat, comptest.repeat)) {
        // increment global counter -- this counts how many tests we started
        atf_comp::_db.report.nrun++;
        atf_comp::zd_run_comptest_Insert(comptest);
        _db.ncore_used += comptest.ncore;
        comptest.dir = algo::DirFileJoin(_db.tempdir,comptest.comptest);
        algo::RemDirRecurse(comptest.dir,false);// clear any old state for this case (maybe from previous run)
        algo::CreateDirRecurse(comptest.dir);
        PrepareInput(comptest);
        comptest.file_test_out = DirFileJoin(comptest.dir,"out");
        verblog("atf_comp.test_start"
                << Keyval("comptest",comptest.comptest)
                << Keyval("n_running",zd_run_comptest_N())
                << Keyval("ncore_used",_db.ncore_used)
                << Keyval("outfile",comptest.file_test_out)
                );

        // calculate timeout
        int timeout = CalcTimeout(comptest);
        // compute command line
        algo_lib::Replscope R;
        Set(R,"$$","$");
        Set(R,"$tempdir",comptest.dir);
        Set(R,"$comptest",comptest.comptest);
        Set(R,"$timeout",tempstr()<<timeout);
        Set(R,"$bindir",atf_comp::_db.bindir);
        SetupCmdline(comptest,R);
        verblog("atf_comp "<<comptest.comptest<<" -report:N -printinput | " << target_Get(comptest) << " "<<Subst(R,GetArgs(comptest)));
        comptest.bash.timeout = timeout;
        comptest.bash.fstdin << "<" << comptest.file_test_in;
        if (_db.cmdline.stream) {
            comptest.bash.cmd.c << " |& tee " << comptest.file_test_out;
        } else {
            comptest.bash.fstdout << ">" << comptest.file_test_out;
            comptest.bash.fstderr = ">&1";
        }
        int rc = bash_Start(comptest.bash);
        if (rc != 0) {
            comptest.err << "Failed to start "<<comptest.bash.cmd.c;
            Comptest_Finish(comptest);
        } else {
            comptest.start = algo::CurrSchedTime();
        }
    }
}

// -----------------------------------------------------------------------------

// Start next test
// If no tests are running, it's always OK to start.
// Otherwise, the test is started only if total "# of cores used" after
// starting the test would not exceed the limit
void atf_comp::StartNextTest() {
    if (!zd_sel_comptest_First()) {
        // done!
    } else if (atf_comp::_db.report.nerr >= _db.cmdline.maxerr) {
        prerr("atf_comp: skipping rest of tests");
        // don't reschedule
    } else if (!_db.ncore_used || _db.ncore_used + zd_sel_comptest_First()->ncore <= atf_comp::_db.cmdline.maxjobs) {
        atf_comp::FComptest &comptest = *atf_comp::zd_sel_comptest_RemoveFirst();
        Comptest_Start(comptest);
        ThScheduleIn(_db.th_runtest, algo::SchedTime(0));
    } else {
        // don't reschedule -- exiting comptest will reschedule
    }
}

// -----------------------------------------------------------------------------

// Compare output of current test with the reference file.
// If tfilt exists, filter output before matching
// Any difference = error
// return true for success, false for error
bool atf_comp::CompareOutput(atf_comp::FComptest &comptest) {
    cstring fname(comptest.file_test_out);
    if (comptest.c_tfilt) {
        fname << ".filt";
        command::bash_proc bash;
        bash.cmd.c = comptest.filter_command;
        bash.fstdin = tempstr() << "<" << comptest.file_test_out;
        bash.fstdout = tempstr() << ">" << fname;
        bash_ExecX(bash);
    };

    cstring expect_fname = tempstr() << "test/atf_comp/" << comptest.comptest;
    verblog("# ----- test output -----\n"<<FileToString(fname)<<"\n# ----- end test output -----");

    verblog("atf_comp.compare_file"
            <<Keyval("file1",fname)
            <<Keyval("file2",expect_fname));
    bool ok = true;
    bool exists = FileQ(expect_fname);
    if (_db.cmdline.capture) {
        CopyFileX(fname, expect_fname, 0644);
        if (!exists) {
            // add to git if new testcase
            (void)SysCmd(tempstr()<<"git add --force "<<expect_fname);
        }
    } else {
        ok = SysCmd(tempstr()
                    << "git --no-pager diff --no-index --ignore-cr-at-eol --color-words"
                    << " " << expect_fname
                    << " " << fname) == 0;
    }
    return ok;
}

// -----------------------------------------------------------------------------

// get number of memory errors for testcase.
// valgrind memcheck log example:
// ==664953== ERROR SUMMARY: 7 errors from 6 contexts (suppressed: 0 from 0)
//                          ~~~
// returns number of errors, in this case - 7
i32 atf_comp::GetNumMemoryErrors(atf_comp::FComptest &comptest) {
    i32 num_errors(0);
    if (comptest.file_memcheck != "") {
        ind_beg(algo::FileLine_curs,line,comptest.file_memcheck) {
            algo::StringIter it(line);
            GetWordCharf(it);
            it.Ws();
            if (SkipStrptr(it,"ERROR SUMMARY:") && TryParseI32(it, num_errors)) {
                break;
            }
        }ind_end;
    }
    return num_errors;
}

// -----------------------------------------------------------------------------

// Cleanup test structure after run
void atf_comp::Comptest_Finish(atf_comp::FComptest &comptest) {
    bool did_run = !(comptest.start == algo::SchedTime());
    algo::ListSep ls("; ");
    if (did_run) {
        if (WIFEXITED(comptest.bash.status)) {
            u8 exit_code = WEXITSTATUS(comptest.bash.status);
            if (exit_code != comptest.exit_code) {
                comptest.err << ls <<"Exit code:"<<exit_code<<" ("<<comptest.exit_code<<" expected)";
            }
        }
        if (WIFSIGNALED(comptest.bash.status)) {
            int sig = WTERMSIG(comptest.bash.status);
            comptest.err << ls << "Signal " << sig << ": " << strsignal(sig);
        }
        if (!CompareOutput(comptest)) {
            comptest.err << ls <<"Output does not match";
        }
        i32 num_errors = GetNumMemoryErrors(comptest);
        if (num_errors != 0) {
            SysCmd(tempstr() << "cat " << comptest.file_memcheck);
            comptest.err << ls << "Memory errors: " << num_errors;
        }
        if (comptest.file_callgrind_out != "" && FileQ(comptest.file_callgrind_out)) {
            prlog("atf_comp.callgrind"
                  << Keyval("output_file",comptest.file_callgrind_out)
                  << Keyval("comment","Use kcachegrind (linux), or qcachegrind (macos) to open the file"));
        }
        comptest.nrun++;
        comptest.finish = algo::CurrSchedTime();
        comptest.elapsed = ElapsedSecs(comptest.start,comptest.finish);
        if (comptest.err != "") {
            prlog("Component test output below:");
            ind_beg(algo::FileLine_curs,line,comptest.file_test_out) {
                prlog("    "<<line);
            }ind_end;
        }
    }
    comptest.success = did_run && ch_N(comptest.err) == 0;
    prlog("atf_comp.comptest"
          <<Keyval("comptest",comptest.comptest)
          <<Keyval("nrun",comptest.nrun)
          <<Keyval("success",comptest.success)
          <<Keyval("elapsed",comptest.elapsed)
          <<Keyval("comment",comptest.err));
    _db.ncore_used -= comptest.ncore;
    atf_comp::zd_run_comptest_Remove(comptest);
    if (comptest.c_covdir && _db.cmdline.covfast) {
        zd_covdir_free_Insert(*comptest.c_covdir);
    }
    // check if comptest needs to run more times
    // (under coverage, maxrepeat is 1)
    // if a comptest fails even once, it is not re-run
    atf_comp::_db.report.npass += did_run && comptest.success;
    atf_comp::_db.report.nerr  += comptest.success ? 0 : 1;
    int lim = algo::Clipped(comptest.repeat, _db.cmdline.minrepeat, _db.cmdline.maxrepeat+1);
    if (comptest.success && comptest.nrun < lim) {
        algo::Refurbish(comptest.bash);
        algo::Refurbish(comptest.start);
        algo::Refurbish(comptest.finish);
        zd_sel_comptest_Insert(comptest);// reinsert
    }
    // maybe start another test
    ThScheduleIn(_db.th_runtest, algo::SchedTime(0));
}

// -----------------------------------------------------------------------------

// Attempted whenever we suspect that waitpid()
// will return something interesting
// Decreases # of jobs
void atf_comp::zd_run_comptest_Step() {
    int status=0;
    int pid = waitpid(-1,&status,WNOHANG);//nonblocking wait
    if (pid > 0) {
        // find test that exited
        atf_comp::FComptest *entry=NULL;
        ind_beg(_db_comptest_curs,comptest,_db) {
            if (comptest.bash.pid == pid) {
                entry=&comptest;
                break;
            }
        }ind_end;
        // clean it up
        if (entry) {
            entry->bash.status=status;
            entry->bash.pid=-1;
            Comptest_Finish(*entry);
        }
    } else {
        // let child processes run until next sigchild
        atf_comp::zd_run_comptest_SetDelay(algo::ToSchedTime(60.0));
    }
}

// trigger waitpid call
static void SignalHandler(int sig) {
    (void)sig;
    atf_comp::zd_run_comptest_SetDelay(algo::SchedTime());// cause it to execute immediately
}

// Set up signal handler to detect process exits
void atf_comp::Main_SetupSigchild() {
    struct sigaction sigact;
    sigact.sa_handler = SignalHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    (void)sigaction(SIGCHLD, &sigact, 0);
}

// -----------------------------------------------------------------------------

// Print selected comptests using a format compatible with Main_Read
// to string OUT.
void atf_comp::Main_Print(cstring &out, bool show_output, bool show_testcase) {
    ind_beg(_db_zd_sel_comptest_curs,comptest,_db) {
        if (show_testcase) {
            out << "comptest " << comptest.comptest;
            if (comptest.c_targs) {
                out <<" " <<comptest.c_targs->args;
            }
            out << eol;
            out << "comment "<<comptest.comment<<eol;
            if (comptest.ncore > 1) {
                out << "ncore " << comptest.ncore << eol;
            }
            out << "timeout "<<comptest.timeout<<eol;
            out << "repeat "<<comptest.repeat<<eol;
            out << "memcheck "<<comptest.memcheck<<eol;
            out << "coverage "<<comptest.coverage<<eol;
            out << "exit_code "<<comptest.exit_code<<eol;
            if (comptest.c_tfilt && ch_N(comptest.c_tfilt->filter)) {
                out << "filter "<<comptest.c_tfilt->filter<<eol;
            }
        }
        ind_beg(comptest_zd_tmsg_curs,tmsg,comptest) {
            if (dir_Get(tmsg) == atfdb_Msgdir_msgdir_in) {
                out << tmsg.msg << eol;
            }
        }ind_end;
        if (show_output) {
            tempstr fname=DirFileJoin("test/atf_comp",comptest.comptest);
            out << eol;
            out << "# Expected output below ("<<fname<<")\n";
            ind_beg(algo::FileLine_curs,line,fname) {
                out << "# "<<line <<eol;
            }ind_end;
        }
        out << eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Merge outputs of multiple atf_cov runs into a single report.
void atf_comp::Main_Coverage() {
    if (atf_comp::_db.cmdline.cfg != dev_Cfg_cfg_coverage) {
        // not applicable
    } else if (algo_lib::_db.exit_code != 0) {
        prlog("atf_comp: coverage: skipping collection of coverage stats");
    } else {
        cstring dir = DirFileJoin(atf_comp::_db.tempdir,"cov.d");
        CreateDirRecurse(dir);
        command::atf_cov_proc atf_cov;
        atf_cov.cmd.covdir = dir;
        atf_cov.cmd.logfile = DirFileJoin(atf_cov.cmd.covdir,"atf_cov.log");
        algo::ListSep ls(":");
        ind_beg(_db_covdir_curs,covdir,_db) {
            atf_cov.cmd.mergepath << ls << covdir.covdir;
        }ind_end;
        atf_cov.cmd.gcov = true;
        atf_cov.cmd.ssim = true;
        atf_cov.cmd.report = true;
        atf_cov.cmd.check = _db.cmdline.covcheck;
        atf_cov.cmd.capture = _db.cmdline.covcapture;
        prlog("atf_comp: "<<atf_cov_ToCmdline(atf_cov));
        int rc=atf_cov_Exec(atf_cov);
        // show summary
        SysCmd(tempstr()<<"cat "<<DirFileJoin(dir,"summary.txt"));
        // show errors
        SysCmd(tempstr()<<"grep success:N "<<atf_cov.cmd.logfile);
        vrfy(rc==0,tempstr()<<"atf_cov exited with code "<<rc);
    }
}

// -----------------------------------------------------------------------------

// Rewrite / normalize component tests
// - Rewrite type tags in all tmsgs
//   If a tmsg corresponds to an unknown type, the message is deleted
// - Rewrite "abc | xyz" as
//   targs abc
//   tfilt xyz
// First, we want to see the true output of abc (the component test), prior to filtering
// Second, xyz will "eat" the non-zero exit code of abc and we don't want to use 'set -o pipefail' tricks
// TODO: also detect errors in command line for subcommand
void atf_comp::Main_Normalize() {
    ind_beg(_db_zd_sel_comptest_curs,comptest,_db) {
        int rank=0;
        ind_beg(comptest_zd_tmsg_curs,tmsg,comptest) {
            tmsg.tmsg=atfdb::Tmsg_Concat_comptest_rank_dir(comptest.comptest,100000+rank*10,dir_Get(tmsg));
            rank++;
            comptest.need_write=true;
            _db.nchange++;
            if (tmsg.istuple) {
                bool skip_dflt=false;
                tempstr result = lib_ctype::NormalizeSsimTuple(tmsg.msg, skip_dflt);
                tmsg.msg = result;
            }
        }ind_end;
        if (comptest.c_targs) {
            tempstr oldargs = GetArgs(comptest);
            tempstr oldfilter = GetFilter(comptest);
            tempstr newfilter(algo::Trimmed(Pathcomp(oldargs,"|LR")));
            if (newfilter == "") {
                newfilter=oldfilter;
            }
            if (newfilter != "") {
                atf_comp::FTfilt *tfilt=comptest.c_tfilt;
                if (!tfilt) {
                    tfilt=&tfilt_Alloc();
                    tfilt->comptest=comptest.comptest;
                    tfilt_XrefMaybe(*tfilt);
                }
            }
            if (comptest.c_tfilt) {
                comptest.c_tfilt->filter = Trimmed(newfilter);
            }
            comptest.c_targs->args = algo::Trimmed(Pathcomp(oldargs,"|LL"));
            comptest.need_write = GetArgs(comptest) != oldargs || GetFilter(comptest) != oldfilter;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Write all testcases from list zd_out_comptest back
// to where they were read from
void atf_comp::Main_Write() {
    if (_db.nchange>0) {
        _db.report.nwrite=comptest_N();
        atf_comp::SaveTuples(_db.cmdline.in);
        command::acr acr;
        acr.print=false;
        acr.write=true;
        acr.report=false;
        acr.query="atfdb.%";
        acr.in=_db.cmdline.in;
        // hack to work around sorting issue
        SysCmd(acr_ToCmdline(acr));
    }
}

// -----------------------------------------------------------------------------

// Select comptests matching regx from command line
// If using insert mode (-i), deselect any testcases not inserted from stdin
// If no comptests selected, print warning
void atf_comp::Main_Select() {
    ind_beg(_db_comptest_curs,comptest,_db) {
        atf_comp::_db.report.ntest++;
        bool match = Regx_Match(_db.cmdline.comptest, comptest.comptest);
        // skip tests marked memcheck:N in memcheck mode
        if (_db.cmdline.memcheck) {
            match = match && (comptest.memcheck || _db.cmdline.force);
        }
        // skip tests marked coverage:N in coverage mode
        if (_db.cmdline.cfg == dev_Cfg_cfg_coverage) {
            match = match && (comptest.coverage || _db.cmdline.force);
        }
        if (match) {
            zd_sel_comptest_Insert(comptest);
        } else {
            atf_comp::_db.report.nskip++;
        }
    }ind_end;
    if (_db.cmdline.i) {
        ind_beg(_db_comptest_curs,comptest,_db) {
            if (!comptest.need_write) {
                zd_sel_comptest_Remove(comptest);
            }
        }ind_end;
    }
    if (zd_sel_comptest_N()==0) {
        prerr("atf_comp.select_warning"
              <<Keyval("comptest", atf_comp::_db.cmdline.comptest)
              <<Keyval("comment", "no tests match regx. See 'acr comptest' for a complete list of runnable tests"));
    }
    _db.report.nselect = zd_sel_comptest_N();
}

// -----------------------------------------------------------------------------

void atf_comp::Main_Debug() {
    ind_beg(_db_zd_sel_comptest_curs,comptest,_db) {
        PrepareInput(comptest);
        command::mdbg_proc mdbg;
        mdbg.cmd.target = target_Get(comptest);
        if (comptest.c_targs) {
            args_Alloc(mdbg.cmd) <<" " <<comptest.c_targs->args;
        }
        args_Alloc(mdbg.cmd) << " <"<<comptest.file_test_in;
        b_Alloc(mdbg.cmd) = _db.cmdline.b;
        int rc=mdbg_Exec(mdbg);
        algo_lib::_db.exit_code=rc;
        if (atf_comp::zd_sel_comptest_N()>1) {
            prlog("atf_comp.mdbg_sel"
                  <<Keyval("comment","Skipping remaining testcases"));
        }
        break;
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_comp::Main_RewriteCmdline() {
    vrfy(!(_db.cmdline.mdbg && _db.cmdline.covcapture)
         || (_db.cmdline.mdbg && _db.cmdline.covcheck)
         ,"-mdbg flag is incompatible with coverage");

    int n_tools = _db.cmdline.memcheck + _db.cmdline.callgrind;
    vrfy(n_tools<=1,"-memcheck and -callgrind are mutually exclusive");

    // in mdbg mode, mdbg will handle the build
    if (_db.cmdline.mdbg) {
        _db.cmdline.cfg=dev_Cfg_cfg_debug;
    }
    if (_db.cmdline.capture || _db.cmdline.covcapture) {
        _db.cmdline.run=true;
    }
    if (_db.cmdline.e && !(_db.cmdline.capture || _db.cmdline.covcapture)) {
        _db.cmdline.run=false;
    }
    if (_db.cmdline.covcapture || _db.cmdline.covcheck) {
        _db.cmdline.build=true;
        _db.cmdline.cfg=dev_Cfg_cfg_coverage;
    }
    if (_db.cmdline.normalize || _db.cmdline.print || _db.cmdline.printinput|| _db.cmdline.mdbg) {
        _db.cmdline.report=false;// will mess up the print
        _db.cmdline.e=false; // doesn't make sense
        _db.cmdline.run = false;
    }
    if (_db.cmdline.maxjobs==0) {
        if (_db.cmdline.cfg==dev_Cfg_cfg_coverage) {
            _db.cmdline.maxjobs = 1;
        } else {
            _db.cmdline.maxjobs = sysconf(_SC_NPROCESSORS_ONLN);
        }
    }
    // if invoked as atf_comp -minrepeat 1000, then set both to 1000
    i32_UpdateMax(_db.cmdline.maxrepeat, _db.cmdline.minrepeat);
    // disable repeat when calculating coverage or doing slow checking
    if (_db.cmdline.memcheck || _db.cmdline.callgrind || _db.cmdline.cfg == dev_Cfg_cfg_coverage) {
        _db.cmdline.minrepeat = i32_Min(1, _db.cmdline.minrepeat);
        _db.cmdline.maxrepeat = 1;
    }
    i32_UpdateMax(_db.cmdline.maxjobs,1);
    // print potentially new command line
    // #AL# I think the options -covcapture, -covcheck should go away
    // In -cfg:coverage mode, the coverage information should be considered part of the test
    // And -cfg:coverage -capture should capture coverage information.
    // In other words, coverage is just a subset of run, a type of output, when cfg=coverage
    // (in the same way as -memcheck is a subset of -run)
    if (_db.cmdline.report) {
        prlog("begin.atf_comp"
              <<Keyval("comptest",_db.cmdline.comptest.expr)
              <<Keyval("build",Bool(_db.cmdline.build))
              <<Keyval("cfg",_db.cmdline.cfg)
              <<Keyval("run",Bool(_db.cmdline.run))
              <<Keyval("maxjobs",_db.cmdline.maxjobs)
              <<Keyval("maxrepeat",_db.cmdline.maxrepeat)
              <<Keyval("capture",Bool(_db.cmdline.capture))
              <<Keyval("edit",Bool(_db.cmdline.e))
              <<Keyval("verbose",Bool(algo_lib::_db.cmdline.verbose)));
    }
}

// -----------------------------------------------------------------------------

void atf_comp::Main() {
    lib_ctype::Init();
    Main_RewriteCmdline();
    // select directory from which executables are launched.
    Main_GuessBindir();
    Main_SetupSigchild();
    // check lock file
    algo_lib::FLockfile lockfile;
    if (_db.cmdline.run || _db.cmdline.capture || _db.cmdline.normalize) {
        vrfy(LockFileInit(lockfile, "temp/atf_comp.lock", FailokQ(true)),
             tempstr("atf_comp.lock")
             <<Keyval("success","N")
             <<Keyval("comment","another instance of atf_comp is already running"));
        vrfy_(StartsWithQ(_db.tempdir, "temp/"));// safety
        algo::RemDirRecurse(atf_comp::_db.tempdir,false);// clear entire state
        algo::CreateDirRecurse(atf_comp::_db.tempdir);// start over
    }
    if (_db.cmdline.check_untracked) {
        tempstr untracked_files;
        CheckUntrackedFiles(untracked_files);
        vrfy(untracked_files == algo::strptr(""), tempstr()<<"Cannot begin testing: Untracked files present: "<<untracked_files);
    }
    Main_Ood();
    if (_db.cmdline.i) {
        Main_Read(algo::Fildes(0));
    }
    Main_Select();
    if (_db.cmdline.normalize) {
        Main_Normalize();
    } else if (_db.cmdline.print || _db.cmdline.printinput) {
        cstring out;
        Main_Print(out,algo_lib::_db.cmdline.verbose && _db.cmdline.print,_db.cmdline.print);
        prlog(out);
    } else if (_db.cmdline.mdbg) {
        Main_Debug();
    }
    if (_db.cmdline.run) {
        // schedule function to invoke next selected test
        hook_Set0(_db.th_runtest, StartNextTest);
        ThScheduleIn(_db.th_runtest, algo::SchedTime(0));
        atf_comp::MainLoop();
        // set non-zero exit code before coverage calculation
        if (atf_comp::_db.report.npass < atf_comp::_db.report.nrun) {
            algo_lib::_db.exit_code=1;
        }
        atf_comp::Main_Coverage();
    }
    // report failed tests at the end
    ind_beg(_db_comptest_curs,comptest,_db) {
        if (comptest.start.value != 0 && !comptest.success) {
            prlog("atf_comp.testfailed"
                  <<Keyval("comptest",comptest.comptest));
        }
    }ind_end;
    if (_db.cmdline.e) {
        // running the testcases destroyed zd_sel_comptest list, recreate it
        Main_Select();
        Main_EditSelected();
    }
    // Write all testcases out
    if (_db.cmdline.write) {
        Main_Write();
    }
    atf_comp::_db.report.success = atf_comp::_db.report.npass==atf_comp::_db.report.nrun;
    if (_db.cmdline.report) {
        prlog(atf_comp::_db.report);
    }
}
