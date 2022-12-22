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
// Source: cpp/acr/ed/target.cpp -- Create, delete, rename target
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr_ed.h"

// -----------------------------------------------------------------------------

// Pick target directory name based on target name:
// samp_xyz -> cpp/samp/
// tut1 -> cpp/
static tempstr PickTargdir(strptr target) {
    tempstr targdir;
    targdir << "cpp";
    strptr prefix = Pathcomp(target, "_RL");
    if (prefix != "") {
        targdir << "/" << prefix;
    }
    return targdir;
}

// -----------------------------------------------------------------------------

void acr_ed::Main_CreateTarget() {
    prlog("acr_ed.create_target  target:"<<acr_ed::_db.cmdline.target);
    algo_lib::Replscope R;
    Set(R, "$target", acr_ed::_db.cmdline.target);

    // replace _ with / in arg for composite names
    // this maps "src_func" -> "src/func"
    strptr prefix = Pathcomp(acr_ed::_db.cmdline.target, "_LL");
    if (prefix == dmmeta_Nstype_nstype_lib) {
        acr_ed::_db.cmdline.nstype = dmmeta_Nstype_nstype_lib;
    }
    bool is_exe = acr_ed::_db.cmdline.nstype == dmmeta_Nstype_nstype_exe;
    bool is_lib = acr_ed::_db.cmdline.nstype == dmmeta_Nstype_nstype_lib;
    bool is_protocol = acr_ed::_db.cmdline.nstype == dmmeta_Nstype_nstype_protocol;
    bool is_ssimdb = acr_ed::_db.cmdline.nstype == dmmeta_Nstype_nstype_ssimdb;
    Set(R, "$targdir", PickTargdir(_db.cmdline.target));

    // namespace
    acr_ed::_db.out_ssim<< dmmeta::Ns(acr_ed::_db.cmdline.target
                                      , acr_ed::_db.cmdline.nstype
                                      , algo::Comment(acr_ed::_db.cmdline.comment)) << eol;

    if (is_protocol) {
        acr_ed::_db.out_ssim<< dmmeta::Nsproto(acr_ed::_db.cmdline.target
                                               , algo::Comment()) << eol;
    }
    if (is_exe || is_lib) {
        acr_ed::_db.out_ssim<< dmmeta::Nsx(acr_ed::_db.cmdline.target
                                           , true/*genthrow*/
                                           , true/*correct_getorcreate*/
                                           , "algo_lib.FDb.malloc"
                                           , true/*sortxref*/
                                           , /*pack*/false
                                           , /*field offsets*/false
                                           , algo::Comment()) << eol;
    }
    // don't emit 'main' for libraries
    if (is_exe) {
        acr_ed::_db.out_ssim<<dmmeta::Main(acr_ed::_db.cmdline.target, false/*ismodule*/)<<eol;
    }

    // tuples for abt to build this executable
    if (is_exe || is_lib || is_protocol) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.target  target:$target");
    }
    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:$targdir/$target.cpp  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:include/$target.h  comment:''");
    }

    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:cpp/gen/$target_gen.cpp  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:include/gen/$target_gen.h  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:include/gen/$target_gen.inl.h  comment:''");
    if (is_exe) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:bin/$target  comment:''");
    }

    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$target/$targdir/$target.cpp  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$target/include/$target.h  comment:''");
    }

    Set(R, "$gentarget", is_ssimdb ? "lib_prot" : "$target");
    Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$gentarget/cpp/gen/$target_gen.cpp  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$gentarget/include/gen/$target_gen.h  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$gentarget/include/gen/$target_gen.inl.h  comment:''");

    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.targdep  targdep:$target.algo_lib  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dev.targdep  targdep:$target.algo_pch  comment:''");
        // add FDb and a command line
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.ctype  ctype:$target.FDb  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.field  field:$target.FDb._db  arg:$target.FDb  reftype:Global  dflt:''  comment:''  ");
    }

    if (is_exe) {
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.ctype  ctype:command.$target  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.cfmt  cfmt:command.$target.Argv  printfmt:Tuple  print:Y  read:Y  sep:''  genop:N  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.field  field:$target.FDb.cmdline  arg:command.$target  reftype:Val  dflt:''  comment:''  ");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.fcmdline  field:$target.FDb.cmdline  read:Y  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.field  field:command.$target.in  arg:algo.cstring  reftype:Val  dflt:'\"data\"'  comment:'Input directory or filename, - for stdin'");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.floadtuples  field:command.$target.in  comment:''");
    }
    if (is_ssimdb) {
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.nsdb  ns:$target  comment:''");
    }
    if (is_exe || is_lib) {
        // sample header
        Ins(&R, acr_ed::_db.script, "mkdir -p $targdir/");
        Ins(&R, acr_ed::_db.script, "cat > include/$target.h << EOF");
        Ins(&R, acr_ed::_db.script, "#include \"include/gen/$target_gen.h\"");
        Ins(&R, acr_ed::_db.script, "#include \"include/gen/$target_gen.inl.h\"");
        Ins(&R, acr_ed::_db.script, "");
        Ins(&R, acr_ed::_db.script, "namespace $target { // update-hdr");
        Ins(&R, acr_ed::_db.script, "}");
        Ins(&R, acr_ed::_db.script, "EOF");
        Ins(&R, acr_ed::_db.script, "");
    }
    if (is_ssimdb) {
        Ins(&R, acr_ed::_db.script, "mkdir -p data/$target/");
    }
    // sample source file
    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.script, "cat > $targdir/$target.cpp << EOF");
        Ins(&R, acr_ed::_db.script, "#include \"include/algo.h\"");
        InsertSrcfileInclude(R,false);
        if (is_exe) {
            Ins(&R, acr_ed::_db.script, "void $target::Main() {");
            Ins(&R, acr_ed::_db.script, "    prlog(\"Hello, World!\");");
            Ins(&R, acr_ed::_db.script, "}");
        }
        Ins(&R, acr_ed::_db.script, "EOF");
        Ins(&R, acr_ed::_db.script, "");
    }

    Ins(&R, acr_ed::_db.script,"bin/amc  # make sure */gen/* files are created");
    acr_ed::_db.need_amc=false; // already did it
    if (is_exe) {
        Ins(&R, acr_ed::_db.script,"ln -s ../build/release/$target bin/$target");
        Ins(&R, acr_ed::_db.script,"git add -f bin/$target");
    }
    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.script,"git add $targdir/$target.cpp");
        Ins(&R, acr_ed::_db.script,"git add include/$target.h");
    }
    Ins(&R, acr_ed::_db.script,"git add cpp/gen/$target_gen.cpp");
    Ins(&R, acr_ed::_db.script,"git add include/gen/$target_gen.h");
    Ins(&R, acr_ed::_db.script,"git add include/gen/$target_gen.inl.h");

    Ins(&R, acr_ed::_db.script,"bin/update-gitfile");
    Ins(&R, acr_ed::_db.script,"bin/src_hdr -targsrc:$target/% -write");
    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.script,"bin/update-hdr");
        Ins(&R, acr_ed::_db.script,"bin/abt -install $target");
    }
}

// -----------------------------------------------------------------------------

void acr_ed::Main_RenameTarget() {
    prlog("acr_ed.rename_target"
          <<Keyval("target",acr_ed::_db.cmdline.target)
          <<Keyval("rename",acr_ed::_db.cmdline.rename));
    acr_ed::FNs *ns = acr_ed::ind_ns_Find(acr_ed::_db.cmdline.target);
    vrfy(ns, tempstr()<<"acr_ed.invalid_target"
         <<Keyval("target",acr_ed::_db.cmdline.target));
    algo_lib::Replscope R;
    Set(R, "$$", "$", false);
    Set(R, "$oldtarg", acr_ed::_db.cmdline.target);
    Set(R, "$newtarg", acr_ed::_db.cmdline.rename);
    Set(R, "$olddir", Pathcomp(acr_ed::_db.cmdline.target,"_LL"));
    Set(R, "$newdir", Pathcomp(acr_ed::_db.cmdline.rename,"_LL"));

    // we access the filesystem here to deal with files which reside
    // at top level under cpp/
    Set(R, "$oldfname", DirectoryQ(Subst(R,"cpp/$olddir")) ? "cpp/$olddir/$oldtarg" : "cpp/$oldtarg");
    Set(R, "$newfname", DirectoryQ(Subst(R,"cpp/$newdir")) ? "cpp/$newdir/$newtarg" : "cpp/$newtarg");

    Ins(&R, acr_ed::_db.script, "bin/acr ctype:command.$oldtarg -rename command.$newtarg  -write -report:N");
    Ins(&R, acr_ed::_db.script, "bin/acr ctype:report.$oldtarg -rename report.$newtarg  -write -report:N");
    Ins(&R, acr_ed::_db.script, "bin/acr target:$oldtarg -rename $newtarg -write -report:N");
    Ins(&R, acr_ed::_db.script, "bin/acr ns:$oldtarg -rename $newtarg -write -report:N");

    Ins(&R, acr_ed::_db.script, "# rename targsrc records");
    // note -- the original name now has $newtarg because acr has recursively updated target name
    Ins(&R, acr_ed::_db.script, "bin/acr targsrc:$newtarg/$oldfname.cpp -rename $newtarg/$newfname.cpp -write -report:N");
    Ins(&R, acr_ed::_db.script, "bin/acr targsrc:$newtarg/cpp/gen/$oldtarg_gen.cpp -rename $newtarg/cpp/gen/$newtarg_gen.cpp -write -report:N");

    Ins(&R, acr_ed::_db.script, "# rename source files");
    Ins(&R, acr_ed::_db.script, "git mv -f include/gen/$oldtarg_gen.h include/gen/$newtarg_gen.h");
    Ins(&R, acr_ed::_db.script, "git mv -f include/gen/$oldtarg_gen.inl.h include/gen/$newtarg_gen.inl.h");
    Ins(&R, acr_ed::_db.script, "git mv -f cpp/gen/$oldtarg_gen.cpp cpp/gen/$newtarg_gen.cpp");
    Ins(&R, acr_ed::_db.script, "git mv -f $oldfname.cpp $newfname.cpp");

    if (ns->nstype == dmmeta_Nstype_nstype_exe) {
        Ins(&R, acr_ed::_db.script, "# move soft link");
        Ins(&R, acr_ed::_db.script, "L=$$(readlink bin/$oldtarg); ln -sf $${L/$oldtarg/$newtarg} bin/$newtarg");
        Ins(&R, acr_ed::_db.script, "git rm bin/$oldtarg");
    }
    Ins(&R, acr_ed::_db.script, "# update remaining gitfile records");
    Ins(&R, acr_ed::_db.script, "bin/update-gitfile");
    acr_ed::NeedAmc();
    Ins(&R, acr_ed::_db.script, "# renaming of $oldtarg -> $newtarg complete");
    Ins(&R, acr_ed::_db.script, "# please edit $newfname.cpp manually");
    Ins(&R, acr_ed::_db.script, "# If you are sure that no corruption will result, you can run");
    Ins(&R, acr_ed::_db.script, "# ");
    Ins(&R, acr_ed::_db.script, "#     sed -i s/$oldtarg::/$newtarg::/g $newfname.cpp");
}

// -----------------------------------------------------------------------------

void acr_ed::Main_DeleteTarget() {
    prlog("acr_ed.delete_target"
          <<Keyval("target",acr_ed::_db.cmdline.target));
    algo_lib::Replscope R;
    Set(R, "$target", acr_ed::_db.cmdline.target);
    Set(R, "$$", "$");

    // remove all sources & gitfiles associated with this target
    Ins(&R, acr_ed::_db.script, "for X in $$(acr targsrc:$target/% -field:src)"
        "; do acr gitfile:$$X -del -write; git rm -f $$X"
        "; done");
    Ins(&R, acr_ed::_db.script, "bin/acr ns:$target -del -write");
    Ins(&R, acr_ed::_db.script, "bin/acr ctype:command.$target -del -write");
    Ins(&R, acr_ed::_db.script, "bin/acr ctype:report.$target -del -write");
    Ins(&R, acr_ed::_db.script, "bin/acr target:$target -del -write");
    Ins(&R, acr_ed::_db.script, "git rm -f bin/$target");
    Ins(&R, acr_ed::_db.script, "bin/update-gitfile");
    Ins(&R, acr_ed::_db.script, "bin/src_hdr -write");
    acr_ed::NeedAmc();
}
