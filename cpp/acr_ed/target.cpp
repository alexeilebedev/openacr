// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- Script generator for common dev tasks
// Exceptions: yes
// Source: cpp/acr_ed/target.cpp -- Create, delete, rename target
//

#include "include/acr_ed.h"
#include "include/lib_git.h"


void acr_ed::edaction_Create_Target() {
    prlog("acr_ed.create_target  target:"<<acr_ed::_db.cmdline.target);
    algo_lib::Replscope R;
    Set(R, "$target", acr_ed::_db.cmdline.target);
    Set(R, "$nstype", acr_ed::_db.cmdline.nstype);

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

    // namespace
    acr_ed::_db.out_ssim<< dmmeta::Ns(acr_ed::_db.cmdline.target
                                      , acr_ed::_db.cmdline.nstype
                                      , dev_License_license_GPL
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
                                           , algo::Comment()) << eol;
    }
    // don't emit 'main' for libraries
    if (is_exe) {
        acr_ed::_db.out_ssim<<dmmeta::Main(acr_ed::_db.cmdline.target, false/*ismodule*/)<<eol;
    }
    // generate C++ code
    acr_ed::_db.out_ssim<< dmmeta::Nscpp(acr_ed::_db.cmdline.target, algo::Comment()) << eol;

    // tuples for abt to build this executable
    if (is_exe || is_lib || is_protocol) {
        dev::Target target;
        target.target="$target";
        Ins(&R, acr_ed::_db.out_ssim, tempstr()<<target);
    }
    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:cpp/$target/$target.cpp  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:include/$target.h  comment:''");
    }

    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:txt/$nstype/$target/README.md  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.readme  gitfile:txt/$nstype/$target/README.md  comment:''");

    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:cpp/gen/$target_gen.cpp  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:include/gen/$target_gen.h  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:include/gen/$target_gen.inl.h  comment:''");
    if (is_exe) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.gitfile  gitfile:bin/$target  comment:''");
    }

    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$target/cpp/$target/$target.cpp  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$target/include/$target.h  comment:''");
    }

    Set(R, "$gentarget", is_ssimdb ? "lib_prot" : "$target");
    Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$gentarget/cpp/gen/$target_gen.cpp  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$gentarget/include/gen/$target_gen.h  comment:''");
    Ins(&R, acr_ed::_db.out_ssim, "dev.targsrc  targsrc:$gentarget/include/gen/$target_gen.inl.h  comment:''");

    if (is_exe || is_lib) {
        Ins(&R, acr_ed::_db.out_ssim, "dev.targdep  targdep:$target.algo_lib  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dev.targdep  targdep:$target.lib_prot  comment:''");
        // add FDb and a command line
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.ctype  ctype:$target.FDb  comment:''");
        Ins(&R, acr_ed::_db.out_ssim, "dmmeta.field  field:$target.FDb._db  arg:$target.FDb  reftype:Global  dflt:''  comment:''  ");
    }

    if (is_exe) {
        dmmeta::Ctype ctype;
        ctype.ctype = "command.$target";
        Ins(&R, acr_ed::_db.out_ssim, tempstr() << ctype);

        dmmeta::Cfmt cfmt;
        cfmt.cfmt = "command.$target.Argv";
        cfmt.printfmt = "Tuple";
        cfmt.print = true;
        cfmt.read = true;
        cfmt.genop = true;
        Ins(&R, acr_ed::_db.out_ssim, tempstr() << cfmt);

        dmmeta::Field field;
        field.field = "$target.FDb.cmdline";
        field.arg = ctype.ctype;
        field.reftype = dmmeta_Reftype_reftype_Val;
        Ins(&R, acr_ed::_db.out_ssim, tempstr() << field);

        dmmeta::Fcmdline fcmdline;
        fcmdline.field=field.field;
        fcmdline.basecmdline="algo_lib.FDb.cmdline";
        fcmdline.read=true;
        Ins(&R, acr_ed::_db.out_ssim, tempstr() << fcmdline);

        dmmeta::Field in;
        in.field="command.$target.in";
        in.arg="algo.cstring";
        in.reftype=dmmeta_Reftype_reftype_Val;
        in.dflt.value = "\"data\"";
        in.comment.value = "Input directory or filename, - for stdin";
        Ins(&R, acr_ed::_db.out_ssim, tempstr() << in);

        dmmeta::Floadtuples floadtuples;
        floadtuples.field=in.field;
        Ins(&R, acr_ed::_db.out_ssim, tempstr() << floadtuples);
    }
    if (is_ssimdb) {
        dmmeta::Nsdb nsdb;
        nsdb.ns="$target";
        Ins(&R, acr_ed::_db.out_ssim, tempstr()<<nsdb);
    }
    if (is_exe || is_lib) {
        // sample header
        Ins(&R, acr_ed::_db.script, "mkdir -p cpp/$target/");
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
        Ins(&R, acr_ed::_db.script, "cat > cpp/$target/$target.cpp << EOF");
        Ins(&R, acr_ed::_db.script, "#include \"include/algo.h\"");
        InsertSrcfileInclude(R,false);
        if (is_exe) {
            Ins(&R, acr_ed::_db.script, "void $target::Main() {");
            Ins(&R, acr_ed::_db.script, "    prlog(\"Hello, World!\");");
            Ins(&R, acr_ed::_db.script, "    $target::MainLoop();");
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
        Ins(&R, acr_ed::_db.script,"git add cpp/$target/$target.cpp");
        Ins(&R, acr_ed::_db.script,"git add include/$target.h");
    }
    Ins(&R, acr_ed::_db.script,"git add cpp/gen/$target_gen.cpp");
    Ins(&R, acr_ed::_db.script,"git add include/gen/$target_gen.h");
    Ins(&R, acr_ed::_db.script,"git add include/gen/$target_gen.inl.h");

    Ins(&R, acr_ed::_db.script, "mkdir -p txt/$nstype/$target/");
    Ins(&R, acr_ed::_db.script,"touch txt/$nstype/$target/README.md");
    Ins(&R, acr_ed::_db.script,"git add txt/$nstype/$target/README.md");

    // update headers
    {
        command::src_hdr src_hdr;
        src_hdr.targsrc.expr=Subst(R,"$target/%");
        src_hdr.write=true;
        Ins(&R, acr_ed::_db.script,src_hdr_ToCmdline(src_hdr));
    }
    // compile it
    if (is_exe || is_lib) {
        command::abt abt;
        abt.install=true;
        abt.target.expr=acr_ed::_db.cmdline.target;
        Ins(&R, acr_ed::_db.script,abt_ToCmdline(abt));
        Ins(&R, acr_ed::_db.script, "echo 'please execute $(acr_compl -install) to add completions support for new target'");
    }
    {// update documentation
        command::abt_md abt_md;
        abt_md.ns.expr=acr_ed::_db.cmdline.target;
        Ins(&R, acr_ed::_db.script,abt_md_ToCmdline(abt_md));
    }
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Rename_Target() {
    prlog("acr_ed.rename_target"
          <<Keyval("target",acr_ed::_db.cmdline.target)
          <<Keyval("rename",acr_ed::_db.cmdline.rename));
    acr_ed::FTarget *target = acr_ed::ind_target_Find(acr_ed::_db.cmdline.target);
    vrfy(target, tempstr()<<"acr_ed.invalid_target"
         <<Keyval("target",acr_ed::_db.cmdline.target));
    acr_ed::FNs *ns = target->p_ns;
    algo_lib::Replscope R;
    Set(R, "$oldtarg", acr_ed::_db.cmdline.target);
    Set(R, "$newtarg", acr_ed::_db.cmdline.rename);

    {
        command::acr acr;
        acr.query << "ns:"<<acr_ed::_db.cmdline.target;
        acr.rename << acr_ed::_db.cmdline.rename;
        acr.write=true;
        acr.report=false;
        acr.x=true;// ssimreqs
        Ins(&R, acr_ed::_db.script, acr_ToCmdline(acr));
    }

    // create directories for new target
    Ins(&R, acr_ed::_db.script, "mkdir -p cpp/$newtarg");

    // move all headers and source files into directories for the new target
    // NOTE: `acr ns:$oldtarg -rename $newtarg` has already renamed the targsrcs,
    // they are all registered under the $newtarg name.
    Ins(&R, acr_ed::_db.script, "# rename targsrc records");
    algo_lib::Regx generated;
    Regx_ReadSql(generated, "%/gen/%", false);
    cstring script2;
    ind_beg(acr_ed::target_zd_targsrc_curs,targsrc,*target) {
        cstring src(src_Get(targsrc));
        Set(R,"$src",src);
        cstring newsrc(src);

        // replace old target name with new target name in the filename portion of the newsrc
        tempstr newdir(GetDirName(src));
        tempstr newfname(StripDirName(src));
        Replace(newfname,_db.cmdline.target,_db.cmdline.rename);
        // rewrite directory
        // headers go into include
        // sources go into cpp/$newtarg
        // generated files are already in place
        bool header = EndsWithQ(src,".h");
        if (!Regx_Match(generated,src)) {
            newdir = tempstr() << (header  ? "include/":"cpp/$newtarg/");
        }
        newsrc = DirFileJoin(newdir,newfname);
        Set(R,"$newsrc",newsrc);

        // rewrite #include's first
        if (header) {
            ind_beg(acr_ed::target_zd_targsrc_curs,thistargsrc,*target) {
                Ins(&R, acr_ed::_db.script, tempstr()<<"sed -i "<<algo::strptr_ToBash("s!#include \"$src\"!#include \"$newsrc\"!g")<<" "<<src_Get(thistargsrc));
            }ind_end;
        }
        // this goes into a temporary variable and gets added to the main script later
        Ins(&R, script2, "bin/acr targsrc:$newtarg/$src -rename $newtarg/$newsrc -write -report:N");
        if (src != newsrc) {
            Ins(&R, script2, "git mv $src $newsrc");
        }
        Ins(&R, script2, "sed -i 's/$oldtarg::/$newtarg::/g' $newsrc");
        Ins(&R, script2, "sed -i 's/namespace $oldtarg/namespace $newtarg/g' $newsrc");
    }ind_end;
    acr_ed::_db.script << script2;

    // TODO: rename md file
    // rename md file -- speculative
    // {
    //     command::acr acr;
    //     acr.query << "readme:txt/"<<acr_ed::_db.cmdline.target<<".md";
    //     acr.rename << "txt/"<<acr_ed::_db.cmdline.rename<<".md";
    //     acr.write=true;
    //     acr.g=true;// issue git mv
    //     acr.report=false;
    //     Ins(&R, acr_ed::_db.script, acr_ToCmdline(acr));
    // }

    if (ns->nstype == dmmeta_Nstype_nstype_exe) {
        Ins(&R, acr_ed::_db.script, "ln -sf ../build/release/$newtarg bin/$oldtarg");
        if (_db.cmdline.target != _db.cmdline.rename) {
            Ins(&R, acr_ed::_db.script, "git mv bin/$oldtarg bin/$newtarg");
        }
    }
    Ins(&R, acr_ed::_db.script, "# update remaining gitfile records");
    Ins(&R, acr_ed::_db.script, "bin/update-gitfile");
    Ins(&R, acr_ed::_db.script, "bin/update-hdr");// update header contents
    acr_ed::NeedAmc();
    Ins(&R, acr_ed::_db.script, "echo 'please execute $(acr_compl -install) to update completions support for renamed target'");
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Delete_Target() {
    {
        command::acr acr;
        acr.query << "ns:"<<acr_ed::_db.cmdline.target;
        acr.del=true;
        acr.write=true;
        acr.tree=true;
        acr.x=true;// all related files
        acr.g=true;// issue 'git rm'
        acr_ed::_db.script << acr_ToCmdline(acr) << eol;
    }
    acr_ed::_db.script << "bin/src_hdr -write" << eol;
    acr_ed::_db.script << "bin/abt_md ''" << eol;
    acr_ed::_db.script << "echo 'please execute $(acr_compl -install) to remove completions support for deleted target'" << eol;
    acr_ed::NeedAmc();
}
