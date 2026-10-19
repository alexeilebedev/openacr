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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/credd.cpp
//
// Comptests for credd, the daemon that holds a session's credentials.  Every
// command runs with HOME set to the test's own directory, so the store under
// test is $tempdir/home/.ssh/credd and the daemon's sockets sit beside it.  The
// master password comes from CREDD_PASSWORD, which is how a run with no
// terminal supplies it, and a secret comes from a file named by -src.  The
// daemon of the second test is started in the foreground under the harness, so
// its pid is tracked and its exit is asserted.

#include "include/algo.h"
#include "include/atf_comp.h"
#include <unistd.h>

// -----------------------------------------------------------------------------
// Value of a $-expression in the comptest's variable scope.
static tempstr CreddVal(strptr expr) {
    tempstr out;
    Ins(&atf_comp::_db.R,out,expr,false);
    return out;
}

// -----------------------------------------------------------------------------
// Run one shell command of the scenario's own setup, silently.
static void CreddQuiet(strptr cmd) {
    SysCmd(CreddVal(tempstr()<<"( "<<cmd<<" ) >/dev/null 2>&1"),FailokQ(true),DryrunQ(false),EchoQ(false));
}

// -----------------------------------------------------------------------------
// Run credd with ARGS against the test's store, with the master password
// PASSWORD in the environment, or none when PASSWORD is empty, and wait for it
// to exit with EXPECT_EXIT.
static void CreddRun(strptr password, strptr args, int expect_exit) {
    if (password != "") {
        atf_comp::SetEnv(algo_lib::dev_envvar_CREDD_PASSWORD,password);
    } else {
        atf_comp::UnsetEnv(algo_lib::dev_envvar_CREDD_PASSWORD);
    }
    atf_comp::FProc &proc = atf_comp::ProcStart(tempstr()<<"$bindir/credd "<<args);
    atf_comp::ProcWait(proc,expect_exit);
}

// -----------------------------------------------------------------------------
// Run CMD, a shell pipeline, in the scenario's environment and wait for it to
// exit with EXPECT_EXIT.  For the commands whose raw output would differ on
// every run, and whose pipeline reduces it to what the test asserts.  The
// $-variables in CMD are left for the harness to substitute, so the line it
// records names $bindir and not the build directory of one configuration.
static void CreddSh(strptr cmd, int expect_exit) {
    atf_comp::FProc &proc = atf_comp::ProcStart(tempstr()<<"bash -c "<<strptr_ToBash(cmd));
    atf_comp::ProcWait(proc,expect_exit);
}

// -----------------------------------------------------------------------------
// The home directory and store the scenario runs in, and a file holding a token
// value for -src to read.
static void CreddSetup() {
    atf_comp::SetVar("HOMEDIR",CreddVal("$tempdir/home"));
    atf_comp::SetVar("STORE",CreddVal("$HOMEDIR/.ssh/credd"));
    atf_comp::SetEnv(algo_lib::dev_envvar_HOME,"$HOMEDIR");
    CreddQuiet("mkdir -p $HOMEDIR/.ssh");
    CreddQuiet("echo glpat-atfcomp-token > $tempdir/tok");
}

// -----------------------------------------------------------------------------
// Drive the store without a daemon, and check each change that credd takes or
// refuses:
// 1. Add token gl to an empty store with password pw1, and check that the first
//    change sets the master password.  List the store as a table, as ssim and as
//    json.
// 2. Add a github token with the permission bogus, add gl a second time, add a
//    token with no kind, and add a token whose secret is on the command line.
//    Check that credd refuses each.  It checks a token's permissions against its
//    kind, and takes a name once.
// 3. Show gl, and check that credd refuses to show a secret with no daemon.
// 4. Run -check with a long window and with a short one, and check that it finds
//    what expires.
// 5. Add an aws credential, remove gl, and check that a second remove of gl fails.
// 6. Change the password to pw2 with -passwd, which asks for the old one.  Check
//    that pw1 then fails to open the store and pw2 opens it.
// 7. Dump what the files hold, and dump it as json.  Check that the json form has
//    one array per table, and that credd refuses -dump -format:text.
// 8. List the store's files, and check that the removed row is gone from them.
void atf_comp::comptest_credd_Store() {
    CreddSetup();
    CreddRun("pw1","-add:gl -kind:gitlab -server:https://gitlab.example.com -account:me -perm:api,read_api -expires:2026-12-31 -src:@$tempdir/tok",0);
    // the default listing is a table; ssim shows the row as the store holds
    // it with the permissions under it, json the cred rows as one array
    CreddRun("pw1","-list",0);
    CreddRun("pw1","-list -format:ssim",0);
    CreddSh("$bindir/credd -list -format:json | grep -o '\"[a-z_]*\":' | sort | uniq -c | sed 's/^ *//'",0);
    CreddRun("pw1","-add:gh -kind:github -perm:repo,bogus -src:@$tempdir/tok",1);
    CreddRun("pw1","-add:gl -kind:gitlab -src:@$tempdir/tok",1);
    // the refusal lists the kinds the tree defines, so only the refusal is kept
    CreddSh("CREDD_PASSWORD=pw1 $bindir/credd -add:nokind -src:@$tempdir/tok 2>&1 | grep -o \"credd.badkind  kind:[^ ]*\"; exit ${PIPESTATUS[0]}",1);
    CreddRun("pw1","-add:cmdline -kind:github -src:glpat-cmdline",1);
    CreddRun("pw1","-show:gl",1);
    CreddRun("pw1","-check -window:30000",0);
    CreddRun("pw1","-check -window:1",0);
    CreddRun("pw1","-add:aws -kind:aws -account:keeper -comment:\"three values as one tuple\" -src:@$tempdir/tok",0);
    CreddRun("pw1","-remove:gl",0);
    CreddRun("pw1","-remove:gl",1);
    CreddRun("pw1","-list",0);
    atf_comp::UnsetEnv(algo_lib::dev_envvar_CREDD_PASSWORD);
    atf_comp::FProc &passwd = atf_comp::ProcStart("$bindir/credd -passwd");
    atf_comp::ProcWrite(passwd, "pw1");
    atf_comp::ProcWrite(passwd, "pw2");
    atf_comp::ProcWrite(passwd, "pw2");
    atf_comp::ProcWriteEof(passwd);
    atf_comp::ProcWait(passwd, 0);
    CreddRun("pw1","-add:late -kind:github -src:@$tempdir/tok",1);
    CreddRun("pw2","-add:late -kind:github -src:@$tempdir/tok",0);
    CreddRun("pw2","-dump",0);
    CreddSh("$bindir/credd -dump -format:json | grep -o '\"[a-z_]*\":' | sort | uniq -c | sed 's/^ *//'",0);
    CreddRun("pw2","-dump -format:text",1);
    CreddSh("LC_ALL=C ls $STORE/creddb && cat $STORE/creddb/credperm.ssim",0);
}

// -----------------------------------------------------------------------------
// Wait until the credd daemon of the scenario's home, $HOMEDIR, answers
// -status, which it does once its socket is bound; a daemon that never answers
// within 200 tries fails the test, and the failure carries what the last
// -status printed, since that is the client's own account of why.
void atf_comp::WaitCredd() {
    bool up = false;
    tempstr last;
    for (int i = 0; i < 200 && !up; i++) {
        last = SysEval(CreddVal("$bindir/credd -status 2>&1"),FailokQ(true));
        up = FindStr(last, "lib_cred.Status") != -1;
        if (!up) {
            usleep(20000);
        }
    }
    // a daemon that never answers is probed once by hand, which tells a socket
    // nothing listens on from a connection the daemon closes unanswered, and the
    // daemon's state and descriptors say whether it spins or waits, and on what
    tempstr probe;
    if (!up) {
        StringToFile("use IO::Socket::UNIX;\n"
                     "my $s = IO::Socket::UNIX->new(Peer => shift) or die \"connect: $!\\n\";\n"
                     "print $s \"lib_cred.StatusReq  client:probe\\n\";\n"
                     "my $r = <$s>;\n"
                     "print defined $r ? \"reply: $r\" : \"noreply: $!\\n\";\n", CreddVal("$tempdir/probe.pl"));
        probe = SysEval(CreddVal("perl $tempdir/probe.pl $STORE/credd.sock 2>&1; ls -la $STORE 2>&1;"
                                 " ps -axo pid,stat,time,command 2>&1 | grep '[c]redd -'; lsof -a -c credd 2>&1 | head -40"),FailokQ(true));
    }
    vrfy(up, tempstr() << "atf_comp: credd never answered on its socket" << Keyval("status", Trimmed(last))
         << Keyval("probe", Trimmed(probe)));
}

// -----------------------------------------------------------------------------
// Run the credd daemon, and check that it serves tokens, survives a bad peer and a
// bad reload, feeds its agent and follows links:
// 1. Add token gl, and start credd -daemon in the foreground under the harness with
//    the password in its environment.
// 2. Show gl, and check that the daemon opens it.  Show nope, and check that the
//    daemon refuses the unknown token.
// 3. Send a request from a peer that closes before it reads the reply, and check
//    that the daemon goes on answering.
// 4. Put a row of an unknown kind in the store, and ask the daemon to reload.
//    Check that the daemon refuses the reload and goes on serving gl.
// 5. Make key k1 with -keygen, and add key k2 from a file.  Check that both reach
//    the agent at the reload that the command line sends.  Remove k1, and check that
//    it leaves the agent.
// 6. Link gli-mr to gl.  Check that credd refuses a link to gli-mr and a link to an
//    absent target.  Check that gli-mr answers with gl's secret, and that credd
//    refuses to remove gl while a link points at it.
// 7. Check that credd refuses to renew key k2 from the token file.  Renew gli-mr
//    from stdin, and check that gl shows the renewed secret.
// 8. Remove gli-mr and gl, and run -stop.  Check that the daemon exits 0, and that
//    the agent socket and the daemon socket are gone.
// The harness starts the daemon itself, so the test tracks its pid and asserts its
// exit.  -stop ends the daemon and the agent with it.
void atf_comp::comptest_credd_Daemon() {
    CreddSetup();
    atf_comp::SetVar("AGENT",CreddVal("$HOMEDIR/.ssh/ssh_auth_sock"));
    CreddRun("pw","-add:gl -kind:gitlab -server:https://gitlab.example.com -account:me -perm:api -src:@$tempdir/tok",0);
    CreddQuiet("ssh-keygen -q -t ed25519 -N '' -C imported -f $tempdir/imp");
    atf_comp::SetEnv(algo_lib::dev_envvar_CREDD_PASSWORD,"pw");
    atf_comp::FProc &daemon = atf_comp::ProcStart("$bindir/credd -daemon");
    atf_comp::WaitCredd();
    CreddRun("","-status",0);
    CreddRun("","-show:gl",0);
    CreddRun("","-show:nope",1);
    // a peer that sends its request and closes before reading the reply: the
    // daemon's write fails, and the daemon goes on answering
    StringToFile("use IO::Socket::UNIX;\n"
                 "my $sock = IO::Socket::UNIX->new(Peer => shift) or die;\n"
                 "print $sock \"lib_cred.ShowReq  name:gl\\n\";\n"
                 "close $sock;\n"
                 "print \"peer left before the reply\\n\";\n", CreddVal("$tempdir/peer.pl"));
    CreddSh("perl $tempdir/peer.pl $STORE/credd.sock",0);
    CreddRun("","-status",0);
    // a row of a kind the daemon does not know reaches the store: the reload
    // fails, and the daemon goes on serving every credential it held
    CreddSh("cp $STORE/creddb/cred.ssim $tempdir/cred.ssim && perl -0 -i -pe 's/^/creddb.cred  cred:bad  kind:nosuchkind\\n/' $STORE/creddb/cred.ssim",0);
    // the command line reads the store itself, so the daemon is asked directly
    StringToFile("use IO::Socket::UNIX;\n"
                 "my $sock = IO::Socket::UNIX->new(Peer => shift) or die;\n"
                 "print $sock shift, \"\\n\";\n"
                 "my $reply = <$sock>;\n"
                 "print $reply =~ /^lib_cred.Error/ ? \"refused\\n\" : $reply;\n", CreddVal("$tempdir/ask.pl"));
    CreddSh("perl $tempdir/ask.pl $STORE/credd.sock 'lib_cred.ReloadReq  client:test'",0);
    CreddSh("perl $tempdir/ask.pl $STORE/credd.sock 'lib_cred.ShowReq  name:gl'",0);
    CreddSh("cp $tempdir/cred.ssim $STORE/creddb/cred.ssim",0);
    atf_comp::SetEnv(algo_lib::dev_envvar_CREDD_PASSWORD,"pw");
    CreddSh("$bindir/credd -keygen:k1 -comment:'made here' | cut -d' ' -f1,3-",0);
    CreddRun("pw","-add:k2 -kind:sshkey -src:@$tempdir/imp -comment:'brought in'",0);
    CreddRun("","-status",0);
    CreddSh("SSH_AUTH_SOCK=$AGENT ssh-add -L | cut -d' ' -f1,3- | sort",0);
    CreddRun("pw","-remove:k1",0);
    CreddSh("SSH_AUTH_SOCK=$AGENT ssh-add -L | cut -d' ' -f1,3-",0);
    CreddRun("pw","-add:gli-mr -linkto:gl -comment:'what the review bot asks for'",0);
    CreddRun("pw","-add:chain -linkto:gli-mr",1);
    CreddRun("pw","-add:nowhere -linkto:absent",1);
    CreddRun("","-show:gli-mr",0);
    CreddRun("pw","-remove:gl",1);
    CreddRun("pw","-list",0);
    CreddRun("pw","-renew:k2 -src:@$tempdir/tok",1);
    atf_comp::SetEnv(algo_lib::dev_envvar_CREDD_PASSWORD,"pw");
    atf_comp::FProc &renew = atf_comp::ProcStart("$bindir/credd -renew:gli-mr -expires:2027-01-31");
    atf_comp::ProcWrite(renew, "glpat-renewed");
    atf_comp::ProcWriteEof(renew);
    atf_comp::ProcWait(renew, 0);
    CreddRun("","-show:gl",0);
    CreddRun("pw","-remove:gli-mr",0);
    CreddRun("pw","-remove:gl",0);
    CreddRun("","-stop",0);
    atf_comp::ProcWait(daemon,0);
    CreddSh("test -e $AGENT || echo agent socket gone; test -e $STORE/credd.sock || echo daemon socket gone",0);
}

// Run the daemon that a service host runs, with -nostore, and check that it serves
// what -seed sends over its socket and writes no table that holds a secret:
// 1. Start credd -nostore -daemon with no password anywhere, and check its status.
// 2. Seed key ci, and check that it reaches the agent.
// 3. Seed ci again from another key, and check that the agent holds the new key
//    alone.
// 4. Seed a file that holds no key, and check that the seed exits 1 and that the
//    agent still holds what it held.
// 5. Seed ci under another kind, seed an empty file, and seed an unknown kind.
//    Check that each seed exits 1.
// 6. Run -add, and check that credd refuses it outright.
// 7. Seed an openai credential of two lines from a pipe with -src:-, and check that
//    -show answers with both lines.
// 8. Check that the store holds no cred.ssim with content (no secret on disk).
// 9. Stop the daemon, and check that it exits 0 and that both its sockets are gone.
//
// A -nostore daemon reads no store and writes none.  It comes up holding no
// credential, and every credential it serves arrives over its socket.  A refusal is
// a reply, so the exit code is what a caller reads.  Every refused seed exits 1.
// Otherwise a job would go on believing it had an identity.  Honoring -add would ask
// for a new password and write the store that -nostore exists to avoid.
void atf_comp::comptest_credd_Nostore() {
    CreddSetup();
    atf_comp::SetVar("AGENT",CreddVal("$HOMEDIR/.ssh/ssh_auth_sock"));
    CreddQuiet("ssh-keygen -q -t ed25519 -N '' -C seeded -f $tempdir/seed");
    CreddQuiet("ssh-keygen -q -t ed25519 -N '' -C second -f $tempdir/seed2");
    atf_comp::UnsetEnv(algo_lib::dev_envvar_CREDD_PASSWORD);
    atf_comp::FProc &daemon = atf_comp::ProcStart("$bindir/credd -nostore -daemon");
    atf_comp::WaitCredd();
    CreddRun("","-nostore -status",0);
    CreddRun("","-nostore -seed:ci -kind:sshkey -src:@$tempdir/seed",0);
    CreddSh("SSH_AUTH_SOCK=$AGENT ssh-add -L | cut -d' ' -f1,3- | sort",0);
    // a second seed of the same name replaces the secret, so a step that
    // retries costs nothing; the agent then holds the new key alone
    CreddRun("","-nostore -seed:ci -kind:sshkey -src:@$tempdir/seed2",0);
    CreddSh("SSH_AUTH_SOCK=$AGENT ssh-add -L | cut -d' ' -f1,3- | sort",0);
    // ssh-agent is what judges the key, and this daemon does not: a file
    // holding anything else seals and stores exactly as a key does.  So a seed
    // the agent refuses has to come back as a refusal, or a job would report an
    // identity the agent cannot sign with.  The agent still holds what it held.
    CreddQuiet("echo not a key at all > $tempdir/notakey");
    CreddRun("","-nostore -seed:bogus -kind:sshkey -src:@$tempdir/notakey",1);
    CreddSh("SSH_AUTH_SOCK=$AGENT ssh-add -L | cut -d' ' -f1,3- | sort",0);
    CreddRun("","-nostore -seed:ci -kind:gitlab -src:@$tempdir/tok",1);
    CreddRun("","-nostore -seed:empty -kind:sshkey -src:@/dev/null",1);
    // the refusal lists the kinds the tree defines, so only the refusal is kept
    CreddSh("$bindir/credd -nostore -seed:ci -kind:nosuch -src:@$tempdir/seed 2>&1 | grep -o \"credd.badkind  kind:[^ ]*\"; exit ${PIPESTATUS[0]}",1);
    CreddRun("pw","-nostore -add:gl -kind:gitlab -src:@$tempdir/tok",1);
    // an openai credential is a shell fragment of several lines, and -src:-
    // reads every line of it from a pipe, so -show answers with both
    CreddSh("printf 'OPENAI_ORG=org-atfcomp\\nOPENAI_TOKEN=sk-atfcomp\\n' | $bindir/credd -nostore -seed:frag -kind:openai -src:-",0);
    CreddRun("","-nostore -show:frag",0);
    CreddRun("","-nostore -status",0);
    // No table was ever written, so the seeded key existed in one process and
    // nowhere else.  The check is for the file rather than the directory: any
    // plain credd invocation creates an empty creddb directory, and the status
    // poll that waited for the daemon is one.
    CreddSh("test -s $STORE/creddb/cred.ssim && echo secret written to disk || echo no secret on disk",0);
    CreddRun("","-nostore -stop",0);
    atf_comp::ProcWait(daemon,0);
    CreddSh("test -e $AGENT || echo agent socket gone; test -e $STORE/credd.sock || echo daemon socket gone",0);
}

// Walk a home with the guided path -add:%, add single files with -add:<file>, and
// check what credd offers, takes and refuses:
// 1. Write a home that holds gli.ssim with two token records, a record naming no
//    server and an unparsed line.  Add an aws *.token file, a binary *.token file,
//    a plain key and a passphrase-protected key.
// 2. Add the plain key's public half with no password, and check that credd refuses
//    it and writes no store.
// 3. Walk the home.  Take one gli record, the aws token and the plain key, and
//    decline the other record.  Check that the locked key fails on a wrong
//    passphrase, and that the walk passes over the binary file.  Check that the walk
//    passes over the record naming no server, with a line.
// 4. Walk again, and check that the walk skips what the store holds and takes the
//    locked key with the right passphrase.
// 5. Write a token file, a key file and a token file without a suffix.  Add each
//    with -add:<file>, and check that credd offers it.
// 6. Check that credd refuses an empty file, an option meant for -add:<name>, and
//    a public key.
// 7. Walk a third time.  Check that the walk adds the declined record and drops its
//    token from gli.ssim, while the record bound in step 3 keeps its own.
// 8. Add a file whose content opens with the github prefix, and check that credd
//    offers it as a github token.
// 9. List the store, and print gli.ssim and the directory.  Check that gli.ssim
//    gains a binding line and keeps its token, in the shape an older gli reads.
void atf_comp::comptest_credd_Import() {
    CreddSetup();
    tempstr gli;
    gli << "lib_gli.Server  remoteurl:ssh://git@gitlab.example.com/x/y.git  serverurl:https://gitlab.example.com  username:me  token:glpat-one  project_path:x/y" << eol;
    gli << "lib_gli.Server  remoteurl:ssh://git@gitlab.example.com/x/y.git  serverurl:https://gitlab.example.com  username:bot  token:glpat-two  project_path:x/y  checkout:/home/me/bot" << eol;
    gli << "lib_gli.Server  remoteurl:ssh://git@nowhere.example.com/x/y.git  serverurl:''  username:me  token:glpat-three  project_path:x/y" << eol;
    gli << "some unparsed line" << eol;
    algo::StringToFile(gli, CreddVal("$HOMEDIR/.ssh/gli.ssim"));
    algo::StringToFile("export AWS_ACCESS_KEY_ID=AKIA\nexport AWS_SECRET_ACCESS_KEY=abc\n", CreddVal("$HOMEDIR/.ssh/me.token"));
    CreddQuiet("ssh-keygen -q -t ed25519 -N '' -C plainkey -f $HOMEDIR/.ssh/id_plain");
    CreddQuiet("ssh-keygen -q -t ed25519 -N kp -C lockedkey -f $HOMEDIR/.ssh/id_locked");
    CreddQuiet("echo known > $HOMEDIR/.ssh/known_hosts");
    algo::StringToFile(strptr("\0\1\2", 3), CreddVal("$HOMEDIR/.ssh/bin.token"));
    // a file that will be refused is refused before the master password is
    // asked: with no password in the environment and stdin closed, a prompt
    // would fail the run another way, and the store must not exist afterwards
    CreddRun("","-add:$HOMEDIR/.ssh/id_plain.pub",1);
    CreddSh("test -e $STORE/creddb/master.ssim || echo no store written",0);
    atf_comp::SetEnv(algo_lib::dev_envvar_CREDD_PASSWORD,"pw");
    atf_comp::FProc &walk = atf_comp::ProcStart("$bindir/credd -add:%");
    atf_comp::ProcWrite(walk, "y");
    atf_comp::ProcWrite(walk, "n");
    atf_comp::ProcWrite(walk, "y");
    atf_comp::ProcWrite(walk, "n");
    atf_comp::ProcWrite(walk, "y");
    atf_comp::ProcWrite(walk, "");
    atf_comp::ProcWrite(walk, "y");
    atf_comp::ProcWrite(walk, "wrong");
    atf_comp::ProcWrite(walk, "y");
    atf_comp::ProcWrite(walk, "");
    atf_comp::ProcWrite(walk, "2027-06-30");
    atf_comp::ProcWriteEof(walk);
    atf_comp::ProcWait(walk, 0);
    atf_comp::FProc &again = atf_comp::ProcStart("$bindir/credd -add:%");
    atf_comp::ProcWrite(again, "n");
    atf_comp::ProcWrite(again, "y");
    atf_comp::ProcWrite(again, "kp");
    atf_comp::ProcWrite(again, "from the locked file");
    atf_comp::ProcWrite(again, "");
    atf_comp::ProcWriteEof(again);
    atf_comp::ProcWait(again, 0);
    algo::StringToFile("glpat-work\n", CreddVal("$HOMEDIR/.ssh/work.token"));
    CreddQuiet("ssh-keygen -q -t ed25519 -N '' -C workkey -f $HOMEDIR/.ssh/id_work");
    algo::StringToFile("", CreddVal("$HOMEDIR/.ssh/empty.token"));
    algo::StringToFile("export AWS_PROFILE=prod\n", CreddVal("$HOMEDIR/.ssh/aws_prod"));
    atf_comp::FProc &token = atf_comp::ProcStart("$bindir/credd -add:$HOMEDIR/.ssh/work.token");
    atf_comp::ProcWrite(token, "y");
    atf_comp::ProcWrite(token, "");
    atf_comp::ProcWriteEof(token);
    atf_comp::ProcWait(token, 0);
    atf_comp::FProc &key = atf_comp::ProcStart("$bindir/credd -add:$HOMEDIR/.ssh/id_work");
    atf_comp::ProcWrite(key, "y");
    atf_comp::ProcWrite(key, "");
    atf_comp::ProcWrite(key, "");
    atf_comp::ProcWriteEof(key);
    atf_comp::ProcWait(key, 0);
    atf_comp::FProc &nodot = atf_comp::ProcStart("$bindir/credd -add:$HOMEDIR/.ssh/aws_prod");
    atf_comp::ProcWrite(nodot, "y");
    atf_comp::ProcWrite(nodot, "");
    atf_comp::ProcWriteEof(nodot);
    atf_comp::ProcWait(nodot, 0);
    CreddRun("pw","-add:$HOMEDIR/.ssh/empty.token",1);
    CreddRun("pw","-add:$HOMEDIR/.ssh/work.token -expires:2027-01-01",1);
    CreddRun("pw","-add:$HOMEDIR/.ssh/id_work.pub",1);
    // the bot record is added on a third walk and its token dropped from the
    // file; the me record, bound on the first walk, keeps its token
    atf_comp::FProc &drop = atf_comp::ProcStart("$bindir/credd -add:%");
    atf_comp::ProcWrite(drop, "y");
    atf_comp::ProcWrite(drop, "y");
    atf_comp::ProcWrite(drop, "y");
    atf_comp::ProcWriteEof(drop);
    atf_comp::ProcWait(drop, 0);
    // a file opening with the github prefix is offered as a github token
    algo::StringToFile("ghp_fake_test_token\n", CreddVal("$HOMEDIR/.ssh/ghdev.token"));
    atf_comp::FProc &ghtok = atf_comp::ProcStart("$bindir/credd -add:$HOMEDIR/.ssh/ghdev.token");
    atf_comp::ProcWrite(ghtok, "y");
    atf_comp::ProcWrite(ghtok, "");
    atf_comp::ProcWriteEof(ghtok);
    atf_comp::ProcWait(ghtok, 0);
    CreddRun("pw","-list",0);
    CreddSh("cat $HOMEDIR/.ssh/gli.ssim",0);
    CreddSh("ls $HOMEDIR/.ssh",0);
}

// -----------------------------------------------------------------------------
// Mint credentials that the tree declares, with no server to mint them at, and
// check what -mint prints, takes and refuses:
// 1. Write a tree under $tempdir/data that declares four credentials: bot, a group
//    token acting as developer with two scopes; pat, a personal token; norole, a
//    group token with no role; and acct, a group token that the test gives no
//    group.
// 2. Mint bot and pat.  Check that -mint prints the steps a person follows, with
//    the name, role and scopes filled in, and takes the token per -src.
// 3. Mint bot again, then ghost, norole and acct.  Check that credd refuses a name
//    already taken, a name the tree does not declare, a group token with no role,
//    and one with no group.
// 4. List the store, and renew bot.  Check that -renew names the role the tree
//    gives bot.
// The store holds no gitlab token to ask with, so -mint prints the steps.
void atf_comp::comptest_credd_Mint() {
    CreddSetup();
    CreddQuiet("mkdir -p $tempdir/data/dev");
    tempstr cred;
    cred << "dev.cred  cred:acct  kind:gitlab_group  envvar:GITLAB_API_TOKEN  comment:''" << eol;
    cred << "dev.cred  cred:bot  kind:gitlab_group  envvar:GITLAB_API_TOKEN  comment:'the review bot'" << eol;
    cred << "dev.cred  cred:norole  kind:gitlab_group  envvar:GITLAB_API_TOKEN  comment:''" << eol;
    cred << "dev.cred  cred:pat  kind:gitlab  envvar:GITLAB_API_TOKEN  comment:'a person reading the api'" << eol;
    algo::StringToFile(cred, CreddVal("$tempdir/data/dev/cred.ssim"));
    tempstr credrole;
    credrole << "dev.credrole  cred:acct  role:gitlab_group/reporter  comment:''" << eol;
    credrole << "dev.credrole  cred:bot  role:gitlab_group/developer  comment:''" << eol;
    algo::StringToFile(credrole, CreddVal("$tempdir/data/dev/credrole.ssim"));
    tempstr credperm;
    credperm << "dev.credperm  credperm:acct/read_api  comment:''" << eol;
    credperm << "dev.credperm  credperm:bot/api  comment:''" << eol;
    credperm << "dev.credperm  credperm:bot/read_repository  comment:''" << eol;
    credperm << "dev.credperm  credperm:pat/read_api  comment:''" << eol;
    algo::StringToFile(credperm, CreddVal("$tempdir/data/dev/credperm.ssim"));
    CreddRun("pw1","-mint:bot -in:$tempdir/data -server:https://gitlab.example.com -account:algornd -src:@$tempdir/tok",0);
    CreddRun("pw1","-mint:pat -in:$tempdir/data -server:https://gitlab.example.com -src:@$tempdir/tok",0);
    CreddRun("pw1","-mint:bot -in:$tempdir/data -server:https://gitlab.example.com -account:algornd -src:@$tempdir/tok",1);
    CreddRun("pw1","-mint:ghost -in:$tempdir/data -src:@$tempdir/tok",1);
    CreddRun("pw1","-mint:norole -in:$tempdir/data -server:https://gitlab.example.com -account:algornd -src:@$tempdir/tok",1);
    CreddRun("pw1","-mint:acct -in:$tempdir/data -server:https://gitlab.example.com -src:@$tempdir/tok",1);
    CreddRun("pw1","-list -format:ssim",0);
    CreddRun("pw1","-renew:bot -in:$tempdir/data -src:@$tempdir/tok",0);
}
