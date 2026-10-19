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
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -h' -type:63");
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
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey:garden.F'");
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
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey garden.F'");
}

void atf_comp::comptest_acr_compl_OptPkeySpacePrefixList() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dpkey garden.F' -type:63");
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
    atf_comp::ProcStart("$bindir/acr_compl -line:'atf_cmdline -sig' -type:63");
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

// Validate `atf_cmdline -str:hello`, a command line with a named option, with
// acr_compl -check, and check that it passes with exit 0.
void atf_comp::comptest_acr_compl_CheckValid() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'atf_cmdline -str:hello'");
}

// Validate `atf_cmdline -blah` with acr_compl -check, and check that it reports the
// unknown option and exits 1.
void atf_comp::comptest_acr_compl_CheckUnknownOpt() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'atf_cmdline -blah'");
}

// Validate `nonexistent_tool -x` with acr_compl -check, and check that it reports
// the unknown command and exits 1.
void atf_comp::comptest_acr_compl_CheckUnknownCmd() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'nonexistent_tool -x'");
}

// Pipe three acr_compl.checkreq rows into acr_compl -check_batch: a valid line, an
// unknown command and an unknown option.  Check that the batch prints one
// acr_compl.checkerr per failing line, stays silent on the valid line, and exits
// nonzero.
void atf_comp::comptest_acr_compl_CheckBatch() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr ssimfile:%'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'nonexistent_tool -x'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:3  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// Validate in batch a line whose -- ends its named options, then a line with an
// unknown option, and check that the batch reports the second line alone:
// 1. Validate `gcache -- g++ -c x.cpp`, and check that g++ and -c pass as
//    positional values.
// 2. Validate `acr -nosuchflag`, and check that the batch still reports its
//    unknown option.
// Every word after -- is a positional value of its line, so acr_compl leaves g++
// and -c unchecked against gcache's option names.  The batch parses each line from
// scratch.  An anon flag left set from the previous line would route every word of
// every later line down the positional branch, which never reaches the
// unknown-option check.  A whole documentation corpus would then validate clean
// after one line that carries --.
void atf_comp::comptest_acr_compl_CheckBatchAnon() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'gcache -- g++ -c x.cpp'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'acr -nosuchflag'");
    atf_comp::ProcWriteEof(proc);
}

// Feed acr_compl -check_batch request rows it fails to parse among valid ones, and
// check that the batch reports each one and fails the run:
// 1. Send a request with an unknown option, and check that the batch reports it.
// 2. Send a request with the unknown attr zzz, as a version-skewed producer would,
//    and a line that is no checkreq tuple at all.  Check that the batch reports
//    each as acr_compl.badreq.
// 3. Send an empty line, and check that the batch reads it as no request and no
//    error.
// 4. Send another request with an unknown option, and check that the batch still
//    validates it, reports it, and exits 1.
// A dropped request would look the same as one that validated clean.
void atf_comp::comptest_acr_compl_CheckBatchBadReq() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'acr %'  zzz:1");
    atf_comp::ProcWrite(proc, "not a checkreq tuple");
    atf_comp::ProcWrite(proc, "");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:3  line:'acr -another_nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// Validate in batch consecutive lines that name different commands, and check that
// acr_compl checks each line against its own command's option names:
// 1. Validate the bare command amc twice, and check that both lines pass.
// 2. Validate `acr -write`, and check that it passes.
// 3. Validate `amc -write x`, and check that the batch reports -write as an
//    unknown option of amc, though an earlier line used it legally.
// -write belongs to acr and is missing from amc.  An option-name index accumulated
// across the batch would hold the union of every command seen so far and accept
// the flag.  A bare command name with no argument leaves a completion offer behind
// it.  The repeat of that line must parse against an empty completion set, because
// the leading offer of the previous line names no option field.
void atf_comp::comptest_acr_compl_CheckBatchCmd() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'amc'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'amc'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:3  line:'acr -write'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:4  line:'amc -write x'");
    atf_comp::ProcWriteEof(proc);
}

// Export the bash completion variables COMP_LINE, COMP_POINT and COMP_TYPE, then
// run acr_compl -check_batch with a valid and an invalid request.  Check that the
// batch reports the invalid request and exits 1.
// COMP_LINE names the line bash asked to complete, a line unrelated to the command
// line this process was given.  A validator spawned from a shell that exported it
// inherits it.  Read as the entrypoint selector, it would discard the process's own
// argv.  The process would then leave -check_batch unparsed and fd 0 unread, and
// exit 0, which the caller reads as every queued command validated clean.  The
// command line decides what runs.
void atf_comp::comptest_acr_compl_CheckBatchCompLine() {
    atf_comp::SetEnv(algo_lib::dev_envvar_COMP_LINE, "acr ");
    atf_comp::SetEnv(algo_lib::dev_envvar_COMP_POINT, "4");
    atf_comp::SetEnv(algo_lib::dev_envvar_COMP_TYPE, "9");
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr ssimfile:%'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:2  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
    atf_comp::ProcWait(proc, 1);
}

// Run acr_compl -check_batch with -schema:- and -data:-, and check that acr_compl
// rejects the combination before either reader opens fd 0.
// The requests and the ssim tuples would both arrive on fd 0.  The loader that runs
// first would consume the whole stream and leave the batch with zero requests to
// validate.  The rejection keeps a batch from passing clean because the schema
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

// Run acr_compl -check_batch with -install, and check that acr_compl rejects the
// combination, so the response stream carries responses alone.
// The installation script's `complete` command would go to the same stdout as the
// response rows, ahead of them.  A consumer reading the stream as tuples would have
// no way to tell the command from a response row.
void atf_comp::comptest_acr_compl_CheckBatchInstall() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch -install");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// Run acr_compl -check_batch with -check and a line, and check that acr_compl
// rejects the combination.
// -check and -check_batch are two validation modes, of which only one runs.  The
// single-line check would drop without a word.  A validation silently skipped is
// what this mode exists to prevent.
void atf_comp::comptest_acr_compl_CheckBatchCheck() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr_compl -check_batch -check -line:'acr -another_nosuch_flag'");
    atf_comp::ProcWrite(proc, "acr_compl.checkreq  id:1  line:'acr -nosuch_flag'");
    atf_comp::ProcWriteEof(proc);
}

// Validate `acr % -t`, a command line with a boolean flag, with acr_compl -check,
// and check that it passes with exit 0.
void atf_comp::comptest_acr_compl_CheckValidFlag() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'acr % -t'");
}

// Validate `acr %`, a command line with an anonymous arg, with acr_compl -check, and
// check that it passes with exit 0.
void atf_comp::comptest_acr_compl_CheckValidAnon() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'acr %'");
}

// Validate `atf_cmdline -str:hello -num:5 -dbl:1.0 -flag`, a command line with
// several options, with acr_compl -check, and check that it passes with exit 0.
void atf_comp::comptest_acr_compl_CheckMultiOpt() {
    atf_comp::ProcStart("$bindir/acr_compl -check -line:'atf_cmdline -str:hello -num:5 -dbl:1.0 -flag'");
}

// Complete -dvariety: without -create, the trigger flag that its dmmeta.finsertwhen
// row names, and check that the composite Pkey flag falls through to the lookup of
// existing pkeys.
void atf_comp::comptest_acr_compl_InsertWhenLookup() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -dvariety:'");
}

// Complete -dvariety: with the trigger flag -create and an empty value, and check
// that insert mode offers the LEFT component values (flowers), each suffixed with
// the / separator.
void atf_comp::comptest_acr_compl_InsertWhenLeft() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -create -dvariety:'");
}

// Complete -dvariety:rose/, a value that holds the separator, with the trigger flag
// -create.  Check that insert mode offers the RIGHT component values (fruits), with
// the typed left half as a prefix.
void atf_comp::comptest_acr_compl_InsertWhenRight() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -create -dvariety:rose/'");
}

// Complete -dvariety:tu, a partly typed left component, with the trigger flag
// -create, and check that insert mode filters the flowers by the typed prefix.
void atf_comp::comptest_acr_compl_InsertWhenLeftPrefix() {
    atf_comp::ProcStart("$bindir/acr_compl -schema:test/acr_compl.schema.ssim -data:test/acr_compl.schema.ssim -line:'atf_cmdline -create -dvariety:tu'");
}

// Make the directories rose, rosemary and 1511-userproc-pidns and the file roseleaf
// under bed/, and complete -dflower:ro.  Check that the typed prefix completes to a
// flower row and to a directory under bed/ alike, and that acr_compl offers rose,
// which is both, once.  The dmmeta.fcompldir row of -dflower names the directory
// bed.
void atf_comp::comptest_acr_compl_DirPrefix() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p bed/rose bed/rosemary bed/1511-userproc-pidns && touch bed/roseleaf && $$OLDPWD/$bindir/acr_compl -schema:$$OLDPWD/test/acr_compl.schema.ssim -data:$$OLDPWD/test/acr_compl.schema.ssim -line:\"atf_cmdline -dflower:ro\" -type:63'");
}

// Make the directories rose, 1511-userproc-pidns and the hidden .pidns under bed/,
// and complete -dflower:pid%ns.  Check that the directory name that the SQL pattern
// matches only in the middle completes, because no name starts with the value, and
// that the hidden directory stays out.
void atf_comp::comptest_acr_compl_DirSubstr() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir -p bed/rose bed/1511-userproc-pidns bed/.pidns && $$OLDPWD/$bindir/acr_compl -schema:$$OLDPWD/test/acr_compl.schema.ssim -data:$$OLDPWD/test/acr_compl.schema.ssim -line:\"atf_cmdline -dflower:pid%ns\"'");
}

// Complete -dflower:ro in a tempdir that lacks the directory bed/, and check that
// the field completes from its table's keys alone.
void atf_comp::comptest_acr_compl_DirMissing() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && $$OLDPWD/$bindir/acr_compl -schema:$$OLDPWD/test/acr_compl.schema.ssim -data:$$OLDPWD/test/acr_compl.schema.ssim -line:\"atf_cmdline -dflower:ro\"'");
}
