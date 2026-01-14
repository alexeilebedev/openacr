// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2023 Astra
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

        // register ssimfile
        acr_ed::RegisterFile(SsimFname("data", acr_ed::_db.cmdline.ssimfile), "");

        // create readme
        acr_ed::RegisterFile(tempstr()<<"txt/ssimdb/"<<ns_Get(ssimfile)<<"/"<<name_Get(ssimfile)<<".md", "");

        dmmeta::Cfmt cfmt(tempstr() << acr_ed::_db.cmdline.ctype << "." << dmmeta_Strfmt_strfmt_String
                          , dmmeta_Printfmt_printfmt_Tuple
                          , true
                          , true
                          , ""
                          , true // genop
                          , algo::Comment());
        acr_ed::_db.out_ssim << cfmt << eol;
    }
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Rename_Ssimfile() {
    prlog("acr_ed.rename_ssimfile"
          <<Keyval("ssimfile",acr_ed::_db.cmdline.ssimfile)
          <<Keyval("rename",acr_ed::_db.cmdline.rename));
    algo_lib::Replscope R;
    acr_ed::FSsimfile &ssimfile = acr_ed::ind_ssimfile_FindX(acr_ed::_db.cmdline.ssimfile);

    dmmeta::Ssimfile newssimfile;
    newssimfile.ssimfile=acr_ed::_db.cmdline.rename;
    newssimfile.ctype=ssimfile.ctype;
    acr_ed::ind_ns_FindX(ns_Get(newssimfile));// produce a useful error message if fail

    Set(R, "$oldkey", acr_ed::_db.cmdline.ssimfile);
    Set(R, "$newkey", acr_ed::_db.cmdline.rename);

    Set(R, "$oldname", name_Get(ssimfile));
    Set(R, "$newname", name_Get(newssimfile));

    Set(R, "$oldfname", SsimFname("data", acr_ed::_db.cmdline.ssimfile));
    Set(R, "$newfname", SsimFname("data", acr_ed::_db.cmdline.rename));

    Set(R, "$oldctype", ssimfile.ctype);

    // update primary key field if its name is the same as ssimfile name
    if (c_field_N(*ssimfile.p_ctype)>0 && name_Get(ssimfile) == name_Get(*c_field_Find(*ssimfile.p_ctype,0))) {
        command::acr acr;
        acr.query = tempstr() << "dmmeta.field:"<<ssimfile.ctype<<"."<<name_Get(ssimfile);
        acr.rename = tempstr() << ssimfile.ctype<<"."<<name_Get(newssimfile);
        acr.write=true;
        _db.script << acr_ToCmdline(acr) << eol;
    }

    // rename the ssimfile record, move the ssimfile,
    // rename the readme, and move the md file
    // -x -- renames md file & git file
    // -g -- adds 'git mv' command
    {
        command::acr acr;
        acr.query << "ssimfile:"<<acr_ed::_db.cmdline.ssimfile;
        acr.rename = acr_ed::_db.cmdline.rename;
        acr.write = true;
        acr.x = true;
        acr.g = true;
        Ins(&R, acr_ed::_db.script, acr_ToCmdline(acr));
    }

    // rename ctype if it was named consistently with the ssimfile
    tempstr oldctype = tempstr()<<ns_Get(ssimfile)<<"."<<ToCamelCase(name_Get(ssimfile));
    if (oldctype == ssimfile.ctype) {
        tempstr newctype = tempstr()<<ns_Get(newssimfile)<<"."<<ToCamelCase(name_Get(newssimfile));
        {
            command::acr acr;
            acr.query = tempstr() << "dmmeta.ctype:"<<ssimfile.ctype;
            acr.rename = tempstr() << newctype;
            acr.write=true;
            _db.script << acr_ToCmdline(acr) << eol;
        }
        // also rename any derived type
        ind_beg(_db_ctype_curs,ctype,_db) {
            if (Basetype(ctype) == ssimfile.p_ctype && name_Get(ctype) == (tempstr()<<"F"<<ToCamelCase(name_Get(ssimfile)))) {
                command::acr acr;
                acr.query  = tempstr()<< "dmmeta.ctype:"<<ctype.ctype;
                acr.rename = tempstr()<<ns_Get(ctype)<<".F"<<ToCamelCase(name_Get(newssimfile));
                acr.write=true;
                _db.script << acr_ToCmdline(acr) << eol;
            }
        }ind_end;
        // also rename any field referring to derived type
        ind_beg(_db_field_curs,field,_db) {
            if (Basetype(*field.p_arg) == ssimfile.p_ctype && name_Get(field) == name_Get(ssimfile)) {
                command::acr acr;
                acr.query  = tempstr()<< "dmmeta.field:"<<field.field;
                acr.rename = tempstr()<<ctype_Get(field)<<"."<<name_Get(newssimfile);
                acr.write=true;
                _db.script << acr_ToCmdline(acr) << eol;
            }
        }ind_end;
        // todo: sed-replacement of "ns::F<oldname>" to "ns::F<newname>" in
        // all source files?
    }
    {
        command::acr acr;
        acr.query = tempstr() << acr_ed::_db.cmdline.rename<<":%";
        acr.print=false;
        acr.check=true;
        _db.script << acr_ToCmdline(acr) << eol;
    }
    acr_ed::NeedAmc();
}

// -----------------------------------------------------------------------------

void acr_ed::edaction_Delete_Ssimfile() {
    prlog("acr_ed.delete_ssimfile  ssimfile:"<<acr_ed::_db.cmdline.ssimfile);
    acr_ed::FSsimfile &ssimfile = acr_ed::ind_ssimfile_FindX(acr_ed::_db.cmdline.ssimfile);
    command::acr acr;
    acr.query = tempstr()<<"ctype:"<<ssimfile.ctype;
    acr.del=true;
    acr.write=true;
    acr.g=true;// git updates
    acr.x=true;// use ssimreq
    acr_ed::_db.script<< acr_ToCmdline(acr)<<eol;
    acr_ed::NeedAmc();
}
