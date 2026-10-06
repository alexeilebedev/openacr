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
// Header: include/lib_cred.h
//

#include "include/gen/lib_cred_gen.h"
#include "include/gen/lib_cred_gen.inl.h"

namespace lib_cred { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_cred/lib_cred.cpp
    //

    // Path of the socket the daemon of the default store listens on:
    // $HOME/.ssh/credd/credd.sock.
    algo::tempstr SockPath();

    // Connect to the unix socket SOCK, send LINE followed by a newline, and read
    // one reply line into OUT_REPLY.  Returns false, with OUT_ERR saying why, when
    // nothing listens on SOCK, when the path is too long for a socket address, or
    // when the daemon sends no newline within two seconds.  The socket is closed
    // before returning either way.
    bool Request(algo::strptr sock, algo::strptr line, algo::cstring &out_reply, algo::cstring &out_err);

    // Ask the daemon on SOCK for its status and put the reply in OUT.  CLIENT names
    // the program asking, for the daemon's log.  True when a daemon answers,
    // whatever it said.
    //
    // A daemon outlives every rebuild of the tools that talk to it, and one built
    // before the status tuple had its present shape answers with a tuple this binary
    // cannot read.  That still counts as answering, so OUT.build reads `unreadable`
    // and a caller reports an old daemon rather than a missing one.
    bool ProbeFrom(algo::strptr sock, algo::strptr client, lib_cred::Status &out);

    // Ask the daemon of this login's own store for its status; see ProbeFrom for
    // what OUT holds and what CLIENT is for.
    bool Probe(algo::strptr client, lib_cred::Status &out);

    // Fetch the secret of credential NAME from the daemon listening on SOCK, naming the
    // caller CLIENT in the daemon's log.  The value lands in OUT_VALUE.  Returns
    // false, with OUT_ERR saying why, when no daemon answers or the daemon refuses:
    // an unknown name, or a secret it cannot open.
    bool FetchFrom(algo::strptr sock, algo::strptr client, algo::strptr name, algo::cstring &out_value, algo::cstring &out_err);

    // Fetch the secret of credential NAME from the daemon of the default store; see
    // FetchFrom.  This is the call a consumer makes.
    bool Fetch(algo::strptr client, algo::strptr name, algo::cstring &out_value, algo::cstring &out_err);

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
    algo::tempstr SysEvalStdin(algo::strptr cmd, algo::strptr input, int &out_status);

    // Return VALUE as a quoted string of a curl config, with each quote and
    // backslash in it escaped, so a secret in it reaches curl as written.  A caller
    // writes the config line around it and hands the config to SysEvalStdin.
    algo::tempstr GetCurlConfigStr(algo::strptr value);

    // Return the value the shell fragment TEXT assigns to the variable VAR, or an
    // empty string when TEXT assigns it nothing.  TEXT is a credential's text, the
    // lines a token file holds for a shell to source, such as
    // `export AWS_ACCESS_KEY_ID=...`.
    //
    // bash reads the fragment, so every quoting a writer chose reads back the way
    // the writer meant it, and the fragment travels on bash's standard input.
    algo::tempstr EvalVar(algo::strptr text, algo::strptr var);

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
    bool FetchFile(algo::strptr client, algo::strptr name, algo::strptr fname, algo::cstring &out_text, algo::cstring &out_err);
}
