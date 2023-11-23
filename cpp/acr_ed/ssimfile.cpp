// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 Astra
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr_ed/ssimfile.cpp -- Create, delete, rename ssim file
//

#include "include/acr_ed.h"

// -----------------------------------------------------------------------------

void acr_ed::edaction_Create_Ssimfile() {
    if (!ind_nsdb_Find(Pathcomp(acr_ed::_db.cmdline.ssimfile,".LL"))) {
        prerr("acr_ed.createssimfile_bad_ssimns"
              <<Keyval("comment","specified namespace is not registered in dmmeta.nsdb table"));
        algo_lib::_db.exit_code++;
    } else {
        edaction_Create_Ctype();
        // print ssimfile record
        dmmeta::Ssimfile ssimfile;
        ssimfile.ssimfile = acr_ed::_db.cmdline.ssimfile;
        ssimfile.ctype = acr_ed::_db.cmdline.ctype;
        acr_ed::_db.out_ssim << ssimfile << eol;

        // default sort  by primary key
        dmmeta::Ssimsort ssimsort;
        ssimsort.ssimfile = ssimfile.ssimfile;
        ssimsort.sortfld = acr_ed::_db.cmdline.field;
        acr_ed::_db.out_ssim << ssimsort << eol;

        // field for comment
        // do not create a comment if base is specified -- it will cause an amc error later
        dmmeta::Field field_comment;// add a comment
        field_comment.field = tempstr() << acr_ed::_db.cmdline.ctype << ".comment";
        field_comment.arg = "algo.Comment";
        acr_ed::_db.out_ssim << field_comment << eol;

        dmmeta::Cfmt cfmt(tempstr() << acr_ed::_db.cmdline.ctype << "." << dmmeta_Strfmt_strfmt_String
                          , dmmeta_Printfmt_printfmt_Tuple
                          , true
                          , true
                          , ""
                          , true // genop
                          , algo::Comment());
        acr_ed::_db.out_ssim << cfmt << eol;

        // register file
        tempstr fname(SsimFname("data", acr_ed::_db.cmdline.ssimfile));
        acr_ed::_db.script << "touch "<<fname<<eol;
        acr_ed::_db.script << "git add "<<fname<<eol;
        acr_ed::_db.script << "update-gitfile"<<eol;
    }
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Rename_Ssimfile() {
    prlog("acr_ed.rename_ssimfile"
          <<Keyval("ssimfile",acr_ed::_db.cmdline.ssimfile)
          <<Keyval("rename",acr_ed::_db.cmdline.rename));
    algo_lib::Replscope R;
    acr_ed::FSsimfile &ssimfile = acr_ed::ind_ssimfile_FindX(acr_ed::_db.cmdline.ssimfile);
    acr_ed::FCtype &ctype = *ssimfile.p_ctype;

    Set(R, "$oldkey", acr_ed::_db.cmdline.ssimfile);
    Set(R, "$newkey", acr_ed::_db.cmdline.rename);

    Set(R, "$olddir", GetNs(acr_ed::_db.cmdline.ssimfile));
    Set(R, "$newdir", GetNs(acr_ed::_db.cmdline.rename));

    Set(R, "$oldname", StripNs("", acr_ed::_db.cmdline.ssimfile));
    Set(R, "$newname", StripNs("", acr_ed::_db.cmdline.rename));

    Set(R, "$oldfname", "data/$olddir/$oldname.ssim");
    Set(R, "$newfname", "data/$newdir/$newname.ssim");

    Set(R, "$ctype", ssimfile.ctype);

    // nothing more to do...
    Ins(&R, acr_ed::_db.script, "bin/acr ssimfile:$oldkey -rename $newkey  -write");
    Ins(&R, acr_ed::_db.script, "git mv $oldfname $newfname");
    Ins(&R, acr_ed::_db.script, "sed -i 's/^$oldkey  /$newkey  /' $newfname");    // modify contents of new ssimfile...
    // update key field
    if (c_field_N(ctype)>0 && StripNs("", acr_ed::_db.cmdline.ssimfile) == name_Get(*c_field_Find(ctype,0))) {
        Ins(&R, acr_ed::_db.script, "acr field:$ctype.$oldname  -rename $ctype.$newname  -write");
    }
    Ins(&R, acr_ed::_db.script, "bin/update-gitfile");
    Ins(&R, acr_ed::_db.script, "acr -check $newkey.% -print:N");    // check result for consistency
    acr_ed::NeedAmc();
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Delete_Ssimfile() {
    prlog("acr_ed.delete_ssimfile  ssimfile:"<<acr_ed::_db.cmdline.ssimfile);
    acr_ed::FSsimfile &ssimfile = acr_ed::ind_ssimfile_FindX(acr_ed::_db.cmdline.ssimfile);
    acr_ed::FCtype &ctype = *ssimfile.p_ctype;
    algo_lib::Replscope R;
    Set(R, "$ctype", ctype.ctype);
    Set(R, "$ssimfile", ssimfile.ssimfile);
    Set(R, "$ssimfname", SsimFname("data", ssimfile.ssimfile));

    Ins(&R, acr_ed::_db.script, "bin/acr ctype:$ctype -del -write");// kills ssimfile record
    Ins(&R, acr_ed::_db.script, "bin/acr gitfile:$ssimfname -del -write");
    Ins(&R, acr_ed::_db.script, "git rm $ssimfname");
    acr_ed::NeedAmc();
}
