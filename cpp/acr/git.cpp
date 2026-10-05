// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2024 AlgoRND
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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/git.cpp -- Git triggers
//

#include "include/algo.h"
#include "include/acr.h"

// Emit the git commands that make the worktree match the selected dev.gitfile
// rows: a deleted row's file is removed, a renamed row's file is moved, and a
// new row's file is created and staged. A file moved or created gets its
// directory first: an ssimfile renamed into a new namespace lands under a
// data directory that does not exist yet. WRITE_OK says the ssimfile write-back
// went through, and the script then runs; otherwise the script is printed.
// A nonzero script status fails the run. acr_ed's rename arrives here as an acr
// run that renames the source file's dev.gitfile row under -write and -g, and by
// the time the script runs the ssimfiles already name the destination. A git mv
// refused because the source is untracked therefore leaves the worktree holding
// the old path while the database names the new one, and the next amc or abt
// compiles against a database naming a file that does not exist. The script's
// status therefore travels out in the exit code, and the diagnostic names the
// script that failed, so a caller that checks either one can tell that run apart
// from a clean one.
void acr::Main_GitTriggers(bool write_ok) {
    acr::FSsimfile *gitfile = ind_ssimfile_Find("dev.gitfile");
    algo::cstring script;
    if (gitfile) {
        ind_beg(acr::ctype_zd_selrec_curs,selrec,*gitfile->p_ctype) {
            bool move = !selrec.del && selrec.oldpkey && *selrec.oldpkey != selrec.pkey;
            bool create = !selrec.del && !move && selrec.isnew;
            if (move || create) {
                script << "mkdir -p "<<strptr_ToBash(tempstr()<<"./"<<algo::GetDirName(selrec.pkey))<<eol;
            }
            if (selrec.del) {
                script << "git rm --force "<<strptr_ToBash(selrec.pkey)<<eol;
            } else if (move) {
                script << "git mv "<<strptr_ToBash(*selrec.oldpkey)<<" "<<strptr_ToBash(selrec.pkey)<<eol;
            } else if (create) {
                script << "touch "<<strptr_ToBash(selrec.pkey)<<eol;
                script << "git add "<<strptr_ToBash(selrec.pkey)<<eol;
            }
        }ind_end;
    }
    if (write_ok) {
        command::bash_proc bash;
        algo_lib::FTempfile tempfile;
        TempfileInitX(tempfile,"acr");
        StringToFile(script,tempfile.filename);
        bash.fstdin << "<"<<tempfile.filename;
        int rc = bash_Exec(bash);
        if (rc!=0) {
            prerr("acr.git_script"<<Keyval("script",script)
                  <<Keyval("status",algo::DescribeWaitStatus(rc))
                  <<Keyval("comment","git script failed"));
            algo_lib::_db.exit_code++;
        }
    } else {
        prlog("# acr output script");
        prlog(script);
    }
}
