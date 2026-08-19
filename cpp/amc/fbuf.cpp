// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2014-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/fbuf.cpp -- Byte buffer
//

#include "include/amc.h"

static bool ReadQ(amc::FFbuf &fbuf) {
    return fbufdir_Get(fbuf) == dmmeta_Fbufdir_fbufdir_in;
}

static bool BytebufQ(amc::FFbuf &fbuf) {
    return fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Bytebuf;
}

static bool HasFdQ(amc::FFbuf &fbuf) {
    return fbuf.iotype != amc::dmmeta_fbufiotype_nofd;
}

// True when FBUF is a write buffer that shares one descriptor with a read
// buffer.  The read buffer then carries the descriptor and its epoll
// registration, and this buffer has no iohook of its own.
static bool PairedQ(amc::FFbuf &fbuf) {
    return ch_N(fbuf.pair) > 0;
}

// The field whose iohook holds FIELD's descriptor: a paired write buffer
// resolves to its read buffer, any other buffer to itself.
static amc::FField &IohookField(amc::FField &field) {
    amc::FFbuf &fbuf = *field.c_fbuf;
    return PairedQ(fbuf) && fbuf.p_pair ? *fbuf.p_pair : field;
}

// The write buffer's field, when FIELD is the read half of a pair; NULL
// otherwise.  The pairing is declared on the write buffer, so the
// reverse direction is read off the ctype's own field list rather than from a
// back-reference: an empty pair is the common case, and an xref keyed on it
// would collide across every fbuf that leaves it unset.
static amc::FField *PairWritefld(amc::FField &field) {
    amc::FField *ret = NULL;
    ind_beg(amc::ctype_c_field_curs,other,*field.p_ctype) {
        if (other.c_fbuf && PairedQ(*other.c_fbuf) && other.c_fbuf->p_pair == &field) {
            ret = &other;
        }
    }ind_end;
    return ret;
}

// Bind $iohook to the member holding this buffer's descriptor, so a body that
// reads or writes the fd names one thing whether the buffer owns its
// registration or borrows its pair's.
static void SetIohook(amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    Set(R, "$iohook", tempstr()<<name_Get(IohookField(field))<<"_iohook");
}

// Test whether CTYPE has an operator == that generated code can call.
// A builtin type carries the native operator; a ccmp with genop:Y has one
// emitted from the ctype's fields; a ccmp with extrn:Y declares that the
// comparison is written by hand outside amc. A ctype with none of the three has
// no ==, and code comparing two of its values does not compile. A ccmp that is
// neither extrn nor genop is not enough: order:Y alone emits the Cmp, Lt and Eq
// functions and no operator. These are the three sources the minmax check reads
// for operator <.
static bool EqOpQ(amc::FCtype &ctype) {
    return ctype.c_bltin!=NULL || (ctype.c_ccmp!=NULL && (ctype.c_ccmp->extrn || ctype.c_ccmp->genop));
}

// -----------------------------------------------------------------------------

// Check the element type of every Bytebuf and Linebuf: one byte wide, and for a
// Linebuf comparable with ==.
// A Linebuf or Bytebuf measures its message in bytes, and its element type is
// the unit the message is handed back in. A Bytebuf of u32 keeps a message of
// 12 bytes as msglen 12 and returns an aryptr<u32> of 12 elements, which is
// 48 bytes -- 36 of them past the data. A Linebuf of u32 scans hdr[msglen]
// while stepping msglen by sizeof(u32), so the fourth comparison already reads
// at byte 48 of a 16-byte line. Only a one-byte element makes the two units
// the same, so the byte-oriented buffer types take one. A Msgbuf carries its
// length in the message header and is free to have any element type.
// Runs after gen_compute_size: only a bltin or extern ctype carries a declared
// csize, and a generated ctype's width comes from the emitted struct body, so
// asking in gen:ns_tclass_field would reject a one-byte generated element as
// too wide. An element whose width is not known in this run cannot be shown to
// be one byte either, and is rejected under its own reason.
// The second requirement is on comparability, and falls on the Linebuf alone: a
// Linebuf finds the end of its message by comparing each element with the
// delimiter, so its ScanMsg reads `hdr[msglen] == <dflt>`. A one-byte generated
// struct passes the width test and has no operator == unless its ccmp asks for
// one, and the run then exits 0 with a generated namespace the C++ compiler
// rejects. The comparison is emitted only for the reading direction, and only
// when the scanner is amc's own: an ffunc ScanMsg with extrn:Y hands the scan to
// the user, who compares the elements however the type allows.
void amc::gen_check_fbuf() {
    ind_beg(amc::_db_field_curs, field, amc::_db) if (field.c_fbuf) {
        amc::FFbuf &fbuf = *field.c_fbuf;
        bool bytelen = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Bytebuf
            || fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;
        i64 nbyte = amc::Ctype_Nbit(*field.p_arg) / 8;
        amc::FFfunc *scanff = amc::FindFfunc(field, "ScanMsg");
        bool eqscan = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf
            && ReadQ(fbuf)
            && (scanff == NULL || !scanff->extrn);
        tempstr reason;
        if (bytelen && nbyte != 1) {
            reason << (nbyte == 0
                       ? "the element type's width is not known in this run, so it cannot be shown to be one byte"
                       : "Bytebuf/Linebuf count length in bytes; the element type must be one byte wide");
        } else if (eqscan && !EqOpQ(*field.p_arg)) {
            reason << "Linebuf scans for the delimiter with ==; the element type needs an equality operator:"
                   << " a builtin type, or a ccmp with genop:Y or extrn:Y";
        }
        if (ch_N(reason) > 0) {
            prerr("amc.fbuf_elem"
                  <<Keyval("fbuf",fbuf.field)
                  <<Keyval("fbuftype",fbuf.fbuftype)
                  <<Keyval("arg",field.arg)
                  <<Keyval("nbyte",nbyte)
                  <<Keyval("comment",reason));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// Generate the buffer's members and the functions its direction and buffer
// type call for; txt/exe/amc/fbuf.md documents the reftype.
void amc::tclass_Fbuf() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    (void)ns;
    vrfy(field.c_fbuf, tempstr()<<"fbuf record required for field "<<field.field);
    amc::FFbuf &fbuf = *field.c_fbuf;

    Set(R, "$dflt", field.dflt.value);
    Set(R, "$ns", ns.ns);

    amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);
    amc::FFcond *space = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_space);

    // space reports that a congested buffer has drained, which is the
    // producer's wake -- and only an out buffer has a producer to wake.  On an
    // in buffer the app is the consumer, and its wake is ready.
    if (space && ReadQ(fbuf)) {
        prerr("amc.fbuf_space_in"
              <<Keyval("fbuf",fbuf.field)
              <<Keyval("comment","space is the producer-side wake of an out buffer; an in buffer has no producer"));
        algo_lib::_db.exit_code++;
    }

    // the ready condition doubles as the file descriptor's readiness callback,
    // which the kernel hands the buffer's parent row and nothing else
    if (ready && HasFdQ(fbuf) && amc::FcondNocallbackQ(*ready)) {
        prerr("amc.fbuf_ready_via"
              <<Keyval("fbuf",fbuf.field)
              <<Keyval("via",ready->via)
              <<Keyval("comment","ready is installed as the fd readiness callback, which passes the parent row; via has no way through"));
        algo_lib::_db.exit_code++;
    }

    // A pair is declared on the write buffer and names the read buffer it
    // shares a descriptor with, so exactly one of the two carries the epoll
    // registration.  Every way of getting that backwards yields a buffer whose
    // descriptor is never armed, and an unarmed buffer shows up only as a
    // connection that stalls -- so the shape is checked here rather than found
    // in production.
    if (PairedQ(fbuf)) {
        amc::FFbuf *pair = fbuf.p_pair ? fbuf.p_pair->c_fbuf : NULL;
        tempstr err;
        if (!fbuf.p_pair) {
            err << "pair names no known field";
        } else if (ReadQ(fbuf)) {
            err << "pair belongs on the write buffer; the read buffer is the one it names";
        } else if (!pair) {
            err << "pair names a field that is not an fbuf";
        } else if (fbuf.p_pair->p_ctype != field.p_ctype) {
            err << "pair names a field of another ctype; the two buffers share one record";
        } else if (!ReadQ(*pair)) {
            err << "pair must name a read buffer";
        } else if (!HasFdQ(fbuf) || !HasFdQ(*pair)) {
            err << "both buffers of a pair need a descriptor (iotype other than nofd)";
        } else if (fbuf.iotype != pair->iotype) {
            err << "both buffers of a pair must have the same iotype";
        }
        if (ch_N(err)) {
            prerr("amc.fbuf_pair"
                  <<Keyval("fbuf",fbuf.field)
                  <<Keyval("pair",fbuf.pair)
                  <<Keyval("comment",err));
            algo_lib::_db.exit_code++;
        }
    }

    bool inmsgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf && ReadQ(fbuf);
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;

    // msgbuf must have lenfld
    vrfy(!inmsgbuf || field.p_arg->c_lenfld, tempstr()<<"Msgbuf requires a type with lenfld. field: "<<field.field);
    vrfy(!linebuf || ch_N(field.dflt.value) > 0, "Linebuf requires dflt (end of line value)");

    // the element type's width is checked in gen_check_fbuf, which runs after
    // struct sizes are computed

    Set(R, "$Rettype", (inmsgbuf ? "$Cpptype*" : "algo::aryptr<$Cpptype>"));

    // how to force the elems to go to the end of of the struct?
    InsVar(R, field.p_ctype    , "u8*", "$name_elems", "NULL", "pointer to elements of indirect array");
    InsVar(R, field.p_ctype    , "u32", "$name_max", "0", "current length of allocated array");
    InsVar(R, field.p_ctype    , "i32", "$name_start", "", "beginning of valid bytes (in bytes)");
    InsVar(R, field.p_ctype    , "i32", "$name_end", "", "end of valid bytes (in bytes)");
    InsVar(R, field.p_ctype    , "i32", "$name_msglen", "", "current message length");
    InsVar(R, field.p_ctype    , "algo::Errcode", "$name_err", "", "system error code");
    if (HasFdQ(fbuf) && field.c_fbuf->iotype == dmmeta_fbufiotype_openssl) {
        InsVar(R, field.p_ctype, "SSL*", "$name_ssl", "", "TLS connection");
    }
    // A paired write buffer declares no iohook: the descriptor and its single
    // registration live on the read buffer it names.
    if (HasFdQ(fbuf) && !PairedQ(fbuf)) {
        InsVar(R, field.p_ctype, "algo_lib::FIohook", "$name_iohook", "", "edge-triggered hook for the buffer");
    }
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        InsVar(R, field.p_ctype, "u64", "$name_n_eagain", "", "eagain counter");
    }
    if (field.do_trace) {
        if (ReadQ(fbuf)) {
            InsVar(R, field.p_ctype, "u64", "$name_n_read_byte", "", "read bytes");
            InsVar(R, field.p_ctype, "u64", "$name_n_read_msg", "", "read messages");
        } else {
            InsVar(R, field.p_ctype, "u64", "$name_n_write_byte", "", "written bytes");
            InsVar(R, field.p_ctype, "u64", "$name_n_write_msg", "", "written messages");
        }
    }
    InsVar(R, field.p_ctype    , "bool", "$name_eof", "", "no more data will be written to buffer");
    InsVar(R, field.p_ctype    , "bool", "$name_msgvalid", "", "current message is valid");
    InsVar(R, field.p_ctype    , "bool", "$name_epoll_enable", "", "use epoll?");
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        InsVar(R, field.p_ctype, "bool", "$name_zerocopy", "", "support zero-copy optimization");
    }
    if (space) {
        InsVar(R, field.p_ctype, "bool", "$name_congested", "", "buffer reached the high-water mark; the drain past the low-water mark wakes the producer");
    }

    if (ready) {
        Set(R, "$ready", name_Get(*ready->p_ins));
    }

    amc::FCtype &arg = *field.p_arg;
    if (inmsgbuf) {
        Set(R, "$lenfld", name_Get(*arg.c_lenfld->p_field));
        Set(R, "$lenval", LengthExpr(arg, "(*hdr)"));
    }
}

// -----------------------------------------------------------------------------

// Generate $name_BeginRead: attach a file descriptor to the read buffer and arm its edge-triggered epoll readiness hook.
void amc::tfunc_Fbuf_BeginRead() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);

    // A read buffer named by a pair is attached with $name_BeginReadWrite,
    // which arms the one registration for both directions; a read-only attach
    // on the same descriptor would leave the write side dark.
    if (ReadQ(fbuf) && HasFdQ(fbuf) && !PairWritefld(field)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Attach file descriptor and begin reading using edge-triggered epoll.");
        Ins(&R, func.comment, "File descriptor becomes owned by $Partype.$name via FIohook field.");
        if (ready) {
            Ins(&R, func.comment, "Whenever the file descriptor becomes readable, insert $parname into $ready.");
        }
        Ins(&R, func.proto, "$name_BeginRead($Parent, algo::Fildes fd)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "$parname.$iohook.fildes = fd;");
        if (ready) {
            Set(R, "$condfunc", amc::FcondInsFunc(*ready));
            Ins(&R, func.body, "callback_Set1($parname.$iohook, $parname, $condfunc);");
            Ins(&R, func.body, "IOEvtFlags flags;");
            Ins(&R, func.body, "read_Set(flags, true);");
            Ins(&R, func.body, "if ($parname.$name_epoll_enable) {");
            Ins(&R, func.body, "    algo_lib::IohookAdd($parname.$iohook, flags);");
            Ins(&R, func.body, "} else {");
            Ins(&R, func.body, "    $condfunc($pararg);");
            Ins(&R, func.body, "}");
        }
    }
}

void amc::tfunc_Fbuf_GetMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    bool msgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf;
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;
    bool bytebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Bytebuf;
    bool custom_scan = amc::FindFfunc(field, "ScanMsg") != NULL;

    if (ReadQ(fbuf)) {
        amc::FFunc& getmsg = amc::CreateCurFunc();
        Ins(&R, getmsg.comment, "Look for valid message at current position in the buffer.");
        Ins(&R, getmsg.comment, "If message is already there, return a pointer to it. Do not skip message (call SkipMsg to do that).");
        Ins(&R, getmsg.comment, "If there is no message, read once from underlying file descriptor and try again.");
        if (linebuf) {
            Ins(&R, getmsg.comment, "The message is found by looking for delimiter $dflt.");
            Ins(&R, getmsg.comment, "The return value is an aryptr. If ret.elems is non-NULL, the message is valid (possibly empty).");
            Ins(&R, getmsg.comment, "If ret.elems is NULL, no message can be extracted from buffer.");
            Ins(&R, getmsg.comment, "The returned aryptr excludes the trailing delimiter.");
            Ins(&R, getmsg.comment, "SkipMsg will skip both the line and the delimiter.");
            Ins(&R, getmsg.comment, "A partial line at the end of input is NOT returned.");
        } else if (bytebuf) {
            if (custom_scan) {
                Ins(&R, getmsg.comment, "The message boundary is determined by a custom ScanMsg function implemented by user");
            } else {
                Ins(&R, getmsg.comment, "The message is any number of bytes > 0");
            }
        } else if (msgbuf) {
            Ins(&R, getmsg.comment, "The message is length-delimited based on field $lenfld field");
        }
        Ins(&R, getmsg.comment, "");
        Ins(&R, getmsg.ret  , "$Rettype",false);
        Ins(&R, getmsg.proto, "$name_GetMsg($Parent)",false);
        Ins(&R, getmsg.body,    "$Rettype ret;");
        Ins(&R, getmsg.body,    "if (!$parname.$name_msgvalid) {");
        Ins(&R, getmsg.body,    "    $name_ScanMsg($pararg);");
        if (HasFdQ(fbuf)) {
            Ins(&R, getmsg.body, "    if (!$parname.$name_msgvalid) {");
            Ins(&R, getmsg.body, "        bool readable = $name_Refill($pararg);");
            Ins(&R, getmsg.body, "        if (readable) {");
            Ins(&R, getmsg.body, "            $name_ScanMsg($pararg);");
            Ins(&R, getmsg.body, "        }");
            Ins(&R, getmsg.body, "    }");
        }
        Ins(&R, getmsg.body,     "}");
        Ins(&R, getmsg.body,     "$Cpptype *hdr = ($Cpptype*)($parname.$name_elems + $parname.$name_start);");
        if (linebuf || bytebuf) {
            Ins(&R, getmsg.body, "if ($parname.$name_msgvalid) {");
            Ins(&R, getmsg.body, "    ret.elems = hdr;");
            Ins(&R, getmsg.body, "    ret.n_elems = $parname.$name_msglen;");
            Ins(&R, getmsg.body, "}");
        } else if (msgbuf) {
            Ins(&R, getmsg.body, "ret = $parname.$name_msgvalid ? hdr : NULL;");
        }
        if (amc::FFcond *eof = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_eof)) {
            Ins(&R, getmsg.body, "if (!$parname.$name_msgvalid && $parname.$name_eof) { // all messages processed");
            amc::AddFcondOp(getmsg.body, *eof, "Insert");
            Ins(&R, getmsg.body, "}");
        }
        Ins(&R, getmsg.body, "return ret;");
    }
}

void amc::tfunc_Fbuf_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    Ins(&R, init.body    , "$parname.$name_elems = NULL; // $name: initialize");
    Ins(&R, init.body    , "$parname.$name_max = 0; // $name: initialize");
    Ins(&R, init.body    , "$parname.$name_end = 0; // $name: initialize");
    Ins(&R, init.body    , "$parname.$name_start = 0; // $name: initialize");
    Ins(&R, init.body    , "$parname.$name_eof = false; // $name: initialize");
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        Ins(&R, init.body, "$parname.$name_zerocopy = false; // $name: initialize");
        Ins(&R, init.body, "$parname.$name_n_eagain = 0; // $name: initialize");
    }
    Ins(&R, init.body, "$parname.$name_msgvalid = false; // $name: initialize");
    Ins(&R, init.body, "$parname.$name_msglen = 0; // $name: initialize");
    Ins(&R, init.body, "$parname.$name_epoll_enable = true; // $name: initialize");
    if (HasFdQ(fbuf) && field.c_fbuf->iotype == dmmeta_fbufiotype_openssl) {
        Ins(&R, init.body, "$parname.$name_ssl = NULL; // $name: initialize");
    }
    // initialize buffer
    Set(R,"$fbufmax",tempstr()<<fbuf.max);
    Ins(&R, init.body, "$name_Realloc($pararg, $fbufmax);");

    if (field.do_trace) {
        if (ReadQ(fbuf)) {
            Ins(&R, init.body, "$parname.$name_n_read_byte = 0; // $name: initialize");
            Ins(&R, init.body, "$parname.$name_n_read_msg = 0; // $name: initialize");
        } else {
            Ins(&R, init.body, "$parname.$name_n_write_byte = 0; // $name: initialize");
            Ins(&R, init.body, "$parname.$name_n_write_msg = 0; // $name: initialize");
        }
    }
}

void amc::tfunc_Fbuf_Max() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& maxitems = amc::CreateCurFunc();
    Ins(&R, maxitems.ret  , "i32", false);
    Ins(&R, maxitems.proto, "$name_Max($Parent)", false);
    Ins(&R, maxitems.body, "return $parname.$name_max;");
}

void amc::tfunc_Fbuf_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& navail = amc::CreateCurFunc();
    Ins(&R, navail.ret  , "i32",false);
    Ins(&R, navail.proto, "$name_N($Parent)",false);
    Ins(&R, navail.body, "return $parname.$name_end - $parname.$name_start;");
}

// Generate $name_Refill: read available bytes from the file descriptor into the read buffer.
void amc::tfunc_Fbuf_Refill() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);
    if (ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& refill = amc::CreateCurFunc();
        Ins(&R, refill.ret  , "bool",false);
        Ins(&R, refill.proto, "$name_Refill($Parent)",false);
        Ins(&R, refill.body    , "bool readable = ValidQ($parname.$iohook.fildes);");
        Ins(&R, refill.body    , "if (readable) {");
        Ins(&R, refill.body    , "    int fd     = $parname.$iohook.fildes.value;");
        Ins(&R, refill.body    , "    i32 max    = $name_Max($pararg);");
        Ins(&R, refill.body    , "    i32 end    = $parname.$name_end;");
        Ins(&R, refill.body    , "    i32 nbytes = end - $parname.$name_start; // # bytes currently in buffer");
        Ins(&R, refill.body    , "    i32 nfree  = max - end; // bytes available at the end of buffer");
        Ins(&R, refill.body    , "    if (nbytes == 0 || nfree == 0) { // make more room for reading (or take advantage of free shift)");
        Ins(&R, refill.body    , "        $name_Shift($pararg);");
        Ins(&R, refill.body    , "        end = $parname.$name_end;");
        Ins(&R, refill.body    , "        nfree = max - end;");
        Ins(&R, refill.body    , "    }");
        if (fbuf.iotype == dmmeta_fbufiotype_openssl) {
            Ins(&R, refill.body, "    if ($parname.$name_ssl) {");
            Ins(&R, refill.body, "        int ret = SSL_read($parname.$name_ssl,$parname.$name_elems + end, nfree);");
            Ins(&R, refill.body, "        int err = SSL_get_error($parname.$name_ssl,ret);");
            Ins(&R, refill.body, "        bool fdretry = err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE;");
            Ins(&R, refill.body, "        bool sslretry = fdretry || err == SSL_ERROR_WANT_CONNECT || err == SSL_ERROR_WANT_ACCEPT || err == SSL_ERROR_WANT_X509_LOOKUP;");
            Ins(&R, refill.body, "        bool zero = err == SSL_ERROR_ZERO_RETURN;");
            Ins(&R, refill.body, "        readable = !fdretry;");
            Ins(&R, refill.body, "        bool error = err && !sslretry;");
            Ins(&R, refill.body, "        bool eof = error || zero;");
            Ins(&R, refill.body, "        $parname.$name_end += i32_Max(ret,0); // new end of bytes");
            Ins(&R, refill.body, "        if (error) {");
            Ins(&R, refill.body, "            $parname.$name_err = algo::MakeErrcode(algo_Errns_ssl, err);");
            Ins(&R, refill.body, "        }");
            Ins(&R, refill.body, "        $parname.$name_eof |= eof;");
            Ins(&R, refill.body, "    } else {");
        } {
            Ins(&R, refill.body, "        ssize_t ret         = read(fd, $parname.$name_elems + end, nfree);");
            Ins(&R, refill.body, "        readable            = !(ret < 0 && errno == EAGAIN);");
            Ins(&R, refill.body, "        bool error          = ret < 0 && errno != EAGAIN; // detect permanent error on this fd");
            Ins(&R, refill.body, "        bool eof            = error || (ret == 0 && nfree > 0);");
            Ins(&R, refill.body, "        $parname.$name_end += i32_Max(ret,0); // new end of bytes");
            Ins(&R, refill.body, "        if (error) {");
            Ins(&R, refill.body, "            $parname.$name_err = algo::FromErrno(errno); // fetch errno");
            Ins(&R, refill.body, "        }");
            Ins(&R, refill.body, "        $parname.$name_eof |= eof;");
        }
        if (fbuf.iotype == dmmeta_fbufiotype_openssl) {
            Ins(&R, refill.body, "    }"); // ssl else
        }
        Ins(&R, refill.body    , "}");
        amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);
        if (ready && ready->rem) {
            Ins(&R, refill.body    , "if (!readable && $parname.$name_epoll_enable) {");
            amc::AddFcondOp(refill.body, *ready, "Remove");
            Ins(&R, refill.body    , "}");
        }
        Ins(&R, refill.body    , "return readable;");
    }
}

void amc::tfunc_Fbuf_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& removeall = amc::CreateCurFunc();
    Ins(&R, removeall.comment, "Discard contents of the buffer.");
    Ins(&R, removeall.ret      , "void",false);
    Ins(&R, removeall.proto    , "$name_RemoveAll($Parent)",false);
    Ins(&R, removeall.body     , "$parname.$name_start    = 0;");
    Ins(&R, removeall.body     , "$parname.$name_end      = 0;");
    Ins(&R, removeall.body     , "$parname.$name_msgvalid = false;");
    if (amc::FFcond *space = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_space)) {
        Ins(&R, removeall.body , "// the buffer is now empty, so a congested producer can proceed");
        Ins(&R, removeall.body , "if ($parname.$name_congested) {");
        Ins(&R, removeall.body , "    $parname.$name_congested = false;");
        amc::AddFcondOp(removeall.body, *space, "Insert");
        Ins(&R, removeall.body , "}");
    }
}

void amc::tfunc_Fbuf_ScanMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    bool msgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf;
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;
    bool bytebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Bytebuf;

    if (ReadQ(fbuf)) {
        amc::FFunc& scanmsg = amc::CreateCurFunc();
        Ins(&R, scanmsg.comment, "");
        Ins(&R, scanmsg.ret  , "void",false);
        Ins(&R, scanmsg.proto, "$name_ScanMsg($Parent)",false);
        // ffunc:<field>/ScanMsg with extrn:Y means the user supplies the scanner;
        // amc emits only the prototype instead of a default byte/line/msg scanner.
        amc::FFfunc *scanff = amc::FindFfunc(field, "ScanMsg", true);
        if (scanff && scanff->extrn) {
            scanmsg.acrkey << "ffunc:"<<field.field<<".ScanMsg";
            scanmsg.extrn=true;
        }
        if (!scanmsg.extrn) {
            scanmsg.priv = true;
            if (linebuf || msgbuf) {
                Ins(&R, scanmsg.body, "$Cpptype *hdr = ($Cpptype*)($parname.$name_elems + $parname.$name_start);");
            }
            Ins(&R, scanmsg.body    , "i32 avail = $name_N($pararg);");
            Ins(&R, scanmsg.body    , "i32 msglen;");
            Ins(&R, scanmsg.body    , "bool found = false;");
            if (linebuf) {
                Ins(&R, scanmsg.body, "// scan for delimiter starting from the previous place where we left off.");
                Ins(&R, scanmsg.body, "// at the end, save offset back to $parname so we don't have to re-scan.");
                Ins(&R, scanmsg.body, "// returned message length **does not include delimiter**.");
                Ins(&R, scanmsg.body, "// a line that exceeds buffer length is not returned.");
                Ins(&R, scanmsg.body, "for (msglen = $parname.$name_msglen; msglen < avail; msglen += sizeof($Cpptype)) {");
                Ins(&R, scanmsg.body, "    if (hdr[msglen] == $dflt) { // delimiter?");
                Ins(&R, scanmsg.body, "        found = true;");
                Ins(&R, scanmsg.body, "        break;");
                Ins(&R, scanmsg.body, "    }");
                Ins(&R, scanmsg.body, "}");
                Ins(&R, scanmsg.body, "if (!found && msglen >= $name_Max($pararg)) {");
                Ins(&R, scanmsg.body, "    $parname.$name_eof = true; // cause user to detect eof");
                Ins(&R, scanmsg.body, "    $parname.$name_err = algo::FromErrno(E2BIG); // argument list too big -- closest error code");
                Ins(&R, scanmsg.body, "}");
            } else if (bytebuf) {
                Ins(&R, scanmsg.body, "found = avail>0;");// all remaining bytes
                Ins(&R, scanmsg.body, "msglen = avail;");
            } else if (msgbuf) {// msgbuf
                Ins(&R, scanmsg.body, "msglen = ssizeof($Cpptype);");// this is the minimum readable message
                Ins(&R, scanmsg.body, "if (avail >= msglen) {");
                Ins(&R, scanmsg.body, "    msglen = $lenval; // check rest of the message");
                Ins(&R, scanmsg.body, "}");
                Ins(&R, scanmsg.body, "found = msglen >= ssizeof($Cpptype) && avail >= msglen;");
                Ins(&R, scanmsg.body, "if (msglen < ssizeof($Cpptype) || msglen > $name_Max($pararg)) {");
                Ins(&R, scanmsg.body, "    $parname.$name_eof = true; // cause user to detect eof");
                Ins(&R, scanmsg.body, "    $parname.$name_err = algo::FromErrno(E2BIG); // argument list too big -- closest error code");
                Ins(&R, scanmsg.body, "}");
            }
            Ins(&R, scanmsg.body    , "$parname.$name_msglen = msglen;");
            Ins(&R, scanmsg.body    , "$parname.$name_msgvalid = found;");
            MaybeUnused(scanmsg, Subst(R,"$parname"));
        }
    }
}

void amc::tfunc_Fbuf_Shift() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& shift = amc::CreateCurFunc();
    shift.priv = true;
    shift.inl = false;
    Ins(&R, shift.comment, "Shift existing bytes over to the beginning of the buffer");
    Ins(&R, shift.ret  , "void",false);
    Ins(&R, shift.proto, "$name_Shift($Parent)",false);
    Ins(&R, shift.body, "i32 start = $parname.$name_start;");
    Ins(&R, shift.body, "i32 bytes_n = $parname.$name_end - start;");
    Ins(&R, shift.body, "if (bytes_n > 0) {");
    Ins(&R, shift.body, "    memmove($parname.$name_elems, $parname.$name_elems + start, bytes_n);");
    Ins(&R, shift.body, "}");
    Ins(&R, shift.body, "$parname.$name_end = bytes_n;");
    Ins(&R, shift.body, "$parname.$name_start = 0;");
}

// Generate $name_SkipBytes: mark a number of buffer bytes as consumed, advancing the read cursor.
void amc::tfunc_Fbuf_SkipBytes() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    if (!ReadQ(fbuf) || fbuf.p_fbuftype->skipbytes) {
        amc::FFunc& skipbytes = amc::CreateCurFunc();
        Ins(&R, skipbytes.comment, "Mark some buffer contents as read.");
        Ins(&R, skipbytes.comment, "");
        Ins(&R, skipbytes.ret      , "void",false);
        Ins(&R, skipbytes.proto    , "$name_SkipBytes($Parent, int n)",false);
        Ins(&R, skipbytes.body     , "int avail = $parname.$name_end - $parname.$name_start;");
        Ins(&R, skipbytes.body     , "n = i32_Min(n,avail);");
        Ins(&R, skipbytes.body     , "$parname.$name_start += n;");
        Ins(&R, skipbytes.body     , "$parname.$name_msgvalid = false;");
        if (field.do_trace && ReadQ(fbuf)) {
            Ins(&R, skipbytes.body, "    if (n) {");
            Ins(&R, skipbytes.body, "        $ns::_db.trace.$partrace_$name_n_read_byte+=n;");
            Ins(&R, skipbytes.body, "        $parname.$name_n_read_byte+=n;");
            Ins(&R, skipbytes.body, "        $ns::_db.trace.$partrace_$name_n_read_msg++;");
            Ins(&R, skipbytes.body, "        $parname.$name_n_read_msg++;");
            Ins(&R, skipbytes.body, "    }");
        }
        if (amc::FFcond *space = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_space)) {
            Ins(&R, skipbytes.body, "// A congested buffer drained to the low-water mark can take a full");
            Ins(&R, skipbytes.body, "// message again, so the producer parked on the space condition is woken");
            Ins(&R, skipbytes.body, "// exactly once per congestion episode: this is the only place bytes");
            Ins(&R, skipbytes.body, "// leave an out buffer, so it is the only place the room can appear.");
            Ins(&R, skipbytes.body, "if ($parname.$name_congested && $name_N($pararg) * 4 <= $name_Max($pararg)) {");
            Ins(&R, skipbytes.body, "    $parname.$name_congested = false;");
            amc::AddFcondOp(skipbytes.body, *space, "Insert");
            Ins(&R, skipbytes.body, "}");
        }
    }
}

// Generate $name_SkipMsg: advance the read cursor past the current message.
void amc::tfunc_Fbuf_SkipMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;

    amc::FFunc& skipmsg = amc::CreateCurFunc();
    skipmsg.inl = false;
    Ins(&R, skipmsg.comment, "Skip current message, if any.");
    Ins(&R, skipmsg.ret  , "void",false);
    Ins(&R, skipmsg.proto, "$name_SkipMsg($Parent)",false);
    Ins(&R, skipmsg.body    , "if ($parname.$name_msgvalid) {");
    Ins(&R, skipmsg.body    , "    int skip = $parname.$name_msglen;");
    if (linebuf) {
        Ins(&R, skipmsg.body, "    skip += ssizeof($Cpptype); // delimiter");
    }
    Ins(&R, skipmsg.body    , "    i32 start = $parname.$name_start;");
    Ins(&R, skipmsg.body    , "    start += skip;");
    Ins(&R, skipmsg.body    , "    $parname.$name_start = start;");
    Ins(&R, skipmsg.body    , "    $parname.$name_msgvalid = false;");
    Ins(&R, skipmsg.body    , "    $parname.$name_msglen   = 0; // reset message length -- important for delimited streams");
    if (field.do_trace && ReadQ(fbuf)) {
        Ins(&R, skipmsg.body, "    $ns::_db.trace.$partrace_$name_n_read_byte+=skip;");
        Ins(&R, skipmsg.body, "    $parname.$name_n_read_byte+=skip;");
        Ins(&R, skipmsg.body, "    $ns::_db.trace.$partrace_$name_n_read_msg++;");
        Ins(&R, skipmsg.body, "    $parname.$name_n_read_msg++;");
    }
    Ins(&R, skipmsg.body    , "}");
}

void amc::tfunc_Fbuf_WriteReserve() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.comment, "Write bytes to the buffer. The entire block is always written or the program exits.");
    Ins(&R, func.ret  , "void",false);
    Ins(&R, func.proto, "$name_WriteReserve($Parent, u8 *in, i32 in_n)",false);
    Ins(&R, func.body , "if ($parname.$name_end - $parname.$name_start + in_n > $name_Max($pararg)) {");
    Ins(&R, func.body , "    $name_Realloc($pararg, $parname.$name_max + i32_Max($parname.$name_max, in_n));");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "if (!$name_WriteAll($pararg, in, in_n)) {");
    Ins(&R, func.body , "    FatalErrorExit(\"$name: out of memory\");");
    Ins(&R, func.body , "}");
}

// Generate $name_WriteAll: append a block of bytes to the write buffer, shifting or flushing to the fd to make room.
void amc::tfunc_Fbuf_WriteAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    amc::FFunc& writeall = amc::CreateCurFunc();
    Ins(&R, writeall.comment, "Write bytes to the buffer. If the entire block is accepted, return true,");
    Ins(&R, writeall.comment, "Otherwise return false.");
    Ins(&R, writeall.comment, "Bytes in the buffer are potentially shifted left to make room for the message.");
    Ins(&R, writeall.comment, "");
    Ins(&R, writeall.ret  , "bool",false);
    Ins(&R, writeall.proto, "$name_WriteAll($Parent, u8 *in, i32 in_n)",false);
    Ins(&R, writeall.body    , "int max = $name_Max($pararg);");
    Ins(&R, writeall.body    , "// check if message doesn't fit. if so, shift bytes over.");
    Ins(&R, writeall.body    , "if ($parname.$name_end + in_n > max) {");
    Ins(&R, writeall.body    , "    $name_Shift($pararg);");
    Ins(&R, writeall.body    , "}");
    Ins(&R, writeall.body    , "// now try to write the message.");
    Ins(&R, writeall.body    , "i32 end = $parname.$name_end;");
    Ins(&R, writeall.body    , "bool fits = end + in_n <= max;");
    Ins(&R, writeall.body, "    if (fits) {");
    if (field.do_trace && !ReadQ(fbuf)) {
        Ins(&R, writeall.body, "    $ns::_db.trace.$partrace_$name_n_write_byte+=in_n;");
        Ins(&R, writeall.body, "    $ns::_db.trace.$partrace_$name_n_write_msg++;");
        Ins(&R, writeall.body, "    $parname.$name_n_write_byte+=in_n;");
        Ins(&R, writeall.body, "    $parname.$name_n_write_msg++;");
    }
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        Ins(&R, writeall.body, "    if ($parname.$name_zerocopy && $name_N($pararg)==0) {// in kernel bypass situations this is faster");
        if (fbuf.iotype == dmmeta_fbufiotype_openssl) {
            Ins(&R, writeall.body, "    int rc = $parname.$name_ssl ? SSL_write($parname.$name_ssl, in, in_n)");
            Ins(&R, writeall.body, "         : write($parname.$iohook.fildes.value, in, in_n);");
        } else {
            Ins(&R, writeall.body, "    int rc = write($parname.$iohook.fildes.value, in, in_n);");
        }
        Ins(&R, writeall.body, "        if (rc >= 0) {");
        Ins(&R, writeall.body, "            in += rc;");
        Ins(&R, writeall.body, "            in_n -= rc;");
        Ins(&R, writeall.body, "        }");
        Ins(&R, writeall.body, "    }");
    }
    Ins(&R, writeall.body    , "    if (in_n > 0) {");
    Ins(&R, writeall.body    , "        memcpy($parname.$name_elems + end, in, in_n);");
    Ins(&R, writeall.body    , "        $parname.$name_end = end + in_n;");
    amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);
    if (ReadQ(fbuf) == false && HasFdQ(fbuf) && ready) {
        Ins(&R, writeall.body, "        // schedule outflow");
        amc::AddFcondOp(writeall.body, *ready, "Insert");
    }
    Ins(&R, writeall.body    , "    }");
    Ins(&R, writeall.body, "    }");
    if (amc::FindFcond(field, amc::amcdb_tcond_Fbuf_space)) {
        Ins(&R, writeall.body, "// At the high-water mark the buffer is full enough that a producer must");
        Ins(&R, writeall.body, "// expect refusals, and an actual refusal proves it whatever the occupancy");
        Ins(&R, writeall.body, "// -- a message larger than a quarter of the buffer can be refused while");
        Ins(&R, writeall.body, "// the buffer still sits below the mark.  Latching on either fact is what");
        Ins(&R, writeall.body, "// turns the later drain past the low-water mark into a single wake");
        Ins(&R, writeall.body, "// ($name_SkipBytes), and it guarantees that a producer whose message can");
        Ins(&R, writeall.body, "// ever fit is woken again; the gap between the marks is what keeps a");
        Ins(&R, writeall.body, "// producer writing at the drain rate from being woken on every message.");
        Ins(&R, writeall.body, "// A message longer than the whole buffer can never fit, so it does not");
        Ins(&R, writeall.body, "// latch -- that caller must resize ($name_WriteReserve).");
        Ins(&R, writeall.body, "int cong_max = $name_Max($pararg);");
        Ins(&R, writeall.body, "bool cong_hi = cong_max > 0 && $name_N($pararg) * 4 >= cong_max * 3;");
        Ins(&R, writeall.body, "bool cong_refused = !fits && in_n <= cong_max;");
        Ins(&R, writeall.body, "if (cong_hi || cong_refused) {");
        Ins(&R, writeall.body, "    $parname.$name_congested = true;");
        Ins(&R, writeall.body, "}");
    }
    Ins(&R, writeall.body    , "return fits;");
}

void amc::tfunc_Fbuf_WriteMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    bool outmsgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf && !ReadQ(fbuf);

    if (outmsgbuf) {
        amc::FCtype &arg = *field.p_arg;
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl = false;
        Ins(&R, func.ret  , "bool",false);
        if (arg.c_lenfld) {
            Set(R, "$writemsg_lenval", amc::LengthExpr(arg, "msg"));
            Ins(&R, func.proto, "$name_WriteMsg($Parent, $Cpptype &msg)",false);
            Ins(&R, func.body    , "return $name_WriteAll($pararg, (u8*)&msg, $writemsg_lenval);");
        } else {
            Ins(&R, func.proto, "$name_WriteMsg($Parent, $Cpptype &msg, u32 len)",false);
            Ins(&R, func.body    , "return $name_WriteAll($pararg, (u8*)&msg, len);");
        }
    }
}

// Generate $name_BeginWrite: attach a file descriptor to the write buffer and arm its edge-triggered outflow hook.
void amc::tfunc_Fbuf_BeginWrite() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);

    // A paired write buffer is attached by its read buffer's
    // $name_BeginReadWrite; it owns no descriptor to attach on its own.
    if (ReadQ(fbuf) == false && HasFdQ(fbuf) && !PairedQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Attach file descriptor and begin outflowing buffer reading using edge-triggered epoll.");
        if (ready) {
            Ins(&R, func.comment, "Whenever buffer is non-empty and fd is writable, insert $parname into $ready.");
        }
        Ins(&R, func.comment, "User should implement a step function that calls $name_Outflow.");
        Ins(&R, func.proto, "$name_BeginWrite($Parent, algo::Fildes fd, bool nodelete)",false);
        Ins(&R, func.ret  , "void",false);
        if (ready) {
            Set(R, "$condfunc", amc::FcondInsFunc(*ready));
            Ins(&R, func.body, "callback_Set1($parname.$iohook, $parname, $condfunc);");
        }
        Ins(&R, func.body, "if (nodelete) {");
        Ins(&R, func.body, "    $parname.$name_epoll_enable    = false; // cannot register fd twice -- disable epoll on shared fd");
        Ins(&R, func.body, "    $parname.$iohook.nodelete = true;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "$parname.$iohook.fildes    = fd;");
        if (ready) {
            Ins(&R, func.body, "IOEvtFlags flags;");
            Ins(&R, func.body, "write_Set(flags, true);");
            Ins(&R, func.body, "if ($parname.$name_epoll_enable) {");
            Ins(&R, func.body, "    algo_lib::IohookAdd($parname.$iohook, flags);");
            Ins(&R, func.body, "}");
        }
    }
}

// Generate $name_PairReady: turn one descriptor's readiness into the ready condition of each buffer of the pair.
void amc::tfunc_Fbuf_PairReady() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    amc::FField *writefldp = PairWritefld(field);
    if (ReadQ(fbuf) && HasFdQ(fbuf) && writefldp) {
        amc::FField &writefld = *writefldp;
        amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);
        amc::FFcond *writeready = amc::FindFcond(writefld, amc::amcdb_tcond_Fbuf_ready);
        amc::FFcond *eof = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_eof);
        Set(R, "$pairname", name_Get(writefld));
        amc::FFunc& func = amc::CreateCurFunc();
        func.priv = true;
        Ins(&R, func.comment, "Readiness callback for the descriptor $name and $pairname share.");
        Ins(&R, func.comment, "The kernel reports which directions are ready; each one arms its own buffer.");
        Ins(&R, func.proto, "$name_PairReady($Parent)",false);
        Ins(&R, func.ret  , "void",false);
        if (ready) {
            Ins(&R, func.body, "if (read_Get($parname.$iohook.flags)) {");
            amc::AddFcondOp(func.body, *ready, "Insert");
            Ins(&R, func.body, "}");
        }
        if (writeready) {
            Ins(&R, func.body, "if (write_Get($parname.$iohook.flags)) {");
            amc::AddFcondOp(func.body, *writeready, "Insert");
            Ins(&R, func.body, "}");
        }
        if (eof) {
            Ins(&R, func.body, "// A descriptor error or hangup reaches neither read nor write, so the");
            Ins(&R, func.body, "// connection would sit unarmed with nothing left to wake it; end it here.");
            Ins(&R, func.body, "if (eof_Get($parname.$iohook.flags) || err_Get($parname.$iohook.flags)) {");
            Ins(&R, func.body, "    $parname.$name_eof = true;");
            amc::AddFcondOp(func.body, *eof, "Insert");
            Ins(&R, func.body, "}");
        }
    }
}

// Generate $name_BeginReadWrite: attach one descriptor to a read/write buffer pair under a single epoll registration.
void amc::tfunc_Fbuf_BeginReadWrite() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    amc::FField *writefld = PairWritefld(field);
    if (ReadQ(fbuf) && HasFdQ(fbuf) && writefld) {
        Set(R, "$pairname", name_Get(*writefld));
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Attach FD to the $name / $pairname buffer pair and arm it for both directions.");
        Ins(&R, func.comment, "An epoll registration is keyed by descriptor, so the pair carries exactly one:");
        Ins(&R, func.comment, "it subscribes to read and write together, and $name_PairReady fans each");
        Ins(&R, func.comment, "reported direction out to the buffer that owns it.  No dup of FD is needed,");
        Ins(&R, func.comment, "which is what lets the pair work under a kernel-bypass stack.");
        Ins(&R, func.comment, "File descriptor becomes owned by $Partype.$name via FIohook field.");
        Ins(&R, func.proto, "$name_BeginReadWrite($Parent, algo::Fildes fd)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "$parname.$iohook.fildes = fd;");
        Ins(&R, func.body, "callback_Set1($parname.$iohook, $parname, $name_PairReady);");
        Ins(&R, func.body, "IOEvtFlags flags;");
        Ins(&R, func.body, "read_Set(flags, true);");
        Ins(&R, func.body, "write_Set(flags, true);");
        Ins(&R, func.body, "algo_lib::IohookAdd($parname.$iohook, flags);");
    }
}

// Generate $name_Outflow: write buffered bytes to the file descriptor, retiring the readiness hook once drained.
void amc::tfunc_Fbuf_Outflow() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field     = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf       = *field.c_fbuf;
    SetIohook(field);
    Set(R,"$partrace", Refname(*field.p_ctype));

    amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);
    amc::FFcond *eof = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_eof);

    if (!ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        if (ready && ready->rem) {
            Ins(&R, func.comment, "Once all bytes are written or when fd buffer is full, buffer is automatically removed from $ready list.");
            Ins(&R, func.comment, "Edge-triggered epoll will re-insert $name into $ready.");
        }
        Ins(&R, func.proto, "$name_Outflow($Parent)",false);
        Ins(&R, func.ret  , "bool",false);
        Ins(&R, func.body    , "int  nwrite   = $name_N($pararg);");
        Ins(&R, func.body    , "int  start    = $parname.$name_start;");
        Ins(&R, func.body    , "int  nwritten;");
        if (fbuf.iotype == dmmeta_fbufiotype_openssl) {
            Ins(&R, func.body, "if ($parname.$name_ssl) {");
            // A writable fd schedules the buffer whether or not anything is in
            // it, so Outflow is reached with nwrite==0 on every fresh TLS
            // connection. OpenSSL documents SSL_write with num=0 as an error,
            // and its return of 0 as a failed write, never as a short one, so
            // progress is a strictly positive return and a zero has to be
            // handled by the sslretry test like any other failure. Without both
            // halves an empty buffer reported an error it never provoked, and a
            // connection returning 0 forever counted every attempt a success
            // and stayed scheduled forever.
            Ins(&R, func.body, "    if (nwrite > 0) {");
            Ins(&R, func.body, "        nwritten = SSL_write($parname.$name_ssl, $parname.$name_elems + start, nwrite);");
            Ins(&R, func.body, "        int err = SSL_get_error($parname.$name_ssl,nwritten);");
            Ins(&R, func.body, "        bool fdretry = err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE;");
            Ins(&R, func.body, "        bool sslretry = fdretry || err == SSL_ERROR_WANT_CONNECT || err == SSL_ERROR_WANT_ACCEPT || err == SSL_ERROR_WANT_X509_LOOKUP;");
            Ins(&R, func.body, "        bool good = nwritten > 0 || sslretry;");
            Ins(&R, func.body, "        // retryable: clamp to 0 (keep polling); hard error: keep negative");
            if (ready && ready->rem) {
                Ins(&R, func.body, "        // (with $name_epoll_enable, the exit below then unschedules the buffer; a nodelete writer must handle $name_err itself)");
            }
            Ins(&R, func.body, "        nwritten = good ? i32_Max(nwritten,0) : -1;");
            if (field.do_trace) {
                Ins(&R, func.body, "        if (fdretry) {");
                Ins(&R, func.body, "            $ns::_db.trace.$partrace_$name_n_eagain++;");
                Ins(&R, func.body, "            $parname.$name_n_eagain++;");
                Ins(&R, func.body, "        }");
            }
            Ins(&R, func.body, "        if (!good) {");
            Ins(&R, func.body, "            $parname.$name_err = algo::MakeErrcode(algo_Errns_ssl, err); // save error code");
            if (eof) {
                Ins(&R, func.body, "            // the end");
                amc::AddFcondOp(func.body, *eof, "Insert");
            }
            Ins(&R, func.body, "        }");
            Ins(&R, func.body, "    } else {");
            Ins(&R, func.body, "        nwritten = 0; // nothing to write: SSL_write rejects a zero byte count");
            Ins(&R, func.body, "    }");
            Ins(&R, func.body, "} else {");
        } {
            Ins(&R, func.body    , "nwritten = nwrite > 0 ? write($parname.$iohook.fildes.value, $parname.$name_elems + start, nwrite) : 0;");
            Ins(&R, func.body    , "bool good     = nwritten >= 0 || errno == EAGAIN;");
            if (field.do_trace) {
                Ins(&R, func.body, "if(nwritten < 0 && errno == EAGAIN) {");
                Ins(&R, func.body, "    $ns::_db.trace.$partrace_$name_n_eagain++;");
                Ins(&R, func.body, "    $parname.$name_n_eagain++;");
                Ins(&R, func.body, "}");
            }
            Ins(&R, func.body    , "if (!good) {");
            Ins(&R, func.body    , "    $parname.$name_err = algo::FromErrno(errno); // save error code");
            if (eof) {
                Ins(&R, func.body, "    // the end");
                amc::AddFcondOp(func.body, *eof, "Insert");
            }
            Ins(&R, func.body    , "}");
        }
        if (fbuf.iotype == dmmeta_fbufiotype_openssl) {
            Ins(&R, func.body, "}");
        }
        Ins(&R, func.body    , "if (nwritten > 0) {");
        Ins(&R, func.body    , "    $name_SkipBytes($pararg,nwritten); // skip written bytes");
        Ins(&R, func.body    , "}");
        if (ready && ready->rem) {
            Ins(&R, func.body    , "bool done = $name_N($pararg)==0;");
            Ins(&R, func.body    , "if ($parname.$name_epoll_enable) {");
            Ins(&R, func.body    , "    done |= nwritten<0;");
            Ins(&R, func.body    , "}");
            Ins(&R, func.body    , "if (done) {");
            Ins(&R, func.body    , "    // done writing");
            amc::AddFcondOp(func.body, *ready, "Remove");
            Ins(&R, func.body    , "}");
        }
        Ins(&R, func.body    , "return nwritten > 0;");
    }
}

void amc::tfunc_Fbuf_EndRead() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    if (ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.proto, "$name_EndRead($Parent)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "if (ValidQ($parname.$iohook.fildes)) {");
        Ins(&R, func.body, "    $parname.$name_eof = true;");
        if (amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready)) {
            amc::AddFcondOp(func.body, *ready, "Insert");
        }
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Fbuf_EndWrite() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    SetIohook(field);

    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.proto, "$name_EndWrite($Parent)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "if (ValidQ($parname.$iohook.fildes)) {");
        Ins(&R, func.body, "    // zero-byte write for remote side");
        Ins(&R, func.body, "    ssize_t rc=write($parname.$iohook.fildes.value, \"\", 0);");
        Ins(&R, func.body, "    (void)rc;");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Fbuf_Realloc() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.comment, "Unconditionally reallocate buffer to have size NEW_MAX");
    Ins(&R, func.comment, "If the buffer has data in it, NEW_MAX is adjusted so that the data is not lost");
    Ins(&R, func.comment, "(best to call this before filling the buffer)");
    Ins(&R, func.proto, "$name_Realloc($Parent, int new_max)",false);
    Ins(&R, func.ret  , "void",false);
    Ins(&R, func.body, "new_max = i32_Max(new_max, $parname.$name_end);");
    Ins(&R, func.body, "u8 *new_mem = $parname.$name_elems");
    Ins(&R, func.body, "            ? (u8*)$basepool_ReallocMem($parname.$name_elems, $parname.$name_max, new_max)");
    Ins(&R, func.body, "            : (u8*)$basepool_AllocMem(new_max);");
    Ins(&R, func.body, "if (UNLIKELY(!new_mem)) {");
    Ins(&R, func.body, "    FatalErrorExit(\"$ns.fbuf_nomem  field:$field  comment:'out of memory'\");");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "$parname.$name_elems = new_mem;");
    Ins(&R, func.body, "$parname.$name_max = new_max;");
}

// Generate the fbuf's contribution to the parent's Uninit: return the
// buffer's memory to the pool it was taken from and leave the field in the
// state a fresh Init produces, so a record that is reinitialized in place
// neither frees the same block twice nor reuses a freed one.
void amc::tfunc_Fbuf_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& uninit = amc::CreateCurFunc();
    Ins(&R, uninit.body, "if ($parname.$name_elems) {");
    Ins(&R, uninit.body, "    $basepool_FreeMem($parname.$name_elems, $parname.$name_max); // ($field) $name_max is the byte size Realloc allocated");
    Ins(&R, uninit.body, "}");
    Ins(&R, uninit.body, "$parname.$name_elems = NULL;");
    Ins(&R, uninit.body, "$parname.$name_max = 0;");
}

void amc::tfunc_Fbuf_GetAlloc() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    if (BytebufQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        AddRetval(func, "algo::Alloc", "ret", "");
        Ins(&R, func.proto, "$name_GetAlloc($Parent)", false);
        Ins(&R, func.body, "ret.ctx = &$parname;");
        Ins(&R, func.body, "void *(*begin)($Partype&,i32) = $name_BeginAlloc;");
        Ins(&R, func.body, "ret.begin = algo::BeginAllocFcn(begin);");
        Ins(&R, func.body, "ret.end = NULL;");
    }
}

void amc::tfunc_Fbuf_GetAllocReserve() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    if (BytebufQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        func.inl=true;
        AddRetval(func, "algo::Alloc", "ret", "");
        Ins(&R, func.proto, "$name_GetAllocReserve($Parent)", false);
        Ins(&R, func.body, "ret.ctx = &$parname;");
        Ins(&R, func.body, "void *(*begin)($Partype&,i32) = $name_BeginAllocReserve;");
        Ins(&R, func.body, "ret.begin = algo::BeginAllocFcn(begin);");
        Ins(&R, func.body, "ret.end = NULL;");
    }
}

// Generate $name_BeginAlloc: reserve a contiguous block of bytes in the byte buffer and return a write pointer, or NULL when it does not fit.
void amc::tfunc_Fbuf_BeginAlloc() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    if (BytebufQ(fbuf)) {
        Set(R,"$partrace", Refname(*field.p_ctype));
        amc::FFunc& func = amc::CreateCurFunc();
        AddRetval(func, "void*", "ret", "NULL");
        Ins(&R, func.comment, "// Return pointer to a block of IN_N contiguous bytes in the buffer.");
        Ins(&R, func.comment, "// No reallocation is performed. If not possible, return NULL.");
        Ins(&R, func.proto, "$name_BeginAlloc($Partype &$parname, i32 in_n)", false);
        Ins(&R, func.body    , "int max = $name_Max($pararg);");
        Ins(&R, func.body    , "if ($parname.$name_end + in_n > max) {");
        Ins(&R, func.body    , "    $name_Shift($pararg);");
        Ins(&R, func.body    , "}");
        Ins(&R, func.body    , "if ($parname.$name_end + in_n <= max) {");
        Ins(&R, func.body    , "    ret = (u8*)$parname.$name_elems + $parname.$name_end;");
        Ins(&R, func.body    , "    $parname.$name_end += in_n;");
        if (field.do_trace && !ReadQ(fbuf)) {
            // FmtAlloc-style sends go through here, not _WriteAll, so the
            // write counters must be bumped here too (one message per alloc).
            Ins(&R, func.body, "    $ns::_db.trace.$partrace_$name_n_write_byte+=in_n;");
            Ins(&R, func.body, "    $ns::_db.trace.$partrace_$name_n_write_msg++;");
            Ins(&R, func.body, "    $parname.$name_n_write_byte+=in_n;");
            Ins(&R, func.body, "    $parname.$name_n_write_msg++;");
        }
        amc::FFcond *ready = amc::FindFcond(field, amc::amcdb_tcond_Fbuf_ready);
        if (ReadQ(fbuf) == false && HasFdQ(fbuf) && ready) {
            Ins(&R, func.body, "    // schedule outflow");
            amc::AddFcondOp(func.body, *ready, "Insert");
        }
        Ins(&R, func.body    , "}");
    }
}

void amc::tfunc_Fbuf_BeginAllocReserve() {
    algo_lib::Replscope &R          = amc::_db.genctx.R;
    amc::FField         &field      = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    if (BytebufQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        AddRetval(func, "void*", "", "");
        Ins(&R, func.comment, "// Return pointer to a block of IN_N contiguous bytes in the buffer.");
        Ins(&R, func.comment, "// Buffer is reallocated as necessary; function always succeeds.");
        Ins(&R, func.proto, "$name_BeginAllocReserve($Partype &$parname, i32 in_n)", false);
        Ins(&R, func.body    , "if ($parname.$name_end - $parname.$name_start + in_n > $name_Max($pararg)) {");
        Ins(&R, func.body    , "    $name_Realloc($pararg, $parname.$name_max + i32_Max($parname.$name_max, in_n));");
        Ins(&R, func.body    , "}");
        Ins(&R, func.body    , "return $name_BeginAlloc($parname, in_n);");
    }
}
