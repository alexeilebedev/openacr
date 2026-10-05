// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/proj.cpp
//

#include "include/algo.h"
#include "include/amc.h"

// A dmmeta.nslang row projects every ctype of a namespace into a language, and a
// dmmeta.ctypelang row projects one ctype.  The projection is those ctypes plus
// every ctype they are made of, and each ctype records the languages it is
// projected into.  Every language's emitter reads that record and nothing else.
// A packed projected ctype is a wire form, and a language writes a codec for it,
// so gen_check_proj refuses one a foreign codec cannot lay out byte for byte.  An
// unpacked projected ctype is a type alone, the shape of a JSON reply or a
// table row.  A dmmeta.displang row names a dispatch a language speaks, over
// messages the projection already holds: the language gets the dispatch's
// signature constant, and in Rust the dispatch's namespace roots the crate.
// This file holds what every language shares, and each language's emitter is a
// file of its own beside it.

// -----------------------------------------------------------------------------

// Add CTYPE, its base and the ctypes its fields are made of to the projection
// into LANG.  A builtin carries no struct of its own and is left out, and so is
// a string with no wire form, which every language reads as its own string.
// The recursion descends one level per nesting of a ctype inside another, so its
// depth is the schema's nesting depth; membership is recorded before the fields
// are walked, so a message whose tail is typed by its own header terminates.
static void ProjVisit(amc::FCtype &ctype, amc::FLang &lang) {
    bool skip = ctype.c_bltin || (ctype.c_cstr && !ctype.c_pack);
    if (!skip && amc::c_lang_ScanInsertMaybe(ctype, lang)) {
        if (amc::FCtype *base = amc::GetBaseType(ctype,NULL)) {
            ProjVisit(*base, lang);
        }
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            bool inst = field.reftype == dmmeta_Reftype_reftype_Val
                || field.reftype == dmmeta_Reftype_reftype_Inlary
                || field.reftype == dmmeta_Reftype_reftype_Opt
                || field.reftype == dmmeta_Reftype_reftype_Varlen
                || field.reftype == dmmeta_Reftype_reftype_Bitfld;
            if (inst) {
                ProjVisit(*field.p_arg, lang);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Collect the projection: every ctype of each nslang row's namespace and the
// ctype of each ctypelang row, closed over the ctypes they are made of, each
// marked with the row's language.  The TypeScript emitter then gets a record
// per projected ctype and per namespace holding one, to assemble its module in.
void amc::gen_prep_proj() {
    ind_beg(amc::_db_nslang_curs, nslang, amc::_db) {
        ind_beg(amc::ns_c_ctype_curs, ctype, *nslang.p_ns) {
            ProjVisit(ctype, *nslang.p_lang);
        }ind_end;
    }ind_end;
    ind_beg(amc::_db_ctypelang_curs, ctypelang, amc::_db) {
        ProjVisit(*ctypelang.p_ctype, *ctypelang.p_lang);
    }ind_end;
    amc::FLang *lang = amc::ind_lang_Find(amc::amcdb_lang_ts);
    if (lang) {
        ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
            // a ctype TypeScript already has a built-in for is a string or a
            // number there, and a field of it reads as that built-in
            bool bltin = ctype.c_cstr || ctype.c_cjsbltin;
            if (amc::ProjCtypeQ(ctype, *lang) && !bltin) {
                amc::FTsctype &tsctype = amc::tsctype_Alloc();
                tsctype.ctype = ctype.ctype;
                (void)amc::tsctype_XrefMaybe(tsctype);
                if (!ctype.p_ns->c_tsns) {
                    amc::FTsns &tsns = amc::tsns_Alloc();
                    tsns.ns = ctype.p_ns->ns;
                    (void)amc::tsns_XrefMaybe(tsns);
                }
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Return TRUE when CTYPE is projected into LANG.
bool amc::ProjCtypeQ(amc::FCtype &ctype, amc::FLang &lang) {
    bool ret = false;
    ind_beg(amc::ctype_c_lang_curs, proj, ctype) {
        ret = ret || &proj == &lang;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when a displang row projects the messages of DISPATCH into LANG.
bool amc::ProjDispatchQ(amc::FDispatch &dispatch, amc::FLang &lang) {
    bool ret = false;
    ind_beg(amc::dispatch_c_displang_curs, displang, dispatch) {
        ret = ret || displang.p_lang == &lang;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when namespace NS has anything to write in LANG: a ctype projected
// into LANG, or a dispatch a displang row projects into LANG.
bool amc::ProjNsQ(amc::FNs &ns, amc::FLang &lang) {
    bool ret = false;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        ret = ret || amc::ProjCtypeQ(ctype, lang);
    }ind_end;
    ind_beg(amc::ns_c_dispatch_curs, dispatch, ns) {
        ret = ret || amc::ProjDispatchQ(dispatch, lang);
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when CTYPE is projected with a codec.  A packed ctype is a wire
// form, so a language encodes and decodes it; any other projected ctype is a
// type alone.
bool amc::ProjCodecQ(amc::FCtype &ctype) {
    return ctype.c_pack != NULL;
}

// -----------------------------------------------------------------------------

// Return TRUE when unpacked CTYPE has a memory layout a projected language
// states: it is plain data, so its bytes are its value, every member is a
// scalar or a nested ctype that starts at its own defaults, and sizing placed
// every member.  The recursion descends one level per nesting.  Its
// size and member offsets are then the ones C++ is held to, which is what lets
// a foreign program share a segment with a C++ one.
bool amc::ProjLayoutQ(amc::FCtype &ctype) {
    bool ret = ctype.plaindata && !ctype.size_unknown && !amc::ProjCodecQ(ctype);
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        bool val = field.reftype == dmmeta_Reftype_reftype_Val && !amc::ProjStringQ(field);
        bool nested = !field.p_arg->c_bltin;
        ret = ret && val && field.offset >= 0 && (!nested || amc::ProjDfltQ(*field.p_arg));
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when the members of projected CTYPE start at their schema
// defaults: it has a codec or a layout.  Any other projected ctype is a
// declaration whose members start at their language's zero.
bool amc::ProjDfltQ(amc::FCtype &ctype) {
    return amc::ProjCodecQ(ctype) || amc::ProjLayoutQ(ctype);
}

// -----------------------------------------------------------------------------

// Return TRUE when a type-only projection declares FIELD as a string: a key into
// another table, a pattern over one, an inline string or array of char, or a
// value of a string ctype.
bool amc::ProjStringQ(amc::FField &field) {
    bool chars = field.reftype == dmmeta_Reftype_reftype_Inlary && field.arg == "char";
    return field.reftype == dmmeta_Reftype_reftype_Pkey
        || field.reftype == dmmeta_Reftype_reftype_Regx
        || field.reftype == dmmeta_Reftype_reftype_RegxSql
        || field.reftype == dmmeta_Reftype_reftype_Smallstr
        || chars
        || field.p_arg->c_cstr != NULL;
}

// -----------------------------------------------------------------------------

// Return TRUE when namespace NS has anything to write in any language.
bool amc::ProjNsAnyQ(amc::FNs &ns) {
    bool ret = false;
    ind_beg(amc::_db_lang_curs, lang, amc::_db) {
        ret = ret || amc::ProjNsQ(ns, lang);
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when the member FIELD is declared, in a projected language, as the
// projected ctype of its arg: any arg that is not a builtin, except a string a
// type-only projection declares as the language's own string.
bool amc::ProjNestedQ(amc::FField &field) {
    bool typeonly_string = !amc::ProjCodecQ(*field.p_ctype) && amc::ProjStringQ(field);
    return !field.p_arg->c_bltin && !typeonly_string;
}

// -----------------------------------------------------------------------------

// Return TRUE when FIELD is a member of its ctype in a projected language.  A
// Base field is an injection point whose fields appear individually, a bitfield
// is an accessor over its source word, and a message's type and length words are
// derived: the encoder writes the message's own type and the frame's length, and
// a decoded value that carried them would restate what the caller had to know to
// decode.
bool amc::ProjMemberQ(amc::FField &field) {
    bool derived = field.p_ctype->c_msgtype && (field.c_typefld || field.c_lenfld);
    return field.reftype != dmmeta_Reftype_reftype_Base
        && field.reftype != dmmeta_Reftype_reftype_Bitfld
        && !derived;
}

// -----------------------------------------------------------------------------

// Return TRUE when FIELD's bytes are the frame's tail rather than a fixed slot:
// a Varlen or an Opt field.
bool amc::ProjTailQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Varlen
        || field.reftype == dmmeta_Reftype_reftype_Opt;
}

// -----------------------------------------------------------------------------

// Return TRUE when FIELD is an rpascal inline string: characters, a spare byte,
// and the count in the byte after them.
bool amc::ProjRpascalQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Smallstr
        && field.c_smallstr
        && field.c_smallstr->strtype == dmmeta_Strtype_strtype_rpascal;
}

// -----------------------------------------------------------------------------

// Return the byte FIELD's rightpad inline string is padded with, read from its C++
// expression: a decimal number or a one-character quoted char.  Return -1 when
// FIELD is not a rightpad string or the expression is neither.
int amc::ProjPadByte(amc::FField &field) {
    int ret = -1;
    bool rightpad = field.reftype == dmmeta_Reftype_reftype_Smallstr
        && field.c_smallstr
        && field.c_smallstr->strtype == dmmeta_Strtype_strtype_rightpad;
    algo::strptr pad = rightpad ? algo::strptr(field.c_smallstr->pad.value) : algo::strptr();
    u8 value = 0;
    if (!rightpad) {
        // no pad byte
    } else if (elems_N(pad) == 3 && pad.elems[0] == '\'' && pad.elems[2] == '\'') {
        ret = u8(pad.elems[1]);
    } else if (u8_ReadStrptrMaybe(value, pad)) {
        ret = value;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when FIELD is a rightpad inline string a projected language
// models: exactly its characters, padded on the right with a known byte.
bool amc::ProjRightpadQ(amc::FField &field) {
    return amc::ProjPadByte(field) >= 0;
}

// -----------------------------------------------------------------------------

// Return TRUE when CTYPE has a tail: a Varlen or an Opt field.
bool amc::ProjAnyTailQ(amc::FCtype &ctype) {
    bool ret = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        ret = ret || amc::ProjTailQ(field);
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when builtin ARG is a scalar with a fixed wire form every projected
// language reads: an integer of 8 to 64 bits, a char, or a bool.
bool amc::ProjScalarQ(amc::FCtype &arg) {
    return arg.ctype == "u8" || arg.ctype == "char"
        || arg.ctype == "u16" || arg.ctype == "u32" || arg.ctype == "u64"
        || arg.ctype == "i8" || arg.ctype == "i16" || arg.ctype == "i32" || arg.ctype == "i64"
        || arg.ctype == "bool";
}

// -----------------------------------------------------------------------------

// Return TRUE when builtin ARG is a floating point number, which a type-only
// projection declares as its language's float.
bool amc::ProjFloatQ(amc::FCtype &arg) {
    return arg.ctype == "double" || arg.ctype == "float";
}

// -----------------------------------------------------------------------------

// Return the bytes FIELD occupies in the fixed region of its ctype's wire form,
// or -1 when FIELD has no wire form.  This is the one place a field's wire form
// is decided, so an encoder and a decoder cannot place a field at two different
// offsets, and neither can two languages.  A Base, a bitfield and a tail occupy
// no fixed slot.  A Val field of a ctype occupies that ctype's size only while
// the ctype has no tail of its own, since a nested tail would run into the next
// slot.  An inline array occupies its elements only when it is fixed, which is
// the shape C++ lays out as a bare element array.  An inline string occupies its
// characters, a spare byte and its count when it is rpascal, and its characters
// alone when it is rightpad; those are the string forms a projected language
// models.
int amc::ProjSlotBytes(amc::FField &field) {
    int ret = -1;
    bool scalar = !field.p_arg->c_bltin || amc::ProjScalarQ(*field.p_arg);
    if (field.reftype == dmmeta_Reftype_reftype_Base || field.reftype == dmmeta_Reftype_reftype_Bitfld) {
        ret = 0;
    } else if (amc::ProjRpascalQ(field)) {
        ret = field.c_smallstr->length + 2;
    } else if (amc::ProjRightpadQ(field)) {
        ret = field.c_smallstr->length;
    } else if (amc::ProjTailQ(field)) {
        ret = 0;
    } else if (field.reftype == dmmeta_Reftype_reftype_Val && scalar && !amc::RuntimeFrameLenQ(*field.p_arg)) {
        ret = field.p_arg->totsize_byte;
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary && scalar && amc::FixaryQ(field) && !amc::RuntimeFrameLenQ(*field.p_arg)) {
        ret = field.c_inlary->max * field.p_arg->totsize_byte;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the byte count of CTYPE's fixed region as a projected codec lays it
// out: every field's slot, then the end offset of every tail but the last, each
// in a word typed by the length field.  A ctype whose C++ size differs has
// padding no projected codec models, and gen_check_proj refuses it.
static int ProjFixedBytes(amc::FCtype &ctype) {
    int ret = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        ret += i32_Max(amc::ProjSlotBytes(field), 0);
    }ind_end;
    amc::FField *lenfield = amc::LengthField(ctype);
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        if (amc::ctype_zd_varlenfld_Next(field) && lenfield) {
            ret += lenfield->p_arg->totsize_byte;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when TEXT is a product of decimal integers, such as 1024*1024,
// which a C++ default may spell and amc folds to one literal for a projection.
static bool ProjProductQ(algo::strptr text) {
    bool ret = algo::FindChar(text, '*') != -1;
    algo::StringIter iter(text);
    while (ret && !iter.EofQ()) {
        algo::strptr factor = algo::GetTokenChar(iter, '*');
        ret = elems_N(factor) > 0;
        for (int i = 0; i < elems_N(factor); i++) {
            ret = ret && algo_lib::DigitCharQ(factor.elems[i]);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the default of FIELD as a projected language states it: a product of
// decimal integers folded to its value, with FOLD a negative default of an
// unsigned scalar folded to the value its bits hold, and any other default as
// written.  The scalar is the one ProjScalarArg names, so a nested member that
// wraps one scalar states its default in that scalar's terms.
//
// A field of type algo.SeqType, a u64, defaults to -1.  C++ converts
// that to the largest u64, while Go refuses -1 for a uint64 and Python's struct
// refuses it for Q, so Go, Python and Rust take the folded literal.  TypeScript
// writes an algo.SeqType through a signed word, where -1 is the value it holds,
// so it passes FOLD false.
tempstr amc::ProjDflt(amc::FField &field, bool fold DFLTVAL(true)) {
    tempstr ret;
    algo::strptr text = field.dflt.value;
    amc::FCtype &scalar = amc::ProjScalarArg(field);
    bool unsign = scalar.c_bltin && !scalar.c_bltin->issigned;
    if (fold && unsign && elems_N(text) > 1 && text.elems[0] == '-' && amc::ProjLiteralQ(text)) {
        u64 mag = 0;
        (void)u64_ReadStrptrMaybe(mag, algo::RestFrom(text, 1));
        u32 bits = scalar.totsize_byte * 8;
        u64 mask = bits >= 64 ? ~u64(0) : (u64(1) << bits) - 1;
        ret << ((u64(0) - mag) & mask);
    } else if (ProjProductQ(text)) {
        u64 value = 1;
        algo::StringIter iter(text);
        while (!iter.EofQ()) {
            u64 factor = 0;
            (void)u64_ReadStrptrMaybe(factor, algo::GetTokenChar(iter, '*'));
            value *= factor;
        }
        ret << value;
    } else {
        ret << text;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when TEXT is a default every projected language can state as a
// literal: a decimal or hex integer, a decimal fraction, a product of decimal
// integers, true, false, or a one-character quoted char.
bool amc::ProjLiteralQ(algo::strptr text) {
    bool hex = elems_N(text) > 2 && text.elems[0] == '0' && (text.elems[1] == 'x' || text.elems[1] == 'X');
    int start = hex ? 2 : (elems_N(text) > 1 && text.elems[0] == '-' ? 1 : 0);
    bool digits = elems_N(text) > start;
    for (int i = start; i < elems_N(text); i++) {
        char c = text.elems[i];
        digits = digits && (algo_lib::DigitCharQ(c) || (hex && algo_lib::HexCharQ(c)));
    }
    bool quoted = elems_N(text) == 3 && text.elems[0] == '\'' && text.elems[2] == '\'';
    // a decimal fraction, such as 0.5, is a float literal in every language
    int dot = algo::FindChar(text, '.');
    bool fraction = !hex && dot > start && dot < elems_N(text) - 1;
    for (int i = start; fraction && i < elems_N(text); i++) {
        fraction = i == dot || algo_lib::DigitCharQ(text.elems[i]);
    }
    return digits || fraction || quoted || ProjProductQ(text) || text == "true" || text == "false";
}

// -----------------------------------------------------------------------------

// Return TRUE when the default of FIELD is its type's zero value, which every
// projected language starts a member at.
bool amc::ProjZeroDfltQ(amc::FField &field) {
    algo::strptr dflt = field.dflt.value;
    return ch_N(dflt) == 0 || dflt == "\"\"" || dflt == "0" || dflt == "false";
}

// -----------------------------------------------------------------------------

// Return the ctype the value of FIELD is stated in: its own type, or the one
// field of that type when the type wraps a single scalar.  A bitfield's value is
// read as this ctype, and a nested member's own default is stated in it.
amc::FCtype &amc::ProjScalarArg(amc::FField &field) {
    amc::FCtype *ret = field.p_arg;
    if (!ret->c_bltin && amc::c_field_N(*ret) == 1) {
        ret = amc::c_field_Find(*ret, 0)->p_arg;
    }
    return *ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when FIELD is a nested member with a default of its own, which a
// projection states on the one scalar its ctype wraps, as C++ does.
bool amc::ProjNestedDfltQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Val && !field.p_arg->c_bltin && !amc::ProjZeroDfltQ(field);
}

// -----------------------------------------------------------------------------

// Return the member of the ctype nested member FIELD has, which the field's own
// default is stated on.  FIELD satisfies ProjNestedDfltQ, and gen_check_proj has
// admitted it, so its ctype wraps one scalar.
amc::FField &amc::ProjNestedDfltField(amc::FField &field) {
    return *amc::c_field_Find(*field.p_arg, 0);
}

// -----------------------------------------------------------------------------

// Return TRUE when a ctype of namespace NS projected into LANG has a member typed
// by a ctype of namespace DEP, so NS's code in LANG imports DEP's.
bool amc::ProjNsDepQ(amc::FNs &ns, amc::FNs &dep, amc::FLang &lang) {
    bool ret = false;
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) if (&dep != &ns && amc::ProjCtypeQ(ctype, lang)) {
        ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
            ret = ret || (amc::ProjNestedQ(field) && field.p_arg->p_ns == &dep);
        }ind_end;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every member of CTYPE whose default a projected language cannot state,
// and return TRUE when there is none.  A ctype whose members start at their
// schema defaults -- one with a codec or a layout -- needs each default as a
// literal, or a nested ctype that starts at its own.  A nested member with a
// default of its own needs a ctype that wraps one scalar, which the literal is
// stated on.
static bool ProjCheckDflt(amc::FCtype &ctype) {
    bool ret = true;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        bool nested = field.reftype == dmmeta_Reftype_reftype_Val && !field.p_arg->c_bltin;
        bool literal = (field.reftype == dmmeta_Reftype_reftype_Val || field.reftype == dmmeta_Reftype_reftype_Inlary)
            && amc::ProjLiteralQ(field.dflt.value);
        bool wrap = &amc::ProjScalarArg(field) != field.p_arg;
        bool zero = amc::ProjZeroDfltQ(field);
        bool ok = nested ? zero || (wrap && literal) : zero || literal;
        if (!ok) {
            algo::strptr why = nested && !wrap
                ? strptr("a nested member's own default needs a ctype that wraps one scalar")
                : strptr("default has no literal form in a projected language");
            prerr("amc.proj_dflt"
                  <<Keyval("field",field.field)
                  <<Keyval("dflt",field.dflt.value)
                  <<Keyval("comment",why));
            ret = false;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every reason CTYPE has no wire form a projected codec can lay out, and
// return TRUE when there is none.  Consider a message whose inline array of char
// sits ahead of a u32: a codec with no form for the array would leave its offset
// where the array began, and the u32 would be read from the array's first bytes.
// The frame decodes and the values are wrong.  So a field no codec can place
// stops the run, before any language writes a line, and every offending field
// is named in one pass.
static bool ProjCheckCtype(amc::FCtype &ctype) {
    bool ret = true;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (amc::ProjSlotBytes(field) < 0) {
            prerr("amc.proj_wire"
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",field.reftype)
                  <<Keyval("arg",field.arg)
                  <<Keyval("comment","field of a projected ctype has no wire form"));
            ret = false;
        }
    }ind_end;
    if (amc::ProjAnyTailQ(ctype) && !(ctype.c_msgtype && amc::LengthField(ctype))) {
        prerr("amc.proj_tail"
              <<Keyval("ctype",ctype.ctype)
              <<Keyval("comment","a ctype with a tail must be a message with a length field that frames it"));
        ret = false;
    }
    // Every codec stores and reads the frame length at the length field's own
    // slot.  A length field that is a bitfield of a header word owns no slot, so
    // a codec would write the length over the next member and read it back from
    // there.
    amc::FField *lenfield = amc::LengthField(ctype);
    if (ctype.c_msgtype && lenfield && lenfield->reftype != dmmeta_Reftype_reftype_Val) {
        prerr("amc.proj_lenfld"
              <<Keyval("field",lenfield->field)
              <<Keyval("reftype",lenfield->reftype)
              <<Keyval("comment","the length field of a projected message must be a Val with a slot of its own"));
        ret = false;
    }
    if (ret && ProjFixedBytes(ctype) != i32(ctype.totsize_byte)) {
        prerr("amc.proj_layout"
              <<Keyval("ctype",ctype.ctype)
              <<Keyval("totsize",ctype.totsize_byte)
              <<Keyval("projsize",ProjFixedBytes(ctype))
              <<Keyval("comment","C++ layout has padding a projected codec does not model; pack the ctype"));
        ret = false;
    }
    ret = ProjCheckDflt(ctype) && ret;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_bitfld) {
        if (!amc::ProjScalarQ(amc::ProjScalarArg(field))) {
            prerr("amc.proj_bitfld"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","bitfield type has no scalar wire form"));
            ret = false;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every field of unpacked CTYPE a type-only projection cannot name, and
// return TRUE when there is none.  A type-only member is a scalar, a string, a
// key into another table, a float, or a projected ctype.  Say a ctype holds a pointer:
// its value is an address in this process, which no other program can read, so
// a language that declared the member would carry a number that means nothing.
// An unpacked ctype with a layout also starts at its defaults, so each must be a
// literal.
static bool ProjCheckType(amc::FCtype &ctype) {
    bool ret = true;
    if (amc::ProjLayoutQ(ctype)) {
        ret = ProjCheckDflt(ctype);
    }
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        bool scalar = field.p_arg->c_bltin && (amc::ProjScalarQ(*field.p_arg) || amc::ProjFloatQ(*field.p_arg));
        bool nested = !field.p_arg->c_bltin;
        bool val = field.reftype == dmmeta_Reftype_reftype_Val && (scalar || nested);
        if (!amc::ProjStringQ(field) && !val) {
            prerr("amc.proj_type"
                  <<Keyval("field",field.field)
                  <<Keyval("reftype",field.reftype)
                  <<Keyval("arg",field.arg)
                  <<Keyval("comment","field of a projected ctype has no type in a projected language"));
            ret = false;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every message of DISPLANG's dispatch the projection does not hold in
// DISPLANG's language, and return TRUE when there is none.  The signature
// constant is computed over every message of the dispatch, so a language that
// claims the signature has to be able to spell each message it covers.
static bool ProjCheckDisplang(amc::FDisplang &displang) {
    bool ret = true;
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg, *displang.p_dispatch) {
        if (!amc::ProjCtypeQ(*dispatch_msg.p_ctype, *displang.p_lang)) {
            prerr("amc.displang_msg"
                  <<Keyval("displang",displang.displang)
                  <<Keyval("ctype",dispatch_msg.p_ctype->ctype)
                  <<Keyval("comment","message of a projected dispatch is not projected; add an nslang or ctypelang row"));
            ret = false;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when CTYPE is projected into a language whose codec this file's
// wire form describes: every language but TypeScript, whose codec admits more
// field forms and is checked by gen_check_ts.
bool amc::ProjWireLangQ(amc::FCtype &ctype) {
    bool ret = false;
    amc::FLang *ts = amc::ind_lang_Find(amc::amcdb_lang_ts);
    ind_beg(amc::ctype_c_lang_curs, lang, ctype) {
        ret = ret || &lang != ts;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Check every projected ctype once, whichever languages it is projected into,
// and every displang row, and fail the run when one does not hold.  A packed
// ctype must have a byte-exact wire form in the languages this file describes,
// an unpacked one a type in every language, and a projected dispatch must speak
// only projected messages.
//
// Each language emitter walks the fields these checks admit, and trusts that
// every one of them has a form in its language.  A packed ctype with a pointer
// member is refused here, and an emitter that walked it anyway would reach a
// field with no wire form and crash.  So the run records, after the last check,
// whether any check refused the schema, and every emitter then writes nothing,
// which is also what ns_write does with whatever it is handed after an error.
void amc::gen_check_proj() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) if (amc::c_lang_N(ctype) > 0) {
        bool ok = true;
        if (!amc::ProjCodecQ(ctype)) {
            ok = ProjCheckType(ctype);
        } else if (amc::ProjWireLangQ(ctype)) {
            ok = ProjCheckCtype(ctype);
        }
        if (!ok) {
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    ind_beg(amc::_db_displang_curs, displang, amc::_db) {
        if (!ProjCheckDisplang(displang)) {
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    amc::_db.proj_refuse = algo_lib::_db.exit_code != 0;
}
