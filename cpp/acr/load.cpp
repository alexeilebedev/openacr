// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/load.cpp -- Load files
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

// Load input records for ssimfile SSIMFILE
// and create an approprate FILE record.
void acr::LoadSsimfile(acr::FSsimfile& ssimfile) {
    acr::FFile *file = ssimfile.c_file;
    if (!FileInputQ() && !file) {
        file = &acr::ind_file_GetOrCreate(SsimFname(acr::_db.cmdline.in, ssimfile.ssimfile));
        ssimfile.c_file = file;
        file->autoloaded = true;
        file->filename = file->file; // save filename
        algo_lib::MmapFile in;
        if (MmapFile_Load(in, file->file)) {
            file->modtime = FdModTime(in.fd.fd);
            verblog("acr.load"<<Keyval("fname",file->file));
            ind_beg(Line_curs,line,in.text) {
                Tuple tuple;
                if (Tuple_ReadStrptrMaybe(tuple, line)) {
                    ssimfile.c_file->lineno = ind_curs(line).i+1;
                    ReadTuple(tuple, *ssimfile.c_file, acr_ReadMode_insert);
                }
            }ind_end;
        }
    }
}

// -----------------------------------------------------------------------------

// Read lines from input text file IN
// Associate loaded records with record FILE.
void acr::ReadLines(acr::FFile &file, algo::Fildes in) {
    Tuple tuple;
    acr::ReadMode read_mode;
    read_mode=acr_ReadMode_insert;// default
    if (_db.cmdline.replace) {
        read_mode=acr_ReadMode_replace;
    } else if (_db.cmdline.merge) {
        read_mode=acr_ReadMode_merge;
    } else if (_db.cmdline.update) {
        read_mode=acr_ReadMode_update;
    }
    // 'del' is an operation that applies to selected records,
    // so it is not applicable here.
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
        ReadLines(file,in.fd);
    } else if (acr::_db.cmdline.in == "-") {
        acr::FFile &file = acr::ind_file_GetOrCreate(acr::_db.cmdline.in);
        file.autoloaded = true;// not new data
        algo_lib::FFildes in;
        in.fd = Fildes(dup(0));
        ReadLines(file,in.fd);
    }
    // Read data from stdin, insert/replace/update/merge into in-memory store
    // If stdio mode is selected, the incoming records form a background
    // for the query(i.e. they are not considered "new")
    if (acr::_db.cmdline.insert || acr::_db.cmdline.replace || acr::_db.cmdline.update || acr::_db.cmdline.merge) {
        algo_lib::FFildes in;
        in.fd = Fildes(dup(0));
        acr::FFile &file = acr::ind_file_GetOrCreate("stdin-insert");
        ReadLines(file,in.fd);
    }
}

// -----------------------------------------------------------------------------

bool acr::EphemeralQ(acr::FFile &file) {
    return ch_N(file.filename)==0;
}


// -----------------------------------------------------------------------------

// True if acr input comes from a named file
bool acr::FileInputQ() {
    return acr::_db.file_input;
}

// -----------------------------------------------------------------------------

// Return true if all records for specified ctype are available and in memory
bool acr::RecordsLoadedQ(acr::FCtype &ctype) {
    return FileInputQ()
        || (ctype.c_ssimfile && ctype.c_ssimfile->c_file);
}
