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
// Target: gcli (exe) -- Gcli - gitlab/github command-line client
// Exceptions: yes
// Source: cpp/gcli/help.cpp
//

#include "include/gcli.h"
// -----------------------------------------------------------------------------
static tempstr Usefield(gcli::FGtblactfld &gtblactfld){
    tempstr ret;
    algo_lib::Replscope R;
    Set(R,"$$","$");
    Set(R,"$field",gcli::field_Get(gtblactfld));
    //    Set(R,"$value",gcli::field_Get(gtblactfld));
    if (gtblactfld.regx){
        Set(R,"$value","regx value");
    } else {
        Set(R,"$value","value");
    }
    if (gtblactfld.field_name_dflt){
        Set(R,"$field",Subst(R,"[$field:]$value"));
    } else {
        Set(R,"$field",Subst(R,"$field:$value"));
    };

    if (gtblactfld.optional){
        Set(R,"$field",Subst(R,"[$field]"));
    } else {
        Set(R,"$field",Subst(R,"<$field>"));
    };
    return Subst(R,"$field");
}
// -----------------------------------------------------------------------------
static tempstr Useline(gcli::FGtblact &gtblact){
    tempstr ret;
    algo_lib::Replscope R;
    Set(R,"$$","$");
    Set(R,"$verb",gtblact.gtblact);
    algo::ListSep ls_sp(" ");
    if (c_gtblactfld_N(gtblact)){
        ret<<Subst(R,"[-fields:]");
        algo::ListSep ls(",");
        ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact){
            ret<<ls<<Usefield(gtblactfld);
        }ind_end;
    }
    if (gtblact.t) {
        if (ret!="") {
            ret<<" ";
        }
        ret<<"[-t]";
    }
    if (gtblact.e) {
        if (ret!="") {
            ret<<" ";
        }
        ret<<"[-e]";
    }
    return ret;
}
// -----------------------------------------------------------------------------
void gcli::ShowHelp(strptr gtbl_key, strptr gact_key){
    algo_lib::Regx gtbl_regx;
    Regx_ReadSql(gtbl_regx,gtbl_key, false);
    algo_lib::Regx gact_regx;
    Regx_ReadSql(gact_regx,gact_key, false);
    cstring out;
    out << "TABLE\tUSE/DFLT\tCOMMENT\n";

    ind_beg(gcli::_db_gtblact_curs, gtblact, gcli::_db) {
        if (gcli::FGtbl *gtbl=gcli::ind_gtbl_Find(gtbl_Get(gtblact))) {
            c_gtblact_InsertMaybe(*gtbl,gtblact);
        }
    }ind_end;

    ind_beg(gcli::_db_gtbl_curs, gtbl, gcli::_db) if (algo_lib::Regx_Match(gtbl_regx,gtbl.gtbl)){
        tempstr comment;
        algo::ListSep ls(",");
        out << gtbl.gtbl
            << "\t"
            << "\t" << gtbl.comment
            << eol;
        if (gcli::_db.cmdline.t){
            ind_beg(gcli::gtbl_c_gtblact_curs,gtblact,gtbl) if (algo_lib::Regx_Match(gact_regx,gact_Get(gtblact))){
                out << "  -" << gact_Get(gtblact)
                    << "\t"
                    << "\t"<< Useline(gtblact)
                    << eol;
                ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact){
                    out << "     " << Usefield(gtblactfld)
                        << "\t" << gtblactfld.dflt
                        << "\t" << gtblactfld.comment
                        //                        << "\t" << gtblactfld.dflt
                        << eol;
                }ind_end;
            }ind_end;
        }
    }ind_end;

    prlog(Tabulated(out,"\t"));
}
// -----------------------------------------------------------------------------
void gcli::gtblact_help_list(gcli::FGtblact &gtblact){
    tempstr gact;
    gact=gcli::GetTblactfld(gclidb_Gtblact_gtblact_help_list,gclidb_Gfld_gfld_action);
    if (gtblact.id!=""
        || gact!="%"){
        gcli::_db.cmdline.t=gtblact.t;
    }
    ShowHelp(gtblact.id,gact);

    if (gtblact.id==""){
        tempstr out;
        out<<"use help[:table] -t for more detailed help, ex \"help:issue -t\" or \"help:help -t\""<<eol;
        out<<"shortcuts: \".\" selector represents current branch, \"..\" selector represents current branch with \".\" to use for notes";
        prlog(out);
    }
}
