// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr_ed/srcfile.cpp -- Create, delete, rename source file
//

#include "include/acr_ed.h"

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

// Create cpp, h or readme file
void acr_ed::edaction_Create_Srcfile() {
    vrfy(DirectoryQ(GetDirName(acr_ed::_db.cmdline.srcfile))
         , tempstr()<<"acr_ed.baddir"
         <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile));

    bool readme = Pathcomp(acr_ed::_db.cmdline.srcfile,"/LL") == "txt";
    bool cpp_or_h = !readme;
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
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);

    // create gitfile record
    acr_ed::_db.out_ssim << dev::Gitfile(acr_ed::_db.cmdline.srcfile)<<eol;

    // create targsrc, readme record
    if (cpp_or_h) {
        dev::Targsrc targsrc;
        targsrc.targsrc = tempstr()<<acr_ed::_db.cmdline.target<<"/"<<acr_ed::_db.cmdline.srcfile;
        targsrc.comment = algo::Comment(acr_ed::_db.cmdline.comment);
        acr_ed::_db.out_ssim << targsrc << eol;
    }
    if (readme) {
        dev::Readme r;
        r.inl=false;
        r.gitfile = acr_ed::_db.cmdline.srcfile;
        r.comment.value = acr_ed::_db.cmdline.comment;
        acr_ed::_db.out_ssim << r << eol;
    }

    // create source file, insert some content
    Ins(&R, acr_ed::_db.script, "cat > $srcfile << EOF");
    if (cpp_or_h) {
        bool mainheader = StripExt(StripDirName(acr_ed::_db.cmdline.srcfile)) == acr_ed::_db.cmdline.target;
        InsertSrcfileInclude(R, mainheader);
    }
    if (readme) {
        Ins(&R, acr_ed::_db.script, "## $srcfile");
    }
    Ins(&R, acr_ed::_db.script, "EOF");

    // updateh file copyright headers
    Ins(&R, acr_ed::_db.script, "git add $srcfile");
    if (cpp_or_h) {
        Ins(&R, acr_ed::_db.script, "bin/src_hdr -tarsrc:$target/% -write");
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

    prlog("acr_ed.rename_srcfile"
          <<Keyval("from",acr_ed::_db.cmdline.srcfile)
          <<Keyval("to",acr_ed::_db.cmdline.rename)
          <<Keyval("target",acr_ed::_db.cmdline.target));

    algo_lib::Replscope R;
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);
    Set(R, "$to", acr_ed::_db.cmdline.rename);

    // reassign source file to a new target, if one was specified on the command
    // line or guessed
    acr_ed::FTarget *target=acr_ed::ind_target_Find(acr_ed::_db.cmdline.target);
    if (target) {
        Set(R, "$target", target->target);
        Ins(&R, acr_ed::_db.script, "acr -targsrc:%/$srcfile -rename $target/$srcfile -rename");
    }

    vrfy(FileQ(acr_ed::_db.cmdline.srcfile),
         tempstr()<<"acr_ed.nosrcfile"
         <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile)
         <<Keyval("comment","source file not found"));

    vrfy(DirectoryQ(GetDirName(acr_ed::_db.cmdline.rename))
         , tempstr()<<"acr_ed.baddir"
         <<Keyval("dirname",GetDirName(acr_ed::_db.cmdline.rename)));

    Ins(&R, acr_ed::_db.script, "acr gitfile:$srcfile -rename:$to -write -print:N");
    Ins(&R, acr_ed::_db.script, "git mv $srcfile $to");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -write");
}

// -----------------------------------------------------------------------------

// Delete cpp,h, or readme file
void acr_ed::edaction_Delete_Srcfile() {
    prlog("acr_ed.delete_srcfile"
          <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile));

    algo_lib::Replscope R;
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);
    vrfy(FileQ(acr_ed::_db.cmdline.srcfile),
         tempstr()<<"acr_ed.nosrcfile"
         <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile)
         <<Keyval("comment","source file not found"));

    Ins(&R, acr_ed::_db.script, "acr gitfile:$srcfile -del -write");
    Ins(&R, acr_ed::_db.script, "git rm -f $srcfile");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -write");
}
