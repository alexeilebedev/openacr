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
// Target: gcli (exe) -- cli utility for gitlab and github
// Exceptions: yes
// Source: cpp/gcli/mr.cpp
//

#include "include/gcli.h"
// -----------------------------------------------------------------------------
void gcli::gclicmd_graphql(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::Main_ShowMrlist() {
    cstring out;
    out << "MR\tISSUE\tAUTHOR\tREVIEWER\tPIPELINE\tSTATE\tTITLE\n";
    ind_beg(gcli::_db_mr_curs, mr, gcli::_db) if (mr.select) {
        out << mr.mr
            << "\t" << mr.source_branch
            << "\t" << mr.author
            << "\t" << mr.reviewer
            << "\t" << mr.pipeline_status
            << "\t" << mr.state<<mr.draft
            << "\t" << mr.title
            << eol;
        if (gcli::_db.cmdline.t){
            out << eol;
            out << "DESCRIPTION" << eol;
            out << mr.description << eol;
            if (c_mrnote_N(mr)){
                out << eol;
                out << "MRNOTE\tAUTHOR\n";
            }
            ind_beg(gcli::mr_c_mrnote_curs, mrnote, mr) if (mrnote.select){
                out << mrnote.mrnote;
                out << "\t" << mrnote.author;
                out << eol;
                //                out << "NOTE:";
                out << mrnote.note;
                out << eol;
                out << eol;
            }ind_end;
            if (c_mrjob_N(mr)){
                out << eol;
                //                out << "MRJOB\tSTATUS\tID" << eol;
                out << "MRJOB\tSTATUS\tRUNNER" << eol;
                ind_beg(gcli::mr_c_mrjob_curs, mrjob,mr) {
                    out << mrjob.mrjob;
                    out << "\t" << mrjob.status;
                    out << "\t" << mrjob.runner;
                    //                    out << "\t" << mrjob.id;
                    out << eol;
                }ind_end;
            }
        }
    }ind_end;
    prlog(Tabulated(out,"\t"));
}
// -----------------------------------------------------------------------------
static tempstr MrName(strptr proj, strptr iid){
    return gclidb::Mr_Concat_proj_iid(tempstr()<<"mr:"<<proj,iid);
}
// -----------------------------------------------------------------------------
static tempstr MrjobName(strptr ref, strptr name){
    return gclidb::Mrjob_Concat_mr_job(ref, name);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_repojobtrace(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_repojob(gcli::FGclicmd &gclicmd){
    // read tuples
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        gclidb::Mrjob mrjob_in;
        if (Mrjob_ReadStrptrMaybe(mrjob_in,tuple.tuples)){
            mrjob_in.mrjob=MrjobName(gclicmd.p_mr->mr,mrjob_in.name);
            // Add/replace mrjob record
            gcli::FMrjob *mrjob;
            mrjob=gcli::ind_mrjob_Find(mrjob_in.mrjob);
            if (!mrjob){
                mrjob=&mrjob_Alloc();
            }
            mrjob_CopyIn(*mrjob,mrjob_in);
            vrfy(mrjob_XrefMaybe(*mrjob),algo_lib::_db.errtext);
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
static void NormDraft(gcli::FMr &mr){
    if (mr.draft!=""){
        if (algo::StartsWithQ(mr.draft,"draft")){
            mr.draft="draft";
        } else {
            bool draft_yes(false);
            if (bool_ReadStrptrMaybe(draft_yes,mr.draft)
                && draft_yes){
                mr.draft="draft";
            }
        }
    }
    if (mr.draft!="draft"){
        mr.draft="";
    } else {
        mr.draft=tempstr()<<"/"<<mr.draft;
    }
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrlist(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form mr from read tuple
        gclidb::Mr mr_in;
        if (Mr_ReadStrptrMaybe(mr_in,tuple.tuples)){
            // Add/replace mr record
            mr_in.mr=MrName(gclicmd.proj,mr_in.mr);
            gcli::FMr &mr=gcli::ind_mr_GetOrCreate(mr_in.mr);
            gclicmd.p_mr=&mr;
            mr_CopyIn(mr,mr_in);
            mr.select=true;
            if (mr.source_repo!=""){
                mr.source_branch=tempstr()<<Keyval(mr.source_repo,mr.source_branch);
            }
            // pipeline is available in details only
            if (mr.state==gcli::Gstate(gclidb_Gstate_gstate_state_opened)
                || gcli::_db.cmdline.t
                ){
                AddGclicmdArg(gclidb_Gclicmd_gclicmd_mrlistdet,iid_Get(mr));
            }
            // prepare to recover notes
            if (gcli::_db.cmdline.t && mr.select) {
                AddGclicmdArg(gclidb_Gclicmd_gclicmd_mrnote,iid_Get(mr));
            }
            NormDraft(mr);
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrlistdet(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form mr from read tuple
        gclidb::Mr mr_in;
        if (Mr_ReadStrptrMaybe(mr_in,tuple.tuples)){
            mr_in.mr=MrName(gclicmd.proj,mr_in.mr);
            // Add/replace mr record
            gcli::FMr &mr=gcli::ind_mr_GetOrCreate(mr_in.mr);
            gclicmd.p_mr=&mr;
            //            mr.pipeline_status=mr_in.pipeline_status;
            mr_CopyIn(mr,mr_in);
            mr.select=true;
            if (gcli::_db.cmdline.t && mr.select) {
                AddGclicmdArg(gclidb_Gclicmd_gclicmd_mrnote,iid_Get(mr));
            }
            // prepare to recover jobs
            if (gcli::_db.cmdline.t
                && mr.select
                && mr.pipeline_id != ""
                && mr.source_branch!=""
                ) {
                if (gcli::FGclicmd *gclicmd_repojob=gcli::ind_gclicmd_Find(gclidb_Gclicmd_gclicmd_repojob)){
                    gclicmd_repojob->p_mr=&mr;
                    AddGclicmdArg(gclidb_Gclicmd_gclicmd_repojob,mr.pipeline_id);
                }
            }
            NormDraft(mr);
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
static tempstr MrnoteName(strptr proj, strptr iid, strptr id){
    return gclidb::Mrnote_Concat_mr_nid(MrName(proj,iid),id);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrnote(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form mrnote from read tuple
        gclidb::Mrnote mrnote_in;
        if (Mrnote_ReadStrptrMaybe(mrnote_in,tuple.tuples)){
            mrnote_in.mrnote=MrnoteName(gclicmd.proj,gclicmd.issue_id,mrnote_in.mrnote);
            // Add/replace mrnote record
            gcli::FMrnote *mrnote;
            mrnote=gcli::ind_mrnote_Find(mrnote_in.mrnote);
            if (!mrnote){
                mrnote=&mrnote_Alloc();
            }
            mrnote_CopyIn(*mrnote,mrnote_in);
            mrnote->select=true;
            if (mrnote->system!=""){
                bool sys;
                bool_ReadStrptrMaybe(sys,mrnote->system);
                mrnote->select=!sys || gcli::_db.cmdline.show_gitlab_system_notes;
            }
        }
    }ind_end;

    ind_beg(gcli::_db_mrnote_curs,mrnote,gcli::_db){
        vrfy(mrnote_XrefMaybe(mrnote),algo_lib::_db.errtext);
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrreview(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_mrlistdet(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrreviewrm(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_mrlistdet(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrreq(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_mrlistdet(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrmod(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_mrlistdet(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrsearch(gcli::FGclicmd &gclicmd){
    gclicmd_mrlist(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mraccept(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_mrlistdet(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_create(gcli::FGtblact &gtblact){
    // Get branch name
    tempstr branch(GetCurrentGitBranch());
    gtblact.id=branch;
    // add mrlist for xref
    // read the issue - it validates its presence
    AddGclicmd(gclidb_Gclicmd_gclicmd_mrlist,true,"");
    gcli::FIssue &issue=ReadSingleIssue(gtblact);

    // Valdiate that all git changes are commited
    AssertGitWorkDirClean();

    // Do branch git push - issue exists and matches the branch
    PushGitBranch(name_Get(gcli::_db.grepo_sel));

    // Load git comments and parse them into title and description
    tempstr title;
    tempstr description;
    ParseGitComment(issue.issue,title,description);

    // prepare REST request
    // Clear previos mrs
    gcli::mr_RemoveAll();
    lib_json::FNode &obj = lib_json::NewObjectNode(NULL);
    gcli::FGclicmd *gclicmd=NULL;
    if (!issue.p_mr_open){
        gclicmd=&AddGclicmd(gclidb_Gclicmd_gclicmd_mrreq,true,"");

        lib_json::NewStringNode(&obj,"source_branch",branch);
        // TODO: Github feature is currently disabled on free private accounts
        if (gcli::_db.grepo_sel.priv=="public"){
            lib_json::NewStringNode(&obj,"draft","Y");
        }
        lib_json::NewStringNode(&obj,"head",branch);
        lib_json::NewStringNode(&obj,"target_branch",gcli::_db.grepo_sel.default_branch);
        lib_json::NewStringNode(&obj,"base",gcli::_db.grepo_sel.default_branch);
        //        lib_json::NewNumberNode(&obj,"issue",iid_Get(*issue));
    } else {
        gclicmd=&AddGclicmd(gclidb_Gclicmd_gclicmd_mrmod,true,iid_Get(*issue.p_mr_open));

        prlog("Merge request is already existing for this branch.");
        prlog("No worry, branch has updated in merge request.");
    }
    lib_json::NewStringNode(&obj,"title",tempstr()<<"Draft: "<<title);
    lib_json::NewStringNode(&obj,"description",description);
    lib_json::NewStringNode(&obj,"body",description);
    lib_json::JsonSerialize(&obj,gclicmd->body);

    gcli::Main_CurlExec();
    gcli::Main_ShowMrlist();

    // Propagate Mr number downstream
    gcli::FMr *mr_new=NULL;
    ind_beg(gcli::_db_mr_curs,mr,gcli::_db){
        mr_new=&mr;
    }ind_end;
    if (mr_new){
        PropagateGtblactID(mr_new->mr);
    }
}
// -----------------------------------------------------------------------------
gcli::FMr & gcli::ReadSingleMr(gcli::FGtblact &gtblact){
    // read the mr we want to update
    tempstr mr(gtblact.id);
    VerifySingleIssue(mr);

    // Populate gtblactfld with mr
    gcli::FGclicmd *gclicmd=&AddGclicmd(gclidb_Gclicmd_gclicmd_mrlistdet,true,gcli::IssueArgNumber(mr,""));
    if (gcli::_db.cmdline.t){
        AddGclicmd(gclidb_Gclicmd_gclicmd_mrnote,false,"");
        AddGclicmd(gclidb_Gclicmd_gclicmd_repojob,false,"");
    }
    gcli::Main_CurlExec();

    vrfy(gclicmd->p_mr,tempstr()
         <<Keyval("mr",mr)
         <<Keyval("comment","mr is not found")
         );
    return *gclicmd->p_mr;
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_update(gcli::FGtblact &gtblact){
    gcli::_db.cmdline.t=gtblact.t;
    tempstr reviewer(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_reviewer));
    // read the mr we want to update
    gcli::FMr &mr=ReadSingleMr(gtblact);
    // populate gtblactflds with the mr
    gclidb::Mr mr_out;
    mr_CopyOut(mr,mr_out);
    TupleToGtblactflds(gtblact,tempstr()<<mr_out);

    // update gtblactfld uval with edits
    if (gcli::_db.cmdline.e) {
        if (!EditGtblactfld(gtblact,mr.mr)){
            prlog("Mr update has not been submitted, you can continue editing it.");
            return;
        }
        // re-populate gtblactflds with the mr
        TupleToGtblactflds(gtblact,tempstr()<<mr_out);
        reviewer=gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_reviewer);
    }

    // Mark fields for update
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) if (gtblactfld.present){
        gtblactfld.update=(gtblactfld.uval!="")
            && (gtblactfld.uval!=gtblactfld.uval2);
    }ind_end;

    // Reviewer for github requires special process
    bool remove_reviewer=(reviewer=="none" && mr.reviewer!="" && gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp);
    bool add_reviewer=(reviewer!="" && gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp);
    if (remove_reviewer){
        gcli::SetGtblactfld(gclidb_Gtblact_gtblact_mr_update,gclidb_Gfld_gfld_reviewer, mr.reviewer);
    }

    // update marked fields
    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_mrmod,true,gcli::IssueArgNumber(iid_Get(mr),""));
        gclicmd.body=d;
        if (add_reviewer){
            gcli::FGclicmd &gclicmd_review=AddGclicmd(gclidb_Gclicmd_gclicmd_mrreview,true,gcli::IssueArgNumber(iid_Get(mr),""));
            gclicmd_review.body=d;
        }
        if (remove_reviewer){
            gcli::FGclicmd &gclicmd_review=AddGclicmd(gclidb_Gclicmd_gclicmd_mrreviewrm,true,gcli::IssueArgNumber(iid_Get(mr),""));
            gclicmd_review.body=d;
        }

        gcli::Main_CurlExec();
        gcli::Main_ShowMrlist();
    } else {
        prlog("Mr has no changes.");
    }
    // consider operation successful, remove file
    if (gcli::_db.edit_file!="") {
        unlink(Zeroterm(gcli::_db.edit_file));
        gcli::_db.edit_file="";
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_list(gcli::FGtblact &gtblact){
    //https://api.github.com/search/issues?q=repo:myname/myrepo+state:open+is:issue+in:description+works
    // Conver fields to query cond(ition)
    // extract field
    tempstr mr(gtblact.id);

    // Set command for rest
    //gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_issuelist,true,gcli::IssueArgNumber(id,"/"));
    if (gcli::SingleIssueQ(mr)){
        ReadSingleMr(gtblact);
        AddGclicmd(gclidb_Gclicmd_gclicmd_mrlist,true,gcli::IssueArgNumber(mr,"/"));
    } else {
        if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp){
            gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_mrsearch,true,gcli::IssueArgNumber(mr,"/"));
            gclicmd.cond=SearchGithubSetCond(gtblact);
        } else if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_glpat){
            gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_mrlist,true,"");
            gclicmd.cond=SearchGitlabSetCond(gtblact);
        }
        // Set command for rest
        AddGclicmd(gclidb_Gclicmd_gclicmd_mrlistdet,false,"");
        AddGclicmd(gclidb_Gclicmd_gclicmd_mrnote,false,"");
        gcli::Main_CurlExec();
    }
    gcli::Main_ShowMrlist();
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mrjob_list(gcli::FGtblact &gtblact){
    // read the mr we want to update
    tempstr mr_id(gtblact.id);
    tempstr new_mr_id(Pathcomp(mr_id,"/RL"));
    gtblact.id = SingleIssueQ(new_mr_id) ? new_mr_id : mr_id;
    // identify mrjobs
    if (gtblact.id==""){
        tempstr branch(GetCurrentGitBranch());
        gcli::FGtblact *gtblact_issue=gcli::ind_gtblact_Find(gclidb_Gtblact_gtblact_issue_list);
        gtblact_issue->id=branch;
        // add mrlist for xref
        // read the issue - it validates its presence
        AddGclicmd(gclidb_Gclicmd_gclicmd_mrlist,true,"");
        // Don't wnat ot read multiple issue notes...
        bool cmdline_t(gcli::_db.cmdline.t);
        gcli::_db.cmdline.t=false;
        gcli::FIssue &issue=ReadSingleIssue(*gtblact_issue);
        gcli::_db.cmdline.t=cmdline_t;

        vrfy (issue.p_mr_open,tempstr()
              <<Keyval("issue",issue.issue)
              <<Keyval("comment","no mrjobs for this issue yet")
              );
        mr_id=gcli::_db.cmdline.t ? tempstr()<<issue.p_mr_open->mr<<"/%"
            : tempstr()<<issue.p_mr_open->mr;
        new_mr_id=Pathcomp(mr_id,"/RL");
        gtblact.id = SingleIssueQ(new_mr_id) ? new_mr_id : mr_id;
        mr_id=gtblact.id;
        mr_RemoveAll();
    }
    gcli::_db.cmdline.t=gtblact.t;
    gcli::FMr &mr=ReadSingleMr(gtblact);

    if (SingleIssueQ(new_mr_id)) {
        AddGclicmd(gclidb_Gclicmd_gclicmd_repojobtrace,false,"");
        algo_lib::Regx mrjob_regx;
        Regx_ReadSql(mrjob_regx,mr_id, false);
        ind_beg(gcli::mr_c_mrjob_curs,mrjob,mr) if (algo_lib::Regx_Match(mrjob_regx,mrjob.mrjob)){
            AddGclicmdArg(gclidb_Gclicmd_gclicmd_repojobtrace,mrjob.id);
        }ind_end;
        gcli::Main_CurlExec();
    } else {
        gcli::Main_ShowMrlist();
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_accept(gcli::FGtblact &gtblact){
    tempstr mr(gtblact.id);
    // gcli::FIssue &issue=ValidateIssue(issue_key);
    // vrfy(issue.p_mr_open,tempstr()
    //      <<Keyval("issue",issue_key)
    //      <<Keyval("comment","issue doesn't have open mrreq")
    //      );
    // gcli::_db.cmdline.mraccept=issue.p_mr_open->mr;
    // verblog("gcli.info "
    //         <<Keyval("issue",issue.issue)
    //         <<Keyval("mraccept",gcli::_db.cmdline.mraccept)
    //         );
    VerifySingleIssue(mr);

    AddGclicmd(gclidb_Gclicmd_gclicmd_mraccept,true,gcli::IssueArgNumber(mr,""));
    gcli::Main_CurlExec();

    // ghub is not returning info
    if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp){
        zd_gtblact_Extend(gclidb_Gtblact_gtblact_mr_list,gtblact.id);
    } else {
        gcli::Main_ShowMrlist();
    }
}
// -----------------------------------------------------------------------------
static void MutateDraft(gcli::FGtblact &gtblact, strptr query_in, algo_lib::Replscope &R){
    // Set MR to draft
    // first get a single issue
    tempstr query(Subst(R,query_in));
    gcli::FGclicmd &gclicmd=gcli::AddGclicmd(gclidb_Gclicmd_gclicmd_graphql,true,"");
    lib_json::FNode &obj = lib_json::NewObjectNode(NULL);
    lib_json::NewStringNode(&obj,"query",query);
    lib_json::JsonSerialize(&obj,gclicmd.body);

    gcli::Main_CurlExec();
    // TODO: Get rid of the call and process the reply
    ReadSingleMr(gtblact);
    gcli::Main_ShowMrlist();
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_approve(gcli::FGtblact &gtblact){
    // Set MR to draft
    // first get a single issue
    // TODO: if not a single issue, guess the issue as the current branch, if it is mr branch
    tempstr mrid(gtblact.id);
    VerifySingleIssue(mrid);

    gcli::FMr &mr=ReadSingleMr(gtblact);

    algo_lib::Replscope R;
    tempstr query;
    if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp){
        //{"query":"mutation {markPullRequestReadyForReview(input:{pullRequestId:\"PR_kwDOKLbbec5a7JY4\"}){pullRequest{isDraft}}}"}
        query="mutation {markPullRequestReadyForReview(input:{pullRequestId:\"$NODE_ID\"}){pullRequest{isDraft}}}";
        Set(R,"$NODE_ID",mr.node_id);
    } else if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_glpat){
        //{"query":"mutation {mergeRequestSetDraft(input:{iid:\"9\",projectPath:\"algornd/glpat\",draft:true}){mergeRequest{title,draft}}}"}
        query="mutation {mergeRequestSetDraft(input:{iid:\"$ID\",projectPath:\"$NODE_ID\",draft:false}){mergeRequest{title,draft}}}";
        Set(R,"$NODE_ID",Pathcomp(mr.node_id,"!RL"));
        Set(R,"$ID",iid_Get(mr));
    }
    MutateDraft(gtblact,query,R);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_needs_work(gcli::FGtblact &gtblact){
    // Set MR to draft
    // first get a single issue
    // TODO: if not a single issue, guess the issue as the current branch, if it is mr branch
    tempstr mrid(gtblact.id);
    VerifySingleIssue(mrid);

    gcli::FMr &mr=ReadSingleMr(gtblact);

    algo_lib::Replscope R;
    tempstr query;
    if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp){
        //{"query":"mutation {convertPullRequestToDraft(input:{pullRequestId:\"PR_kwDOKLbbec5a7JY4\"}){pullRequest{isDraft}}}"}
        query="mutation {convertPullRequestToDraft(input:{pullRequestId:\"$NODE_ID\"}){pullRequest{isDraft}}}";
        Set(R,"$NODE_ID",mr.node_id);
    } else if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_glpat){
        //{"query":"mutation {mergeRequestSetDraft(input:{iid:\"9\",projectPath:\"algornd/glpat\",draft:true}){mergeRequest{title,draft}}}"}
        query="mutation {mergeRequestSetDraft(input:{iid:\"$ID\",projectPath:\"$NODE_ID\",draft:true}){mergeRequest{title,draft}}}";
        Set(R,"$NODE_ID",Pathcomp(mr.node_id,"!RL"));
        Set(R,"$ID",iid_Get(mr));
    }
    MutateDraft(gtblact,query,R);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_start(gcli::FGtblact &gtblact){
    vrfy_(gcli::_db.unix_user.ch_n);
    tempstr id(gtblact.id);
    tempstr mr_key=GetTargetKey(id);
    gcli::FMr &mr=ReadSingleMr(gtblact);
    //    gcli::FMr &mr=ValidateMr(mr_key);
    // Check branch exists
    tempstr mr_branch(mr.mr);
    Replace(mr_branch,":","_");
    CheckGitBranchExists(mr_branch);
    // Check dir is clean
    AssertGitWorkDirClean();
    // Checkout branch
    // git doesn't like ":" in a branch name, replace with "_"
    gcli::GitCheckoutBranch(mr_branch,mr.source_branch);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mr_stop(gcli::FGtblact &gtblact){
    // best use - deletes mr branch while you are in it
    // gcli . -stop
    // form mr branch name
    tempstr id(gtblact.id);
    tempstr mr_key=GetTargetKey(id);
    tempstr mr_branch("mr_");
    mr_branch<<mr_key;
    gcli::GitRemoveMrBranch(mr_branch);
}
