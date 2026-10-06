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
        // acr $key -t | acr -in:$dir -insert -write
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
        // a failed write (missing directory, permission) fails the run:
        // exiting 0 with the records silently unwritten would leave the
        // caller trusting a dataset that was never updated
        if (!algo::SaveFile(out, file.file, "acr.file_write", "output file could not be written")) {
            algo_lib::_db.exit_code++;
        }
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
    // a failed write fails the run, same as the dataset-mode write above
    if (!algo::SaveFile(print.out, acr::_db.cmdline.in, "acr.file_write", "output file could not be written")) {
        algo_lib::_db.exit_code++;
    }
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
            } else if (file.sticky) {
                // A sticky file with no filename is the schema -meta read over
                // a dataset: its rows stay its own and it is never rewritten,
                // so an edit that landed on one of them would be reported
                // applied and go nowhere.  The other filename-less files (the
                // stdin pipe, the -e temp file) are not sticky, so their rows
                // were re-homed to a dataset file at insert; what stays on them
                // is a row of a ctype with no ssimfile, dropped as always.
                int n_edit = 0;
                ind_beg(acr::file_zd_frec_curs, rec, file) {
                    n_edit += rec.mod || rec.del || rec.isnew;
                }ind_end;
                if (n_edit > 0) {
                    prerr("acr.schema_readonly"
                          <<Keyval("file",file.file)
                          <<Keyval("n_edit",n_edit)
                          <<Keyval("comment","rows read from -schema to answer -meta are not written; edit the schema with -in naming its directory"));
                    algo_lib::_db.exit_code++;
                }
            }
        }ind_end;
    }
    acr::_db.report.n_file_mod += algo_lib::_db.stringtofile_nwrite - nbefore;
}
