// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/lpool.cpp -- Variable-length free pool
//
// Lpool serves size classes of powers of two, from 2^minlevels up.
// Small classes (records up to a quarter of the blk size) are served out
// of blks: a blk is an ordinary block of the blk-size level, temporarily
// stamped with a header and dedicated to one class; records of that
// class are carved from it linearly, freed records form a per-blk LIFO
// list and are reallocated from it, and a fully drained blk reverts to
// an ordinary block on the blk-size free list, reusable by any class.
// Classes above the largest blk-served class are raw blocks: per-level
// free lists, refilled from the base pool and split downward on demand.
// A refill requests exactly the refill level's size -- a whole number of
// 2MB base granules -- and the base pool returns big blocks
// granule-aligned, so every carved block of blk size and up is
// blk-aligned and FreeMem can locate a record's blk by address mask.
// FreeMem must be called with a size of the same class the memory was
// allocated with: a record's blk is found by masking the record address,
// so a size from a different class reads another class's memory as a blk
// header.

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Count # of allocated elements
static bool HaveCountQ(amc::FField &field) {
    return field.arg != "u8";
}

// -----------------------------------------------------------------------------

void amc::tclass_Lpool() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    // the blk address mask requires the base pool to return big blocks
    // granule-aligned; Sbrk guarantees it, and Lpool itself preserves it
    amc::FField *basepool = GetBasepool(field);
    bool aligned_base = basepool && (basepool->reftype == dmmeta_Reftype_reftype_Sbrk || basepool->reftype == dmmeta_Reftype_reftype_Lpool);
    if (!aligned_base) {
        prerr("amc.lpool_basepool"
              <<Keyval("field",field.field)
              <<Keyval("basepool",(basepool ? strptr(basepool->field) : strptr("(none)")))
              <<Keyval("comment","Lpool requires a granule-aligning base pool (Sbrk or Lpool); add a dmmeta.basepool row"));
        algo_lib::_db.exit_code=1;
    }
    Set(R, "$minlevels", tempstr()<<4);// lowest size levels to skip. i.e. 4 -> min allocsize is 2^4
    Set(R, "$nlevels", tempstr()<<36);// number of levels; 36 with minlevels:4 = (1<<40) = 1TB
    Set(R, "$blkcell", tempstr()<<12);// level whose blocks back the blks (64K)
    Set(R, "$blkcls", tempstr()<<11);// classes below this are blk-served (max blk record size 16K)
    Set(R, "$blksize", tempstr()<<65536);// blk size in bytes, 1<<(blkcell+minlevels)
    Set(R, "$blkmask", tempstr()<<65535);// address mask locating a record's blk
    Set(R, "$blkhdr", tempstr()<<64);// blk header size; first record offset

    Ins(&R, *ns.hdr, "struct $name_Lpblock {");
    Ins(&R, *ns.hdr, "    $name_Lpblock* next;");
    Ins(&R, *ns.hdr, "};");
    Ins(&R, *ns.hdr, "");
    Ins(&R, *ns.hdr, "struct $name_Lpblk { // blk: block dedicated to one size class");
    Ins(&R, *ns.hdr, "    $name_Lpblk*   next;    // next blk of the class with free space");
    Ins(&R, *ns.hdr, "    $name_Lpblk**  pprev;   // back link; NULL = not on the class list");
    Ins(&R, *ns.hdr, "    $name_Lpblock* freerec; // freed records of this blk, LIFO");
    Ins(&R, *ns.hdr, "    u32            rsize;   // record size (class size)");
    Ins(&R, *ns.hdr, "    u32            live;    // records in use");
    Ins(&R, *ns.hdr, "    u32            tip;     // offset of the next never-allocated record");
    Ins(&R, *ns.hdr, "    u32            cell;    // class index of rsize");
    Ins(&R, *ns.hdr, "};");

    // instance
    if (HaveCountQ(field)) {
        InsVar(R, field.p_ctype, "i64", "$name_n", "", "Number of elements");
    }
    InsVar(R, field.p_ctype, "$name_Lpblock*", "$name_free[$nlevels]", "", "Lpool levels");
    InsVar(R, field.p_ctype, "$name_Lpblk*", "$name_blk[$blkcls]", "", "Dedicated blks with free space, per class");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void", "", "");
    AddProtoArg(func, "void*", "mem");
    AddProtoArg(func, "u64", "size");
    Ins(&R, func.comment, "SIZE must be of the same class the memory was allocated with.");
    Ins(&R, func.body, "size = u64_Max(size,1ULL<<$minlevels);");
    Ins(&R, func.body, "u64 cell = algo::u64_BitScanReverse(size-1) + 1 - $minlevels;");
    Ins(&R, func.body, "if (mem && cell < $blkcls) {");
    Ins(&R, func.body, "    // a blk-class record returns to its blk, found by address mask");
    Ins(&R, func.body, "    $name_Lpblk *blk = ($name_Lpblk*)((u64)mem & ~(u64)$blkmask);");
    Ins(&R, func.body, "    $name_Lpblock *rec = ($name_Lpblock*)mem;");
    Ins(&R, func.body, "    rec->next = blk->freerec;");
    Ins(&R, func.body, "    blk->freerec = rec;");
    Ins(&R, func.body, "    blk->live--;");
    Ins(&R, func.body, "    if (blk->pprev == NULL) { // regained space: rejoin the class list");
    Ins(&R, func.body, "        blk->next = $parname.$name_blk[blk->cell];");
    Ins(&R, func.body, "        blk->pprev = &$parname.$name_blk[blk->cell];");
    Ins(&R, func.body, "        if (blk->next) {");
    Ins(&R, func.body, "            blk->next->pprev = &blk->next;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        $parname.$name_blk[blk->cell] = blk;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    // drained blk reverts to an ordinary block on the blk-size level,");
    Ins(&R, func.body, "    // reusable by any class; the last blk of a class is kept dedicated");
    Ins(&R, func.body, "    // so a lone alloc/free cycle does not thrash dedication");
    Ins(&R, func.body, "    bool sole = $parname.$name_blk[blk->cell] == blk && blk->next == NULL;");
    Ins(&R, func.body, "    if (blk->live == 0 && !sole) {");
    Ins(&R, func.body, "        *blk->pprev = blk->next;");
    Ins(&R, func.body, "        if (blk->next) {");
    Ins(&R, func.body, "            blk->next->pprev = blk->pprev;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        $name_Lpblock *raw = ($name_Lpblock*)blk;");
    Ins(&R, func.body, "        raw->next = $parname.$name_free[$blkcell];");
    Ins(&R, func.body, "        $parname.$name_free[$blkcell] = raw;");
    Ins(&R, func.body, "    }");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "    $parname.$name_n--;");
    }
    Ins(&R, func.body, "} else if (mem && cell < $nlevels) {");
    Ins(&R, func.body, "    $name_Lpblock *temp = ($name_Lpblock*)mem; // push  singly linked list");
    Ins(&R, func.body, "    temp->next = $parname.$name_free[cell];");
    Ins(&R, func.body, "    $parname.$name_free[cell] = temp;");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "    $parname.$name_n--;");
    }
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

// Generate the lpool's untyped allocator. Small size classes are served
// from a per-class block (a freed record first, else the bump tip); larger
// sizes take a whole level block. Raw blocks come from the lowest populated
// free level at or above the request, splitting oversized upper halves back
// onto the free lists, and are refilled from the base pool at huge-page
// friendly sizes when every suitable level is empty.
void amc::tfunc_Lpool_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void*","retval","NULL");
    AddProtoArg(func, "u64","size");
    Ins(&R, func.comment, "Allocate new piece of memory at least SIZE bytes long.");
    Ins(&R, func.comment, "If not successful, return NULL");
    Ins(&R, func.comment, "The allocated block is at least 1<<$minlevels");
    Ins(&R, func.comment, "The maximum allocation size is at most 1<<($nlevels+$minlevels)");
    Ins(&R, func.body, "size     = u64_Max(size,1ULL<<$minlevels); // enforce alignment");
    Ins(&R, func.body, "u64 cell = algo::u64_BitScanReverse(size-1) + 1 - $minlevels;");
    Ins(&R, func.body, "$name_Lpblk *blk = cell < $blkcls ? $parname.$name_blk[cell] : NULL;");
    Ins(&R, func.body, "if (cell < $nlevels && blk == NULL) {");
    Ins(&R, func.body, "    // acquire a raw block: for a blk class, a blk-size block to dedicate;");
    Ins(&R, func.body, "    // otherwise the requested level. Serve from the lowest populated");
    Ins(&R, func.body, "    // level at or above it, splitting the upper halves back down.");
    Ins(&R, func.body, "    u64 rawcell = cell < $blkcls ? (u64)$blkcell : cell;");
    Ins(&R, func.body, "    void *rawmem = NULL;");
    Ins(&R, func.body, "    u64 i = rawcell;");
    Ins(&R, func.body, "    for (; i < $nlevels; i++) {");
    Ins(&R, func.body, "        $name_Lpblock *rawblk = $parname.$name_free[i];");
    Ins(&R, func.body, "        if (rawblk) {");
    Ins(&R, func.body, "            $parname.$name_free[i] = rawblk->next;");
    Ins(&R, func.body, "            rawmem = rawblk;");
    Ins(&R, func.body, "            break;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    // if no suitable block, refill from the base allocator with exactly");
    Ins(&R, func.body, "    // the level size, a whole number of base granules; the base pool");
    Ins(&R, func.body, "    // returns big blocks granule-aligned, so every carved block of blk");
    Ins(&R, func.body, "    // size and up is blk-aligned (FreeMem locates a record's blk by");
    Ins(&R, func.body, "    // address mask)");
    Ins(&R, func.body, "    if (UNLIKELY(!rawmem)) {");
    Ins(&R, func.body, "        i = u64_Max(rawcell, 21-$minlevels); // 2MB min -- allow huge page to be used");
    Ins(&R, func.body, "        rawmem = $basepool_AllocMem(1ULL<<(i+$minlevels));");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (LIKELY(rawmem)) {");
    Ins(&R, func.body, "        // if block is more than 2x as large as needed, return the upper half to the free");
    Ins(&R, func.body, "        // list (repeatedly). meanwhile, rawmem doesn't change.");
    Ins(&R, func.body, "        while (i > rawcell) {");
    Ins(&R, func.body, "            i--;");
    Ins(&R, func.body, "            u64 half = 1ULL<<(i+$minlevels);");
    Ins(&R, func.body, "            $name_Lpblock *shed = ($name_Lpblock*)((u8*)rawmem + half);");
    Ins(&R, func.body, "            shed->next = $parname.$name_free[i];");
    Ins(&R, func.body, "            $parname.$name_free[i] = shed;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        if (cell < $blkcls) { // stamp a fresh blk dedicated to this class");
    Ins(&R, func.body, "            blk = ($name_Lpblk*)rawmem;");
    Ins(&R, func.body, "            blk->freerec = NULL;");
    Ins(&R, func.body, "            blk->rsize = 1u<<(cell+$minlevels);");
    Ins(&R, func.body, "            blk->live = 0;");
    Ins(&R, func.body, "            blk->tip = $blkhdr;");
    Ins(&R, func.body, "            blk->cell = (u32)cell;");
    Ins(&R, func.body, "            blk->next = NULL;");
    Ins(&R, func.body, "            blk->pprev = &$parname.$name_blk[cell];");
    Ins(&R, func.body, "            $parname.$name_blk[cell] = blk;");
    Ins(&R, func.body, "        } else {");
    Ins(&R, func.body, "            retval = rawmem;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (blk) { // serve one record: a freed record first, else bump the tip");
    Ins(&R, func.body, "    $name_Lpblock *rec = blk->freerec;");
    Ins(&R, func.body, "    if (rec) {");
    Ins(&R, func.body, "        blk->freerec = rec->next;");
    Ins(&R, func.body, "        retval = rec;");
    Ins(&R, func.body, "    } else {");
    Ins(&R, func.body, "        retval = (u8*)blk + blk->tip;");
    Ins(&R, func.body, "        blk->tip += blk->rsize;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    blk->live++;");
    Ins(&R, func.body, "    if (blk->freerec == NULL && blk->tip + blk->rsize > $blksize) {");
    Ins(&R, func.body, "        // full: leave the class list until a record comes back");
    Ins(&R, func.body, "        *blk->pprev = blk->next;");
    Ins(&R, func.body, "        if (blk->next) {");
    Ins(&R, func.body, "            blk->next->pprev = blk->pprev;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        blk->pprev = NULL;");
    Ins(&R, func.body, "        blk->next = NULL;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "$parname.$name_n += retval != NULL;");
    }
    // the alloc trace counter is owned by the typed alloc layer
    // (Pool.AllocMaybe / Pool.AllocExtraMaybe), which every traced lpool has:
    // do_trace requires a non-u8 arg. Counting here as well would double-count.
}

// -----------------------------------------------------------------------------

// Generate the lpool's pre-reservation function: allocate NBUF buffers of
// BUFSIZE bytes through the allocator, then free them all, leaving the
// free store stocked for later allocation.
void amc::tfunc_Lpool_ReserveBuffers() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "bool", "retval", "true");
    AddProtoArg(func, "u64", "nbuf");
    AddProtoArg(func, "u64", "bufsize");
    Ins(&R, func.comment, "Stock the free store with NBUF buffers of size BUFSIZE:");
    Ins(&R, func.comment, "allocate them all, then free them all, chaining through the buffers");
    Ins(&R, func.body, "$name_Lpblock *head = NULL;");
    Ins(&R, func.body, "for (u64 i = 0; i < nbuf; i++) {");
    Ins(&R, func.body, "    $name_Lpblock *temp = ($name_Lpblock*)$name_AllocMem($pararg, bufsize);");
    Ins(&R, func.body, "    if (temp == NULL) {");
    Ins(&R, func.body, "        retval = false;// an unservable bufsize or an exhausted base pool reserves nothing further");
    Ins(&R, func.body, "        break;");
    Ins(&R, func.body, "    } else {");
    Ins(&R, func.body, "        temp->next = head;");
    Ins(&R, func.body, "        head = temp;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "while (head) {");
    Ins(&R, func.body, "    $name_Lpblock *next = head->next;");
    Ins(&R, func.body, "    $name_FreeMem($pararg, head, bufsize);");
    Ins(&R, func.body, "    head = next;");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_ReallocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc(true);
    AddRetval(func, "void*", "ret", "oldmem");
    AddProtoArg(func, "void*", "oldmem");
    AddProtoArg(func, "u64", "old_size");
    AddProtoArg(func, "u64", "new_size");
    Ins(&R, func.comment, "Allocate new block, copy old to new, delete old.");
    Ins(&R, func.comment, "If the new size is same as old size, do nothing.");
    Ins(&R, func.comment, "In all other cases, new memory is allocated (i.e. size reduction is not a no-op)");
    Ins(&R, func.comment, "If no memory, return NULL; old memory remains untouched");
    Ins(&R, func.body, "if (new_size != old_size) {");
    Ins(&R, func.body, "    ret = $name_AllocMem($pararg, new_size);");
    Ins(&R, func.body, "    if (ret && oldmem) {"); // check for oldmem if required for gcc -O2
    Ins(&R, func.body, "        memcpy(ret,oldmem,u64_Min(new_size,old_size));");
    Ins(&R, func.body, "        $name_FreeMem($pararg, oldmem, old_size);");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    GenTclass(amc_tclass_Pool);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.body, "memset($parname.$name_free, 0, sizeof($parname.$name_free));");
    Ins(&R, func.body, "memset($parname.$name_blk, 0, sizeof($parname.$name_blk));");
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        Ins(&R, func.body, "$parname.$name_n = 0;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Lpool_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    if (HaveCountQ(*amc::_db.genctx.p_field)) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "i64", "", "");
        }
        Ins(&R, func.body, "return $parname.$name_n;");
    }
}
