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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/atf_comp.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

// Diff filtered output against reference file, print the colored diff,
// and return true if the files differ.  The single producer/consumer of
// the `diff` shell invocation in this file.
static bool ShowRefDiff(strptr refpath, strptr filtered, strptr comptest_name) {
    tempstr tmppath;
    tmppath << "temp/atf_comp." << comptest_name << ".diff";
    algo::StringToFile(filtered, tmppath);
    tempstr diffcmd;
    diffcmd << "diff --color=always " << strptr_ToBash(refpath) << " " << strptr_ToBash(tmppath);
    tempstr diff = SysEval(diffcmd, FailokQ(true), 1024*1024);
    DeleteFile(tmppath);
    bool changed = ch_N(diff) > 0;
    if (changed) {
        prlog(diff);
    }
    return changed;
}

// Compare or capture test log against reference file, and name the reason the
// comparison failed; an empty reason is a pass.  The reason is this function's
// to state because it is the only one that knows which of the two ways a
// comparison fails happened.
// Apply tfilt filter once on the complete log if specified.
static tempstr CheckOutput(atf_comp::FComptest &comptest) {
    tempstr filtered;
    if (comptest.c_tfilt && ch_N(comptest.c_tfilt->filter) > 0) {
        tempstr tmpfile;
        tmpfile << "temp/atf_comp." << comptest.comptest << ".unfilt";
        algo::StringToFile(atf_comp::_db.log, tmpfile);
        tempstr cmd;
        cmd << "( " << comptest.c_tfilt->filter << " ) < " << strptr_ToBash(tmpfile);
        filtered = SysEval(cmd, FailokQ(true), 1024*1024);
        algo::DeleteFile(tmpfile);
    } else {
        filtered = atf_comp::_db.log;
    }
    filtered = tempstr() << Trimmed(filtered) << "\n";
    // reference file path
    tempstr refpath;
    refpath << "test/atf_comp/" << comptest.comptest;
    tempstr fail_reason;
    u8 mode = atf_comp::_db.cmdline.mode;
    if (mode == command_atf_comp_mode_capture) {
        bool is_new = !FileQ(refpath);
        bool changed = false;
        if (!is_new) {
            // diff existing reference vs new content before overwriting,
            // so the user can see what's being captured
            changed = ShowRefDiff(refpath, filtered, comptest.comptest);
        }
        algo::StringToFile(filtered, refpath);
        if (is_new) {
            SysCmd(tempstr() << "git add " << strptr_ToBash(refpath), FailokQ(true), DryrunQ(false));
            atf_comp::_db.n_capture++;
        }
        prlog("atf_comp.capture"
              <<Keyval("file", refpath)
              <<Keyval("status", is_new ? "new" : changed ? "changed" : "unchanged"));
    } else if (!FileQ(refpath)) {
        // A comptest whose reference file was never committed compares its
        // output against nothing.  `diff` names the missing file on stderr and
        // writes nothing to stdout, and an empty stdout is what "no difference"
        // looks like, so the test passes for as long as the file stays absent.
        // That is not a mismatch: it is a test that has never been verified, so
        // it is reported as its own kind of failure.
        prlog("atf_comp.noref"
              <<Keyval("comptest", comptest.comptest)
              <<Keyval("file", refpath)
              <<Keyval("comment", "capture the reference with -capture"));
        fail_reason << "reference file absent";
    } else if (ShowRefDiff(refpath, filtered, comptest.comptest)) {
        fail_reason << "output mismatch";
    }
    return fail_reason;
}

// Reclaim /dev/shm segments left behind by an earlier cluster, and say so only
// when there were any.
//
// A comptest that times out has its cluster killed, and a killed process unlinks
// nothing, so its segments hold their share of the tmpfs until some later process
// sweeps.  The next test that wants a large topology then starts with that much
// less room and fails for a reason that belongs to the test before it, which is
// how one failure becomes several.  The sweep unlinks only segments whose writer
// or creator is gone, so a cluster running elsewhere on the host keeps its own.
static void SweepShm() {
    tempstr out;
    out << Trimmed(SysEval("amsspy -clean", FailokQ(true), 1024));
    if (ch_N(out) > 0 && !EndsWithQ(out, "removed:0")) {
        prlog(out);
    }
}

// Print the log a failed comptest accumulated, under the name of that comptest.
static void ShowLog(atf_comp::FComptest &comptest) {
    prlog("atf_comp.log"<<Keyval("comptest", comptest.comptest));
    prlog(Trimmed(atf_comp::_db.log));
}

// Return C++ function name for comptest, e.g. "atf_comp::comptest_acr_BadInsert"
static tempstr ComptestFuncname(strptr comptest_name) {
    tempstr funcname;
    funcname << "atf_comp::comptest_" << comptest_name;
    Replace(funcname, ".", "_");
    Replace(funcname, "-", "_");
    return funcname;
}

void atf_comp::Main() {
    algo::CreateDirRecurse("temp");
    if (_db.cmdline.capture) {
        _db.cmdline.mode = command_atf_comp_mode_capture;
    }
    if (_db.cmdline.ee) {
        _db.cmdline.mode = command_atf_comp_mode_editsource;
    }
    if (_db.cmdline.e) {
        _db.cmdline.mode = command_atf_comp_mode_edit;
    }
    u8 mode = _db.cmdline.mode;
    // -cfg:coverage (set by atf_ci's coverage cijob) selects the curated
    // coverage comptest subset; atf_ci owns the covdir and the gcov/merge.
    bool coverage = _db.cmdline.cfg == dev_Cfg_cfg_coverage;
    if (mode == command_atf_comp_mode_mdbg || mode == command_atf_comp_mode_mdbgall) {
        _db.cmdline.cfg = dev_Cfg_cfg_debug;
    }
    // select matching comptests
    ind_beg(_db_comptest_curs, comptest, _db) {
        bool match = Regx_Match(_db.cmdline.comptest, comptest.comptest);
        if (mode == command_atf_comp_mode_memcheck) {
            match = match && comptest.memcheck;
        }
        if (coverage) {
            match = match && comptest.coverage;
        }
        if (match) {
            zd_select_Insert(comptest);
        }
    }ind_end;
    vrfy(zd_select_N() > 0, tempstr() << "atf_comp.nomatch"
         << Keyval("comptest", _db.cmdline.comptest)
         << Keyval("comment", "no matching comptests"));
    if (mode == command_atf_comp_mode_edit) {
        // open all selected comptest records in the editor
        tempstr alt;
        ind_beg(_db_zd_select_curs, comptest, _db) {
            if (ch_N(alt) > 0) {
                alt << "|";
            }
            alt << comptest.comptest;
        }ind_end;
        command::acr_proc acr;
        acr.cmd.query = tempstr() << "comptest:(" << alt << ")";
        acr.cmd.t = true;
        acr.cmd.e = true;
        acr_ExecX(acr);
    } else if (mode == command_atf_comp_mode_editsource) {
        // open all selected comptest source functions in the editor
        tempstr alt;
        ind_beg(_db_zd_select_curs, comptest, _db) {
            tempstr funcname(comptest.comptest);
            Translate(funcname,".","_");
            if (ch_N(alt) > 0) {
                alt << "|";
            }
            alt << funcname;
        }ind_end;
        command::src_func_proc src_func;
        src_func.cmd.func.expr = tempstr() << "atf_comp.comptest_(" << alt << ")";
        src_func.cmd.e = true;
        src_func.cmd.list = true;
        src_func.cmd.showloc = true;
        src_func.fstderr = ">&1";
        src_func_ExecX(src_func);
    } else if (mode == command_atf_comp_mode_print || mode == command_atf_comp_mode_printinput) {
        ind_beg(_db_zd_select_curs, comptest, _db) {
            tempstr refpath;
            refpath << "test/atf_comp/" << comptest.comptest;
            tempstr content = algo::FileToString(refpath, algo::FileFlags());
            if (mode == command_atf_comp_mode_printinput) {
                ind_beg(algo::Line_curs, line, content) {
                    int pos = algo::FindStr(line, " <- ");
                    if (pos >= 0) {
                        prlog(algo::RestFrom(line, pos + 4));
                    }
                }ind_end;
            } else {
                prlog(Trimmed(content));
            }
        }ind_end;
    } else if (mode == command_atf_comp_mode_del) {
        command::acr_proc acr;
        acr.cmd.query = tempstr() << "comptest:" << _db.cmdline.comptest;
        acr.cmd.del = true;
        acr.cmd.write = true;
        acr.cmd.x = true;
        acr.cmd.g = true;
        acr_ExecX(acr);
    } else if (mode == command_atf_comp_mode_mdbg || mode == command_atf_comp_mode_mdbgall) {
        vrfy(zd_select_N() == 1, tempstr() << "expected exactly one comptest, got " << zd_select_N());
        atf_comp::FComptest &comptest = *zd_select_First();
        command::mdbg_proc mdbg;
        mdbg.cmd.target = "atf_comp";
        mdbg.cmd.cfg = _db.cmdline.cfg;
        mdbg.cmd.nonstop = mode == command_atf_comp_mode_mdbgall;
        b_Alloc(mdbg.cmd) = ComptestFuncname(comptest.comptest);
        args_Alloc(mdbg.cmd) = comptest.comptest;
        args_Alloc(mdbg.cmd) << "-cfg:"<<_db.cmdline.cfg;// ask debug atf_comp to spawn debugg child
        args_Alloc(mdbg.cmd) = "-v";// run child atf_comp in verbose mode
        prlog("atf_comp.mdbg"<<Keyval("cmd", mdbg_ToCmdline(mdbg)));
        mdbg_Exec(mdbg);
    } else {
        int nrun = 0;
        int npass = 0;
        int nerr = 0;
        // derive bindir from cfg
        tempstr bindir;
        bindir << "build/" << _db.cmdline.cfg;
        // scale timeouts for slow modes
        double timeout_scale = 1;
        if (mode == command_atf_comp_mode_memcheck) {
            timeout_scale = 30;
        } else if (mode == command_atf_comp_mode_valgrind) {
            timeout_scale = 50;
        } else if (_db.cmdline.cfg == dev_Cfg_cfg_debug || _db.cmdline.cfg == dev_Cfg_cfg_coverage) {
            timeout_scale = 4;
        }
        ind_beg(_db_zd_select_curs, comptest, _db) {
            nrun++;
            _db.c_cur_comptest = &comptest;
            proc_RemoveAll();
            algo::Refurbish(_db.R);
            ch_RemoveAll(_db.log);
            SweepShm();
            tempstr tempdir;
            tempdir << "temp/atf_comp/" << comptest.comptest;
            algo::RemDirRecurse(tempdir, true);
            algo::CreateDirRecurse(tempdir);
            SetVar("tempdir", tempdir);
            SetTestenv(timeout_scale > 1);
            SetVar("bindir", bindir);
            SetVar("comptest", comptest.comptest);
            comptest.timeout = comptest.timeout * timeout_scale;
            SetVar("timeout", tempstr() << comptest.timeout);
            Set(_db.R, "$$", "$", false);
            _db.t0 = algo::CurrSchedTime();
            algo::SchedTime t0 = _db.t0;
            prlog("atf_comp.begin"<<Keyval("comptest", comptest.comptest));
            bool ok = true;
            tempstr fail_reason;
            try {
                comptest.step();
            } catch (algo_lib::ErrorX &x) {
                prlog("atf_comp.error"
                      <<Keyval("comptest", comptest.comptest)
                      <<Keyval("comment", x.str));
                fail_reason = x.str;
                ok = false;
            }
            ProcWaitAll();
            if (ok) {
                fail_reason = CheckOutput(comptest);
                ok = ch_N(fail_reason) == 0;
            } else {
                ShowLog(comptest);
            }
            double dur = algo::ElapsedSecs(t0, algo::CurrSchedTime());
            int nlines = 0;
            ind_beg(algo::Line_curs, line, _db.log) {
                nlines++;
                (void)line;
            }ind_end;
            if (ok) {
                npass++;
            } else {
                nerr++;
                _db.fail_summary << "atf_comp.fail"
                                 << Keyval("comptest", comptest.comptest)
                                 << Keyval("reason", fail_reason) << "\n";
            }
            prlog("atf_comp.end"
                  <<Keyval("comptest", comptest.comptest)
                  <<Keyval("success", Bool(ok))
                  <<Keyval("nlines", nlines)
                  <<Keyval("duration", dur));
            if (nerr >= _db.cmdline.maxerr) {
                prlog("atf_comp.maxerr"<<Keyval("nerr", nerr)<<Keyval("maxerr", _db.cmdline.maxerr));
                break;
            }
        }ind_end;
        if (ch_N(_db.fail_summary) > 0) {
            prlog(Trimmed(_db.fail_summary));
        }
        prlog("report.atf_comp"
              <<Keyval("nrun", nrun)
              <<Keyval("npass", npass)
              <<Keyval("nerr", nerr));
        algo_lib::_db.exit_code = nerr > 0;
        if (_db.n_capture > 0) {
            SysCmd("update-gitfile", FailokQ(true), DryrunQ(false));
        }
    }
}
