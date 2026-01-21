// Copyright (C) 2024,2026 AlgoRND
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
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/scan.cpp
//

#include "include/algo.h"
#include "include/abt.h"

// -----------------------------------------------------------------------------

static tempstr RemoveDots(strptr path_) {
    tempstr path(path_);
    int i=-1;
    while ((i = FindStr(path,"/../")) !=-1) {
        path = DirFileJoin(Pathcomp(ch_FirstN(path,i),"/RL"), ch_RestFrom(path,i+4));
    }
    return path;
}

// -----------------------------------------------------------------------------

// Locate file INCL for source file SRCFILE
// and return the resulting record (possibly NULL)
// IS_SYS indicates whether the file was included with <>'s (and so uses the system include
// search path)
static abt::FInclude *FindInclude(abt::FSrcfile &srcfile, strptr incl, bool is_sys) {
    strptr dir = GetDirName(srcfile.srcfile);
    tempstr fname(incl);
    bool existed = abt::ind_srcfile_Find(incl);
    bool found = existed || abt::GetFilestat(incl).exists;

    // try path relative to source file
    if (!found) {
        fname = RemoveDots(DirFileJoin(dir,incl));
        found = abt::GetFilestat(fname).exists;
    }
    if (!found && is_sys) {
        ind_beg(abt::_db_sysincl_curs,sysincl,abt::_db) {
            fname = tempstr() << sysincl << algo::MaybeDirSep << incl;
            found = abt::GetFilestat(fname).exists;
            if (found) {
                break;
            }
        }ind_end;
    }
    tempstr include_key =tempstr()<<srcfile.srcfile<<":"<<fname;
    abt::FInclude *include = abt::ind_include_Find(include_key);// include -- relation on two srcfiles
    if (found && !include) {
        abt::ind_srcfile_GetOrCreate(fname); // create missing srcfile
        include          = &abt::include_Alloc();
        include->include = include_key;
        include->sys     = is_sys;
        include_XrefMaybe(*include);
    }
    return include;
}

// -----------------------------------------------------------------------------

// Build a global list of system include paths.
// This uses the first selected builddir, which is not a problem in practice
// but theoretically different builddirs have different sysincls
static void ComputeSysincl() {
    bool success=false;
    ind_beg(abt::_db_builddir_curs,builddir,abt::_db) if (builddir.select) {
        // this it he command line that returns the list
        // of system include paths on this machine
        tempstr text;
        if (compiler_Get(builddir) == dev_Compiler_compiler_cl) {
            text = tempstr() << "#include <...>\n"
                             << algo::SysEval("cygpath -u -p \"\\$INCLUDE\"",FailokQ(true),1024*1024)
                             << "\n"
                             << "End of search list\n";
        } else {
            tempstr cmd;
            cmd << compiler_Get(builddir) << " -x c++ -E -v - </dev/null 2>&1";
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
            if (inside && abt::
                GetFilestat(path).isdir) {
                abt::sysincl_Alloc() = Trimmed(path);
            }
        }ind_end;
        vrfy(success,
             tempstr("abt.sysincl_warning")
             <<Keyval("compiler",compiler_Get(builddir))
             <<Keyval("comment", "cannot compute list of system include paths"));
        // #AL# compute the system includes once!
        break;
    }ind_end;
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
                abt::FInclude *include = FindInclude(*srcfile,incl,is_sys);
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

// This function always scans the entire graph of all sources
void abt::ScanSrcfile() {
    if (abt::_db.cmdline.printcmd || abt::_db.cmdline.jcdb!="") {
        // if generating a bootstrap file,don't require the target compiler
        // to be present.
    } else {
        ComputeSysincl();
    }

    // walk over target sources; each file corresponds to a single file; create Srcfile records
    ind_beg(abt::_db_targsrc_curs, targsrc, abt::_db) {
        if (SourceQ(targsrc)) {
            tempstr pathname(src_Get(targsrc));
            vrfy(abt::GetFilestat(pathname).exists, tempstr()
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
        srcfile->objkey = abt::GetObjkey(srcfile->srcfile);
        // read file, scan includes
        ScanHeaders(srcfile);
        abt::zs_srcfile_read_RemoveFirst();
    }
}
