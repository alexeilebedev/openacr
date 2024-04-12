// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/amc/disp/casetype.cpp
//

#include "include/amc.h"


// ----------------------------------------------------------------------------

// Create a casetype suitable for this dispatch.
// A casetype is necessary when there is no single header type that
// can be used to describe all the leaves of the dispatch.
// For instance, a dispatch that mixes ns:A and ns:B messages
// is still usable (with Read) because we create a special casetype.
static amc::FCtype *CreateCasetype(amc::FDispatch &dispatch) {
    // Case type itself
    dmmeta::Ctype ctype;
    ctype.ctype = tempstr() << dispatch.dispatch<<"Case";
    ctype.comment.value = tempstr() << "Enum for dispatch "<<dispatch.dispatch;
    amc::FCtype *casetype = amc::ctype_InsertMaybe(ctype);

    // Value
    // #AL# TODO: what if this value is narrower than the target message type?
    dmmeta::Field field(tempstr() << ctype.ctype << ".value"
                        , "u32"
                        , dmmeta_Reftype_reftype_Val
                        , algo::CppExpr()
                        , algo::Comment());
    amc::FField &ffield = *amc::InsField(field);
    (void)ffield;

    // Read/write this casetype
    dmmeta::Cfmt cfmt(tempstr() << ctype.ctype << "." << dmmeta_Strfmt_strfmt_String
                      , dmmeta_Printfmt_printfmt_Raw
                      , true // read
                      , false // print
                      , "" // separator
                      , true // genop
                      , algo::Comment());
    amc::cfmt_InsertMaybe(cfmt);

    // Make it packed. Why?
    dmmeta::Pack pack;
    pack.ctype=ctype.ctype;
    amc::pack_InsertMaybe(pack);

    // Allow constructor
    dmmeta::Cpptype cpptype;
    cpptype.ctype = ctype.ctype;
    cpptype.ctor = true;
    amc::cpptype_InsertMaybe(cpptype);

    amc::anonfld_InsertMaybe(dmmeta::Anonfld(field.field, algo::Comment()));
    amc::fcast_InsertMaybe(dmmeta::Fcast(field.field, "", algo::Comment()));

    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispmsg, dispatch) {
        dmmeta::Fconst fconst;
        fconst.fconst      = tempstr() << ctype.ctype << ".value/" << ctype_Get(dispmsg);
        fconst.value.value = dispmsg.selector;
        amc::fconst_InsertMaybe(fconst);
        // be able to read lowercase name as well as the actual ctype name
        if (dispmsg.p_ctype->c_ssimfile) {
            fconst.fconst      = tempstr() << ctype.ctype << ".value/" << dispmsg.p_ctype->c_ssimfile->ssimfile;
            amc::fconst_InsertMaybe(fconst);
        }
    }ind_end;

    return casetype;
}

// -----------------------------------------------------------------------------

// Look for the earliest base type which defines a type field
// But isn't itself a specific message.
// That will be our header.
static amc::FCtype *GuessMsgHeader(amc::FCtype *ctype) {
    amc::FCtype *ret=NULL;
    amc::FCtype *hdr = GetBaseType(*ctype,NULL);
    if (hdr) {
        ret = GuessMsgHeader(hdr);
    }
    if (!ret && ctype->c_typefld && !ctype->c_msgtype) {
        ret=ctype;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Determine common header for all messages in the dispatch
// Compute values of P_CTYPE_HDR and P_CASETYPE.
// P_CTYPE_HDR is non-NULL if all of the messages in the dispatch share the same message
// header.
// P_CASETYPE is always non-NULL. It is set to the type field of the common message
// header, if one exists, or to a newly created enum.
void amc::Disp_CreateCasetype(amc::FDispatch &dispatch) {
    amc::FCtype *common_base=NULL;
    bool multiheader = false;
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispatch_msg, dispatch) {
        amc::FCtype *ctype = dispatch_msg.p_ctype;
        amc::FCtype *base = GuessMsgHeader(ctype);
        if (!common_base) {
            common_base = base;
        } else if (common_base == base) {
        } else {
            multiheader=true;
        }
    }ind_end;
    dispatch.p_ctype_hdr = multiheader ? NULL : common_base;
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispmsg, dispatch) {
        if (dispatch.p_ctype_hdr && !multiheader) {
            vrfy(dispmsg.p_ctype->c_msgtype,
                 tempstr()<<"amc.bad_dispatch_msg"
                 <<Keyval("dispatch_msg",dispmsg.dispatch_msg)
                 <<Keyval("ctype",ctype_Get(dispmsg))
                 <<Keyval("comment","Ctype is missing a msgtype field"));
            dispmsg.selector << dispmsg.p_ctype->c_msgtype->type.value;
        } else {
            dispmsg.selector << ind_curs(dispmsg).index+1;
        }
    }ind_end;
    amc::FCtype *hdr = dispatch.p_ctype_hdr;
    amc::FTypefld *typefld = hdr ? hdr->c_typefld : NULL;
    amc::FCtype *casetype = typefld ? typefld->p_field->p_arg : NULL;
    bool use_casetype = !hdr || !typefld || c_field_N(*casetype)==0;
    dispatch.p_casetype = use_casetype ? CreateCasetype(dispatch) : casetype;
}
