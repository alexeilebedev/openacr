//
// include/gen/mysql2ssim_gen.h
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
#include "include/gen/algo_lib_gen.h"
//#pragma endinclude
// gen:ns_enums

// --- mysql2ssim_FieldIdEnum

enum mysql2ssim_FieldIdEnum {        // mysql2ssim.FieldId.value
     mysql2ssim_FieldId_value   = 0
};

enum { mysql2ssim_FieldIdEnum_N = 1 };

namespace mysql2ssim { // gen:ns_pkeytypedef
} // gen:ns_pkeytypedef
namespace mysql2ssim { // gen:ns_tclass_field
extern const char *mysql2ssim_help;
} // gen:ns_tclass_field
// gen:ns_fwddecl2
namespace mysql2ssim { struct _db_table_names_curs; }
namespace mysql2ssim { struct _db_in_tables_curs; }
namespace mysql2ssim { struct FTobltin_vals_curs; }
namespace mysql2ssim { struct trace; }
namespace mysql2ssim { struct FDb; }
namespace mysql2ssim { struct FTobltin; }
namespace mysql2ssim { struct FieldId; }
namespace mysql2ssim { extern struct mysql2ssim::FDb _db; }
namespace mysql2ssim { // gen:ns_print_struct

// --- mysql2ssim.trace
#pragma pack(push,1)
struct trace { // mysql2ssim.trace
    // func:mysql2ssim.trace..Ctor
    inline               trace() __attribute__((nothrow));
};
#pragma pack(pop)

// print string representation of ROW to string STR
// cfmt:mysql2ssim.trace.String  printfmt:Tuple
// func:mysql2ssim.trace..Print
void                 trace_Print(mysql2ssim::trace& row, algo::cstring& str) __attribute__((nothrow));

// --- mysql2ssim.FDb
// create: mysql2ssim.FDb._db (Global)
struct FDb { // mysql2ssim.FDb: In-memory database for mysql2ssim
    algo::cstring*        table_names_elems;   // pointer to elements
    u32                   table_names_n;       // number of elements in array
    u32                   table_names_max;     // max. capacity of array before realloc
    algo::cstring*        in_tables_elems;     // pointer to elements
    u32                   in_tables_n;         // number of elements in array
    u32                   in_tables_max;       // max. capacity of array before realloc
    command::mysql2ssim   cmdline;             //
    algo_lib::FTempfile   tempfile;            // Temp file where downloaded mysql goes before being handed to acr
    mysql2ssim::trace     trace;               //
};

// Read argc,argv directly into the fields of the command line(s)
// The following fields are updated:
//     mysql2ssim.FDb.cmdline
//     algo_lib.FDb.cmdline
// func:mysql2ssim.FDb._db.ReadArgv
void                 ReadArgv() __attribute__((nothrow));
// Main loop.
// func:mysql2ssim.FDb._db.MainLoop
void                 MainLoop();
// Main step
// func:mysql2ssim.FDb._db.Step
void                 Step();
// Main function
// func:mysql2ssim.FDb._db.Main
// this function is 'extrn' and implemented by user
void                 Main();
// Parse strptr into known type and add to database.
// Return value is true unless an error occurs. If return value is false, algo_lib::_db.errtext has error text
// func:mysql2ssim.FDb._db.InsertStrptrMaybe
bool                 InsertStrptrMaybe(algo::strptr str);
// Load all finputs from given directory.
// func:mysql2ssim.FDb._db.LoadTuplesMaybe
bool                 LoadTuplesMaybe(algo::strptr root, bool recursive) __attribute__((nothrow));
// Load all finputs from given file.
// Read tuples from file FNAME into this namespace's in-memory database.
// If RECURSIVE is TRUE, then also load these tuples into any parent namespaces
// It a file referred to by FNAME is missing, no error is reported (it's considered an empty set).
// Function returns TRUE if all records were parsed and inserted without error.
// If the function returns FALSE, use algo_lib::DetachBadTags() for error description
// func:mysql2ssim.FDb._db.LoadTuplesFile
bool                 LoadTuplesFile(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load all finputs from given file descriptor.
// func:mysql2ssim.FDb._db.LoadTuplesFd
bool                 LoadTuplesFd(algo::Fildes fd, algo::strptr fname, bool recursive) __attribute__((nothrow));
// Load specified ssimfile.
// func:mysql2ssim.FDb._db.LoadSsimfileMaybe
bool                 LoadSsimfileMaybe(algo::strptr fname, bool recursive) __attribute__((nothrow));
// Calls Step function of dependencies
// func:mysql2ssim.FDb._db.Steps
void                 Steps();
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
// func:mysql2ssim.FDb._db.XrefMaybe
bool                 _db_XrefMaybe();

// Reserve space (this may move memory). Insert N element at the end.
// Return aryptr to newly inserted block.
// If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.
// func:mysql2ssim.FDb.table_names.Addary
algo::aryptr<algo::cstring> table_names_Addary(algo::aryptr<algo::cstring> rhs) __attribute__((nothrow));
// Reserve space. Insert element at the end
// The new element is initialized to a default value
// func:mysql2ssim.FDb.table_names.Alloc
algo::cstring&       table_names_Alloc() __attribute__((__warn_unused_result__, nothrow));
// Reserve space for new element, reallocating the array if necessary
// Insert new element at specified index. Index must be in range or a fatal error occurs.
// func:mysql2ssim.FDb.table_names.AllocAt
algo::cstring&       table_names_AllocAt(int at) __attribute__((__warn_unused_result__, nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:mysql2ssim.FDb.table_names.AllocN
algo::aryptr<algo::cstring> table_names_AllocN(int n_elems) __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:mysql2ssim.FDb.table_names.EmptyQ
inline bool          table_names_EmptyQ() __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:mysql2ssim.FDb.table_names.Find
inline algo::cstring* table_names_Find(u64 t) __attribute__((__warn_unused_result__, nothrow));
// Return array pointer by value
// func:mysql2ssim.FDb.table_names.Getary
inline algo::aryptr<algo::cstring> table_names_Getary() __attribute__((nothrow));
// Return pointer to last element of array, or NULL if array is empty
// func:mysql2ssim.FDb.table_names.Last
inline algo::cstring* table_names_Last() __attribute__((nothrow, pure));
// Return max. number of items in the array
// func:mysql2ssim.FDb.table_names.Max
inline i32           table_names_Max() __attribute__((nothrow));
// Return number of items in the array
// func:mysql2ssim.FDb.table_names.N
inline i32           table_names_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Remove item by index. If index outside of range, do nothing.
// func:mysql2ssim.FDb.table_names.Remove
void                 table_names_Remove(u32 i) __attribute__((nothrow));
// func:mysql2ssim.FDb.table_names.RemoveAll
void                 table_names_RemoveAll() __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:mysql2ssim.FDb.table_names.RemoveLast
void                 table_names_RemoveLast() __attribute__((nothrow));
// Make sure N *more* elements will fit in array. Process dies if out of memory
// func:mysql2ssim.FDb.table_names.Reserve
inline void          table_names_Reserve(int n) __attribute__((nothrow));
// Make sure N elements fit in array. Process dies if out of memory
// func:mysql2ssim.FDb.table_names.AbsReserve
void                 table_names_AbsReserve(int n) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:mysql2ssim.FDb.table_names.qFind
inline algo::cstring& table_names_qFind(u64 t) __attribute__((nothrow));
// Return reference to last element of array. No bounds checking
// func:mysql2ssim.FDb.table_names.qLast
inline algo::cstring& table_names_qLast() __attribute__((nothrow));
// Return row id of specified element
// func:mysql2ssim.FDb.table_names.rowid_Get
inline u64           table_names_rowid_Get(algo::cstring &elem) __attribute__((nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:mysql2ssim.FDb.table_names.AllocNVal
algo::aryptr<algo::cstring> table_names_AllocNVal(int n_elems, const algo::cstring& val) __attribute__((nothrow));
// A single element is read from input string and appended to the array.
// If the string contains an error, the array is untouched.
// Function returns success value.
// func:mysql2ssim.FDb.table_names.ReadStrptrMaybe
bool                 table_names_ReadStrptrMaybe(algo::strptr in_str) __attribute__((nothrow));

// Reserve space (this may move memory). Insert N element at the end.
// Return aryptr to newly inserted block.
// If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.
// func:mysql2ssim.FDb.in_tables.Addary
algo::aryptr<algo::cstring> in_tables_Addary(algo::aryptr<algo::cstring> rhs) __attribute__((nothrow));
// Reserve space. Insert element at the end
// The new element is initialized to a default value
// func:mysql2ssim.FDb.in_tables.Alloc
algo::cstring&       in_tables_Alloc() __attribute__((__warn_unused_result__, nothrow));
// Reserve space for new element, reallocating the array if necessary
// Insert new element at specified index. Index must be in range or a fatal error occurs.
// func:mysql2ssim.FDb.in_tables.AllocAt
algo::cstring&       in_tables_AllocAt(int at) __attribute__((__warn_unused_result__, nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:mysql2ssim.FDb.in_tables.AllocN
algo::aryptr<algo::cstring> in_tables_AllocN(int n_elems) __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:mysql2ssim.FDb.in_tables.EmptyQ
inline bool          in_tables_EmptyQ() __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:mysql2ssim.FDb.in_tables.Find
inline algo::cstring* in_tables_Find(u64 t) __attribute__((__warn_unused_result__, nothrow));
// Return array pointer by value
// func:mysql2ssim.FDb.in_tables.Getary
inline algo::aryptr<algo::cstring> in_tables_Getary() __attribute__((nothrow));
// Return pointer to last element of array, or NULL if array is empty
// func:mysql2ssim.FDb.in_tables.Last
inline algo::cstring* in_tables_Last() __attribute__((nothrow, pure));
// Return max. number of items in the array
// func:mysql2ssim.FDb.in_tables.Max
inline i32           in_tables_Max() __attribute__((nothrow));
// Return number of items in the array
// func:mysql2ssim.FDb.in_tables.N
inline i32           in_tables_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Remove item by index. If index outside of range, do nothing.
// func:mysql2ssim.FDb.in_tables.Remove
void                 in_tables_Remove(u32 i) __attribute__((nothrow));
// func:mysql2ssim.FDb.in_tables.RemoveAll
void                 in_tables_RemoveAll() __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:mysql2ssim.FDb.in_tables.RemoveLast
void                 in_tables_RemoveLast() __attribute__((nothrow));
// Make sure N *more* elements will fit in array. Process dies if out of memory
// func:mysql2ssim.FDb.in_tables.Reserve
inline void          in_tables_Reserve(int n) __attribute__((nothrow));
// Make sure N elements fit in array. Process dies if out of memory
// func:mysql2ssim.FDb.in_tables.AbsReserve
void                 in_tables_AbsReserve(int n) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:mysql2ssim.FDb.in_tables.qFind
inline algo::cstring& in_tables_qFind(u64 t) __attribute__((nothrow));
// Return reference to last element of array. No bounds checking
// func:mysql2ssim.FDb.in_tables.qLast
inline algo::cstring& in_tables_qLast() __attribute__((nothrow));
// Return row id of specified element
// func:mysql2ssim.FDb.in_tables.rowid_Get
inline u64           in_tables_rowid_Get(algo::cstring &elem) __attribute__((nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:mysql2ssim.FDb.in_tables.AllocNVal
algo::aryptr<algo::cstring> in_tables_AllocNVal(int n_elems, const algo::cstring& val) __attribute__((nothrow));
// A single element is read from input string and appended to the array.
// If the string contains an error, the array is untouched.
// Function returns success value.
// func:mysql2ssim.FDb.in_tables.ReadStrptrMaybe
bool                 in_tables_ReadStrptrMaybe(algo::strptr in_str) __attribute__((nothrow));

// proceed to next item
// func:mysql2ssim.FDb.table_names_curs.Next
inline void          _db_table_names_curs_Next(_db_table_names_curs &curs) __attribute__((nothrow));
// func:mysql2ssim.FDb.table_names_curs.Reset
inline void          _db_table_names_curs_Reset(_db_table_names_curs &curs, mysql2ssim::FDb &parent) __attribute__((nothrow));
// cursor points to valid item
// func:mysql2ssim.FDb.table_names_curs.ValidQ
inline bool          _db_table_names_curs_ValidQ(_db_table_names_curs &curs) __attribute__((nothrow));
// item access
// func:mysql2ssim.FDb.table_names_curs.Access
inline algo::cstring& _db_table_names_curs_Access(_db_table_names_curs &curs) __attribute__((nothrow));
// proceed to next item
// func:mysql2ssim.FDb.in_tables_curs.Next
inline void          _db_in_tables_curs_Next(_db_in_tables_curs &curs) __attribute__((nothrow));
// func:mysql2ssim.FDb.in_tables_curs.Reset
inline void          _db_in_tables_curs_Reset(_db_in_tables_curs &curs, mysql2ssim::FDb &parent) __attribute__((nothrow));
// cursor points to valid item
// func:mysql2ssim.FDb.in_tables_curs.ValidQ
inline bool          _db_in_tables_curs_ValidQ(_db_in_tables_curs &curs) __attribute__((nothrow));
// item access
// func:mysql2ssim.FDb.in_tables_curs.Access
inline algo::cstring& _db_in_tables_curs_Access(_db_in_tables_curs &curs) __attribute__((nothrow));
// Set all fields to initial values.
// func:mysql2ssim.FDb..Init
void                 FDb_Init();
// func:mysql2ssim.FDb..Uninit
void                 FDb_Uninit() __attribute__((nothrow));

// --- mysql2ssim.FTobltin
struct FTobltin { // mysql2ssim.FTobltin
    bool             warn;         //   false  Conversion warning
    bool             err;          //   false  Conversion error
    algo::cstring*   vals_elems;   // pointer to elements
    u32              vals_n;       // number of elements in array
    u32              vals_max;     // max. capacity of array before realloc
    // func:mysql2ssim.FTobltin..AssignOp
    mysql2ssim::FTobltin& operator =(const mysql2ssim::FTobltin &rhs) __attribute__((nothrow));
    // func:mysql2ssim.FTobltin..Ctor
    inline               FTobltin() __attribute__((nothrow));
    // func:mysql2ssim.FTobltin..Dtor
    inline               ~FTobltin() __attribute__((nothrow));
    // func:mysql2ssim.FTobltin..CopyCtor
    FTobltin(const mysql2ssim::FTobltin &rhs) __attribute__((nothrow));
};

// Reserve space (this may move memory). Insert N element at the end.
// Return aryptr to newly inserted block.
// If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.
// func:mysql2ssim.FTobltin.vals.Addary
algo::aryptr<algo::cstring> vals_Addary(mysql2ssim::FTobltin& parent, algo::aryptr<algo::cstring> rhs) __attribute__((nothrow));
// Reserve space. Insert element at the end
// The new element is initialized to a default value
// func:mysql2ssim.FTobltin.vals.Alloc
algo::cstring&       vals_Alloc(mysql2ssim::FTobltin& parent) __attribute__((__warn_unused_result__, nothrow));
// Reserve space for new element, reallocating the array if necessary
// Insert new element at specified index. Index must be in range or a fatal error occurs.
// func:mysql2ssim.FTobltin.vals.AllocAt
algo::cstring&       vals_AllocAt(mysql2ssim::FTobltin& parent, int at) __attribute__((__warn_unused_result__, nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:mysql2ssim.FTobltin.vals.AllocN
algo::aryptr<algo::cstring> vals_AllocN(mysql2ssim::FTobltin& parent, int n_elems) __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
// func:mysql2ssim.FTobltin.vals.EmptyQ
inline bool          vals_EmptyQ(mysql2ssim::FTobltin& parent) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
// func:mysql2ssim.FTobltin.vals.Find
inline algo::cstring* vals_Find(mysql2ssim::FTobltin& parent, u64 t) __attribute__((__warn_unused_result__, nothrow));
// Return array pointer by value
// func:mysql2ssim.FTobltin.vals.Getary
inline algo::aryptr<algo::cstring> vals_Getary(const mysql2ssim::FTobltin& parent) __attribute__((nothrow));
// Return pointer to last element of array, or NULL if array is empty
// func:mysql2ssim.FTobltin.vals.Last
inline algo::cstring* vals_Last(mysql2ssim::FTobltin& parent) __attribute__((nothrow, pure));
// Return max. number of items in the array
// func:mysql2ssim.FTobltin.vals.Max
inline i32           vals_Max(mysql2ssim::FTobltin& parent) __attribute__((nothrow));
// Return number of items in the array
// func:mysql2ssim.FTobltin.vals.N
inline i32           vals_N(const mysql2ssim::FTobltin& parent) __attribute__((__warn_unused_result__, nothrow, pure));
// Remove item by index. If index outside of range, do nothing.
// func:mysql2ssim.FTobltin.vals.Remove
void                 vals_Remove(mysql2ssim::FTobltin& parent, u32 i) __attribute__((nothrow));
// func:mysql2ssim.FTobltin.vals.RemoveAll
void                 vals_RemoveAll(mysql2ssim::FTobltin& parent) __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
// func:mysql2ssim.FTobltin.vals.RemoveLast
void                 vals_RemoveLast(mysql2ssim::FTobltin& parent) __attribute__((nothrow));
// Make sure N *more* elements will fit in array. Process dies if out of memory
// func:mysql2ssim.FTobltin.vals.Reserve
inline void          vals_Reserve(mysql2ssim::FTobltin& parent, int n) __attribute__((nothrow));
// Make sure N elements fit in array. Process dies if out of memory
// func:mysql2ssim.FTobltin.vals.AbsReserve
void                 vals_AbsReserve(mysql2ssim::FTobltin& parent, int n) __attribute__((nothrow));
// Copy contents of RHS to PARENT.
// func:mysql2ssim.FTobltin.vals.Setary
void                 vals_Setary(mysql2ssim::FTobltin& parent, mysql2ssim::FTobltin &rhs) __attribute__((nothrow));
// Copy specified array into vals, discarding previous contents.
// If the RHS argument aliases the array (refers to the same memory), throw exception.
// func:mysql2ssim.FTobltin.vals.Setary2
void                 vals_Setary(mysql2ssim::FTobltin& parent, const algo::aryptr<algo::cstring> &rhs) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking.
// func:mysql2ssim.FTobltin.vals.qFind
inline algo::cstring& vals_qFind(mysql2ssim::FTobltin& parent, u64 t) __attribute__((nothrow));
// Return reference to last element of array. No bounds checking
// func:mysql2ssim.FTobltin.vals.qLast
inline algo::cstring& vals_qLast(mysql2ssim::FTobltin& parent) __attribute__((nothrow));
// Return row id of specified element
// func:mysql2ssim.FTobltin.vals.rowid_Get
inline u64           vals_rowid_Get(mysql2ssim::FTobltin& parent, algo::cstring &elem) __attribute__((nothrow));
// Reserve space. Insert N elements at the end of the array, return pointer to array
// func:mysql2ssim.FTobltin.vals.AllocNVal
algo::aryptr<algo::cstring> vals_AllocNVal(mysql2ssim::FTobltin& parent, int n_elems, const algo::cstring& val) __attribute__((nothrow));
// A single element is read from input string and appended to the array.
// If the string contains an error, the array is untouched.
// Function returns success value.
// func:mysql2ssim.FTobltin.vals.ReadStrptrMaybe
bool                 vals_ReadStrptrMaybe(mysql2ssim::FTobltin& parent, algo::strptr in_str) __attribute__((nothrow));

// proceed to next item
// func:mysql2ssim.FTobltin.vals_curs.Next
inline void          FTobltin_vals_curs_Next(FTobltin_vals_curs &curs) __attribute__((nothrow));
// func:mysql2ssim.FTobltin.vals_curs.Reset
inline void          FTobltin_vals_curs_Reset(FTobltin_vals_curs &curs, mysql2ssim::FTobltin &parent) __attribute__((nothrow));
// cursor points to valid item
// func:mysql2ssim.FTobltin.vals_curs.ValidQ
inline bool          FTobltin_vals_curs_ValidQ(FTobltin_vals_curs &curs) __attribute__((nothrow));
// item access
// func:mysql2ssim.FTobltin.vals_curs.Access
inline algo::cstring& FTobltin_vals_curs_Access(FTobltin_vals_curs &curs) __attribute__((nothrow));
// Set all fields to initial values.
// func:mysql2ssim.FTobltin..Init
inline void          FTobltin_Init(mysql2ssim::FTobltin& parent);
// func:mysql2ssim.FTobltin..Uninit
void                 FTobltin_Uninit(mysql2ssim::FTobltin& parent) __attribute__((nothrow));

// --- mysql2ssim.FieldId
#pragma pack(push,1)
struct FieldId { // mysql2ssim.FieldId: Field read helper
    i32   value;   //   -1
    // func:mysql2ssim.FieldId.value.Cast
    inline               operator mysql2ssim_FieldIdEnum() const __attribute__((nothrow));
    // func:mysql2ssim.FieldId..Ctor
    inline               FieldId() __attribute__((nothrow));
    // func:mysql2ssim.FieldId..FieldwiseCtor
    explicit inline               FieldId(i32 in_value) __attribute__((nothrow));
    // func:mysql2ssim.FieldId..EnumCtor
    inline               FieldId(mysql2ssim_FieldIdEnum arg) __attribute__((nothrow));
};
#pragma pack(pop)

// Get value of field as enum type
// func:mysql2ssim.FieldId.value.GetEnum
inline mysql2ssim_FieldIdEnum value_GetEnum(const mysql2ssim::FieldId& parent) __attribute__((nothrow));
// Set value of field from enum type.
// func:mysql2ssim.FieldId.value.SetEnum
inline void          value_SetEnum(mysql2ssim::FieldId& parent, mysql2ssim_FieldIdEnum rhs) __attribute__((nothrow));
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
// func:mysql2ssim.FieldId.value.ToCstr
const char*          value_ToCstr(const mysql2ssim::FieldId& parent) __attribute__((nothrow));
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
// func:mysql2ssim.FieldId.value.Print
void                 value_Print(const mysql2ssim::FieldId& parent, algo::cstring &lhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
// func:mysql2ssim.FieldId.value.SetStrptrMaybe
bool                 value_SetStrptrMaybe(mysql2ssim::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
// func:mysql2ssim.FieldId.value.SetStrptr
void                 value_SetStrptr(mysql2ssim::FieldId& parent, algo::strptr rhs, mysql2ssim_FieldIdEnum dflt) __attribute__((nothrow));
// Convert string to field. Return success value
// func:mysql2ssim.FieldId.value.ReadStrptrMaybe
bool                 value_ReadStrptrMaybe(mysql2ssim::FieldId& parent, algo::strptr rhs) __attribute__((nothrow));

// Read fields of mysql2ssim::FieldId from an ascii string.
// The format of the string is the format of the mysql2ssim::FieldId's only field
// func:mysql2ssim.FieldId..ReadStrptrMaybe
bool                 FieldId_ReadStrptrMaybe(mysql2ssim::FieldId &parent, algo::strptr in_str) __attribute__((nothrow));
// Set all fields to initial values.
// func:mysql2ssim.FieldId..Init
inline void          FieldId_Init(mysql2ssim::FieldId& parent);
// print string representation of ROW to string STR
// cfmt:mysql2ssim.FieldId.String  printfmt:Raw
// func:mysql2ssim.FieldId..Print
void                 FieldId_Print(mysql2ssim::FieldId& row, algo::cstring& str) __attribute__((nothrow));
} // gen:ns_print_struct
namespace mysql2ssim { // gen:ns_curstext

struct _db_table_names_curs {// cursor
    typedef algo::cstring ChildType;
    algo::cstring* elems;
    int n_elems;
    int index;
    _db_table_names_curs() { elems=NULL; n_elems=0; index=0; }
};


struct _db_in_tables_curs {// cursor
    typedef algo::cstring ChildType;
    algo::cstring* elems;
    int n_elems;
    int index;
    _db_in_tables_curs() { elems=NULL; n_elems=0; index=0; }
};


struct FTobltin_vals_curs {// cursor
    typedef algo::cstring ChildType;
    algo::cstring* elems;
    int n_elems;
    int index;
    FTobltin_vals_curs() { elems=NULL; n_elems=0; index=0; }
};

} // gen:ns_curstext
namespace mysql2ssim { // gen:ns_func
// func:mysql2ssim...StaticCheck
void                 StaticCheck();
} // gen:ns_func
// func:mysql2ssim...main
int                  main(int argc, char **argv);
#if defined(WIN32)
// func:mysql2ssim...WinMain
int WINAPI           WinMain(HINSTANCE,HINSTANCE,LPSTR,int);
#endif
// gen:ns_operators
namespace algo {
inline algo::cstring &operator <<(algo::cstring &str, const mysql2ssim::trace &row);// cfmt:mysql2ssim.trace.String
inline algo::cstring &operator <<(algo::cstring &str, const mysql2ssim::FieldId &row);// cfmt:mysql2ssim.FieldId.String
}
