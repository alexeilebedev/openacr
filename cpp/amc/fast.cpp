// Copyright (C) 2024-2026 AlgoRND
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
// Source: cpp/amc/fast.cpp -- Fixfast generator
//

#include "include/amc.h"

//------------------------------------------------------------------------------

static amc::FField *GetSoleValField(amc::FCtype &ctype) {
    amc::FField *ret(NULL);
    int count(0);
    ind_beg(amc::ctype_c_field_curs,field,ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Val) {
            ret = &field;
            ++count;
        }
    }ind_end;
    if (count != 1) {
        ret = NULL;
    }
    return ret;
}

//------------------------------------------------------------------------------

// The pmask named PMASK on the FAST state record of NS, <ns>.FastState, or NULL
// when the namespace declares no such pmask.
// A FAST codec compares each field against the value the peer last saw, and
// qualifies that value with two facts: the assigned pmask says a value has
// been seen at all, the present pmask says the last one was present.  The
// pmaskfld row is what names the accessors the codec calls: the pmask's own
// field name prefixes the whole-word bitset accessors, and its funcname
// spells the per-field ones -- a pmask named present yields <field>_PresentQ,
// <field>_SetPresent and <field>_Present_GetBit.  Reading the row is what
// keeps the emitted names equal to the generated ones, so a pmask renamed in
// the schema renames both together.
static amc::FPmaskfld *FindStatePmask(amc::FNs &ns, strptr pmask) {
    amc::FField *field = amc::ind_field_Find(tempstr()<<ns.ns<<".FastState."<<pmask);
    return field ? field->c_pmaskfld : NULL;
}

// Whether CTYPE gets FAST codec functions of its own.  A cfast ctype encoded
// as a type carries a single value and is coded inline by whichever message
// holds it, so it gets none.
static bool FastCodecQ(amc::FCtype &ctype) {
    return ctype.c_cfast && ctype.c_cfast->encoding != dmmeta_Fastenc_fastenc_type;
}

// Report a FAST state pmask the codecs of NS need and the namespace does not
// declare: every accessor that qualifies a field's previous value is named
// after that pmask, so without the row there is no name to emit and the
// expressions the codec builds come out empty.
static void CheckStatePmask(amc::FNs &ns, strptr pmask) {
    if (!FindStatePmask(ns, pmask)) {
        prerr("amc.fast_state"
              <<Keyval("pmask",tempstr()<<ns.ns<<".FastState."<<pmask)
              <<Keyval("comment","FAST codec qualifies each field's previous value through this pmask of the namespace's FastState record"));
        algo_lib::_db.exit_code++;
    }
}

//------------------------------------------------------------------------------

// Return C++ expression(s) assigning VALEXPR to the present bit of FIELD in
// every presence mask of which the field is a member; PARENT names the parent
// record (collapsed by ParentArgExpr when the parent is a global).
// A decoder learns from the wire whether the field was present and has to
// record that answer either way, which amc::SetPresentExpr cannot do -- it
// marks presence unconditionally. Returns empty string if the field has no
// pmask membership.
static tempstr SetPresentValExpr(amc::FField &field, strptr parent, strptr valexpr) {
    tempstr out;
    tempstr parent_arg(amc::ParentArgExpr(*field.p_ctype,parent,true));
    algo::ListSep ls("; ");
    ind_beg(amc::field_c_pmaskfld_member_curs,pmaskfld_member,field) {
        out << ls << name_Get(*pmaskfld_member.p_pmaskfld->p_field) << "_qSetBitVal("
            << parent_arg << pmaskfld_member.bit << ", " << valexpr << ")";
    }ind_end;
    return out;
}

//------------------------------------------------------------------------------

// Check that every namespace with FAST codecs declares the two pmasks those
// codecs qualify each field's previous value through.
// The pmask is a field of the namespace's own FastState record, so the missing
// row is one defect of the namespace no matter how many messages the namespace
// codes: a protocol with a hundred templates needs the one row, and reporting
// it per message, or per codec function of each message, would charge the same
// defect hundreds of times and count each charge as an error.
void amc::gen_check_fast() {
    ind_beg(amc::_db_ns_curs,ns,amc::_db) {
        bool codec = false;
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
            codec = codec || FastCodecQ(ctype);
        }ind_end;
        if (codec) {
            CheckStatePmask(ns, "assigned");
            CheckStatePmask(ns, "present");
        }
    }ind_end;
}

//------------------------------------------------------------------------------

// C++ expression testing PMASKFLD's bit for the state field named NAME.
// Empty when the namespace declares no such pmask, which gen_check_fast has
// reported.
static tempstr StateBitQExpr(strptr name, amc::FPmaskfld *pmaskfld) {
    tempstr ret;
    if (pmaskfld) {
        ret << name << "_" << pmaskfld->funcname << "Q(state)";
    }
    return ret;
}

// C++ statement setting PMASKFLD's bit for the state field named NAME.
// Empty when the namespace declares no such pmask.
static tempstr StateSetBitExpr(strptr name, amc::FPmaskfld *pmaskfld) {
    tempstr ret;
    if (pmaskfld) {
        ret << name << "_Set" << pmaskfld->funcname << "(state)";
    }
    return ret;
}

// C++ statement assigning VALEXPR to PMASKFLD's bit for the state field named
// NAME.  Empty when the namespace declares no such pmask.
static tempstr StateSetBitValExpr(strptr name, amc::FPmaskfld *pmaskfld, strptr valexpr) {
    tempstr ret;
    if (pmaskfld) {
        ret << name_Get(*pmaskfld->p_field) << "_qSetBitVal(state, " << name << "_"
            << pmaskfld->funcname << "_GetBit(state), " << valexpr << ")";
    }
    return ret;
}

//------------------------------------------------------------------------------

// Prepare FAST data
void amc::gen_fast_presence() {
    // sort by position
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        amc::c_ffast_QuickSort(ctype);
    }ind_end;
}

//------------------------------------------------------------------------------

// Get number of pmap bits for combination of operator and optional flag:
// as per FAST specification Appendix 3.3.1
//            Presence Map Bit is Required
// Operator   Mandatory Optional
// None       No        No
// constant   No        Yes*
// copy       Yes       Yes
// default    Yes       Yes
// delta      No        No
// increment  Yes       Yes
// tail       Yes       Yes
//
// * An optional field with the constant operator will occupy a single bit.
// The bit will be set on if the input value is equal to the initial value
// specified in the instruction context.
// The bit will be set off if the input value is absent.
//
static int PmapBitsN(strptr op, bool optional) {
    int ret(1);
    if (op == dmmeta_Fastop_fastop_none
        || op == dmmeta_Fastop_fastop_delta
        || (op == dmmeta_Fastop_fastop_constant && !optional)) {
        ret = 0;
    }
    return ret;
}

//------------------------------------------------------------------------------

static int PmapBitsN(amc::FCtype &ctype); // ignore:static_fwd_decl

// Get pmap bits for field
//
// If a group field is optional, it will occupy a single bit in the presence map.
// A sequence has an associated length field containing an unsigned integer
// indicating the number of encoded elements. A sequence can be mandatory or optional.
// An optional sequence means that the length field is optional.
//
static int PmapBitsN(amc::FField &field) {
    int ret(0);
    if (field.c_ffast_field) {
        if (field.c_ffast_field->encoding == dmmeta_Fastenc_fastenc_group) {
            // optional group  - one bit
            ret = field.c_ffast_field->optional ? 1 : 0;
        } else if (field.c_ffast_field->encoding == dmmeta_Fastenc_fastenc_sequence) {
            if (field.c_ffast_length) {
                ret = PmapBitsN(field.c_ffast_length->op,field.c_ffast_field->optional);
            }
        } else if (field.c_ffast_field->encoding == dmmeta_Fastenc_fastenc_template) {
            // static template is transparent - N bits in parent template
            ret = PmapBitsN(*field.p_arg);
        } else {
            // primitive or defined field - combination of operator and optional flag
            ret = PmapBitsN(field.c_ffast_field->op,field.c_ffast_field->optional);
            // + individual decimal mantissa
        }
    }
    return ret;
}

//------------------------------------------------------------------------------

// Get pmap bits for ctype (excluding TID bit)
static int PmapBitsN(amc::FCtype &ctype) {
    int ret(0);
    if (ctype.c_cfast) {
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            ret += PmapBitsN(field);
        }ind_end;
    }
    return ret;
}

//------------------------------------------------------------------------------

// get lib_fast API name portion depending on encoding
static strptr PrimitiveEncoding(strptr encoding) {
    strptr ret;
    if (encoding == dmmeta_Fastenc_fastenc_signed) {
        ret = "Signed";
    } else if (encoding == dmmeta_Fastenc_fastenc_unsigned) {
        ret = "Unsigned";
    } else if (encoding == dmmeta_Fastenc_fastenc_string) {
        ret = "String";
    } else if (encoding == dmmeta_Fastenc_fastenc_byteVector) {
        ret = "ByteVector";
    } else if (encoding == dmmeta_Fastenc_fastenc_scaled) {
        ret = "Scaled";
    }
    return ret;
}

//------------------------------------------------------------------------------

// get base value for delta in absence of initial value
static strptr DeltaBase(amc::FFfast &ffast) {
    strptr ret;
    if (ch_N(ffast.value.value)) {
        ret = ffast.value.value;
    } else if (ffast.encoding == dmmeta_Fastenc_fastenc_signed
               || ffast.encoding == dmmeta_Fastenc_fastenc_unsigned) {
        ret = "0";
    } else if (ffast.encoding == dmmeta_Fastenc_fastenc_string) {
        ret = "strptr()";
    } else if (ffast.encoding == dmmeta_Fastenc_fastenc_byteVector) {
        ret = "strptr()";
    } else if (ffast.encoding == dmmeta_Fastenc_fastenc_scaled) {
        ret = "algo::Decimal(0,0)";
    }
    return ret;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------    ENCODE    ----------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static void FastEncodePrimitiveType(algo_lib::Replscope &R, amc::FFunc &func, bool optional, strptr op) {
    if (optional) {
        if (op == dmmeta_Fastop_fastop_none) {
            Ins(&R, func.body,"    if ($fldgetprs) {");
            Ins(&R, func.body,"        lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
            Ins(&R, func.body,"    } else {");
            Ins(&R, func.body,"        lib_fast::EncodeNull(buf);");
            Ins(&R, func.body,"    }");
        } else if (op == dmmeta_Fastop_fastop_constant) {
            Ins(&R, func.body,"    if ($fldgetprs) {");
            Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"    }");
        } else if (op == dmmeta_Fastop_fastop_default) {
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"    if ($fldgetprs) {");
                Ins(&R, func.body,"        if ($fldval != $fldival) {");
                Ins(&R, func.body,"            lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
                Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
                Ins(&R, func.body,"        }");
                Ins(&R, func.body,"    } else {");
                Ins(&R, func.body,"        lib_fast::EncodeNull(buf);");
                Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
                Ins(&R, func.body,"    }");
            } else {
                Ins(&R, func.body,"    if ($fldgetprs) {");
                Ins(&R, func.body,"        lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
                Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
                Ins(&R, func.body,"    }");
            }
        } else if (op == dmmeta_Fastop_fastop_copy) {
            Ins(&R, func.body,"    if ($fldgetprs) {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"        if ($fldoldgetass ? !$fldoldgetprs || $fldval != $fldoldval : $fldval != $fldival) {");
            } else {
                Ins(&R, func.body,"        if (!$fldoldgetass || !$fldoldgetprs ||  $fldval != $fldoldval ) {");
            }
            Ins(&R, func.body,"            lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
            Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"    } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"        if ($fldoldgetass && $fldoldgetprs) {");
            } else {
                Ins(&R, func.body,"        if ($fldoldgetass) {");
            }
            Ins(&R, func.body,"            lib_fast::EncodeNull(buf);");
            Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    $fldoldval = $fldval;");
            Ins(&R, func.body,"    $fldoldsetprs;");
            Ins(&R, func.body,"    $fldoldsetass;");
        } else if (op == dmmeta_Fastop_fastop_delta) {
            Ins(&R, func.body,"    if (!$fldoldgetass) {");
            Ins(&R, func.body,"        $fldoldsetass;");
            Ins(&R, func.body,"        $fldoldsetprs;");
            Ins(&R, func.body,"        $fldoldval = $flddeltabase;");
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    if ($fldgetprs) {");
            // Specification explicitly says that
            // "it is a dynamic error [ERR D6] if the previous value is empty."
            // XETRA violates this rule, it sends delta for empty value.
            // We assume that delta is from initial or type-specific base value.
            Ins(&R, func.body,"        if (!$fldoldgetprs) {");
            Ins(&R, func.body,"            $fldoldval = $flddeltabase;");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        lib_fast::EncodeDelta$fldenc(buf,$fldoldval,$fldval,$fldopt);");
            Ins(&R, func.body,"    } else {");
            Ins(&R, func.body,"        lib_fast::EncodeNull(buf);");
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    $fldoldval = $fldval;");
            Ins(&R, func.body,"    $fldoldsetprs;");
        } else if (op == dmmeta_Fastop_fastop_increment) {
            Ins(&R, func.body,"    if ($fldgetprs) {");
            Ins(&R, func.body,"        if ($fldoldgetass) {");
            Ins(&R, func.body,"            if (!$fldoldgetprs || $fldval != $fldoldval + 1) {");
            Ins(&R, func.body,"                lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
            Ins(&R, func.body,"                lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"            if ($fldval != $fldival) {");
                Ins(&R, func.body,"                lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
                Ins(&R, func.body,"                lib_fast::SetPmapBit(pmap,$fldpbit);");
                Ins(&R, func.body,"            }");
            } else {
                Ins(&R, func.body,"            lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
                Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
            }
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"    } else {");
            Ins(&R, func.body,"        if (!$fldoldgetass || $fldoldgetprs) {");
            Ins(&R, func.body,"            lib_fast::EncodeNull(buf);");
            Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    $fldoldval = $fldval;");
            Ins(&R, func.body,"    $fldoldsetprs;");
            Ins(&R, func.body,"    $fldoldsetass;");
        } else if (op == dmmeta_Fastop_fastop_tail) {
            prerr("tail operator unsupported");
        }
    } else { // mandatory
        if (op == dmmeta_Fastop_fastop_none) {
            Ins(&R, func.body,"    lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
        } else if (op == dmmeta_Fastop_fastop_constant) {
            // no-op
        } else if (op == dmmeta_Fastop_fastop_default) {
            Ins(&R, func.body,"    if ($fldval != $fldival) {");
            Ins(&R, func.body,"        lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
            Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"    }");
        } else if (op == dmmeta_Fastop_fastop_copy) {
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"    if ($fldoldgetass ? $fldval != $fldoldval : $fldval != $fldival) {");
            } else {
                Ins(&R, func.body,"    if (!$fldoldgetass || $fldval != $fldoldval) {");
            }
            Ins(&R, func.body,"        lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
            Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    $fldoldval = $fldval;");
            Ins(&R, func.body,"    $fldoldsetass;");
        } else if (op == dmmeta_Fastop_fastop_delta) {
            Ins(&R, func.body,"    if (!$fldoldgetass) {");
            Ins(&R, func.body,"        $fldoldsetass;");
            Ins(&R, func.body,"        $fldoldval = $flddeltabase;");
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    lib_fast::EncodeDelta$fldenc(buf,$fldoldval,$fldval,$fldopt);");
            Ins(&R, func.body,"    $fldoldval = $fldval;");
        } else if (op == dmmeta_Fastop_fastop_increment) {
            Ins(&R, func.body,"    if ($fldoldgetass) {");
            Ins(&R, func.body,"        if ($fldval != $fldoldval + 1) {");
            Ins(&R, func.body,"            lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
            Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"    } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"        if ($fldval != $fldival) {");
                Ins(&R, func.body,"            lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
                Ins(&R, func.body,"            lib_fast::SetPmapBit(pmap,$fldpbit);");
                Ins(&R, func.body,"        }");
            } else {
                Ins(&R, func.body,"        lib_fast::Encode$fldenc(buf,$fldval,$fldopt);");
                Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
            }
            Ins(&R, func.body,"    }");
            Ins(&R, func.body,"    $fldoldval = $fldval;");
            Ins(&R, func.body,"    $fldoldsetass;");
        } else if (op == dmmeta_Fastop_fastop_tail) {
            prerr("tail operator unsupported");
        }
    }
}

//------------------------------------------------------------------------------

// Type FAST encode
void amc::tfunc_Ctype_FastEncode() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (FastCodecQ(ctype)) {
        dmmeta::FastencCase ctype_encoding;
        dmmeta::fastenc_SetStrptrMaybe(ctype_encoding,ctype.c_cfast->encoding);
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc(true);
        Ins(&R, func.ret  , "void", false);
        AddProtoArg(func, Refto("algo::ByteAry"), "buf");
        AddProtoArg(func, Refto("FastState"), "state");
        AddProtoArg(func, Refto(ctype.cpp_type), "parent");
        // the two pmasks on the namespace's FastState record whose accessors
        // qualify each field's previous value; gen_check_fast has reported a
        // namespace that declares neither
        amc::FPmaskfld *state_ass = FindStatePmask(*ctype.p_ns, "assigned");
        amc::FPmaskfld *state_prs = FindStatePmask(*ctype.p_ns, "present");
        if (PmapBitsN(ctype) || ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
            Ins(&R, func.body,"    int index = ary_N(buf);");
            Ins(&R, func.body,"    u64 pmap(0);");
        }
        if (ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
            // tid - copy mandatory
            if (ctype.c_cfast->id) {
                Set(R, "$tidgetass", StateBitQExpr("tid", state_ass));
                Set(R, "$tidsetass", StateSetBitExpr("tid", state_ass));
                Ins(&R, func.body,"    if (!$tidgetass || parent.id != state.tid) {");
                Ins(&R, func.body,"        lib_fast::EncodeUnsigned(buf,parent.id,false);");
                Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,0);");
                Ins(&R, func.body,"    }");
                Ins(&R, func.body,"    $tidsetass;");
                Ins(&R, func.body,"    state.tid = parent.id;");
            }
        }
        int pbit(ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template);
        ind_beg(ctype_c_ffast_curs,ffast,ctype) {
            Set(R, "$fldname", name_Get(*ffast.p_field));
            Set(R, "$fldkey", ffast.name);
            Set(R, "$fldval", "parent.$fldname");
            Set(R, "$fldgetprs", amc::PresentQExpr(*ffast.p_field,"parent"));
            Set(R, "$fldoldval", "state.$fldkey");
            Set(R, "$fldoldgetass", StateBitQExpr(name_Get(*ffast.p_field), state_ass));
            Set(R, "$fldoldsetass", StateSetBitExpr(name_Get(*ffast.p_field), state_ass));
            Set(R, "$fldoldgetprs", StateBitQExpr(name_Get(*ffast.p_field), state_prs));
            Set(R, "$fldoldsetprs", StateSetBitValExpr(name_Get(*ffast.p_field), state_prs, amc::PresentQExpr(*ffast.p_field,"parent")));
            Set(R, "$fldargtype", name_Get(*ffast.p_field->p_arg));
            Set(R, "$fldopt", ffast.optional ? "true" : "false");
            Set(R, "$fldpbit", tempstr()<<pbit);
            Set(R, "$fldival", ffast.value.value);
            Set(R, "$fldenc",PrimitiveEncoding(ffast.encoding));
            Set(R, "$flddeltabase",DeltaBase(ffast));
            cstring  cmt = tempstr() << "    // " << name_Get(*ffast.p_field)
                                     << " " << ffast.encoding
                                     << " " << ffast.op
                                     << " " << (ffast.optional?"optional":"mandatory");
            Ins(&R, func.body,cmt);
            if (ffast.encoding == dmmeta_Fastenc_fastenc_template) {
                prerr("templateRef unsupported");
            } else if (ffast.p_field->reftype == dmmeta_Reftype_reftype_Varlen) {
                Ins(&R, func.body,"    {");
                Ins(&R, func.body,"        u32 length = $fldname_N(parent);");
                Set(R, "$fldval", "length");
                Set(R, "$fldgetprs", "length"); // zero means not present
                if (ffast.p_field->c_ffast_length) {
                    Set(R, "$fldname", name_Get(*ffast.p_field));
                    Set(R, "$fldkey", ffast.p_field->c_ffast_length->name);
                    Set(R, "$fldoldval", "state.$fldkey");
                    Set(R, "$fldoldgetass", StateBitQExpr(ffast.p_field->c_ffast_length->name, state_ass));
                    Set(R, "$fldoldsetass", StateSetBitExpr(ffast.p_field->c_ffast_length->name, state_ass));
                    Set(R, "$fldoldgetprs", StateBitQExpr(ffast.p_field->c_ffast_length->name, state_prs));
                    Set(R, "$fldoldsetprs", StateSetBitValExpr(ffast.p_field->c_ffast_length->name, state_prs, "prs"));
                    Set(R, "$fldival", ffast.p_field->c_ffast_length->value.value);
                    Set(R, "$fldenc",PrimitiveEncoding(ffast.p_field->c_ffast_length->encoding));
                    Set(R, "$flddeltabase",DeltaBase(*ffast.p_field->c_ffast_length));
                    FastEncodePrimitiveType(R,func,ffast.optional,ffast.p_field->c_ffast_length->op);
                } else {
                    Set(R, "$fldenc","Unsigned");
                    FastEncodePrimitiveType(R,func,ffast.optional,dmmeta_Fastop_fastop_none);
                }
                Ins(&R, func.body,"        ind_beg($Name_$fldname_curs,elem,parent) {");
                Ins(&R, func.body,"            $fldargtype_FastEncode(buf,state,elem);");
                Ins(&R, func.body,"        }ind_end;");
                Ins(&R, func.body,"    }");
            } else if (ffast.encoding == dmmeta_Fastenc_fastenc_group || ffast.encoding == dmmeta_Fastenc_fastenc_sequence) {
                if (ffast.optional) {
                    Ins(&R, func.body,"    if ($fldgetprs) {");
                    Ins(&R, func.body,"        $fldargtype_FastEncode(buf,state,$fldval);");
                    Ins(&R, func.body,"        lib_fast::SetPmapBit(pmap,$fldpbit);");
                    Ins(&R, func.body,"    }");
                } else {
                    Ins(&R, func.body,"    $fldargtype_FastEncode(buf,state,$fldval);");
                }
            } else  if (ffast.encoding == dmmeta_Fastenc_fastenc_type) {
                amc::FField *typefld = GetSoleValField(*ffast.p_field->p_arg);
                if (typefld && typefld->c_ffast_field) {
                    Set(R, "$typefldname",name_Get(*typefld));
                    Set(R, "$fldval", "$fldval.$typefldname");
                    Set(R, "$fldoldval", "$fldoldval.$typefldname");
                    Set(R, "$fldenc", PrimitiveEncoding(typefld->c_ffast_field->encoding));
                    Set(R, "$flddeltabase",DeltaBase(ffast));
                    FastEncodePrimitiveType(R,func,ffast.optional,ffast.op);
                }
            } else {
                FastEncodePrimitiveType(R,func,ffast.optional,ffast.op);
            }
            pbit += PmapBitsN(*ffast.p_field);
        }ind_end;
        if (PmapBitsN(ctype) || ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
            Ins(&R, func.body,"    lib_fast::InsertPmap(buf,index,pmap);");
        }
    }
}

// Encode FAST messages
static void EncodeMsgs() {
    algo_lib::Replscope R;
    amc::FNs &ns = *amc::_db.c_ns;
    Set(R,"$ns",ns.ns);
    amc::FFunc *func(NULL);
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (ctype.c_cfast && ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
        if (!func) {
            func = &amc::ind_func_GetOrCreate(Subst(R,"$ns.TemplateHeaderMsgs..FastEncode"));
            Ins(&R, func->ret  , "void", false);
            Ins(&R, func->proto, "TemplateHeaderMsgs_FastEncode(algo::ByteAry &buf, FastState &state, $ns::TemplateHeader &header)");
            Ins(&R, func->body , "switch (header.id) {");
        }
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        Set(R, "$TID", tempstr() << ctype.c_cfast->id);
        Ins(&R, func->body , "    case $TID:");
        Ins(&R, func->body , "        $Cpptype_FastEncode(buf,state,($Cpptype&)header);");
        if (ctype.c_cfast->reset) {
            Ins(&R, func->body , "        state.assigned = 0;");
            Ins(&R, func->body , "        state.present = 0;");
        }
        Ins(&R, func->body , "        break;");
    }ind_end;
    if (func) {
        Ins(&R, func->body , "}");
        MaybeUnused(*func,"header");
        MaybeUnused(*func,"state");
        MaybeUnused(*func,"buf");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------    DECODE    ----------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static void FastDecodePrimitiveType(algo_lib::Replscope &R, amc::FFunc &func, bool optional, strptr op) {
    if (optional) {
        if (op == dmmeta_Fastop_fastop_none) {
            Ins(&R, func.body,"        bool prs = !lib_fast::DecodeNull(from);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"            if (!ok) {");
            Ins(&R, func.body,"                state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldsetprs;");
        } else if (op == dmmeta_Fastop_fastop_constant) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        $fldsetprs;");
        } else if (op == dmmeta_Fastop_fastop_default) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            prs = !lib_fast::DecodeNull(from);");
            Ins(&R, func.body,"            if (prs) {");
            Ins(&R, func.body,"                ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"                if (!ok) {");
            Ins(&R, func.body,"                    state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"                }");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"            $fldval = $fldival;");
                Ins(&R, func.body,"            prs = true;");
            }
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldsetprs;");
        } else if (op == dmmeta_Fastop_fastop_copy) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            prs = !lib_fast::DecodeNull(from);");
            Ins(&R, func.body,"            if (prs) {");
            Ins(&R, func.body,"                ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"                if (!ok) {");
            Ins(&R, func.body,"                    state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"                }");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else if ($fldoldgetass) {");
            Ins(&R, func.body,"            $fldval = $fldoldval;");
            Ins(&R, func.body,"            prs = $fldoldgetprs;");
            Ins(&R, func.body,"        } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"                $fldval = $fldival;");
                Ins(&R, func.body,"                prs = true;");
            } else {
                Ins(&R, func.body,"                prs = false;");
            }
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldsetprs;");
            Ins(&R, func.body,"        $fldoldsetass;");
            Ins(&R, func.body,"        $fldoldsetprs;");
            Ins(&R, func.body,"        $fldoldval = $fldval;");
        } else if (op == dmmeta_Fastop_fastop_delta) {
            Ins(&R, func.body,"        if (!$fldoldgetass) {");
            Ins(&R, func.body,"            $fldoldsetass;");
            Ins(&R, func.body,"            bool prs(true);");
            Ins(&R, func.body,"            $fldoldsetprs;");
            Ins(&R, func.body,"            $fldoldval = $flddeltabase;");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        bool prs = !lib_fast::DecodeNull(from);");
            Ins(&R, func.body,"        if (prs) {");
            // Specification explicitly says that
            // "it is a dynamic error [ERR D6] if the previous value is empty."
            // XETRA violates this rule, it sends delta for empty value.
            // We assume that delta is from initial or type-specific base value.
            Ins(&R, func.body,"            if (!$fldoldgetprs) {");
            Ins(&R, func.body,"                $fldoldval = $flddeltabase;");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"            ok = lib_fast::DecodeDelta$fldenc(from,$fldoldval,$fldval,$fldopt);");
            Ins(&R, func.body,"            if (!ok) {");
            Ins(&R, func.body,"                state.error << \"$Field: bad $fldenc delta\" << eol;");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"            $fldoldval = $fldval;");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldsetprs;");
            Ins(&R, func.body,"        $fldoldsetprs;");
        } else if (op == dmmeta_Fastop_fastop_increment) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            prs = !lib_fast::DecodeNull(from);");
            Ins(&R, func.body,"            if (prs) {");
            Ins(&R, func.body,"                ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"                if (!ok) {");
            Ins(&R, func.body,"                    state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"                }");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else if ($fldoldgetass) {");
            Ins(&R, func.body,"            $fldval = $fldoldval + 1;");
            Ins(&R, func.body,"            prs = $fldoldgetprs;");
            Ins(&R, func.body,"        } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"            $fldval = $fldival;");
                Ins(&R, func.body,"            prs = true;");
            } else {
                Ins(&R, func.body,"            prs = false;");
            }
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldsetprs;");
            Ins(&R, func.body,"        $fldoldsetass;");
            Ins(&R, func.body,"        $fldoldsetprs;");
            Ins(&R, func.body,"        $fldoldval = $fldval;");
        } else if (op == dmmeta_Fastop_fastop_tail) {
            prerr("tail operator unsupported");
        }
    } else { // mandatory
        if (op == dmmeta_Fastop_fastop_none) {
            Ins(&R, func.body,"        ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"        if (!ok) {");
            Ins(&R, func.body,"            state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"        }");
        } else if (op == dmmeta_Fastop_fastop_constant) {
            // no-op
        } else if (op == dmmeta_Fastop_fastop_default) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"            if (!ok) {");
            Ins(&R, func.body,"                state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else {");
            Ins(&R, func.body,"            $fldval = $fldival;");
            Ins(&R, func.body,"        }");
        } else if (op == dmmeta_Fastop_fastop_copy) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"            if (!ok) {");
            Ins(&R, func.body,"                state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else if ($fldoldgetass) {");
            Ins(&R, func.body,"            $fldval = $fldoldval;");
            Ins(&R, func.body,"        } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"            $fldval = $fldival;");
            } else {
                Ins(&R, func.body,"            ok = false;");
                Ins(&R, func.body,"            state.error << \"$Field: copy operator: previous value unassigned\" << eol;");
            }
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldoldsetass;");
            Ins(&R, func.body,"        $fldoldval = $fldval;");
        } else if (op == dmmeta_Fastop_fastop_delta) {
            Ins(&R, func.body,"        if (!$fldoldgetass) {");
            Ins(&R, func.body,"            $fldoldsetass;");
            Ins(&R, func.body,"            $fldoldval = $flddeltabase;");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        ok = lib_fast::DecodeDelta$fldenc(from,$fldoldval,$fldval,$fldopt);");
            Ins(&R, func.body,"        if (!ok) {");
            Ins(&R, func.body,"            state.error << \"$Field: bad $fldenc delta\" << eol;");
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldoldval = $fldval;");
        } else if (op == dmmeta_Fastop_fastop_increment) {
            Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
            Ins(&R, func.body,"        if (prs) {");
            Ins(&R, func.body,"            ok = lib_fast::Decode$fldenc(from,$fldval,$fldopt);");
            Ins(&R, func.body,"            if (!ok) {");
            Ins(&R, func.body,"                state.error << \"$Field: bad $fldenc\" << eol;");
            Ins(&R, func.body,"            }");
            Ins(&R, func.body,"        } else if ($fldoldgetass) {");
            Ins(&R, func.body,"            $fldval = $fldoldval + 1;");
            Ins(&R, func.body,"        } else {");
            if (ch_N(Subst(R,"$fldival"))) {
                Ins(&R, func.body,"            $fldval = $fldival;");
            } else {
                Ins(&R, func.body,"            ok = false;");
                Ins(&R, func.body,"            state.error << \"$Field: increment operator: previous value unassigned\" << eol;");
            }
            Ins(&R, func.body,"        }");
            Ins(&R, func.body,"        $fldoldsetass;");
            Ins(&R, func.body,"        $fldoldval = $fldval;");
        } else if (op == dmmeta_Fastop_fastop_tail) {
            vrfy(0,"tail operator unsupported");
        }
    }
}

//------------------------------------------------------------------------------

// Type FAST decode
void amc::tfunc_Ctype_FastDecode() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (FastCodecQ(ctype)) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        AddRetval(func, "bool", "ok", "true");
        AddProtoArg(func, "algo::memptr&", "from");
        if (ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
            AddProtoArg(func, "u64", "pmap");
        }
        AddProtoArg(func, Refto("FastState"),"state");
        AddProtoArg(func, Refto(ctype.cpp_type),"parent");
        // the two pmasks on the namespace's FastState record whose accessors
        // qualify each field's previous value; gen_check_fast has reported a
        // namespace that declares neither
        amc::FPmaskfld *state_ass = FindStatePmask(*ctype.p_ns, "assigned");
        amc::FPmaskfld *state_prs = FindStatePmask(*ctype.p_ns, "present");
        if (ctype.c_cfast->encoding != dmmeta_Fastenc_fastenc_template && PmapBitsN(ctype)) {
            Set(R,"$Ctype",ctype.ctype);
            Ins(&R, func.body , "u64 pmap;");
            Ins(&R, func.body , "ok = lib_fast::DecodePmap(from,pmap);");
            Ins(&R, func.body , "if (!ok) {");
            Ins(&R, func.body , "    state.error << \"$Ctype: bad pmap\" << eol;");
            Ins(&R, func.body , "}");
        }
        int pbit = ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template;
        ind_beg(ctype_c_ffast_curs,ffast,ctype) {
            Set(R, "$Field", ffast.p_field->field);
            Set(R, "$fldname", name_Get(*ffast.p_field));
            Set(R, "$fldkey", ffast.name);
            Set(R, "$fldval", "parent.$fldname");
            Set(R, "$fldsetprs", SetPresentValExpr(*ffast.p_field,"parent","prs"));
            Set(R, "$fldoldval", "state.$fldkey");
            Set(R, "$fldoldgetass", StateBitQExpr(ffast.name, state_ass));
            Set(R, "$fldoldsetass", StateSetBitExpr(ffast.name, state_ass));
            Set(R, "$fldoldgetprs", StateBitQExpr(ffast.name, state_prs));
            Set(R, "$fldoldsetprs", StateSetBitValExpr(ffast.name, state_prs, "prs"));
            Set(R, "$fldargtype", name_Get(*ffast.p_field->p_arg));
            Set(R, "$fldopt", ffast.optional ? "true" : "false");
            Set(R, "$fldpbit", tempstr()<<pbit);
            Set(R, "$fldival", ffast.value.value);
            Set(R, "$fldenc",PrimitiveEncoding(ffast.encoding));
            Set(R, "$flddeltabase",DeltaBase(ffast));
            cstring  cmt = tempstr() << "    // " << name_Get(*ffast.p_field)
                                     << " " << ffast.encoding
                                     << " " << ffast.op
                                     << " " << (ffast.optional?"optional":"mandatory");
            Ins(&R, func.body,cmt);
            Ins(&R, func.body,"    if (ok) {");
            if (ffast.encoding == dmmeta_Fastenc_fastenc_template) {
                prerr("templateRef unsupported");
            } else if (ffast.p_field->reftype == dmmeta_Reftype_reftype_Varlen) {
                Ins(&R, func.body,"        u32 length(0);"); //  zero-length means not present
                Set(R, "$fldval", "length");
                Set(R, "$fldsetprs", ""); // no need to set
                if (ffast.p_field->c_ffast_length) {
                    Set(R, "$fldname", name_Get(*ffast.p_field));
                    Set(R, "$fldkey", ffast.p_field->c_ffast_length->name);
                    Set(R, "$fldoldval", "state.$fldkey");
                    Set(R, "$fldoldgetass", StateBitQExpr(ffast.p_field->c_ffast_length->name, state_ass));
                    Set(R, "$fldoldsetass", StateSetBitExpr(ffast.p_field->c_ffast_length->name, state_ass));
                    Set(R, "$fldoldgetprs", StateBitQExpr(ffast.p_field->c_ffast_length->name, state_prs));
                    Set(R, "$fldoldsetprs", StateSetBitValExpr(ffast.p_field->c_ffast_length->name, state_prs, "prs"));
                    Set(R, "$fldival", ffast.p_field->c_ffast_length->value.value);
                    Set(R, "$fldenc",PrimitiveEncoding(ffast.p_field->c_ffast_length->encoding));
                    Set(R, "$flddeltabase",DeltaBase(*ffast.p_field->c_ffast_length));
                    FastDecodePrimitiveType(R,func,ffast.optional,ffast.p_field->c_ffast_length->op);
                } else {
                    Set(R, "$fldenc","Unsigned");
                    FastDecodePrimitiveType(R,func,ffast.optional,dmmeta_Fastop_fastop_none);
                }
                Ins(&R, func.body , "          if (!ok) {");
                Ins(&R, func.body , "               state.error << \"$Field: bad length\" << eol;");
                Ins(&R, func.body , "          }");
                if (ffast.optional) {
                    Ins(&R, func.body,"        length = prs ? length : 0;");
                }
                Ins(&R, func.body,"            for (unsigned i=0; ok && i<length; ++i) {");
                Ins(&R, func.body,"                $fldargtype &seq = *new(ary_AllocN(lib_fast::_db.varlen,sizeof seq).elems) $fldargtype;");
                Ins(&R, func.body,"                ok = $fldargtype_FastDecode(from,state,seq);");
                Ins(&R, func.body , "              if (!ok) {");
                Ins(&R, func.body , "                  state.error << \"$Field: element #\"<< i+1 << eol;");
                Ins(&R, func.body , "              }");
                Ins(&R, func.body,"            }");
            } else if (ffast.encoding == dmmeta_Fastenc_fastenc_group || ffast.encoding == dmmeta_Fastenc_fastenc_sequence) {
                if (ffast.optional) {
                    Ins(&R, func.body,"        bool prs = lib_fast::GetPmapBit(pmap,$fldpbit);");
                    Ins(&R, func.body,"        $fldsetprs;");
                    Ins(&R, func.body,"        if (prs) {");
                    Ins(&R, func.body,"            ok = $fldargtype_FastDecode(from,state,$fldval);");
                    Ins(&R, func.body,"        }");
                } else {
                    Ins(&R, func.body,"        ok = $fldargtype_FastDecode(from,state,$fldval);");
                }
            } else  if (ffast.encoding == dmmeta_Fastenc_fastenc_type) {
                amc::FField *typefld = GetSoleValField(*ffast.p_field->p_arg);
                if (typefld && typefld->c_ffast_field) {
                    Set(R, "$typefldname",name_Get(*typefld));
                    Set(R, "$fldval", "$fldval.$typefldname");
                    Set(R, "$fldoldval", "$fldoldval.$typefldname");
                    Set(R, "$fldenc", PrimitiveEncoding(typefld->c_ffast_field->encoding));
                    Set(R, "$flddeltabase",DeltaBase(ffast));
                    FastDecodePrimitiveType(R,func,ffast.optional,ffast.op);
                }
            } else {
                FastDecodePrimitiveType(R,func,ffast.optional,ffast.op);
            }
            Ins(&R, func.body,"    }");
            pbit += PmapBitsN(*ffast.p_field);
        }ind_end;
    }
}

//------------------------------------------------------------------------------

// Decode FAST messages
static void DecodeMsgs() {
    algo_lib::Replscope R;
    amc::FNs &ns = *amc::_db.c_ns;
    Set(R,"$ns",ns.ns);
    amc::FFunc *func(NULL);
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (ctype.c_cfast && ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
        if (!func) {
            func = &amc::ind_func_GetOrCreate(Subst(R,"$ns.TemplateHeaderMsgs..FastDecode"));
            Ins(&R, func->ret  , "$ns::TemplateHeader *", false);
            Ins(&R, func->proto, "TemplateHeaderMsgs_FastDecode(algo::memptr &from, FastState &state,algo::ByteAry &buf)", false);
            Ins(&R, func->body , "int index = ary_N(buf);");
            Ins(&R, func->body , "TemplateHeader *ret(NULL);");
            Ins(&R, func->body , "u64 pmap;");
            Ins(&R, func->body , "u32 tid(0);");
            Ins(&R, func->body , "bool ok = lib_fast::DecodePmap(from,pmap);");
            Ins(&R, func->body , "if (!ok) {");
            Ins(&R, func->body , "    state.error << \"$ns.TemplateHeader: bad pmap\" << eol;");
            Ins(&R, func->body , "}");
            Set(R, "$Field", "$ns.TemplateHeader.id");
            Set(R, "$fldval", "tid");
            Set(R, "$fldkey", "tid");
            amc::FPmaskfld *state_ass = FindStatePmask(*ctype.p_ns, "assigned");
            Set(R, "$fldoldval", "state.$fldkey");
            Set(R, "$fldoldgetass", StateBitQExpr("tid", state_ass));
            Set(R, "$fldoldsetass", StateSetBitExpr("tid", state_ass));
            Set(R, "$fldopt", "false");
            Set(R, "$fldpbit", "0");
            Set(R, "$fldival", "");
            Set(R, "$fldenc","Unsigned");
            Ins(&R, func->body , "if (ok) {");
            FastDecodePrimitiveType(R,*func,false,dmmeta_Fastop_fastop_copy);
            Ins(&R, func->body , "}");
            Ins(&R, func->body , "if (ok) {");
            Ins(&R, func->body , "    switch (tid) {");
        }
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        Set(R, "$TID", tempstr() << ctype.c_cfast->id);
        Ins(&R, func->body , "    case $TID:");
        Ins(&R, func->body , "        {");
        if (ctype.c_cfast->reset) {
            Ins(&R, func->body , "            state.assigned = 0;");
            Ins(&R, func->body , "            state.present = 0;");
        }
        if (amc::RuntimeFrameLenQ(ctype)) {
            Ins(&R, func->body , "            ary_RemoveAll(lib_fast::_db.varlen);");
        }
        Ins(&R, func->body , "            $Cpptype &msg = *new(ary_AllocN(buf,sizeof msg).elems) $Cpptype;");
        Ins(&R, func->body , "            ok = $Name_FastDecode(from,pmap,state,msg);");
        if (amc::RuntimeFrameLenQ(ctype)) {
            Ins(&R, func->body , "            msg.length += ary_N(lib_fast::_db.varlen);");
            Ins(&R, func->body , "            ary_Addary(buf,ary_Getary(lib_fast::_db.varlen));");
            Ins(&R, func->body , "            ary_RemoveAll(lib_fast::_db.varlen);");
        }
        Ins(&R, func->body , "            ret = ok ? &Castbase(*($Cpptype*)(buf.ary_elems+index)) :  NULL;");
        Ins(&R, func->body , "        }");
        Ins(&R, func->body , "        break;");
    }ind_end;
    if (func) {
        Ins(&R, func->body , "    default:");
        Ins(&R, func->body , "       ok = false;");
        Ins(&R, func->body , "       state.error << \"$ns.TemplateHeader.id: unknown template \" << tid << eol;");
        Ins(&R, func->body , "    }");
        Ins(&R, func->body , "}");
        Ins(&R, func->body , "return ret;");
        MaybeUnused(*func,"from");
        MaybeUnused(*func,"state");
        MaybeUnused(*func,"buf");
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------    ENCODE FIX   -------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void amc::tfunc_Ctype_FixEncode() {
    algo_lib::Replscope R;
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (ctype.c_cfast && ctype.c_cfast->encoding != dmmeta_Fastenc_fastenc_type) {
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        amc::FFunc& func = amc::CreateCurFunc(true);
        Ins(&R, func.ret  , "void", false);
        AddProtoArg(func, Refto("cstring"), "buf");
        AddProtoArg(func, Refto(ctype.cpp_type), "parent");
        AddProtoArg(func, "char", "soh");
        ind_beg(ctype_c_ffast_curs,ffast,ctype) if (fastinstr_Get(ffast) != dmmeta_Fastinstr_fastinstr_length) {
            Set(R, "$fldname", name_Get(*ffast.p_field));
            Set(R, "$fldval", ffast.p_field->c_cppfunc ? "$fldname_Get(parent)" : "parent.$fldname");
            Set(R, "$fldgetprs", amc::PresentQExpr(*ffast.p_field,"parent"));
            Set(R, "$fldtag", tempstr()<<ffast.id);
            Set(R, "$fldargtype", name_Get(*ffast.p_field->p_arg));
            if (ffast.p_field->c_ffast_length && ffast.p_field->c_ffast_length->id) {
                Set(R, "$fldlentag", tempstr()<<ffast.p_field->c_ffast_length->id);
                if (ffast.encoding == dmmeta_Fastenc_fastenc_sequence) {
                    if (ffast.p_field->reftype == dmmeta_Reftype_reftype_Varlen) {
                        Ins(&R, func.body,"    {");
                        Ins(&R, func.body,"        u32 length = $fldname_N(parent);");
                        Ins(&R, func.body,"        if (length) {");
                        Ins(&R, func.body,"            buf << \"$fldlentag=\" << length << soh;");
                        Ins(&R, func.body,"        }");
                        Ins(&R, func.body,"    }");
                    } else {
                        Ins(&R, func.body,"    buf << \"$fldlentag=1\" << soh;");
                    }
                } else {
                    if (ffast.optional) {
                        Ins(&R, func.body,"    if ($fldgetprs) {");
                        Ins(&R, func.body,"        buf << \"$fldlentag=\" << ch_N(parent) << soh;");
                        Ins(&R, func.body,"    }");
                    } else {
                        Ins(&R, func.body,"    buf << \"$fldlentag=\" << ch_N(parent) << soh;");
                    }
                }
            }
            if (ffast.encoding == dmmeta_Fastenc_fastenc_template) {
                prerr("templateRef unsupported");
            } else if (ffast.encoding == dmmeta_Fastenc_fastenc_group) {
                if (ffast.optional) {
                    Ins(&R, func.body,"    if ($fldgetprs) {");
                    Ins(&R, func.body,"         $fldargtype_FixEncode(buf,$fldval,soh);");
                    Ins(&R, func.body,"    }");
                } else {
                    Ins(&R, func.body,"    $fldargtype_FixEncode(buf,$fldval, soh);");
                }
            } else if (ffast.p_field->reftype == dmmeta_Reftype_reftype_Varlen) {
                Ins(&R, func.body,"    ind_beg($Name_$fldname_curs,elem,parent) {");
                Ins(&R, func.body,"        $fldargtype_FixEncode(buf,elem,soh);");
                Ins(&R, func.body,"    }ind_end;");
            } else if (ffast.id) {
                if (ffast.optional) {
                    Ins(&R, func.body,"    if ($fldgetprs) {");
                    Ins(&R, func.body,"        buf << \"$fldtag=\" << $fldval << soh;");
                    Ins(&R, func.body,"    }");
                } else {
                    Ins(&R, func.body,"    buf << \"$fldtag=\" << $fldval << soh;");
                }
            }
        }ind_end;
    }
}

//------------------------------------------------------------------------------

// Encode FIX message
static void EncodeFixMsgs() {
    algo_lib::Replscope R;
    amc::FNs &ns = *amc::_db.c_ns;
    Set(R,"$ns",ns.ns);
    amc::FFunc *func(NULL);
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) if (ctype.c_cfast && ctype.c_cfast->encoding == dmmeta_Fastenc_fastenc_template) {
        if (!func) {
            func = &amc::ind_func_GetOrCreate(Subst(R,"$ns.TemplateHeaderMsgs..FixEncode"));
            Ins(&R, func->ret  , "void", false);
            Ins(&R, func->proto, "TemplateHeaderMsgs_FixEncode(cstring &buf, $ns::TemplateHeader &header, char soh)", false);
            Ins(&R, func->body , "    switch (header.id) {");
        }
        Set(R, "$Cpptype", ctype.cpp_type);
        Set(R, "$Name", name_Get(ctype));
        Set(R, "$TID", tempstr() << ctype.c_cfast->id);
        Ins(&R, func->body , "    case $TID:");
        Ins(&R, func->body , "        $Name_FixEncode(buf,*$Cpptype_Castdown(header),soh);");
        Ins(&R, func->body , "        break;");
    }ind_end;
    if (func) {
        Ins(&R, func->body , "    }");
    }
}

//------------------------------------------------------------------------------

// FAST - ns functions
void amc::gen_ns_fast() {
    EncodeMsgs();
    DecodeMsgs();
    EncodeFixMsgs();
}
