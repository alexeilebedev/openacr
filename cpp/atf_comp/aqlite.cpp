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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/aqlite.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_aqlite_CompileOptions() {
    atf_comp::ProcStart("$bindir/aqlite -in:- \"PRAGMA compile_options;\"");
}

void atf_comp::comptest_aqlite_Constraints() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/aqlite -in:- \"select count(*) as or_test from field where ns = 'dmmeta' OR arg like 'algo.cstr%'; select count(*) as and_test from field where ns = 'dmmeta' AND reftype = 'Val'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:dmmeta.TestConstraint     comment:\"Test ctype for constraint combinations\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestConstraint.id    arg:i32          reftype:Val   dflt:\"\"  comment:\"ID field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestConstraint.name  arg:algo.cstring reftype:Val   dflt:\"\"  comment:\"Name\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestConstraint.flag  arg:i32          reftype:Val   dflt:\"0\" comment:\"Flag\"");
}

void atf_comp::comptest_aqlite_ErrorHandling() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/aqlite -in:- \"select count(*) from field where nonexistent_field = 'test'\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.ErrorTest.id  arg:i32  reftype:Val  dflt:\"\"  comment:\"Error test field\"");
}

void atf_comp::comptest_aqlite_Joins() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/aqlite -in:- \"select count(*) as basic_join from field f join ctype c using (ctype) where f.ns = 'dmmeta'; select count(*) as self_join from field f1 join field f2 on f1.ctype = f2.ctype where f1.ns = 'dmmeta' and f1.field <> f2.field\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:dmmeta.TestJoin    comment:\"Test ctype for JOIN operations\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestJoin.id       arg:i32          reftype:Val   dflt:\"\"   comment:\"Primary key\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestJoin.name     arg:algo.cstring reftype:Val   dflt:\"\"   comment:\"Name field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestJoin.value    arg:i32          reftype:Val   dflt:\"0\"  comment:\"Value field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.TestJoin.extra    arg:algo.cstring reftype:Val   dflt:\"\"   comment:\"Extra field\"");
}

void atf_comp::comptest_aqlite_Number() {
    atf_comp::ProcStart("$bindir/aqlite \"select * from field where dflt = 0 and ns = 'dev'\"");
}

void atf_comp::comptest_aqlite_PatternMatching() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/aqlite -in:- \"select count(*) as cstring_fields from field where arg like '%cstring%'; select count(*) as i32_fields from field where arg like '%i32%'; select count(*) as comment_with_test from field where comment like '%test%'\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:dmmeta.PatternTest    comment:\"Test ctype for pattern matching\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.PatternTest.id      arg:i32          reftype:Val   dflt:\"\"      comment:\"Test identifier\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.PatternTest.desc    arg:algo.cstring reftype:Val   dflt:\"\"      comment:\"Test description field\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.PatternTest.flags   arg:i32          reftype:Val   dflt:\"0\"     comment:\"Test flags value\"");
}

void atf_comp::comptest_aqlite_Performance() {
    atf_comp::ProcStart("$bindir/aqlite \"select distinct 'test' from field join ctype using(ctype)\"");
}

void atf_comp::comptest_aqlite_Smoke() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/aqlite -in:- \"select rowid,* from ctype; select rowid,* from field\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:i16                  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:i32                  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:abt.FArch.msghdr      arg:dev.Arch        reftype:Base    dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:abt.FCfg.msghdr       arg:dev.Cfg         reftype:Base    dflt:\"\"  comment:\"\"");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:abt.FCompiler.msghdr  arg:dev.Compiler    reftype:Base    dflt:\"\"  comment:\"\"");
}
