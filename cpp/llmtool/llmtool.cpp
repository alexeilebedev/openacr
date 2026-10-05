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
// Source: cpp/llmtool/llmtool.cpp
//
// The price list, and the choice between the tool's two commands.
// Claude Code charges by the token, at a rate that depends on the model and
// on which of four categories the token falls in: fresh input, output, a read
// from the prompt cache, or a write to it.  A cache write is charged again by
// how long the entry is held, five minutes or an hour.  All of those rates
// live in a json file rather than in this source, so a price change is an edit
// to data; every model the file names becomes one llmtool.FModel row here.
// A model the file does not name is still counted.  Its row is created on
// first sight with every rate left at zero and priced:N, so its tokens appear
// in the report and its dollars read zero rather than being computed from some
// other model's price.  A wrong price is worse than a missing one: the reader
// of a confident figure has no way to tell it was invented.

#include "include/algo.h"
#include "include/llmtool.h"
#include "include/lib_json.h"

// Return the row for MODEL, creating an unpriced one the first time the model
// is seen.  Every caller reaches a model's rates and running totals through
// this function, so a model named only by a transcript and a model named only
// by the price list both end up as rows and neither is silently dropped.
llmtool::FModel &llmtool::GetModel(algo::strptr model) {
    llmtool::FModel *ret = llmtool::ind_model_Find(model);
    if (!ret) {
        ret = &llmtool::model_Alloc();
        ret->model = model;
        llmtool::ind_model_InsertMaybe(*ret);
        llmtool::bh_model_Insert(*ret);
    }
    return *ret;
}

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
tempstr llmtool::FindRate() {
    tempstr ret(_db.cmdline.rate);
    tempstr exe = algo::GetExePath();
    algo::strptr dir = algo::GetDirName(exe);
    while (ret == "" && ch_N(dir) > 1) {
        algo::strptr trimmed = ch_FirstN(dir, ch_N(dir) - 1);
        tempstr candidate = algo::DirFileJoin(trimmed, ".claude/costrate.json");
        if (algo::FileQ(candidate)) {
            ret = candidate;
        }
        dir = algo::GetDirName(trimmed);
    }
    return ret;
}

// Read the price list and turn each of its model entries into an
// llmtool.FModel row carrying that model's five rates.  The file also names
// the duration to assume for a cache write whose duration went unrecorded;
// that lands on _db.cache_write_default.  A missing or unparsable file is not
// fatal -- every model then reads as unpriced, which the report states.
void llmtool::LoadRate() {
    lib_json::FParser parser;
    algo::cstring text = algo::FileToString(llmtool::FindRate(), algo::FileFlags());
    lib_json::JsonParse(parser, text);
    lib_json::JsonParse(parser, "");
    if (parser.root_node) {
        _db.cache_write_default = lib_json::strptr_Get(parser.root_node, "cache_write_default");
        lib_json::FNode *model_node = lib_json::node_Find(parser.root_node, "model");
        if (model_node) {
            ind_beg(lib_json::node_c_child_curs, field, *model_node) {
                lib_json::FNode *rate_node = c_child_Find(field, 0);
                if (rate_node) {
                    llmtool::FModel &model = llmtool::GetModel(field.value);
                    model.priced = true;
                    double_ReadStrptrMaybe(model.rate.input, lib_json::strptr_Get(rate_node, "input"));
                    double_ReadStrptrMaybe(model.rate.output, lib_json::strptr_Get(rate_node, "output"));
                    double_ReadStrptrMaybe(model.rate.cache_read, lib_json::strptr_Get(rate_node, "cache_read"));
                    double_ReadStrptrMaybe(model.rate.cache_write_5m, lib_json::strptr_Get(rate_node, "cache_write_5m"));
                    double_ReadStrptrMaybe(model.rate.cache_write_1h, lib_json::strptr_Get(rate_node, "cache_write_1h"));
                }
            }ind_end;
        }
    }
    if (_db.cache_write_default == "") {
        _db.cache_write_default = "5m";
    }
}

// Charge NTOKEN cache-write tokens whose duration the transcript did not
// record, at the duration the price list says to assume for them.
//
// A current Claude Code records every cache write's duration, so this rate is
// reached only by an older transcript, and the report says so when it is.  The
// figure is the one estimate in the whole report: an hour-long write costs
// 1.6x a five-minute one, so guessing the wrong duration moves the cache-write
// column by more than half again.
double llmtool::UntimedCacheWriteUsd(llmtool::FModel &model, u64 ntoken) {
    double rate = _db.cache_write_default == "1h" ? model.rate.cache_write_1h : model.rate.cache_write_5m;
    return ntoken / 1000000.0 * rate;
}

// Read the token count NAME out of a request's usage object.
//
// Every count in the report passes through here, so a field Claude Code stops
// writing degrades to a zero rather than to a parse failure, and a count that
// arrives as a json string rather than a number is read the same either way.
u64 llmtool::GetToken(lib_json::FNode *usage, algo::strptr name) {
    u64 ret = 0;
    u64_ReadStrptrMaybe(ret, lib_json::strptr_Get(usage, name));
    return ret;
}

// Load the price list, then carry out whichever command the options name.
// The price list is read either way: -status prices one request with it, and
// -analyze prices every request in a transcript with it.
void llmtool::Main() {
    llmtool::LoadRate();
    if (_db.cmdline.status) {
        llmtool::Status();
    }
    if (_db.cmdline.analyze) {
        llmtool::Analyze();
    }
}
