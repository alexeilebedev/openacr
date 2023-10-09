// Copyright (C) 2023 AlgoRND
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
// Target: atf_fuzz (exe)
// Exceptions: yes
// Source: cpp/atf/atf_fuzz.cpp
//

#include "include/atf_fuzz.h"

void atf_fuzz::fuzzstrat_skip_inputs() {
    srandom(_db.cmdline.seed);
    if (_db.cmdline.inputfile == "") {
        _db.cmdline.inputfile = "temp/atf_fuzz.in";
        command::acr_in_proc acr_in;
        acr_in.cmd.ns.expr = _db.cmdline.target;
        acr_in.cmd.data=true;
        acr_in.fstdout << ">"<<_db.cmdline.inputfile;
        vrfy(acr_in_Exec(acr_in)==0, "acr_in failed");
        prlog("atf_fuzz: prepared input file using acr_in, "<<_db.cmdline.inputfile);
    }
    cstring inputs=FileToString(_db.cmdline.inputfile);
    int nline=0;
    ind_beg(Line_curs,line,inputs) {
        (void)line;
        nline++;
    }ind_end;
    prlog("atf_fuzz.skip_inputs.begin"
          <<Keyval("inputfile",_db.cmdline.inputfile)
          <<Keyval("reprofile",_db.cmdline.reprofile)
          <<Keyval("nline",nline));
    DeleteFile(_db.cmdline.reprofile);
    int ncrash=0;
    {
        command::sandbox_proc sandbox;
        sandbox.cmd.name.expr = dev_Sandbox_sandbox_atf_fuzz;
        sandbox.cmd.reset = true;
        sandbox_ExecX(sandbox);
    }
    tempstr sbpath = algo_lib::SandboxDir(dev_Sandbox_sandbox_atf_fuzz);
    int iter=0;
    ind_beg(Line_curs,line,inputs) {
        if (algo::double_WeakRandom(1) <= _db.cmdline.testprob) {
            prlog(iter+1<<" of "<<nline);
            command::bash_proc sh;
            sh.cmd.c<<"cd "<<sbpath
                    <<" && echo "<<strptr_ToBash(tempstr()<<"acr.delete "<<line)
                    <<" | acr -insert -write >/dev/null"
                    <<" && "<<_db.cmdline.target<<" "<<_db.cmdline.args;
            // in verbose mode, see subcommand output
            sh.fstdout=">temp/atf_fuzz.childout";
            sh.fstderr=">&1";
            (void)command::bash_Exec(sh);
            if (algo_lib::_db.cmdline.verbose) {
                tempstr str;
                InsertIndent(str,algo::FileToString("temp/atf_fuzz.childout"),4);
                verblog(str);
            }
            if (WIFSIGNALED(sh.status) && (WTERMSIG(sh.status) == SIGSEGV)) {
                ncrash++;
                prlog("CRASH "<<sh.cmd.c);
                StringToFile(sh.cmd.c,_db.cmdline.reprofile,algo_FileFlags_append);
            }
            command::sandbox_proc cleanup;
            cleanup.cmd.name.expr = dev_Sandbox_sandbox_atf_fuzz;
            cleanup.cmd.clean = true;
            sandbox_ExecX(cleanup);
        }
        iter++;
    }ind_end;
    prlog("atf_fuzz.skip_inputs.end"
          <<Keyval("ncases",nline)
          <<Keyval("ncrash",ncrash)
          <<Keyval("success",ncrash==0)
          <<Keyval("reprofile",_db.cmdline.reprofile));
}

void atf_fuzz::Main() {
    _db.c_target=ind_target_Find(_db.cmdline.target);
    vrfy(_db.c_target, tempstr()
         <<"atf_fuzz.badtarget"
         <<Keyval("target",_db.cmdline.target)
         <<Keyval("comment","Target not found"));
    ind_beg(_db_fuzzstrat_curs,fuzzstrat,_db) {
        if (Regx_Match(_db.cmdline.fuzzstrat,fuzzstrat.fuzzstrat)) {
            prlog("Apply strategy "<<fuzzstrat.fuzzstrat<<" to target "<<_db.c_target->target);
            fuzzstrat.step();
        }
    }ind_end;
}
