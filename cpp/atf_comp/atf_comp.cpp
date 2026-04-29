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
// Source: cpp/atf_comp/atf_comp.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

// Compare or capture test log against reference file.
// Apply tfilt filter once on the complete log if specified.
static bool CheckOutput(atf_comp::FComptest &comptest) {
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
    bool ok = true;
    u8 mode = atf_comp::_db.cmdline.mode;
    if (mode == command_atf_comp_mode_capture) {
        bool is_new = !FileQ(refpath);
        algo::StringToFile(filtered, refpath);
        if (is_new) {
            SysCmd(tempstr() << "git add " << strptr_ToBash(refpath), FailokQ(true), DryrunQ(false));
            atf_comp::_db.n_capture++;
        }
        prlog("atf_comp.capture"<<Keyval("file", refpath));
    } else {
        // write to temp, diff against reference
        tempstr tmppath;
        tmppath << "temp/atf_comp." << comptest.comptest;
        algo::StringToFile(filtered, tmppath);
        tempstr diffcmd;
        diffcmd << "diff --color=always " << strptr_ToBash(refpath) << " " << strptr_ToBash(tmppath);
        tempstr diff = SysEval(diffcmd, FailokQ(true), 1024*1024);
        if (ch_N(diff) > 0) {
            prlog(diff);
            ok = false;
        }
        DeleteFile(tmppath);
    }
    return ok;
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
    bool coverage = mode == command_atf_comp_mode_covcheck || mode == command_atf_comp_mode_covcapture;
    if (coverage) {
        CoverageSetup();
    }
    if (mode == command_atf_comp_mode_mdbg) {
        _db.cmdline.cfg = dev_Cfg_cfg_debug;
    }
    // build non-release configs before running
    if (_db.cmdline.cfg != dev_Cfg_cfg_release) {
        command::abt_proc abt;
        Regx_ReadSql(abt.cmd.target, "%", true);
        abt.cmd.cfg.expr = _db.cmdline.cfg;
        abt.cmd.install = false;// just build
        abt_ExecX(abt);
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
        vrfy(zd_select_N() == 1, tempstr() << "expected exactly one comptest, got " << zd_select_N());
        atf_comp::FComptest &comptest = *zd_select_First();
        command::acr_proc acr;
        acr.cmd.query = tempstr() << "comptest:" << comptest.comptest;
        acr.cmd.t = true;
        acr.cmd.e = true;
        acr_ExecX(acr);
    } else if (mode == command_atf_comp_mode_editsource) {
        vrfy(zd_select_N() == 1, tempstr() << "expected exactly one comptest, got " << zd_select_N());
        atf_comp::FComptest &comptest = *zd_select_First();
        command::src_func_proc src_func;
        tempstr funcname(comptest.comptest);
        Translate(funcname,".","_");
        src_func.cmd.func.expr = tempstr() << "atf_comp.comptest_"<<funcname;
        src_func.cmd.e = true;
        src_func.cmd.list = true;
        src_func.cmd.showloc = true;
        SysCmd(src_func_ToCmdline(src_func));
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
    } else if (mode == command_atf_comp_mode_mdbg) {
        vrfy(zd_select_N() == 1, tempstr() << "expected exactly one comptest, got " << zd_select_N());
        atf_comp::FComptest &comptest = *zd_select_First();
        command::mdbg_proc mdbg;
        mdbg.cmd.target = "atf_comp";
        mdbg.cmd.cfg = _db.cmdline.cfg;
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
            tempstr tempdir;
            tempdir << "temp/atf_comp/" << comptest.comptest;
            algo::RemDirRecurse(tempdir, true);
            algo::CreateDirRecurse(tempdir);
            SetVar("tempdir", tempdir);
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
                ok = CheckOutput(comptest);
                if (!ok) {
                    fail_reason = "output mismatch";
                }
            }
            double dur = algo::ElapsedSecs(t0, algo::CurrSchedTime());
            int exit_code = 0;
            int nlines = 0;
            ind_beg(atf_comp::_db_proc_curs, proc, atf_comp::_db) {
                exit_code = proc.status;
            }ind_end;
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
                  <<Keyval("exit_code", exit_code)
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
        if (coverage && algo_lib::_db.exit_code == 0) {
            CoverageFinalize();
        }
    }
}
