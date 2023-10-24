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
// Target: gcache (exe) -- Compiler cache
// Exceptions: yes
// Source: cpp/gcache/gcache.cpp
//

#include "include/algo.h"
#include "include/gcache.h"

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
void gcache::RemoveOldFilesRecurse(strptr dir, algo::UnTime del_thresh, algo::UnTime access_thresh) {
    ind_beg(algo::Dir_curs,E,DirFileJoin(dir,"*")) {
        if (E.is_dir) {
            RemoveOldFilesRecurse(E.pathname, del_thresh, access_thresh);
            (void)RemDir(E.pathname); // kept if non-empty
        } else {
            _db.cleanreport.n_cachefile++;
            if (E.mtime < del_thresh) {
                _db.cleanreport.n_cachefile_del++;
                (void)algo::DeleteFile(E.pathname);
            } else {
                _db.cleanreport.n_cachefile_recent += E.mtime >= access_thresh;
                _db.cleanreport.new_cachesize_mb += E.size;// count bytes, later will divide
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
    errno_vrfy_(rename(Zeroterm(temp),Zeroterm(_db.logfname))==0);
}

//------------------------------------------------------------------------------

// cahe dir: remove files older than one week
void gcache::RemoveOldFiles() {
    if (ch_N(_db.dir)) {
        if (_db.cmdline.clean) {
            prlog("cleaning the cache...");
            SysCmd(tempstr()<<"find "<<_db.dir<<" -type f -wholename \"*/*/*\" -delete");
            SysCmd(tempstr()<<"find "<<_db.dir<<" -type d -empty -delete");
        } else {
            bool doit=_db.cmdline.gc;// this flag forces cleaning
            cstring gctime_file = DirFileJoin(_db.dir,"gc.time");
            algo::UnTime gc_thresh = _db.report.starttime - algo::UnDiffHMS(1*24,0,0);
            algo::UnTime log_thresh = _db.report.starttime - algo::UnDiffHMS(2*24,0,0);
            algo::UnTime file_thresh = _db.report.starttime - algo::UnDiffHMS(7*24,0,0);
            if (!doit) {
                doit = algo::ModTime(gctime_file) < gc_thresh;
            }
            if (doit) {
                (void)close(OpenWrite(gctime_file).value);// update gc.time modification time
                CleanLog(log_thresh);
                RemoveOldFilesRecurse(_db.dir,file_thresh,log_thresh);
                _db.cleanreport.new_cachesize_mb /= 1024*1024;
                prlog(_db.cleanreport);
            }
        }
    }
}

//------------------------------------------------------------------------------
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
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static bool SourceQ(strptr fname) {
    strptr ext=GetFileExt(fname);
    return !StartsWithQ(fname,"-") && (ext==".h" || ext==".hpp" || ext==".cpp" || ext==".c" || ext==".c++");
}

//------------------------------------------------------------------------------

// Transform compilation command
// If COMPILING:
// - replace -c by "-fpreprocessed -c _db.preproc_file"
// OTHERWISE (preprocessing)
// - replace -o target by temporary file
// - replace -c by -E
// If the gcache is disabled (_db.dir is empty)
// or the preprocessed file doesn't exist (_db.preproc_file is empty -- happens
// when the command line has -E or -S options), then the command is returned as-is
tempstr gcache::MakeCmd(bool compiling) {
    tempstr cmd;
    algo::ListSep ls(" ");
    bool is_target(false);
    bool as_is = !ch_N(_db.dir) || !ch_N(_db.preproc_file);
    ind_beg(command::gcache_cmd_curs,arg,_db.cmdline) {
        if (as_is) {
            cmd << ls << arg;
        } else if (bool_Update(is_target,false)) {
            cmd << ls << algo::strptr_ToBash(compiling ? _db.target : _db.preproc_file);
        } else if (arg == "-c") {
            cmd << ls << (compiling ? "-fpreprocessed -c" : "-E");
        } else {
            cmd << ls << algo::strptr_ToBash(compiling && SourceQ(arg) ? _db.preproc_file : arg);
            if (arg == "-o") {
                is_target = true;// expect target name
            }
        }
    }ind_end;
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

// Update SHA1 from file
void gcache::UpdateFromFile(Sha1Ctx &sha1ctx, strptr filename) {
    algo_lib::FFildes fd;
    fd.fd = OpenRead(filename);
    u8 buf[BUFSIZ];
    int size;
    while ((size = read(fd.fd.value,buf,sizeof buf)) > 0) {
        Update(sha1ctx,algo::memptr(buf,size));
    }
}

//------------------------------------------------------------------------------

// if command is eligible to be cached:
// - compose preprocessor command line
// - preprocess source
// - compute SHA1 hash from command line, and preprocessed content
// - compose path of cached file
void gcache::ComputeHash() {
    bool cacheable = ch_N(_db.dir)
        && !_db.do_not_compile
        && !_db.do_not_assemble
        && _db.do_not_link
        && ch_N(_db.target);
    if (cacheable) {
        _db.preproc_file = tempstr() << _db.target << ".ii";// g++ needs to recognize this as a c++ file
        cstring preproc_cmd = MakeCmd(false);// preprocessing command line
        if (!RunCmd(preproc_cmd)) {
            _db.report.preproc_size=GetFileSize(_db.preproc_file);
            Sha1Ctx sha1ctx;
            Update(sha1ctx,strptr_ToMemptr(preproc_cmd));
            UpdateFromFile(sha1ctx,_db.preproc_file);
            // '#pragma GCC pch_preprocess' does not exist in preprocessed files;
            // full text of prepocessed header is included
            Finish(sha1ctx);
            cstring sha1 = tempstr() << GetDigest(sha1ctx);
            _db.cached = DirFileJoin(_db.dir,ch_FirstN(sha1,2));
            mkdir(Zeroterm(_db.cached), 0775);
            _db.cached = DirFileJoin(_db.cached,ch_GetRegion(sha1,2,2));
            mkdir(Zeroterm(_db.cached), 0775);
            _db.cached = DirFileJoin(_db.cached,sha1);
            _db.report.cached_file=_db.cached;
        }
    }
}

//------------------------------------------------------------------------------

// Copy from one fd to another
void gcache::CopyX(algo::Fildes from, algo::Fildes to) {
    bool use_cfr = true;
    bool ok =false;
    // try using in-kernel copy
    if (use_cfr) {
        struct stat stat;
        errno_vrfy(fstat(from.value, &stat)==0,"fstat");
        ssize_t res= copy_file_range(from.value, NULL, to.value, NULL, stat.st_size, 0);
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
            errno_vrfy_(write(to.value,buf,size) == size);
        }
        errno_vrfy(size==0,"read()");
    }
}

//------------------------------------------------------------------------------

// if cached file exists, copy it to target;
// othewise run command, atomically copy result to cache.
void gcache::Cache() {
    algo_lib::FFildes from, to;
    if (ch_N(_db.cached) && ValidQ(from.fd = OpenRead(_db.cached,algo::FileFlags()))) {
        to.fd = OpenWrite(_db.target,algo_FileFlags__throw);
        CopyX(from.fd,to.fd);
        _db.report.hit=true; // cache hit
    } else {
        cstring compile_cmd = MakeCmd(true);// compiling command line
        if (ch_N(compile_cmd)) {
            algo_lib::_db.exit_code = RunCmd(compile_cmd);
        }
        if (!algo_lib::_db.exit_code && ch_N(_db.target) && ch_N(_db.cached)) {
            from.fd = OpenRead(_db.target,algo_FileFlags__throw);
            cstring temp;
            to.fd = CreateReplacementFile(_db.cached,temp,0644);
            CopyX(from.fd,to.fd);
            errno_vrfy_(rename(Zeroterm(temp),Zeroterm(_db.cached))==0);
        }
    }
}

// -----------------------------------------------------------------------------

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
        ind_beg(algo::FileLine_curs,line,_db.logfname) {
            report::gcache report;
            if (report::gcache_ReadStrptrMaybe(report,line)) {
                if (report.starttime >= _db.cmdline.after) {
                    nrun++;
                    nhit+=report.hit;
                }
            }
        }ind_end;
        prlog(nhit*100/i32_Max(nrun,1)<<"%");
    }
}

//------------------------------------------------------------------------------

// main routine
void gcache::Main() {
    _db.report.starttime=algo::CurrUnTime();
    ManageCacheDir();
    _db.logfname=DirFileJoin(_db.dir,"log.ssim");
    RemoveOldFiles();
    ProcessCommandLine();
    ComputeHash();
    Cache();

    // delete temporary file
    if (_db.preproc_file != "") {
        DeleteFile(_db.preproc_file);
    }

    _db.report.elapsed_sec=algo::ToSecs(algo::CurrUnTime()-_db.report.starttime);
    Log();
    Report();
}
