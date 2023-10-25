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
// Source: cpp/gcli/search.cpp
//

#include "include/gcli.h"
// -----------------------------------------------------------------------------
// Set conditions based on the fields
tempstr gcli::SearchGitlabSetCond(gcli::FGtblact &gtblact){
    tempstr cond;
    // state
    tempstr state(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_state));
    tempstr gstate=gcli::Gstate(gclidb::Gstate_Concat_st_state("state",state));
    vrfy(gstate!="",tempstr()
         <<Keyval("state",state)
         <<Keyval("comment","state doesn't match a valid state")
         );
    cond<<"state="<<gstate;
    // assignee
    tempstr assignee(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_assignee));
    if (assignee!=""){
        if (assignee=="none") {
            cond<<"&assignee_id=none";
            cond<<"&assignee=none"; //github
        } else {
            cond<<"&assignee_username="<<Urlencode(assignee); //gitlab
            cond<<"&assignee="<<Urlencode(assignee); //github
        }
    }
    // author
    tempstr author(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_author));
    if (author!=""){
        cond<<"&author_username="<<Urlencode(author); //gitlab
    }
    // milestone
    tempstr milestone(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_milestone));
    if (milestone!=""){
        cond<<"&milestone="<<Urlencode(milestone); //gitlab
    }
    // reviewer
    tempstr reviewer(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_reviewer));
    if (reviewer!=""){
        cond<<"&reviewer_username="<<Urlencode(reviewer); //gitlab
    }
    // label
    // TODO: VP - labels cant have spaces or do urlencoding...?
    tempstr labels(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_labels));
    if (labels!=""){
        cond<<"&labels="<<Urlencode(labels); //gitlab
    }
    // description
    tempstr description(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_description));
    if (description!=""){
        cond<<"&in=description&search="<<Urlencode(description); //gitlab
    }
    // title
    tempstr title(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_title));
    if (title!=""){
        cond<<"&in=title&search="<<Urlencode(title); //gitlab
    }
    return cond;
}
// -----------------------------------------------------------------------------
// Set conditions based on the fields
tempstr gcli::SearchGithubSetCond(gcli::FGtblact &gtblact){
    tempstr cond;
    // state
    tempstr state(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_state));
    tempstr gstate=gcli::Gstate(gclidb::Gstate_Concat_st_state("state",state));
    vrfy(gstate!="",tempstr()
         <<Keyval("state",state)
         <<Keyval("comment","state doesn't match a valid state")
         );
    cond<<"+state:"<<gstate;
    // assignee
    tempstr assignee(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_assignee));
    if (assignee!=""){
        if (assignee=="none") {
            cond<<"+no:assignee"; //github
        } else {
            cond<<"+assignee:"<<Urlencode(assignee); //github
        }
    }
    // author
    tempstr author(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_author));
    if (author!=""){
        cond<<"+author:"<<Urlencode(author); //github
    }
    // milestone
    tempstr milestone(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_milestone));
    if (milestone!=""){
        cond<<"+milestone:"<<Urlencode(milestone); //github
    }
    // labels
    tempstr labels(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_labels));
    if (labels!=""){
        cond<<"+label:"<<Urlencode(labels); //github
    }
    // description
    tempstr description(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_description));
    if (description!=""){
        cond<<"+in:description&search+"<<Urlencode(description); //github
    }
    // title
    tempstr title(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_title));
    if (title!=""){
        cond<<"+in:title+"<<Urlencode(title); //github
    }
    return cond;
}
