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
// Source: cpp/amc/lang_rs.cpp
//

#include "include/algo.h"
#include "include/amc.h"

// A dmmeta.displang row whose language is rs names a dispatch whose messages a
// Rust program speaks.  proj.cpp collects the projection and checks its layout;
// this file writes it into one crate under rs/gen, a module file per namespace,
// rs/gen/src/<ns>_gen.rs.  The file of the dispatch's own namespace is the crate
// root: it declares the other namespaces' modules, and holds DecodeError, the
// Encode trait, the wire helpers every codec calls and the dispatch signature.
// Each projected ctype becomes a struct deriving Debug, Clone, Copy, PartialEq
// and Eq, with Default returning the schema defaults, an associated SIZE and TYPE,
// a decode function, and an Encode implementation.  A tail is a byte slice
// borrowed from the decoded buffer, so a ctype with a tail carries the lifetime
// 'a of that buffer and decoding copies nothing.  The text is laid out as rustfmt
// lays it out, so `cargo fmt --check` passes over the crate.

// -----------------------------------------------------------------------------

// Return the Rust scalar type of builtin ARG: a scalar with a wire form, or a
// float a type-only projection declares.
static algo::strptr RsScalarType(amc::FCtype &arg) {
    algo::strptr ret = "u8";
    if (arg.ctype == "u16") {
        ret = "u16";
    } else if (arg.ctype == "u32") {
        ret = "u32";
    } else if (arg.ctype == "u64") {
        ret = "u64";
    } else if (arg.ctype == "i8") {
        ret = "i8";
    } else if (arg.ctype == "i16") {
        ret = "i16";
    } else if (arg.ctype == "i32") {
        ret = "i32";
    } else if (arg.ctype == "i64") {
        ret = "i64";
    } else if (arg.ctype == "bool") {
        ret = "bool";
    } else if (arg.ctype == "double") {
        ret = "f64";
    } else if (arg.ctype == "float") {
        ret = "f32";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when TEXT is a Rust keyword of edition 2024, strict or reserved,
// which no identifier may be spelled as.
static bool RsKeywordQ(algo::strptr text) {
    return text == "as" || text == "async" || text == "await" || text == "break" || text == "const"
        || text == "continue" || text == "crate" || text == "dyn" || text == "else" || text == "enum"
        || text == "extern" || text == "false" || text == "fn" || text == "for" || text == "gen"
        || text == "if" || text == "impl" || text == "in" || text == "let" || text == "loop"
        || text == "match" || text == "mod" || text == "move" || text == "mut" || text == "pub"
        || text == "ref" || text == "return" || text == "self" || text == "Self" || text == "static"
        || text == "struct" || text == "super" || text == "trait" || text == "true" || text == "type"
        || text == "unsafe" || text == "use" || text == "where" || text == "while" || text == "abstract"
        || text == "become" || text == "box" || text == "do" || text == "final" || text == "macro"
        || text == "override" || text == "priv" || text == "typeof" || text == "unsized" || text == "virtual"
        || text == "yield" || text == "try" || text == "_";
}

// -----------------------------------------------------------------------------

// Return NAME as a Rust identifier: NAME itself, with an underscore appended
// when NAME is a keyword or the bare underscore.
static tempstr RsIdent(algo::strptr name) {
    tempstr ret(name);
    if (RsKeywordQ(name)) {
        ret << "_";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Rust type naming ARG as seen from the module of namespace NS, in
// the crate whose root is the module of namespace ROOT: a scalar for a builtin,
// the struct name for a ctype of NS, and the struct name under its module's
// crate path otherwise.
static tempstr RsTypeRef(amc::FNs &ns, amc::FNs *root, amc::FCtype &arg) {
    tempstr ret;
    if (arg.c_bltin) {
        ret << RsScalarType(arg);
    } else if (arg.p_ns == &ns) {
        ret << name_Get(arg);
    } else if (arg.p_ns == root) {
        ret << "crate::" << name_Get(arg);
    } else {
        ret << "crate::" << arg.p_ns->ns << "::" << name_Get(arg);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the name CTYPE is spelled with inside its own module: the struct name,
// with the lifetime of the decoded buffer when CTYPE has a tail.
static tempstr RsSelfType(amc::FCtype &ctype) {
    tempstr ret;
    ret << name_Get(ctype);
    if (amc::ProjAnyTailQ(ctype)) {
        ret << "<'a>";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Rust literal of schema default DFLT for a scalar of builtin ARG.  A
// quoted char becomes its code, an empty default the zero of the type, and a hex
// literal of a signed type is written unsigned and cast, since a signed literal
// cannot hold the high bit.
static tempstr RsLiteral(amc::FCtype &arg, algo::strptr dflt) {
    tempstr ret;
    bool zero = ch_N(dflt) == 0 || dflt == "\"\"" || dflt == "false" || dflt == "0";
    bool hex = elems_N(dflt) > 2 && dflt.elems[0] == '0' && (dflt.elems[1] == 'x' || dflt.elems[1] == 'X');
    bool sign = StartsWithQ(arg.ctype, "i");
    if (arg.ctype == "bool") {
        ret << (dflt == "true" || dflt == "1" ? "true" : "false");
    } else if (zero) {
        ret << "0";
    } else if (dflt == "true") {
        ret << "1";
    } else if (elems_N(dflt) == 3 && dflt.elems[0] == '\'') {
        ret << int(u8(dflt.elems[1]));
    } else if (hex && sign) {
        ret << dflt << "u" << arg.totsize_byte * 8 << " as " << RsScalarType(arg);
    } else {
        ret << dflt;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Rust expression reading a scalar of builtin ARG at byte offset
// expression OFF of buf, stored big-endian when BIGEND is set.
static tempstr RsGetExpr(amc::FCtype &arg, bool bigend, algo::strptr off) {
    tempstr ret;
    if (arg.ctype == "u8" || arg.ctype == "char") {
        ret << "buf[" << off << "]";
    } else if (arg.ctype == "i8") {
        ret << "buf[" << off << "] as i8";
    } else if (arg.ctype == "bool") {
        ret << "buf[" << off << "] != 0";
    } else {
        ret << RsScalarType(arg) << "::from_" << (bigend ? "be" : "le") << "_bytes(wire::get(buf, " << off << "))";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Rust range of the WIDTH bytes at offset expression OFF: numeric
// bounds when OFF is a number, and OFF plus WIDTH otherwise.
static tempstr RsRange(algo::strptr off, int width) {
    tempstr ret;
    int num = 0;
    if (i32_ReadStrptrMaybe(num, off)) {
        ret << num << ".." << num + width;
    } else {
        ret << off << ".." << off << " + " << width;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Append to OUT, at indent INDENT, the statement writing scalar expression VALUE
// of builtin ARG at byte offset expression OFF of buf, stored big-endian when
// BIGEND is set.  A one-byte VALUE is assigned, so a caller passing a reference
// passes it dereferenced; a wider VALUE is the receiver of to_le_bytes, and a
// cast in it is parenthesized by the caller.
static void RsPutStmt(cstring &out, int indent, amc::FCtype &arg, bool bigend, algo::strptr off, algo::strptr value) {
    char_PrintNTimes(' ', out, indent);
    if (arg.ctype == "u8" || arg.ctype == "char") {
        out << "buf[" << off << "] = " << value << ";" << eol;
    } else if (arg.ctype == "i8" || arg.ctype == "bool") {
        out << "buf[" << off << "] = " << value << " as u8;" << eol;
    } else {
        out << "buf[" << RsRange(off, arg.totsize_byte) << "].copy_from_slice(&" << value << ".to_" << (bigend ? "be" : "le") << "_bytes());" << eol;
    }
}

// -----------------------------------------------------------------------------

// Append to OUT, at indent INDENT, the struct literal of type NAME whose members
// are ITEM, each "member: expression", opened by LEAD and closed by TAIL.  The
// literal is laid out as rustfmt lays it out: on one line when its members take
// at most 18 columns and the line fits in 100, one member per line otherwise.
static void RsStructLit(cstring &out, int indent, algo::strptr lead, algo::strptr name, algo::StringAry &item, algo::strptr tail) {
    tempstr body;
    algo::ListSep ls(", ");
    ind_beg(algo::StringAry_ary_curs, text, item) {
        body << ls << text;
    }ind_end;
    int width = indent + ch_N(lead) + ch_N(name) + ch_N(body) + 5 + ch_N(tail);
    char_PrintNTimes(' ', out, indent);
    out << lead << name;
    if (ary_N(item) == 0) {
        out << " {}" << tail << eol;
    } else if (ch_N(body) <= 18 && width <= 100) {
        out << " { " << body << " }" << tail << eol;
    } else {
        out << " {" << eol;
        ind_beg(algo::StringAry_ary_curs, text, item) {
            char_PrintNTimes(' ', out, indent + 4);
            out << text << "," << eol;
        }ind_end;
        char_PrintNTimes(' ', out, indent);
        out << "}" << tail << eol;
    }
}

// -----------------------------------------------------------------------------

// Append to OUT, at indent INDENT, the doc comment TEXT, trimmed, when it is not
// empty.
static void RsDoc(cstring &out, int indent, algo::strptr text) {
    algo::strptr trimmed = algo::Trimmed(text);
    if (ch_N(trimmed)) {
        char_PrintNTimes(' ', out, indent);
        out << "/// " << trimmed << eol;
    }
}

// -----------------------------------------------------------------------------

// Return TRUE when the derived Default of CTYPE's struct is its schema default:
// every scalar member defaults to zero, every nested member to its own Default
// with no default of the member's own, and every inline array is short enough
// for Default to be derived over it.
static bool RsDeriveDefaultQ(amc::FCtype &ctype) {
    bool ret = true;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field) && !amc::ProjTailQ(field)) {
        bool nested = !field.p_arg->c_bltin;
        bool shortary = field.reftype != dmmeta_Reftype_reftype_Inlary || field.c_inlary->max <= 32;
        ret = ret && shortary && ((nested && !amc::ProjNestedDfltQ(field)) || amc::ProjZeroDfltQ(field));
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every member or bitfield of CTYPE whose Rust name would collide with an
// associated item of the generated struct, and return TRUE when there is none.
static bool RsCheckName(amc::FCtype &ctype) {
    bool ret = true;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_bitfld) {
        algo::strptr name = name_Get(field);
        if (name == "size" || name == "encode" || name == "encode_into" || name == "decode" || name == "decode_fixed") {
            prerr("amc.rs_name"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","bitfield name hides a method of the generated Rust struct"));
            ret = false;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Append to OUT the struct of CTYPE for the module of namespace NS, in the crate
// rooted at namespace ROOT: its doc
// comment, its derives, and a public member per projected field.  A tail is a
// byte slice borrowed for the lifetime 'a.  A type-only CTYPE declares a string
// member as a String and may hold a float, so it derives neither Copy nor Eq,
// and starts at its Default.
static void RsGenStruct(cstring &out, amc::FNs &ns, amc::FNs *root, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    tempstr title;
    title << ctype.ctype;
    if (ch_N(algo::Trimmed(ctype.comment))) {
        title << ": " << algo::Trimmed(ctype.comment);
    }
    int nmember = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        nmember++;
    }ind_end;
    Set(R,"$title",title);
    Set(R,"$selftype",RsSelfType(ctype));
    Ins(&R,out,"");
    Ins(&R,out,"/// $title");
    if (!amc::ProjDfltQ(ctype)) {
        Ins(&R,out,"#[derive(Debug, Clone, PartialEq, Default)]");
    } else if (RsDeriveDefaultQ(ctype)) {
        Ins(&R,out,"#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]");
    } else {
        Ins(&R,out,"#[derive(Debug, Clone, Copy, PartialEq, Eq)]");
    }
    if (nmember) {
        Ins(&R,out,"pub struct $selftype {");
    } else {
        Ins(&R,out,"pub struct $selftype {}");
    }
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        tempstr type;
        if (!amc::ProjCodecQ(ctype) && amc::ProjStringQ(field)) {
            type << "String";
        } else if (amc::ProjTailQ(field)) {
            type << "&'a [u8]";
        } else if (amc::ProjRpascalQ(field)) {
            type << "crate::Rpascal<" << field.c_smallstr->length << ">";
        } else if (amc::ProjRightpadQ(field)) {
            type << "crate::Rightpad<" << field.c_smallstr->length << ", " << amc::ProjPadByte(field) << ">";
        } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
            type << "[" << RsTypeRef(ns, root, *field.p_arg) << "; " << field.c_inlary->max << "]";
        } else {
            type << RsTypeRef(ns, root, *field.p_arg);
        }
        RsDoc(out, 4, field.comment);
        Set(R,"$member",RsIdent(name_Get(field)));
        Set(R,"$memtype",type);
        Ins(&R,out,"    pub $member: $memtype,");
    }ind_end;
    if (nmember) {
        Ins(&R,out,"}");
    }
}

// -----------------------------------------------------------------------------

// Append to OUT the memory layout of CTYPE: its size and the offset of each
// member, as C++ lays the struct out, so a Rust program can share a segment
// with a C++ one.
static void RsGenLayout(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    Set(R,"$ctypename",name_Get(ctype));
    Set(R,"$ctypeid",ctype.ctype);
    Set(R,"$sizeof",tempstr() << ctype.totsize_byte);
    Ins(&R,out,"");
    Ins(&R,out,"/// The memory layout of $ctypeid as C++ lays the struct out: its size, and the");
    Ins(&R,out,"/// offset of each member.");
    Ins(&R,out,"#[allow(non_upper_case_globals)]");
    Ins(&R,out,"impl $ctypename {");
    Ins(&R,out,"    pub const Sizeof: usize = $sizeof;");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        Set(R,"$fldname",name_Get(field));
        Set(R,"$fldoff",tempstr() << field.offset);
        Ins(&R,out,"    pub const Off_$fldname: usize = $fldoff;");
    }ind_end;
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Append to OUT the Default implementation of CTYPE for the module of namespace
// NS in the crate rooted at namespace ROOT, when the derived one is not the schema default: every member takes its
// schema default, a nested member its own Default, and a tail the empty slice.
static void RsGenDefault(cstring &out, amc::FNs &ns, amc::FNs *root, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    if (!RsDeriveDefaultQ(ctype)) {
        algo::StringAry item;
        ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
            amc::FCtype &arg = *field.p_arg;
            tempstr value;
            if (amc::ProjTailQ(field)) {
                value << "&[]";
            } else if (amc::ProjRpascalQ(field) || amc::ProjRightpadQ(field)) {
                value << "Default::default()";
            } else if (amc::ProjNestedDfltQ(field)) {
                // the nested literal sits at the member's indent inside the
                // literal below, so it is laid out there and handed over as one
                // member, without the indent and the comma the outer one adds
                amc::FField &inner = amc::ProjNestedDfltField(field);
                algo::StringAry innermember;
                ary_Alloc(innermember) << RsIdent(name_Get(inner)) << ": " << RsLiteral(*inner.p_arg, amc::ProjDflt(field));
                cstring nested;
                tempstr lead;
                lead << RsIdent(name_Get(field)) << ": ";
                RsStructLit(nested, 12, lead, RsTypeRef(ns, root, arg), innermember, ",");
                algo::strptr text = algo::RestFrom(strptr(nested), int(12 + ch_N(lead)));
                value << algo::FirstN(text, int(elems_N(text) - 2));
            } else if (field.reftype == dmmeta_Reftype_reftype_Val && !arg.c_bltin) {
                value << "Default::default()";
            } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
                value << RsLiteral(arg, amc::ProjDflt(field));
            } else if (!arg.c_bltin) {
                value << "[" << RsTypeRef(ns, root, arg) << "::default(); " << field.c_inlary->max << "]";
            } else {
                value << "[" << RsLiteral(arg, amc::ProjDflt(field)) << "; " << field.c_inlary->max << "]";
            }
            ary_Alloc(item) << RsIdent(name_Get(field)) << ": " << value;
        }ind_end;
        Set(R,"$ctypename",name_Get(ctype));
        Ins(&R,out,"");
        if (amc::ProjAnyTailQ(ctype)) {
            Ins(&R,out,"impl Default for $ctypename<'_> {");
        } else {
            Ins(&R,out,"impl Default for $ctypename {");
        }
        Ins(&R,out,"    /// Returns the schema default of every member.");
        Ins(&R,out,"    fn default() -> Self {");
        RsStructLit(out, 8, "", name_Get(ctype), item, "");
        Ins(&R,out,"    }");
        Ins(&R,out,"}");
    }
}

// -----------------------------------------------------------------------------

// Append to OUT the named constants of CTYPE: every fconst of a scalar member.
// When the member is the ctype's one anonymous member, a constant is a value of
// the ctype itself, so it compares against a decoded member directly; any other
// constant is a scalar named after its member.  The fconsts of a type word are
// the message types of the whole schema, and each message states its own as
// TYPE, so those are left out.  A constant named like a bitfield of CTYPE takes
// an underscore, since the bitfield's accessor shares the impl's namespace.
static void RsGenConst(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    tempstr impltype;
    impltype << name_Get(ctype) << (amc::ProjAnyTailQ(ctype) ? "<'_>" : "");
    int nmember = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        nmember++;
    }ind_end;
    bool any = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        bool want = amc::ProjMemberQ(field) && !field.c_typefld && field.p_arg->c_bltin && amc::c_fconst_N(field) > 0;
        if (want) {
            bool typed = field.c_anonfld && nmember == 1;
            if (!any) {
                Set(R,"$impltype",impltype);
                Ins(&R,out,"");
                Ins(&R,out,"#[allow(non_upper_case_globals)]");
                Ins(&R,out,"impl $impltype {");
            }
            any = true;
            ind_beg(amc::field_c_fconst_curs, fconst, field) {
                tempstr name;
                if (!field.c_anonfld) {
                    name << name_Get(field) << "_";
                }
                name << amc::strptr_ToCppIdent(name_Get(fconst),true);
                // a constant and a bitfield accessor share the impl's namespace
                ind_beg(amc::ctype_c_field_curs, bitfld, ctype) if (bitfld.c_bitfld && name_Get(bitfld) == strptr(name)) {
                    name << "_";
                }ind_end;
                tempstr value;
                if (StartsWithQ(field.arg, "u")) {
                    value << u64(fconst.int_val);
                } else {
                    value << fconst.int_val;
                }
                RsDoc(out, 4, fconst.comment);
                if (typed) {
                    algo::StringAry item;
                    ary_Alloc(item) << RsIdent(name_Get(field)) << ": " << value;
                    tempstr lead;
                    lead << "pub const " << RsIdent(name) << ": " << name_Get(ctype) << " = ";
                    RsStructLit(out, 4, lead, name_Get(ctype), item, ";");
                } else {
                    Set(R,"$constname",RsIdent(name));
                    Set(R,"$scalartype",RsScalarType(*field.p_arg));
                    Set(R,"$constval",value);
                    Ins(&R,out,"    pub const $constname: $scalartype = $constval;");
                }
            }ind_end;
        }
    }ind_end;
    if (any) {
        Ins(&R,out,"}");
    }
}

// -----------------------------------------------------------------------------

// Append to ITEM a "member: expression" per fixed member of CTYPE, laid out in
// the module of namespace NS of the crate rooted at namespace ROOT, reading the member from buf at its offset.
static void RsDecodeItem(amc::FNs &ns, amc::FNs *root, amc::FCtype &ctype, algo::StringAry &item) {
    int offset = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        amc::FCtype &arg = *field.p_arg;
        bool bigend = field.c_fbigend != NULL;
        int elemsize = arg.totsize_byte;
        tempstr off;
        off << offset;
        tempstr elemoff;
        elemoff << offset << " + i * " << elemsize;
        tempstr value;
        if (!amc::ProjMemberQ(field) || amc::ProjTailQ(field)) {
            // derived words and tails are read by the caller
        } else if (amc::ProjRpascalQ(field)) {
            value << "crate::Rpascal::decode_fixed(&buf[" << offset << "..])";
        } else if (amc::ProjRightpadQ(field)) {
            value << "crate::Rightpad::decode_fixed(&buf[" << offset << "..])";
        } else if (field.reftype == dmmeta_Reftype_reftype_Val && arg.c_bltin) {
            value << RsGetExpr(arg, bigend, off);
        } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
            value << RsTypeRef(ns, root, arg) << "::decode_fixed(&buf[" << offset << "..])";
        } else if (arg.c_bltin && (arg.ctype == "u8" || arg.ctype == "char")) {
            value << "wire::get(buf, " << offset << ")";
        } else if (arg.c_bltin) {
            value << "std::array::from_fn(|i| " << RsGetExpr(arg, bigend, elemoff) << ")";
        } else {
            value << "std::array::from_fn(|i| " << RsTypeRef(ns, root, arg) << "::decode_fixed(&buf[" << elemoff << "..]))";
        }
        if (ch_N(value)) {
            ary_Alloc(item) << RsIdent(name_Get(field)) << ": " << value;
        }
        offset += i32_Max(amc::ProjSlotBytes(field), 0);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Append to OUT the statements of a decode body checking the type word and the
// length word of message CTYPE, and binding the frame length to `length` when
// the message has a tail.  A ctype that is not a framed message gets none.
static void RsDecodeFrame(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    int offset = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        amc::FCtype &arg = *field.p_arg;
        bool bigend = field.c_fbigend != NULL;
        tempstr off;
        off << offset;
        Set(R,"$ctypeid",ctype.ctype);
        Set(R,"$totsize",tempstr() << ctype.totsize_byte);
        if (field.c_typefld && ctype.c_msgtype) {
            Set(R,"$getexpr",RsGetExpr(arg, bigend, off));
            Set(R,"$msgid",tempstr() << ctype.c_msgtype->type);
            Ins(&R,out,"        let typ = $getexpr;");
            Ins(&R,out,"        wire::check_type(typ as u64, $msgid, \"$ctypeid\")?;");
        } else if (field.c_lenfld && ctype.c_msgtype) {
            amc::FLenfld &lenfld = *field.c_lenfld;
            tempstr expand;
            expand << RsGetExpr(arg, bigend, off) << " as i64";
            if (lenfld.scale != 1) {
                expand << " * " << lenfld.scale;
            }
            if (lenfld.extra != 0) {
                expand << " - (" << lenfld.extra << ")";
            }
            Set(R,"$expand",expand);
            Ins(&R,out,"        let length = $expand;");
            if (amc::ProjAnyTailQ(ctype)) {
                Ins(&R,out,"        let length = wire::check_length(buf, length, $totsize, \"$ctypeid\")?;");
            } else {
                Ins(&R,out,"        wire::check_length(buf, length, $totsize, \"$ctypeid\")?;");
            }
        }
        offset += i32_Max(amc::ProjSlotBytes(field), 0);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Append to OUT the inherent implementation of CTYPE for the module of namespace
// NS in the crate rooted at namespace ROOT: SIZE, TYPE for a message, decode, and for a ctype with no tail a
// decode_fixed that nested members are read with.  A decode refuses a buffer
// shorter than the fixed region, a message whose type word is another message's,
// a frame length outside the buffer, and an end offset outside the tail or below
// the one before it, so a frame from a peer of another layout is an error and
// never a read past its end.  Bitfield accessors follow.
static void RsGenImpl(cstring &out, amc::FNs &ns, amc::FNs *root, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    int csize = ctype.totsize_byte;
    bool tail = amc::ProjAnyTailQ(ctype);
    tempstr self = RsSelfType(ctype);
    algo::StringAry item;
    RsDecodeItem(ns, root, ctype, item);
    Set(R,"$selftype",self);
    Set(R,"$csize",tempstr() << csize);
    Set(R,"$ctypeid",ctype.ctype);
    Ins(&R,out,"");
    if (tail) {
        Ins(&R,out,"impl<'a> $selftype {");
    } else {
        Ins(&R,out,"impl $selftype {");
    }
    Ins(&R,out,"    /// The bytes of the fixed region.");
    Ins(&R,out,"    pub const SIZE: usize = $csize;");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_typefld && ctype.c_msgtype) {
        Set(R,"$typewordtype",RsScalarType(*field.p_arg));
        Set(R,"$msgid",tempstr() << ctype.c_msgtype->type);
        Ins(&R,out,"    /// The message type of $ctypeid.");
        Ins(&R,out,"    pub const TYPE: $typewordtype = $msgid;");
    }ind_end;
    if (!tail) {
        Ins(&R,out,"");
        Ins(&R,out,"    /// Reads the fixed region at the head of BUF; panics when BUF is shorter");
        Ins(&R,out,"    /// than SIZE.");
        if (ary_N(item)) {
            Ins(&R,out,"    pub fn decode_fixed(buf: &[u8]) -> $selftype {");
        } else {
            Ins(&R,out,"    pub fn decode_fixed(_buf: &[u8]) -> $selftype {");
        }
        RsStructLit(out, 8, "", name_Get(ctype), item, "");
        Ins(&R,out,"    }");
    }
    Ins(&R,out,"");
    Ins(&R,out,"    /// Reads the wire form at the head of BUF, refused when BUF is short or the");
    if (tail) {
        Ins(&R,out,"    /// frame is inconsistent.  A tail borrows from BUF.");
        Ins(&R,out,"    pub fn decode(buf: &'a [u8]) -> Result<$selftype, DecodeError> {");
    } else {
        Ins(&R,out,"    /// frame is inconsistent.");
        Ins(&R,out,"    pub fn decode(buf: &[u8]) -> Result<$selftype, DecodeError> {");
    }
    Ins(&R,out,"        wire::check_size(buf, $csize, \"$ctypeid\")?;");
    RsDecodeFrame(out, ctype);
    if (!tail) {
        Set(R,"$ctypename",name_Get(ctype));
        Ins(&R,out,"        Ok($ctypename::decode_fixed(buf))");
    } else {
        // the end offsets follow the fixed slots, in varlen order
        amc::FField *lenfield = amc::LengthField(ctype);
        int offset = 0;
        ind_beg(amc::ctype_c_field_curs, field, ctype) {
            offset += i32_Max(amc::ProjSlotBytes(field), 0);
        }ind_end;
        tempstr prev("0");
        ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
            if (amc::ctype_zd_varlenfld_Next(field) && lenfield) {
                tempstr end;
                end << "e_" << name_Get(field);
                tempstr off;
                off << offset;
                tempstr args;
                args << end << ", " << prev << ", length - " << csize << ", \"" << field.field << "\"";
                Set(R,"$endname",end);
                Set(R,"$endget",RsGetExpr(*lenfield->p_arg, lenfield->c_fbigend != NULL, off));
                Set(R,"$endargs",args);
                Set(R,"$prevend",prev);
                Set(R,"$fldid",field.field);
                Ins(&R,out,"        let $endname = $endget as usize;");
                // rustfmt puts one argument per line once the arguments pass 60
                // columns; short of that it keeps the statement on one line while
                // it fits in 100 columns, and moves the call below the binding
                // when it does not
                int oneline = 8 + 4 + ch_N(end) + 19 + ch_N(args) + 3;
                if (ch_N(args) <= 60 && oneline <= 100) {
                    Ins(&R,out,"        let $endname = wire::check_end($endargs)?;");
                } else if (ch_N(args) <= 60) {
                    Ins(&R,out,"        let $endname =");
                    Ins(&R,out,"            wire::check_end($endargs)?;");
                } else {
                    Ins(&R,out,"        let $endname = wire::check_end(");
                    Ins(&R,out,"            $endname,");
                    Ins(&R,out,"            $prevend,");
                    Ins(&R,out,"            length - $csize,");
                    Ins(&R,out,"            \"$fldid\",");
                    Ins(&R,out,"        )?;");
                }
                offset += lenfield->p_arg->totsize_byte;
                prev = end;
            }
        }ind_end;
        tempstr beg;
        beg << csize;
        ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
            tempstr end("length");
            if (amc::ctype_zd_varlenfld_Next(field)) {
                end = tempstr() << csize << " + e_" << name_Get(field);
            }
            ary_Alloc(item) << RsIdent(name_Get(field)) << ": &buf[" << beg << ".." << end << "]";
            beg = end;
        }ind_end;
        ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            ary_Alloc(item) << RsIdent(name_Get(field)) << ": &buf[" << csize << "..length]";
        }ind_end;
        // the struct literal's members are in field order, which the tails
        // appended last may not be; a literal names its members, so order is free
        RsStructLit(out, 8, "Ok(", name_Get(ctype), item, ")");
    }
    Ins(&R,out,"    }");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_bitfld) {
        amc::FBitfld &bitfld = *field.c_bitfld;
        amc::FField &src = *bitfld.p_srcfield;
        amc::FCtype &valarg = amc::ProjScalarArg(field);
        u64 mask = bitfld.width >= 64 ? ~u64(0) : (u64(1) << bitfld.width) - 1;
        tempstr hexmask;
        algo::u64_PrintHex(mask, hexmask, 1, true);
        algo::strptr srctype = RsScalarType(*src.p_arg);
        tempstr utype;
        utype << "u" << src.p_arg->totsize_byte * 8;
        bool sign = srctype != strptr(utype);
        tempstr word;
        word << "self." << RsIdent(name_Get(src));
        tempstr uword;
        if (sign) {
            uword << "(" << word << " as " << utype << ")";
        } else {
            uword << word;
        }
        tempstr name = RsIdent(name_Get(field));
        algo::strptr valtype = RsScalarType(valarg);
        tempstr title;
        title << "Returns bitfield " << name_Get(field);
        if (ch_N(algo::Trimmed(field.comment))) {
            title << ": " << algo::Trimmed(field.comment);
        }
        Set(R,"$title",title);
        Ins(&R,out,"");
        Ins(&R,out,"    /// $title");
        // a shift by zero and a cast to the type a value already has are left
        // out, so clippy finds nothing to say about the accessors
        tempstr shifted;
        if (bitfld.offset == 0) {
            shifted << uword;
        } else {
            shifted << "(" << uword << " >> " << bitfld.offset << ")";
        }
        tempstr cast;
        if (valtype == strptr(utype)) {
            cast << "x";
        } else {
            cast << "(x as " << utype << ")";
        }
        tempstr stored;
        if (bitfld.offset == 0) {
            stored << cast;
        } else {
            stored << "(" << cast << " << " << bitfld.offset << ")";
        }
        tempstr expr;
        expr << "(" << uword << " & !mask) | (" << stored << " & mask)";
        Set(R,"$accessor",name);
        Set(R,"$bitname",name_Get(field));
        Set(R,"$valtype",valtype);
        Set(R,"$utype",utype);
        Set(R,"$srctype",srctype);
        Set(R,"$shifted",shifted);
        Set(R,"$mask",hexmask);
        Set(R,"$bitoffset",tempstr() << bitfld.offset);
        Set(R,"$word",word);
        Set(R,"$setexpr",expr);
        Ins(&R,out,"    pub fn $accessor(&self) -> $valtype {");
        if (valarg.ctype == "bool") {
            Ins(&R,out,"        ($shifted & $mask) != 0");
        } else if (valtype == strptr(utype)) {
            Ins(&R,out,"        $shifted & $mask");
        } else {
            Ins(&R,out,"        ($shifted & $mask) as $valtype");
        }
        Ins(&R,out,"    }");
        Ins(&R,out,"");
        Ins(&R,out,"    /// Stores X into bitfield $bitname.");
        Ins(&R,out,"    pub fn set_$bitname(&mut self, x: $valtype) {");
        if (bitfld.offset == 0) {
            Ins(&R,out,"        let mask: $utype = $mask;");
        } else {
            Ins(&R,out,"        let mask: $utype = $mask << $bitoffset;");
        }
        if (sign) {
            Ins(&R,out,"        $word = ($setexpr) as $srctype;");
        } else {
            Ins(&R,out,"        $word = $setexpr;");
        }
        Ins(&R,out,"    }");
    }ind_end;
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Append to OUT the Encode implementation of CTYPE.  size is the fixed region
// plus every tail.  encode refuses a buffer
// shorter than size by panicking on the slice, copies the tails after the fixed
// region, then writes the fixed slots in schema order: a message's type word
// holds its type, its length word the frame length through the length field's
// scale and extra, and after the slots come the end offsets of every varlen but
// the last, counted from the end of the fixed region.
static void RsGenEncode(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    int csize = ctype.totsize_byte;
    bool tail = amc::ProjAnyTailQ(ctype);
    amc::FField *lenfield = amc::LengthField(ctype);
    algo::StringAry tailname;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        ary_Alloc(tailname) << name_Get(field);
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        ary_Alloc(tailname) << name_Get(field);
    }ind_end;
    // the sum stays on one line while it fits in 100 columns; past that rustfmt
    // keeps the first tail beside the fixed size and moves each later one to a
    // line of its own
    tempstr sizeline;
    sizeline << csize;
    ind_beg(algo::StringAry_ary_curs, name, tailname) {
        sizeline << " + self." << RsIdent(name) << ".len()";
    }ind_end;
    tempstr sizeexpr;
    sizeexpr << csize;
    int itailname = 0;
    ind_beg(algo::StringAry_ary_curs, name, tailname) {
        bool wrap = 8 + ch_N(sizeline) > 100 && itailname > 0;
        sizeexpr << (wrap ? "\n            + self." : " + self.") << RsIdent(name) << ".len()";
        itailname++;
    }ind_end;
    tempstr length;
    if (tail) {
        length << "length";
    } else {
        length << csize;
    }
    Set(R,"$ctypename",name_Get(ctype));
    Set(R,"$ctypeid",ctype.ctype);
    Set(R,"$csize",tempstr() << csize);
    Set(R,"$sizeexpr",sizeexpr);
    Set(R,"$length",length);
    Ins(&R,out,"");
    if (tail) {
        Ins(&R,out,"impl Encode for $ctypename<'_> {");
    } else {
        Ins(&R,out,"impl Encode for $ctypename {");
    }
    Ins(&R,out,"    fn size(&self) -> usize {");
    Ins(&R,out,"        $sizeexpr");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    fn encode(&self, buf: &mut [u8]) -> usize {");
    if (tail) {
        Ins(&R,out,"        let length = self.size();");
    }
    Ins(&R,out,"        let buf = &mut buf[..$length];");
    tempstr prev;
    int itail = 0;
    ind_beg(algo::StringAry_ary_curs, name, tailname) {
        itail++;
        bool last = itail == ary_N(tailname);
        tempstr beg;
        beg << csize << (ch_N(prev) ? tempstr() << " + " << prev : tempstr());
        Set(R,"$tailbeg",beg);
        Set(R,"$tailmember",RsIdent(name));
        if (last) {
            Ins(&R,out,"        buf[$tailbeg..].copy_from_slice(self.$tailmember);");
        } else {
            tempstr end;
            end << "e_" << name;
            tempstr sum;
            if (ch_N(prev)) {
                sum << prev << " + ";
            }
            sum << "self." << RsIdent(name) << ".len()";
            Set(R,"$endname",end);
            Set(R,"$endsum",sum);
            Ins(&R,out,"        let $endname = $endsum;");
            Ins(&R,out,"        buf[$tailbeg..$csize + $endname].copy_from_slice(self.$tailmember);");
            prev = end;
        }
    }ind_end;
    int offset = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        amc::FCtype &arg = *field.p_arg;
        bool bigend = field.c_fbigend != NULL;
        tempstr off;
        off << offset;
        tempstr member;
        member << "self." << RsIdent(name_Get(field));
        if (field.c_typefld && ctype.c_msgtype) {
            RsPutStmt(out, 8, arg, bigend, off, tempstr() << ctype.c_msgtype->type << RsScalarType(arg));
        } else if (field.c_lenfld && ctype.c_msgtype) {
            amc::FLenfld &lenfld = *field.c_lenfld;
            if (tail) {
                u64 maxlen = u64_Min(amc::LenfldMaxLen(lenfld), amc::FrameLenMax());
                tempstr cond;
                cond << "length > " << maxlen;
                if (amc::LenfldLowGuardNeededQ(lenfld)) {
                    cond << " || length < " << -lenfld.extra;
                }
                if (lenfld.scale != 1) {
                    cond << " || (length as i64 + (" << lenfld.extra << ")) % " << lenfld.scale << " != 0";
                }
                Set(R,"$lencond",cond);
                Ins(&R,out,"        if $lencond {");
                Ins(&R,out,"            wire::unrepresentable(\"$ctypeid\", length);");
                Ins(&R,out,"        }");
            }
            tempstr stored;
            if (!tail) {
                stored << (i64(csize) + lenfld.extra) / lenfld.scale << RsScalarType(arg);
            } else if (lenfld.scale == 1 && lenfld.extra == 0) {
                stored << "(length as " << RsScalarType(arg) << ")";
            } else {
                stored << "(((length as i64 + (" << lenfld.extra << ")) / " << lenfld.scale << ") as " << RsScalarType(arg) << ")";
            }
            RsPutStmt(out, 8, arg, bigend, off, stored);
        } else if (!amc::ProjMemberQ(field) || amc::ProjTailQ(field)) {
            // a bitfield and a tail have no slot, and a Base field's slots are its own fields
        } else if (field.reftype == dmmeta_Reftype_reftype_Val && arg.c_bltin) {
            RsPutStmt(out, 8, arg, bigend, off, member);
        } else if (field.reftype == dmmeta_Reftype_reftype_Val || amc::ProjRpascalQ(field) || amc::ProjRightpadQ(field)) {
            Set(R,"$member",member);
            Set(R,"$offset",off);
            Ins(&R,out,"        $member.encode(&mut buf[$offset..]);");
        } else if (arg.c_bltin && (arg.ctype == "u8" || arg.ctype == "char")) {
            Set(R,"$member",member);
            Set(R,"$range",RsRange(off, field.c_inlary->max));
            Ins(&R,out,"        buf[$range].copy_from_slice(&$member);");
        } else {
            tempstr elemoff;
            elemoff << offset << " + i * " << arg.totsize_byte;
            Set(R,"$member",member);
            Set(R,"$elemoff",elemoff);
            Ins(&R,out,"        for (i, x) in $member.iter().enumerate() {");
            if (!arg.c_bltin) {
                Ins(&R,out,"            x.encode(&mut buf[$elemoff..]);");
            } else {
                RsPutStmt(out, 12, arg, bigend, elemoff, arg.totsize_byte == 1 ? "*x" : "x");
            }
            Ins(&R,out,"        }");
        }
        offset += i32_Max(amc::ProjSlotBytes(field), 0);
    }ind_end;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        if (amc::ctype_zd_varlenfld_Next(field) && lenfield) {
            tempstr off;
            off << offset;
            tempstr value;
            value << "(e_" << name_Get(field) << " as " << RsScalarType(*lenfield->p_arg) << ")";
            RsPutStmt(out, 8, *lenfield->p_arg, lenfield->c_fbigend != NULL, off, value);
            offset += lenfield->p_arg->totsize_byte;
        }
    }ind_end;
    Set(R,"$retlength",length);
    Ins(&R,out,"        $retlength");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Append to OUT the statements of a test testing CTYPE: a value encodes to its
// size and decodes back to itself, once from its defaults and, when CTYPE has a
// tail, once with every tail filled with a different number of bytes, which puts
// every end offset at a different place.  A buffer one byte short is refused.
static void RsGenTestCtype(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    algo::strptr name = name_Get(ctype);
    int ntail = 0;
    int nfixed = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        if (amc::ProjTailQ(field)) {
            ntail++;
        } else {
            nfixed++;
        }
    }ind_end;
    for (int pass = 0; pass < (ntail ? 2 : 1); pass++) {
        Ins(&R,out,"");
        Set(R,"$ctypename",name);
        if (pass == 0) {
            Ins(&R,out,"        let value = $ctypename::default();");
        } else {
            algo::StringAry item;
            int itail = 0;
            ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjTailQ(field)) {
                itail++;
                algo::cstring &text = ary_Alloc(item);
                text << RsIdent(name_Get(field)) << ": b\"";
                char_PrintNTimes(char('0' + itail), text, itail);
                text << "\"";
            }ind_end;
            if (nfixed) {
                // the members the literal leaves out take their defaults
                Ins(&R,out,"        let value = $ctypename {");
                ind_beg(algo::StringAry_ary_curs, text, item) {
                    Set(R,"$item",text);
                    Ins(&R,out,"            $item,");
                }ind_end;
                Ins(&R,out,"            ..Default::default()");
                Ins(&R,out,"        };");
            } else {
                RsStructLit(out, 8, "let value = ", name, item, ";");
            }
        }
        Ins(&R,out,"        let mut buf = Vec::new();");
        Ins(&R,out,"        assert_eq!(value.encode_into(&mut buf), value.size());");
        Ins(&R,out,"        assert_eq!($ctypename::decode(&buf), Ok(value));");
        Ins(&R,out,"        assert!($ctypename::decode(&buf[..buf.len() - 1]).is_err());");
    }
}

// -----------------------------------------------------------------------------

// Append to OUT the test module of the module of namespace NS: one test, run by
// cargo test, round-tripping every ctype of NS projected into LANG with a codec.
static void RsGenTest(cstring &out, amc::FNs &ns, amc::FLang &lang) {
    algo_lib::Replscope R;
    Ins(&R,out,"");
    Ins(&R,out,"#[cfg(test)]");
    Ins(&R,out,"mod test {");
    Ins(&R,out,"    use super::*;");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Every struct of the module encodes to its size and decodes back to itself.");
    Ins(&R,out,"    #[test]");
    Ins(&R,out,"    fn roundtrip() {");
    Ins(&R,out,"        use crate::Encode;");
    ind_beg(amc::ns_c_ctype_curs, ctype, ns) if (amc::ProjCtypeQ(ctype, lang) && amc::ProjCodecQ(ctype)) {
        RsGenTestCtype(out, ctype);
    }ind_end;
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Return the namespace whose module is the root of the crate LANG is projected
// into: the first namespace, in namespace order, holding a dispatch a displang
// row projects into LANG, or NULL when there is none.
static amc::FNs *RsRootNs(amc::FLang &lang) {
    amc::FNs *ret = NULL;
    ind_beg(amc::_db_ns_curs, cand, amc::_db) {
        bool dispatch = false;
        ind_beg(amc::ns_c_dispatch_curs, disp, cand) {
            dispatch = dispatch || amc::ProjDispatchQ(disp, lang);
        }ind_end;
        if (!ret && dispatch) {
            ret = &cand;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Append to OUT the crate root that precedes the module of namespace ROOT: the
// module of every other namespace projected into LANG, DecodeError, the Encode
// trait and the wire helpers the generated codecs call.
static void RsGenRoot(cstring &out, amc::FLang &lang, amc::FNs &root) {
    algo_lib::Replscope R;
    Ins(&R,out,"// Code generated by amc. DO NOT EDIT.");
    Ins(&R,out,"");
    Ins(&R,out,"//! Codecs of the projected protocol, one module per namespace, laid out byte for");
    Ins(&R,out,"//! byte as the C++ structs are.  A struct with a tail borrows the tail from the");
    Ins(&R,out,"//! buffer it was decoded from, so decoding copies nothing.");
    Ins(&R,out,"");
    // a member or an accessor is spelled as the schema spells its field, MHz
    // and _throw among them, which Rust's case lint would reject
    Ins(&R,out,"#![allow(non_snake_case)]");
    Ins(&R,out,"");
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (&ns != &root && amc::ProjNsQ(ns, lang)) {
        Set(R,"$depns",ns.ns);
        Ins(&R,out,"#[path = \"$depns_gen.rs\"]");
        Ins(&R,out,"pub mod $depns;");
    }ind_end;
    Ins(&R,out,"");
    Ins(&R,out,"use std::fmt;");
    Ins(&R,out,"");
    Ins(&R,out,"/// The reason a buffer does not decode as the ctype asked for.");
    Ins(&R,out,"#[derive(Debug, Clone, Copy, PartialEq, Eq)]");
    Ins(&R,out,"pub enum DecodeError {");
    Ins(&R,out,"    /// The buffer is shorter than the fixed region of CTYPE.");
    Ins(&R,out,"    Short {");
    Ins(&R,out,"        ctype: &'static str,");
    Ins(&R,out,"        have: usize,");
    Ins(&R,out,"        want: usize,");
    Ins(&R,out,"    },");
    Ins(&R,out,"    /// The type word of CTYPE holds another message's type.");
    Ins(&R,out,"    Type {");
    Ins(&R,out,"        ctype: &'static str,");
    Ins(&R,out,"        have: u64,");
    Ins(&R,out,"        want: u64,");
    Ins(&R,out,"    },");
    Ins(&R,out,"    /// The frame length of CTYPE is below its fixed region or past the buffer.");
    Ins(&R,out,"    Length {");
    Ins(&R,out,"        ctype: &'static str,");
    Ins(&R,out,"        length: i64,");
    Ins(&R,out,"        fixed: usize,");
    Ins(&R,out,"        have: usize,");
    Ins(&R,out,"    },");
    Ins(&R,out,"    /// The end offset of varlen FIELD lies outside the tail.");
    Ins(&R,out,"    End { field: &'static str, end: usize },");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl fmt::Display for DecodeError {");
    Ins(&R,out,"    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {");
    Ins(&R,out,"        match *self {");
    Ins(&R,out,"            DecodeError::Short { ctype, have, want } => {");
    Ins(&R,out,"                write!(f, \"{ctype}: {have} bytes, want at least {want}\")");
    Ins(&R,out,"            }");
    Ins(&R,out,"            DecodeError::Type { ctype, have, want } => {");
    Ins(&R,out,"                write!(f, \"{ctype}: message type {have}, want {want}\")");
    Ins(&R,out,"            }");
    Ins(&R,out,"            DecodeError::Length {");
    Ins(&R,out,"                ctype,");
    Ins(&R,out,"                length,");
    Ins(&R,out,"                fixed,");
    Ins(&R,out,"                have,");
    Ins(&R,out,"            } => write!(f, \"{ctype}: frame length {length} outside [{fixed},{have}]\"),");
    Ins(&R,out,"            DecodeError::End { field, end } => write!(f, \"{field}: end {end} outside the tail\"),");
    Ins(&R,out,"        }");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl std::error::Error for DecodeError {}");
    Ins(&R,out,"");
    Ins(&R,out,"/// The wire form of a value: its size, and the bytes it writes.");
    Ins(&R,out,"pub trait Encode {");
    Ins(&R,out,"    /// Returns the bytes the value occupies on the wire.");
    Ins(&R,out,"    fn size(&self) -> usize;");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Writes the wire form at the head of BUF and returns its byte count, which is");
    Ins(&R,out,"    /// size().  Panics when BUF is shorter.");
    Ins(&R,out,"    fn encode(&self, buf: &mut [u8]) -> usize;");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Appends the wire form to OUT and returns its byte count.");
    Ins(&R,out,"    fn encode_into(&self, out: &mut Vec<u8>) -> usize {");
    Ins(&R,out,"        let start = out.len();");
    Ins(&R,out,"        out.resize(start + self.size(), 0);");
    Ins(&R,out,"        self.encode(&mut out[start..])");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"/// An rpascal inline string of at most N bytes.  Its wire form is N + 2 bytes:");
    Ins(&R,out,"/// the characters, a spare byte, and the count of characters.");
    Ins(&R,out,"#[derive(Clone, Copy, PartialEq, Eq)]");
    Ins(&R,out,"pub struct Rpascal<const N: usize> {");
    Ins(&R,out,"    ch: [u8; N],");
    Ins(&R,out,"    n: u8,");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize> Rpascal<N> {");
    Ins(&R,out,"    /// Returns TEXT as a string, cut to its first N bytes.");
    Ins(&R,out,"    pub fn new(text: &[u8]) -> Self {");
    Ins(&R,out,"        let mut ret = Self::default();");
    Ins(&R,out,"        let n = text.len().min(N);");
    Ins(&R,out,"        ret.ch[..n].copy_from_slice(&text[..n]);");
    Ins(&R,out,"        ret.n = n as u8;");
    Ins(&R,out,"        ret");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Returns the characters of the string.");
    Ins(&R,out,"    pub fn as_bytes(&self) -> &[u8] {");
    Ins(&R,out,"        &self.ch[..self.n as usize]");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Reads the wire form at the head of BUF, holding a count above N to N; panics");
    Ins(&R,out,"    /// when BUF is shorter than N + 2.");
    Ins(&R,out,"    pub fn decode_fixed(buf: &[u8]) -> Self {");
    Ins(&R,out,"        Self::new(&buf[..(buf[N + 1] as usize).min(N)])");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize> Default for Rpascal<N> {");
    Ins(&R,out,"    fn default() -> Self {");
    Ins(&R,out,"        Rpascal { ch: [0; N], n: 0 }");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize> fmt::Debug for Rpascal<N> {");
    Ins(&R,out,"    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {");
    Ins(&R,out,"        fmt::Debug::fmt(&String::from_utf8_lossy(self.as_bytes()), f)");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize> Encode for Rpascal<N> {");
    Ins(&R,out,"    fn size(&self) -> usize {");
    Ins(&R,out,"        N + 2");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    fn encode(&self, buf: &mut [u8]) -> usize {");
    Ins(&R,out,"        buf[..N].copy_from_slice(&self.ch);");
    Ins(&R,out,"        buf[N] = 0;");
    Ins(&R,out,"        buf[N + 1] = self.n;");
    Ins(&R,out,"        N + 2");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"/// A rightpad inline string of at most N bytes.  Its wire form is N bytes: the");
    Ins(&R,out,"/// characters, padded on the right with PAD, which reading strips back off.");
    Ins(&R,out,"#[derive(Clone, Copy, PartialEq, Eq)]");
    Ins(&R,out,"pub struct Rightpad<const N: usize, const PAD: u8> {");
    Ins(&R,out,"    ch: [u8; N],");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize, const PAD: u8> Rightpad<N, PAD> {");
    Ins(&R,out,"    /// Returns TEXT as a string, cut to its first N bytes.");
    Ins(&R,out,"    pub fn new(text: &[u8]) -> Self {");
    Ins(&R,out,"        let mut ret = Self::default();");
    Ins(&R,out,"        let n = text.len().min(N);");
    Ins(&R,out,"        ret.ch[..n].copy_from_slice(&text[..n]);");
    Ins(&R,out,"        ret");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Returns the characters of the string, without the trailing pad.");
    Ins(&R,out,"    pub fn as_bytes(&self) -> &[u8] {");
    Ins(&R,out,"        let mut n = N;");
    Ins(&R,out,"        while n > 0 && self.ch[n - 1] == PAD {");
    Ins(&R,out,"            n -= 1;");
    Ins(&R,out,"        }");
    Ins(&R,out,"        &self.ch[..n]");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Reads the wire form at the head of BUF; panics when BUF is shorter than N.");
    Ins(&R,out,"    pub fn decode_fixed(buf: &[u8]) -> Self {");
    Ins(&R,out,"        Self::new(&buf[..N])");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize, const PAD: u8> Default for Rightpad<N, PAD> {");
    Ins(&R,out,"    fn default() -> Self {");
    Ins(&R,out,"        Rightpad { ch: [PAD; N] }");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize, const PAD: u8> fmt::Debug for Rightpad<N, PAD> {");
    Ins(&R,out,"    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {");
    Ins(&R,out,"        fmt::Debug::fmt(&String::from_utf8_lossy(self.as_bytes()), f)");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"impl<const N: usize, const PAD: u8> Encode for Rightpad<N, PAD> {");
    Ins(&R,out,"    fn size(&self) -> usize {");
    Ins(&R,out,"        N");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    fn encode(&self, buf: &mut [u8]) -> usize {");
    Ins(&R,out,"        buf[..N].copy_from_slice(&self.ch);");
    Ins(&R,out,"        N");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
    Ins(&R,out,"");
    Ins(&R,out,"/// Helpers the generated codecs call.");
    Ins(&R,out,"#[doc(hidden)]");
    Ins(&R,out,"pub mod wire {");
    Ins(&R,out,"    use super::DecodeError;");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Returns the N bytes of BUF at OFF; panics when BUF holds fewer.");
    Ins(&R,out,"    #[inline]");
    Ins(&R,out,"    pub fn get<const N: usize>(buf: &[u8], off: usize) -> [u8; N] {");
    Ins(&R,out,"        let mut ret = [0; N];");
    Ins(&R,out,"        ret.copy_from_slice(&buf[off..off + N]);");
    Ins(&R,out,"        ret");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Refuses BUF when it holds fewer than WANT bytes, the fixed region of CTYPE.");
    Ins(&R,out,"    #[inline]");
    Ins(&R,out,"    pub fn check_size(buf: &[u8], want: usize, ctype: &'static str) -> Result<(), DecodeError> {");
    Ins(&R,out,"        if buf.len() < want {");
    Ins(&R,out,"            let have = buf.len();");
    Ins(&R,out,"            Err(DecodeError::Short { ctype, have, want })");
    Ins(&R,out,"        } else {");
    Ins(&R,out,"            Ok(())");
    Ins(&R,out,"        }");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Refuses type word HAVE of CTYPE unless it is WANT.");
    Ins(&R,out,"    #[inline]");
    Ins(&R,out,"    pub fn check_type(have: u64, want: u64, ctype: &'static str) -> Result<(), DecodeError> {");
    Ins(&R,out,"        if have != want {");
    Ins(&R,out,"            Err(DecodeError::Type { ctype, have, want })");
    Ins(&R,out,"        } else {");
    Ins(&R,out,"            Ok(())");
    Ins(&R,out,"        }");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Returns frame length LENGTH of CTYPE, refused unless it covers the fixed");
    Ins(&R,out,"    /// region FIXED and fits in BUF.");
    Ins(&R,out,"    #[inline]");
    Ins(&R,out,"    pub fn check_length(");
    Ins(&R,out,"        buf: &[u8],");
    Ins(&R,out,"        length: i64,");
    Ins(&R,out,"        fixed: usize,");
    Ins(&R,out,"        ctype: &'static str,");
    Ins(&R,out,"    ) -> Result<usize, DecodeError> {");
    Ins(&R,out,"        let have = buf.len();");
    Ins(&R,out,"        if length < fixed as i64 || length > have as i64 {");
    Ins(&R,out,"            Err(DecodeError::Length {");
    Ins(&R,out,"                ctype,");
    Ins(&R,out,"                length,");
    Ins(&R,out,"                fixed,");
    Ins(&R,out,"                have,");
    Ins(&R,out,"            })");
    Ins(&R,out,"        } else {");
    Ins(&R,out,"            Ok(length as usize)");
    Ins(&R,out,"        }");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Refuses end offset END of varlen FIELD unless it lies in [PREV, LIMIT].");
    Ins(&R,out,"    #[inline]");
    Ins(&R,out,"    pub fn check_end(");
    Ins(&R,out,"        end: usize,");
    Ins(&R,out,"        prev: usize,");
    Ins(&R,out,"        limit: usize,");
    Ins(&R,out,"        field: &'static str,");
    Ins(&R,out,"    ) -> Result<usize, DecodeError> {");
    Ins(&R,out,"        if end < prev || end > limit {");
    Ins(&R,out,"            Err(DecodeError::End { field, end })");
    Ins(&R,out,"        } else {");
    Ins(&R,out,"            Ok(end)");
    Ins(&R,out,"        }");
    Ins(&R,out,"    }");
    Ins(&R,out,"");
    Ins(&R,out,"    /// Panics: frame length LENGTH of CTYPE has no form in its length word.");
    Ins(&R,out,"    #[cold]");
    Ins(&R,out,"    pub fn unrepresentable(ctype: &'static str, length: usize) -> ! {");
    Ins(&R,out,"        panic!(\"{ctype}: frame length {length} is not representable in the length field\")");
    Ins(&R,out,"    }");
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Write rs/gen/src/<ns>_gen.rs for the current namespace: a struct, its
// constants, its Default, decode and Encode per ctype of the projection, and the
// signature of each dispatch of the namespace a displang row projects into Rust.
// The module of the first namespace holding such a dispatch is the crate root,
// and a second namespace holding one is refused, since a crate has one root.  A
// namespace with nothing to project writes nothing.
void amc::gen_lang_rs() {
    algo_lib::Replscope R;
    amc::FNs &ns = *amc::_db.c_ns;
    amc::FLang *lang = amc::ind_lang_Find(amcdb_lang_rs);
    if (!amc::_db.proj_refuse && lang && amc::ProjNsQ(ns, *lang)) {
        amc::FNs *root = RsRootNs(*lang);
        cstring body;
        bool anycodec = false;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) if (amc::ProjCtypeQ(ctype, *lang)) {
            if (!RsCheckName(ctype)) {
                algo_lib::_db.exit_code++;
            }
            RsGenStruct(body, ns, root, ctype);
            RsGenConst(body, ctype);
            if (amc::ProjDfltQ(ctype)) {
                RsGenDefault(body, ns, root, ctype);
            }
            if (amc::ProjCodecQ(ctype)) {
                anycodec = true;
                RsGenImpl(body, ns, root, ctype);
                RsGenEncode(body, ctype);
            }
            if (amc::ProjLayoutQ(ctype)) {
                RsGenLayout(body, ctype);
            }
        }ind_end;
        ind_beg(amc::ns_c_dispatch_curs, dispatch, ns) if (amc::ProjDispatchQ(dispatch, *lang)) {
            if (root != &ns) {
                prerr("amc.rs_root"
                      <<Keyval("dispatch",dispatch.dispatch)
                      <<Keyval("root",root->ns)
                      <<Keyval("comment","Rust projects dispatches of one namespace, whose module is the crate root"));
                algo_lib::_db.exit_code++;
            }
            tempstr bytes;
            for (int i = 0; i < 20; i++) {
                bytes << "\\x";
                algo::u64_PrintHex(dispatch.signature.signature_elems[i], bytes, 2, false);
            }
            body << eol;
            body << "/// The signature of dispatch " << dispatch.dispatch << ", which a peer built from the" << eol;
            body << "/// same schema presents and expects." << eol;
            body << "#[allow(non_upper_case_globals)]" << eol;
            body << "pub const " << name_Get(dispatch) << "_Signature: [u8; 20] =" << eol;
            body << "    *b\"" << bytes << "\";" << eol;
        }ind_end;
        if (anycodec) {
            RsGenTest(body, ns, *lang);
        }
        // a namespace new to the projection may have no directory yet, and the
        // write at the end of the run does not make one
        algo::CreateDirRecurse("rs/gen/src");
        cstring &out = amc::outfile_Create(tempstr() << "rs/gen/src/" << ns.ns << "_gen.rs").text;
        if (root == &ns) {
            RsGenRoot(out, *lang, ns);
        } else {
            Ins(&R,out,"// Code generated by amc. DO NOT EDIT.");
            if (anycodec) {
                Ins(&R,out,"");
                Ins(&R,out,"use crate::{DecodeError, Encode, wire};");
            }
        }
        out << body;
    }
}
