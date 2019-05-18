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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/atree.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/atf_amc.h"
#include "include/gen/atf_amc_gen.h"

static bool RootQ(atf_amc::FAvl& node){
    return node.tr_avl_up == NULL;
}


static i32 Depth(atf_amc::FAvl* node){
    i32 result = 0;
    if(node != NULL){
        result = node->tr_avl_depth;
    }
    return result;
}

static void PrintNode(atf_amc::FAvl& node){
    prlog(Keyval("n",node.n) <<
          Keyval("d",node.tr_avl_depth)<<
          Keyval("l",node.tr_avl_left)<<
          Keyval("r",node.tr_avl_right));

}
//Check balance.
static bool BalanceCheck(atf_amc::FAvl* root){
    bool result = true;
    if(root != NULL){
        i32 ldepth = Depth(root->tr_avl_left);
        i32 rdepth = Depth(root->tr_avl_right);
        result &= Abs(ldepth - rdepth) < 2;
        if(!result){
            prlog("Violation on node");
            PrintNode(*root);
        }
        result &= BalanceCheck(root->tr_avl_left);
        result &= BalanceCheck(root->tr_avl_right);
    }

    return result;
}

//Check that stated depth equals actual depth
//DepthCheck(root, root->depth);
static bool DepthCheck(atf_amc::FAvl& node){
    bool result = node.tr_avl_depth == i32_Max(Depth(node.tr_avl_left), Depth(node.tr_avl_right)) + 1;
    if(node.tr_avl_left != NULL){
        result &= DepthCheck(*node.tr_avl_left);
    }
    if(node.tr_avl_right != NULL){
        result &= DepthCheck(*node.tr_avl_right);
    }
    return result;
}

static bool SortCheck(atf_amc::FAvl& node){
    bool result = true;
    if(node.tr_avl_left != NULL){
        result &= node.n >= node.tr_avl_left->n;
        result &= SortCheck(*node.tr_avl_left);
    }
    if(node.tr_avl_right != NULL){
        result &= node.n <= node.tr_avl_right->n;
        result &= SortCheck(*node.tr_avl_right);
    }
    return result;
}

static bool ConnectivityCheck(atf_amc::FAvl& node){
    bool result = true;
    if(node.tr_avl_left != NULL){
        result &= node.tr_avl_left->tr_avl_up == &node;
        result &= ConnectivityCheck(*node.tr_avl_left);
    }
    if(node.tr_avl_right != NULL){
        result &= node.tr_avl_right->tr_avl_up == &node;
        result &= ConnectivityCheck(*node.tr_avl_right);
    }
    return result;
}

static i32 Count(atf_amc::FAvl* node){
    i32 result = node != NULL;
    if(node != NULL){
        result += Count(node->tr_avl_left);
        result += Count(node->tr_avl_right);
    }
    return result;
}


static void InOrderPrint(){
    prlog("Ascending..");
    prlog(Keyval("Root", atf_amc::_db.tr_avl_root)<<
          Keyval("N", atf_amc::_db.tr_avl_n));
    ind_beg(atf_amc::_db_tr_avl_curs, elem, atf_amc::_db){
        prlog(Keyval("n",elem.n) <<
              Keyval("d",elem.tr_avl_depth)<<
              Keyval("l",elem.tr_avl_left)<<
              Keyval("r",elem.tr_avl_right));
    }ind_end;
}


static bool Check(bool success, strptr reason){
    if(!success){
        prlog(Keyval("error", reason));
    }
    return success;
}

static void InvariantCheck(){
    bool result = true;
    result &= Check(Count(atf_amc::_db.tr_avl_root)==atf_amc::_db.tr_avl_n, "Count");
    if(atf_amc::_db.tr_avl_root != NULL){
        result &= Check(ConnectivityCheck(*atf_amc::_db.tr_avl_root), "Connectivity");
        result &= Check(BalanceCheck(atf_amc::_db.tr_avl_root), "Balance");
        result &= Check(SortCheck(*atf_amc::_db.tr_avl_root), "Sort");
        result &= Check(DepthCheck(*atf_amc::_db.tr_avl_root), "Depth");
        result &= Check(RootQ(*atf_amc::_db.tr_avl_root), "Root");
    }
    if(!result){
        InOrderPrint();
        vrfy(false, "Because of errors above.");
    }
}


static atf_amc::FAvl* Add(i32 n, bool print = false){
    atf_amc::FAvl& avl = atf_amc::avl_Alloc();
    avl.n = n;
    //prlog("Inserting:"<<n);
    atf_amc::tr_avl_Insert(avl);

    InvariantCheck();
    if(print){
        InOrderPrint();
    }
    return &avl;
}

static void Remove(atf_amc::FAvl& node){
    atf_amc::tr_avl_Remove(node);
    //InOrderPrint();
    InvariantCheck();
}



static void ReverseOrderPrint(){
    prlog("Descending..");
    atf_amc::FAvl* node = atf_amc::tr_avl_Last();
    while(node){
        prlog(node->n);
        node = atf_amc::tr_avl_Prev(*node);
    }
}


static void RemoveAll(){
    atf_amc::FAvl* node;
    while((node = atf_amc::tr_avl_First())){
        Remove(*node);
    }
}

static void SimpleCases(){
    atf_amc::FAvl* elem[100];
    elem[1] = Add(1);
    elem[2] = Add(2);
    elem[3] = Add(3);
    elem[4] = Add(4);
    elem[5] = Add(5);
    elem[6] = Add(6);
    elem[7] = Add(7);
    ReverseOrderPrint();
    Remove(*elem[1]);
    Remove(*elem[7]);
    Remove(*elem[2]);
    Remove(*elem[6]);
    Remove(*elem[5]);
    Remove(*elem[4]);
    ReverseOrderPrint();
    RemoveAll();

    Add(1);
    Add(2);
    Add(3);
    RemoveAll();
}

static void SpecialCases(){
    prlog("Some tricky cases:");
    Add(98);
    Add(97);
    Add(99);
    Add(97);
    Add(97);
    Add(96);
    RemoveAll();

    Add(45);
    Add(43);
    Add(46);
    Add(44);
    Add(43);
    Add(46);
    Add(46);
    Add(43);
    Add(43);
    Add(43);
    Add(44);
    RemoveAll();

    Add(5);
    Add(1);
    Add(8);
    Add(6);
    Add(10);
    Add(7);
    RemoveAll();

    Add(9);
    Add(8);
    Add(8);
    Add(10);
    Add(9);
    Add(9);
    RemoveAll();

    prlog("Random delete");
    atf_amc::FAvl* elem[100];
    frep_(i,100){
        elem[i] = Add(i);
    }
    Remove(*elem[13]);
    Remove(*elem[49]);
    Remove(*elem[9]);
    Remove(*elem[93]);
    RemoveAll();
}

void atf_amc::amctest_atree_test1(){
    SimpleCases();
    SpecialCases();
    frep_(i,17){
        Add(i32_WeakRandom(100)%2);
    }
    InOrderPrint();
    RemoveAll();


    prlog("All zeros:");
    frep_(i,1000){
        Add(0);
    }
    Add(2);
    Add(3);
    RemoveAll();
    prlog("tight range:");
    frep_(i,1000){
        Add(i32_WeakRandom(100)%3);
    }
    Add(2);
    Add(3);
    RemoveAll();
    InvariantCheck();
    i64 c = get_cycles();
    prlog("All random:");
    frep_(i,10000){
        Add(i32_WeakRandom(100));
    }
    c = get_cycles() - c;
    prlog("Insert  "<< atf_amc::_db.tr_avl_n<<" :"<<c<<" cycles");
    c = get_cycles();
    atf_amc::FAvl* next = tr_avl_First();
    prlog("Iteration:");
    while(next){
        next = tr_avl_Next(*next);
    }
    c = get_cycles() - c;
    prlog("Iterate "<< atf_amc::_db.tr_avl_n<<" :"<<c<<" cycles");
    RemoveAll();
    InvariantCheck();
    prlog("Done Testing Atree");
}


void atf_amc::amctest_atree_test2(){
    atf_amc::FAvl* elem[100];
    elem[0] = Add(0);
    elem[1] = Add(1);
    elem[2] = Add(1);
    elem[3] = Add(1);
    elem[4] = Add(1);
    elem[5] = Add(2);
    elem[6] = Add(4);
    elem[7] = Add(4);
    elem[8] = Add(4);
    elem[9] = Add(0);
    elem[10] = Add(4);
    elem[11] = Add(4);
    elem[12] = Add(4);
    elem[13] = Add(4);
    elem[14] = Add(4);
    elem[15] = Add(4);
    elem[16] = Add(4);
    elem[17] = Add(4);

    //PrintNode(*tr_avl_FirstGe(1));
    vrfy_(1 == tr_avl_FirstGe(1)->n);
    vrfy_(1 == tr_avl_LastLt(2)->n);
    vrfy_(elem[5] == tr_avl_FirstGe(2));
    vrfy_(elem[5] == tr_avl_LastLt(4));
    atf_amc::FAvl* row = tr_avl_FirstGe(1);
    int ones = 0;
    while(row != tr_avl_FirstGe(2)){
        ones++;
        row = tr_avl_Next(*row);
    }
    vrfy_(ones == 4);
}

// -----------------------------------------------------------------------------

static void StringToTree(strptr elems) {
    while (elems.n_elems) {
        Add(ParseI32(Pathcomp(elems,",LL"),0));
        elems = Pathcomp(elems,",LR");
    }
}

// Check that the subset of elements of tree ELEMS
// which are within range [left..right)
// equals to RESULT
static void CheckRange(strptr elems, int left, int right, strptr result, bool eq) {
    atf_amc::avl_RemoveAll();
    StringToTree(elems);
    atf_amc::FAvl *beg = atf_amc::tr_avl_LastLt(left);
    if (!beg) {
        beg = atf_amc::tr_avl_First();
    }
    ListSep ls(",");
    tempstr actual_result;
    while (beg && beg->n < right) {
        if (beg->n >= left) {
            actual_result << ls << beg->n;
        }
        beg=tr_avl_Next(*beg);
    }
    vrfy((actual_result==result)==eq,tempstr()
         <<"atf_amc.range_search_error"
         <<Keyval("tree",elems)
         <<Keyval("left",left)
         <<Keyval("right",right)
         <<Keyval("expected",result)
         <<Keyval("actual",actual_result));
}

void atf_amc::amctest_atree_RangeSearch() {
    CheckRange("0,1,2",0,1,"0",true);
    CheckRange("0,1,2",1,2,"1",true);
    CheckRange("0,1,2",2,3,"2",true);
    CheckRange("0,1,2",0,3,"0,1,2",true);
    CheckRange("0,1,2",0,0,"",true);
    CheckRange("0,1,2",-1,0,"",true);

    CheckRange("",0,1,"",true);
    CheckRange("0",0,1,"0",true);
    CheckRange("0,0",0,1,"0,0",true);
    CheckRange("0,0,0",0,1,"0,0,0",true);
    CheckRange("4,3,2,1",2,5,"2,3,4",true);
    CheckRange("4,3,2,1",1,4,"1,2,3",true);
}
