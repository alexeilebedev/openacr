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
// Header: include/gcli.h
//

#include "include/algo.h"
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

    // Git-safe rendering of a repo or issue key: "/" flattened to "_", usable as
    // a git remote, branch, or file name (algornd/arnd.33 -> algornd_arnd.33)
    tempstr GitName(strptr key);

    // Git remote gcli operates through: the git-safe name of the selected repo,
    // or the repo key itself when only the older slash-named remote is fetched
    // in this clone.  Resolved once and cached on _db.git_remote.
    tempstr GitRemote();

    // Iid of the issue BRANCH is bound to, empty if none.  The binding is the
    // trailing .<iid> of an issue-key-shaped branch name (git-safe or older
    // slash form), or the commit footer 'closes #<iid> (<branch>)' for branches
    // named some other way (e.g. by gli).  The (<branch>) suffix pins the footer
    // to this branch, so commits merged from other branches never match; the
    // newest matching footer wins.
    tempstr BranchIssueIid(strptr branch);

    // Full issue key for the issue BRANCH is bound to, empty if none.
    // The project part comes from the configured repo whose name (git-safe or
    // slash form) matches the branch's own project part; when the branch does not
    // carry one (e.g. a gli-named branch), the selected or first active repo.
    // Callable before repo selection -- the shortcut resolver runs first and the
    // key it produces is what drives repo selection.
    tempstr BranchIssue(strptr branch);

    // First local branch bound to issue IID, empty if none
    tempstr FindIssueBranch(strptr iid);
    void PushGitBranch(strptr remote_name);
    void ParseGitComment(strptr issue_key,tempstr &title,tempstr &description);
    bool CheckGitBranchExists(strptr issue_key);
    void GitCheckoutBranch(strptr target_branch);

    // Create local branch TARGET_BRANCH tracking SOURCE_BRANCH of the repo remote
    void GitCheckoutBranch(strptr target_branch, strptr source_branch);

    // Create the work branch for ISSUE off the repo default branch and seed it
    // with an empty commit binding the branch to the issue.  The branch is the
    // git-safe issue key (algornd_arnd.33); the commit carries the issue title,
    // the description, and the footer 'closes #<iid> (<branch>)' -- the shape
    // gli recognizes, and the server auto-closes the issue on merge.
    void GitCheckoutMasterBranch(gcli::FIssue &issue);
    void GitRemoveMrBranch(strptr mr_branch);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_gitconfig_list(gcli::FGtblact&); // gstatic/gclidb.gtblact:gitconfig_list
    // void gtblact_gitconfig_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:gitconfig_create

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
    // void gtblact_help_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:help_list

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
    // void gclicmd_issuelist(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:issuelist
    // void gclicmd_issuesearch(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:issuesearch
    // void gclicmd_issuenote(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:issuenote
    // void gclicmd_issueadd(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:issueadd
    // void gclicmd_issuemod(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:issuemod
    // void gtblact_issue_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issue_list
    // void gtblact_issue_update(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issue_update
    // void gtblact_issue_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issue_create
    // void gtblact_issue_start(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issue_start
    // void gtblact_issue_needs_work(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issue_needs_work
    // void gtblact_issue_stop(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issue_stop

    // -------------------------------------------------------------------
    // cpp/gcli/main.cpp
    //
    void RemoveEditFile();
    gcli::FGclicmd&  AddGclicmd(strptr gclicmd_key, bool argOK, strptr arg);
    void AddGclicmdArg(strptr gclicmd_key, strptr arg);
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:gcli

    // -------------------------------------------------------------------
    // cpp/gcli/milestone.cpp
    //
    void Main_ShowMslist();
    tempstr MilestoneName(strptr proj, strptr iid);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_mslist(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mslist
    gcli::FMilestone& GetMilestone(strptr milestone_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_milestone_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:milestone_list

    // -------------------------------------------------------------------
    // cpp/gcli/mr.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_graphql(gcli::FGclicmd&); // gstatic/gclidb.gclicmd:graphql
    void Main_ShowMrlist();

    // Print a job trace.  -limit tails the trace to its last N lines (0 = full);
    // errors:Y keeps only error-looking lines (capped at 10, same heuristic
    // as gli), for a quick read of why a job failed.
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_repojobtrace(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:repojobtrace
    // void gclicmd_repojob(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:repojob
    // void gclicmd_mrlist(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrlist
    // void gclicmd_mrlistdet(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrlistdet
    // void gclicmd_mrnote(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrnote
    // void gclicmd_mrreview(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrreview
    // void gclicmd_mrreviewrm(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrreviewrm
    // void gclicmd_mrreq(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrreq
    // void gclicmd_mrmod(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrmod
    // void gclicmd_mrsearch(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mrsearch
    // void gclicmd_mraccept(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:mraccept
    // void gtblact_mr_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_create
    gcli::FMr & ReadSingleMr(gcli::FGtblact &gtblact);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_mr_update(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_update
    void Mr_SearchCond(gcli::FGtblact &gtblact);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_mr_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_list
    // void gtblact_mrjob_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mrjob_list

    // A retry response carries the replacement job; record it like a job listing
    // void gclicmd_jobretry(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:jobretry

    // Retry CI jobs of the selected MR (gitlab only): the failed and canceled
    // jobs, or with mrjob:<mr>/<regex> the jobs matching the regex regardless of
    // status.  With an empty selector the MR is resolved from the current branch.
    // void gtblact_mrjob_retry(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mrjob_retry
    // void gtblact_mr_accept(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_accept
    // void gtblact_mr_approve(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_approve
    // void gtblact_mr_needs_work(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_needs_work
    // void gtblact_mr_start(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_start
    // void gtblact_mr_stop(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mr_stop

    // -------------------------------------------------------------------
    // cpp/gcli/note.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_noteadd(gcli::FGclicmd&); // gstatic/gclidb.gclicmd:noteadd
    // void gclicmd_mrnoteadd(gcli::FGclicmd&); // gstatic/gclidb.gclicmd:mrnoteadd
    // void gclicmd_notemod(gcli::FGclicmd&); // gstatic/gclidb.gclicmd:notemod
    // void gclicmd_mrnotemod(gcli::FGclicmd&); // gstatic/gclidb.gclicmd:mrnotemod
    // void gtblact_issuenote_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issuenote_create
    // void gtblact_issuenote_update(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issuenote_update
    // void gtblact_issuenote_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:issuenote_list
    // void gtblact_mrnote_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mrnote_list
    // void gtblact_mrnote_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mrnote_create
    // void gtblact_mrnote_update(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:mrnote_update

    // -------------------------------------------------------------------
    // cpp/gcli/repo.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_repo_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:repo_list
    // void gclicmd_token2repos(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:token2repos
    void LoadGrepo();
    void Main_ManageAuth();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_repo_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:repo_create
    // void gtblact_reporemote_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:reporemote_list
    // void gtblact_repo_update(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:repo_update

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
    // cpp/gcli/token.cpp
    //

    // Print selected token records; the secret appears only in a create response
    // (list responses never carry it)
    void Main_ShowTokenlist();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_tokenlist(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:tokenlist
    // void gclicmd_tokenadd(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:tokenadd

    // Revoke returns 204 with no body; nothing to parse
    // void gclicmd_tokendel(gcli::FGclicmd&); // gstatic/gclidb.gclicmd:tokendel
    // void gtblact_token_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:token_list

    // Create a temporary project access token: scope api, role Developer is
    // implied by the credential, expires the day after tomorrow (the server
    // retires it even if nobody revokes it).  The created token record is
    // printed as an ssim tuple -- the one response that carries the secret.
    // void gtblact_token_create(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:token_create

    // Revoke the selected token (gcli token:<proj>.<id> -stop)
    // void gtblact_token_stop(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:token_stop

    // -------------------------------------------------------------------
    // cpp/gcli/user.cpp
    //
    void Main_ShowUserlist();
    //     (user-implemented function, prototype is in amc-generated header)
    // void gclicmd_user(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:user
    // void gclicmd_userlist(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:userlist
    // void gclicmd_userdet(gcli::FGclicmd &gclicmd); // gstatic/gclidb.gclicmd:userdet
    gcli::FUser& GetUser(strptr user_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void gtblact_user_list(gcli::FGtblact &gtblact); // gstatic/gclidb.gtblact:user_list
}
