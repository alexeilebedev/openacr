// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: acr_ed (exe) -- Script generator for common dev tasks
// Exceptions: yes
// Source: cpp/acr_ed/srcfile.cpp -- Create, delete, rename source file
//

#include "include/acr_ed.h"
#include "include/gen/dev_gen.h"

// -----------------------------------------------------------------------------

// Find a target which has the most source files
// in the same directory as srcfile
static acr_ed::FTarget *GuessTarget(strptr srcfile) {
    ind_beg(acr_ed::_db_targsrc_curs,targsrc,acr_ed::_db) {
        if (GetDirName(srcfile) == GetDirName(src_Get(targsrc))) {
            targsrc.p_target->score++;
        }
    }ind_end;
    acr_ed::FTarget *best_target = NULL;
    int best_score = 0;
    ind_beg(acr_ed::_db_target_curs,target,acr_ed::_db) {
        if (i32_UpdateMax(best_score,target.score)) {
            best_target = &target;
        }
    }ind_end;
    return best_target;
}

// -----------------------------------------------------------------------------

// Create cpp, script, h or readme file
void acr_ed::edaction_Create_Srcfile() {
    vrfy(DirectoryQ(GetDirName(acr_ed::_db.cmdline.srcfile))
         , tempstr()<<"acr_ed.baddir"
         <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile));

    bool readme = Pathcomp(acr_ed::_db.cmdline.srcfile,"/LL") == "txt";
    bool cpp = GetFileExt(acr_ed::_db.cmdline.srcfile) == ".cpp";
    bool h = GetFileExt(acr_ed::_db.cmdline.srcfile) == ".h";
    bool script = StartsWithQ(acr_ed::_db.cmdline.srcfile, "bin/");
    bool cpp_or_h = cpp || h;
    bool need_target = cpp_or_h;

    if (need_target) {
        if (acr_ed::_db.cmdline.target == "") {
            if (acr_ed::FTarget *target = GuessTarget(acr_ed::_db.cmdline.srcfile)) {
                acr_ed::_db.cmdline.target = target->target;
            }
        }
        vrfy(acr_ed::_db.cmdline.target != ""
             && acr_ed::ind_target_Find(acr_ed::_db.cmdline.target)
             , tempstr()<<"acr_ed.badtarget"
             <<Keyval("target",acr_ed::_db.cmdline.target));
    }

    prlog("acr_ed.create_srcfile"
          <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile)
          <<Keyval("target",acr_ed::_db.cmdline.target));

    algo_lib::Replscope R;
    Set(R, "$target", acr_ed::_db.cmdline.target);
    Set(R, "$basename", StripDirName(acr_ed::_db.cmdline.srcfile));
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);

    // create targsrc, readme record
    if (cpp_or_h) {
        dev::Targsrc targsrc;
        targsrc.targsrc = tempstr()<<acr_ed::_db.cmdline.target<<"/"<<acr_ed::_db.cmdline.srcfile;
        targsrc.comment = algo::Comment(acr_ed::_db.cmdline.comment);
        acr_ed::_db.out_ssim << targsrc << eol;
    }

    // create file, insert some content
    // but only if file doesn't exist
    if (!FileQ(_db.cmdline.srcfile)) {
        acr_ed::_db.script<<"cat > "<<acr_ed::_db.cmdline.srcfile<<" << EOF"<<eol;
        if (cpp_or_h) {
            bool mainheader = StripExt(StripDirName(acr_ed::_db.cmdline.srcfile)) == acr_ed::_db.cmdline.target;
            InsertSrcfileInclude(R, mainheader);
        }
        if (readme) {
            Ins(&R, acr_ed::_db.script, "## $srcfile");
        }
        if (script) {
            Ins(&R, acr_ed::_db.script, "#!/usr/bin/env bash");
        }
        Ins(&R, acr_ed::_db.script, "EOF");
    }

    // update file copyright headers
    RegisterFile(Subst(R,"$srcfile"),_db.cmdline.comment);
    if (cpp_or_h) {
        Ins(&R, acr_ed::_db.script, "bin/src_hdr -targsrc:$target/% -write -update_copyright");
    }
    if (script) {
        Ins(&R, acr_ed::_db.script, "bin/src_hdr -scriptfile:$srcfile -write -update_copyright");
        Ins(&R, acr_ed::_db.script, "bin/atf_ci update_script -check_clean:N");
        acr_ed::RegisterFile(Subst(R,"txt/script/$basename.md"), "");
    }
    ScriptEditFile(R,acr_ed::_db.cmdline.srcfile);
}

// -----------------------------------------------------------------------------

// Rename cpp, h, or readme file
void acr_ed::edaction_Rename_Srcfile() {
    // if not target is specified, guess one from the target directory
    if (acr_ed::_db.cmdline.target == "") {
        if (acr_ed::FTarget *target = GuessTarget(acr_ed::_db.cmdline.rename)) {
            acr_ed::_db.cmdline.target = target->target;
        }
    }
    algo_lib::Replscope R;
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);
    Set(R, "$to", acr_ed::_db.cmdline.rename);

    // reassign source file to a new target, if one was specified on the command
    // line or guessed
    acr_ed::FTarget *target=acr_ed::ind_target_Find(acr_ed::_db.cmdline.target);
    if (target) {
        Set(R, "$target", target->target);
        Ins(&R, acr_ed::_db.script, "acr targsrc:%/$srcfile -rename $target/$to -write");
    }

    vrfy(FileQ(acr_ed::_db.cmdline.srcfile),
         tempstr()<<"acr_ed.nosrcfile"
         <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile)
         <<Keyval("comment","source file not found"));

    vrfy(DirectoryQ(GetDirName(acr_ed::_db.cmdline.rename))
         , tempstr()<<"acr_ed.baddir"
         <<Keyval("dirname",GetDirName(acr_ed::_db.cmdline.rename)));

    Ins(&R, acr_ed::_db.script, "acr gitfile:$srcfile -rename:$to -write -print:N -g");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -write");
}

// -----------------------------------------------------------------------------

// Delete cpp,h, or readme file
void acr_ed::edaction_Delete_Srcfile() {
    vrfy(FileQ(acr_ed::_db.cmdline.srcfile),
         tempstr()<<"acr_ed.nosrcfile"
         <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile)
         <<Keyval("comment","source file not found"));

    command::acr acr;
    acr.query << "dev.gitfile:"<<acr_ed::_db.cmdline.srcfile;
    acr.del=true;
    acr.write=true;
    acr.g=true;
    acr_ed::_db.script <<  acr_ToCmdline(acr) << eol;
    acr_ed::_db.script << "bin/src_hdr -write" << eol;
}
