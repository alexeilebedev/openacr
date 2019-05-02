// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr/ed/unittest.cpp -- Create, delete, rename unit test
//

#include "include/acr_ed.h"
#include "include/gen/atfdb_gen.h"
#include "include/gen/atfdb_gen.inl.h"

// -----------------------------------------------------------------------------

void acr_ed::Main_CreateUnittest() {
    prlog("acr_ed.create_unittest  unittest:"<<acr_ed::_db.cmdline.unittest);
    algo_lib::Replscope R;
    Set(R, "$ns", atfdb::Unittest_target_Get(acr_ed::_db.cmdline.unittest));
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

    tempstr test_file = FileToString(Subst(R,"cpp/atf/unit/$ns.cpp"));
    int nline=0;
    ind_beg(Line_curs,line,test_file) {
        nline++;
        (void)line;
    }ind_end;
    Ins(&R, acr_ed::_db.script, "cat >> cpp/atf/unit/$ns.cpp << EOF");
    Ins(&R, acr_ed::_db.script, "// --------------------------------------------------------------------------------");
    Ins(&R, acr_ed::_db.script, "");
    Ins(&R, acr_ed::_db.script, "void atf_unit::unittest_$ns_$Name() {");
    Ins(&R, acr_ed::_db.script, "    TESTCMP(0,0); // test code goes here");
    Ins(&R, acr_ed::_db.script, "}");
    Ins(&R, acr_ed::_db.script, "EOF");
    Ins(&R, acr_ed::_db.script, "amc");
    Set(R, "$prefix", acr_ed::_db.cmdline.e ? "" : "# ");
    Ins(&R, acr_ed::_db.script, tempstr()<<"$prefixemacs -nw +"<<nline+4<<" cpp/atf/unit/$ns.cpp");
    Ins(&R, acr_ed::_db.script, "$prefixabt -install atf_unit && atf_unit $ns.$Name");
}

// -----------------------------------------------------------------------------

// Create a new normalization check
void acr_ed::Main_CreateNormcheck() {
    prlog("acr_ed.create_normcheck"
          <<Keyval("normcheck",acr_ed::_db.cmdline.normcheck));
    acr_ed::_db.out_ssim<<"atfdb.normcheck"
                        <<Keyval("normcheck",acr_ed::_db.cmdline.normcheck)
                        <<Keyval("comment","")
                        <<eol;
}
