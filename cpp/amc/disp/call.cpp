// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/disp/call.cpp
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void Call_UpdateCycles(amc::FDispatch &dispatch, amc::FDispatchmsg &dispmsg) {
    algo_lib::Replscope R;
    Set(R, "$ns", ns_Get(dispatch));
    Set(R, "$Dname", name_Get(dispatch));
    Set(R, "$Disp", dispatch.dispatch);
    Set(R, "$Msgname", StripNs("",ctype_Get(dispmsg)));
    amc::FFunc& updatecycles = amc::ind_func_GetOrCreate(Subst(R, "$Disp.$Msgname.UpdateCycles"));
    updatecycles.glob        = true;
    updatecycles.priv        = true;
    Ins(&R, updatecycles.ret    , "void",false);
    Ins(&R, updatecycles.proto  , "$Dname_$Msgname_UpdateCycles()",false);
    Ins(&R, updatecycles.body, "u64 cur_cycles = algo::get_cycles();");
    Ins(&R, updatecycles.body, "$ns::_db.trace.dispatch_$Dname_$Msgname_cycles += cur_cycles - algo_lib::_db.clock;");
    Ins(&R, updatecycles.body, "algo_lib::_db.clock = algo::SchedTime(cur_cycles);");
}

// -----------------------------------------------------------------------------

static void AddCtxProtoArg(amc::FDispatch &dispatch, amc::FFunc &func) {
    if (dispatch.c_dispctx) {
        amc::AddProtoArg(func,tempstr()<<dispatch.c_dispctx->p_ctype->cpp_type<<" &", "ctx");
    }
}

static void Call_msgname(algo_lib::Replscope &R, amc::FDispatch &dispatch) {
    ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,dispatch) {
        Set(R, "$msgname", StripNs("",ctype_Get(msg)));
        Set(R, "$msgns", GetNs(ctype_Get(msg)));
        amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Disp.$msgns.$msgname"));
        func.extrn = true;
        func.ret = "void";
        Ins(&R, func.comment, "User-implemented callback function for dispatch $Disp", false);
        Ins(&R, func.proto, "$Disp_$msgname()", false); {
            AddCtxProtoArg(dispatch,func);
            amc::AddProtoArg(func, tempstr() << msg.p_ctype->cpp_type << " &", "msg");
            amc::AddProtoArg(func, "u32", "msg_len", dispatch.haslen);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Call-dispatch by type
static void Call_Raw(algo_lib::Replscope &R, amc::FDispatch &dispatch) {
    bool trace_cycle = dispatch.c_disptrace && dispatch.c_disptrace->cycle;
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Disp..DispatchRaw"));
    func.ret = "int";
    func.glob = true;
    Ins(&R, func.proto, "$DispDispatchRaw()", false);
    AddCtxProtoArg(dispatch,func);
    amc::AddProtoArg(func, dispatch.p_casetype->cpp_type, "type");
    amc::AddProtoArg(func, "u8 *", "msg");
    amc::AddProtoArg(func, "u32", "len");
    Ins(&R, func.body        , "int ret = 0;");
    Ins(&R, func.body        , "switch(type) {" );
    ind_beg(amc::dispatch_c_dispatch_msg_curs, dispmsg,dispatch) {
        bool strict = dispatch.strict && !VarlenQ(*dispmsg.p_ctype);
        Set(R, "$Msgtype", dispmsg.selector);
        Set(R, "$Msgname", StripNs("",ctype_Get(dispmsg)));
        Set(R, "$Ctype", amc::NsToCpp(ctype_Get(dispmsg)));
        Set(R, "$Compare", strict ? "==" : "<=");
        Ins(&R, func.body    , "case $Msgtype: if (sizeof($Ctype) $Compare len) {");
        if (dispatch.c_disptrace) {
            Ins(&R, func.body, "    ++$ns::_db.trace.dispatch_$Disp_$Msgname;");
        }
        Ins(&R, func.body    , "    $ns::$Disp_$Msgname();"); {
            amc::AddArg(func.body, "ctx", dispatch.c_dispctx);
            amc::AddArg(func.body, Subst(R,"($Ctype&)*msg"));
            amc::AddArg(func.body, "len", dispatch.haslen);
        }
        if (trace_cycle) {
            Ins(&R, func.body, "    $ns::$Disp_$Msgname_UpdateCycles();");
        }
        Ins(&R, func.body    , "    ret = (int)sizeof($Ctype);");
        Ins(&R, func.body    , "} break;");
    }ind_end;
    Ins(&R, func.body        , "default:");
    if (dispatch.c_disptrace) {
        Ins(&R, func.body    , "    ++$ns::_db.trace.dispatch_$Disp_Unkmsg;");
    }
    if (dispatch.unk) {
        Set(R, "$Hdrtype" , dispatch.p_ctype_hdr->cpp_type);
        Ins(&R, func.body    , "    ret = $ns::$Disp_Unkmsg();"); {
            amc::AddArg(func.body, "ctx", dispatch.c_dispctx);
            amc::AddArg(func.body, Subst(R,"*($Hdrtype*)msg"));
            amc::AddArg(func.body, "len", dispatch.haslen);
        }
    } else {
        Ins(&R, func.body    , "    break;");
    }
    Ins(&R, func.body        , "}");
    Ins(&R, func.body        , "return ret;" );
}

// -----------------------------------------------------------------------------

static bool CanDispatchHdr(amc::FDispatch &dispatch) {
    return dispatch.p_ctype_hdr
        && (dispatch.haslen || dispatch.p_ctype_hdr->c_lenfld)
        && (dispatch.p_ctype_hdr->c_typefld);
}

// -----------------------------------------------------------------------------

// Dispatch entry point given a reference to a message header.
// Address of message is the same as the addres of the message header
// Message type is obtained from the header
// Message length is obtained from the header if known, as parameter if unknown
// Return accepted message length, or 0 if unknown
static void Call_Hdr(algo_lib::Replscope &R, amc::FDispatch &dispatch) {
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Disp..Dispatch"));
    func.ret = "int";
    func.glob = true;
    Ins(&R, func.proto, "$DispDispatch()", false); {
        AddCtxProtoArg(dispatch,func);
        amc::AddProtoArg(func, amc::Refto(dispatch.p_ctype_hdr->cpp_type)<<" ", "msg");
        amc::AddProtoArg(func, "u32", "msg_len", dispatch.haslen);
    }
    // expression to access the value of the type field
    // given a header reference.
    amc::FCtype *hdr=dispatch.p_ctype_hdr;
    Ins(&R, func.body, "return $DispDispatchRaw();"); {
        amc::AddArg(func.body, "ctx", dispatch.c_dispctx);
        amc::AddArg(func.body, amc::Castto(dispatch.p_casetype->cpp_type
                                           , FieldvalExpr(hdr, *hdr->c_typefld->p_field, "msg")));
        amc::AddArg(func.body, "(u8*)&msg");
        amc::AddArg(func.body, !dispatch.haslen ? amc::LengthExpr(*hdr, "msg") : strptr("msg_len"));
    }
}

// -----------------------------------------------------------------------------

// Dispatch entry point which takes
// - message head reference
// - length, if absent in header
// Return void
static void Call_Hdr2(algo_lib::Replscope &R, amc::FDispatch &dispatch) {
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Disp..Dispatch2"));
    func.ret = "void";
    func.glob = true;
    Ins(&R, func.comment, "void rettype useful for hooks", false);
    Ins(&R, func.proto  , "v$DispDispatch()", false); {
        AddCtxProtoArg(dispatch,func);
        amc::AddProtoArg(func, amc::Refto(dispatch.p_ctype_hdr->cpp_type)<<" ", "msg");
        amc::AddProtoArg(func, "u32", "msg_len", dispatch.haslen);
    }
    Ins(&R, func.body   , "$DispDispatch();"); {
        amc::AddArg(func.body, "ctx", dispatch.c_dispctx);
        amc::AddArg(func.body, "msg");
        amc::AddArg(func.body, "msg_len", dispatch.haslen);
    }
}

// -----------------------------------------------------------------------------

// Declaration for user-provided function for the default case
static void Call_Unkmsg(algo_lib::Replscope &R, amc::FDispatch &dispatch) {
    amc::FFunc &func = amc::ind_func_GetOrCreate(Subst(R, "$ns.$Disp..Unkmsg"));
    func.extrn = true;
    func.ret = "int";
    Ins(&R, func.comment, "User-implemented callback function for dispatch $Disp", false);
    Ins(&R, func.proto, "$Disp_Unkmsg()", false); {
        AddCtxProtoArg(dispatch,func);
        amc::AddProtoArg(func, amc::Refto(dispatch.p_ctype_hdr->cpp_type)<<" ", "msg");
        amc::AddProtoArg(func, "u32", "msg_len", dispatch.haslen);
    }
}

// -----------------------------------------------------------------------------

// Generator for call-type dispatch (where we invoke a user function
// for each case)
void amc::Disp_Call(amc::FDispatch& dispatch) {
    bool candispatchhdr = CanDispatchHdr(dispatch);
    bool trace_cycle = dispatch.c_disptrace && dispatch.c_disptrace->cycle;
    algo_lib::Replscope R;
    Set(R, "$Disp", name_Get(dispatch));
    Set(R, "$ns"  , ns_Get(dispatch));

    if (trace_cycle) {
        ind_beg(amc::dispatch_c_dispatch_msg_curs, msg,dispatch) {
            Call_UpdateCycles(dispatch,msg);
        }ind_end;
    }

    vrfy(candispatchhdr  || !dispatch.unk
         , tempstr() << "amc.say_unk"
         <<Keyval("dispatch",dispatch.dispatch)
         <<Keyval("comment","dispatch by header is not possible, please set unk:N"));

    // external declarations!
    Call_msgname(R,dispatch);

    if (dispatch.unk) {
        Call_Unkmsg(R,dispatch);
    }

    // Dispatch by casetype!
    // This is the raw form
    Call_Raw(R, dispatch);

    // Dispatching by header alone is not always possible
    // (missing length field, missing type field in header etc.)
    if (candispatchhdr) {
        Call_Hdr(R, dispatch);
        Call_Hdr2(R, dispatch);
    }
}
