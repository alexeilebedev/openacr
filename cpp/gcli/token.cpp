// Copyright (C) 2026 AlgoRND
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
// Source: cpp/gcli/token.cpp
//

#include "include/algo.h"
#include "include/gcli.h"
// -----------------------------------------------------------------------------
// Print selected token records; the secret appears only in a create response
// (list responses never carry it)
void gcli::Main_ShowTokenlist(){
    algo_lib::FTxttbl txttbl;
    AddRow(txttbl);
    AddCols(txttbl,"TOKEN,NAME,ACTIVE,REVOKED,EXPIRES,SCOPES");
    ind_beg(gcli::_db_token_curs, token, gcli::_db) if (token.select){
        AddRow(txttbl);
        AddCol(txttbl,token.token);
        AddCol(txttbl,token.name);
        AddCol(txttbl,token.active);
        AddCol(txttbl,token.revoked);
        AddCol(txttbl,token.expires);
        AddCol(txttbl,token.scopes);
    }ind_end;
    prlog(txttbl);
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_tokenlist(gcli::FGclicmd &gclicmd){
    // process stanza
    ind_beg(gclicmd_c_tuples_curs,tuple,gclicmd){
        // Form token from read tuple
        gclidb::Token token_in;
        if (Token_ReadStrptrMaybe(token_in,tuple.tuples)){
            token_in.token=gclidb::Token_Concat_project_iid(gclicmd.proj,token_in.token);
            // Add/replace token record
            gcli::FToken &token=gcli::ind_token_GetOrCreate(token_in.token);
            token_CopyIn(token,token_in);
            token.select=true;
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_tokenadd(gcli::FGclicmd &gclicmd){
    gcli::gclicmd_tokenlist(gclicmd);
}
// -----------------------------------------------------------------------------
// Revoke returns 204 with no body; nothing to parse
void gcli::gclicmd_tokendel(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::gtblact_token_list(gcli::FGtblact &gtblact){
    AddGclicmd(gclidb_Gclicmd_gclicmd_tokenlist,true,"");
    gcli::Main_CurlExec();

    algo_lib::Regx token_regx;
    Regx_ReadSql(token_regx,gtblact.id, false);
    ind_beg(gcli::_db_token_curs,token,gcli::_db){
        token.select&=algo_lib::Regx_Match(token_regx,token.token);
    }ind_end;

    gcli::Main_ShowTokenlist();
}
// -----------------------------------------------------------------------------
// Create a temporary project access token: scope api, role Developer is
// implied by the credential, expires the day after tomorrow (the server
// retires it even if nobody revokes it).  The created token record is
// printed as an ssim tuple -- the one response that carries the secret.
void gcli::gtblact_token_create(gcli::FGtblact &gtblact){
    tempstr name(gcli::GetTblactfld(gtblact.gtblact,gclidb_Gfld_gfld_name));
    algo::TimeStruct expires_ts(algo::GetLocalTimeStruct(algo::CurrUnTime()+algo::UnDiffSecs(i64(2*86400))));
    tempstr expires;
    TimeStruct_Print(expires_ts,expires,"%Y-%m-%d");

    lib_json::FNode &obj = lib_json::NewObjectNode(NULL);
    lib_json::NewStringNode(&obj,"name",name);
    lib_json::NewStringNode(&obj,"expires_at",expires);
    lib_json::FNode &ary = lib_json::NewArrayNode(&obj,"scopes");
    lib_json::NewStringNode(&ary,"","api");

    gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_tokenadd,true,"");
    lib_json::JsonSerialize(&obj,gclicmd.body);
    gcli::Main_CurlExec();

    // print the created record: the tuple carries the secret
    ind_beg(gcli::_db_token_curs,token,gcli::_db) if (token.select){
        gclidb::Token token_out;
        token_CopyOut(token,token_out);
        prlog(token_out);
    }ind_end;
}
// -----------------------------------------------------------------------------
// Revoke the selected token (gcli token:<proj>.<id> -stop)
void gcli::gtblact_token_stop(gcli::FGtblact &gtblact){
    tempstr id(gtblact.id);
    VerifySingleIssue(id);
    AddGclicmd(gclidb_Gclicmd_gclicmd_tokendel,true,gcli::IssueArgNumber(id,""));
    gcli::Main_CurlExec();
    prlog("gcli.token_revoked "<<Keyval("token",GetTargetKey(id)));
}
