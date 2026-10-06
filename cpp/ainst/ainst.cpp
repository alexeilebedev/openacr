// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: ainst (exe) -- Install third-party software described by dev.extpkg
// Exceptions: yes
// Source: cpp/ainst/ainst.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Fill the selection list from the command line, in the extpkg table's order,
// which is sorted by name.
static void SelectExtpkg() {
    ind_beg(ainst::_db_extpkg_curs,extpkg,ainst::_db) {
        if (Regx_Match(ainst::_db.cmdline.extpkg,extpkg.extpkg)) {
            ainst::zd_sel_extpkg_Insert(extpkg);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Run SCRIPT through bash, or print what would run.
// The script is written to a file first so a failure names a line the reader
// can look at, which a script fed to bash on a pipe cannot do.
static void RunScript(algo::strptr script) {
    tempstr path;
    path << ainst::_db.cmdline.outdir << "/ainst.sh";
    CreateDirRecurse(Pathcomp(path,"/RL"));
    algo::StringToFile(script,path);
    tempstr cmd;
    cmd << "bash " << algo::strptr_ToBash(path);
    SysCmd(cmd,algo::FailokQ(false),algo::DryrunQ(ainst::_db.cmdline.dry_run),algo::EchoQ(true));
}

// -----------------------------------------------------------------------------

// Select the installations, then do the one thing the command line asked for.
// The actions share their whole body: each is the same generated script,
// printed whole or as its function library alone, run, or run under a
// buildroot and packaged.
void ainst::Main() {
    SelectExtpkg();
    tempstr script;
    if (ainst::_db.cmdline.update) {
        ainst::UpdateExtpkg();
    } else if (ainst::_db.cmdline.rpm) {
        ainst::WriteRpmScript(script);
    } else if (ainst::_db.cmdline.lib) {
        ainst::WriteFunc(script);
    } else {
        ainst::WriteScript(script);
    }
    if (ainst::_db.cmdline.update) {
        // UpdateExtpkg has already said what moved
    } else if (ainst::_db.cmdline.script || ainst::_db.cmdline.lib) {
        prlog_(script);
    } else if (ainst::_db.cmdline.install || ainst::_db.cmdline.rpm) {
        RunScript(script);
    } else {
        ind_beg(ainst::_db_zd_sel_extpkg_curs,extpkg,ainst::_db) {
            dev::Extpkg extpkg_out;
            extpkg_CopyOut(extpkg,extpkg_out);
            prlog(extpkg_out);
        }ind_end;
    }
}
