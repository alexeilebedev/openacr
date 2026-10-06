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
// Target: llmtool (exe) -- Claude Code cost and status reporting
// Exceptions: yes
// Source: cpp/llmtool/status.cpp
//
// The status line Claude Code displays above its prompt.
// Claude Code runs a command of the user's choosing every couple of seconds
// and shows the line it prints.  It hands that command a json object on stdin
// describing the session as it stands: the model in use, the working
// directory, how much of the context window is spoken for, what the session
// has cost so far, and the token usage of the most recent api request.  This
// file reads that object and writes the line.
// The line prices the most recent request, and only that one.  Claude Code
// reports cumulative cost as a single number and offers no cumulative token
// split, so the four category figures here can cover nothing wider than the
// one request whose usage it does report.  A cumulative breakdown comes from
// llmtool -analyze, which reads the transcript instead.
// The categories are shares rather than dollars.  One request's category costs
// run to four decimal places, which nobody compares at a glance and nobody
// acts on; what a per-request split is read for is which category is consuming
// the request, and that is a share.  The dollar amount stays on last, where it
// is worth knowing absolutely.
// A field that would read zero is left out entirely, so the line stays short
// when little is happening and grows only when there is something to say.

#include "include/algo.h"
#include "include/llmtool.h"
#include "include/lib_json.h"

// Print USD as dollars and cents.
tempstr llmtool::PrintUsd(double usd) {
    algo::I64Dec2 dec;
    algo::value_qSetDouble(dec, usd);
    tempstr ret;
    ret << "$" << dec;
    return ret;
}

// Return the count Claude Code reports at PATH, which it writes either as a
// plain number or as an object counting the same thing under one of several
// names -- count, n, or the name the caller passes as FALLBACK.
u64 llmtool::GetCount(lib_json::FNode *root, algo::strptr path, algo::strptr fallback) {
    u64 ret = 0;
    lib_json::FNode *node = lib_json::node_Find(root, path);
    if (node && node->type == lib_json_FNode_type_object) {
        ret = llmtool::GetToken(node, "count");
        if (ret == 0) {
            ret = llmtool::GetToken(node, "n");
        }
        if (ret == 0) {
            ret = llmtool::GetToken(node, fallback);
        }
    } else if (node) {
        ret = llmtool::GetToken(root, path);
    }
    return ret;
}

// Return DIR with the user's home directory written as a tilde, which is how
// a status line spends its width on the part of the path that varies.
tempstr llmtool::Tildify(algo::strptr dir) {
    tempstr ret(dir);
    const char *home = getenv(algo_lib::dev_envvar_HOME);
    if (home && home[0] && StartsWithQ(dir, home)) {
        ret = tempstr() << "~" << ch_RestFrom(dir, ch_N(strptr(home)));
    }
    return ret;
}

// Return the number of tracked files modified in DIR's working tree.
int llmtool::CountModified(algo::strptr dir) {
    tempstr cmd;
    cmd << "git -C " << algo::strptr_ToBash(dir) << " ls-files -m";
    tempstr modified = algo::SysEval(cmd, algo::FailokQ(true), 1024 * 1024);
    int ret = 0;
    ind_beg(algo::Line_curs, line, modified) {
        if (line != "") {
            ret++;
        }
    }ind_end;
    return ret;
}

// Write the working directory and its modified-file count, which open the
// line because they say where the session is before saying what it is doing.
// ROOT is the status object Claude Code supplied; OUT is the line so far.
//
// A directory that no longer exists contributes nothing: a status line is
// redrawn every couple of seconds, and shelling out to git in a directory that
// has been deleted or unmounted costs a subprocess every refresh to report a
// number that could not mean anything.
void llmtool::PrintCwd(lib_json::FNode *root, algo::cstring &out) {
    tempstr dir(lib_json::strptr_Get(root, "workspace.current_dir"));
    if (dir == "") {
        dir = lib_json::strptr_Get(root, "cwd");
    }
    if (dir != "" && algo::DirectoryQ(dir)) {
        out << "cwd:" << llmtool::Tildify(dir);
        int nmod = llmtool::CountModified(dir);
        if (nmod > 0) {
            out << "  nmod:" << nmod;
        }
        out << "  ";
    }
}

// Carry out -status: read the status object Claude Code writes to stdin and
// print the one line it displays.
void llmtool::Status() {
    algo::cstring text;
    ind_beg(algo::FileLine_curs, line, algo::Fildes(0)) {
        text << line << eol;
    }ind_end;
    lib_json::FParser parser;
    lib_json::JsonParse(parser, text);
    lib_json::JsonParse(parser, "");
    algo::cstring out;
    if (parser.root_node) {
        lib_json::FNode *root = parser.root_node;
        llmtool::PrintCwd(root, out);
        tempstr display(lib_json::strptr_Get(root, "model.display_name"));
        out << (display == "" ? tempstr("claude") : display);
        u64 ctx = llmtool::GetToken(root, "context_window.total_input_tokens");
        ctx += llmtool::GetToken(root, "context_window.total_output_tokens");
        u64 limit = llmtool::GetToken(root, "context_window.context_window_size");
        if (ctx > 0) {
            out << "  ctx:" << (ctx + 500) / 1000 << "K";
        }
        if (limit > 0) {
            out << "  limit:" << (limit + 500) / 1000 << "K";
        }
        double cost = 0;
        double_ReadStrptrMaybe(cost, lib_json::strptr_Get(root, "cost.total_cost_usd"));
        if (cost > 0) {
            out << "  cost:" << llmtool::PrintUsd(cost);
        }
        u64 nagent = llmtool::GetCount(root, "agents", "running");
        u64 ntask = llmtool::GetCount(root, "tasks", "total");
        if (nagent > 0) {
            out << "  agents:" << nagent;
        }
        if (ntask > 0) {
            out << "  tasks:" << ntask;
        }
    }
    prlog(out);
}
