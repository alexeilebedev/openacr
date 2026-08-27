// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/msgschema.cpp
//
// Derive the message-layout tables dmmeta.payloadhdr, dmmeta.msg and
// dmmeta.msgfield from the schema.  A runtime consumer (the web console's
// schema-driven decoder) reads these tables to decode a message it was not
// compiled against: dmmeta.payloadhdr gives the type/length layout of each
// message header ctype, dmmeta.msg maps (payload header, msgtype) to a
// message ctype, and dmmeta.msgfield lists each message's leaf scalar
// fields with resolved byte offsets.  The tables are written back to the
// database by gen_table_write, alongside ctypelen and userfunc.
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

// Append one leaf field row of MSG: dotted NAME at byte OFFSET, read as
// scalar ctype ARG of WIDTH bytes (for a fixed char array, the array length).
// The row is returned so a caller can describe what the columns above cannot.
static amc::FMsgfield &AddMsgfield(amc::FMsg &msg, algo::strptr name, int offset, algo::strptr arg, int width, bool bigend, bool varlen) {
    amc::FMsgfield &row = amc::msgfield_Alloc();
    row.msgfield = tempstr() << msg.ctype << "/" << name;
    row.offset = offset;
    row.arg = arg;
    row.width = width;
    row.bigend = bigend;
    row.varlen = varlen;
    (void)amc::msgfield_XrefMaybe(row);
    return row;
}

// Say on ROW how the inline string of ctype ARG ends: an rpascal string by the
// count in its last byte, a padded one where its padding begins.  The padding
// character is written as its byte value, and a format that pads with nothing
// writes nothing.
static void StrtypeSet(amc::FMsgfield &row, amc::FCtype &arg) {
    ind_beg(amc::ctype_c_field_curs, field, arg) {
        if (field.c_smallstr) {
            u64 pad = 0;
            algo::strptr expr = field.c_smallstr->pad.value;
            row.strtype = field.c_smallstr->strtype;
            if (ch_N(expr) > 0 && MsgtypeEvalMaybe(expr, pad)) {
                row.pad = tempstr() << pad;
            }
        }
    }ind_end;
}

// Walk the physical fields of CTYPE, advancing OFFSET past each one, and
// classify every field as a leaf scalar, a fixed string, a nested struct
// (recursed into with a dotted PREFIX), a varlen tail, or framing (the
// typefld/lenfld the payload header already describes).  With EMIT set,
// append a dmmeta.msgfield row per leaf under MSG; without it, only
// classify.  Returns false when some field cannot be represented — the
// caller then derives no rows for the message at all, because a partial
// layout would place later fields at wrong offsets and decode garbage.
static bool WalkMsgfield(amc::FCtype &ctype, amc::FMsg *msg, algo::strptr prefix, int &offset, bool emit) {
    bool ok = true;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (ok && !FldfuncQ(field)) {
            amc::FCtype &arg = *field.p_arg;
            int size = arg.totsize_byte == 0 ? 1 : arg.totsize_byte;
            tempstr name;
            name << prefix << name_Get(field);
            if (field.reftype == dmmeta_Reftype_reftype_Base) {
                // base fields are cloned into the derived ctype; the clones follow
            } else if (field.reftype == dmmeta_Reftype_reftype_Bitfld) {
                // computed from its source field; no storage of its own
            } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
                // an optional trailing message; it begins past the fixed
                // layout this walk measures, and AddOpttail describes it
            } else if (field.reftype == dmmeta_Reftype_reftype_Varlen) {
                if (arg.ctype == "char" || arg.ctype == "u8") {
                    if (emit) {
                        AddMsgfield(*msg, name, offset, arg.ctype, 0, false, true);
                    }
                } else {
                    ok = false;
                }
            } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
                bool fixed_char = FixaryQ(field) && arg.ctype == "char" && field.c_inlary;
                if (fixed_char) {
                    if (emit) {
                        AddMsgfield(*msg, name, offset, arg.ctype, size * field.c_inlary->max, false, false);
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
                        AddMsgfield(*msg, name, offset, arg.ctype, size, field.c_fbigend != NULL, false);
                    }
                } else if (arg.c_cstr) {
                    if (emit) {
                        amc::FMsgfield &row = AddMsgfield(*msg, name, offset, "char", size, false, false);
                        StrtypeSet(row, arg);
                    }
                } else if (c_field_N(arg) > 0) {
                    tempstr subprefix;
                    subprefix << name << ".";
                    int subo = offset;
                    ok = WalkMsgfield(arg, msg, subprefix, subo, emit);
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
            AddMsgfield(msg, name_Get(field), ctype.totsize_byte, field.p_arg->ctype, 0, false, true);
        }
    }ind_end;
}

// Derive dmmeta.payloadhdr, dmmeta.msg and dmmeta.msgfield rows for every
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
            bool layoutok = payloadhdr && typeok && WalkMsgfield(ctype, NULL, "", endoffset, false);
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
                    (void)WalkMsgfield(ctype, &msg, "", offset, true);
                    AddOpttail(msg, ctype);
                }
            }
        }
    }ind_end;
}
