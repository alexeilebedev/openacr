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
// Source: cpp/atf_comp/acr_compl.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"

void atf_comp::comptest_acr_compl_A01a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline --'");
}

void atf_comp::comptest_acr_compl_A01b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- '");
}

void atf_comp::comptest_acr_compl_A01c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- ' -type:63");
}

void atf_comp::comptest_acr_compl_A02a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR'");
}

void atf_comp::comptest_acr_compl_A02b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR '");
}

void atf_comp::comptest_acr_compl_A02c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR ' -type:63");
}

void atf_comp::comptest_acr_compl_A03a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1'");
}

void atf_comp::comptest_acr_compl_A03b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 '");
}

void atf_comp::comptest_acr_compl_A03c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 ' -type:63");
}

void atf_comp::comptest_acr_compl_A04a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001'");
}

void atf_comp::comptest_acr_compl_A04b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 '");
}

void atf_comp::comptest_acr_compl_A04c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 ' -type:63");
}

void atf_comp::comptest_acr_compl_A05a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y'");
}

void atf_comp::comptest_acr_compl_A05b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y '");
}

void atf_comp::comptest_acr_compl_A05c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y ' -type:63");
}

void atf_comp::comptest_acr_compl_A06a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y -2'");
}

void atf_comp::comptest_acr_compl_A06b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y -2 '");
}

void atf_comp::comptest_acr_compl_A06c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y -2 ' -type:63");
}

void atf_comp::comptest_acr_compl_A07a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y -2 -3'");
}

void atf_comp::comptest_acr_compl_A07b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y -2 -3 '");
}

void atf_comp::comptest_acr_compl_A07c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -- -STR -1 -0.0001 Y -2 -3 ' -type:63");
}

void atf_comp::comptest_acr_compl_Acr01() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr '");
}

void atf_comp::comptest_acr_compl_Acr02() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr -query'");
}

void atf_comp::comptest_acr_compl_Acr03() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr -query:'");
}

void atf_comp::comptest_acr_compl_Acr04() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr -query '");
}

void atf_comp::comptest_acr_compl_Acr05() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr f'");
}

void atf_comp::comptest_acr_compl_Acr06() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr fruit'");
}

void atf_comp::comptest_acr_compl_Acr07() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr rui'");
}

void atf_comp::comptest_acr_compl_Acr08() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr garden.fruit:'");
}

void atf_comp::comptest_acr_compl_Acr09() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr garden.fruit:p'");
}

void atf_comp::comptest_acr_compl_Acr10() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'acr garden.fruit:pe'");
}

void atf_comp::comptest_acr_compl_Acr11() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:- -line:'acr garden.fruit:pe'");
    atf_comp::ProcWrite(proc, "garden.flower  flower:rose");
    atf_comp::ProcWrite(proc, "garden.flower  flower:tulip");
    atf_comp::ProcWrite(proc, "garden.fruit  fruit:apple");
    atf_comp::ProcWrite(proc, "garden.fruit  fruit:plum");
    atf_comp::ProcWrite(proc, "garden.fruit  fruit:peach");
}

void atf_comp::comptest_acr_compl_Acr12() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -schema:- -data:- -line:'acr garden.fruit:pe'");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:''");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo.Smallstr50");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo.cstring");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.Cmdline");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:algo_lib.FDb");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:acr.FDb");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:command.acr");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:dmmeta.Ssimfile");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:garden.Flower");
    atf_comp::ProcWrite(proc, "dmmeta.ctype  ctype:garden.Fruit");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:''");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:acr.FDb.cmdline  arg:command.acr");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:dmmeta.Ssimfile.ssimfile  arg:algo.Smallstr50");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:algo_lib.FDb.cmdline  arg:algo_lib.Cmdline");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:command.acr.query arg:algo.cstring");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:garden.Flower.flower arg:algo.Smallstr50");
    atf_comp::ProcWrite(proc, "dmmeta.field  field:garden.Fruit.fruit arg:algo.Smallstr50");
    atf_comp::ProcWrite(proc, "dmmeta.ssimfile ssimfile:dmmeta.ssimfile  ctype:dmmeta.Ssimfile");
    atf_comp::ProcWrite(proc, "dmmeta.ssimfile ssimfile:garden.flower  ctype:garden.Flower");
    atf_comp::ProcWrite(proc, "dmmeta.ssimfile ssimfile:garden.fruit  ctype:garden.Fruit");
    atf_comp::ProcWrite(proc, "dmmeta.ccmdline  ctype:command.acr  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:''");
    atf_comp::ProcWrite(proc, "garden.flower  flower:rose");
    atf_comp::ProcWrite(proc, "garden.flower  flower:tulip");
    atf_comp::ProcWrite(proc, "garden.fruit  fruit:apple");
    atf_comp::ProcWrite(proc, "garden.fruit  fruit:plum");
    atf_comp::ProcWrite(proc, "garden.fruit  fruit:peach");
}

void atf_comp::comptest_acr_compl_Acr13() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'acr field:command.atf_cmdline.%'");
}

void atf_comp::comptest_acr_compl_BadExe() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -line:'blah'");
}

void atf_comp::comptest_acr_compl_BadOpt() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -blah'");
}

void atf_comp::comptest_acr_compl_BadOptColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -blah:'");
}

void atf_comp::comptest_acr_compl_BadOptColonSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -blah: '");
}

void atf_comp::comptest_acr_compl_BadOptSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -blah '");
}

void atf_comp::comptest_acr_compl_Bare() {
    atf_comp::ProcStart("$bindir/acr_compl");
}

void atf_comp::comptest_acr_compl_DblColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -dbl:'");
}

void atf_comp::comptest_acr_compl_DblColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -dbl:' -type:63");
}

void atf_comp::comptest_acr_compl_DblSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -dbl '");
}

void atf_comp::comptest_acr_compl_DblSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -dbl ' -type:63");
}

void atf_comp::comptest_acr_compl_EnumCtypeColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -cconst:'");
}

void atf_comp::comptest_acr_compl_EnumCtypeColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -cconst:' -type:63");
}

void atf_comp::comptest_acr_compl_EnumCtypeSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -cconst '");
}

void atf_comp::comptest_acr_compl_EnumCtypeSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -cconst ' -type:63");
}

void atf_comp::comptest_acr_compl_EnumFieldColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -fconst:'");
}

void atf_comp::comptest_acr_compl_EnumFieldColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -fconst:' -type:63");
}

void atf_comp::comptest_acr_compl_EnumFieldSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -fconst '");
}

void atf_comp::comptest_acr_compl_EnumFieldSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -fconst ' -type:63");
}

void atf_comp::comptest_acr_compl_FlagColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -flag:'");
}

void atf_comp::comptest_acr_compl_FlagColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -flag:' -type:63");
}

void atf_comp::comptest_acr_compl_FlagSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -flag '");
}

void atf_comp::comptest_acr_compl_FlagSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -flag ' -type:63");
}

void atf_comp::comptest_acr_compl_Install() {
    // here, don't use $bindir!
    // Use the fixed test schema so the captured output is stable as new
    // command:: targets are added to the live tree.
    atf_comp::ProcStart("build/release/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.data.ssim -install");
}

void atf_comp::comptest_acr_compl_NumColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -num:'");
}

void atf_comp::comptest_acr_compl_NumColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -num:' -type:63");
}

void atf_comp::comptest_acr_compl_NumSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -num '");
}

void atf_comp::comptest_acr_compl_NumSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -num ' -type:63");
}

void atf_comp::comptest_acr_compl_OptCumul() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag -verbose -'");
}

void atf_comp::comptest_acr_compl_OptCumulAlias() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag -v -'");
}

void atf_comp::comptest_acr_compl_OptD() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -d'");
}

void atf_comp::comptest_acr_compl_OptDList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -d' -type:63");
}

void atf_comp::comptest_acr_compl_OptH() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -h'");
}

void atf_comp::comptest_acr_compl_OptHList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -h -type:63'");
}

void atf_comp::comptest_acr_compl_OptNonCumul() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag -help -'");
}

void atf_comp::comptest_acr_compl_OptNonCumulAlias() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag -h -'");
}

void atf_comp::comptest_acr_compl_OptPkeyColon() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:'");
}

void atf_comp::comptest_acr_compl_OptPkeyColonFull() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:garden.Fruit'");
}

void atf_comp::comptest_acr_compl_OptPkeyColonFullList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:garden.Fruit' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeyColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeyColonPrefix() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:garden.Fruit'");
}

void atf_comp::comptest_acr_compl_OptPkeyColonPrefixList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:garden.F' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeyColonSubstr() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:rui'");
}

void atf_comp::comptest_acr_compl_OptPkeyColonSubstrList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:rui' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeySpace() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey '");
}

void atf_comp::comptest_acr_compl_OptPkeySpaceFull() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey garden.Fruit'");
}

void atf_comp::comptest_acr_compl_OptPkeySpaceFullList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey garden.Fruit' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeySpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey ' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeySpacePrefix() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey garden.Fruit'");
}

void atf_comp::comptest_acr_compl_OptPkeySpacePrefixList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey garden.Fruit' -type:63");
}

void atf_comp::comptest_acr_compl_OptPkeySpaceSubstr() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey rui'");
}

void atf_comp::comptest_acr_compl_OptPkeySpaceSubstrList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey rui' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxColon() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:'");
}

void atf_comp::comptest_acr_compl_OptRegxColonFull() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:garden.Fruit'");
}

void atf_comp::comptest_acr_compl_OptRegxColonFullList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:garden.Fruit' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxColonPrefix() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:garden.F'");
}

void atf_comp::comptest_acr_compl_OptRegxColonPrefixList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:garden.F' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxColonSubstr() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:rui'");
}

void atf_comp::comptest_acr_compl_OptRegxColonSubstrList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx:rui' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx '");
}

void atf_comp::comptest_acr_compl_OptRegxSpaceFull() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx garden.Fruit'");
}

void atf_comp::comptest_acr_compl_OptRegxSpaceFullList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx garden.Fruit' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx ' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxSpacePrefix() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx garden.F'");
}

void atf_comp::comptest_acr_compl_OptRegxSpacePrefixList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx garden.F' -type:63");
}

void atf_comp::comptest_acr_compl_OptRegxSpaceSubstr() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx rui'");
}

void atf_comp::comptest_acr_compl_OptRegxSpaceSubstrList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dregx rui' -type:63");
}

void atf_comp::comptest_acr_compl_OptSig() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -sig'");
}

void atf_comp::comptest_acr_compl_OptSigList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -sig -type:63'");
}

void atf_comp::comptest_acr_compl_OptV() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -v'");
}

void atf_comp::comptest_acr_compl_OptVList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -v' -type:63");
}

void atf_comp::comptest_acr_compl_R01a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline <'  -type:63");
}

void atf_comp::comptest_acr_compl_R01b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline < '  -type:63");
}

void atf_comp::comptest_acr_compl_R01c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline <abc'  -type:63");
}

void atf_comp::comptest_acr_compl_R01d() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline <abc '  -type:63");
}

void atf_comp::comptest_acr_compl_R02a() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline 2'  -type:63");
}

void atf_comp::comptest_acr_compl_R02b() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline 2>'  -type:63");
}

void atf_comp::comptest_acr_compl_R02c() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline 2>/dev/null'  -type:63");
}

void atf_comp::comptest_acr_compl_R02d() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline 2>/dev/null '  -type:63");
}

void atf_comp::comptest_acr_compl_StrColon() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -str:'");
}

void atf_comp::comptest_acr_compl_StrColonList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -str:' -type:63");
}

void atf_comp::comptest_acr_compl_StrSpace() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -str '");
}

void atf_comp::comptest_acr_compl_StrSpaceList() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -str ' -type:63");
}

void atf_comp::comptest_acr_compl_T01() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline '");
}

void atf_comp::comptest_acr_compl_T02() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: '");
}

void atf_comp::comptest_acr_compl_T03() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: '");
}

void atf_comp::comptest_acr_compl_T04() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: '");
}

void atf_comp::comptest_acr_compl_T05() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: '");
}

void atf_comp::comptest_acr_compl_T06() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: '");
}

void atf_comp::comptest_acr_compl_T07() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: '");
}

void atf_comp::comptest_acr_compl_T08() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag '");
}

void atf_comp::comptest_acr_compl_T09() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag -'");
}

void atf_comp::comptest_acr_compl_T10() {
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -astr: -anum: -adbl: -str: -num: -dbl: -aflag -' -type:63");
}

// valid command line with named option
void atf_comp::comptest_acr_compl_CheckValid() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'atf_cmdline -str:hello'");
}

// unknown option
void atf_comp::comptest_acr_compl_CheckUnknownOpt() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'atf_cmdline -blah'");
}

// unknown command
void atf_comp::comptest_acr_compl_CheckUnknownCmd() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'nonexistent_tool -x'");
}

// batch-mode validation: pipe acr_compl.checkreq rows in via stdin;
// expect one acr_compl.checkerr per failing line, silence on success,
// and a non-zero exit code if any line failed.
void atf_comp::comptest_acr_compl_CheckBatch() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr ssimfile:%'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'nonexistent_tool -x'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:3  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation of a line whose -- ends its named options: every
// word after -- is a positional value of that line, so g++ and -c are not
// checked against gcache's option names. The next line in the batch is
// parsed from scratch and its unknown option is still reported -- an anon
// flag left set from the previous line would route every word of every
// later line down the positional branch, which never reaches the
// unknown-option check, and a whole documentation corpus would validate
// clean after one line carrying --.
void atf_comp::comptest_acr_compl_CheckBatchAnon() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'gcache -- g++ -c x.cpp'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'acr -nosuchflag'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation fed a request row it cannot parse: an unknown attr
// (a version-skewed producer) and a line that is not a checkreq tuple at
// all. A dropped request is indistinguishable from one that validated
// clean, so every unparsed line is reported and fails the run; the
// surrounding requests are still validated, and an empty line carries no
// request and is not an error.
void atf_comp::comptest_acr_compl_CheckBatchBadReq() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'acr %'  zzz:1");
    atf_comp::ProcWrite(proc, "not a checkreq tuple");
    atf_comp::ProcWrite(proc, "");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:3  line:'acr -another_nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation of consecutive lines naming different commands. Each
// line is checked against its own command's option names, so -write, which
// belongs to acr and not to amc, is reported on the amc line even though an
// earlier line used it legally. An option-name index accumulated across the
// batch would hold the union of every command seen so far and accept the
// flag. A bare command name with no argument leaves a completion offer behind
// it; a repeat of that line must be parsed against an empty completion set,
// since the leading offer of the previous line names no option field.
void atf_comp::comptest_acr_compl_CheckBatchCmd() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'amc'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'amc'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:3  line:'acr -write'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:4  line:'amc -write x'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation started from a shell that exported the bash
// completion environment. COMP_LINE names the line bash asked to complete and
// says nothing about the command line this process was given, so a validator
// spawned from such a shell inherits it. Read as the entrypoint selector, it
// discarded the process's own argv: -check_batch was never parsed, fd 0 was
// never read, and the run exited 0, which the caller reads as every queued
// command validated clean. The command line decides what runs; the inherited
// variable does not.
void atf_comp::comptest_acr_compl_CheckBatchCompLine() {
    atf_comp::FProc &proc = atf_comp::ProcStart("env COMP_LINE='acr ' COMP_POINT=4 COMP_TYPE=9 $bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr ssimfile:%'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation asked to read the schema or the data from stdin as
// well: the requests and the ssim tuples would both arrive on fd 0, and the
// loader that runs first consumes the whole stream, leaving the batch with
// zero requests to validate. The combination is rejected before either
// reader opens fd 0, so a batch is never reported clean because the schema
// loader ate its requests.
void atf_comp::comptest_acr_compl_CheckBatchSchemaStdin() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch -schema:- -data:-");
    // The rejection is read to end of output before the request is offered,
    // so the tool is known to have exited and dropped the read end of its
    // stdin pipe by the time the write happens. The request the tool never
    // consumes is what the other two rejection rows write while the tool is
    // still exiting, and this row is the one that makes the outcome of that
    // write the same every run.
    atf_comp::ProcRead(proc, "");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation asked to print the completion installation script as
// well: the `complete` command goes to the same stdout that carries the
// response rows, ahead of them and not a response row, and a consumer reading
// the stream as tuples has no way to tell the difference. The combination is
// rejected, so nothing but responses ever reaches the response stream.
void atf_comp::comptest_acr_compl_CheckBatchInstall() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch -install");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// batch-mode validation asked to check a single line as well: -check and
// -check_batch are two validation modes, only one of them runs, and the
// single-line check would be dropped without a word -- a validation silently
// not performed is what this mode exists to prevent. The combination is
// rejected instead.
void atf_comp::comptest_acr_compl_CheckBatchCheck() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch -check -line:'acr -another_nosuch_flag'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// valid command with boolean flag
void atf_comp::comptest_acr_compl_CheckValidFlag() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'acr % -t'");
}

// valid command with anonymous arg
void atf_comp::comptest_acr_compl_CheckValidAnon() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'acr %'");
}

// valid command with multiple options
void atf_comp::comptest_acr_compl_CheckMultiOpt() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'atf_cmdline -str:hello -num:5 -dbl:1.0 -flag'");
}

// dmmeta.finsertwhen: composite Pkey flag falls through to the existing-pkey
// lookup when its trigger flag is absent
void atf_comp::comptest_acr_compl_InsertWhenLookup() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dvariety:'");
}

// trigger flag present, empty value: insert mode offers the LEFT component
// (flower) values, each suffixed with the / separator
void atf_comp::comptest_acr_compl_InsertWhenLeft() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -create -dvariety:'");
}

// trigger flag present, value contains the separator: insert mode offers
// the RIGHT component (fruit) values, with the typed left half as a prefix
void atf_comp::comptest_acr_compl_InsertWhenRight() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -create -dvariety:rose/'");
}

// trigger flag present, partially-typed left component: insert mode filters
// flowers by prefix
void atf_comp::comptest_acr_compl_InsertWhenLeftPrefix() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -create -dvariety:tu'");
}
