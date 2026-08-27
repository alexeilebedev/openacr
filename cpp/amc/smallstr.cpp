// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
//
// License: GPL
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/smallstr.cpp -- Small strings
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// The strtype pad character of SMALLSTR: a quoted pad ("'x'") carries the
// character between the quotes; any other spelling is the NUL byte.
// The pad is substituted into the generated code as written, so it is a C++
// character literal and its escaped spellings name the same bytes the
// compiler will see: "'\0'" is the NUL byte and "'\x30'" is the digit zero,
// not the backslash that begins them. Reading the second character verbatim
// would judge every escaped pad against the backslash, and scanning the whole
// spelling would judge it against the digits of its own escape body -- so a
// NUL pad, no digit of any base and no part of any number, would be refused
// for the '0' that spells it.
char amc::PadChar(amc::FSmallstr &smallstr) {
    strptr padval = smallstr.pad.value;
    char retval = '\0';
    if (elems_N(padval) >= 3 && padval.elems[0] == '\'') {
        if (padval.elems[1] == '\\') {
            // UnescapeC reads the escape body -- the characters between the
            // backslash and the closing quote -- packed into a u32,
            // first character in the low byte
            int nch = i32_Min(4, elems_N(padval) - 3);
            u32 esc = 0;
            frep_(i,nch) {
                esc |= u32(u8(padval.elems[2+i])) << (8*i);
            }
            u8 ch = 0;
            if (algo::UnescapeC(esc,nch,ch)) {
                retval = char(ch);
            }
        } else {
            retval = padval.elems[1];
        }
    }
    return retval;
}

// True when the right-pad byte of SMALLSTR cannot be read as part of a
// number: a digit, a sign, a dot and the exponent character all can, and a
// field padded with one of them cannot be told from a field whose stored
// number ends in that character
static bool GoodRnumPadQ(amc::FSmallstr &smallstr) {
    return !ch_GetBit(amc::_db.ValidRnumPad,u8(amc::PadChar(smallstr)));
}

static bool GoodBaseQ(amc::FSmallstr &smallstr) {
    bool good = smallstr.c_numstr->base == 10
        || ParseI32(Pathcomp(ctype_Get(*smallstr.p_field),"eRR"),0)==smallstr.c_numstr->base;
    return good;
}

// -----------------------------------------------------------------------------

// The numeric-type token of ctype name NAME: the first '_'-separated
// component of the form I<digits> or U<digits> ("I16" in LspaceStr3_I16);
// empty when the name carries none
static strptr NumtypeToken(strptr name) {
    strptr retval;
    ind_beg(algo::Sep_curs, token, name, '_') {
        if (!elems_N(retval) && elems_N(token) >= 2 && (token.elems[0] == 'I' || token.elems[0] == 'U')) {
            bool digits = true;
            for (int i = 1; i < elems_N(token); i++) {
                digits = digits && algo_lib::DigitCharQ(token.elems[i]);
            }
            if (digits) {
                retval = token;
            }
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

static void CheckSmallstr(amc::FSmallstr &smallstr, amc::FNumstr *numstr) {
    // check string length -- must appear in the name
    // and be consistent with declared length
    // e.g. algo.Smallstr95
    // algo.LspaceStr7_I32_Base36
    if (smallstr.strict) {
        algo::Smallstr50 suffix(Pathcomp(name_Get(*smallstr.p_field->p_ctype),"_LLBLLrRR"));
        int isuffix = ParseI32(suffix,0);
        if (isuffix != 0 && isuffix != smallstr.length) {
            algo_lib::_db.exit_code++;
            prerr("amc.badsuffix"
                  <<Keyval("field",smallstr.field)
                  <<Keyval("suffix",suffix)
                  <<Keyval("length",smallstr.length)
                  <<Keyval("comment","Please use a suffix consistent with string length (or a non-numeric suffix)"));
        }
    }
    if (numstr && smallstr.strict) {
        strptr prefix =
            smallstr.strtype == dmmeta_Strtype_strtype_rightpad && smallstr.pad.value == "' '" ? "RspaceStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_leftpad && smallstr.pad.value == "' '" ? "LspaceStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_leftpad && smallstr.pad.value == "'0'" ? "LnumStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_leftpad && smallstr.pad.value == "0" ? "LnullStr"
            : smallstr.strtype == dmmeta_Strtype_strtype_rightpad && smallstr.pad.value == "0" ? "RnullStr"
            : "";
        if (!StartsWithQ(name_Get(*smallstr.p_field->p_ctype),prefix)) {
            algo_lib::_db.exit_code++;
            prerr("amc.numstr_badprefix"
                  <<Keyval("field",smallstr.field)
                  <<Keyval("strtype",smallstr.strtype)
                  <<Keyval("requiredprefix",prefix)
                  <<Keyval("comment","Please use a consistent prefix"));
        }
        // the numeric-type token in the ctype name must match the builtin the
        // numstr numtype stands for: a name that says I16 over a u16 numtype
        // misleads every reader about which sign regime the type stores. The
        // numtype is judged by the builtin it stands for and not by the way it
        // is spelled, because a numtype may name a wrapper ctype over the
        // builtin (algo.Uint32 over u32), and a name stating that wrapper's
        // width and sign is truthful. A numtype standing for no builtin leaves
        // the token nothing to be judged against, and tclass_Numstr rejects
        // such a field on its own
        strptr numtok = NumtypeToken(name_Get(*smallstr.p_field->p_ctype));
        amc::BltinId bltin_id(amc_BltinIdEnum(0));
        bool numtype_known = amc::GetBltinId(*numstr->p_numtype, bltin_id);
        tempstr numtype_tok;
        if (numtype_known) {
            strptr_ToUpper(value_ToCstr(bltin_id), numtype_tok);
        }
        if (numtype_known && elems_N(numtok) && numtok != numtype_tok) {
            algo_lib::_db.exit_code++;
            prerr("amc.numstr_badnumtype"
                  <<Keyval("field",smallstr.field)
                  <<Keyval("name_token",numtok)
                  <<Keyval("numtype",numstr->numtype)
                  <<Keyval("comment","ctype name and numstr numtype disagree"));
        }
    }
    // check that *no* digit character is used as a right-pad: reading the
    // field as a number cannot tell such a pad from a stored digit. A
    // numstr field is exempt -- tclass_Numstr rejects exactly the pads
    // that are digits of the field's own base
    if (!numstr && smallstr.strtype == dmmeta_Strtype_strtype_rightpad && !GoodRnumPadQ(smallstr)) {
        algo_lib::_db.exit_code++;
        prerr("amc.invalidpad"
              <<Keyval("field",smallstr.field)
              <<Keyval("pad",smallstr.pad)
              <<Keyval("comment","right-pad character cannot be confused with a number"));
    }
    if (numstr && smallstr.strict && !GoodBaseQ(smallstr)) {
        algo_lib::_db.exit_code++;
        prerr("amc.badbase"
              <<Keyval("field",smallstr.field)
              <<Keyval("base",numstr->base)
              <<Keyval("comment","mismatch between ctype name and numstr base"));
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Smallstr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    vrfy(field.c_smallstr, "smallstr required");
    amc::FSmallstr& smallstr = *field.c_smallstr;

    CheckSmallstr(smallstr, smallstr.c_numstr);

    Set(R, "$max_length"   , tempstr() << smallstr.length);
    Set(R, "$pad"   , ch_N(smallstr.pad.value) ? strptr(smallstr.pad.value) : strptr("0"));
    // an rpascal string keeps its length in a single byte, so 255 is the
    // longest one whose length is representable. The count of errors the run
    // exits with is the count of diagnostics it printed, so this check raises
    // the count rather than setting it: assigning would discard whatever the
    // fields examined before this one reported. test/atf_comp/amc.BadSmallstrToobig
    // pins the count over a universe holding two oversize strings
    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal && smallstr.length >= 256) {
        algo_lib::_db.exit_code++;
        prerr("smallstr.toobig"
              <<Keyval("field",field.field)
              <<Keyval("length",smallstr.length)
              <<Keyval("comment","smallstr length too large (must be <= 255)"));
    }

    InsStruct(R, field.p_ctype, "enum { $name_max = $max_length };");

    // allow Rpascal strings on the wire
    // if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal && field.p_ctype->c_pack) {
    //     prerr("amc.pascalpack"
    //           <<Keyval("field",field.field)
    //           <<Keyval("comment","Rpascal string cannot be packed (it is not wire-safe)"));
    //     algo_lib::_db.exit_code=1;
    // }

    // declare field as u8[]; not char* because none of these are
    // zero-terminated by construction.
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad || smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        InsStruct(R, field.p_ctype, "u8 $name[$max_length];");
    } else {
        InsStruct(R, field.p_ctype, "u8 $name[$max_length+1];");
        InsStruct(R, field.p_ctype, "u8 n_$name;");// extra 1 byte is required by rpascal.
        InsStruct(R, field.p_ctype, "");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Smallstr_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;

    // init function
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    init.ismacro = false; // cannot be a macro, participates in reflection
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (HasReadQ(*field.p_ctype)) {
        amc::FFunc& rd = amc::CreateCurFunc();
        Ins(&R, rd.comment, "Convert string to field. Return success value");
        Ins(&R, rd.ret  , "bool", false);
        Ins(&R, rd.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr rhs)", false);
        Ins(&R, rd.body, "bool retval = false;");
        Ins(&R, rd.body, "if (rhs.n_elems <= $max_length) {");
        Ins(&R, rd.body, "    $name_SetStrptr($pararg, rhs);");
        Ins(&R, rd.body, "    retval = true;");
        Ins(&R, rd.body, "} else {");
        Ins(&R, rd.body, "    algo_lib::AppendErrtext(\"comment\",\"text too long, limit $max_length\");");
        Ins(&R, rd.body, "}");
        Ins(&R, rd.body, "return retval;");
    }
}

void amc::tfunc_Smallstr_Print() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    // print function.
    // strip any padding first).
    amc::FFunc& print = amc::CreateCurFunc();
    Ins(&R, print.ret  , "void", false);
    Ins(&R, print.proto, "$name_Print($Parent, algo::cstring &out)", false);
    Ins(&R, print.body, "ch_Addary(out, $name_Getary($pararg));");
}

// -----------------------------------------------------------------------------

// compute length
void amc::tfunc_Smallstr_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;
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

// -----------------------------------------------------------------------------

// Max # of elements (constant)
void amc::tfunc_Smallstr_Max() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& maxitems = amc::CreateCurFunc();
    maxitems.inl=true;
    Ins(&R, maxitems.comment, "always return constant $max_length");
    Ins(&R, maxitems.ret  , "int", false);
    Ins(&R, maxitems.proto, "$name_Max($Parent)", false);
    Ins(&R, maxitems.body, "(void)$parname;");
    Ins(&R, maxitems.body, "return $max_length;");
}

// -----------------------------------------------------------------------------

// Set value as strptr
// For a padded string, the string value is allowed to use the pad character
// inside the string, i.e. a space-padded field can have a space ("abc def").
// Length of a padded string is determined by stripping the padded characters
// from the appropriate end.
void amc::tfunc_Smallstr_SetStrptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSmallstr& smallstr = *field.c_smallstr;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void", "", "");
    AddProtoArg(func, "const algo::strptr&", "rhs");
    Ins(&R, func.comment, "Set string to the value provided by RHS.");
    Ins(&R, func.comment, "If RHS is too large, it is silently clipped.");
    Ins(&R, func.body, "int len = i32_Min(rhs.n_elems, $max_length);");
    Ins(&R, func.body, "char *rhs_elems = rhs.elems;");
    Ins(&R, func.body, "int i = 0;");
    Ins(&R, func.body, "int j = 0;");
    if (smallstr.strtype == dmmeta_Strtype_strtype_leftpad) {
        Ins(&R, func.body, "for (; j < $max_length - len; j++) {");
        Ins(&R, func.body, "    $parname.$name[j] = $pad;");
        Ins(&R, func.body, "}");
    }
    Ins(&R, func.body, "for (; i < len; i++, j++) {");
    Ins(&R, func.body, "    $parname.$name[j] = rhs_elems[i];");
    Ins(&R, func.body, "}");
    if (smallstr.strtype == dmmeta_Strtype_strtype_rightpad) {
        Ins(&R, func.body, "for (; j < $max_length; j++) {");
        Ins(&R, func.body, "    $parname.$name[j] = $pad;");
        Ins(&R, func.body, "}");
    }
    if (smallstr.strtype == dmmeta_Strtype_strtype_rpascal) {
        Ins(&R, func.body, "$parname.n_$name       = u8(len);");
    }
}

// -----------------------------------------------------------------------------

// Assignment operator from strptr
// Generated only if the containing struct has only one field
void amc::tfunc_Smallstr_AssignStrptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (c_field_N(ctype)==1) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.member=true;
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "operator =(const algo::strptr &str)", false);
        Ins(&R, func.body, "$name_SetStrptr(*this, str);");
    }
}

// -----------------------------------------------------------------------------

// Construct from strptr
// Generated only if the containing struct has only one field
void amc::tfunc_Smallstr_CtorStrptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (c_field_N(ctype)==1) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        func.comment="";// erase it
        func.member=true;
        Ins(&R, func.proto, "$Name(const algo::strptr &rhs)", false);
        Ins(&R, func.body, "$name_SetStrptr(*this, rhs);");
    }
}
