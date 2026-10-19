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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/covfinal.cpp
//

#include "include/algo.h"
#include "include/atf_ci.h"


// -----------------------------------------------------------------------------

// Merge the line counts of every coverage job into one measurement, write the
// reports, and check each target against dev.tgtcov (or, with -capture,
// rebaseline dev.tgtcov + dev.uncovfunc).  A coverage job is a cijob with a
// citest marked gcov, and its counts are in CovJobDir.
//
// In CI the counts arrive as the artifacts of the coverageA and coverageB jobs
// of the same pipeline.  A local run finds whatever an earlier run left in
// temp/cov, which may be another commit's: merged, it would report and even
// capture floors for a tree nobody measured.  So each job stamped the commit
// it measured, and a directory whose stamp is missing or names another commit
// fails the run before anything is merged.
void atf_ci::citest_cov_finalize() {
    tempstr head(Trimmed(SysEval("git rev-parse HEAD", FailokQ(false), 1024)));
    tempstr mergepath;
    bool fresh = true;
    ind_beg(atf_ci::_db_citest_curs, citest, atf_ci::_db) {
        tempstr covdir = atf_ci::CovJobDir(citest.cijob);
        // a job with several gcov citests is listed once
        bool listed = FindStr(tempstr() << ":" << mergepath << ":", tempstr() << ":" << covdir << ":") != -1;
        if (citest.gcov && !listed) {
            tempstr commit(Trimmed(FileToString(DirFileJoin(covdir,"commit"))));
            if (ch_N(mergepath)) {
                mergepath << ":";
            }
            mergepath << covdir;
            if (commit != head) {
                fresh = false;
                prlog("atf_ci.cov_stale"
                      <<Keyval("covdir",covdir)
                      <<Keyval("commit",commit)
                      <<Keyval("head",head)
                      <<Keyval("comment",commit == "" ? "no line counts here; run the coverage job first" : "line counts of another commit; run the coverage job again"));
            }
        }
    }ind_end;
    int rc = 1;
    if (fresh) {
        command::atf_cov_proc atf_cov;
        atf_cov.cmd.covdir = "temp/cov";
        atf_cov.cmd.logfile = "temp/cov/atf_cov.log";
        atf_cov.cmd.mergepath = mergepath;
        atf_cov.cmd.report = true;
        atf_cov.cmd.check = !CaptureQ();
        atf_cov.cmd.capture = CaptureQ();
        prlog("atf_ci.coverage"<<Keyval("cmd", atf_cov_ToCmdline(atf_cov)));
        rc = atf_cov_Exec(atf_cov);
        SysCmd(tempstr() << "cat temp/cov/summary.txt");
    }
    if (rc != 0) {
        algo_lib::_db.exit_code = 1;
    }
}
