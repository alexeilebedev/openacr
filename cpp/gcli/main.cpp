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
// Source: cpp/gcli/main.cpp
//

#include "include/algo.h"
#include "include/gcli.h"

// -----------------------------------------------------------------------------
// Manage environment variables: USER,HOME,EDITOR
static void Main_ManageEnv() {
    gcli::_db.unix_user = getenv("USER");
    gcli::_db.home = getenv("HOME");
    gcli::_db.editor = getenv("EDITOR");

    bool need_editor(false);
    ind_beg(gcli::_db_zd_gtblact_curs,gtblact,gcli::_db){
        need_editor|=(gtblact.e && gcli::_db.cmdline.e);
    }ind_end;

    vrfy(gcli::_db.unix_user.ch_n,"Your USER environment variable has not been set");
    vrfy(gcli::_db.home.ch_n,"Your HOME environment variable has not been set");
    vrfy(!need_editor || gcli::_db.editor.ch_n, "Your environment variable EDITOR has not been set");
}
// -----------------------------------------------------------------------------
static void SetHeaderToken(strptr token){
    ind_beg(gcli::_db_gtypeh_curs,gtypeh,gcli::_db){
        Replace(gtypeh.header,"$TOKEN",token);
    }ind_end;
}
// -----------------------------------------------------------------------------
static void Main_Setopts(){
    if (gcli::_db.cmdline.accept){
        gcli::_db.cmdline.approve=true;
    }
    // Validate table name/params
    gcli::ValidateGtbl();
    // Map cmdline args to verbs
    gcli::ArgsToTuple();

    // prepare support structure for gscope
    if (gcli::_db.cmdline.dry_run){
        algo_lib::_db.cmdline.verbose=true;
    }
    // parse _db.tokenbuf into gtblact fields
    gcli::TokenbufToGtblactflds();
    gcli::VerifyGtblactflds();

    // prepare support structure for gclicmdf2j
    ind_beg(gcli::_db_gclicmd_curs,gclicmd,gcli::_db){
        if (gcli::FGclicmd *gclicmd_base=gcli::ind_gclicmd_Find(gclicmd.gclicmdf2j)){
            ind_beg(gcli::gclicmd_c_gclicmdf2j_curs,gclicmdf2j,*gclicmd_base){
                tempstr gclicmdf_key;
                gclicmdf_key=gcli::FGclicmdf_Concat_gclicmd_field(gclicmd.gclicmd,field_Get(gclicmdf2j));
                gcli::FGclicmdf &gclicmdf=gcli::ind_gclicmdf_GetOrCreate(gclicmdf_key);

                tempstr gclicmdj2f_key;
                gclicmdj2f_key=gcli::FGclicmdj2f_Concat_gclicmd_jkey(gclicmd.gclicmd,jkey_Get(gclicmdf2j));
                gcli::FGclicmdj2f &gclicmdj2f=gcli::ind_gclicmdj2f_GetOrCreate(gclicmdj2f_key);
                gclicmdj2f.dup=gclicmdf2j.dup;
                c_gclicmdf_Insert(gclicmdj2f,gclicmdf);
            }ind_end;
        }
    }ind_end;

    // ind_beg(gcli::_db_gclicmdj2f_curs,gclicmdj2f,gcli::_db){
    //      ind_beg(gcli::gclicmdj2f_c_gclicmdf_curs,gclicmdf,gclicmdj2f){
    //          prlog(Keyval(gclicmdj2f.gclicmdj2f,field_Get(gclicmdf)));
    //      }ind_end;
    // }ind_end;

    // prepare ctype support structure for fields
    ind_beg(gcli::_db_gclicmdf_curs,gclicmdf,gcli::_db){
        gcli::FGclicmdc &gclicmdc=gcli::ind_gclicmdc_GetOrCreate(gclicmdc_Get(gclicmdf));
        c_gclicmdf_InsertMaybe(gclicmdc,gclicmdf);
        if (gcli::FGclicmd *gclicmd=gcli::ind_gclicmd_Find(gclicmd_Get(gclicmdf))){
            c_gclicmdc_InsertMaybe(*gclicmd,gclicmdc);
        }
    }ind_end;

    Main_ManageEnv();
    gcli::Main_ManageAuth();
    SetHeaderToken(gcli::_db.grepo_sel.token);
}
// -----------------------------------------------------------------------------
gcli::FGclicmd&  gcli::AddGclicmd(strptr gclicmd_key, bool argOK, strptr arg){
    gcli::FGclicmd *gclicmd=gcli::ind_gclicmd_Find(gclicmd_key);
    c_gclicmd_InsertMaybe(*gclicmd);
    if (argOK) {
        c_gclicmdarg_RemoveAll(*gclicmd);
        gcli::AddGclicmdArg(gclicmd_key,arg);
    }
    return *gclicmd;
}
// -----------------------------------------------------------------------------
void gcli::AddGclicmdArg(strptr gclicmd_key, strptr arg){
    if (gcli::FGclicmd *gclicmd=gcli::ind_gclicmd_Find(gclicmd_key)){
        tempstr gclicmdarg_key;
        gclicmdarg_key=gcli::FGclicmdarg_Concat_gclicmd_arg(gclicmd_key,arg);
        gcli::FGclicmdarg &gclicmdarg=gcli::ind_gclicmdarg_GetOrCreate(gclicmdarg_key);
        c_gclicmdarg_InsertMaybe(*gclicmd,gclicmdarg);
    }
}
// -----------------------------------------------------------------------------
static tempstr GitBranch(){
    tempstr ret(gcli::GetCurrentGitBranch());
    if (StartsWithQ(ret,"mr_")){
        Replace(ret,"mr_","mr:");
    }
    return ret;
}
// -----------------------------------------------------------------------------
static void Main_Shortcuts(){
    tempstr table(Pathcomp(gcli::_db.cmdline.selector,":RL"));
    tempstr sel(Pathcomp(gcli::_db.cmdline.selector,":RR"));
    if (sel=="."
        || (sel=="%" && gcli::_db.cmdline.t)
        ){
        gcli::_db.cmdline.selector=tempstr()<<table<<":"<<GitBranch();
    }
    if (sel==".."){
        gcli::_db.cmdline.selector=tempstr()<<table<<":"<<GitBranch()<<".";
    }
}
// -----------------------------------------------------------------------------
void gcli::Main() {
    Main_Shortcuts();
    Main_Setopts();

    // Visit verbs in the sorted order, new verbs can be added to the tail
    ind_beg(gcli::_db_zd_gtblact_curs,gtblact,gcli::_db) {
        verblog(Keyval("gtblact",gtblact.gtblact)
                <<Keyval("id",gtblact.id)
                );
        gtblact.step(gtblact);
    }ind_end;
}
// -----------------------------------------------------------------------------
// TODO: Maybe convert gcli to GraphQL?
// Examples are below
// Github mutations
//curl -k 'https://api.github.com/graphql' -X POST -H "Authorization: Bearer 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"
//
//{"query":"mutation {markPullRequestReadyForReview(input:{pullRequestId:\"PR_kwDOKLbbec5a7JY4\"}){pullRequest{isDraft}}}"}
//{"query":"mutation {convertPullRequestToDraft(input:{pullRequestId:\"PR_kwDOKLbbec5a7JY4\"}){pullRequest{isDraft}}}"}

// Github discovery
//{"query":"query {__type(name:\"PullRequest\"){name kind description fields {name}}}"}
// Github query
//{"query":"query{repository(owner:\"vparizhs\",name:\"ghp\"){pullRequest(number:461){state,title,number,isDraft,headRefName,updatedAt,id}}}"}

// Gitlab mutations
//curl -k 'https://192.168.1.45:1010/api/graphql' -X POST -H "Authorization: Bearer glpat-Hf38RB2J9GUZRhc54FCs" -H "Content-Type: application/json" --data
//
//{"query":"mutation {mergeRequestSetDraft(input:{iid:\"9\",projectPath:\"algornd/glpat\",draft:true}){mergeRequest{title,draft}}}"}

// Gitlab discovery
//{"query":"query {__type(name: \"MergeRequest\") {kind,name,fields {name,description,type{name}}}}"}
// Gitlab query
//{"query":"query{project(fullPath:\"algornd/glpat\"){mergeRequest(iid: \"9\"){title}}}"}
//{"query":"query{project(fullPath:\"algornd/glpat\"){issue(iid: \"330\"){title}}}"}
