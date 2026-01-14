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
// Source: cpp/gcli/milestone.cpp
//

#include "include/gcli.h"
// -----------------------------------------------------------------------------
void gcli::Main_ShowMslist(){
    algo_lib::FTxttbl txttbl;
    AddRow(txttbl);
    AddCols(txttbl,"MILESTONE,TITLE");
    if (gcli::_db.cmdline.t){
        AddCol(txttbl,"DESCRIPTION");
    }
    ind_beg(gcli::_db_milestone_curs, milestone, gcli::_db) if (milestone.select){
        AddRow(txttbl);
        AddCol(txttbl,milestone.milestone);
        AddCol(txttbl,milestone.title);
        if (gcli::_db.cmdline.t){
            AddCol(txttbl,milestone.description);
        }
    }ind_end;
    prlog(txttbl);
}
// -----------------------------------------------------------------------------
tempstr gcli::MilestoneName(strptr proj, strptr iid){
    return gclidb::Milestone_Concat_project_iid(proj,iid);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mslist(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form milestone from read tuple
        gclidb::Milestone milestone_in;
        if (Milestone_ReadStrptrMaybe(milestone_in,tuple.tuples)){
            milestone_in.milestone=MilestoneName(gclicmd.proj,milestone_in.milestone);
            // Add/replace milestone record
            gcli::FMilestone &milestone=gcli::ind_milestone_GetOrCreate(milestone_in.milestone);
            milestone.select=true;
            milestone_CopyIn(milestone,milestone_in);
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
gcli::FMilestone& gcli::GetMilestone(strptr milestone_key) {
    gcli::AddGclicmd(gclidb_Gclicmd_gclicmd_mslist,true,"");
    gcli::Main_CurlExec();

    gcli::FMilestone *milestone = gcli::ind_milestone_Find(milestone_key);
    if (!milestone){
        prlog("gcli.error "
              <<Keyval("milestone",milestone_key)
              <<Keyval("comment","milestone does not exist, pick up from mslist below")
              );
        Main_ShowMslist();
    }
    vrfy_(milestone);
    return *milestone;
}
// -----------------------------------------------------------------------------
void gcli::gtblact_milestone_list(gcli::FGtblact &gtblact){
    AddGclicmd(gclidb_Gclicmd_gclicmd_mslist,true,"");
    gcli::Main_CurlExec();

    algo_lib::Regx milestone_regx;
    Regx_ReadSql(milestone_regx,gtblact.id, false);
    algo_lib::Regx title_regx;
    Regx_ReadSql(title_regx,gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_title), false);
    ind_beg(gcli::_db_milestone_curs,milestone,gcli::_db){
        milestone.select&=algo_lib::Regx_Match(milestone_regx,milestone.milestone);
        milestone.select&=algo_lib::Regx_Match(title_regx,milestone.title);
    }ind_end;

    gcli::Main_ShowMslist();
}
