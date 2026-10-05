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
// Header: include/credd.h
//

#include "include/lib_cred.h"
#include "include/lib_json.h"
#include "include/gen/credd_gen.h"
#include "include/gen/credd_gen.inl.h"

namespace credd { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/credd/credd.cpp
    //

    // Report the daemon's status, or that none answers.
    void Status();

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
    void Start();

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
    void Seed();

    // Ask the daemon to stop, and report the answer.
    void Stop();

    // Print the secret of the credential -show names, as the daemon opens it,
    // with no decoration, so $(credd -show:name) is the value.  A refusal is
    // reported and the exit code is 1.
    void Show();

    // List every credential whose expiry falls within -window days from now, with
    // the days left.  A credential that never expires is not listed.
    void Check();

    // Add what -add names.  `%` walks the store's parent directory and offers
    // every token and key it holds; a path, which has a slash in it, offers the
    // file it names the same way; any other value is the name of the credential
    // to add, its secret read per -src.  A name is never read as a file, so
    // `credd -add:id_ed25519` run from inside ~/.ssh adds a credential of that
    // name and leaves the file of that name alone.
    void Add();

    // Make an ed25519 key under the name -keygen gives and add it sealed.
    // ssh-keygen writes the pair into the store's private temp directory, both
    // files are read and unlinked, and the public key is printed so it can be
    // installed where the key will log in; with -pubfile it is written to that
    // file as well, which is what a tool that takes a key file reads.
    void Keygen();

    // Renew the token -renew names: print the page where its kind reissues a
    // token, with the server, name and permissions filled in, then read the new
    // value per -src, ask the master password, and reseal.  A new -expires
    // replaces the old; without one the old expiry stands.
    void Renew();

    // Remove the credential -remove names.  The master password is asked, as for
    // every change to the store.  The row is marked and left out of the files
    // written; a removed key leaves the agent at the daemon's reload.  A
    // credential that links stand for stays until the links are removed, so a
    // tool asking by the link's name never finds a name that points at nothing.
    void Remove();

    // Change the master password.  The old one is asked and checked, the new one
    // twice; every secret is opened under the old key and sealed under the new,
    // the master record gets a fresh salt and verifier, and the store is written.
    // A running daemon holds the old key, so it is stopped and has to be started
    // again.
    void Passwd();

    // List every credential; needs no daemon.  With -format:auto or text the
    // listing is a table of cred, kind, server and comment.  With ssim it is the
    // rows as the store holds them, the sealed secret written as the word sealed
    // and a token's permissions indented under it.  With json it is the cred rows
    // as one array.
    void List();

    // Print the whole store, every secret still sealed, as ssim tuples (-format:auto
    // or ssim) or, with -format:json, as one object holding an array per table.
    // What prints is what the files hold, so the ssim form placed under the store's
    // creddb directory is the store restored.
    void Dump();

    // Run the verbs given, in the order command.credd declares them.  With none,
    // report the daemon's status.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:credd

    // -------------------------------------------------------------------
    // cpp/credd/daemon.cpp
    //

    // Path of the daemon's socket, inside the store.
    algo::tempstr DaemonSock();

    // Path of the ssh agent's socket: ssh_auth_sock beside the store, which is
    // ~/.ssh/ssh_auth_sock for the default store.  A shell exports SSH_AUTH_SOCK
    // naming it, once, and every login finds the same agent.
    algo::tempstr AgentSock();

    // Send LINE to this store's daemon and return its reply, as lib_cred::Request
    // reports it.
    bool Ask(algo::strptr line, algo::cstring &out_reply, algo::cstring &out_err);

    // Ask this store's daemon for its status.  Returns true when one answers, with
    // its status in OUT.  A daemon built before the status tuple had its present
    // shape answers with a tuple this binary cannot read; that still counts as
    // answering, and OUT.build then says unreadable, so the caller reports an old
    // daemon rather than a missing one.
    bool ProbeDaemon(lib_cred::Status &out);

    // True when STATUS came from a daemon built from the same source as this
    // binary.  A daemon outlives every rebuild, and one left over from an older
    // build reads the store under old names or answers in old shapes, so a
    // credential the command line just added is invisible to it.
    bool SameBuildQ(lib_cred::Status &status);

    // Say that the running daemon is from another build than this binary, and
    // what to do about it.  Nothing is said when the builds agree.
    void WarnOldBuild(lib_cred::Status &status);

    // Serve the store on its socket, with the master key in _db.key, until a stop
    // request or a signal.  The process refuses to be dumped and locks its memory,
    // so the key does not reach a core file or swap.  The ssh agent is started
    // and loaded first, then the socket is bound, mode 0600, and the loop runs.
    // On the way out the agent is emptied and stopped, the socket removed, and
    // the key overwritten.
    void RunDaemon();

    // -------------------------------------------------------------------
    // cpp/credd/import.cpp
    //

    // True when KIND holds a token: it names a place to mint one or says how,
    // where sshkey and link do neither.
    bool TokenKindQ(credd::FKind &kind);

    // The kinds a person may name, for a prompt: every kind that holds a token,
    // and sshkey when WITH_SSHKEY, joined by commas in the table's order.
    algo::tempstr KindChoices(bool with_sshkey);

    // Whether the file at PATHNAME, named FILENAME within its directory, is a
    // token file: a name that is not all suffix, and text that reads as a token.
    // WHY receives the refusal's wording when it is not.  The one judgment of a
    // token file: the walk applies it to each *.token it offers, and -add:<file>
    // applies it before the master password is asked, so a file that will be
    // refused costs no prompt and leaves no store behind.
    bool TokenFileQ(algo::strptr pathname, algo::strptr filename, algo::cstring &why);

    // Walk the tokens and keys under the store's parent directory and add each
    // one confirmed: what -add:% does.  The master password is asked first, so a
    // new store is set up before anything is offered, and then not again.  gli's
    // records come first, then the *.token files, then the private keys, each
    // set in path order; a .pub file is the public half of a key and is passed
    // over.  Add asks the master password before and prints the summary after.
    void ImportDir();

    // What the file at PATHNAME would be added as -- gli, key or token -- or
    // empty when it is refused, with WHY saying so.  A gli.ssim is gli's records,
    // a private key becomes an sshkey named after the file, and a text file
    // holding a token becomes a token named after the file without its suffix.  A
    // .pub file is the public half of a key, which the walk passes over.  This is
    // the judgment -add:<file> makes before it asks the master password, so a file
    // that will be refused costs no prompt and leaves no store behind, and
    // ImportFile acts on the class decided here.
    algo::tempstr ImportFileClass(algo::strptr pathname, algo::cstring &why);

    // Offer the one file at PATHNAME, already judged CLASS by ImportFileClass, the
    // way the walk offers what it finds: what -add:<file> does once the master
    // password is in hand.  Each offer is explained and confirmed.
    void ImportFile(algo::strptr pathname, algo::strptr fileclass);

    // -------------------------------------------------------------------
    // cpp/credd/seal.cpp
    //

    // Fill OUT with N bytes from the system random source.  False when the source
    // fails, which leaves OUT empty.
    bool RandomBytes(int n, algo::cstring &out);

    // Derive the 32-byte master key from PASSWORD and SALT with scrypt, at a cost
    // of N=2^15, r=8, p=1, which takes a fraction of a second and 32MB.  False
    // when libcrypto refuses, and OUT_KEY is then empty.
    bool DeriveKey(algo::strptr password, algo::strptr salt, algo::cstring &out_key);

    // Seal PLAIN under the 32-byte KEY: a fresh 12-byte nonce, the ciphertext,
    // and the 16-byte tag, concatenated and written to OUT as base64.  False when
    // KEY has the wrong length or libcrypto fails.
    bool Seal(algo::strptr key, algo::strptr plain, algo::cstring &out);

    // Open SEALED, a value Seal produced, under KEY into OUT.  False when the
    // base64 does not decode, the value is too short to carry a nonce and a tag,
    // KEY has the wrong length, or the tag does not verify, which is what a wrong
    // key or a damaged record looks like.  OUT is empty on failure.
    bool Unseal(algo::strptr key, algo::strptr sealed, algo::cstring &out);

    // -------------------------------------------------------------------
    // cpp/credd/store.cpp
    //

    // Path of the store: -dir when given, else $HOME/.ssh/credd.
    algo::tempstr StoreDir();

    // Read every table from the store into memory, replacing what was there.
    // Creates the store directory, mode 0700, when it does not exist, so the first
    // run of any verb leaves an empty store behind.  Dies naming the file and line
    // when a row does not load, which includes a credential whose kind the binary
    // does not know.
    //
    // Under -nostore there are no tables to read and none are created.  The
    // directory is still made, because the daemon's socket lives in it, and the
    // pools are left empty for a seed request to fill.
    void LoadStore();

    // Write every table of the store back to disk.  A credential marked removed
    // is left out, and so are its permissions.
    void SaveStore();

    // The store's master record, or NULL when no password has been set yet.
    credd::FMaster *FindMaster();

    // Derive the key for PASSWORD from the master record's salt and prove it by
    // opening the verifier.  OUT_KEY holds the key when the password is right;
    // otherwise it is empty and the function returns false.
    bool Unlock(algo::strptr password, algo::cstring &out_key);

    // Make PASSWORD the store's master password: a fresh salt, and a verifier
    // sealed under the key derived from it, in the master record, which is created
    // when the store has none.  OUT_KEY holds the new key.  Nothing is written to
    // disk; the caller saves the store once every secret is sealed under the key.
    void InitMaster(algo::strptr password, algo::cstring &out_key);

    // Ask PROMPT and return the answer: at a terminal the line typed, echoed;
    // under a pipe its next line.  DFLT is offered in the prompt and is the answer
    // when the line is empty.  A verb that asks this at a terminal only, since a
    // script gives every option, tests isatty first.
    algo::tempstr AskPlain(algo::strptr prompt, algo::strptr dflt);

    // Ask QUESTION as a yes-or-no question, answered with y; anything else is no.
    bool AskYes(algo::strptr question);

    // Ask PROMPT for a line that is not shown as it is typed: at a terminal with
    // echo off, under a pipe its next line.  Dies when neither can answer.
    algo::tempstr AskSecret(algo::strptr prompt);

    // Ask PROMPT for the master password: $CREDD_PASSWORD when it is set, which is
    // how a test, a shell rc that already holds the password, or a service host
    // supplies it without a terminal; otherwise as AskSecret does.
    algo::tempstr AskPassword(algo::strptr prompt);

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
    void UnlockPrompt();

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
    void EphemeralKey();

    // Read a secret from where SRC names.  "-" is the terminal: one line with echo
    // off under PROMPT, or with MULTILINE every line up to an empty one, echoed,
    // since a pasted key is not typed.  "@file" is that file, whole.  Anything
    // else is refused, so a secret is never given on the command line, where the
    // process list would show it.  When stdin is a pipe, "-" reads from it.
    algo::tempstr ReadSecret(algo::strptr src, bool multiline, algo::strptr prompt);

    // Read the secret of a credential of kind KIND from where -src names, and
    // return it.  A multiline kind, an ssh key or a shell fragment, is read at the
    // terminal as pasted lines up to an empty one; any other kind is one line read
    // with echo off under PROMPT.
    algo::tempstr ReadKindSecret(credd::FKind &kind, algo::strptr prompt);

    // The current time to the second.  A created stamp is a moment a person reads,
    // and the nanoseconds a clock offers say nothing about it.
    algo::UnTime NowSec();

    // The moment the expiry TEXT names: YYYY-MM-DD, taken as the start of that day
    // in local time.  Empty means never and yields zero.  Dies on any other form,
    // so this is also how -expires is checked before it is stored.
    algo::UnTime ParseExpires(algo::strptr text);

    // Add one credperm row to CRED per comma-separated name in PERMS, refusing a
    // name the credential's kind does not offer.
    void AddCredperm(credd::FCred &cred, algo::strptr perms);

    // The names of CRED's permissions, comma-separated, in the order they were
    // added.
    algo::tempstr PermList(credd::FCred &cred);

    // True when a credential named NAME is in the store.
    bool NameTakenQ(algo::strptr name);

    // Add REC to the store as a new credential, with the plaintext SECRET sealed
    // under the master key, PERMS as its permissions, and the created stamp set
    // now.  A link, kind link with linkto set, stores no secret.  The master password is asked here if it has not been in this run.
    // The files are written and the running daemon told to reload.  REC's name
    // must be free; see NameTakenQ.
    void StoreCred(creddb::Cred &rec, algo::strptr perms, algo::strptr secret);

    // The credential NAME stands for: itself, or for a link the credential it
    // names.  NULL when NAME is unknown or its link points at nothing.  A link
    // points at a credential that is not a link, so one hop resolves it.
    credd::FCred *Target(algo::strptr name);

    // The names of the links that stand for CRED, comma-separated; empty when
    // none does.
    algo::tempstr LinkList(credd::FCred &cred);

    // Tell the running daemon the store changed, and point out a daemon from
    // another build, which cannot have taken the change.  A store with no daemon
    // is left alone; the next -start reads the files.
    void NotifyReload();

    // A private directory for the moment a private key has to exist as a file:
    // ssh-keygen writes one and reads one.  Mode 0700 inside the store; a file
    // there is unlinked as soon as it has been read.
    algo::tempstr KeyTempDir();

    // The public key of the private key PRIV, as `ssh-keygen -y` derives it, with
    // whatever comment the key carries.  PRIV passes through a file in the store's
    // private temp directory, unlinked before the call returns.  Empty when
    // ssh-keygen cannot read PRIV, which is what an encrypted or damaged key looks
    // like.
    algo::tempstr DerivePubkey(algo::strptr name, algo::strptr priv);
}
