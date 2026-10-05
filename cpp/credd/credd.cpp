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
// Source: cpp/credd/credd.cpp
//
// The verbs.  Every verb here is the command line's: a verb that reads the
// store reads the files, a verb that changes the store asks the master
// password, writes the files and tells the running daemon to reload, and a
// verb that needs a secret open asks the daemon.  Main runs the verbs in the
// order they are declared in command.credd.  The guided paths of -add, the
// walk of -add:% and the one file of -add:<file>, are in import.cpp.

#include "include/algo.h"
#include "include/credd.h"
#include <fcntl.h>
#include <unistd.h>

// The named credential must not exist yet, and its name carries no slash,
// which -add reads as a path.
static void CheckNewName(algo::strptr name) {
    vrfy(algo::FindChar(name, '/') < 0, tempstr()
         << "credd.badname" << Keyval("name", name)
         << Keyval("comment", "a slash makes a path; a credential's name has none"));
    vrfy(!credd::NameTakenQ(name), tempstr()
         << "credd.exists" << Keyval("name", name)
         << Keyval("comment", "a credential of this name is in the store; -remove it first"));
}

// Report the daemon's status, or that none answers.
void credd::Status() {
    lib_cred::Status status;
    if (credd::ProbeDaemon(status)) {
        prlog(status);
        credd::WarnOldBuild(status);
    } else {
        prlog("credd.down" << Keyval("dir", credd::_db.dir) << Keyval("comment", "no daemon answers; credd -start"));
    }
}

// Start the daemon unless one answers already, in which case a daemon from
// another build is pointed out.  The master password is asked
// here, at the terminal, and the child inherits the derived key across the
// fork, so the daemon never sees the password and needs no terminal.  The
// child's output goes to credd.log in the store.  The parent waits until the
// socket answers, forgets the key, and reports the status.
//
// With -nostore there is no store and so no password to ask for: the key is
// drawn from the system random source instead, and the daemon comes up holding
// nothing, waiting for a seed request to give it something to serve.
void credd::Start() {
    lib_cred::Status status;
    if (credd::ProbeDaemon(status)) {
        prlog("credd.running" << Keyval("dir", status.dir) << Keyval("agent", status.agent));
        credd::WarnOldBuild(status);
    } else {
        if (credd::_db.cmdline.nostore) {
            credd::EphemeralKey();
        } else {
            credd::UnlockPrompt();
        }
        algo::tempstr logfile = algo::DirFileJoin(credd::_db.dir, "credd.log");
        pid_t pid = fork();
        errno_vrfy_(pid >= 0);
        if (pid == 0) {
            (void)setsid();
            int devnull = open("/dev/null", O_RDONLY);
            int log = open(Zeroterm(logfile), O_WRONLY | O_CREAT | O_APPEND, 0600);
            (void)dup2(devnull, 0);
            (void)dup2(log, 1);
            (void)dup2(log, 2);
            close(devnull);
            close(log);
            credd::RunDaemon();
            _exit(0);
        }
        credd::_db.key = "";
        bool up = false;
        for (int i = 0; i < 100 && !up; i++) {
            up = credd::ProbeDaemon(status);
            if (!up) {
                usleep(50000);
            }
        }
        vrfy(up, tempstr() << "credd.startfail" << Keyval("log", logfile));
        prlog(status);
    }
}

// Give the running daemon the credential -seed names, read per -src, and report
// what it said.
//
// This is how a service host is loaded.  A workstation puts a credential in its
// store with -add, which seals it on disk under the master password and tells
// the daemon to read the store again.  A runner has no store and no password:
// the secret goes straight to the daemon, which holds it for as long as it runs
// and writes nothing.  Whatever ends the job ends the daemon, and the secret is
// gone with it.
//
// The secret is read per -src -- `@file` or a pipe -- for the same reason every
// other verb reads it that way: a value on a command line is in the process
// list for anyone on the machine to read.  -kind says what the credential is,
// as it does for -add, and the daemon refuses a kind it does not know.
void credd::Seed() {
    algo::strptr name = credd::_db.cmdline.seed;
    credd::FKind *kind = credd::ind_kind_Find(credd::_db.cmdline.kind);
    vrfy(kind, tempstr() << "credd.badkind" << Keyval("kind", credd::_db.cmdline.kind)
         << Keyval("comment", tempstr() << "-kind is required with -seed: one of " << credd::KindChoices(true)));
    lib_cred::SeedReq req;
    req.client = "credd";
    req.name = name;
    req.kind = kind->kind;
    req.secret << algo::Trimmed(credd::ReadKindSecret(*kind, "Secret value: "));
    if (kind->kind == creddb_Kind_kind_sshkey) {
        req.secret << eol;
    }
    algo::cstring reply;
    algo::cstring err;
    lib_cred::Error error;
    bool ok = credd::Ask(tempstr() << req, reply, err);
    // the daemon answering is not the daemon agreeing: a refusal is a reply
    // like any other, and a caller that reads only the exit code would go on
    // without the identity it asked for
    if (ok && lib_cred::Error_ReadStrptrMaybe(error, reply)) {
        ok = false;
        err = "";
        err << "credd.refused" << Keyval("name", name) << Keyval("msg", error.msg);
    }
    if (ok) {
        prlog(reply);
    } else {
        prerr(err);
        algo_lib::_db.exit_code = 1;
    }
}

// -----------------------------------------------------------------------------

// Ask the daemon to stop, and report the answer.
void credd::Stop() {
    lib_cred::StopReq req;
    req.client = "credd";
    algo::cstring reply;
    algo::cstring err;
    if (credd::Ask(tempstr() << req, reply, err)) {
        prlog(reply);
    } else {
        prlog(err);
    }
}

// Print the secret of the credential -show names, as the daemon opens it,
// with no decoration, so $(credd -show:name) is the value.  A refusal is
// reported and the exit code is 1.
void credd::Show() {
    algo::cstring value;
    algo::cstring err;
    credd::FCred *target = credd::Target(credd::_db.cmdline.show);
    if (credd::_db.cmdline.pubfile != "") {
        // The public half is metadata of the store, sealed to nobody, so it is
        // read here without the daemon and without the master password.
        vrfy(target, tempstr() << "credd.notfound" << Keyval("name", credd::_db.cmdline.show));
        vrfy(target->kind == creddb_Kind_kind_sshkey && target->pubkey != "", tempstr()
             << "credd.nopubkey" << Keyval("cred", target->cred) << Keyval("kind", target->kind)
             << Keyval("comment", "only an ssh key has a public half"));
        algo::StringToFile(tempstr() << target->pubkey << eol, credd::_db.cmdline.pubfile);
        prlog("credd.pubfile" << Keyval("cred", target->cred) << Keyval("file", credd::_db.cmdline.pubfile)
              << Keyval("comment", "the public key, as authorized_keys takes it"));
    } else if (lib_cred::FetchFrom(credd::DaemonSock(), "credd", credd::_db.cmdline.show, value, err)) {
        prlog(value);
    } else {
        prerr(err);
        algo_lib::_db.exit_code = 1;
    }
}

// List every credential whose expiry falls within -window days from now, with
// the days left.  A credential that never expires is not listed.
void credd::Check() {
    algo::UnTime now = algo::CurrUnTime();
    i64 horizon = now.value + i64(credd::_db.cmdline.window) * 86400 * 1000000000LL;
    ind_beg(credd::_db_cred_curs, cred, credd::_db) {
        algo::UnTime expires = credd::ParseExpires(cred.expires);
        if (expires.value != 0 && expires.value < horizon) {
            prlog("credd.expiring" << Keyval("cred", cred.cred) << Keyval("kind", cred.kind)
                  << Keyval("expires", cred.expires)
                  << Keyval("days", i64((expires.value - now.value) / (86400 * 1000000000LL))));
        }
    }ind_end;
}

// The permissions KIND offers, one line each with its comment, as a prompt
// shows them before asking which the token carries.
static void PrintPermList(credd::FKind &kind) {
    ind_beg(credd::kind_c_perm_curs, perm, kind) {
        prlog("  " << credd::name_Get(perm) << "  " << perm.comment);
    }ind_end;
}

// Add REC as a link to the credential -linkto names, which has to exist and
// be a credential of its own, since a link resolves in one hop.  A tool that
// asks for REC's name gets the target's secret, so the same token can answer
// as gli-mr and gli-manage, or two tokens can, without the tool knowing.
static void AddLink(creddb::Cred &rec) {
    credd::FCred *target = credd::ind_cred_Find(credd::_db.cmdline.linkto);
    vrfy(target, tempstr() << "credd.notfound" << Keyval("name", credd::_db.cmdline.linkto)
         << Keyval("comment", "-linkto names a credential that is not in the store"));
    vrfy(target->kind != creddb_Kind_kind_link, tempstr() << "credd.linkchain" << Keyval("name", target->cred)
         << Keyval("linkto", target->linkto)
         << Keyval("comment", "that is itself a link; link to what it stands for"));
    rec.kind = creddb_Kind_kind_link;
    rec.linkto = target->cred;
    rec.comment = algo::Comment(strptr(credd::_db.cmdline.comment));
    credd::StoreCred(rec, "", "");
}

// Add REC with a secret of its own; see Add.
static void AddSecret(creddb::Cred &rec) {
    bool tty = isatty(0);
    algo::tempstr kindname(credd::_db.cmdline.kind);
    if (kindname == "" && tty) {
        kindname = credd::AskPlain(tempstr() << "Kind (" << credd::KindChoices(true) << ")", "");
    }
    credd::FKind *kind = credd::ind_kind_Find(kindname);
    vrfy(kind, tempstr() << "credd.badkind" << Keyval("kind", kindname)
         << Keyval("comment", tempstr() << "-kind is required with -add: one of " << credd::KindChoices(true)));
    rec.kind = kind->kind;
    bool sshkey = rec.kind == creddb_Kind_kind_sshkey;
    rec.server = credd::_db.cmdline.server;
    rec.account = credd::_db.cmdline.account;
    algo::tempstr perms(credd::_db.cmdline.perm);
    if (!sshkey && tty) {
        rec.server = credd::AskPlain("Server URL", rec.server);
        rec.account = credd::AskPlain("Account", rec.account);
        if (perms == "" && credd::c_perm_N(*kind) > 0) {
            prlog("Permissions this kind offers:");
            PrintPermList(*kind);
            perms = credd::AskPlain("Permissions the token was issued with, comma-separated", "");
        }
    }
    rec.comment = algo::Comment(strptr(credd::_db.cmdline.comment));
    rec.expires = credd::_db.cmdline.expires;
    if (tty) {
        rec.comment = algo::Comment(strptr(credd::AskPlain("Comment", rec.comment)));
        rec.expires = strptr(credd::AskPlain("Expires (YYYY-MM-DD, empty = never)", rec.expires));
    }
    vrfy(rec.kind != creddb_Kind_kind_link, tempstr() << "credd.nolink"
         << Keyval("comment", "a link is made with -linkto, and has no secret to read"));
    (void)credd::ParseExpires(rec.expires);
    algo::tempstr secret = credd::ReadKindSecret(*kind, "Secret value: ");
    algo::tempstr plain;
    plain << algo::Trimmed(secret);
    vrfy(plain != "", "credd.emptysecret  comment:\"no secret was given\""); // ignore:hand_quote
    if (sshkey) {
        plain << eol;
        vrfy(algo::StartsWithQ(plain, "-----BEGIN"), tempstr()
             << "credd.badkey" << Keyval("name", rec.cred) << Keyval("comment", "that is not a private key in PEM or OpenSSH format"));
        rec.pubkey = credd::DerivePubkey(rec.cred, plain);
        vrfy(rec.pubkey != "", tempstr() << "credd.badkey" << Keyval("name", rec.cred)
             << Keyval("comment", "ssh-keygen cannot read a public key out of it; a passphrase-protected key goes through -add:<file>"));
    }
    credd::StoreCred(rec, perms, plain);
}

// Add the credential named NAME.  At a terminal every option not given is
// asked, so `credd -add:name` alone walks through kind, server, account,
// permissions, comment and expiry; under a pipe the options are what was
// given.  The secret is read per -src: one line for a token, and pasted lines
// ended by an empty one for a multiline kind -- a shell fragment, or an sshkey,
// whose public half is derived from it.
// The master password is asked last, so a paste that goes wrong costs nothing.
static void AddNamed(algo::strptr name) {
    creddb::Cred rec;
    rec.cred = name;
    CheckNewName(rec.cred);
    if (credd::_db.cmdline.linkto != "") {
        AddLink(rec);
    } else {
        AddSecret(rec);
    }
}

// Add what -add names.  `%` walks the store's parent directory and offers
// every token and key it holds; a path, which has a slash in it, offers the
// file it names the same way; any other value is the name of the credential
// to add, its secret read per -src.  A name is never read as a file, so
// `credd -add:id_ed25519` run from inside ~/.ssh adds a credential of that
// name and leaves the file of that name alone.
void credd::Add() {
    algo::strptr add = credd::_db.cmdline.add;
    bool path = algo::FindChar(add, '/') >= 0;
    bool guided = add == "%" || path;
    // The guided paths read the kind, the comment and the secret from the file
    // and ask what else they need, so an option meant for -add:<name> would be
    // taken silently; it is refused instead.
    command::credd &cmd = credd::_db.cmdline;
    bool named_opt = cmd.linkto != "" || cmd.kind != "" || cmd.server != "" || cmd.account != "" || cmd.perm != ""
        || cmd.expires != "" || cmd.comment != "" || cmd.src != "-";
    vrfy(!(guided && named_opt), tempstr() << "credd.badopt" << Keyval("add", add)
         << Keyval("comment", "-linkto, -kind, -server, -account, -perm, -expires, -comment and -src go with -add:<name>; a file or the walk reads them from the file and asks"));
    if (guided) {
        // the file is judged before the master password is asked: a file
        // that will be refused costs no prompt and leaves no store behind
        vrfy(add == "%" || algo::FileQ(add), tempstr() << "credd.badfile" << Keyval("file", add) << Keyval("comment", "not an existing regular file"));
        algo::cstring why;
        algo::tempstr fileclass = path ? credd::ImportFileClass(add, why) : algo::tempstr();
        vrfy(!path || fileclass != "", tempstr() << "credd.badfile" << Keyval("file", add) << Keyval("comment", why));
        // the master password once, before anything is offered, and the
        // walk's or the file's summary after
        credd::UnlockPrompt();
        int n_before = credd::cred_N();
        if (add == "%") {
            credd::ImportDir();
        } else {
            credd::ImportFile(add, fileclass);
        }
        prlog("");
        prlog("credd.import" << Keyval("n_add", credd::cred_N() - n_before) << Keyval("n_cred", credd::cred_N())
              << Keyval("comment", "credd -list shows the store; credd -start serves it"));
    } else {
        AddNamed(add);
    }
}

// Make an ed25519 key under the name -keygen gives and add it sealed.
// ssh-keygen writes the pair into the store's private temp directory, both
// files are read and unlinked, and the public key is printed so it can be
// installed where the key will log in; with -pubfile it is written to that
// file as well, which is what a tool that takes a key file reads.
void credd::Keygen() {
    creddb::Cred rec;
    rec.cred = credd::_db.cmdline.keygen;
    CheckNewName(rec.cred);
    algo::tempstr keyfile = algo::DirFileJoin(credd::KeyTempDir(), rec.cred);
    algo::tempstr pubfile;
    pubfile << keyfile << ".pub";
    (void)unlink(Zeroterm(keyfile));
    (void)unlink(Zeroterm(pubfile));
    algo::tempstr cmd;
    cmd << "ssh-keygen -q -t ed25519 -N '' -C "
        << algo::strptr_ToBash(credd::_db.cmdline.comment != "" ? strptr(credd::_db.cmdline.comment) : strptr(rec.cred))
        << " -f " << algo::strptr_ToBash(keyfile);
    int rc = algo::SysCmd(cmd, FailokQ(true));
    algo::tempstr priv = algo::FileToString(keyfile);
    rec.pubkey << algo::Trimmed(algo::FileToString(pubfile));
    (void)unlink(Zeroterm(keyfile));
    (void)unlink(Zeroterm(pubfile));
    vrfy(rc == 0 && priv != "" && rec.pubkey != "", tempstr() << "credd.keygenfail" << Keyval("cmd", cmd));
    rec.kind = creddb_Kind_kind_sshkey;
    rec.comment = algo::Comment(strptr(credd::_db.cmdline.comment));
    rec.expires = credd::_db.cmdline.expires;
    (void)credd::ParseExpires(rec.expires);
    credd::StoreCred(rec, "", priv);
    if (credd::_db.cmdline.pubfile != "") {
        algo::StringToFile(tempstr() << rec.pubkey << eol, credd::_db.cmdline.pubfile);
        prlog("credd.pubfile" << Keyval("file", credd::_db.cmdline.pubfile) << Keyval("comment", "the public key, as authorized_keys takes it"));
    }
    prlog(rec.pubkey);
}

// Renew the token -renew names: print the page where its kind reissues a
// token, with the server, name and permissions filled in, then read the new
// value per -src, ask the master password, and reseal.  A new -expires
// replaces the old; without one the old expiry stands.
void credd::Renew() {
    credd::FCred *target = credd::Target(credd::_db.cmdline.renew);
    vrfy(target, tempstr() << "credd.notfound" << Keyval("name", credd::_db.cmdline.renew));
    credd::FCred &cred = *target;
    vrfy(cred.kind != creddb_Kind_kind_sshkey, tempstr() << "credd.notoken" << Keyval("cred", cred.cred)
         << Keyval("comment", "an ssh key is not renewed; -keygen a new one and -remove this"));
    algo_lib::Replscope R;
    Set(R, "$server", cred.server);
    Set(R, "$name", cred.cred);
    Set(R, "$account", cred.account);
    Set(R, "$perm", credd::PermList(cred));
    algo::tempstr url;
    Ins(&R, url, cred.p_kind->tokenurl, false);
    Set(R, "$tokenurl", url);
    algo::tempstr howto;
    Ins(&R, howto, cred.p_kind->howto, false);
    if (cred.cred != credd::_db.cmdline.renew) {
        prlog(credd::_db.cmdline.renew << " is a link to " << cred.cred << ", so that is the token renewed.");
    }
    prlog("Renewing " << cred.cred << ", a " << cred.kind << " token for " << cred.account << " at " << cred.server
          << (cred.expires != "" ? tempstr() << ", expiring " << cred.expires : tempstr()) << ".");
    algo::tempstr links = credd::LinkList(cred);
    if (links != "") {
        prlog("The links " << links << " stand for it and follow along.");
    }
    if (credd::c_credperm_N(cred) > 0) {
        prlog("It was issued with these permissions, which the new token needs too:");
        ind_beg(credd::cred_c_credperm_curs, credperm, cred) {
            credd::FPerm *perm = credd::ind_perm_Find(creddb::Perm_Concat_kind_name(cred.kind, credd::name_Get(credperm)));
            prlog("  " << credd::name_Get(credperm) << (perm ? tempstr() << ": " << perm->comment : tempstr()));
        }ind_end;
    }
    if (howto != "") {
        prlog(howto);
    }
    prlog("The new value replaces the old one in the store once you paste it and give the master password; the running daemon serves it at once.");
    (void)credd::ParseExpires(credd::_db.cmdline.expires);
    algo::tempstr secret = credd::ReadKindSecret(*cred.p_kind, "New token value: ");
    vrfy(algo::Trimmed(secret) != "", "credd.emptysecret  comment:\"no token value was given\""); // ignore:hand_quote
    credd::UnlockPrompt();
    vrfy(credd::Seal(credd::_db.key, algo::Trimmed(secret), cred.secret), "credd.seal");
    cred.created = credd::NowSec();
    if (credd::_db.cmdline.expires != "") {
        cred.expires = credd::_db.cmdline.expires;
    }
    credd::SaveStore();
    credd::NotifyReload();
    prlog("credd.renewed" << Keyval("cred", cred.cred) << Keyval("expires", cred.expires));
}

// Remove the credential -remove names.  The master password is asked, as for
// every change to the store.  The row is marked and left out of the files
// written; a removed key leaves the agent at the daemon's reload.  A
// credential that links stand for stays until the links are removed, so a
// tool asking by the link's name never finds a name that points at nothing.
void credd::Remove() {
    algo::strptr name = credd::_db.cmdline.remove;
    credd::FCred *cred = credd::ind_cred_Find(name);
    vrfy(cred, tempstr() << "credd.notfound" << Keyval("name", name));
    algo::tempstr links = credd::LinkList(*cred);
    vrfy(links == "", tempstr() << "credd.haslink" << Keyval("name", name) << Keyval("link", links)
         << Keyval("comment", "links stand for this credential; -remove them first"));
    credd::UnlockPrompt();
    cred->removed = true;
    credd::SaveStore();
    credd::NotifyReload();
    prlog("credd.remove" << Keyval("name", name));
}

// Change the master password.  The old one is asked and checked, the new one
// twice; every secret is opened under the old key and sealed under the new,
// the master record gets a fresh salt and verifier, and the store is written.
// A running daemon holds the old key, so it is stopped and has to be started
// again.
void credd::Passwd() {
    credd::UnlockPrompt();
    algo::cstring oldkey(credd::_db.key);
    algo::tempstr password = credd::AskPassword("New master password: ");
    algo::tempstr again = credd::AskPassword("Again: ");
    vrfy(password != "", "credd.emptypassword  comment:\"the master password cannot be empty\""); // ignore:hand_quote
    vrfy(password == again, "credd.mismatch  comment:\"the two answers differ; nothing changed\""); // ignore:hand_quote
    credd::InitMaster(password, credd::_db.key);
    ind_beg(credd::_db_cred_curs, cred, credd::_db) if (cred.kind != creddb_Kind_kind_link) {
        algo::cstring plain;
        vrfy(credd::Unseal(oldkey, cred.secret, plain), tempstr() << "credd.unseal" << Keyval("cred", cred.cred));
        vrfy(credd::Seal(credd::_db.key, plain, cred.secret), "credd.seal");
    }ind_end;
    credd::SaveStore();
    lib_cred::Status status;
    if (credd::ProbeDaemon(status)) {
        credd::Stop();
        prlog("credd.passwd" << Keyval("comment", "master password changed; the daemon was stopped, credd -start to run it under the new one"));
    } else {
        prlog("credd.passwd" << Keyval("comment", "master password changed"));
    }
}

// Add ROW, one printed ssim tuple, to ARRAY as a JSON object with one string
// member per attribute.
static void JsonRow(lib_json::FNode &array, algo::strptr row) {
    algo::Tuple tuple;
    if (algo::Tuple_ReadStrptrMaybe(tuple, row)) {
        lib_json::FNode &object = lib_json::NewObjectNode(&array);
        ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
            lib_json::NewStringNode(&object, attr.name, attr.value);
        }ind_end;
    }
}

// List every credential; needs no daemon.  With -format:auto or text the
// listing is a table of cred, kind, server and comment.  With ssim it is the
// rows as the store holds them, the sealed secret written as the word sealed
// and a token's permissions indented under it.  With json it is the cred rows
// as one array.
void credd::List() {
    u8 format = credd::_db.cmdline.format == command_credd_format_auto ? u8(command_credd_format_text) : credd::_db.cmdline.format;
    algo::tempstr table;
    algo::tempstr rows;
    table << "cred\tkind\tserver\tcomment" << eol;
    ind_beg(credd::_db_cred_curs, cred, credd::_db) {
        creddb::Cred out;
        credd::cred_CopyOut(cred, out);
        out.secret = "sealed";
        table << cred.cred << "\t" << cred.kind << "\t" << cred.server << "\t" << cred.comment << eol;
        rows << out << eol;
        if (format == command_credd_format_ssim) {
            ind_beg(credd::cred_c_credperm_curs, credperm, cred) {
                creddb::Credperm permout;
                credd::credperm_CopyOut(credperm, permout);
                rows << "  " << permout << eol;
            }ind_end;
        }
    }ind_end;
    if (format == command_credd_format_text) {
        prlog_(algo::Tabulated(table, "\t"));
    } else if (format == command_credd_format_json) {
        lib_json::FNode &root = lib_json::NewArrayNode(NULL);
        ind_beg(algo::Line_curs, line, rows) {
            JsonRow(root, line);
        }ind_end;
        algo::cstring out;
        lib_json::JsonSerialize(&root, out, 1);
        prlog(out);
        lib_json::node_Delete(root);
    } else {
        prlog_(rows);
    }
}

// Print the whole store, every secret still sealed, as ssim tuples (-format:auto
// or ssim) or, with -format:json, as one object holding an array per table.
// What prints is what the files hold, so the ssim form placed under the store's
// creddb directory is the store restored.
void credd::Dump() {
    algo::tempstr rows[3];
    algo::strptr tables[3] = {"master", "cred", "credperm"};
    ind_beg(credd::_db_master_curs, master, credd::_db) {
        creddb::Master out;
        credd::master_CopyOut(master, out);
        rows[0] << out << eol;
    }ind_end;
    ind_beg(credd::_db_cred_curs, cred, credd::_db) {
        creddb::Cred out;
        credd::cred_CopyOut(cred, out);
        rows[1] << out << eol;
        ind_beg(credd::cred_c_credperm_curs, credperm, cred) {
            creddb::Credperm permout;
            credd::credperm_CopyOut(credperm, permout);
            rows[2] << permout << eol;
        }ind_end;
    }ind_end;
    u8 format = credd::_db.cmdline.format == command_credd_format_auto ? u8(command_credd_format_ssim) : credd::_db.cmdline.format;
    if (format == command_credd_format_json) {
        lib_json::FNode &root = lib_json::NewObjectNode(NULL);
        for (int i = 0; i < 3; i++) {
            lib_json::FNode &array = lib_json::NewArrayNode(&root, tables[i]);
            ind_beg(algo::Line_curs, line, rows[i]) {
                JsonRow(array, line);
            }ind_end;
        }
        algo::cstring out;
        lib_json::JsonSerialize(&root, out, 1);
        prlog(out);
        lib_json::node_Delete(root);
    } else {
        vrfy(format == command_credd_format_ssim, tempstr() << "credd.badformat" << Keyval("format", command::format_ToCstr(credd::_db.cmdline))
             << Keyval("comment", "-dump prints ssim or json"));
        for (int i = 0; i < 3; i++) {
            prlog_(rows[i]);
        }
    }
}

// Run the verbs given, in the order command.credd declares them.  With none,
// report the daemon's status.
void credd::Main() {
    credd::LoadStore();
    bool any = credd::_db.cmdline.start || credd::_db.cmdline.stop || credd::_db.cmdline.status
        || credd::_db.cmdline.list || credd::_db.cmdline.show != "" || credd::_db.cmdline.check
        || credd::_db.cmdline.add != "" || credd::_db.cmdline.keygen != ""
        || credd::_db.cmdline.renew != "" || credd::_db.cmdline.remove != "" || credd::_db.cmdline.passwd
        || credd::_db.cmdline.dump || credd::_db.cmdline.daemon || credd::_db.cmdline.seed != "";
    if (credd::_db.cmdline.start) {
        credd::Start();
    }
    if (credd::_db.cmdline.seed != "") {
        credd::Seed();
    }
    if (credd::_db.cmdline.stop) {
        credd::Stop();
    }
    if (credd::_db.cmdline.status || !any) {
        credd::Status();
    }
    if (credd::_db.cmdline.list) {
        credd::List();
    }
    if (credd::_db.cmdline.show != "") {
        credd::Show();
    }
    if (credd::_db.cmdline.check) {
        credd::Check();
    }
    if (credd::_db.cmdline.add != "") {
        credd::Add();
    }
    if (credd::_db.cmdline.keygen != "") {
        credd::Keygen();
    }
    if (credd::_db.cmdline.renew != "") {
        credd::Renew();
    }
    if (credd::_db.cmdline.remove != "") {
        credd::Remove();
    }
    if (credd::_db.cmdline.passwd) {
        credd::Passwd();
    }
    if (credd::_db.cmdline.dump) {
        credd::Dump();
    }
    if (credd::_db.cmdline.daemon) {
        if (credd::_db.cmdline.nostore) {
            credd::EphemeralKey();
        } else {
            credd::UnlockPrompt();
        }
        credd::RunDaemon();
    }
}
