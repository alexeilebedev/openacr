// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/load.cpp -- Load files
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

// Report input line TEXT (at FILE's current lineno) that cannot be loaded,
// for REASON: a parse failure or a ctype acr does not know.  A dropped line
// would not survive a -write -- the file is rewritten from the rows that
// loaded, so an unloadable row silently vanishes, possibly weeks later via
// an -insert into an unrelated row of the same file.  Recording the load
// failure and a nonzero exit blocks the rewrite (main.cpp gates -write on
// exit_code==0, and editor mode on !load_failed), so the file keeps every
// line it held.  Blank and comment-only lines parse into an empty tuple and
// never reach this path.
void acr::ReportBadLine(acr::FFile &file, algo::strptr text, algo::strptr reason) {
    prerr(file.file<<":"<<file.lineno<<": acr.badline"
          <<Keyval("reason",reason)
          <<Keyval("text",text));
    _db.report.n_badline++;
    if (!_db.load_failed) {
        _db.load_failed = true;
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

// Load records for this ctype from the appropriate ssimfile.
// This does nothing if acr is operating in file mode.
// A dataset holds only the ssimfiles it needs, so an ssimfile whose path
// resolves to nothing loads as an empty table. Any other read failure -- a
// permission problem, an i/o error, a mapping that did not succeed -- fails the
// run instead: the query would otherwise answer from a table missing every row
// of that file, and a -write would rewrite the file from the rows that did
// load, dropping the rest. acr.DsetFileReadDeny pins both halves.
void acr::LoadRecords(acr::FCtype &ctype) {
    if (acr::FSsimfile *ssimfile = ctype.c_ssimfile) {
        acr::FFile *file = ctype.c_ssimfile->c_file;
        if (!file && !FileInputQ()) {
            file = &acr::ind_file_GetOrCreate(SsimFname(acr::_db.cmdline.in, ssimfile->ssimfile));
            ssimfile->c_file = file;
            file->autoloaded = true;
            file->filename = file->file; // save filename
            algo_lib::MmapFile in;
            if (MmapFile_Load(in, file->file)) {
                file->modtime = FdModTime(in.fd.fd);
                verblog("acr.load"<<Keyval("fname",file->file));
                Tuple tuple;
                ind_beg(Line_curs,line,in.text) {
                    ssimfile->c_file->lineno = ind_curs(line).i+1;
                    if (Tuple_ReadStrptrMaybe(tuple, line)) {
                        ReadTuple(tuple, *ssimfile->c_file, acr_ReadMode_acr_insert);
                    } else {
                        ReportBadLine(*ssimfile->c_file, line, "cannot parse line");
                    }
                }ind_end;
            } else if (errno==ENOENT || errno==ENOTDIR) {
                // the path names no file, so the table stays empty
            } else {
                algo::PrerrFileFail("acr.file_read", file->file, "ssimfile could not be read");
                acr::_db.load_failed = true;
                algo_lib::_db.exit_code++;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Return default read mode as specified on the command line
acr::ReadMode acr::DefaultReadMode() {
    acr::ReadMode read_mode;
    read_mode = acr_ReadMode_acr_insert;
    if (_db.cmdline.sel) {
        read_mode=acr_ReadMode_acr_select;
    } else if (_db.cmdline.replace) {
        read_mode=acr_ReadMode_acr_replace;
    } else if (_db.cmdline.merge) {
        read_mode=acr_ReadMode_acr_merge;
    } else if (_db.cmdline.update) {
        read_mode=acr_ReadMode_acr_update;
    }
    return read_mode;
}

// -----------------------------------------------------------------------------

// Read lines from fd IN, associating them with file FILE
// The read mode is READ_MODE
void acr::ReadLines(acr::FFile &file, algo::Fildes in, acr::ReadMode read_mode) {
    verblog("readlines "<<file.filename);
    Tuple tuple;
    ind_beg(algo::FileLine_curs,line,in) {
        if (Tuple_ReadStrptrMaybe(tuple,line)) {
            acr::FRec *rec = acr::ReadTuple(tuple, file, read_mode);
            (void)rec;
        } else {
            ReportBadLine(file, line, "cannot parse line");
        }
        file.lineno++;
    }ind_end;
}

// -----------------------------------------------------------------------------

void acr::Main_ReadIn() {
    // load data from "-in:..."
    if (FileInputQ()) {
        acr::FFile &file = acr::ind_file_GetOrCreate(acr::_db.cmdline.in);
        file.sticky = true;
        file.autoloaded = true;// not new data
        algo_lib::FFildes in;
        in.fd = OpenRead(acr::_db.cmdline.in, algo::FileFlags());
        file.filename = acr::_db.cmdline.in;
        if (!ValidQ(in.fd)) {
            // the path exists (that is how file mode was selected) but cannot
            // be opened, e.g. a permission problem; answering from nothing
            // would pass the bad input off as a true empty result
            algo::PrerrFileFail("acr.file_read", acr::_db.cmdline.in, "input file could not be read");
            acr::_db.load_failed = true;
            algo_lib::_db.exit_code++;
        } else {
            file.modtime = FdModTime(in.fd);
            ReadLines(file,in.fd,acr::DefaultReadMode());
        }
    } else if (acr::_db.cmdline.in == "-") {
        acr::FFile &file = acr::ind_file_GetOrCreate(acr::_db.cmdline.in);
        file.autoloaded = true;// not new data
        file.stdin = true;
        ReadLines(file,Fildes(0),acr::DefaultReadMode());
    } else if (DirectoryQ(acr::_db.cmdline.in)) {
        // a dataset directory hands over its ssimfiles one at a time, as the
        // query reaches each table, so a directory acr cannot search fails one
        // read per table touched and never names the directory itself. Probe it
        // once here instead. Missing individual ssimfiles inside a searchable
        // directory stay tolerated (LoadRecords).
        if (!DirSearchableQ(acr::_db.cmdline.in)) {
            algo::PrerrFileFail("acr.file_read", acr::_db.cmdline.in, "dataset directory could not be searched");
            acr::_db.load_failed = true;
            algo_lib::_db.exit_code++;
        }
    } else {
        // -in names neither a readable file, nor stdin, nor a dataset
        // directory. Loading an empty dataset and exiting 0 would let a
        // mistyped -in (or a wrong working directory) pass as a true empty
        // result; fail the run naming the path.
        algo::PrerrFileFail("acr.file_read", acr::_db.cmdline.in, "input directory or file could not be read");
        acr::_db.load_failed = true;
        algo_lib::_db.exit_code++;
    }
    // Read data from stdin, insert/replace/update/merge into in-memory store
    // If stdio mode is selected, the incoming records form a background
    // for the query(i.e. they are not considered "new")
    if (_db.cmdline.sel || _db.cmdline.insert || _db.cmdline.replace || _db.cmdline.update || _db.cmdline.merge) {
        acr::FFile &file = acr::ind_file_GetOrCreate("stdin");
        file.stdin = true;
        ReadLines(file,Fildes(0),acr::DefaultReadMode());
    }
}

// -----------------------------------------------------------------------------

// True if acr input comes from a named file
bool acr::FileInputQ() {
    return acr::_db.file_input;
}
