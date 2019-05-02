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
// Header: include/acr_ed.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/gen/acr_ed_gen.h"
#include "include/gen/acr_ed_gen.inl.h"

namespace acr_ed { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr/ed/ctype.cpp -- Create, delete, rename ctype
    //

    // Create a new ctype
    // The new type can be relational, i.e. a subset of a cross product of 2 other types,
    // or an in-memory type.
    // Example 1:
    // acr_ed -create -ctype acmdb.Devos -subset1 acmdb.Device -subset2 acmdb.Os -separator /
    // Example 2:
    // acr_ed -create -ctype atf_tmsg.FOrder -pooltype Tpool -indexed
    //
    void Main_CreateCtype();
    void Main_DeleteCtype();
    void Main_RenameCtype();

    // -------------------------------------------------------------------
    // cpp/acr/ed/field.cpp -- Create, delete, rename field
    //
    void Main_DeleteField();
    void Main_CreateField();

    // Look at field FIELD, which is of reftype acr_ed::_db.cmdline.reftype.
    // Create any required record for it:
    // Ptrary -> dmmeta.ptrary
    // Llist -> dmmeta.llist
    // Tary -> dmmeta.tary
    // Bheap -> dmmeta.bheap
    // Thash -> dmmeta.thash
    // Inlary -> dmmeta.inlary
    void InsertFieldExtras(strptr field, strptr arg, strptr reftype);

    // -------------------------------------------------------------------
    // cpp/acr/ed/finput.cpp
    //

    // #AL# todo: merge this with -create -ctype
    void Main_CreateFinput();

    // -------------------------------------------------------------------
    // cpp/acr/ed/main.cpp
    //

    // Request that amc runs after the current script
    void NeedAmc();
    acr_ed::FCtype *Basetype(acr_ed::FCtype &ctype);
    acr_ed::FField *PkeyField(strptr ctype);
    tempstr ToLowerUnder(strptr str);
    void InsertSrcfileInclude(algo_lib::Replscope &R, bool mainheader);
    void Main();

    // -------------------------------------------------------------------
    // cpp/acr/ed/sandbox.cpp
    //
    void BeginSandbox();

    // Change to sandbox directory
    void EnterSandboxX();

    // Return from sandbox to original directory.
    // Sandbox stays in place.
    void ExitSandbox();

    // Build specified tools inside the sandbox
    void BuildX(strptr what);

    // -------------------------------------------------------------------
    // cpp/acr/ed/srcfile.cpp -- Create, delete, rename source file
    //
    void Main_CreateSrcfile();
    void Main_RenameSrcfile();
    void Main_DeleteSrcfile();

    // -------------------------------------------------------------------
    // cpp/acr/ed/ssimfile.cpp -- Create, delete, rename ssim file
    //
    void Main_CreateSsimfile();
    void Main_RenameSsimfile();
    void Main_DeleteSsimfile();

    // -------------------------------------------------------------------
    // cpp/acr/ed/target.cpp -- Create, delete, rename target
    //
    void Main_CreateTarget();
    void Main_RenameTarget();
    void Main_DeleteTarget();

    // -------------------------------------------------------------------
    // cpp/acr/ed/unittest.cpp -- Create, delete, rename unit test
    //
    void Main_CreateUnittest();

    // Create a new normalization check
    void Main_CreateNormcheck();
}
