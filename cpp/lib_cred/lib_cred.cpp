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
// Target: lib_cred (lib) -- Client library: fetch a credential from credd by name
// Exceptions: yes
// Source: cpp/lib_cred/lib_cred.cpp
//
// The client side of credd's socket.  A request is one ssim tuple on one
// line, the reply is one tuple on one line, and the connection closes after
// it.  Fetch is what a consumer calls: it asks for a credential by its symbolic
// name and gets the secret back, or a reason it did not.  Request is the
// layer under it, which credd's own command line uses for the other verbs.
// FetchFile is the call of a tool that also runs where no daemon is: it takes
// the credential from credd, or else from the token file the credential
// stands for.  EvalVar reads one variable out of such a text, and
// SysEvalStdin hands a secret to a child without putting it on a command line.

#include "include/algo.h"
#include "include/lib_cred.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"
#include <sys/socket.h>
#include <sys/un.h>

// Path of the socket the daemon of the default store listens on:
// $HOME/.ssh/credd/credd.sock.
algo::tempstr lib_cred::SockPath() {
    algo::tempstr ret;
    const char *home = getenv(algo_lib::dev_envvar_HOME);
    ret << (home ? home : "") << "/.ssh/credd/credd.sock";
    return ret;
}

// Connect to the unix socket SOCK, send LINE followed by a newline, and read
// one reply line into OUT_REPLY.  Returns false, with OUT_ERR saying why, when
// nothing listens on SOCK, when the path is too long for a socket address, or
// when the daemon sends no newline within two seconds.  The socket is closed
// before returning either way.
bool lib_cred::Request(algo::strptr sock, algo::strptr line, algo::cstring &out_reply, algo::cstring &out_err) {
    out_reply = "";
    out_err = "";
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    bool ok = elems_N(sock) < int(sizeof(addr.sun_path));
    if (!ok) {
        out_err << "lib_cred.longpath" << Keyval("sock", sock);
    } else {
        memcpy(addr.sun_path, sock.elems, elems_N(sock));
    }
    int fd = ok ? socket(AF_UNIX, SOCK_STREAM, 0) : -1;
    ok = ok && fd >= 0;
    if (ok && connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        ok = false;
        out_err << "lib_cred.noanswer" << Keyval("sock", sock) << Keyval("errno", strerror(errno))
                << Keyval("comment", "no credd answers; start one with credd -start");
    }
    if (ok) {
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        (void)setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
        algo::tempstr out;
        out << line << eol;
        ssize_t nwrite = write(fd, out.ch_elems, out.ch_n);
        ok = nwrite == ssize_t(out.ch_n);
        if (!ok) {
            out_err << "lib_cred.writefail" << Keyval("sock", sock) << Keyval("errno", strerror(errno));
        }
    }
    bool more = ok;
    while (more) {
        char buf[4096];
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n <= 0) {
            more = false;
            ok = ok && ch_N(out_reply) > 0;
            if (!ok) {
                out_err << "lib_cred.noreply" << Keyval("sock", sock);
            }
        } else {
            out_reply << algo::strptr(buf, n);
            more = memchr(buf, '\n', n) == NULL;
        }
    }
    if (fd >= 0) {
        close(fd);
    }
    if (ok) {
        algo::tempstr trimmed;
        trimmed << algo::Trimmed(out_reply);
        out_reply = trimmed;
    }
    return ok;
}

// Ask the daemon on SOCK for its status and put the reply in OUT.  CLIENT names
// the program asking, for the daemon's log.  True when a daemon answers,
// whatever it said.
//
// A daemon outlives every rebuild of the tools that talk to it, and one built
// before the status tuple had its present shape answers with a tuple this binary
// cannot read.  That still counts as answering, so OUT.build reads `unreadable`
// and a caller reports an old daemon rather than a missing one.
bool lib_cred::ProbeFrom(algo::strptr sock, algo::strptr client, lib_cred::Status &out) {
    lib_cred::StatusReq req;
    req.client = client;
    algo::cstring reply;
    algo::cstring err;
    bool ok = lib_cred::Request(sock, tempstr() << req, reply, err);
    if (ok && !lib_cred::Status_ReadStrptrMaybe(out, reply)) {
        out.build = "unreadable";
    }
    return ok;
}

// Ask the daemon of this login's own store for its status; see ProbeFrom for
// what OUT holds and what CLIENT is for.
bool lib_cred::Probe(algo::strptr client, lib_cred::Status &out) {
    return lib_cred::ProbeFrom(lib_cred::SockPath(), client, out);
}

// Fetch the secret of credential NAME from the daemon listening on SOCK, naming the
// caller CLIENT in the daemon's log.  The value lands in OUT_VALUE.  Returns
// false, with OUT_ERR saying why, when no daemon answers or the daemon refuses:
// an unknown name, or a secret it cannot open.
bool lib_cred::FetchFrom(algo::strptr sock, algo::strptr client, algo::strptr name, algo::cstring &out_value, algo::cstring &out_err) {
    out_value = "";
    lib_cred::ShowReq req;
    req.client = client;
    req.name = name;
    algo::cstring reply;
    bool ok = lib_cred::Request(sock, tempstr() << req, reply, out_err);
    lib_cred::Value value;
    lib_cred::Error error;
    if (ok && lib_cred::Value_ReadStrptrMaybe(value, reply)) {
        out_value = value.value;
    } else if (ok && lib_cred::Error_ReadStrptrMaybe(error, reply)) {
        ok = false;
        out_err << "lib_cred.refused" << Keyval("name", name) << Keyval("msg", error.msg);
    } else if (ok) {
        ok = false;
        out_err << "lib_cred.badreply" << Keyval("name", name) << Keyval("reply", reply);
    }
    return ok;
}

// Fetch the secret of credential NAME from the daemon of the default store; see
// FetchFrom.  This is the call a consumer makes.
bool lib_cred::Fetch(algo::strptr client, algo::strptr name, algo::cstring &out_value, algo::cstring &out_err) {
    return lib_cred::FetchFrom(lib_cred::SockPath(), client, name, out_value, out_err);
}

// Run CMD in bash with INPUT on its standard input, and return what it prints
// on standard output.  OUT_STATUS receives the exit code, or -1 when INPUT did
// not reach the process whole.
//
// A secret handed to a child on its command line is in the process table for
// as long as the child runs, where any user of the machine can read it, and it
// is in every log line that prints the command.  The child's standard input is
// seen by the child alone.  So a caller puts the secret in INPUT, and CMD reads
// it from there: curl with -K - takes a header from a config on stdin, and a
// shell takes a fragment with eval "$(cat)".  INPUT is written whole before the
// output is read, so it has to fit in a pipe, which a credential does.
algo::tempstr lib_cred::SysEvalStdin(algo::strptr cmd, algo::strptr input, int &out_status) {
    command::bash_proc bash;
    bash.cmd.c = cmd;
    bash.fstdin = "|";
    bash.fstdout = "|";
    int rc = command::bash_Start(bash);
    ssize_t nwrite = rc == 0 ? write(bash.to_stdin.value, input.elems, input.n_elems) : -1;
    algo_lib::Close(bash.to_stdin);
    algo::tempstr ret;
    if (rc == 0) {
        // FdToString ends what it read with a newline of its own
        algo::tempstr out(algo::FdToString(bash.from_stdout));
        ret << algo::ch_FirstN(out, ch_N(out) - 1);
    }
    command::bash_Wait(bash);
    out_status = nwrite == ssize_t(input.n_elems) ? algo::WaitStatusToExitCode(bash.status) : -1;
    return ret;
}

// Return VALUE as a quoted string of a curl config, with each quote and
// backslash in it escaped, so a secret in it reaches curl as written.  A caller
// writes the config line around it and hands the config to SysEvalStdin.
algo::tempstr lib_cred::GetCurlConfigStr(algo::strptr value) {
    algo::tempstr ret;
    ret << '"';
    for (int i = 0; i < value.n_elems; i++) {
        if (value[i] == '"' || value[i] == '\\') {
            ret << '\\';
        }
        ret << value[i];
    }
    ret << '"';
    return ret;
}

// Return the value the shell fragment TEXT assigns to the variable VAR, or an
// empty string when TEXT assigns it nothing.  TEXT is a credential's text, the
// lines a token file holds for a shell to source, such as
// `export AWS_ACCESS_KEY_ID=...`.
//
// bash reads the fragment, so every quoting a writer chose reads back the way
// the writer meant it, and the fragment travels on bash's standard input.
algo::tempstr lib_cred::EvalVar(algo::strptr text, algo::strptr var) {
    algo::tempstr cmd;
    cmd << "eval \"$(cat)\" >/dev/null 2>&1; printf %s \"${" << var << ":-}\"";
    int status = 0;
    return lib_cred::SysEvalStdin(cmd, text, status);
}

// Read the text of credential NAME for the program CLIENT into OUT_TEXT: credd's
// secret of that name when a daemon holds one, and otherwise the content of the
// file FNAME.  Return false, with OUT_ERR naming both places, when neither holds
// it.
//
// A credential that a tool reads from a file under ~/.ssh holds that file's
// content as its secret, which is what credd -add:% makes of a token file.  So a
// tool reads one text from either place and parses it one way, and a person
// moves a token into credd with credd -add:<name> -src:@<file> and removes the
// file.  A file stays the answer on a host with no person on it, where nobody
// starts a daemon.
bool lib_cred::FetchFile(algo::strptr client, algo::strptr name, algo::strptr fname, algo::cstring &out_text, algo::cstring &out_err) {
    algo::cstring err;
    bool ret = lib_cred::Fetch(client, name, out_text, err);
    if (!ret && algo::FileQ(fname)) {
        out_text = algo::FileToString(fname);
        ret = true;
    }
    out_err = "";
    if (!ret) {
        out_err << "lib_cred.nocred" << Keyval("name", name) << Keyval("file", fname) << Keyval("credd", err)
                << Keyval("comment", "neither credd nor the file holds this credential");
    }
    return ret;
}
