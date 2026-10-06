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
// Source: cpp/amc/lang_go.cpp
//
// A dmmeta.displang row whose language is go names a dispatch whose messages a
// Go program speaks.  proj.cpp collects the projection and checks its layout;
// this file writes it.  Each projected ctype becomes a Go struct with Size,
// Encode, Decode and Init, laid out byte for byte as the C++ struct is.  A
// variable-length tail is a byte slice aliasing the frame, and an rpascal inline
// string is a Go string, copied out of its fixed slot and back into it.  One file
// is written per namespace, go/gen/<ns>/<ns>_gen.go, and the Go import path of a
// package is read from the module line of go/go.mod.  The dispatch signature is
// emitted beside the dispatch's own namespace, so a Go client presents the
// signature of the schema it was generated from, and a gateway built from
// another schema refuses it.

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Return the Go scalar type of builtin ctype ARG, or an empty string when ARG
// is not a builtin Go can declare.
static algo::strptr GoScalarType(amc::FCtype &arg) {
    algo::strptr ret;
    if (arg.ctype == "u8" || arg.ctype == "char") {
        ret = "uint8";
    } else if (arg.ctype == "u16") {
        ret = "uint16";
    } else if (arg.ctype == "u32") {
        ret = "uint32";
    } else if (arg.ctype == "u64") {
        ret = "uint64";
    } else if (arg.ctype == "i8") {
        ret = "int8";
    } else if (arg.ctype == "i16") {
        ret = "int16";
    } else if (arg.ctype == "i32") {
        ret = "int32";
    } else if (arg.ctype == "i64") {
        ret = "int64";
    } else if (arg.ctype == "bool") {
        ret = "bool";
    } else if (arg.ctype == "double") {
        ret = "float64";
    } else if (arg.ctype == "float") {
        ret = "float32";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Go type naming ARG as seen from the package of namespace NS: a
// scalar for a builtin, the struct name for a ctype of NS, and the struct name
// qualified by its package otherwise.
static tempstr GoTypeRef(amc::FNs &ns, amc::FCtype &arg) {
    tempstr ret;
    if (arg.c_bltin) {
        ret << GoScalarType(arg);
    } else if (arg.p_ns == &ns) {
        ret << name_Get(arg);
    } else {
        ret << arg.p_ns->ns << "." << name_Get(arg);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Go identifier of FIELD: its name in CamelCase, so it is exported,
// with an underscore appended when it names a method the codec of its ctype
// puts on the struct.
static tempstr GoName(amc::FField &field) {
    tempstr ret;
    algo::strptr_PrintCamel(name_Get(field), ret);
    bool method = amc::ProjCodecQ(*field.p_ctype)
        && (ret == "Size" || ret == "Init" || ret == "Encode" || ret == "Decode");
    if (method) {
        ret << "_";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Emit into OUT the statement writing scalar value $value of builtin ARG at
// buf[$off:], in the byte order BIGEND names.
static void GoPutScalar(algo_lib::Replscope &R, cstring &out, amc::FCtype &arg, bool bigend) {
    Set(R,"$order",bigend ? "BigEndian" : "LittleEndian");
    if (arg.ctype == "u8" || arg.ctype == "char" || arg.ctype == "i8") {
        Ins(&R,out,"    buf[$off] = uint8($value)");
    } else if (arg.ctype == "bool") {
        Ins(&R,out,"    buf[$off] = 0");
        Ins(&R,out,"    if $value {");
        Ins(&R,out,"        buf[$off] = 1");
        Ins(&R,out,"    }");
    } else if (arg.ctype == "u16" || arg.ctype == "i16") {
        Ins(&R,out,"    binary.$order.PutUint16(buf[$off:], uint16($value))");
    } else if (arg.ctype == "u32" || arg.ctype == "i32") {
        Ins(&R,out,"    binary.$order.PutUint32(buf[$off:], uint32($value))");
    } else {
        Ins(&R,out,"    binary.$order.PutUint64(buf[$off:], uint64($value))");
    }
}

// -----------------------------------------------------------------------------

// Return the Go expression reading a scalar of builtin ARG at buf[$off:], in
// the byte order BIGEND names, converted to ARG's Go type.
static tempstr GoGetScalarExpr(amc::FCtype &arg, bool bigend) {
    tempstr ret;
    algo::strptr order = bigend ? "BigEndian" : "LittleEndian";
    algo::strptr gotype = GoScalarType(arg);
    if (arg.ctype == "u8" || arg.ctype == "char" || arg.ctype == "i8") {
        ret << gotype << "(buf[$off])";
    } else if (arg.ctype == "bool") {
        ret << "buf[$off] != 0";
    } else if (arg.ctype == "u16" || arg.ctype == "i16") {
        ret << gotype << "(binary." << order << ".Uint16(buf[$off:]))";
    } else if (arg.ctype == "u32" || arg.ctype == "i32") {
        ret << gotype << "(binary." << order << ".Uint32(buf[$off:]))";
    } else {
        ret << gotype << "(binary." << order << ".Uint64(buf[$off:]))";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Emit into OUT the struct declaring CTYPE's members in Go, for the package of
// namespace NS.  A type-only CTYPE declares a string member as a Go string.
static void GoGenStruct(algo_lib::Replscope &R, cstring &out, amc::FNs &ns, amc::FCtype &ctype) {
    Set(R,"$comment",ch_N(ctype.comment) ? tempstr() << ": " << ctype.comment : tempstr(),false);
    Ins(&R,out,"");
    Ins(&R,out,"// $Ctype is $ctypename$comment");
    Ins(&R,out,"type $Ctype struct {");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        tempstr gotype;
        if (!amc::ProjCodecQ(ctype) && amc::ProjStringQ(field)) {
            gotype << "string";
        } else if (amc::ProjTailQ(field)) {
            gotype << "[]byte";
        } else if (amc::ProjRpascalQ(field) || amc::ProjRightpadQ(field)) {
            gotype << "string";
        } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
            gotype << "[" << field.c_inlary->max << "]" << GoTypeRef(ns, *field.p_arg);
        } else {
            gotype << GoTypeRef(ns, *field.p_arg);
        }
        Set(R,"$member",GoName(field));
        Set(R,"$gotype",gotype);
        Set(R,"$comment",ch_N(field.comment) ? tempstr() << "\t// " << field.comment : tempstr(),false);
        Ins(&R,out,"    $member\t$gotype$comment");
    }ind_end;
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Emit into OUT the Init function of CTYPE, which sets every member to its
// schema default.  The zero value is where it starts, a member with a default
// takes it, and a member of a ctype is initialized by that ctype's Init.
static void GoGenInit(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype) {
    Ins(&R,out,"");
    Ins(&R,out,"// Init sets every member of PARENT to its schema default.");
    Ins(&R,out,"func (parent *$Ctype) Init() {");
    Ins(&R,out,"    *parent = $Ctype{}");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        Set(R,"$member",GoName(field));
        tempstr dflt = amc::ProjDflt(field);
        bool scalar = field.p_arg->c_bltin != NULL;
        if (amc::ProjRpascalQ(field) || amc::ProjRightpadQ(field)) {
            // an inline string starts empty, which is the zero value of a Go string
        } else if (field.reftype == dmmeta_Reftype_reftype_Val && !scalar) {
            Ins(&R,out,"    parent.$member.Init()");
            if (amc::ProjNestedDfltQ(field)) {
                Set(R,"$inner",GoName(amc::ProjNestedDfltField(field)));
                Set(R,"$dflt",dflt,false);
                Ins(&R,out,"    parent.$member.$inner = $dflt");
            }
        } else if (amc::ProjZeroDfltQ(field)) {
            // the zero value is the default
        } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
            Set(R,"$dflt",dflt,false);
            Ins(&R,out,"    parent.$member = $dflt");
        } else {
            // gen_check_proj admits a literal default only on a Val or an inline array
            Set(R,"$dflt",dflt,false);
            Ins(&R,out,"    for i := range parent.$member {");
            Ins(&R,out,"        parent.$member[i] = $dflt");
            Ins(&R,out,"    }");
        }
    }ind_end;
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Emit into OUT the memory layout of CTYPE: its size and the offset of each
// member, as C++ lays the struct out, so a Go program can share a segment with
// a C++ one.
static void GoGenLayout(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype) {
    Set(R,"$sizeof",tempstr() << ctype.totsize_byte);
    Ins(&R,out,"");
    Ins(&R,out,"// $Ctype_Sizeof is the size of $ctypename in memory, and each $Ctype_Off_ constant");
    Ins(&R,out,"// the offset of a member, as C++ lays the struct out.");
    Ins(&R,out,"const (");
    Ins(&R,out,"    $Ctype_Sizeof\t= $sizeof");
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjMemberQ(field)) {
        Set(R,"$fldname",name_Get(field));
        Set(R,"$fldoff",tempstr() << field.offset);
        Ins(&R,out,"    $Ctype_Off_$fldname\t= $fldoff");
    }ind_end;
    Ins(&R,out,")");
}

// -----------------------------------------------------------------------------

// Emit into OUT the Size function of CTYPE: the fixed region plus every tail's
// bytes, which is the frame length Encode writes.
static void GoGenSize(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype) {
    tempstr expr;
    expr << ctype.totsize_byte;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (amc::ProjTailQ(field)) {
        expr << " + len(parent." << GoName(field) << ")";
    }ind_end;
    Set(R,"$sizeexpr",expr);
    Ins(&R,out,"");
    Ins(&R,out,"// Size returns the bytes PARENT occupies on the wire.");
    Ins(&R,out,"func (parent *$Ctype) Size() int {");
    Ins(&R,out,"    return $sizeexpr");
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Emit into OUT the statement writing FIELD's fixed slot at buf[$off:], for the
// package of namespace NS.  A message's type word gets the message's type, and
// its length word the frame length `length`, stored through the length field's
// scale and extra.  Every other slot is the member's value.
static void GoEncodeSlot(algo_lib::Replscope &R, cstring &out, amc::FNs &ns, amc::FCtype &ctype, amc::FField &field) {
    bool bigend = field.c_fbigend != NULL;
    if (field.c_typefld && ctype.c_msgtype) {
        Set(R,"$value",tempstr() << ctype.c_msgtype->type);
        GoPutScalar(R, out, *field.p_arg, bigend);
    } else if (field.c_lenfld && ctype.c_msgtype) {
        amc::FLenfld &lenfld = *field.c_lenfld;
        u64 maxlen = u64_Min(amc::LenfldMaxLen(lenfld), amc::FrameLenMax());
        tempstr cond;
        tempstr stored("length");
        cond << "length > " << maxlen;
        if (amc::LenfldLowGuardNeededQ(lenfld)) {
            cond << " || length < " << -lenfld.extra;
        }
        if (lenfld.extra != 0) {
            stored = tempstr() << "(length + (" << lenfld.extra << "))";
        }
        if (lenfld.scale != 1) {
            cond << " || " << stored << "%" << lenfld.scale << " != 0";
            stored << "/" << lenfld.scale;
        }
        Set(R,"$cond",cond);
        Ins(&R,out,"    if $cond {");
        Ins(&R,out,"        panic(fmt.Sprintf(\"$ctypename: frame length %d is not representable in the length field\", length))");
        Ins(&R,out,"    }");
        Set(R,"$value",stored);
        GoPutScalar(R, out, *field.p_arg, bigend);
    } else if (amc::ProjRpascalQ(field)) {
        Set(R,"$nchar",tempstr() << field.c_smallstr->length);
        Ins(&R,out,"    n$member := copy(buf[$off:$off+$nchar], parent.$member)");
        Ins(&R,out,"    clear(buf[$off+n$member : $off+$nchar+2])");
        Ins(&R,out,"    buf[$off+$nchar+1] = uint8(n$member)");
    } else if (amc::ProjRightpadQ(field)) {
        Set(R,"$nchar",tempstr() << field.c_smallstr->length);
        Set(R,"$pad",tempstr() << amc::ProjPadByte(field));
        Ins(&R,out,"    n$member := copy(buf[$off:$off+$nchar], parent.$member)");
        Ins(&R,out,"    for i := $off + n$member; i < $off+$nchar; i++ {");
        Ins(&R,out,"        buf[i] = $pad");
        Ins(&R,out,"    }");
    } else if (field.reftype == dmmeta_Reftype_reftype_Val && field.p_arg->c_bltin) {
        Set(R,"$value","parent.$member");
        GoPutScalar(R, out, *field.p_arg, bigend);
    } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
        Ins(&R,out,"    parent.$member.Encode(buf[$off:])");
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary && field.p_arg->c_bltin) {
        Set(R,"$elemsize",tempstr() << field.p_arg->totsize_byte);
        Set(R,"$off","$off+i*$elemsize");
        Set(R,"$value","parent.$member[i]");
        tempstr stmt;
        GoPutScalar(R, stmt, *field.p_arg, bigend);
        Ins(&R,out,"    for i := range parent.$member {");
        InsertIndent(out, stmt, 2);
        Ins(&R,out,"    }");
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
        Set(R,"$elemsize",tempstr() << field.p_arg->totsize_byte);
        Ins(&R,out,"    for i := range parent.$member {");
        Ins(&R,out,"        parent.$member[i].Encode(buf[$off+i*$elemsize:])");
        Ins(&R,out,"    }");
    }
    (void)ns;
}

// -----------------------------------------------------------------------------

// Emit into OUT the statement reading FIELD's fixed slot at buf[$off:].  A
// message's type word is checked against the message's type, and its length
// word is expanded into the frame length `length` and checked against both the
// fixed region and the bytes BUF holds.
static void GoDecodeSlot(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype, amc::FField &field) {
    bool bigend = field.c_fbigend != NULL;
    if (field.c_typefld && ctype.c_msgtype) {
        Set(R,"$type",tempstr() << ctype.c_msgtype->type);
        Set(R,"$get",GoGetScalarExpr(*field.p_arg, bigend));
        Ins(&R,out,"    if typ := $get; typ != $type {");
        Ins(&R,out,"        return fmt.Errorf(\"$ctypename: message type %d, want $type\", typ)");
        Ins(&R,out,"    }");
    } else if (field.c_lenfld && ctype.c_msgtype) {
        amc::FLenfld &lenfld = *field.c_lenfld;
        tempstr expand;
        expand << "int(" << GoGetScalarExpr(*field.p_arg, bigend) << ")";
        if (lenfld.scale != 1) {
            expand << "*" << lenfld.scale;
        }
        if (lenfld.extra != 0) {
            expand << " - (" << lenfld.extra << ")";
        }
        Set(R,"$get",expand);
        Ins(&R,out,"    length := $get");
        Ins(&R,out,"    if length < $csize || length > len(buf) {");
        Ins(&R,out,"        return fmt.Errorf(\"$ctypename: frame length %d outside [$csize,%d]\", length, len(buf))");
        Ins(&R,out,"    }");
    } else if (amc::ProjRpascalQ(field)) {
        Set(R,"$nchar",tempstr() << field.c_smallstr->length);
        Ins(&R,out,"    n$member := int(buf[$off+$nchar+1])");
        Ins(&R,out,"    if n$member > $nchar {");
        Ins(&R,out,"        n$member = $nchar");
        Ins(&R,out,"    }");
        Ins(&R,out,"    parent.$member = string(buf[$off : $off+n$member])");
    } else if (amc::ProjRightpadQ(field)) {
        Set(R,"$nchar",tempstr() << field.c_smallstr->length);
        Set(R,"$pad",tempstr() << amc::ProjPadByte(field));
        Ins(&R,out,"    n$member := $nchar");
        Ins(&R,out,"    for n$member > 0 && buf[$off+n$member-1] == $pad {");
        Ins(&R,out,"        n$member--");
        Ins(&R,out,"    }");
        Ins(&R,out,"    parent.$member = string(buf[$off : $off+n$member])");
    } else if (field.reftype == dmmeta_Reftype_reftype_Val && field.p_arg->c_bltin) {
        Set(R,"$get",GoGetScalarExpr(*field.p_arg, bigend));
        Ins(&R,out,"    parent.$member = $get");
    } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
        Ins(&R,out,"    parent.$member.Decode(buf[$off:])");
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary && field.p_arg->c_bltin) {
        Set(R,"$elemsize",tempstr() << field.p_arg->totsize_byte);
        Set(R,"$off","$off+i*$elemsize");
        Set(R,"$get",GoGetScalarExpr(*field.p_arg, bigend));
        Ins(&R,out,"    for i := range parent.$member {");
        Ins(&R,out,"        parent.$member[i] = $get");
        Ins(&R,out,"    }");
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
        Set(R,"$elemsize",tempstr() << field.p_arg->totsize_byte);
        Ins(&R,out,"    for i := range parent.$member {");
        Ins(&R,out,"        parent.$member[i].Decode(buf[$off+i*$elemsize:])");
        Ins(&R,out,"    }");
    }
}

// -----------------------------------------------------------------------------

// Emit into OUT the Encode function of CTYPE for the package of namespace NS.
// The tails are copied first, after the fixed region, because their lengths
// are what the frame length and the end-offset words hold; the fixed slots
// follow in schema order, then the end offset of every tail but the last.
static void GoGenEncode(algo_lib::Replscope &R, cstring &out, amc::FNs &ns, amc::FCtype &ctype) {
    amc::FField *lenfield = amc::LengthField(ctype);
    Ins(&R,out,"");
    Ins(&R,out,"// Encode writes the wire form of PARENT at the head of BUF and returns the");
    Ins(&R,out,"// byte count, which is Size().  BUF must hold Size() bytes.");
    Ins(&R,out,"func (parent *$Ctype) Encode(buf []byte) int {");
    Ins(&R,out,"    length := parent.Size()");
    Ins(&R,out,"    _ = buf[length-1]");
    if (amc::ProjAnyTailQ(ctype)) {
        Ins(&R,out,"    pos := $csize");
    }
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        Set(R,"$member",GoName(field));
        Ins(&R,out,"    pos += copy(buf[pos:], parent.$member)");
        if (amc::ctype_zd_varlenfld_Next(field)) {
            Set(R,"$name",name_Get(field));
            Ins(&R,out,"    $name_end := pos - $csize");
        }
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        Set(R,"$member",GoName(field));
        Ins(&R,out,"    pos += copy(buf[pos:], parent.$member)");
    }ind_end;
    int offset = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        Set(R,"$member",GoName(field));
        Set(R,"$off",tempstr() << offset);
        GoEncodeSlot(R, out, ns, ctype, field);
        offset += i32_Max(amc::ProjSlotBytes(field), 0);
    }ind_end;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        if (amc::ctype_zd_varlenfld_Next(field) && lenfield) {
            Set(R,"$off",tempstr() << offset);
            Set(R,"$value",tempstr() << name_Get(field) << "_end");
            GoPutScalar(R, out, *lenfield->p_arg, lenfield->c_fbigend != NULL);
            offset += lenfield->p_arg->totsize_byte;
        }
    }ind_end;
    Ins(&R,out,"    return length");
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Emit into OUT the Decode function of CTYPE.  A message is framed by its length
// word, and every end offset is refused unless it lies inside the tail region
// and at or past the one before it, so a frame from a client of another layout
// is an error and never a read past its end.  A tail is a byte slice aliasing
// BUF, text and binary alike, so decoding a record copies nothing.
static void GoGenDecode(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype) {
    amc::FField *lenfield = amc::LengthField(ctype);
    bool framed = ctype.c_msgtype && lenfield;
    Ins(&R,out,"");
    Ins(&R,out,"// Decode reads PARENT from the wire form at the head of BUF, and returns an");
    Ins(&R,out,"// error when BUF is too short or the frame is inconsistent.  A byte slice");
    Ins(&R,out,"// member aliases BUF.");
    Ins(&R,out,"func (parent *$Ctype) Decode(buf []byte) error {");
    Ins(&R,out,"    if len(buf) < $csize {");
    Ins(&R,out,"        return fmt.Errorf(\"$ctypename: %d bytes, want at least $csize\", len(buf))");
    Ins(&R,out,"    }");
    int offset = 0;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        Set(R,"$member",GoName(field));
        Set(R,"$off",tempstr() << offset);
        GoDecodeSlot(R, out, ctype, field);
        offset += i32_Max(amc::ProjSlotBytes(field), 0);
    }ind_end;
    Set(R,"$prev_end","0");
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        Set(R,"$name",name_Get(field));
        Set(R,"$member",GoName(field));
        if (amc::ctype_zd_varlenfld_Next(field) && lenfield) {
            Set(R,"$off",tempstr() << offset);
            Set(R,"$get",GoGetScalarExpr(*lenfield->p_arg, lenfield->c_fbigend != NULL));
            Ins(&R,out,"    $name_end := int($get)");
            Ins(&R,out,"    if $name_end < $prev_end || $name_end > length-$csize {");
            Ins(&R,out,"        return fmt.Errorf(\"$ctypename: $name end %d outside the tail\", $name_end)");
            Ins(&R,out,"    }");
            offset += lenfield->p_arg->totsize_byte;
            Set(R,"$prev_end",tempstr() << name_Get(field) << "_end");
        }
    }ind_end;
    // gofmt puts blanks around the colons of a slice one of whose endpoints is a
    // sum, and none around those of a slice whose endpoints are both plain
    tempstr beg;
    beg << ctype.totsize_byte;
    ind_beg(amc::ctype_zd_varlenfld_curs, field, ctype) {
        Set(R,"$member",GoName(field));
        tempstr end("length");
        if (amc::ctype_zd_varlenfld_Next(field)) {
            end = tempstr() << ctype.totsize_byte << "+" << name_Get(field) << "_end";
        }
        bool sum = algo::FindChar(beg, '+') >= 0 || algo::FindChar(end, '+') >= 0;
        Set(R,"$beg",beg);
        Set(R,"$end",end);
        Set(R,"$colon",sum ? " : " : ":");
        Ins(&R,out,"    parent.$member = buf[$beg$colon$end]");
        beg = end;
    }ind_end;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Opt) {
        Set(R,"$member",GoName(field));
        Ins(&R,out,"    parent.$member = buf[$csize:length]");
    }ind_end;
    if (framed && !amc::ProjAnyTailQ(ctype)) {
        // a frame with no tail reads its length only to check it
        Ins(&R,out,"    _ = length");
    }
    Ins(&R,out,"    return nil");
    Ins(&R,out,"}");
}

// -----------------------------------------------------------------------------

// Emit into OUT the getter and setter of every bitfield of CTYPE.  A bitfield
// is a run of WIDTH bits at OFFSET inside its source word; the getter reads it
// as the bitfield's own scalar type, or as bool when the bitfield is a flag.
static void GoGenBitfld(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype) {
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_bitfld) {
        amc::FBitfld &bitfld = *field.c_bitfld;
        amc::FField &src = *bitfld.p_srcfield;
        amc::FCtype &valarg = amc::ProjScalarArg(field);
        u64 mask = bitfld.width >= 64 ? ~u64(0) : (u64(1) << bitfld.width) - 1;
        Set(R,"$accessor",GoName(field));
        Set(R,"$bitname",name_Get(field));
        Set(R,"$word",GoName(src));
        Set(R,"$srcgotype",GoScalarType(*src.p_arg));
        Set(R,"$valtype",GoScalarType(valarg));
        Set(R,"$bitoffset",tempstr() << bitfld.offset);
        tempstr hexmask;
        algo::u64_PrintHex(mask, hexmask, 1, true);
        Set(R,"$mask",hexmask);
        Set(R,"$comment",ch_N(field.comment) ? tempstr() << ": " << field.comment : tempstr(),false);
        if (valarg.ctype == "bool") {
            Ins(&R,out,"");
            Ins(&R,out,"// $accessor returns bitfield $bitname of PARENT$comment");
            Ins(&R,out,"func (parent $Ctype) $accessor() bool {");
            Ins(&R,out,"    return (parent.$word>>$bitoffset)&$mask != 0");
            Ins(&R,out,"}");
            Ins(&R,out,"");
            Ins(&R,out,"// Set$accessor stores X into bitfield $bitname of PARENT.");
            Ins(&R,out,"func (parent *$Ctype) Set$accessor(x bool) {");
            Ins(&R,out,"    var v $srcgotype");
            Ins(&R,out,"    if x {");
            Ins(&R,out,"        v = 1");
            Ins(&R,out,"    }");
            Ins(&R,out,"    parent.$word = (parent.$word &^ ($mask << $bitoffset)) | (v << $bitoffset)");
            Ins(&R,out,"}");
        } else {
            Ins(&R,out,"");
            Ins(&R,out,"// $accessor returns bitfield $bitname of PARENT$comment");
            Ins(&R,out,"func (parent $Ctype) $accessor() $valtype {");
            Ins(&R,out,"    return $valtype((parent.$word >> $bitoffset) & $mask)");
            Ins(&R,out,"}");
            Ins(&R,out,"");
            Ins(&R,out,"// Set$accessor stores X into bitfield $bitname of PARENT.");
            Ins(&R,out,"func (parent *$Ctype) Set$accessor(x $valtype) {");
            Ins(&R,out,"    parent.$word = (parent.$word &^ ($mask << $bitoffset)) | (($srcgotype(x) & $mask) << $bitoffset)");
            Ins(&R,out,"}");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Emit into OUT the named constants of CTYPE: its message type when it is a
// message, and every fconst of a scalar member, typed as that member.  The
// fconsts of a type word are the message types of the whole schema, and each
// message states its own, so those are left out.
static void GoGenConst(algo_lib::Replscope &R, cstring &out, amc::FCtype &ctype) {
    if (ctype.c_msgtype) {
        Set(R,"$type",tempstr() << ctype.c_msgtype->type);
        Ins(&R,out,"");
        Ins(&R,out,"// $Ctype_Type is the message type of $ctypename.");
        Ins(&R,out,"const $Ctype_Type uint32 = $type");
    }
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        bool want = amc::ProjMemberQ(field) && !field.c_typefld && field.p_arg->c_bltin && amc::c_fconst_N(field) > 0;
        if (want) {
            tempstr prefix;
            prefix << name_Get(ctype);
            if (!field.c_anonfld) {
                prefix << "_" << name_Get(field);
            }
            Set(R,"$gotype",GoScalarType(*field.p_arg));
            Ins(&R,out,"");
            Ins(&R,out,"const (");
            ind_beg(amc::field_c_fconst_curs, fconst, field) {
                bool unsig = StartsWithQ(field.arg, "u");
                Set(R,"$fcname",tempstr() << prefix << "_" << amc::strptr_ToCppIdent(name_Get(fconst),true));
                Set(R,"$value",unsig ? tempstr() << u64(fconst.int_val) : tempstr() << fconst.int_val);
                Ins(&R,out,"    $fcname\t$gotype = $value");
            }ind_end;
            Ins(&R,out,")");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Return TEXT with the tab-separated cells of its lines padded into columns, the
// way gofmt aligns a struct's members and the values of a const block.  A column
// is as wide as its widest cell over a run of consecutive lines that all have a
// cell in it, and a line that runs out of cells ends the run, so a member with no
// comment aligns its type with the members around it and leaves their comment
// column alone.  The last cell of a line is followed by one space.
static tempstr GoAligned(algo::strptr text) {
    algo::StringAry line;
    ind_beg(algo::Line_curs, text_line, text) {
        ary_Alloc(line) << text_line;
    }ind_end;
    // one pass per column: each pass pads the cell before the first tab left on a
    // line and takes that tab out, so the next pass sees the next column
    bool more = true;
    while (more) {
        more = false;
        for (i64 i = 0; i < ary_N(line); i++) {
            i64 end = i;
            i64 width = 0;
            while (end < ary_N(line) && algo::FindChar(ary_qFind(line, end), '\t') >= 0) {
                width = i64_Max(width, algo::FindChar(ary_qFind(line, end), '\t'));
                end++;
            }
            more = more || end > i;
            for (; i < end; i++) {
                cstring &text_line = ary_qFind(line, i);
                i64 at = algo::FindChar(text_line, '\t');
                tempstr padded;
                padded << ch_FirstN(text_line, at);
                char_PrintNTimes(' ', padded, width - at + 1);
                padded << ch_RestFrom(text_line, at + 1);
                text_line = padded;
            }
        }
    }
    tempstr ret;
    ind_beg(algo::StringAry_ary_curs, text_line, line) {
        ret << text_line << eol;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return TEXT with every leading run of four spaces turned into one tab, which is
// how gofmt indents, so `gofmt -l` reports nothing about the generated packages.
// The emitter writes its nesting in spaces because that is how the rest of amc
// writes it; the one place the two conventions meet is here.
static tempstr GoTabify(algo::strptr text) {
    tempstr ret;
    ind_beg(algo::Line_curs, line, text) {
        int space = 0;
        while (space < elems_N(line) && line[space] == ' ') {
            space++;
        }
        for (int i = 0; i < space / 4; i++) {
            ret << '\t';
        }
        ret << ch_RestFrom(line, space - space % 4) << eol;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return the Go module path declared by go/go.mod, or an empty string when the
// file is absent or declares none.
static tempstr GoModulePath() {
    tempstr ret;
    ind_beg(algo::FileLine_curs, line, "go/go.mod") {
        algo::strptr trimmed = algo::Trimmed(line);
        if (StartsWithQ(trimmed, "module ")) {
            ret = algo::Trimmed(ch_RestFrom(trimmed, 7));
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Write go/gen/<ns>/<ns>_gen.go for the current namespace: a Go struct and codec
// per ctype of the projection, the constants those ctypes carry, and the
// signature of each dispatch of the namespace a displang row projects into Go.
// A namespace with nothing to project writes nothing.
void amc::gen_lang_go() {
    amc::FNs &ns = *amc::_db.c_ns;
    amc::FLang *lang = amc::ind_lang_Find(amcdb_lang_go);
    if (!amc::_db.proj_refuse && lang && amc::ProjNsQ(ns, *lang)) {
        algo_lib::Replscope R;
        R.strict = 2;
        tempstr module = GoModulePath();
        if (!ch_N(module)) {
            prerr("amc.go_mod"
                  <<Keyval("ns",ns.ns)
                  <<Keyval("comment","go/go.mod declares no module; the Go packages have no import path"));
            algo_lib::_db.exit_code++;
        }
        cstring body;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) if (amc::ProjCtypeQ(ctype, *lang)) {
            Set(R,"$Ctype",name_Get(ctype));
            Set(R,"$ctypename",ctype.ctype);
            Set(R,"$csize",tempstr() << ctype.totsize_byte);
            GoGenConst(R, body, ctype);
            GoGenStruct(R, body, ns, ctype);
            if (amc::ProjDfltQ(ctype)) {
                GoGenInit(R, body, ctype);
            }
            if (amc::ProjCodecQ(ctype)) {
                GoGenSize(R, body, ctype);
                GoGenEncode(R, body, ns, ctype);
                GoGenDecode(R, body, ctype);
                GoGenBitfld(R, body, ctype);
            }
            if (amc::ProjLayoutQ(ctype)) {
                GoGenLayout(R, body, ctype);
            }
        }ind_end;
        ind_beg(amc::ns_c_dispatch_curs, dispatch, ns) if (amc::ProjDispatchQ(dispatch, *lang)) {
            tempstr bytes;
            algo::ListSep ls(", ");
            for (int i = 0; i < 20; i++) {
                bytes << ls;
                algo::u64_PrintHex(dispatch.signature.signature_elems[i], bytes, 2, true);
            }
            Set(R,"$name",name_Get(dispatch));
            Set(R,"$dispatch",dispatch.dispatch);
            Set(R,"$bytes",bytes);
            Ins(&R,body,"");
            Ins(&R,body,"// $name_Signature is the signature of dispatch $dispatch, which a peer");
            Ins(&R,body,"// built from the same schema presents and expects.");
            Ins(&R,body,"var $name_Signature = [20]uint8{$bytes}");
        }ind_end;
        Set(R,"$ns",ns.ns);
        Set(R,"$module",module);
        // a namespace new to the projection has no directory yet, and the
        // write at the end of the run does not make one
        algo::CreateDirRecurse(Subst(R,"go/gen/$ns"));
        // gofmt sorts the import paths of a group, so they are collected and
        // sorted rather than written as they are met
        algo::StringAry import_path;
        ary_Alloc(import_path) << "encoding/binary";
        ary_Alloc(import_path) << "fmt";
        ind_beg(amc::_db_ns_curs, dep, amc::_db) {
            if (amc::ProjNsDepQ(ns, dep, *lang)) {
                Set(R,"$dep",dep.ns);
                ary_Alloc(import_path) << Subst(R,"$module/gen/$dep");
            }
        }ind_end;
        for (i64 i = 1; i < ary_N(import_path); i++) {
            for (i64 j = i; j > 0 && ary_qFind(import_path, j) < ary_qFind(import_path, j - 1); j--) {
                tempstr held(ary_qFind(import_path, j));
                ary_qFind(import_path, j) = ary_qFind(import_path, j - 1);
                ary_qFind(import_path, j - 1) = held;
            }
        }
        cstring head;
        Ins(&R,head,"// Code generated by amc. DO NOT EDIT.");
        Ins(&R,head,"");
        Ins(&R,head,"package $ns");
        Ins(&R,head,"");
        Ins(&R,head,"import (");
        ind_beg(algo::StringAry_ary_curs, import_line, import_path) {
            Set(R,"$import",import_line);
            Ins(&R,head,"    \"$import\"");
        }ind_end;
        Ins(&R,head,")");
        Ins(&R,head,"");
        Ins(&R,head,"var _ = binary.LittleEndian");
        Ins(&R,head,"var _ = fmt.Errorf");
        cstring &out = amc::outfile_Create(Subst(R,"go/gen/$ns/$ns_gen.go")).text;
        out << GoTabify(head) << GoTabify(GoAligned(body));
    }
}
