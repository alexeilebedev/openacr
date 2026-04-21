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
// Header: include/atf_comp.h
//

#include "include/gen/atf_comp_gen.h"
#include "include/gen/atf_comp_gen.inl.h"

namespace atf_comp { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_BadInsert(); // gstatic/atfdb.comptest:acr.BadInsert
    // void comptest_acr_BadNs(); // gstatic/atfdb.comptest:acr.BadNs
    // void comptest_acr_BadPkey(); // gstatic/atfdb.comptest:acr.BadPkey
    // void comptest_acr_BadReftype(); // gstatic/atfdb.comptest:acr.BadReftype
    // void comptest_acr_CascDel(); // gstatic/atfdb.comptest:acr.CascDel
    // void comptest_acr_CascDel2(); // gstatic/atfdb.comptest:acr.CascDel2
    // void comptest_acr_CascDel3(); // gstatic/atfdb.comptest:acr.CascDel3
    // void comptest_acr_DelField(); // gstatic/atfdb.comptest:acr.DelField
    // void comptest_acr_DelRecord(); // gstatic/atfdb.comptest:acr.DelRecord
    // void comptest_acr_DeleteReinsert(); // gstatic/atfdb.comptest:acr.DeleteReinsert
    // void comptest_acr_Fields(); // gstatic/atfdb.comptest:acr.Fields
    // void comptest_acr_FieldsComma(); // gstatic/atfdb.comptest:acr.FieldsComma
    // void comptest_acr_GitTrigger1(); // gstatic/atfdb.comptest:acr.GitTrigger1
    // void comptest_acr_Insert(); // gstatic/atfdb.comptest:acr.Insert
    // void comptest_acr_InsertDelete(); // gstatic/atfdb.comptest:acr.InsertDelete
    // void comptest_acr_Merge(); // gstatic/atfdb.comptest:acr.Merge
    // void comptest_acr_Meta1(); // gstatic/atfdb.comptest:acr.Meta1
    // void comptest_acr_Meta2(); // gstatic/atfdb.comptest:acr.Meta2
    // void comptest_acr_Meta3(); // gstatic/atfdb.comptest:acr.Meta3
    // void comptest_acr_NullTrunc(); // gstatic/atfdb.comptest:acr.NullTrunc
    // void comptest_acr_QueryCtype(); // gstatic/atfdb.comptest:acr.QueryCtype
    // void comptest_acr_RenameCollision(); // gstatic/atfdb.comptest:acr.RenameCollision
    // void comptest_acr_RenameField(); // gstatic/atfdb.comptest:acr.RenameField
    // void comptest_acr_RenameRecord(); // gstatic/atfdb.comptest:acr.RenameRecord
    // void comptest_acr_Replace(); // gstatic/atfdb.comptest:acr.Replace
    // void comptest_acr_Select(); // gstatic/atfdb.comptest:acr.Select
    // void comptest_acr_SelectStdin(); // gstatic/atfdb.comptest:acr.SelectStdin
    // void comptest_acr_SelectTree(); // gstatic/atfdb.comptest:acr.SelectTree
    // void comptest_acr_TooManyArgs(); // gstatic/atfdb.comptest:acr.TooManyArgs
    // void comptest_acr_Trunc(); // gstatic/atfdb.comptest:acr.Trunc
    // void comptest_acr_UpdateBad(); // gstatic/atfdb.comptest:acr.UpdateBad
    // void comptest_acr_UpdateGood(); // gstatic/atfdb.comptest:acr.UpdateGood
    // void comptest_acr_Where(); // gstatic/atfdb.comptest:acr.Where

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_compl.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_compl_A01a(); // gstatic/atfdb.comptest:acr_compl.A01a
    // void comptest_acr_compl_A01b(); // gstatic/atfdb.comptest:acr_compl.A01b
    // void comptest_acr_compl_A01c(); // gstatic/atfdb.comptest:acr_compl.A01c
    // void comptest_acr_compl_A02a(); // gstatic/atfdb.comptest:acr_compl.A02a
    // void comptest_acr_compl_A02b(); // gstatic/atfdb.comptest:acr_compl.A02b
    // void comptest_acr_compl_A02c(); // gstatic/atfdb.comptest:acr_compl.A02c
    // void comptest_acr_compl_A03a(); // gstatic/atfdb.comptest:acr_compl.A03a
    // void comptest_acr_compl_A03b(); // gstatic/atfdb.comptest:acr_compl.A03b
    // void comptest_acr_compl_A03c(); // gstatic/atfdb.comptest:acr_compl.A03c
    // void comptest_acr_compl_A04a(); // gstatic/atfdb.comptest:acr_compl.A04a
    // void comptest_acr_compl_A04b(); // gstatic/atfdb.comptest:acr_compl.A04b
    // void comptest_acr_compl_A04c(); // gstatic/atfdb.comptest:acr_compl.A04c
    // void comptest_acr_compl_A05a(); // gstatic/atfdb.comptest:acr_compl.A05a
    // void comptest_acr_compl_A05b(); // gstatic/atfdb.comptest:acr_compl.A05b
    // void comptest_acr_compl_A05c(); // gstatic/atfdb.comptest:acr_compl.A05c
    // void comptest_acr_compl_A06a(); // gstatic/atfdb.comptest:acr_compl.A06a
    // void comptest_acr_compl_A06b(); // gstatic/atfdb.comptest:acr_compl.A06b
    // void comptest_acr_compl_A06c(); // gstatic/atfdb.comptest:acr_compl.A06c
    // void comptest_acr_compl_A07a(); // gstatic/atfdb.comptest:acr_compl.A07a
    // void comptest_acr_compl_A07b(); // gstatic/atfdb.comptest:acr_compl.A07b
    // void comptest_acr_compl_A07c(); // gstatic/atfdb.comptest:acr_compl.A07c
    // void comptest_acr_compl_Acr01(); // gstatic/atfdb.comptest:acr_compl.Acr01
    // void comptest_acr_compl_Acr02(); // gstatic/atfdb.comptest:acr_compl.Acr02
    // void comptest_acr_compl_Acr03(); // gstatic/atfdb.comptest:acr_compl.Acr03
    // void comptest_acr_compl_Acr04(); // gstatic/atfdb.comptest:acr_compl.Acr04
    // void comptest_acr_compl_Acr05(); // gstatic/atfdb.comptest:acr_compl.Acr05
    // void comptest_acr_compl_Acr06(); // gstatic/atfdb.comptest:acr_compl.Acr06
    // void comptest_acr_compl_Acr07(); // gstatic/atfdb.comptest:acr_compl.Acr07
    // void comptest_acr_compl_Acr08(); // gstatic/atfdb.comptest:acr_compl.Acr08
    // void comptest_acr_compl_Acr09(); // gstatic/atfdb.comptest:acr_compl.Acr09
    // void comptest_acr_compl_Acr10(); // gstatic/atfdb.comptest:acr_compl.Acr10
    // void comptest_acr_compl_Acr11(); // gstatic/atfdb.comptest:acr_compl.Acr11
    // void comptest_acr_compl_Acr12(); // gstatic/atfdb.comptest:acr_compl.Acr12
    // void comptest_acr_compl_Acr13(); // gstatic/atfdb.comptest:acr_compl.Acr13
    // void comptest_acr_compl_BadExe(); // gstatic/atfdb.comptest:acr_compl.BadExe
    // void comptest_acr_compl_BadOpt(); // gstatic/atfdb.comptest:acr_compl.BadOpt
    // void comptest_acr_compl_BadOptColon(); // gstatic/atfdb.comptest:acr_compl.BadOptColon
    // void comptest_acr_compl_BadOptColonSpace(); // gstatic/atfdb.comptest:acr_compl.BadOptColonSpace
    // void comptest_acr_compl_BadOptSpace(); // gstatic/atfdb.comptest:acr_compl.BadOptSpace
    // void comptest_acr_compl_Bare(); // gstatic/atfdb.comptest:acr_compl.Bare
    // void comptest_acr_compl_DblColon(); // gstatic/atfdb.comptest:acr_compl.DblColon
    // void comptest_acr_compl_DblColonList(); // gstatic/atfdb.comptest:acr_compl.DblColonList
    // void comptest_acr_compl_DblSpace(); // gstatic/atfdb.comptest:acr_compl.DblSpace
    // void comptest_acr_compl_DblSpaceList(); // gstatic/atfdb.comptest:acr_compl.DblSpaceList
    // void comptest_acr_compl_EnumCtypeColon(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeColon
    // void comptest_acr_compl_EnumCtypeColonList(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeColonList
    // void comptest_acr_compl_EnumCtypeSpace(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeSpace
    // void comptest_acr_compl_EnumCtypeSpaceList(); // gstatic/atfdb.comptest:acr_compl.EnumCtypeSpaceList
    // void comptest_acr_compl_EnumFieldColon(); // gstatic/atfdb.comptest:acr_compl.EnumFieldColon
    // void comptest_acr_compl_EnumFieldColonList(); // gstatic/atfdb.comptest:acr_compl.EnumFieldColonList
    // void comptest_acr_compl_EnumFieldSpace(); // gstatic/atfdb.comptest:acr_compl.EnumFieldSpace
    // void comptest_acr_compl_EnumFieldSpaceList(); // gstatic/atfdb.comptest:acr_compl.EnumFieldSpaceList
    // void comptest_acr_compl_FlagColon(); // gstatic/atfdb.comptest:acr_compl.FlagColon
    // void comptest_acr_compl_FlagColonList(); // gstatic/atfdb.comptest:acr_compl.FlagColonList
    // void comptest_acr_compl_FlagSpace(); // gstatic/atfdb.comptest:acr_compl.FlagSpace
    // void comptest_acr_compl_FlagSpaceList(); // gstatic/atfdb.comptest:acr_compl.FlagSpaceList
    // void comptest_acr_compl_Install(); // gstatic/atfdb.comptest:acr_compl.Install
    // void comptest_acr_compl_NumColon(); // gstatic/atfdb.comptest:acr_compl.NumColon
    // void comptest_acr_compl_NumColonList(); // gstatic/atfdb.comptest:acr_compl.NumColonList
    // void comptest_acr_compl_NumSpace(); // gstatic/atfdb.comptest:acr_compl.NumSpace
    // void comptest_acr_compl_NumSpaceList(); // gstatic/atfdb.comptest:acr_compl.NumSpaceList
    // void comptest_acr_compl_OptCumul(); // gstatic/atfdb.comptest:acr_compl.OptCumul
    // void comptest_acr_compl_OptCumulAlias(); // gstatic/atfdb.comptest:acr_compl.OptCumulAlias
    // void comptest_acr_compl_OptD(); // gstatic/atfdb.comptest:acr_compl.OptD
    // void comptest_acr_compl_OptDList(); // gstatic/atfdb.comptest:acr_compl.OptDList
    // void comptest_acr_compl_OptH(); // gstatic/atfdb.comptest:acr_compl.OptH
    // void comptest_acr_compl_OptHList(); // gstatic/atfdb.comptest:acr_compl.OptHList
    // void comptest_acr_compl_OptNonCumul(); // gstatic/atfdb.comptest:acr_compl.OptNonCumul
    // void comptest_acr_compl_OptNonCumulAlias(); // gstatic/atfdb.comptest:acr_compl.OptNonCumulAlias
    // void comptest_acr_compl_OptPkeyColon(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColon
    // void comptest_acr_compl_OptPkeyColonFull(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonFull
    // void comptest_acr_compl_OptPkeyColonFullList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonFullList
    // void comptest_acr_compl_OptPkeyColonList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonList
    // void comptest_acr_compl_OptPkeyColonPrefix(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonPrefix
    // void comptest_acr_compl_OptPkeyColonPrefixList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonPrefixList
    // void comptest_acr_compl_OptPkeyColonSubstr(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonSubstr
    // void comptest_acr_compl_OptPkeyColonSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptPkeyColonSubstrList
    // void comptest_acr_compl_OptPkeySpace(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpace
    // void comptest_acr_compl_OptPkeySpaceFull(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceFull
    // void comptest_acr_compl_OptPkeySpaceFullList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceFullList
    // void comptest_acr_compl_OptPkeySpaceList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceList
    // void comptest_acr_compl_OptPkeySpacePrefix(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpacePrefix
    // void comptest_acr_compl_OptPkeySpacePrefixList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpacePrefixList
    // void comptest_acr_compl_OptPkeySpaceSubstr(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceSubstr
    // void comptest_acr_compl_OptPkeySpaceSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptPkeySpaceSubstrList
    // void comptest_acr_compl_OptRegxColon(); // gstatic/atfdb.comptest:acr_compl.OptRegxColon
    // void comptest_acr_compl_OptRegxColonFull(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonFull
    // void comptest_acr_compl_OptRegxColonFullList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonFullList
    // void comptest_acr_compl_OptRegxColonList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonList
    // void comptest_acr_compl_OptRegxColonPrefix(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonPrefix
    // void comptest_acr_compl_OptRegxColonPrefixList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonPrefixList
    // void comptest_acr_compl_OptRegxColonSubstr(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonSubstr
    // void comptest_acr_compl_OptRegxColonSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptRegxColonSubstrList
    // void comptest_acr_compl_OptRegxSpace(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpace
    // void comptest_acr_compl_OptRegxSpaceFull(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceFull
    // void comptest_acr_compl_OptRegxSpaceFullList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceFullList
    // void comptest_acr_compl_OptRegxSpaceList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceList
    // void comptest_acr_compl_OptRegxSpacePrefix(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpacePrefix
    // void comptest_acr_compl_OptRegxSpacePrefixList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpacePrefixList
    // void comptest_acr_compl_OptRegxSpaceSubstr(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceSubstr
    // void comptest_acr_compl_OptRegxSpaceSubstrList(); // gstatic/atfdb.comptest:acr_compl.OptRegxSpaceSubstrList
    // void comptest_acr_compl_OptSig(); // gstatic/atfdb.comptest:acr_compl.OptSig
    // void comptest_acr_compl_OptSigList(); // gstatic/atfdb.comptest:acr_compl.OptSigList
    // void comptest_acr_compl_OptV(); // gstatic/atfdb.comptest:acr_compl.OptV
    // void comptest_acr_compl_OptVList(); // gstatic/atfdb.comptest:acr_compl.OptVList
    // void comptest_acr_compl_R01a(); // gstatic/atfdb.comptest:acr_compl.R01a
    // void comptest_acr_compl_R01b(); // gstatic/atfdb.comptest:acr_compl.R01b
    // void comptest_acr_compl_R01c(); // gstatic/atfdb.comptest:acr_compl.R01c
    // void comptest_acr_compl_R01d(); // gstatic/atfdb.comptest:acr_compl.R01d
    // void comptest_acr_compl_R02a(); // gstatic/atfdb.comptest:acr_compl.R02a
    // void comptest_acr_compl_R02b(); // gstatic/atfdb.comptest:acr_compl.R02b
    // void comptest_acr_compl_R02c(); // gstatic/atfdb.comptest:acr_compl.R02c
    // void comptest_acr_compl_R02d(); // gstatic/atfdb.comptest:acr_compl.R02d
    // void comptest_acr_compl_StrColon(); // gstatic/atfdb.comptest:acr_compl.StrColon
    // void comptest_acr_compl_StrColonList(); // gstatic/atfdb.comptest:acr_compl.StrColonList
    // void comptest_acr_compl_StrSpace(); // gstatic/atfdb.comptest:acr_compl.StrSpace
    // void comptest_acr_compl_StrSpaceList(); // gstatic/atfdb.comptest:acr_compl.StrSpaceList
    // void comptest_acr_compl_T01(); // gstatic/atfdb.comptest:acr_compl.T01
    // void comptest_acr_compl_T02(); // gstatic/atfdb.comptest:acr_compl.T02
    // void comptest_acr_compl_T03(); // gstatic/atfdb.comptest:acr_compl.T03
    // void comptest_acr_compl_T04(); // gstatic/atfdb.comptest:acr_compl.T04
    // void comptest_acr_compl_T05(); // gstatic/atfdb.comptest:acr_compl.T05
    // void comptest_acr_compl_T06(); // gstatic/atfdb.comptest:acr_compl.T06
    // void comptest_acr_compl_T07(); // gstatic/atfdb.comptest:acr_compl.T07
    // void comptest_acr_compl_T08(); // gstatic/atfdb.comptest:acr_compl.T08
    // void comptest_acr_compl_T09(); // gstatic/atfdb.comptest:acr_compl.T09
    // void comptest_acr_compl_T10(); // gstatic/atfdb.comptest:acr_compl.T10

    // valid command line with named option
    // void comptest_acr_compl_CheckValid(); // gstatic/atfdb.comptest:acr_compl.CheckValid

    // unknown option
    // void comptest_acr_compl_CheckUnknownOpt(); // gstatic/atfdb.comptest:acr_compl.CheckUnknownOpt

    // unknown command
    // void comptest_acr_compl_CheckUnknownCmd(); // gstatic/atfdb.comptest:acr_compl.CheckUnknownCmd

    // valid command with boolean flag
    // void comptest_acr_compl_CheckValidFlag(); // gstatic/atfdb.comptest:acr_compl.CheckValidFlag

    // valid command with anonymous arg
    // void comptest_acr_compl_CheckValidAnon(); // gstatic/atfdb.comptest:acr_compl.CheckValidAnon

    // valid command with multiple options
    // void comptest_acr_compl_CheckMultiOpt(); // gstatic/atfdb.comptest:acr_compl.CheckMultiOpt

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_dm.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_dm_Conflict(); // gstatic/atfdb.comptest:acr_dm.Conflict
    // void comptest_acr_dm_Merge(); // gstatic/atfdb.comptest:acr_dm.Merge
    // void comptest_acr_dm_RenameTuple(); // gstatic/atfdb.comptest:acr_dm.RenameTuple

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_ed.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_ed_CreateCtype(); // gstatic/atfdb.comptest:acr_ed.CreateCtype
    // void comptest_acr_ed_CreateMsg(); // gstatic/atfdb.comptest:acr_ed.CreateMsg
    // void comptest_acr_ed_CreateSrcfileTarget(); // gstatic/atfdb.comptest:acr_ed.CreateSrcfileTarget
    // void comptest_acr_ed_CreateSsimfile(); // gstatic/atfdb.comptest:acr_ed.CreateSsimfile
    // void comptest_acr_ed_CreateSsimfileBadNs(); // gstatic/atfdb.comptest:acr_ed.CreateSsimfileBadNs
    // void comptest_acr_ed_CreateTarget(); // gstatic/atfdb.comptest:acr_ed.CreateTarget

    // -------------------------------------------------------------------
    // cpp/atf_comp/acr_in.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_acr_in_Reverse(); // gstatic/atfdb.comptest:acr_in.Reverse
    // void comptest_acr_in_Simple(); // gstatic/atfdb.comptest:acr_in.Simple
    // void comptest_acr_in_Tree(); // gstatic/atfdb.comptest:acr_in.Tree

    // -------------------------------------------------------------------
    // cpp/atf_comp/aqlite.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_aqlite_CompileOptions(); // gstatic/atfdb.comptest:aqlite.CompileOptions
    // void comptest_aqlite_Constraints(); // gstatic/atfdb.comptest:aqlite.Constraints
    // void comptest_aqlite_ErrorHandling(); // gstatic/atfdb.comptest:aqlite.ErrorHandling
    // void comptest_aqlite_Joins(); // gstatic/atfdb.comptest:aqlite.Joins
    // void comptest_aqlite_Number(); // gstatic/atfdb.comptest:aqlite.Number
    // void comptest_aqlite_PatternMatching(); // gstatic/atfdb.comptest:aqlite.PatternMatching
    // void comptest_aqlite_Performance(); // gstatic/atfdb.comptest:aqlite.Performance
    // void comptest_aqlite_Smoke(); // gstatic/atfdb.comptest:aqlite.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_cmdline.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_atf_cmdline_Bare(); // gstatic/atfdb.comptest:atf_cmdline.Bare
    // void comptest_atf_cmdline_Debug(); // gstatic/atfdb.comptest:atf_cmdline.Debug
    // void comptest_atf_cmdline_Help(); // gstatic/atfdb.comptest:atf_cmdline.Help
    // void comptest_atf_cmdline_Minimal(); // gstatic/atfdb.comptest:atf_cmdline.Minimal
    // void comptest_atf_cmdline_MinimalExec(); // gstatic/atfdb.comptest:atf_cmdline.MinimalExec
    // void comptest_atf_cmdline_Rich(); // gstatic/atfdb.comptest:atf_cmdline.Rich
    // void comptest_atf_cmdline_RichExec(); // gstatic/atfdb.comptest:atf_cmdline.RichExec
    // void comptest_atf_cmdline_Sig(); // gstatic/atfdb.comptest:atf_cmdline.Sig
    // void comptest_atf_cmdline_Verbose(); // gstatic/atfdb.comptest:atf_cmdline.Verbose
    // void comptest_atf_cmdline_Version(); // gstatic/atfdb.comptest:atf_cmdline.Version

    // -------------------------------------------------------------------
    // cpp/atf_comp/atf_comp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:atf_comp

    // -------------------------------------------------------------------
    // cpp/atf_comp/coverage.cpp
    //

    // Set up coverage mode: force cfg, clear and create covdir
    void CoverageSetup();

    // Run final coverage merge pass after all tests complete
    void CoverageFinalize();

    // -------------------------------------------------------------------
    // cpp/atf_comp/jkv.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_jkv_ArrayFill(); // gstatic/atfdb.comptest:jkv.ArrayFill
    // void comptest_jkv_ReverseSmoke(); // gstatic/atfdb.comptest:jkv.ReverseSmoke
    // void comptest_jkv_Smoke(); // gstatic/atfdb.comptest:jkv.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/mdbg.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_mdbg_OutOfOrderArgs(); // gstatic/atfdb.comptest:mdbg.OutOfOrderArgs
    // void comptest_mdbg_Smoke(); // gstatic/atfdb.comptest:mdbg.Smoke
    // void comptest_mdbg_SmokeBreak(); // gstatic/atfdb.comptest:mdbg.SmokeBreak
    // void comptest_mdbg_SmokeBreak2(); // gstatic/atfdb.comptest:mdbg.SmokeBreak2

    // -------------------------------------------------------------------
    // cpp/atf_comp/orgfile.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_orgfile_Hash(); // gstatic/atfdb.comptest:orgfile.Hash
    // void comptest_orgfile_MoveByDate(); // gstatic/atfdb.comptest:orgfile.MoveByDate
    // void comptest_orgfile_Dedup(); // gstatic/atfdb.comptest:orgfile.Dedup
    // void comptest_orgfile_DedupPathregx(); // gstatic/atfdb.comptest:orgfile.DedupPathregx
    // void comptest_orgfile_MoveNoop(); // gstatic/atfdb.comptest:orgfile.MoveNoop
    // void comptest_orgfile_ConsumeInput(); // gstatic/atfdb.comptest:orgfile.ConsumeInput
    // void comptest_orgfile_MoveDot(); // gstatic/atfdb.comptest:orgfile.MoveDot

    // -------------------------------------------------------------------
    // cpp/atf_comp/proc.cpp
    //

    // Set test variable and add to Replscope
    void SetVar(strptr name, strptr value);

    // Spawn subprocess with $-substitution, return reference.
    atf_comp::FProc &ProcStart(strptr cmd);

    // Write a line to process stdin. If proc.subst, apply $-substitution.
    void ProcWrite(atf_comp::FProc &proc, strptr msg);

    // Send signal to process
    void ProcKill(atf_comp::FProc &proc, int signal);

    // Close process stdin (signal EOF)
    void ProcWriteEof(atf_comp::FProc &proc);

    // Read process stdout until UNTIL pattern is found (or EOF).
    // Empty UNTIL means read until EOF (process exit).
    // Detect atf.var lines and set variables.
    // Emit output lines to log as "proc -> line".
    // Use Fbuf Linebuf for line reassembly; poll with 1-second timeout
    // for test timeout enforcement.
    tempstr ProcRead(atf_comp::FProc &proc, strptr until);

    // Wait for process to exit, drain remaining stdout into log.
    void ProcWait(atf_comp::FProc &proc);

    // Get process exit code
    int ProcStatus(atf_comp::FProc &proc);

    // Wait for all procs in reverse order
    void ProcWaitAll();

    // -------------------------------------------------------------------
    // cpp/atf_comp/samp_meng.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_samp_meng_Smoke(); // gstatic/atfdb.comptest:samp_meng.Smoke

    // -------------------------------------------------------------------
    // cpp/atf_comp/sandbox.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_sandbox_Anon(); // gstatic/atfdb.comptest:sandbox.Anon
    // void comptest_sandbox_Clean(); // gstatic/atfdb.comptest:sandbox.Clean
    // void comptest_sandbox_Command(); // gstatic/atfdb.comptest:sandbox.Command
    // void comptest_sandbox_Create(); // gstatic/atfdb.comptest:sandbox.Create
    // void comptest_sandbox_Del(); // gstatic/atfdb.comptest:sandbox.Del
    // void comptest_sandbox_Diff(); // gstatic/atfdb.comptest:sandbox.Diff
    // void comptest_sandbox_Gc(); // gstatic/atfdb.comptest:sandbox.Gc
    // void comptest_sandbox_List(); // gstatic/atfdb.comptest:sandbox.List
    // void comptest_sandbox_Pull(); // gstatic/atfdb.comptest:sandbox.Pull
    // void comptest_sandbox_Reset(); // gstatic/atfdb.comptest:sandbox.Reset
    // void comptest_sandbox_StdinMulti(); // gstatic/atfdb.comptest:sandbox.StdinMulti

    // -------------------------------------------------------------------
    // cpp/atf_comp/ssimfilt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_ssimfilt_Csv(); // gstatic/atfdb.comptest:ssimfilt.Csv
    // void comptest_ssimfilt_CsvField(); // gstatic/atfdb.comptest:ssimfilt.CsvField
    // void comptest_ssimfilt_FirstTag(); // gstatic/atfdb.comptest:ssimfilt.FirstTag
    // void comptest_ssimfilt_Json(); // gstatic/atfdb.comptest:ssimfilt.Json
    // void comptest_ssimfilt_JsonRecursive(); // gstatic/atfdb.comptest:ssimfilt.JsonRecursive
    // void comptest_ssimfilt_MatchField(); // gstatic/atfdb.comptest:ssimfilt.MatchField
    // void comptest_ssimfilt_MatchTag(); // gstatic/atfdb.comptest:ssimfilt.MatchTag
    // void comptest_ssimfilt_SelectField(); // gstatic/atfdb.comptest:ssimfilt.SelectField
    // void comptest_ssimfilt_Stable(); // gstatic/atfdb.comptest:ssimfilt.Stable
    // void comptest_ssimfilt_Table(); // gstatic/atfdb.comptest:ssimfilt.Table

    // -------------------------------------------------------------------
    // cpp/atf_comp/sv2ssim.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void comptest_sv2ssim_Convert1(); // gstatic/atfdb.comptest:sv2ssim.Convert1
    // void comptest_sv2ssim_Convert1Signed(); // gstatic/atfdb.comptest:sv2ssim.Convert1Signed
    // void comptest_sv2ssim_Convert2(); // gstatic/atfdb.comptest:sv2ssim.Convert2
    // void comptest_sv2ssim_Convert2Tsv(); // gstatic/atfdb.comptest:sv2ssim.Convert2Tsv
    // void comptest_sv2ssim_UniqueFieldName(); // gstatic/atfdb.comptest:sv2ssim.UniqueFieldName
}
