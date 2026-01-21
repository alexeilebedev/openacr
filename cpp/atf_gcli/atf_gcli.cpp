// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: atf_gcli (exe) -- test harness for gcli
// Exceptions: yes
// Source: cpp/atf_gcli/atf_gcli.cpp
//

#include "include/algo.h"
#include "include/atf_gcli.h"

//
// This is a driver for gcli test
//
// Assumptions for gcli
//
// Gitlab
// 1. A private project "glpat" is created
// 2. A project "access token" key for the project is generated: glpat-Hf38RB2J9GUZRhc54FCs - shows only this project
// Token is converted to base64 file in test/gcli/github_token.base64
// <token> | base64 >test/gcli/gitlab_token.base64
// 3. A public/private key for the project "deploy key" is used
// 4. ~/.ssh/config is modified to use the key for host "glpat?"
//
// Github
// 1. A private repo "vparizhs/ghp" is created (can be a different user?)
// 2. A "fine grained" "access token" key for the project is generated, it is pretty long
//  - also shows public projects r/o!
//  Token is converted to base64 file in test/gcli/github_token.base64
// <token> | base64 test/gcli/github_token.base64
// 3. A public/private key for the repo "deploy key" is used
// 4. ~/.ssh/config is modified to use the key for host "ghp?"
//
// -----------------------------------------------------------------------------
static atf_gcli::FGclienvsub& gclienvsub_Get(strptr gclienv_key,strptr gclisub_key){
    // tempstr gclienvsub;
    // gclienvsub=gclidb::Gclienvsub_Concat_gclienv_sub(gtblactt.gclienv,gclisub_key);
    return atf_gcli::ind_gclienvsub_FindX(gclidb::Gclienvsub_Concat_gclienv_sub(gclienv_key,gclisub_key));
}
// -----------------------------------------------------------------------------
static bool CheckIssue(strptr repo, strptr token_in){
    tempstr token(token_in);
    tempstr repo2(Pathcomp(token,".LL"));
    // Found some id at the start of the line
    u32 num_id;
    tempstr issue_id(Pathcomp(token,".LR"));
    // is it aaa.num (issue) or aaa.num.num (note)?
    // issue id?
    return
        repo==repo2
        && u32_ReadStrptrMaybe(num_id,issue_id)
        && (num_id!=0)
        && (issue_id==tempstr()<<num_id)
        ;
}
// -----------------------------------------------------------------------------
static bool CheckNote(strptr repo, strptr token_in){
    bool ret(false);
    // Found some id at the start of the line
    tempstr token(token_in);
    tempstr repo2(Pathcomp(token,".LL"));
    if (repo==repo2){
        // is it aaa.num (issue) or aaa.num.num (note)?
        // issue id?
        u32 num_id;
        tempstr issue_id(Pathcomp(token,".LR"));
        if (u32_ReadStrptrMaybe(num_id,issue_id)
            && (num_id!=0)
            && (issue_id==tempstr()<<num_id)
            ) {
        } else if (u32_ReadStrptrMaybe(num_id,Pathcomp(token,".RR")) && (num_id!=0)){
            ret=true;
        }
    }
    return ret;
}
// -----------------------------------------------------------------------------
static bool CheckMr(strptr repo, strptr token_in){
    tempstr ret;
    tempstr token(token_in);
    return
        Replace(token,"mr:","")
        && CheckIssue(repo,token)
        ;
}
// -----------------------------------------------------------------------------
static bool CheckMrNote(strptr repo, strptr token_in){
    tempstr ret;
    tempstr token(token_in);
    return
        Replace(token,"mr:","")
        && CheckNote(repo,token)
        ;
}
// -----------------------------------------------------------------------------
static bool CheckPound(strptr repo, strptr token_in){
    tempstr token(token_in);
    return
        Replace(token,"#","")
        && CheckIssue(repo,token)
        ;
}
// -----------------------------------------------------------------------------
static tempstr ParseLineTokens(atf_gcli::FGtblacttst &gtblacttst,strptr out_in){
    atf_gcli::FGclienvsub &gclienvsub_repo=gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__REPO);
    tempstr repo(gclienvsub_repo.uval);
    tempstr line_out;
    algo::ListSep ls(" ");
    ind_beg(algo::Sep_curs,line,out_in,' ') if (line!=""){
        tempstr token_out(line);
        if (CheckIssue(repo,line)){
            token_out="$ISSUE";
            if (gtblact_Get(gtblacttst)==gclidb_Gtblact_gtblact_issue_create){
                gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__ISSUE).uval=line;
                tempstr issue_(line);
                Replace(issue_,".","#");
                gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__ISSUE_).uval=issue_;
            }
            verblog(Keyval("ISSUE",line));
        } else if (CheckNote(repo,line)){
            token_out="$NOTE";
            if (gtblact_Get(gtblacttst)==gclidb_Gtblact_gtblact_issuenote_create){
                gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__NOTE).uval=line;
            }
            verblog(Keyval("NOTE",line));
        } else if (CheckMrNote(repo,line)){
            token_out="$MNOTE";
            if (gtblact_Get(gtblacttst)==gclidb_Gtblact_gtblact_mrnote_create){
                tempstr repl(line);
                gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__MNOTE).uval=tempstr()<<"note_"<<line;
            }
            verblog(Keyval("MNOTE",line));
        } else if (CheckMr(repo,line)){
            token_out="$MR";
            if (gtblact_Get(gtblacttst)==gclidb_Gtblact_gtblact_mr_create){
                gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__MR).uval=line;
                tempstr mr_(line);
                Replace(mr_,":","_");
                gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__MR_).uval=mr_;
            }
            verblog(Keyval("MR",line));
        } else if (CheckPound(repo,line)){
            token_out="#$ISSUE";
            verblog(Keyval("#",line));
        }
        line_out<<ls<<token_out;
    }ind_end;
    return line_out;
}
// -----------------------------------------------------------------------------
static void CaptureArtefacts(atf_gcli::FGtblacttst &gtblacttst){
    u32 num(1);
    ind_beg(algo::FileLine_curs,line,atf_gcli::_db.tempfile.filename) if (line!=""){
        tempstr gtblacttstout_key;
        tempstr num_out;
        i64_PrintPadLeft(num++,num_out,2);
        gtblacttstout_key=
            gclidb::Gtblacttstout_Concat_gtblacttst_out(gtblacttst.gtblacttst,tempstr()<<num_out);
        atf_gcli::FGtblacttstout *gtblacttstout=atf_gcli::ind_gtblacttstout_Find(gtblacttstout_key);
        if (!gtblacttstout){
            gtblacttstout=&atf_gcli::gtblacttstout_Alloc();
            gtblacttstout->gtblacttstout=gtblacttstout_key;
        }
        vrfy(gtblacttstout_XrefMaybe(*gtblacttstout),algo_lib::_db.errtext);
        // remove issue number from the line if present
        tempstr out(line);
        Replace(out,"\t"," ");
        out=tempstr()<<ParseLineTokens(gtblacttst,out);
        // Reverse substitution
        ind_beg(atf_gcli::gclienv_c_gclienvsub_curs,gclienvsub,*gtblacttst.p_gclienv) if (gclienvsub.rev){
            Replace(out,gclienvsub.uval,sub_Get(gclienvsub));
        }ind_end;

        gtblacttstout->tout=out;
    }ind_end;
}
// -----------------------------------------------------------------------------
static void DiffCapture(atf_gcli::FGtblacttst &gtblacttst){
    ind_beg(atf_gcli::gtblacttst_c_gtblacttstout_curs,gtblacttstout,gtblacttst) {
        tempstr in1(gtblacttstout.text);
        tempstr in2(gtblacttstout.tout);
        Replace(in1," ","");
        Replace(in2," ","");
        gtblacttst.fail|=in1!=in2;
    }ind_end;
    prlog("atf_gcli.result "
          <<Keyval("test",gtblacttst.gtblacttst)
          <<Keyval("passed",!gtblacttst.fail)
          <<Keyval("comment",gtblacttst.comment)
          );
}
// -----------------------------------------------------------------------------
static void PrintCapture(atf_gcli::FGtblacttst &gtblacttst){
    tempstr out;
    ind_beg(atf_gcli::gtblacttst_c_gtblacttstout_curs,gtblacttstout,gtblacttst) if (gtblacttstout.tout!=""){
        gclidb::Gtblacttstout gtblacttstout_out;
        gtblacttstout_CopyOut(gtblacttstout,gtblacttstout_out);
        gtblacttstout_out.text=gtblacttstout.tout;
        out<<gtblacttstout_out<<eol;
    }ind_end;
    prlog(out);
}
// -----------------------------------------------------------------------------
static void PrintStats(){
    u32 n_failed(0);
    ind_beg(atf_gcli::_db_c_gtblacttst_curs,gtblacttst,atf_gcli::_db) {
        n_failed+=gtblacttst.fail;
        if (gclienv_Get(gtblacttst)==gclidb_Gclienv_gclienv_ghub){
            gtblacttst.p_gtblact->ghub_run++;
            gtblacttst.p_gtblact->ghub_fail+=gtblacttst.fail;
        }
        if (gclienv_Get(gtblacttst)==gclidb_Gclienv_gclienv_glab
            || gclienv_Get(gtblacttst)==gclidb_Gclienv_gclienv_all){
            gtblacttst.p_gtblact->glab_run++;
            gtblacttst.p_gtblact->glab_fail+=gtblacttst.fail;
        }
    }ind_end;
    prlog("atf_gcli.report "
          <<Keyval("tests",atf_gcli::c_gtblacttst_N())
          <<Keyval("success",atf_gcli::c_gtblacttst_N() - n_failed)
          <<Keyval("failure",n_failed)
          );

    ind_beg(atf_gcli::_db_gtblact_curs,gtblact,atf_gcli::_db){
        prlog(Keyval("glab_run",gtblact.glab_run)
              <<Keyval("glab_fail",gtblact.glab_fail)
              <<Keyval("ghub_run",gtblact.ghub_run)
              <<Keyval("ghub_fail",gtblact.ghub_fail)
              <<Keyval("gtblact",gtblact.gtblact)
              );
    }ind_end;
    vrfy(n_failed==0,"atf_gcli failed some tests");
}
// -----------------------------------------------------------------------------
static void PrepGitdir(){
    ind_beg(atf_gcli::_db_gclienv_curs,gclienv,atf_gcli::_db) if (gclienv.gclienv!="all"){
        tempstr repo(gclienvsub_Get(gclienv.gclienv,gclidb_Gclisub_gclisub__REPO).uval);
        tempstr gitdir(gclienvsub_Get(gclienv.gclienv,gclidb_Gclisub_gclisub__GITDIR).uval);
        tempstr idrsa_file(gclienvsub_Get(gclienv.gclienv,gclidb_Gclisub_gclisub__ID_RSA).uval);
        if (repo!=""){
            tempstr cmd;
            algo_lib::Replscope R;

            Set(R,"$proj",repo);
            Set(R,"$gitdir",gitdir);
            Set(R,"$std","");
            if (!algo_lib::_db.cmdline.verbose){
                Set(R,"$std","1>/dev/null 2>/dev/null");
            }

            Ins(&R,cmd,"rm -fr temp/.$gitdir $std");
            Ins(&R,cmd,"mkdir -p temp/.$gitdir");
            Ins(&R,cmd,"git -C temp/.$gitdir init $std");
            // set the private key for the project
            Set(R,"$idrsa_file",idrsa_file);
            Ins(&R,cmd,"cat $idrsa_file | base64 -d > temp/.$gitdir/id_rsa");
            Ins(&R,cmd,"chmod go-rwx temp/.$gitdir/id_rsa");
            // point git to the private key
            Set(R,"$PWD",getenv("PWD"));
            Ins(&R,cmd,"git -C temp/.$gitdir config core.sshCommand \"ssh -i $PWD/temp/.$gitdir/id_rsa\"");
            Ins(&R,cmd,"echo \"# atf_gcli $proj\" >> temp/.$gitdir/README.md");
            Ins(&R,cmd,"git  -C temp/.$gitdir add README.md $std");
            Ins(&R,cmd,"git  -C temp/.$gitdir commit -m \"first commit\" $std");
            Ins(&R,cmd,"git  -C temp/.$gitdir branch -M master $std");
            Ins(&R,cmd,"git -C temp/.$gitdir push -u $proj master -f $std");

            Ins(&R,cmd,"mkdir -p temp/.$gitdir/temp");
            SysCmd(cmd,FailokQ(false),DryrunQ(atf_gcli::_db.cmdline.dry_run));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_gcli::DebugDump() {
    tempstr ghub_out;
    tempstr glab_out;
    tempstr all_out;
    ind_beg(atf_gcli::_db_gtblacttst_curs,gtblacttst,atf_gcli::_db) {
        gclidb::Gtblacttst gtblacttst_out;
        gtblacttst_CopyOut(gtblacttst,gtblacttst_out);
        tempstr left(Pathcomp(gtblacttst_out.gtblacttst,".LL"));
        tempstr right(Pathcomp(gtblacttst_out.gtblacttst,".LR"));
        gtblacttst_out.gtblacttst=left<<"."<<gclienv_Get(gtblacttst_out)<<"."<<right;
        if (gclienv_Get(gtblacttst_out)=="ghub"){
            ghub_out<<gtblacttst_out<<eol;
        } else if (gclienv_Get(gtblacttst_out)=="glab"){
            glab_out<<gtblacttst_out<<eol;
        } else {
            all_out<<gtblacttst_out<<eol;
        }

        ind_beg(atf_gcli::gtblacttst_c_gtblacttstout_curs,gtblacttstout,gtblacttst){
            gclidb::Gtblacttstout gtblacttstout_out;
            gtblacttstout_CopyOut(gtblacttstout,gtblacttstout_out);

            gtblacttstout_out.gtblacttstout=tempstr()<<gclidb::Gtblacttstout_Concat_gtblacttst_out(gtblacttst_out.gtblacttst,out_Get(gtblacttstout));
            prlog(gtblacttstout_out);
        }ind_end;
    }ind_end;
    prlog(all_out);
    prlog(glab_out);
    prlog(ghub_out);
}

// -----------------------------------------------------------------------------
//Github doesn't like to see its tokens visible, even though these are "test only" tokens
static void RecoverTokenFromBase64(){
    ind_beg(atf_gcli::_db_gclienvsub_curs,gclienvsub,atf_gcli::_db) if (sub_Get(gclienvsub)==gclidb_Gclisub_gclisub__TOKEN){
        tempstr cmd("base64 -d ");
        cmd<<gclienvsub.uval;
        gclienvsub.uval=Trimmed(SysEval(cmd,FailokQ(false),1024*1024));
        verblog(Keyval("TOKEN",gclienvsub.uval));
    }ind_end;
}
// -----------------------------------------------------------------------------
void atf_gcli::Main() {
    RecoverTokenFromBase64();

    if (algo_lib::_db.cmdline.debug) {
        DebugDump();
    }
    if (!atf_gcli::_db.cmdline.skip_git_init){
        PrepGitdir();
    }

    TempfileInitX(atf_gcli::_db.tempfile,"atf_gcli");
    // Initalize some preliminary executions
    // Select tests
    ind_beg(atf_gcli::_db_gtblacttst_curs,gtblacttst,atf_gcli::_db) if (algo_lib::Regx_Match(atf_gcli::_db.cmdline.gtblacttst,gtblacttst.gtblacttst)
                                                                        && gtblacttst.working){
        // Always do auth_init first
        if (!atf_gcli::_db.cmdline.skip_init && gtblacttst.p_gclienv->gclienv!="all"){
            c_gtblacttst_InsertMaybe(ind_gtblacttst_FindX(tempstr()<<gclidb_Gtblact_gtblact_repo_create<<"."<<gtblacttst.p_gclienv->gclienv<<"."<<gtblacttst.p_gclienv->gclienv));
            c_gtblacttst_InsertMaybe(ind_gtblacttst_FindX(tempstr()<<gclidb_Gtblact_gtblact_gitconfig_create<<"."<<gtblacttst.p_gclienv->gclienv<<"."<<gtblacttst.p_gclienv->gclienv));
        }
        // Tests that need id or note_id have to have new issues created first
        if (atf_gcli::_db.cmdline.id==""){
            if (Replace(gtblacttst.cmd,"$ISSUE","$ISSUE")){
                c_gtblacttst_InsertMaybe(ind_gtblacttst_FindX(tempstr()<<gclidb_Gtblact_gtblact_issue_create<<"."<<gtblacttst.p_gclienv->gclienv<<"."<<gtblacttst.p_gclienv->gclienv));
            }
            if (Replace(gtblacttst.cmd,"$NOTE","$NOTE")){
                c_gtblacttst_InsertMaybe(ind_gtblacttst_FindX(tempstr()<<gclidb_Gtblact_gtblact_issue_create<<"."<<gtblacttst.p_gclienv->gclienv<<"."<<gtblacttst.p_gclienv->gclienv));
                c_gtblacttst_InsertMaybe(ind_gtblacttst_FindX(tempstr()<<gclidb_Gtblact_gtblact_issuenote_create<<"."<<gtblacttst.p_gclienv->gclienv<<"."<<gtblacttst.p_gclienv->gclienv));
            }
        }
    }ind_end;

    // Select tests
    ind_beg(atf_gcli::_db_gtblacttst_curs,gtblacttst,atf_gcli::_db) if (algo_lib::Regx_Match(atf_gcli::_db.cmdline.gtblacttst,gtblacttst.gtblacttst)
                                                                        && gtblacttst.working){
        c_gtblacttst_InsertMaybe(gtblacttst);
    }ind_end;

    // Run selected tests
    ind_beg(atf_gcli::_db_c_gtblacttst_curs,gtblacttst,atf_gcli::_db) {
        // Form command
        tempstr table(gtbl_Get(*gtblacttst.p_gtblact));
        gtblacttst.cmd=tempstr()
            <<gtblacttst.cmd
            <<" -"<<gact_Get(*gtblacttst.p_gtblact)
            <<" "<<gtblacttst.p_gclienv->addon
            <<" >"<<atf_gcli::_db.tempfile.filename;
        Replace(gtblacttst.cmd,"$TABLE",table);

        if (atf_gcli::_db.cmdline.id!=""){
            gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__ISSUE).uval=atf_gcli::_db.cmdline.id;
        }
        if (atf_gcli::_db.cmdline.note!=""){
            gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__NOTE).uval=atf_gcli::_db.cmdline.note;
        }
        if (atf_gcli::_db.cmdline.mr!=""){
            gclienvsub_Get(gclienv_Get(gtblacttst),gclidb_Gclisub_gclisub__MR).uval=atf_gcli::_db.cmdline.mr;
        }
        // Do env etc substitutions
        ind_beg(atf_gcli::gclienv_c_gclienvsub_curs,gclienvsub,*gtblacttst.p_gclienv) if (gclienvsub.fwd) {
            Replace(gtblacttst.cmd,sub_Get(gclienvsub),gclienvsub.uval);
        }ind_end;
        // Issue command
        SysCmd(gtblacttst.cmd,FailokQ(true),DryrunQ(atf_gcli::_db.cmdline.dry_run));
        // Capture artefacts
        CaptureArtefacts(gtblacttst);
        // Diff capture
        if (!atf_gcli::_db.cmdline.capture){
            DiffCapture(gtblacttst);
            // Print capture
        } else {
            PrintCapture(gtblacttst);
        }
    }ind_end;

    if (!atf_gcli::_db.cmdline.capture){
        PrintStats();
    }
}
