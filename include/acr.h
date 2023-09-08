// (C) AlgoEngineering LLC 2008-2013
// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Header: include/acr.h
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//
// ACR: Algo Cross-Reference
// ACR Interface

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

    // insert tuple into database.
    // if tuple cannot be inserted, return NULL.
    // FILE    input file. if NULL, assign file of ssimfile!
    //
    acr::FRec* ReadTuple(Tuple &tuple, acr::FFile &file, bool insert);

    // TUPLE: input tuple
    // CTYPE: associated type, as found via the type tag
    // ATTR: pkey attr
    acr::FRec *CreateRec(acr::FFile &file, acr::FCtype *ctype, algo::Tuple &tuple, algo::Attr *pkey_attr, bool insert);

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

    // Load input records for ssimfile SSIMFILE
    // and create an approprate FILE record.
    void LoadSsimfile(acr::FSsimfile& ssimfile);

    // Read lines from input text file IN
    // Associate loaded records with record FILE.
    void ReadLines(acr::FFile &file, algo_lib::InTextFile &in);
    void Main_ReadIn();
    bool EphemeralQ(acr::FFile &file);

    // True if acr input comes from a named file
    bool FileInputQ();

    // Return true if all records for specified ctype are available and in memory
    bool RecordsLoadedQ(acr::FCtype &ctype);

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
    void Rec_Deselect(acr::FRec& rec);

    // De-select all records
    void Rec_DeselectAll();

    // Select all records from all files
    void Rec_SelectAll();

    // Conditionally insert record into selection set
    // - Record is added to zd_selrec list for is ctype
    // - Record is added to zd_all_selrec (global list)
    // - Selected ctype is added to zd_sel_ctype list
    bool Rec_Select(acr::FRec& rec);

    // Deselect all records.
    // Select only records in the error set
    void SelectErrRecs();

    // -------------------------------------------------------------------
    // cpp/acr/verb.cpp -- Command-line verbs
    //
    void Main_Cmd();

    // Print fields in a column
    void Main_Field();

    // Print fields in a column
    void Main_Regxof();
    void Main_Mysql();
    void Main_Browser();

    // Select ctypes of selected records, deselect records themselves
    void Main_SelectMeta();
    void Main_AcrEdit();

    // -------------------------------------------------------------------
    // cpp/acr/write.cpp -- Write files
    //

    // Write all modified files back to disk
    // Support both single-file and dataset modes.
    void WriteFiles();
}
