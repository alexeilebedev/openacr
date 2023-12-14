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
// Target: acr_compl (exe) -- ACR shell auto-complete for all targets
// Exceptions: NO
// Header: include/acr_compl.h
//

#include "include/algo.h"
#include "include/gen/acr_compl_gen.h"
#include "include/gen/acr_compl_gen.inl.h"

namespace acr_compl { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr_compl/main.cpp
    //

    // adapted from cpp/amc/global.cpp
    tempstr GetCmdArgType(acr_compl::FField& field);

    // adapted from cpp/amc/global.cpp
    bool CmdArgValueRequiredQ(acr_compl::FField &field);

    // adapted from cpp/amc/global.cpp
    bool CmdArgRequiredQ(acr_compl::FField &field);

    // adapted from cpp/amc/global.cpp
    acr_compl::FField *GetEnumField(acr_compl::FField &field);
    acr_compl::FCompletion &AddFieldToCompletion(acr_compl::FField &field, acr_compl_BadnessEnum badness);

    // mark as seen field and linked alias or base field
    void MarkSeen(acr_compl::FField &field);

    // whether the field is eligible to be proposed, e.g. not already seen
    bool EligibleQ(acr_compl::FField &field);
    bool CumulativeQ(acr_compl::FField &field);

    // whether the completion is unique at lowest badness -
    // get two first elements from the heap,
    // and compare their badnesses
    bool UniqueCompletionQ();

    // Read source data for completions from file FNAME into global table _db.complsource
    void LoadComplsource(strptr fname);

    // Create completion entries for string VALUE, which is a piece of a key for
    // type CTYPE
    void Main_Line_Ctype(acr_compl::FCtype *ctype, strptr value, strptr compl_prefix, bool exact);
    void Main_Line_Ctype(acr_compl::FCtype *ctype, strptr value, strptr compl_prefix);

    // process ACR query attribute
    void Main_Line_Acr(acr_compl::FField *, strptr value, cstring &compl_prefix);

    // perform completion
    void Main_Line();

    // Generate a script to install completion handler
    void Main_Install(strptr prog);

    // main routine
    void Main();

    // -------------------------------------------------------------------
    // cpp/acr_compl/shell.cpp
    //
    void Main_SplitLineToArgv();

    //
    // Perform single and double quote shell expansion
    // Other expansions are not supported
    // qtype  is quote type on which the input string ends (active quoting)
    // qpoint is the point in the ___output___ string corresponding to the point
    // in the input string where the active quote starts
    tempstr Main_ShellExpand(strptr in, acr_compl::Shellqtype *qtype, i32 *qpoint);

    // properly escape the character for: 1) readline, 2) bash
    void char_PrintEscBash(char c, cstring &out, acr_compl_ShellqtypeEnum type);
    void strptr_PrintEscBash(strptr str, cstring &out, acr_compl_ShellqtypeEnum type);
}
