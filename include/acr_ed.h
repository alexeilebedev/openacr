// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2024 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr_ed (exe) -- Script generator for common dev tasks
// Exceptions: yes
// Header: include/acr_ed.h
//

#include "include/algo.h"
#include "include/gen/acr_ed_gen.h"
#include "include/gen/acr_ed_gen.inl.h"

namespace acr_ed { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/acr_ed/ctype.cpp -- Create, delete, rename ctype
    //

    // Pick a default reftype when creating a subset
    // If the target type is relational, use Pkey. Otherwise use Val
    dmmeta::ReftypePkey SubsetPickReftype(algo::strptr ctype_key);

    // Structured pkey creation: triggered with -subset X -subset2 Y -separator Z
    // Two fields are created under CTYPE:
    // one referring to ctype cmdline.subset, the other to cmdline.subset2
    // The fields are substrings of FIELD_PKEY
    void CreateCrossProduct(dmmeta::Ctype &ctype, dmmeta::Field &field_pkey);

    // Looking for max(msgtypes value) where msgtypes first field is Base and arg is the target subset
    // and return max+1 assuming that msgtypes value is integer.
    // ignore:bigret
    cstring getNextMsgTypeValue(strptr target_subset);

    // Create a new ctype
    // The new type can be relational, i.e. a subset of a cross product of 2 other types,
    // or an in-memory type.
    // Example 1:
    // acr_ed -create -ctype acmdb.Devos -subset1 acmdb.Device -subset2 acmdb.Os -separator /
    // Example 2:
    // acr_ed -create -ctype atf_tmsg.FOrder -pooltype Tpool -indexed
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Ctype(); // gstatic/dev.edaction:Create_Ctype

    // acr_ed -ctype:X -del -write
    // void edaction_Delete_Ctype(); // gstatic/dev.edaction:Delete_Ctype

    // acr_ed -ctype:X -rename:Y -write
    // void edaction_Rename_Ctype(); // gstatic/dev.edaction:Rename_Ctype

    // -------------------------------------------------------------------
    // cpp/acr_ed/field.cpp -- Create, delete, rename field
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Delete_Field(); // gstatic/dev.edaction:Delete_Field
    // void edaction_Rename_Field(); // gstatic/dev.edaction:Rename_Field
    // void edaction_Create_Field(); // gstatic/dev.edaction:Create_Field

    // Look at field FIELD, which is of reftype acr_ed::_db.cmdline.reftype.
    // Create any required record for it:
    // Ptrary -> dmmeta.ptrary
    // Llist -> dmmeta.llist
    // Tary -> dmmeta.tary
    // Bheap -> dmmeta.bheap
    // Thash -> dmmeta.thash
    // Inlary -> dmmeta.inlary
    void InsertFieldExtras(strptr field, algo::strptr arg, strptr reftype);

    // Trivial function to make a field indexed by a hash.
    // This is equivalent to creating an FDb.ind_<name>
    void CreateHashIndex(dmmeta::Field &field);

    // -------------------------------------------------------------------
    // cpp/acr_ed/finput.cpp
    //

    // #AL# todo: merge this with -create -ctype
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Finput(); // gstatic/dev.edaction:Create_Finput

    // -------------------------------------------------------------------
    // cpp/acr_ed/main.cpp
    //

    // Request that amc runs after the current script
    void NeedAmc();
    void RegisterFile(algo::strptr fname, algo::strptr comment);

    // Retrieve BASE type for CTYPE
    acr_ed::FCtype *Basetype(acr_ed::FCtype &ctype);

    // Retrieve pkey field for ctype CTYPE
    // or throw an exception if CTYPE has no fields
    acr_ed::FField *PkeyField(algo::strptr pkey);

    // Convert string to lower_under format: SomeString -> some_String
    tempstr ToLowerUnder(strptr str);

    // Convert string to CamelCase format: some_string -> SomeString
    tempstr ToCamelCase(strptr str);
    void InsertSrcfileInclude(algo_lib::Replscope &R, bool mainheader);
    void BuildTest();
    void ScriptEditFile(algo_lib::Replscope &R, strptr fname);
    void ProcessAction();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:acr_ed

    // -------------------------------------------------------------------
    // cpp/acr_ed/srcfile.cpp -- Create, delete, rename source file
    //

    // Create cpp, script, h or readme file
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Srcfile(); // gstatic/dev.edaction:Create_Srcfile

    // Rename cpp, h, or readme file
    // void edaction_Rename_Srcfile(); // gstatic/dev.edaction:Rename_Srcfile

    // Delete cpp,h, or readme file
    // void edaction_Delete_Srcfile(); // gstatic/dev.edaction:Delete_Srcfile

    // -------------------------------------------------------------------
    // cpp/acr_ed/ssimfile.cpp -- Create, delete, rename ssim file
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Ssimfile(); // gstatic/dev.edaction:Create_Ssimfile
    // void edaction_Rename_Ssimfile(); // gstatic/dev.edaction:Rename_Ssimfile
    // void edaction_Delete_Ssimfile(); // gstatic/dev.edaction:Delete_Ssimfile

    // -------------------------------------------------------------------
    // cpp/acr_ed/target.cpp -- Create, delete, rename target
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Target(); // gstatic/dev.edaction:Create_Target
    // void edaction_Rename_Target(); // gstatic/dev.edaction:Rename_Target
    // void edaction_Delete_Target(); // gstatic/dev.edaction:Delete_Target

    // -------------------------------------------------------------------
    // cpp/acr_ed/unittest.cpp -- Create, delete, rename unit test
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Unittest(); // gstatic/dev.edaction:Create_Unittest

    // Create a new normalization check
    // void edaction_Create_Citest(); // gstatic/dev.edaction:Create_Citest
}
