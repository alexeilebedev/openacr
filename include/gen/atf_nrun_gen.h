//
// include/gen/atf_nrun_gen.h
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
#include "include/gen/command_gen.h"
#include "include/gen/algo_gen.h"
//#pragma endinclude
// gen:ns_enums

// --- atf_nrun_FieldIdEnum

enum atf_nrun_FieldIdEnum {        // atf_nrun.FieldId.value
     atf_nrun_FieldId_value   = 0
};

enum { atf_nrun_FieldIdEnum_N = 1 };

namespace atf_nrun { // gen:ns_pkeytypedef
} // gen:ns_pkeytypedef
namespace atf_nrun { // gen:ns_tclass_field
extern const char *atf_nrun_help;
} // gen:ns_tclass_field
// gen:ns_fwddecl2
namespace atf_nrun { struct _db_fentry_curs; }
namespace atf_nrun { struct _db_zd_todo_curs; }
namespace atf_nrun { struct trace; }
namespace atf_nrun { struct FDb; }
namespace atf_nrun { struct FEntry; }
namespace atf_nrun { struct FieldId; }
namespace atf_nrun { extern struct atf_nrun::FDb _db; }
namespace atf_nrun { // gen:ns_print_struct

// --- atf_nrun.trace
#pragma pack(push,1)
struct trace { // atf_nrun.trace
    // func:atf_nrun.trace..Ctor
    inline               trace() __attribute__((nothrow));
};
#pragma pack(pop)

// print string representation of ROW to string STR
// cfmt:atf_nrun.trace.String  printfmt:Tuple
// func:atf_nrun.trace..Print
void                 trace_Print(atf_nrun::trace& row, algo::cstring& str) __attribute__((nothrow));

// --- atf_nrun.FDb
// create: atf_nrun.FDb._db (Global)
struct FDb { // atf_nrun.FDb: In-memory database for atf_nrun
    command::atf_nrun    cmdline;                     //
    atf_nrun::FEntry*    fentry_lary[32];             // level array
    i32                  fentry_n;                    // number of elements in array
    atf_nrun::FEntry**   ind_running_buckets_elems;   // pointer to bucket array
    i32                  ind_running_buckets_n;       // number of elements in bucket array
    i32                  ind_running_n;               // number of elements in the hash table
    algo::SchedTime      ind_running_next;            // atf_nrun.FDb.ind_running              Next invocation time
    algo::SchedTime      ind_running_delay;           // atf_nrun.FDb.ind_running              Delay between invocations
    atf_nrun::FEntry*    zd_todo_head;                // zero-terminated doubly linked list
    i32                  zd_todo_n;                   // zero-terminated doubly linked list
    atf_nrun::FEntry*    zd_todo_tail;                // pointer to last element
    algo::SchedTime      zd_todo_next;                // atf_nrun.FDb.zd_todo                  Next invocation time
    algo::SchedTime      zd_todo_delay;               // atf_nrun.FDb.zd_todo                  Delay between invocations
    atf_nrun::trace      trace;                       //
};

// Read argc,argv directly into the fields of the command line(s)
// The following fields are updated:
//     atf_nrun.FDb.cmdline
//     algo_lib.FDb.cmdline
// func:atf_nrun.FDb._db.ReadArgv
void                 ReadArgv() __attribute__((nothrow));
// Main loop.
// func:atf_nrun.FDb._db.MainLoop
void                 MainLoop();
// Main step
// func:atf_nrun.FDb._db.Step
void                 Step();
// Main function
// func:atf_nrun.FDb._db.Main
// this function is 'extrn' and implemented by user
void                 Main();
// Parse strptr into known type and add to database.
// Return value is true unless an error occurs. If return value is false, algo_lib::_db.errtext has error text
// func:atf_nrun.FDb._db.InsertStrptrMaybe
bool                 InsertStrptrMaybe(algo::strptr str);
// Load all finputs from given directory.
// func:atf_nrun.FDb._db.LoadTuplesMaybe
bool                 LoadTuplesMaybe(algo::strptr root, bool recursive) __attribute__((nothrow));
// Load all finputs from given file.
// Read tuples from file FNAME into this namespace's in-memory database.
// If RECURSIVE is TRUE, then also load these tuples into any parent namespaces
// It a file referred to by FNAME is missing, no error is reported (it's considered an empty set).
// Function returns TRUE if all records were parsed and inserted without error.
// If the function returns FALSE, use algo_lib::DetachBadTags() for error description
// func:atf_nrun.FDb._db.LoadTuplesFile
bool                 LoadTuplesFile(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load all finputs from given file descriptor.
// func:atf_nrun.FDb._db.LoadTuplesFd
bool                 LoadTuplesFd(algo::Fildes fd, algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load specified ssimfile.
// func:atf_nrun.FDb._db.LoadSsimfileMaybe
bool                 LoadSsimfileMaybe(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Calls Step function of dependencies
// func:atf_nrun.FDb._db.Steps
void                 Steps();
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:atf_nrun.FDb._db.XrefMaybe
bool                 _db_XrefMaybe();

// Allocate memory for new default row.
// If out of memory, process is killed.
// func:atf_nrun.FDb.fentry.Alloc
atf_nrun::FEntry&    fentry_Alloc() __attribute__((__warn_unused_result__, nothrow));
// Allocate memory for new element. If out of memory, return NULL.
// func:atf_nrun.FDb.fentry.AllocMaybe
atf_nrun::FEntry*    fentry_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
// Allocate space for one element. If no memory available, return NULL.
// func:atf_nrun.FDb.fentry.AllocMem
void*                fentry_AllocMem() __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:atf_nrun.FDb.fentry.EmptyQ
inline bool          fentry_EmptyQ() __attribute__((nothrow, pure));
// Look up row by row id. Return NULL if out of range
// func:atf_nrun.FDb.fentry.Find
inline atf_nrun::FEntry* fentry_Find(u64 t) __attribute__((__warn_unused_result__, nothrow, pure));
// Return pointer to last element of array, or NULL if array is empty
// func:atf_nrun.FDb.fentry.Last
inline atf_nrun::FEntry* fentry_Last() __attribute__((nothrow, pure));
// Return number of items in the pool
// func:atf_nrun.FDb.fentry.N
inline i32           fentry_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Remove all elements from Lary
// func:atf_nrun.FDb.fentry.RemoveAll
void                 fentry_RemoveAll() __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:atf_nrun.FDb.fentry.RemoveLast
void                 fentry_RemoveLast() __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:atf_nrun.FDb.fentry.qFind
inline atf_nrun::FEntry& fentry_qFind(u64 t) __attribute__((nothrow, pure));
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:atf_nrun.FDb.fentry.XrefMaybe
bool                 fentry_XrefMaybe(atf_nrun::FEntry &row);

// Return true if hash is empty
// func:atf_nrun.FDb.ind_running.EmptyQ
inline bool          ind_running_EmptyQ() __attribute__((nothrow));
// Find row by key. Return NULL if not found.
// func:atf_nrun.FDb.ind_running.Find
atf_nrun::FEntry*    ind_running_Find(i32 key) __attribute__((__warn_unused_result__, nothrow));
// Look up row by key and return reference. Throw exception if not found
// func:atf_nrun.FDb.ind_running.FindX
atf_nrun::FEntry&    ind_running_FindX(i32 key);
// Find row by key. If not found, create and x-reference a new row with with this key.
// func:atf_nrun.FDb.ind_running.GetOrCreate
atf_nrun::FEntry&    ind_running_GetOrCreate(i32 key) __attribute__((nothrow));
// Return number of items in the hash
// func:atf_nrun.FDb.ind_running.N
inline i32           ind_running_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Insert row into hash table. Return true if row is reachable through the hash after the function completes.
// func:atf_nrun.FDb.ind_running.InsertMaybe
bool                 ind_running_InsertMaybe(atf_nrun::FEntry& row) __attribute__((nothrow));
// Remove reference to element from hash index. If element is not in hash, do nothing
// func:atf_nrun.FDb.ind_running.Remove
void                 ind_running_Remove(atf_nrun::FEntry& row) __attribute__((nothrow));
// Reserve enough room in the hash for N more elements. Return success code.
// func:atf_nrun.FDb.ind_running.Reserve
void                 ind_running_Reserve(int n) __attribute__((nothrow));
// func:atf_nrun.FDb.ind_running.Step
// this function is 'extrn' and implemented by user
void                 ind_running_Step() __attribute__((nothrow));
// Set inter-step delay to specified value.
// The difference between new delay and current delay is added to the next scheduled time.
// func:atf_nrun.FDb.ind_running.SetDelay
void                 ind_running_SetDelay(algo::SchedTime delay) __attribute__((nothrow));

// Return true if index is empty
// func:atf_nrun.FDb.zd_todo.EmptyQ
inline bool          zd_todo_EmptyQ() __attribute__((__warn_unused_result__, nothrow, pure));
// If index empty, return NULL. Otherwise return pointer to first element in index
// func:atf_nrun.FDb.zd_todo.First
inline atf_nrun::FEntry* zd_todo_First() __attribute__((__warn_unused_result__, nothrow, pure));
// Return true if row is in the linked list, false otherwise
// func:atf_nrun.FDb.zd_todo.InLlistQ
inline bool          zd_todo_InLlistQ(atf_nrun::FEntry& row) __attribute__((__warn_unused_result__, nothrow));
// Insert row into linked list. If row is already in linked list, do nothing.
// func:atf_nrun.FDb.zd_todo.Insert
void                 zd_todo_Insert(atf_nrun::FEntry& row) __attribute__((nothrow));
// If index empty, return NULL. Otherwise return pointer to last element in index
// func:atf_nrun.FDb.zd_todo.Last
inline atf_nrun::FEntry* zd_todo_Last() __attribute__((__warn_unused_result__, nothrow, pure));
// Return number of items in the linked list
// func:atf_nrun.FDb.zd_todo.N
inline i32           zd_todo_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Return pointer to next element in the list
// func:atf_nrun.FDb.zd_todo.Next
inline atf_nrun::FEntry* zd_todo_Next(atf_nrun::FEntry &row) __attribute__((__warn_unused_result__, nothrow));
// Return pointer to previous element in the list
// func:atf_nrun.FDb.zd_todo.Prev
inline atf_nrun::FEntry* zd_todo_Prev(atf_nrun::FEntry &row) __attribute__((__warn_unused_result__, nothrow));
// Remove element from index. If element is not in index, do nothing.
// func:atf_nrun.FDb.zd_todo.Remove
void                 zd_todo_Remove(atf_nrun::FEntry& row) __attribute__((nothrow));
// Empty the index. (The rows are not deleted)
// func:atf_nrun.FDb.zd_todo.RemoveAll
void                 zd_todo_RemoveAll() __attribute__((nothrow));
// If linked list is empty, return NULL. Otherwise unlink and return pointer to first element.
// Call FirstChanged trigger.
// func:atf_nrun.FDb.zd_todo.RemoveFirst
atf_nrun::FEntry*    zd_todo_RemoveFirst() __attribute__((nothrow));
// Return reference to last element in the index. No bounds checking.
// func:atf_nrun.FDb.zd_todo.qLast
inline atf_nrun::FEntry& zd_todo_qLast() __attribute__((__warn_unused_result__, nothrow));
// First element of index changed.
// func:atf_nrun.FDb.zd_todo.FirstChanged
void                 zd_todo_FirstChanged() __attribute__((nothrow));
// func:atf_nrun.FDb.zd_todo.Step
// this function is 'extrn' and implemented by user
void                 zd_todo_Step() __attribute__((nothrow));
// Set inter-step delay to specified value.
// The difference between new delay and current delay is added to the next scheduled time.
// func:atf_nrun.FDb.zd_todo.SetDelay
void                 zd_todo_SetDelay(algo::SchedTime delay) __attribute__((nothrow));

// cursor points to valid item
// func:atf_nrun.FDb.fentry_curs.Reset
inline void          _db_fentry_curs_Reset(_db_fentry_curs &curs, atf_nrun::FDb &parent) __attribute__((nothrow));
// cursor points to valid item
// func:atf_nrun.FDb.fentry_curs.ValidQ
inline bool          _db_fentry_curs_ValidQ(_db_fentry_curs &curs) __attribute__((nothrow));
// proceed to next item
// func:atf_nrun.FDb.fentry_curs.Next
inline void          _db_fentry_curs_Next(_db_fentry_curs &curs) __attribute__((nothrow));
// item access
// func:atf_nrun.FDb.fentry_curs.Access
inline atf_nrun::FEntry& _db_fentry_curs_Access(_db_fentry_curs &curs) __attribute__((nothrow));
// cursor points to valid item
// func:atf_nrun.FDb.zd_todo_curs.Reset
inline void          _db_zd_todo_curs_Reset(_db_zd_todo_curs &curs, atf_nrun::FDb &parent) __attribute__((nothrow));
// cursor points to valid item
// func:atf_nrun.FDb.zd_todo_curs.ValidQ
inline bool          _db_zd_todo_curs_ValidQ(_db_zd_todo_curs &curs) __attribute__((nothrow));
// proceed to next item
// func:atf_nrun.FDb.zd_todo_curs.Next
inline void          _db_zd_todo_curs_Next(_db_zd_todo_curs &curs) __attribute__((nothrow));
// item access
// func:atf_nrun.FDb.zd_todo_curs.Access
inline atf_nrun::FEntry& _db_zd_todo_curs_Access(_db_zd_todo_curs &curs) __attribute__((nothrow));
// Set all fields to initial values.
// func:atf_nrun.FDb..Init
void                 FDb_Init();
// func:atf_nrun.FDb..Uninit
void                 FDb_Uninit() __attribute__((nothrow));

// --- atf_nrun.FEntry
// create: atf_nrun.FDb.fentry (Lary)
// global access: fentry (Lary, by rowid)
// global access: ind_running (Thash, hash field pid)
// global access: zd_todo (Llist)
struct FEntry { // atf_nrun.FEntry
    atf_nrun::FEntry*   ind_running_next;   // hash next
    atf_nrun::FEntry*   zd_todo_next;       // zslist link; -1 means not-in-list
    atf_nrun::FEntry*   zd_todo_prev;       // previous element
    algo::cstring       command;            // Command to execute
    i32                 pid;                //   0  Pid of running bash job
    algo::cstring       job_path;           //   "bin/bash"  path for executable
    command::bash       job_cmd;            // command line for child process
    algo::cstring       job_fstdin;         // redirect for stdin
    algo::cstring       job_fstdout;        // redirect for stdout
    algo::cstring       job_fstderr;        // redirect for stderr
    pid_t               job_pid;            //   0  pid of running child process
    i32                 job_timeout;        //   0  optional timeout for child process
    i32                 job_status;         //   0  last exit status of child process
    // reftype Exec of atf_nrun.FEntry.job prohibits copy
    // func:atf_nrun.FEntry..AssignOp
    atf_nrun::FEntry&    operator =(const atf_nrun::FEntry &rhs) = delete;
    // reftype Exec of atf_nrun.FEntry.job prohibits copy
    // func:atf_nrun.FEntry..CopyCtor
    FEntry(const atf_nrun::FEntry &rhs) = delete;
private:
    // func:atf_nrun.FEntry..Ctor
    inline               FEntry() __attribute__((nothrow));
    // func:atf_nrun.FEntry..Dtor
    inline               ~FEntry() __attribute__((nothrow));
    friend atf_nrun::FEntry&    fentry_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_nrun::FEntry*    fentry_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fentry_RemoveAll() __attribute__((nothrow));
    friend void                 fentry_RemoveLast() __attribute__((nothrow));
};

// Start subprocess
// If subprocess already running, do nothing. Otherwise, start it
// func:atf_nrun.FEntry.job.Start
int                  job_Start(atf_nrun::FEntry& fentry) __attribute__((nothrow));
// Start subprocess & Read output
// func:atf_nrun.FEntry.job.StartRead
algo::Fildes         job_StartRead(atf_nrun::FEntry& fentry, algo_lib::FFildes &read) __attribute__((nothrow));
// Kill subprocess and wait
// func:atf_nrun.FEntry.job.Kill
void                 job_Kill(atf_nrun::FEntry& fentry);
// Wait for subprocess to return
// func:atf_nrun.FEntry.job.Wait
void                 job_Wait(atf_nrun::FEntry& fentry) __attribute__((nothrow));
// Start + Wait
// Execute subprocess and return exit code
// func:atf_nrun.FEntry.job.Exec
int                  job_Exec(atf_nrun::FEntry& fentry) __attribute__((nothrow));
// Start + Wait, throw exception on error
// Execute subprocess; throw human-readable exception on error
// func:atf_nrun.FEntry.job.ExecX
void                 job_ExecX(atf_nrun::FEntry& fentry);
// Call execv()
// Call execv with specified parameters
// func:atf_nrun.FEntry.job.Execv
int                  job_Execv(atf_nrun::FEntry& fentry) __attribute__((nothrow));
// func:atf_nrun.FEntry.job.ToCmdline
algo::tempstr        job_ToCmdline(atf_nrun::FEntry& fentry) __attribute__((nothrow));
// Form array from the command line
// func:atf_nrun.FEntry.job.ToArgv
void                 job_ToArgv(atf_nrun::FEntry& fentry, algo::StringAry& args) __attribute__((nothrow));

// Set all fields to initial values.
// func:atf_nrun.FEntry..Init
void                 FEntry_Init(atf_nrun::FEntry& fentry);
// func:atf_nrun.FEntry..Uninit
void                 FEntry_Uninit(atf_nrun::FEntry& fentry) __attribute__((nothrow));

// --- atf_nrun.FieldId
#pragma pack(push,1)
struct FieldId { // atf_nrun.FieldId: Field read helper
    i32   value;   //   -1
    // func:atf_nrun.FieldId.value.Cast
    inline               operator atf_nrun_FieldIdEnum() const __attribute__((nothrow));
    // func:atf_nrun.FieldId..Ctor
    inline               FieldId() __attribute__((nothrow));
    // func:atf_nrun.FieldId..FieldwiseCtor
    explicit inline               FieldId(i32 in_value) __attribute__((nothrow));
    // func:atf_nrun.FieldId..EnumCtor
    inline               FieldId(atf_nrun_FieldIdEnum arg) __attribute__((nothrow));
};
#pragma pack(pop)

// Get value of field as enum type
// func:atf_nrun.FieldId.value.GetEnum
inline atf_nrun_FieldIdEnum value_GetEnum(const atf_nrun::FieldId& parent) __attribute__((nothrow));
// Set value of field from enum type.
// func:atf_nrun.FieldId.value.SetEnum
inline void          value_SetEnum(atf_nrun::FieldId& parent, atf_nrun_FieldIdEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
// func:atf_nrun.FieldId.value.ToCstr
const char*          value_ToCstr(const atf_nrun::FieldId& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
// func:atf_nrun.FieldId.value.Print
void                 value_Print(const atf_nrun::FieldId& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
// func:atf_nrun.FieldId.value.SetStrptrMaybe
bool                 value_SetStrptrMaybe(atf_nrun::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
// func:atf_nrun.FieldId.value.SetStrptr
void                 value_SetStrptr(atf_nrun::FieldId& parent, algo::strptr rhs, atf_nrun_FieldIdEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
// func:atf_nrun.FieldId.value.ReadStrptrMaybe
bool                 value_ReadStrptrMaybe(atf_nrun::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));

// Read fields of atf_nrun::FieldId from an ascii string.
// The format of the string is the format of the atf_nrun::FieldId's only field
// func:atf_nrun.FieldId..ReadStrptrMaybe
bool                 FieldId_ReadStrptrMaybe(atf_nrun::FieldId &parent, algo::strptr in_str) __attribute__((nothrow));
// Set all fields to initial values.
// func:atf_nrun.FieldId..Init
inline void          FieldId_Init(atf_nrun::FieldId& parent);
// print string representation of ROW to string STR
// cfmt:atf_nrun.FieldId.String  printfmt:Raw
// func:atf_nrun.FieldId..Print
void                 FieldId_Print(atf_nrun::FieldId& row, algo::cstring& str) __attribute__((nothrow));
} // gen:ns_print_struct
namespace atf_nrun { // gen:ns_curstext

struct _db_fentry_curs {// cursor
    typedef atf_nrun::FEntry ChildType;
    atf_nrun::FDb *parent;
    i64 index;
    _db_fentry_curs(){ parent=NULL; index=0; }
};


struct _db_zd_todo_curs {// fcurs:atf_nrun.FDb.zd_todo/curs
    typedef atf_nrun::FEntry ChildType;
    atf_nrun::FEntry* row;
    _db_zd_todo_curs() {
        row = NULL;
    }
};

} // gen:ns_curstext
namespace atf_nrun { // gen:ns_func
// func:atf_nrun...StaticCheck
void                 StaticCheck();
} // gen:ns_func
// func:atf_nrun...main
int                  main(int argc, char **argv);
#if defined(WIN32)
// func:atf_nrun...WinMain
int WINAPI           WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
#endif
// gen:ns_operators
namespace algo {
inline algo::cstring &operator <<(algo::cstring &str, const atf_nrun::trace &row);// cfmt:atf_nrun.trace.String
inline algo::cstring &operator <<(algo::cstring &str, const atf_nrun::FieldId &row);// cfmt:atf_nrun.FieldId.String
}
