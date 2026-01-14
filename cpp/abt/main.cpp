// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/main.cpp -- Algo Build Tool - Main file
//
// TODO Create entries for object files and add them as input dependencies to link step.
// Currently, the out-of-date flag propagation is unconvincing

#ifndef WIN32
#include <sys/wait.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif
#include "include/abt.h"

// -----------------------------------------------------------------------------

bool abt::HeaderExtQ(strptr ext) {
    return ext == ".h" || ext == ".hpp";
}

// -----------------------------------------------------------------------------

// Access cached 'stat' results for FNAME
abt::FFilestat &abt::GetFilestat(strptr fname) {
    abt::FFilestat *filestat=abt::ind_filestat_Find(fname);
    if (!filestat) {
        filestat =&abt::filestat_Alloc();
        vrfy(filestat, "abt.filestat_out_of_mem");
        filestat->filename=fname;
        (void)filestat_XrefMaybe(*filestat);
        struct stat S;
        if (0==stat(Zeroterm(filestat->filename),&S)) {
            filestat->modtime = ToUnTime(algo::UnixTime(S.st_mtime));
            filestat->size = S.st_size;
            filestat->isdir = S_ISDIR(S.st_mode);
            filestat->exists = true;
        }
    }
    return *filestat;
}

// -----------------------------------------------------------------------------

// how are we using this execkey???
i64 abt::execkey_Get(abt::FSyscmd &cmd) {
    return (i64(cmd.nprereq) << 32) | (i64(cmd.syscmd) & 0xffffffff);
}

// -----------------------------------------------------------------------------

bool abt::SourceQ(abt::FTargsrc &targsrc) {
    return ext_Get(targsrc)!="h";
}

// -----------------------------------------------------------------------------

static void Main_ComputeAlldep(abt::FTarget &target, abt::FTarget &parent) {
    if (c_alldep_ScanInsertMaybe(target,parent)) {
        ind_beg(abt::target_c_targdep_curs, targdep, parent) {
            Main_ComputeAlldep(target,*targdep.p_parent);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static void Main_SelectTarget() {
    // create list of targets to consider:
    // any target matching command line regex, but filtered for compatibility with
    // current configuration (uname-compiler.cfg-arch)
    ind_beg(abt::_db_target_curs, target,abt::_db) {
        if (Regx_Match(abt::_db.cmdline.target, target.target)) {
            target.origsel=true;
            abt::zs_sel_target_Insert(target);
        }
    }ind_end;

    // recursively select target dependencies; the list zs_sel_target grows as this scanning proceeds.
    for(abt::FTarget *target = abt::zs_sel_target_First(); target; target=target->zs_sel_target_next) {
        ind_beg(abt::target_c_targdep_curs, dep,*target) {
            abt::zs_sel_target_Insert(*dep.p_parent);// modifies NEXT pointer possibly!
        }ind_end;
    }

    // Check if no match occured
    if (ch_N(abt::_db.cmdline.target.expr) && abt::zs_sel_target_N() == 0) {
        prerr("abt.nomatch"
              <<Keyval("target", abt::_db.cmdline.target)
              <<Keyval("comment","No target matches specified pattern"));
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

// compute obj key by replacing path components
// with .
// So, cpp/abt/main.cpp becomes cpp.abt.main.cpp
// Next step will be to replace the extension
tempstr abt::GetObjkey(strptr source) {
    tempstr ret(source);
    Translate(ret, "/", ".");
    return ret;
}

// -----------------------------------------------------------------------------

// Compute object pathname by combinindg builddir path,
// objkey, and extension.
tempstr abt::GetObjpath(abt::FBuilddir &builddir, abt::FSrcfile &srcfile) {
    tempstr ret;
    // Replace extension
    strptr tgt_ext;
    strptr ext = GetFileExt(srcfile.srcfile);
    if (ext == ".cpp" || ext == ".c" || ext == ".cc") {
        tgt_ext = builddir.p_compiler->objext;
    } else if (ext == ".rc" && builddir.p_compiler->rc != "") {
        tgt_ext = ".res";
    }
    if (ch_N(tgt_ext)) {
        ret << builddir.path << algo::MaybeDirSep;
        if (!abt::HeaderExtQ(ext)) {
            ret << StripExt(srcfile.objkey);
        } else {
            ret << srcfile.objkey;
        }
        ret << tgt_ext;
    }
    return ret;
}

// -----------------------------------------------------------------------------

void abt::DeleteFileV(strptr path) {
    verblog("rm -f "<<path);
    DeleteFile(path);
}

// -----------------------------------------------------------------------------

// Delete object files for selected targets from all selected build dirs
static void Main_Clean() {
    ind_beg(abt::_db_builddir_curs, builddir, abt::_db) if (builddir.select) {
        ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
            abt::DeleteFileV(GetOutfile(builddir,target));
            ind_beg(abt::target_c_srcfile_curs, src,target) if (Regx_Match(abt::_db.cmdline.srcfile,src.srcfile)) {
                tempstr objpath = GetObjpath(builddir,src);
                if (ch_N(objpath)) {
                    abt::DeleteFileV(objpath);
                    if (cfg_Get(builddir) == dev_Cfg_cfg_coverage) {
                        // clean coverage files
                        tempstr gcdafile(ReplaceExt(objpath, ".gcda"));
                        tempstr gcnofile(ReplaceExt(objpath, ".gcno"));
                        abt::DeleteFileV(gcdafile);
                        abt::DeleteFileV(gcnofile);
                    }
                }
            }ind_end;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_List() {
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        bool list_targ = true;
        if (abt::_db.cmdline.ood) { // filter by out-of-date
            list_targ &= target.ood;
        }
        if (list_targ) {
            dev::Target out;
            target_CopyOut(target,out);
            prlog(out);
        }
        ind_beg(abt::target_c_srcfile_curs, src,target) if (Regx_Match(abt::_db.cmdline.srcfile, src.srcfile)) {
            if (!abt::_db.cmdline.ood || src.ood) {
                dev::Srcfile out;
                srcfile_CopyOut(src,out);
                prlog(out);
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void ListIncl(abt::FSrcfile &srcfile) {
    if (!srcfile.printed) {
        srcfile.printed=true;
        ind_beg(abt::srcfile_zd_include_curs,include,srcfile) if (algo_lib::Regx_Match(abt::_db.cmdline.srcfile,srcfile.srcfile)) {
            include.wantprint=true;
            ListIncl(*include.p_header);
        }ind_end;
    }
}

// List includes relevant to selected targets
static void Main_ListIncl() {
    ind_beg(abt::_db_zs_sel_target_curs,target,abt::_db) {// list includes for all sources of selected targets
        ind_beg(abt::target_c_targsrc_curs,targsrc,target) if (SourceQ(targsrc) && Regx_Match(abt::_db.cmdline.srcfile,src_Get(targsrc))) {
            if (abt::FSrcfile *srcfile=abt::ind_srcfile_Find(src_Get(targsrc))) {
                if (!abt::_db.cmdline.ood || srcfile->ood) {
                    ListIncl(*srcfile);
                }
            }
        }ind_end;
    }ind_end;
    // print selected includes
    ind_beg(abt::_db_include_curs,include,abt::_db) if (include.wantprint && Regx_Match(abt::_db.cmdline.srcfile,srcfile_Get(include))) {
        dev::Include out;
        abt::include_CopyOut(include,out);
        prlog(out);
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_Helpscreen() {
    tempstr all_tgts;
    int len_before=0;
    algo::ListSep ls;
    ind_beg(abt::_db_target_curs, target,abt::_db) {
        all_tgts << ls;
        if (ch_N(all_tgts) > len_before + 80) { all_tgts << "\n    "; len_before=ch_N(all_tgts); }
        all_tgts << target.target;
    }ind_end;
    prlog("all targets:" <<all_tgts);
}

// -----------------------------------------------------------------------------

// Create JCDB file for all selected build directories & targets
static void Main_CreateJcdb() {
    tempstr jcdb;
    jcdb << "[";
    ind_beg(abt::_db_builddir_curs,builddir,abt::_db) if (builddir.select) {
        algo::ListSep ls(",");
        ind_beg(abt::_db_zs_sel_target_curs,target,abt::_db) {
            tempstr outfile = GetOutfile(builddir,target);
            Set(builddir.R, "$target", target.target);
            Set(builddir.R, "$outfile", outfile);
            Set(builddir.R, "$outbase", StripExt(outfile));
            ind_beg(abt::target_c_srcfile_curs,srcfile,target) {
                Set(builddir.R,"$srcfile",srcfile.srcfile);
                Set(builddir.R,"$objpath",GetObjpath(builddir,srcfile));
                jcdb
                    << ls
                    << "{\"directory\":\"" << algo::GetCurDir() << "\""
                    << ",\"command\":\"" << abt::EvalSrcfileCmdline(builddir,target,srcfile) << "\""
                    << ",\"file\":\"" << srcfile.srcfile << "\"}" << eol;
            }ind_end;
        }ind_end;
    }ind_end;
    jcdb << "]" << eol;
    StringToFile(jcdb,abt::_db.cmdline.jcdb);
}

// -----------------------------------------------------------------------------

static void line_n_Update(abt::FSrcfile *srcfile) {
    if (srcfile->line_n == 0) {
        algo_lib::MmapFile file;
        MmapFile_Load(file,srcfile->srcfile);
        ind_beg(Line_curs, curs, file.text) {
            (void)curs;
            srcfile->line_n++;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Create commands between START and END for building target TARGET.
//
// compilation workflow for each target:
// cmd_start : start
// comptarg  : begin of compile all files
// linktarg  : begin of link all files
// end       : end of target workflow
//
// start -> compile -> link -> end
//
// all actual commands are allocated to a range between two nodes,
// for instance {start,comptarg}, or {comp,linktarg}
//
static void Main_CreateCmds(abt::FBuilddir &builddir, abt::FSyscmd *start, abt::FSyscmd *end) {
    algo_lib::Replscope &R = builddir.R;
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) if (target.p_ns->nstype != dmmeta_Nstype_nstype_none) {
        target.syscmd_start           = abt::NewCmd(start,end);
        target.syscmd_compile         = abt::NewCmd(target.syscmd_start,end);
        target.syscmd_link            = abt::NewCmd(target.syscmd_compile,end);
        abt::FSyscmd *syscmd_insttarg = abt::NewCmd(target.syscmd_link,end);
        target.syscmd_end             = abt::NewCmd(syscmd_insttarg,end);
        tempstr outfile = GetOutfile(builddir,target);
        Set(R, "$target", target.target);
        Set(R, "$outfile", outfile);
        Set(R, "$outbase", StripExt(outfile));

        // build step
        ind_beg(abt::target_c_srcfile_curs, srcfile,target) if (Regx_Match(abt::_db.cmdline.srcfile,srcfile.srcfile)) {
            // preprocess source file
            Set(R,"$srcfile",srcfile.srcfile);
            tempstr objpath=GetObjpath(builddir,srcfile);
            if (abt::_db.cmdline.preproc) {
                Set(R,"$objpath",ReplaceExt(objpath, ".i"));
                abt::FSyscmd& cmd_preproc = *NewCmd(target.syscmd_start, target.syscmd_compile);
                cmd_preproc.command << abt::EvalSrcfileCmdline(builddir,target,srcfile) << " -E";
            }
            // compile source file or precompiled header
            if (abt::_db.cmdline.build && srcfile.ood) {
                Set(R,"$objpath",objpath);
                abt::FSyscmd& cmd_compile = *NewCmd(target.syscmd_start, target.syscmd_compile);
                cmd_compile.outfile = objpath;
                // for coverage config, remove .gcda files - when .o and .gcno gets recompiled,
                // .gcda is no longer valid, and MUST be removed
                if (cfg_Get(builddir) == dev_Cfg_cfg_coverage) {
                    cmd_compile.command << "rm -f "<<ReplaceExt(objpath, ".gcda")<<"; ";
                }
                cmd_compile.command << abt::EvalSrcfileCmdline(builddir,target,srcfile);
                line_n_Update(&srcfile);
                cmd_compile.line_n += srcfile.line_n;
            }
        }ind_end;

        // if a source file filter as specified, do not perform link step
        bool canlink = accepts_all_Get(abt::_db.cmdline.srcfile.flags);

        // link step
        if (abt::_db.cmdline.build && target.ood && canlink) {
            cstring objs,libs;
            DepsObjList(builddir,target,objs,libs);
            Set(R,"$objs",objs);
            Set(R,"$libs",libs);
            if (target.p_ns->nstype == dmmeta_Nstype_nstype_lib || target.p_ns->nstype == dmmeta_Nstype_nstype_exe) {
                tempstr tempfile = tempstr()<< outfile<<".tmp";
                Set(R, "$origoutfile", "$outfile");
                Set(R, "$outfile", tempfile);
                abt::FSyscmd& cmd = *NewCmd(target.syscmd_link, target.syscmd_end);
                cmd.outfile = tempfile;// file to clean on failure
                cmd.command << Subst(R,"rm -f $outfile");
                if (target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
                    // recreate lib from scratch,
                    // otherwise old stale archive members don't get removed
                    if (compiler_Get(builddir) == dev_Compiler_compiler_cl) {
                        cmd.command << "\n"<<Subst(R,"$_ar /nologo $objs /out:$outfile");
                    } else {
                        cmd.command << "\n"<<Subst(R,"$_ar cr $outfile $objs && $ranlib $outfile");
                    }
                } else {
                    cmd.command << "\n"<<abt::EvalLinkCmdline(builddir,target);
                }
                cmd.command << Subst(R,"\nmv -f $outfile $origoutfile");
            }
        }

        // with -install, make bin/<target> point to the created executable
        if (abt::_db.cmdline.install && target.p_ns->nstype == dmmeta_Nstype_nstype_exe && canlink) {
            abt::FSyscmd& cmd_inst = *NewCmd(syscmd_insttarg, target.syscmd_end);
            cmd_inst.redirect = false;
            Ins(&R, cmd_inst.command, "ln -sf ../$out_dir/$target bin/$target");
        }
    }ind_end;

    // create dependencies between build commands
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) if (target.p_ns->nstype != dmmeta_Nstype_nstype_none) {
        ind_beg(abt::target_c_targdep_curs, dep,target) {
            // library or list of objects -- can compile in parallel with it, must wait for link step
            if (dep.p_parent->p_ns->nstype == dmmeta_Nstype_nstype_lib
                || dep.p_parent->p_ns->nstype == dmmeta_Nstype_nstype_objlist) {
                abt::syscmddep_InsertMaybe(dev::Syscmddep(target.syscmd_link->rowid, dep.p_parent->syscmd_end->rowid));
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Examine command line flags
// Scan list of builddirs and mark matching ones as welected
// The default CFG is release
// The default UNAME,COMPILER,ARCH are read from the soft link build/$cfg
// Overriding any of these options on the command-line is possible.
// The resulting builddir key (see acr builddir) must be in the table of supported configurations.
static void Main_GuessParams() {
    // pick a default config (release most of the time)
    if (!ch_N(abt::_db.cmdline.cfg.expr)) {
        Regx_ReadSql(abt::_db.cmdline.cfg, dev_Cfg_cfg_release, true);
    }
    ind_beg(abt::_db_cfg_curs,cfg,abt::_db) if (cfg.cfg!="" && Regx_Match(abt::_db.cmdline.cfg, cfg.cfg)) {
        // fill in uname, compiler, arch from defaults
        // provided by bootstrap
        // For instance, build/release -> Linux-clang++.release-x86_64
        // means that Linux,clang++ and x86_64 can be guessed
        // from cfg parameter alone
        tempstr dflt_builddir(algo::ReadLink(tempstr("build/")<<cfg.cfg));
        tempstr uname(abt::_db.cmdline.uname);
        tempstr compiler(abt::_db.cmdline.compiler);
        tempstr arch(abt::_db.cmdline.arch);
        if (uname == "") {
            uname = dev::Builddir_uname_Get(dflt_builddir);
        }
        if (compiler == "") {
            compiler = dev::Builddir_compiler_Get(dflt_builddir);
        }
        if (arch == "") {
            arch = dev::Builddir_arch_Get(dflt_builddir);
        }
        tempstr key_builddir = dev::Builddir_Concat_uname_compiler_cfg_arch(uname,compiler,cfg.cfg,arch);
        abt::FBuilddir *builddir = abt::ind_builddir_Find(key_builddir);
        vrfy(builddir, tempstr()<<"abt.builddir"
             <<Keyval("builddir",key_builddir)
             <<Keyval("comment","unsupported configuration; add this key to dev.builddir table"));
        builddir->select = true;
        if (abt::_db.c_builddir == NULL) {
            abt::_db.c_builddir = builddir;
        }
        // pick default output directory
        // use the soft link ($cfg) if none of the other parameters we customized on the command line
        // and -shortlink wasn't set.
        // this allows soft links installed under bin with -install to retain symbolic names
        // (e.g. amc -> ../build/release/amc rather than ../build/Linux-g++.release-x86_64/amc
        builddir->path << "build/" << (key_builddir == builddir->builddir && !abt::_db.cmdline.shortlink ? tempstr(cfg.cfg) : key_builddir);
        //initialize replacements
        Set(builddir->R,"$uname"   ,uname);
        Set(builddir->R,"$compiler",compiler);
        Set(builddir->R,"$cfg"     ,cfg.cfg);
        Set(builddir->R,"$arch"    ,arch);
        Set(builddir->R,"$out_dir" ,builddir->path);
        Set(builddir->R,"$_ar"     ,builddir->p_compiler->ar);// avoid substitution conflict with $arch
        Set(builddir->R,"$ranlib"  ,builddir->p_compiler->ranlib);
        Set(builddir->R,"$libext"  ,builddir->p_compiler->libext);
        Set(builddir->R,"$link"    ,builddir->p_compiler->link);
        Set(builddir->R,"$buildpath",builddir->path);
        Set(builddir->R,"$builddir","$uname-$compiler.$cfg-$arch");
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CreateTmpdir() {
    algo_lib::GetTempDir();
    if (abt::_db.cmdline.printcmd) {
        prlog("mkdir -p temp " << algo_lib::_db.tempdir);
    }
    setenv("TMPDIR",Zeroterm(algo_lib::_db.tempdir),1);
}

// -----------------------------------------------------------------------------

static void DetectCache() {
    // ignore in 'printcmd' (bootstrap) mode
    if (!abt::_db.cmdline.printcmd && abt::_db.cmdline.jcdb=="") {
        bool gcache_enabled = algo::DirectoryQ(".gcache/") && algo::FileQ("bin/gcache");
        bool ccache_enabled = algo::DirectoryQ(".ccache/");
        // use switch without default to ensure that all options are processed
        switch (cache_GetEnum(abt::_db.cmdline)) {
        case command_abt_cache_auto:
            abt::_db.gcache=gcache_enabled;
            abt::_db.ccache=ccache_enabled && !abt::_db.gcache;
            break;
        case command_abt_cache_none:
            break;
        case command_abt_cache_gcache:
        case command_abt_cache_gcache_force:
            abt::_db.gcache=gcache_enabled;
            if (!abt::_db.gcache) {
                prerr("abt.notice"<<Keyval("message","gcache disabled, first run gcache -install -enable"));
            }
            break;
        case command_abt_cache_ccache:
            abt::_db.ccache=ccache_enabled;
            if (!abt::_db.ccache) {
                prerr("abt.notice"<<Keyval("message","ccache disabled, first run ccashe-use"));
            }
        }
    }
}

// -----------------------------------------------------------------------------

static void CheckActed() {
    bool acted = false;
    acted |= abt::_db.cmdline.list;
    acted |= abt::_db.cmdline.listincl;
    acted |= ch_N(abt::_db.cmdline.disas.expr)>0;
    acted |= abt::_db.cmdline.preproc;
    acted |= abt::_db.cmdline.build;
    acted |= abt::_db.cmdline.clean;
    acted |= abt::_db.cmdline.install;

    if (!acted) {
        prerr("abt.noaction"
              <<Keyval("comment","no action specified. use -help for more information"));
    }
    // display list of groups, and available targets.
    if (!acted) {
        Main_Helpscreen();
    }
}

// -----------------------------------------------------------------------------

static void RewriteOpts() {
    if (!abt::_db.cmdline.build
        && !abt::_db.cmdline.list
        && !abt::_db.cmdline.listincl
        && !abt::_db.cmdline.ood
        && !abt::_db.cmdline.clean
        && !abt::_db.cmdline.preproc
        && ch_N(abt::_db.cmdline.target.expr)) {
        abt::_db.cmdline.build=true; // pick a reasonable default
    }
    // pick reasonable maxjobs
    // can we limit these compilers to a group so they don't suck up all cpu on a machine?
    if (abt::_db.cmdline.maxjobs == 0) {
        abt::_db.cmdline.maxjobs = i32_Max(4,sysconf(_SC_NPROCESSORS_ONLN)/2);
    }
    abt::_db.cmdline.maxjobs = i32_Max(abt::_db.cmdline.maxjobs, 1);

    // implies
    if (abt::_db.cmdline.ood) {
        abt::_db.cmdline.list = true;
    }
    // install implies build
    if (abt::_db.cmdline.install) {
        abt::_db.cmdline.build = true;
    }
    if (abt::_db.cmdline.printcmd) {
        prlog("set -e");// exit on first error
        prlog("set -x");// show commands
        abt::_db.cmdline.maxjobs = 1;// deterministic output
        abt::_db.cmdline.force = true;// make everything out-of-date
    }
}

// -----------------------------------------------------------------------------

static void Main_Dry() {
    if (abt::_db.cmdline.dry_run) {
        ind_beg(abt::_db_syscmd_curs, syscmd,abt::_db) {
            dev::Syscmd temp;
            abt::syscmd_CopyOut(syscmd, temp);
            prlog(temp);
        }ind_end;
        ind_beg(abt::_db_syscmddep_curs, syscmddep,abt::_db) {
            dev::Syscmddep temp;
            abt::syscmddep_CopyOut(syscmddep, temp);
            prlog(temp);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

tempstr abt::GetOutfile(abt::FBuilddir &builddir, abt::FTarget &target) {
    tempstr ret;
    tempstr prefix = DirFileJoin(builddir.path, target.target);
    if (target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
        ret << prefix << "-" << arch_Get(builddir) << builddir.p_compiler->libext;
    } else if (target.p_ns->nstype == dmmeta_Nstype_nstype_exe) {
        ret << prefix << builddir.p_compiler->exeext;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void Main_ShowReport() {
    abt::_db.report.n_target = abt::zs_sel_target_N();
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        if (abt::_db.cmdline.install && target.p_ns->nstype != dmmeta_Nstype_nstype_none) {
            // this is inaccurate -- only shows last builddir
            abt::_db.report.n_install += target.syscmd_end->status ==0;
        }
    }ind_end;
    if (abt::_db.cmdline.report) {
        prlog(abt::_db.report);
    }
}

// -----------------------------------------------------------------------------

static void CreateDirRecurseV(strptr dir) {
    CreateDirRecurse(dir);
    if (abt::_db.cmdline.printcmd) {
        prlog("mkdir -p "<<dir<<" || true");
    }
}

// -----------------------------------------------------------------------------

void abt::Main() {
    algo::UnTime starttime = algo::CurrUnTime();
    algo::SetupExitSignals();

    _db.tty=isatty(1);
    RewriteOpts();
    Main_GuessParams();
    DetectCache();
    if (abt::_db.cmdline.build) {
        CreateTmpdir();
    }
    ind_beg(_db_tool_opt_curs,tool_opt,_db) {
        Regx_ReadSql(tool_opt.regx_opt,Pathcomp(tool_opt.tool_opt,"/LL"),true);
        Regx_ReadSql(tool_opt.regx_target,target_Get(tool_opt),true);
    }ind_end;
    Main_SelectTarget();

    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        Main_ComputeAlldep(target,target);
    }ind_end;

    // scan source files and headers
    abt::ScanSrcfile();

    // create top-level build directory
    if (abt::_db.cmdline.build && algo_lib::_db.exit_code == 0) {
        ind_beg(_db_builddir_curs,builddir,_db) if (builddir.select) {
            CreateDirRecurseV(DirFileJoin("build",cfg_Get(builddir)));
            CreateDirRecurseV(builddir.path);
            LockFileInit(builddir.lockfile, DirFileJoin(builddir.path, "abt.lock"));
        }ind_end;
    }

    // clean -- note, all cleaning has to be done before all compiling
    // or out-of-date determination
    if (abt::_db.cmdline.clean) {
        Main_Clean();
    }

    abt::ComputeTimestamps();

    // list (print to stdout)
    if (abt::_db.cmdline.list) {
        Main_List();
    }

    if (abt::_db.cmdline.listincl) {
        Main_ListIncl();
    }

    if (algo_lib::_db.exit_code==0) {
        if (abt::_db.cmdline.jcdb!="") {
            Main_CreateJcdb();
        } else if (abt::_db.cmdline.dry_run) {
            Main_Dry();
        } else {
            abt::FSyscmd *start=NULL,*end=NULL;
            ind_beg(_db_builddir_curs,builddir,_db) if (builddir.select) {
                abt::ComputeOod(builddir);
                Main_CreateCmds(builddir,start,end);
            }ind_end;
            abt::Main_Build();
        }
    }

    // check if we actually did anything, and offer help if we didn't.
    CheckActed();

    if (ch_N(abt::_db.cmdline.disas.expr)) {
        abt::Main_Disas();// show disassembly
    }

    // report abt warnings and errors, and execution time
    abt::_db.report.time = algo::CurrUnTime()-starttime;
    if (_db.cmdline.build && _db.gcache && !abt::zs_sel_target_EmptyQ()) {
        // compute hit rate from moment of invocation
        command::gcache gcache;
        gcache.hitrate=true;
        gcache.after=starttime;
        cstring out = SysEval(command::gcache_ToCmdline(gcache)<<" 2>&1",FailokQ(true),1024);
        ind_beg(Line_curs,line,out) {
            report::gcache_hitrate hr;
            if (gcache_hitrate_ReadStrptrMaybe(hr,line)) {
                abt::_db.report.hitrate=hr.hitrate;
                abt::_db.report.pch_hitrate=hr.pch_hitrate;
                break;
            }
        }ind_end;
    }

    Main_ShowReport();
    verblog(abt::_db.trace);

    if (abt::_db.report.n_err > 0) {
        algo_lib::_db.exit_code = 1;
    }
}
