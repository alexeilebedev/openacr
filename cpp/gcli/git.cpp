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
// Source: cpp/gcli/git.cpp
//

#include "include/gcli.h"
// -----------------------------------------------------------------------------
static tempstr GitCmd(strptr gitcmd) {
    tempstr cmd("git");
    if (gcli::_db.cmdline.gitdir.ch_n) {
        cmd << " -C " << gcli::_db.cmdline.gitdir;
    }
    cmd << " " << gitcmd;
    return cmd;
}
// -----------------------------------------------------------------------------
void gcli::AssertGitWorkDirClean() {
    bool clean = !SysCmd(GitCmd("update-index --refresh >/dev/null"), FailokQ(true), DryrunQ(false))
        && !SysCmd(GitCmd("diff-index --quiet HEAD --"), FailokQ(true), DryrunQ(false));
    vrfy(clean,"Working directory is not clean");
}
// -----------------------------------------------------------------------------
tempstr gcli::GetCurrentGitBranch(){
    // git branch --show-current doesn't work on older gits
    tempstr branch(Trimmed(SysEval(GitCmd("rev-parse --symbolic-full-name HEAD"), FailokQ(false), 1024)));
    // convert refs/heads/XX -> XX
    // (use temp string to avoid assigning substrin to itself)
    Replace(branch,"refs/heads/","");
    return branch;
}
// -----------------------------------------------------------------------------
// Git-safe rendering of a repo or issue key: "/" flattened to "_", usable as
// a git remote, branch, or file name (algornd/arnd.33 -> algornd_arnd.33)
tempstr gcli::GitName(strptr key){
    tempstr ret(key);
    Translate(ret,"/","_");
    return ret;
}
// -----------------------------------------------------------------------------
// Git remote gcli operates through: the git-safe name of the selected repo,
// or the repo key itself when only the older slash-named remote is fetched
// in this clone.  Resolved once and cached on _db.git_remote.
tempstr gcli::GitRemote(){
    if (gcli::_db.git_remote==""){
        tempstr name(GitName(gcli::_db.grepo_sel.name));
        tempstr dflt_ref(tempstr()<<"/"<<gcli::_db.grepo_sel.default_branch<<" >/dev/null");
        bool have_new = !SysCmd(GitCmd(tempstr()<<"rev-parse --verify --quiet "<<name<<dflt_ref), FailokQ(true), DryrunQ(false), EchoQ(false));
        bool have_old = !SysCmd(GitCmd(tempstr()<<"rev-parse --verify --quiet "<<gcli::_db.grepo_sel.name<<dflt_ref), FailokQ(true), DryrunQ(false), EchoQ(false));
        gcli::_db.git_remote = !have_new && have_old ? tempstr(gcli::_db.grepo_sel.name) : name;
    }
    return tempstr()<<gcli::_db.git_remote;
}
// -----------------------------------------------------------------------------
// Iid of the issue BRANCH is bound to, empty if none.  The binding is the
// trailing .<iid> of an issue-key-shaped branch name (git-safe or older
// slash form), or the commit footer 'closes #<iid> (<branch>)' for branches
// named some other way (e.g. by gli).  The (<branch>) suffix pins the footer
// to this branch, so commits merged from other branches never match; the
// newest matching footer wins.
tempstr gcli::BranchIssueIid(strptr branch){
    tempstr ret(gcli::IssueArgNumber(branch,""));
    if (ret==""){
        cstring log(SysEval(GitCmd(tempstr()<<"log -100 --format=%s%n%b "<<strptr_ToBash(branch)<<" 2>/dev/null"),FailokQ(true),1024000));
        strptr prefix("closes #");
        tempstr suffix;
        suffix<<"("<<branch<<")";
        ind_beg(algo::Line_curs,line,log){
            if (ret==""){
                strptr trimmed=Trimmed(line);
                if (StartsWithQ(trimmed,prefix) && EndsWithQ(trimmed,suffix)){
                    algo::StringIter it(trimmed);
                    it.index=prefix.n_elems;
                    int n=0;
                    if (algo::TryParseI32(it,n) && n>0){
                        ret<<n;
                    }
                }
            }
        }ind_end;
    }
    return ret;
}
// -----------------------------------------------------------------------------
// Full issue key for the issue BRANCH is bound to, empty if none.
// The project part comes from the configured repo whose name (git-safe or
// slash form) matches the branch's own project part; when the branch does not
// carry one (e.g. a gli-named branch), the selected or first active repo.
// Callable before repo selection -- the shortcut resolver runs first and the
// key it produces is what drives repo selection.
tempstr gcli::BranchIssue(strptr branch){
    gcli::LoadGrepo();
    tempstr ret;
    tempstr iid(BranchIssueIid(branch));
    if (iid!=""){
        tempstr projpart(Pathcomp(branch,".RL"));
        tempstr proj;
        ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db) if (grepo.active && proj==""){
            if (grepo.name==projpart || GitName(grepo.name)==projpart){
                proj=grepo.name;
            }
        }ind_end;
        if (proj=="" && gcli::_db.grepo_sel.name!=""){
            proj=gcli::_db.grepo_sel.name;
        }
        ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db) if (grepo.active && proj==""){
            proj=grepo.name;
        }ind_end;
        if (proj!=""){
            ret=gclidb::Issue_Concat_project_iid(proj,iid);
        }
    }
    return ret;
}
// -----------------------------------------------------------------------------
// First local branch bound to issue IID, empty if none
tempstr gcli::FindIssueBranch(strptr iid){
    tempstr found;
    cstring branches(SysEval(GitCmd("for-each-ref --format='%(refname:short)' refs/heads/"),FailokQ(true),65536));
    ind_beg(algo::Line_curs,branch,branches){
        if (branch!="" && found==""){
            if (BranchIssueIid(branch)==iid){
                found=branch;
            }
        }
    }ind_end;
    return found;
}
// -----------------------------------------------------------------------------
void gcli::PushGitBranch(strptr remote_name){
    SysCmd(GitCmd(tempstr()<<"push "<<remote_name<<" HEAD -f"), FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run), algo_EchoQ_true);
}
// -----------------------------------------------------------------------------
void gcli::ParseGitComment(strptr issue_key,tempstr &title,tempstr &description){
    // Load branch comment
    cstring head_meta(Trimmed(SysEval(GitCmd(tempstr()<<"show --quiet --format=\"%B\" "<<issue_key)
                                      ,FailokQ(false), 64*1024)));
    ind_beg(Line_curs,line,head_meta){
        if (title==""){
            title<<line;
        } else {
            description<<line<<eol;
        }
    }ind_end;
    title=tempstr()<<Trimmed(title);
    description=tempstr()<<Trimmed(description);
}
// -----------------------------------------------------------------------------
bool gcli::CheckGitBranchExists(strptr issue_key){
    return SysCmd(GitCmd(tempstr()<<"rev-parse --verify >/dev/null 2>/dev/null "<<issue_key));
}
// -----------------------------------------------------------------------------
void gcli::GitCheckoutBranch(strptr target_branch){
    SysCmd(GitCmd(tempstr()<<"checkout "<<target_branch), FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run));
}
// -----------------------------------------------------------------------------
// Create local branch TARGET_BRANCH tracking SOURCE_BRANCH of the repo remote
void gcli::GitCheckoutBranch(strptr target_branch, strptr source_branch){
    tempstr remote(GitRemote());
    SysCmd(GitCmd(tempstr()<<"fetch "<<remote<<" "<<source_branch)
           , FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run));
    tempstr co_cmd;
    co_cmd << " checkout --track -b "
           << target_branch
           << " "
           << remote<<"/"<<source_branch;
    SysCmd(GitCmd(co_cmd), FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run));
}
// -----------------------------------------------------------------------------
// Create the work branch for ISSUE off the repo default branch and seed it
// with an empty commit binding the branch to the issue.  The branch is the
// git-safe issue key (algornd_arnd.33); the commit carries the issue title,
// the description, and the footer 'closes #<iid> (<branch>)' -- the shape
// gli recognizes, and the server auto-closes the issue on merge.
void gcli::GitCheckoutMasterBranch(gcli::FIssue &issue){
    tempstr branch(GitName(issue.issue));
    gcli::GitCheckoutBranch(branch,gcli::_db.grepo_sel.default_branch);
    // strptr_ToBash is broken, so using temp file
    tempstr iid(iid_Get(issue));
    cstring commit_comment;
    commit_comment << issue.title << eol << eol;
    ind_beg(Line_curs,line,issue.description){
        if (line=="```"){
            // skip -- git commit messages should not have markdown
        } else if (FindStr(line,tempstr()<<"#"<<iid)!=-1){
            // skip -- the footer is the one issue reference in the comment
        } else {
            commit_comment << line << eol;
        }
    }ind_end;
    commit_comment << "closes #" << iid << " (" << branch << ")" << eol;

    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"gcli");
    StringToFile(commit_comment,tempfile.filename);
    tempstr cmt_cmd;
    cmt_cmd << "commit --allow-empty -F "
            << GetFullPath(tempfile.filename)
            << " 1>/dev/null 2>/dev/null";
    if (gcli::_db.cmdline.dry_run){
        prlog(Keyval("commit comment",commit_comment));
    }
    SysCmd(GitCmd(cmt_cmd), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run));
    // publish commit message
    tempstr title;
    tempstr description;
    ParseGitComment(branch,title,description);
    prlog(title);
    prlog(description);
}
// -----------------------------------------------------------------------------
void gcli::GitRemoveMrBranch(strptr mr_branch){
    SysCmd(GitCmd("checkout master"), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run));
    SysCmd(GitCmd(tempstr()<<"branch -D "<<mr_branch), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run));
}
// -----------------------------------------------------------------------------
// Compose per-repo git-remote setup commands.  The remote is named by the
// git-safe repo name (algornd_arnd); an older slash-named remote of the same
// repo is left in place untouched.  An already-configured remote wins: the
// clone's .git/config is authoritative (a hand-corrected host or port must
// survive gitconfig -create), and gcli only adds the missing ones from the
// server-advertised ssh url.
static void SetGitRemote(){
    gcli::LoadGrepo();
    // Set git remote
    algo_lib::Replscope R;
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        tempstr cmd;
        Set(R,"$repo",gcli::GitName(grepo.name));
        Set(R,"$githost",grepo.ssh_repo);
        // Set git remote
        cmd=GitCmd(Subst(R," remote add $repo $githost 2>/dev/null"))<<eol;
        grepo.gitremote_cmd=cmd;
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gtblact_gitconfig_list(gcli::FGtblact&){
    SetGitRemote();

    // Show git remote
    tempstr grep_cmd;
    tempstr cmd;
    algo::ListSep ls("|");
    algo_lib::Replscope R;
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        tempstr gitname(gcli::GitName(grepo.name));
        Set(R,"$repo",grepo.name);
        Set(R,"$gitname",gitname);
        grep_cmd<<ls<<Subst(R,"^$gitname\t");
        // also show the older slash-named remote of the same repo
        if (gitname!=grepo.name){
            grep_cmd<<ls<<Subst(R,"^$repo\t");
        }
    }ind_end;

    cmd=tempstr()<<" remote -v | grep -P '"<<grep_cmd<<"'"; // ignore:hand_quote
    SysCmd(GitCmd(cmd), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run));
}
// -----------------------------------------------------------------------------
void gcli::gtblact_gitconfig_create(gcli::FGtblact &gtblact){
    SetGitRemote();
    // Execute git remote settings
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        SysCmd(grepo.gitremote_cmd, FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(false));
    }ind_end;
    if (!gcli::_db.cmdline.dry_run){
        gcli::gtblact_gitconfig_list(gtblact);
    }
}
