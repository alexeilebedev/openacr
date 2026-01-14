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
// Exceptions: NO
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

// whether ctype is js or ts
bool amc::JsQ(amc::FCtype &ctype) {
    return ctype.p_ns->c_nsjs
        && (!ctype.p_ns->c_nsjs->gensel || ctype.c_jstype);
}

// whether ctype is ts
bool amc::TsQ(amc::FCtype &ctype) {
    return ctype.p_ns->c_nsjs
        && ctype.p_ns->c_nsjs->typescript
        && (!ctype.p_ns->c_nsjs->gensel || ctype.c_jstype);
}

// compute javascript class name
tempstr amc::JsType(amc::FCtype &ctype) {
    return tempstr()<<name_Get(ctype);
}

bool amc::JsBooleanQ(amc::FField &field) {
    return field.arg == "bool" || field.arg == "js.Boolean";
}

bool amc::JsNumberQ(algo::strptr arg) {
    return arg == "u8" || arg == "u16" || arg == "u32"
        || arg == "i8" || arg == "i16" || arg == "i32"
        || arg == "double" || arg == "js.Number";
}

bool amc::JsBigintQ(algo::strptr arg) {
    return arg == "u64" || arg == "i64" || arg == "js.BigInt";
}

bool amc::JsStringQ(amc::FField &field) {
    return field.p_arg->c_cstr || field.arg == "js.String";
}

// only inline arrays of u8 and char are supported
bool amc::JsInlaryQ(amc::FField &field) {
    return (field.reftype == dmmeta_Reftype_reftype_Inlary
            && (field.c_inlary->min == field.c_inlary->max)
            && (field.arg == "char" || field.arg == "u8"));
}

bool amc::JsByteArrayQ(amc::FField &field) {
    bool ret = (field.reftype == dmmeta_Reftype_reftype_Varlen
                || field.reftype == dmmeta_Reftype_reftype_Opt)
        && field.arg != "char";
    return ret || JsInlaryQ(field);
}

bool amc::JsVarlenStringQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Varlen && field.arg == "char";
}

tempstr amc::JsGetArg(amc::FField &field) {
    tempstr arg;
    if (field.reftype == dmmeta_Reftype_reftype_Base) {
        // nothing
    } else if (JsByteArrayQ(field)) {
        arg = "Uint8Array";
    } else if (JsBooleanQ(field)) {
        arg = "boolean";
    } else if (JsNumberQ(field.arg)) {
        arg = "number";
    } else if (JsBigintQ(field.arg)) {
        arg = "bigint";
    } else if (JsStringQ(field)) {
        arg = "string";
    } else {
        arg = JsType(*field.p_arg);
    }
    return arg;
}

// json-serialized type for fields
tempstr amc::JsJsonArg(amc::FField &field) {
    tempstr arg = JsGetArg(field);
    if (JsBigintQ(field.arg)) {
        arg = "number";
    } else if (!TsLocalTypeQ(field)) {
        arg = "string";
    }
    return arg;
}

// explicit cast to javascript type
tempstr amc::JsCastType(amc::FField &field) {
    tempstr arg = JsGetArg(field);
    tempstr ret;
    if (arg == "bigint") {
        ret = "BigInt";
    } else if (arg == "number") {
        ret = "Number";
    } else if (arg == "boolean") {
        ret = "Boolean";
    } else if (arg == "string") {
        ret = "String";
    } else if (field.reftype == dmmeta_Reftype_reftype_Bitfld && field.c_bitfld) {
        // - return             ((this.value & 0b11111111000000000000000000000000) >>> 24);
        // + return new Proctype((this.value & 0b11111111000000000000000000000000) >>> 24);
        ret << "new "<<arg;
    }
    return ret;
}

// compute expression for default value of field
tempstr amc::JsDefaultExpr(amc::FField &field) {
    tempstr ret;
    if (field.reftype == dmmeta_Reftype_reftype_Val) {
        if (ch_N(field.dflt.value)) {
            ret << JsCastType(field) << "(" << field.dflt.value << ")";
        } else if (field.p_arg->c_cdflt) {
            ret << JsCastType(field) << "(" << field.p_arg->c_cdflt->jsdflt << ")";
        } else if (field.p_arg->c_cstr) {
            ret = "\"\"";
        } else {
            ret = tempstr() << "new "<<JsType(*field.p_arg);
        }
    } else if (JsByteArrayQ(field)) {
        ret = "new Uint8Array";
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary
               || field.reftype == dmmeta_Reftype_reftype_Tary
               || field.reftype == dmmeta_Reftype_reftype_Lary
               || field.reftype == dmmeta_Reftype_reftype_Varlen
               || field.reftype == dmmeta_Reftype_reftype_Llist
               || field.reftype == dmmeta_Reftype_reftype_Ptrary) {
        ret = field.arg == "char" ? "\"\"" : "[]";
    } else if (field.reftype == dmmeta_Reftype_reftype_Ptr
               || field.reftype == dmmeta_Reftype_reftype_Upptr
               || field.reftype == dmmeta_Reftype_reftype_Opt) {
        ret = "null";
    } else if (field.reftype == dmmeta_Reftype_reftype_Thash) {
        ret = "new Map()";
    }
    return ret;
}

// Calculate if the field's type is locally defined in this namespace
bool amc::TsLocalTypeQ(amc::FField &field) {
    return field.reftype == dmmeta_Reftype_reftype_Base
        || JsBooleanQ(field)
        || JsNumberQ(field.arg)
        || JsBigintQ(field.arg)
        || JsStringQ(field)
        || JsByteArrayQ(field)
        || field.p_ctype->p_ns == field.p_arg->p_ns;
}

// add import statement for name if not already in place
void amc::TsImportMaybe(amc::FField &field, strptr name) {
    if (!TsLocalTypeQ(field)) {
        auto &R = _db.genctx.R;
        Set(R,"$symbol",name);
        Set(R,"$fromns",field.p_arg->p_ns->ns);
        tempstr import;
        Ins(&R,import,"import { $symbol } from \"./$fromns_gen\"");
        if (FindStr(field.p_ctype->p_ns->js.import,import)==-1) {
            field.p_ctype->p_ns->js.import << import;
        }
    }
}

// Return a javscript expression evaluating NAME, which is of type CTYPE
// to a number
tempstr amc::JsGetnumExpr(amc::FCtype &ctype, algo::strptr name) {
    tempstr ret;
    if (JsNumberQ(ctype.ctype)) {
        ret = name;
    } else {
        ind_beg(ctype_c_field_curs,field,ctype) {
            ret << name << "." << name_Get(field);
        }ind_end;
    }
    return ret;
}

void amc::JsGenBitfld() {
    auto &field = *_db.genctx.p_field;
    auto &R = _db.genctx.R;
    cstring &cls = field.p_ctype->js.body;
    cstring bitmask("0b");
    char_PrintNTimes('1',bitmask,field.c_bitfld->width);
    char_PrintNTimes('0',bitmask,field.c_bitfld->offset);
    Set(R,"$srcname",name_Get(*field.c_bitfld->p_srcfield));
    Set(R,"$bitoffset",tempstr()<<field.c_bitfld->offset);
    Set(R,"$bitmask",bitmask);
    Ins(&R,cls,"$comment");
    Ins(&R,cls,"get $name(): $arg {");
    Ins(&R,cls,"    return $cast((this.$srcname & $bitmask) >>> $bitoffset);");
    Ins(&R,cls,"}");
    Ins(&R,cls,"set $name(x: number) {");
    Ins(&R,cls,"    this.$srcname ^= $bitmask & this.$srcname");
    Ins(&R,cls,"    this.$srcname |= (+x << $bitoffset) & $bitmask;");
    Ins(&R,cls,"}");
}

void amc::TsGenEnum() {
    auto &field = *_db.genctx.p_field;
    auto &R = _db.genctx.R;
    cstring &out = _db.genctx.p_ns->js.body;
    Ins(&R,out,"");
    if (field.c_anonfld) {
        Ins(&R,out,"export enum $jstypeEnum {");
    } else {
        Ins(&R,out,"export enum $jstype_$name_Enum {");
    }
    ind_beg(field_c_fconst_curs,fconst,field) {
        tempstr fcname(strptr_ToCppIdent(name_Get(fconst),true));
        Set(R,"$fcname",fcname);
        Set(R,"$value",tempstr()<<fconst.value);
        Ins(&R,out,"   $fcname = $value,");
    }ind_end;
    Ins(&R,out,"}");
}

void amc::JsGenField() {
    auto &field = *_db.genctx.p_field;
    auto &R = _db.genctx.R;
    Set(R,"$jstype",JsType(*field.p_ctype));
    Set(R,"$name",name_Get(field));
    Set(R,"$arg",JsGetArg(field));
    Set(R,"$dflt",JsDefaultExpr(field));
    Set(R,"$cast",JsCastType(field));
    prcat(verbose2,"amc.jsgenfield"
          <<Keyval("field",field.field)
          <<Keyval("jstype",Subst(R,"$jstype"))
          <<Keyval("name",Subst(R,"$name"))
          <<Keyval("arg",Subst(R,"$arg"))
          <<Keyval("dflt",Subst(R,"$dflt"))
          <<Keyval("cast",Subst(R,"$cast")));
    tempstr comment;
    if (ch_N(field.comment)) {
        comment << " // " << field.comment;
    }
    Set(R,"$comment",comment);
    if (field.reftype == dmmeta_Reftype_reftype_Val
        || field.reftype == dmmeta_Reftype_reftype_Varlen
        || field.reftype == dmmeta_Reftype_reftype_Inlary
        || field.reftype == dmmeta_Reftype_reftype_Opt) {
        if (field.p_ctype->c_msgtype ? (!field.c_typefld && !field.c_lenfld) : true) {
            if (TsQ(*field.p_ctype)) {
                MaybeSep(field.p_ctype->js.args);
                Ins(&R,field.p_ctype->js.args,"$name?: $arg",false);
                Ins(&R,field.p_ctype->js.body,"$name: $arg;$comment");
                Ins(&R,field.p_ctype->js.ctor,"this.$name = typeof $name != 'undefined' ? $name : $dflt;");
                TsImportMaybe(field,JsGetArg(field));
            } else {
                Ins(&R,field.p_ctype->js.ctor,"this.$name = $dflt;$comment");
            }
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
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (!ns.c_nsjs->gensel || ctype.c_jstype) {
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

void amc::TsEncodeSimpleType(algo_lib::Replscope &R, cstring &out, amc::FField &field) {
    Set(R,"$arg",JsGetArg(field));
    if (field.arg == "u8") {
        Ins(&R,out,"    view.setUint8($offset,$value);");
    } else if (field.arg == "u16") {
        Ins(&R,out,"    view.setUint16($offset,$value,$lend);");
    } else if (field.arg == "u32") {
        Ins(&R,out,"    view.setUint32($offset,$value,$lend);");
    } else if (field.arg == "u64") {
        Ins(&R,out,"    view.setBigUint64($offset,$value,$lend);");
    } else if (field.arg == "i8") {
        Ins(&R,out,"    view.setInt8($offset,$value);");
    } else if (field.arg == "i16") {
        Ins(&R,out,"    view.setInt16($offset,$value,$lend);");
    } else if (field.arg == "i32") {
        Ins(&R,out,"    view.setInt32($offset,$value,$lend);");
    } else if (field.arg == "i64") {
        Ins(&R,out,"    view.setBigInt64($offset,$value,$lend);");
    } else {
        Ins(&R,out,"    $arg_Encode(view,$offset,$value);");
        TsImportMaybe(field,"$arg_Encode");
    }
}

void amc::TsGenEncode() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    cstring &out = ctype.js.funcs;
    Ins(&R,out,"");
    Ins(&R,out,"export function $jstype_Encode(view: DataView, offset: number, parent: $jstype) {");
    Set(R,"$csize", tempstr() << ctype.totsize_byte);
    Ins(&R,out,"    let length = $csize;");
    ind_beg(ctype_zd_varlenfld_curs,field,ctype) {
        Set(R,"$name",name_Get(field));
        // write byte array at the end of the struct (length = current length of the struct)
        if (JsByteArrayQ(field)) {
            Ins(&R,out,"    new Uint8Array(view.buffer,length).set(parent.$name, parent.$name.length);");
            Ins(&R,out,"    length += parent.$name.length;");
        } else if (JsVarlenStringQ(field)) {
            Ins(&R,out,"    length += _g_text_encoder.encodeInto(parent.$name,new Uint8Array(view.buffer,length)).written;");
        }
        if (ctype_zd_varlenfld_Next(field)) {
            Ins(&R,out,"    let $name_end = length - $csize;");
        }
    }ind_end;
    ind_beg(ctype_c_field_curs,field,ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            Set(R,"$name",name_Get(field));
            Ins(&R,out,"    new Uint8Array(view.buffer,length).set(parent.$name, parent.$name.length);");
            Ins(&R,out,"    length += parent.$name.length;");
        }
    }ind_end;
    int offset(0);
    ind_beg(ctype_c_field_curs,field,ctype) {
        Set(R,"$name",name_Get(field));
        Set(R,"$offset",tempstr()<<"offset+"<<offset);
        Set(R,"$lend",field.c_fbigend?"false":"true");
        if (field.reftype == dmmeta_Reftype_reftype_Base
            || field.reftype == dmmeta_Reftype_reftype_Varlen
            || field.reftype == dmmeta_Reftype_reftype_Bitfld) {
        } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
            if (field.c_typefld && ctype.c_msgtype) {
                Set(R,"$value",tempstr()<<ctype.c_msgtype->type);
            } else if (field.c_lenfld) {
                Set(R,"$extralen", tempstr() << field.c_lenfld->extra);
                Ins(&R,out,"    length += $extralen;");
                Set(R,"$value","length");
            } else {
                Set(R,"$value","parent.$name");
            }
            TsEncodeSimpleType(R,out,field);
            offset += field.p_arg->totsize_byte;
        } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            // already supported
        } else if (JsInlaryQ(field)) {
            Set(R,"$name",name_Get(field));
            Ins(&R,out,"    new Uint8Array(view.buffer,length).set(parent.$name, parent.$name.length);");
            Ins(&R,out,"    length += parent.$name.length;");
        } else {
            vrfy(0,tempstr()<<"amc_js.unsupported_reftype" << Keyval("field",field.field));
        }
    }ind_end;
    ind_beg(ctype_zd_varlenfld_curs,field,ctype) {
        if (ctype_zd_varlenfld_Next(field)) {
            // write name_end
            Set(R,"$name",name_Get(field));
            Set(R,"$value","$name_end");
            Set(R,"$offset",tempstr()<<"offset+"<<offset);
            Set(R,"$lend",ctype.c_lenfld->p_field->c_fbigend?"false":"true");
            TsEncodeSimpleType(R,out,*ctype.c_lenfld->p_field);
            offset += ctype.c_lenfld->p_field->p_arg->totsize_byte;
        }
    }ind_end;
    Ins(&R,out,"    return length;");
    Ins(&R,out,"}");
}

void amc::TsDecodeSimpleType(algo_lib::Replscope &R, cstring &out, amc::FField &field) {
    if (field.arg == "u8") {
        Ins(&R,out,"    $value = view.getUint8($offset);");
    } else if (field.arg == "u16") {
        Ins(&R,out,"    $value = view.getUint16($offset,$lend);");
    } else if (field.arg == "u32") {
        Ins(&R,out,"    $value = view.getUint32($offset,$lend);");
    } else if (field.arg == "u64") {
        Ins(&R,out,"    $value = view.getBigUint64($offset,$lend);");
    } else if (field.arg == "i8") {
        Ins(&R,out,"    $value = view.getInt8($offset);");
    } else if (field.arg == "i16") {
        Ins(&R,out,"    $value = view.getInt16($offset,$lend);");
    } else if (field.arg == "i32") {
        Ins(&R,out,"    $value = view.getInt32($offset,$lend);");
    } else if (field.arg == "i64") {
        Ins(&R,out,"    $value = view.getBigInt64($offset,$lend);");
    } else {
        Ins(&R,out,"    $value = $arg_Decode(view,$offset);");
        TsImportMaybe(field,"$arg_Decode");
    }
}

void amc::TsGenDecode() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    cstring &out = ctype.js.funcs;
    Ins(&R,out,"");
    Ins(&R,out,"export function $jstype_Decode(view: DataView, offset: number): $jstype {");
    Ins(&R,out,"    let parent = new $jstype;");
    int offset(0);
    ind_beg(ctype_c_field_curs,field,ctype) {
        Set(R,"$name",name_Get(field));
        Set(R,"$arg",JsGetArg(field));
        Set(R,"$offset",tempstr()<<"offset+"<<offset);
        Set(R,"$lend",field.c_fbigend?"false":"true");
        if (field.reftype == dmmeta_Reftype_reftype_Base
            || field.reftype == dmmeta_Reftype_reftype_Bitfld) {
        } else if (field.c_typefld && ctype.c_msgtype) {
            offset += field.p_arg->totsize_byte;
        } else if (field.reftype == dmmeta_Reftype_reftype_Val) {
            if (field.c_lenfld) {
                Ins(&R,out,"    let length = 0;");
                Set(R,"$value","length");
                Set(R,"$extralen", tempstr() << field.c_lenfld->extra);
            } else {
                Set(R,"$value","parent.$name");
            }
            TsDecodeSimpleType(R,out,field);
            if (field.c_lenfld) {
                Ins(&R,out,"    length -= $extralen;");
            }
            offset += field.p_arg->totsize_byte;
        } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            // support as byte array only
            Set(R,"$fldlength",tempstr()<<"length-"<<offset);
            Ins(&R,out,"    parent.$name = new Uint8Array(view.buffer,$offset,$fldlength);");
        } else if (JsInlaryQ(field)) {
            int len = field.c_inlary->max;
            Set(R,"$fldlength",tempstr()<<len);
            Ins(&R,out,"    parent.$name = new Uint8Array(view.buffer,$offset,$fldlength);");
        }
    }ind_end;
    // collect valen ends
    ind_beg(ctype_zd_varlenfld_curs,field,ctype) {
        if (ctype_zd_varlenfld_Next(field)) {
            // read name_end
            Set(R,"$name",name_Get(field));
            Set(R,"$value","let $name_end");
            Set(R,"$offset",tempstr()<<"offset+"<<offset);
            Set(R,"$lend",ctype.c_lenfld->p_field->c_fbigend?"false":"true");
            TsDecodeSimpleType(R,out,*ctype.c_lenfld->p_field);
            offset += ctype.c_lenfld->p_field->p_arg->totsize_byte;
        }
    }ind_end;
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
        if (JsByteArrayQ(field)) {
            Ins(&R,out,"    parent.$name = new Uint8Array(view.buffer,$offset,$fldlength);");
        } else if (JsVarlenStringQ(field)) {
            Ins(&R,out,"    parent.$name = _g_text_decoder.decode(new Uint8Array(view.buffer,$offset,$fldlength));");
        }
    }ind_end;
    Ins(&R,out,"    return parent;");
    Ins(&R,out,"}");
}

void amc::TsGenJsonInterface() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    cstring &out = ctype.js.funcs;
    Ins(&R,out,"");
    Ins(&R,out,"export interface $jstypeJson {");
    ind_beg(ctype_c_field_curs,field,ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Val) {
            if (ctype.c_msgtype ? (!field.c_typefld && !field.c_lenfld) : true) {
                Set(R,"$name",name_Get(field));
                Set(R,"$jsonarg",JsJsonArg(field));
                tempstr comment;
                if (ch_N(field.comment)) {
                    comment << " // " << field.comment;
                }
                Set(R,"$comment",comment);
                Ins(&R,out,"    $name: $jsonarg;$comment");
            }
        }
    }ind_end;
    Ins(&R,out,"}");
}

void amc::JsGenCtype() {
    auto &ctype = *_db.genctx.p_ctype;
    auto &R = _db.genctx.R;
    auto &cls = ctype.js.body;
    Set(R,"$jstype",JsType(ctype));
    if (TsQ(ctype) && ctype.c_pack) {
        TsGenEncode();
        TsGenDecode();
    }
    if (TsQ(ctype)) {
        TsGenJsonInterface();
    }
    if (JsQ(ctype)) {
        Set(R,"$csize", tempstr() << ctype.totsize_byte);
        if (TsQ(ctype)) {
            Ins(&R,cls,"    static size() : number {");
        } else {
            Ins(&R,cls,"    static size() {");
        }
        Ins(&R,cls,"        return $csize;");
        Ins(&R,cls,"    }");
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
    auto &R = _db.genctx.R;
    int msgcount(0);
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        msgcount += JsQ(ctype) && ctype.c_msgtype;
    }ind_end;
    if (msgcount) {
        cstring &out = ns.js.body;
        Ins(&R,out,"");
        Ins(&R,out,"export enum $ns_Msgtype {");
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (JsQ(ctype) && ctype.c_msgtype) {
            Set(R,"$name",name_Get(ctype));
            Set(R,"$value",tempstr()<<ctype.c_msgtype->type);
            Ins(&R,out,"    $name = $value,");
        }ind_end;
        Ins(&R,out,"}");
    }
}

void amc::JsGenNs() {
    auto &ns = *_db.genctx.p_ns;
    auto &R = _db.genctx.R;
    Set(R,"$ns",ns.ns);
    Ins(&R,ns.js.body,"let _g_text_decoder = new TextDecoder('utf-8');");
    Ins(&R,ns.js.body,"let _g_text_encoder = new TextEncoder;");
    TsGenMsgtype();
}

void amc::gen_nsjs_ns() {
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.c_nsjs) {
        amc::_db.genctx.p_ns = &ns;
        ResetVars(amc::_db.genctx);
        JsGenNs();
    }ind_end;
}

void amc::gen_nsjs_module() {
    auto &ns = *_db.c_ns;
    algo_lib::Replscope R;
    R.strict=2;
    if (ns.c_nsjs) {
        Set(R,"$ns",ns.ns);
        if (ns.c_nsjs->typescript) {
            ns.js.mdl = &outfile_Create(Subst(R,"ts/gen/$ns_gen.ts")).text;
            Ins(&R,*ns.js.mdl,"export {};");
        } else {
            ns.js.mdl = &outfile_Create(Subst(R,"js/gen/$ns_gen.js")).text;
        }
        *ns.js.mdl << ns.js.import << ns.js.body;
        ind_beg(ns_c_ctype_curs,ctype,ns) if (JsQ(ctype)) {
            Set(R,"$jstype",JsType(ctype));
            Set(R,"$args",ctype.js.args);
            Ins(&R,*ns.js.mdl,"");
            if (TsQ(ctype)) {
                Ins(&R,*ns.js.mdl,"export class $jstype {");
                InsertIndent(*ns.js.mdl,ctype.js.body,1);
                Ins(&R,*ns.js.mdl,"    constructor($args) {");
                InsertIndent(*ns.js.mdl,ctype.js.ctor,2);
                Ins(&R,*ns.js.mdl,"    }");
                Ins(&R,*ns.js.mdl,"}");
            } else {
                Ins(&R,*ns.js.mdl,"class $jstype {");
                Ins(&R,*ns.js.mdl,"    constructor() {");
                InsertIndent(*ns.js.mdl,ctype.js.ctor,2);
                Ins(&R,*ns.js.mdl,"        Object.preventExtensions(this);");
                Ins(&R,*ns.js.mdl,"    }");
                InsertIndent(*ns.js.mdl,ctype.js.body,1);
                Ins(&R,*ns.js.mdl,"}");
            }
            *ns.js.mdl << ctype.js.funcs;
        }ind_end;
    }
}
