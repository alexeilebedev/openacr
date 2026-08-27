// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Exceptions: yes
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
    Ins(&R, pnew.p_func->comment, "If BUF has no room for the message, construct nothing and return NULL.");
    Ins(&R, pnew.p_func->comment, "After constructing, advance BUF appropriate number of bytes forward");

    Ins(&R, pnew.preamble, "if (len > u64(elems_N(buf))) {");
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

static void PnewShm(algo_lib::Replscope &R, amc::Genpnew &pnew) {
    AddProtoArg(*pnew.p_func, "lib_ams::FShm &", "shm");
    Ins(&R, pnew.p_func->comment, "If SHM has no room for the message, construct nothing and return NULL.");
    pnew.req_pack=true;

    if (bool_Update(amc::_db.has_ams_fwd_declare,true)) {
        if (amc::FNs *ns = amc::ind_ns_Find("lib_ams")) {
            // forward-declare
            BeginNsBlock(*ns->hdr, *ns, "");
            Ins(&R, *ns->hdr, "struct FShm;");
            Ins(&R, *ns->hdr, "void *BeginWrite(lib_ams::FShm &shm, int len);");
            Ins(&R, *ns->hdr, "void EndWrite(lib_ams::FShm &shm, void *msg, int len);");
            EndNsBlock(*ns->hdr, *ns, "");
        }
    }

    Ins(&R, pnew.preamble, "msg = ($Cpptype*)lib_ams::BeginWrite(shm,int(len));");
    Ins(&R, pnew.preamble, "if (!msg) {");
    Ins(&R, pnew.preamble, "    return NULL; // no room.");
    Ins(&R, pnew.preamble, "}");

    Ins(&R, pnew.postamble, "lib_ams::EndWrite(shm,msg,int(len));");
}

// -----------------------------------------------------------------------------

static void PnewAlloc(algo_lib::Replscope &R, amc::Genpnew &pnew) {
    AddProtoArg(*pnew.p_func, "const algo::Alloc &", "alloc");
    Ins(&R, pnew.p_func->comment, "If ALLOC cannot provide the space, construct nothing and return NULL.");
    pnew.req_pack=true;

    Ins(&R, pnew.preamble, "msg = ($Cpptype*)alloc.begin(alloc.ctx, int(len));");
    Ins(&R, pnew.preamble, "if (!msg) {");
    Ins(&R, pnew.preamble, "    return NULL; // no room.");
    Ins(&R, pnew.preamble, "}");

    Ins(&R, pnew.postamble, "if (alloc.end) {");
    Ins(&R, pnew.postamble, "    alloc.end(alloc.ctx, msg, int(len)); // finalize");
    Ins(&R, pnew.postamble, "}");
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
            Ins(&R, func.body, AssignExpr(field, "*msg", LenfldStoreExpr(*lenfld, "len"), true)<<";");
        } else if (field.reftype == dmmeta_Reftype_reftype_Varlen) {
            Ins(&R, func.body, "memcpy($name_Addr(*msg), $name.elems, $name_ary_len);");
            if (ctype_zd_varlenfld_Next(field)) {
                if (ctype_zd_varlenfld_Prev(field)) {
                    Set(R, "$prevendexpr", VarlenEndExpr("(*msg)",*ctype_zd_varlenfld_Prev(field)));
                    Set(R, "$endassign", VarlenEndAssign("(*msg)",field,Subst(R,"$prevendexpr + $name_ary_len")));
                } else {
                    Set(R, "$endassign", VarlenEndAssign("(*msg)",field,Subst(R,"$name_ary_len")));
                }
                Ins(&R, func.body, "$endassign;");
            }
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
    ind_beg(amc::ctype_zd_varlenfld_curs,varlenfld,ctype) {
        Set(R, "$name", name_Get(varlenfld));
        Set(R, "$Vartype", varlenfld.p_arg->c_lenfld ? strptr("u8") : varlenfld.cpp_type);
        Ins(&R, func.body, tempstr() << "u64 $name_ary_len = u64(elems_N($name)) * sizeof($Vartype);");
        Ins(&R, func.body, "len += $name_ary_len;");
    }ind_end;

    if (ctype.c_optfld) {
        Set(R, "$name", name_Get(*ctype.c_optfld));
        if (!ctype.c_optfld->p_arg->c_lenfld) {
            AddProtoArg(func, "int", "opt_len");
        } else {
            Set(R, "$optlen", LengthExpr(*ctype.c_optfld->p_arg, "$name[0]"));
            Ins(&R, func.body, "int opt_len = $name ? $optlen : 0;");
        }
        // The Opt byte count is signed -- passed by the caller, or read off
        // the payload's own length word, which can be corrupt. A negative
        // count underallocates the fixed portion (the header stores already
        // overflow the buffer), and the Opt memcpy (Pnew_CopyFields) converts
        // it to a huge size_t. Fail construction (NULL) before any buffer
        // space is taken (GenAllocFunc guards the pool paths the same way).
        Ins(&R, func.comment, "A negative Opt byte count constructs nothing and returns NULL.");
        Ins(&R, func.body, "if (opt_len < 0) {");
        Ins(&R, func.body, "    return NULL; // a negative count would underallocate the fixed portion");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "len += opt_len;");
    }

    // The runtime total (fixed size + varlen/opt bytes) is stored through
    // the lenfld formula (len + extra) / scale (Pnew_CopyFields); a total
    // that is not a scale multiple has no representable length word (the
    // truncated store would make the reader reconstruct less than was
    // written), and a total beyond the storable range wraps mod 2^N through
    // the word's own type, framing a shorter message than was written.
    // Fail construction (NULL) before any buffer space is taken.
    // A fixed-only total needs no guard: gen_check_lenfld proves it
    // storable at generation time.
    amc::FLenfld *lenfld = ctype.c_lenfld;
    if (amc::RuntimeFrameLenQ(ctype)) {
        if (lenfld && amc::LenfldGuardNeededQ(*lenfld)) {
            Set(R, "$lenchk", LenfldCheckExpr(*lenfld, "len"));
            Ins(&R, func.comment, "A total the length field cannot store constructs nothing and returns NULL.");
            Ins(&R, func.body, "if (!($lenchk)) {");
            Ins(&R, func.body, "    return NULL; // total not representable in the length field");
            Ins(&R, func.body, "}");
        }
        // Every buffer expresses its size as an i32: the Alloc and Shm arms
        // pass int(len) to the allocator, and the reader reconstructs the
        // total as an i32 (LengthExpr). A varlen element count is an i64 and
        // the byte total is a u64, so a caller can arrive here with a total
        // above the frame-length domain -- an aryptr of 2^29 u32 elements is
        // 2GiB of payload. Such a total takes only its low 32 bits of space
        // while Pnew_CopyFields copies the whole count, so the frame is
        // refused (NULL) before any buffer space is taken. This is the
        // domain's enforcement point on the C++ side; the check appears only
        // where nothing already bounds the total below it, a lenfld whose
        // storable range stops short contributing the tighter bound above.
        bool bounded = lenfld && amc::LenfldMaxLen(*lenfld) < amc::FrameLenMax();
        if (!bounded) {
            tempstr framemax;
            algo::u64_PrintHex(amc::FrameLenMax(), framemax, 1, true, false);
            Set(R, "$framemax", framemax);
            Ins(&R, func.comment, "A total beyond the i32 frame length domain constructs nothing and returns NULL.");
            Ins(&R, func.body, "if (len > $framemax) {");
            Ins(&R, func.body, "    return NULL; // a frame length is an i32, and so is every buffer size argument");
            Ins(&R, func.body, "}");
        }
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
    case amc_Pnewtype_Shm         : PnewShm(R, genpnew); break;
    case amc_Pnewtype_Append      : PnewAppend(R, genpnew); break;
    case amc_Pnewtype_Alloc       : PnewAlloc(R, genpnew); break;
    default                       : vrfy(0, "unsupported buftype"); break;
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
    strptr buftype = buftype_Get(pnew);
    Set(R, "$Bufarg", buftype == "Shm" ? strptr("SHM") : buftype == "Alloc" ? strptr("ALLOC") : strptr("BUF"));
    Ins(&R, func.comment, "Construct a new $Cpptype in the space provided by $Bufarg.");
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
