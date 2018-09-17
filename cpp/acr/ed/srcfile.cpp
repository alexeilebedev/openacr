// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr/ed/srcfile.cpp -- Create, delete, rename source file
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr_ed.h"


// -----------------------------------------------------------------------------

static void RequireSrcfileDir(strptr srcfile) {
    vrfy(DirectoryQ(GetDirName(srcfile))
         , tempstr()<<"acr_ed.baddir"
         <<Keyval("srcfile",srcfile));
}

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

void acr_ed::Main_CreateSrcfile() {
    if (acr_ed::_db.cmdline.target == "") {
        if (acr_ed::FTarget *target = GuessTarget(acr_ed::_db.cmdline.srcfile)) {
            prlog("acr_ed.guess_target"
                  <<Keyval("target",target->target));
            acr_ed::_db.cmdline.target = target->target;
        }
    }
    prlog("acr_ed.create_srcfile"
          <<Keyval("target",acr_ed::_db.cmdline.target));

    algo_lib::Replscope R;
    Set(R, "$target", acr_ed::_db.cmdline.target);
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);
    vrfy(acr_ed::_db.cmdline.target != ""
         && acr_ed::ind_target_Find(acr_ed::_db.cmdline.target)
         , tempstr()<<"acr_ed.badtarget"
         <<Keyval("target",acr_ed::_db.cmdline.target));

    RequireSrcfileDir(acr_ed::_db.cmdline.srcfile);

    // create gitfile
    acr_ed::_db.out_ssim << dev::Gitfile(acr_ed::_db.cmdline.srcfile)<<eol;

    // create targsrc
    tempstr targsrc;
    targsrc<<acr_ed::_db.cmdline.target<<"/"<<acr_ed::_db.cmdline.srcfile;
    acr_ed::_db.out_ssim << dev::Targsrc(targsrc,algo::Comment(acr_ed::_db.cmdline.comment))<<eol;

    // create source file
    Ins(&R, acr_ed::_db.script, "cat > $srcfile << EOF");
    bool mainheader = StripExt(StripDirName(acr_ed::_db.cmdline.srcfile)) == acr_ed::_db.cmdline.target;
    InsertSrcfileInclude(R, mainheader);
    Ins(&R, acr_ed::_db.script, "EOF");

    // updateh file copyright headers
    Ins(&R, acr_ed::_db.script, "git add $srcfile");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -targsrc:$target/% -write");
}

// -----------------------------------------------------------------------------

void acr_ed::Main_RenameSrcfile() {
    prlog("acr_ed.rename_srcfile"
          <<Keyval("from",acr_ed::_db.cmdline.srcfile)
          <<Keyval("to",acr_ed::_db.cmdline.rename));

    algo_lib::Replscope R;
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);
    Set(R, "$to", acr_ed::_db.cmdline.rename);
    RequireSrcfileDir(acr_ed::_db.cmdline.srcfile);

    Ins(&R, acr_ed::_db.script, "acr gitfile:$srcfile -rename:$to -write -print:N");
    Ins(&R, acr_ed::_db.script, "git mv $srcfile $to");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -write");
}

// -----------------------------------------------------------------------------

void acr_ed::Main_DeleteSrcfile() {
    prlog("acr_ed.delete_srcfile"
          <<Keyval("srcfile",acr_ed::_db.cmdline.srcfile));

    algo_lib::Replscope R;
    Set(R, "$srcfile", acr_ed::_db.cmdline.srcfile);
    RequireSrcfileDir(acr_ed::_db.cmdline.srcfile);

    Ins(&R, acr_ed::_db.script, "acr gitfile:$srcfile -del -write");
    Ins(&R, acr_ed::_db.script, "git rm -f $srcfile");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -write");
}
