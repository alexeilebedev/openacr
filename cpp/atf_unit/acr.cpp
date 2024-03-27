// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/acr.cpp
//

#include "include/atf_unit.h"

// no test for -e, -report, -v, -h, -sig, -b, -my
// no test for -pretty, -xref, -rename, -loose, -cmd, -meta

// -----------------------------------------------------------------------------

static void _CheckQuery(const char *file, int line, strptr source, strptr query, strptr extraargs, strptr expect) {
    const char *fname = "temp/x";
    StringToFile(source, fname);
    command::acr acr;
    acr.report = false;
    acr.in = fname;
    acr.pretty = false;
    acr.query = query;
    acr.report = false;
    tempstr cmd = tempstr() << acr_ToCmdline(acr) << " " << extraargs;
    tempstr value(Trimmed(SysEval(cmd,FailokQ(true),1024*100)));
    expect=Trimmed(expect);
    bool ok = expect==value;
    vrfy(ok,
         tempstr()<<file<<":"<<line<<": atf_unit.acr  "
         <<Keyval("success","N")
         <<Keyval("command",cmd)
         <<Keyval("output",value)
         <<Keyval("expected",expect));
}

#define CheckQuery(source,query,extraargs,expect) _CheckQuery(__FILE__,__LINE__,source,query,extraargs,expect)

// -----------------------------------------------------------------------------

// Check selecting a single tuple from file.
void atf_unit::unittest_acr_Read1() {
    CheckQuery("dmmeta.ctype ctype:X\n"
               "dmmeta.ctype ctype:Y\n"
               , "ctype:X"
               , ""
               , "dmmeta.ctype  ctype:X  comment:\"\"\n"
               );
}

// --------------------------------------------------------------------------------

// Check that selecting all tuples from a mixed file
// reorders them in topological order by ctype (i.e. ctype, then field)
void atf_unit::unittest_acr_Read2() {
    CheckQuery("dmmeta.ctype ctype:X\n"
               "dmmeta.field field:X.y  reftype:Val\n"
               "dmmeta.ctype ctype:Y\n"
               , "%"
               , ""
               , "dmmeta.ctype  ctype:X  comment:\"\"\n"
               "dmmeta.ctype  ctype:Y  comment:\"\"\n\n"
               "dmmeta.field  field:X.y  arg:\"\"  reftype:Val  dflt:\"\"  comment:\"\"\n"
               );
}
// --------------------------------------------------------------------------------

// Check that selection on non-pkey attribute works
void atf_unit::unittest_acr_Read3() {
    CheckQuery("dmmeta.field field:X.a  reftype:A\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               "dmmeta.field field:X.b2  reftype:B\n"
               , "dmmeta.field.reftype:B"
               , ""
               , "dmmeta.field  field:X.b  arg:\"\"  reftype:B  dflt:\"\"  comment:\"\"\n"
               "dmmeta.field  field:X.b2  arg:\"\"  reftype:B  dflt:\"\"  comment:\"\"\n"
               );
}

// --------------------------------------------------------------------------------

// Check that -rowid numbers records, but not for relations which are fully sorted
void atf_unit::unittest_acr_Rowid1() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-rowid"
               , "dmmeta.ctype  ctype:X  comment:\"\"\n\n"
               "dmmeta.field  field:X.b  acr.rowid:0       arg:\"\"  reftype:B  dflt:\"\"  comment:\"\"\n"
               "dmmeta.field  field:X.c  acr.rowid:1       arg:\"\"  reftype:C  dflt:\"\"  comment:\"\"\n"
               );
}

// --------------------------------------------------------------------------------

// Check that with -print:N, nothing is shown
void atf_unit::unittest_acr_Read5() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-print:N"
               , "");
}

// -----------------------------------------------------------------------------

// Test re-writing a single file specified with -in
void atf_unit::unittest_acr_Write1() {
    CheckQuery("dmmeta.ns  ns:a\n"
               "dmmeta.ctype ctype:a.B\n"
               , "%"
               , "-write -print:N; cat temp/x"
               , "dmmeta.ns  ns:a  nstype:\"\"  license:\"\"  comment:\"\"\n\n"
               "dmmeta.ctype  ctype:a.B  comment:\"\"");
}

// --------------------------------------------------------------------------------

// Test re-writing a single file back in tree mode
void atf_unit::unittest_acr_Write2() {
    CheckQuery("dmmeta.ns  ns:a\n"
               "dmmeta.ctype ctype:a.B\n"
               , "%"
               , "-write -tree -print:N; cat temp/x"
               , "dmmeta.ns  ns:a  nstype:\"\"  license:\"\"  comment:\"\"\n"
               "  dmmeta.ctype  ctype:a.B  comment:\"\"");
}

// --------------------------------------------------------------------------------

// Test that -insert -trunc removes all existing records of a given type
// upon first insertion
void atf_unit::unittest_acr_Insert1() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-print:N; echo \"dmmeta.field  field:X.d reftype:D\" "
               "| acr -in:temp/x  -insert -trunc -write -print:N -report:N; cat temp/x"
               , "dmmeta.ctype  ctype:X  comment:\"\"\n\n"
               "dmmeta.field  field:X.d  arg:\"\"  reftype:D  dflt:\"\"  comment:\"\"");
}

// --------------------------------------------------------------------------------

// Insert a single record into file, no truncation
void atf_unit::unittest_acr_Insert2() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-print:N; echo \"dmmeta.field  field:X.d reftype:D\" "
               "| acr -in:temp/x  -insert -write -print:N -report:N; cat temp/x"
               , "dmmeta.ctype  ctype:X  comment:\"\"\n\n"
               "dmmeta.field  field:X.b  arg:\"\"  reftype:B  dflt:\"\"  comment:\"\"\n"
               "dmmeta.field  field:X.c  arg:\"\"  reftype:C  dflt:\"\"  comment:\"\"\n"
               "dmmeta.field  field:X.d  arg:\"\"  reftype:D  dflt:\"\"  comment:\"\""
               );
}

// --------------------------------------------------------------------------------

// Check that acr detects bad references.
void atf_unit::unittest_acr_Check1() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-check 2>&1 | grep badrefs"
               , "acr.badrefs  ctype:dmmeta.Ctype  nbad:1\n"
               "acr.badrefs  ctype:dmmeta.Field  nbad:4");
}

// --------------------------------------------------------------------------------

// Check that -unused deselects records that are referred to
void atf_unit::unittest_acr_Unused1() {
    CheckQuery("dmmeta.ns  ns:a\n"
               "dmmeta.ctype ctype:a.B\n"
               , "%"
               , "-unused"
               , "dmmeta.ctype  ctype:a.B  comment:\"\"");
}

// --------------------------------------------------------------------------------

// Check that -fldfunc expansion operates
void atf_unit::unittest_acr_Fldfunc1() {
    CheckQuery("dmmeta.ctype ctype:a.B\n"
               , "%"
               , "-fldfunc"
               , "dmmeta.ctype  ctype:a.B  ns:a  name:B  comment:\"\"");
}

// --------------------------------------------------------------------------------

// Check that selection + -nup selects an appropriate record
// and not another available record
void atf_unit::unittest_acr_Xref1() {
    CheckQuery("dmmeta.ns  ns:a\n"
               "dmmeta.ns  ns:b\n"
               "dmmeta.ctype ctype:a.B\n"
               , "ctype:a.B"
               , "-nup:1"
               , "dmmeta.ns  ns:a  nstype:\"\"  license:\"\"  comment:\"\"\n\n"
               "dmmeta.ctype  ctype:a.B  comment:\"\"");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_acr_Xref2() {
    CheckQuery("dmmeta.ns  ns:a\n"
               "dmmeta.ns  ns:b\n"
               "dmmeta.ctype ctype:a.B\n"
               , "ns:a"
               , "-ndown:1"
               , "dmmeta.ns  ns:a  nstype:\"\"  license:\"\"  comment:\"\"\n\n"
               "dmmeta.ctype  ctype:a.B  comment:\"\"");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_acr_Field1() {
    CheckQuery("dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-field:ctype"
               , "X\n"
               "X\n");
}

// --------------------------------------------------------------------------------

// Construct regx of matching records
// Input order is preserved, and dots are escaped (to allow interoperability with perl)
void atf_unit::unittest_acr_Regx1() {
    CheckQuery("dmmeta.field field:X.b  reftype:B.b\n"
               "dmmeta.field field:X.c  reftype:C.c\n"
               "dmmeta.ctype ctype:Y\n"
               , "field:X.%"
               , "-regxof:reftype"
               , "(B\\.b|C\\.c)");
}

// --------------------------------------------------------------------------------

// Delete a record from a file
void atf_unit::unittest_acr_Del1() {
    CheckQuery("dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "field:X.b"
               , "-del -write -print:N; cat temp/x"
               , "dmmeta.field  field:X.c  arg:\"\"  reftype:C  dflt:\"\"  comment:\"\"");
}

// --------------------------------------------------------------------------------

// Update a non-primary attribute
void atf_unit::unittest_acr_Merge1() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-print:N; echo \"dmmeta.field  field:X.c reftype:CCC\" "
               "| acr -in:temp/x  -merge -write -print:N -report:N; cat temp/x"
               , "dmmeta.ctype  ctype:X  comment:\"\"\n\n"
               "dmmeta.field  field:X.b  arg:\"\"  reftype:B  dflt:\"\"  comment:\"\"\n"
               "dmmeta.field  field:X.c  arg:\"\"  reftype:CCC  dflt:\"\"  comment:\"\"\n"
               );
}

// --------------------------------------------------------------------------------

// Replace a record. Unspecified attributes revert to defaults.
void atf_unit::unittest_acr_Replace1() {
    CheckQuery("dmmeta.ctype  ctype:X\n"
               "dmmeta.field field:X.b  reftype:B\n"
               "dmmeta.field field:X.c  reftype:C\n"
               , "%"
               , "-print:N; echo \"dmmeta.field  field:X.c\" "
               "| acr -in:temp/x  -replace -write -print:N -report:N; cat temp/x"
               , "dmmeta.ctype  ctype:X  comment:\"\"\n\n"
               "dmmeta.field  field:X.b  arg:\"\"  reftype:B  dflt:\"\"  comment:\"\"\n"
               "dmmeta.field  field:X.c  arg:\"\"  reftype:Val  dflt:\"\"  comment:\"\"\n"
               );
}
