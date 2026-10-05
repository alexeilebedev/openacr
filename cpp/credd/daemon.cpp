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
// Source: cpp/credd/daemon.cpp
//
// The daemon: the process that holds the master key and answers the socket.
// It listens on credd.sock inside the store, and it owns an ssh agent whose
// socket sits beside the store, so every private key of the store is loaded
// once and every ssh from this box uses it.  A request is one tuple on one
// line and gets one tuple back; the request and reply types are lib_cred's.
// The daemon never writes the store: the command line does that and then
// sends a reload.

#include "include/algo.h"
#include "include/credd.h"
#include <signal.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#if defined(__linux__)
#include <sys/prctl.h>
#endif

// Path of the daemon's socket, inside the store.
algo::tempstr credd::DaemonSock() {
    return algo::DirFileJoin(credd::_db.dir, "credd.sock");
}

// Path of the ssh agent's socket: ssh_auth_sock beside the store, which is
// ~/.ssh/ssh_auth_sock for the default store.  A shell exports SSH_AUTH_SOCK
// naming it, once, and every login finds the same agent.
algo::tempstr credd::AgentSock() {
    return algo::DirFileJoin(algo::GetDirName(credd::_db.dir), "ssh_auth_sock");
}

// Send LINE to this store's daemon and return its reply, as lib_cred::Request
// reports it.
bool credd::Ask(algo::strptr line, algo::cstring &out_reply, algo::cstring &out_err) {
    return lib_cred::Request(credd::DaemonSock(), line, out_reply, out_err);
}

// Ask this store's daemon for its status.  Returns true when one answers, with
// its status in OUT.  A daemon built before the status tuple had its present
// shape answers with a tuple this binary cannot read; that still counts as
// answering, and OUT.build then says unreadable, so the caller reports an old
// daemon rather than a missing one.
bool credd::ProbeDaemon(lib_cred::Status &out) {
    bool ok = lib_cred::ProbeFrom(credd::DaemonSock(), "credd", out);
    if (ok && out.build == "unreadable") {
        out.dir = credd::_db.dir;
    }
    return ok;
}

// True when STATUS came from a daemon built from the same source as this
// binary.  A daemon outlives every rebuild, and one left over from an older
// build reads the store under old names or answers in old shapes, so a
// credential the command line just added is invisible to it.
bool credd::SameBuildQ(lib_cred::Status &status) {
    return status.build == algo::gitinfo_Get();
}

// Say that the running daemon is from another build than this binary, and
// what to do about it.  Nothing is said when the builds agree.
void credd::WarnOldBuild(lib_cred::Status &status) {
    if (!credd::SameBuildQ(status)) {
        prlog("credd.oldbuild" << Keyval("dir", status.dir) << Keyval("daemon", status.build)
              << Keyval("comment", "the daemon runs another build of credd than this command; credd -stop, then credd -start"));
    }
}

// Start an ssh agent on the agent socket, replacing whatever socket file was
// there, and remember its pid.  Dies when ssh-agent does not start.
static void StartAgent() {
    algo::tempstr sock = credd::AgentSock();
    (void)unlink(Zeroterm(sock));
    algo::tempstr cmd;
    cmd << "ssh-agent -a " << algo::strptr_ToBash(sock);
    int status = 0;
    algo::tempstr out = algo::SysEval(cmd, FailokQ(true), 4096, false, &status);
    // ssh-agent prints shell assignments; the pid is the one that matters
    ind_beg(algo::Line_curs, line, out) {
        if (algo::StartsWithQ(line, "SSH_AGENT_PID=")) {
            algo::strptr num = algo::Pathcomp(line, "=LR;LL");
            i32 pid = 0;
            (void)i32_ReadStrptrMaybe(pid, num);
            credd::_db.agent_pid = pid;
        }
    }ind_end;
    vrfy(status == 0 && credd::_db.agent_pid > 0, tempstr()
         << "credd.agentfail" << Keyval("cmd", cmd) << Keyval("status", status) << Keyval("out", out));
    setenv(algo_lib::dev_envvar_SSH_AUTH_SOCK, Zeroterm(sock), 1);
}

// Make the agent hold exactly the keys of the store: empty it, then feed it
// every sshkey credential that opens under the master key.  A key that does
// not open is reported and skipped, since the daemon is running with the
// password the store's verifier accepted, so such a key is a damaged record.
//
// Return whether the credential named WANT reached the agent, and put the
// reason it did not into WHY.  A caller that has just taken a secret in needs
// that answer, because ssh-add judges the key and this daemon does not: a file
// holding anything other than a private key seals and stores exactly as a real
// one does, and the first thing that looks at its contents is the agent.  So a
// caller told only that the secret was held would report an identity the agent
// cannot sign with.  An empty WANT asks about no credential and the answer is
// true; so is a WANT naming one that is not an sshkey, since no agent is
// involved in holding it.
//
// WHY travels back to the client and into the job log, so it carries the fact
// rather than a pointer to this daemon's log: the log lives under the store
// directory, and the CI caller that most needs the reason removes that
// directory in the after_script that follows its failure.
static bool LoadAgent(algo::strptr want, algo::cstring &why) {
    bool ret = true;
    (void)algo::SysCmd("ssh-add -q -D 2>/dev/null", FailokQ(true));
    ind_beg(credd::_db_cred_curs, cred, credd::_db) if (cred.kind == creddb_Kind_kind_sshkey) {
        algo::cstring key;
        algo::cstring reason;
        bool ok = credd::Unseal(credd::_db.key, cred.secret, key);
        if (!ok) {
            reason << "the private key does not open under the master key; record damaged";
            prerr("credd.badkey" << Keyval("cred", cred.cred) << Keyval("comment", reason));
        } else {
            command::bash_proc bash;
            bash.cmd.c = "ssh-add -q -";
            bash.fstdin = "|";
            int rc = command::bash_Start(bash);
            ssize_t nwrite = rc == 0 ? write(bash.to_stdin.value, key.ch_elems, key.ch_n) : -1;
            command::bash_Wait(bash);
            ok = rc == 0 && nwrite == ssize_t(key.ch_n) && algo::WaitStatusToExitCode(bash.status) == 0;
            if (!ok) {
                reason << "ssh-add refused the key, exiting " << algo::WaitStatusToExitCode(bash.status);
                prerr("credd.addfail" << Keyval("cred", cred.cred) << Keyval("status", bash.status)
                      << Keyval("comment", reason));
            } else {
                verblog("credd.loaded" << Keyval("cred", cred.cred));
            }
        }
        if (cred.cred == want) {
            ret = ok;
            why = reason;
        }
    }ind_end;
    return ret;
}

// Hold the credential SEED brings for as long as this daemon runs, and write
// the reply into OUT.
//
// A workstation's daemon gets its credentials from a store a person unlocked.
// A service host has neither: no files, nobody at a terminal.  Its credentials
// arrive here instead, over a socket whose peer this daemon has already checked
// runs as its own user.
//
// The secret is sealed under the daemon's own key, which is random, made at
// startup and never written anywhere.  That is what keeps this path from being
// a special case: the row it produces is indistinguishable from one read out of
// a store, so the agent reload below feeds it to ssh-agent through the code a
// person's keys go through.
//
// Seeding a name twice replaces its secret rather than refusing it, so a job
// that retries a step need not know whether its first attempt got through; the
// kind is fixed by the first seed, and a second one naming a different kind is
// refused rather than quietly re-typing the credential.  No public half is
// derived, because deriving one runs ssh-keygen over a file holding the private
// key, and a daemon that holds no files is the whole point.
//
// REQ is the request's type tag, which comes back in the reply so a caller can
// see which request was answered.
static void SeedCred(lib_cred::SeedReq &seed, algo::strptr req, algo::cstring &out) {
    lib_cred::Error error;
    credd::FKind *kind = credd::ind_kind_Find(seed.kind);
    credd::FCred *held = credd::ind_cred_Find(seed.name);
    if (!credd::_db.cmdline.nostore) {
        error.msg << "this daemon holds a store; add a credential with credd -add, which seals it on disk";
        out << error;
    } else if (!kind) {
        error.msg << "no such kind";
        out << error;
    } else if (algo::Trimmed(seed.secret) == "") {
        error.msg << "no secret was given";
        out << error;
    } else if (held && held->kind != kind->kind) {
        error.msg << "that name is already held, under kind " << held->kind;
        out << error;
    } else {
        credd::FCred *cred = held;
        bool ok = true;
        if (!cred) {
            cred = &credd::cred_Alloc();
            cred->cred = seed.name;
            cred->kind = kind->kind;
            ok = credd::cred_XrefMaybe(*cred);
        }
        ok = ok && credd::Seal(credd::_db.key, seed.secret, cred->secret);
        bool loaded = false;
        algo::cstring why;
        if (ok) {
            cred->created = credd::NowSec();
            loaded = LoadAgent(seed.name, why);
        }
        if (!ok) {
            error.msg << "the credential could not be held; " << algo_lib::_db.errtext;
            out << error;
        } else if (!loaded) {
            error.msg << "the agent did not take this credential: " << why;
            out << error;
        } else {
            lib_cred::Done done;
            done.req = req;
            out << done;
            prlog("credd.seed" << Keyval("client", seed.client) << Keyval("name", seed.name)
                  << Keyval("kind", cred->kind) << Keyval("n_cred", credd::cred_N()));
        }
    }
}

// Empty the agent, stop it, and remove its socket.
static void StopAgent() {
    if (credd::_db.agent_pid > 0) {
        (void)algo::SysCmd("ssh-add -q -D 2>/dev/null", FailokQ(true));
        (void)kill(credd::_db.agent_pid, SIGTERM);
        credd::_db.agent_pid = 0;
    }
    (void)unlink(Zeroterm(credd::AgentSock()));
}

// Answer request LINE with one reply tuple in OUT.  A show request opens the
// named credential, or the one a link of that name stands for, under the master key; a status request reports the counts; a
// reload request reads the store again and reloads the agent, so a change the
// command line just wrote is served at once; a seed request hands this daemon a
// credential to hold; a stop request is answered, and the daemon then closes
// its socket and exits.  Anything else is an error.
static void Answer(algo::strptr line, algo::cstring &out) {
    lib_cred::ShowReq show;
    lib_cred::StatusReq status;
    lib_cred::ReloadReq reload;
    lib_cred::SeedReq seed;
    lib_cred::StopReq stop;
    lib_cred::Error error;
    if (lib_cred::ShowReq_ReadStrptrMaybe(show, line)) {
        credd::FCred *cred = credd::Target(show.name);
        lib_cred::Value value;
        bool ok = cred && credd::Unseal(credd::_db.key, cred->secret, value.value);
        verblog("credd.show" << Keyval("client", show.client) << Keyval("name", show.name) << Keyval("ok", ok));
        if (ok) {
            value.name = show.name;
            out << value;
        } else {
            error.msg << (cred ? "the secret does not open under the master key" : "no such credential");
            out << error;
        }
    } else if (lib_cred::StatusReq_ReadStrptrMaybe(status, line)) {
        lib_cred::Status reply;
        reply.dir = credd::_db.dir;
        reply.agent = credd::AgentSock();
        reply.n_cred = credd::cred_N();
        reply.build = algo::gitinfo_Get();
        out << reply;
    } else if (lib_cred::ReloadReq_ReadStrptrMaybe(reload, line)) {
        // A storeless daemon has no files to read again, and its credentials
        // live nowhere else, so reading would empty the agent and lose them.
        if (credd::_db.cmdline.nostore) {
            error.msg << "this daemon holds no store; there is nothing to reload";
            out << error;
        } else {
            try {
                credd::LoadStore();
                algo::cstring why;
                (void)LoadAgent("", why);
                lib_cred::Done done;
                done.req = algo::GetTypeTag(line);
                out << done;
            } catch (algo_lib::ErrorX &x) {
                error.msg << x.str;
                out << error;
            }
        }
    } else if (lib_cred::SeedReq_ReadStrptrMaybe(seed, line)) {
        SeedCred(seed, algo::GetTypeTag(line), out);
    } else if (lib_cred::StopReq_ReadStrptrMaybe(stop, line)) {
        lib_cred::Done done;
        done.req = algo::GetTypeTag(line);
        out << done;
        credd::_db.stop = true;
    } else {
        error.msg << "unknown request";
        out << error;
    }
}

// True when the peer of FD runs as this user.  Another uid gets no answer,
// and root is another uid: the socket's mode already keeps others out, and
// this is the check for a socket whose directory mode was loosened by hand.
static bool PeerIsSelfQ(int fd) {
    bool ok = false;
#if defined(SO_PEERCRED)
    struct ucred cred;
    socklen_t len = sizeof(cred);
    ok = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) == 0 && cred.uid == getuid();
#else
    uid_t uid = 0;
    gid_t gid = 0;
    ok = getpeereid(fd, &uid, &gid) == 0 && uid == getuid();
#endif
    return ok;
}

// Accept every connection waiting on the listening socket and answer each
// with one line.  The listening socket is edge-triggered, so this drains it.
// A connection from another user is closed unanswered.  A client that sends
// no newline within two seconds, or more than 64K, is closed too, so one stuck
// client cannot hold the daemon.  After a stop request has been answered the
// listening socket is removed from the loop, which is what ends the daemon.
static void Accept() {
    bool more = true;
    while (more) {
        int fd = accept(credd::_db.listen.fildes.value, NULL, NULL);
        if (fd < 0) {
            more = false;
        } else {
            (void)fcntl(fd, F_SETFD, FD_CLOEXEC);
            // the listening socket is non-blocking, and on BSD and macOS a
            // connection inherits that from it, so its first read would find
            // nothing yet and close it unanswered.  The read below waits under
            // the receive timeout, so the connection is made blocking.
            algo::SetBlockingMode(algo::Fildes(fd), true);
            struct timeval tv;
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
            (void)setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
            bool ok = PeerIsSelfQ(fd);
            algo::tempstr line;
            bool reading = ok;
            while (reading) {
                char buf[4096];
                ssize_t n = read(fd, buf, sizeof(buf));
                if (n <= 0) {
                    reading = false;
                    ok = false;
                } else {
                    line << algo::strptr(buf, n);
                    reading = memchr(buf, '\n', n) == NULL && ch_N(line) < 65536;
                    ok = memchr(buf, '\n', n) != NULL;
                }
            }
            if (ok) {
                algo::tempstr reply;
                Answer(algo::Trimmed(algo::Pathcomp(line, "\nLL")), reply);
                reply << eol;
                ssize_t nwrite = write(fd, reply.ch_elems, reply.ch_n);
                if (nwrite != ssize_t(reply.ch_n)) {
                    int err = errno;
                    prerr("credd.peergone" << Keyval("errno", strerror(err))
                          << Keyval("comment", "the peer closed before reading its reply; the daemon goes on"));
                }
            }
            close(fd);
            if (credd::_db.stop) {
                more = false;
                algo_lib::IohookRemove(credd::_db.listen);
                close(credd::_db.listen.fildes.value);
                credd::_db.listen.fildes = algo::Fildes();
            }
        }
    }
}

// Treat SIGTERM and SIGINT as a stop request that arrived from outside.
static void OnSignal(int) {
    algo_lib::ReqExitMainLoop();
}

// Serve the store on its socket, with the master key in _db.key, until a stop
// request or a signal.  The process refuses to be dumped and locks its memory,
// so the key does not reach a core file or swap.  The ssh agent is started
// and loaded first, then the socket is bound, mode 0600, and the loop runs.
// On the way out the agent is emptied and stopped, the socket removed, and
// the key overwritten.
void credd::RunDaemon() {
#if defined(PR_SET_DUMPABLE)
    (void)prctl(PR_SET_DUMPABLE, 0);
#endif
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        verblog("credd.nomlock" << Keyval("errno", strerror(errno)));
    }
    // A peer can be gone by the time its reply is written: a client whose
    // timeout expired while the agent was loading, or an ssh-add that exited
    // before reading the key on its stdin.  Under the default disposition that
    // write raises SIGPIPE and the process holding every key dies without a
    // word.  Ignored, the same write answers EPIPE, an error each site reports.
    signal(SIGPIPE, SIG_IGN);
    algo::tempstr sock = credd::DaemonSock();
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    vrfy(ch_N(sock) < int(sizeof(addr.sun_path)), tempstr() << "credd.longpath" << Keyval("sock", sock));
    memcpy(addr.sun_path, sock.ch_elems, sock.ch_n);
    (void)unlink(Zeroterm(sock));
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    errno_vrfy_(fd >= 0);
    (void)fcntl(fd, F_SETFD, FD_CLOEXEC);
    mode_t oldmask = umask(077);
    int rc = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    umask(oldmask);
    errno_vrfy_(rc == 0);
    errno_vrfy_(listen(fd, 16) == 0);
    algo::SetBlockingMode(algo::Fildes(fd), false);
    StartAgent();
    algo::cstring why;
    (void)LoadAgent("", why);
    credd::_db.listen.fildes = algo::Fildes(fd);
    callback_Set0(credd::_db.listen, Accept);
    algo::IOEvtFlags flags;
    read_Set(flags, true);
    algo_lib::IohookAdd(credd::_db.listen, flags);
    signal(SIGTERM, OnSignal);
    signal(SIGINT, OnSignal);
    prlog("credd.listen" << Keyval("sock", sock) << Keyval("agent", credd::AgentSock())
          << Keyval("n_cred", credd::cred_N()));
    credd::MainLoop();
    if (ValidQ(credd::_db.listen.fildes)) {
        algo_lib::IohookRemove(credd::_db.listen);
        close(credd::_db.listen.fildes.value);
    }
    StopAgent();
    (void)unlink(Zeroterm(sock));
    memset(credd::_db.key.ch_elems, 0, credd::_db.key.ch_n);
    credd::_db.key = "";
    prlog("credd.exit" << Keyval("sock", sock));
}
