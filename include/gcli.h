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
// Header: include/gcli.h
//

#include "include/gen/gcli_gen.h"
#include "include/gen/gcli_gen.inl.h"
#define old_intf false

namespace gcli { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/gcli/git.cpp
    //
    void AssertGitWorkDirClean();
    tempstr GetCurrentGitBranch();
    void PushGitBranch(strptr remote_name);
    void ParseGitComment(strptr issue_key,tempstr &title,tempstr &description);
    void CheckGitBranchExists(strptr issue_key);
    void GitCheckoutBranch(strptr target_branch, strptr source_branch);
    void GitCheckoutMasterBranch(gcli::FIssue &issue);
    void GitRemoveMrBranch(strptr mr_branch);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_gitconfig_list(gcli::FGtblact&);
    // void gtblact_gitconfig_create(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/gtblact.cpp
    //
    void PropagateGtblactID(strptr id);

    // Parse selector into a table and an id
    void ValidateGtbl();

    // void gcli::ArgsToTuple(int argc, char **argv) {
    void ArgsToTuple();

    // parse key:value,key:value,... or value - no other mix is covered
    void TokenbufToGtblactflds();
    void zd_gtblact_Extend(strptr gtblact_key, strptr id);
    gcli::FGtblact & SetGtblactfld(strptr gtblact_key, strptr field_key, strptr val);
    void SetGtblactfldDflt(strptr gtblact_key, strptr field_key);
    void VerifyGtblactflds();
    tempstr GetTblactfld(strptr verb, strptr field);

    // take an issue and populate gtblactfld uval2 with its values
    void TupleToGtblactflds(gcli::FGtblact &gtblact,strptr str);
    bool EditGtblactfld(gcli::FGtblact &gtblact,strptr issue_key_in);
    void VerifyGtblactfldsUpdate(gcli::FGtblact &gtblact);
    tempstr GtblactfieldsToJson(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/help.cpp
    //
    void ShowHelp(strptr gtbl_key, strptr gact_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_help_list(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/issue.cpp
    //
    void Main_ShowIssuelist();

    // Interpret -issue argument as a number, or specific issue id
    // gitlab -issue 33        --> return 33
    // gitlab -issue algouk.33 --> return 33
    // gitlab -issue %         --> return 0
    tempstr IssueArgNumber(strptr issue_key, strptr slash);
    bool SingleIssueQ(strptr issue_key);
    void VerifySingleIssue(strptr issue_key);
    tempstr GetTargetKey(strptr issue_key);
    gcli::FIssue & ValidateIssue(strptr issue_key);
    gcli::FIssue & ReadSingleIssue(gcli::FGtblact &gtblact);
    tempstr Gstate(strptr gstate_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_issuelist(gcli::FGclicmd &gclicmd);
    // void gclicmd_issuesearch(gcli::FGclicmd &gclicmd);
    // void gclicmd_issuenote(gcli::FGclicmd &gclicmd);
    // void gclicmd_issueadd(gcli::FGclicmd &gclicmd);
    // void gclicmd_issuemod(gcli::FGclicmd &gclicmd);
    // void gtblact_issue_list(gcli::FGtblact &gtblact);
    // void gtblact_issue_update(gcli::FGtblact &gtblact);
    // void gtblact_issue_create(gcli::FGtblact &gtblact);
    // void gtblact_issue_start(gcli::FGtblact &gtblact);
    // void gtblact_issue_needs_work(gcli::FGtblact &gtblact);
    // void gtblact_issue_stop(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/main.cpp
    //
    gcli::FGclicmd&  AddGclicmd(strptr gclicmd_key, bool argOK, strptr arg);
    void AddGclicmdArg(strptr gclicmd_key, strptr arg);
    void Main();

    // -------------------------------------------------------------------
    // cpp/gcli/milestone.cpp
    //
    void Main_ShowMslist();
    tempstr MilestoneName(strptr proj, strptr iid);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_mslist(gcli::FGclicmd &gclicmd);
    gcli::FMilestone& GetMilestone(strptr milestone_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_milestone_list(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/mr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_graphql(gcli::FGclicmd&);
    void Main_ShowMrlist();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_repojobtrace(gcli::FGclicmd&);
    // void gclicmd_repojob(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrlist(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrlistdet(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrnote(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrreview(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrreviewrm(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrreq(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrmod(gcli::FGclicmd &gclicmd);
    // void gclicmd_mrsearch(gcli::FGclicmd &gclicmd);
    // void gclicmd_mraccept(gcli::FGclicmd &gclicmd);
    // void gtblact_mr_create(gcli::FGtblact &gtblact);
    gcli::FMr & ReadSingleMr(gcli::FGtblact &gtblact);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_mr_update(gcli::FGtblact &gtblact);
    void Mr_SearchCond(gcli::FGtblact &gtblact);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_mr_list(gcli::FGtblact &gtblact);
    // void gtblact_mrjob_list(gcli::FGtblact &gtblact);
    // void gtblact_mr_accept(gcli::FGtblact &gtblact);
    // void gtblact_mr_approve(gcli::FGtblact &gtblact);
    // void gtblact_mr_needs_work(gcli::FGtblact &gtblact);
    // void gtblact_mr_start(gcli::FGtblact &gtblact);
    // void gtblact_mr_stop(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/note.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_noteadd(gcli::FGclicmd&);
    // void gclicmd_mrnoteadd(gcli::FGclicmd&);
    // void gclicmd_notemod(gcli::FGclicmd&);
    // void gclicmd_mrnotemod(gcli::FGclicmd&);
    // void gtblact_issuenote_create(gcli::FGtblact &gtblact);
    // void gtblact_issuenote_update(gcli::FGtblact &gtblact);
    // void gtblact_issuenote_list(gcli::FGtblact &gtblact);
    // void gtblact_mrnote_list(gcli::FGtblact &gtblact);
    // void gtblact_mrnote_create(gcli::FGtblact &gtblact);
    // void gtblact_mrnote_update(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/repo.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_repo_list(gcli::FGtblact &gtblact);
    // void gclicmd_token2repos(gcli::FGclicmd &gclicmd);
    void LoadGrepo();
    void Main_ManageAuth();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_repo_create(gcli::FGtblact &gtblact);
    // void gtblact_reporemote_list(gcli::FGtblact &gtblact);
    // void gtblact_repo_update(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/rest.cpp
    //
    tempstr Urlencode(strptr str_in);
    void CurlExec(gcli::FGclicmd &gclicmd);
    void Main_CurlExec();

    // -------------------------------------------------------------------
    // cpp/gcli/search.cpp
    //

    // Set conditions based on the fields
    tempstr SearchGitlabSetCond(gcli::FGtblact &gtblact);

    // Set conditions based on the fields
    tempstr SearchGithubSetCond(gcli::FGtblact &gtblact);

    // -------------------------------------------------------------------
    // cpp/gcli/user.cpp
    //
    void Main_ShowUserlist();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_user(gcli::FGclicmd &gclicmd);
    // void gclicmd_userlist(gcli::FGclicmd &gclicmd);
    // void gclicmd_userdet(gcli::FGclicmd &gclicmd);
    gcli::FUser& GetUser(strptr user_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_user_list(gcli::FGtblact &gtblact);
}
