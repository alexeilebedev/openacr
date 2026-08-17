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
        if (!algo::SaveFile(".",DirFileJoin(_db.cmdline.dir,".keep"),"gcache.error","cache directory marker could not be written")) {
            algo_lib::_db.exit_code += 1;
        }
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
            algo_lib::_db.exit_code += 1;
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
                    gcache::FCachefile &cachefile = cachefile_Alloc();
                    cachefile.pathname = E.pathname;
                    cachefile.mtime = E.mtime;
                    cachefile.size = E.size;
                    (void)cachefile_XrefMaybe(cachefile);// enters bh_cachefile (ordered by last use)
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
                // A log line younger than THRESH says its entry was used
                // recently, and the age pass below reads an entry's modification
                // time rather than the log, so the entry's time is moved up to
                // now and the entry survives the pass.
                // Two things the refresh must not do follow from where the line
                // and the entry can disagree. The entry a recent line names can
                // be gone already -- evicted by the byte budget, or deleted by
                // the age pass of an earlier run -- and a refresh that opens the
                // path for writing creates it, empty, at a key a compile is about
                // to ask for. And a refresh that opens an existing entry without
                // writing to it moves no time at all, which leaves an entry in
                // daily use aging out of the retention window anyway.
                // Setting the time through the path does both jobs and neither
                // harm: it fails on an entry that is gone, and it moves the time
                // of one that is there. gcache.EmptyEntry pins both halves.
                // A log line names two entries when the compile it describes used a
                // precompiled header, and the age pass reads the times of both. The
                // compiles that hit that header write nothing to it -- they take a
                // shared lock and read it -- so its time stands still at the moment
                // it was built, and a header a build has used every day since is
                // deleted at the end of the retention window and precompiled again.
                // Both paths are therefore refreshed, and a line whose compile used
                // no precompiled header carries an empty path, which fails the way a
                // deleted entry does.
                (void)utimensat(AT_FDCWD,Zeroterm(report.cached_file),NULL,0);
                (void)utimensat(AT_FDCWD,Zeroterm(report.pch_file),NULL,0);
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

// Delete surviving cache files, oldest last-use first, until the cache fits
// the -maxmb budget.
// The age-based pass alone does not bound the cache: a machine whose builds
// span several configurations (release, coverage, debug) can write tens of
// gigabytes of objects inside the one-week retention window and fill the disk
// it shares with the builds themselves.  What the disk runs out of is bytes,
// not age, so a byte budget backs the age pass.  Eviction goes in last-use
// order: CleanLog has already refreshed the mtime of every file the access
// log saw used within two days, so mtime ranks files by last use.  A file
// written after the walk collected it carries a fresh mtime and sits at the
// far end of the heap, so an active compile's output is the last candidate.
static void EvictToBudget() {
    i64 budget = gcache::_db.cmdline.maxmb * i64(1024*1024);
    while (gcache::_db.cleanreport.new_cachesize_mb > budget && !gcache::bh_cachefile_EmptyQ()) {
        gcache::FCachefile &cachefile = *gcache::bh_cachefile_RemoveFirst();
        (void)algo::DeleteFile(cachefile.pathname);
        gcache::_db.cleanreport.new_cachesize_mb -= cachefile.size;
        gcache::_db.cleanreport.n_cachefile_evict++;
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
            EvictToBudget();
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

// Derive the .gcno notes path of a compile whose object file is O_TARGET.
// gcc names the notes after the object with the object's extension replaced, and
// appends to a name that carries none, so an object named x.obj is answered with
// x.gcno and one named foo with foo.gcno. The name is therefore derived rather
// than read as a .o: an object name of any other shape writes notes all the same,
// and a compile whose notes go unrecognized is published as a bare object, so a
// later hit restores a fresh object beside notes that are absent or left over from
// another compile.
// An output named the way a precompiled header is named is derived the same way,
// because the derivation follows the output name and not what the compile produces:
// gcc answers `-c -o h.h.gch` with h.h.gcno, which is that name with its extension
// replaced. The other spelling, h.h.gch-h.gcno, comes from a compile with no -c, and
// gcache caches nothing without -c. What gcache itself leaves at such an output is
// an ordinary object rather than a precompiled header, since the compile it runs
// takes the preprocessed text of the translation unit, and the notes name is the one
// derived here either way.
// gcache.CoverageFlag compiles a notes-writing flag into an object named x.obj, one
// named foo and one named h.h.gch, and pins that the notes come back from a hit on
// each.
static tempstr GcnoSibling(strptr o_target) {
    tempstr ret;
    ret << StripExt(o_target) << ".gcno";
    return ret;
}

//------------------------------------------------------------------------------

// Clear the path of cache entry CACHE_FNAME of anything a publish cannot replace.
// An earlier per-file cache format wrote a directory where this one writes a
// single file, and a cache directory shared between builds can still hold one.
// A rename cannot replace a directory, so a publish that left it standing would
// never take the path, and the translation unit whose key names it would miss on
// every build from then on. Dropping it lets the publish take the path once,
// whichever of the two entry formats is being written.
static void DropStaleEntry(strptr cache_fname) {
    if (DirectoryQ(cache_fname)) {
        RemDirRecurse(cache_fname, true);
    }
}

//------------------------------------------------------------------------------

// Publish a --coverage compile's object file O_FNAME and its .gcno notes
// GCNO_FNAME as one cache file CACHE_FNAME: an 8-byte offset to where the .gcno
// begins, then the .o bytes, then the .gcno bytes.  A cache entry stays a
// single file with a single mtime, so the time-based cleanup -- its gc age
// test and CleanLog's access-time refresh -- handles coverage entries the same
// as bare-.o ones; and the single write is one atomic rename,
// so a build sharing the cache can never pair a fresh .o with a stale .gcno.
// MmapFile.text is the file's exact bytes; FileToString must NOT be used here
// -- it stops at the first NUL and would truncate the object file.
// An entry is published only when both halves are there to publish.  A half
// that reads as no bytes -- the file is gone, or is a directory, or cannot be
// mapped -- would go into the blob as an empty region, and a later hit would
// then restore one half of the pair and leave whatever sits in the other
// half's place: exactly the mismatch the single-file format exists to
// prevent.  Skipping the publish costs the next build a cache miss, which is
// what a missing entry always costs, so a half-readable pair is reported and
// not stored.
static void FilesToBlob(strptr o_fname, strptr gcno_fname, const strptr &cache_fname) {
    algo_lib::MmapFile o_file;
    algo_lib::MmapFile gcno_file;
    bool o_ok = MmapFile_Load(o_file, o_fname) && o_file.text.n_elems > 0;
    bool gcno_ok = FileQ(gcno_fname) && MmapFile_Load(gcno_file, gcno_fname) && gcno_file.text.n_elems > 0;
    if (!o_ok || !gcno_ok) {
        prerr("gcache.warning"
              <<Keyval("o_file",o_fname)
              <<Keyval("gcno_file",gcno_fname)
              <<Keyval("comment","cache entry not published: a coverage entry needs both halves"));
    } else {
        u64 gcno_off = sizeof(gcno_off) + u64(o_file.text.n_elems);
        cstring blob;
        blob << strptr((char*)&gcno_off, sizeof(gcno_off));
        blob << o_file.text;
        blob << gcno_file.text;
        DropStaleEntry(cache_fname);
        // publishing is the last step of a compile that already succeeded, and the
        // write is one atomic rename, so a failure leaves no partial entry and
        // costs the next build a cache miss and nothing else. It is reported and
        // the compile still counts as successful: a full cache must not fail a
        // build that produced the object it was asked for.
        // The entry is written whatever the path already holds, so its
        // modification time is the time of this publish. That time is what the
        // cleanup reads: an entry older than the retention window is deleted, and
        // the byte budget evicts in oldest-last-use order. So a publish that left
        // an existing entry's time alone would offer the cleanup an entry that
        // looks a week older than the build that wrote it.
        algo::SaveFileFresh(blob, cache_fname, "gcache.warning", "cache entry could not be published", 0644);
    }
}

//------------------------------------------------------------------------------

// Split a cached coverage blob (see FilesToBlob), read through the held fd
// FROM, into the object file O_FNAME and its .gcno sibling GCNO_FNAME.  The
// whole pair comes from one cache file published by a single atomic rename, so
// it is always mutually consistent; a concurrent republish is one more rename
// and cannot skew the halves.  Returns false when the bytes are not a blob
// holding two non-empty halves -- a legacy bare-.o entry, a leftover
// directory, a short read, or an offset sitting at either end of the blob --
// so the caller recompiles rather than restore half a pair.
static bool BlobToFiles(algo::Fildes from, strptr o_fname, strptr gcno_fname) {
    bool ret = false;
    cstring blob;
    char buf[BUFSIZ];
    int size;
    while ((size = read(from.value, buf, sizeof buf)) > 0) {
        blob << strptr(buf, size);
    }
    u64 gcno_off = 0;
    if (size == 0 && ch_N(blob) >= int(sizeof(gcno_off))) {
        memcpy(&gcno_off, blob.ch_elems, sizeof(gcno_off));
    }
    // The offset has to leave bytes on both sides of itself. An offset equal to
    // the header size describes an empty object half, and one equal to the
    // blob's length describes empty notes; either way the entry can only
    // restore one half of a pair and leave whatever sits in the other half's
    // place, which is the stamp mismatch the format exists to prevent. Such an
    // entry counts as a miss, so the compile runs and republishes it.
    if (gcno_off > sizeof(gcno_off) && gcno_off < u64(ch_N(blob))) {
        strptr o_bytes = ch_GetRegion(blob, sizeof(gcno_off), gcno_off - sizeof(gcno_off));
        strptr gcno_bytes = ch_GetRegion(blob, gcno_off, u64(ch_N(blob)) - gcno_off);
        // restoring is the build step itself: an unwritten .o leaves the link
        // reaching for a file that is not there, or worse pairing a stale one
        // with a fresh .gcno, so the failure fails the run the same way the
        // bare-.o restore path does. The blob was valid, which is what RET
        // reports, so a recompile would hit the same unwritable target.
        // Either both halves land or neither is left fresh, since a mismatched
        // pair is what the blob format exists to prevent. A target path that
        // rejects the object but takes its notes -- the object's own name held
        // by a directory, or a quota the first write exhausts -- would leave
        // the notes of a compile whose object never arrived, and the notes'
        // write failing would leave the mirror image of that. So the notes are
        // written only after the object, and an object that outlives a failed
        // notes write is removed: a fresh object reads as up to date to the
        // build tool that asked for it, which would then skip the recompile
        // that fixes the pair. That removal is the one step whose failure the
        // run cannot repair, so it is reported on its own: the fresh object is
        // still standing next to notes that were never written.
        // Each half is written whatever its target path already holds, so each
        // comes out of the restore carrying the time of this hit. A build tool
        // asks for an object whose modification time sits behind its sources, and
        // a hit can find the target already holding the very bytes the cache has
        // -- a touched header that moved no preprocessed text keeps the cache key
        // and so keeps hitting. Leaving that object as it stands answers the
        // request without clearing what prompted it, and the next build asks for
        // the same translation unit again. gcache.HitMtime backdates a target and
        // pins that a hit leaves both halves newer than the moment it began.
        bool o_ok = algo::SaveFileFresh(o_bytes, o_fname, "gcache.error", "cached object file could not be restored", 0644);
        bool gcno_ok = true;
        if (o_ok) {
            gcno_ok = algo::SaveFileFresh(gcno_bytes, gcno_fname, "gcache.error", "cached coverage notes could not be restored", 0644);
        }
        if (o_ok && !gcno_ok) {
            bool del_ok = DeleteFile(o_fname);
            if (!del_ok) {
                algo::PrerrFileFail("gcache.error", o_fname, "restored object file could not be removed after its notes write failed");
            }
        }
        if (!o_ok || !gcno_ok) {
            algo_lib::_db.exit_code += 1;
        }
        ret = true;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Whether the cache entry held open on FD holds bytes a compile can be served
// from: a regular file that is not empty.
// A path under the cache directory can hold something no compile can use, in two
// shapes. An entry of no bytes is one -- a build that ran out of disk, an
// interrupted write by another tool, a cleanup that recreated the path of an entry
// it had deleted -- and a directory left at the path by the earlier per-file entry
// format is the other. Serving a compile from either restores an object that was
// never written: the empty file is renamed over the target and the compile reports
// success, so the link that follows is what fails, and the directory cannot be read
// at all, which fails the compile itself. Both shapes outlive the build that met
// them, because a hit does not republish the entry it read, so the next build with
// the same key meets the same entry and fails the same way.
// So an entry counts as a hit only in the one shape a compile can be served from,
// and everything else is a miss: the compile runs and publishes an entry with bytes
// in it over whatever stood at the path. The coverage format reaches the same
// verdict from the bytes themselves, since neither shape begins with an offset
// leaving bytes on both sides of itself.
// gcache.EmptyEntry pins both shapes in both entry formats.
static bool UsableEntryQ(algo::Fildes fd) {
    struct stat st;
    bool ret = ValidQ(fd) && fstat(fd.value,&st)==0 && S_ISREG(st.st_mode) && st.st_size > 0;
    return ret;
}

//------------------------------------------------------------------------------

// Whether the wrapped gcc invocation writes coverage notes next to its object
// file.
// A coverage entry is cached as a single .o+.gcno blob (FilesToBlob) rather than
// a bare .o, so this selects the cache format on both write and read. The blob
// exists to keep the two halves of one compile together, which makes it the
// right format for exactly those compiles that produce both halves.
// A compile that writes no notes has one half to cache and caches as a bare
// object; asking for the blob format there would leave every translation unit
// unpublishable and so permanently uncached. Reading the notes-writing compile
// as a bare one is the opposite mistake and just as silent: a hit then restores
// a fresh object beside notes that are absent or left over from another compile,
// and gcov reports the translation unit at no coverage with no diagnostic.
// So the whole set of spellings has to be recognized, and gcc's driver gives
// each notes-writing flag more than one. -ftest-coverage writes the notes, and
// --test-coverage is the driver's own long form of it; --coverage asks for
// instrumentation and notes together, and -coverage is that flag with one dash.
// -fprofile-arcs -- and --profile-arcs, its long form -- instruments the object
// and writes no notes file, so the long forms are not a rule about the number of
// dashes: the two spellings of -fprofile-arcs sit on the other side of it.
// gcache.CoverageFlag compiles each of these spellings twice and pins whether
// the notes come back.
static bool CoverageQ() {
    bool ret = false;
    ind_beg(command::gcache_cmd_curs, arg, gcache::_db.cmdline) {
        ret = ret
            || arg == "--coverage"
            || arg == "-coverage"
            || arg == "-ftest-coverage"
            || arg == "--test-coverage";
    }ind_end;
    return ret;
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
    // Exec hands back the raw wait status, so it is decoded here, and the shared
    // decode is the one that does it: a child that exited yields its exit code
    // and one killed by a signal yields 128 plus the signal, the convention a
    // shell reports the same death with. A child that never started is reported
    // as -1, which is no wait status at all, so that case is answered before the
    // decode with 127 -- the shell's own code for a command that did not run, and
    // a code an OOM-killed compiler can no longer be confused with.
    return rc == -1 ? 127 : algo::WaitStatusToExitCode(rc);
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
// Returns false when the precompiled header's own compile failed: the caller
// must not go on to the compile that would include it, and the failure is
// already counted in the run's exit code.
bool gcache::Pch() {
    bool ret = true;
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
            int rc = RunCmd(MakeCmd(h,tmp,"-fpreprocessed"));
            algo_lib::_db.exit_code += rc;
            DeleteFile(h);
            if (rc) {
                ret = false;
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
    return ret;
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
        int preproc_rc = RunCmd(preproc_cmd);
        algo_lib::_db.exit_code += preproc_rc;
        if (!preproc_rc) {
            ch_Reserve(_db.preproc_text,GetFileSize(_db.preproc_file));
            _db.preproc_text = FileToString(_db.preproc_file);
            _db.report.preproc_size = ch_N(_db.preproc_text);
            // '#pragma GCC pch_preprocess' does not exist in preprocessed files;
            // full text of prepocessed header is included
            cstring sha1 = Sha1(preproc_cmd,_db.preproc_text);
            _db.cached = CachedFile(sha1);
            _db.report.cached_file=_db.cached;
            // A --coverage compile emits a .gcno notes file next to the .o, and
            // gcov pairs a run's .gcda with that .gcno by a stamp; a cached .o
            // restored next to a .gcno from a different compile is rejected as a
            // stamp mismatch and the object reports 0% coverage.  Cache the two
            // as one blob (FilesToBlob) written with a single rename, so a
            // concurrent build sharing the cache can never pair a fresh .o with
            // a stale .gcno.  Non-coverage entries stay a bare .o.
            tempstr gcno_target = GcnoSibling(_db.target);
            bool coverage = CoverageQ();
            // hold the cached fd open so its contents stay readable even if the
            // file is deleted while this function runs
            algo_lib::FFildes from;
            from.fd = OpenRead(_db.cached,algo::FileFlags());
            // An entry is a hit only when its bytes can serve the format the
            // compile needs, which UsableEntryQ decides for both formats: an
            // entry that is not a regular file with bytes in it is a miss.
            // A coverage hit must split cleanly into (.o, .gcno); a legacy
            // bare-.o entry or a torn blob fails the split and recompiles.
            bool hit = !_db.cmdline.force && UsableEntryQ(from.fd);
            if (hit && coverage) {
                hit = BlobToFiles(from.fd, _db.target, gcno_target);
            } else if (hit) {
                if (!FdToFile(from.fd, _db.target)) {
                    prerr("gcache.error"
                          <<Keyval("from",_db.cached)
                          <<Keyval("to",_db.target)
                          <<Keyval("comment","copying cached file to target failed"));
                    algo_lib::_db.exit_code += 1;
                }
            }
            if (hit) {
                _db.report.hit=true; // cache hit
            } else {
                int compile_rc = 1;// a precompiled header that failed to build fails the compile
                if (Pch()) { // rewrites _db.preproc_file
                    compile_rc = RunCmd(MakeCmd(_db.preproc_file,"","-fpreprocessed"));
                    algo_lib::_db.exit_code += compile_rc;
                }
                if (!compile_rc && coverage) {
                    FilesToBlob(_db.target, gcno_target, _db.cached);
                } else if (!compile_rc) {
                    // the object the compile wrote gets a handle of its own: the
                    // cached fd is what holds the entry's contents readable, and a
                    // handle reassigned to another file closes neither
                    algo_lib::FFildes object;
                    object.fd = OpenRead(_db.target,algo::FileFlags());
                    // The rule that decides a hit decides a publish too: an entry
                    // of no bytes is of no use to the compile that reads it, so a
                    // target holding none is not published. A compile asked to
                    // write its object to /dev/null is the shape that reaches
                    // this, and it is reported the way FilesToBlob reports a
                    // coverage half it cannot publish -- a warning, with the
                    // compile still counting as successful.
                    bool empty = ValidQ(object.fd) && algo::GetFileSize(object.fd) == 0;
                    if (empty) {
                        prerr("gcache.warning"
                              <<Keyval("o_file",_db.target)
                              <<Keyval("comment","cache entry not published: an object file of no bytes"));
                    } else if (ValidQ(object.fd)) {
                        // the coverage publish clears the path the same way, and for
                        // the same reason: a rename cannot replace a directory
                        DropStaleEntry(_db.cached);
                        // same policy as the coverage publish in FilesToBlob:
                        // report and let the successful compile stand
                        if (!FdToFile(object.fd, _db.cached)) {
                            prerr("gcache.warning"
                                  <<Keyval("from",_db.target)
                                  <<Keyval("to",_db.cached)
                                  <<Keyval("comment","cache entry could not be published"));
                        }
                    }
                }
            }
            DeleteFile(_db.preproc_file);
        } // else preproc failed
    } else if (runnable) {
        algo_lib::_db.exit_code += RunCmd(MakeCmd());
    }

    _db.report.elapsed_sec=algo::ToSecs(algo::CurrUnTime()-_db.report.starttime);
    Log();
    Report();
}
