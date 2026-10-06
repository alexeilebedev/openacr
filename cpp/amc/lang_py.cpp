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
// Source: cpp/amc/lang_py.cpp
//
// A dmmeta.displang row whose language is py names a dispatch whose messages a
// Python program speaks.  proj.cpp collects the projection and checks its
// layout; this file writes it.  One module is written per namespace,
// py/gen/<ns>_gen.py, and the modules import each other relatively, so py/gen
// is a package.  Each projected ctype becomes a class with __slots__, a
// constructor taking every member's schema default, size, encode_into and a
// decode classmethod.  The fixed region of a ctype, nested ctypes included, is
// one precompiled little-endian struct.Struct, so a message is packed or
// unpacked by one call and its nested members are rebuilt from the tuple
// without running their constructors.  A tail is a slice of the decoded
// buffer, which aliases it when the buffer is a memoryview.  The dispatch
// signature is emitted as bytes beside the dispatch's own namespace.

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Return the struct format character of builtin ARG, a scalar with a wire form.
static char PyFmtChar(amc::FCtype &arg) {
    char ret = 'B';
    if (arg.ctype == "i8") {
        ret = 'b';
    } else if (arg.ctype == "u16") {
        ret = 'H';
    } else if (arg.ctype == "i16") {
        ret = 'h';
    } else if (arg.ctype == "u32") {
        ret = 'I';
    } else if (arg.ctype == "i32") {
        ret = 'i';
    } else if (arg.ctype == "u64") {
        ret = 'Q';
    } else if (arg.ctype == "i64") {
        ret = 'q';
    } else if (arg.ctype == "bool") {
        ret = '?';
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when a scalar of builtin ARG stored big-endian, as BIGEND says, is
// carried through the struct as raw bytes: a big-endian word wider than a byte.
static bool PyRawQ(amc::FCtype &arg, bool bigend) {
    return bigend && arg.totsize_byte > 1;
}

// -----------------------------------------------------------------------------

// Append to FMT the format of one scalar of builtin ARG, stored big-endian when
// BIGEND is set.
static void PyFmtScalar(cstring &fmt, amc::FCtype &arg, bool bigend) {
    if (PyRawQ(arg, bigend)) {
        fmt << arg.totsize_byte << 's';
    } else {
        fmt << PyFmtChar(arg);
    }
}

// -----------------------------------------------------------------------------

// Return the Python expression packing integer expression VALUE as a scalar of
// builtin ARG stored big-endian when BIGEND is set.
static tempstr PyPutExpr(amc::FCtype &arg, bool bigend, algo::strptr value) {
    tempstr ret;
    if (PyRawQ(arg, bigend)) {
        ret << "(" << value << ").to_bytes(" << arg.totsize_byte << ", 'big', signed=" << (StartsWithQ(arg.ctype, "i") ? "True" : "False") << ")";
    } else {
        ret << value;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Python expression reading unpacked item ITEM as a scalar of builtin
// ARG stored big-endian when BIGEND is set.
static tempstr PyGetExpr(amc::FCtype &arg, bool bigend, algo::strptr item) {
    tempstr ret;
    if (PyRawQ(arg, bigend)) {
        ret << "int.from_bytes(" << item << ", 'big', signed=" << (StartsWithQ(arg.ctype, "i") ? "True" : "False") << ")";
    } else {
        ret << item;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when TEXT is a Python keyword, which no attribute may be named.
static bool PyKeywordQ(algo::strptr text) {
    return text == "False" || text == "None" || text == "True" || text == "and" || text == "as"
        || text == "assert" || text == "async" || text == "await" || text == "break" || text == "class"
        || text == "continue" || text == "def" || text == "del" || text == "elif" || text == "else"
        || text == "except" || text == "finally" || text == "for" || text == "from" || text == "global"
        || text == "if" || text == "import" || text == "in" || text == "is" || text == "lambda"
        || text == "nonlocal" || text == "not" || text == "or" || text == "pass" || text == "raise"
        || text == "return" || text == "try" || text == "while" || text == "with" || text == "yield";
}

// -----------------------------------------------------------------------------

// Return the Python attribute name of FIELD: its schema name, with an underscore
// appended when the name is a Python keyword, or names a member the codec of its
// ctype puts on the class.
static tempstr PyName(amc::FField &field) {
    tempstr ret(name_Get(field));
    bool method = amc::ProjCodecQ(*field.p_ctype)
        && (ret == "size" || ret == "encode_into" || ret == "decode" || ret == "TYPE");
    if (PyKeywordQ(ret) || method) {
        ret << "_";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Python expression naming the class of ctype ARG as seen from the
// module of namespace NS: the bare class name for a ctype of NS, and the name
// qualified by the other namespace's module otherwise.
static tempstr PyTypeRef(amc::FNs &ns, amc::FCtype &arg) {
    tempstr ret;
    if (arg.p_ns != &ns) {
        ret << arg.p_ns->ns << "_gen.";
    }
    ret << name_Get(arg);
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Python literal of schema default DFLT for a scalar of builtin ARG:
// a quoted char becomes its code, true and false become True and False, and an
// empty default is the zero of the type.
static tempstr PyLiteral(amc::FCtype &arg, algo::strptr dflt) {
    tempstr ret;
    bool zero = ch_N(dflt) == 0 || dflt == "\"\"";
    if (arg.ctype == "bool") {
        ret << (dflt == "true" || dflt == "1" ? "True" : "False");
    } else if (zero || dflt == "false") {
        ret << "0";
    } else if (dflt == "true") {
        ret << "1";
    } else if (elems_N(dflt) == 3 && dflt.elems[0] == '\'') {
        ret << int(u8(dflt.elems[1]));
    } else {
        ret << dflt;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE when length field LENFLD stores the frame length as it is, with no
// scale and no extra, so the encoder packs the length itself.
static bool PyPlainLenfldQ(amc::FLenfld &lenfld) {
    return lenfld.scale == 1 && lenfld.extra == 0;
}

// -----------------------------------------------------------------------------

// Walk the fixed region of CTYPE, laid out in the module of namespace NS, and
// build the three parts of its codec.  FMT receives the struct format of every
// slot, ARGS the comma-separated expressions packing them from the object PATH
// names, and DEC the statements that set the members of the object VAR names
// from the unpacked tuple t.  IDX is the tuple index of the next slot and NTEMP
// numbers the temporaries a nested member is rebuilt in.  TOP is set for the
// ctype being coded, whose type word packs the message type, whose length word
// packs the frame length or the local lenword, and whose end offsets pack the
// locals e_<name>.  A nested ctype has no tail, so its words pack constants.
// CHECK receives the statements refusing a frame whose type or length word is
// wrong.
static void PyFlatten(amc::FNs &ns, amc::FCtype &ctype, algo::strptr path, algo::strptr var, bool top, cstring &fmt, cstring &args, cstring &dec, cstring &check, int &idx, int &ntemp) {
    algo_lib::Replscope R;
    Set(R,"$flatctype",ctype.ctype);
    Set(R,"$totsize",tempstr() << ctype.totsize_byte);
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjSlotBytes(field) > 0) {
        amc::FCtype &arg = *field.p_arg;
        bool bigend = field.c_fbigend != NULL;
        tempstr member;
        member << path << "." << PyName(field);
        tempstr item;
        item << "t[" << idx << "]";
        Set(R,"$fldvar",var);
        Set(R,"$fldname",PyName(field));
        Set(R,"$flditem",item);
        if (field.c_typefld && ctype.c_msgtype) {
            PyFmtScalar(fmt, arg, bigend);
            args << ", " << PyPutExpr(arg, bigend, tempstr() << ctype.c_msgtype->type);
            if (top) {
                Set(R,"$fldget",PyGetExpr(arg, bigend, item));
                Set(R,"$msgid",tempstr() << ctype.c_msgtype->type);
                Ins(&R,check,"        if $fldget != $msgid:");
                Ins(&R,check,"            raise ValueError('$flatctype: message type %d, want $msgid' % $fldget)");
            }
            idx++;
        } else if (field.c_lenfld && ctype.c_msgtype) {
            amc::FLenfld &lenfld = *field.c_lenfld;
            PyFmtScalar(fmt, arg, bigend);
            tempstr stored;
            if (!top) {
                stored << ctype.totsize_byte;
            } else if (PyPlainLenfldQ(lenfld)) {
                stored << "length";
            } else {
                stored << "lenword";
            }
            args << ", " << PyPutExpr(arg, bigend, stored);
            if (top) {
                tempstr expand;
                expand << PyGetExpr(arg, bigend, item);
                if (lenfld.scale != 1) {
                    expand << " * " << lenfld.scale;
                }
                if (lenfld.extra != 0) {
                    expand << " - (" << lenfld.extra << ")";
                }
                Set(R,"$expand",expand);
                Ins(&R,check,"        length = $expand");
                Ins(&R,check,"        if length < $totsize or length > n:");
                Ins(&R,check,"            raise ValueError('$flatctype: frame length %d outside [$totsize,%d]' % (length, n))");
            }
            idx++;
        } else if (amc::ProjRpascalQ(field)) {
            // an inline string: its characters, a spare byte, and the count.  struct
            // pads a short value with NULs and cuts a long one to the length, so
            // the count is cut the same way and never exceeds what was stored
            fmt << field.c_smallstr->length << "sxB";
            args << ", " << member << ", min(len(" << member << "), " << field.c_smallstr->length << ")";
            Set(R,"$lenidx",tempstr() << idx + 1);
            Ins(&R,dec,"        $fldvar.$fldname = $flditem[:t[$lenidx]]");
            idx += 2;
        } else if (amc::ProjRightpadQ(field)) {
            // an inline string: its characters, padded on the right with the pad
            // byte, which decode strips back off
            tempstr pad;
            pad << "bytes([" << amc::ProjPadByte(field) << "])";
            fmt << field.c_smallstr->length << "s";
            args << ", " << member << ".ljust(" << field.c_smallstr->length << ", " << pad << ")";
            Set(R,"$pad",pad);
            Ins(&R,dec,"        $fldvar.$fldname = $flditem.rstrip($pad)");
            idx++;
        } else if (field.reftype == dmmeta_Reftype_reftype_Val && arg.c_bltin) {
            PyFmtScalar(fmt, arg, bigend);
            args << ", " << PyPutExpr(arg, bigend, member);
            Set(R,"$fldget",PyGetExpr(arg, bigend, item));
            Ins(&R,dec,"        $fldvar.$fldname = $fldget");
            idx++;
        } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
            tempstr temp;
            ntemp++;
            temp << "v" << ntemp;
            Set(R,"$temp",temp);
            Set(R,"$typeref",PyTypeRef(ns, arg));
            Ins(&R,dec,"        $temp = _new($typeref)");
            PyFlatten(ns, arg, member, temp, false, fmt, args, dec, check, idx, ntemp);
            Set(R,"$fldvar",var);
            Set(R,"$fldname",PyName(field));
            Set(R,"$temp",temp);
            Ins(&R,dec,"        $fldvar.$fldname = $temp");
        } else if (field.reftype == dmmeta_Reftype_reftype_Inlary && arg.c_bltin && arg.totsize_byte == 1 && arg.ctype != "bool" && arg.ctype != "i8") {
            // an array of bytes travels as one bytes object
            fmt << field.c_inlary->max << 's';
            args << ", " << member;
            Ins(&R,dec,"        $fldvar.$fldname = $flditem");
            idx++;
        } else if (field.reftype == dmmeta_Reftype_reftype_Inlary && arg.c_bltin) {
            int nelem = field.c_inlary->max;
            for (int i = 0; i < nelem; i++) {
                PyFmtScalar(fmt, arg, bigend);
            }
            tempstr slice;
            slice << "t[" << idx << ":" << idx + nelem << "]";
            Set(R,"$slice",slice);
            if (PyRawQ(arg, bigend)) {
                args << ", *[" << PyPutExpr(arg, bigend, "x") << " for x in " << member << "]";
                Set(R,"$fldget",PyGetExpr(arg, bigend, "x"));
                Ins(&R,dec,"        $fldvar.$fldname = [$fldget for x in $slice]");
            } else {
                args << ", *" << member;
                Ins(&R,dec,"        $fldvar.$fldname = list($slice)");
            }
            idx += nelem;
        } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
            algo::ListSep ls(", ");
            tempstr elems;
            for (int i = 0; i < field.c_inlary->max; i++) {
                tempstr temp;
                ntemp++;
                temp << "v" << ntemp;
                elems << ls << temp;
                Set(R,"$temp",temp);
                Set(R,"$typeref",PyTypeRef(ns, arg));
                Ins(&R,dec,"        $temp = _new($typeref)");
                PyFlatten(ns, arg, tempstr() << member << "[" << i << "]", temp, false, fmt, args, dec, check, idx, ntemp);
            }
            Set(R,"$fldvar",var);
            Set(R,"$fldname",PyName(field));
            Set(R,"$elems",elems);
            Ins(&R,dec,"        $fldvar.$fldname = [$elems]");
        }
    }ind_end;
    amc::FField *lenfield = amc::LengthField(ctype);
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        if (top && amc::ctype_zd_varlenfld_Next(field) && lenfield) {
            bool bigend = lenfield->c_fbigend != NULL;
            PyFmtScalar(fmt, *lenfield->p_arg, bigend);
            args << ", " << PyPutExpr(*lenfield->p_arg, bigend, tempstr() << "e_" << name_Get(field));
            Set(R,"$endname",tempstr() << "e_" << name_Get(field));
            Set(R,"$endget",PyGetExpr(*lenfield->p_arg, bigend, tempstr() << "t[" << idx << "]"));
            Ins(&R,dec,"        $endname = $endget");
            idx++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Emit into OUT the named constants of CTYPE: every fconst of a scalar member,
// named as the Go projection names it.  The fconsts of a type word are the
// message types of the whole schema, and each message states its own, so those
// are left out.
static void PyGenConst(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        bool want = amc::ProjMemberQ(field) && !field.c_typefld && field.p_arg->c_bltin && amc::c_fconst_N(field) > 0;
        if (want) {
            tempstr prefix;
            prefix << name_Get(ctype);
            if (!field.c_anonfld) {
                prefix << "_" << name_Get(field);
            }
            bool unsig = StartsWithQ(field.arg, "u");
            ind_beg(amc::field_c_fconst_curs, fconst, field) {
                tempstr value;
                if (unsig) {
                    value << u64(fconst.int_val);
                } else {
                    value << fconst.int_val;
                }
                Set(R,"$constname",tempstr() << prefix << "_" << amc::strptr_ToCppIdent(name_Get(fconst),true));
                Set(R,"$constval",value);
                Ins(&R,out,"$constname = $constval");
            }ind_end;
            Ins(&R,out,"");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Emit into OUT the class head of CTYPE for the module of namespace NS: its
// comment, its __slots__, its message type, and a constructor whose parameters
// default to the schema defaults.  A nested member defaults to a fresh instance
// of its class, and an inline array of bytes to its bytes.  A type-only CTYPE
// declares a string member as a str, and starts every member at its zero.
static void PyGenInit(cstring &out, amc::FNs &ns, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    tempstr title;
    title << name_Get(ctype) << " is " << ctype.ctype;
    if (ch_N(ctype.comment)) {
        title << ": " << ctype.comment;
    }
    Set(R,"$clsname",name_Get(ctype));
    Set(R,"$title",title);
    Ins(&R,out,"# $title");
    Ins(&R,out,"class $clsname:");
    Ins(&R,out,"    __slots__ = (");
    int nmember = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        nmember++;
        Set(R,"$slot",PyName(field));
        if (ch_N(field.comment)) {
            Set(R,"$memcomment",field.comment);
            Ins(&R,out,"        '$slot',  # $memcomment");
        } else {
            Ins(&R,out,"        '$slot',");
        }
    }ind_end;
    Ins(&R,out,"    )");
    if (ctype.c_msgtype) {
        Set(R,"$msgtype",tempstr() << ctype.c_msgtype->type);
        Ins(&R,out,"    TYPE = $msgtype");
    }
    Ins(&R,out,"");
    if (nmember == 0) {
        Ins(&R,out,"    def __init__(self):");
        Ins(&R,out,"        pass");
    } else {
        cstring body;
        bool typeonly = !amc::ProjDfltQ(ctype);
        Ins(&R,out,"    def __init__(self,");
        int imember = 0;
        ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
            amc::FCtype &arg = *field.p_arg;
            tempstr name = PyName(field);
            tempstr dflt;
            tempstr value;
            value << name;
            if (typeonly && amc::ProjStringQ(field)) {
                dflt << "''";
            } else if (typeonly && arg.c_bltin) {
                dflt << PyLiteral(arg, "");
            } else if (amc::ProjTailQ(field) || amc::ProjRpascalQ(field) || amc::ProjRightpadQ(field)) {
                dflt << "b''";
            } else if (amc::ProjNestedDfltQ(field)) {
                amc::FField &inner = amc::ProjNestedDfltField(field);
                dflt << "None";
                value = tempstr() << PyTypeRef(ns, arg) << "(" << PyName(inner) << "="
                                  << PyLiteral(*inner.p_arg, amc::ProjDflt(field)) << ") if " << name << " is None else " << name;
            } else if (field.reftype == dmmeta_Reftype_reftype_Val && !arg.c_bltin) {
                dflt << "None";
                value = tempstr() << PyTypeRef(ns, arg) << "() if " << name << " is None else " << name;
            } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
                dflt << PyLiteral(arg, amc::ProjDflt(field));
            } else if (arg.c_bltin && arg.totsize_byte == 1 && arg.ctype != "bool" && arg.ctype != "i8") {
                dflt << "bytes([" << PyLiteral(arg, amc::ProjDflt(field)) << "]) * " << field.c_inlary->max;
            } else if (arg.c_bltin) {
                dflt << "None";
                value = tempstr() << "[" << PyLiteral(arg, amc::ProjDflt(field)) << "] * " << field.c_inlary->max << " if " << name << " is None else " << name;
            } else {
                dflt << "None";
                value = tempstr() << "[" << PyTypeRef(ns, arg) << "() for _ in range(" << field.c_inlary->max << ")] if " << name << " is None else " << name;
            }
            imember++;
            Set(R,"$argname",name);
            Set(R,"$argdflt",dflt);
            Set(R,"$argvalue",value);
            if (imember < nmember) {
                Ins(&R,out,"                 $argname=$argdflt,");
            } else {
                Ins(&R,out,"                 $argname=$argdflt):");
            }
            Ins(&R,body,"        self.$argname = $argvalue");
        }ind_end;
        out << body;
    }
}

// -----------------------------------------------------------------------------

// Emit into OUT the memory layout of CTYPE after its class: its size and the
// offset of each member, as C++ lays the struct out, so a Python program can
// share a segment with a C++ one.
static void PyGenLayout(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    Set(R,"$clsname",name_Get(ctype));
    Set(R,"$ctypename",ctype.ctype);
    Set(R,"$sizeof",tempstr() << ctype.totsize_byte);
    out << eol << eol;
    Ins(&R,out,"# $clsname_Sizeof is the size of $ctypename in memory, and each $clsname_Off_");
    Ins(&R,out,"# constant the offset of a member, as C++ lays the struct out.");
    Ins(&R,out,"$clsname_Sizeof = $sizeof");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        Set(R,"$fldname",name_Get(field));
        Set(R,"$fldoff",tempstr() << field.offset);
        Ins(&R,out,"$clsname_Off_$fldname = $fldoff");
    }ind_end;
}

// -----------------------------------------------------------------------------

// Emit into OUT the size, encode_into and decode methods of CTYPE for the module
// of namespace NS, and before the class, into HEAD, the struct.Struct its fixed
// region is packed with.  Encode refuses a buffer too short for the frame, and
// decode refuses a frame whose type, length or end offsets are inconsistent,
// so a frame from a peer of another layout is an error and never a read past
// its end.
static void PyGenCodec(cstring &head, cstring &out, amc::FNs &ns, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    int csize = ctype.totsize_byte;
    tempstr sname;
    sname << "_S_" << name_Get(ctype);
    cstring fmt;
    cstring args;
    cstring dec;
    cstring check;
    int idx = 0;
    int ntemp = 0;
    PyFlatten(ns, ctype, "self", "self", true, fmt, args, dec, check, idx, ntemp);
    Set(R,"$struct",sname);
    Set(R,"$csize",tempstr() << csize);
    Set(R,"$ctypeid",ctype.ctype);
    Set(R,"$fmtstr",fmt);
    Ins(&R,head,"$struct = struct.Struct('<$fmtstr')");
    // the tails in wire order: the varlen fields, then an Opt field
    cstring taillen;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        taillen << " + len(self." << PyName(field) << ")";
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        taillen << " + len(self." << PyName(field) << ")";
    }ind_end;
    Set(R,"$taillen",taillen);
    Ins(&R,out,"");
    Ins(&R,out,"    def size(self):");
    Ins(&R,out,"        return $csize$taillen");
    // encode: every tail's length is taken once, and the end offsets the fixed
    // region carries are running sums of those lengths
    Ins(&R,out,"");
    Ins(&R,out,"    def encode_into(self, buf, offset=0):");
    tempstr prev_end;
    tempstr length;
    length << csize;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        Set(R,"$tailname",name_Get(field));
        Set(R,"$tailmember",PyName(field));
        Ins(&R,out,"        m_$tailname = self.$tailmember");
        Ins(&R,out,"        l_$tailname = len(m_$tailname)");
        if (amc::ctype_zd_varlenfld_Next(field)) {
            tempstr end;
            end << "e_" << name_Get(field);
            tempstr sum;
            if (ch_N(prev_end)) {
                sum << prev_end << " + ";
            }
            sum << "l_" << name_Get(field);
            Set(R,"$endname",end);
            Set(R,"$endsum",sum);
            Ins(&R,out,"        $endname = $endsum");
            prev_end = end;
        }
        length << " + l_" << name_Get(field);
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        Set(R,"$tailname",name_Get(field));
        Set(R,"$tailmember",PyName(field));
        Ins(&R,out,"        m_$tailname = self.$tailmember");
        Ins(&R,out,"        l_$tailname = len(m_$tailname)");
        length << " + l_" << name_Get(field);
    }ind_end;
    Set(R,"$length",length);
    Ins(&R,out,"        length = $length");
    Ins(&R,out,"        if len(buf) - offset < length:");
    Ins(&R,out,"            raise ValueError('$ctypeid: %d bytes, want %d' % (len(buf) - offset, length))");
    amc::FField *lenfield = amc::LengthField(ctype);
    if (ctype.c_msgtype && lenfield && lenfield->c_lenfld) {
        amc::FLenfld &lenfld = *lenfield->c_lenfld;
        u64 maxlen = u64_Min(amc::LenfldMaxLen(lenfld), amc::FrameLenMax());
        tempstr cond;
        tempstr stored("length");
        cond << "length > " << maxlen;
        if (amc::LenfldLowGuardNeededQ(lenfld)) {
            cond << " or length < " << -lenfld.extra;
        }
        if (lenfld.extra != 0) {
            stored = tempstr() << "(length + (" << lenfld.extra << "))";
        }
        if (lenfld.scale != 1) {
            cond << " or " << stored << " % " << lenfld.scale << " != 0";
            stored << " // " << lenfld.scale;
        }
        Set(R,"$lencond",cond);
        Ins(&R,out,"        if $lencond:");
        Ins(&R,out,"            raise ValueError('$ctypeid: frame length %d is not representable in the length field' % length)");
        if (!PyPlainLenfldQ(lenfld)) {
            Set(R,"$lenstored",stored);
            Ins(&R,out,"        lenword = $lenstored");
        }
    }
    Set(R,"$packargs",args);
    Ins(&R,out,"        $struct.pack_into(buf, offset$packargs)");
    bool anytail = amc::ProjAnyTailQ(ctype);
    if (anytail) {
        Ins(&R,out,"        pos = offset + $csize");
    }
    int ntail = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        ntail += amc::ProjTailQ(field) ? 1 : 0;
    }ind_end;
    int itail = 0;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        Set(R,"$tailname",name_Get(field));
        Ins(&R,out,"        buf[pos:pos + l_$tailname] = m_$tailname");
        itail++;
        if (itail < ntail) {
            Ins(&R,out,"        pos += l_$tailname");
        }
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        Set(R,"$tailname",name_Get(field));
        Ins(&R,out,"        buf[pos:pos + l_$tailname] = m_$tailname");
    }ind_end;
    Ins(&R,out,"        return length");
    Ins(&R,out,"");
    Ins(&R,out,"    @classmethod");
    Ins(&R,out,"    def decode(cls, buf, offset=0):");
    Ins(&R,out,"        n = len(buf) - offset");
    Ins(&R,out,"        if n < $csize:");
    Ins(&R,out,"            raise ValueError('$ctypeid: %d bytes, want at least $csize' % n)");
    Ins(&R,out,"        t = $struct.unpack_from(buf, offset)");
    out << check;
    if (anytail && !(ctype.c_msgtype && lenfield)) {
        // gen_check_proj refuses a tail on an unframed ctype
        Ins(&R,out,"        length = n");
    }
    Ins(&R,out,"        self = _new(cls)");
    out << dec;
    tempstr prev("0");
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        if (amc::ctype_zd_varlenfld_Next(field) && lenfield) {
            tempstr end;
            end << "e_" << name_Get(field);
            Set(R,"$endname",end);
            Set(R,"$prevend",prev);
            Set(R,"$tailname",name_Get(field));
            Ins(&R,out,"        if $endname < $prevend or $endname > length - $csize:");
            Ins(&R,out,"            raise ValueError('$ctypeid: $tailname end %d outside the tail' % $endname)");
            prev = end;
        }
    }ind_end;
    tempstr beg;
    beg << "offset + " << csize;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        tempstr end("offset + length");
        if (amc::ctype_zd_varlenfld_Next(field)) {
            end = tempstr() << "offset + " << csize << " + e_" << name_Get(field);
        }
        Set(R,"$tailmember",PyName(field));
        Set(R,"$tailbeg",beg);
        Set(R,"$tailend",end);
        Ins(&R,out,"        self.$tailmember = buf[$tailbeg:$tailend]");
        beg = end;
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        Set(R,"$tailmember",PyName(field));
        Ins(&R,out,"        self.$tailmember = buf[offset + $csize:offset + length]");
    }ind_end;
    Ins(&R,out,"        return self");
}

// -----------------------------------------------------------------------------

// Emit into OUT a property per bitfield of CTYPE.  A bitfield is a run of WIDTH
// bits at OFFSET inside its source word; the getter reads it as an integer, or
// as a bool when the bitfield is a flag, and the setter stores it back into the
// word, wrapping a signed word into its range.
static void PyGenBitfld(cstring &out, amc::FCtype &ctype) {
    algo_lib::Replscope R;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_bitfld) {
        amc::FBitfld &bitfld = *field.c_bitfld;
        amc::FField &src = *bitfld.p_srcfield;
        amc::FCtype &valarg = amc::ProjScalarArg(field);
        u64 mask = bitfld.width >= 64 ? ~u64(0) : (u64(1) << bitfld.width) - 1;
        int nbit = src.p_arg->totsize_byte * 8;
        tempstr hexmask;
        algo::u64_PrintHex(mask, hexmask, 1, true);
        tempstr wordmask;
        algo::u64_PrintHex(nbit >= 64 ? ~u64(0) : (u64(1) << nbit) - 1, wordmask, 1, true);
        Set(R,"$accessor",PyName(field));
        Set(R,"$word",tempstr() << "self." << PyName(src));
        Set(R,"$bitoffset",tempstr() << bitfld.offset);
        Set(R,"$mask",hexmask);
        Set(R,"$fullmask",wordmask);
        Ins(&R,out,"");
        Ins(&R,out,"    @property");
        Ins(&R,out,"    def $accessor(self):");
        if (ch_N(field.comment)) {
            Set(R,"$comment",field.comment);
            Ins(&R,out,"        # $comment");
        }
        if (valarg.ctype == "bool") {
            Ins(&R,out,"        return (($word >> $bitoffset) & $mask) != 0");
        } else {
            Ins(&R,out,"        return ($word >> $bitoffset) & $mask");
        }
        Ins(&R,out,"");
        Ins(&R,out,"    @$accessor.setter");
        Ins(&R,out,"    def $accessor(self, x):");
        Ins(&R,out,"        v = (($word & ~($mask << $bitoffset)) | ((int(x) & $mask) << $bitoffset)) & $fullmask");
        if (StartsWithQ(src.p_arg->ctype, "i")) {
            Set(R,"$signbit",tempstr() << nbit - 1);
            Set(R,"$nbit",tempstr() << nbit);
            Ins(&R,out,"        if v >= 1 << $signbit:");
            Ins(&R,out,"            v -= 1 << $nbit");
        }
        Ins(&R,out,"        $word = v");
    }ind_end;
}

// -----------------------------------------------------------------------------

// Write py/gen/<ns>_gen.py for the current namespace: a class and codec per
// ctype of the projection, the constants those ctypes carry, and the signature
// of each dispatch of the namespace a displang row projects into Python.  A
// namespace with nothing to project writes nothing.
void amc::gen_lang_py() {
    algo_lib::Replscope R;
    amc::FNs &ns = *amc::_db.c_ns;
    amc::FLang *lang = amc::ind_lang_Find(amcdb_lang_py);
    if (!amc::_db.proj_refuse && lang && amc::ProjNsQ(ns, *lang)) {
        cstring body;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) if (amc::ProjCtypeQ(ctype, *lang)) {
            cstring head;
            cstring cls;
            PyGenInit(cls, ns, ctype);
            if (amc::ProjCodecQ(ctype)) {
                PyGenCodec(head, cls, ns, ctype);
                PyGenBitfld(cls, ctype);
            }
            if (amc::ProjLayoutQ(ctype)) {
                PyGenLayout(cls, ctype);
            }
            body << eol << eol;
            PyGenConst(body, ctype);
            body << head << eol << eol << cls;
        }ind_end;
        ind_beg(amc::ns_c_dispatch_curs, dispatch, ns) if (amc::ProjDispatchQ(dispatch, *lang)) {
            tempstr hex;
            for (int i = 0; i < 20; i++) {
                algo::u64_PrintHex(dispatch.signature.signature_elems[i], hex, 2, false);
            }
            Set(R,"$dispname",name_Get(dispatch));
            Set(R,"$dispatch",dispatch.dispatch);
            Set(R,"$sighex",hex);
            body << eol << eol;
            Ins(&R,body,"# $dispname_Signature is the signature of dispatch $dispatch, which a peer");
            Ins(&R,body,"# built from the same schema presents and expects.");
            Ins(&R,body,"$dispname_Signature = bytes.fromhex('$sighex')");
        }ind_end;
        tempstr fname;
        fname << "py/gen/" << ns.ns << "_gen.py";
        // the write at the end of the run does not create a directory
        algo::CreateDirRecurse("py/gen");
        cstring &out = amc::outfile_Create(fname).text;
        Ins(&R,out,"# Code generated by amc. DO NOT EDIT.");
        Ins(&R,out,"");
        Ins(&R,out,"import struct");
        ind_beg(amc::_db_ns_curs, dep, amc::_db) {
            if (amc::ProjNsDepQ(ns, dep, *lang)) {
                Set(R,"$depns",dep.ns);
                Ins(&R,out,"from . import $depns_gen");
            }
        }ind_end;
        Ins(&R,out,"");
        Ins(&R,out,"_new = object.__new__");
        out << body;
    }
}
