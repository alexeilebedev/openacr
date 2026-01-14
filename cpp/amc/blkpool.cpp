// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/blkpool.cpp -- Block pool
//

#include "include/amc.h"


void amc::tclass_Blkpool() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;

    Ins(&R, *ns.hdr, "struct $name_Bpbuf {");
    Ins(&R, *ns.hdr, "    $name_Bpbuf *next; // next free buffer");
    Ins(&R, *ns.hdr, "    u32 refcount; // number of memory blocks in this buffer");
    Ins(&R, *ns.hdr, "    u32 size; // total size of this buffer");
    Ins(&R, *ns.hdr, "};");
    Ins(&R, *ns.hdr, "");

    GenTclass(amc_tclass_Pool);

    // pointer to pool from which element was allocated.
    InsVar(R, field.p_arg, "$name_Bpbuf*", "$name_buf", "", "Enclosing Bpbuf");

    InsStruct(R, field.p_ctype, "// blkpool $field");
    InsVar(R, field.p_ctype, "$name_Bpbuf*", "$name_free", "", "list of free bufs");
    InsVar(R, field.p_ctype, "$name_Bpbuf*", "$name_cur", "", "current buffer");
    InsVar(R, field.p_ctype, "u32", "$name_pos", "", "position inside curbuf");
    InsVar(R, field.p_ctype, "u32", "$name_buf_dflt_size", "", "size of next buffer");
}

void amc::tfunc_Blkpool_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& allocmem = amc::CreateCurFunc();
    Ins(&R, allocmem.comment, "Allocate n bytes. If no memory available, return NULL.");
    Ins(&R, allocmem.comment, "If requested block size exceeds Blkpool buffer size, or out-of-memory, return NULL");
    Ins(&R, allocmem.ret  , "$Cpptype*", false);
    Ins(&R, allocmem.proto, "$name_AllocMem($Parent, size_t size)", false);
    Ins(&R, allocmem.body, "size = (size + 15) & ~15; // 16-byte alignment");
    Ins(&R, allocmem.body, "$Cpptype *ret = NULL;");
    Ins(&R, allocmem.body, "$name_Bpbuf *curbuf  = $parname.$name_cur;");
    Ins(&R, allocmem.body, "u32       pos  = $parname.$name_pos;");
    Ins(&R, allocmem.body, "bool      fits = curbuf && pos + size <= curbuf->size;// check if the bytes fit current buffer");
    Ins(&R, allocmem.body, "if (UNLIKELY(!fits)) {");
    Ins(&R, allocmem.body, "    curbuf = $parname.$name_free;");
    Ins(&R, allocmem.body, "    // check if the free buffer exists and is reasonbly large");
    Ins(&R, allocmem.body, "    // (next empty buffer may be too small, due to user's miscalculation)");
    Ins(&R, allocmem.body, "    if (!curbuf || size + sizeof($name_Bpbuf) > curbuf->size) {");
    Ins(&R, allocmem.body, "        u32 rsrv_size = u32_Max(size + sizeof($name_Bpbuf), $parname.$name_buf_dflt_size);");
    Ins(&R, allocmem.body, "        $name_ReserveBuffers($pararg, 1, rsrv_size);");
    Ins(&R, allocmem.body, "        curbuf = $parname.$name_free;");
    Ins(&R, allocmem.body, "    }");
    Ins(&R, allocmem.body, "    if (curbuf) {");
    Ins(&R, allocmem.body, "        $parname.$name_free = curbuf->next;");
    Ins(&R, allocmem.body, "        curbuf->next = NULL;");
    Ins(&R, allocmem.body, "        pos = ssizeof($name_Bpbuf);");
    Ins(&R, allocmem.body, "        fits = pos + size <= curbuf->size;");
    Ins(&R, allocmem.body, "    }");
    Ins(&R, allocmem.body, "    $parname.$name_cur = curbuf;");
    Ins(&R, allocmem.body, "}");
    Ins(&R, allocmem.body, "// allocate requested number of bytes");
    Ins(&R, allocmem.body, "// increment buf refcount.");
    Ins(&R, allocmem.body, "if (fits) {");
    Ins(&R, allocmem.body, "    ret                  = ($Cpptype*)((u8*)curbuf + pos);");
    Ins(&R, allocmem.body, "    // set back pointer.");
    Ins(&R, allocmem.body, "    // IMPORTANT: subsequent ctor must not overwrite $name_buf.");
    Ins(&R, allocmem.body, "    ret->$name_buf       = curbuf;");
    Ins(&R, allocmem.body, "    $parname.$name_pos   = pos + size;");
    Ins(&R, allocmem.body, "    curbuf->refcount    += 1;");
    Ins(&R, allocmem.body, "}");
    Ins(&R, allocmem.body, "return ret;");
}

void amc::tfunc_Blkpool_SetBufferSize() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& setbufsize = amc::CreateCurFunc();
    Ins(&R, setbufsize.ret  , "void", false);
    Ins(&R, setbufsize.comment, "Set size of all future buffers to NBYTES each.");
    Ins(&R, setbufsize.proto, "$name_SetBufferSize($Parent, u32 nbytes)", false);
    Ins(&R, setbufsize.body, "$parname.$name_buf_dflt_size = nbytes;");
}

void amc::tfunc_Blkpool_ReserveBuffers() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& reserve = amc::CreateCurFunc();
    Ins(&R, reserve.ret  , "bool", false);
    Ins(&R, reserve.proto, "$name_ReserveBuffers($Parent, u32 nbuf, u32 size)", false);
    Ins(&R, reserve.body, "bool retval = true;");
    Ins(&R, reserve.body, "for (u32 i = 0; i < nbuf; i++) {");
    Ins(&R, reserve.body, "    $name_Bpbuf *buf = ($name_Bpbuf*)$basepool_AllocMem(size);");
    Ins(&R, reserve.body, "    if (buf) {");
    Ins(&R, reserve.body, "        buf->refcount = 0;");
    Ins(&R, reserve.body, "        buf->size = size;");
    Ins(&R, reserve.body, "        buf->next = $parname.$name_free;");
    Ins(&R, reserve.body, "        $parname.$name_free = buf;");
    Ins(&R, reserve.body, "    } else {");
    Ins(&R, reserve.body, "        retval = false;");
    Ins(&R, reserve.body, "        break;");
    Ins(&R, reserve.body, "    }");
    Ins(&R, reserve.body, "}");
    Ins(&R, reserve.body, "return retval;");
}

void amc::tfunc_Blkpool_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& freemem = amc::CreateCurFunc();
    Ins(&R, freemem.ret  , "void", false);
    Ins(&R, freemem.proto, "$name_FreeMem($Parent, $Cpptype *row, size_t size)", false);
    Ins(&R, freemem.body, "(void)size;");
    Ins(&R, freemem.body, "$name_Bpbuf *buf = row->$name_buf;");
    Ins(&R, freemem.body, "row->$name_buf = NULL;");
    Ins(&R, freemem.body, "u32 rc = buf->refcount;");
    Ins(&R, freemem.body, "if (UNLIKELY(rc == 0)) {");
    Ins(&R, freemem.body, "    FatalErrorExit(\"$ns.blkpool_double_free  pool:$field  comment:'access to freed buffer'\");");
    Ins(&R, freemem.body, "}");
    Ins(&R, freemem.body, "buf->refcount = rc-1;");
    Ins(&R, freemem.body, "if (rc == 1) {");
    Ins(&R, freemem.body, "    if (buf == $parname.$name_cur) {");
    Ins(&R, freemem.body, "        // free rewind");
    Ins(&R, freemem.body, "        $parname.$name_pos = ssizeof($name_Bpbuf);");
    Ins(&R, freemem.body, "    } else {");
    Ins(&R, freemem.body, "        buf->next = $parname.$name_free;");
    Ins(&R, freemem.body, "        $parname.$name_free = buf;");
    Ins(&R, freemem.body, "    }");
    Ins(&R, freemem.body, "}");
}

void amc::tfunc_Blkpool_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    Ins(&R, init.body, "$parname.$name_buf_dflt_size   = 4096;");
    Ins(&R, init.body, "$parname.$name_pos             = 0; // past the end");
    Ins(&R, init.body, "$parname.$name_free = NULL;");
    Ins(&R, init.body, "$parname.$name_cur = NULL;");
}
