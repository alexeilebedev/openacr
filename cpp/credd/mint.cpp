// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: credd (exe) -- Hold a session's credentials unlocked in memory and hand them to the tools that authenticate
// Exceptions: yes
// Source: cpp/credd/mint.cpp
//
// The verb -mint, the rotation -renew asks for, and the steps text the two
// share.
// A tool in this tree names the credential it asks credd for in dev.cred, and
// says what that credential must be able to do in dev.credrole and
// dev.credperm: gli-mr is a group token acting as a developer with the api
// scope.  -mint reads those rows from the tree and adds the credential to the
// store with exactly that kind, role and set of permissions, so the person
// making it never chooses among the provider's scopes.
// Where the kind has an endpoint that mints (creddb.kind minturl), credd asks
// the provider with the person's own gitlab token from the store, and GitLab
// decides whether that person may: a group token needs an Owner of the group.
// Where the kind has no such endpoint, where the store holds no token to ask
// with, or where the provider refuses, credd prints the steps for a person --
// the request to forward to an Owner -- and reads the token they send back.
// A token issued with the self_rotate permission renews the same way: -renew
// asks the provider to rotate it, authenticated by the token itself, and falls
// back to the steps when the provider refuses.

#include "include/algo.h"
#include "include/credd.h"
#include "include/lib_curl.h"
#include "include/lib_json.h"

// Read the row of dev.cred named NAME from the tree's ssimfile directory DIR
// into OUT_CRED, the role dev.credrole gives it into OUT_ROLE, and the names
// of the permissions dev.credperm lists for it, comma-separated, into
// OUT_PERM.  Return true when dev.cred has the row.  A missing file reads as
// an empty table.
bool credd::ReadNeed(algo::strptr dir, algo::strptr name, dev::Cred &out_cred, algo::cstring &out_role, algo::cstring &out_perm) {
    bool found = false;
    algo::ListSep sep(",");
    ind_beg(algo::FileLine_curs, line, algo::SsimFname(dir, dmmeta_Ssimfile_ssimfile_dev_cred)) {
        dev::Cred cred;
        if (dev::Cred_ReadStrptrMaybe(cred, line) && cred.cred == name) {
            out_cred = cred;
            found = true;
        }
    }ind_end;
    ind_beg(algo::FileLine_curs, line, algo::SsimFname(dir, dmmeta_Ssimfile_ssimfile_dev_credrole)) {
        dev::Credrole credrole;
        if (dev::Credrole_ReadStrptrMaybe(credrole, line) && credrole.cred == name) {
            out_role = credrole.role;
        }
    }ind_end;
    ind_beg(algo::FileLine_curs, line, algo::SsimFname(dir, dmmeta_Ssimfile_ssimfile_dev_credperm)) {
        dev::Credperm credperm;
        if (dev::Credperm_ReadStrptrMaybe(credperm, line) && dev::cred_Get(credperm) == name) {
            out_perm << sep << dev::name_Get(credperm);
        }
    }ind_end;
    return found;
}

// Return the steps a person follows to get a token of KIND, with the page
// that issues it and every placeholder filled in: SERVER, the credential's
// NAME, the ACCOUNT the token belongs to (for a group or project token, its
// path), the comma-separated permissions PERM, and the ROLE's name.  An empty
// ROLE reads as the role the token has now, which is what -renew asks for.
algo::tempstr credd::GetHowto(credd::FKind &kind, algo::strptr server, algo::strptr name, algo::strptr account, algo::strptr perm, algo::strptr role) {
    algo_lib::Replscope R;
    Set(R, "$server", server);
    Set(R, "$name", name);
    Set(R, "$account", account);
    Set(R, "$perm", perm);
    Set(R, "$role", role != "" ? role : strptr("it has now"));
    algo::tempstr url;
    Ins(&R, url, kind.tokenurl, false);
    Set(R, "$tokenurl", url);
    algo::tempstr ret;
    Ins(&R, ret, kind.howto, false);
    return ret;
}

// True when CRED may mint on SERVER: a gitlab token issued by SERVER, or by any
// server when SERVER is empty.  A token sent as a bearer anywhere else would
// hand its secret to a server that did not issue it.
static bool MinterQ(credd::FCred &cred, algo::strptr server) {
    return cred.kind == creddb_Kind_kind_gitlab && (server == "" || cred.server == server);
}

// Return the store's token that mints on SERVER over the API, NULL when there
// is none.  -minter names it outright, and it has to be a gitlab token of
// SERVER all the same.  Otherwise it is a gitlab token for SERVER, or for any
// server when SERVER is empty, in name order, and one recorded with the api
// permission comes first.  A token -add:% brought in from gli.ssim records no
// permissions, and it is usually the person's own api token, so a token
// without the record is still asked with; GitLab refuses it if it cannot mint.
// A link resolves to what it stands for.
static credd::FCred *FindMinter(algo::strptr server) {
    credd::FCred *ret = NULL;
    credd::FCred *any = NULL;
    if (credd::_db.cmdline.minter != "") {
        ret = credd::Target(credd::_db.cmdline.minter);
        vrfy(ret, tempstr() << "credd.notfound" << Keyval("name", credd::_db.cmdline.minter)
             << Keyval("comment", "-minter names a credential that is not in the store"));
        vrfy(MinterQ(*ret, server), tempstr() << "credd.badminter" << Keyval("name", ret->cred)
             << Keyval("kind", ret->kind) << Keyval("server", ret->server)
             << Keyval("comment", "-minter has to name a gitlab token of the server the token is minted at"));
    } else {
        ind_beg(credd::_db_cred_curs, cred, credd::_db) {
            bool api = credd::ind_credperm_Find(creddb::Credperm_Concat_cred_name(cred.cred, "api")) != NULL;
            bool fit = MinterQ(cred, server);
            if (fit && api && !ret) {
                ret = &cred;
            }
            if (fit && !any) {
                any = &cred;
            }
        }ind_end;
    }
    return ret ? ret : any;
}

// POST the JSON object BODY to URL, authenticated by the plaintext token
// BEARER, and read the token GitLab answers with.  Return the HTTP status, 0
// when the request did not complete.  On a 2xx answer the token is in
// OUT_TOKEN and the expiry the provider set in OUT_EXPIRES; otherwise OUT_MSG
// holds what the provider or the transport said.  The token travels in a
// header of an in-process request and never on a command line.
static int PostToken(algo::strptr url, algo::strptr bearer, lib_json::FNode &body, algo::cstring &out_token, algo::cstring &out_expires, algo::cstring &out_msg) {
    lib_curl::FRequest creq;
    creq.method = "POST";
    creq.url = url;
    lib_json::JsonSerialize(&body, creq.body);
    headers_Alloc(creq) = tempstr() << "Authorization: Bearer " << bearer;
    headers_Alloc(creq) = "Accept: application/json";
    headers_Alloc(creq) = "Content-Type: application/json";
    verblog("credd.tokenreq" << Keyval("url", creq.url) << Keyval("body", creq.body));
    lib_curl::FResponse resp;
    int status = 0;
    try {
        lib_curl::Curl(creq, resp);
        status = resp.code;
    } catch (algo_lib::ErrorX &x) {
        out_msg = x.str;
    }
    lib_json::FParser parser;
    bool json = status != 0 && lib_json::JsonParse(parser, resp.body) && parser.root_node;
    if (status / 100 == 2 && json) {
        out_token = lib_json::strptr_Get(parser.root_node, "token");
        out_expires = lib_json::strptr_Get(parser.root_node, "expires_at");
    } else if (json) {
        // GitLab says why under message, or under error for an auth failure
        algo::strptr message = lib_json::strptr_Get(parser.root_node, "message");
        out_msg = message != "" ? message : lib_json::strptr_Get(parser.root_node, "error");
    } else if (status != 0) {
        out_msg = resp.status_line;
    }
    return status;
}

// Return the endpoint URL with $server replaced by SERVER and $account by
// ACCOUNT, the account escaped for a path.
static algo::tempstr GetApiUrl(algo::strptr url, algo::strptr server, algo::strptr account) {
    algo_lib::Replscope R;
    algo::tempstr account_uri;
    algo::strptr_PrintUri(account, account_uri, false);
    Set(R, "$server", server);
    Set(R, "$account", account_uri);
    algo::tempstr ret;
    Ins(&R, ret, url, false);
    return ret;
}

// Ask the provider to mint a token of KIND, at the endpoint the kind names on
// SERVER for ACCOUNT, authenticated by the plaintext token MINTER.  The token
// is named NAME and carries the access level of ROLE, the comma-separated
// scopes PERM, and the expiry EXPIRES when that is not empty; an empty one
// leaves the provider to set the latest it allows.  Return the HTTP status,
// with the outputs as PostToken gives them.
static int MintOverApi(credd::FKind &kind, algo::strptr server, algo::strptr account, algo::strptr minter, algo::strptr name, credd::FRole &role, algo::strptr perm, algo::strptr expires, algo::cstring &out_token, algo::cstring &out_expires, algo::cstring &out_msg) {
    lib_json::FNode &body = lib_json::NewObjectNode(NULL);
    lib_json::NewStringNode(&body, "name", name);
    lib_json::NewNumberNode(&body, "access_level", tempstr() << role.level);
    lib_json::FNode &scopes = lib_json::NewArrayNode(&body, "scopes");
    ind_beg(algo::Sep_curs, scope, perm, ',') {
        lib_json::NewStringNode(&scopes, strptr(), scope);
    }ind_end;
    if (expires != "") {
        lib_json::NewStringNode(&body, "expires_at", expires);
    }
    int status = PostToken(GetApiUrl(kind.minturl, server, account), minter, body, out_token, out_expires, out_msg);
    lib_json::node_Delete(body);
    return status;
}

// True when HTTP STATUS says the provider refused the asker, rather than the
// request: 401 and 403, and 404 for a group or project the asker may not see.
// A refusal sends the person to the printed steps.
bool credd::RefusedQ(int status) {
    return status == 401 || status == 403 || status == 404;
}

// Return the expiry to ask for when CRED is renewed with no -expires: today
// plus the lifetime the token was issued with, from the day it was added to
// the day it expires.  A token recorded with no expiry asks for 364 days:
// GitLab allows 365 by default, counted from its own date, and a local date
// a day ahead of the server's would land one past the maximum.
//
// GitLab gives a rotated token one week when the request names no expiry.  A
// token issued for a year and renewed when -check names it two weeks before
// its end would then come back expiring a week later, and -check would name
// it again at once.  Asking for the old lifetime keeps the cadence the token
// was issued with, which GitLab accepted once already.
algo::tempstr credd::GetRenewExpires(credd::FCred &cred) {
    algo::tempstr ret;
    algo::UnTime expires = credd::ParseExpires(cred.expires);
    i64 lifetime = expires.value - cred.created.value;
    if (cred.expires == "" || lifetime <= 0) {
        lifetime = i64(364) * 86400 * 1000000000;
    }
    algo::UnTime renewed = credd::NowSec();
    renewed.value += lifetime;
    algo::UnTime_PrintSpec(renewed, ret, "%Y-%m-%d");
    return ret;
}

// Ask the provider to rotate the token CRED, authenticated by its own
// plaintext value PLAIN.  GitLab revokes the old value and answers with a new
// one, which expires on EXPIRES when that is not empty and on a date of the
// provider's choosing otherwise.  Return the HTTP status, 200 when the token
// was rotated, with the outputs as PostToken gives them.  Rotation needs the
// kind's endpoint (creddb.kind rotateurl) and the token's self_rotate
// permission; the caller checks both.
int credd::RotateOverApi(credd::FCred &cred, algo::strptr plain, algo::strptr expires, algo::cstring &out_token, algo::cstring &out_expires, algo::cstring &out_msg) {
    lib_json::FNode &body = lib_json::NewObjectNode(NULL);
    if (expires != "") {
        lib_json::NewStringNode(&body, "expires_at", expires);
    }
    int status = PostToken(GetApiUrl(cred.p_kind->rotateurl, cred.server, cred.account), plain, body, out_token, out_expires, out_msg);
    lib_json::node_Delete(body);
    return status;
}

// Add the credential -mint names, made to the measure the tree's dev.cred,
// dev.credrole and dev.credperm give it; see the top of this file.  The
// server and the account come from -server and -account, asked at a terminal
// when the kind mints over the API and they are missing; the server defaults
// to the minting token's.  The master password is asked before the request,
// since the minting token is sealed, and the new token is sealed under it.
// Expiry is -expires, or the latest the provider allows; the comment is
// -comment, or the dev.cred row's.
void credd::Mint() {
    algo::strptr name = credd::_db.cmdline.mint;
    credd::CheckNewName(name);
    dev::Cred need;
    algo::cstring rolename;
    algo::cstring perm;
    vrfy(credd::ReadNeed(credd::_db.cmdline.in, name, need, rolename, perm), tempstr()
         << "credd.noneed" << Keyval("name", name) << Keyval("in", credd::_db.cmdline.in)
         << Keyval("comment", "dev.cred has no row of this name; a tool's credential is declared there, with its role in dev.credrole and permissions in dev.credperm"));
    credd::FKind *kind = credd::ind_kind_Find(need.kind);
    vrfy(kind && kind->kind != creddb_Kind_kind_link && kind->kind != creddb_Kind_kind_sshkey, tempstr()
         << "credd.badkind" << Keyval("name", name) << Keyval("kind", need.kind)
         << Keyval("comment", "-mint makes a token; dev.cred names a kind credd cannot mint, or one this build does not know"));
    credd::FRole *role = rolename != "" ? credd::ind_role_Find(rolename) : NULL;
    vrfy(rolename == "" || (role && credd::kind_Get(*role) == kind->kind), tempstr()
         << "credd.badrole" << Keyval("name", name) << Keyval("role", rolename)
         << Keyval("comment", "dev.credrole names a role this build does not know, or one of another kind"));
    bool api = kind->minturl != "";
    vrfy(!api || role, tempstr() << "credd.norole" << Keyval("name", name) << Keyval("kind", kind->kind)
         << Keyval("comment", "a token of this kind acts in a role; dev.credrole has no row for this credential"));
    // the server a minter was issued by is the default answer, and the minter
    // is chosen again once the answer is in, since the request goes there
    credd::FCred *guess = api ? FindMinter(credd::_db.cmdline.server) : NULL;
    algo::tempstr server(credd::_db.cmdline.server);
    if (server == "" && guess) {
        server = guess->server;
    }
    algo::tempstr account(credd::_db.cmdline.account);
    if (isatty(0)) {
        server = credd::AskPlain("Server URL", server);
        account = credd::AskPlain(api ? "Group or project path" : "Account", account);
    }
    credd::FCred *minter = api && server != "" ? FindMinter(server) : NULL;
    vrfy(!api || (server != "" && account != ""), tempstr() << "credd.badopt" << Keyval("mint", name)
         << Keyval("comment", "-server and -account say where the token is made: the GitLab URL and the group or project path"));
    (void)credd::ParseExpires(credd::_db.cmdline.expires);
    algo::strptr rolelabel = role ? credd::name_Get(*role) : strptr();
    prlog("Minting " << name << ", a " << kind->kind << " token"
          << (role ? tempstr() << " acting as " << rolelabel : tempstr())
          << (perm != "" ? tempstr() << " with " << perm : tempstr())
          << (account != "" ? tempstr() << ", for " << account : tempstr())
          << (server != "" ? tempstr() << " at " << server : tempstr()) << ".");
    credd::UnlockPrompt();
    int status = 0;
    algo::cstring token;
    algo::cstring expires(credd::_db.cmdline.expires);
    algo::cstring msg;
    if (minter) {
        algo::cstring plain;
        vrfy(credd::Unseal(credd::_db.key, minter->secret, plain), tempstr() << "credd.unseal" << Keyval("cred", minter->cred));
        status = MintOverApi(*kind, server, account, plain, name, *role, perm, credd::_db.cmdline.expires, token, expires, msg);
    }
    // A refusal sends the person to the steps below: GitLab says no to anyone
    // who is not an Owner, and the request is then theirs to forward.  A group
    // the asker may not see answers 404, which is the same refusal.  Any other
    // failure is a mistake in the request, and the steps would repeat it.
    bool refused = credd::RefusedQ(status);
    vrfy(!minter || status == 201 || refused, tempstr() << "credd.mintfail" << Keyval("name", name)
         << Keyval("status", status) << Keyval("msg", msg));
    if (status == 201) {
        prlog("credd.minted" << Keyval("name", name) << Keyval("by", minter->cred) << Keyval("expires", expires));
    } else {
        if (refused) {
            prlog("credd.mintrefused" << Keyval("by", minter->cred) << Keyval("account", minter->account)
                  << Keyval("status", status) << Keyval("msg", msg)
                  << Keyval("comment", "that account may not make this token; the steps below are for one who may"));
        } else if (api) {
            prlog("credd.nominter" << Keyval("server", server)
                  << Keyval("comment", "no gitlab token of yours for this server is in the store to ask with; the steps below are for a person"));
        }
        prlog(credd::GetHowto(*kind, server, name, account, perm, rolelabel));
        token = algo::Trimmed(credd::ReadKindSecret(*kind, "Token value: "));
    }
    vrfy(token != "", "credd.emptysecret  comment:\"no token value was given\""); // ignore:hand_quote
    creddb::Cred rec;
    rec.cred = name;
    rec.kind = kind->kind;
    rec.server = server;
    rec.account = account;
    rec.expires = expires;
    rec.comment = algo::Comment(credd::_db.cmdline.comment != "" ? strptr(credd::_db.cmdline.comment) : strptr(need.comment));
    credd::StoreCred(rec, perm, token);
}
