// Copyright (C) 2023-2024 AlgoRND
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
static bool HasFdQ(amc::FFbuf &fbuf) {
    return fbuf.insready != "" && fbuf.insready != fbuf.field;
}

//
// see tex/amc/amc_fbuf.tex for documentation
//
void amc::tclass_Fbuf() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;

    Set(R, "$dflt"     , field.dflt.value);
    Set(R, "$ns"     , ns.ns);

    (void)ns;
    vrfy(field.c_fbuf, tempstr()<<"fbuf record required for field "<<field.field);
    amc::FFbuf &fbuf = *field.c_fbuf;
    bool inmsgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf && ReadQ(fbuf);
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;

    // msgbuf must have lenfld
    vrfy(!inmsgbuf || field.p_arg->c_lenfld, tempstr()<<"Msgbuf requires a type with lenfld. field: "<<field.field);
    vrfy(!linebuf || ch_N(field.dflt.value) > 0, "Linebuf requires dflt (end of line value)");

    Set(R, "$bufsize", tempstr()<<fbuf.max);
    Set(R, "$Rettype", (inmsgbuf ? "$Cpptype*" : "algo::aryptr<$Cpptype>"));

    // how to force the elems to go to the end of of the struct?
    if (fbuf.max == 0) {
        InsVar(R, field.p_ctype    , "u8*", "$name_elems", "NULL", "pointer to elements of indirect array");
        InsVar(R, field.p_ctype    , "u32", "$name_max", "0", "current length of allocated array");
    } else {
        InsVar(R, field.p_ctype    , "u8", "$name_elems[$bufsize]", "", "pointer to elements of inline array");
    }
    InsVar(R, field.p_ctype    , "i32", "$name_start", "", "beginning of valid bytes (in bytes)");
    InsVar(R, field.p_ctype    , "i32", "$name_end", "", "end of valid bytes (in bytes)");
    InsVar(R, field.p_ctype    , "bool", "$name_eof", "", "no more data will be written to buffer");
    InsVar(R, field.p_ctype    , "algo::Errcode", "$name_err", "", "system error code");
    InsVar(R, field.p_ctype    , "bool", "$name_msgvalid", "", "current message is valid");
    InsVar(R, field.p_ctype    , "i32", "$name_msglen", "", "current message length");
    if (ReadQ(fbuf) && HasFdQ(fbuf)) {
        InsVar(R, field.p_ctype, "algo_lib::FIohook", "$name_iohook", "", "edge-triggered hook for refilling buffer");
    }
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        InsVar(R, field.p_ctype, "algo_lib::FIohook", "$name_iohook", "", "edge-triggered hook for emptying buffer");
        InsVar(R, field.p_ctype, "bool", "$name_zerocopy", "", "support zero-copy optimization");
        InsVar(R, field.p_ctype, "u64", "$name_n_eagain", "", "eagain counter");
    }
    InsVar(R, field.p_ctype    , "bool", "$name_epoll_enable", "", "use epoll?");
    if (fbuf.max > 0) {
        InsStruct(R, field.p_ctype    , "enum { $name_max = $bufsize };");
    }

    Set(R, "$ready", name_Get(*fbuf.p_insready));
    Set(R, "$eof", name_Get(*fbuf.p_inseof));

    amc::FCtype &arg = *field.p_arg;
    if (inmsgbuf) {
        Set(R, "$lenfld", name_Get(*arg.c_lenfld->p_field));
        Set(R, "$lenval", LengthExpr(arg, "(*hdr)"));
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Fbuf_BeginRead() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Attach file descriptor and begin reading using edge-triggered epoll.");
        Ins(&R, func.comment, "File descriptor becomes owned by $Partype.$name via FIohook field.");
        Ins(&R, func.comment, "Whenever the file descriptor becomes readable, insert $parname into $ready.");
        Ins(&R, func.proto, "$name_BeginRead($Parent, algo::Fildes fd)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "callback_Set1($parname.$name_iohook, $parname, $ns::$ready_Insert);");
        Ins(&R, func.body, "$parname.$name_iohook.fildes = fd;");
        Ins(&R, func.body, "IOEvtFlags flags;");
        Ins(&R, func.body, "read_Set(flags, true);");
        Ins(&R, func.body, "if ($pararg.$name_epoll_enable) {");
        Ins(&R, func.body, "    algo_lib::IohookAdd($parname.$name_iohook, flags);");
        Ins(&R, func.body, "} else {");
        Ins(&R, func.body, "    $ns::$ready_Insert($pararg);");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Fbuf_GetMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    bool msgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf;
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;
    bool bytebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Bytebuf;
    bool bytebuf_extern = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_BytebufExtern;

    if (ReadQ(fbuf)) {
        amc::FFunc& getmsg = amc::CreateCurFunc();
        Ins(&R, getmsg.comment, "Look for valid message at current position in the buffer.");
        Ins(&R, getmsg.comment, "If message is already there, return a pointer to it. Do not skip message (call SkipMsg to do that).");
        Ins(&R, getmsg.comment, "If there is no message, read once from underlying file descriptor and try again.");
        if (linebuf) {
            Ins(&R, getmsg.comment, "The message is found by looking for delimiter $dflt.");
            Ins(&R, getmsg.comment, "The return value is an aryptr. If ret.elems is non-NULL, the message is valid (possibly empty).");
            Ins(&R, getmsg.comment, "If ret.elems is NULL, no message can be extracted from buffer.");
            Ins(&R, getmsg.comment, "The returned aryptr excludes the trailing deliminter.");
            Ins(&R, getmsg.comment, "SkipMsg will skip both the line and the deliminter.");
            Ins(&R, getmsg.comment, "A partial line at the end of input is NOT returned (TODO?)");
        } else if (bytebuf) {
            Ins(&R, getmsg.comment, "The message is any number of bytes > 0");
        } else if (bytebuf_extern) {
            Ins(&R, getmsg.comment, "The message boundary is determined by a custom ScanMsg function implemented by user");
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
        if (linebuf || bytebuf || bytebuf_extern) {
            Ins(&R, getmsg.body, "if ($parname.$name_msgvalid) {");
            Ins(&R, getmsg.body, "    ret.elems = hdr;");
            Ins(&R, getmsg.body, "    ret.n_elems = $parname.$name_msglen;");
            Ins(&R, getmsg.body, "}");
        } else if (msgbuf) {
            Ins(&R, getmsg.body, "ret = $parname.$name_msgvalid ? hdr : NULL;");
        }
        if (fbuf.inseof != fbuf.field) {
            Ins(&R, getmsg.body, "if (!$parname.$name_msgvalid && $parname.$name_eof) { // all messages processed");
            Ins(&R, getmsg.body, "    $ns::$eof_Insert($pararg);");
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
    if (fbuf.max == 0) {
        Ins(&R, init.body    , "$parname.$name_elems = NULL; // $name: initialize");
        Ins(&R, init.body    , "$parname.$name_max = 0; // $name: initialize");
    }
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
}

void amc::tfunc_Fbuf_Max() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    amc::FFunc& maxitems = amc::CreateCurFunc();
    Ins(&R, maxitems.ret  , "i32", false);
    Ins(&R, maxitems.proto, "$name_Max($Parent)", false);
    if (fbuf.max == 0) {
        Ins(&R, maxitems.body, "return $pararg.$name_max;");
    } else {
        Ins(&R, maxitems.body, "return $bufsize;");
    }
    MaybeUnused(maxitems,Subst(R,"$pararg"));
}

void amc::tfunc_Fbuf_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& navail = amc::CreateCurFunc();
    Ins(&R, navail.ret  , "i32",false);
    Ins(&R, navail.proto, "$name_N($Parent)",false);
    Ins(&R, navail.body, "return $parname.$name_end - $parname.$name_start;");
}

void amc::tfunc_Fbuf_Refill() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& refill = amc::CreateCurFunc();
        Ins(&R, refill.ret  , "bool",false);
        Ins(&R, refill.proto, "$name_Refill($Parent)",false);
        Ins(&R, refill.body    , "bool readable = ValidQ($parname.$name_iohook.fildes);");
        Ins(&R, refill.body    , "if (readable) {");
        Ins(&R, refill.body    , "    int fd     = $parname.$name_iohook.fildes.value;");
        Ins(&R, refill.body    , "    i32 max    = $name_Max($pararg);");
        Ins(&R, refill.body    , "    i32 end    = $parname.$name_end;");
        Ins(&R, refill.body    , "    i32 nbytes = end - $parname.$name_start; // # bytes currently in buffer");
        Ins(&R, refill.body    , "    i32 nfree  = max - end; // bytes available at the end of buffer");
        Ins(&R, refill.body    , "    if (nbytes == 0 || nfree == 0) { // make more room for reading (or take advantage of free shift)");
        Ins(&R, refill.body    , "        $name_Shift($pararg);");
        Ins(&R, refill.body    , "        end = $parname.$name_end;");
        Ins(&R, refill.body    , "        nfree = max - end;");
        Ins(&R, refill.body    , "    }");
        Ins(&R, refill.body    , "    ssize_t ret         = read(fd, $parname.$name_elems + end, nfree);");
        Ins(&R, refill.body    , "    readable            = !(ret < 0 && errno == EAGAIN);");
        Ins(&R, refill.body    , "    bool error          = ret < 0 && errno != EAGAIN; // detect permanent error on this fd");
        Ins(&R, refill.body    , "    bool eof            = error || (ret == 0 && nfree > 0);");
        Ins(&R, refill.body    , "    $parname.$name_end += i32_Max(ret,0); // new end of bytes");
        Ins(&R, refill.body    , "    if (error) {");
        Ins(&R, refill.body    , "        $parname.$name_err = algo::FromErrno(errno); // fetch errno");
        Ins(&R, refill.body    , "    }");
        Ins(&R, refill.body    , "    $parname.$name_eof |= eof;");
        Ins(&R, refill.body    , "}");
        Ins(&R, refill.body    , "if (!readable && $pararg.$name_epoll_enable) {");
        Ins(&R, refill.body    , "    $ns::$ready_Remove($pararg);");
        Ins(&R, refill.body    , "}");
        Ins(&R, refill.body    , "return readable;");
    }
}

void amc::tfunc_Fbuf_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& removeall = amc::CreateCurFunc();
    Ins(&R, removeall.comment, "Discard contents of the buffer.");
    Ins(&R, removeall.ret      , "void",false);
    Ins(&R, removeall.proto    , "$name_RemoveAll($Parent)",false);
    Ins(&R, removeall.body     , "$parname.$name_start    = 0;");
    Ins(&R, removeall.body     , "$parname.$name_end      = 0;");
    Ins(&R, removeall.body     , "$parname.$name_msgvalid = false;");
}

void amc::tfunc_Fbuf_ScanMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;
    bool msgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf;
    bool linebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Linebuf;
    bool bytebuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Bytebuf;
    bool bytebuf_extern = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_BytebufExtern;

    if (ReadQ(fbuf)) {
        amc::FFunc& scanmsg = amc::CreateCurFunc();
        Ins(&R, scanmsg.comment, "");
        Ins(&R, scanmsg.ret  , "void",false);
        Ins(&R, scanmsg.proto, "$name_ScanMsg($Parent)",false);
        if (bytebuf_extern) {
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
            } else if (bytebuf || bytebuf_extern) {
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
    }
}

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
    Ins(&R, skipmsg.body    , "}");
}

void amc::tfunc_Fbuf_WriteAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    amc::FFunc& writeall = amc::CreateCurFunc();
    writeall.inl = false;
    Ins(&R, writeall.comment, "Write bytes to the buffer. If the entire block is written, return true,");
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
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        Ins(&R, writeall.body, "if ($parname.$name_zerocopy && fits && out_N($pararg)==0) {// in kernel bypass situations this is faster");
        Ins(&R, writeall.body, "    int rc = write($pararg.out_iohook.fildes.value, in, in_n);");
        Ins(&R, writeall.body, "    if (rc >= 0) {");
        Ins(&R, writeall.body, "        in += rc;");
        Ins(&R, writeall.body, "        in_n -= rc;");
        Ins(&R, writeall.body, "    }");
        Ins(&R, writeall.body, "}");
    }
    Ins(&R, writeall.body    , "if (fits && in_n > 0) {");
    Ins(&R, writeall.body    , "    memcpy($parname.$name_elems + end, in, in_n);");
    Ins(&R, writeall.body    , "    $parname.$name_end = end + in_n;");
    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        Ins(&R, writeall.body, "    $ready_Insert($pararg); // schedule outflow");
    }
    Ins(&R, writeall.body    , "}");
    Ins(&R, writeall.body    , "return fits;");
}

void amc::tfunc_Fbuf_WriteMsg() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    bool outmsgbuf = fbuf.fbuftype == dmmeta_Fbuftype_fbuftype_Msgbuf && !ReadQ(fbuf);

    if (outmsgbuf) {
        amc::FCtype &arg = *field.p_arg;
        amc::FFunc& writeall = amc::CreateCurFunc();
        writeall.inl = false;
        Ins(&R, writeall.ret  , "bool",false);
        if (arg.c_lenfld) {
            Set(R, "$writemsg_lenval", amc::LengthExpr(arg, "msg"));
            Ins(&R, writeall.proto, "$name_WriteMsg($Parent, $Cpptype &msg)",false);
            Ins(&R, writeall.body    , "return $name_WriteAll($pararg, (u8*)&msg, $writemsg_lenval);");
        } else {
            Ins(&R, writeall.proto, "$name_WriteMsg($Parent, $Cpptype &msg, u32 len)",false);
            Ins(&R, writeall.body    , "return $name_WriteAll($pararg, (u8*)&msg, len);");
        }
    }
}

void amc::tfunc_Fbuf_BeginWrite() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Attach file descriptor and begin outflowing buffer reading using edge-triggered epoll.");
        Ins(&R, func.comment, "Whenever buffer is non-empty and fd is writable, insert $parname into $ready.");
        Ins(&R, func.comment, "User should implement a step function that calls $name_Outflow.");
        Ins(&R, func.proto, "$name_BeginWrite($Parent, algo::Fildes fd, bool nodelete)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "callback_Set1($parname.$name_iohook, $parname, $ns::$ready_Insert);");
        Ins(&R, func.body, "$parname.$name_iohook.fildes    = fd;");
        Ins(&R, func.body, "if (nodelete) {");
        Ins(&R, func.body, "    $parname.$name_epoll_enable    = false; // cannot register fd twice -- disable epoll on shared fd");
        Ins(&R, func.body, "    $parname.$name_iohook.nodelete = true;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "IOEvtFlags flags;");
        Ins(&R, func.body, "write_Set(flags, true);");
        Ins(&R, func.body, "if ($parname.$name_epoll_enable) {");
        Ins(&R, func.body, "    algo_lib::IohookAdd($parname.$name_iohook, flags);");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Fbuf_Outflow() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field     = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf       = *field.c_fbuf;
    Set(R,"$partrace", Refname(*field.p_ctype));

    if (!ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Once all bytes are written or when fd buffer is full, buffer is automatically removed from $ready list.");
        Ins(&R, func.comment, "Edge-triggered epoll will re-insert $name into $ready.");
        Ins(&R, func.proto, "$name_Outflow($Parent)",false);
        Ins(&R, func.ret  , "bool",false);
        Ins(&R, func.body    , "int  nwrite   = $name_N($pararg);");
        Ins(&R, func.body    , "int  start    = $pararg.$name_start;");
        Ins(&R, func.body    , "int  nwritten = nwrite > 0 ? write($pararg.$name_iohook.fildes.value, $pararg.$name_elems + start, nwrite) : 0;");
        Ins(&R, func.body    , "bool good     = nwritten >= 0 || errno == EAGAIN;");
        if (field.do_trace) {
            Ins(&R, func.body    , "if(errno == EAGAIN) {");
            Ins(&R, func.body    , "    $ns::_db.trace.$partrace_$name_n_eagain++;");
            Ins(&R, func.body    , "    $pararg.$name_n_eagain++;");
            Ins(&R, func.body    , "}");
        }
        Ins(&R, func.body    , "if (!good) {");
        Ins(&R, func.body    , "    $pararg.$name_err = algo::FromErrno(errno); // save error code");
        if (fbuf.inseof != fbuf.field) {
            Ins(&R, func.body, "    $ns::$eof_Insert($pararg); // the end");
        }
        Ins(&R, func.body    , "}");
        Ins(&R, func.body    , "if (nwritten > 0) {");
        Ins(&R, func.body    , "    $name_SkipBytes($pararg,nwritten); // skip written bytes");
        Ins(&R, func.body    , "}");
        Ins(&R, func.body    , "bool done = out_N($pararg)==0;");
        Ins(&R, func.body    , "if ($pararg.$name_epoll_enable) {");
        Ins(&R, func.body    , "    done |= nwritten<0;");
        Ins(&R, func.body    , "}");
        Ins(&R, func.body    , "if (done) {");
        Ins(&R, func.body    , "    $ns::$ready_Remove($pararg); // done writing");
        Ins(&R, func.body    , "}");
        Ins(&R, func.body    , "return nwritten > 0;");
    }
}

void amc::tfunc_Fbuf_EndRead() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (ReadQ(fbuf) && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.proto, "$name_EndRead($Parent)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "if (ValidQ($parname.$name_iohook.fildes)) {");
        Ins(&R, func.body, "    $parname.$name_eof = true;");
        Ins(&R, func.body, "    $ns::$ready_Insert($pararg);");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Fbuf_EndWrite() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (ReadQ(fbuf) == false && HasFdQ(fbuf)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.proto, "$name_EndWrite($Parent)",false);
        Ins(&R, func.ret  , "void",false);
        Ins(&R, func.body, "if (ValidQ($parname.$name_iohook.fildes)) {");
        Ins(&R, func.body, "    // zero-byte write for remote side");
        Ins(&R, func.body, "    ssize_t rc=write($parname.$name_iohook.fildes.value, \"\", 0);");
        Ins(&R, func.body, "    (void)rc;");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Fbuf_Realloc() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (fbuf.max==0) {
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
}

void amc::tfunc_Fbuf_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFbuf &fbuf = *field.c_fbuf;

    if (fbuf.max==0) {
        amc::FFunc& uninit = amc::CreateCurFunc();
        Ins(&R, uninit.body, "if ($parname.$name_elems) {");
        Ins(&R, uninit.body, "    $basepool_FreeMem($parname.$name_elems, sizeof($Cpptype)*$parname.$name_max); // ($field)");
        Ins(&R, uninit.body, "}");
        Ins(&R, uninit.body, "$parname.$name_elems = NULL;");
        Ins(&R, uninit.body, "$parname.$name_max = 0;");
    }
}
