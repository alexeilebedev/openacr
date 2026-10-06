// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

// Read the tuples of FILE, an ssimfile of one table, into that table.  The
// caller says what the file is for through its flags; this only reads it.
// A dataset holds only the ssimfiles it needs, so a path that resolves to
// nothing loads as an empty table.  Any other read failure -- a permission
// problem, an i/o error, a mapping that did not succeed -- fails the run
// instead: the query would otherwise answer from a table missing every row of
// that file, and a -write would rewrite the file from the rows that did load,
// dropping the rest.  acr.DsetFileReadDeny pins both halves.
static void LoadFileRecords(acr::FFile &file) {
    file.autoloaded = true;
    algo_lib::MmapFile in;
    if (MmapFile_Load(in, file.file)) {
        file.modtime = acr::FdModTime(in.fd.fd);
        verblog("acr.load"<<Keyval("fname",file.file));
        Tuple tuple;
        ind_beg(Line_curs,line,in.text) {
            file.lineno = ind_curs(line).i+1;
            if (Tuple_ReadStrptrMaybe(tuple, line)) {
                acr::ReadTuple(tuple, file, acr_ReadMode_acr_insert);
            } else {
                acr::ReportBadLine(file, line, "cannot parse line");
            }
        }ind_end;
    } else if (errno==ENOENT || errno==ENOTDIR) {
        // the path names no file, so the table stays empty
    } else {
        algo::PrerrFileFail("acr.file_read", file.file, "ssimfile could not be read");
        acr::_db.load_failed = true;
        algo_lib::_db.exit_code++;
    }
}

// Load records for this ctype from its ssimfile under -in.
// This does nothing if acr is operating in file mode.
// While -meta is selecting, a table -in left empty is read from -schema as
// well, the directory the schema came from: the ctype and field rows -meta
// answers with are schema, and a dataset queried through -in carries none of
// its own.  A table -in does hold, as a second checkout's data/ does, is taken
// as read, so nothing is inserted twice and the report counts only what the
// query ignored.  The schema file is read once, whether the table was bound
// under -in before -meta ran or is first touched by it, and it is read for
// answering only: it carries no filename, so a -write never rewrites it, and
// it is sticky, so its rows stay its own and a -write never copies them into
// the -in file.  An edit that lands on one of its rows is refused at write
// time (WriteFiles), since the row has no file to go to.
void acr::LoadRecords(acr::FCtype &ctype) {
    if (acr::FSsimfile *ssimfile = ctype.c_ssimfile) {
        acr::FFile *file = ssimfile->c_file;
        if (!file && !FileInputQ()) {
            file = &acr::ind_file_GetOrCreate(SsimFname(acr::_db.cmdline.in, ssimfile->ssimfile));
            ssimfile->c_file = file;
            file->filename = file->file; // save filename
            LoadFileRecords(*file);
        }
        if (_db.metaload && file && zd_frec_EmptyQ(*file)) {
            tempstr schemafile = SsimFname(acr::_db.cmdline.schema, ssimfile->ssimfile);
            if (!acr::ind_file_Find(schemafile)) {
                acr::FFile &schema = acr::ind_file_GetOrCreate(schemafile);
                schema.sticky = true;
                LoadFileRecords(schema);
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
