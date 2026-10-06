// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- Script generator for common dev tasks
// Exceptions: yes
// Source: cpp/acr_ed/unittest.cpp -- Create, delete, rename unit test
//

#include "include/acr_ed.h"
#include "include/gen/atfdb_gen.h"
#include "include/gen/atfdb_gen.inl.h"

// -----------------------------------------------------------------------------

void acr_ed::edaction_Create_Unittest() {
    prlog("acr_ed.create_unittest  unittest:"<<acr_ed::_db.cmdline.unittest);
    algo_lib::Replscope R;
    Set(R, "$ns", atfdb::Unittest_ns_Get(acr_ed::_db.cmdline.unittest));
    Set(R, "$test", acr_ed::_db.cmdline.unittest);
    Set(R, "$Name", atfdb::Unittest_testname_Get(acr_ed::_db.cmdline.unittest));

    vrfy(acr_ed::ind_ns_Find(Subst(R,"$ns"))
         ,tempstr() << "acr_ed.create_unittest"
         <<Keyval("unittest",acr_ed::_db.cmdline.unittest)
         <<Keyval("perf","N")
         <<Keyval("comment","Format is <ns>.<testname>"));

    acr_ed::_db.out_ssim<<"atfdb.unittest"
                        <<Keyval("unittest",Subst(R,"$test"))
                        <<Keyval("comment","")
                        <<eol;

    Ins(&R, acr_ed::_db.script, "cat >> cpp/atf_unit/$ns.cpp << EOF");
    Ins(&R, acr_ed::_db.script, "// --------------------------------------------------------------------------------");
    Ins(&R, acr_ed::_db.script, "");
    Ins(&R, acr_ed::_db.script, "void atf_unit::unittest_$ns_$Name() {");
    Ins(&R, acr_ed::_db.script, "    TESTCMP(0,0); // test code goes here");
    Ins(&R, acr_ed::_db.script, "}");
    Ins(&R, acr_ed::_db.script, "EOF");
    Ins(&R, acr_ed::_db.script, "amc");
    ScriptEditFile(R,"cpp/atf_unit/$ns.cpp");
    Ins(&R, acr_ed::_db.script, "$prefixabt -install atf_unit && atf_unit $ns.$Name");
}

// -----------------------------------------------------------------------------

// Create a new normalization check
void acr_ed::edaction_Create_Citest() {
    prlog("acr_ed.create_citest"
          <<Keyval("citest",acr_ed::_db.cmdline.citest));
    atfdb::Citest citest;
    citest.citest = acr_ed::_db.cmdline.citest;
    citest.cijob = atfdb_cijob_normalize;
    citest.comment.value = acr_ed::_db.cmdline.comment;
    acr_ed::_db.out_ssim<<citest<<eol;
}

// -----------------------------------------------------------------------------
