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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/retry.cpp -- retry_curs
//

#include "include/algo.h"

// Return the time since the loop began, as text for a log line, such as 2.3s.
static tempstr ElapsedText(double elapsed) {
    tempstr ret;
    algo::double_PrintPrec(elapsed, ret, 1, false, false);
    ret << "s";
    return ret;
}

// Turn the verbose log category back on, if CURS is the cursor that turned
// it off for the body.  A nested loop finds verbose already off and leaves the
// restore to the loop outside it.
void algo::retry_curs_Unmute(algo::retry_curs &curs) {
    if (curs.mute) {
        algo_lib_logcat_verbose.enabled = true;
        curs.mute = false;
    }
}

// Begin a retry loop with a WAIT_SEC budget, kept in CURS.  Callers scale
// WAIT_SEC (e.g. by a slow-build factor); the cursor uses it as given.  FAILOK
// selects what budget exhaustion means: the default raises the last comment as
// the failure (the condition was mandatory); with FAILOK the loop simply ends,
// logging the last comment once -- for conditions worth waiting for but legal
// to proceed without.
void algo::retry_curs_Reset(algo::retry_curs &curs, double wait_sec, bool failok DFLTVAL(false)) {
    curs.accept = false;
    curs.failok = failok;
    curs.deadline_sec = wait_sec;
    curs.t0 = algo::CurrUnTime();
    curs.niter = 0;
    curs.nsec = 0;
    ch_RemoveAll(curs.comment);
    ch_RemoveAll(curs.prev_comment);
}

// Return true while the body of CURS has not accepted and the budget is not
// spent.  At least one attempt always runs.  Clearing the comment here, just
// before the next attempt, lets the body simply append.
//
// The body runs with the verbose log category off.  A body that polls a
// command five times a second would otherwise echo the command line on every
// attempt, and fifty identical lines would bury the comment, which is the line
// that says what the body saw.  retry_curs_Next turns verbose back on.
//
// When the loop ends, a verbose run logs the verdict: accept, proceed (budget
// spent under failok) or giveup.  A failok loop logs its verdict and last
// comment even when not verbose, since proceeding without the condition is
// worth a line.  A loop without failok that spends its budget raises the last
// comment as the failure.
bool algo::retry_curs_ValidQ(algo::retry_curs &curs) {
    double elapsed = ToSecs(algo::CurrUnTime() - curs.t0);
    bool verbose = algo_lib::_db.cmdline.verbose;
    bool expired = !curs.accept && curs.niter > 0 && elapsed >= curs.deadline_sec;
    bool ret = !curs.accept && !expired;
    if (ret) {
        ch_RemoveAll(curs.comment);
        curs.mute = algo_lib_logcat_verbose.enabled;
        algo_lib_logcat_verbose.enabled = false;
    } else if (verbose || (expired && curs.failok)) {
        tempstr out;
        out << "algo.retry"
            << Keyval("verdict", curs.accept ? "accept" : curs.failok ? "proceed" : "giveup")
            << Keyval("niter", curs.niter)
            << Keyval("elapsed", ElapsedText(elapsed));
        if (!verbose) {
            // a verbose run has logged this comment already, on the line where it appeared
            out << Keyval("comment", curs.comment);
        }
        prlog(out);
    }
    vrfy(!expired || curs.failok, tempstr() << "retry: gave up after " << curs.niter << " tries over "
         << curs.deadline_sec << "s: " << curs.comment);
    return ret;
}

// Turn verbose back on after the body of CURS, count the attempt, log its comment when verbose, and sleep one poll
// interval before the next try (skipped once accepted).
//
// A loop that waits for a cluster to settle evaluates the same condition five
// times a second, and the answer rarely changes: a ten-second wait printing
// every attempt buries the two answers that matter under fifty copies of the
// first.  So the log carries a line only when the comment differs from the one
// before, with the time since the loop began, and a dot for each second that
// passes in between.  A wait that sees ten distinct answers prints ten lines.
void algo::retry_curs_Next(algo::retry_curs &curs) {
    algo::retry_curs_Unmute(curs);
    curs.niter++;
    if (algo_lib::_db.cmdline.verbose) {
        double elapsed = ToSecs(algo::CurrUnTime() - curs.t0);
        i32 sec = i32(elapsed);
        if (curs.niter == 1 || curs.comment != curs.prev_comment) {
            prlog("algo.retry"
                  << Keyval("elapsed", ElapsedText(elapsed))
                  << Keyval("iter", curs.niter)
                  << Keyval("comment", curs.comment));
            curs.prev_comment = curs.comment;
        } else if (sec > curs.nsec) {
            algo::PrlogDot(sec - curs.nsec);
        }
        curs.nsec = sec;
    }
    if (!curs.accept) {
        usleep(200000);
    }
}

// The cursor CURS is itself the handle the body reads and writes; return it.
algo::retry_curs &algo::retry_curs_Access(algo::retry_curs &curs) {
    return curs;
}
