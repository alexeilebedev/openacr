//
// include/gen/ams_sendtest_gen.h
// Generated by AMC
//
// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2023 AlgoRND
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


#pragma once
#include "include/gen/algo_lib_gen.h"
#include "include/gen/algo_gen.h"
#include "include/gen/command_gen.h"
#include "include/gen/ams_gen.h"
#include "include/gen/lib_ams_gen.h"
//#pragma endinclude
// gen:ns_enums

// --- ams_sendtest_FieldIdEnum

enum ams_sendtest_FieldIdEnum {        // ams_sendtest.FieldId.value
     ams_sendtest_FieldId_value   = 0
};

enum { ams_sendtest_FieldIdEnum_N = 1 };

namespace ams_sendtest { // gen:ns_pkeytypedef
} // gen:ns_pkeytypedef
namespace ams_sendtest { // gen:ns_tclass_field
extern const char *ams_sendtest_help;
} // gen:ns_tclass_field
// gen:ns_fwddecl2
namespace ams_sendtest { struct _db_child_curs; }
namespace ams_sendtest { struct AmsSendTest; }
namespace ams_sendtest { struct FChild; }
namespace ams_sendtest { struct trace; }
namespace ams_sendtest { struct FDb; }
namespace ams_sendtest { struct FieldId; }
namespace ams_sendtest { extern struct ams_sendtest::FDb _db; }
namespace ams_sendtest { // gen:ns_print_struct

// --- ams_sendtest.AmsSendTest
struct AmsSendTest { // ams_sendtest.AmsSendTest
    u64                   n_msg_send;         //   0  Number of messages sent
    u64                   n_msg_recv;         //   0  Number of messages received
    u64                   n_write_wait;       //   0  Number of waits by parent
    u64                   n_msg_limit;        //   1000000  Limit of messages to send
    algo_lib::FTimehook   h_write;            //
    u64                   sum_recv_latency;   //   0
    // func:ams_sendtest.AmsSendTest..Ctor
    inline               AmsSendTest() __attribute__((nothrow));
};

// Set all fields to initial values.
// func:ams_sendtest.AmsSendTest..Init
inline void          AmsSendTest_Init(ams_sendtest::AmsSendTest& parent);
// print string representation of ROW to string STR
// cfmt:ams_sendtest.AmsSendTest.String  printfmt:Tuple
// func:ams_sendtest.AmsSendTest..Print
void                 AmsSendTest_Print(ams_sendtest::AmsSendTest& row, algo::cstring& str) __attribute__((nothrow));

// --- ams_sendtest.FChild
// create: ams_sendtest.FDb.child (Lary)
// global access: child (Lary, by rowid)
struct FChild { // ams_sendtest.FChild
    algo::cstring           child_path;      //   "bin/ams_sendtest"  path for executable
    command::ams_sendtest   child_cmd;       // command line for child process
    algo::cstring           child_fstdin;    // redirect for stdin
    algo::cstring           child_fstdout;   // redirect for stdout
    algo::cstring           child_fstderr;   // redirect for stderr
    pid_t                   child_pid;       //   0  pid of running child process
    i32                     child_timeout;   //   0  optional timeout for child process
    i32                     child_status;    //   0  last exit status of child process
    ams::ProcId             proc_id;         //
private:
    // func:ams_sendtest.FChild..Ctor
    inline               FChild() __attribute__((nothrow));
    // func:ams_sendtest.FChild..Dtor
    inline               ~FChild() __attribute__((nothrow));
    friend ams_sendtest::FChild& child_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend ams_sendtest::FChild* child_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 child_RemoveAll() __attribute__((nothrow));
    friend void                 child_RemoveLast() __attribute__((nothrow));
};

// Start subprocess
// If subprocess already running, do nothing. Otherwise, start it
// func:ams_sendtest.FChild.child.Start
int                  child_Start(ams_sendtest::FChild& child) __attribute__((nothrow));
// Start subprocess & Read output
// func:ams_sendtest.FChild.child.StartRead
algo::Fildes         child_StartRead(ams_sendtest::FChild& child, algo_lib::FFildes &read) __attribute__((nothrow));
// Kill subprocess and wait
// func:ams_sendtest.FChild.child.Kill
void                 child_Kill(ams_sendtest::FChild& child);
// Wait for subprocess to return
// func:ams_sendtest.FChild.child.Wait
void                 child_Wait(ams_sendtest::FChild& child) __attribute__((nothrow));
// Start + Wait
// Execute subprocess and return exit code
// func:ams_sendtest.FChild.child.Exec
int                  child_Exec(ams_sendtest::FChild& child) __attribute__((nothrow));
// Start + Wait, throw exception on error
// Execute subprocess; throw human-readable exception on error
// func:ams_sendtest.FChild.child.ExecX
void                 child_ExecX(ams_sendtest::FChild& child);
// Call execv()
// Call execv with specified parameters
// func:ams_sendtest.FChild.child.Execv
int                  child_Execv(ams_sendtest::FChild& child) __attribute__((nothrow));
// func:ams_sendtest.FChild.child.ToCmdline
algo::tempstr        child_ToCmdline(ams_sendtest::FChild& child) __attribute__((nothrow));
// Form array from the command line
// func:ams_sendtest.FChild.child.ToArgv
void                 child_ToArgv(ams_sendtest::FChild& child, algo::StringAry& args) __attribute__((nothrow));

// Set all fields to initial values.
// func:ams_sendtest.FChild..Init
void                 FChild_Init(ams_sendtest::FChild& child);
// func:ams_sendtest.FChild..Uninit
void                 FChild_Uninit(ams_sendtest::FChild& child) __attribute__((nothrow));

// --- ams_sendtest.trace
#pragma pack(push,1)
struct trace { // ams_sendtest.trace
    // func:ams_sendtest.trace..Ctor
    inline               trace() __attribute__((nothrow));
};
#pragma pack(pop)

// print string representation of ROW to string STR
// cfmt:ams_sendtest.trace.String  printfmt:Tuple
// func:ams_sendtest.trace..Print
void                 trace_Print(ams_sendtest::trace& row, algo::cstring& str) __attribute__((nothrow));

// --- ams_sendtest.FDb
// create: ams_sendtest.FDb._db (Global)
struct FDb { // ams_sendtest.FDb: In-memory database for ams_sendtest
    lib_ams::FStream*           c_out;            // Output goes here. optional pointer
    command::ams_sendtest       cmdline;          //
    ams_sendtest::AmsSendTest   ams_send_test;    //
    ams_sendtest::FChild*       child_lary[32];   // level array
    i32                         child_n;          // number of elements in array
    ams_sendtest::trace         trace;            //
};

// Read argc,argv directly into the fields of the command line(s)
// The following fields are updated:
//     ams_sendtest.FDb.cmdline
//     algo_lib.FDb.cmdline
// func:ams_sendtest.FDb._db.ReadArgv
void                 ReadArgv() __attribute__((nothrow));
// Main loop.
// func:ams_sendtest.FDb._db.MainLoop
void                 MainLoop();
// Main step
// func:ams_sendtest.FDb._db.Step
void                 Step();
// Main function
// func:ams_sendtest.FDb._db.Main
// this function is 'extrn' and implemented by user
void                 Main();
// Parse strptr into known type and add to database.
// Return value is true unless an error occurs. If return value is false, algo_lib::_db.errtext has error text
// func:ams_sendtest.FDb._db.InsertStrptrMaybe
bool                 InsertStrptrMaybe(algo::strptr str);
// Load all finputs from given directory.
// func:ams_sendtest.FDb._db.LoadTuplesMaybe
bool                 LoadTuplesMaybe(algo::strptr root, bool recursive) __attribute__((nothrow));
// Load all finputs from given file.
// Read tuples from file FNAME into this namespace's in-memory database.
// If RECURSIVE is TRUE, then also load these tuples into any parent namespaces
// It a file referred to by FNAME is missing, no error is reported (it's considered an empty set).
// Function returns TRUE if all records were parsed and inserted without error.
// If the function returns FALSE, use algo_lib::DetachBadTags() for error description
// func:ams_sendtest.FDb._db.LoadTuplesFile
bool                 LoadTuplesFile(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load all finputs from given file descriptor.
// func:ams_sendtest.FDb._db.LoadTuplesFd
bool                 LoadTuplesFd(algo::Fildes fd, algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load specified ssimfile.
// func:ams_sendtest.FDb._db.LoadSsimfileMaybe
bool                 LoadSsimfileMaybe(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Calls Step function of dependencies
// func:ams_sendtest.FDb._db.Steps
void                 Steps();
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:ams_sendtest.FDb._db.XrefMaybe
bool                 _db_XrefMaybe();

// Allocate memory for new default row.
// If out of memory, process is killed.
// func:ams_sendtest.FDb.child.Alloc
ams_sendtest::FChild& child_Alloc() __attribute__((__warn_unused_result__, nothrow));
// Allocate memory for new element. If out of memory, return NULL.
// func:ams_sendtest.FDb.child.AllocMaybe
ams_sendtest::FChild* child_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
// Allocate space for one element. If no memory available, return NULL.
// func:ams_sendtest.FDb.child.AllocMem
void*                child_AllocMem() __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:ams_sendtest.FDb.child.EmptyQ
inline bool          child_EmptyQ() __attribute__((nothrow, pure));
// Look up row by row id. Return NULL if out of range
// func:ams_sendtest.FDb.child.Find
inline ams_sendtest::FChild* child_Find(u64 t) __attribute__((__warn_unused_result__, nothrow, pure));
// Return pointer to last element of array, or NULL if array is empty
// func:ams_sendtest.FDb.child.Last
inline ams_sendtest::FChild* child_Last() __attribute__((nothrow, pure));
// Return number of items in the pool
// func:ams_sendtest.FDb.child.N
inline i32           child_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Remove all elements from Lary
// func:ams_sendtest.FDb.child.RemoveAll
void                 child_RemoveAll() __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:ams_sendtest.FDb.child.RemoveLast
void                 child_RemoveLast() __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:ams_sendtest.FDb.child.qFind
inline ams_sendtest::FChild& child_qFind(u64 t) __attribute__((nothrow, pure));
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:ams_sendtest.FDb.child.XrefMaybe
bool                 child_XrefMaybe(ams_sendtest::FChild &row);

// cursor points to valid item
// func:ams_sendtest.FDb.child_curs.Reset
inline void          _db_child_curs_Reset(_db_child_curs &curs, ams_sendtest::FDb &parent) __attribute__((nothrow));
// cursor points to valid item
// func:ams_sendtest.FDb.child_curs.ValidQ
inline bool          _db_child_curs_ValidQ(_db_child_curs &curs) __attribute__((nothrow));
// proceed to next item
// func:ams_sendtest.FDb.child_curs.Next
inline void          _db_child_curs_Next(_db_child_curs &curs) __attribute__((nothrow));
// item access
// func:ams_sendtest.FDb.child_curs.Access
inline ams_sendtest::FChild& _db_child_curs_Access(_db_child_curs &curs) __attribute__((nothrow));
// Set all fields to initial values.
// func:ams_sendtest.FDb..Init
void                 FDb_Init();
// func:ams_sendtest.FDb..Uninit
void                 FDb_Uninit() __attribute__((nothrow));

// --- ams_sendtest.FieldId
#pragma pack(push,1)
struct FieldId { // ams_sendtest.FieldId: Field read helper
    i32   value;   //   -1
    // func:ams_sendtest.FieldId.value.Cast
    inline               operator ams_sendtest_FieldIdEnum() const __attribute__((nothrow));
    // func:ams_sendtest.FieldId..Ctor
    inline               FieldId() __attribute__((nothrow));
    // func:ams_sendtest.FieldId..FieldwiseCtor
    explicit inline               FieldId(i32 in_value) __attribute__((nothrow));
    // func:ams_sendtest.FieldId..EnumCtor
    inline               FieldId(ams_sendtest_FieldIdEnum arg) __attribute__((nothrow));
};
#pragma pack(pop)

// Get value of field as enum type
// func:ams_sendtest.FieldId.value.GetEnum
inline ams_sendtest_FieldIdEnum value_GetEnum(const ams_sendtest::FieldId& parent) __attribute__((nothrow));
// Set value of field from enum type.
// func:ams_sendtest.FieldId.value.SetEnum
inline void          value_SetEnum(ams_sendtest::FieldId& parent, ams_sendtest_FieldIdEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
// func:ams_sendtest.FieldId.value.ToCstr
const char*          value_ToCstr(const ams_sendtest::FieldId& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
// func:ams_sendtest.FieldId.value.Print
void                 value_Print(const ams_sendtest::FieldId& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
// func:ams_sendtest.FieldId.value.SetStrptrMaybe
bool                 value_SetStrptrMaybe(ams_sendtest::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
// func:ams_sendtest.FieldId.value.SetStrptr
void                 value_SetStrptr(ams_sendtest::FieldId& parent, algo::strptr rhs, ams_sendtest_FieldIdEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
// func:ams_sendtest.FieldId.value.ReadStrptrMaybe
bool                 value_ReadStrptrMaybe(ams_sendtest::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));

// Read fields of ams_sendtest::FieldId from an ascii string.
// The format of the string is the format of the ams_sendtest::FieldId's only field
// func:ams_sendtest.FieldId..ReadStrptrMaybe
bool                 FieldId_ReadStrptrMaybe(ams_sendtest::FieldId &parent, algo::strptr in_str) __attribute__((nothrow));
// Set all fields to initial values.
// func:ams_sendtest.FieldId..Init
inline void          FieldId_Init(ams_sendtest::FieldId& parent);
// print string representation of ROW to string STR
// cfmt:ams_sendtest.FieldId.String  printfmt:Raw
// func:ams_sendtest.FieldId..Print
void                 FieldId_Print(ams_sendtest::FieldId& row, algo::cstring& str) __attribute__((nothrow));
} // gen:ns_print_struct
namespace ams_sendtest { // gen:ns_curstext

struct _db_child_curs {// cursor
    typedef ams_sendtest::FChild ChildType;
    ams_sendtest::FDb *parent;
    i64 index;
    _db_child_curs(){ parent=NULL; index=0; }
};

} // gen:ns_curstext
namespace ams_sendtest { // gen:ns_func
// func:ams_sendtest...StaticCheck
void                 StaticCheck();
} // gen:ns_func
// func:ams_sendtest...main
int                  main(int argc, char **argv);
#if defined(WIN32)
// func:ams_sendtest...WinMain
int WINAPI           WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
#endif
// gen:ns_operators
namespace algo {
inline algo::cstring &operator <<(algo::cstring &str, const ams_sendtest::AmsSendTest &row);// cfmt:ams_sendtest.AmsSendTest.String
inline algo::cstring &operator <<(algo::cstring &str, const ams_sendtest::trace &row);// cfmt:ams_sendtest.trace.String
inline algo::cstring &operator <<(algo::cstring &str, const ams_sendtest::FieldId &row);// cfmt:ams_sendtest.FieldId.String
}
