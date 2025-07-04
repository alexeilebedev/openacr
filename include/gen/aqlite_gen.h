//
// include/gen/aqlite_gen.h
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
#include "include/lib_sqlite.h"
#include "include/gen/command_gen.h"
#include "include/gen/algo_gen.h"
//#pragma endinclude
// gen:ns_enums

// --- aqlite_FieldIdEnum

enum aqlite_FieldIdEnum {        // aqlite.FieldId.value
     aqlite_FieldId_value   = 0
};

enum { aqlite_FieldIdEnum_N = 1 };

namespace aqlite { // gen:ns_pkeytypedef
} // gen:ns_pkeytypedef
namespace aqlite { // gen:ns_tclass_field
extern const char *aqlite_help;
} // gen:ns_tclass_field
// gen:ns_fwddecl2
namespace aqlite { struct trace; }
namespace aqlite { struct FDb; }
namespace aqlite { struct FieldId; }
namespace aqlite { extern struct aqlite::FDb _db; }
namespace aqlite { // gen:ns_print_struct

// --- aqlite.trace
#pragma pack(push,1)
struct trace { // aqlite.trace
    // func:aqlite.trace..Ctor
    inline               trace() __attribute__((nothrow));
};
#pragma pack(pop)

// print string representation of ROW to string STR
// cfmt:aqlite.trace.String  printfmt:Tuple
// func:aqlite.trace..Print
void                 trace_Print(aqlite::trace& row, algo::cstring& str) __attribute__((nothrow));

// --- aqlite.FDb
// create: aqlite.FDb._db (Global)
struct FDb { // aqlite.FDb
    command::aqlite   cmdline;     //
    algo::Tuple       last_cols;   //
    i32               stmt;        //   0
    aqlite::trace     trace;       //
};

// Read argc,argv directly into the fields of the command line(s)
// The following fields are updated:
//     aqlite.FDb.cmdline
//     algo_lib.FDb.cmdline
// func:aqlite.FDb._db.ReadArgv
void                 ReadArgv() __attribute__((nothrow));
// Main loop.
// func:aqlite.FDb._db.MainLoop
void                 MainLoop();
// Main step
// func:aqlite.FDb._db.Step
void                 Step();
// Main function
// func:aqlite.FDb._db.Main
// this function is 'extrn' and implemented by user
void                 Main();
// Parse strptr into known type and add to database.
// Return value is true unless an error occurs. If return value is false, algo_lib::_db.errtext has error text
// func:aqlite.FDb._db.InsertStrptrMaybe
bool                 InsertStrptrMaybe(algo::strptr str);
// Load all finputs from given directory.
// func:aqlite.FDb._db.LoadTuplesMaybe
bool                 LoadTuplesMaybe(algo::strptr root, bool recursive) __attribute__((nothrow));
// Load all finputs from given file.
// Read tuples from file FNAME into this namespace's in-memory database.
// If RECURSIVE is TRUE, then also load these tuples into any parent namespaces
// It a file referred to by FNAME is missing, no error is reported (it's considered an empty set).
// Function returns TRUE if all records were parsed and inserted without error.
// If the function returns FALSE, use algo_lib::DetachBadTags() for error description
// func:aqlite.FDb._db.LoadTuplesFile
bool                 LoadTuplesFile(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load all finputs from given file descriptor.
// func:aqlite.FDb._db.LoadTuplesFd
bool                 LoadTuplesFd(algo::Fildes fd, algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load specified ssimfile.
// func:aqlite.FDb._db.LoadSsimfileMaybe
bool                 LoadSsimfileMaybe(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Calls Step function of dependencies
// func:aqlite.FDb._db.Steps
void                 Steps();
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:aqlite.FDb._db.XrefMaybe
bool                 _db_XrefMaybe();

// Set all fields to initial values.
// func:aqlite.FDb..Init
void                 FDb_Init();
// func:aqlite.FDb..Uninit
inline void          FDb_Uninit() __attribute__((nothrow));

// --- aqlite.FieldId
#pragma pack(push,1)
struct FieldId { // aqlite.FieldId: Field read helper
    i32   value;   //   -1
    // func:aqlite.FieldId.value.Cast
    inline               operator aqlite_FieldIdEnum() const __attribute__((nothrow));
    // func:aqlite.FieldId..Ctor
    inline               FieldId() __attribute__((nothrow));
    // func:aqlite.FieldId..FieldwiseCtor
    explicit inline               FieldId(i32 in_value) __attribute__((nothrow));
    // func:aqlite.FieldId..EnumCtor
    inline               FieldId(aqlite_FieldIdEnum arg) __attribute__((nothrow));
};
#pragma pack(pop)

// Get value of field as enum type
// func:aqlite.FieldId.value.GetEnum
inline aqlite_FieldIdEnum value_GetEnum(const aqlite::FieldId& parent) __attribute__((nothrow));
// Set value of field from enum type.
// func:aqlite.FieldId.value.SetEnum
inline void          value_SetEnum(aqlite::FieldId& parent, aqlite_FieldIdEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
// func:aqlite.FieldId.value.ToCstr
const char*          value_ToCstr(const aqlite::FieldId& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
// func:aqlite.FieldId.value.Print
void                 value_Print(const aqlite::FieldId& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
// func:aqlite.FieldId.value.SetStrptrMaybe
bool                 value_SetStrptrMaybe(aqlite::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
// func:aqlite.FieldId.value.SetStrptr
void                 value_SetStrptr(aqlite::FieldId& parent, algo::strptr rhs, aqlite_FieldIdEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
// func:aqlite.FieldId.value.ReadStrptrMaybe
bool                 value_ReadStrptrMaybe(aqlite::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));

// Read fields of aqlite::FieldId from an ascii string.
// The format of the string is the format of the aqlite::FieldId's only field
// func:aqlite.FieldId..ReadStrptrMaybe
bool                 FieldId_ReadStrptrMaybe(aqlite::FieldId &parent, algo::strptr in_str) __attribute__((nothrow));
// Set all fields to initial values.
// func:aqlite.FieldId..Init
inline void          FieldId_Init(aqlite::FieldId& parent);
// print string representation of ROW to string STR
// cfmt:aqlite.FieldId.String  printfmt:Raw
// func:aqlite.FieldId..Print
void                 FieldId_Print(aqlite::FieldId& row, algo::cstring& str) __attribute__((nothrow));
} // gen:ns_print_struct
namespace aqlite { // gen:ns_func
// func:aqlite...StaticCheck
void                 StaticCheck();
} // gen:ns_func
// func:aqlite...main
int                  main(int argc, char **argv);
#if defined(WIN32)
// func:aqlite...WinMain
int WINAPI           WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
#endif
// gen:ns_operators
namespace algo {
inline algo::cstring &operator <<(algo::cstring &str, const aqlite::trace &row);// cfmt:aqlite.trace.String
inline algo::cstring &operator <<(algo::cstring &str, const aqlite::FieldId &row);// cfmt:aqlite.FieldId.String
}
