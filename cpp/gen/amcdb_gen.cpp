//
// cpp/gen/amcdb_gen.cpp
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


#include "include/algo.h"  // hard-coded include
#include "include/gen/amcdb_gen.h"
#include "include/gen/amcdb_gen.inl.h"
#include "include/gen/algo_gen.h"
#include "include/gen/algo_gen.inl.h"
//#pragma endinclude
// compile-time string constants for amcdb.Curstype.curstype
const char *amcdb_Curstype_curstype_bitcurs     = "bitcurs";
const char *amcdb_Curstype_curstype_curs        = "curs";
const char *amcdb_Curstype_curstype_delcurs     = "delcurs";
const char *amcdb_Curstype_curstype_oncecurs    = "oncecurs";
const char *amcdb_Curstype_curstype_unordcurs   = "unordcurs";

// compile-time string constants for amcdb.Tclass.tclass
const char *amcdb_Tclass_tclass_Alias     = "Alias";
const char *amcdb_Tclass_tclass_Atree     = "Atree";
const char *amcdb_Tclass_tclass_Base      = "Base";
const char *amcdb_Tclass_tclass_Bheap     = "Bheap";
const char *amcdb_Tclass_tclass_Bitfld    = "Bitfld";
const char *amcdb_Tclass_tclass_Bitset    = "Bitset";
const char *amcdb_Tclass_tclass_Blkpool   = "Blkpool";
const char *amcdb_Tclass_tclass_Charset   = "Charset";
const char *amcdb_Tclass_tclass_Cmp       = "Cmp";

const char *amcdb_Tclass_tclass_Count      = "Count";
const char *amcdb_Tclass_tclass_Cppfunc    = "Cppfunc";
const char *amcdb_Tclass_tclass_Cppstack   = "Cppstack";
const char *amcdb_Tclass_tclass_Ctype      = "Ctype";
const char *amcdb_Tclass_tclass_Ctype2     = "Ctype2";

const char *amcdb_Tclass_tclass_Dec   = "Dec";

const char *amcdb_Tclass_tclass_Delptr   = "Delptr";
const char *amcdb_Tclass_tclass_Exec     = "Exec";
const char *amcdb_Tclass_tclass_Fbuf     = "Fbuf";
const char *amcdb_Tclass_tclass_Fcast    = "Fcast";
const char *amcdb_Tclass_tclass_Fconst   = "Fconst";
const char *amcdb_Tclass_tclass_Field    = "Field";
const char *amcdb_Tclass_tclass_Field2   = "Field2";
const char *amcdb_Tclass_tclass_Global   = "Global";
const char *amcdb_Tclass_tclass_Hook     = "Hook";
const char *amcdb_Tclass_tclass_Inlary   = "Inlary";
const char *amcdb_Tclass_tclass_Io       = "Io";
const char *amcdb_Tclass_tclass_Lary     = "Lary";
const char *amcdb_Tclass_tclass_Llist    = "Llist";
const char *amcdb_Tclass_tclass_Lpool    = "Lpool";
const char *amcdb_Tclass_tclass_Malloc   = "Malloc";
const char *amcdb_Tclass_tclass_Ns       = "Ns";

const char *amcdb_Tclass_tclass_Numstr   = "Numstr";
const char *amcdb_Tclass_tclass_Opt      = "Opt";
const char *amcdb_Tclass_tclass_Pkey     = "Pkey";
const char *amcdb_Tclass_tclass_Pmask    = "Pmask";
const char *amcdb_Tclass_tclass_Pool     = "Pool";
const char *amcdb_Tclass_tclass_Ptr      = "Ptr";
const char *amcdb_Tclass_tclass_Ptrary   = "Ptrary";

const char *amcdb_Tclass_tclass_Regx      = "Regx";
const char *amcdb_Tclass_tclass_RegxSql   = "RegxSql";

const char *amcdb_Tclass_tclass_Sbrk       = "Sbrk";
const char *amcdb_Tclass_tclass_Smallstr   = "Smallstr";
const char *amcdb_Tclass_tclass_Sort       = "Sort";
const char *amcdb_Tclass_tclass_Step       = "Step";
const char *amcdb_Tclass_tclass_Substr     = "Substr";
const char *amcdb_Tclass_tclass_Tary       = "Tary";
const char *amcdb_Tclass_tclass_Thash      = "Thash";
const char *amcdb_Tclass_tclass_Tpool      = "Tpool";
const char *amcdb_Tclass_tclass_Upptr      = "Upptr";
const char *amcdb_Tclass_tclass_Val        = "Val";
const char *amcdb_Tclass_tclass_Varlen     = "Varlen";
const char *amcdb_Tclass_tclass_ZSListMT   = "ZSListMT";

namespace amcdb { // gen:ns_print_proto
    // func:amcdb...SizeCheck
    inline static void   SizeCheck();
} // gen:ns_print_proto

// --- amcdb.Bltin..ReadFieldMaybe
bool amcdb::Bltin_ReadFieldMaybe(amcdb::Bltin& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_ctype: {
            retval = algo::Smallstr100_ReadStrptrMaybe(parent.ctype, strval);
            break;
        }
        case amcdb_FieldId_likeu64: {
            retval = bool_ReadStrptrMaybe(parent.likeu64, strval);
            break;
        }
        case amcdb_FieldId_bigendok: {
            retval = bool_ReadStrptrMaybe(parent.bigendok, strval);
            break;
        }
        case amcdb_FieldId_issigned: {
            retval = bool_ReadStrptrMaybe(parent.issigned, strval);
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Bltin..ReadStrptrMaybe
// Read fields of amcdb::Bltin from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Bltin_ReadStrptrMaybe(amcdb::Bltin &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.bltin") || algo::StripTypeTag(in_str, "amcdb.Bltin");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Bltin_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Bltin..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Bltin.String  printfmt:Tuple
void amcdb::Bltin_Print(amcdb::Bltin& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.bltin";

    algo::Smallstr100_Print(row.ctype, temp);
    PrintAttrSpaceReset(str,"ctype", temp);

    bool_Print(row.likeu64, temp);
    PrintAttrSpaceReset(str,"likeu64", temp);

    bool_Print(row.bigendok, temp);
    PrintAttrSpaceReset(str,"bigendok", temp);

    bool_Print(row.issigned, temp);
    PrintAttrSpaceReset(str,"issigned", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb.Curstype..ReadFieldMaybe
bool amcdb::Curstype_ReadFieldMaybe(amcdb::Curstype& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_curstype: {
            retval = algo::Smallstr50_ReadStrptrMaybe(parent.curstype, strval);
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Curstype..ReadStrptrMaybe
// Read fields of amcdb::Curstype from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Curstype_ReadStrptrMaybe(amcdb::Curstype &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.curstype") || algo::StripTypeTag(in_str, "amcdb.Curstype");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Curstype_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Curstype..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Curstype.String  printfmt:Tuple
void amcdb::Curstype_Print(amcdb::Curstype& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.curstype";

    algo::Smallstr50_Print(row.curstype, temp);
    PrintAttrSpaceReset(str,"curstype", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb.FieldId.value.ToCstr
// Convert numeric value of field to one of predefined string constants.
// If string is found, return a static C string. Otherwise, return NULL.
const char* amcdb::value_ToCstr(const amcdb::FieldId& parent) {
    const char *ret = NULL;
    switch(value_GetEnum(parent)) {
        case amcdb_FieldId_ctype           : ret = "ctype";  break;
        case amcdb_FieldId_likeu64         : ret = "likeu64";  break;
        case amcdb_FieldId_bigendok        : ret = "bigendok";  break;
        case amcdb_FieldId_issigned        : ret = "issigned";  break;
        case amcdb_FieldId_comment         : ret = "comment";  break;
        case amcdb_FieldId_curstype        : ret = "curstype";  break;
        case amcdb_FieldId_gen             : ret = "gen";  break;
        case amcdb_FieldId_perns           : ret = "perns";  break;
        case amcdb_FieldId_regxtype        : ret = "regxtype";  break;
        case amcdb_FieldId_tclass          : ret = "tclass";  break;
        case amcdb_FieldId_tfunc           : ret = "tfunc";  break;
        case amcdb_FieldId_dflt            : ret = "dflt";  break;
        case amcdb_FieldId_name            : ret = "name";  break;
        case amcdb_FieldId_hasthrow        : ret = "hasthrow";  break;
        case amcdb_FieldId_leaf            : ret = "leaf";  break;
        case amcdb_FieldId_poolfunc        : ret = "poolfunc";  break;
        case amcdb_FieldId_inl             : ret = "inl";  break;
        case amcdb_FieldId_wur             : ret = "wur";  break;
        case amcdb_FieldId_pure            : ret = "pure";  break;
        case amcdb_FieldId_ismacro         : ret = "ismacro";  break;
        case amcdb_FieldId_value           : ret = "value";  break;
    }
    return ret;
}

// --- amcdb.FieldId.value.Print
// Convert value to a string. First, attempt conversion to a known string.
// If no string matches, print value as a numeric value.
void amcdb::value_Print(const amcdb::FieldId& parent, algo::cstring &lhs) {
    const char *strval = value_ToCstr(parent);
    if (strval) {
        lhs << strval;
    } else {
        lhs << parent.value;
    }
}

// --- amcdb.FieldId.value.SetStrptrMaybe
// Convert string to field.
// If the string is invalid, do not modify field and return false.
// In case of success, return true
bool amcdb::value_SetStrptrMaybe(amcdb::FieldId& parent, algo::strptr rhs) {
    bool ret = false;
    switch (elems_N(rhs)) {
        case 3: {
            switch (u64(algo::ReadLE16(rhs.elems))|(u64(rhs[2])<<16)) {
                case LE_STR3('g','e','n'): {
                    value_SetEnum(parent,amcdb_FieldId_gen); ret = true; break;
                }
                case LE_STR3('i','n','l'): {
                    value_SetEnum(parent,amcdb_FieldId_inl); ret = true; break;
                }
                case LE_STR3('w','u','r'): {
                    value_SetEnum(parent,amcdb_FieldId_wur); ret = true; break;
                }
            }
            break;
        }
        case 4: {
            switch (u64(algo::ReadLE32(rhs.elems))) {
                case LE_STR4('d','f','l','t'): {
                    value_SetEnum(parent,amcdb_FieldId_dflt); ret = true; break;
                }
                case LE_STR4('l','e','a','f'): {
                    value_SetEnum(parent,amcdb_FieldId_leaf); ret = true; break;
                }
                case LE_STR4('n','a','m','e'): {
                    value_SetEnum(parent,amcdb_FieldId_name); ret = true; break;
                }
                case LE_STR4('p','u','r','e'): {
                    value_SetEnum(parent,amcdb_FieldId_pure); ret = true; break;
                }
            }
            break;
        }
        case 5: {
            switch (u64(algo::ReadLE32(rhs.elems))|(u64(rhs[4])<<32)) {
                case LE_STR5('c','t','y','p','e'): {
                    value_SetEnum(parent,amcdb_FieldId_ctype); ret = true; break;
                }
                case LE_STR5('p','e','r','n','s'): {
                    value_SetEnum(parent,amcdb_FieldId_perns); ret = true; break;
                }
                case LE_STR5('t','f','u','n','c'): {
                    value_SetEnum(parent,amcdb_FieldId_tfunc); ret = true; break;
                }
                case LE_STR5('v','a','l','u','e'): {
                    value_SetEnum(parent,amcdb_FieldId_value); ret = true; break;
                }
            }
            break;
        }
        case 6: {
            switch (u64(algo::ReadLE32(rhs.elems))|(u64(algo::ReadLE16(rhs.elems+4))<<32)) {
                case LE_STR6('t','c','l','a','s','s'): {
                    value_SetEnum(parent,amcdb_FieldId_tclass); ret = true; break;
                }
            }
            break;
        }
        case 7: {
            switch (u64(algo::ReadLE32(rhs.elems))|(u64(algo::ReadLE16(rhs.elems+4))<<32)|(u64(rhs[6])<<48)) {
                case LE_STR7('c','o','m','m','e','n','t'): {
                    value_SetEnum(parent,amcdb_FieldId_comment); ret = true; break;
                }
                case LE_STR7('i','s','m','a','c','r','o'): {
                    value_SetEnum(parent,amcdb_FieldId_ismacro); ret = true; break;
                }
                case LE_STR7('l','i','k','e','u','6','4'): {
                    value_SetEnum(parent,amcdb_FieldId_likeu64); ret = true; break;
                }
            }
            break;
        }
        case 8: {
            switch (algo::ReadLE64(rhs.elems)) {
                case LE_STR8('b','i','g','e','n','d','o','k'): {
                    value_SetEnum(parent,amcdb_FieldId_bigendok); ret = true; break;
                }
                case LE_STR8('c','u','r','s','t','y','p','e'): {
                    value_SetEnum(parent,amcdb_FieldId_curstype); ret = true; break;
                }
                case LE_STR8('h','a','s','t','h','r','o','w'): {
                    value_SetEnum(parent,amcdb_FieldId_hasthrow); ret = true; break;
                }
                case LE_STR8('i','s','s','i','g','n','e','d'): {
                    value_SetEnum(parent,amcdb_FieldId_issigned); ret = true; break;
                }
                case LE_STR8('p','o','o','l','f','u','n','c'): {
                    value_SetEnum(parent,amcdb_FieldId_poolfunc); ret = true; break;
                }
                case LE_STR8('r','e','g','x','t','y','p','e'): {
                    value_SetEnum(parent,amcdb_FieldId_regxtype); ret = true; break;
                }
            }
            break;
        }
    }
    return ret;
}

// --- amcdb.FieldId.value.SetStrptr
// Convert string to field.
// If the string is invalid, set numeric value to DFLT
void amcdb::value_SetStrptr(amcdb::FieldId& parent, algo::strptr rhs, amcdb_FieldIdEnum dflt) {
    if (!value_SetStrptrMaybe(parent,rhs)) value_SetEnum(parent,dflt);
}

// --- amcdb.FieldId.value.ReadStrptrMaybe
// Convert string to field. Return success value
bool amcdb::value_ReadStrptrMaybe(amcdb::FieldId& parent, algo::strptr rhs) {
    bool retval = false;
    retval = value_SetStrptrMaybe(parent,rhs); // try symbol conversion
    if (!retval) { // didn't work? try reading as underlying type
        retval = i32_ReadStrptrMaybe(parent.value,rhs);
    }
    return retval;
}

// --- amcdb.FieldId..ReadStrptrMaybe
// Read fields of amcdb::FieldId from an ascii string.
// The format of the string is the format of the amcdb::FieldId's only field
bool amcdb::FieldId_ReadStrptrMaybe(amcdb::FieldId &parent, algo::strptr in_str) {
    bool retval = true;
    retval = retval && value_ReadStrptrMaybe(parent, in_str);
    return retval;
}

// --- amcdb.FieldId..Print
// print string representation of ROW to string STR
// cfmt:amcdb.FieldId.String  printfmt:Raw
void amcdb::FieldId_Print(amcdb::FieldId& row, algo::cstring& str) {
    amcdb::value_Print(row, str);
}

// --- amcdb.Gen..ReadFieldMaybe
bool amcdb::Gen_ReadFieldMaybe(amcdb::Gen& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_gen: {
            retval = algo::Smallstr50_ReadStrptrMaybe(parent.gen, strval);
            break;
        }
        case amcdb_FieldId_perns: {
            retval = bool_ReadStrptrMaybe(parent.perns, strval);
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Gen..ReadStrptrMaybe
// Read fields of amcdb::Gen from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Gen_ReadStrptrMaybe(amcdb::Gen &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.gen") || algo::StripTypeTag(in_str, "amcdb.Gen");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Gen_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Gen..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Gen.String  printfmt:Tuple
void amcdb::Gen_Print(amcdb::Gen& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.gen";

    algo::Smallstr50_Print(row.gen, temp);
    PrintAttrSpaceReset(str,"gen", temp);

    bool_Print(row.perns, temp);
    PrintAttrSpaceReset(str,"perns", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb.Regxtype..ReadFieldMaybe
bool amcdb::Regxtype_ReadFieldMaybe(amcdb::Regxtype& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_regxtype: {
            retval = algo::Smallstr50_ReadStrptrMaybe(parent.regxtype, strval);
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Regxtype..ReadStrptrMaybe
// Read fields of amcdb::Regxtype from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Regxtype_ReadStrptrMaybe(amcdb::Regxtype &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.regxtype") || algo::StripTypeTag(in_str, "amcdb.Regxtype");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Regxtype_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Regxtype..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Regxtype.String  printfmt:Tuple
void amcdb::Regxtype_Print(amcdb::Regxtype& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.regxtype";

    algo::Smallstr50_Print(row.regxtype, temp);
    PrintAttrSpaceReset(str,"regxtype", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb.Tclass..ReadFieldMaybe
bool amcdb::Tclass_ReadFieldMaybe(amcdb::Tclass& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_tclass: {
            retval = algo::Smallstr50_ReadStrptrMaybe(parent.tclass, strval);
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Tclass..ReadStrptrMaybe
// Read fields of amcdb::Tclass from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Tclass_ReadStrptrMaybe(amcdb::Tclass &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.tclass") || algo::StripTypeTag(in_str, "amcdb.Tclass");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Tclass_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Tclass..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Tclass.String  printfmt:Tuple
void amcdb::Tclass_Print(amcdb::Tclass& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.tclass";

    algo::Smallstr50_Print(row.tclass, temp);
    PrintAttrSpaceReset(str,"tclass", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb.Tcurs.curstype.Get
algo::Smallstr50 amcdb::curstype_Get(amcdb::Tcurs& parent) {
    algo::Smallstr50 ret(algo::Pathcomp(parent.tfunc, ".RR"));
    return ret;
}

// --- amcdb.Tcurs.curstype.Get2
algo::Smallstr50 amcdb::Tcurs_curstype_Get(algo::strptr arg) {
    algo::Smallstr50 ret(algo::Pathcomp(arg, ".RR"));
    return ret;
}

// --- amcdb.Tcurs..ReadFieldMaybe
bool amcdb::Tcurs_ReadFieldMaybe(amcdb::Tcurs& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_tfunc: {
            retval = algo::Smallstr50_ReadStrptrMaybe(parent.tfunc, strval);
            break;
        }
        case amcdb_FieldId_dflt: {
            retval = bool_ReadStrptrMaybe(parent.dflt, strval);
            break;
        }
        case amcdb_FieldId_curstype: {
            retval = false;
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Tcurs..ReadStrptrMaybe
// Read fields of amcdb::Tcurs from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Tcurs_ReadStrptrMaybe(amcdb::Tcurs &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.tcurs") || algo::StripTypeTag(in_str, "amcdb.Tcurs");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Tcurs_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Tcurs..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Tcurs.String  printfmt:Tuple
void amcdb::Tcurs_Print(amcdb::Tcurs& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.tcurs";

    algo::Smallstr50_Print(row.tfunc, temp);
    PrintAttrSpaceReset(str,"tfunc", temp);

    bool_Print(row.dflt, temp);
    PrintAttrSpaceReset(str,"dflt", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb.Tfunc.tclass.Get
algo::Smallstr50 amcdb::tclass_Get(amcdb::Tfunc& parent) {
    algo::Smallstr50 ret(algo::Pathcomp(parent.tfunc, ".RL"));
    return ret;
}

// --- amcdb.Tfunc.tclass.Get2
algo::Smallstr50 amcdb::Tfunc_tclass_Get(algo::strptr arg) {
    algo::Smallstr50 ret(algo::Pathcomp(arg, ".RL"));
    return ret;
}

// --- amcdb.Tfunc.name.Get
algo::Smallstr50 amcdb::name_Get(amcdb::Tfunc& parent) {
    algo::Smallstr50 ret(algo::Pathcomp(parent.tfunc, ".RR"));
    return ret;
}

// --- amcdb.Tfunc.name.Get2
algo::Smallstr50 amcdb::Tfunc_name_Get(algo::strptr arg) {
    algo::Smallstr50 ret(algo::Pathcomp(arg, ".RR"));
    return ret;
}

// --- amcdb.Tfunc..Concat_tclass_name
tempstr amcdb::Tfunc_Concat_tclass_name( const algo::strptr& tclass ,const algo::strptr& name ) {
    return tempstr() << tclass <<'.'<< name ;
}

// --- amcdb.Tfunc..ReadFieldMaybe
bool amcdb::Tfunc_ReadFieldMaybe(amcdb::Tfunc& parent, algo::strptr field, algo::strptr strval) {
    bool retval = true;
    amcdb::FieldId field_id;
    (void)value_SetStrptrMaybe(field_id,field);
    switch(field_id) {
        case amcdb_FieldId_tfunc: {
            retval = algo::Smallstr50_ReadStrptrMaybe(parent.tfunc, strval);
            break;
        }
        case amcdb_FieldId_tclass: {
            retval = false;
            break;
        }
        case amcdb_FieldId_name: {
            retval = false;
            break;
        }
        case amcdb_FieldId_hasthrow: {
            retval = bool_ReadStrptrMaybe(parent.hasthrow, strval);
            break;
        }
        case amcdb_FieldId_leaf: {
            retval = bool_ReadStrptrMaybe(parent.leaf, strval);
            break;
        }
        case amcdb_FieldId_poolfunc: {
            retval = bool_ReadStrptrMaybe(parent.poolfunc, strval);
            break;
        }
        case amcdb_FieldId_inl: {
            retval = bool_ReadStrptrMaybe(parent.inl, strval);
            break;
        }
        case amcdb_FieldId_wur: {
            retval = bool_ReadStrptrMaybe(parent.wur, strval);
            break;
        }
        case amcdb_FieldId_pure: {
            retval = bool_ReadStrptrMaybe(parent.pure, strval);
            break;
        }
        case amcdb_FieldId_ismacro: {
            retval = bool_ReadStrptrMaybe(parent.ismacro, strval);
            break;
        }
        case amcdb_FieldId_comment: {
            retval = algo::Comment_ReadStrptrMaybe(parent.comment, strval);
            break;
        }
        default: break;
    }
    if (!retval) {
        algo_lib::AppendErrtext("attr",field);
    }
    return retval;
}

// --- amcdb.Tfunc..ReadStrptrMaybe
// Read fields of amcdb::Tfunc from an ascii string.
// The format of the string is an ssim Tuple
bool amcdb::Tfunc_ReadStrptrMaybe(amcdb::Tfunc &parent, algo::strptr in_str) {
    bool retval = true;
    retval = algo::StripTypeTag(in_str, "amcdb.tfunc") || algo::StripTypeTag(in_str, "amcdb.Tfunc");
    ind_beg(algo::Attr_curs, attr, in_str) {
        retval = retval && Tfunc_ReadFieldMaybe(parent, attr.name, attr.value);
    }ind_end;
    return retval;
}

// --- amcdb.Tfunc..Init
// Set all fields to initial values.
void amcdb::Tfunc_Init(amcdb::Tfunc& parent) {
    parent.hasthrow = bool(false);
    parent.leaf = bool(true);
    parent.poolfunc = bool(false);
    parent.inl = bool(false);
    parent.wur = bool(false);
    parent.pure = bool(false);
    parent.ismacro = bool(false);
}

// --- amcdb.Tfunc..Print
// print string representation of ROW to string STR
// cfmt:amcdb.Tfunc.String  printfmt:Tuple
void amcdb::Tfunc_Print(amcdb::Tfunc& row, algo::cstring& str) {
    algo::tempstr temp;
    str << "amcdb.tfunc";

    algo::Smallstr50_Print(row.tfunc, temp);
    PrintAttrSpaceReset(str,"tfunc", temp);

    bool_Print(row.hasthrow, temp);
    PrintAttrSpaceReset(str,"hasthrow", temp);

    bool_Print(row.leaf, temp);
    PrintAttrSpaceReset(str,"leaf", temp);

    bool_Print(row.poolfunc, temp);
    PrintAttrSpaceReset(str,"poolfunc", temp);

    bool_Print(row.inl, temp);
    PrintAttrSpaceReset(str,"inl", temp);

    bool_Print(row.wur, temp);
    PrintAttrSpaceReset(str,"wur", temp);

    bool_Print(row.pure, temp);
    PrintAttrSpaceReset(str,"pure", temp);

    bool_Print(row.ismacro, temp);
    PrintAttrSpaceReset(str,"ismacro", temp);

    algo::Comment_Print(row.comment, temp);
    PrintAttrSpaceReset(str,"comment", temp);
}

// --- amcdb...SizeCheck
inline static void amcdb::SizeCheck() {
}
