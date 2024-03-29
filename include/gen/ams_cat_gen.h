//
// include/gen/ams_cat_gen.h
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
//#pragma endinclude
// gen:ns_enums

// --- ams_cat_FieldIdEnum

enum ams_cat_FieldIdEnum {        // ams_cat.FieldId.value
     ams_cat_FieldId_value   = 0
};

enum { ams_cat_FieldIdEnum_N = 1 };

namespace ams_cat { // gen:ns_pkeytypedef
} // gen:ns_pkeytypedef
namespace ams_cat { // gen:ns_field
extern const char *ams_cat_help;
} // gen:ns_field
// gen:ns_fwddecl2
namespace ams_cat { struct trace; }
namespace ams_cat { struct FDb; }
namespace ams_cat { struct FieldId; }
namespace ams_cat { extern struct ams_cat::FDb _db; }
namespace ams_cat { // gen:ns_print_struct

// --- ams_cat.trace
#pragma pack(push,1)
struct trace { // ams_cat.trace
    trace();
};
#pragma pack(pop)

// print string representation of ams_cat::trace to string LHS, no header -- cprint:ams_cat.trace.String
void                 trace_Print(ams_cat::trace & row, algo::cstring &str) __attribute__((nothrow));

// --- ams_cat.FDb
// create: ams_cat.FDb._db (Global)
struct FDb { // ams_cat.FDb
    command::ams_cat   cmdline;   //
    ams_cat::trace     trace;     //
};

// Read argc,argv directly into the fields of the command line(s)
// The following fields are updated:
//     ams_cat.FDb.cmdline
//     algo_lib.FDb.cmdline
void                 ReadArgv() __attribute__((nothrow));
// Main loop.
void                 MainLoop();
// Main step
void                 Step();
// Main function
void                 Main();
void                 StaticCheck();
// Parse strptr into known type and add to database.
// Return value is true unless an error occurs. If return value is false, algo_lib::_db.errtext has error text
bool                 InsertStrptrMaybe(algo::strptr str);
// Load all finputs from given directory.
bool                 LoadTuplesMaybe(algo::strptr root, bool recursive) __attribute__((nothrow));
// Load all finputs from given file.
// Read tuples from file FNAME into this namespace's in-memory database.
// If RECURSIVE is TRUE, then also load these tuples into any parent namespaces
// It a file referred to by FNAME is missing, no error is reported (it's considered an empty set).
// Function returns TRUE if all records were parsed and inserted without error.
// If the function returns FALSE, use algo_lib::DetachBadTags() for error description
bool                 LoadTuplesFile(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load all finputs from given file descriptor.
bool                 LoadTuplesFd(algo::Fildes fd, algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load specified ssimfile.
bool                 LoadSsimfileMaybe(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Calls Step function of dependencies
void                 Steps();
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
bool                 _db_XrefMaybe();

// Set all fields to initial values.
void                 FDb_Init();
void                 FDb_Uninit() __attribute__((nothrow));

// --- ams_cat.FieldId
#pragma pack(push,1)
struct FieldId { // ams_cat.FieldId: Field read helper
    i32   value;   //   -1
    inline operator ams_cat_FieldIdEnum() const;
    explicit FieldId(i32                            in_value);
    FieldId(ams_cat_FieldIdEnum arg);
    FieldId();
};
#pragma pack(pop)

// Get value of field as enum type
ams_cat_FieldIdEnum  value_GetEnum(const ams_cat::FieldId& parent) __attribute__((nothrow));
// Set value of field from enum type.
void                 value_SetEnum(ams_cat::FieldId& parent, ams_cat_FieldIdEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
const char*          value_ToCstr(const ams_cat::FieldId& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
void                 value_Print(const ams_cat::FieldId& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
bool                 value_SetStrptrMaybe(ams_cat::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
void                 value_SetStrptr(ams_cat::FieldId& parent, algo::strptr rhs, ams_cat_FieldIdEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
bool                 value_ReadStrptrMaybe(ams_cat::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));

// Read fields of ams_cat::FieldId from an ascii string.
// The format of the string is the format of the ams_cat::FieldId's only field
bool                 FieldId_ReadStrptrMaybe(ams_cat::FieldId &parent, algo::strptr in_str);
// Set all fields to initial values.
void                 FieldId_Init(ams_cat::FieldId& parent);
// print string representation of ams_cat::FieldId to string LHS, no header -- cprint:ams_cat.FieldId.String
void                 FieldId_Print(ams_cat::FieldId & row, algo::cstring &str) __attribute__((nothrow));
} // gen:ns_print_struct
namespace ams_cat { // gen:ns_func
} // gen:ns_func
int                  main(int argc, char **argv);
#if defined(WIN32)
int WINAPI           WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
#endif
// gen:ns_operators
namespace algo {
inline algo::cstring &operator <<(algo::cstring &str, const ams_cat::trace &row);// cfmt:ams_cat.trace.String
inline algo::cstring &operator <<(algo::cstring &str, const ams_cat::FieldId &row);// cfmt:ams_cat.FieldId.String
}
