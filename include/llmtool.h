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
// Header: include/llmtool.h
//

#include "include/gen/llmtool_gen.h"
#include "include/gen/llmtool_gen.inl.h"

namespace llmtool { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/llmtool/analyze.cpp
    //

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
    tempstr DirName(algo::strptr dir);

    // Return the directory llmtool would find DIR's transcripts in.
    //
    // Claude Code derives a session's storage location from the directory the
    // session runs in, by replacing every slash with a dash and hanging the result
    // under ~/.claude/projects.  The mapping is one-way -- a dash in the original
    // path is indistinguishable from a slash afterwards -- so this function is
    // used to find the transcripts of a directory llmtool already knows, and never
    // to recover a directory from a folder name.  Each transcript line states the
    // directory it ran in, which is where the report gets that instead.
    tempstr ProjectDir(algo::strptr dir);

    // Return the row for DIR, creating it and computing its display name once.
    llmtool::FDir &GetDir(algo::strptr dir);

    // Return the row that accumulates WORKTREE's spend under QUALIFIER, creating
    // it on first use.  The comma-separated key is composed here and nowhere else,
    // so the report's row identity has one definition.
    llmtool::FStat &GetStat(algo::strptr worktree, algo::strptr qualifier);

    // Charge one transcript line's api request to its worktree and its model.
    // SUBAGENT says which qualifier the line's spend belongs under.
    //
    // The line is charged only if it has not been charged already.  A request id
    // that is already in ind_req belongs to a line the caller has seen before --
    // the same request written out again -- and passing it on would double the
    // bill, so the insert being a no-op is what makes the totals right.  A line
    // with no usage, or one Claude Code marks synthetic, was never an api request
    // and costs nothing.
    void CountRequest(lib_json::FNode *root, bool subagent);

    // Read the transcript at PATH and charge every api request it records.
    // SUBAGENT says whether the file is a subagent's transcript.
    //
    // Most lines of a transcript are not api replies -- prompts, attachments, mode
    // changes, file snapshots -- and building a json tree for each of them is the
    // bulk of the work in a scan of a large session.  A line that does not contain
    // the word usage cannot carry a usage object, so testing for the substring
    // first skips the parse for them.  It is a filter and not a decision: a line
    // that passes still has to satisfy CountRequest.
    void ScanTranscript(algo::strptr path, bool subagent);

    // Read the session transcript at PATH together with the transcripts of every
    // subagent that session spawned.  A subagent keeps its own file beside the
    // session's, which is what lets the report separate a session's own spend from
    // the spend of the agents it fanned out to.
    void ScanSession(algo::strptr path);

    // Read every session Claude Code has recorded under PROJDIR.
    void ScanProject(algo::strptr projdir);

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
    void PrintReport();

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
    void Analyze();

    // -------------------------------------------------------------------
    // cpp/llmtool/llmtool.cpp
    //

    // Return the row for MODEL, creating an unpriced one the first time the model
    // is seen.  Every caller reaches a model's rates and running totals through
    // this function, so a model named only by a transcript and a model named only
    // by the price list both end up as rows and neither is silently dropped.
    llmtool::FModel &GetModel(algo::strptr model);

    // Return the path of the price list to read.  An explicit -rate is taken as
    // given; with none, .claude/costrate.json is looked for by walking up from the
    // directory holding this binary.
    //
    // The price list is found relative to the tool and not to the current
    // directory, because it belongs to the tool.  Its shape and the code that
    // parses it change together, so the copy that matches this binary is the one
    // in the tree this binary was built from -- and finding it that way is what
    // lets wt/mybranch/bin/llmtool be run from anywhere and still price correctly.
    // A current directory that happens to have a .claude of its own is not the
    // authority on what this binary understands.
    //
    // GetDirName keeps the separator it cuts at, so each step drops that separator
    // before asking for the parent.  Without it the second call is handed a name
    // that already ends in a slash, cuts nothing, and the walk stops one directory
    // above the binary -- close enough to look like it works and far enough to
    // find nothing.
    tempstr FindRate();

    // Read the price list and turn each of its model entries into an
    // llmtool.FModel row carrying that model's five rates.  The file also names
    // the duration to assume for a cache write whose duration went unrecorded;
    // that lands on _db.cache_write_default.  A missing or unparsable file is not
    // fatal -- every model then reads as unpriced, which the report states.
    void LoadRate();

    // Charge NTOKEN cache-write tokens whose duration the transcript did not
    // record, at the duration the price list says to assume for them.
    //
    // A current Claude Code records every cache write's duration, so this rate is
    // reached only by an older transcript, and the report says so when it is.  The
    // figure is the one estimate in the whole report: an hour-long write costs
    // 1.6x a five-minute one, so guessing the wrong duration moves the cache-write
    // column by more than half again.
    double UntimedCacheWriteUsd(llmtool::FModel &model, u64 ntoken);

    // Read the token count NAME out of a request's usage object.
    //
    // Every count in the report passes through here, so a field Claude Code stops
    // writing degrades to a zero rather than to a parse failure, and a count that
    // arrives as a json string rather than a number is read the same either way.
    u64 GetToken(lib_json::FNode *usage, algo::strptr name);

    // Load the price list, then carry out whichever command the options name.
    // The price list is read either way: -status prices one request with it, and
    // -analyze prices every request in a transcript with it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:llmtool

    // -------------------------------------------------------------------
    // cpp/llmtool/status.cpp
    //

    // Print USD as dollars and cents.
    tempstr PrintUsd(double usd);

    // Return the count Claude Code reports at PATH, which it writes either as a
    // plain number or as an object counting the same thing under one of several
    // names -- count, n, or the name the caller passes as FALLBACK.
    u64 GetCount(lib_json::FNode *root, algo::strptr path, algo::strptr fallback);

    // Return DIR with the user's home directory written as a tilde, which is how
    // a status line spends its width on the part of the path that varies.
    tempstr Tildify(algo::strptr dir);

    // Return the number of tracked files modified in DIR's working tree.
    int CountModified(algo::strptr dir);

    // Write the working directory and its modified-file count, which open the
    // line because they say where the session is before saying what it is doing.
    // ROOT is the status object Claude Code supplied; OUT is the line so far.
    //
    // A directory that no longer exists contributes nothing: a status line is
    // redrawn every couple of seconds, and shelling out to git in a directory that
    // has been deleted or unmounted costs a subprocess every refresh to report a
    // number that could not mean anything.
    void PrintCwd(lib_json::FNode *root, algo::cstring &out);

    // Carry out -status: read the status object Claude Code writes to stdin and
    // print the one line it displays.
    void Status();
}
