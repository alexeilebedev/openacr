// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
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
// Source: cpp/amc/pnew.cpp -- Custom constructors
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool AcceptFieldQ(amc::FCtype &ctype, amc::FField &field) {
    bool skip = false;
    skip |=  GetLenfld(field) != NULL;
    skip |=  field.c_typefld && ctype.c_msgtype;
    skip |=  FldfuncQ(field);
    skip |=  PadQ(field);
    bool ok = field.reftype == dmmeta_Reftype_reftype_Varlen
        || field.reftype == dmmeta_Reftype_reftype_Opt
        || ValQ(field);
    return ok && !skip;}


// -----------------------------------------------------------------------------

static void PnewMemptr(algo_lib::Replscope &R, amc::Genpnew &pnew) {
    AddProtoArg(*pnew.p_func, "algo::memptr &", "buf");
    Ins(&R, pnew.p_func->comment, "After constructing, advance BUF appropriate number of bytes forward");

    Ins(&R, pnew.preamble, "if (len > u32(elems_N(buf))) {");
    Ins(&R, pnew.preamble, "    return NULL; // no room.");
    Ins(&R, pnew.preamble, "}");
    Ins(&R, pnew.preamble, "msg = ($Cpptype*)buf.elems;");
    Ins(&R, pnew.preamble, "buf        = RestFrom(buf,len);");
}

// -----------------------------------------------------------------------------

static void PnewByteAry(algo_lib::Replscope &R, amc::Genpnew &pnew) {
    AddProtoArg(*pnew.p_func, "algo::ByteAry &", "buf");

    Ins(&R, pnew.preamble, "ary_RemoveAll(buf);");
    Ins(&R, pnew.preamble, "msg = ($Cpptype*)ary_AllocN(buf,len).elems;");
}

static void PnewAppend(algo_lib::Replscope &R, amc::Genpnew &pnew) {
    AddProtoArg(*pnew.p_func, "algo::ByteAry &", "buf");

    Ins(&R, pnew.preamble, "msg = ($Cpptype*)ary_AllocN(buf,len).elems;");
}

// -----------------------------------------------------------------------------

static void PnewAmsStream(algo_lib::Replscope &R, amc::Genpnew &pnew) {
    AddProtoArg(*pnew.p_func, "lib_ams::FStream &", "stream");
    pnew.req_pack=true;

    if (bool_Update(amc::_db.has_ams_fwd_declare,true)) {
        if (amc::FNs *ns = amc::ind_ns_Find("lib_ams")) {
            // forward-declare
            BeginNsBlock(*ns->hdr, *ns, "");
            Ins(&R, *ns->hdr, "struct FStream;");
            Ins(&R, *ns->hdr, "void *BeginWrite(lib_ams::FStream &stream, int len);");
            Ins(&R, *ns->hdr, "void EndWrite(lib_ams::FStream &stream, void *msg, int len);");
            EndNsBlock(*ns->hdr, *ns, "");
        }
    }

    Ins(&R, pnew.preamble, "msg = ($Cpptype*)lib_ams::BeginWrite(stream,int(len));");
    Ins(&R, pnew.preamble, "if (!msg) {");
    Ins(&R, pnew.preamble, "    return NULL; // no room.");
    Ins(&R, pnew.preamble, "}");

    Ins(&R, pnew.postamble, "lib_ams::EndWrite(stream,msg,int(len));");
}

// -----------------------------------------------------------------------------

static void Pnew_CopyFields(amc::Genpnew &genpnew) {
    algo_lib::Replscope &R=genpnew.R;
    amc::FFunc &func =*genpnew.p_func;
    amc::FCtype &ctype =*genpnew.p_ctype;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        Set(R, "$name", name_Get(field));
        if (PadQ(field)) {
            // initialize pad bytes to zeros.
            // not doing so is a security leak.
            Ins(&R, func.body, "memset(&msg->$name, 0, sizeof(msg->$name));");
        } else if (field.c_typefld && ctype.c_msgtype) {
            Ins(&R, func.body, AssignExpr(field, "*msg", ctype.c_msgtype->type.value, true)<<";");
        } else if (FixaryQ(field)) {
            Ins(&R, func.body, "$name_Setary(*msg, $name);");
        } else if (amc::FLenfld *lenfld = GetLenfld(field)) {
            Set(R, "$extra", tempstr() << lenfld->extra);
            Ins(&R, func.body, AssignExpr(field, "*msg", "len + ($extra)", true)<<";");
        } else if (field.reftype == dmmeta_Reftype_reftype_Varlen) {
            Ins(&R, func.body, "memcpy($name_Addr(*msg), $name.elems, ary_len);");
        } else if (field.reftype == dmmeta_Reftype_reftype_Opt) {
            Ins(&R, func.body, "if ($name) {");
            Ins(&R, func.body, "    memcpy((u8*)msg + sizeof($Cpptype), $name, opt_len);");
            Ins(&R, func.body, "}");
        } else if (!FldfuncQ(field) && ValQ(field)) {
            Ins(&R, func.body, AssignExpr(field, "*msg", "$name", false)<<";");
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void HandleLen(amc::Genpnew &genpnew) {
    algo_lib::Replscope &R=genpnew.R;
    amc::FFunc &func =*genpnew.p_func;
    amc::FCtype &ctype = *genpnew.p_ctype;

    Ins(&R, func.body, "size_t len = sizeof($Cpptype);");

    if (ctype.c_varlenfld) {
        Set(R, "$name", name_Get(*ctype.c_varlenfld));
        Set(R, "$Vartype", ctype.c_varlenfld->p_arg->c_lenfld ? strptr("u8") : ctype.c_varlenfld->cpp_type);
        Ins(&R, func.body, tempstr() << "u32 ary_len = elems_N($name) * sizeof($Vartype);");
        Ins(&R, func.body, "len += ary_len;");
    }

    if (ctype.c_optfld) {
        Set(R, "$name", name_Get(*ctype.c_optfld));
        if (!ctype.c_optfld->p_arg->c_lenfld) {
            AddProtoArg(func, "int", "opt_len");
        } else {
            Set(R, "$optlen", LengthExpr(*ctype.c_optfld->p_arg, "$name[0]"));
            Ins(&R, func.body, "int opt_len = $name ? $optlen : 0;");
        }
        Ins(&R, func.body, "len += opt_len;");
    }
}

// -----------------------------------------------------------------------------

static void DispatchBuftype(amc::FPnew &pnew, amc::Genpnew &genpnew) {
    algo_lib::Replscope &R=genpnew.R;
    amc::Pnewtype pnewtype;
    value_SetStrptr(pnewtype,buftype_Get(pnew),amc_Pnewtype_Memptr);
    switch(pnewtype) {
    case amc_Pnewtype_Memptr      : PnewMemptr(R, genpnew); break;
    case amc_Pnewtype_ByteAry     : PnewByteAry(R, genpnew); break;
    case amc_Pnewtype_AmsStream   : PnewAmsStream(R, genpnew); break;
    case amc_Pnewtype_Append      : PnewAppend(R, genpnew); break;
    default                           : vrfy(0, "unsupported buftype"); break;
    }
}

// -----------------------------------------------------------------------------

// Generate various constructors
void amc::GenPnew(amc::FNs& ns, amc::FPnew& pnew, amc::FCtype& ctype) {
    amc::Genpnew genpnew;
    genpnew.p_ctype = &ctype;
    algo_lib::Replscope &R=genpnew.R;
    Set(R, "$ns", ns.ns);
    Set(R, "$Cpptype", ctype.cpp_type);
    Set(R, "$Name", name_Get(ctype));
    Set(R, "$Buftype", buftype_Get(pnew));

    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R,"$ns...$Name_Fmt$Buftype"));
    genpnew.p_func = &func;
    func.glob = true;
    AddRetval(func, Subst(R, "$Cpptype *"), "msg", "NULL");
    func.proto = Subst(R, "$Name_Fmt$Buftype()");
    Ins(&R, func.comment, "Construct a new $Cpptype in the space provided by BUF.");
    Ins(&R, func.comment, "If BUF doesn't have enough space available, throw exception.");
    DispatchBuftype(pnew, genpnew);

    // build function arguments
    ind_beg(amc::ctype_c_field_curs, field,ctype) {
        if (AcceptFieldQ(ctype,field)) {
            AddProtoArg(func, Argtype(field), name_Get(field));
        }
    }ind_end;

    bool hasret = buftype_Get(pnew) != "StreamTuple";
    if (hasret) {
        HandleLen(genpnew);
    }
    func.body << genpnew.preamble;
    // set up 'msg' pointer
    // we use early 'return NULL' in case allocation fails
    if (hasret) {
        Pnew_CopyFields(genpnew);
    }
    MaybeUnused(func,"msg");
    func.body << genpnew.postamble;

    vrfy(!genpnew.req_pack || ctype.c_pack
         , tempstr()<<"amc.pnewpack"
         <<Keyval("buftype",buftype_Get(pnew))
         <<Keyval("ctype",ctype.ctype)
         <<Keyval("comment", "Buffer type requires ctype to be packed"));
}
