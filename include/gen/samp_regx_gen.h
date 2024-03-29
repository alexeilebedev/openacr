//
// include/gen/samp_regx_gen.h
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

// --- samp_regx_FieldIdEnum

enum samp_regx_FieldIdEnum {        // samp_regx.FieldId.value
     samp_regx_FieldId_value   = 0
};

enum { samp_regx_FieldIdEnum_N = 1 };

namespace samp_regx { // gen:ns_pkeytypedef
} // gen:ns_pkeytypedef
namespace samp_regx { // gen:ns_field
extern const char *samp_regx_help;
} // gen:ns_field
// gen:ns_fwddecl2
namespace samp_regx { struct trace; }
namespace samp_regx { struct FDb; }
namespace samp_regx { struct FieldId; }
namespace samp_regx { extern struct samp_regx::FDb _db; }
namespace samp_regx { // gen:ns_print_struct

// --- samp_regx.trace
#pragma pack(push,1)
struct trace { // samp_regx.trace
    trace();
};
#pragma pack(pop)

// print string representation of samp_regx::trace to string LHS, no header -- cprint:samp_regx.trace.String
void                 trace_Print(samp_regx::trace & row, algo::cstring &str) __attribute__((nothrow));

// --- samp_regx.FDb
// create: samp_regx.FDb._db (Global)
struct FDb { // samp_regx.FDb
    command::samp_regx   cmdline;   //
    samp_regx::trace     trace;     //
};

// Read argc,argv directly into the fields of the command line(s)
// The following fields are updated:
//     samp_regx.FDb.cmdline
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

// --- samp_regx.FieldId
#pragma pack(push,1)
struct FieldId { // samp_regx.FieldId: Field read helper
    i32   value;   //   -1
    inline operator samp_regx_FieldIdEnum() const;
    explicit FieldId(i32                            in_value);
    FieldId(samp_regx_FieldIdEnum arg);
    FieldId();
};
#pragma pack(pop)

// Get value of field as enum type
samp_regx_FieldIdEnum value_GetEnum(const samp_regx::FieldId& parent) __attribute__((nothrow));
// Set value of field from enum type.
void                 value_SetEnum(samp_regx::FieldId& parent, samp_regx_FieldIdEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
const char*          value_ToCstr(const samp_regx::FieldId& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
void                 value_Print(const samp_regx::FieldId& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
bool                 value_SetStrptrMaybe(samp_regx::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
void                 value_SetStrptr(samp_regx::FieldId& parent, algo::strptr rhs, samp_regx_FieldIdEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
bool                 value_ReadStrptrMaybe(samp_regx::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));

// Read fields of samp_regx::FieldId from an ascii string.
// The format of the string is the format of the samp_regx::FieldId's only field
bool                 FieldId_ReadStrptrMaybe(samp_regx::FieldId &parent, algo::strptr in_str);
// Set all fields to initial values.
void                 FieldId_Init(samp_regx::FieldId& parent);
// print string representation of samp_regx::FieldId to string LHS, no header -- cprint:samp_regx.FieldId.String
void                 FieldId_Print(samp_regx::FieldId & row, algo::cstring &str) __attribute__((nothrow));
} // gen:ns_print_struct
namespace samp_regx { // gen:ns_func
} // gen:ns_func
int                  main(int argc, char **argv);
#if defined(WIN32)
int WINAPI           WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
#endif
// gen:ns_operators
namespace algo {
inline algo::cstring &operator <<(algo::cstring &str, const samp_regx::trace &row);// cfmt:samp_regx.trace.String
inline algo::cstring &operator <<(algo::cstring &str, const samp_regx::FieldId &row);// cfmt:samp_regx.FieldId.String
}
