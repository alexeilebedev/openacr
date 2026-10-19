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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/acr_ed.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_ed_CreateCtype() {
    atf_comp::ProcStart("$bindir/acr_ed -create -ctype ams.ChildTypeMsg -subset ams.MsgHeader -msgtype:400 -reftype:Base");
}

void atf_comp::comptest_acr_ed_CreateMsg() {
    atf_comp::ProcStart("$bindir/acr_ed -create -ctype ams.AbcMsg -subset ams.MsgHeader");
}

void atf_comp::comptest_acr_ed_CreateSrcfileTarget() {
    atf_comp::ProcStart("$bindir/acr_ed -create -srcfile:cpp/acr_ed/blah.cpp -target:algo_lib");
}

void atf_comp::comptest_acr_ed_CreateSsimfile() {
    atf_comp::ProcStart("$bindir/acr_ed -create -ssimfile dmmeta.xyz");
}

void atf_comp::comptest_acr_ed_CreateSsimfileBadNs() {
    atf_comp::ProcStart("$bindir/acr_ed --create -ssimfile ams.test -subset algo.Smallstr10");
}

void atf_comp::comptest_acr_ed_CreateTarget() {
    atf_comp::ProcStart("$bindir/acr_ed -create -target xyz");
}

// Delete a field of a ssimfile's ctype and a field of an in-memory ctype, and check
// that only the first delete rewrites a ssimfile:
// 1. Delete dmmeta.Ns.license, and check that acr_ed drops the schema row and then
//    rewrites the ssimfile that held the field's values.
// 2. Delete amc.FNs.p_license, and check that acr_ed emits the delete alone.
// A field on an in-memory ctype has no ssimfile that holds its values.
void atf_comp::comptest_acr_ed_DelField() {
    atf_comp::ProcStart("bash -c '$bindir/acr_ed -del -field dmmeta.Ns.license"
                        " && $bindir/acr_ed -del -field amc.FNs.p_license'");
}

// Rename a field with four spellings of the new name, and check which ones acr_ed
// accepts:
// 1. Rename dmmeta.Ns.license to the bare name licence, and check that acr_ed
//    resolves it against the field's own ctype.
// 2. Rename it to field:dmmeta.Ns.licence, and check that acr_ed refuses the query
//    prefix.
// 3. Rename it to dmmeta.Ctype.license, and check that acr_ed refuses the move to a
//    new ctype.
// 4. Rename amc.FNs.p_license to amc.FCtype.p_license, and check that the move goes
//    through.
// The two refused spellings mean something other than what they look like.  After
// the move in step 3 the old rows would keep a column that no field claims.  Two
// in-memory ctypes have no such column, so the move in step 4 goes through.
void atf_comp::comptest_acr_ed_RenameField() {
    atf_comp::ProcStart("bash -c '$bindir/acr_ed -field dmmeta.Ns.license -rename licence"
                        " ; $bindir/acr_ed -field dmmeta.Ns.license -rename field:dmmeta.Ns.licence"
                        " ; $bindir/acr_ed -field dmmeta.Ns.license -rename dmmeta.Ctype.license"
                        " ; $bindir/acr_ed -field amc.FNs.p_license -rename amc.FCtype.p_license'");
}
