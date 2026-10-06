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

// A field delete drops the schema row and then rewrites the ssimfile that held
// the field's values.  A field on an in-memory ctype has no such file, so the
// second invocation emits the delete alone.
void atf_comp::comptest_acr_ed_DelField() {
    atf_comp::ProcStart("bash -c '$bindir/acr_ed -del -field dmmeta.Ns.license"
                        " && $bindir/acr_ed -del -field amc.FNs.p_license'");
}

// A field rename resolves a bare new name against the field's own ctype, and
// refuses the two spellings that cannot mean what they look like: a query
// prefix, and a new ctype whose old rows would keep a column no field claims.
// Between two in-memory ctypes there is no such column, so the move goes
// through.
void atf_comp::comptest_acr_ed_RenameField() {
    atf_comp::ProcStart("bash -c '$bindir/acr_ed -field dmmeta.Ns.license -rename licence"
                        " ; $bindir/acr_ed -field dmmeta.Ns.license -rename field:dmmeta.Ns.licence"
                        " ; $bindir/acr_ed -field dmmeta.Ns.license -rename dmmeta.Ctype.license"
                        " ; $bindir/acr_ed -field amc.FNs.p_license -rename amc.FCtype.p_license'");
}
