// Copyright (C) 2026 AlgoRND
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
// Target: atf_comp (exe) -- Algo Test Framework - Component test execution
// Exceptions: yes
// Source: cpp/atf_comp/acr.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_BadInsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will not\"");
}

void atf_comp::comptest_acr_BadNs() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.ctype ctype:x.Y");
}

void atf_comp::comptest_acr_BadPkey() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.ctype");
}

void atf_comp::comptest_acr_BadReftype() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -check");
    atf_comp::ProcWrite(proc, "dmmeta.field field:command.acr_ed.blah arg:bool");
}

void atf_comp::comptest_acr_CascDel() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:ns1 -del");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ns1.c comment:\"Will be deleted as well\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ns1.c.d arg:i32 reftype:Val comment:\"Will be deleted as well\"");
}

void atf_comp::comptest_acr_CascDel2() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"Will be inserted and deleted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:ns1.c comment:\"Will be deleted as well\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:ns1.c.d arg:i32 reftype:Val comment:\"Will be deleted as well\"");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns  ns:ns1");
}

void atf_comp::comptest_acr_CascDel3() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -replace");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.field  field:acr.FBltin.base");
}

void atf_comp::comptest_acr_DelField() {
    atf_comp::ProcStart("$bindir/acr field:dmmeta.Ns.license -del");
}

void atf_comp::comptest_acr_DelRecord() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- target:samp -del");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:samp  nstype:exe  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.target  target:samp  license:GPL  compat:%-%g%.%-%");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:samp.algo_lib  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/1.cpp          comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/2.cpp      comment:\"\"");
}

void atf_comp::comptest_acr_DeleteReinsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns  ns:amc");
    atf_comp::ProcWrite(proc, "acr.insert dmmeta.ns  ns:amc  nstype:exe  license:GPL  comment:\"Algo Cross-Reference - ssimfile database & update tool\"");
}

void atf_comp::comptest_acr_Fields() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns -field:nstype -field:comment");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe       comment:\"comment 1\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    comment:\"comment 2\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol  comment:\"comment 3\"");
}

void atf_comp::comptest_acr_FieldsComma() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns -field:nstype,comment");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe       comment:\"comment 1\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    comment:\"comment 2\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol  comment:\"comment 3\"");
}

void atf_comp::comptest_acr_GitTrigger1() {
    atf_comp::ProcStart("$bindir/acr gitfile:cpp/acr/git.cpp -del -g");
}

void atf_comp::comptest_acr_Insert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will not\"");
}

void atf_comp::comptest_acr_InsertDelete() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert");
    atf_comp::ProcWrite(proc, "acr.insert dmmeta.ns ns:ttt");
    atf_comp::ProcWrite(proc, "acr.delete dmmeta.ns ns:ttt");
}

void atf_comp::comptest_acr_Merge() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -merge ns:acr");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr        nstype:lib         comment:\"nstype attribute will be updated\"");
}

void atf_comp::comptest_acr_Meta1() {
    atf_comp::ProcStart("$bindir/acr anonfld -meta -report:N");
}

void atf_comp::comptest_acr_Meta2() {
    atf_comp::ProcStart("$bindir/acr ftuple -meta");
}

void atf_comp::comptest_acr_Meta3() {
    atf_comp::ProcStart("$bindir/acr ftuple -meta -cmt");
}

void atf_comp::comptest_acr_NullTrunc() {
    atf_comp::ProcStart("$bindir/acr ns -report:N | $bindir/acr -trunc -insert");
}

void atf_comp::comptest_acr_QueryCtype() {
    atf_comp::ProcStart("$bindir/acr dmmeta.ctype:dmmeta.Ctype");
}

void atf_comp::comptest_acr_RenameCollision() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:ns1 -rename ns2");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"\"");
}

void atf_comp::comptest_acr_RenameField() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- field:dmmeta.Ns.nstype -rename dmmeta.Ns.blah");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.Ns.nstype  arg:dmmeta.Nstype  reftype:Pkey  dflt:\"\"  comment:\"Namespace type\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"\"");
}

void atf_comp::comptest_acr_RenameRecord() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- target:samp -rename:xyz");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:samp  nstype:exe  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.target  target:samp  compat:%-%g%.%-%");
    atf_comp::ProcWrite(proc, "dev.targdep  targdep:samp.algo_lib  comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/1.cpp          comment:\"\"");
    atf_comp::ProcWrite(proc, "dev.targsrc  targsrc:samp/cpp/samp/2.cpp      comment:\"\"");
}

void atf_comp::comptest_acr_Replace() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -replace ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be replaced\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will remain\"");
}

void atf_comp::comptest_acr_Select() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns:nsi%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"This will NOT be selected\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:nsi1        nstype:exe      license:GPL   comment:\"This will be selected\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:nsi2      nstype:ssimdb    license:GPL  comment:\"This will be selected\"");
}

void atf_comp::comptest_acr_SelectStdin() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -sel");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr");
}

void atf_comp::comptest_acr_SelectTree() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ctype:command.acr -t");
    atf_comp::ProcWrite(proc, "dev.license  license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.nstype  nstype:protocol  comment:\"Protocol (not a target)\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:command  nstype:protocol  license:GPL  comment:\"Command line descriptions\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.acr  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.query  arg:algo.cstring  reftype:Val  dflt:'\"\"'  comment:\"Regx to match record\"");
    atf_comp::ProcWrite(proc, "dmmeta.anonfld  field:command.acr.query  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.schema    arg:algo.cstring  reftype:Val  dflt:'\"data\"'  comment:\"Directory for initializing acr meta-data\"");
    atf_comp::ProcWrite(proc, "dmmeta.floadtuples  field:command.acr.schema    comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctypelen  ctype:command.acr  len:192  alignment:8  padbytes:21");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:acr.FDb.cmdline  arg:command.acr  reftype:Val  dflt:\"\"  comment:\"command line\"");
    atf_comp::ProcWrite(proc, "dmmeta.fcmdline  field:acr.FDb.cmdline  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:\"\"");
}

void atf_comp::comptest_acr_TooManyArgs() {
    atf_comp::ProcStart("$bindir/acr x y");
}

void atf_comp::comptest_acr_Trunc() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert -trunc -report:N");
    atf_comp::ProcWrite(proc, "dev.timefmt timefmt:xyz");
}

void atf_comp::comptest_acr_UpdateBad() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -update ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"Namespace doesn't exist, but not a failure\"");
}

void atf_comp::comptest_acr_UpdateGood() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -update ns:acr");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr        nstype:lib      license:zzz   comment:\"Namespace exists, attribute will be updated\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:acr        nstype:ssimdb   comment:\"Namespace exists, attribute will be updated\"");
}

void atf_comp::comptest_acr_Where() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -in:- ns -where:nstype:protocol");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:GPL   comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns2      nstype:ssimdb    license:GPL  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns3         nstype:protocol    license:GPL   comment:\"\"");
}
