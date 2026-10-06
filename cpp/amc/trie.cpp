// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/trie.cpp
//
// A Trie holds values keyed by a dense integer (dmmeta.trie.linfld), in a tree
// of fixed-size nodes.  Each level of the tree branches on nbit bits of the key,
// so a node has 2^nbit children and a leaf holds 2^nbit value slots, one per
// value of the key's low nbit bits.  A leaf keeps an occupancy bit per slot, so
// a slot is empty or holds a value whatever the value is.
// The key is absolute.  The root covers keys below 2^(nbit*(height+1)), and a
// key past that raises the root by one level: a new node whose first child is
// the old root.  So the tree grows one fixed node at a time, and no operation
// ever copies what the tree already holds.  A leaf is freed when its last value
// goes, and an interior node when its last child goes.
// The parent caches the leaf the last lookup found.  Consecutive keys share a
// leaf, so an append at the tip, or a reader walking forward, finds its leaf
// with one compare.

#include "include/algo.h"
#include "include/amc.h"

// -----------------------------------------------------------------------------

// Report the defects in FIELD's dmmeta.trie row that would make the generated
// code wrong: a key that does not reach an integer builtin, or a fanout out of
// range.
static void CheckTrie(amc::FField &field) {
    amc::FTrie &trie = *field.c_trie;
    amc::FField &linfld = *trie.p_linfld;
    amc::FCtype *terminal = amc::LinfldTerminal(linfld);
    if (amc::FldfuncQ(linfld)) {
        prerr("amc.trie_linfld_fldfunc"
              <<Keyval("trie",field.field)
              <<Keyval("linfld",trie.linfld)
              <<Keyval("comment","linfld must be a plain Val field, not a fldfunc"));
        algo_lib::_db.exit_code=1;
    }
    if (!terminal || !terminal->c_bltin->likeu64 || !terminal->c_csize || terminal->c_csize->size > 8) {
        prerr("amc.trie_linfld_type"
              <<Keyval("trie",field.field)
              <<Keyval("linfld",trie.linfld)
              <<Keyval("comment","linfld must reach an integer builtin (u8..u64), possibly via single-field wrappers"));
        algo_lib::_db.exit_code=1;
    }
    if (trie.nbit < 1 || trie.nbit > 16) {
        prerr("amc.trie_nbit"
              <<Keyval("trie",field.field)
              <<Keyval("nbit",i32(trie.nbit))
              <<Keyval("comment","nbit must be in 1..16"));
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

// Declare the fields a Trie adds to its parent and the leaf and node structs,
// set the substitutions every Trie tfunc uses, and check the dmmeta.trie row.
void amc::tclass_Trie() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FTrie &trie = *field.c_trie;
    i32 nbit = i32(trie.nbit);
    i32 nslot = 1 << nbit;

    Set(R, "$dflt"      , field.dflt.value);
    Set(R, "$Keytype"   , trie.p_linfld->p_ctype->cpp_type);
    Set(R, "$linsuffix" , LinfldSuffix(*trie.p_linfld));
    Set(R, "$nbit"      , tempstr() << nbit);
    Set(R, "$fanout"     , tempstr() << nslot);
    Set(R, "$slotmask"  , tempstr() << (nslot - 1));
    Set(R, "$occwords"      , tempstr() << i32_Max(1, nslot / 64));
    Set(R, "$Leaf"      , "$ns::$Parname_$name_Leaf");
    Set(R, "$Node"      , "$ns::$Parname_$name_Node");

    InsVar(R, field.p_ctype, "void*", "$name_root", "", "root of the trie: a leaf at height 0, else a node");
    InsVar(R, field.p_ctype, "u32", "$name_height", "", "number of interior levels above the leaves");
    InsVar(R, field.p_ctype, "$Leaf*", "$name_last", "", "leaf the last lookup found, or NULL");
    InsVar(R, field.p_ctype, "u64", "$name_lastkey", "", "key of the cached leaf, low $nbit bits zero");
    InsVar(R, field.p_ctype, "i64", "$name_n", "", "number of values the trie holds");
    InsVar(R, field.p_ctype, "i32", "$name_nleaf", "", "number of resident leaves");
    InsVar(R, field.p_ctype, "i32", "$name_nnode", "", "number of resident interior nodes");

    amc::ind_fwddecl_GetOrCreate(Subst(R,"$ns.$ns.$Parname_$name_Leaf"));
    amc::ind_fwddecl_GetOrCreate(Subst(R,"$ns.$ns.$Parname_$name_Node"));
    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_Leaf {// leaf of $field: $fanout value slots and their occupancy");
    Ins(&R, ns.curstext, "    $Cpptype* elem;// $fanout value slots; slot s holds the value whose key is the leaf's key plus s");
    Ins(&R, ns.curstext, "    u32 n;// number of occupied slots; the leaf is freed when it reaches zero");
    Ins(&R, ns.curstext, "    u64 occ[$occwords];// bit s is set while slot s holds a value");
    Ins(&R, ns.curstext, "};");
    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_Node {// interior node of $field");
    Ins(&R, ns.curstext, "    void* child[$fanout];// a node, or a leaf at the lowest interior level; NULL where absent");
    Ins(&R, ns.curstext, "};");

    CheckTrie(field);
}

// -----------------------------------------------------------------------------

// Generate the initializer, which leaves the trie empty with no node allocated.
void amc::tfunc_Trie_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& init = amc::CreateCurFunc();
    Ins(&R, init.body, "$parname.$name_root    \t= NULL; // ($field)");
    Ins(&R, init.body, "$parname.$name_height  \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_last    \t= NULL; // ($field)");
    Ins(&R, init.body, "$parname.$name_lastkey \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_n       \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_nleaf   \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_nnode   \t= 0; // ($field)");
}

// -----------------------------------------------------------------------------

// Generate the destructor, which frees every node of a trie on a record.
void amc::tfunc_Trie_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& uninit = amc::CreateCurFunc();
    if (GlobalQ(*field.p_ctype)) {
        Ins(&R, uninit.body, "// skip destruction of $name in global scope");
    } else {
        Ins(&R, uninit.body, "$name_RemoveAll($pararg); // free every node ($field)");
    }
}

// -----------------------------------------------------------------------------

// Generate N, which returns the number of values the trie holds.
void amc::tfunc_Trie_N() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i64", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

// -----------------------------------------------------------------------------

// Generate Bytes, the memory the trie holds: each leaf's header and value
// slots, and each interior node, from the resident counts the parent keeps.
void amc::tfunc_Trie_Bytes() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "u64", false);
    Ins(&R, func.proto, "$name_Bytes($Cparent)", false);
    Ins(&R, func.body, "return u64($parname.$name_nleaf) * (sizeof($Leaf) + sizeof($Cpptype) * $fanout)");
    Ins(&R, func.body, "    + u64($parname.$name_nnode) * sizeof($Node);");
}

// -----------------------------------------------------------------------------

// Generate EmptyQ, which returns true when the trie holds no value.
void amc::tfunc_Trie_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

// -----------------------------------------------------------------------------

// Generate FindLeaf, which returns the leaf covering key K or NULL, and caches
// the leaf it found.  The walk descends one level per nbit bits of K, starting
// at the root, which covers keys below 2^(nbit*(height+1)).
void amc::tfunc_Trie_FindLeaf() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc();
    func.priv = true;
    Ins(&R, func.ret  , "$Leaf*", false);
    Ins(&R, func.proto, "$name_FindLeaf($Parent, u64 k)", false);
    Ins(&R, func.body, "u64 lkey = k & ~u64($slotmask);");
    Ins(&R, func.body, "$Leaf *ret = NULL;");
    Ins(&R, func.body, "u32 top = $nbit * ($parname.$name_height + 1);");
    Ins(&R, func.body, "if ($parname.$name_last && $parname.$name_lastkey == lkey) {");
    Ins(&R, func.body, "    ret = $parname.$name_last;");
    Ins(&R, func.body, "} else if ($parname.$name_root && (top >= 64 || (k >> top) == 0)) {");
    Ins(&R, func.body, "    void *node = $parname.$name_root;");
    Ins(&R, func.body, "    for (u32 level = $parname.$name_height; node && level > 0; level--) {");
    Ins(&R, func.body, "        node = (($Node*)node)->child[(k >> ($nbit * level)) & $slotmask];");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    ret = ($Leaf*)node;");
    Ins(&R, func.body, "    if (ret) {");
    Ins(&R, func.body, "        $parname.$name_last = ret;");
    Ins(&R, func.body, "        $parname.$name_lastkey = lkey;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");
}

// -----------------------------------------------------------------------------

// Generate Find, which returns the value at a key, or NULL when its slot is empty.
void amc::tfunc_Trie_Find() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& find = amc::CreateCurFunc();
    Ins(&R, find.ret  , "$Cpptype*", false);
    Ins(&R, find.proto, "$name_Find($Parent, $Keytype key)", false);
    Ins(&R, find.body, "u64 k = u64(key$linsuffix);");
    Ins(&R, find.body, "$Leaf *leaf = $name_FindLeaf($pararg, k);");
    Ins(&R, find.body, "u32 s = u32(k & $slotmask);");
    Ins(&R, find.body, "return leaf && ((leaf->occ[s >> 6] >> (s & 63)) & 1) ? leaf->elem + s : NULL;");
}

// -----------------------------------------------------------------------------

// Generate Alloc, which returns the value at a key and constructs it in an
// empty slot.  A key the root does not cover raises the root a level at a time,
// and a missing node or leaf on the way down is allocated and linked in.
void amc::tfunc_Trie_Alloc() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& alloc = amc::CreateCurFunc();
    Ins(&R, alloc.ret  , "$Cpptype&", false);
    Ins(&R, alloc.proto, "$name_Alloc($Parent, $Keytype key)", false);
    Ins(&R, alloc.body, "u64 k = u64(key$linsuffix);");
    Ins(&R, alloc.body, "$Leaf *leaf = $name_FindLeaf($pararg, k);");
    Ins(&R, alloc.body, "if (!leaf) {");
    Ins(&R, alloc.body, "    // raise the root until it covers K; an empty trie only needs the height");
    Ins(&R, alloc.body, "    while (!($nbit * ($parname.$name_height + 1) >= 64 || (k >> ($nbit * ($parname.$name_height + 1))) == 0)) {");
    Ins(&R, alloc.body, "        if ($parname.$name_root) {");
    Ins(&R, alloc.body, "            $Node *node = ($Node*)$basepool_AllocMem(sizeof($Node));");
    Ins(&R, alloc.body, "            if (UNLIKELY(!node)) {");
    Ins(&R, alloc.body, "                FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, alloc.body, "            }");
    Ins(&R, alloc.body, "            memset(node, 0, sizeof($Node));");
    Ins(&R, alloc.body, "            node->child[0] = $parname.$name_root;");
    Ins(&R, alloc.body, "            $parname.$name_root = node;");
    Ins(&R, alloc.body, "            $parname.$name_nnode++;");
    Ins(&R, alloc.body, "        }");
    Ins(&R, alloc.body, "        $parname.$name_height++;");
    Ins(&R, alloc.body, "    }");
    Ins(&R, alloc.body, "    // descend, linking in each missing node");
    Ins(&R, alloc.body, "    void **slot = &$parname.$name_root;");
    Ins(&R, alloc.body, "    for (u32 level = $parname.$name_height; level > 0; level--) {");
    Ins(&R, alloc.body, "        if (!*slot) {");
    Ins(&R, alloc.body, "            $Node *node = ($Node*)$basepool_AllocMem(sizeof($Node));");
    Ins(&R, alloc.body, "            if (UNLIKELY(!node)) {");
    Ins(&R, alloc.body, "                FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, alloc.body, "            }");
    Ins(&R, alloc.body, "            memset(node, 0, sizeof($Node));");
    Ins(&R, alloc.body, "            *slot = node;");
    Ins(&R, alloc.body, "            $parname.$name_nnode++;");
    Ins(&R, alloc.body, "        }");
    Ins(&R, alloc.body, "        slot = &(($Node*)*slot)->child[(k >> ($nbit * level)) & $slotmask];");
    Ins(&R, alloc.body, "    }");
    Ins(&R, alloc.body, "    leaf = ($Leaf*)$basepool_AllocMem(sizeof($Leaf));");
    Ins(&R, alloc.body, "    $Cpptype *elem = ($Cpptype*)$basepool_AllocMem(sizeof($Cpptype) * $fanout);");
    Ins(&R, alloc.body, "    if (UNLIKELY(!leaf || !elem)) {");
    Ins(&R, alloc.body, "        FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, alloc.body, "    }");
    Ins(&R, alloc.body, "    memset(leaf, 0, sizeof($Leaf));");
    Ins(&R, alloc.body, "    leaf->elem = elem;");
    Ins(&R, alloc.body, "    *slot = leaf;");
    Ins(&R, alloc.body, "    $parname.$name_nleaf++;");
    Ins(&R, alloc.body, "    $parname.$name_last = leaf;");
    Ins(&R, alloc.body, "    $parname.$name_lastkey = k & ~u64($slotmask);");
    Ins(&R, alloc.body, "}");
    Ins(&R, alloc.body, "u32 s = u32(k & $slotmask);");
    Ins(&R, alloc.body, "$Cpptype *elem = leaf->elem + s;");
    Ins(&R, alloc.body, "if (!((leaf->occ[s >> 6] >> (s & 63)) & 1)) {");
    Ins(&R, alloc.body, "    new (elem) $Cpptype($dflt);");
    Ins(&R, alloc.body, "    leaf->occ[s >> 6] |= u64(1) << (s & 63);");
    Ins(&R, alloc.body, "    leaf->n++;");
    Ins(&R, alloc.body, "    $parname.$name_n++;");
    Ins(&R, alloc.body, "}");
    Ins(&R, alloc.body, "return *elem;");
}

// -----------------------------------------------------------------------------

// Generate FreeNode, which destroys every value under NODE at LEVEL and frees
// NODE with its subtree.  The recursion is one call per level, so its depth is
// the height of the trie, at most 64/nbit rounded up.
void amc::tfunc_Trie_FreeNode() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& func = amc::CreateCurFunc();
    func.priv = true;
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_FreeNode($Parent, void *node, u32 level)", false);
    Ins(&R, func.body, "if (level == 0) {");
    Ins(&R, func.body, "    $Leaf *leaf = ($Leaf*)node;");
    if (HasDtorQ(*field.p_arg)) {
        Ins(&R, func.body, "    for (u32 s = 0; s < $fanout; s++) {");
        Ins(&R, func.body, "        if ((leaf->occ[s >> 6] >> (s & 63)) & 1) {");
        Ins(&R, func.body, "            leaf->elem[s].~$Ctype();");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "    }");
    }
    Ins(&R, func.body, "    $parname.$name_n -= leaf->n;");
    Ins(&R, func.body, "    if ($parname.$name_last == leaf) {");
    Ins(&R, func.body, "        $parname.$name_last = NULL;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    $basepool_FreeMem(leaf->elem, sizeof($Cpptype) * $fanout);");
    Ins(&R, func.body, "    $basepool_FreeMem(leaf, sizeof($Leaf));");
    Ins(&R, func.body, "    $parname.$name_nleaf--;");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    $Node *inode = ($Node*)node;");
    Ins(&R, func.body, "    for (u32 i = 0; i < $fanout; i++) {");
    Ins(&R, func.body, "        if (inode->child[i]) {");
    Ins(&R, func.body, "            $name_FreeNode($pararg, inode->child[i], level - 1);");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    $basepool_FreeMem(inode, sizeof($Node));");
    Ins(&R, func.body, "    $parname.$name_nnode--;");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

// Generate RemoveAll, which destroys every value and frees every node.
void amc::tfunc_Trie_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& removeall = amc::CreateCurFunc();
    Ins(&R, removeall.ret  , "void", false);
    Ins(&R, removeall.proto, "$name_RemoveAll($Parent)", false);
    Ins(&R, removeall.body, "if ($parname.$name_root) {");
    Ins(&R, removeall.body, "    $name_FreeNode($pararg, $parname.$name_root, $parname.$name_height);");
    Ins(&R, removeall.body, "}");
    Ins(&R, removeall.body, "$parname.$name_root = NULL;");
    Ins(&R, removeall.body, "$parname.$name_height = 0;");
    Ins(&R, removeall.body, "$parname.$name_last = NULL;");
}

// -----------------------------------------------------------------------------

// Generate RemoveRange, which destroys the values at keys LO..HI-1, and its
// recursive helper RemoveNode.  A leaf or a subtree the range covers whole is
// freed at once without visiting its slots.  An interior node the removal left
// with no child is freed on the way back up; it holds no count, so the test is
// a scan of its children, and it runs only for a node one of whose children
// was just freed.
void amc::tfunc_Trie_RemoveRange() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    {
        amc::FFunc& func = amc::CreateCurFunc(false, "RemoveNode");
        func.priv = true;
        func.comment = "";
        Ins(&R, func.comment, "Destroy the values at keys LO..HI-1 under the node in SLOT, which covers keys");
        Ins(&R, func.comment, "from BASE at LEVEL.  SLOT is cleared when the node is freed.");
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$name_RemoveNode($Parent, void **slot, u32 level, u64 base, u64 lo, u64 hi)", false);
        Ins(&R, func.body, "u32 shift = $nbit * level;");
        Ins(&R, func.body, "// the last key this node covers; at the root of a full-height trie it is the last u64");
        Ins(&R, func.body, "u64 last = shift + $nbit >= 64 ? ~u64(0) : base + ((u64(1) << (shift + $nbit)) - 1);");
        Ins(&R, func.body, "if (hi - 1 < base || lo > last) {");
        Ins(&R, func.body, "    // the range misses this node");
        Ins(&R, func.body, "} else if (lo <= base && hi - 1 >= last) {");
        Ins(&R, func.body, "    $name_FreeNode($pararg, *slot, level);");
        Ins(&R, func.body, "    *slot = NULL;");
        Ins(&R, func.body, "} else if (level == 0) {");
        Ins(&R, func.body, "    $Leaf *leaf = ($Leaf*)*slot;");
        Ins(&R, func.body, "    u32 s0 = lo <= base ? 0 : u32(lo - base);");
        Ins(&R, func.body, "    u32 s1 = hi - 1 >= last ? $fanout : u32(hi - base);");
        Ins(&R, func.body, "    for (u32 s = s0; s < s1; s++) {");
        Ins(&R, func.body, "        if ((leaf->occ[s >> 6] >> (s & 63)) & 1) {");
        if (HasDtorQ(*field.p_arg)) {
            Ins(&R, func.body, "            leaf->elem[s].~$Ctype();");
        }
        Ins(&R, func.body, "            leaf->occ[s >> 6] &= ~(u64(1) << (s & 63));");
        Ins(&R, func.body, "            leaf->n--;");
        Ins(&R, func.body, "            $parname.$name_n--;");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "    if (leaf->n == 0) {");
        Ins(&R, func.body, "        $name_FreeNode($pararg, leaf, 0);");
        Ins(&R, func.body, "        *slot = NULL;");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "} else {");
        Ins(&R, func.body, "    $Node *node = ($Node*)*slot;");
        Ins(&R, func.body, "    u64 i0 = lo <= base ? 0 : (lo - base) >> shift;");
        Ins(&R, func.body, "    u64 i1 = hi - 1 >= last ? $slotmask : (hi - 1 - base) >> shift;");
        Ins(&R, func.body, "    bool freed = false;");
        Ins(&R, func.body, "    for (u64 i = i0; i <= i1; i++) {");
        Ins(&R, func.body, "        if (node->child[i]) {");
        Ins(&R, func.body, "            $name_RemoveNode($pararg, &node->child[i], level - 1, base + (i << shift), lo, hi);");
        Ins(&R, func.body, "            freed = freed || !node->child[i];");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "    bool empty = freed;");
        Ins(&R, func.body, "    for (u32 i = 0; empty && i < $fanout; i++) {");
        Ins(&R, func.body, "        empty = !node->child[i];");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "    if (empty) {");
        Ins(&R, func.body, "        $basepool_FreeMem(node, sizeof($Node));");
        Ins(&R, func.body, "        $parname.$name_nnode--;");
        Ins(&R, func.body, "        *slot = NULL;");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "}");
    }

    {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$name_RemoveRange($Parent, $Keytype lo, $Keytype hi)", false);
        Ins(&R, func.body, "u64 klo = u64(lo$linsuffix);");
        Ins(&R, func.body, "u64 khi = u64(hi$linsuffix);");
        Ins(&R, func.body, "if ($parname.$name_root && klo < khi) {");
        Ins(&R, func.body, "    $name_RemoveNode($pararg, &$parname.$name_root, $parname.$name_height, 0, klo, khi);");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "if (!$parname.$name_root) {");
        Ins(&R, func.body, "    $parname.$name_height = 0;");
        Ins(&R, func.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate Remove, which destroys the value at one key through RemoveNode.  The
// range it passes ends one past the key, and at the largest key that end wraps
// to 0; RemoveNode compares the range's last key (HI-1), which is the key again,
// so the largest key is removed like any other.
void amc::tfunc_Trie_Remove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_Remove($Parent, $Keytype key)", false);
    Ins(&R, func.body, "u64 k = u64(key$linsuffix);");
    Ins(&R, func.body, "if ($parname.$name_root) {");
    Ins(&R, func.body, "    $name_RemoveNode($pararg, &$parname.$name_root, $parname.$name_height, 0, k, k + 1);");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (!$parname.$name_root) {");
    Ins(&R, func.body, "    $parname.$name_height = 0;");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

// Generate NextLeaf, which returns the first leaf at or past a key under a node,
// descending only into children that exist, so an absent subtree costs one
// skipped pointer.  The parent argument only tells two tries of one name apart.
void amc::tfunc_Trie_NextLeaf() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& func = amc::CreateCurFunc();
    func.priv = true;
    Ins(&R, func.comment, "Return the first leaf at or past key K under NODE, which covers keys from BASE");
    Ins(&R, func.comment, "at LEVEL, and set LKEY to its key.  Return NULL when there is none.");
    Ins(&R, func.ret  , "$Leaf*", false);
    Ins(&R, func.proto, "$name_NextLeaf($Parent, void *node, u32 level, u64 base, u64 k, u64 &lkey)", false);
    Ins(&R, func.body, "$Leaf *ret = NULL;");
    Ins(&R, func.body, "if (level == 0) {");
    Ins(&R, func.body, "    // a child is entered with K inside its range; only a root leaf can lie below K");
    Ins(&R, func.body, "    ret = k <= base + $slotmask ? ($Leaf*)node : NULL;");
    Ins(&R, func.body, "    lkey = base;");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    u32 shift = $nbit * level;");
    Ins(&R, func.body, "    u64 i0 = k <= base ? 0 : (k - base) >> shift;");
    Ins(&R, func.body, "    for (u64 i = i0; !ret && i < $fanout; i++) {");
    Ins(&R, func.body, "        void *child = (($Node*)node)->child[i];");
    Ins(&R, func.body, "        if (child) {");
    Ins(&R, func.body, "            u64 cbase = base + (i << shift);");
    Ins(&R, func.body, "            ret = $name_NextLeaf($pararg, child, level - 1, cbase, k < cbase ? cbase : k, lkey);");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");
    if (!GlobalQ(*field.p_ctype)) {
        MaybeUnused(func, Subst(R,"$parname"));
    }
}

// -----------------------------------------------------------------------------

// Generate PrevLeaf, the mirror of NextLeaf: the last leaf at or below a key.
void amc::tfunc_Trie_PrevLeaf() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& func = amc::CreateCurFunc();
    func.priv = true;
    Ins(&R, func.comment, "Return the last leaf at or below key K under NODE, which covers keys from BASE");
    Ins(&R, func.comment, "at LEVEL, and set LKEY to its key.  K is at or past BASE.  Return NULL when");
    Ins(&R, func.comment, "there is none.");
    Ins(&R, func.ret  , "$Leaf*", false);
    Ins(&R, func.proto, "$name_PrevLeaf($Parent, void *node, u32 level, u64 base, u64 k, u64 &lkey)", false);
    Ins(&R, func.body, "$Leaf *ret = NULL;");
    Ins(&R, func.body, "if (level == 0) {");
    Ins(&R, func.body, "    ret = ($Leaf*)node;");
    Ins(&R, func.body, "    lkey = base;");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    u32 shift = $nbit * level;");
    Ins(&R, func.body, "    u64 i1 = u64_Min((k - base) >> shift, $slotmask);");
    Ins(&R, func.body, "    for (u64 i = i1 + 1; !ret && i > 0; i--) {");
    Ins(&R, func.body, "        void *child = (($Node*)node)->child[i - 1];");
    Ins(&R, func.body, "        if (child) {");
    Ins(&R, func.body, "            u64 cbase = base + ((i - 1) << shift);");
    Ins(&R, func.body, "            u64 clast = cbase + ((u64(1) << shift) - 1);");
    Ins(&R, func.body, "            ret = $name_PrevLeaf($pararg, child, level - 1, cbase, k > clast ? clast : k, lkey);");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");
    if (!GlobalQ(*field.p_ctype)) {
        MaybeUnused(func, Subst(R,"$parname"));
    }
}

// -----------------------------------------------------------------------------

// Generate NextKey, which finds the lowest key at or above FROM that holds a
// value.  The first leaf comes through FindLeaf, so a walk forward through
// dense keys stays on the cached leaf, and a leaf finds its next occupied slot
// a 64-bit word at a time.
void amc::tfunc_Trie_NextKey() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.comment, "Set OUT to the lowest key at or above FROM that holds a value, and return true;");
    Ins(&R, func.comment, "return false and leave OUT alone when there is none.");
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.proto, "$name_NextKey($Parent, $Keytype from, $Keytype &out)", false);
    Ins(&R, func.body, "u64 k = u64(from$linsuffix);");
    Ins(&R, func.body, "u64 lkey = k & ~u64($slotmask);");
    Ins(&R, func.body, "$Leaf *leaf = $name_FindLeaf($pararg, k);");
    Ins(&R, func.body, "if (!leaf && $parname.$name_root) {");
    Ins(&R, func.body, "    leaf = $name_NextLeaf($pararg, $parname.$name_root, $parname.$name_height, 0, k, lkey);");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "bool ret = false;");
    Ins(&R, func.body, "while (leaf && !ret) {");
    Ins(&R, func.body, "    u32 s = k <= lkey ? 0 : u32(k - lkey);");
    Ins(&R, func.body, "    for (u32 w = s >> 6; !ret && w < $occwords; w++) {");
    Ins(&R, func.body, "        u64 bits = leaf->occ[w] & (w == (s >> 6) ? ~u64(0) << (s & 63) : ~u64(0));");
    Ins(&R, func.body, "        if (bits) {");
    Ins(&R, func.body, "            out = from;");
    Ins(&R, func.body, "            out$linsuffix = lkey + w * 64 + algo::u64_BitScanForward(bits);");
    Ins(&R, func.body, "            ret = true;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (!ret) {");
    Ins(&R, func.body, "        k = lkey + $fanout;");
    Ins(&R, func.body, "        leaf = k == 0 ? NULL : $name_NextLeaf($pararg, $parname.$name_root, $parname.$name_height, 0, k, lkey);");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");
}

// -----------------------------------------------------------------------------

// Generate PrevKey, the mirror of NextKey: the highest key at or below FROM that
// holds a value.
void amc::tfunc_Trie_PrevKey() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.comment, "Set OUT to the highest key at or below FROM that holds a value, and return true;");
    Ins(&R, func.comment, "return false and leave OUT alone when there is none.");
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.proto, "$name_PrevKey($Parent, $Keytype from, $Keytype &out)", false);
    Ins(&R, func.body, "u64 k = u64(from$linsuffix);");
    Ins(&R, func.body, "u64 lkey = k & ~u64($slotmask);");
    Ins(&R, func.body, "$Leaf *leaf = $name_FindLeaf($pararg, k);");
    Ins(&R, func.body, "if (!leaf && $parname.$name_root) {");
    Ins(&R, func.body, "    leaf = $name_PrevLeaf($pararg, $parname.$name_root, $parname.$name_height, 0, k, lkey);");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "bool ret = false;");
    Ins(&R, func.body, "while (leaf && !ret) {");
    Ins(&R, func.body, "    u32 s = k >= lkey + $slotmask ? $slotmask : u32(k - lkey);");
    Ins(&R, func.body, "    for (u32 w = (s >> 6) + 1; !ret && w > 0; w--) {");
    Ins(&R, func.body, "        u32 top = w - 1 == (s >> 6) ? (s & 63) : 63;");
    Ins(&R, func.body, "        u64 bits = leaf->occ[w - 1] & (top == 63 ? ~u64(0) : (u64(1) << (top + 1)) - 1);");
    Ins(&R, func.body, "        if (bits) {");
    Ins(&R, func.body, "            out = from;");
    Ins(&R, func.body, "            out$linsuffix = lkey + (w - 1) * 64 + algo::u64_BitScanReverse(bits);");
    Ins(&R, func.body, "            ret = true;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (!ret) {");
    Ins(&R, func.body, "        k = lkey - 1;");
    Ins(&R, func.body, "        leaf = lkey == 0 ? NULL : $name_PrevLeaf($pararg, $parname.$name_root, $parname.$name_height, 0, k, lkey);");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return ret;");
}

// -----------------------------------------------------------------------------

// Generate the cursor, which visits the values in key order, a leaf at a time
// through NextLeaf.
void amc::tfunc_Trie_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    bool glob = GlobalQ(*field.p_ctype);
    // NextLeaf takes the parent only to tell two tries of one name apart
    Set(R, "$curspararg" , glob ? "" : "*curs.parent");
    Set(R, "$resetpararg", glob ? "" : "parent");

    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_curs {// cursor");
    Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext, "    $Partype *parent;");
    Ins(&R, ns.curstext, "    $Leaf *leaf;// current leaf; NULL at the end");
    Ins(&R, ns.curstext, "    u64 lkey;// key of the current leaf");
    Ins(&R, ns.curstext, "    u32 slot;// current slot in the leaf");
    Ins(&R, ns.curstext, "    $Parname_$name_curs() { parent=NULL; leaf=NULL; lkey=0; slot=0; }");
    Ins(&R, ns.curstext, "};");
    Ins(&R, ns.curstext, "");

    {
        amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
        curs_next.inl = false;
        Ins(&R, curs_next.comment, "proceed to next item");
        Ins(&R, curs_next.ret  , "void", false);
        Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
        Ins(&R, curs_next.body, "curs.slot++;");
        Ins(&R, curs_next.body, "bool done = false;");
        Ins(&R, curs_next.body, "while (!done) {");
        Ins(&R, curs_next.body, "    if (!curs.leaf) {");
        Ins(&R, curs_next.body, "        done = true;");
        Ins(&R, curs_next.body, "    } else if (curs.slot >= $fanout) {");
        Ins(&R, curs_next.body, "        u64 next = curs.lkey + $fanout;");
        Ins(&R, curs_next.body, "        curs.leaf = next == 0 ? NULL : $name_NextLeaf($curspararg, curs.parent->$name_root, curs.parent->$name_height, 0, next, curs.lkey);");
        Ins(&R, curs_next.body, "        curs.slot = 0;");
        Ins(&R, curs_next.body, "    } else if ((curs.leaf->occ[curs.slot >> 6] >> (curs.slot & 63)) & 1) {");
        Ins(&R, curs_next.body, "        done = true;");
        Ins(&R, curs_next.body, "    } else {");
        Ins(&R, curs_next.body, "        curs.slot++;");
        Ins(&R, curs_next.body, "    }");
        Ins(&R, curs_next.body, "}");
    }

    {
        amc::FFunc& reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
        reset.inl = false;
        Ins(&R, reset.ret  , "void", false);
        Ins(&R, reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
        Ins(&R, reset.body, "curs.parent = &parent;");
        Ins(&R, reset.body, "curs.leaf = parent.$name_root ? $name_NextLeaf($resetpararg, parent.$name_root, parent.$name_height, 0, 0, curs.lkey) : NULL;");
        Ins(&R, reset.body, "curs.slot = 0;");
        Ins(&R, reset.body, "if (curs.leaf && !(curs.leaf->occ[0] & 1)) {");
        Ins(&R, reset.body, "    $Parname_$name_curs_Next(curs); // advance to the first occupied slot");
        Ins(&R, reset.body, "}");
    }

    {
        amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.leaf != NULL;");
    }

    {
        amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return curs.leaf->elem[curs.slot];");
    }
}
