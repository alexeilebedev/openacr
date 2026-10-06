// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Header: include/acr.h -- Header file
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
    // cpp/acr/check.cpp -- Check constraints & referential integrity
    //

    // Fill replscope R from tuple TUPLE
    // using schema from record REC.
    // Return child record pkey from SSIMREQ
    tempstr GetChildKey(acr::FRec &rec, acr::FSsimreq &ssimreq, algo::Tuple &tuple, algo_lib::Replscope &R);

    // Fill R with wildcards for every variable that might be required by GetChildKey
    void FillWildcardKey(acr::FSsimreq &ssimreq, algo_lib::Regx &regx_child);

    // Check ssimreq table
    // e.g.
    // dmmeta.ssimreq  ssimreq:atfdb.Comptest.comptest:% child:dev.gitfile:test/atf_comp/$comptest    req:Y  bidir:N  comment:""
    // here,
    // parent=atfdb.Comptest
    // child=dev.Gitfile
    // We scan parent records and then check that child table records are found
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
    // of the value of its SSIMSORT attribute and the newly provided ROWID.
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
    // cpp/acr/git.cpp -- Git triggers
    //

    // Emit the git commands that make the worktree match the selected dev.gitfile
    // rows: a deleted row's file is removed, a renamed row's file is moved, and a
    // new row's file is created and staged. A file moved or created gets its
    // directory first: an ssimfile renamed into a new namespace lands under a
    // data directory that does not exist yet. WRITE_OK says the ssimfile write-back
    // went through, and the script then runs; otherwise the script is printed.
    // A nonzero script status fails the run. acr_ed's rename arrives here as an acr
    // run that renames the source file's dev.gitfile row under -write and -g, and by
    // the time the script runs the ssimfiles already name the destination. A git mv
    // refused because the source is untracked therefore leaves the worktree holding
    // the old path while the database names the new one, and the next amc or abt
    // compiles against a database naming a file that does not exist. The script's
    // status therefore travels out in the exit code, and the diagnostic names the
    // script that failed, so a caller that checks either one can tell that run apart
    // from a clean one.
    void Main_GitTriggers(bool write_ok);

    // -------------------------------------------------------------------
    // cpp/acr/load.cpp -- Load files
    //

    // Report input line TEXT (at FILE's current lineno) that cannot be loaded,
    // for REASON: a parse failure or a ctype acr does not know.  A dropped line
    // would not survive a -write -- the file is rewritten from the rows that
    // loaded, so an unloadable row silently vanishes, possibly weeks later via
    // an -insert into an unrelated row of the same file.  Recording the load
    // failure and a nonzero exit blocks the rewrite (main.cpp gates -write on
    // exit_code==0, and editor mode on !load_failed), so the file keeps every
    // line it held.  Blank and comment-only lines parse into an empty tuple and
    // never reach this path.
    void ReportBadLine(acr::FFile &file, algo::strptr text, algo::strptr reason);

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
    // cpp/acr/main.cpp -- Main file
    //
    algo::UnTime FdModTime(algo::Fildes fd);
    strptr Typetag(acr::FCtype &ctype);

    // cached lookup
    // ignore:ptr_byref
    void LookupField(acr::FRec &rec, strptr fieldname, acr::FCtype *&prev_ctype, acr::FField *&prev_field);
    void Main_CmdQuery();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:acr

    // -------------------------------------------------------------------
    // cpp/acr/print.cpp -- Code for output
    //

    // Print selected records using formatting options
    // specified on command line.
    void Print(acr::FPrint &print);
    void FlushStdout(acr::FPrint &print);

    // Print selected records to fd
    void PrintToFd(acr::FPrint &print, algo::Fildes fd);

    // -------------------------------------------------------------------
    // cpp/acr/query.cpp -- Run query
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
    // cpp/acr/select.cpp -- Selection of records
    //

    // Remove record from the per-ctype selected set
    // Remove record from global selected set
    // If the record was the last selected record for its ctype,
    // remove its ctype from the selected list
    void Rec_Deselect(acr::FRec& rec);

    // De-select all records
    // - zd_all_selrec list is cleared
    // - zd_ctype_selrec list is cleared for each ctype
    // - zd_sel_ctype list is cleared
    void Rec_DeselectAll();

    // Select all records from all files
    void Rec_SelectAll();

    // Select all records that were modified or deleted
    // This includes records that were both inserted and deleted during this run
    void SelectModified();

    // Conditionally insert record into selection set
    // - Record is added to zd_ctype_selrec list for is ctype
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

    // Select ctypes of selected records, deselect records themselves.
    // The ctype and field rows selected here come from -schema, the directory
    // the schema was read from, so a dataset queried through -in answers -meta
    // from the schema it was checked against.
    void Main_SelectMeta();
    void Main_SelectUp();

    // Check if field FIELD of type CHILD is the pkey of CHILD
    // or the leftmost prefix of pkey of CHILD
    bool LeftCheck(acr::FCtype &child, acr::FField &field);

    // extend selected front down
    // the search starts with all selected records, where we clear the visit flag
    // we then create a list C_CTYPE_FRONT of all potential ctypes that might reference these selected records,
    // we then scan records for these ctypes, and add new records to the selected list
    // The function returns the number of records added.
    // The function performs one iteration of the downward transitive closure. Looping until
    // SelectDown returns 0 finds all downward references.
    int Main_SelectDown(bool unused);
    void Main_AcrEdit();

    // Mark SSIMREQ child records for deletion
    void DelChildRecords(acr::FRec &rec);

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
