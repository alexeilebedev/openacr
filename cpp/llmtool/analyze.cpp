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
// Source: cpp/llmtool/analyze.cpp
//
// Reading the transcripts, and the report drawn from them.
// Claude Code writes one transcript file per session, under
// ~/.claude/projects/<directory>/, where <directory> is the working directory
// with every slash turned into a dash.  A subagent the session spawns keeps
// its own transcript in a subagents/ folder beside it.  Every line of a
// transcript is a json object, and the lines that came back from the api carry
// that request's token usage, the model that served it, and the directory the
// work happened in.  That is everything the report needs, so it needs no
// collector, no daemon and no telemetry -- only the files already on disk.
// One property of the format decides the shape of the code.  A single api
// request occupies several transcript lines, and every one of them repeats
// that request's usage in full, so adding the lines up counts most requests
// several times over; on a measured session that overstated output tokens by
// 2.7x.  The lines of one request share a request id.  So the first thing this
// file does with a request is insert its id into llmtool.FDb.ind_req, and only
// an id that was not already there is counted -- which makes a repeated line a
// no-op rather than a second charge.
// The report is ssim tuples rather than a drawn table.  One row is one
// worktree and one qualifier, main or subagent, and `llmtool -analyze |
// ssimfilt -t` is the table.  Rows partition the spend: no row contains
// another, so any subset of them can be added up.

#include "include/algo.h"
#include "include/llmtool.h"
#include "include/lib_json.h"

// Name DIR the way the report names it: relative to the directory llmtool runs
// in, or abbreviated with a tilde when that comes out shorter.
//
// The rows of an -all report are worktrees of one repo, so the reader is
// looking for the difference between them, not for the prefix they share.
// Spelling a sibling worktree ../mybranch says where it is in one glance,
// where the absolute path buries it at the end of six components that every
// other row repeats.  A directory in another tree entirely has no short
// relative name -- ../../../../elsewhere is worse than ~/elsewhere -- so
// whichever of the two forms is shorter wins.
tempstr llmtool::DirName(algo::strptr dir) {
    tempstr ret;
    if (!StartsWithQ(dir, "/")) {
        ret = dir == "" ? strptr("?") : dir;
    } else {
        tempstr curdir = algo::GetCurDir();
        algo::strptr cwd(curdir);
        int ncommon = 0;
        int i = 0;
        while (i < ch_N(dir) && i < ch_N(cwd) && dir[i] == cwd[i]) {
            if (dir[i] == '/') {
                ncommon = i;
            }
            i++;
        }
        bool cwd_ends = i == ch_N(cwd) && (i == ch_N(dir) || dir[i] == '/');
        bool dir_ends = i == ch_N(dir) && (i == ch_N(cwd) || cwd[i] == '/');
        if (cwd_ends || dir_ends) {
            ncommon = i;
        }
        int nup = 0;
        int j = ncommon;
        while (j < ch_N(cwd)) {
            if (cwd[j] == '/') {
                nup++;
            }
            j++;
        }
        while (nup > 0) {
            if (ch_N(ret) > 0) {
                ret << "/";
            }
            ret << "..";
            nup--;
        }
        if (ncommon < ch_N(dir)) {
            if (ch_N(ret) > 0) {
                ret << "/";
            }
            ret << ch_RestFrom(dir, ncommon + 1);
        }
        if (ch_N(ret) == 0) {
            ret = ".";
        }
        const char *home = getenv(algo_lib::dev_envvar_HOME);
        if (home && home[0] && StartsWithQ(dir, home)) {
            tempstr tilde;
            tilde << "~" << ch_RestFrom(dir, ch_N(strptr(home)));
            if (ch_N(tilde) < ch_N(ret)) {
                ret = tilde;
            }
        }
    }
    return ret;
}

// Return the directory llmtool would find DIR's transcripts in.
//
// Claude Code derives a session's storage location from the directory the
// session runs in, by replacing every slash with a dash and hanging the result
// under ~/.claude/projects.  The mapping is one-way -- a dash in the original
// path is indistinguishable from a slash afterwards -- so this function is
// used to find the transcripts of a directory llmtool already knows, and never
// to recover a directory from a folder name.  Each transcript line states the
// directory it ran in, which is where the report gets that instead.
tempstr llmtool::ProjectDir(algo::strptr dir) {
    tempstr ret;
    ret << getenv(algo_lib::dev_envvar_HOME) << "/.claude/projects/";
    int i = 0;
    while (i < ch_N(dir)) {
        ret << (dir[i] == '/' ? '-' : dir[i]);
        i++;
    }
    return ret;
}

// Return the row for DIR, creating it and computing its display name once.
llmtool::FDir &llmtool::GetDir(algo::strptr dir) {
    llmtool::FDir *ret = llmtool::ind_dir_Find(dir);
    if (!ret) {
        ret = &llmtool::dir_Alloc();
        ret->dir = dir;
        ret->name = llmtool::DirName(dir);
        llmtool::ind_dir_InsertMaybe(*ret);
    }
    return *ret;
}

// Return the row that accumulates WORKTREE's spend under QUALIFIER, creating
// it on first use.  The comma-separated key is composed here and nowhere else,
// so the report's row identity has one definition.
llmtool::FStat &llmtool::GetStat(algo::strptr worktree, algo::strptr qualifier) {
    tempstr key;
    key << worktree << "," << qualifier;
    llmtool::FStat *ret = llmtool::ind_stat_Find(key);
    if (!ret) {
        ret = &llmtool::stat_Alloc();
        ret->stat = key;
        llmtool::ind_stat_InsertMaybe(*ret);
        llmtool::bh_stat_Insert(*ret);
    }
    return *ret;
}

// Charge one transcript line's api request to its worktree and its model.
// SUBAGENT says which qualifier the line's spend belongs under.
//
// The line is charged only if it has not been charged already.  A request id
// that is already in ind_req belongs to a line the caller has seen before --
// the same request written out again -- and passing it on would double the
// bill, so the insert being a no-op is what makes the totals right.  A line
// with no usage, or one Claude Code marks synthetic, was never an api request
// and costs nothing.
void llmtool::CountRequest(lib_json::FNode *root, bool subagent) {
    lib_json::FNode *usage = lib_json::node_Find(root, "message.usage");
    algo::strptr model_id = lib_json::strptr_Get(root, "message.model");
    algo::strptr reqid = lib_json::strptr_Get(root, "requestId");
    if (reqid == "") {
        reqid = lib_json::strptr_Get(root, "uuid");
    }
    bool billable = usage && reqid != "" && model_id != "" && model_id != "<synthetic>";
    if (billable && !llmtool::ind_req_Find(reqid)) {
        llmtool::FReq &req = llmtool::req_Alloc();
        req.reqid = reqid;
        llmtool::ind_req_InsertMaybe(req);
        llmtool::FModel &model = llmtool::GetModel(model_id);
        lib_json::FNode *duration = lib_json::node_Find(usage, "cache_creation");
        bool timed = duration && duration->type == lib_json_FNode_type_object;
        u64 input = llmtool::GetToken(usage, "input_tokens");
        u64 output = llmtool::GetToken(usage, "output_tokens");
        u64 cache_read = llmtool::GetToken(usage, "cache_read_input_tokens");
        u64 write_5m = llmtool::GetToken(usage, "cache_creation.ephemeral_5m_input_tokens");
        u64 write_1h = llmtool::GetToken(usage, "cache_creation.ephemeral_1h_input_tokens");
        u64 untimed = timed ? 0 : llmtool::GetToken(usage, "cache_creation_input_tokens");
        double usd_input = input / 1000000.0 * model.rate.input;
        double usd_output = output / 1000000.0 * model.rate.output;
        double usd_cache_read = cache_read / 1000000.0 * model.rate.cache_read;
        double usd_cache_write = write_5m / 1000000.0 * model.rate.cache_write_5m;
        usd_cache_write += write_1h / 1000000.0 * model.rate.cache_write_1h;
        usd_cache_write += llmtool::UntimedCacheWriteUsd(model, untimed);
        model.n_request++;
        model.input += input;
        model.output += output;
        model.cache_read += cache_read;
        model.cache_write += write_5m + write_1h + untimed;
        model.usd += usd_input + usd_output + usd_cache_read + usd_cache_write;
        llmtool::FDir &dir = llmtool::GetDir(lib_json::strptr_Get(root, "cwd"));
        llmtool::FStat &stat = llmtool::GetStat(dir.name, subagent ? strptr("subagent") : strptr("main"));
        stat.n_request++;
        stat.input += input;
        stat.output += output;
        stat.cache_read += cache_read;
        stat.cache_write += write_5m + write_1h + untimed;
        stat.cache_write_untimed += untimed;
        stat.usd_input += usd_input;
        stat.usd_output += usd_output;
        stat.usd_cache_read += usd_cache_read;
        stat.usd_cache_write += usd_cache_write;
    }
}

// Read the transcript at PATH and charge every api request it records.
// SUBAGENT says whether the file is a subagent's transcript.
//
// Most lines of a transcript are not api replies -- prompts, attachments, mode
// changes, file snapshots -- and building a json tree for each of them is the
// bulk of the work in a scan of a large session.  A line that does not contain
// the word usage cannot carry a usage object, so testing for the substring
// first skips the parse for them.  It is a filter and not a decision: a line
// that passes still has to satisfy CountRequest.
void llmtool::ScanTranscript(algo::strptr path, bool subagent) {
    ind_beg(algo::FileLine_curs, line, path) {
        if (algo::FindStr(line, "usage") >= 0) {
            lib_json::FParser parser;
            lib_json::JsonParse(parser, line);
            lib_json::JsonParse(parser, "");
            if (parser.root_node) {
                llmtool::CountRequest(parser.root_node, subagent);
            }
        }
    }ind_end;
}

// Read the session transcript at PATH together with the transcripts of every
// subagent that session spawned.  A subagent keeps its own file beside the
// session's, which is what lets the report separate a session's own spend from
// the spend of the agents it fanned out to.
void llmtool::ScanSession(algo::strptr path) {
    llmtool::ScanTranscript(path, false);
    tempstr pattern;
    pattern << algo::StripExt(path) << "/subagents/*.jsonl";
    ind_beg(algo::Dir_curs, entry, pattern) {
        llmtool::ScanTranscript(entry.pathname, true);
    }ind_end;
}

// Read every session Claude Code has recorded under PROJDIR.
void llmtool::ScanProject(algo::strptr projdir) {
    tempstr pattern;
    pattern << projdir << "/*.jsonl";
    ind_beg(algo::Dir_curs, entry, pattern) {
        llmtool::ScanSession(entry.pathname);
    }ind_end;
}

// Print the report: one report.llmtool row per worktree and qualifier, then
// one report.llmtool_model row per model.
//
// The model rows are what keeps the worktree rows honest.  A model the price
// list does not name is counted and not priced, so its worktree row states
// fewer dollars than were actually spent; its model row carries priced:N and
// the tokens that went unpriced, which is the only place a reader can see that
// the dollars are low and by how much.  A model that served no request in this
// report has nothing to say about it, and the price list names every model the
// project might ever run, so only the models actually used get a row.
//
// Dollars are accumulated in double and emitted as a fixed-point decimal.  The
// accumulation is over hundreds of thousands of per-request figures, most of
// them a small fraction of a cent, and rounding each one as it arrives would
// walk the total away from the truth.
//
// A cache write whose duration the transcript did not record was priced at the
// duration the price list nominates, which makes that row's cache-write
// dollars an estimate.  A current Claude Code times every cache write, so the
// count is zero for every report drawn from a current transcript; it goes to
// stderr rather than into a column of its own, which would otherwise be a
// permanently empty column in every table the tool ever prints.
void llmtool::PrintReport() {
    u64 untimed = 0;
    ind_beg(llmtool::_db_bh_stat_curs, stat, llmtool::_db) {
        report::llmtool out;
        out.stat = stat.stat;
        out.n_request = stat.n_request;
        out.input = stat.input;
        out.output = stat.output;
        out.cache_read = stat.cache_read;
        out.cache_write = stat.cache_write;
        untimed += stat.cache_write_untimed;
        algo::value_qSetDouble(out.usd_input, stat.usd_input);
        algo::value_qSetDouble(out.usd_output, stat.usd_output);
        algo::value_qSetDouble(out.usd_cache_read, stat.usd_cache_read);
        algo::value_qSetDouble(out.usd_cache_write, stat.usd_cache_write);
        algo::value_qSetDouble(out.usd, stat.usd_input + stat.usd_output + stat.usd_cache_read + stat.usd_cache_write);
        prlog(out);
    }ind_end;
    ind_beg(llmtool::_db_bh_model_curs, model, llmtool::_db) if (model.n_request > 0) {
        report::llmtool_model out;
        out.model = model.model;
        out.n_request = model.n_request;
        out.input = model.input;
        out.output = model.output;
        out.cache_read = model.cache_read;
        out.cache_write = model.cache_write;
        algo::value_qSetDouble(out.usd, model.usd);
        out.priced = model.priced;
        prlog(out);
    }ind_end;
    if (untimed > 0) {
        prerr("llmtool.untimed_cache_write"
              << Keyval("ntoken", untimed)
              << Keyval("priced_as", _db.cache_write_default)
              << Keyval("comment", "duration unrecorded; cache-write dollars are an estimate"));
    }
}

// Carry out -analyze: pick the transcripts the options select, read them, and
// print what they cost.
//
// The four selections narrow from widest to tightest.  -all reads every
// directory Claude Code has recorded, which is what answers "where has my
// money gone across all my worktrees".  With no option at all the report
// covers the directory llmtool runs in, every session of it, which is the
// question asked from inside a worktree.  -session and -transcript each name
// one session, the first by id within this directory and the second by path,
// which is how a session belonging to another directory is reached.
void llmtool::Analyze() {
    if (_db.cmdline.transcript != "") {
        llmtool::ScanSession(_db.cmdline.transcript);
    } else if (_db.cmdline.session != "") {
        tempstr path;
        path << llmtool::ProjectDir(algo::GetCurDir()) << "/" << _db.cmdline.session << ".jsonl";
        llmtool::ScanSession(path);
    } else if (_db.cmdline.all) {
        tempstr pattern;
        pattern << getenv(algo_lib::dev_envvar_HOME) << "/.claude/projects/*";
        ind_beg(algo::Dir_curs, entry, pattern) if (entry.is_dir) {
            llmtool::ScanProject(entry.pathname);
        }ind_end;
    } else {
        llmtool::ScanProject(llmtool::ProjectDir(algo::GetCurDir()));
    }
    llmtool::PrintReport();
}
