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
// Target: sandbox (exe)
// Exceptions: yes
// Source: cpp/sandbox.cpp
//

#include "include/sandbox.h"

// -----------------------------------------------------------------------------

void sandbox::ResetSandbox(sandbox::FSandbox &sandbox) {
    prerr("sandbox.reset"
          <<Keyval("sandbox",sandbox.sandbox)
          <<Keyval("dir",sandbox.dir));
    // use git clone
    // shallow repos (such as used in CI jobs) cannot use --local.
    if (!algo::FileObjectExistsQ(sandbox.dir)) {
        bool shallow=algo::Trimmed(algo::SysEval("git rev-parse --is-shallow-repository",FailokQ(true),1024))=="true";
        SysCmd(tempstr()<<"git clone "<<(shallow ? "" : "--local")<<" . "<<sandbox.dir<<" 2>/dev/null");
    }
    tempstr curdir=algo::GetCurDir();
    int rc=SysCmd(tempstr()
                  <<"git -C "<<sandbox.dir<<" fetch "<<curdir<<" -q"
                  // checkout -B may fail if directory non-clean
                  <<" && "
                  <<"git -C "<<sandbox.dir<<" reset --hard -q"
                  <<" && "
                  <<"git -C "<<sandbox.dir<<" clean --exclude=build --exclude=.gcache -fxq ."
                  <<" && "
                  <<"git -C "<<sandbox.dir<<" checkout -B baseline $(git-current-ref) -q");
    if (rc!=0) {
        prlog("reset failed, sandbox looks unrelated to current git dir. re-initializing");
        algo::RemDirRecurse(sandbox.dir,true);
        rc=SysCmd(tempstr()<<"git clone --local . "<<sandbox.dir, FailokQ(false));
    }
    // files/directories to clone to the subdirectory
    tempstr gitfiles;
    ind_beg(_db_sbpath_curs,sbpath,_db) {
        gitfiles<<" "<<sbpath.sbpath;
    }ind_end;
    // rsync modified files into sandbox
    SysCmd(tempstr()<<"rsync --relative --ignore-missing-args -ac $(git ls-files -m) "<<gitfiles<<" "<<sandbox.dir<<"/", FailokQ(false));
    // make sure the git repo has no unstaged changes
    SysCmd(tempstr()<<"git -C "<<sandbox.dir<<" commit --allow-empty -m baseline -q 2>/dev/null", FailokQ(false));
}

// -----------------------------------------------------------------------------

void sandbox::AcrApply(algo::strptr script, algo::strptr op) {
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"sandbox");
    StringToFile(script, tempfile.filename);

    command::acr_proc acr;
    if (op=="-merge") {
        acr.cmd.merge=true;
    }
    if (op=="-insert") {
        acr.cmd.insert=true;
    }
    if (op=="-del") {
        acr.cmd.del=true;
    }
    acr.cmd.write=true;
    acr.fstdin << "<"<<tempfile.filename;
    acr_ExecX(acr);
}

// -----------------------------------------------------------------------------

void sandbox::ListSandbox() {
    algo_lib::FTxttbl txttbl;
    AddCol(txttbl, "Sandbox");
    AddCol(txttbl, "Size");
    AddCol(txttbl, "Clean");
    AddCol(txttbl, "Path");
    AddCol(txttbl, "Comment");
    ind_beg(_db_sandbox_curs,sandbox,_db) {
        tempstr size;
        if (FileObjectExistsQ(sandbox.dir)) {
            tempstr cmd;
            cmd<<"du -sh "<<sandbox.dir<<" | awk '{print $1}' 2>/dev/null";
            size << Trimmed(SysEval(cmd,FailokQ(true),1024*10));
        } else {
            size << "N/A";
        }
        tempstr clean;
        {
            tempstr cmd;
            cmd<<"git -C "<<sandbox.dir<<" ls-files -m 2>/dev/null";
            clean << ((Trimmed(SysEval(cmd,FailokQ(true),1024*10))=="") ? "Y" : "N");
        }
        AddRow(txttbl);
        AddCell(txttbl) << sandbox.sandbox;
        AddCell(txttbl) << size;
        AddCell(txttbl) << clean;
        AddCell(txttbl) << sandbox.dir;
        AddCell(txttbl) << sandbox.comment;
    }ind_end;
    prlog(txttbl);
}

// -----------------------------------------------------------------------------

void sandbox::Main() {
    if (_db.cmdline.create) {
        _db.cmdline.reset=true;
    }
    if (_db.cmdline.create) {
        tempstr script;
        dev::Sandbox sandbox;
        sandbox.sandbox=_db.cmdline.name.expr;
        script << sandbox << eol;
        if (sandbox_InsertMaybe(sandbox), "insert failed") {
            AcrApply(script,"-insert");
        } else {
            // sandbox may already exist, not a problem
        }
    }
    int nsel=0;
    ind_beg(_db_sandbox_curs,sandbox,_db) {
        sandbox.select=Regx_Match(_db.cmdline.name,sandbox.sandbox);
        sandbox.dir = algo_lib::SandboxDir(sandbox.sandbox);
        nsel += sandbox.select;
    }ind_end;
    if (nsel == 0) {
        _db.cmdline.list=true;
        algo_lib::_db.exit_code=1;
        ind_beg(_db_sandbox_curs,sandbox,_db) {
            sandbox.select=true;
        }ind_end;
    }
    if (_db.cmdline.list) {
        ListSandbox();
    }
    // early exit
    if (algo_lib::_db.exit_code!=0) {
        return;
    }
    if (_db.cmdline.reset) {
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            ResetSandbox(sandbox);
        }ind_end;
    }
    if (cmd_N(_db.cmdline)) {
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            algo_lib::SandboxEnter(sandbox.sandbox);
            tempstr fullcmd;
            ind_beg(command::sandbox_cmd_curs,cmd,_db.cmdline) {
                fullcmd<<" "<<strptr_ToBash(cmd);
            }ind_end;
            // remember exit code
            int rc=SysCmd(fullcmd);
            if (rc!=0) {
                algo_lib::_db.exit_code=rc;
            }
            algo_lib::SandboxExit();
        }ind_end;
    }
    if (_db.cmdline.diff) {
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            algo_lib::SandboxEnter(sandbox.sandbox);
            SysCmd("git diff");
            algo_lib::SandboxExit();
        }ind_end;
    }
    if (_db.cmdline.clean) {
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            algo_lib::SandboxEnter(sandbox.sandbox);
            SysCmd(tempstr()<<"git reset --hard -q");
            SysCmd(tempstr()<<"git clean -df . -q");
            algo_lib::SandboxExit();
        }ind_end;
    }
    if (_db.cmdline.gc) {
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            algo_lib::SandboxEnter(sandbox.sandbox);
            SysCmd(tempstr()<<"git gc");
            algo_lib::SandboxExit();
        }ind_end;
    }
    if (_db.cmdline.del) {
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            SysCmd(tempstr()<<"rm -rf "<<sandbox.dir);
            tempstr script;
            dev::Sandbox sbox;
            sbox.sandbox=_db.cmdline.name.expr;
            script << "acr.delete "<<sbox << eol;
            AcrApply(script,"-insert");
        }ind_end;
    }
}
