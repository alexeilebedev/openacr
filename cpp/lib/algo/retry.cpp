// Copyright (C) 2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/retry.cpp -- retry_curs
//

#include "include/algo.h"

// Begin a retry loop with a WAIT-second budget.  Callers scale WAIT (e.g. by
// a slow-build factor); the cursor uses it as given.  FAILOK selects what
// budget exhaustion means: the default raises the last comment as the
// failure (the condition was mandatory); with FAILOK the loop simply ends,
// logging the last comment once -- for conditions worth waiting for but
// legal to proceed without.
void algo::retry_curs_Reset(algo::retry_curs &curs, double wait_sec, bool failok DFLTVAL(false)) {
    curs.accept = false;
    curs.failok = failok;
    curs.deadline_sec = wait_sec;
    curs.t0 = algo::CurrUnTime();
    curs.niter = 0;
    ch_RemoveAll(curs.comment);
}

// Continue while the body has not accepted and the budget is not spent.  At
// least one attempt always runs; once the budget is gone without acceptance,
// the last comment is raised as the failure -- or, under failok, logged once
// and the loop ends.  Clearing comment here, just before the next attempt,
// lets the body simply append.
bool algo::retry_curs_ValidQ(algo::retry_curs &curs) {
    bool ret = true;
    if (curs.accept) {
        ret = false;
    } else if (curs.niter > 0 && ToSecs(algo::CurrUnTime() - curs.t0) >= curs.deadline_sec) {
        if (curs.failok) {
            prlog("retry: proceeding after " << curs.niter << " tries over "
                  << curs.deadline_sec << "s: " << curs.comment);
            ret = false;
        } else {
            vrfy(false, tempstr() << "retry: gave up after " << curs.niter << " tries over "
                 << curs.deadline_sec << "s: " << curs.comment);
        }
    } else {
        ch_RemoveAll(curs.comment);
    }
    return ret;
}

// Count the attempt, log the comment when verbose, and sleep one poll
// interval before the next try (skipped once accepted).
void algo::retry_curs_Next(algo::retry_curs &curs) {
    curs.niter++;
    if (algo_lib::_db.cmdline.verbose) {
        prlog("algo.retry"
              << Keyval("iter", curs.niter)
              << Keyval("accept", curs.accept)
              << Keyval("comment", curs.comment));
    }
    if (!curs.accept) {
        usleep(200000);
    }
}

// The cursor is itself the handle the body reads and writes.
algo::retry_curs &algo::retry_curs_Access(algo::retry_curs &curs) {
    return curs;
}
