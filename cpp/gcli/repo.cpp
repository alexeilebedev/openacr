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
// Source: cpp/gcli/repo.cpp
//

#include "include/algo.h"
#include "include/gcli.h"
// -----------------------------------------------------------------------------
static void Main_ShowGrepolist(){
    algo_lib::FTxttbl txttbl;
    AddRow(txttbl);
    AddCols(txttbl,"REPO/REMOTE,DFLT,HOST,PRIV,TOKEN");
    tempstr dflt("dflt");
    ind_beg(gcli::_db_grepo_curs, grepo, gcli::_db) {
        if (grepo.select) {
            AddRow(txttbl);
            AddCol(txttbl,grepo.name);
            AddCol(txttbl,dflt);
            AddCol(txttbl,host_Get(grepo));
            AddCol(txttbl,grepo.priv);
            AddCol(txttbl,grepo.token);
        }
        dflt="";
    }ind_end;
    prlog(txttbl);
}
// -----------------------------------------------------------------------------
static void Main_ShowGreporemotelist(){
    Main_ShowGrepolist();
}
// -----------------------------------------------------------------------------
void gcli::gtblact_repo_list(gcli::FGtblact &gtblact){
    gcli::LoadGrepo();
    algo_lib::Regx repo_regx;
    Regx_ReadSql(repo_regx,gtblact.id, false);
    algo_lib::Regx host_regx;
    Regx_ReadSql(host_regx,gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_host), false);
    algo_lib::Regx token_regx;
    Regx_ReadSql(token_regx,gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_token), false);
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        grepo.select=algo_lib::Regx_Match(repo_regx,grepo.name);
        grepo.select&=algo_lib::Regx_Match(host_regx,host_Get(grepo));
        grepo.select&=algo_lib::Regx_Match(token_regx,grepo.token);
    }ind_end;
    Main_ShowGrepolist();
}
// -----------------------------------------------------------------------------
static void GrepoEdit(bool edit){
    // save old auth file
    CreateDirRecurse(GetDirName(gcli::_db.auth_file));
    cstring cmd;
    algo_lib::Replscope R;
    Set(R,"$file",gcli::_db.auth_file);

    // record collected projects for the auth_token
    tempstr out_first;
    tempstr out_last;
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        gclidb::Grepo grepo_out;
        grepo_CopyOut(grepo,grepo_out);
        if (grepo.select){
            out_first<<grepo_out<<eol;
        } else {
            out_last<<grepo_out<<eol;
        }
    }ind_end;
    tempstr out;
    out<<"#"<<eol;
    out<<"#"<<Keyval("NOTE","Move the tuples around to set the default project as the first line")<<eol;
    out<<"#"<<eol;
    out<<out_first<<out_last;

    vrfy_(StringToFile(out,Subst(R,"$file.tmp"),algo::FileFlags(),false));
    errno_vrfy_(!chmod(Zeroterm(Subst(R,"$file.tmp")),0600));

    if (edit){
        vrfy_(gcli::_db.editor.ch_n);
        Set(R,"$editor",gcli::_db.editor);
        cmd=Subst(R,"$editor $file.tmp");
        SysCmd(cmd, FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run));
    }

    Refurbish(cmd);
    Ins(&R,cmd,"touch $file");
    Ins(&R,cmd,"mv $file $file.bkp");
    Ins(&R,cmd,"mv $file.tmp $file");
    SysCmd(cmd, FailokQ(false), DryrunQ(gcli::_db.cmdline.dry_run));

    // read the modified file again
    gcli::grepo_RemoveAll();
    gcli::LoadGrepo();
    //    (void)gcli::LoadSsimfileMaybe(gcli::_db.auth_file);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_token2repos(gcli::FGclicmd &gclicmd){
    // process stanzas/tuples
    ind_beg(gcli::gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form grepo from read tuple
        gclidb::Grepo grepo_in;
        if (Grepo_ReadStrptrMaybe(grepo_in,tuple.tuples)){
            grepo_in.grepo=tempstr()
                <<gclidb::Grepo_Concat_host_fname(gclicmd.host,grepo_in.grepo);
            grepo_in.token=gclicmd.token;
            // Set repo name
            grepo_in.name=fname_Get(grepo_in);
            // Add/replace grepo record
            if (grepo_in.priv!="internal"
                && Regx_Match(gcli::_db.regx_repo,grepo_in.grepo)
                ){
                gcli::FGrepo &grepo=gcli::ind_grepo_GetOrCreate(grepo_in.grepo);
                grepo_CopyIn(grepo,grepo_in);
                grepo.select=true;
            }
        }
    }ind_end;
    // Process the end
    if (gclicmd.write_auth){
        GrepoEdit(false);
    }
}
// -----------------------------------------------------------------------------
void gcli::LoadGrepo(){
    // Backward-Comparibility code
    // Load grepo file
    gcli::_db.auth_file=SsimFname(DirFileJoin(gcli::_db.home,gcli::_db.cmdline.authdir),dmmeta_Ssimfile_ssimfile_gclidb_grepo);
    // if file was not read, read it
    if (gcli::grepo_N()==0){
        (void)gcli::LoadSsimfileMaybe(gcli::_db.auth_file,false);
    }
}
// -----------------------------------------------------------------------------
static bool GrepoSelQ(){
    return gcli::_db.grepo_sel.token!="";
}
// -----------------------------------------------------------------------------
static void GrepoSelect(){
    // Find if an id was specified
    // any id is good to identify the project
    tempstr target;
    ind_beg(gcli::_db_zd_gtblact_curs,gtblact,gcli::_db) {
        target=gtblact.id;
    }ind_end;

    tempstr target_no_dot(Pathcomp(target,".LL"));
    algo_lib::Regx target_regx;
    Regx_ReadSql(target_regx,target_no_dot, false);

    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db) if (grepo.active){
        if (algo_lib::Regx_Match(target_regx,grepo.name)){
            grepo_CopyOut(grepo,gcli::_db.grepo_sel);
            break;
        }
    }ind_end;
    // Select default project
    // pick up first active environment if no match
    if (!GrepoSelQ()){
        ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db) if (grepo.active){
            grepo_CopyOut(grepo,gcli::_db.grepo_sel);
            break;
        }ind_end;
    }
    vrfy(GrepoSelQ(),tempstr()
         <<Keyval("auth_file",gcli::_db.auth_file)
         <<eol
         <<Keyval("comment","no project matching auth_file records is selected by target")
         <<eol
         <<Keyval("comment","to initialize auth_file record please supply personal access token via selector:repo -create command")
         <<eol
         <<Keyval("comment","visit <https://docs.gitlab.com/ee/user/profile/personal_access_tokens.html> for directions")
         );
    verblog("gcli.info"
            <<Keyval("guessed_proj",gcli::_db.grepo_sel.grepo)
            );

}
// -----------------------------------------------------------------------------
static void SetGtype(strptr auth_token){
    ind_beg(gcli::_db_gtypeprefix_curs,gtypeprefix,gcli::_db){
        if (StartsWithQ(auth_token,prefix_Get(gtypeprefix))){
            gcli::_db.p_gtype=gtypeprefix.p_gtype;
        }
    }ind_end;
    if (!gcli::_db.p_gtype){
        vrfy(gcli::_db.p_gtype, tempstr()
             <<Keyval("gtype",auth_token)
             <<Keyval("comment","unsupported gtype")
             );
    }
}

// -----------------------------------------------------------------------------
static void AuthInit(gcli::FGtblact &gtblact){
    tempstr token(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_token));
    tempstr host(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_host));

    gcli::_db.grepo_sel.token=token;
    SetGtype(gcli::_db.grepo_sel.token);
    // try to find default host if absent
    if (host==""){
        host=gcli::_db.p_gtype->default_url;
    }
    verblog("gcli.info "
            <<Keyval("token",token)
            <<Keyval("host",host)
            );
    gcli::_db.grepo_sel.grepo=gclidb::Grepo_Concat_host_fname(host,"none");
}
// -----------------------------------------------------------------------------
void gcli::Main_ManageAuth(){
    LoadGrepo();

    gcli::FGtblact *gtblact_init=gcli::ind_gtblact_Find(gclidb_Gtblact_gtblact_repo_create);
    gcli::FGtblact *gtblact_show=gcli::ind_gtblact_Find(gclidb_Gtblact_gtblact_reporemote_list);
    //    Set up gcli::_db.grepo_sel
    if (gtblact_init->select) {
        AuthInit(*gtblact_init);
    } else if (gtblact_show->select){
        AuthInit(*gtblact_show);
    } else if (gcli::_db.need_auth){
        GrepoSelect();
        SetGtype(gcli::_db.grepo_sel.token);
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_repo_create(gcli::FGtblact &gtblact){
    // check if the default repo is specified
    tempstr default_repo(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_default_repo));
    // TODO: make -e option work? Worth it?
    // update gtblactfld uval with edits
    // if (gcli::_db.cmdline.e) {
    //     // need to mark available fields to edit...
    //     gclidb::Grepo grepo_out;
    //     TupleToGtblactflds(gtblact,tempstr()<<grepo_out);

    //     if (!EditGtblactfld(gtblact,"")){
    //         prlog("Grepo create has not been submitted, you can continue composing it.");
    //         return;
    //     }
    // }
    if (default_repo!=""){
        // Append auth_set_default into the queue
        zd_gtblact_Extend(gclidb_Gtblact_gtblact_repo_update,gtblact.id);
    } else {
        // Append show into the queue
        zd_gtblact_Extend(gclidb_Gtblact_gtblact_repo_list,gtblact.id);
    }
    // set up repo regex
    tempstr repo(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_repo));
    if (repo!=""){
        Regx_ReadSql(gcli::_db.regx_repo,repo, false);
    }

    gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_token2repos,true,"");
    gclicmd.write_auth=true;
    gcli::Main_CurlExec();
}
// -----------------------------------------------------------------------------
void gcli::gtblact_reporemote_list(gcli::FGtblact &gtblact){
    gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_token2repos,true,"");
    gclicmd.write_auth=false;

    // set up repo regex
    tempstr repo(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_repo));
    if (repo!=""){
        Regx_ReadSql(gcli::_db.regx_repo,repo, false);
    }
    gcli::grepo_RemoveAll();
    gcli::Main_CurlExec();
    Main_ShowGreporemotelist();
}
// -----------------------------------------------------------------------------
void gcli::gtblact_repo_update(gcli::FGtblact &gtblact){
    // extract field
    tempstr repo(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_default_repo));

    // LoadGrepo();
    // GrepoEdit(true);

    // Insert auth_show into the queue
    zd_gtblact_Extend(gclidb_Gtblact_gtblact_repo_list,gtblact.id);
    // and set selection to %
    gcli::SetGtblactfldDflt(gclidb_Gtblact_gtblact_repo_list,gclidb_Gfld_gfld_repo);
    // record collected projects for the auth_token
    gcli::LoadGrepo();
    tempstr out_first;
    tempstr out_last;
    ind_beg(gcli::_db_grepo_curs,grepo,gcli::_db){
        gclidb::Grepo grepo_out;
        grepo_CopyOut(grepo,grepo_out);
        if (grepo.name==repo){
            out_first<<grepo_out<<eol;
        } else {
            out_last<<grepo_out<<eol;
        }
    }ind_end;
    vrfy(out_first!="" || gcli::_db.cmdline.e,tempstr()
         <<Keyval("repo",repo)
         <<Keyval("comment","specified remote is not in the list")
         );

    tempstr out;
    out<<out_first<<out_last;
    vrfy_(StringToFile(out,gcli::_db.auth_file,algo::FileFlags(),false));
    // force new read
    gcli::grepo_RemoveAll();

    if (gcli::_db.cmdline.e){
        LoadGrepo();
        GrepoEdit(true);
    }
}
