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
// Source: cpp/credd/store.cpp
//
// The store on disk and the master password that opens it.  The store is a
// directory of ssimfiles, one per creddb table, under ~/.ssh/credd by
// default.  This file reads them into the tables in memory, writes the tables
// back, asks the terminal for the master password and the other answers a
// verb needs, and turns a password into the key the secrets are sealed under.
// The secrets themselves are opened and sealed in seal.cpp; the verbs that
// change the store are in credd.cpp and import.cpp.

#include "include/algo.h"
#include "include/credd.h"
#include <unistd.h>

// Path of the store: -dir when given, else $HOME/.ssh/credd.
algo::tempstr credd::StoreDir() {
    algo::tempstr ret;
    if (credd::_db.cmdline.dir != "") {
        ret << credd::_db.cmdline.dir;
    } else {
        const char *home = getenv(algo_lib::dev_envvar_HOME);
        ret << (home ? home : "") << "/.ssh/credd";
    }
    return ret;
}

// Read every table from the store into memory, replacing what was there.
// Creates the store directory, mode 0700, when it does not exist, so the first
// run of any verb leaves an empty store behind.  Dies naming the file and line
// when a row does not load, which includes a credential whose kind the binary
// does not know.
//
// Under -nostore there are no tables to read and none are created.  The
// directory is still made, because the daemon's socket lives in it, and the
// pools are left empty for a seed request to fill.
void credd::LoadStore() {
    credd::_db.dir = credd::StoreDir();
    algo::CreateDirRecurse(credd::_db.dir, true, 0700);
    credd::credperm_RemoveAll();
    credd::cred_RemoveAll();
    credd::master_RemoveAll();
    if (!credd::_db.cmdline.nostore) {
        algo::CreateDirRecurse(algo::DirFileJoin(credd::_db.dir, "creddb"), true, 0700);
        algo_lib::_db.errtext = "";
        vrfy(credd::LoadTuplesMaybe(credd::_db.dir, false), tempstr()
             << "credd.badstore" << Keyval("dir", credd::_db.dir) << eol << algo_lib::_db.errtext);
    }
}

// Write TEXT as the store's file for SSIMFILE, mode 0600, replacing the old
// file in one rename.
static void SaveTable(algo::strptr ssimfile, algo::strptr text) {
    algo::tempstr fname(algo::SsimFname(credd::_db.dir, ssimfile));
    vrfy(algo::SafeStringToFile(text, fname, 0600), tempstr()
         << "credd.writefail" << Keyval("file", fname));
}

// Write every table of the store back to disk.  A credential marked removed
// is left out, and so are its permissions.
void credd::SaveStore() {
    algo::tempstr master;
    ind_beg(credd::_db_master_curs, row, credd::_db) {
        creddb::Master out;
        credd::master_CopyOut(row, out);
        master << out << eol;
    }ind_end;
    algo::tempstr cred;
    algo::tempstr credperm;
    ind_beg(credd::_db_cred_curs, row, credd::_db) {
        if (!row.removed) {
            creddb::Cred out;
            credd::cred_CopyOut(row, out);
            cred << out << eol;
            ind_beg(credd::cred_c_credperm_curs, perm, row) {
                creddb::Credperm permout;
                credd::credperm_CopyOut(perm, permout);
                credperm << permout << eol;
            }ind_end;
        }
    }ind_end;
    SaveTable("creddb.master", master);
    SaveTable("creddb.cred", cred);
    SaveTable("creddb.credperm", credperm);
}

// The store's master record, or NULL when no password has been set yet.
credd::FMaster *credd::FindMaster() {
    return credd::master_N() ? &credd::master_qFind(0) : NULL;
}

// Derive the key for PASSWORD from the master record's salt and prove it by
// opening the verifier.  OUT_KEY holds the key when the password is right;
// otherwise it is empty and the function returns false.
bool credd::Unlock(algo::strptr password, algo::cstring &out_key) {
    credd::FMaster *master = credd::FindMaster();
    algo::cstring salt;
    algo::cstring check;
    bool ok = master && algo::strptr_ReadBase64(master->salt, salt);
    ok = ok && credd::DeriveKey(password, salt, out_key);
    ok = ok && credd::Unseal(out_key, master->verifier, check);
    ok = ok && check == "credd";
    if (!ok) {
        out_key = "";
    }
    return ok;
}

// Make PASSWORD the store's master password: a fresh salt, and a verifier
// sealed under the key derived from it, in the master record, which is created
// when the store has none.  OUT_KEY holds the new key.  Nothing is written to
// disk; the caller saves the store once every secret is sealed under the key.
void credd::InitMaster(algo::strptr password, algo::cstring &out_key) {
    algo::cstring salt;
    vrfy(credd::RandomBytes(16, salt), "credd.norandom");
    credd::FMaster *master = credd::FindMaster();
    if (!master) {
        master = &credd::master_Alloc();
        master->master = "default";
        vrfy(credd::master_XrefMaybe(*master), algo_lib::_db.errtext);
    }
    master->salt = "";
    algo::strptr_PrintBase64(salt, master->salt);
    vrfy(credd::DeriveKey(password, salt, out_key), tempstr() << "credd.kdf" << Keyval("comment", "scrypt refused"));
    vrfy(credd::Seal(out_key, "credd", master->verifier), tempstr() << "credd.seal" << Keyval("comment", "libcrypto refused"));
}

// Read one line from stdin, which is not a terminal, into OUT.  False at end
// of input.  This is how a test or a script answers a prompt.
static bool ReadPipeLine(algo::cstring &out) {
    out = "";
    char buf[4096];
    bool ok = fgets(buf, sizeof(buf), stdin) != NULL;
    if (ok) {
        out << algo::Trimmed(algo::strptr(buf, strlen(buf)));
    }
    return ok;
}

// Ask PROMPT and return the answer: at a terminal the line typed, echoed;
// under a pipe its next line.  DFLT is offered in the prompt and is the answer
// when the line is empty.  A verb that asks this at a terminal only, since a
// script gives every option, tests isatty first.
algo::tempstr credd::AskPlain(algo::strptr prompt, algo::strptr dflt) {
    algo::tempstr ret;
    algo::cstring out;
    algo::tempstr shown;
    shown << prompt;
    if (dflt != "") {
        shown << " [" << dflt << "]";
    }
    shown << ": ";
    bool ok = isatty(0) ? algo_lib::ReadPlain(shown, out) : ReadPipeLine(out);
    ret << (ok && out != "" ? strptr(out) : dflt);
    return ret;
}

// Ask QUESTION as a yes-or-no question, answered with y; anything else is no.
bool credd::AskYes(algo::strptr question) {
    algo::tempstr answer = credd::AskPlain(tempstr() << question << " (y/N)", "n");
    return answer == "y" || answer == "Y" || answer == "yes";
}

// Ask PROMPT for a line that is not shown as it is typed: at a terminal with
// echo off, under a pipe its next line.  Dies when neither can answer.
algo::tempstr credd::AskSecret(algo::strptr prompt) {
    algo::tempstr ret;
    algo::cstring out;
    bool ok = isatty(0) ? algo_lib::ReadMasked(prompt, out) : ReadPipeLine(out);
    vrfy(ok, tempstr() << "credd.noterminal" << Keyval("prompt", prompt));
    ret << out;
    return ret;
}

// Ask PROMPT for the master password: $CREDD_PASSWORD when it is set, which is
// how a test, a shell rc that already holds the password, or a service host
// supplies it without a terminal; otherwise as AskSecret does.
algo::tempstr credd::AskPassword(algo::strptr prompt) {
    algo::tempstr ret;
    const char *env = getenv(algo_lib::dev_envvar_CREDD_PASSWORD);
    if (env) {
        ret << env;
    } else {
        ret = credd::AskSecret(prompt);
    }
    return ret;
}

// Put the master key in _db.key, once: a key already held is kept.  When the
// store has a master record the password is asked and checked against it, and
// a wrong password ends the run.  When the store has none, this is the first
// use: a new password is asked twice, the master record is created and the
// store is written.
//
// Every verb that changes the store comes through here, so this is where
// -nostore refuses them.  Without the refusal such a verb takes the empty
// tables -nostore left in memory for a first use, asks for a new password and
// writes a store -- which is the one thing the caller asked not to happen, and
// it happens silently, leaving a sealed credential on a disk nobody meant to
// touch.
void credd::UnlockPrompt() {
    vrfy(!credd::_db.cmdline.nostore, tempstr() << "credd.nostore" << Keyval("comment", "-nostore keeps no store to change; a credential reaches the daemon with -seed"));
    if (credd::_db.key != "") {
        // already unlocked in this run
    } else if (credd::FindMaster()) {
        algo::tempstr password = credd::AskPassword("Master password: ");
        vrfy(credd::Unlock(password, credd::_db.key), tempstr()
             << "credd.badpassword" << Keyval("dir", credd::_db.dir)
             << Keyval("comment", "the master password does not match this store"));
    } else {
        prlog("credd.newstore" << Keyval("dir", credd::_db.dir)
              << Keyval("comment", "no master password yet; choose one"));
        algo::tempstr password = credd::AskPassword("New master password: ");
        algo::tempstr again = credd::AskPassword("Again: ");
        vrfy(password != "", tempstr() << "credd.emptypassword" << Keyval("comment", "the master password cannot be empty"));
        vrfy(password == again, tempstr() << "credd.mismatch" << Keyval("comment", "the two answers differ; nothing changed"));
        credd::InitMaster(password, credd::_db.key);
        credd::SaveStore();
        prlog("credd.master" << Keyval("dir", credd::_db.dir) << Keyval("comment", "master password set"));
    }
}

// Put a random key in _db.key, for a daemon that holds no store.
//
// A workstation's daemon unlocks its store with a password a person typed, and
// everything downstream of that -- sealing a secret, unsealing it to feed the
// agent -- works from the key the password derived.  A service host has no
// person and no store, so there is no password to derive anything from, and the
// secrets it holds arrive over its socket and die with it.
//
// Giving such a daemon a key drawn from the system random source keeps every
// other path identical: a seeded secret is sealed exactly as a stored one is,
// the agent loads both through the same unseal, and no code asks which kind of
// daemon it is running in.  The key is never written, never derived from
// anything, and nothing outside the process needs it, so what it protects is
// the process's own memory rather than any file.
void credd::EphemeralKey() {
    vrfy(credd::RandomBytes(32, credd::_db.key), tempstr() << "credd.norandom" << Keyval("comment", "the system random source refused"));
}

// -----------------------------------------------------------------------------

// Read a secret from where SRC names.  "-" is the terminal: one line with echo
// off under PROMPT, or with MULTILINE every line up to an empty one, echoed,
// since a pasted key is not typed.  "@file" is that file, whole.  Anything
// else is refused, so a secret is never given on the command line, where the
// process list would show it.  When stdin is a pipe, "-" reads from it.
algo::tempstr credd::ReadSecret(algo::strptr src, bool multiline, algo::strptr prompt) {
    algo::tempstr ret;
    if (src == "-") {
        if (multiline) {
            if (isatty(0)) {
                prlog(prompt);
            }
            algo::cstring line;
            bool more = true;
            while (more) {
                bool ok = isatty(0) ? algo_lib::ReadPlain("", line) : ReadPipeLine(line);
                more = ok && line != "";
                if (more) {
                    ret << line << eol;
                }
            }
        } else {
            ret = credd::AskSecret(prompt);
        }
    } else if (algo::StartsWithQ(src, "@")) {
        ret << algo::FileToString(algo::ch_RestFrom(src, 1));
    } else {
        vrfy(false, tempstr() << "credd.badsrc" << Keyval("src", src)
             << Keyval("comment", "-src is - for the terminal or @file; a secret is never passed on the command line"));
    }
    return ret;
}

// Read the secret of a credential of kind KIND from where -src names, and
// return it.  A multiline kind, an ssh key or a shell fragment, is read at the
// terminal as pasted lines up to an empty one; any other kind is one line read
// with echo off under PROMPT.
algo::tempstr credd::ReadKindSecret(credd::FKind &kind, algo::strptr prompt) {
    algo::strptr paste = kind.kind == creddb_Kind_kind_sshkey
        ? algo::strptr("Paste the private key, then an empty line:")
        : algo::strptr("Paste the lines, then an empty line:");
    return credd::ReadSecret(credd::_db.cmdline.src, kind.multiline, kind.multiline ? paste : prompt);
}

// The current time to the second.  A created stamp is a moment a person reads,
// and the nanoseconds a clock offers say nothing about it.
algo::UnTime credd::NowSec() {
    algo::UnTime ret = algo::CurrUnTime();
    ret.value -= ret.value % 1000000000LL;
    return ret;
}

// The moment the expiry TEXT names: YYYY-MM-DD, taken as the start of that day
// in local time.  Empty means never and yields zero.  Dies on any other form,
// so this is also how -expires is checked before it is stored.
algo::UnTime credd::ParseExpires(algo::strptr text) {
    algo::UnTime ret;
    if (text != "") {
        algo::tempstr iso;
        iso << text;
        iso << "T00:00:00";
        vrfy(elems_N(text) == 10 && algo::UnTime_ReadStrptrMaybe(ret, iso), tempstr()
             << "credd.badexpires" << Keyval("expires", text)
             << Keyval("comment", "write the expiry as YYYY-MM-DD"));
    }
    return ret;
}

// Add one credperm row to CRED per comma-separated name in PERMS, refusing a
// name the credential's kind does not offer.
void credd::AddCredperm(credd::FCred &cred, algo::strptr perms) {
    ind_beg(algo::Sep_curs, name, perms, ',') {
        algo::strptr trimmed = algo::Trimmed(name);
        if (trimmed != "") {
            vrfy(credd::ind_perm_Find(creddb::Perm_Concat_kind_name(cred.kind, trimmed)), tempstr()
                 << "credd.badperm" << Keyval("kind", cred.kind) << Keyval("perm", trimmed)
                 << Keyval("comment", "not a permission of this kind; credd -list shows the vocabulary"));
            credd::FCredperm &credperm = credd::credperm_Alloc();
            credperm.credperm = creddb::Credperm_Concat_cred_name(cred.cred, trimmed);
            vrfy(credd::credperm_XrefMaybe(credperm), algo_lib::_db.errtext);
        }
    }ind_end;
}

// The names of CRED's permissions, comma-separated, in the order they were
// added.
algo::tempstr credd::PermList(credd::FCred &cred) {
    algo::tempstr ret;
    algo::ListSep sep(",");
    ind_beg(credd::cred_c_credperm_curs, credperm, cred) {
        ret << sep << credd::name_Get(credperm);
    }ind_end;
    return ret;
}

// True when a credential named NAME is in the store.
bool credd::NameTakenQ(algo::strptr name) {
    return credd::ind_cred_Find(name) != NULL;
}

// Add REC to the store as a new credential, with the plaintext SECRET sealed
// under the master key, PERMS as its permissions, and the created stamp set
// now.  A link, kind link with linkto set, stores no secret.  The master password is asked here if it has not been in this run.
// The files are written and the running daemon told to reload.  REC's name
// must be free; see NameTakenQ.
void credd::StoreCred(creddb::Cred &rec, algo::strptr perms, algo::strptr secret) {
    credd::UnlockPrompt();
    credd::FCred &cred = credd::cred_Alloc();
    cred.cred = rec.cred;
    cred.kind = rec.kind;
    cred.server = rec.server;
    cred.account = rec.account;
    cred.pubkey = rec.pubkey;
    cred.created = credd::NowSec();
    cred.expires = rec.expires;
    cred.comment = rec.comment;
    cred.linkto = rec.linkto;
    // a link stands for another credential and has no secret of its own
    if (cred.kind != creddb_Kind_kind_link) {
        vrfy(credd::Seal(credd::_db.key, secret, cred.secret), "credd.seal");
    }
    vrfy(credd::cred_XrefMaybe(cred), algo_lib::_db.errtext);
    credd::AddCredperm(cred, perms);
    credd::SaveStore();
    credd::NotifyReload();
    prlog("credd.add" << Keyval("cred", cred.cred) << Keyval("kind", cred.kind)
          << Keyval("linkto", cred.linkto) << Keyval("perm", credd::PermList(cred)));
}

// The credential NAME stands for: itself, or for a link the credential it
// names.  NULL when NAME is unknown or its link points at nothing.  A link
// points at a credential that is not a link, so one hop resolves it.
credd::FCred *credd::Target(algo::strptr name) {
    credd::FCred *ret = credd::ind_cred_Find(name);
    if (ret && ret->kind == creddb_Kind_kind_link) {
        ret = credd::ind_cred_Find(ret->linkto);
    }
    return ret;
}

// The names of the links that stand for CRED, comma-separated; empty when
// none does.
algo::tempstr credd::LinkList(credd::FCred &cred) {
    algo::tempstr ret;
    algo::ListSep sep(",");
    ind_beg(credd::_db_cred_curs, other, credd::_db) {
        if (other.kind == creddb_Kind_kind_link && other.linkto == cred.cred && !other.removed) {
            ret << sep << other.cred;
        }
    }ind_end;
    return ret;
}

// Tell the running daemon the store changed, and point out a daemon from
// another build, which cannot have taken the change.  A store with no daemon
// is left alone; the next -start reads the files.
void credd::NotifyReload() {
    lib_cred::ReloadReq req;
    req.client = "credd";
    algo::cstring reply;
    algo::cstring err;
    lib_cred::Error error;
    if (credd::Ask(tempstr() << req, reply, err) && lib_cred::Error_ReadStrptrMaybe(error, reply)) {
        prerr("credd.reloadfail" << Keyval("msg", error.msg));
    }
    // a daemon from an older build reads the store under old names and reports
    // the reload done while it saw nothing, so the build is checked here too
    lib_cred::Status status;
    if (credd::ProbeDaemon(status)) {
        credd::WarnOldBuild(status);
    }
}

// A private directory for the moment a private key has to exist as a file:
// ssh-keygen writes one and reads one.  Mode 0700 inside the store; a file
// there is unlinked as soon as it has been read.
algo::tempstr credd::KeyTempDir() {
    algo::tempstr ret = algo::DirFileJoin(credd::_db.dir, "tmp");
    algo::CreateDirRecurse(ret, true, 0700);
    return ret;
}

// The public key of the private key PRIV, as `ssh-keygen -y` derives it, with
// whatever comment the key carries.  PRIV passes through a file in the store's
// private temp directory, unlinked before the call returns.  Empty when
// ssh-keygen cannot read PRIV, which is what an encrypted or damaged key looks
// like.
algo::tempstr credd::DerivePubkey(algo::strptr name, algo::strptr priv) {
    algo::tempstr ret;
    algo::tempstr keyfile = algo::DirFileJoin(credd::KeyTempDir(), name);
    (void)unlink(Zeroterm(keyfile));
    vrfy(algo::SafeStringToFile(priv, keyfile, 0600), tempstr() << "credd.writefail" << Keyval("file", keyfile));
    int status = 0;
    ret << algo::Trimmed(algo::SysEval(tempstr() << "ssh-keygen -y -P '' -f " << algo::strptr_ToBash(keyfile) << " 2>/dev/null"
                                       , FailokQ(true), 65536, false, &status));
    (void)unlink(Zeroterm(keyfile));
    if (status != 0) {
        ret = "";
    }
    return ret;
}
