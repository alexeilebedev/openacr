// Copyright (C) 2023-2024 AlgoRND
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
void gcli::CheckGitBranchExists(strptr issue_key){
    vrfy(SysCmd(GitCmd(tempstr()<<"rev-parse --verify >/dev/null 2>/dev/null "<<issue_key)), tempstr()
         <<Keyval("branch",issue_key)
         <<Keyval("comment","branch exists")
         );
}
// -----------------------------------------------------------------------------
void gcli::GitCheckoutBranch(strptr target_branch, strptr source_branch){
    SysCmd(GitCmd(tempstr()<<"fetch "<<gcli::_db.grepo_sel.name<<" "<<source_branch)
           , FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(algo_lib::_db.cmdline.verbose));
    tempstr co_cmd;
    co_cmd << " checkout --track -b "
           << target_branch
           << " "
           << gcli::_db.grepo_sel.name<<"/"<<source_branch;
    SysCmd(GitCmd(co_cmd), FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(algo_lib::_db.cmdline.verbose));
}
// -----------------------------------------------------------------------------
void gcli::GitCheckoutMasterBranch(gcli::FIssue &issue){
    gcli::GitCheckoutBranch(issue.issue,gcli::_db.grepo_sel.default_branch);
    // strptr_ToBash is broken, so using temp file
    cstring commit_comment;
    // # in gitlab is a markdown sign!
    tempstr issue_issue(issue.issue);
    Replace(issue_issue,".","#");
    commit_comment << "Issue " <<issue_issue << " " << issue.title
                   << "\n\n" << issue.description
                   << "\n\ncloses " << issue_issue;

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
    SysCmd(GitCmd(cmt_cmd), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(algo_lib::_db.cmdline.verbose));
    // publish commit message
    tempstr title;
    tempstr description;
    ParseGitComment(issue.issue,title,description);
    prlog(title);
    prlog(description);
}
// -----------------------------------------------------------------------------
void gcli::GitRemoveMrBranch(strptr mr_branch){
    SysCmd(GitCmd("checkout master"), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(algo_lib::_db.cmdline.verbose));
    SysCmd(GitCmd(tempstr()<<"branch -D "<<mr_branch), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(algo_lib::_db.cmdline.verbose));
}
// -----------------------------------------------------------------------------
static gcli::FGrepogitport& GetGrepogitport(strptr host){
    gcli::FGrepogitport *grepogitport=gcli::ind_grepogitport_Find(host);
    if (!grepogitport){
        grepogitport=gcli::ind_grepogitport_Find(gclidb_Grepogitport_grepogitport_default);
    }
    return *grepogitport;
}
// -----------------------------------------------------------------------------
static void SetSshConfig(){
    // Set ssh config for git remote
    algo_lib::Replscope R;
    Set(R,"$$","$");
    Set(R,"$HOME",gcli::_db.home);
    ind_beg(gcli::_db_githost_curs,githost,gcli::_db){
        // clear ssh_cong, it can be called twice
        githost.ssh_config="";
        tempstr cmd;
        Ins(&R,cmd,"mkdir -p $HOME/.ssh/config.d");
        Ins(&R,cmd,"touch $HOME/.ssh/config");

        Set(R,"$HOSTSSH",githost.githost);
        Set(R,"$IP",githost.ip);
        Set(R,"$PORT",githost.port);
        Set(R,"$ID",githost.id);
        // Remove name from ssh config
        Ins(&R,cmd,"sed -i -e '/$HOSTSSH[[:blank:]]*$$/d' $HOME/.ssh/config");
        // Add name to ssh config
        Ins(&R,cmd,"echo \"Include ~/.ssh/config.d/$HOSTSSH\" >> $HOME/.ssh/config");
        // Remove config file
        Ins(&R,cmd,"rm -f $HOME/.ssh/config.d/$HOSTSSH");
        // Form config file
        Ins(&R,cmd,"cat << 'EOFGCLI' > $HOME/.ssh/config.d/$HOSTSSH");
        Ins(&R,githost.ssh_config,"# Generated by gcli");
        Ins(&R,githost.ssh_config,"Host $HOSTSSH");
        Ins(&R,githost.ssh_config,"HostName $IP");
        Ins(&R,githost.ssh_config,"Port $PORT");
        Ins(&R,githost.ssh_config,"IdentityFile ~/.ssh/$ID");
        Ins(&R,githost.ssh_config,"StrictHostKeyChecking no");
        Ins(&R,githost.ssh_config,"UserKnownHostsFile=/dev/null");
        Ins(&R,githost.ssh_config,"LogLevel=QUIET");
        cmd<<githost.ssh_config<<eol;
        Ins(&R,cmd,"EOFGCLI");
        if (githost.id_file!=""){
            Set(R,"$sshfile",githost.id_file);
            Ins(&R,cmd,"chmod go-rwx $sshfile; ln -srf $sshfile ~/.ssh/$ID");
        }
        githost.ssh_cmd=cmd;
    }ind_end;
}
// -----------------------------------------------------------------------------
static void SetGitRemote(){
    gcli::LoadGrepo();
    // Set git remote
    algo_lib::Replscope R;
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        tempstr cmd;
        // Find a ssh port for git remote
        gclidb::Githost githost_in;
        githost_in.githost=grepo.ssh_repo;
        tempstr host(host_Get(githost_in));
        gcli::FGrepogitport &grepogitport=GetGrepogitport(host);
        Set(R,"$host",host);
        Set(R,"$sshport",grepogitport.port);

        // Find a name like id_rsa
        tempstr repo(grepo.name);
        Set(R,"$repo",repo);
        Set(R,"$sshid","id_rsa");
        Set(R,"$sshfile","");
        if (gcli::FGrepossh *grepossh=gcli::ind_grepossh_Find(repo)){
            Set(R,"$sshid",name_Get(*grepossh));
            Set(R,"$sshfile",grepossh->sshid);
        }

        host=Subst(R,"$host_$sshport_$sshid");
        Set(R,"$githost",gclidb::Githost_Concat_git_host_gitrepo(git_Get(githost_in),host,gitrepo_Get(githost_in)));
        gcli::FGithost &githost=gcli::ind_githost_GetOrCreate(host);
        githost.ip=Subst(R,"$host");
        githost.port=Subst(R,"$sshport");
        githost.id=Subst(R,"$sshid");
        githost.id_file=Subst(R,"$sshfile");

        // Set git remote
        cmd=GitCmd(Subst(R," remote remove $repo 2>/dev/null"))<<eol;
        cmd<<GitCmd(Subst(R," remote add $repo $githost 2>/dev/null"))<<eol;
        grepo.gitremote_cmd=cmd;
    }ind_end;

    SetSshConfig();
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
        tempstr repo(grepo.name);
        Set(R,"$repo",repo);
        grep_cmd<<ls<<Subst(R,"^$repo\t");
    }ind_end;

    cmd=tempstr()<<" remote -v | grep -P '"<<grep_cmd<<"'";
    SysCmd(GitCmd(cmd), FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), algo::EchoQ(algo_lib::_db.cmdline.verbose));
    prlog("");
    prlog("$HOME/.ssh/config/config.d settings");
    ind_beg(gcli::_db_githost_curs,githost,gcli::_db){
        prlog(githost.ssh_config<<eol);
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gtblact_gitconfig_create(gcli::FGtblact &gtblact){
    SetGitRemote();
    // Execute git remote settings
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        SysCmd(grepo.gitremote_cmd, FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(false));
    }ind_end;
    // Execute githost ssh remote settings
    ind_beg(gcli::_db_githost_curs,githost,gcli::_db){
        SysCmd(githost.ssh_cmd, FailokQ(true), DryrunQ(gcli::_db.cmdline.dry_run), EchoQ(false));
    }ind_end;
    // Display the resutls

    if (!gcli::_db.cmdline.dry_run){
        gcli::gtblact_gitconfig_list(gtblact);
    }
}
