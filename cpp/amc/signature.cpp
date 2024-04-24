// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/signature.cpp
//

#include "include/amc.h"
#include "include/sha.h"

// Recursive function to compute all ctype dependencies
// (transitive/reflexive closure for ctype on fields)
// Global purpose is to detect the change in deep bottom type
// which impacts binary footprint of the top type
void amc::SignatureVisit(amc::FCtype &ctype) {
    if(!zs_sig_visit_InLlistQ(ctype)) {
        zs_sig_visit_Insert(ctype);
        amc::FCtype *base = GetBaseType(ctype,NULL);
        if (base) {
            SignatureVisit(*base);
        }
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            // for binary dependency, take only in-place instances
            // indices and dynamic instances are not interesting
            // in case of Pkey, key field type is taken
            // functions are also interesting
            bool is_val_inst = field.reftype == dmmeta_Reftype_reftype_Val;
            is_val_inst     |= field.reftype == dmmeta_Reftype_reftype_Inlary;
            is_val_inst     |= field.reftype == dmmeta_Reftype_reftype_Opt;
            is_val_inst     |= field.reftype == dmmeta_Reftype_reftype_Varlen;
            if (is_val_inst) {
                SignatureVisit(*field.p_arg);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void amc::gen_prep_signature() {
    // compute own ctype signature
    // hash fields only those impact binary compatibility
    // signature change is more predictable vs. hashing whole ssim line
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        ctype.original = true;
        Sha1Ctx signature;
        // ctype name
        Update(signature, strptr_ToMemptr(name_Get(ctype)));
        ind_beg(amc::ctype_c_field_curs, field,ctype) {
            Update(signature, strptr_ToMemptr(field.field));
            Update(signature, strptr_ToMemptr(field.arg));
            Update(signature, strptr_ToMemptr(field.reftype));
            if (WidthMin(field)>1) {
                Update(signature, strptr_ToMemptr(tempstr()<<WidthMin(field)));
            }
            if (WidthMax(field) > WidthMin(field)) {
                Update(signature, strptr_ToMemptr(tempstr()<<WidthMax(field)));
            }
            if (field.c_smallstr) {
                Update(signature, strptr_ToMemptr(tempstr()<<field.c_smallstr->length));
                Update(signature, strptr_ToMemptr(field.c_smallstr->strtype));
                Update(signature, strptr_ToMemptr(field.c_smallstr->pad.value));
                if (field.c_smallstr->c_numstr) {
                    Update(signature, strptr_ToMemptr(tempstr()<<field.c_smallstr->c_numstr->base));
                }
            }
        }ind_end;
        Finish(signature);
        ctype.signature = GetDigest(signature);
    }ind_end;
    // mix-in fconst to ctype
    ind_beg(amc::_db_fconst_curs,fconst,amc::_db) {
        Sha1Ctx signature;
        Update(signature, strptr_ToMemptr(fconst.fconst));
        Update(signature, strptr_ToMemptr(fconst.value.value));
        Finish(signature);
        CombineSignaturesUnordered(fconst.p_field->p_ctype->signature, GetDigest(signature));
    }ind_end;

    // compute dispatch signatures
    ind_beg(amc::_db_dispatch_curs,dispatch,amc::_db) {
        // combine all ctypes
        ind_beg(amc::dispatch_c_dispatch_msg_curs,dispatch_msg,dispatch) {
            SignatureVisit(*dispatch_msg.p_ctype);
        }ind_end;
        // update dispatch signature
        ind_beg(amc::_db_zs_sig_visit_curs,dep_ctype,amc::_db) {
            CombineSignaturesUnordered(dispatch.signature,dep_ctype.signature);
        }ind_end;
        amc::zs_sig_visit_RemoveAll();
        // add to dispsig
        dmmeta::Dispsig dispsig;
        dispsig.dispsig = dispatch.dispatch;
        dispsig.signature = dispatch.signature;
        amc::dispsig_InsertMaybe(dispsig);
    }ind_end;

    // compute ns input signature
    ind_beg(amc::_db_ns_curs,ns,amc::_db) {
        bool has_input = false;
        ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
            ind_beg(amc::ctype_c_field_curs,field,ctype) if (field.c_finput) {
                has_input = true;
                SignatureVisit(*field.p_arg); // cbase maybe?
            }ind_end;
            // update ns signature
            ind_beg(amc::_db_zs_sig_visit_curs,dep_ctype,amc::_db) {
                CombineSignaturesUnordered(ns.signature_input,dep_ctype.signature);
            }ind_end;
            amc::zs_sig_visit_RemoveAll();
        }ind_end;
        // add to dispsig
        if (has_input) {
            dmmeta::Dispsig dispsig;
            dispsig.dispsig = tempstr()<<ns.ns<<".Input";
            dispsig.signature = ns.signature_input;
            amc::dispsig_InsertMaybe(dispsig);
        }
    }ind_end;

    // sort tables for output
    amc::c_dispsig_sorted_QuickSort();
}

// -----------------------------------------------------------------------------

// compute order-independent superposition of two signatures
// that means H(A,B) == H(B,A)
// sum byte-wise without carry
// exception if length does not match
void amc::CombineSignaturesUnordered(algo::Sha1sig &dst, algo::Sha1sig src) {
    frep_(i,sha1sig_N(dst)) {
        sha1sig_qFind(dst,i) += sha1sig_qFind(src,i);
    }
}
