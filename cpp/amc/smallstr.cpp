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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/smallstr.cpp -- Small strings
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool ValidRnumPadQ(strptr pad) {
    bool ret=true;
    if (ch_N(pad)>0 && pad.elems[0]!='\'') {// non-string OK
    } else {
        for (int i=0;i<ch_N(pad); i++) {
            if (ch_GetBit(amc::_db.ValidRnumPad,pad.elems[i])) {
                ret= false;
                break;
            }
        }
    }
    return ret;
}

static bool GoodBaseQ(amc::FSmallstr &smallstr) {
    bool good = smallstr.c_numstr->base == 10
        || ParseI32(Pathcomp(ctype_Get(*smallstr.p_field),"eRR"),0)==smallstr.c_numstr->base;
    return good;
}

// -----------------------------------------------------------------------------

static void CheckSmallstr(amc::FSmallstr &smallstr, amc::FNumstr *numstr) {
    // check string length -- must appear in the name
    // and be consistent with declared length
    // e.g. algo.Smallstr95
    // algo.LspaceStr7_I32_Base36
    if (smallstr.strict) {
        algo::Smallstr50 suffix(Pathcomp(name_Get(*smallstr.p_field->p_ctype),"_LLBLLrRR"));
        if (ParseI32(suffix,0) != smallstr.length) {
            algo_lib::_db.exit_code=1;
            prerr("amc.badsuffix"
                  <<Keyval("field",smallstr.field)
                  <<Keyval("suffix",suffix)
                  <<Keyval("length",smallstr.length)
                  <<Keyval("comment","Please use a suffix consistent with string length"));
        }
    }
    if (numstr && smallstr.strict) {
        strptr prefix =
            smallstr.strtype == dmmeta_Strtype_strtype_rightpad && smallstr.pad.value == "' '" ? "RspaceStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_leftpad && smallstr.pad.value == "' '" ? "LspaceStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_leftpad && smallstr.pad.value == "0" ? "LnullStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_rightpad && smallstr.pad.value == "0" ? "RnullStr"
            : "";
        if (!StartsWithQ(name_Get(*smallstr.p_field->p_ctype),prefix)) {
            algo_lib::_db.exit_code=1;
            prerr("amc.numstr_badprefix"
                  <<Keyval("field",smallstr.field)
                  <<Keyval("strtype",smallstr.strtype)
                  <<Keyval("requiredprefix",prefix)
                  <<Keyval("comment","Please use a consistent prefix"));
        }
    }
    // check that *no* digit character is used as a right-pad
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad && !ValidRnumPadQ(smallstr.pad.value)) {
        algo_lib::_db.exit_code=1;
        prerr("amc.invalidpad"
              <<Keyval("field",smallstr.field)
              <<Keyval("pad",smallstr.pad)
              <<Keyval("comment","right-pad character cannot be confused with a number"));
    }
    if (numstr && smallstr.strict && !GoodBaseQ(smallstr)) {
        algo_lib::_db.exit_code=1;
        prerr("amc.badbase"
              <<Keyval("field",smallstr.field)
              <<Keyval("base",numstr->base)
              <<Keyval("comment","mismatch between ctype name and numstr base"));
    }
    if (numstr && smallstr.pad.value=="'0'" && smallstr.strtype == dmmeta_Strtype_strtype_rightpad) {
        algo_lib::_db.exit_code=1;
        prerr("amc.rightpad_insanity"
              <<Keyval("name",smallstr.field)
              <<Keyval("comment","Numstr cannot be right-padded with digit '0'."));
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Smallstr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;

    vrfy(field.c_smallstr, "smallstr required");
    amc::FSmallstr& smallstr = *field.c_smallstr;

    CheckSmallstr(smallstr, smallstr.c_numstr);

    Set(R, "$max_length"   , tempstr() << smallstr.length);
    Set(R, "$pad"   , ch_N(smallstr.pad.value) ? strptr(smallstr.pad.value) : strptr("0"));
    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal && smallstr.length >= 256) {
        prerr("smallstr.toobig"
              <<Keyval("field",field.field)
              <<Keyval("length",smallstr.length)
              <<Keyval("comment","smallstr length too large (must be <= 255)"));
        algo_lib::_db.exit_code=1;
    }

    InsStruct(R, field.p_ctype, "enum { $name_max = $max_length };");

    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal && field.p_ctype->c_pack) {
        prerr("amc.pascalpack"
              <<Keyval("field",field.field)
              <<Keyval("comment","Rpascal string cannot be packed (it is not wire-safe)"));
        algo_lib::_db.exit_code=1;
    }

    // #AL# declare fields
    // make sure not to use char* because then it's tempting to use them where char*
    // is expected -- but none of these are zero-terminated by construction.
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad || smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        InsStruct(R, field.p_ctype, "u8 $name[$max_length];");
    } else {
        InsStruct(R, field.p_ctype, "u8 $name[$max_length+1];");
        InsStruct(R, field.p_ctype, "u8 n_$name;");// extra 1 byte is required by rpascal.
        InsStruct(R, field.p_ctype, "");
    }

    // copy ctor
    InsStruct(R, field.p_ctype, "$Parname(const $Partype &rhs) {");
    InsStruct(R, field.p_ctype, "operator =(rhs);");
    InsStruct(R, field.p_ctype, "}");

    // initialization from strptr
    InsStruct(R, field.p_ctype, "explicit $Parname(algo::strptr s) {");
    InsStruct(R, field.p_ctype, "operator =(s);");
    InsStruct(R, field.p_ctype, "}");

    // assign strptr
    InsStruct(R, field.p_ctype, "void operator =(const algo::strptr & str);");
    Ins(&R, *ns.inl, "inline void $Partype::operator =(const algo::strptr & str) { $name_Set(*this, str); }");

    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal) {
        InsStruct(R, field.p_ctype, "void operator =(const $Parname &s) {");
        InsStruct(R, field.p_ctype, "    n_$name = s.n_$name;");
        InsStruct(R, field.p_ctype, "    memcpy($name, s.$name, n_$name);");
        InsStruct(R, field.p_ctype, "}");
        InsStruct(R, field.p_ctype, "");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Smallstr_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    // init function
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    Ins(&R, init.ret  , "void", false);
    Ins(&R, init.proto, "$name_Init($Partype &$parname)", false);
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad || smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        // can't figure out how to squash coverity 'bad_memset' warning...
        // all padded strings are small anyway, so just use a loop.
        if (smallstr.pad.value == "'0'") {
            Ins(&R, init.body, "for (int i = 0; i < $max_length; i++) {");
            Ins(&R, init.body, "    $parname.$name[i] = $pad;");
            Ins(&R, init.body, "}");
        } else {
            Ins(&R, init.body, "memset($parname.$name, $pad, $max_length);");
        }
    } else {
        Ins(&R, init.body, "$parname.n_$name = 0;");
    }
}

void amc::tfunc_Smallstr_Getary() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    amc::FFunc& getary = amc::CreateCurFunc();
    Ins(&R, getary.comment, "Access string as array of chars");
    Ins(&R, getary.ret  , "algo::aryptr<char>", false);
    Ins(&R, getary.proto, "$name_Getary($Cparent)", false);
    Ins(&R, getary.body, "int len = $name_N($pararg);");
    Ins(&R, getary.body, "algo::aryptr<char> ret((char*)$parname.$name, len);");
    if (smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        Ins(&R, getary.body, "ret.elems += $max_length - len;");
    }
    Ins(&R, getary.body, "return ret;");
}

void amc::tfunc_Smallstr_Add() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal) {
        amc::FFunc& addchar = amc::CreateCurFunc();
        addchar.inl = true;
        Ins(&R, addchar.comment, "Append character to string.");
        Ins(&R, addchar.comment, "If there is no space for an extra character, do nothing.");
        Ins(&R, addchar.ret  , "void", false);
        Ins(&R, addchar.proto, "$name_Add($Parent, char c)", false);
        Ins(&R, addchar.body, "if ($parname.n_$name < $max_length) {");
        Ins(&R, addchar.body, "    $parname.$name[$parname.n_$name++] = c;");
        Ins(&R, addchar.body, "}");
    }
}

void amc::tfunc_Smallstr_AddStrptr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal) {
        amc::FFunc& addstrptr = amc::CreateCurFunc();
        addstrptr.inl = true;
        Ins(&R, addstrptr.comment, "Append string to this string.");
        Ins(&R, addstrptr.comment, "If there is no space for an extra character, trim.");
        Ins(&R, addstrptr.comment, "If there is no space left, dump core in debug mode.");
        Ins(&R, addstrptr.ret  , "void", false);
        Ins(&R, addstrptr.proto, "$name_AddStrptr($Parent, algo::strptr str)", false);
        Ins(&R, addstrptr.body, "int n_new = str.n_elems;");
        Ins(&R, addstrptr.body, "if ($parname.n_$name + n_new > $max_length) {");
        Ins(&R, addstrptr.body, "    n_new = $max_length - $parname.n_$name;");
        Ins(&R, addstrptr.body, "}");
        Ins(&R, addstrptr.body, "memcpy($parname.$name + $parname.n_$name, str.elems, n_new);");
        Ins(&R, addstrptr.body, "$parname.n_$name = u8($parname.n_$name + n_new);");
    }
}

void amc::tfunc_Smallstr_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (HasReadQ(*field.p_ctype)) {
        amc::FFunc& rd = amc::CreateCurFunc();
        Ins(&R, rd.comment, "Convert string to field. Return success value");
        Ins(&R, rd.ret  , "bool", false);
        Ins(&R, rd.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr rhs)", false);
        Ins(&R, rd.body, "bool retval = false;");
        Ins(&R, rd.body, "if (rhs.n_elems <= $max_length) {");
        Ins(&R, rd.body, "    $name_qSet($pararg, rhs);");
        Ins(&R, rd.body, "    retval = true;");
        Ins(&R, rd.body, "} else {");
        Ins(&R, rd.body, "    algo_lib::SaveBadTag(\"comment\",\"text too long, limit $max_length\");");
        Ins(&R, rd.body, "}");
        Ins(&R, rd.body, "return retval;");
    }
}

void amc::tfunc_Smallstr_Print() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    // print function.
    // strip any padding first).
    amc::FFunc& print = amc::CreateCurFunc();
    Ins(&R, print.ret  , "void", false);
    Ins(&R, print.proto, "$name_Print($Parent, algo::cstring &out)", false);
    Ins(&R, print.body, "ch_Addary(out, $name_Getary($pararg));");
}

void amc::tfunc_Smallstr_Hash() {
}

void amc::tfunc_Smallstr_HashStrptr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFunc& hash = amc::CreateCurFunc();
    // PARENT IS LIMITED TO HAVING 1 SMALLSTR FIELD
    vrfy(amc::c_datafld_N(*field.p_ctype)==1, "amc.smallstr_hash  comment:'Hash for smallstr is limited to one field'");

    // HASH FUNCTION
    // The hash function is calculated on the valid characters of the string.
    // I.e. if the string representation is changed (from leftpad to rightpad, etc)
    // the hash function doesn't change.
    // This guarantee is important, as string lengths often mismatch between keys (e.g.
    // string 'xyz' stored as RspaceStr10 vs RspaceStr20)
    hash.inl=true;
    Ins(&R, hash.ret  , "u32", false);
    Ins(&R, hash.proto, "$Parname_Hash(u32 prev, const algo::strptr &str)", false);
    Ins(&R, hash.body, "return algo::CRC32Step(prev, (u8*)str.elems, str.n_elems);");
}


void amc::tfunc_Smallstr_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    // compute length
    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "int", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "u64 ret;");
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad) {
        Ins(&R, nitems.body, "ret = $max_length;");
        Ins(&R, nitems.body, "while (ret>0 && $parname.$name[ret-1]==u8($pad)) {");
        Ins(&R, nitems.body, "    ret--;");
        Ins(&R, nitems.body, "}");
    } else if (smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        Ins(&R, nitems.body, "ret = 0;");
        Ins(&R, nitems.body, "while (ret<$max_length && $parname.$name[ret]==u8($pad)) {");
        Ins(&R, nitems.body, "    ret++;");
        Ins(&R, nitems.body, "}");
        Ins(&R, nitems.body, "ret = $max_length - ret;");
    } else {
        Ins(&R, nitems.body, "ret = $parname.n_$name;");
    }
    Ins(&R, nitems.body, "return int(ret);");
}


void amc::tfunc_Smallstr_Max() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    // Max # of elements (constant)
    amc::FFunc& maxitems = amc::CreateCurFunc();
    maxitems.inl=true;
    Ins(&R, maxitems.comment, "always return constant $max_length");
    Ins(&R, maxitems.ret  , "int", false);
    Ins(&R, maxitems.proto, "$name_Max($Parent)", false);
    Ins(&R, maxitems.body, "(void)$parname;");
    Ins(&R, maxitems.body, "return $max_length;");
}

void amc::tfunc_Smallstr_qSet() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    // Set from strptr, no checking
    amc::FFunc& qset = amc::CreateCurFunc();
    qset.priv = true;
    Ins(&R, qset.ret  , "void", false);
    Ins(&R, qset.comment, "Set string to the value provided by RHS.");
    Ins(&R, qset.comment, "No bounds checking is performed");
    Ins(&R, qset.proto, "$name_qSet($Parent, const algo::strptr &rhs)", false);
    Ins(&R, qset.body, "int len = rhs.n_elems;");
    Ins(&R, qset.body, "char *rhs_elems = rhs.elems;");
    Ins(&R, qset.body, "int i = 0;");
    Ins(&R, qset.body, "int j = 0;");
    if (smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        Ins(&R, qset.body, "for (; j < $max_length - len; j++) {");
        Ins(&R, qset.body, "    $parname.$name[j] = $pad;");
        Ins(&R, qset.body, "}");
    }
    Ins(&R, qset.body, "for (; i < len; i++, j++) {");
    Ins(&R, qset.body, "    $parname.$name[j] = rhs_elems[i];");
    Ins(&R, qset.body, "}");
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad) {
        Ins(&R, qset.body, "for (; j < $max_length; j++) {");
        Ins(&R, qset.body, "    $parname.$name[j] = $pad;");
        Ins(&R, qset.body, "}");
    }
    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal) {
        Ins(&R, qset.body, "$parname.n_$name       = u8(len);");
    }
}

void amc::tfunc_Smallstr_Set() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    // Set from strptr, clip
    amc::FFunc& set = amc::CreateCurFunc();
    Ins(&R, set.ret  , "void", false);
    Ins(&R, set.comment, "Set string to the value provided by RHS.");
    Ins(&R, set.comment, "If RHS is too large, it is silently clipped.");
    Ins(&R, set.proto, "$name_Set($Parent, const algo::strptr &rhs)", false);
    Ins(&R, set.body, "int len = i32_Min(rhs.n_elems, $max_length);");
    Ins(&R, set.body, "$name_qSet($parname, algo::strptr(rhs.elems, len));");
}
