// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/msgschema.cpp
//
// Derive the message-layout tables gendb.payloadhdr, gendb.msg and
// gendb.msgfield from the schema.  A runtime consumer (the web console's
// schema-driven decoder) reads these tables to decode a message it was not
// compiled against: gendb.payloadhdr gives the type/length layout of each
// message header ctype, gendb.msg maps (payload header, msgtype) to a
// message ctype, and gendb.msgfield lists each message's leaf scalar
// fields with resolved byte offsets.  The tables are written back to the
// database by gen_table_write, alongside ctypelen and cppsym.
// Uniqueness of the msgtype number within one payload header is enforced
// here: a collision is an amc error, so a merged database (platform +
// customer namespaces) cannot silently shadow a message.

#include "include/algo.h"
#include "include/amc.h"

// Round SIZE up to a multiple of ALIGNMENT (a power of two).
static int RoundUpSize(int size, int alignment) {
    return (size+alignment-1) & ~(alignment-1);
}

// Evaluate msgtype expression EXPR: decimal, hex (0x...), or a single-quoted
// char literal ('A').  On success set VALUE and return true.
static bool MsgtypeEvalMaybe(algo::strptr expr, u64 &value) {
    algo::strptr s = algo::Trimmed(expr);
    bool ret = false;
    value = 0;
    if (elems_N(s) == 3 && s[0] == '\'' && s[2] == '\'') {
        value = u8(s[1]);
        ret = true;
    } else if (elems_N(s) > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        ret = true;
        for (int i = 2; i < elems_N(s); i++) {
            u8 digit = 0;
            ret = ret && algo::ParseHex1(s[i], digit) == 1;
            value = value*16 + digit;
        }
    } else {
        ret = u64_ReadStrptrMaybe(value, s);
    }
    return ret;
}

// True if CTYPE is a leaf scalar the runtime decoder reads directly.
static bool ScalarArgQ(amc::FCtype &ctype) {
    algo::strptr name = ctype.ctype;
    return name == "u8" || name == "u16" || name == "u32" || name == "u64"
        || name == "i8" || name == "i16" || name == "i32" || name == "i64"
        || name == "bool" || name == "char" || name == "float" || name == "double";
}

// Append one field row of MSG: dotted NAME, laid out as FLDTYPE, holding ctype
// ARG at byte OFFSET, with WIDTH and AUX read as FLDTYPE says (algo.Fldtype).
//
// ARG is a leaf scalar for every fixed field, and the element ctype of a varlen:
// a varlen of char is text and a varlen of a message ctype is a run of framed
// messages, and a reader of the row wants to be told which.
static void AddMsgfield(amc::FMsg &msg, algo::strptr name, algo::Fldtype fldtype, algo::strptr arg, int offset, int width, int aux) {
    amc::FMsgfield &row = amc::msgfield_Alloc();
    row.msgfield = tempstr() << msg.ctype << "/" << name;
    row.fldtype = fldtype;
    row.arg = arg;
    row.offset = offset;
    row.width = width;
    row.aux = aux;
    (void)amc::msgfield_XrefMaybe(row);
}

// Return the layout of the inline string ctype ARG, and set PAD to its padding
// byte: rpascal ends at the count in its last byte, rightpad where its padding
// begins, and leftpad where its padding ends.  Set OK to false for any other
// format, which no reader of the table decodes.
static algo::Fldtype StrFldtypeGet(amc::FCtype &arg, int &pad, bool &ok) {
    algo::Fldtype ret(algo_Fldtype_rpascal);
    ind_beg(amc::ctype_c_field_curs, field, arg) {
        if (field.c_smallstr) {
            u64 padval = 0;
            algo::strptr expr = field.c_smallstr->pad.value;
            ok = ok && algo::Fldtype_ReadStrptrMaybe(ret, field.c_smallstr->strtype);
            if (ch_N(expr) > 0 && MsgtypeEvalMaybe(expr, padval)) {
                pad = int(padval);
            }
        }
    }ind_end;
    return ret;
}

// Return how many fields of CTYPE a gendb.msgfield row describes: each
// computed field, and each field with storage of its own that is not framing.
static int MsgfieldN(amc::FCtype &ctype) {
    int ret = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        bool stored = !FldfuncQ(field) && !field.c_typefld && !field.c_lenfld
            && (field.reftype == dmmeta_Reftype_reftype_Val || field.reftype == dmmeta_Reftype_reftype_Inlary);
        ret += field.c_cppfunc || stored ? 1 : 0;
    }ind_end;
    return ret;
}

// Walk the physical fields of CTYPE, advancing OFFSET past each one, and
// classify every field as a leaf scalar, a fixed string, a nested struct
// (recursed into with a dotted PREFIX), a varlen tail, or framing (the
// typefld/lenfld the payload header already describes).  A cppfunc field takes
// no bytes and gets a row of its own: a reader that knows the field by name
// computes it with the field's getter, and every other reader skips it.  With EMIT set,
// append a gendb.msgfield row per leaf under MSG; without it, only
// classify.  Returns false when some field cannot be represented — the
// caller then derives no rows for the message at all, because a partial
// layout would place later fields at wrong offsets and decode garbage.
//
// WHOLE says that PREFIX is the whole name of CTYPE's one data field.  A struct
// of one data field, such as ams.ProcId around its value, stands for that
// field, so ams.LogMsg's proc_id is the row ams.LogMsg/proc_id: the inner name
// (proc_id.value) says nothing a reader wants to type when naming the field.
static bool WalkMsgfield(amc::FCtype &ctype, amc::FMsg *msg, algo::strptr prefix, int &offset, bool emit, bool whole) {
    bool ok = true;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (ok && emit && field.c_cppfunc) {
            tempstr name;
            name << prefix << (whole ? algo::strptr() : name_Get(field));
            AddMsgfield(*msg, name, algo::Fldtype(algo_Fldtype_cppfunc), field.p_arg->ctype, 0, 0, 0);
        }
        if (ok && !FldfuncQ(field)) {
            amc::FCtype &arg = *field.p_arg;
            int size = arg.totsize_byte == 0 ? 1 : arg.totsize_byte;
            tempstr name;
            name << prefix << (whole ? algo::strptr() : name_Get(field));
            if (field.reftype == dmmeta_Reftype_reftype_Base) {
                // base fields are cloned into the derived ctype; the clones follow
            } else if (field.reftype == dmmeta_Reftype_reftype_Bitfld) {
                // computed from its source field; no storage of its own
            } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
                // an optional trailing message; it begins past the fixed
                // layout this walk measures, and AddOpttail describes it
            } else if (field.reftype == dmmeta_Reftype_reftype_Varlen) {
                // the varlens follow the fixed layout and the end fields amc adds
                // for them, so AddVarlen describes them once the walk is over
            } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
                bool fixed_char = FixaryQ(field) && arg.ctype == "char" && field.c_inlary;
                if (fixed_char) {
                    if (emit) {
                        AddMsgfield(*msg, name, algo::Fldtype(algo_Fldtype_fixed), arg.ctype, offset, size * field.c_inlary->max, 0);
                    }
                    offset += size * field.c_inlary->max;
                } else {
                    ok = false;
                }
            } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
                if (!amc::PackQ(ctype)) {
                    offset = RoundUpSize(offset, arg.alignment);
                }
                if (field.c_typefld || field.c_lenfld) {
                    // framing; the payload header row describes it
                } else if (ScalarArgQ(arg)) {
                    if (emit) {
                        algo::Fldtype fldtype(field.c_fbigend ? algo_Fldtype_fixed_be : algo_Fldtype_fixed);
                        AddMsgfield(*msg, name, fldtype, arg.ctype, offset, size, 0);
                    }
                } else if (arg.c_cstr) {
                    int pad = 0;
                    algo::Fldtype fldtype = StrFldtypeGet(arg, pad, ok);
                    if (emit && ok) {
                        AddMsgfield(*msg, name, fldtype, "char", offset, size, pad);
                    }
                } else if (c_field_N(arg) > 0) {
                    bool one = MsgfieldN(arg) == 1;
                    tempstr subprefix;
                    subprefix << name << (one ? "" : ".");
                    int subo = offset;
                    ok = WalkMsgfield(arg, msg, subprefix, subo, emit, one);
                } else {
                    ok = false;
                }
                offset += size;
            } else {
                ok = false;
            }
        }
    }ind_end;
    return ok;
}

// Byte offset of TARGET among the physical fields of HDR, or -1 when TARGET
// is not a fixed-offset field of HDR.
static int FieldOffsetGet(amc::FCtype &hdr, amc::FField &target) {
    int ret = -1;
    int offset = 0;
    ind_beg(amc::ctype_c_field_curs, field, hdr) {
        if (!FldfuncQ(field) && field.reftype == dmmeta_Reftype_reftype_Val) {
            amc::FCtype &arg = *field.p_arg;
            if (!amc::PackQ(hdr)) {
                offset = RoundUpSize(offset, arg.alignment);
            }
            if (ret == -1 && &field == &target) {
                ret = offset;
            }
            offset += arg.totsize_byte == 0 ? 1 : arg.totsize_byte;
        }
    }ind_end;
    return ret;
}

// Find or derive the payload header row for header ctype HDR.  Returns NULL
// when HDR's type/length fields are not fixed-offset scalars — such a header
// cannot be described to the runtime decoder.
static amc::FPayloadhdr *GetOrCreatePayloadhdr(amc::FCtype &hdr) {
    amc::FPayloadhdr *ret = hdr.c_payloadhdr;
    if (!ret && hdr.c_typefld && hdr.c_lenfld) {
        amc::FField &typefld = *hdr.c_typefld->p_field;
        amc::FField &lenfld = *hdr.c_lenfld->p_field;
        int typeoffset = FieldOffsetGet(hdr, typefld);
        int lenoffset = FieldOffsetGet(hdr, lenfld);
        bool scalar = ScalarArgQ(*typefld.p_arg) && ScalarArgQ(*lenfld.p_arg);
        if (typeoffset >= 0 && lenoffset >= 0 && scalar) {
            amc::FPayloadhdr &row = amc::payloadhdr_Alloc();
            row.ctype = hdr.ctype;
            row.typeoffset = typeoffset;
            row.typewidth = typefld.p_arg->totsize_byte;
            row.typebig = typefld.c_fbigend != NULL;
            row.lenoffset = lenoffset;
            row.lenwidth = lenfld.p_arg->totsize_byte;
            row.lenbig = lenfld.c_fbigend != NULL;
            row.lenscale = hdr.c_lenfld->scale;
            row.lenextra = hdr.c_lenfld->extra;
            row.len = hdr.totsize_byte;
            if (amc::payloadhdr_XrefMaybe(row)) {
                ret = &row;
            }
        }
    }
    return ret;
}

// Describe the Opt tail of CTYPE as a varlen field of MSG: the wrapped message
// its carrier holds past the end of its own storage, where the generated
// accessor reads it.  Only a tail whose ctype is a payload header is
// described; nothing else says where the tail's type and length sit.
static void AddOpttail(amc::FMsg &msg, amc::FCtype &ctype) {
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Opt && GetOrCreatePayloadhdr(*field.p_arg)) {
            AddMsgfield(msg, name_Get(field), algo::Fldtype(algo_Fldtype_varlen), field.p_arg->ctype, ctype.totsize_byte, 0, 0);
        }
    }ind_end;
}

// Describe the varlen fields of CTYPE as fields of MSG, whose length field is
// LENWIDTH bytes wide.
//
// Consider a message with a varlen key followed by a varlen payload.  The two
// share one area past the fixed layout, and the message says where the key
// stops in a field amc adds to it, key_end, which no dmmeta.field names.  amc adds one such
// end field per varlen but the last, as wide as the length field, together
// after the fixed fields, and each holds an offset into the varlen area.  So
// the area begins at the message's full size, and a varlen runs from the end
// field before it, or from the area's start, to its own end field, or to the
// end of the message.  A row says all of that with offsets: OFFSET is the
// area's start, WIDTH the offset of the end field it starts at, and AUX the
// offset of the end field it ends at.
static void AddVarlen(amc::FMsg &msg, amc::FCtype &ctype, int lenwidth) {
    int nvarlen = amc::zd_varlenfld_N(ctype);
    int base = ctype.totsize_byte;
    int idx = 0;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        int endoff = base - (nvarlen - 1 - idx) * lenwidth;
        bool last = idx == nvarlen - 1;
        algo::Fldtype fldtype(last ? algo_Fldtype_varlen : algo_Fldtype_varlen2);
        int begfld = idx == 0 ? 0 : endoff - lenwidth;
        AddMsgfield(msg, name_Get(field), fldtype, field.p_arg->ctype, base, begfld, last ? 0 : endoff);
        idx++;
    }ind_end;
}

// Derive gendb.payloadhdr, gendb.msg and gendb.msgfield rows for every
// message ctype whose ultimate base is a header with typefld and lenfld,
// and enforce that msgtype numbers are unique within one payload header.
// Packing is not required: the layout walk inserts the same alignment
// padding the compiler does, so an unpacked message derives correct
// offsets.  Messages whose layout cannot be represented (an unsupported
// reftype, a non-scalar leaf) are skipped whole, with a verbose log line
// naming the ctype.  Runs after compute_size (offsets need sizes) and
// before table_write (which persists the derived rows).
void amc::gen_msgschema() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        amc::FCtype *hdr = amc::UltimateBaseType(&ctype, NULL);
        if (ctype.c_msgtype && hdr && hdr != &ctype) {
            amc::FPayloadhdr *payloadhdr = GetOrCreatePayloadhdr(*hdr);
            u64 typeval = 0;
            bool typeok = MsgtypeEvalMaybe(ctype.c_msgtype->type.value, typeval);
            int endoffset = 0;
            bool layoutok = payloadhdr && typeok && WalkMsgfield(ctype, NULL, "", endoffset, false, false);
            if (!layoutok) {
                verblog("amc.msgschema_skip"
                        << Keyval("ctype", ctype.ctype)
                        << Keyval("comment", "message layout not representable; no msg row derived"));
            } else {
                amc::FMsg &msg = amc::msg_Alloc();
                msg.ctype = ctype.ctype;
                msg.payloadhdr = hdr->ctype;
                msg.type = typeval;
                msg.len = ctype.totsize_byte;
                msg.typekey = tempstr() << hdr->ctype << "/" << typeval;
                bool unique = amc::ind_msg_typekey_InsertMaybe(msg);
                amc::FMsg *other = unique ? NULL : amc::ind_msg_typekey_Find(msg.typekey);
                amccheck(unique, "amc.msgtype_collision"
                         << Keyval("ctype", ctype.ctype)
                         << Keyval("ctype2", (other ? algo::strptr(other->ctype) : algo::strptr()))
                         << Keyval("payloadhdr", hdr->ctype)
                         << Keyval("type", typeval)
                         << Keyval("comment", "msgtype numbers must be unique within one payload header"));
                if (unique && amc::msg_XrefMaybe(msg)) {
                    int offset = 0;
                    (void)WalkMsgfield(ctype, &msg, "", offset, true, false);
                    AddVarlen(msg, ctype, payloadhdr->lenwidth);
                    AddOpttail(msg, ctype);
                }
            }
        }
    }ind_end;
}
