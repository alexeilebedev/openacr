// Copyright (C) 2026 AlgoRND
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
// Exceptions: yes
// Source: cpp/amc/blkhash.cpp
//
// A Blkhash is a unique hash index for keys with a dense integer component
// (dmmeta.blkhash.linfld).  The low linbits bits of that component select a
// slot directly inside a block of 2^linbits entry pointers; the rest of the
// key, with those bits masked off, identifies the block itself and is hashed
// to find it in a small bucket array.  A matched block plus a slot determines
// the full key exactly, so the index needs no per-entry chain and adds no
// intrusive fields to the row.  Blocks are fixed-size and come from the
// basepool, so a block freed when its last slot clears is recycled for the
// next block of the same index with no fragmentation.

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Return the member access path from a key value to the integer linear
// component named by blkhash.linfld, e.g. ".seq.value" for a (stream,seq) key.
// Descends single-field wrapper ctypes (algo.SeqType) until a builtin is hit.
static tempstr LinSuffix(amc::FBlkhash &blkhash) {
    tempstr ret;
    amc::FField *fld = blkhash.p_linfld;
    ret << "." << name_Get(*fld);
    amc::FCtype *ctype = fld->p_arg;
    while (ctype->c_bltin == NULL && c_datafld_N(*ctype) == 1) {
        fld = c_datafld_Find(*ctype, 0);
        ret << "." << name_Get(*fld);
        ctype = fld->p_arg;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the builtin ctype at the end of the linfld wrapper chain,
// or NULL if the chain does not terminate in a builtin.
static amc::FCtype *LinTerminal(amc::FBlkhash &blkhash) {
    amc::FCtype *ctype = blkhash.p_linfld->p_arg;
    while (ctype->c_bltin == NULL && c_datafld_N(*ctype) == 1) {
        ctype = c_datafld_Find(*ctype, 0)->p_arg;
    }
    return ctype->c_bltin ? ctype : NULL;
}

// -----------------------------------------------------------------------------

static bool EarlierPoolQ(amc::FField &field, amc::FField &first_inst) {
    bool same_scope = field.p_ctype == first_inst.p_ctype;
    bool glob = GlobalQ(*field.p_ctype);
    bool pool_earlier = first_inst.rowid < field.rowid;
    bool ret = !glob && same_scope && !field.c_cascdel;
    ret = ret && pool_earlier;
    ret = ret && (first_inst.reftype == dmmeta_Reftype_reftype_Lary
                  || first_inst.reftype == dmmeta_Reftype_reftype_Tary);
    return ret;
}

// -----------------------------------------------------------------------------

static void Blkhash_Check(amc::FField &field) {
    // check if the field is an index for an item whose pool is in the same
    // scope but declared earlier (e.g. Lary, Tary).
    amc::FField *first_inst = amc::FirstInst(*field.p_arg);
    if (first_inst && EarlierPoolQ(field,*first_inst)) {
        prerr("amc.hash_order"
              <<Keyval("index_field",field.field)
              <<Keyval("element_field",first_inst->field)
              <<Keyval("element_type",field.p_arg->ctype)
              <<Keyval("comment","Index field is declared in the same scope"
                       " and later than the pool of its element field"
                       " but this index is not cascdel. This will cause a crash during destruction."
                       " Either add cascdel to this index, or reorder index and pool"));
        algo_lib::_db.exit_code=1;
    }
    amc::FBlkhash &blkhash = *field.c_blkhash;
    amc::FField &linfld = *blkhash.p_linfld;
    amc::FCtype *keytype = blkhash.p_hashfld->p_arg;
    amc::FCtype *terminal = LinTerminal(blkhash);
    if (linfld.p_ctype != keytype) {
        prerr("amc.blkhash_linfld"
              <<Keyval("blkhash",field.field)
              <<Keyval("linfld",blkhash.linfld)
              <<Keyval("keytype",keytype->ctype)
              <<Keyval("comment","linfld must be a field of the key ctype (hashfld arg)"));
        algo_lib::_db.exit_code=1;
    }
    if (FldfuncQ(linfld)) {
        prerr("amc.blkhash_linfld_fldfunc"
              <<Keyval("blkhash",field.field)
              <<Keyval("linfld",blkhash.linfld)
              <<Keyval("comment","linfld must be a plain Val field, not a fldfunc"));
        algo_lib::_db.exit_code=1;
    }
    if (!terminal || !terminal->c_bltin->likeu64 || !terminal->c_csize || terminal->c_csize->size > 8) {
        prerr("amc.blkhash_linfld_type"
              <<Keyval("blkhash",field.field)
              <<Keyval("linfld",blkhash.linfld)
              <<Keyval("comment","linfld must reach an integer builtin (u8..u64), possibly via single-field wrappers"));
        algo_lib::_db.exit_code=1;
    }
    if (blkhash.linbits < 1 || blkhash.linbits > 24) {
        prerr("amc.blkhash_linbits"
              <<Keyval("blkhash",field.field)
              <<Keyval("linbits",i32(blkhash.linbits))
              <<Keyval("comment","linbits must be in 1..24"));
        algo_lib::_db.exit_code=1;
    }
    amc::FChash *chash = keytype->c_chash;
    if (!chash || chash->hashtype == dmmeta_Hashtype_hashtype_Linear) {
        prerr("amc.blkhash_keyhash"
              <<Keyval("blkhash",field.field)
              <<Keyval("keytype",keytype->ctype)
              <<Keyval("comment","key ctype must have a chash record with a real hash function"));
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Blkhash() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FBlkhash &blkhash = *field.c_blkhash;
    amc::FField *hashfld = blkhash.p_hashfld;

    Set(R, "$Hashfldarg" , Argtype(*hashfld));
    Set(R, "$Hashfldtype", tempstr() << ns_Get(*hashfld->p_arg) << "::" << name_Get(*hashfld->p_arg));
    Set(R, "$gethashfld" , FieldvalExpr(field.p_arg, *hashfld,"row"));
    Set(R, "$linsuffix"  , LinSuffix(blkhash));
    Set(R, "$linbits"    , tempstr() << i32(blkhash.linbits));
    Set(R, "$slotn"      , tempstr() << (1<<i32(blkhash.linbits)));
    Set(R, "$slotmask"   , tempstr() << ((1<<i32(blkhash.linbits))-1));
    Set(R, "$Blk"        , "$Parname_$name_Blk");

    InsVar(R, field.p_ctype     , "$Blk**", "$name_buckets_elems", "", "pointer to bucket array (chains of blocks)");
    InsVar(R, field.p_ctype     , "i32", "$name_buckets_n", "", "number of buckets (power of 2)");
    InsVar(R, field.p_ctype     , "i32", "$name_nblk", "", "number of resident blocks");
    InsVar(R, field.p_ctype     , "i32", "$name_n", "", "number of elements in the index");

    amc::ind_fwddecl_GetOrCreate(Subst(R,"$ns.$ns.$Parname_$name_Blk"));
    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_Blk {// block of $field: masked key + 2^$linbits direct slots");
    Ins(&R, ns.curstext, "    $Hashfldtype key;// key with the low $linbits linfld bits zeroed; identifies the block");
    Ins(&R, ns.curstext, "    $Parname_$name_Blk* next;// bucket chain");
    Ins(&R, ns.curstext, "    $Cpptype** elem;// $slotn slot pointers; slot = linfld & $slotmask");
    Ins(&R, ns.curstext, "    u32 hashval;// hash of masked key");
    Ins(&R, ns.curstext, "    u32 n;// number of occupied slots; block is freed when it reaches zero");
    Ins(&R, ns.curstext, "};");

    Blkhash_Check(field);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_Find() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& find = amc::CreateCurFunc();
    Ins(&R, find.ret  , "$Cpptype*", false);
    Ins(&R, find.proto, "$name_Find($Parent, $Hashfldarg key)", false);
    Ins(&R, find.body, "$Hashfldtype mkey = key;");
    Ins(&R, find.body, "mkey$linsuffix &= ~u64($slotmask);");
    Ins(&R, find.body, "u32 index = $Hashfldtype_Hash(0, mkey) & ($parname.$name_buckets_n - 1);");
    Ins(&R, find.body, "$Blk *blk = $parname.$name_buckets_elems[index];");
    Ins(&R, find.body, "while (blk && !(blk->key == mkey)) {");
    Ins(&R, find.body, "    blk = blk->next;");
    Ins(&R, find.body, "}");
    Ins(&R, find.body, "$Cpptype *ret = blk ? blk->elem[u64(key$linsuffix) & $slotmask] : NULL;");
    Ins(&R, find.body, "return ret;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_FindX() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& findx = amc::CreateCurFunc();
    Ins(&R, findx.ret  , "$Cpptype&", false);
    Ins(&R, findx.proto, "$name_FindX($Parent, $Hashfldarg key)", false);
    Ins(&R, findx.body, "$Cpptype* ret = $name_Find($pararg, key);");
    Ins(&R, findx.body, "vrfy(ret, tempstr() << \"$ns.key_error  table:$name  key:'\"<<key<<\"'  comment:'key not found'\");");
    Ins(&R, findx.body, "return *ret;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_InsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& ins = amc::CreateCurFunc();
    Ins(&R, ins.comment, "Insert row into the index. Return true if row is reachable through the index after the function completes.");
    Ins(&R, ins.ret  , "bool", false);
    Ins(&R, ins.proto, "$name_InsertMaybe($Parent, $Cpptype& row)", false);
    Ins(&R, ins.body, "bool retval = true;");
    Ins(&R, ins.body, "$Hashfldtype mkey = $gethashfld;");
    Ins(&R, ins.body, "u32 slot = u32(u64(mkey$linsuffix) & $slotmask);");
    Ins(&R, ins.body, "mkey$linsuffix &= ~u64($slotmask);");
    Ins(&R, ins.body, "u32 hashval = $Hashfldtype_Hash(0, mkey);");
    Ins(&R, ins.body, "u32 index = hashval & ($parname.$name_buckets_n - 1);");
    Ins(&R, ins.body, "$Blk *blk = $parname.$name_buckets_elems[index];");
    Ins(&R, ins.body, "while (blk && !(blk->key == mkey)) {");
    Ins(&R, ins.body, "    blk = blk->next;");
    Ins(&R, ins.body, "}");
    Ins(&R, ins.body, "if (!blk) {");
    Ins(&R, ins.body, "    if ($parname.$name_nblk + 1 > $parname.$name_buckets_n) {");
    Ins(&R, ins.body, "        // grow bucket array, re-chain blocks by stored hashval");
    Ins(&R, ins.body, "        i32 new_nbuckets = $parname.$name_buckets_n * 2;");
    Ins(&R, ins.body, "        $Blk **new_buckets = ($Blk**)$basepool_AllocMem(sizeof($Blk*) * new_nbuckets);");
    Ins(&R, ins.body, "        if (UNLIKELY(!new_buckets)) {");
    Ins(&R, ins.body, "            FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, ins.body, "        }");
    Ins(&R, ins.body, "        memset(new_buckets, 0, sizeof($Blk*) * new_nbuckets);");
    Ins(&R, ins.body, "        for (i32 i = 0; i < $parname.$name_buckets_n; i++) {");
    Ins(&R, ins.body, "            $Blk *elem = $parname.$name_buckets_elems[i];");
    Ins(&R, ins.body, "            while (elem) {");
    Ins(&R, ins.body, "                $Blk *next = elem->next;");
    Ins(&R, ins.body, "                u32 bidx   = elem->hashval & (new_nbuckets - 1);");
    Ins(&R, ins.body, "                elem->next = new_buckets[bidx];");
    Ins(&R, ins.body, "                new_buckets[bidx] = elem;");
    Ins(&R, ins.body, "                elem = next;");
    Ins(&R, ins.body, "            }");
    Ins(&R, ins.body, "        }");
    Ins(&R, ins.body, "        $basepool_FreeMem($parname.$name_buckets_elems, sizeof($Blk*) * $parname.$name_buckets_n);");
    Ins(&R, ins.body, "        $parname.$name_buckets_elems = new_buckets;");
    Ins(&R, ins.body, "        $parname.$name_buckets_n = new_nbuckets;");
    Ins(&R, ins.body, "        index = hashval & ($parname.$name_buckets_n - 1);");
    Ins(&R, ins.body, "    }");
    Ins(&R, ins.body, "    $Cpptype **elems = ($Cpptype**)$basepool_AllocMem(sizeof($Cpptype*) * $slotn);");
    Ins(&R, ins.body, "    void *blkmem = $basepool_AllocMem(sizeof($Blk));");
    Ins(&R, ins.body, "    if (UNLIKELY(!elems || !blkmem)) {");
    Ins(&R, ins.body, "        FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, ins.body, "    }");
    Ins(&R, ins.body, "    memset(elems, 0, sizeof($Cpptype*) * $slotn);");
    Ins(&R, ins.body, "    blk = new(blkmem) $Blk;");
    Ins(&R, ins.body, "    blk->key = mkey;");
    Ins(&R, ins.body, "    blk->hashval = hashval;");
    Ins(&R, ins.body, "    blk->n = 0;");
    Ins(&R, ins.body, "    blk->elem = elems;");
    Ins(&R, ins.body, "    blk->next = $parname.$name_buckets_elems[index];");
    Ins(&R, ins.body, "    $parname.$name_buckets_elems[index] = blk;");
    Ins(&R, ins.body, "    $parname.$name_nblk++;");
    Ins(&R, ins.body, "}");
    Ins(&R, ins.body, "if (blk->elem[slot] == &row) {");
    Ins(&R, ins.body, "    // row already in the index");
    Ins(&R, ins.body, "} else if (blk->elem[slot]) {");
    Ins(&R, ins.body, "    retval = false; // a different row with the same key");
    Ins(&R, ins.body, "} else {");
    Ins(&R, ins.body, "    blk->elem[slot] = &row;");
    Ins(&R, ins.body, "    blk->n++;");
    Ins(&R, ins.body, "    $parname.$name_n++;");
    if (amc::FindFfunc(field, amcdb_cbtype_OnXref, true)) {
        Ins(&R, ins.body, "    $name_OnXref($pararg, row); // dmmeta.ffunc:$field/OnXref");
    }
    Ins(&R, ins.body, "}");
    Ins(&R, ins.body, "return retval;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_Remove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& rem = amc::CreateCurFunc();
    Ins(&R, rem.ret  , "void", false);
    Ins(&R, rem.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R, rem.body, "$Hashfldtype mkey = $gethashfld;");
    Ins(&R, rem.body, "u32 slot = u32(u64(mkey$linsuffix) & $slotmask);");
    Ins(&R, rem.body, "mkey$linsuffix &= ~u64($slotmask);");
    Ins(&R, rem.body, "u32 index = $Hashfldtype_Hash(0, mkey) & ($parname.$name_buckets_n - 1);");
    Ins(&R, rem.body, "$Blk **prev = &$parname.$name_buckets_elems[index];");
    Ins(&R, rem.body, "while ($Blk *blk = *prev) {");
    Ins(&R, rem.body, "    if (blk->key == mkey) {");
    Ins(&R, rem.body, "        if (blk->elem[slot] == &row) { // ignore requests to remove a row that's not in the index");
    Ins(&R, rem.body, "            blk->elem[slot] = NULL;");
    Ins(&R, rem.body, "            blk->n--;");
    Ins(&R, rem.body, "            $parname.$name_n--;");
    if (amc::FindFfunc(field, amcdb_cbtype_OnUnref, true)) {
        Ins(&R, rem.body, "            $name_OnUnref($pararg, row); // dmmeta.ffunc:$field/OnUnref");
    }
    Ins(&R, rem.body, "            if (blk->n == 0) { // empty block returns to the pool");
    Ins(&R, rem.body, "                *prev = blk->next;");
    Ins(&R, rem.body, "                $basepool_FreeMem(blk->elem, sizeof($Cpptype*) * $slotn);");
    Ins(&R, rem.body, "                blk->~$Parname_$name_Blk();");
    Ins(&R, rem.body, "                $basepool_FreeMem(blk, sizeof($Blk));");
    Ins(&R, rem.body, "                $parname.$name_nblk--;");
    Ins(&R, rem.body, "            }");
    Ins(&R, rem.body, "        }");
    Ins(&R, rem.body, "        break;");
    Ins(&R, rem.body, "    }");
    Ins(&R, rem.body, "    prev = &blk->next;");
    Ins(&R, rem.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_FindRemove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (amc::FindFfunc(field, "FindRemove", true)) {
        amc::FFunc& findrem = amc::CreateCurFunc();
        Ins(&R, findrem.ret  , "$Cpptype*", false);
        Ins(&R, findrem.proto, "$name_FindRemove($Parent, $Hashfldarg key)", false);
        Ins(&R, findrem.body, "$Hashfldtype mkey = key;");
        Ins(&R, findrem.body, "u32 slot = u32(u64(mkey$linsuffix) & $slotmask);");
        Ins(&R, findrem.body, "mkey$linsuffix &= ~u64($slotmask);");
        Ins(&R, findrem.body, "u32 index = $Hashfldtype_Hash(0, mkey) & ($parname.$name_buckets_n - 1);");
        Ins(&R, findrem.body, "$Blk **prev = &$parname.$name_buckets_elems[index];");
        Ins(&R, findrem.body, "$Cpptype *ret = NULL;");
        Ins(&R, findrem.body, "while ($Blk *blk = *prev) {");
        Ins(&R, findrem.body, "    if (blk->key == mkey) {");
        Ins(&R, findrem.body, "        ret = blk->elem[slot];");
        Ins(&R, findrem.body, "        if (ret) {");
        Ins(&R, findrem.body, "            blk->elem[slot] = NULL;");
        Ins(&R, findrem.body, "            blk->n--;");
        Ins(&R, findrem.body, "            $parname.$name_n--;");
        Ins(&R, findrem.body, "            if (blk->n == 0) { // empty block returns to the pool");
        Ins(&R, findrem.body, "                *prev = blk->next;");
        Ins(&R, findrem.body, "                $basepool_FreeMem(blk->elem, sizeof($Cpptype*) * $slotn);");
        Ins(&R, findrem.body, "                blk->~$Parname_$name_Blk();");
        Ins(&R, findrem.body, "                $basepool_FreeMem(blk, sizeof($Blk));");
        Ins(&R, findrem.body, "                $parname.$name_nblk--;");
        Ins(&R, findrem.body, "            }");
        Ins(&R, findrem.body, "        }");
        Ins(&R, findrem.body, "        break;");
        Ins(&R, findrem.body, "    }");
        Ins(&R, findrem.body, "    prev = &blk->next;");
        Ins(&R, findrem.body, "}");
        Ins(&R, findrem.body, "return ret;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();// already exists!
        Ins(&R, cascdel.body, "for (i32 i = 0; i < $parname.$name_buckets_n; i++) {");
        Ins(&R, cascdel.body, "    $Blk *blk = $parname.$name_buckets_elems[i];");
        Ins(&R, cascdel.body, "    while (blk) {");
        Ins(&R, cascdel.body, "        $Blk *next = blk->next;");
        Ins(&R, cascdel.body, "        // deleting the last element frees blk; count first, read nothing after");
        Ins(&R, cascdel.body, "        u32 remaining = blk->n;");
        Ins(&R, cascdel.body, "        for (u32 s = 0; remaining > 0; s++) {");
        Ins(&R, cascdel.body, "            $Cpptype *elem = blk->elem[s];");
        Ins(&R, cascdel.body, "            if (elem) {");
        Ins(&R, cascdel.body, "                remaining--;");
        Ins(&R, cascdel.body, DeleteExpr(field, "$pararg", "*elem") << ";");
        Ins(&R, cascdel.body, "            }");
        Ins(&R, cascdel.body, "        }");
        Ins(&R, cascdel.body, "        blk = next;");
        Ins(&R, cascdel.body, "    }");
        Ins(&R, cascdel.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    init.priv = true;// static
    Ins(&R, init.body, "// initialize block hash index for $Cpptype;");
    Ins(&R, init.body, "$parname.$name_n             \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_nblk          \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_buckets_n     \t= 4; // ($field)");
    Ins(&R, init.body, "$parname.$name_buckets_elems \t= ($Blk**)$basepool_AllocMem(sizeof($Blk*)*$parname.$name_buckets_n); // initial buckets ($field)");
    Ins(&R, init.body, "if (!$parname.$name_buckets_elems) {");
    Ins(&R, init.body, "    FatalErrorExit(\"out of memory\"); // ($field)");
    Ins(&R, init.body, "}");
    Ins(&R, init.body, "memset($parname.$name_buckets_elems, 0, sizeof($Blk*)*$parname.$name_buckets_n); // ($field)");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& uninit = amc::CreateCurFunc();
    if (GlobalQ(*field.p_ctype)) {
        Ins(&R, uninit.body, "// skip destruction of $name in global scope");
    } else {
        Ins(&R, uninit.body, "// free all blocks, then the bucket array ($field)");
        Ins(&R, uninit.body, "for (i32 i = 0; i < $parname.$name_buckets_n; i++) {");
        Ins(&R, uninit.body, "    $Blk *blk = $parname.$name_buckets_elems[i];");
        Ins(&R, uninit.body, "    while (blk) {");
        Ins(&R, uninit.body, "        $Blk *next = blk->next;");
        Ins(&R, uninit.body, "        $basepool_FreeMem(blk->elem, sizeof($Cpptype*) * $slotn);");
        Ins(&R, uninit.body, "        blk->~$Parname_$name_Blk();");
        Ins(&R, uninit.body, "        $basepool_FreeMem(blk, sizeof($Blk));");
        Ins(&R, uninit.body, "        blk = next;");
        Ins(&R, uninit.body, "    }");
        Ins(&R, uninit.body, "}");
        Ins(&R, uninit.body, "$basepool_FreeMem($parname.$name_buckets_elems, sizeof($Blk*)*$parname.$name_buckets_n); // ($field)");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Blkhash_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    Set(R, "$curspararg", ch_N(Subst(R,"$pararg")) ? "*curs.$parname" : "");

    Ins(&R, ns.curstext    , "");
    Ins(&R, ns.curstext    , "struct $Parname_$name_curs {// cursor");
    Ins(&R, ns.curstext    , "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext    , "    $Partype *parent;");
    Ins(&R, ns.curstext    , "    i32 bucket;");
    Ins(&R, ns.curstext    , "    $Parname_$name_Blk *blk;");
    Ins(&R, ns.curstext    , "    i32 slot;");
    Ins(&R, ns.curstext    , "    $Parname_$name_curs() { parent=NULL; bucket=0; blk=NULL; slot=0; }");
    Ins(&R, ns.curstext    , "};");
    Ins(&R, ns.curstext    , "");

    {
        amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
        curs_next.inl=false;// uninline it
        Ins(&R, curs_next.comment, "proceed to next item");
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
        Ins(&R, curs_next.body, "curs.slot++;");
        Ins(&R, curs_next.body, "bool done = false;");
        Ins(&R, curs_next.body, "while (!done) {");
        Ins(&R, curs_next.body, "    if (!curs.blk) {");
        Ins(&R, curs_next.body, "        curs.bucket++;");
        Ins(&R, curs_next.body, "        if (curs.bucket >= curs.parent->$name_buckets_n) {");
        Ins(&R, curs_next.body, "            done = true; // end of index: blk stays NULL");
        Ins(&R, curs_next.body, "        } else {");
        Ins(&R, curs_next.body, "            curs.blk = curs.parent->$name_buckets_elems[curs.bucket];");
        Ins(&R, curs_next.body, "            curs.slot = 0;");
        Ins(&R, curs_next.body, "        }");
        Ins(&R, curs_next.body, "    } else if (curs.slot >= $slotn) {");
        Ins(&R, curs_next.body, "        curs.blk = curs.blk->next;");
        Ins(&R, curs_next.body, "        curs.slot = 0;");
        Ins(&R, curs_next.body, "    } else if (curs.blk->elem[curs.slot]) {");
        Ins(&R, curs_next.body, "        done = true; // resident slot found");
        Ins(&R, curs_next.body, "    } else {");
        Ins(&R, curs_next.body, "        curs.slot++;");
        Ins(&R, curs_next.body, "    }");
        Ins(&R, curs_next.body, "}");
    }

    {
        amc::FFunc& reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
        reset.inl=false;// uninline it
        Ins(&R, reset.ret  , "void", false);
        Ins(&R, reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
        Ins(&R, reset.body, "curs.parent = &parent;");
        Ins(&R, reset.body, "curs.bucket = 0;");
        Ins(&R, reset.body, "curs.blk = parent.$name_buckets_elems[0]; // index never has zero buckets");
        Ins(&R, reset.body, "curs.slot = -1;");
        Ins(&R, reset.body, "$Parname_$name_curs_Next(curs); // advance to first resident slot");
    }

    {
        amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.blk != NULL;");
    }

    {
        amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return *curs.blk->elem[curs.slot];");
    }
}
