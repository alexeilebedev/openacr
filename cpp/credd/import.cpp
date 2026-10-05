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
// Source: cpp/credd/import.cpp
//
// -add:% and -add:<file>: the guided path into the store for what a person
// already has under ~/.ssh before credd existed.  Three things live there.
// gli's server records in gli.ssim hold API tokens as plain text.  The aws
// tools read shell fragments from *.token files.  And ssh keys sit in files,
// some behind a passphrase.  Each file is named, what will happen to it and
// how it is retrieved afterwards is said, the person is asked, and the ones
// confirmed are added.  -add:% walks the directory and offers every file;
// -add:<file> offers that one.  Nothing found is deleted; the person removes
// the original once the credd path works.

#include "include/algo.h"
#include "include/credd.h"
#include <unistd.h>

// The directory the walk searches: the parent of the store, ~/.ssh for the
// default store.
static algo::tempstr SshDir() {
    return algo::tempstr(algo::GetDirName(credd::_db.dir));
}

// The host part of URL, with its port when the URL names one:
// https://gitlab.example.com/x → gitlab.example.com, https://gitlab.example.com:8443
// → gitlab.example.com:8443; empty when URL carries no scheme://host.
static algo::tempstr UrlHost(algo::strptr url) {
    algo::tempstr ret;
    algo::URL parsed;
    if (algo::URL_ReadStrptrMaybe(parsed, url) && parsed.protocol != "") {
        ret << parsed.server;
        if (parsed.port != -1) {
            ret << ":" << parsed.port;
        }
    }
    return ret;
}

// Say that NAME is already in the store, so this one is skipped.
static void SkipTaken(algo::strptr name, algo::strptr what) {
    prlog("credd.skip" << Keyval("name", name) << Keyval("source", what)
          << Keyval("comment", "a credential of this name is in the store already"));
}

// The token records of gli.ssim at FNAME, one credential each.  A record
// holds a token for a server as an account; it becomes a gitlab credential
// named by the server's host and port, or host@checkout for a record bound to
// a checkout, so two records for one host that differ in account alone share a
// name and the second is passed over with a line saying so.  When any is added, the person is offered a rewrite of the file
// with a binding line per record, which is how gli fetches the token from
// credd from then on.  The token stays in the record unless the person says
// to drop it, since a gli built without credd, in another checkout on the
// same machine, reads only that.
static void ImportGli(algo::strptr fname) {
    prlog("");
    prlog("gli keeps API tokens as plain text in " << fname << ".");
    // bindings already there, one line each as `remoteurl|checkout` between
    // newlines, so a record that has one is not offered again and a remoteurl
    // that is a suffix of another's does not read as bound
    algo::tempstr bound;
    bound << eol;
    // gli owns the record format and credd does not link gli, so both record
    // types of the file are read as tuples, by the head gli writes
    ind_beg(algo::FileLine_curs, line, fname) {
        algo::Tuple servercred;
        if (algo::Tuple_ReadStrptrMaybe(servercred, line) && servercred.head.value == "lib_gli.Servercred") {
            bound << algo::attr_GetString(servercred, "remoteurl") << "|" << algo::attr_GetString(servercred, "checkout") << eol;
        }
    }ind_end;
    algo::tempstr out;
    algo::tempstr added;
    int n_import = 0;
    ind_beg(algo::FileLine_curs, line, fname) {
        algo::Tuple tuple;
        algo::tempstr binding;
        bool record = algo::Tuple_ReadStrptrMaybe(tuple, line) && tuple.head.value == "lib_gli.Server"
            && algo::attr_GetString(tuple, "token") != "";
        algo::tempstr key;
        key << eol << algo::attr_GetString(tuple, "remoteurl") << "|" << algo::attr_GetString(tuple, "checkout") << eol;
        if (record && algo::FindStr(bound, key) >= 0) {
            prlog("credd.bound" << Keyval("remoteurl", algo::attr_GetString(tuple, "remoteurl")) << Keyval("checkout", algo::attr_GetString(tuple, "checkout"))
                  << Keyval("comment", "the record already carries a binding line"));
        } else if (record) {
            algo::strptr serverurl = algo::attr_GetString(tuple, "serverurl");
            algo::strptr checkout = algo::attr_GetString(tuple, "checkout");
            algo::strptr username = algo::attr_GetString(tuple, "username");
            creddb::Cred rec;
            algo::tempstr host = UrlHost(serverurl);
            algo::tempstr name;
            name << host;
            if (checkout != "") {
                name << "@" << algo::StripDirName(checkout);
            }
            rec.cred = strptr(name);
            credd::FCred *existing = host != "" ? credd::ind_cred_Find(rec.cred) : NULL;
            // the record's own credential, from an earlier walk whose rewrite
            // was declined or whose file was restored: same server, same
            // account and the same token under the seal, and it gets its
            // binding line.  A record whose token differs holds a rotation the
            // store does not, and is passed over with a line saying so.
            algo::cstring stored;
            bool same_row = existing && existing->kind == creddb_Kind_kind_gitlab && existing->server == serverurl && existing->account == username;
            bool own = same_row && credd::Unseal(credd::_db.key, existing->secret, stored) && strptr(stored) == algo::attr_GetString(tuple, "token");
            bool bind = false;
            bool offered = false;
            prlog("");
            if (host == "") {
                prerr("credd.badrecord" << Keyval("file", fname) << Keyval("remoteurl", algo::attr_GetString(tuple, "remoteurl"))
                      << Keyval("comment", "serverurl carries no scheme://host, so the token has no credential name; skipped"));
            } else if (own) {
                prlog("credd.bindhere" << Keyval("name", rec.cred) << Keyval("source", fname)
                      << Keyval("comment", "the store holds this record's token already; the record gets its binding line"));
                bind = true;
            } else if (same_row) {
                prlog("credd.skip" << Keyval("name", rec.cred) << Keyval("source", fname)
                      << Keyval("comment", "the store holds a different token for this server and account; renew the credential with -renew:<name> from the record's value, or add the record's under another name"));
            } else if (existing) {
                prlog("credd.skip" << Keyval("name", rec.cred) << Keyval("source", fname)
                      << Keyval("comment", "a credential of this name is in the store for another server or account; a second record for one host collides by name, so add it with -add:<name> -kind:gitlab"));
            } else {
                prlog("Record for remote " << algo::attr_GetString(tuple, "remoteurl") << ": a token for "
                      << serverurl << " as " << algo::attr_GetString(tuple, "username") << ".");
                prlog("  It becomes credential " << rec.cred << ", kind gitlab, sealed under the master password.");
                prlog("  A binding line naming it goes after the record, and gli built with credd fetches the token from credd.");
                offered = true;
            }
            if (offered) {
                if (credd::AskYes(tempstr() << "Add " << rec.cred << "?")) {
                    rec.kind = creddb_Kind_kind_gitlab;
                    rec.server = serverurl;
                    rec.account = username;
                    rec.comment = algo::Comment(strptr("from gli.ssim"));
                    credd::StoreCred(rec, "", algo::attr_GetString(tuple, "token"));
                    bind = true;
                }
            }
            if (bind) {
                algo::Tuple servercred;
                servercred.head.value = "lib_gli.Servercred";
                algo::attr_Add(servercred, "remoteurl", algo::attr_GetString(tuple, "remoteurl"));
                algo::attr_Add(servercred, "checkout", checkout);
                algo::attr_Add(servercred, "cred", rec.cred);
                binding << servercred;
                added << line << eol;
                n_import++;
            }
        }
        out << line << eol;
        if (binding != "") {
            out << binding << eol;
        }
    }ind_end;
    if (n_import > 0) {
        prlog("");
        prlog("The tokens stay in the records, so a gli built without credd, in another checkout on this machine, keeps working.");
        prlog("Once every gli on this machine is built with credd, run gli -auth in each checkout and answer - at its token prompt to drop the token from the record.");
        if (credd::AskYes(tempstr() << "Rewrite " << fname << " with the binding lines?")) {
            if (credd::AskYes("Also drop the tokens from the records now? Only a gli built with credd can then use them")) {
                // the lines added this run, whole: a record whose line extends
                // another's, the same record without a checkout, keeps its token
                algo::tempstr addedkey;
                addedkey << eol << added;
                algo::tempstr dropped;
                ind_beg(algo::Line_curs, outline, out) {
                    algo::Tuple tuple;
                    bool mine = algo::FindStr(addedkey, tempstr() << eol << outline << eol) >= 0;
                    if (mine && algo::Tuple_ReadStrptrMaybe(tuple, outline)) {
                        algo::attr_Set(tuple, "token", "");
                        dropped << tuple << eol;
                    } else {
                        dropped << outline << eol;
                    }
                }ind_end;
                out = dropped;
            }
            vrfy(algo::SafeStringToFile(out, fname, 0600), tempstr() << "credd.writefail" << Keyval("file", fname));
            prlog("credd.rewrite" << Keyval("file", fname) << Keyval("n_record", n_import));
        }
    }
}

// True when KIND holds a token: it names a place to mint one or says how,
// where sshkey and link do neither.
bool credd::TokenKindQ(credd::FKind &kind) {
    return kind.tokenurl != "" || kind.howto != "";
}

// The kind TEXT, a secret's content, suggests: the token kind whose prefix
// (creddb.kind prefix, one or more starts separated by spaces) the trimmed
// text begins with, else aws, which is what a *.token file with no
// recognizable start has held so far.  A prefix is matched at the front and
// not searched for inside the text, because an AWS shell fragment can name an
// arbitrary host or carry any other mark anywhere in itself.  The table is the one
// place a kind is described, so a kind added as a row is guessed here without
// a change to this code.
static algo::strptr GuessKind(algo::strptr text) {
    algo::strptr ret = creddb_Kind_kind_aws;
    algo::strptr trimmed = algo::Trimmed(text);
    bool found = false;
    ind_beg(credd::_db_kind_curs, kind, credd::_db) {
        ind_beg(algo::Word_curs, prefix, kind.prefix) {
            if (!found && algo::StartsWithQ(trimmed, prefix)) {
                ret = kind.kind;
                found = true;
            }
        }ind_end;
    }ind_end;
    return ret;
}

// The kinds a person may name, for a prompt: every kind that holds a token,
// and sshkey when WITH_SSHKEY, joined by commas in the table's order.
algo::tempstr credd::KindChoices(bool with_sshkey) {
    algo::tempstr ret;
    algo::ListSep sep(", ");
    ind_beg(credd::_db_kind_curs, kind, credd::_db) {
        if (credd::TokenKindQ(kind) || (with_sshkey && kind.kind == creddb_Kind_kind_sshkey)) {
            ret << sep << kind.kind;
        }
    }ind_end;
    return ret;
}

// Fill _db.c_importfile with the files under the store's parent directory
// matching PATTERN, sorted by path.  A directory listing comes in the order
// the filesystem keeps it, which differs from one machine to the next, so the
// walk sorts before it asks: a person, or a scripted answer, then meets the
// files in the same order everywhere.
static void ListSshDir(algo::strptr pattern) {
    credd::c_importfile_RemoveAll();
    credd::importfile_RemoveAll();
    ind_beg(algo::Dir_curs, entry, algo::DirFileJoin(SshDir(), pattern)) if (!entry.is_dir) {
        credd::FImportfile &importfile = credd::importfile_Alloc();
        importfile.pathname = entry.pathname;
        importfile.filename = entry.filename;
        vrfy(credd::importfile_XrefMaybe(importfile), algo_lib::_db.errtext);
    }ind_end;
    credd::c_importfile_QuickSort();
}

// True when TEXT, a file's content, can be a token: it is not empty, it is
// text, and it is short.  A private key or a binary file is neither.
static bool TokenTextQ(algo::strptr text) {
    bool ret = algo::ch_N(algo::Trimmed(text)) > 0 && algo::ch_N(text) <= 4096;
    for (int i = 0; i < algo::ch_N(text); i++) {
        char ch = text.elems[i];
        if (ch == 0 || (ch > 0 && ch < 0x20 && ch != '\n' && ch != '\r' && ch != '\t')) {
            ret = false;
        }
    }
    return ret;
}

// Whether the file at PATHNAME, named FILENAME within its directory, is a
// token file: a name that is not all suffix, and text that reads as a token.
// WHY receives the refusal's wording when it is not.  The one judgment of a
// token file: the walk applies it to each *.token it offers, and -add:<file>
// applies it before the master password is asked, so a file that will be
// refused costs no prompt and leaves no store behind.
bool credd::TokenFileQ(algo::strptr pathname, algo::strptr filename, algo::cstring &why) {
    bool named = algo::StripExt(filename) != "";
    bool token = named && TokenTextQ(algo::FileToString(pathname));
    if (!named) {
        why << "the file's name is all suffix, so it names no credential";
    } else if (!token) {
        why << "not a text file holding a token";
    }
    return token;
}

// Offer the token in the file at PATHNAME, named FILENAME within its
// directory, as one credential named by the file without its suffix, or by
// the whole file name when it has none, the whole file as the secret.  The
// kind is guessed from the content and confirmed.  A file whose content is
// not a token, and a name the store holds already, are passed over with a
// line.  Answer whether the file was fit (TokenFileQ), so a caller that
// offered one file on purpose can fail on a refusal the walk only reports.
static bool ImportTokenFile(algo::strptr pathname, algo::strptr filename) {
    algo::tempstr text = algo::FileToString(pathname);
    creddb::Cred rec;
    rec.cred = algo::StripExt(filename);
    algo::tempstr guess(GuessKind(text));
    algo::cstring why;
    bool token = credd::TokenFileQ(pathname, filename, why);
    bool taken = token && credd::NameTakenQ(rec.cred);
    bool offered = token && !taken;
    prlog("");
    if (!token) {
        prerr("credd.badfile" << Keyval("file", pathname) << Keyval("comment", tempstr() << why << "; skipped"));
    } else if (taken) {
        SkipTaken(rec.cred, pathname);
    } else {
        prlog(pathname << " holds a token, " << algo::ch_N(text) << " bytes, looks like " << guess << ".");
        prlog("  It becomes credential " << rec.cred << ", the file's content as the secret.");
        prlog("  Retrieve it with credd -show:" << rec.cred << ", or eval \"$(credd -show:" << rec.cred << ")\" for a shell fragment.");
        prlog("  The file stays until you remove it.");
    }
    if (offered) {
        if (credd::AskYes(tempstr() << "Add " << rec.cred << "?")) {
            algo::tempstr kindname = credd::AskPlain(tempstr() << "Kind (" << credd::KindChoices(false) << ")", guess);
            credd::FKind *kind = credd::ind_kind_Find(kindname);
            if (!kind || !credd::TokenKindQ(*kind)) {
                prerr("credd.badkind" << Keyval("kind", kindname) << Keyval("comment", "not a kind of token; skipped"));
            } else {
                rec.kind = kind->kind;
                rec.comment = algo::Comment(strptr(tempstr() << "from " << filename));
                algo::tempstr plain;
                plain << algo::Trimmed(text);
                credd::StoreCred(rec, "", plain);
            }
        }
    }
    return token;
}

// True when FILE's first line says it is a private key.
static bool PrivateKeyFileQ(algo::strptr file) {
    bool ret = false;
    algo::tempstr head;
    ind_beg(algo::FileLine_curs, line, file) {
        head << line;
        break;
    }ind_end;
    ret = algo::StartsWithQ(head, "-----BEGIN") && algo::FindStr(head, "PRIVATE KEY") >= 0;
    return ret;
}

// The private key in FILE with its passphrase removed, through a copy in the
// store's private temp directory.  ssh-keygen asks for the passphrase through
// SSH_ASKPASS, so it never appears on a command line; the asker is a one-line
// script in the same directory that prints an environment variable, and both
// are unlinked before returning.  Empty when the passphrase is wrong.
static algo::tempstr Decrypt(algo::strptr name, algo::strptr file, algo::strptr passphrase) {
    algo::tempstr ret;
    algo::tempstr tmpdir = credd::KeyTempDir();
    algo::tempstr copy = algo::DirFileJoin(tmpdir, name);
    algo::tempstr asker = algo::DirFileJoin(tmpdir, "askpass.sh");
    (void)unlink(Zeroterm(copy));
    vrfy(algo::SafeStringToFile(algo::FileToString(file), copy, 0600), tempstr() << "credd.writefail" << Keyval("file", copy));
    vrfy(algo::SafeStringToFile("#!/bin/sh\nprintf '%s\\n' \"$CREDD_ASKPASS_ANSWER\"\n", asker, 0700), tempstr() << "credd.writefail" << Keyval("file", asker));
    algo::tempstr answer;
    answer << passphrase;
    setenv(algo_lib::dev_envvar_CREDD_ASKPASS_ANSWER, Zeroterm(answer), 1);
    algo::tempstr cmd;
    cmd << "SSH_ASKPASS_REQUIRE=force SSH_ASKPASS=" << algo::strptr_ToBash(asker)
        << " ssh-keygen -q -p -N '' -f " << algo::strptr_ToBash(copy) << " </dev/null >/dev/null 2>&1";
    int rc = algo::SysCmd(cmd, FailokQ(true));
    unsetenv(algo_lib::dev_envvar_CREDD_ASKPASS_ANSWER);
    if (rc == 0) {
        ret << algo::FileToString(copy);
    }
    (void)unlink(Zeroterm(copy));
    (void)unlink(Zeroterm(asker));
    return ret;
}

// Offer the private key in the file at PATHNAME, named FILENAME within its
// directory, as one sshkey credential named by the file.  A key behind a
// passphrase is asked for it and stored without one, since the master
// password is what guards the store.  The file stays where it is.  A name the
// store holds already is skipped.
static void ImportKeyFile(algo::strptr pathname, algo::strptr filename) {
    creddb::Cred rec;
    rec.cred = filename;
    algo::tempstr priv = algo::FileToString(pathname);
    bool taken = credd::NameTakenQ(rec.cred);
    // ssh-keygen is forked only for a key the walk may add
    algo::tempstr pubkey = taken ? algo::tempstr() : credd::DerivePubkey(rec.cred, priv);
    bool encrypted = pubkey == "";
    prlog("");
    if (taken) {
        SkipTaken(rec.cred, pathname);
    } else {
        prlog(pathname << " is a private key" << (encrypted ? ", behind a passphrase." : "."));
        prlog("  It becomes credential " << rec.cred << ", kind sshkey, sealed under the master password.");
        prlog("  The daemon loads it into the agent at " << credd::AgentSock() << "; with SSH_AUTH_SOCK naming that, ssh finds it.");
        prlog("  Once the key is in the store the file is no longer needed, and you can remove it.  credd never removes it.");
    }
    if (!taken) {
        if (credd::AskYes(tempstr() << "Add " << rec.cred << "?")) {
            if (encrypted) {
                algo::tempstr passphrase = credd::AskSecret(tempstr() << "Passphrase of " << filename << ": ");
                priv = Decrypt(rec.cred, pathname, passphrase);
                pubkey = priv != "" ? credd::DerivePubkey(rec.cred, priv) : algo::tempstr();
            }
            if (pubkey == "") {
                prerr("credd.badkey" << Keyval("name", rec.cred)
                      << Keyval("comment", encrypted ? "wrong passphrase, or ssh-keygen cannot read it; skipped" : "ssh-keygen cannot read it; skipped"));
            } else {
                algo::strptr keycomment = algo::Pathcomp(pubkey, " LR LR");
                rec.kind = creddb_Kind_kind_sshkey;
                rec.pubkey = pubkey;
                rec.comment = algo::Comment(strptr(credd::AskPlain("Comment", keycomment)));
                rec.expires = strptr(credd::AskPlain("Expires (YYYY-MM-DD, empty = never)", ""));
                (void)credd::ParseExpires(rec.expires);
                credd::StoreCred(rec, "", priv);
            }
        }
    }
}

// Walk the tokens and keys under the store's parent directory and add each
// one confirmed: what -add:% does.  The master password is asked first, so a
// new store is set up before anything is offered, and then not again.  gli's
// records come first, then the *.token files, then the private keys, each
// set in path order; a .pub file is the public half of a key and is passed
// over.  Add asks the master password before and prints the summary after.
void credd::ImportDir() {
    prlog("credd -add:% walks " << SshDir() << " for credentials that predate credd and adds the ones you confirm to "
          << credd::_db.dir << ".");
    prlog("Each is sealed under the master password.  Nothing found here is deleted.");
    algo::tempstr gli = algo::DirFileJoin(SshDir(), "gli.ssim");
    if (algo::FileQ(gli)) {
        ImportGli(gli);
    }
    ListSshDir("*.token");
    ind_beg(credd::_db_c_importfile_curs, entry, credd::_db) {
        ImportTokenFile(entry.pathname, entry.filename);
    }ind_end;
    ListSshDir("*");
    ind_beg(credd::_db_c_importfile_curs, entry, credd::_db) if (!algo::EndsWithQ(entry.filename, ".pub") && PrivateKeyFileQ(entry.pathname)) {
        ImportKeyFile(entry.pathname, entry.filename);
    }ind_end;
}

// What the file at PATHNAME would be added as -- gli, key or token -- or
// empty when it is refused, with WHY saying so.  A gli.ssim is gli's records,
// a private key becomes an sshkey named after the file, and a text file
// holding a token becomes a token named after the file without its suffix.  A
// .pub file is the public half of a key, which the walk passes over.  This is
// the judgment -add:<file> makes before it asks the master password, so a file
// that will be refused costs no prompt and leaves no store behind, and
// ImportFile acts on the class decided here.
algo::tempstr credd::ImportFileClass(algo::strptr pathname, algo::cstring &why) {
    algo::strptr filename = algo::StripDirName(pathname);
    algo::tempstr ret;
    bool pub = algo::EndsWithQ(filename, ".pub");
    algo::cstring tokenwhy;
    if (filename == "gli.ssim") {
        ret << "gli";
    } else if (pub) {
        why << "the public half of a key; add the private key file";
    } else if (PrivateKeyFileQ(pathname)) {
        ret << "key";
    } else if (credd::TokenFileQ(pathname, filename, tokenwhy)) {
        ret << "token";
    } else {
        why << "not a gli.ssim, a private key or a text file holding a token named by its file";
    }
    return ret;
}

// Offer the one file at PATHNAME, already judged CLASS by ImportFileClass, the
// way the walk offers what it finds: what -add:<file> does once the master
// password is in hand.  Each offer is explained and confirmed.
void credd::ImportFile(algo::strptr pathname, algo::strptr fileclass) {
    algo::strptr filename = algo::StripDirName(pathname);
    if (fileclass == "gli") {
        ImportGli(pathname);
    } else if (fileclass == "key") {
        ImportKeyFile(pathname, filename);
    } else {
        vrfy(ImportTokenFile(pathname, filename), tempstr() << "credd.badfile" << Keyval("file", pathname));
    }
}
