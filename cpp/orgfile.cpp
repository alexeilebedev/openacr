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
// Target: orgfile (exe) -- organize and deduplicate files by timestamp and by contents
// Exceptions: yes
// Source: cpp/orgfile.cpp
//

#include "include/algo.h"
#include "include/orgfile.h"

// -----------------------------------------------------------------------------

static bool TimeStruct_Match(TimeStruct &ts, strptr str, bool dirname) {
    bool ret=false;
    ind_beg(orgfile::_db_timefmt_curs,timefmt,orgfile::_db) {
        algo::StringIter iter(str);
        if (dirname == timefmt.dirname && algo::TimeStruct_Read(ts, iter, timefmt.timefmt)) {
            ret=true;
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Attempt to determine a file's year-month-date from
// its pathname.
// Photos are often stored in directories that look like
//   x/2008_02_03/IMG12343.CRW
// Or
//   x/2008-02-03/IMG12343.CRW
// In this case, extract 2008,02,03 as the date.
// Also check if the filename itself contains the timestamp, such as
// PSX_YYYYmmdd_hhmmss.jpg
//
// If this heuristic doesn't work, use files's modification time.
// (important -- not the creation timestamp; file may have been moved; this
//   changes creation timestamp but keeps the modification timestamp)
static bool GetTimestamp(strptr path, TimeStruct &ts) {
    bool ret = TimeStruct_Match(ts, Pathcomp(path,"/RL/RR"),true)
        || TimeStruct_Match(ts, algo::StripDirName(path),false);
    if (!ret) {// go by the modification date (creation date is unreliable)
        struct stat st;
        ret=stat(Zeroterm(tempstr(path)),&st)==0;
        algo::UnixTime mtime(st.st_mtime);
        ts=algo::GetLocalTimeStruct(mtime);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// sha1 returns
// SHA1 (.lesshst) = a11ef917ac87065eb0864bc86548d3a1f27e2bca
// sha1sum returns
// 1024f5693cc1a56e2f870385dab217829a875b41  README.md
// This function accepts both formats
static tempstr TrimHash(strptr in) {
    tempstr ret(Trimmed(Pathcomp(in,"=RR")));
    tempstr ret2(Trimmed(Pathcomp(ret," LL")));
    return ret2;
}

// -----------------------------------------------------------------------------

// Access filename entry for file FNAME.
// Also compute file's hash.
// filename->p_filehash fetches the file hash entry.
// A single filehash may have multiple filenames associated with it.
orgfile::FFilename *orgfile::AccessFilename(strptr fname) {
    orgfile::FFilename *filename = ind_filename_Find(fname);
    if (!filename) {
        filename = &filename_Alloc();
        tempstr cmd;
        cmd << _db.cmdline.hash;
        cmd << " ";
        strptr_PrintBash(fname,cmd);
        filename->filename = fname;
        filename->filehash = TrimHash(SysEval(cmd,FailokQ(true),1024));
        // cascade create filehash
        ind_filehash_GetOrCreate(filename->filehash);
        vrfy(filename_XrefMaybe(*filename),algo_lib::_db.errtext);
    }
    return filename;
}

// -----------------------------------------------------------------------------

// Determine new filename for FNAME.
tempstr orgfile::GetTgtFname(strptr pathname) {
    tempstr pattern(_db.cmdline.move);
    if (EndsWithQ(pattern,"/")) {
        pattern << StripDirName(pathname);
    }
    tempstr filename(StripDirName(pathname));
    Replace(pattern,"$filename",filename);
    Replace(pattern,"$ext",GetFileExt(pathname));
    Replace(pattern,"$basename",StripExt(filename));
    Replace(pattern,"$dir",GetDirName(pathname));
    TimeStruct ts;
    GetTimestamp(pathname,ts);
    tempstr tgtfname;
    TimeStruct_Print(ts, tgtfname, pattern);
    return tgtfname;
}

// -----------------------------------------------------------------------------

// Read filenames from STDIN.
// For each file, compute its file hash.
// Delete file file if it's a duplicate (and -commit was specified)
void orgfile::DedupFile(strptr pathname) {
    orgfile::FFilename *srcfilename = AccessFilename(pathname);
    if (Regx_Match(_db.cmdline.dedup, pathname)) {
        if (c_filename_N(*srcfilename->p_filehash) > 1) {// can dedup?
            orgfile::dedup dedup;
            dedup.original = c_filename_Find(*srcfilename->p_filehash,0)->filename;
            dedup.duplicate = pathname;
            dedup.comment = "contents are identical (based on hash)";
            prlog(dedup);
            if (orgfile::_db.cmdline.commit) {// do dedup
                if (DeleteFile(srcfilename->filename)) {
                    filename_Delete(*srcfilename);
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Move file from FROM to TO
// Use the move command as it can transfer files across filesystem boundaries
static int SystemMv(algo::strptr from, algo::strptr to) {
    CreateDirRecurse(GetDirName(to));
    tempstr cmd;
    cmd<<"mv ";
    strptr_PrintBash(from,cmd);
    cmd<<" ";
    strptr_PrintBash(to,cmd);
    // source and tgt files may be on different filesystems,
    // don't use rename(); use the mv command
    return SysCmd(cmd);
}

// -----------------------------------------------------------------------------

// Move file SRC to TGTFNAME.
// If destination file exists, it is pointed to by TGT.
// If the move succeeds, source entry is deleted to reflect this.
void orgfile::MoveFile(orgfile::FFilename *src, orgfile::FFilename *tgt, strptr tgtfname) {
    vrfy(tgt==NULL || src->filehash==tgt->filehash, "internal error: move with overwrite");
    if (SystemMv(src->filename, tgtfname)) {
        if (!tgt) {
            tgt = &filename_Alloc();
            tgt->filename = tgtfname;
            tgt->filehash = src->filehash;// definitely exists
            vrfy_(filename_XrefMaybe(*tgt));
        }
        // move succeeded, entry no longer needed
        filename_Delete(*src);
    }
}

// -----------------------------------------------------------------------------

// Add suffixes to FNAME until it becomes a suitable
// name for a new file
static tempstr MakeUnique(strptr fname) {
    int index=2;
    cstring ret;
    do {
        ret = tempstr() << StripExt(fname) << "-" << index << GetFileExt(fname);
        index++;
    } while (FileQ(ret));
    return tempstr(ret);
}

// -----------------------------------------------------------------------------
// Read filenames files from STDIN (one per line).
// For each file, determine its new destination by calling GetTgtFname.
// Create new directory structure as appropriate.
// Move the file into place if there was no conflict, or if the file content
//   hash exactly matches
void orgfile::MoveFile(strptr pathname) {
    bool canmove=false;
    orgfile::move move;
    move.pathname=pathname;
    if (FileQ(pathname)) {
        move.tgtfile=GetTgtFname(pathname);
    } else {
        move.comment = "file doesn't exist";
    }
    orgfile::FFilename *src = AccessFilename(pathname);
    orgfile::FFilename *tgt = NULL;
    if (move.tgtfile != "" && move.tgtfile != pathname) {
        if (!FileQ(move.tgtfile)) {
            canmove = true;
            move.comment = "move file";
        } else {
            // only if it's known to exist
            tgt = AccessFilename(move.tgtfile);
            canmove = src->filehash == tgt->filehash;
            if (canmove) {
                move.comment = "move file (proven duplicate)";
            } else {
                move.comment = "move file (renaming for uniqueness)";
                move.tgtfile = MakeUnique(move.tgtfile);
                // how could this possibly return anything except NULL?
                tgt = ind_filename_Find(move.tgtfile);
                canmove = true;
            }
        }
        prlog(move);
        if (canmove && _db.cmdline.commit) {// do move
            MoveFile(src,tgt,move.tgtfile);
        }
    }
}

// -----------------------------------------------------------------------------

bool orgfile::RawMove(strptr line) {
    bool ret=false;
    orgfile::move move;
    if (move_ReadStrptrMaybe(move,line)) {
        ret = true;
        bool canmove = false;
        if (_db.cmdline.undo) {
            TSwap(move.pathname, move.tgtfile);
        }
        if (FileQ(move.pathname)) {// source
            move.comment = "move file";
            canmove=true;
        } else {
            move.comment = "original not found";
        }
        prlog(move);
        if (canmove && _db.cmdline.commit) {// do move
            SystemMv(move.pathname, move.tgtfile);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

bool orgfile::RawDedup(strptr line) {
    bool ret = false;
    orgfile::dedup dedup;
    if (dedup_ReadStrptrMaybe(dedup,line)) {
        ret = true;
        prlog(dedup);
        if (_db.cmdline.commit && !_db.cmdline.undo) {
            DeleteFile(dedup.duplicate);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

void orgfile::Main() {
    if (DirectoryQ(_db.cmdline.move) && !EndsWithQ(_db.cmdline.move,"/")) {
        _db.cmdline.move << "/";
    }
    ind_beg(algo::FileLine_curs,line,algo::Fildes(0)) {
        if (FileQ(line)) {
            if (_db.cmdline.move != "") {
                MoveFile(line);
            } else if (_db.cmdline.dedup.expr != "") {
                DedupFile(line);
            }
        } else {
            RawMove(line) || RawDedup(line);
        }
    }ind_end;
}
