// Copyright (C) 2023-2026 AlgoRND
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
// Source: cpp/gcli/user.cpp
//

#include "include/gcli.h"

// -----------------------------------------------------------------------------
void gcli::Main_ShowUserlist() {
    algo_lib::FTxttbl txttbl;
    AddRow(txttbl);
    AddCols(txttbl,"USER,NAME");
    if (gcli::_db.cmdline.t){
        AddCol(txttbl,"ID");
    }
    ind_beg(gcli::_db_user_curs, user, gcli::_db) if (user.select){
        AddRow(txttbl);
        AddCol(txttbl,user.user);
        AddCol(txttbl,user.name);
        if (gcli::_db.cmdline.t){
            AddCol(txttbl,tempstr()<<user.id);
        }
    }ind_end;
    prlog(txttbl);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_user(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_userlist(gclicmd);
    // TODO: what was this code doing?
    // if (gcli::_db.cmdline.iassignto == ""){
    //     ind_beg(gcli::_db_user_curs,user,gcli::_db){
    //         gcli::_db.cmdline.iassignto=user.user;
    //     }ind_end;
    // }
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_userlist(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form user from read tuple
        gclidb::User user_in;
        if (User_ReadStrptrMaybe(user_in,tuple.tuples)){
            // Add/replace user record
            gcli::FUser &user=gcli::ind_user_GetOrCreate(user_in.user);
            user.select=true;
            user_CopyIn(user,user_in);
            // prepare to recover user name (github)
            if (user.name=="" && gcli::_db.cmdline.t) {
                AddGclicmdArg(gclidb_Gclicmd_gclicmd_userdet,user.user);
            }
        }
    }ind_end;

}
// -----------------------------------------------------------------------------
void gcli::gclicmd_userdet(gcli::FGclicmd &gclicmd){
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form user from read tuple
        gclidb::User user_in;
        if (User_ReadStrptrMaybe(user_in,tuple.tuples)){
            // Add/replace user record
            gcli::FUser &user=gcli::ind_user_GetOrCreate(user_in.user);
            user.select=true;
            user.name=user_in.name;
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
gcli::FUser& gcli::GetUser(strptr user_key) {
    gcli::AddGclicmd(gclidb_Gclicmd_gclicmd_userlist,true,"");
    gcli::Main_CurlExec();

    gcli::FUser *user = gcli::ind_user_Find(user_key);
    if (!user){
        prlog("gcli.error "
              <<Keyval("user",user_key)
              <<Keyval("comment","user does not exist, pick up from ulist below")
              );
        gcli::Main_ShowUserlist();
    }
    vrfy_(user);
    return *user;
}
// -----------------------------------------------------------------------------
void gcli::gtblact_user_list(gcli::FGtblact &gtblact){
    if (!gcli::user_N()){
        AddGclicmd(gclidb_Gclicmd_gclicmd_userlist,true,"");
        AddGclicmd(gclidb_Gclicmd_gclicmd_userdet,false,"");
        gcli::Main_CurlExec();
    }
    algo_lib::Regx user_regx;
    Regx_ReadSql(user_regx,gtblact.id, false);
    algo_lib::Regx name_regx;
    Regx_ReadSql(name_regx,gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_name), false);
    ind_beg(gcli::_db_user_curs,user,gcli::_db){
        user.select&=algo_lib::Regx_Match(user_regx,user.user);
        user.select&=algo_lib::Regx_Match(name_regx,user.name);
    }ind_end;
    gcli::Main_ShowUserlist();
}
