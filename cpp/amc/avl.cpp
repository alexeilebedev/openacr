// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/avl.cpp
//
// Created By: hayk.mkrtchyan alexei.lebedev
// Recent Changes: hayk.mkrtchyan alexei.lebedev
//

#include "include/amc.h"
#include "include/gen/amc_gen.h"

//Initialize the structs etc.
void amc::tclass_Atree(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    vrfy(field.c_sortfld, "amc.avl: sortfield must must be specified through sortfld table");
    amc::FField &sortfld   = *field.c_sortfld->p_sortfld;

    Set(R, "$Sortfld"   , name_Get(sortfld));
    Set(R, "$Sortstore" , sortfld.cpp_type);
    Set(R, "$Left"      , "$name_left");
    Set(R, "$Right"     , "$name_right");
    Set(R, "$Up"        , "$name_up");
    Set(R, "$Depth"     , "$name_depth");
    Set(R, "$Root"      , "$parname.$name_root");
    Set(R, "$NElem"     , "$parname.$name_n");

    InsVar(R, field.p_ctype     , "$Cpptype*", "$name_root", "", "Root of the tree");
    InsVar(R, field.p_ctype     , "i32", "$name_n", "", "number of elements in the tree");

    InsVar(R, field.p_arg       , "$Cpptype*", "$Up", "",    "pointer to parent");
    InsVar(R, field.p_arg       , "$Cpptype*", "$Left", "",  "Left child");
    InsVar(R, field.p_arg       , "$Cpptype*", "$Right", "", "Right child");
    InsVar(R, field.p_arg       , "i32"      , "$Depth", "", "Depth");
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname"     , Refname(*field.p_arg));

    Ins(&R, child_init->body  , "$fname.$Up = ($Cpptype*)-1; // ($field) not in tree");
    Ins(&R, child_init->body  , "$fname.$Left = NULL;");
    Ins(&R, child_init->body  , "$fname.$Right = NULL;");
    Ins(&R, child_init->body  , "$fname.$Depth = 0;");
}

//Predicate function, can take parent as argument in the case of complicated comparisons.
void amc::tfunc_Atree_ElemLt(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
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


static void GenIter(strptr name, bool next){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, tempstr()<<"$name_"<<name<<"($Cpptype& node)", false);
    Set(R, "$IterNextElem"  , next ? "$Right" :  "$Left" );
    Set(R, "$IterNextExpr"  , next ? "$name_FirstImpl" : "$name_LastImpl");
    AddRetval(func, Subst(R,"$Cpptype*"),"result","&node");
    Ins(&R, func.body, "if(result->$IterNextElem == NULL){");
    Ins(&R, func.body, "    while(result->$Up != NULL && result->$Up->$IterNextElem == result){");
    Ins(&R, func.body, "        result = result->$Up;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    result = result->$Up;");
    Ins(&R, func.body, "}else{");
    Ins(&R, func.body, "    result = $IterNextExpr(result->$IterNextElem);");
    Ins(&R, func.body, "}");
}

//Generate the Prev function for the element.
//Always symmetrical to Next through substitution of
//First -> Last and Left -> Right.
void amc::tfunc_Atree_Prev(){
    GenIter("Prev", false);
}

//Generate the Next function for the element
void amc::tfunc_Atree_Next(){
    GenIter("Next", true);
}


//Initialize Root and Number of elements
void amc::tfunc_Atree_Init(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.body, "$Root = NULL; // ($field)");
    Ins(&R, func.body, "$NElem = 0;");
}


//Returns true if the element is in tree.
void amc::tfunc_Atree_InTreeQ(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.proto, "$name_InTreeQ($Cpptype& row)", false);
    Ins(&R, func.body, "return row.$Up != ($Cpptype*)-1;");
}


//Returns true if the tree is empty.
//Plant a tree if empty!
void amc::tfunc_Atree_EmptyQ(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, func.ret  , "bool", false);
    Ins(&R, func.body, "return $Root == NULL;");
}



//Given an element finds the smallest element in the subtree.
void amc::tfunc_Atree_FirstImpl(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, "$name_FirstImpl($Cpptype* root)" , false);
    AddRetval(func, Subst(R,"$Cpptype*"),"result","root");
    Ins(&R, func.body, "while(result != NULL && result->$Left != NULL){");
    Ins(&R, func.body, "    result = result->$Left;");
    Ins(&R, func.body, "}");
}

//Given an element finds the largest element in the subtree.
void amc::tfunc_Atree_LastImpl(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, "$name_LastImpl($Cpptype* root)" , false);
    AddRetval(func, Subst(R,"$Cpptype*"),"result","root");
    Ins(&R, func.body, "while(result != NULL && result->$Right != NULL){");
    Ins(&R, func.body, "    result = result->$Right;");
    Ins(&R, func.body, "}");
}


//Returns the smallest element
void amc::tfunc_Atree_First(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, "$name_First($Parent)" , false);
    Ins(&R, func.ret, Subst(R,"$Cpptype*"), false);
    Ins(&R, func.body, "return $name_FirstImpl($Root);");
}

//Returns the largest element.
void amc::tfunc_Atree_Last(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, "$name_Last($Parent)" , false);
    Ins(&R, func.ret, Subst(R,"$Cpptype*"), false);
    Ins(&R, func.body, "return $name_LastImpl($Root);");
}

//Finds the child that violates the balance. Left child if no disbalance.
void amc::tfunc_Atree_TallerChild(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = true;
    func.priv = true;
    Ins(&R, func.ret,   Subst(R, "$Cpptype*"), false);
    Ins(&R, func.proto, "$name_TallerChild($Cpptype& node)", false);
    Ins(&R, func.body, "return $name_Balance(node) < 0 ? node.$Right : node.$Left;");
}

//Disconnect the node from its parent.
void amc::tfunc_Atree_Disconnect(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    func.priv = true;
    Ins(&R, func.ret,   "void", false);
    Ins(&R, func.proto, "$name_Disconnect($Cpptype& node)", false);
    Ins(&R, func.body,  "$Cpptype* parent = node.$Up;");
    Ins(&R, func.body,  "if(parent != NULL){");
    Ins(&R, func.body,  "    bool left = parent->$Left == &node;");
    Ins(&R, func.body,  "    (left ? parent->$Left : parent->$Right) = NULL;");
    Ins(&R, func.body,  "}");
    Ins(&R, func.body,  "node.$Up = NULL;");
}

//Rotates the tree from the direction from->to
//Assumption is that from is the child of to.
void amc::tfunc_Atree_Turn(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    func.priv = true;
    Ins(&R, func.proto, "$name_Turn($Cpptype& from, $Cpptype& to)", false);
    Ins(&R, func.ret,   "void", false);
    Ins(&R, func.body,  "$Cpptype* root = to.$Up;");
    Ins(&R, func.body,  "bool dir = root && root->$Left == &to;");
    Ins(&R, func.body,  "$name_Connect(root, &from, dir);");
    Ins(&R, func.body,  "dir = to.$Left == &from;");
    Ins(&R, func.body,  "$Cpptype* orphan = (dir ? from.$Right : from.$Left);//other side");
    Ins(&R, func.body,  "$name_Connect(&from, &to , !dir);");
    Ins(&R, func.body,  "$name_Connect(&to, orphan, dir);");
}


//1. Find disbalanced child.
//2. Find disbalanced grandchild.
//3. turn the grandchild toward child to have a-b-c simple case.
//4. Turn child onto node.
//5. Make sure to NOT turn the grandchild into child if child is balanced(Many hours of debugging).
void amc::tfunc_Atree_Rebalance(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, "$name_Rebalance($Cpptype& node)", false);
    Ins(&R, func.ret,   "void", false);
    Ins(&R, func.body,  "if (algo::Abs($name_Balance(node)) > 1){");
    Ins(&R, func.body,  "    $Cpptype* deep1 = $name_TallerChild(node);");
    Ins(&R, func.body,  "    $Cpptype* deep2 = $name_TallerChild(*deep1);");
    Ins(&R, func.body,  "    bool turn = $name_Balance(*deep1)!=0 && (node.$Left == deep1) != (deep1->$Left == deep2);");
    Ins(&R, func.body,  "    if(turn){");
    Ins(&R, func.body,  "        $name_Turn(*deep2, *deep1);");
    Ins(&R, func.body,  "        algo::TSwap(deep1, deep2);");
    Ins(&R, func.body,  "    }");
    Ins(&R, func.body,  "    $name_Turn(*deep1, node);");
    Ins(&R, func.body,  "    $UpdateDepth(node);");
    Ins(&R, func.body,  "    $UpdateDepth(*deep2);");
    Ins(&R, func.body,  "    $UpdateDepth(*deep1);");
    Ins(&R, func.body,  "}");
}


void amc::tfunc_Atree_UpdateDepth(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    func.priv = true;
    Ins(&R, func.proto, "$UpdateDepth($Cpptype& node)", false);
    Ins(&R, func.ret,   "void", false);
    Ins(&R, func.body,  "i32 ldepth = node.$Left  != NULL ? node.$Left->$Depth : 0;");
    Ins(&R, func.body,  "i32 rdepth = node.$Right != NULL ? node.$Right->$Depth : 0;");
    Ins(&R, func.body,  "node.$Depth = i32_Max(ldepth, rdepth) + 1;");
}

//Keep updating the depth and propagating up.
//If a node needs rebalancing we rebalance and go up
//Notice that after rebalance a node can go deeper down the tree.
void amc::tfunc_Atree_Propagate(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.proto, "$name_Propagate($Cpptype& pnode)", false);
    AddRetval(func, Subst(R,"$Cpptype*"),"root","&pnode");
    Ins(&R, func.body,  "$Cpptype* node = &pnode;");
    Ins(&R, func.body,  "while(node != NULL){");
    Ins(&R, func.body,  "    $UpdateDepth(*node);");
    Ins(&R, func.body,  "    $name_Rebalance(*node);");
    Ins(&R, func.body,  "    root = node;");
    Ins(&R, func.body,  "    node = node->$Up;");
    Ins(&R, func.body,  "}");
}



//Iterate down the tree starting from the parent and place
//the element in the appropriate leaf.
//Note that balance might be broken after this operation.
void amc::tfunc_Atree_InsertImpl(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.ret,   "void", false);
    Ins(&R, func.proto, "$name_InsertImpl($Parent, $Cpptype* parent, $Cpptype& row)" , false);
    Ins(&R, func.body,  "bool left = false;");
    Ins(&R, func.body,  "while(parent != NULL){");
    Ins(&R, func.body,  "    left = $name_ElemLt($pararg, row, *parent);");
    Ins(&R, func.body,  "    $Cpptype* side = left ? parent->$Left : parent->$Right;");
    Ins(&R, func.body,  "    if(side == NULL){");
    Ins(&R, func.body,  "        break;");
    Ins(&R, func.body,  "    }");
    Ins(&R, func.body,  "    parent = side;");
    Ins(&R, func.body,  "}");
    Ins(&R, func.body,  "$name_Connect(parent, &row, left);");
}

//1.Insert element starting from the root.
//2. Rebalance if necessary.
//3. Assign new root.
void amc::tfunc_Atree_Insert(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.proto,"$name_Insert($Parent, $Cpptype& row)", false);
    Ins(&R, func.ret,  "void", false);
    Ins(&R, func.body, "if(!$name_InTreeQ(row)){");
    Ins(&R, func.body, "    $NElem++;");
    Ins(&R, func.body, "    $name_InsertImpl($pararg, $Root, row);");
    Ins(&R, func.body, "    $Root = $name_Propagate(row);");
    Ins(&R, func.body, "}");
}

//Leftbalance - Rightbalance
void amc::tfunc_Atree_Balance(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.ret  , "i32", false);
    Ins(&R, func.proto, "$name_Balance($Cpptype& row)", false);
    Ins(&R, func.body , "i32 left  = row.$Left  ? row.$Left->$Depth  : 0;");
    Ins(&R, func.body , "i32 right = row.$Right ? row.$Right->$Depth : 0;");
    Ins(&R, func.body , "return left - right;");
}


//A recursive function to delete elements.
//if del flag is set it also destroys the elements.
void amc::tfunc_Atree_RemoveAllImpl(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField& field = *_db.genfield.p_field;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_RemoveAllImpl($Parent, $Cpptype* root, bool del)", false);
    Ins(&R, func.body ,     "if(root != NULL){");
    Ins(&R, func.body ,     "    $name_RemoveAllImpl($pararg, root->$Left, del);");
    Ins(&R, func.body ,     "    $name_RemoveAllImpl($pararg, root->$Right, del);");
    Ins(&R, func.body ,     "    $name_Disconnect(*root);");
    Ins(&R, func.body ,     "    root->$Depth = 0;//the pointers are taken care of by Disconnect");
    Ins(&R, func.body ,     "    root->$Up = ($Cpptype*)-1;//the pointers are taken care of by Disconnect");
    if (field.c_cascdel){
        Ins(&R, func.body , "    if(del){");
        Ins(&R, func.body,            DeleteExpr(field,"$pararg","*root")<<";");
        Ins(&R, func.body , "    }");
    }
    Ins(&R, func.body ,     "}");
}

void amc::tfunc_Atree_Cascdel(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (field.c_cascdel){
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.body , "$name_RemoveAllImpl($pararg, $Root, true);");
        Ins(&R, func.body, "$Root = NULL;");
        Ins(&R, func.body, "$NElem = 0;");
    }
}

//Remove all elements without deleting them.
void amc::tfunc_Atree_RemoveAll(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = true;
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_RemoveAll($Parent)", false);
    Ins(&R, func.body , "$name_RemoveAllImpl($pararg, $Root, false);");
    Ins(&R, func.body , "$Root = NULL;");
    Ins(&R, func.body , "$NElem = 0;");
}


//Remove the first element. Rebalance. Update root.
void amc::tfunc_Atree_RemoveFirst(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_RemoveFirst($Parent)", false);
    Ins(&R, func.body , "if(!$name_EmptyQ($pararg)){");
    Ins(&R, func.body , "    $name_Remove($pararg, *$name_First($pararg));");
    Ins(&R, func.body , "}");
}

//Reinsert an element
void amc::tfunc_Atree_Reinsert(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_Reinsert($Parent, $Cpptype& node)", false);
    Ins(&R, func.body, "$name_Remove($pararg, node);");
    Ins(&R, func.body, "$name_Insert($pararg, node);");
}

//Connect 2 elements (either can be NULL).
void amc::tfunc_Atree_Connect(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = true;
    func.priv = true;
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_Connect($Cpptype* parent, $Cpptype* child, bool left)", false);
    Ins(&R, func.body , "if(parent){");
    Ins(&R, func.body , "    (left ? parent->$Left : parent->$Right) = child;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "if(child){");
    Ins(&R, func.body , "    child->$Up = parent;");
    Ins(&R, func.body , "}");
}
//1. Find next/prev element in my subtree (opposite of imbalance direction).
//2. If that element has a child, swap it with the child by turning from child.
//3. Swap the element to be removed with the next.
//4. Remove the element.
//5. Propagate up from the next's parent.
void amc::tfunc_Atree_Remove(){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.inl = false;
    Ins(&R, func.ret  , "void", false);
    Ins(&R, func.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R, func.body , "if(!$name_InTreeQ(row)){");
    Ins(&R, func.body , "    return;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "$Cpptype* next = NULL;");
    Ins(&R, func.body , "if(row.$Depth > 1){");
    Ins(&R, func.body , "    next = $name_Balance(row) < 0 ? $name_FirstImpl(row.$Right) : $name_LastImpl(row.$Left);");
    Ins(&R, func.body , "    $Cpptype* leaf = $name_TallerChild(*next);");
    Ins(&R, func.body , "    if(leaf){");
    Ins(&R, func.body , "        $name_Turn(*leaf, *next);");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "$Cpptype* root = row.$Up;");
    Ins(&R, func.body , "$Cpptype* prop = root;//propagate point");
    Ins(&R, func.body , "if(next){");
    Ins(&R, func.body , "    prop = next->$Up == &row ? next : next->$Up;");
    Ins(&R, func.body , "    $name_Disconnect(*next);");
    Ins(&R, func.body , "    $name_Connect(next, row.$Left, true);");
    Ins(&R, func.body , "    $name_Connect(next, row.$Right, false);");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "bool dir = root && root->$Left == &row;");
    Ins(&R, func.body , "$name_Connect(root, next, dir);");
    Ins(&R, func.body , "$Root = prop ? $name_Propagate(*prop) : NULL;");
    Ins(&R, func.body , "row.$Depth = 0;");
    Ins(&R, func.body , "row.$Left = NULL;");
    Ins(&R, func.body , "row.$Right = NULL;");
    Ins(&R, func.body , "row.$Up = ($Cpptype*)-1;");
    Ins(&R, func.body , "$NElem--;");
}


//generate [.,.) range functions.
static void GenFind(strptr name, bool greater){
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
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
    Ins(&R, func.body , "while(result){");
    Ins(&R, func.body , "    left = $CompDir;");
    Ins(&R, func.body , "    $Cpptype* side = left ? result->$Left : result->$Right;");
    Ins(&R, func.body , "    if(side==NULL){");
    Ins(&R, func.body , "        break;");
    Ins(&R, func.body , "    }");
    Ins(&R, func.body , "    result = side;");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "while(result && $CompNext){");
    Ins(&R, func.body , "    result = $Dir(*result);");
    Ins(&R, func.body , "}");
    Ins(&R, func.body , "return result;");
}

//First element that is greater or equal to the given value
void amc::tfunc_Atree_FirstGe(){
    GenFind("FirstGe", true);
}

//First element that is smaller than given value
void amc::tfunc_Atree_LastLt(){
    GenFind("LastLt", false);
}


//Generate cursor related funcs and struct.
//Pretty straightforward since Next is done without extra state.
void amc::tfunc_Atree_curs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField& field = *_db.genfield.p_field;
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

    amc::FFunc& curs_reset = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Reset"));
    curs_reset.inl = true;
    Ins(&R, curs_reset.comment, "cursor points to valid item");
    Ins(&R, curs_reset.ret  , "void", false);
    Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype& $CursArg)", false);
    Ins(&R, curs_reset.body, "curs.row = $name_First($CursArg);");

    amc::FFunc& curs_validq = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.ValidQ"));
    curs_validq.inl = true;
    Ins(&R, curs_validq.comment, "cursor points to valid item");
    Ins(&R, curs_validq.ret  , "bool", false);
    Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
    Ins(&R, curs_validq.body, "return curs.row != NULL;");

    amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Next"));
    curs_next.inl = true;
    Ins(&R, curs_next.comment, "proceed to next item");
    Ins(&R, curs_next.ret  , "void", false);
    Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
    Ins(&R, curs_next.body, "curs.row = $name_Next(*curs.row);");

    amc::FFunc& curs_access = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Access"));
    curs_access.inl = true;
    Ins(&R, curs_access.comment, "item access");
    Ins(&R, curs_access.ret  , "$Cpptype&", false);
    Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
    Ins(&R, curs_access.body, "return *curs.row;");

}
