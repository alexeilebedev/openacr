// Copyright (C) 2023-2026 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/js.cpp
//

#include "include/algo.h"
#include "include/amc.h"

// add separator if lhs is not empty
void amc::MaybeSep(cstring &lhs, strptr sep DFLTVAL(", ")){
    if (ch_N(lhs)) {
        lhs << sep;
    }
}

// return TRUE if javscript output is requested for this ctype
bool amc::JsQ(amc::FCtype &ctype) {
    amc::FNsjs *nsjs = ctype.p_ns->c_nsjs;
    return nsjs && ctype.c_jstype;
}

// return TRUE if typescript output is requested for this ctype
bool amc::TsQ(amc::FCtype &ctype) {
    return JsQ(ctype) && ctype.p_ns->c_nsjs->typescript;
}

// Map FIELD to a JavaScript built-in type.
// The built-in type is Uint8Array, Array, Map, string, number, boolean, bigint, undefined, and object
// For unknown types, "object" is returned.
// From field and jsbltin, a JavaScript classname can be constructed with GetJsClass.
amc::FJsbltin *amc::GetJsBltin(amc::FField &field, bool json DFLTVAL(false)) {
    amc::FJsbltin *ret = &amc_jsbltin_undefined;
    if (field.p_arg->c_cstr || field.reftype == dmmeta_Reftype_reftype_Regx) {
        ret = &amc_jsbltin_string;
    } else if (field.reftype == dmmeta_Reftype_reftype_Val || field.reftype == dmmeta_Reftype_reftype_Bitfld) {
        if (json && field.p_arg->c_cjson) {
            // custom mapping to a JavaScript type in JSON context via cjson table
            ret = field.p_arg->c_cjson->p_jsbltin;
        } else if (field.p_arg->c_cjsbltin) {
            // custom mapping to a JavaScript built-in type in non-JSON context via cjs table
            ret = field.p_arg->c_cjsbltin->p_jsbltin;
        } else {
            ret = &amc_jsbltin_object;
        }
    } else if (field.reftype == dmmeta_Reftype_reftype_Base) {
        // nothing
    } else if (field.reftype == dmmeta_Reftype_reftype_Tary) {
        ret = &amc_jsbltin_Array;
    } else if (field.reftype == dmmeta_Reftype_reftype_Thash || field.reftype == dmmeta_Reftype_reftype_Blkhash) {
        ret = &amc_jsbltin_Map;
    } else if (field.reftype == dmmeta_Reftype_reftype_Ptr || field.reftype == dmmeta_Reftype_reftype_Upptr) {
        ret = &amc_jsbltin_object;
    } else if (field.reftype == dmmeta_Reftype_reftype_Varlen || field.reftype == dmmeta_Reftype_reftype_Opt) {
        ret = field.arg == "char" ? &amc_jsbltin_string : &amc_jsbltin_Uint8Array;
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
        ret = field.arg == "char" ? &amc_jsbltin_string : &amc_jsbltin_Uint8Array;
    }
    return ret;
}

algo::tempstr amc::GetJsClass(amc::FField &field, amc::FJsbltin *jsbltin) {
    tempstr ret;
    if (jsbltin->jsbltin == dmmeta_jsbltin_object) {
        ret << name_Get(*field.p_arg);
    } else {
        ret << jsbltin->jsbltin;
    }
    return ret;
}

algo::tempstr amc::GetJsClass(amc::FField &field, bool json DFLTVAL(false)) {
    return GetJsClass(field,GetJsBltin(field,json));
}

// String constructing specified javascript type from provided value
tempstr amc::JsDflt(amc::FField &field) {
    tempstr ret;
    amc::FJsbltin *jsbltin=GetJsBltin(field);
    if (jsbltin->cons == "new" && (field.reftype == dmmeta_Reftype_reftype_Val || field.reftype == dmmeta_Reftype_reftype_Inlary)) {
        // object type + val = create new object in constructor
        ret << "new "<< GetJsClass(field,jsbltin);
    } else if (field.dflt.value != "" && !(field.reftype == dmmeta_Reftype_reftype_Tary)) {
        // BigInt(<numeric literal>) rounds through a double and corrupts
        // values past 2^53; the string form parses the literal exactly.
        algo::strptr quote = jsbltin->cons == "BigInt" ? strptr("\"") : strptr("");
        ret << jsbltin->cons << "(" << quote << field.dflt << quote << ")";
    } else {
        ret << jsbltin->dflt;
    }
    return ret;
}

// String constructing specified javascript type from provided value
tempstr amc::JsCons(amc::FField &field, algo::strptr val) {
    tempstr ret;
    amc::FJsbltin *jsbltin=GetJsBltin(field);
    ret << jsbltin->cons;
    if (jsbltin->cons == "new") {
        ret << " "<<GetJsClass(field,jsbltin);
    }
    if (val != "") {
        ret << "(" << val << ")";
    }
    return ret;
}

// add import statement for name if not already in place
void amc::TsImportMaybe(amc::FField &field, strptr name) {
    amc::FJsbltin *jsbltin=GetJsBltin(field);
    if (!(jsbltin->jsbltin == dmmeta_jsbltin_object)) {
        // not an object
    } else if (field.p_arg->p_ns == field.p_ctype->p_ns) {
        // same namespace, don't import
    } else {
        auto &R = _db.genctx.R;
        Set(R,"$symbol",name);
        Set(R,"$fromns",field.p_arg->p_ns->ns);
        tempstr imports;
        Ins(&R,imports,"import { $symbol } from \"./$fromns_gen\"");
        if (FindStr(field.p_ctype->p_ns->c_nsjs->imports,imports)==-1) {
            field.p_ctype->p_ns->c_nsjs->imports << imports;
        }
    }
}

// Return a javscript expression evaluating NAME, which is of type CTYPE
// to a number
tempstr amc::JsGetnumExpr(amc::FCtype &ctype, algo::strptr name) {
    tempstr ret;
    if (ctype.c_cjsbltin && ctype.c_cjsbltin->jsbltin == dmmeta_jsbltin_number) {
        ret = name;
    } else {
        ind_beg(ctype_c_field_curs,field,ctype) {
            ret << name << "." << name_Get(field);
        }ind_end;
    }
    return ret;
}

// Whether FIELD's javascript representation is bigint (u64/i64, or an arg
// whose declared jsbltin is bigint) rather than a plain number -- the same
// test TsSerdeSimpleType uses to pick getBigUint64/getBigInt64.
static bool JsBigintFieldQ(amc::FField &field) {
    bool ret = field.arg == "u64" || field.arg == "i64";
    if (!ret && field.p_arg->c_cjsbltin) {
        ret = field.p_arg->c_cjsbltin->p_jsbltin == &amc_jsbltin_bigint;
    }
    return ret;
}

// A varlen frame's length and its end offsets are computed as JS numbers:
// they come from byte counts the encoder accumulates and from buffer offsets
// the decoder indexes with.  The word holding them on the wire may be a u64,
// which DataView writes with setBigUint64 and reads with getBigUint64, and JS
// refuses to mix bigint with number in arithmetic or to index a Uint8Array
// with one.  Storing the frame as bigint throughout would push that conversion
// into every offset expression; the conversion belongs at the accessor
// instead, which is the one place the two representations meet.  The frame
// stays number everywhere else.  Narrowing is exact: a frame length is bounded
// by the buffer holding it, so it is always inside 2^53.

// Widen NUMEXPR for storing into LENFIELD, whose wire word may be bigint.
static tempstr TsFrameStoreExpr(amc::FField &lenfield, algo::strptr numexpr) {
    tempstr ret;
    if (JsBigintFieldQ(lenfield)) {
        ret << "BigInt(" << numexpr << ")";
    } else {
        ret << numexpr;
    }
    return ret;
}

// Destination the accessor reads a frame word into.  A bigint wire word lands
// in a temporary, because the assignment itself is what the typechecker
// rejects: narrowing after the fact still stores a bigint into a number.
// DECLARE is the keyword the number path needs in front of LOCAL, empty when
// the local already exists; the bigint path always declares its temporary.
static tempstr TsFrameLoadDest(amc::FField &lenfield, algo::strptr declare, algo::strptr local) {
    tempstr ret;
    if (JsBigintFieldQ(lenfield)) {
        ret << "let _w_" << local;
    } else {
        ret << declare << local;
    }
    return ret;
}

void amc::JsGenBitfld() {
    auto &field = *_db.genctx.p_field;
    auto &R = _db.genctx.R;
    if (amc::FJstype *jstype = field.p_ctype->c_jstype) {
        // A bigint-backed source field (e.g. a u64 coordinate) needs BigInt
        // literals and operators throughout: JS bitwise ops reject mixing
        // bigint and number, and BigInt has no unsigned right shift.
        bool bigsrc = JsBigintFieldQ(*field.c_bitfld->p_srcfield);
        strptr suffix = bigsrc ? strptr("n") : strptr("");
        strptr shiftop = bigsrc ? strptr(">>") : strptr(">>>");
        tempstr xexpr(bigsrc ? strptr("BigInt(x)") : strptr("+x"));
        cstring bitmask("0b");
        char_PrintNTimes('1',bitmask,field.c_bitfld->width);
        char_PrintNTimes('0',bitmask,field.c_bitfld->offset);
        bitmask << suffix;
        tempstr bitoffset;
        bitoffset << field.c_bitfld->offset << suffix;
        Set(R,"$srcname",name_Get(*field.c_bitfld->p_srcfield));
        Set(R,"$bitoffset",bitoffset);
        Set(R,"$bitmask",bitmask);
        Set(R,"$jsclass",GetJsClass(field));
        Set(R,"$shiftop",shiftop);
        Set(R,"$xexpr",xexpr);
        Set(R,"$cons",JsCons(field,"(this.$srcname & $bitmask) $shiftop $bitoffset"));
        Ins(&R,jstype->body,"$comment");
        Ins(&R,jstype->body,"get $name(): $jsclass {");
        Ins(&R,jstype->body,"    return $cons;");
        Ins(&R,jstype->body,"}");
        Ins(&R,jstype->body,"set $name(x: number) {");
        Ins(&R,jstype->body,"    this.$srcname ^= $bitmask & this.$srcname;");
        Ins(&R,jstype->body,"    this.$srcname |= ($xexpr << $bitoffset) & $bitmask;");
        Ins(&R,jstype->body,"}");
    }
}

void amc::TsGenEnum() {
    auto &field = *_db.genctx.p_field;
    auto &R = _db.genctx.R;
    amc::FNsjs &nsjs = *_db.genctx.p_ns->c_nsjs;
    Ins(&R,nsjs.filebody,"");
    if (field.c_anonfld) {
        Ins(&R,nsjs.filebody,"export enum $ctypeEnum {");
    } else {
        Ins(&R,nsjs.filebody,"export enum $ctype_$name_Enum {");
    }
    ind_beg(field_c_fconst_curs,fconst,field) {
        tempstr fcname(strptr_ToCppIdent(name_Get(fconst),true));
        Set(R,"$fcname",fcname);
        Set(R,"$value",tempstr()<<fconst.value);
        Ins(&R,nsjs.filebody,"   $fcname = $value,");
    }ind_end;
    Ins(&R,nsjs.filebody,"}");
}

void amc::JsGenField() {
    auto &field = *_db.genctx.p_field;
    auto &R = _db.genctx.R;
    amc::FJstype *jstype = field.p_ctype->c_jstype;
    Set(R,"$ctype",name_Get(*field.p_ctype));
    Set(R,"$name",name_Get(field));
    Set(R,"$jsclass",GetJsClass(field));
    Set(R,"$dflt",JsDflt(field));
    prcat(verbose2,"amc.jsgenfield"
          <<Keyval("field",field.field)
          <<Keyval("field.dflt",field.dflt)
          <<Keyval("ctype",Subst(R,"$ctype"))
          <<Keyval("name",Subst(R,"$name"))
          <<Keyval("jsclass",Subst(R,"$jsclass"))
          <<Keyval("dflt",Subst(R,"$dflt")));
    tempstr comment;
    if (ch_N(field.comment)) {
        comment << " // " << field.comment;
    }
    Set(R,"$comment",comment);
    if (field.reftype == dmmeta_Reftype_reftype_Base) {
        // ignore - injection point
    } else if (field.c_bitfld) {
        // bitfields are not initialized
    } else if (field.p_ctype->c_msgtype ? (!field.c_typefld && !field.c_lenfld) : true) {
        if (TsQ(*field.p_ctype)) {// typescript
            MaybeSep(jstype->args);
            Ins(&R,jstype->args,"$name?: $jsclass",false);
            Ins(&R,jstype->body,"$name: $jsclass;$comment");
            Ins(&R,jstype->ctor,"this.$name = $name !== undefined ? $name : $dflt;");
            TsImportMaybe(field,GetJsClass(field));
        } else {
            Ins(&R,jstype->ctor,"this.$name = $dflt;$comment");// javscript
        }
    }
    if (field.reftype == dmmeta_Reftype_reftype_Bitfld && field.c_bitfld) {
        JsGenBitfld();
    }
    if (c_fconst_N(field) && !field.c_bitfld) {
        TsGenEnum();
    }
}

void amc::gen_nsjs_field() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nsjs) {
        amc::_db.genctx.p_ns = &ns;
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (ctype.c_jstype) {
            amc::_db.genctx.p_ctype = &ctype;
            ind_beg(amc::ctype_c_field_curs, field,ctype) {
                amc::_db.genctx.p_field = &field;
                ResetVars(amc::_db.genctx);
                JsGenField();
            }ind_end;
            amc::_db.genctx.p_ctype = NULL;
            amc::_db.genctx.p_field = NULL;
        }ind_end;
    }ind_end;
}

// Return the TS numerator of LENFLD's store formula when the frame occupies
// `length` bytes: length + extra.  The stored value is that numerator over
// scale -- the exact inverse of the reader formula (LengthExpr: total =
// stored*scale - extra) -- and both the store expression and the guards
// testing it are written in terms of it.
static tempstr TsLenfldNumExpr(amc::FLenfld &lenfld) {
    tempstr ret;
    if (lenfld.extra > 0) {
        ret << "(length + " << lenfld.extra << ")";
    } else if (lenfld.extra < 0) {
        ret << "(length - " << -lenfld.extra << ")";
    } else {
        ret << "length";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Emit into OUT the TS guards refusing a frame byte count LENFLD cannot store.
// Consider a u8 length word framing a message whose payload the caller fills
// from a text box: at 260 bytes the store writes 260 mod 256 = 4, and the
// reader frames 4 bytes and takes the remaining 256 as the next message's
// header.  The frame was sent, no error was raised, and the stream desynchronizes
// from that point on.  Every store of a runtime total has this shape -- the
// wire word is narrower than the count the encoder computed, or scaled so that
// only some counts round-trip, or offset so that a small count goes negative --
// and in each case the truncation is silent and the damage is at the reader.
// So the total is tested before it is stored, and the frame is refused with a
// RangeError naming which bound it broke.  A ctype whose frame length is fixed
// needs no test: gen_check_lenfld proves that total storable at generation
// time, so only RuntimeFrameLenQ ctypes are guarded here.  The three terms
// mirror LenfldCheckExpr, each emitted only where its failure is possible.
// The upper bound is the frame-length domain (FrameLenMax) narrowed by the
// length word's own range -- the same bound the C++ emitters refuse at.  A JS
// byte count is bounded only by 2^53, so this encoder could hold a total the
// domain excludes; a frame it produces has to be one a C++ reader can
// reconstruct, and that reader works in the domain.
static void TsLenfldCheckStmt(algo_lib::Replscope &R, cstring &out, amc::FLenfld &lenfld) {
    if (amc::RuntimeFrameLenQ(*lenfld.p_field->p_ctype)) {
        if (amc::LenfldLowGuardNeededQ(lenfld)) {
            Set(R,"$minlen",tempstr() << -lenfld.extra);
            Ins(&R,out,"    if (length < $minlen) {");
            Ins(&R,out,"        throw new RangeError(\"$ctype_Encode: frame length \" + length + \" falls below the length field range (min $minlen)\");");
            Ins(&R,out,"    }");
        }
        if (lenfld.scale != 1) {
            Set(R,"$numexpr",TsLenfldNumExpr(lenfld));
            Set(R,"$scale",tempstr() << lenfld.scale);
            Ins(&R,out,"    if ($numexpr % $scale !== 0) {");
            Ins(&R,out,"        throw new RangeError(\"$ctype_Encode: frame length \" + length + \" is not representable in the length field (scale $scale)\");");
            Ins(&R,out,"    }");
        }
        u64 maxlen = u64_Min(amc::LenfldMaxLen(lenfld), amc::FrameLenMax());
        Set(R,"$maxlen",tempstr() << maxlen);
        Ins(&R,out,"    if (length > $maxlen) {");
        Ins(&R,out,"        throw new RangeError(\"$ctype_Encode: frame length \" + length + \" exceeds the frame length range (max $maxlen)\");");
        Ins(&R,out,"    }");
    }
}

// -----------------------------------------------------------------------------

// Return the TS expression for the value stored in LENFLD when the frame
// occupies `length` bytes: (length + extra) / scale.  `length` itself remains
// the true frame byte count and is what the encoder returns.  The expression
// is exact only for a total TsLenfldCheckStmt has already admitted.
static tempstr TsLenfldStoreExpr(amc::FLenfld &lenfld) {
    tempstr ret;
    ret << TsLenfldNumExpr(lenfld);
    if (lenfld.scale != 1) {
        ret << " / " << lenfld.scale;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Emit into OUT the TS statements expanding the stored lenfld value held in
// `length` to the frame byte count (LengthExpr: total = stored*scale - extra).
static void TsLenfldLengthStmt(algo_lib::Replscope &R, cstring &out, amc::FLenfld &lenfld) {
    if (lenfld.scale != 1) {
        Set(R,"$scale", tempstr() << lenfld.scale);
        Ins(&R,out,"    length *= $scale;");
    }
    if (lenfld.extra > 0) {
        Set(R,"$extra", tempstr() << lenfld.extra);
        Ins(&R,out,"    length -= $extra;");
    } else if (lenfld.extra < 0) {
        Set(R,"$extra", tempstr() << -lenfld.extra);
        Ins(&R,out,"    length += $extra;");
    }
}

// -----------------------------------------------------------------------------

// Emit into OUT the TS statements appending the byte-array field $name to the
// frame tail: guard that the bytes fit the view's remaining window, then write
// them at the buffer-absolute rebase of the struct-relative position `length`
// (a Uint8Array over view.buffer is buffer-absolute, so the position rebases
// by the view's byteOffset plus the struct's offset).  The guard throws the
// same named RangeError shape as the string branch, and the window is bounded
// by the view's byteLength: a frame encoded into a DataView that is a slice
// of a larger buffer (a staging buffer holding several frames) must overflow
// into an error naming the message and field, not into the adjacent frame's
// bytes.  Shared by the raw-byte varlen and Opt branches.
static void TsEncodeByteTail(algo_lib::Replscope &R, cstring &out) {
    Ins(&R,out,"    if (parent.$name.length > view.byteLength - (offset + length)) {");
    Ins(&R,out,"        throw new RangeError(\"$ctype_Encode: $name does not fit in the target buffer\");");
    Ins(&R,out,"    }");
    Ins(&R,out,"    new Uint8Array(view.buffer,view.byteOffset+offset+length,view.byteLength-(offset+length)).set(parent.$name);");
    Ins(&R,out,"    length += parent.$name.length;");
}

// -----------------------------------------------------------------------------

// Bytes FIELD occupies in the fixed-size region of its ctype's wire form:
// 0 for a field that occupies no fixed slot, and -1 for a field with no wire
// form at all.  This is where a field's wire form is decided, once: the
// encoder and the decoder read the slot width from here rather than each
// computing its own, so a field cannot sit at one offset in $ctype_Encode and
// another in $ctype_Decode, and cannot be written by one and skipped by the
// other.  gen_check_jstype rejects the schema whose field lands on -1.
//
// The cases with no slot each have their bytes somewhere else: a Base is an
// injection point whose fields appear individually, a Bitfld is computed from
// the source word that does have a slot, and a Varlen or Opt field's bytes
// are the frame's tail, past every fixed slot.
//
// Opt and Inlary are byte copies in both directions -- the encoder fills a
// Uint8Array window from the member, and the decoder hands the member a
// Uint8Array window -- so each maps to the wire only when the member's own JS
// type is Uint8Array.  A member whose JS type is `string`, which is what an
// arg of char or of a cstr ctype produces, has no byte-copy form in either
// direction, and is refused rather than encoded one way and decoded the
// other.
//
// A Varlen tail has a text form as well as a byte form, so it takes either JS
// type -- but the text form is what the arg char means, and a cstr arg such
// as algo.Smallstr50 projects the member as `string` while its tail on the
// wire is a run of that ctype's records rather than encoded text.  So Varlen
// admits the byte-array member and the char arg, and refuses the remaining
// combination; the encoder and the decoder then branch on the member's JS
// type and reach the same answer.
//
// A Val slot is the arg's whole wire form only while the arg's own wire form
// is entirely fixed.  An arg that carries a Varlen or an Opt appends a tail
// past its fixed size, and the nested encoder returns a length the outer walk
// has no slot to account for, so the outer walk resumes on top of the nested
// tail and the outer length word undercounts the frame.  Such a field is
// refused; the shape it wants is an Opt, whose bytes are the outer frame's
// own tail.
//
// An Inlary slot is `max` elements wide only while the array is fixed
// (min == max), which is the shape the C++ side lays out as a bare element
// array.  A variable inlary lays out an element block followed by a live
// element count, so its C++ footprint exceeds `max` elements and the count
// word has no JS member to carry it; such a field is refused rather than
// placed at a width that would slide every later field.  The accept and
// reject rows of this whole table are pinned by the amc.BadJstypeWire
// comptest.
static int TsFixedSlotBytes(amc::FField &field) {
    int ret = -1;
    bool bytecopy = amc::GetJsBltin(field)->jsbltin == amc::dmmeta_jsbltin_Uint8Array;
    bool argtail = amc::RuntimeFrameLenQ(*field.p_arg);
    if (field.reftype == dmmeta_Reftype_reftype_Base
        || field.reftype == dmmeta_Reftype_reftype_Bitfld) {
        ret = 0;
    } else if (field.reftype == dmmeta_Reftype_reftype_Varlen && (bytecopy || field.arg == "char")) {
        ret = 0;
    } else if (field.reftype == dmmeta_Reftype_reftype_Opt && bytecopy) {
        ret = 0;
    } else if (field.reftype == dmmeta_Reftype_reftype_Val && !argtail) {
        ret = field.p_arg->totsize_byte;
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary && bytecopy && amc::FixaryQ(field)) {
        ret = field.c_inlary->max * field.p_arg->totsize_byte;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Refuse a jstype'd packed ctype carrying a field with no TypeScript wire
// form, naming every such field rather than stopping at the first.
//
// Consider a message whose second field is an inline array of char followed
// by a u32 sequence number.  The array has no byte-copy wire form, so the
// generated decoder reads nothing for it -- and, reading nothing, leaves the
// running offset where the array began, so the sequence number is read from
// the array's first four bytes and every later field slides with it.  The
// frame decodes, no error is raised, and the values are wrong.
//
// What makes that possible is a field admitted into the layout with no wire
// form to give it.  So the schema is refused here, before any code is
// generated: TsFixedSlotBytes is the one place a field's wire form is
// decided, and a field it cannot place is a schema error.  Reporting rather
// than throwing is what lets one run name every offending field; amc writes
// no file at all once a generation error is recorded, so a refused schema
// yields no encoder and no decoder rather than a silently wrong one.
//
// The ctype as a whole is checked for one thing the same way.  A Varlen or Opt
// field's bytes are the frame's tail, and the decoder finds where that tail
// ends by reading the frame's length word out of the ctype's length field.  A
// ctype that declares a tail and no length field leaves the decoder with no
// length to read, so it is refused as amc.jstype_lenfld.
void amc::gen_check_jstype() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) if (amc::TsQ(ctype) && ctype.c_pack) {
        if (amc::RuntimeFrameLenQ(ctype) && !amc::LengthField(ctype)) {
            prerr("amc.jstype_lenfld"
                  <<Keyval("ctype",ctype.ctype)
                  <<Keyval("comment","ctype has a varlen or Opt tail and no length field to frame it"));
            algo_lib::_db.exit_code++;
        }
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            if (TsFixedSlotBytes(field) < 0) {
                prerr("amc.jstype_wire"
                      <<Keyval("field",field.field)
                      <<Keyval("reftype",field.reftype)
                      <<Keyval("jsclass",amc::GetJsClass(field))
                      <<Keyval("comment","field has no TypeScript wire form; it can be neither encoded nor decoded"));
                algo_lib::_db.exit_code++;
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Emit into OUT the statement carrying FIELD's fixed slot in the ENCODE or
// the decode direction, at the $offset the caller has set.
// A Base injection, a Bitfld slice and a Varlen tail carry no statement here.
// An Opt tail carries one on the decode side only: the encoder appends its
// bytes before this walk runs, while the decoder recovers them as the window
// from the field's offset to the end of the frame.
static void TsGenFixedField(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype, amc::FField &field, bool encode) {
    tempstr jsclass = amc::GetJsClass(field,false);
    if (field.reftype == dmmeta_Reftype_reftype_Val) {
        bool lenfld = field.c_lenfld != NULL;
        bool typefld = field.c_typefld && ctype.c_msgtype;
        bool wideword = lenfld && JsBigintFieldQ(field);
        // A message ctype's type word is not a member of its TypeScript class:
        // the word selects which decoder runs, so a decoded record that carried
        // it would be restating what the caller had to know to decode at all.
        // The encoder writes the constant, and the decoder reads nothing --
        // the slot still advances the offset, which is what TsFixedSlotBytes
        // answers and neither direction decides for itself.
        bool skip = !encode && typefld;
        if (!encode && lenfld) {
            Ins(&R,out,"    let length = 0;");
            Set(R,"$value",TsFrameLoadDest(field,"","length"));
        } else if (!encode) {
            Set(R,"$value","parent.$name");
        } else if (typefld) {
            Set(R,"$value",tempstr()<<ctype.c_msgtype->type);
        } else if (lenfld) {
            TsLenfldCheckStmt(R,out,*field.c_lenfld);
            Set(R,"$value",TsFrameStoreExpr(field,TsLenfldStoreExpr(*field.c_lenfld)));
        } else {
            Set(R,"$value","parent.$name");
        }
        if (!skip) {
            amc::TsSerdeSimpleType(R,out,field,encode);
        }
        if (!encode && lenfld) {
            if (wideword) {
                Ins(&R,out,"    length = Number(_w_length);");
            }
            // A ctype with no msgtype carries its length word as an ordinary
            // member, so a caller that decodes a frame and re-encodes it reads
            // that member back. Left unwritten it holds the constructor's zero,
            // which prints as a zero-word frame and compares unequal to the
            // record that produced it. The member mirrors the wire word, so it
            // is assigned before TsLenfldLengthStmt expands `length` to the
            // byte total.
            if (!ctype.c_msgtype) {
                Set(R,"$word",wideword ? "_w_length" : "length");
                Ins(&R,out,"    parent.$name = $word;");
            }
            TsLenfldLengthStmt(R,out,*field.c_lenfld);
        }
    } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        if (!encode) {
            Ins(&R,out,"    parent.$name = new Uint8Array(view.buffer,view.byteOffset+$offset,length-$slotoffset);");
        }
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
        Set(R,"$nbyte",tempstr() << TsFixedSlotBytes(field));
        if (encode) {
            // fixed-size storage at the field's own offset (part of
            // totsize_byte, so it contributes nothing to `length`); zero-fill
            // so a short source still yields deterministic wire bytes
            Ins(&R,out,"    let _a_$name = new Uint8Array(view.buffer,view.byteOffset+$offset,$nbyte);");
            Ins(&R,out,"    _a_$name.fill(0);");
            Ins(&R,out,"    _a_$name.set(parent.$name);");
        } else {
            Ins(&R,out,"    parent.$name = new Uint8Array(view.buffer,view.byteOffset+$offset,$nbyte);");
        }
    }
}

// -----------------------------------------------------------------------------

// Emit into OUT the statements for CTYPE's fixed portion in the ENCODE or the
// decode direction, and return the byte offset just past it.
// The fixed portion is one slot per field, in schema order, followed by the
// end offset of every varlen tail but the last. Both directions are emitted
// from this one walk, so a field's offset and its wire case are derived once
// and an encoder and a decoder cannot disagree about where a field sits.
// Every slot width here is a width gen_check_jstype has already admitted.
//
// A tail's end offset is stored in a word typed by the ctype's length field,
// so a ctype that declares a tail and no length field has no word to store it
// in. That ctype is already a recorded rejection -- gen_check_jstype draws
// amc.jstype_lenfld on it -- but a recorded rejection does not stop the walk
// from running, so the walk has to answer for the case itself rather than
// read a length field that is not there.
static int TsGenFixedWalk(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype, bool encode) {
    int offset(0);
    amc::FField *lenfield = amc::LengthField(ctype);
    ind_beg(amc::ctype_c_field_curs,field,ctype) {
        Set(R,"$name",name_Get(field));
        Set(R,"$offset",tempstr()<<"offset+"<<offset);
        Set(R,"$slotoffset",tempstr()<<offset);
        Set(R,"$lend",field.c_fbigend?"false":"true");
        TsGenFixedField(R,out,ctype,field,encode);
        offset += TsFixedSlotBytes(field);
    }ind_end;
    if (lenfield) {
        ind_beg(amc::ctype_zd_varlenfld_curs,field,ctype) {
            if (amc::ctype_zd_varlenfld_Next(field)) {
                Set(R,"$name",name_Get(field));
                Set(R,"$offset",tempstr()<<"offset+"<<offset);
                Set(R,"$lend",lenfield->c_fbigend?"false":"true");
                if (encode) {
                    Set(R,"$value",TsFrameStoreExpr(*lenfield,"$name_end"));
                } else {
                    Set(R,"$value",TsFrameLoadDest(*lenfield,"let ","$name_end"));
                }
                amc::TsSerdeSimpleType(R,out,*lenfield,encode);
                if (!encode && JsBigintFieldQ(*lenfield)) {
                    Ins(&R,out,"    let $name_end = Number(_w_$name_end);");
                }
                offset += lenfield->p_arg->totsize_byte;
            }
        }ind_end;
    }
    return offset;
}

// -----------------------------------------------------------------------------

// Generate $ctype_Encode: write PARENT's wire form into VIEW at OFFSET --
// the varlen and Opt tails first (establishing the frame length), then the
// fixed-size fields; returns the frame byte count.
void amc::TsGenEncode() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    if (amc::FJstype *jstype = ctype.c_jstype) {
        Ins(&R,jstype->funcs,"");
        Ins(&R,jstype->funcs,"export function $ctype_Encode(view: DataView, offset: number, parent: $ctype) {");
        Set(R,"$csize", tempstr() << ctype.totsize_byte);
        Ins(&R,jstype->funcs,"    let length = $csize;");
        // encode varlen strings
        ind_beg(ctype_zd_varlenfld_curs,field,ctype) {
            tempstr jsclass = GetJsClass(field,false);
            Set(R,"$name",name_Get(field));
            // write byte array at the end of the struct (see TsEncodeByteTail)
            if (jsclass == dmmeta_jsbltin_Uint8Array) {
                TsEncodeByteTail(R,jstype->funcs);
            } else if (jsclass == dmmeta_jsbltin_string) {
                // encodeInto never throws on exhaustion: it fills the window and
                // reports {read,written}, so an oversized string would be cut
                // mid-encode and the length word would frame the truncated content
                // as a well-formed message (a stream open then names a different,
                // shorter path with no client-side error).  read < source length
                // detects the cut; throw RangeError like the raw-byte .set path.
                Ins(&R,jstype->funcs,"    let _e_$name = _g_text_encoder.encodeInto(parent.$name,new Uint8Array(view.buffer,view.byteOffset+offset+length,view.byteLength-(offset+length)));");
                Ins(&R,jstype->funcs,"    if (_e_$name.read < parent.$name.length) {");
                Ins(&R,jstype->funcs,"        throw new RangeError(\"$ctype_Encode: $name does not fit in the target buffer\");");
                Ins(&R,jstype->funcs,"    }");
                Ins(&R,jstype->funcs,"    length += _e_$name.written;");
            }
            if (ctype_zd_varlenfld_Next(field)) {
                Ins(&R,jstype->funcs,"    let $name_end = length - $csize;");
            }
        }ind_end;
        // encode binary varlen portion
        ind_beg(ctype_c_field_curs,field,ctype) {
            if (field.reftype == dmmeta_Reftype_reftype_Opt) {
                Set(R,"$name",name_Get(field));
                TsEncodeByteTail(R,jstype->funcs);
            }
        }ind_end;
        TsGenFixedWalk(R,jstype->funcs,ctype,true);
        Ins(&R,jstype->funcs,"    return length;");
        Ins(&R,jstype->funcs,"}");
    }
}

// Emit into OUT the TS statement writing (ENCODE) or reading (!ENCODE)
// FIELD's fixed-size wire form at $offset: DataView get/set for scalars,
// a NUL-padded byte window for a fixed-length string, and a nested
// $jsclass_Encode/_Decode call for a ctype-valued field.
void amc::TsSerdeSimpleType(algo_lib::Replscope &R, cstring &out, amc::FField &field, bool encode) {
    if (field.arg == "u8") {
        if (encode) {
            Ins(&R,out,"    view.setUint8($offset,$value);");
        } else {
            Ins(&R,out,"    $value = view.getUint8($offset);");
        }
    } else if (field.arg == "u16") {
        if (encode) {
            Ins(&R,out,"    view.setUint16($offset,$value,$lend);");
        } else {
            Ins(&R,out,"    $value = view.getUint16($offset,$lend);");
        }
    } else if (field.arg == "u32") {
        if (encode) {
            Ins(&R,out,"    view.setUint32($offset,$value,$lend);");
        } else {
            Ins(&R,out,"    $value = view.getUint32($offset,$lend);");
        }
    } else if (field.arg == "u64") {
        if (encode) {
            Ins(&R,out,"    view.setBigUint64($offset,$value,$lend);");
        } else {
            Ins(&R,out,"    $value = view.getBigUint64($offset,$lend);");
        }
    } else if (field.arg == "i8") {
        if (encode) {
            Ins(&R,out,"    view.setInt8($offset,$value);");
        } else {
            Ins(&R,out,"    $value = view.getInt8($offset);");
        }
    } else if (field.arg == "i16") {
        if (encode) {
            Ins(&R,out,"    view.setInt16($offset,$value,$lend);");
        } else {
            Ins(&R,out,"    $value = view.getInt16($offset,$lend);");
        }
    } else if (field.arg == "i32") {
        if (encode) {
            Ins(&R,out,"    view.setInt32($offset,$value,$lend);");
        } else {
            Ins(&R,out,"    $value = view.getInt32($offset,$lend);");
        }
    } else if (field.arg == "i64" || (field.p_arg->c_cjsbltin && field.p_arg->c_cjsbltin->p_jsbltin == &amc_jsbltin_bigint)) {
        if (encode) {
            Ins(&R,out,"    view.setBigInt64($offset,$value,$lend);");
        } else {
            Ins(&R,out,"    $value = view.getBigInt64($offset,$lend);");
        }
    } else if (field.arg == "bool") {
        // bool occupies one byte on the wire; map to/from a JS boolean.
        if (encode) {
            Ins(&R,out,"    view.setUint8($offset,$value ? 1 : 0);");
        } else {
            Ins(&R,out,"    $value = view.getUint8($offset) !== 0;");
        }
    } else if (GetJsBltin(field) == &amc_jsbltin_string) {
        // fixed-length, NUL-padded string: read/write totsize_byte bytes, trim pad on decode.
        Set(R,"$nbyte", tempstr() << field.p_arg->totsize_byte);
        if (encode) {
            Ins(&R,out,"    let _a_$name = new Uint8Array(view.buffer,view.byteOffset+$offset,$nbyte);");
            Ins(&R,out,"    _a_$name.fill(0);");
            Ins(&R,out,"    _g_text_encoder.encodeInto($value,_a_$name);");
        } else {
            Ins(&R,out,"    let _s_$name = new Uint8Array(view.buffer,view.byteOffset+$offset,$nbyte);");
            Ins(&R,out,"    let _e_$name = _s_$name.indexOf(0);");
            Ins(&R,out,"    $value = _g_text_decoder.decode(_e_$name < 0 ? _s_$name : _s_$name.subarray(0,_e_$name));");
        }
    } else {
        tempstr jsclass = GetJsClass(field);
        Set(R,"$jsclass",jsclass);
        if (encode) {
            Ins(&R,out,"    $jsclass_Encode(view,$offset,$value);");
            TsImportMaybe(field,"$jsclass_Encode");
        } else {
            Ins(&R,out,"    $value = $jsclass_Decode(view,$offset);");
            TsImportMaybe(field,"$jsclass_Decode");
        }
    }
}

// Generate $ctype_Decode: read a $ctype from VIEW at OFFSET -- fixed-size
// fields at their static offsets, then the varlen tails framed by the
// message length field.
void amc::TsGenDecode() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    if (amc::FJstype *jstype = ctype.c_jstype) {
        Ins(&R,jstype->funcs,"");
        Ins(&R,jstype->funcs,"export function $ctype_Decode(view: DataView, offset: number): $ctype {");
        Ins(&R,jstype->funcs,"    let parent = new $ctype;");
        int offset = TsGenFixedWalk(R,jstype->funcs,ctype,false);
        // read fields
        ind_beg(ctype_zd_varlenfld_curs,field,ctype) {
            Set(R,"$name",name_Get(field));
            Set(R,"$offset",tempstr()<<"offset+"<<offset);
            if (ctype_zd_varlenfld_Next(field)) {
                Set(R,"$fldlength","$name_end");
            } else {
                Set(R,"$fldlength",tempstr()<<"length-"<<offset);
            }
            if (ctype_zd_varlenfld_Prev(field)) {
                Set(R,"$prevname",name_Get(*ctype_zd_varlenfld_Prev(field)));
                Set(R,"$offset","$offset+$prevname_end");
                Set(R,"$fldlength","$fldlength-$prevname_end");
            }
            if (GetJsClass(field,false) == dmmeta_jsbltin_string) {
                Ins(&R,jstype->funcs,"    parent.$name = _g_text_decoder.decode(new Uint8Array(view.buffer,view.byteOffset+$offset,$fldlength));");
            } else {
                Ins(&R,jstype->funcs,"    parent.$name = new Uint8Array(view.buffer,view.byteOffset+$offset,$fldlength);");
            }
        }ind_end;
        Ins(&R,jstype->funcs,"    return parent;");
        Ins(&R,jstype->funcs,"}");
    }
}

// -----------------------------------------------------------------------------

// Whether FIELD appears in the printed form of its ctype:
// Base injection, bitfields, and message header type/length fields carry no display value.
static bool TsPrintFieldQ(amc::FField &field) {
    bool skip = field.reftype == dmmeta_Reftype_reftype_Base
        || field.c_bitfld != NULL
        || (field.p_ctype->c_msgtype && (field.c_typefld || field.c_lenfld));
    return !skip;
}

// TS template-literal fragment rendering FIELD of `parent` as text:
// an fconst field prints the constant name, a nested ctype recurses through
// its _Print, a byte array prints its length, a scalar prints its value.
static tempstr TsPrintValueExpr(amc::FField &field) {
    tempstr ret;
    amc::FJsbltin *jsbltin = amc::GetJsBltin(field);
    tempstr access(tempstr() << "parent." << name_Get(field));
    if (c_fconst_N(field) && jsbltin->jsbltin == amc::dmmeta_jsbltin_number) {
        tempstr enumname(tempstr() << name_Get(*field.p_ctype));
        if (field.c_anonfld) {
            enumname << "Enum";
        } else {
            enumname << "_" << name_Get(field) << "_Enum";
        }
        ret << "${" << enumname << "[" << access << "] ?? " << access << "}";
    } else if (jsbltin->jsbltin == amc::dmmeta_jsbltin_Uint8Array) {
        ret << "${" << access << ".length}b";
    } else if (jsbltin->jsbltin == amc::dmmeta_jsbltin_object) {
        tempstr jsclass = amc::GetJsClass(field);
        amc::TsImportMaybe(field, tempstr() << jsclass << "_Print");
        ret << "${" << jsclass << "_Print(" << access << ")}";
    } else {
        ret << "${" << access << "}";
    }
    return ret;
}

// Generate $ctype_Print: render a decoded value as text, the TS counterpart of
// the C++ Tuple print -- space-separated field:value pairs; a ctype with a
// single printed field prints the bare value.
// The body is a template literal, so it is assembled raw rather than through
// Ins (whose $-substitution would eat the literal's ${...} interpolations).
void amc::TsGenPrint() {
    auto &ctype = *_db.genctx.p_ctype;
    if (amc::FJstype *jstype = ctype.c_jstype) {
        int nfield = 0;
        ind_beg(ctype_c_field_curs,field,ctype) {
            nfield += TsPrintFieldQ(field);
        }ind_end;
        tempstr body;
        ind_beg(ctype_c_field_curs,field,ctype) {
            if (TsPrintFieldQ(field)) {
                if (ch_N(body)) {
                    body << " ";
                }
                if (nfield > 1) {
                    body << name_Get(field) << ":";
                }
                body << TsPrintValueExpr(field);
            }
        }ind_end;
        jstype->funcs << "\n";
        jstype->funcs << "export function " << name_Get(ctype) << "_Print(parent: " << name_Get(ctype) << "): string {\n";
        jstype->funcs << "    return `" << body << "`;\n";
        jstype->funcs << "}\n";
    }
}

// -----------------------------------------------------------------------------

// Generate interface for current ctype that is to be used against JSON payloads
// With JSON, most fields remain as strings
void amc::TsGenJsonInterface() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    if (amc::FJstype *jstype = ctype.c_jstype) {
        bool api_ns = ctype.p_ns->c_nsjs && ctype.p_ns->c_nsjs->ifaceonly;
        Ins(&R,jstype->funcs,"");
        Ins(&R,jstype->funcs, api_ns ? "export interface $ctype {" : "export interface $ctypeJson {");
        ind_beg(ctype_c_field_curs,field,ctype) {
            if (ctype.c_msgtype && field.c_typefld) {
                // type field in a message
            } else if (ctype.c_msgtype && field.c_lenfld) {
                // length field in a message
            } else if (field.reftype != dmmeta_Reftype_reftype_Val) {
                // skip it-- but what about all the types we support...
            } else {
                Set(R,"$name",name_Get(field));
                Set(R,"$jsclass",GetJsClass(field,true/*json*/));
                tempstr comment;
                if (ch_N(field.comment)) {
                    comment << " // " << field.comment;
                }
                Set(R,"$comment",comment);
                Ins(&R,jstype->funcs,"    $name: $jsclass;$comment");
            }
        }ind_end;
        Ins(&R,jstype->funcs,"}");
    }
}

void amc::JsGenCtype() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    if (amc::FJstype *jstype = ctype.c_jstype) {
        Set(R,"$ctype",name_Get(ctype));
        if (TsQ(ctype) && ctype.c_pack) {
            TsGenEncode();
            TsGenDecode();
            TsGenPrint();
        }
        if (TsQ(ctype)) {
            TsGenJsonInterface();
        }
        Set(R,"$csize", tempstr() << ctype.totsize_byte);
        Set(R,"$number", (TsQ(ctype) ? ": number " : ""));
        Ins(&R,jstype->body,"    static size() $number{");
        Ins(&R,jstype->body,"        return $csize;");
        Ins(&R,jstype->body,"    }");
    }
}

void amc::gen_nsjs_ctype() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nsjs) {
        amc::_db.genctx.p_ns = &ns;
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (JsQ(ctype)) {
            amc::_db.genctx.p_ctype = &ctype;
            ResetVars(amc::_db.genctx);
            JsGenCtype();
            amc::_db.genctx.p_ctype = NULL;
        }ind_end;
    }ind_end;
}

void amc::TsGenMsgtype() {
    auto &ns = *_db.genctx.p_ns;
    amc::FNsjs &nsjs = *ns.c_nsjs;
    auto &R = _db.genctx.R;
    int msgcount(0);
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        msgcount += JsQ(ctype) && ctype.c_msgtype;
    }ind_end;
    if (msgcount) {
        Ins(&R,nsjs.filebody,"");
        Ins(&R,nsjs.filebody,"export enum $ns_Msgtype {");
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (JsQ(ctype) && ctype.c_msgtype) {
            Set(R,"$name",name_Get(ctype));
            Set(R,"$value",tempstr()<<ctype.c_msgtype->type);
            Ins(&R,nsjs.filebody,"    $name = $value,");
        }ind_end;
        Ins(&R,nsjs.filebody,"}");
    }
}

// Generate $ns_MsgPrint: decode a packed message by its msgtype and render it
// with the ctype's _Print; returns null for a type this namespace does not define.
void amc::TsGenMsgPrint() {
    auto &ns = *_db.genctx.p_ns;
    amc::FNsjs &nsjs = *ns.c_nsjs;
    auto &R = _db.genctx.R;
    int msgcount(0);
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        msgcount += TsQ(ctype) && ctype.c_msgtype && ctype.c_pack;
    }ind_end;
    if (msgcount) {
        Ins(&R,nsjs.filebody,"");
        Ins(&R,nsjs.filebody,"export function $ns_MsgPrint(type: number, view: DataView, offset: number): {name: string, text: string} | null {");
        Ins(&R,nsjs.filebody,"    switch (type) {");
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (TsQ(ctype) && ctype.c_msgtype && ctype.c_pack) {
            Set(R,"$name",name_Get(ctype));
            Set(R,"$value",tempstr()<<ctype.c_msgtype->type);
            Ins(&R,nsjs.filebody,"    case $value: return {name: \"$name\", text: $name_Print($name_Decode(view, offset))};");
        }ind_end;
        Ins(&R,nsjs.filebody,"    default: return null;");
        Ins(&R,nsjs.filebody,"    }");
        Ins(&R,nsjs.filebody,"}");
    }
}

void amc::JsGenNs() {
    auto &ns = *_db.genctx.p_ns;
    amc::FNsjs &nsjs = *ns.c_nsjs;
    auto &R = _db.genctx.R;
    Set(R,"$ns",ns.ns);
    Ins(&R,nsjs.filebody,"let _g_text_decoder = new TextDecoder('utf-8');");
    Ins(&R,nsjs.filebody,"let _g_text_encoder = new TextEncoder;");
    TsGenMsgtype();
    TsGenMsgPrint();
}

// -----------------------------------------------------------------------------

// nsjs_ns gen phase: emit each nsjs namespace's file-level prologue -- the
// text codec globals, the msgtype enum, and the message print dispatch.
void amc::gen_nsjs_ns() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nsjs) {
        amc::_db.genctx.p_ns = &ns;
        ResetVars(amc::_db.genctx);
        JsGenNs();
    }ind_end;
}

// -----------------------------------------------------------------------------

static void TsJsEscape(cstring &out, strptr s) {
    for (int i = 0; i < elems_N(s); i++) {
        char c = s.elems[i];
        if (c == '"' || c == '\\') {
            out << '\\';
        }
        out << c;
    }
}

// Coercion kind a text-tuple attr of FIELD parses into on the TS side:
// numeric args parse with Number() (u64 narrows to double -- acceptable for
// display, the same loss the UI's numAttr always had), bool with a truth
// test, everything else stays a string.
static strptr TsCoercionOf(amc::FField &field) {
    tempstr jsclass(amc::GetJsClass(field, true/*json*/));
    return jsclass == "number" || jsclass == "bigint" ? strptr("number")
        : jsclass == "boolean" ? strptr("boolean")
        : strptr("string");
}

// -----------------------------------------------------------------------------

// Default value of FIELD as the TS form presents it: the dmmeta dflt with
// its ssim quoting removed ('"data"' -> data), other values verbatim.
static tempstr TsDfltUnquote(amc::FField &field) {
    strptr val = field.dflt.value;
    if (elems_N(val) >= 2 && val.elems[0] == '"' && val.elems[val.n_elems - 1] == '"') {
        val = ch_GetRegion(val, 1, val.n_elems - 2);
    }
    return tempstr(val);
}

// -----------------------------------------------------------------------------

// TS identifier for report ctype NAME: "abt_batch" -> "AbtBatch".
// Project every report.* ctype into report_gen.ts as a runtime coercion
// table (attr name -> number|boolean|string, driving a generic text-tuple
// parser) plus the matching row interface -- a UI that reads report records
// off a stream tracks the schema instead of hand-maintaining the field list.
// Pure projection of the loaded dmmeta model, in schema order.
static void TsGenReportSchema(cstring &out, amc::FNs &ns) {
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        tempstr tsname;
        algo::strptr_PrintCamel(name_Get(ctype),tsname);
        out << "\n// " << ctype.ctype << ": " << ctype.comment << "\n";
        out << "export const Report" << tsname << " = {\n";
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            if (field.reftype != dmmeta_Reftype_reftype_Base) {
                out << "  " << name_Get(field) << ": \"" << TsCoercionOf(field) << "\",\n";
            }
        }ind_end;
        out << "} as const;\n\n";
        out << "export interface Report" << tsname << "Row {\n";
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            if (field.reftype != dmmeta_Reftype_reftype_Base) {
                out << "  readonly " << name_Get(field) << ": " << TsCoercionOf(field) << ";\n";
            }
        }ind_end;
        out << "}\n";
    }ind_end;
}

// Project every ssimfile ctype of an ssimdb namespace into a table of
// descriptors: attr name, coercion type, dmmeta default, and — for substr
// fields — the source field and Pathcomp expr they derive from.  A UI that
// composes ssim tuples for the namespace builds them from these descriptors
// (field names, pkey composition) instead of hand-typed strings, so the
// dmmeta schema stays the single source of truth.  Substr fields are not
// stored in tuples; they exist so the client can compose the pkey value
// (e.g. dmmeta.field's pkey <ctype>.<name> from ctype and name).
static void TsGenSsimSchema(cstring &out, amc::FNs &ns) {
    out << "\nexport interface SsimFieldDesc {\n";
    out << "  readonly field: string;\n";
    out << "  readonly type: 'string' | 'number' | 'boolean';\n";
    out << "  readonly dflt: string;\n";
    out << "  readonly substr: { readonly srcfield: string; readonly expr: string } | null;\n";
    out << "}\n\n";
    out << "export interface SsimfileDesc {\n";
    out << "  readonly ssimfile: string;\n";
    out << "  readonly pkey: string;\n";
    out << "  readonly fields: readonly SsimFieldDesc[];\n";
    out << "}\n\n";
    tempstr nsname;
    algo::strptr_PrintCamel(ns.ns,nsname);
    out << "export const " << nsname << "Ssimfile = {\n";
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        if (amc::FSsimfile *ssimfile = ctype.c_ssimfile) {
            out << "  // " << ctype.ctype << ": " << ctype.comment << "\n";
            out << "  \"" << ssimfile->ssimfile << "\": {\n";
            out << "    ssimfile: \"" << ssimfile->ssimfile << "\",\n";
            bool first = true;
            ind_beg(amc::ctype_c_field_curs, field, ctype) {
                if (first) {
                    out << "    pkey: \"" << name_Get(field) << "\",\n";
                    out << "    fields: [\n";
                }
                out << "      { field: \"" << name_Get(field) << "\"";
                out << ", type: \"" << TsCoercionOf(field) << "\"";
                out << ", dflt: \"";
                TsJsEscape(out, strptr(TsDfltUnquote(field)));
                out << "\"";
                if (amc::FSubstr *substr = field.c_substr) {
                    out << ", substr: { srcfield: \"" << name_Get(*substr->p_srcfield) << "\", expr: \"";
                    TsJsEscape(out, substr->expr.value);
                    out << "\" } },\n";
                } else {
                    out << ", substr: null },\n";
                }
                first = false;
            }ind_end;
            out << "    ],\n";
            out << "  },\n";
        }
    }ind_end;
    out << "} as const;\n";
}

void amc::gen_nsjs_module() {
    auto &ns = *_db.c_ns;
    algo_lib::Replscope R;
    R.strict=2;
    if (amc::FNsjs *nsjs = ns.c_nsjs) {
        Set(R,"$ns",ns.ns);
        cstring &out= outfile_Create(Subst(R,nsjs->typescript ? "ts/gen/$ns_gen.ts" : "js/gen/$ns_gen.js")).text;
        if (nsjs->typescript) {
            Ins(&R,out,"export {};");
        }
        bool api_ns = nsjs->ifaceonly;
        if (!api_ns) {
            out << nsjs->imports << nsjs->filebody;
        }
        ind_beg(ns_c_ctype_curs,ctype,ns) {
            if (amc::FJstype *jstype = ctype.c_jstype) {
                Set(R,"$ctype",name_Get(ctype));
                Set(R,"$args",jstype->args);
                if (api_ns) {
                    out << jstype->funcs;
                } else {
                    Ins(&R,out,"");
                    if (TsQ(ctype)) {
                        Ins(&R,out,"export class $ctype {");
                        InsertIndent(out,jstype->body,1);
                        Ins(&R,out,"    constructor($args) {");
                        InsertIndent(out,jstype->ctor,2);
                        Ins(&R,out,"    }");
                        Ins(&R,out,"}");
                    } else {
                        Ins(&R,out,"class $ctype {");
                        Ins(&R,out,"    constructor() {");
                        InsertIndent(out,jstype->ctor,2);
                        Ins(&R,out,"        Object.preventExtensions(this);");
                        Ins(&R,out,"    }");
                        InsertIndent(out,jstype->body,1);
                        Ins(&R,out,"}");
                    }
                    out << jstype->funcs;
                }
            }
        }ind_end;
        if (ns.ns == "report") {
            TsGenReportSchema(out, ns);
        }
        if (ns.nstype == dmmeta_Nstype_nstype_ssimdb) {
            TsGenSsimSchema(out, ns);
        }
    }
}
