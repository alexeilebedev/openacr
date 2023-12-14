// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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

static abt::FFilestat &Filestat(strptr fname) {
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

static bool SourceQ(abt::FTargsrc &targsrc) {
    return ext_Get(targsrc)!="h";
}

// -----------------------------------------------------------------------------

abt::FSyscmd& abt::NewCmd(abt::FSyscmd *start, abt::FSyscmd *end) {
    abt::FSyscmd *cmd = &abt::ind_syscmd_GetOrCreate(abt::syscmd_N());
    if (start) {
        abt::syscmddep_InsertMaybe(dev::Syscmddep(cmd->rowid, start->rowid));//child,parent
    }
    if (end) {
        abt::syscmddep_InsertMaybe(dev::Syscmddep(end->rowid, cmd->rowid));
    }
    return *cmd;
}

// -----------------------------------------------------------------------------

static void ComputeCumMod(abt::FSrcfile &srcfile) {
    if (bool_Update(srcfile.cum_mod_visited,true)) {
        zd_inclstack_Insert(srcfile);
        ind_beg(abt::srcfile_zd_include_curs, include, srcfile) {
            ComputeCumMod(*include.p_header);
            i64_UpdateMax(srcfile.cum_modtime.value, include.p_header->cum_modtime.value);
        }ind_end;
        zd_inclstack_Remove(srcfile);
    } else {
        if (zd_inclstack_InLlistQ(srcfile) & !StartsWithQ(srcfile.srcfile,"extern/")) {
            cstring out;
            strptr sep("\n    ");
            ind_beg(abt::_db_zd_inclstack_curs,s,abt::_db) {
                out<<sep<<s.srcfile;
            }ind_end;
            out<<sep<<srcfile.srcfile;
            prlog(srcfile.srcfile<<":1: abt.circular_include  "<<out);
            algo_lib::_db.exit_code=1;
        }
    }
}

// Determine if linking step is needed for the target
// It is needed if any of the dependents are out-of-date or
// has a newer output than the target's output
static void ComputeOod(abt::FTarget &target) {
    if (!target.ood_visited) {
        target.ood_visited = true;
        ind_beg(abt::target_c_targdep_curs, targdep, target) {
            ComputeOod(*targdep.p_parent);
            target.ood = target.ood || targdep.p_parent->ood
                || target.out_modtime.value < targdep.p_parent->out_modtime.value;
        }ind_end;
    }
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

static void Main_ComputeAlllib() {// transitively collect all libraries for target
    ind_beg(abt::_db_target_curs, target, abt::_db) {
        ind_beg(abt::target_c_alldep_curs, dep, target) {
            ind_beg(abt::target_c_targsyslib_curs, targsyslib, dep) {
                if (Regx_Match(targsyslib.uname, abt::_db.cmdline.uname)) {
                    c_alllib_ScanInsertMaybe(target,*targsyslib.p_syslib);
                }
            }ind_end;
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Check if specified target is compatible with
// currently selected configuration
static bool CompatTargetQ(abt::FTarget &target) {
    bool ret = true;
    if (!abt::_db.cmdline.listincl) {
        algo_lib::Regx compat;
        Regx_ReadSql(compat,target.compat,true);
        ret = Regx_Match(compat,abt::_db.builddir);
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void Main_SelectTarget() {
    // create list of targets to consider:
    // any target matching command line regex, but filtered for compatibility with
    // current configuration (uname-compiler.cfg-arch)
    ind_beg(abt::_db_target_curs, target,abt::_db) {
        if (Regx_Match(abt::_db.cmdline.target, target.target) && CompatTargetQ(target)) {
            abt::zs_origsel_target_Insert(target);
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
        abt::_db.report.n_err++;
    }
}

// -----------------------------------------------------------------------------

static tempstr RemoveDotsOnce(strptr path) {
    int i = FindStr(path,"/../");
    return i==-1 ? tempstr(path) : DirFileJoin(Pathcomp(FirstN(path,i),"/RL"), RestFrom(path,i+4));
}

static tempstr RemoveDots(strptr path_) {
    tempstr path(path_);
    tempstr out;
    do {
        out = RemoveDotsOnce(path);
        if (out == path) {
            break;
        }
        path = out;
    } while (true);
    return out;
}

// -----------------------------------------------------------------------------

static abt::FInclude *FindFile(abt::FSrcfile &srcfile, strptr incl, bool is_sys) {
    strptr dir = GetDirName(srcfile.srcfile);
    tempstr fname(incl);
    bool existed = abt::ind_srcfile_Find(incl);
    bool found = existed || Filestat(incl).exists;

    // try path relative to source file
    if (!found) {
        fname = RemoveDots(DirFileJoin(dir,incl));
        found = Filestat(fname).exists;
    }
    if (!found && is_sys) {
        ind_beg(abt::_db_sysincl_curs,sysincl,abt::_db) {
            fname = tempstr() << sysincl << algo::MaybeDirSep << incl;
            found = Filestat(fname).exists;
            if (found) {
                break;
            }
        }ind_end;
    }
    tempstr key =tempstr()<<srcfile.srcfile<<":"<<fname;
    abt::FInclude *include = abt::ind_include_Find(key);// include -- relation on two srcfiles
    if (found && !include) {
        abt::ind_srcfile_GetOrCreate(fname); // create missing srcfile
        include          = &abt::include_Alloc();
        include->include = key;
        include->sys     = is_sys;
        include_XrefMaybe(*include);
    }
    return include;
}

// -----------------------------------------------------------------------------

static void Main_ComputeSysincl() {
    bool success=false;
    if (abt::_db.cmdline.printcmd || abt::_db.cmdline.jcdb!="") {
        success=true;
        // do nothing -- system includes won't be used
    } else {
        // this it he command line that returns the list
        // of system include paths on this machine
        tempstr text;
        if (abt::_db.cmdline.compiler == dev_Compiler_compiler_cl) {
            text = tempstr() << "#include <...>\n"
                             << algo::SysEval("cygpath -u -p \"\\$INCLUDE\"",FailokQ(true),1024*1024)
                             << "\n"
                             << "End of search list\n";
        } else {
            tempstr cmd;
            cmd << abt::_db.cmdline.compiler << " -x c++ -E -v - </dev/null 2>&1";
            text = SysEval(cmd,FailokQ(true),1024*1024);
        }
        // parse it for the paths...
        bool inside = false;
        ind_beg(Line_curs,line,text) {
            if (StartsWithQ(line, "#include <...>")) {
                inside = true;
            }
            if (StartsWithQ(line, "End of search list")) {
                inside = false;
                success = true;
            }
            strptr path = Trimmed(line);
            if (inside && Filestat(path).isdir) {
                abt::sysincl_Alloc() = Trimmed(path);
            }
        }ind_end;
    }
    vrfy(success,
         tempstr("abt.sysincl_warning")
         <<Keyval("compiler",abt::_db.cmdline.compiler)
         <<Keyval("comment", "cannot compute list of system include paths"));
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

static void ScanHeaders(abt::FSrcfile *srcfile) {
    algo_lib::MmapFile file;
    MmapFile_Load(file,srcfile->srcfile);
    u64 bytes_not_scanned = 0;
    u64 bytes_total = file.text.n_elems;
    ind_beg(Line_curs, curs, file.text) {
        algo::StringIter line(curs);
        if (SkipChar(line, '#')) {
            bool is_include = SkipStrptr(line.Ws(), "include");
            bool is_quote = SkipChar(line.Ws(), '\"');
            bool is_sys   = SkipChar(line.Ws(), '<');
            is_include = is_include && (is_quote || is_sys);
            if (is_include && !is_sys) {
                strptr incl(GetTokenChar(line, is_quote ? '\"' : '>'));
                int lineno = ind_curs(curs).i + 1;
                abt::FInclude *include = FindFile(*srcfile,incl,is_sys);
                if (!include && !is_sys) {
                    prerr(srcfile->srcfile<<":"<<lineno<<": abt can't find "<<incl);
                    algo_lib::_db.exit_code++;
                }
            }
        } else if (SkipStrptr(line, "//#pragma endinclude")) {
            bytes_not_scanned += file.text.n_elems - (curs.elems-file.text.elems);
            // take a hint that no further includes will occur in this file
            // (saves a bunch of page faults when scanning)
            // this can only be done in headers because otherwise line count is off
            break;
        }
    }ind_end;
    verblog("abt.scanfile"
            <<Keyval("srcfile",srcfile->srcfile)
            <<Keyval("bytes_n", bytes_total)
            <<Keyval("bytes_not_scanned", bytes_not_scanned));
}

// -----------------------------------------------------------------------------

// compute obj key by replace path components
// with .
// So, cpp/abt/main.cpp becomes cpp.abt.main.cpp
// Next step will be to replace the extension
tempstr abt::GetObjkey(strptr source) {
    tempstr ret(source);
    Translate(ret, "/", ".");
    return ret;
}

// -----------------------------------------------------------------------------

static void ReadSrcfile_ComputeObjpath(abt::FSrcfile &srcfile) {
    srcfile.objkey = abt::GetObjkey(srcfile.srcfile);
    // Replace extension
    strptr tgt_ext;
    strptr ext = GetFileExt(srcfile.srcfile);
    if (ext == ".cpp" || ext == ".c") {
        tgt_ext = abt::_db.c_compiler->objext;
    } else if (ext == ".rc" && abt::_db.c_compiler->rc != "") {
        tgt_ext = ".res";
    }
    if (ch_N(tgt_ext)) {
        srcfile.objpath << abt::_db.cmdline.out_dir << algo::MaybeDirSep;
        if (!abt::HeaderExtQ(ext)) {
            srcfile.objpath << StripExt(srcfile.objkey);
        } else {
            srcfile.objpath << srcfile.objkey;
        }
        srcfile.objpath << tgt_ext;
    }
}

// -----------------------------------------------------------------------------

// This function always scans the entire graph of all sources
static void Main_ReadSrcfile() {
    // walk over target sources; each file corresponds to a single file; create Srcfile records
    ind_beg(abt::_db_targsrc_curs, targsrc, abt::_db) {
        if (SourceQ(targsrc)) {
            tempstr pathname(src_Get(targsrc));
            vrfy(Filestat(pathname).exists, tempstr()
                 <<Keyval("src",src_Get(targsrc))
                 <<Keyval("comment","file not found"));
            abt::FSrcfile& src_rec = abt::ind_srcfile_GetOrCreate(pathname);
            src_rec.p_target = targsrc.p_target;
            c_srcfile_Insert(*targsrc.p_target, src_rec);
        }
    }ind_end;

    // Recursively read headers for each srcfile: these also become Srcfiles but they are
    // not connected to the target.
    // compute mod time for each srcfile
    while(abt::FSrcfile *srcfile = abt::zs_srcfile_read_First()) {
        ReadSrcfile_ComputeObjpath(*srcfile);
        // read file, scan includes
        ScanHeaders(srcfile);
        abt::zs_srcfile_read_RemoveFirst();
    }
    if (algo_lib::_db.exit_code > 0) {
        _exit(1); // exit right away. nothing smarter can be done. TODO: something smarter.
    }
}

// -----------------------------------------------------------------------------

void abt::DeleteFileV(strptr path) {
    verblog("rm -f "<<path);
    DeleteFile(path);
}

// -----------------------------------------------------------------------------

static void Main_Clean() {
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        abt::DeleteFileV(target.outfile);
        ind_beg(abt::target_c_srcfile_curs, src,target) if (ch_N(src.objpath)) {
            abt::DeleteFileV(src.objpath);
            // clean coverage files
            tempstr gcdafile(ReplaceExt(src.objpath, ".gcda"));
            tempstr gcnofile(ReplaceExt(src.objpath, ".gcno"));
            abt::DeleteFileV(gcdafile);
            abt::DeleteFileV(gcnofile);
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
        ind_beg(abt::target_c_srcfile_curs, src,target) {
            bool list_src = true;
            if (abt::_db.cmdline.ood) {
                list_src      &= src.ood;
            }
            if (list_src) {
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
        ind_beg(abt::srcfile_zd_include_curs,include,srcfile) {
            include.wantprint=true;
            ListIncl(*include.p_header);
        }ind_end;
    }
}

// List includes relevant to selected targets
static void Main_ListIncl() {
    ind_beg(abt::_db_zs_sel_target_curs,target,abt::_db) {// list includes for all sources of selected targets
        ind_beg(abt::target_c_targsrc_curs,targsrc,target) if (SourceQ(targsrc)) {
            abt::FSrcfile *srcfile=abt::ind_srcfile_Find(src_Get(targsrc));
            if (srcfile) {
                ListIncl(*srcfile);
            }
        }ind_end;
    }ind_end;
    ind_beg(abt::_db_include_curs,include,abt::_db) if (include.wantprint) {// print selected includes
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

static void Main_CreateJcdb(algo_lib::Replscope &R) {
    tempstr jcdb;
    jcdb << "[";
    algo::ListSep ls(",");
    ind_beg(abt::_db_zs_sel_target_curs,target,abt::_db) {
        Set(R, "$target", target.target);
        Set(R, "$outfile", target.outfile);
        Set(R, "$outbase", StripExt(target.outfile));
        ind_beg(abt::target_c_srcfile_curs,srcfile,target) {
            Set(R,"$srcfile",srcfile.srcfile);
            Set(R,"$objpath",srcfile.objpath);
            jcdb
                << ls
                << "{\"directory\":\"" << algo::GetCurDir() << "\""
                << ",\"command\":\"" << abt::EvalSrcfileCmdline(R,target,srcfile) << "\""
                << ",\"file\":\"" << srcfile.srcfile << "\"}" << eol;
        }ind_end;
    }ind_end;
    jcdb << "]" << eol;
    StringToFile(jcdb,abt::_db.cmdline.jcdb);
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
static void Main_CreateCmds(algo_lib::Replscope &R, abt::FTarget &target, abt::FSyscmd *start, abt::FSyscmd *end) {
    abt::FSyscmd& targ_start    = NewCmd(start,end);
    abt::FSyscmd& targ_comptarg = NewCmd(&targ_start,end);
    abt::FSyscmd& targ_linktarg = NewCmd(&targ_comptarg,end);
    abt::FSyscmd& targ_insttarg = NewCmd(&targ_linktarg,end);
    abt::FSyscmd& targ_end      = NewCmd(&targ_insttarg,end);
    target.targ_start   = &targ_start;
    target.targ_compile = &targ_comptarg;
    target.targ_link    = &targ_linktarg;
    target.targ_end     = &targ_end;
    Set(R, "$target", target.target);
    Set(R, "$outfile", target.outfile);
    Set(R, "$outbase", StripExt(target.outfile));

    // build step
    ind_beg(abt::target_c_srcfile_curs, srcfile,target) {
        // preprocess source file
        Set(R,"$srcfile",srcfile.srcfile);
        if (abt::_db.cmdline.preproc) {
            Set(R,"$objpath",ReplaceExt(srcfile.objpath, ".i"));
            abt::FSyscmd& cmd_preproc = NewCmd(&targ_start, &targ_comptarg);
            prlog(abt::EvalSrcfileCmdline(R,target,srcfile));
            cmd_preproc.command << abt::EvalSrcfileCmdline(R,target,srcfile) << " -E";
        }
        // compile source file or precompiled header
        if (abt::_db.cmdline.build && srcfile.ood) {
            Set(R,"$objpath",srcfile.objpath);
            abt::FSyscmd& cmd_compile = NewCmd(&targ_start, &targ_comptarg);
            cmd_compile.outfile = srcfile.objpath;
            // for coverage config, remove .gcda files - when .o and .gcno gets recompiled,
            // .gcda is no longer valid, and MUST be removed
            if (abt::_db.cmdline.cfg == dev_Cfg_cfg_coverage) {
                cmd_compile.command << "rm -f "<<ReplaceExt(srcfile.objpath, ".gcda")<<"; ";
            }
            cmd_compile.command << abt::EvalSrcfileCmdline(R,target,srcfile);
            line_n_Update(&srcfile);
            cmd_compile.line_n += srcfile.line_n;
        }
    }ind_end;

    // link step
    if (abt::_db.cmdline.build && target.ood) {
        cstring objs,libs;
        DepsObjList(target,objs,libs);
        Set(R,"$objs",objs);
        Set(R,"$libs",libs);
        if (target.p_ns->nstype == dmmeta_Nstype_nstype_lib || target.p_ns->nstype == dmmeta_Nstype_nstype_exe) {
            tempstr tempfile = tempstr()<< target.outfile<<".tmp";
            Set(R, "$origoutfile", "$outfile");
            Set(R, "$outfile", tempfile);
            abt::FSyscmd& cmd = NewCmd(&targ_linktarg, &targ_end);
            cmd.outfile = tempfile;// file to clean on failure
            cmd.command << Subst(R,"rm -f $outfile");
            if (target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
                // recreate lib from scratch,
                // otherwise old stale archive members don't get removed
                if (abt::_db.cmdline.compiler == dev_Compiler_compiler_cl) {
                    cmd.command << "\n"<<Subst(R,"$_ar /nologo $objs /out:$outfile");
                } else {
                    cmd.command << "\n"<<Subst(R,"$_ar cr $outfile $objs && $ranlib $outfile");
                }
            } else {
                cmd.command << "\n"<<abt::EvalLinkCmdline(R,target);
            }
            cmd.command << Subst(R,"\nmv -f $outfile $origoutfile");
        }
    }

    // with -install, make bin/<target> point to the created executable
    if (abt::_db.cmdline.install && target.p_ns->nstype == dmmeta_Nstype_nstype_exe) {
        abt::FSyscmd& cmd_inst = NewCmd(&targ_insttarg, &targ_end);
        cmd_inst.redirect = false;
        Ins(&R, cmd_inst.command, "ln -sf ../$out_dir/$target bin/$target");
    }
}

// -----------------------------------------------------------------------------

static void MaybeSetVar(algo::Smallstr50 &var, strptr value, strptr source) {
    if (var=="" && value != "") {
        var = value;
        verblog("# abt: selecting "<<source<<" = "<<value);
    }
}

// -----------------------------------------------------------------------------

static void Main_GuessParams(algo_lib::Replscope &R) {
    // pick a default config (release most of the time)
    if (!ch_N(abt::_db.cmdline.cfg)) {
        abt::_db.cmdline.cfg = dev_Cfg_cfg_release;
    }
    // fill in uname, compiler, arch from defaults
    // provided by bootstrap
    // For instance, build/release -> Linux-clang++.release-x86_64
    // means that Linux,clang++ and x86_64 can be guessed
    // from cfg parameter alone
    tempstr default_builddir(algo::ReadLink(tempstr("build/")<<abt::_db.cmdline.cfg));
    MaybeSetVar(abt::_db.cmdline.uname, dev::Builddir_uname_Get(default_builddir), "uname from softlink in build/");
    MaybeSetVar(abt::_db.cmdline.compiler, dev::Builddir_compiler_Get(default_builddir), "compiler from softlink in build/");
    MaybeSetVar(abt::_db.cmdline.arch, dev::Builddir_arch_Get(default_builddir), "arch from softlink in build/");

#ifndef WIN32
    if (!ch_N(abt::_db.cmdline.arch) || !ch_N(abt::_db.cmdline.uname)) {
        struct utsname un;
        uname(&un);
        if (!ch_N(abt::_db.cmdline.arch)) abt::_db.cmdline.arch   = un.machine;
        if (!ch_N(abt::_db.cmdline.uname)) abt::_db.cmdline.uname = un.sysname;
    }
#endif

    // cause errors if any of these guys are not found
    vrfy(abt::ind_uname_Find   (abt::_db.cmdline.uname)   , tempstr()<<"abt.bad_uname"   <<Keyval("uname",abt::_db.cmdline.uname));
    vrfy(abt::ind_compiler_Find(abt::_db.cmdline.compiler), tempstr()<<"abt.bad_compiler"<<Keyval("compiler",abt::_db.cmdline.compiler));
    vrfy(abt::ind_cfg_Find     (abt::_db.cmdline.cfg)     , tempstr()<<"abt.bad_cfg"     <<Keyval("cfg",abt::_db.cmdline.cfg));
    vrfy(abt::ind_arch_Find    (abt::_db.cmdline.arch)    , tempstr()<<"abt.bad_arch"    <<Keyval("arch",abt::_db.cmdline.arch));

    // pick compiler params
    abt::_db.c_compiler = abt::ind_compiler_Find(abt::_db.cmdline.compiler);
    vrfy(abt::_db.c_compiler , tempstr()<<"abt.compiler bad choice :"<<abt::_db.cmdline.compiler);

    abt::_db.builddir = dev::Builddir_Concat_uname_compiler_cfg_arch(abt::_db.cmdline.uname
                                                                     ,abt::_db.cmdline.compiler
                                                                     ,abt::_db.cmdline.cfg
                                                                     ,abt::_db.cmdline.arch);
    // pick default output directory
    // use the soft link ($cfg) if none of the other parameters we customized on the command line
    // this allows soft links installed under bin with -install to retain symbolic names
    // (e.g. amc -> ../build/release/amc rather than ../build/Linux-g++.release-x86_64/amc
    if (!ch_N(abt::_db.cmdline.out_dir)) {
        abt::_db.cmdline.out_dir << "build/" << (abt::_db.builddir == default_builddir
                                                 ? abt::_db.cmdline.cfg
                                                 : abt::_db.builddir);
    }
    //initialize replacements
    Set(R,"$uname"   ,abt::_db.cmdline.uname);
    Set(R,"$compiler",abt::_db.cmdline.compiler);
    Set(R,"$cfg"     ,abt::_db.cmdline.cfg);
    Set(R,"$arch"    ,abt::_db.cmdline.arch);
    Set(R,"$out_dir" ,abt::_db.cmdline.out_dir);
    Set(R,"$_ar"     ,abt::_db.c_compiler->ar);// avoid substitution conflict with $arch
    Set(R,"$ranlib"  ,abt::_db.c_compiler->ranlib);
    Set(R,"$libext"  ,abt::_db.c_compiler->libext);
    Set(R,"$link"    ,abt::_db.c_compiler->link);
    Set(R,"$builddir","$uname-$compiler.$cfg-$arch");
}

// -----------------------------------------------------------------------------

static void Main_ComputeTimestamps() {
    ind_beg(abt::_db_srcfile_curs, srcfile, abt::_db) {
        srcfile.cum_modtime = Filestat(srcfile.srcfile).modtime;
    }ind_end;
    // compute cumulative modification time for each file
    ind_beg(abt::_db_srcfile_curs, srcfile, abt::_db) {
        ComputeCumMod(srcfile);
    }ind_end;

    // compute out-of-date flag for each file
    ind_beg(abt::_db_srcfile_curs, srcfile, abt::_db) {
        algo::UnTime src_out_modtime = Filestat(srcfile.objpath).modtime;
        srcfile.ood  = UnTime_Lt(src_out_modtime, srcfile.cum_modtime);
        srcfile.ood |= abt::_db.cmdline.force;
        srcfile.ood |= Filestat(srcfile.objpath).size == 0;// #AL# zero-sized file is out of date
    }ind_end;

    // compute target cumulative modification time
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        ind_beg(abt::target_c_srcfile_curs, src,target) {
            target.cum_modtime.value = i64_Max(target.cum_modtime.value, src.cum_modtime.value);
        }ind_end;
    }ind_end;

    // compute out-of-date flag for each target (collect it from source files)
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        ind_beg(abt::target_c_srcfile_curs, src,target) {
            target.ood |= src.ood;
        }ind_end;
        target.ood |= abt::_db.cmdline.force;
        if (ch_N(target.outfile)) {
            target.out_modtime = Filestat(target.outfile).modtime;
            target.ood |= target.cum_modtime.value > target.out_modtime.value;
            target.ood |= Filestat(target.outfile).size == 0;// #AL# zero sized file is out of date
        }
    }ind_end;

    // propagate out-of-date flag across targets
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        ComputeOod(target);
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CreateTmpdir() {
    errno_vrfy(algo::CreateDirRecurse("temp")
               ,tempstr()<<"abt.createdir"
               <<Keyval("comment","failed to create temp directory"));
    if (abt::_db.cmdline.printcmd) {
        prlog("mkdir -p temp");
    }
    setenv("TMPDIR","temp",1);
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

static void Main_ShowOod() {
    bool realexec = !abt::_db.cmdline.dry_run && !abt::_db.cmdline.printcmd;
    int ood_src=0, ood_lib=0, ood_exe=0, ood_objlist=0;
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        ood_objlist += target.p_ns->nstype == dmmeta_Nstype_nstype_objlist && target.ood;
        ood_lib     += target.p_ns->nstype == dmmeta_Nstype_nstype_lib && target.ood;
        ood_exe     += target.p_ns->nstype == dmmeta_Nstype_nstype_exe && target.ood;
        ind_beg(abt::target_c_srcfile_curs, srcfile, target) {
            ood_src += srcfile.ood;
        }ind_end;
    }ind_end;
    if (realexec && abt::zs_sel_target_N() > 0 && abt::_db.cmdline.report) {
        prlog("abt.config"
              <<Keyval("config",abt::_db.builddir)
              <<Keyval("cache",(abt::_db.ccache ? "ccache" : abt::_db.gcache ? "gcache" : "none"))
              <<Keyval("out_dir",abt::_db.cmdline.out_dir)
              );
        prlog("abt.outofdate"
              <<Keyval("src", ood_src)
              <<Keyval("lib", ood_lib)
              <<Keyval("exe", ood_exe)
              <<Keyval("objlist", ood_objlist)
              );
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

static void Main_Cmddep() {
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        if (target.p_ns->nstype != dmmeta_Nstype_nstype_none) {
            ind_beg(abt::target_c_targdep_curs, dep,target) {
                // library or list of objects -- can compile in parallel with it, must wait for link step
                if (dep.p_parent->p_ns->nstype == dmmeta_Nstype_nstype_lib
                    || dep.p_parent->p_ns->nstype == dmmeta_Nstype_nstype_objlist) {
                    abt::syscmddep_InsertMaybe(dev::Syscmddep(target.targ_link->rowid, dep.p_parent->targ_end->rowid));
                }
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_ComputeOutfile() {
    ind_beg(abt::_db_target_curs, tgt,abt::_db) {
        vrfy(!ch_N(tgt.outfile), "internal error");
        if (tgt.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
            tgt.outfile << abt::_db.cmdline.out_dir
                        << algo::MaybeDirSep
                        << tgt.target << "-" << abt::_db.cmdline.arch << abt::_db.c_compiler->libext;
        } else if (tgt.p_ns->nstype == dmmeta_Nstype_nstype_exe) {
            tgt.outfile << abt::_db.cmdline.out_dir
                        << algo::MaybeDirSep << tgt.target
                        << abt::_db.c_compiler->exeext;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_ShowReport() {
    abt::_db.report.n_target = abt::zs_sel_target_N();
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        if (abt::_db.cmdline.install && target.p_ns->nstype != dmmeta_Nstype_nstype_none) {
            abt::_db.report.n_install += target.targ_end->status ==0;
        }
    }ind_end;
    if (abt::_db.cmdline.report) {
        prlog(abt::_db.report);
    }
}

// -----------------------------------------------------------------------------

void abt::Main_PrepOpts() {
    ind_beg(_db_tool_opt_curs,tool_opt,_db) {
        Regx_ReadSql(tool_opt.regx_opt,Pathcomp(tool_opt.tool_opt,"/LL"),true);
        Regx_ReadSql(tool_opt.regx_target,target_Get(tool_opt),true);
        tool_opt.select = Regx_Match(tool_opt.regx_opt,_db.builddir);
    }ind_end;
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
    algo_lib::Replscope R;
    Main_GuessParams(R);
    DetectCache();
    if (abt::_db.cmdline.build) {
        CreateTmpdir();
    }

    Main_ComputeSysincl();

    Main_PrepOpts();
    Main_SelectTarget();
    if (abt::_db.report.n_err) {
        return;
    }

    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        Main_ComputeAlldep(target,target);
    }ind_end;

    Main_ComputeAlllib();
    // Scan headers for selected targets only
    Main_ReadSrcfile();
    // compute output file name for each target
    Main_ComputeOutfile();

    // create top-level build directory
    algo_lib::FLockfile lockfile;
    if (abt::_db.cmdline.build) {
        CreateDirRecurseV(DirFileJoin("build",abt::_db.cmdline.cfg));
        CreateDirRecurseV(abt::_db.cmdline.out_dir);
        // create lock file
        LockFileInit(lockfile, DirFileJoin(abt::_db.cmdline.out_dir, "abt.lock"));
    }

    // clean -- note, all cleaning has to be done before all compiling
    // or out-of-date determination
    if (abt::_db.cmdline.clean) {
        Main_Clean();
    }

    Main_ComputeTimestamps();

    // list (print to stdout)
    if (abt::_db.cmdline.list) {
        Main_List();
    }

    if (abt::_db.cmdline.listincl) {
        Main_ListIncl();
    }

    abt::FSyscmd *start=NULL,*end=NULL;
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        if (target.p_ns->nstype != dmmeta_Nstype_nstype_none) {// no commands for groups
            Main_CreateCmds(R,target,start,end);
        }
    }ind_end;

    // add dependencies between targets.
    Main_Cmddep();

    // compute number of executable commands
    Main_ShowOod();

    if (abt::_db.cmdline.jcdb!="") {
        Main_CreateJcdb(R);
    } else if (abt::_db.cmdline.dry_run) {
        Main_Dry();
    } else {
        abt::Main_Build();
    }

    // check if we actually did anything, and offer help if we didn't.
    CheckActed();

    if (ch_N(abt::_db.cmdline.disas.expr)) {
        abt::Main_Disas();// show disassembly
    }

    // report abt warnings and errors, and execution time
    abt::_db.report.time = algo::CurrUnTime()-starttime;
    // compute hit rate from moment of invocation
    if (_db.gcache) {
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
