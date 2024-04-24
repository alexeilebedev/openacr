// Copyright (C) 2023-2024 AlgoRND
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

// Load records for this ctype from the appropriate ssimfile
// This does nothing if acr is operating in file mode, or if the ssimfile doesn't exist
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
                    if (Tuple_ReadStrptrMaybe(tuple, line)) {
                        ssimfile->c_file->lineno = ind_curs(line).i+1;
                        ReadTuple(tuple, *ssimfile->c_file, acr_ReadMode_acr_insert);
                    }
                }ind_end;
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
        file.modtime = FdModTime(in.fd);
        ReadLines(file,in.fd,acr::DefaultReadMode());
    } else if (acr::_db.cmdline.in == "-") {
        acr::FFile &file = acr::ind_file_GetOrCreate(acr::_db.cmdline.in);
        file.autoloaded = true;// not new data
        ReadLines(file,Fildes(0),acr::DefaultReadMode());
    }
    // Read data from stdin, insert/replace/update/merge into in-memory store
    // If stdio mode is selected, the incoming records form a background
    // for the query(i.e. they are not considered "new")
    if (_db.cmdline.sel || _db.cmdline.insert || _db.cmdline.replace || _db.cmdline.update || _db.cmdline.merge) {
        acr::FFile &file = acr::ind_file_GetOrCreate("stdin");
        ReadLines(file,Fildes(0),acr::DefaultReadMode());
    }
}

// -----------------------------------------------------------------------------

// True if acr input comes from a named file
bool acr::FileInputQ() {
    return acr::_db.file_input;
}
