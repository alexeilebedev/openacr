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
// Source: cpp/gcli/issue.cpp
//

#include "include/algo.h"
#include "include/gcli.h"
// -----------------------------------------------------------------------------
void gcli::Main_ShowIssuelist() {
    cstring out;
    out << "ISSUE\tMR\tAUTHOR\tLABELS\tASSIGNEE\tMILESTONE\tSTATE\tTITLE\n";
    ind_beg(gcli::_db_issue_curs, issue, gcli::_db) if (issue.select) {
        out << issue.issue
            << "\t" << issue.mr
            //            << "\t" << issue.milestone
            << "\t" << issue.author
            << "\t" << issue.labels
            << "\t" << issue.assignee
            << "\t" << issue.milestone
            << "\t" << issue.state
            << "\t" << issue.title
            << eol;
        if (gcli::_db.cmdline.t){
            out << eol;
            out << "DESCRIPTION" << eol;
            out <<issue.description << eol;
            if (c_issuenote_N(issue)){
                out << eol;
                out << "ISSUENOTE\tAUTHOR\n";
            }
            ind_beg(gcli::issue_c_issuenote_curs, issuenote, issue) if (issuenote.select){
                out << issuenote.issuenote;
                out << "\t" << issuenote.author;
                out << eol;
                //                out << "NOTE:";
                out << issuenote.note;
                out << eol;
                out << eol;
            }ind_end;

        }
    }ind_end;
    prlog(Tabulated(out,"\t"));
}
// -----------------------------------------------------------------------------
static tempstr IssueName(strptr proj, strptr iid){
    return gclidb::Issue_Concat_project_iid(proj,iid);
}
// -----------------------------------------------------------------------------
static tempstr IssuenoteName(strptr proj, strptr iid, strptr id){
    return gclidb::Issuenote_Concat_issue_nid(IssueName(proj,iid),id);
}
// -----------------------------------------------------------------------------
// Interpret -issue argument as a number, or specific issue id
// gitlab -issue 33        --> return 33
// gitlab -issue algouk.33 --> return 33
// gitlab -issue %         --> return 0
tempstr gcli::IssueArgNumber(strptr issue_key, strptr slash) {
    tempstr ret;
    gclidb::Issue issue;
    issue.issue=tempstr()<<issue_key;
    tempstr expr(iid_Get(issue));
    u32 issuenum=0;
    u32_ReadStrptrMaybe(issuenum,expr);
    if (issuenum!=0
        && !Replace(expr,"%","%")
        ){
        ret<<slash<<issuenum;
    }
    return ret;
}
// -----------------------------------------------------------------------------
bool gcli::SingleIssueQ(strptr issue_key){
    return gcli::IssueArgNumber(issue_key,"")!="";
}
// -----------------------------------------------------------------------------
void gcli::VerifySingleIssue(strptr issue_key){
    vrfy(SingleIssueQ(issue_key),tempstr()
         <<Keyval("target",issue_key)
         <<Keyval("comment","target has to identify a single issue")
         );
}
// -----------------------------------------------------------------------------
tempstr gcli::GetTargetKey(strptr issue_key){
    VerifySingleIssue(issue_key);
    tempstr issue_num(gcli::IssueArgNumber(issue_key,""));
    tempstr proj(name_Get(gcli::_db.grepo_sel));

    return IssueName(proj,issue_num);
}
// -----------------------------------------------------------------------------
gcli::FIssue & gcli::ValidateIssue(strptr issue_key){
    gcli::FIssue *issue = gcli::ind_issue_Find(issue_key);
    if (gcli::_db.cmdline.dry_run) {
        issue=&gcli::ind_issue_GetOrCreate(issue_key);
    }
    vrfy(issue,tempstr()
         <<Keyval("issue",issue_key)
         <<Keyval("comment","Issue does not exist")
         );
    return *issue;
}
// -----------------------------------------------------------------------------
gcli::FIssue & gcli::ReadSingleIssue(gcli::FGtblact &gtblact){
    // read the issue we want to update
    tempstr id(gtblact.id);
    VerifySingleIssue(id);

    // Populate gtblactfield with issue
    gcli::FGclicmd *gclicmd=&AddGclicmd(gclidb_Gclicmd_gclicmd_issuelist,true,gcli::IssueArgNumber(id,"/"));
    if (gcli::_db.cmdline.t){
        AddGclicmd(gclidb_Gclicmd_gclicmd_issuenote,false,"");
    }
    gcli::Main_CurlExec();

    vrfy(gclicmd->p_issue,tempstr()
         <<Keyval("issue",id)
         <<Keyval("comment","issue is not found")
         );
    return *gclicmd->p_issue;
}
// -----------------------------------------------------------------------------
tempstr gcli::Gstate(strptr gstate_key){
    tempstr ret;
    tempstr gstatet_key;
    gstatet_key=
        gclidb::Gstatet_Concat_gstate_gtype(gstate_key,gcli::_db.p_gtype->gtype);
    if (gcli::FGstatet *gstatet=gcli::ind_gstatet_Find(gstatet_key)){
        ret=gstatet->state;
    }
    return ret;
}
// -----------------------------------------------------------------------------
static void IssueToMr(){
    // add MR field to issues
    // TODO: Github source branch is not matching - need to match by title...
    ind_beg(gcli::_db_mr_curs,mr,gcli::_db) {
        tempstr source_branch(mr.source_branch);
        if (mr.source_repo!=""){
            source_branch=Pathcomp(mr.source_branch,":LR");
        }
        if (gcli::FIssue *issue=gcli::ind_issue_Find(source_branch)){
            if (mr.state==gcli::Gstate(gclidb_Gstate_gstate_state_opened)){
                issue->p_mr_open=&mr;
                if (issue->mr!=""){
                    issue->mr<<",";
                }
                issue->mr<<mr.mr<<"/"<<mr.state<<mr.draft;
            }
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_issuelist(gcli::FGclicmd &gclicmd){
    // read tuples
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        gclidb::Issue issue_in;
        if (Issue_ReadStrptrMaybe(issue_in,tuple.tuples)){
            //        prlog(issue_in);
            issue_in.issue=IssueName(gclicmd.proj,issue_in.issue);
            // Add/replace issue record
            gcli::FIssue &issue=gcli::ind_issue_GetOrCreate(issue_in.issue);
            issue_CopyIn(issue,issue_in);
            gclicmd.p_issue=&issue;
            // if (issue.milestone!=""){
            //     issue.milestone=MilestoneName(gclicmd.proj,issue.milestone);
            //     if (gcli::FMilestone *milestone=gcli::ind_milestone_Find(issue.milestone)){
            //         issue.milestone_title=milestone->title;
            //     }
            // }
            issue.select=true;
            // prepare to recover notes
            if (gcli::_db.cmdline.t && issue.select) {
                AddGclicmdArg(gclidb_Gclicmd_gclicmd_issuenote,iid_Get(issue));
            }
        }
    }ind_end;
    IssueToMr();
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_issuesearch(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_issuelist(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_issuenote(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form issuenote from read tuple
        gclidb::Issuenote issuenote_in;
        if (Issuenote_ReadStrptrMaybe(issuenote_in,tuple.tuples)){
            issuenote_in.issuenote=IssuenoteName(gclicmd.proj,gclicmd.issue_id,issuenote_in.issuenote);
            // Add/replace issuenote record
            gcli::FIssuenote *issuenote;
            issuenote=gcli::ind_issuenote_Find(issuenote_in.issuenote);
            if (!issuenote){
                issuenote=&issuenote_Alloc();
            }
            issuenote_CopyIn(*issuenote,issuenote_in);
            issuenote->select=true;
            if (issuenote->system!=""){
                bool sys;
                bool_ReadStrptrMaybe(sys,issuenote->system);
                issuenote->select=!sys || gcli::_db.cmdline.show_gitlab_system_notes;
            }
        }
    }ind_end;

    ind_beg(gcli::_db_issuenote_curs,issuenote,gcli::_db){
        vrfy(issuenote_XrefMaybe(issuenote),algo_lib::_db.errtext);
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_issueadd(gcli::FGclicmd &gclicmd){
    gclicmd_issuelist(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_issuemod(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_issuelist(gclicmd);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issue_list(gcli::FGtblact &gtblact){
    //https://api.github.com/search/issues?q=repo:myname/myrepo+state:open+is:issue+in:description+works
    // Conver fields to query cond(ition)
    // extract field
    tempstr id(gtblact.id);

    // get milestone list for xref
    AddGclicmd(gclidb_Gclicmd_gclicmd_mslist,true,"");
    // get mrlist for xref
    AddGclicmd(gclidb_Gclicmd_gclicmd_mrlist,true,"");

    if (gcli::SingleIssueQ(id)){
        AddGclicmd(gclidb_Gclicmd_gclicmd_issuelist,true,gcli::IssueArgNumber(id,"/"));
    } else if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_ghp){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_issuesearch,true,gcli::IssueArgNumber(id,"/"));
        gclicmd.cond=SearchGithubSetCond(gtblact);
    } else if (gcli::_db.p_gtype->gtype==gclidb_Gtype_gtype_glpat){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_issuelist,true,gcli::IssueArgNumber(id,"/"));
        gclicmd.cond=SearchGitlabSetCond(gtblact);
    }

    AddGclicmd(gclidb_Gclicmd_gclicmd_issuenote,false,"");
    if (gcli::_db.cmdline.dry_run){
        AddGclicmdArg(gclidb_Gclicmd_gclicmd_issuenote,"N");
    }
    gcli::Main_CurlExec();

    algo_lib::Regx issue_regx;
    Regx_ReadSql(issue_regx,id, false);
    ind_beg(gcli::_db_issue_curs,issue,gcli::_db){
        issue.select&=algo_lib::Regx_Match(issue_regx,issue.issue);
    }ind_end;

    gcli::Main_ShowIssuelist();
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issue_update(gcli::FGtblact &gtblact){
    gcli::_db.cmdline.t=gtblact.t;
    // read the issue we want to update
    gcli::FIssue &issue=ReadSingleIssue(gtblact);
    // populate gtblactfields with the issue
    gclidb::Issue issue_out;
    issue_CopyOut(issue,issue_out);
    TupleToGtblactflds(gtblact,tempstr()<<issue_out);

    // update gtblactfld uval with edits
    if (gcli::_db.cmdline.e) {
        if (!EditGtblactfld(gtblact,issue.issue)){
            prlog("Issue update has not been submitted, you can continue editing it.");
            return;
        }
        // re-populate gtblactflds with the issue
        TupleToGtblactflds(gtblact,tempstr()<<issue_out);
    }

    // Mark fields for update
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) if (gtblactfld.present){
        gtblactfld.update=(gtblactfld.uval!="")
            && (gtblactfld.uval!=gtblactfld.uval2);
    }ind_end;

    // update marked fields
    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_issuemod,true,gcli::IssueArgNumber(iid_Get(issue),""));
        gclicmd.body=d;
        gcli::Main_CurlExec();
        gcli::Main_ShowIssuelist();
    } else {
        prlog("Issue has no changes.");
        gcli::Main_ShowIssuelist();
    }
    // consider operation successful, remove file
    if (gcli::_db.edit_file!="") {
        unlink(Zeroterm(gcli::_db.edit_file));
        gcli::_db.edit_file="";
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issue_create(gcli::FGtblact &gtblact){
    // create the issue
    // mark all touched fields as update
    tempstr id(gtblact.id);
    // update gtblactfld uval with edits
    if (gcli::_db.cmdline.e) {
        // need to mark available fields to edit...
        gclidb::Issue issue_out;
        TupleToGtblactflds(gtblact,tempstr()<<issue_out);

        if (!EditGtblactfld(gtblact,"")){
            prlog("Issue create has not been submitted, you can continue composing it.");
            return;
        }
    }

    gcli::VerifyGtblactfldsUpdate(gtblact);
    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_issueadd,true,gcli::IssueArgNumber("",""));
        gclicmd.body=d;

        gcli::Main_CurlExec();
        gcli::Main_ShowIssuelist();

        // Propagate Issue number downstream
        gcli::FIssue *issue_new=NULL;
        ind_beg(gcli::_db_issue_curs,issue,gcli::_db){
            issue_new=&issue;
        }ind_end;
        if (issue_new){
            PropagateGtblactID(issue_new->issue);
        }
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issue_start(gcli::FGtblact &gtblact){
    vrfy_(gcli::_db.unix_user.ch_n);
    tempstr id(gtblact.id);
    tempstr issue_key=GetTargetKey(id);
    gcli::FIssue &issue=ReadSingleIssue(gtblact);
    //    gcli::FIssue &issue=ValidateIssue(issue_key);
    // Check branch exists
    CheckGitBranchExists(issue_key);
    // Check dir is clean
    AssertGitWorkDirClean();
    // Checkout branch
    GitCheckoutMasterBranch(issue);
    // Add "in_progress" label here, preserve other labels
    gcli::_db.cmdline.e=false; // just in case
    //    zd_gtblact_Extend(gclidb_Gtblact_gtblact_issue_update);
    // if "in_progress" label present, just show the issue.
    tempstr labels(issue.labels);
    if (Replace(labels,gclidb_Label_label_in_progress,gclidb_Label_label_in_progress)){
        zd_gtblact_Extend(gclidb_Gtblact_gtblact_issue_list,id);
    } else {
        if (labels!=""){
            labels<<",";
        }
        labels<<gclidb_Label_label_in_progress;
        gcli::SetGtblactfld(gclidb_Gtblact_gtblact_issue_update,gclidb_Gfld_gfld_labels, labels);
        zd_gtblact_Extend(gclidb_Gtblact_gtblact_issue_update,id);
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issue_needs_work(gcli::FGtblact &gtblact){
    // reopen the issue
    gcli::SetGtblactfld(gclidb_Gtblact_gtblact_issue_update,gclidb_Gfld_gfld_state, "open");
    zd_gtblact_Extend(gclidb_Gtblact_gtblact_issue_update,gtblact.id);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issue_stop(gcli::FGtblact &gtblact){
    // close the issue
    gcli::SetGtblactfld(gclidb_Gtblact_gtblact_issue_update,gclidb_Gfld_gfld_state, "close");
    zd_gtblact_Extend(gclidb_Gtblact_gtblact_issue_update,gtblact.id);
}
