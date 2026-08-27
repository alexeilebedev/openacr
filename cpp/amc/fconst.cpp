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
// Source: cpp/amc/fconst.cpp -- Constants, string <-> int conversion
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Bit mask FIELD's store applies to a value: the declared width for a Bitfld
// field, whose setter masks the store to those bits, and all ones for any other
// field, whose store is the arg type itself.
// The one place the mask is computed. Everything that has to know which bits a
// field's store keeps asks here -- the bitfield accessors that emit the mask
// itself, the decimal reader's range, an fconst value's range check, and a
// length field's largest frame -- because the width the mask is built from is a
// signed schema argument that a run can reach before anything has refused it,
// and each of those callers spelling the guard for itself is how they came to
// spell it differently.
// The shift computes the mask for the widths from one to sixty-three, the widths
// that name fewer bits than the word: the mask is a u64 shifted by the width, and
// a u64 has no shift by sixty-four or more. A width of exactly sixty-four is a
// legal width whose bits are every bit of the word, so all ones is that width's
// exact mask rather than a fallback -- the fixture's w64 field is one, and its
// fconst naming every bit of a u64 draws no range line.
// A width outside one to sixty-four is out of bounds, and the bitfield check
// reports it as such naming the field before any accessor is emitted. All ones is
// what this rule then falls back on, which leaves the store's bounds the arg
// type's own rather than computed from bits the schema does not describe: the
// value's own range check says nothing about a value the field's width, not the
// value, made impossible.
// test/amc/fconst_bitfld_width.ssim pins the widths this rule accepts and
// refuses, and amc.FconstBitfldWidth is the golden.
u64 amc::FieldStoreMask(amc::FField &field) {
    u64 retval = 0xffffffffffffffff;
    if (field.c_bitfld && field.c_bitfld->width >= 1 && field.c_bitfld->width < 64) {
        retval = (u64(1) << field.c_bitfld->width) - 1;
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Max value FIELD's store can hold: the arg type's max (GetMinMax), clamped to
// the bits the store keeps (FieldStoreMask). Returns false when the arg
// resolves to no integer type: the field then has no numeric range and MAX
// holds the widest-signed fallback.
bool amc::FieldMaxStore(amc::FField &field, u64 &max) {
    u64 minval = 0;
    bool issigned = false;
    bool known = amc::GetMinMax(*field.p_arg, minval, max, issigned);
    if (known) {
        max = u64_Min(max, amc::FieldStoreMask(field));
    }
    return known;
}

// -----------------------------------------------------------------------------

// Min value FIELD's store can hold: the arg type's min (GetMinMax), raised to
// zero when the store keeps fewer bits than a signed arg type spans.
// A Bitfld's Set masks the store to the declared width, and its Get casts those
// bits back to the arg type without sign-extending them. So a 4-bit field with
// arg i8 stores -1 as 0xf and reads it back as 15. The store keeps the sign only
// when its bits, minus the one the sign itself takes, still reach the arg type's
// maximum -- an 8-bit field with arg i8, where the mask round-trips every
// negative. Returns false when the arg resolves to no integer type: the field
// then has no numeric range and MIN holds the widest-signed fallback.
bool amc::FieldMinStore(amc::FField &field, u64 &min) {
    u64 maxval = 0;
    bool issigned = false;
    bool known = amc::GetMinMax(*field.p_arg, min, maxval, issigned);
    if (known && issigned && (amc::FieldStoreMask(field) >> 1) < maxval) {
        min = 0;
    }
    return known;
}

// -----------------------------------------------------------------------------

// Return the condition a value named VALNAME, parsed as FIELD's arg type, must
// satisfy for FIELD's store to hold it unchanged; empty when the arg type
// cannot produce a value the store narrows.
// A read parses into a temporary of the arg type and hands it to the field's
// Set function, and a Bitfld's Set masks the store to the declared width. So a
// 10-bit field with arg u16 takes "1024" from a tuple, parses it as 1024 and
// stores 0 -- a value the input never named. The store's own bounds
// (FieldMinStore, FieldMaxStore) say which end of the arg type's range it
// narrows, and each end contributes its own term: a field spanning its whole arg
// type (8 bits over u8), and a 1-bit field with arg bool, narrow neither end and
// need no condition, while a 7-bit field with arg i8 holds every positive value
// the arg type does and no negative one, so it takes the low-end term alone.
static tempstr FieldStoreRangeExpr(amc::FField &field, strptr valname) {
    tempstr ret;
    u64 argmin = 0;
    u64 argmax = 0;
    u64 minstore = 0;
    u64 maxstore = 0;
    bool issigned = false;
    bool known = amc::GetMinMax(*field.p_arg, argmin, argmax, issigned);
    known = known && amc::FieldMinStore(field, minstore);
    known = known && amc::FieldMaxStore(field, maxstore);
    bool lowterm = known && i64(minstore) > i64(argmin);
    bool highterm = known && maxstore < argmax;
    if (lowterm || highterm) {
        tempstr valexpr(valname);
        if (c_datafld_N(*field.p_arg) == 1) {
            // the arg type wraps its value in a single field, and Set stores
            // through that field; compare the same expression
            valexpr = amc::FieldvalExpr(field.p_arg, *c_datafld_Find(*field.p_arg,0), valname);
        }
        if (lowterm) {
            // the store holds no negative: the sign it drops reads back as a
            // positive number, so a negative value is refused outright
            ret << valexpr << " >= 0";
        }
        if (lowterm && highterm) {
            ret << " && ";
        }
        if (highterm) {
            tempstr hexval;
            u64_PrintHex(maxstore, hexval, 2, true, false);
            ret << valexpr << " <= " << hexval;
        }
    }
    return ret;
}

static bool FieldBitsetQ(amc::FField &field) {
    return !bh_bitfld_EmptyQ(field);
}

void amc::tclass_Fconst() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    bool is_string = FieldStringQ(field);

    vrfy(!FldfuncQ(field), "amc.gen_field_fconst  error:'fldfunc may not be combined with fconst'");
    Set(R, "$Enumtype", Enumtype(field));

    // Automatically create a ccast record for a single-field non-string ctype
    // Example:
    //
    // +    Status(tdm_StatusEnum arg) : status(bool(arg)) {}
    // +    operator tdm_StatusEnum() const { return tdm_StatusEnum(status); }
    if (!is_string && c_field_N(*field.p_ctype) == 1 && field.c_fcast && !FieldBitsetQ(field)) {
        field.c_fcast->expr = Enumtype(field);
    }

    // Get field as enum
    // A zero-length fconst name is legal here, and five rows in the tree have
    // one. On a numeric field the name is the constant's string form and the
    // empty name is the enum's sentinel, the member the empty string converts
    // to. On a string field the fconst's own value carries the string, so the
    // name is only the C++ symbol's last component, and a gconst over an
    // ssimfile whose primary key can be empty produces exactly that: the
    // dmmeta.ns row for the root namespace, and the dev.arch, dev.cfg,
    // dev.compiler and dev.license rows that stand for "any", each yield a
    // symbol ending in the field name and an underscore.
    if (is_string) {
        tempstr text;
        text << "// compile-time string constants for "<<field.field<<"\n";
        int n=0;
        ind_beg(amc::field_c_fconst_curs, fconst, field) {
            text << "const char *";
            text << fconst.cpp_name;
            text << "\t = ";
            strptr_PrintCppQuoted(fconst.value.value, text, '"');
            text << ";\n";
            n++;
            // pick a break-point that avoids global diffs
            if (strptr_Hash(0,fconst.cpp_name)%100 > 90) {
                text << "\n";
                *ns.cpp << Tabulated(text, "\t", "ll", 2);
                ch_RemoveAll(text);
                n=0;
            }
        }ind_end;
        if (n > 0) {
            text << "\n";
            *ns.cpp << Tabulated(text, "\t", "ll", 2);
        }
    }
}

// Generate $name_GetEnum: read the field's stored value as its enum type.
void amc::tfunc_Fconst_GetEnum() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& getenum = amc::CreateCurFunc();
        Ins(&R, getenum.comment, "Get value of field as enum type");
        Ins(&R, getenum.ret  , "$Enumtype", false);
        Ins(&R, getenum.proto, "$name_GetEnum($Cparent)", false);
        // bitfld / fbigend / fldfunc fields have no direct member; read via the generated accessor
        if (field.c_fbigend || field.c_bitfld || FldfuncQ(field)) {
            Ins(&R, getenum.body, "return $Enumtype($name_Get($pararg));");
        } else {
            Ins(&R, getenum.body, "return $Enumtype($parname.$name);");
        }
    }
}

void amc::tfunc_Fconst_SetEnum() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& setenum = amc::CreateCurFunc();
        Ins(&R, setenum.comment, "Set value of field from enum type.");
        Ins(&R, setenum.ret  , "void", false);
        Ins(&R, setenum.proto, "$name_SetEnum($Parent, $Enumtype rhs)", false);
        Ins(&R, setenum.body, AssignExpr(field, "$parname", "rhs", true)<<";");
    }
}

// Generate $name_ToCstr: map the field's numeric value to its string constant, or NULL when none matches.
void amc::tfunc_Fconst_ToCstr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& tocstr = amc::CreateCurFunc();
        Ins(&R, tocstr.comment, "Convert numeric value of field to one of predefined string constants.");
        Ins(&R, tocstr.comment, "If string is found, return a static C string. Otherwise, return NULL.");
        Ins(&R, tocstr.ret  , "const char*", false);
        Ins(&R, tocstr.proto, "$name_ToCstr($Cparent)", false);
        Ins(&R, tocstr.body,  "const char *ret = NULL;");
        Ins(&R, tocstr.body,  "switch($name_GetEnum($pararg)) {");
        // compose switch statement.
        // note: some fields have two or more names for one numeric value.
        // only the first will be returned.
        ind_beg(amc::field_c_fconst_curs, fconst, field) if (amc::ind_fconst_int_InsertMaybe(fconst)) {
            tempstr caselabel;
            strptr_PrintPadRight(fconst.cpp_name, caselabel,  30);
            Set(R, "$caselabel" , caselabel);
            Set(R, "$casestrval", name_Get(fconst));
            Ins(&R, tocstr.body,  "    case $caselabel: ret = \"$casestrval\";  break;");
        }ind_end;
        ind_beg(amc::field_c_fconst_curs, fconst, field) {
            amc::ind_fconst_int_Remove(fconst);
        }ind_end;
        Ins(&R, tocstr.body,  "}");
        Ins(&R, tocstr.body,  "return ret;");
    }
}

// Generate $name_Print: emit the field as its string constant, falling back to the numeric value.
void amc::tfunc_Fconst_Print() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        Set(R, "$getexpr", amc::FieldvalExpr(field.p_ctype,field,"$parname"));
        amc::FFunc& prn = amc::CreateCurFunc();
        Ins(&R, prn.comment, "Convert $name to a string. First, attempt conversion to a known string.");
        Ins(&R, prn.comment, "If no string matches, print $name as a numeric value.");
        Ins(&R, prn.ret  , "void", false);
        Ins(&R, prn.proto, "$name_Print($Cparent, algo::cstring &lhs)", false);
        Ins(&R, prn.body,  "const char *strval = $name_ToCstr($pararg);");
        Ins(&R, prn.body,  "if (strval) {");
        Ins(&R, prn.body,  "    lhs << strval;");
        Ins(&R, prn.body,  "} else {");
        Ins(&R, prn.body,  "    lhs << $getexpr;");
        Ins(&R, prn.body,  "}");
    }
}

// Generate $name_SetStrptrMaybe: set the field from a string constant, returning false when the string matches none.
void amc::tfunc_Fconst_SetStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& mbssp = amc::CreateCurFunc();
        Ins(&R, mbssp.comment, "Convert string to field.");
        Ins(&R, mbssp.comment, "If the string is invalid, do not modify field and return false.");
        Ins(&R, mbssp.comment, "In case of success, return true");
        Ins(&R, mbssp.ret  , "bool", false);
        Ins(&R, mbssp.proto, "$name_SetStrptrMaybe($Parent, algo::strptr rhs)", false);
        Ins(&R, mbssp.body,  "bool ret = false;");
        Ins(&R, mbssp.body,  "switch (elems_N(rhs)) {");
        // compute list of enumstr and index it by length.
        amc::enumstr_len_RemoveAll();
        amc::enumstr_RemoveAll();
        ind_beg(amc::field_c_fconst_curs, fconst, field) {
            amc::Enumstr key(ch_N(name_Get(fconst)), algo::RnullStr8(FirstN(strptr(name_Get(fconst)),8)));
            amc::ind_enumstr_len_GetOrCreate(key.len);        // pre-create
            amc::FEnumstr &enumstr = amc::ind_enumstr_GetOrCreate(key);
            c_fconst_Insert(enumstr, fconst);
        }ind_end;
        while (amc::FEnumstrLen *enumstr_len = amc::bh_enumstr_len_RemoveFirst()) {
            int len = enumstr_len->len;
            Ins(&R, mbssp.body, tempstr()<<"case "<<len<<": {");
            if (enumstr_len->len == 0) {
                // A zero-length name is the enum's sentinel member (a
                // proctype:"" or unit:"" row): the empty string converts to
                // it. At most one such fconst can exist per field -- the name
                // is part of the fconst's key -- so the bucket holds at most
                // one entry.
                if (bh_enumstr_N(*enumstr_len) > 0) {
                    amc::FFconst *fconst = amc::c_fconst_Find(*amc::bh_enumstr_First(*enumstr_len), 0);
                    Set(R, "$Const", fconst->cpp_name);
                    Ins(&R, mbssp.body, tempstr()<<"    $name_SetEnum($pararg,$Const);");
                    Ins(&R, mbssp.body, tempstr()<<"    ret = true;");
                }
            } else {
                Ins(&R, mbssp.body, tempstr()<<"    switch ("<<amc::VarStringToInteger("rhs",len <= 8? len : 8)<<") {");
                while (amc::FEnumstr *enumstr = amc::bh_enumstr_RemoveFirst(*enumstr_len)) {
                    Ins(&R, mbssp.body, tempstr()<<"    case "<<amc::ConstStringToCaseLabel(enumstr->enumstr.str)<<": {");
                    ind_beg(amc::enumstr_c_fconst_curs, fconst, *enumstr) {
                        Set(R, "$Const", fconst.cpp_name);
                        if (len>8) {
                            tempstr val;
                            strptr_PrintCpp(RestFrom(strptr(name_Get(fconst)),8), val);
                            Set(R, "$remain", tempstr()<<len-8);
                            Set(R, "$Val", val);
                            Ins(&R, mbssp.body, "        if (memcmp(rhs.elems+8,$Val,$remain)==0) { $name_SetEnum($pararg,$Const); ret = true; break; }");
                        } else {
                            Ins(&R, mbssp.body, "        $name_SetEnum($pararg,$Const); ret = true; break;");
                        }
                    }ind_end;
                    if (len>8) {
                        Ins(&R, mbssp.body, "        break;");
                    }
                    Ins(&R, mbssp.body, "    }");
                }
                Ins(&R, mbssp.body, "    }");
            }
            Ins(&R, mbssp.body, "    break;");// top case
            Ins(&R, mbssp.body, "}");
        }
        Ins(&R, mbssp.body,  "}");
        Ins(&R, mbssp.body,  "return ret;");
    }
}

// Generate $name_SetStrptr: set the field from a string constant, using the supplied default when none matches.
void amc::tfunc_Fconst_SetStrptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field) && !FieldBitsetQ(field)) {
        amc::FFunc& ssp = amc::CreateCurFunc();
        Ins(&R, ssp.comment, "Convert string to field.");
        Ins(&R, ssp.comment, "If the string is invalid, set numeric value to DFLT");
        Ins(&R, ssp.ret  , "void", false);
        Ins(&R, ssp.proto, "$name_SetStrptr($Parent, algo::strptr rhs, $Enumtype dflt)", false);
        Ins(&R, ssp.body,  "if (!$name_SetStrptrMaybe($pararg,rhs)) $name_SetEnum($pararg,dflt);");
    }
}

// Generate $name_ReadStrptrMaybe: read the field from a string, trying the string constant first, then the underlying type.
void amc::tfunc_Fconst_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!FieldStringQ(field)  && !FieldBitsetQ(field) && HasReadQ(*field.p_ctype)) {
        amc::FFunc& rd = amc::CreateCurFunc();
        Ins(&R, rd.comment, "Convert string to field. Return success value");
        Ins(&R, rd.ret  , "bool", false);
        Ins(&R, rd.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr rhs)", false);
        Ins(&R, rd.body    , "bool retval = false;");
        Ins(&R, rd.body    , "retval = $name_SetStrptrMaybe($pararg,rhs); // try symbol conversion");
        Ins(&R, rd.body    , "if (!retval) { // didn't work? try reading as underlying type");
        // a field whose stores route through the generated Set (no direct member,
        // or a Set dispatching side effects) reads into a temporary; the symbolic
        // path above stores via SetEnum, and the numeric fallback must match it
        if (AssignViaSetQ(field)) {
            Ins(&R, rd.body, "    $Fldtype $name_tmp;");
            Ins(&R, rd.body, "    retval = $Fldtype_ReadStrptrMaybe($name_tmp, rhs);");
            tempstr rangeexpr = FieldStoreRangeExpr(field, "$name_tmp");
            if (ch_N(rangeexpr) > 0) {
                Ins(&R, rd.body, tempstr()<<"    retval = retval && ("<<rangeexpr<<");// value must fit the field's store");
            }
            Ins(&R, rd.body, "    if (retval) {");
            Ins(&R, rd.body, "        $name_Set($pararg, $name_tmp);");
            Ins(&R, rd.body, "    }");
        } else {
            Ins(&R, rd.body, "    retval = $Fldtype_ReadStrptrMaybe($parname.$name,rhs);");
        }
        Ins(&R, rd.body    , "}");
        Ins(&R, rd.body    , "return retval;");
    }
}

// -----------------------------------------------------------------------------

// Read VALUE as one complete C++ integer literal, the spelling the generated
// code hands the compiler: an optional sign, then 0x and hex digits, or a
// leading zero and octal digits, or decimal digits, then an optional u/U/l/L
// type suffix and nothing more. MAG receives the magnitude, NEG whether a
// minus sign preceded it, and FIT whether the magnitude fits 64 bits -- when
// it does not, MAG has wrapped and says nothing about the value.
// The bases are the compiler's, because the compiler reads the same
// characters: a value spelled 0377 stores 255, and a checker reading it as
// decimal 377 would reject a constant that fits a u8 store.
// Return false when the value is spelled as anything else -- an expression, a
// character literal, an LE_STR, a binary or digit-separated literal, a name
// only the compiler resolves -- since such a value carries no number a range
// can be compared with.
static bool ReadIntlitMaybe(algo::strptr value, u64 &mag, bool &neg, bool &fit) {
    int i = 0;
    int n = value.n_elems;
    u64 base = 10;
    mag = 0;
    neg = false;
    fit = true;
    if (i < n && (value[i] == '+' || value[i] == '-')) {
        neg = value[i] == '-';
        i++;
    }
    if (i + 1 < n && value[i] == '0' && (value[i+1] == 'x' || value[i+1] == 'X')) {
        base = 16;
        i += 2;
    } else if (i + 1 < n && value[i] == '0') {
        // the zero that selects the base is left where it stands, because it is
        // also a digit of the value: consuming it makes "0u" -- a bare zero
        // carrying a type suffix -- read as a literal with no digits at all,
        // which returns false and exempts the value from the range check
        base = 8;
    }
    int first = i;
    bool digitq = true;
    while (digitq && i < n) {
        u8 digit = 0;
        int hexq = algo::ParseHex1(value[i], digit);
        digitq = hexq != 0 && u64(digit) < base;
        if (digitq) {
            fit = fit && mag <= (u64(-1) - digit) / base;
            mag = mag * base + digit;
            i++;
        }
    }
    bool anydigit = i > first;
    while (i < n && (value[i] == 'u' || value[i] == 'U' || value[i] == 'l' || value[i] == 'L')) {
        i++;
    }
    return anydigit && i == n;
}

// -----------------------------------------------------------------------------

// Validate each fconst's value against the store of the field it names.
// A ten-bit bitfield with arg u32 that carries an fconst of 2000 sets 976
// through the generated Set, so a tuple naming that symbol is accepted and the
// field comes back holding a number the schema never named, which ToCstr and
// GetEnum can no longer map to the symbol. The store's bounds (FieldMinStore,
// FieldMaxStore) are the bounds the same field's reader already imposes on a
// numeric input, so the symbol carrying the value is held to them too. A value
// outside them fails the run naming the fconst and the range.
// The store bounds a magnitude, one side at a time: the positive side reaches
// the store's maximum, and the negative side reaches the negation of its
// minimum, which is zero for an unsigned store and for bits that drop a signed
// arg type's sign. Carrying sign and magnitude apart is what lets an unsigned
// store hold every value up to 18446744073709551615, which no signed reading of
// the same 64 bits can express.
// A value that is not one complete integer literal -- a shift expression, a
// character literal, an LE_STR, a name that only the C++ compiler resolves,
// digits with trailing text -- carries no number to compare and is not bounded
// here. Neither is a field whose arg resolves to no integer type, which has no
// numeric range at all; the value of a string field is a string rather than a
// number, and it falls under that same case, since a cstr ctype wraps a
// character array and GetMinMax resolves only a chain of single Val fields down
// to an integer bltin. A magnitude that outruns 64 bits fits no store and is
// reported, since every store's own maximum is a 64-bit number.
// The fconsts of a field that sources bitfields are the masks of those bitfields
// rather than values of the field, so a word using its top bit carries a mask
// past its own signed maximum by design; each such mask is bounded by the bits
// of the word it sources, which gen_check_bitfld checks on the bitfield itself.
void amc::gen_check_fconst() {
    ind_beg(amc::_db_fconst_curs, fconst, amc::_db) {
        amc::FField &field = *fconst.p_field;
        u64 mag = 0;
        bool neg = false;
        bool fit = false;
        u64 argmin = 0;
        u64 argmax = 0;
        u64 minstore = 0;
        u64 maxstore = 0;
        bool issigned = false;
        bool numeric = ReadIntlitMaybe(fconst.value.value, mag, neg, fit) && !FieldBitsetQ(field);
        bool known = numeric && amc::GetMinMax(*field.p_arg, argmin, argmax, issigned);
        known = known && amc::FieldMinStore(field, minstore);
        known = known && amc::FieldMaxStore(field, maxstore);
        bool inrange = true;
        if (known) {
            u64 magmax = neg ? u64(0) - minstore : maxstore;
            inrange = fit && mag <= magmax;
        }
        if (!inrange) {
            tempstr minstr;
            tempstr maxstr;
            if (issigned) {
                minstr << i64(minstore);
                maxstr << i64(maxstore);
            } else {
                minstr << minstore;
                maxstr << maxstore;
            }
            prerr("amc.fconst_range"
                  <<Keyval("fconst",fconst.fconst)
                  <<Keyval("value",fconst.value)
                  <<Keyval("min",minstr)
                  <<Keyval("max",maxstr)
                  <<Keyval("comment","fconst value does not fit the field's store and would be stored masked"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}
