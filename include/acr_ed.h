// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2024 Astra
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
    // acr_ed -create -ctype atf_tmsg.FOrder -reftype Tpool -indexed
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Ctype(); // dev.edaction:Create_Ctype

    // acr_ed -ctype:X -del -write
    // void edaction_Delete_Ctype(); // dev.edaction:Delete_Ctype

    // acr_ed -ctype:X -rename:Y -write
    // void edaction_Rename_Ctype(); // dev.edaction:Rename_Ctype

    // -------------------------------------------------------------------
    // cpp/acr_ed/dispatch_msg.cpp -- Create dispatch_msg record
    //

    // Add a dmmeta.dispatch_msg record routing a message ctype to a dispatch.
    // Pkey form: "<dispatch>/<msgtype-ctype>", e.g. "lib_prot.Client/ams.LogMsg".
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_DispatchMsg(); // dev.edaction:Create_DispatchMsg

    // -------------------------------------------------------------------
    // cpp/acr_ed/field.cpp -- Create, delete, rename field
    //

    // Delete a field from the schema, and rewrite every ssimfile whose rows carry
    // its values so that the values go with it.
    //
    // Dropping the schema row alone is not enough.  acr parses a data row against
    // the current ctype and ignores an attribute it does not recognize, which is
    // what lets a tuple survive a schema that has moved on; the same tolerance
    // leaves the deleted field's values sitting in the ssimfile, invisible to
    // acr -check, to amc, and to a query of the row itself.
    //
    // Reading a row through the new schema and writing it back is what drops the
    // attribute, so each ssimfile holding rows of the edited ctype is rewritten
    // once the schema row is gone.
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Delete_Field(); // dev.edaction:Delete_Field

    // Rename a field within its ctype, refusing the spellings of -rename that
    // cannot mean what they look like.
    //
    // A bare new name is the ordinary form: `acr_ed -field a.B.c -rename d` renames
    // the field to a.B.d.  A rename does not move a field between ctypes, so the
    // ctype is already known, and the full pkey a.B.d means the same thing.
    //
    // `-rename field:a.B.d` is the spelling a query uses, and it is not one here.
    // `acr` reads `field:a.B` as the ctype and writes a record whose pkey no query
    // finds, so the prefix is refused rather than stripped -- -rename takes a field
    // name and nothing else.
    //
    // A new name carrying a different ctype moves the field to another table.  `acr`
    // renames the column of the ctype named in the *new* pkey, so where the old
    // ctype has an ssimfile its rows keep an attribute no field claims -- invisible
    // to `acr -check`, to `amc` and to a query of the row, which is the state a
    // field delete was taught to avoid.  The move is refused whenever either side is
    // ssim-backed, and it is a delete and a create rather than a rename.  Between
    // two in-memory ctypes there is no column to strand, so it goes through.
    // void edaction_Rename_Field(); // dev.edaction:Rename_Field
    // void edaction_Create_Field(); // dev.edaction:Create_Field

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

    // Add a dmmeta.fstep record on an existing field with the chosen steptype
    // (default Inline).
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Fstep(); // dev.edaction:Create_Fstep

    // Add a dmmeta.fcurs record for a custom cursor on an existing field.
    // The fcurs pkey is "<field>/<curstype-name>", e.g. "ns.FDb.ind_x/curs".
    // void edaction_Create_Fcurs(); // dev.edaction:Create_Fcurs

    // -------------------------------------------------------------------
    // cpp/acr_ed/finput.cpp
    //

    // #AL# todo: merge this with -create -ctype
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Finput(); // dev.edaction:Create_Finput

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
    void ScriptEditFile(algo_lib::Replscope &R, strptr fname);
    void ProcessAction();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:acr_ed

    // -------------------------------------------------------------------
    // cpp/acr_ed/srcfile.cpp -- Create, delete, rename source file
    //

    // Create cpp, script, h or readme file
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Srcfile(); // dev.edaction:Create_Srcfile

    // Rename cpp, h, or readme file
    // void edaction_Rename_Srcfile(); // dev.edaction:Rename_Srcfile

    // Delete cpp,h, or readme file
    // void edaction_Delete_Srcfile(); // dev.edaction:Delete_Srcfile

    // -------------------------------------------------------------------
    // cpp/acr_ed/ssimfile.cpp -- Create, delete, rename ssim file
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Ssimfile(); // dev.edaction:Create_Ssimfile
    // void edaction_Rename_Ssimfile(); // dev.edaction:Rename_Ssimfile
    // void edaction_Delete_Ssimfile(); // dev.edaction:Delete_Ssimfile

    // -------------------------------------------------------------------
    // cpp/acr_ed/target.cpp -- Create, delete, rename target
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Target(); // dev.edaction:Create_Target
    // void edaction_Rename_Target(); // dev.edaction:Rename_Target
    // void edaction_Delete_Target(); // dev.edaction:Delete_Target

    // -------------------------------------------------------------------
    // cpp/acr_ed/unittest.cpp -- Create, delete, rename unit test
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void edaction_Create_Unittest(); // dev.edaction:Create_Unittest

    // Create a new normalization check
    // void edaction_Create_Citest(); // dev.edaction:Create_Citest
}
