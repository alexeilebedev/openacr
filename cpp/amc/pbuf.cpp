// Copyright (C) 2026 AlgoRND
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
// Source: cpp/amc/pbuf.cpp -- Protobuf wire codec
//

#include "include/amc.h"

// True if CTYPE has the shape the pb_type enum codec compiles against: an
// fconst-carrying i32 'value' field. The codec substitutes $fldval.value
// straight into the lib_pb varint calls, whose only overload binds i32&,
// so a value field of any other integer type yields generated code that
// does not compile.
static bool PbufEnumQ(amc::FCtype &ctype) {
    bool ret = false;
    ind_beg(amc::ctype_c_field_curs,field,ctype) {
        if (name_Get(field) == "value" && c_fconst_N(field) > 0 && field.arg == "i32") {
            ret = true;
        }
    }ind_end;
    return ret;
}

// True if CTYPE carries fields that belong on the wire: an fpbuf field of its
// own, or a base that carries some.
static bool PbufWirefldQ(amc::FCtype &ctype) {
    bool ret = false;
    ind_beg(amc::ctype_c_field_curs,field,ctype) {
        if (field.c_fpbuf) {
            ret = true;
        } else if (field.reftype == dmmeta_Reftype_reftype_Base && PbufWirefldQ(*field.p_arg)) {
            ret = true;
        }
    }ind_end;
    return ret;
}

// True if the decoder's store into FIELD names a function amc generates.
// A singular field whose value lives behind an accessor is stored through
// its generated Set, and only three shapes have one: an fbigend and a
// bitfld field, whose Set writes the underlying storage, and a cppfunc
// with set:Y, whose Set is declared for the author to implement. A substr,
// a falias and a cppfunc with set:N generate a Get and no Set. A repeated
// field is storable whatever its shape, because the decoder appends
// elements through Alloc instead of storing through Set.
// Pinned by comptest amc.BadPbufStore, whose controls are the three
// singular shapes that do have a Set.
static bool PbufStorableQ(amc::FField &field) {
    bool no_set = field.c_substr
        || field.c_falias
        || (field.c_cppfunc && !field.c_cppfunc->set);
    return field.reftype == dmmeta_Reftype_reftype_Tary || !no_set;
}

// True if a store through FIELD's generated Set already marks the field
// present in every pmask it belongs to. Only the Val Set carries the pmask
// update, and only for a field that has a member of its own to write: a
// Bitfld's Set writes a slice of its source word and a fldfunc's Set goes
// through a computed or hand-written accessor, neither of which touches a
// presence bit.
static bool SetMarksPresentQ(amc::FField &field) {
    return c_pmaskfld_member_N(field) > 0
        && field.reftype == dmmeta_Reftype_reftype_Val
        && !amc::FldfuncQ(field);
}

// True if the decoder's store into the singular field FIELD must go through
// the field's generated Set instead of into a member of the parent record.
// Two shapes need it. A field with no member of its own (NoDirectMemberQ: an
// fbigend byte-swapped word, a bitfld slice of another field, a fldfunc
// computed view) offers the decoder nothing to write but the accessor pair. A
// field carrying fcond rows has a Set that inserts and removes the parent
// record on the lists its value selects, and a store into the member would
// leave that membership behind.
// A pmask membership generates a Set too, and it does not belong here: that
// Set's added effect is the presence update, which the decoder emits itself
// through the field's generated SetPresent accessor -- SetPresentForMember
// writes both, sibling-variant clear included -- so such a field decodes into
// its member and is marked present after. Comptests amc.PbufStore and
// pbapi.DecodeOneof pin the two halves of that claim.
static bool StoreViaSetQ(amc::FField &field) {
    return amc::NoDirectMemberQ(field) || c_fcond_N(field) > 0;
}

// -----------------------------------------------------------------------------

// Shape checks for one field of the cpbuf ctype CTYPE (gen_check_pbuf):
// duplicate field numbers within the ctype, an inherited base, then reftype,
// the store the decoder needs, arg against the declared pb_type, and the
// packed flag.
// The store check covers the accessor-routed shapes: the decoder of a
// singular field whose value has no member of its own calls the field's
// generated Set, and a substr, a falias or a cppfunc with set:N generates
// none, so the emitted call names a function that does not exist. The
// shapes that do have a Set -- fbigend, bitfld, cppfunc with set:Y -- are
// accepted, and so is a repeated field of any shape, whose elements the
// decoder appends through Alloc.
// Emission walks the ctype's own fields, so a base contributes nothing: a
// derived message whose base carries wire fields would encode none of them
// and would have no decoder case for their field numbers, losing them in both
// directions with nothing on the wire to say so. Inheritance of wire fields
// is therefore rejected rather than silently dropped.
// Packed encoding exists only for a repeated field whose elements are varint
// or fixed-width: the format has no packed form for a length-delimited
// element, since each one already carries its own length. Emission reads the
// flag in that one branch only, so packed on a singular field or on a
// repeated string, bytes or message field would be read and then ignored,
// leaving the author with the ordinary encoding they did not ask for.
static void CheckPbufField(amc::FField &field, amc::FCtype &ctype) {
    // one report per duplicate pair: the earlier field in ctype
    // order stays silent, the later one names it
    amc::FField *dup = NULL;
    if (field.c_fpbuf) {
        ind_beg(amc::ctype_c_field_curs,other,ctype) {
            if (&other == &field) {
                break;
            }
            if (other.c_fpbuf && other.c_fpbuf->field_number == field.c_fpbuf->field_number) {
                dup = &other;
                break;
            }
        }ind_end;
    }
    if (dup) {
        prerr("amc.pbuf_field_number"
              <<Keyval("field",field.field)
              <<Keyval("field_number",field.c_fpbuf->field_number)
              <<Keyval("dup",dup->field)
              <<Keyval("comment","duplicate protobuf field number within one ctype"));
        algo_lib::_db.exit_code++;
    }
    if (!field.c_fpbuf && field.reftype == dmmeta_Reftype_reftype_Base && PbufWirefldQ(*field.p_arg)) {
        prerr("amc.pbuf_base"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("comment","a cpbuf ctype cannot inherit wire fields; the base's fields would be absent from this message"));
        algo_lib::_db.exit_code++;
    }
    amc::FPbtype *pbtype = field.c_fpbuf ? amc::ind_pbtype_Find(field.c_fpbuf->pb_type) : NULL;
    bool wirefld = field.c_fpbuf != NULL;
    if (wirefld && field.reftype != dmmeta_Reftype_reftype_Val && field.reftype != dmmeta_Reftype_reftype_Tary && field.reftype != dmmeta_Reftype_reftype_Bitfld) {
        prerr("amc.pbuf_reftype"
              <<Keyval("field",field.field)
              <<Keyval("reftype",field.reftype)
              <<Keyval("comment","fpbuf requires a Val, Tary or Bitfld field; this reftype has no wire mapping"));
        algo_lib::_db.exit_code++;
    } else if (wirefld && !PbufStorableQ(field)) {
        prerr("amc.pbuf_store"
              <<Keyval("field",field.field)
              <<Keyval("reftype",field.reftype)
              <<Keyval("comment","fpbuf on a singular field with no generated Set; a substr, a falias or a cppfunc with set:N has a Get only"));
        algo_lib::_db.exit_code++;
    } else if (wirefld && pbtype && ch_N(pbtype->argtype) && field.arg != pbtype->argtype) {
        prerr("amc.pbuf_arg"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("pb_type",pbtype->pbtype)
              <<Keyval("argtype",pbtype->argtype)
              <<Keyval("comment","field arg incompatible with pb_type; the lib_pb codec accepts only argtype"));
        algo_lib::_db.exit_code++;
    } else if (wirefld && pbtype && pbtype->pbtype == "message" && !field.p_arg->c_cpbuf) {
        prerr("amc.pbuf_arg"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("pb_type",pbtype->pbtype)
              <<Keyval("comment","pb_type message requires a cpbuf arg with its own codec"));
        algo_lib::_db.exit_code++;
    } else if (wirefld && pbtype && pbtype->pbtype == "enum" && !PbufEnumQ(*field.p_arg)) {
        prerr("amc.pbuf_arg"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("pb_type",pbtype->pbtype)
              <<Keyval("comment","pb_type enum requires an arg with an fconst-carrying i32 value field"));
        algo_lib::_db.exit_code++;
    } else if (wirefld && pbtype && field.c_fpbuf->packed && (field.reftype != dmmeta_Reftype_reftype_Tary || pbtype->wire_type == 2)) {
        prerr("amc.pbuf_packed"
              <<Keyval("field",field.field)
              <<Keyval("reftype",field.reftype)
              <<Keyval("pb_type",pbtype->pbtype)
              <<Keyval("comment","packed applies only to a repeated field of a varint or fixed pb_type"));
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

// Check that every fpbuf field names a known protobuf type: pb_type must
// have an amcdb.pbtype row, the table binding each protobuf type to its
// wire type and lib_pb codec functions. With an unrecognized pb_type the
// codec would emit an encoder that writes the field tag with no payload and
// a decoder case that consumes the tag but not the value -- both corrupt
// the wire stream.
// The field number is bounded by the wire tag: EncodeTag packs (number<<3)
// | wire_type into a varint, so a number at or above 2^29 aliases a
// different field on the wire, and number 0 encodes tag 0, which the
// protobuf wire format reserves as illegal -- conformant peers reject the
// message even though it round-trips against amc's own decoder. The span
// 19000..19999 is rejected on the same ground: the protobuf format reserves
// it for its own implementation, so protoc refuses to compile the equivalent
// .proto and a peer rejects the field on the wire. Duplicate numbers within
// one ctype would emit a decoder switch with two identical case labels:
// uncompilable generated code.
// The field's own shape is checked against the declared pb_type: the codec
// substitutes the field access straight into the lib_pb codec calls, so an
// arg other than the one the codec functions accept (pbtype.argtype), a
// non-cpbuf arg under pb_type message, an arg without the enum shape
// (PbufEnumQ) under pb_type enum, or a reftype with no wire mapping
// (anything but Val, Tary, Bitfld -- including Base, which emission never
// writes to the wire) produces generated code that does not compile --
// shipped at exit 0 unless rejected here. The same holds for a singular
// field the decoder can only store through a Set that amc never generates
// (a substr, a falias, a cppfunc with set:N).
// An fpbuf row can also be silently dead: the codec is generated only for
// a ctype with a cpbuf row, so fpbuf rows on a cpbuf-less ctype produce no
// code at all and the omission would surface only as a later link error or
// as wire messages missing those fields. Such rows are rejected here too.
// The message's own attribute, cpbuf.syntax, names a row of dmmeta.pbsyntax,
// the table that says whether the syntax has implicit presence. A syntax
// with no row leaves the encoder without that answer, and it would emit the
// explicit-presence shape -- every scalar on the wire whatever its value --
// for a message the author declared otherwise. acr -check rejects the
// spelling when the row goes through the database, but amc reads a schema
// out of -in_dir too, so the same value is checked here.
void amc::gen_check_pbuf() {
    ind_beg(amc::_db_cpbuf_curs,cpbuf,amc::_db) {
        if (!amc::ind_pbsyntax_Find(cpbuf.syntax)) {
            prerr("amc.pbuf_syntax"
                  <<Keyval("ctype",cpbuf.ctype)
                  <<Keyval("syntax",cpbuf.syntax)
                  <<Keyval("comment","unknown cpbuf syntax, expected a dmmeta.pbsyntax value"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    ind_beg(amc::_db_fpbuf_curs,fpbuf,amc::_db) {
        if (!amc::ind_pbtype_Find(fpbuf.pb_type)) {
            prerr("amc.pbuf_type"
                  <<Keyval("field",fpbuf.field)
                  <<Keyval("pb_type",fpbuf.pb_type)
                  <<Keyval("comment","unknown pb_type, expected an amcdb.pbtype value"));
            algo_lib::_db.exit_code++;
        }
        if (fpbuf.field_number == 0 || fpbuf.field_number >= (u32(1) << 29)) {
            prerr("amc.pbuf_field_number"
                  <<Keyval("field",fpbuf.field)
                  <<Keyval("field_number",fpbuf.field_number)
                  <<Keyval("comment","protobuf field number must be in [1, 2^29-1] to fit the wire tag"));
            algo_lib::_db.exit_code++;
        } else if (fpbuf.field_number >= 19000 && fpbuf.field_number <= 19999) {
            prerr("amc.pbuf_field_number"
                  <<Keyval("field",fpbuf.field)
                  <<Keyval("field_number",fpbuf.field_number)
                  <<Keyval("comment","protobuf reserves field numbers 19000..19999 for its own implementation"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            if (field.c_fpbuf && !ctype.c_cpbuf) {
                prerr("amc.pbuf_ctype"
                      <<Keyval("field",field.field)
                      <<Keyval("ctype",ctype.ctype)
                      <<Keyval("comment","fpbuf requires a cpbuf row on the field's ctype; without one no codec is generated"));
                algo_lib::_db.exit_code++;
            } else if (ctype.c_cpbuf) {
                CheckPbufField(field,ctype);
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// True if FPBUFCASE is the first variant of its oneof in ctype order.
// The oneof is checked as a group, and the group reports on this variant, so
// one malformed oneof draws one diagnostic however many variants it has.
static bool FirstCaseQ(amc::FFpbufcase &fpbufcase) {
    bool ret = true;
    ind_beg(amc::ctype_c_fpbufcase_curs,other,*fpbufcase.p_field->p_ctype) {
        if (&other == &fpbufcase) {
            break;
        }
        if (oneof_Get(other) == oneof_Get(fpbufcase)) {
            ret = false;
            break;
        }
    }ind_end;
    return ret;
}

// True if every variant of FPBUFCASE's oneof is a member of PMASKFLD.
static bool AllCaseMemberQ(amc::FFpbufcase &fpbufcase, amc::FPmaskfld &pmaskfld) {
    bool ret = true;
    ind_beg(amc::ctype_c_fpbufcase_curs,other,*fpbufcase.p_field->p_ctype) {
        if (oneof_Get(other) == oneof_Get(fpbufcase) && !amc::FindMember(*other.p_field,&pmaskfld)) {
            ret = false;
        }
    }ind_end;
    return ret;
}

// True if some presence mask has every variant of FPBUFCASE's oneof as a
// member.
static bool SharedPmaskQ(amc::FFpbufcase &fpbufcase) {
    bool ret = false;
    ind_beg(amc::field_c_pmaskfld_member_curs,member,*fpbufcase.p_field) {
        if (AllCaseMemberQ(fpbufcase,*member.p_pmaskfld)) {
            ret = true;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Check that the variants of every oneof share one presence mask.
// A oneof is a union: at most one variant may be set, and the encoder must
// write at most one variant's bytes. Consider a message with variants a and
// b and no presence mask: the encoder guards each singular field with the
// proto3 default-value test, so a record with both members nonzero writes
// both field numbers, and the peer keeps whichever came last -- the sender
// silently drops the other. Two variants on two different masks fail the
// same way, because the sibling bits a store clears are the ones on the mask
// it writes.
// The mutual exclusion the encoder relies on therefore lives entirely in one
// mask: a store into a variant sets that variant's bit and clears the bit of
// every sibling variant on the same mask, so the encoder's presence guard is
// true for at most one of them. Requiring a mask that holds every variant of
// the oneof is the condition under which that holds.
// This runs after gen_pmask rather than alongside the other fpbuf checks
// because a mask's member set is derived there: a pmaskfld with no explicit
// member rows takes every field of the ctype, and the membership this rule
// reads does not exist until that has happened.
void amc::gen_check_pbufcase() {
    ind_beg(amc::_db_fpbufcase_curs,fpbufcase,amc::_db) {
        if (FirstCaseQ(fpbufcase) && !SharedPmaskQ(fpbufcase)) {
            prerr("amc.pbuf_oneof"
                  <<Keyval("fpbufcase",fpbufcase.fpbufcase)
                  <<Keyval("ctype",fpbufcase.p_field->p_ctype->ctype)
                  <<Keyval("comment","every variant of a oneof must be a member of one shared pmask; without it the encoder writes every non-default variant"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Emit encode call for a single scalar value
// $buf, $fldval, $fldname and $Argtype must be set in R before calling
static void EmitEncode(algo_lib::Replscope &R, amc::FFunc &func, amc::FPbtype &pbtype) {
    if (pbtype.pbtype == "message") {
        Ins(&R,func.body,"algo::ByteAry $fldname_buf;");
        Ins(&R,func.body,"$Argtype_PbufEncode($fldname_buf,$fldval);");
        Ins(&R,func.body,"lib_pb::EncodeBytes($buf,ary_Getary($fldname_buf));");
    } else {
        Set(R,"$Suffix",pbtype.suffix);
        Set(R,"$encval",pbtype.encval);
        Ins(&R,func.body,"lib_pb::Encode$Suffix($buf,$encval);");
    }
}

// Emit decode call for a single scalar value
// $buf, $fldval, $fldname and $Argtype must be set in R before calling
static void EmitDecode(algo_lib::Replscope &R, amc::FFunc &func, amc::FPbtype &pbtype) {
    if (pbtype.pbtype == "message") {
        Ins(&R,func.body,"algo::memptr $fldname_sub;");
        Ins(&R,func.body,"ok = ok && lib_pb::DecodeSubmsg($buf,$fldname_sub);");
        Ins(&R,func.body,"ok = ok && $Argtype_PbufDecode($fldname_sub,$fldval);");
    } else {
        Set(R,"$Suffix",pbtype.suffix);
        Set(R,"$decval",pbtype.decval);
        Ins(&R,func.body,"ok = ok && lib_pb::Decode$Suffix($buf,$decval);");
    }
}

// Generate the protobuf codec of a cpbuf ctype: DIR 0 emits the encoder
// (write each fpbuf field to a byte array), DIR 1 the decoder (a
// tag-driven switch reading fields from a memptr)
void amc::PbufCodec(int dir) {
    bool encode = dir == 0;
    bool decode = !encode;
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_cpbuf) {
        // an unknown syntax is rejected by gen_check_pbuf, which withholds
        // output; the emission that follows reads it as explicit presence
        amc::FPbsyntax *pbsyntax = amc::ind_pbsyntax_Find(ctype.c_cpbuf->syntax);
        bool implicit_presence = pbsyntax && pbsyntax->implicit_presence;
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc &func = amc::CreateCurFunc(true);
        if (encode) {
            Ins(&R, func.ret, "void", false);
            AddProtoArg(func, Refto("algo::ByteAry"), "buf");
        }
        if (decode) {
            AddRetval(func, "bool", "ok", "true");
            AddProtoArg(func, Refto("algo::memptr"), "buf");
        }
        AddProtoArg(func, Refto(ctype.cpp_type), "parent");
        if (decode) {
            // Tag-driven decode loop
            Ins(&R,func.body,"while (ok && elems_N(buf) > 0) {");
            Ins(&R,func.body,"u32 pb_field_number(0);");
            Ins(&R,func.body,"u8 pb_wire_type(0);");
            Ins(&R,func.body,"ok = lib_pb::DecodeTag(buf,pb_field_number,pb_wire_type);");
            Ins(&R,func.body,"if (!ok) {");
            Ins(&R,func.body,"    break;");
            Ins(&R,func.body,"}");
            Ins(&R,func.body,"switch (pb_field_number) {");
        }
        Set(R, "$buf", "buf");
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            bool pbfld = field.c_fpbuf && field.reftype != dmmeta_Reftype_reftype_Base;
            amc::FPbtype *pbtype = pbfld ? amc::ind_pbtype_Find(field.c_fpbuf->pb_type) : NULL;
            if (pbtype) {// unknown pb_type: rejected by gen_check_pbuf, emit nothing
                u32 field_number = field.c_fpbuf->field_number;
                bool packed = field.c_fpbuf->packed;
                bool repeated = field.reftype == dmmeta_Reftype_reftype_Tary;
                int wire_type = pbtype->wire_type;
                bool has_pmask = c_pmaskfld_member_N(field) > 0;
                Set(R, "$Argtype", field.p_arg->cpp_type);
                Set(R, "$fldname", name_Get(field));
                Set(R, "$fldnum", tempstr() << field_number);
                Set(R, "$wiretype", tempstr() << wire_type);
                if (encode) {
                    if (repeated) {
                        if (packed && wire_type != 2) {
                            // Packed repeated: encode all values into temp buf, emit as length-delimited
                            Ins(&R,func.body,"if ($fldname_N(parent) > 0) {");
                            Ins(&R,func.body,"algo::ByteAry $fldname_packed;");
                            Set(R,"$buf","$fldname_packed");
                            Ins(&R,func.body,"ind_beg($Cpptype_$fldname_curs,$fldname,parent) {");
                            Set(R,"$fldval","$fldname");
                            EmitEncode(R,func,*pbtype);
                            Ins(&R,func.body,"}ind_end;");
                            Set(R,"$buf","buf");
                            Ins(&R,func.body,"lib_pb::EncodeTag($buf,$fldnum,2);");
                            Ins(&R,func.body,"lib_pb::EncodeBytes($buf,ary_Getary($fldname_packed));");
                            Ins(&R,func.body,"}");
                        } else {
                            // Non-packed repeated: emit tag+value per element
                            Ins(&R,func.body,"ind_beg($Cpptype_$fldname_curs,$fldname,parent) {");
                            Ins(&R,func.body,"lib_pb::EncodeTag($buf,$fldnum,$wiretype);");
                            Set(R,"$fldval","$fldname");
                            EmitEncode(R,func,*pbtype);
                            Ins(&R,func.body,"}ind_end;");
                        }
                    } else {
                        // Singular field: read through FieldvalExpr, which routes
                        // bitfld/fbigend/fldfunc fields through their Get accessor
                        Set(R,"$fldval",FieldvalExpr(field.p_ctype,field,"parent"));
                        bool has_guard = false;
                        if (has_pmask) {
                            // Optional with presence: emit only when the present bit is set
                            Ins(&R,func.body,tempstr() << "if (" << PresentQExpr(field,"parent") << ") {");
                            has_guard = true;
                        } else if (implicit_presence && pbtype->pbtype != "message") {
                            // Implicit presence: a scalar equal to its zero
                            // value is not written (a message field carries no
                            // such rule -- an empty submessage is still bytes)
                            tempstr chk = Subst(R,pbtype->nondflt);
                            Ins(&R,func.body,tempstr() << "if (" << chk << ") {");
                            has_guard = true;
                        }
                        Ins(&R,func.body,"lib_pb::EncodeTag($buf,$fldnum,$wiretype);");
                        EmitEncode(R,func,*pbtype);
                        if (has_guard) {
                            Ins(&R,func.body,"}");
                        }
                    }
                }
                if (decode) {
                    // A known field number arriving with a wire type other than the
                    // schema-declared one must not be decoded as the declared type:
                    // the mismatched value is treated like an unknown field and
                    // skipped by its own wire type, mirroring the default case.
                    Ins(&R,func.body,"case $fldnum: {");
                    if (repeated && wire_type != 2) {
                        // Repeated primitive: parsers accept both the packed
                        // (length-delimited) and the one-element-per-tag form,
                        // regardless of the declared packed flag
                        Ins(&R,func.body,"if (pb_wire_type == 2) {");
                        Ins(&R,func.body,"algo::memptr $fldname_packed;");
                        Ins(&R,func.body,"ok = ok && lib_pb::DecodeSubmsg($buf,$fldname_packed);");
                        Set(R,"$buf","$fldname_packed");
                        Ins(&R,func.body,"while (ok && elems_N($buf) > 0) {");
                        Set(R,"$fldval",tempstr() << name_Get(field) << "_Alloc(parent)");
                        EmitDecode(R,func,*pbtype);
                        Ins(&R,func.body,"}");
                        Set(R,"$buf","buf");
                        if (has_pmask) {
                            SetPresent(func,"parent",field);
                        }
                        Ins(&R,func.body,"} else if (pb_wire_type == $wiretype) {");
                        Set(R,"$fldval",tempstr() << name_Get(field) << "_Alloc(parent)");
                        EmitDecode(R,func,*pbtype);
                        if (has_pmask) {
                            SetPresent(func,"parent",field);
                        }
                    } else {
                        // One value per tag. The protobuf format lets one field
                        // number arrive twice in the same message -- that is
                        // what makes concatenating two encoded messages equal
                        // merging them -- and asks for a different outcome per
                        // shape: a repeated field appends both occurrences, a
                        // singular scalar keeps the later one, and a singular
                        // embedded message merges the later one into the
                        // earlier.
                        // Decoding into the destination the record already owns
                        // produces all three at no copy. A repeated field
                        // allocates an element per occurrence, so occurrences
                        // append. A singular field decodes into its own member:
                        // the nested message decoder writes only the members
                        // its occurrence carries and leaves the rest of the
                        // value standing, which is the merge, while the varint,
                        // fixed, string and bytes decoders each replace the
                        // value they are handed, which is the scalar's
                        // replacement.
                        // A singular field with no member to decode into
                        // (StoreViaSetQ) is the one shape that pays for a
                        // temporary. Under pb_type message the temporary starts
                        // from the field's current value, because the merge has
                        // to see the earlier occurrence's members and Get is
                        // the only way to read them; that costs a copy in and a
                        // copy out per occurrence, and the accessor pair trades
                        // in values, so there is no cheaper store. A scalar's
                        // temporary starts empty, since its decoder replaces
                        // whatever it is handed.
                        // A pmask member is marked present once: the store
                        // through a plain Val field's Set already carries the
                        // pmask update, and only the stores that do not
                        // (in place, through a bitfld or fldfunc accessor,
                        // into a repeated element) are followed by SetPresent
                        // Pinned by comptests amc.PbufStore and
                        // pbapi.DecodeMerge.
                        bool need_set = !repeated && StoreViaSetQ(field);
                        bool set_marks_present = need_set && SetMarksPresentQ(field);
                        bool merge_tmp = need_set && pbtype->pbtype == "message";
                        Ins(&R,func.body,"if (pb_wire_type == $wiretype) {");
                        if (repeated) {
                            Set(R,"$fldval",tempstr() << name_Get(field) << "_Alloc(parent)");
                        } else if (need_set) {
                            tempstr decl;
                            decl << Subst(R,"$Argtype $fldname_tmp");
                            if (merge_tmp) {
                                decl << "(" << FieldvalExpr(field.p_ctype,field,"parent") << ")";
                            } else {
                                decl << Subst(R," = $Argtype()");
                            }
                            Ins(&R,func.body,tempstr() << decl << ";");
                            Set(R,"$fldval","$fldname_tmp");
                        } else {
                            Set(R,"$fldval",tempstr() << "parent." << name_Get(field));
                        }
                        EmitDecode(R,func,*pbtype);
                        if (need_set) {
                            Ins(&R,func.body,tempstr() << AssignExpr(field,"parent",Subst(R,"$fldname_tmp"),false) << ";");
                        }
                        if (has_pmask && !set_marks_present) {
                            SetPresent(func,"parent",field);
                        }
                    }
                    Ins(&R,func.body,"} else {");
                    Ins(&R,func.body,"ok = lib_pb::SkipField(buf,pb_wire_type);");
                    Ins(&R,func.body,"}");
                    Ins(&R,func.body,"} break;");
                }
            }
        }ind_end;
        if (decode) {
            // Default case: skip unknown fields
            Ins(&R,func.body,"default:");
            Ins(&R,func.body,"ok = lib_pb::SkipField(buf,pb_wire_type);");
            Ins(&R,func.body,"break;");
            Ins(&R,func.body,"}"); // switch
            Ins(&R,func.body,"}"); // while
        }
    }
}

void amc::tfunc_Ctype_PbufEncode() {
    PbufCodec(0);
}

void amc::tfunc_Ctype_PbufDecode() {
    PbufCodec(1);
}
