// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: gcache (exe) -- Compiler cache
// Exceptions: yes
// Source: cpp/gcache/gcache.cpp
//

#include "include/algo.h"
#include "include/gcache.h"

//------------------------------------------------------------------------------

static bool Flock(Fildes fd, int flags) {
    return  ValidQ(fd) && flock(fd.value,flags)==0;
}

// -----------------------------------------------------------------------------

static void ShowStats(algo::strptr dir) {
    prlog("cache directory: " <<dir);
    prlog("# files: "<<SysEval(tempstr()<<"find "<<dir<<" -type f | wc -l",FailokQ(true),1024));
    SysCmd(tempstr()<<"du -sh "<<dir);
}
//------------------------------------------------------------------------------

// get fill path of cache dir if exists
void gcache::ManageCacheDir() {
    strptr linkfile(".gcache");
    if (_db.cmdline.disable || _db.cmdline.install) {
        DeleteFile(linkfile);// remove the soft link
    }
    if (_db.cmdline.install) {
        _db.cmdline.enable=true;
        prlog("setting up gcache...");
        algo::CreateDirRecurse(_db.cmdline.dir, false, 0775);
        algo::SafeStringToFile(".",DirFileJoin(_db.cmdline.dir,".keep"));
        // inherit group writable flag
        SysCmd(tempstr()<<"chmod -R 0775 "<<_db.cmdline.dir);
        SysCmd(tempstr()<<"find "<<_db.cmdline.dir<<" -type d | xargs chmod g+s");
        prlog("done");
    }
    if (_db.cmdline.enable) {
        if (DirectoryQ(_db.cmdline.dir)) {
            SysCmd(tempstr()<<"ln -fsn "<<_db.cmdline.dir<<" .gcache");
        } else {
            prlog("cache directory "<<_db.cmdline.dir<<" doesn't exist. try with -install option");
            algo_lib::_db.exit_code=1;
        }
    }
    tempstr symlink = ReadLink(linkfile);
    if (_db.cmdline.stats) {
        // display stats for the enabled cache
        if (DirectoryQ(symlink)) {
            ShowStats(tempstr()<<linkfile<<"/");
        } else if (DirectoryQ(_db.cmdline.dir)) {
            prlog("gcache is not enabled. displaying stats for "<<_db.cmdline.dir);
            ShowStats(_db.cmdline.dir);
        }
    }
    _db.dir = GetFullPath(ch_N(symlink)?symlink:linkfile);
    if (!DirectoryQ(_db.dir)) {
        ch_RemoveAll(_db.dir);
    }
}

//------------------------------------------------------------------------------

// recursively remove older files
void gcache::RemoveOldFilesRecurse(strptr dir, algo::UnTime del_thresh, algo::UnTime access_thresh, bool subdir DFLTVAL(false)) {
    ind_beg(algo::Dir_curs,E,DirFileJoin(dir,"*")) {
        if (E.is_dir) {
            RemoveOldFilesRecurse(E.pathname, del_thresh, access_thresh,true);
            (void)RemDir(E.pathname); // kept if non-empty
        } else if (subdir) {
            strptr name = Pathcomp(E.pathname,".LL");
            cstring lock = tempstr() << name << ".lock";
            algo_lib::FFildes lockfd;
            if (name == E.pathname || !ValidQ(lockfd.fd = OpenRead(lock)) || Flock(lockfd.fd,LOCK_EX|LOCK_NB)) {
                _db.cleanreport.n_cachefile++;
                if (!del_thresh.value || E.mtime < del_thresh) {
                    _db.cleanreport.n_cachefile_del++;
                    (void)algo::DeleteFile(E.pathname);
                } else {
                    _db.cleanreport.n_cachefile_recent += E.mtime >= access_thresh;
                    _db.cleanreport.new_cachesize_mb += E.size;// count bytes, later will divide
                }
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Remove log entries older than THRESH
void gcache::CleanLog(algo::UnTime thresh) {
    algo_lib::FFildes out;
    tempstr temp;
    out.fd=CreateReplacementFile(_db.logfname,temp,0644);
    ind_beg(algo::FileLine_curs,line,_db.logfname) {
        report::gcache report;
        if (report::gcache_ReadStrptrMaybe(report,line)) {
            _db.cleanreport.n_logline++;
            if (report.starttime >= thresh) {
                // update modification time of cached file
                // this prevents recently accessed files from being deleted
                (void)close(OpenWrite(report.cached_file,algo_FileFlags_append).value);
                WriteFileX(out.fd,strptr_ToMemptr(tempstr()<<report<<eol));
            } else {
                _db.cleanreport.n_logline_del++;
            }
        }
    }ind_end;
    if (rename(Zeroterm(temp),Zeroterm(_db.logfname)) != 0) {
        verblog("gcache.notice"
                <<Keyval("fname",_db.logfname)
                <<Keyval("comment","failed to rewrite log file"));
        DeleteFile(temp);
    }
}

//------------------------------------------------------------------------------

// cache administration
void gcache::Clean() {
    if (ch_N(_db.dir)) {
        algo::UnTime gc_thresh = _db.report.starttime - algo::UnDiffHMS(1*24,0,0);
        algo::UnTime log_thresh = _db.report.starttime - algo::UnDiffHMS(2*24,0,0);
        algo::UnTime file_thresh = _db.cmdline.clean ? algo::UnTime() :_db.report.starttime - algo::UnDiffHMS(7*24,0,0);
        cstring gctime_file = DirFileJoin(_db.dir,"gc.time");
        bool requested = _db.cmdline.clean || _db.cmdline.gc;
        if (requested || algo::ModTime(gctime_file) < gc_thresh) {
            (void)close(OpenWrite(gctime_file).value);// update gc.time modification time
            CleanLog(log_thresh);
            RemoveOldFilesRecurse(_db.dir,file_thresh,log_thresh);
            _db.cleanreport.new_cachesize_mb /= 1024*1024;
        }
        // don't print cleanreport if the garbage collection
        // was triggered in the middle of a build (i.e. not requested by a command-line flag)
        if (requested) {
            prlog(_db.cleanreport);
        }
    }
}

// -----------------------------------------------------------------------------

static bool SourceQ(strptr fname) {
    strptr ext=GetFileExt(fname);
    return !StartsWithQ(fname,"-") && (ext==".h" || ext==".hpp" || ext==".cpp" || ext==".c" || ext==".c++"|| ext==".cc");
}

//------------------------------------------------------------------------------

// Process command line
void gcache::ProcessCommandLine() {
    bool is_target(false);
    if (ch_N(_db.dir) && cmd_N(_db.cmdline) >=1) {
        ind_beg(command::gcache_cmd_curs,arg,_db.cmdline) {
            if (bool_Update(is_target,false)) {
                _db.target = arg;
            } else {
                _db.do_not_compile  |= arg == "-E";
                _db.do_not_assemble |= arg == "-S";
                _db.do_not_link     |= arg == "-c";
                is_target            = arg == "-o";
                if (SourceQ(arg)) {
                    _db.report.source = arg;
                }
            }
        }ind_end;
    }
}

//------------------------------------------------------------------------------

// When precompiling replace, c by c-header, c++ by c++-header
static tempstr ConvertLang(strptr from, bool precomp) {
    tempstr to(from);
    if (precomp && (from == "c" || from == "c++")) {
        to << "-header";
    }
    return to;
}

//------------------------------------------------------------------------------

// Transform compilation command
// Recognize phase by target suffix:
// .ii -- preprocess -- replace -c by -E
// .gch -- precompile -- replace -x lang by -x lang-header
// When supplied, replace source, target, add extra flags.
tempstr gcache::MakeCmd(strptr source DFLTVAL(""), strptr target DFLTVAL(""), strptr flags DFLTVAL("")) {
    tempstr cmd;
    algo::ListSep ls(" ");
    bool is_target(false);
    bool is_lang(false);
    bool preproc = EndsWithQ(target,".ii");
    bool precomp = EndsWithQ(target,".gch");
    ind_beg(command::gcache_cmd_curs,arg,_db.cmdline) {
        tempstr new_arg;
        if (bool_Update(is_target,false)) {
            new_arg << (ch_N(target) ? target : arg);
        } else if (bool_Update(is_lang,false)) {
            new_arg << ConvertLang(arg,precomp);
        } else if (arg == "-c" && preproc) {
            new_arg << "-E";
        } else if (SourceQ(arg)) {
            new_arg << (ch_N(source) ? source : arg);
        } else {
            new_arg << arg;
            if (arg == "-o") {
                is_target = true;// expect target name
            } else if (arg == "-x") {
                is_lang = true;// expect lang
            }
        }
        cmd << ls << algo::strptr_ToBash(new_arg);
    }ind_end;
    if (ch_N(flags)) {
        cmd << ls << flags;
    }
    return cmd;
}

//------------------------------------------------------------------------------

// run specified cmd under bash, return exit code
int gcache::RunCmd(strptr cmd) {
    command::bash_proc bash;
    bash.cmd.c = cmd;
    int rc = bash_Exec(bash);
    return rc == -1 ? 127 : WIFEXITED(rc) ? WEXITSTATUS(rc) : 127;
}

//------------------------------------------------------------------------------

// Compute SHA1 digest from concatenation of str1 and str2
// return lowercase hex string
static tempstr Sha1(strptr str1, strptr str2) {
    Sha1Ctx sha1ctx;
    Update(sha1ctx,strptr_ToMemptr(str1));
    Update(sha1ctx,strptr_ToMemptr(str2));
    Finish(sha1ctx);
    return tempstr() << GetDigest(sha1ctx);
}

//------------------------------------------------------------------------------

// Compose cached file name from sha1 hash
tempstr gcache::CachedFile(strptr sha1) {
    tempstr out;
    out = DirFileJoin(_db.dir,ch_FirstN(sha1,2));
    mkdir(Zeroterm(out), 0775);
    out = DirFileJoin(out,ch_GetRegion(sha1,2,2));
    mkdir(Zeroterm(out), 0775);
    out = DirFileJoin(out,sha1);
    return out;
}

//------------------------------------------------------------------------------

// Copy contents of file descriptor FROM
// to file TO_FNAME
// The copying is done into a temporary file, which is then renamed to TO_FNAME
// If at any stage the operation fails, the temporary file is deleted
bool gcache::FdToFile(algo::Fildes from, algo::cstring &to_fname) {
    bool ret = true;
    algo_lib::FFildes to;
    algo::tempstr temp;
    to.fd = CreateReplacementFile(to_fname,temp,0644);
    if (ValidQ(to.fd)) {
        bool use_cfr = true;
        bool ok =false;
        // try using in-kernel copy
        if (use_cfr) {
            struct stat stat;
            errno_vrfy(fstat(from.value, &stat)==0,"fstat");
            ssize_t res= copy_file_range(from.value, NULL, to.fd.value, NULL, stat.st_size, 0);
            ok = res==stat.st_size;
            if (ok) {
                _db.report.copy_file_range=true;
            }
        }
        // try the old-fashioned way
        if (!ok) {
            char buf[BUFSIZ];
            int size;
            while ((size = read(from.value,buf,sizeof buf)) > 0) {
                if (!(write(to.fd.value,buf,size) == size)) {
                    ret=false;
                    break;
                }
            }
            if (size!=0) {
                ret=false;// last read must be a zero-byte read
            }
        }
        if (ret) {
            // this may fail if target directory is deleted while the
            // function is executing
            ret = rename(Zeroterm(temp),Zeroterm(to_fname))==0;
            if (!ret) {
                verblog("gcache.notice"
                        <<Keyval("from",to_fname)
                        <<Keyval("to",to_fname)
                        <<Keyval("comment","rename FROM->TO failed"));
            }
        }
    } else {
        verblog("gcache.notice"
                <<Keyval("fname",to_fname)
                <<Keyval("comment","create replacement file for fname failed"));
        ret=false;
    }
    // clean up temporary file if we didn't end up moving it into place
    if (!ret) {
        DeleteFile(temp);
    }
    return ret;
}

//------------------------------------------------------------------------------

// Append report to log file
void gcache::Log() {
    if (ch_N(_db.dir) && cmd_N(_db.cmdline) > 0 && _db.cached != "") {
        algo_lib::FFildes logfile;
        logfile.fd=OpenWrite(DirFileJoin(_db.dir,"log.ssim"),algo_FileFlags_append);
        tempstr out;
        out<<_db.report<<eol;
        WriteFile(logfile.fd, (u8*)out.ch_elems, out.ch_n);
    }
}

// -----------------------------------------------------------------------------

void gcache::Report() {
    if (_db.cmdline.report) {
        prlog(_db.report);
    } else if (algo_lib::_db.cmdline.verbose) {
        verblog(_db.report);
    }
    if (_db.cmdline.hitrate && ch_N(_db.dir)) {
        int nrun=0;
        int nhit=0;
        int npchhit=0;
        ind_beg(algo::FileLine_curs,line,_db.logfname) {
            report::gcache report;
            if (report::gcache_ReadStrptrMaybe(report,line)) {
                if (report.starttime >= _db.cmdline.after) {
                    nrun++;
                    nhit+=report.hit;
                    npchhit+=report.pch_hit;
                }
            }
        }ind_end;
        report::gcache_hitrate rep;
        rep.hitrate = tempstr() << nhit*100/i32_Max(nrun,1)<<"%";
        rep.pch_hitrate = tempstr() << npchhit*100/i32_Max(nrun,1)<<"%";
        prlog(rep);
    }
}

//------------------------------------------------------------------------------

// Precompile header
// Parse preprocessed file
// Get source line number information
// as per https://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
//   `# linenum filename flags`
//   - flag 1 - start of included file;
//   - flag 2 - returning to a file after having inclided another file.
// Build index array with the following information:
// - pointer to parent
// - name - included file name
// - begin - character position of file begin (including flag 1 marker);
// - inner_end - character position of file end (excluding flag 2 marker);
// - outer_end - character position of file end (including flag 2 marker);
// - mlines_before - number of meaningful source lines before start of the file.
//
// The following function prototype is used as directive that this
// file is eligible to be precompiled:
//   void __gcache_pragma_pch_preprocess();
//
// Important condition is that there is no any meaningful source line before this file.
// Note that only one precompiled header is possible per compilation unit.
// However, it is possible to put the directive in multiple files.
// As result, the latest file  having no any meaningful source line before is taken.
//
void gcache::Pch() {
    FHeader &root = header_Alloc(); // entire preproc_file
    FHeader *cur = &root;
    FHeader *pch = NULL;
    int mlines(0); // C++ meaningful lines
    int line_begin(0);
    ind_beg(algo::Line_curs,line,_db.preproc_text) {
        int line_end = ind_curs(line).contents.index;
        algo::StringIter it(line);
        if (SkipChar(it,'#')) {
            u32 lineno(0);
            cstring file;
            u32 flag(0);
            bool ok = TryParseU32(it, lineno)
                && algo::cstring_ReadCmdarg(file,it.Ws(),true);
            while (ok && TryParseU32(it,flag)) {
                if (flag==1) {
                    FHeader *parent = cur;
                    cur = &header_Alloc();
                    cur->parent = parent;
                    cur->name = file;
                    cur->begin = line_begin;
                    cur->mlines_before = mlines;
                } else if (flag==2) {
                    cur->inner_end = line_begin;
                    cur->outer_end = line_end;
                    verblog(*cur);
                    cur = cur->parent;
                    if (!cur) {
                        pch = NULL;
                        break;
                    }
                }
            }
        } else {
            mlines += ch_N(Trimmed(line)) ? 1 : 0;
            if (SkipStrptr(it.Ws(),"void")
                && SkipStrptr(it.Ws(),"__gcache_pragma_pch_preprocess")
                && SkipChar(it.Ws(),'(')
                && SkipChar(it.Ws(),')')
                && SkipChar(it.Ws(),';')) {
                if (!cur->mlines_before) {
                    pch = cur;
                }
            }
        }
        line_begin = line_end;
    }ind_end;
    if (cur != &root) {
        prlog(cur);
        prerr(_db.preproc_file<<": gcache warning: File nesting problem");
        pch = NULL;
    }
    if (pch && pch != &root) {
        strptr  text   = ch_GetRegion(_db.preproc_text,pch->begin,pch->inner_end-pch->begin);
        cstring sha1   = Sha1(MakeCmd(pch->name,tempstr()<<pch->name<<".gch"),text);
        cstring base   = CachedFile(sha1);
        cstring h      = tempstr() << base << ".h";
        cstring gch    = tempstr() << base << ".gch";
        cstring tmp    = tempstr() << base << ".tmp.gch";
        cstring lock   = tempstr() << base << ".lock";
        _db.lockfd.fd  = OpenWrite(lock);
        // First try to acquire exclusive lock, build gch if does not exist.
        bool build = (!FileQ(gch) || _db.cmdline.force) && Flock(_db.lockfd.fd,LOCK_EX|LOCK_NB);
        if (build) {
            StringToFile(text,h);
            algo_lib::_db.exit_code = RunCmd(MakeCmd(h,tmp,"-fpreprocessed"));
            DeleteFile(h);
            if (algo_lib::_db.exit_code) {
                DeleteFile(tmp);
                DeleteFile(gch);
            } else if (rename(Zeroterm(tmp),Zeroterm(gch))!=0){
                DeleteFile(tmp);
                prerr("gcache.warning"
                      <<Keyval("from",tmp)
                      <<Keyval("to",gch)
                      <<Keyval("comment",tempstr()<<"rename FROM->TO failed: "<<strerror(errno)));
            }
            Flock(_db.lockfd.fd,LOCK_UN);
        }
        // Then try to acquire shared lock (or downgrade if already held),
        // this will protect gch from deletion.
        bool use = FileQ(gch) && (!_db.cmdline.force || build) && Flock(_db.lockfd.fd,LOCK_SH|LOCK_NB);
        if (use) {
            FHeader *top = pch;
            for(; top->parent != &root; top = top->parent) {}
            cstring ii;
            ii << ch_FirstN(_db.preproc_text,top->begin);
            ii << "#pragma GCC pch_preprocess \"" << gch << "\"" << eol;
            ii << ch_GetRegion(_db.preproc_text,top->begin,pch->begin-top->begin);
            ii << ch_RestFrom(_db.preproc_text,pch->outer_end);
            StringToFile(ii,_db.preproc_file);
        }
        _db.report.pch_hit = !build && use;
        _db.report.pch_file = gch;
        _db.report.pch_source = pch->name;
    } else {
        // this is annoying in compilation log, move to gcache log
        // prerr(_db.preproc_file<<": gcache notice: Could not determine header to precompile");
    }
}

//------------------------------------------------------------------------------

// main routine
void gcache::Main() {
    _db.report.starttime=algo::CurrUnTime();
    ManageCacheDir();
    _db.logfname=DirFileJoin(_db.dir,"log.ssim");
    Clean();
    ProcessCommandLine();
    bool runnable = cmd_N(_db.cmdline);
    bool cacheable = runnable
        && ch_N(_db.dir)
        && !_db.do_not_compile
        && !_db.do_not_assemble
        && _db.do_not_link
        && ch_N(_db.target);
    if (cacheable) {
        _db.preproc_file = tempstr() << _db.target << ".ii";// g++ needs to recognize this as a c++ file
        cstring preproc_cmd = MakeCmd("",_db.preproc_file);// preprocessing command line
        algo_lib::_db.exit_code = RunCmd(preproc_cmd);
        if (!algo_lib::_db.exit_code) {
            ch_Reserve(_db.preproc_text,GetFileSize(_db.preproc_file));
            _db.preproc_text = FileToString(_db.preproc_file);
            _db.report.preproc_size = ch_N(_db.preproc_text);
            // '#pragma GCC pch_preprocess' does not exist in preprocessed files;
            // full text of prepocessed header is included
            cstring sha1 = Sha1(preproc_cmd,_db.preproc_text);
            _db.cached = CachedFile(sha1);
            _db.report.cached_file=_db.cached;
            // open cached file
            // this ensures the contents of the file are readable
            // even if the file is deleted while the function runs
            algo_lib::FFildes from;
            if (_db.cmdline.force || !ValidQ(from.fd = OpenRead(_db.cached,algo::FileFlags()))) {
                Pch(); // rewrites _db.preproc_file
                if (!algo_lib::_db.exit_code) {
                    algo_lib::_db.exit_code = RunCmd(MakeCmd(_db.preproc_file,"","-fpreprocessed"));
                }
                if (!algo_lib::_db.exit_code) {
                    from.fd = OpenRead(_db.target,algo::FileFlags());
                    // save built file to cache
                    if (ValidQ(from.fd)) {
                        FdToFile(from.fd, _db.cached);
                    }
                }
            } else {
                // copying from cache to object file
                // if this fails, set non-zero exit code
                if (!FdToFile(from.fd, _db.target)) {
                    prerr("gcache.error"
                          <<Keyval("from",_db.cached)
                          <<Keyval("to",_db.target)
                          <<Keyval("comment","copying cached file to target failed"));
                    algo_lib::_db.exit_code=1;
                }
                _db.report.hit=true; // cache hit
            }
            DeleteFile(_db.preproc_file);
        } // else preproc failed
    } else if (runnable) {
        algo_lib::_db.exit_code = RunCmd(MakeCmd());
    }

    _db.report.elapsed_sec=algo::ToSecs(algo::CurrUnTime()-_db.report.starttime);
    Log();
    Report();
}
