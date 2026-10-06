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
// The store without a daemon.  The first change to an empty store sets the
// master password; a token's permissions are checked against its kind; a name
// is taken once; a secret cannot be shown with no daemon; -check finds what
// expires; a removed row is gone from the files; the password changes with the
// old one, and the old one then no longer opens the store; -dump prints what
// the files hold, and the json form has one array per table.
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
        last = SysEval(CreddVal("$bindir/credd -status 2>&1"),FailokQ(true),4096);
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
                                 " ps -axo pid,stat,time,command 2>&1 | grep '[c]redd -'; lsof -a -c credd 2>&1 | head -40"),FailokQ(true),16384);
    }
    vrfy(up, tempstr() << "atf_comp: credd never answered on its socket" << Keyval("status", Trimmed(last))
         << Keyval("probe", Trimmed(probe)));
}

// -----------------------------------------------------------------------------
// The daemon.  It is started in the foreground under the harness with the
// password in its environment, so its pid is tracked and its exit asserted.
// It opens a token on request and refuses an unknown one; a key made by
// -keygen and one added from a file both reach its agent at the reload the
// command line sends, and a removed one leaves it.  A link answers with its
// target's secret, a link to a link is refused, a target with links stays,
// and renewing through a link renews the target.  -stop ends the daemon and
// the agent with it.
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

// The daemon a service host runs: -nostore, so no store is read and none is
// written, and the credentials it serves arrive over its socket from -seed.
//
// It comes up holding nothing and with no password anywhere.  A seeded key
// reaches its agent, seeding the same name again replaces the secret, and
// seeding it under another kind is refused.  A refusal is a reply, so the exit
// code is what a caller reads: every refused seed exits 1, or a job would go on
// believing it had an identity.  -add is refused outright, since honouring it
// would ask for a new password and write the store -nostore exists to avoid.
// When it stops, what is left in the directory is neither of its sockets, and no
// table holding a secret was written at any point.
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

// The guided path: -add:% over a home holding gli.ssim with two token
// records, an aws *.token file, a binary *.token file the walk passes over, a
// plain key and a passphrase-protected key.
// The first pass takes one gli record, the token and the plain key, declines
// the other record, and fails the locked key on a wrong passphrase; the second
// pass skips what the store holds and takes the locked key with the right
// passphrase.  Then -add:<file> offers one token file, one key file and one
// token file without a suffix that arrived after the walk, and refuses an
// empty file, a public key and an option meant for -add:<name>; a gli record
// naming no server is passed over with a line.  A third walk adds the declined
// record and drops its token from the file, the bound record keeping its own.
// A file whose content opens with a kind's prefix is offered as that kind's token.
// gli.ssim gains a binding
// line and keeps its token, in the shape an older gli reads.
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
