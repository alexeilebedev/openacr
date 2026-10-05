// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/avl.cpp -- AVL tree
//

#include "include/amc.h"
#include "include/gen/amc_gen.h"

// Declare the tree's fields: on the parent the root, the element count and the cached
// first and last elements; on each element its parent, children and subtree height.
// An element outside the tree has its parent pointer at -1.
void amc::tclass_Atree(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    vrfy(field.c_sortfld, "amc.avl: sortfield must must be specified through sortfld table");
    amc::FField &sortfld   = *field.c_sortfld->p_sortfld;

    Set(R, "$Sortfld"   , name_Get(sortfld));
    Set(R, "$Sortstore" , sortfld.cpp_type);
    Set(R, "$Left"      , "$xfname_left");
    Set(R, "$Right"     , "$xfname_right");
    Set(R, "$Up"        , "$xfname_up");
    Set(R, "$Depth"     , "$xfname_depth");
    Set(R, "$Root"      , "$parname.$name_root");
    Set(R, "$NElem"     , "$parname.$name_n");
    Set(R, "$Firstptr"  , "$parname.$name_first");
    Set(R, "$Lastptr"   , "$parname.$name_last");

    InsVar(R, field.p_ctype     , "$Cpptype*", "$name_root", "", "Root of the tree");
    InsVar(R, field.p_ctype     , "i32", "$name_n", "", "number of elements in the tree");
    InsVar(R, field.p_ctype     , "$Cpptype*", "$name_first", "", "Smallest element, NULL when empty");
    InsVar(R, field.p_ctype     , "$Cpptype*", "$name_last", "", "Largest element, NULL when empty");

    InsVar(R, field.p_arg       , "$Cpptype*", "$Up", "",    "pointer to parent");
    InsVar(R, field.p_arg       , "$Cpptype*", "$Left", "",  "Left child");
    InsVar(R, field.p_arg       , "$Cpptype*", "$Right", "", "Right child");
    InsVar(R, field.p_arg       , "i32"      , "$Depth", "", "Height of the subtree, 1 for a leaf");
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname"     , Refname(*field.p_arg));

    Ins(&R, child_init->body  , "$fname.$Up = ($Cpptype*)-1; // ($field) not in tree");
    Ins(&R, child_init->body  , "$fname.$Left = NULL;");
    Ins(&R, child_init->body  , "$fname.$Right = NULL;");
    Ins(&R, child_init->body  , "$fname.$Depth = 0;");
}

// Generate the sort predicate, which takes the parent for comparisons that need it.
void amc::tfunc_Atree_ElemLt(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FField   &sortfld   = *field.c_sortfld->p_sortfld;

    amc::FFunc& lt = amc::CreateCurFunc();
    lt.priv = true;

    Ins(&R, lt.ret  , "bool",false);
    Ins(&R, lt.proto, "$name_ElemLt($Parent, $Cpptype &a, $Cpptype &b)",false);
    Ins(&R, lt.body, "(void)$parname;");
    amc::FCtype *base = GetBaseType(*field.p_arg,NULL);

    bool ownfld = sortfld.c_fcmp && (sortfld.p_ctype == field.p_arg || (base && sortfld.p_ctype == base));
    if (ownfld) {
        Ins(&R, lt.body, "return $Sortfld_Lt(a, b);");// direct field of child type
    } else {
        Set(R, "$aval", FieldvalExpr(field.p_arg, sortfld, "a"));
        Set(R, "$bval", FieldvalExpr(field.p_arg, sortfld, "b"));
        Ins(&R, lt.body, "return $aval < $bval;");
    }
}

// Generate NAME, the in-order neighbor of an element: the outermost element of its subtree
// on the FWD side, or else the nearest ancestor reached from the BACK side.
static void GenIter(strptr name, strptr fwd, strptr back){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = true;
    Set(R, "$Fwd"  , fwd);
    Set(R, "$Back" , back);
    Ins(&R, func.ret  , "$Cpptype*", false);
    Ins(&R, func.proto, tempstr()<<"$name_"<<name<<"($Cpptype& node)", false);
    Ins(&R, func.body, "$Cpptype* result = node.$Fwd;");
    Ins(&R, func.body, "if (result) {");
    Ins(&R, func.body, "    while (result->$Back) {");
    Ins(&R, func.body, "        result = result->$Back;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    $Cpptype* child = &node;");
    Ins(&R, func.body, "    result = node.$Up;");
    Ins(&R, func.body, "    while (result && result->$Fwd == child) {");
    Ins(&R, func.body, "        child  = result;");
    Ins(&R, func.body, "        result = result->$Up;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "return result;");
}

// Generate Prev, the mirror of Next.
void amc::tfunc_Atree_Prev(){
    GenIter("Prev", "$Left", "$Right");
}

// Generate Next: the element after NODE in sort order, or NULL after the last.
void amc::tfunc_Atree_Next(){
    GenIter("Next", "$Right", "$Left");
}

// Generate Init: an empty tree.
void amc::tfunc_Atree_Init(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.body, "$Root = NULL; // ($field)");
    Ins(&R, func.body, "$NElem = 0;");
    Ins(&R, func.body, "$Firstptr = NULL;");
    Ins(&R, func.body, "$Lastptr = NULL;");
}

// Generate InTreeQ: true when the element is in the tree.
void amc::tfunc_Atree_InTreeQ(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.proto, "$name_InTreeQ($Cpptype& row)", false);
    Ins(&R, func.body, "return row.$Up != ($Cpptype*)-1;");
}

// Generate EmptyQ: true when the tree holds no element.
void amc::tfunc_Atree_EmptyQ(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.body, "return $Root == NULL;");
}

// Generate First: the smallest element, read from the parent.
void amc::tfunc_Atree_First(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = true;
    Ins(&R, func.proto, "$name_First($Parent)" , false);
    Ins(&R, func.ret, Subst(R,"$Cpptype*"), false);
    Ins(&R, func.body, "return $Firstptr;");
}

// Generate Last: the largest element, read from the parent.
void amc::tfunc_Atree_Last(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = true;
    Ins(&R, func.proto, "$name_Last($Parent)" , false);
    Ins(&R, func.ret, Subst(R,"$Cpptype*"), false);
    Ins(&R, func.body, "return $Lastptr;");
}

// Generate NAME, a single rotation that lifts the RISE child of NODE into NODE's place
// and returns it. The rise child's OTHER subtree moves under NODE, and the heights of the
// two moved elements are recomputed from their children.
static void GenRotate(strptr name, strptr rise, strptr other){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    func.priv = true;
    Set(R, "$Rise"  , rise);
    Set(R, "$Other" , other);
    Ins(&R, func.ret  , "$Cpptype*", false);
    Ins(&R, func.proto, tempstr()<<"$name_"<<name<<"($Parent, $Cpptype& node)", false);
    Ins(&R, func.body , "$Cpptype* top = node.$Rise;");
    Ins(&R, func.body , "$Cpptype* mid = top->$Other;");
    Ins(&R, func.body , "$Cpptype* up  = node.$Up;");
    Ins(&R, func.body , "node.$Rise = mid;");
    Ins(&R, func.body , "if (mid) {");
    Ins(&R, func.body , "    mid->$Up = &node;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "top->$Other = &node;");
    Ins(&R, func.body , "node.$Up    = top;");
    Ins(&R, func.body , "top->$Up    = up;");
    Ins(&R, func.body , "if (up == NULL) {");
    Ins(&R, func.body , "    $Root = top;");
    Ins(&R, func.body , "} else if (up->$Left == &node) {");
    Ins(&R, func.body , "    up->$Left = top;");
    Ins(&R, func.body , "} else {");
    Ins(&R, func.body , "    up->$Right = top;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "i32 hother = node.$Other ? node.$Other->$Depth : 0;");
    Ins(&R, func.body , "i32 hmid   = mid ? mid->$Depth : 0;");
    Ins(&R, func.body , "node.$Depth = i32_Max(hother, hmid) + 1;");
    Ins(&R, func.body , "i32 hrise  = top->$Rise ? top->$Rise->$Depth : 0;");
    Ins(&R, func.body , "top->$Depth = i32_Max(node.$Depth, hrise) + 1;");
    Ins(&R, func.body , "return top;");
}

// Generate RotateLeft: the right child rises.
void amc::tfunc_Atree_RotateLeft(){
    GenRotate("RotateLeft", "$Right", "$Left");
}

// Generate RotateRight: the left child rises.
void amc::tfunc_Atree_RotateRight(){
    GenRotate("RotateRight", "$Left", "$Right");
}

// Generate Rebalance, which restores heights and balance from NODE up to the root after
// one element was linked or unlinked below NODE. A subtree whose height comes out the same
// as before leaves every ancestor as it was, so the walk stops there: an insert stops at
// most one rotation after it starts, and a remove continues only while heights shrink.
void amc::tfunc_Atree_Rebalance(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    func.priv = true;
    Ins(&R, func.ret,   "void", false);
    Ins(&R, func.proto, "$name_Rebalance($Parent, $Cpptype* node)", false);
    Ins(&R, func.body,  "while (node) {");
    Ins(&R, func.body,  "    i32 old = node->$Depth;");
    Ins(&R, func.body,  "    i32 hl  = node->$Left  ? node->$Left->$Depth  : 0;");
    Ins(&R, func.body,  "    i32 hr  = node->$Right ? node->$Right->$Depth : 0;");
    Ins(&R, func.body,  "    if (hl > hr + 1) {");
    Ins(&R, func.body,  "        $Cpptype* child = node->$Left;");
    Ins(&R, func.body,  "        if ((child->$Left ? child->$Left->$Depth : 0) < (child->$Right ? child->$Right->$Depth : 0)) {");
    Ins(&R, func.body,  "            $name_RotateLeft($pararg, *child);");
    Ins(&R, func.body,  "        }");
    Ins(&R, func.body,  "        node = $name_RotateRight($pararg, *node);");
    Ins(&R, func.body,  "    } else if (hr > hl + 1) {");
    Ins(&R, func.body,  "        $Cpptype* child = node->$Right;");
    Ins(&R, func.body,  "        if ((child->$Right ? child->$Right->$Depth : 0) < (child->$Left ? child->$Left->$Depth : 0)) {");
    Ins(&R, func.body,  "            $name_RotateRight($pararg, *child);");
    Ins(&R, func.body,  "        }");
    Ins(&R, func.body,  "        node = $name_RotateLeft($pararg, *node);");
    Ins(&R, func.body,  "    } else {");
    Ins(&R, func.body,  "        node->$Depth = i32_Max(hl, hr) + 1;");
    Ins(&R, func.body,  "    }");
    Ins(&R, func.body,  "    node = node->$Depth == old ? NULL : node->$Up;");
    Ins(&R, func.body,  "}");
}

// Generate Insert: descend once to the leaf position, link the row there, note whether
// every step went left (a new first) or right (a new last), and rebalance from the parent.
// A row equal to existing elements goes after them.
void amc::tfunc_Atree_Insert(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.proto,"$name_Insert($Parent, $Cpptype& row)", false);
    Ins(&R, func.ret,  "void", false);
    Ins(&R, func.body, "if (row.$Up == ($Cpptype*)-1) {");
    Ins(&R, func.body, "    $Cpptype* up  = NULL;");
    Ins(&R, func.body, "    $Cpptype* cur = $Root;");
    Ins(&R, func.body, "    bool left  = false;");
    Ins(&R, func.body, "    bool first = true;");
    Ins(&R, func.body, "    bool last  = true;");
    Ins(&R, func.body, "    while (cur) {");
    Ins(&R, func.body, "        up    = cur;");
    Ins(&R, func.body, "        left  = $name_ElemLt($pararg, row, *cur);");
    Ins(&R, func.body, "        first = first && left;");
    Ins(&R, func.body, "        last  = last && !left;");
    Ins(&R, func.body, "        cur   = left ? cur->$Left : cur->$Right;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    row.$Up    = up;");
    Ins(&R, func.body, "    row.$Depth = 1;");
    Ins(&R, func.body, "    if (up == NULL) {");
    Ins(&R, func.body, "        $Root = &row;");
    Ins(&R, func.body, "    } else if (left) {");
    Ins(&R, func.body, "        up->$Left = &row;");
    Ins(&R, func.body, "    } else {");
    Ins(&R, func.body, "        up->$Right = &row;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (first) {");
    Ins(&R, func.body, "        $Firstptr = &row;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    if (last) {");
    Ins(&R, func.body, "        $Lastptr = &row;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    $NElem++;");
    Ins(&R, func.body, "    $name_Rebalance($pararg, up);");
    if (amc::FindFfunc(field, amcdb_cbtype_OnXref, true)) {
        Ins(&R, func.body, "    $name_OnXref($pararg, row); // dmmeta.ffunc:$field/OnXref");
    }
    Ins(&R, func.body, "}");
}

// Emit into FUNC the loop that unlinks every element of the tree, and with DEL deletes
// each one. The loop detaches a child before descending into it and resets an element
// once both its children are gone, so it needs no stack and no recursion.
static void GenUnlinkAll(amc::FFunc &func, bool del){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    Ins(&R, func.body , "$Cpptype* node = $Root;");
    Ins(&R, func.body , "while (node) {");
    Ins(&R, func.body , "    $Cpptype* next = node->$Left;");
    Ins(&R, func.body , "    if (next) {");
    Ins(&R, func.body , "        node->$Left = NULL;");
    Ins(&R, func.body , "    } else if (node->$Right) {");
    Ins(&R, func.body , "        next = node->$Right;");
    Ins(&R, func.body , "        node->$Right = NULL;");
    Ins(&R, func.body , "    } else {");
    Ins(&R, func.body , "        next = node->$Up;");
    Ins(&R, func.body , "        node->$Up = ($Cpptype*)-1;");
    Ins(&R, func.body , "        node->$Depth = 0;");
    if (del) {
        Ins(&R, func.body, tempstr() << "        " << DeleteExpr(field,"$pararg","*node") << ";");
    }
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    node = next;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "$Root = NULL;");
    Ins(&R, func.body , "$NElem = 0;");
    Ins(&R, func.body , "$Firstptr = NULL;");
    Ins(&R, func.body , "$Lastptr = NULL;");
}

// Generate the cascade delete of every element, when the field has cascdel.
void amc::tfunc_Atree_Cascdel(){
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.c_cascdel){
        amc::FFunc& func = amc::CreateCurFunc();
        GenUnlinkAll(func, true);
    }
}

// Generate RemoveAll: unlink every element, deleting none.
void amc::tfunc_Atree_RemoveAll(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_RemoveAll($Parent)", false);
    GenUnlinkAll(func, false);
}

// Generate RemoveFirst: remove the smallest element, if any.
void amc::tfunc_Atree_RemoveFirst(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_RemoveFirst($Parent)", false);
    Ins(&R, func.body , "if ($Firstptr) {");
    Ins(&R, func.body , "    $name_Remove($pararg, *$Firstptr);");
    Ins(&R, func.body , "}");
}

// Generate Reinsert: move an element whose key changed to its new place.
void amc::tfunc_Atree_Reinsert(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_Reinsert($Parent, $Cpptype& node)", false);
    Ins(&R, func.body, "$name_Remove($pararg, node);");
    Ins(&R, func.body, "$name_Insert($pararg, node);");
}

// Generate Remove. A row with two children trades places with its successor, the
// leftmost element of its right subtree, which has no left child; any other row is
// replaced by its only child. Rebalancing starts at the lowest element whose children
// changed. The cached first and last move to the row's neighbors before it unlinks.
void amc::tfunc_Atree_Remove(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R, func.body , "if (row.$Up != ($Cpptype*)-1) {");
    Ins(&R, func.body , "    if ($Firstptr == &row) {");
    Ins(&R, func.body , "        $Firstptr = $name_Next(row);");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    if ($Lastptr == &row) {");
    Ins(&R, func.body , "        $Lastptr = $name_Prev(row);");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    $Cpptype* up   = row.$Up;");
    Ins(&R, func.body , "    $Cpptype* fix  = up;");
    Ins(&R, func.body , "    $Cpptype* repl = row.$Left ? row.$Left : row.$Right;");
    Ins(&R, func.body , "    if (row.$Left && row.$Right) {");
    Ins(&R, func.body , "        repl = row.$Right;");
    Ins(&R, func.body , "        while (repl->$Left) {");
    Ins(&R, func.body , "            repl = repl->$Left;");
    Ins(&R, func.body , "        }");
    Ins(&R, func.body , "        fix = repl;");
    Ins(&R, func.body , "        if (repl->$Up != &row) {");
    Ins(&R, func.body , "            fix = repl->$Up;");
    Ins(&R, func.body , "            fix->$Left = repl->$Right;");
    Ins(&R, func.body , "            if (repl->$Right) {");
    Ins(&R, func.body , "                repl->$Right->$Up = fix;");
    Ins(&R, func.body , "            }");
    Ins(&R, func.body , "            repl->$Right = row.$Right;");
    Ins(&R, func.body , "            row.$Right->$Up = repl;");
    Ins(&R, func.body , "        }");
    Ins(&R, func.body , "        repl->$Left = row.$Left;");
    Ins(&R, func.body , "        row.$Left->$Up = repl;");
    Ins(&R, func.body , "        repl->$Depth = row.$Depth;");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    if (repl) {");
    Ins(&R, func.body , "        repl->$Up = up;");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    if (up == NULL) {");
    Ins(&R, func.body , "        $Root = repl;");
    Ins(&R, func.body , "    } else if (up->$Left == &row) {");
    Ins(&R, func.body , "        up->$Left = repl;");
    Ins(&R, func.body , "    } else {");
    Ins(&R, func.body , "        up->$Right = repl;");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    $name_Rebalance($pararg, fix);");
    Ins(&R, func.body , "    row.$Up    = ($Cpptype*)-1;");
    Ins(&R, func.body , "    row.$Left  = NULL;");
    Ins(&R, func.body , "    row.$Right = NULL;");
    Ins(&R, func.body , "    row.$Depth = 0;");
    Ins(&R, func.body , "    $NElem--;");
    if (amc::FindFfunc(field, amcdb_cbtype_OnUnref, true)) {
        Ins(&R, func.body, "    $name_OnUnref($pararg, row); // dmmeta.ffunc:$field/OnUnref");
    }
    Ins(&R, func.body , "}");
}

// Generate NAME, a bound search.  With GREATER it returns the first element whose key
// is not less than VAL; without, the last element whose key is less than VAL.  It
// descends to the element where the search runs out of children, then steps with Next
// (or Prev) past the elements on the wrong side of VAL, which is at most one step.
static void GenFind(strptr name, bool greater){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FField   &sortfld   = *field.c_sortfld->p_sortfld;
    amc::FFunc& func = amc::CreateCurFunc();
    Set(R, "$aval"     , FieldvalExpr(field.p_arg, sortfld, "(*result)"));
    Set(R, "$Dir"      , greater ? "$name_Next" : "$name_Prev");
    // use operator < only
    Set(R, "$CompDir"  , greater ? "!($aval < val)" : "val < $aval");
    Set(R, "$CompNext" , greater ? "$aval < val"  : "!($aval < val)");
    func.inl = false;
    Ins(&R, func.ret  , "$Cpptype*", false);
    Ins(&R, func.proto, tempstr()<<"$name_"<<name<<"($Parent, const $Sortstore& val)", false);
    Ins(&R, func.body , "$Cpptype* result = $Root;");
    Ins(&R, func.body , "bool left = false;");
    Ins(&R, func.body , "while (result) {");
    Ins(&R, func.body , "    left = $CompDir;");
    Ins(&R, func.body , "    $Cpptype* side = left ? result->$Left : result->$Right;");
    Ins(&R, func.body , "    if (side == NULL) {");
    Ins(&R, func.body , "        break;");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    result = side;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "while (result && $CompNext) {");
    Ins(&R, func.body , "    result = $Dir(*result);");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "return result;");
}

// Generate FirstGe: the first element not less than a sortfld value.
void amc::tfunc_Atree_FirstGe(){
    GenFind("FirstGe", true);
}

// Generate LastLt: the last element less than a sortfld value.
void amc::tfunc_Atree_LastLt(){
    GenFind("LastLt", false);
}

// Generate the cursor, which walks with Next and needs no state beyond the current row.
void amc::tfunc_Atree_curs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField& field = *_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    Ins(&R, ns.curstext    , "");
    Ins(&R, ns.curstext    , "struct $Parname_$name_curs {// cursor");
    Ins(&R, ns.curstext    , "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext    , "    $Cpptype* row;");
    Ins(&R, ns.curstext    , "    $Parname_$name_curs() {");
    Ins(&R, ns.curstext    , "        row = NULL;");
    Ins(&R, ns.curstext    , "    }");
    Ins(&R, ns.curstext    , "};");
    Ins(&R, ns.curstext    , "");


    Set(R, "$CursArg", GlobalQ(*field.p_ctype) ? "" : "parent");

    amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
    Ins(&R, curs_reset.comment, "cursor points to valid item");
    Ins(&R, curs_reset.ret  , "void", false);
    Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype& $CursArg)", false);
    Ins(&R, curs_reset.body, "curs.row = $name_First($CursArg);");

    amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
    Ins(&R, curs_validq.comment, "cursor points to valid item");
    Ins(&R, curs_validq.ret  , "bool", false);
    Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
    Ins(&R, curs_validq.body, "return curs.row != NULL;");

    amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
    Ins(&R, curs_next.comment, "proceed to next item");
    Ins(&R, curs_next.ret  , "void", false);
    Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
    Ins(&R, curs_next.body, "curs.row = $name_Next(*curs.row);");

    amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
    Ins(&R, curs_access.comment, "item access");
    Ins(&R, curs_access.ret  , "$Cpptype&", false);
    Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
    Ins(&R, curs_access.body, "return *curs.row;");

}
