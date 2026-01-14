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
// Source: cpp/acr/write.cpp -- Write files
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

// Save ssimfile (single table) back to disk.
// Collect all records in file, sort them by sort field.
// Optionally create missing second-level directory (e.g. data/dmmeta).
static void WriteFile(acr::FWrite &write, acr::FFile &file) {
    // collect records
    acr::c_cmtrec_RemoveAll(write);
    ind_beg(acr::file_zd_frec_curs, rec, file) {
        if (!rec.del) {
            acr::c_cmtrec_Insert(write, rec);
        }
    }ind_end;
    cstring out;
    // sort them by original rowid
    acr::c_cmtrec_QuickSort(write);
    ind_beg(acr::write_c_cmtrec_curs,rec,write) {
        // print record to string according to schema
        PrintAttr(out, rec.tuple.head.name, rec.tuple.head.value);
        tempstr attr;
        ind_beg(acr::ctype_c_field_curs, field, *rec.p_ctype) {
            if (!field.isfldfunc) {
                ch_RemoveAll(attr);
                EvalAttrDflt(rec.tuple, field, attr);
                PrintAttrSpace(out, name_Get(field), attr);
            }
        }ind_end;
        out << eol;
    }ind_end;
    acr::c_cmtrec_RemoveAll(write);
    // skip writing empty string to a non-existent file
    bool dowrite = ch_N(out) || FileQ(file.file);
    if (acr::_db.cmdline.write && dowrite) {
        // attempt to create up to 1 level of directories
        // this supports use such as
        // acr $key -t | acr -data_dir $dir -write
        // where the number of ssim namespaces under $key doesn't have
        // to be known in advance
        tempstr dirname(GetDirName(file.file));
        if (!FileObjectExistsQ(dirname)) {
            int rc = mkdir(Zeroterm(dirname),0755);
            if (rc != 0) {
                verblog("acr.mkdir"
                        <<Keyval("dir",dirname)
                        <<Keyval("rc", rc));
            }
        }
        (void)SafeStringToFile(out, file.file);
    }
}

// -----------------------------------------------------------------------------

// Save records back to a single file as specified by -out option.
// File is written in tree mode.
// If -out is ommitted, input file is the same as output file;
// in this case, ALL RECORDS that were not deleted are saved back.
// If output file is different from input, then only selected records are saved.
static void SaveSingleFile() {
    acr::Rec_SelectAll();
    acr::FPrint print;
    print.fstdout   = false;     // save it
    print.tree     = acr::_db.cmdline.tree;
    print.pretty   = false;
    print.maxgroup = INT_MAX;
    print.cmt      = false;
    print.rowid    = false;     // omit it
    print.showstatus = false;
    print.loose    = false;     // full referential integrity required when saving
    Print(print);
    (void)SafeStringToFile(print.out, acr::_db.cmdline.in);
}

// -----------------------------------------------------------------------------

// Write all modified files back to disk
// Support both single-file and dataset modes.
void acr::WriteFiles() {
    u32 nbefore=algo_lib::_db.stringtofile_nwrite;
    if (acr::_db.file_input) {
        SaveSingleFile();
    } else {
        acr::FWrite write;
        ind_beg(acr::_db_file_curs, file, acr::_db) {
            if (ch_N(file.filename) != 0) {
                WriteFile(write,file);
            }
        }ind_end;
    }
    acr::_db.report.n_file_mod += algo_lib::_db.stringtofile_nwrite > nbefore;
}
