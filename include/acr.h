// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Header: include/acr.h
//
// ACR: Algo Cross-Reference
// ACR Interface

#include "include/algo.h"
#include "include/gen/acr_gen.h"
#include "include/gen/acr_gen.inl.h"

namespace acr { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr/check.cpp
    //
    void CheckSsimreq();
    void Main_Check();

    // -------------------------------------------------------------------
    // cpp/acr/createrec.cpp -- Create record
    //

    // Insert record in ACR's database.
    // Upon first access of ssimfile, load ssimfile from disk.
    // If -trunc option is set, mark all records for deletion
    acr::FRec* ReadTuple(Tuple &tuple, acr::FFile &file, acr::ReadMode read_mode);

    // Calculate record's SORTKEY which is a combination
    // of the value of its SSIMSORT attribute and rowid.
    void UpdateSortkey(acr::FRec &rec, float rowid);

    // Create a new record from tuple TUPLE, having primary key PKEY_ATTR and type
    // CTYPE (as found via the type tag).
    // if INSERT is specified, the record inserted. Otherwise, it's deleted
    // This function checks CMDLINE.REPLACE flag to see if the record is allowed
    // to replace an existing record; if CMDLINE.MERGE is specified, attributes are merged
    // into an existing record if one exists
    acr::FRec *CreateRec(acr::FFile &file, acr::FCtype *ctype, algo::Tuple &tuple, algo::Attr *pkey_attr, acr::ReadMode read_mode);

    // -------------------------------------------------------------------
    // cpp/acr/err.cpp -- Show errors / suggestions
    //
    void NoteErr(acr::FCtype* ctype, acr::FRec* rec, acr::FField *fld, strptr text);

    // -------------------------------------------------------------------
    // cpp/acr/eval.cpp -- Evaluate attributes
    //
    void Evalattr_Step(acr::FEvalattr &evalattr, algo::Tuple &tuple);

    // Retrieve attribute of TUPLE corresponding to FIELD.
    // Supports SUBSTR expressions.
    void EvalAttrDflt(Tuple &tuple, acr::FField &field, cstring &ret);

    // Locate attribute in TUPLE whose name is FIELD.NAME
    // If FIELD is a fldfunc, locate the source attribute
    // and apply substring expression to retrieve the value.
    // The result is a tempstr.
    // can this function be shared?
    // I see it is being implemented in three different places
    tempstr EvalAttr(Tuple &tuple, acr::FField &field);

    // -------------------------------------------------------------------
    // cpp/acr/load.cpp -- Load files
    //

    // Load records for this ctype from the appropriate ssimfile
    // This does nothing if acr is operating in file mode, or if the ssimfile doesn't exist
    void LoadRecords(acr::FCtype &ctype);

    // Return default read mode as specified on the command line
    acr::ReadMode DefaultReadMode();

    // Read lines from fd IN, associating them with file FILE
    // The read mode is READ_MODE
    void ReadLines(acr::FFile &file, algo::Fildes in, acr::ReadMode read_mode);
    void Main_ReadIn();

    // True if acr input comes from a named file
    bool FileInputQ();

    // -------------------------------------------------------------------
    // cpp/acr/main.cpp
    //
    algo::UnTime FdModTime(algo::Fildes fd);

    // Parse query and return object
    acr::FQuery& ParseQuery(strptr expr);
    strptr Typetag(acr::FCtype &ctype);

    // cached lookup
    // ignore:ptr_byref
    void LookupField(acr::FRec &rec, strptr fieldname, acr::FCtype *&prev_ctype, acr::FField *&prev_field);
    void Main();

    // -------------------------------------------------------------------
    // cpp/acr/print.cpp
    //

    // Print selected records using formatting options
    // specified on command line.
    void Print(acr::FPrint &print);
    void FlushStdout(acr::FPrint &print);

    // Print selected records to fd
    void PrintToFd(acr::FPrint &print, algo::Fildes fd);

    // -------------------------------------------------------------------
    // cpp/acr/run.cpp -- Run query
    //

    // Visit all records matching QUERY.
    // Perform actions specified bu QUERY -- rename, delete, select, etc.
    // For selection:
    // follow up and down cross-reference links across all types of ssimfiles.
    // Matching records are added to zd_all_selrec index;
    // Matching ssimfiles are added to db.c_sel_ctype index.
    void RunQuery(acr::FQuery &query);
    void RunAllQueries();

    // -------------------------------------------------------------------
    // cpp/acr/select.cpp -- Selection
    //

    // Remove record from the per-ctype selected set
    // Remove record from global selected set
    // If the record was the last selected record for its ctype,
    // remove its ctype from the selected list
    void Rec_Deselect(acr::FRec& rec);

    // De-select all records
    // - zd_all_selrec list is cleared
    // - zd_selrec list is cleared for each ctype
    // - zd_sel_ctype list is cleared
    void Rec_DeselectAll();

    // Select all records from all files
    void Rec_SelectAll();

    // Select all records that were modified or deleted
    // This includes records that were both inserted and deleted during this run
    void SelectModified();

    // Conditionally insert record into selection set
    // - Record is added to zd_selrec list for is ctype
    // - Record is added to zd_all_selrec (global list)
    // - Selected ctype is added to zd_sel_ctype list
    bool Rec_Select(acr::FRec& rec);

    // -------------------------------------------------------------------
    // cpp/acr/verb.cpp -- Command-line verbs
    //
    void Main_Cmd();

    // Print fields in a column
    void Main_Field();

    // Print fields in a column
    void Main_Regxof();
    void Main_Mysql();

    // Add ctype and its transitive closure to the list of selected records
    // This produces a new query but doesn't run it
    void ScheduleSelectCtype(acr::FCtype &ctype_ctype, acr::FCtype &ctype);

    // Select ctypes of selected records, deselect records themselves
    void SelectMeta();
    void Main_AcrEdit();

    // Start with selected records
    // Find all dependent records and delete them as well
    // In the end, de-select records that were both inserted and deleted
    void CascadeDelete();

    // -------------------------------------------------------------------
    // cpp/acr/write.cpp -- Write files
    //

    // Write all modified files back to disk
    // Support both single-file and dataset modes.
    void WriteFiles();
}
