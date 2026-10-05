// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/hook.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

// Parameter 1: user-specified callback
// Parameter 2: reference to record whose hook is being called
static void Hook2(algo_lib::Replscope &R, atf_amc::Hooktype &hook) {
    Set(R,"$X","Y");
    hook.value=2;
}

// Parameter 1: user-specified callback
static void Hook1(atf_amc::Hooktype &hook) {
    hook.value=1;
}

static int _i;
static void Hook0() {
    _i=99;
}

void atf_amc::amctest_Hook1() {
    algo_lib::Replscope R;

    atf_amc::Hooktype hooktype;

    // make sure call was not made when hook is not initialized
    callback_Call(hooktype,hooktype);
    vrfy_(hooktype.value==0);
    vrfy_(Subst(R,"$X")=="$X");

    // make sure parameters got bound, and call was made
    callback_Set2(hooktype, R, Hook2);
    callback_Call(hooktype,hooktype);
    vrfy_(hooktype.value==2);
    vrfy_(Subst(R,"$X")=="Y");

    callback_Set1(hooktype, hooktype, Hook1);
    // Call a million times to make sure there are no stack leaks
    for (int i=0; i<1000000; i++) {
        callback_Call(hooktype,hooktype);
    }
    vrfy_(hooktype.value==1);

    // Call zero-argument version.
    // It should work.
    // Call a million times to make sure there are no stack leaks
    callback_Set0(hooktype, Hook0);
    for (int i=0; i<1000000; i++) {
        callback_Call(hooktype,hooktype);
    }
    vrfy_(_i==99);
}

// -----------------------------------------------------------------------------

// Static hooks with argument
// amctest itself uses static hook without argument, so we'll assume it's been
// tested.
void atf_amc::listtype_cd(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_cdl(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_cs(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_csl(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_zd(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_zdl(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_zs(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::listtype_zsl(atf_amc::FListtype &listtype) {
    listtype.seen = true;
}

void atf_amc::amctest_Hook2() {
    // call all static hooks
    // they must have been properly initialized
    ind_beg(atf_amc::_db_listtype_curs,listtype,atf_amc::_db) {
        step_Call(listtype,listtype);
    }ind_end;

    // check that all records were visited
    ind_beg(atf_amc::_db_listtype_curs,listtype,atf_amc::_db) {
        vrfy_(listtype.seen);
    }ind_end;
}
