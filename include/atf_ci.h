// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Header: include/atf_ci.h
//

#include "include/algo.h"
#include "include/gen/atf_ci_gen.h"
#include "include/gen/atf_ci_gen.inl.h"

#include "include/lib_ctype.h"

namespace atf_ci { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_ci/apm.cpp
    //
    void CitestApm();
    void CitestApmReinstall();

    // -------------------------------------------------------------------
    // cpp/atf_ci/comp.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_atf_amc(); // gstatic/atfdb.citest:atf_amc

    // Indent any .json files under ts/.
    // void citest_check_json(); // gstatic/atfdb.citest:check_json
    // void citest_atf_unit(); // gstatic/atfdb.citest:atf_unit
    // void citest_atf_comp(); // gstatic/atfdb.citest:atf_comp

    // Move a numbered stream through every shape an ams lane can take, and then
    // through the same shapes past a reader too slow to keep up with it.
    //
    // Three independent choices make a lane's shape, so the shapes are a cross
    // product rather than a list: where a message goes (one lane every reader shares,
    // or a lane per reader), how it travels (inline in those lanes, or as a reference
    // to a slot on the writer's message board), and how a party that has to wait is
    // woken (the poll loop finds the data, or a signal delivers it).  ams_sendtest
    // numbers every message and checks each arrival against its position, so a
    // reference resolved to the wrong slot, or a ring wrapped over bytes a reader had
    // not yet consumed, surfaces as a payload mismatch rather than as a count that
    // happens to add up.
    //
    // A reader that pauses 200 microseconds per message is three orders slower than
    // the writer, so it falls behind and stays there.  The lane it reads is an order
    // smaller than the stream, so the writer runs out of room to put its next
    // message.  What the writer does about that is the contract worth checking, and
    // it differs by send mode.  A
    // non-blocking send is refused, and the refusal has to be counted and retried
    // rather than dropped, so the run must report refusals and still deliver every
    // message to every reader.  A blocking send waits for room instead, so it must
    // never be refused at all.  Both end on the message count rather than on the
    // tool's time limit, which is the property a slow reader threatens: the lane
    // slows to the reader's rate, and it does not stop.
    // void citest_ams_sendtest(); // gstatic/atfdb.citest:ams_sendtest

    // Runs in sandbox
    // void citest_acr_ed_ssimfile(); // gstatic/atfdb.citest:acr_ed_ssimfile

    // Runs in sandbox
    // void citest_acr_ed_ssimdb(); // gstatic/atfdb.citest:acr_ed_ssimdb

    // Runs in sandbox
    // void citest_acr_ed_unittest(); // gstatic/atfdb.citest:acr_ed_unittest

    // Runs in sandbox
    // void citest_acr_ed_target(); // gstatic/atfdb.citest:acr_ed_target

    // Runs in sandbox
    // void citest_abt_md_after_ssimfile_is_added(); // gstatic/atfdb.citest:abt_md_after_ssimfile_is_added
    // void citest_apm(); // gstatic/atfdb.citest:apm
    // void citest_apm_reinstall(); // gstatic/atfdb.citest:apm_reinstall

    // Check that each citest function lives in the file matching its cijob.
    // Expected: citest:xyz with cijob:zzz → function citest_xyz in cpp/atf_ci/zzz.cpp
    // TODO: make this table-driven, with table describing all function contraints
    // void citest_check_citest(); // gstatic/atfdb.citest:check_citest

    // -------------------------------------------------------------------
    // cpp/atf_ci/coverage.cpp
    //

    // First coverage citest: clear stale per-citest covdirs, then build every
    // target with the coverage cfg.  atf_ci is the single place that builds the
    // coverage binaries; the test tools never build.  No -install, so bin/ keeps
    // pointing at the release build -- the citests reach the instrumented
    // binaries through -bindir:build/coverage instead.  cov_prep runs no
    // instrumented binary, so it needs no covdir of its own.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_cov_prep(); // gstatic/atfdb.citest:cov_prep

    // Run the C++ unit-test suite against the coverage build so unit-tested
    // library functions are credited.  atf_unit spawns no children, so naming
    // the instrumented binary directly is enough; GCC_PROFILE_DIR routes its
    // gcda.  perf_secs:0 skips the timing benchmarks.
    // void citest_atf_unit_cov(); // gstatic/atfdb.citest:atf_unit_cov

    // Run every comptest against the coverage build: cfg:coverage makes
    // atf_comp's $bindir = build/coverage, and the gcda land in the covdir
    // RunCiTest exported via GCC_PROFILE_DIR.
    // void citest_atf_comp_cov(); // gstatic/atfdb.citest:atf_comp_cov

    // Last coverage citest: gcov + merge every per-citest covdir, write the
    // reports, and check against dev.tgtcov (or, with -capture, rebaseline
    // dev.tgtcov + dev.uncovfunc).
    // void citest_cov_finalize(); // gstatic/atfdb.citest:cov_finalize

    // -------------------------------------------------------------------
    // cpp/atf_ci/main.cpp
    //
    bool CaptureQ();

    // Compare the current `git ls-files -m` snapshot against the previous
    // one stored in atf_ci::_db.modfiles, report any newly-modified files,
    // and update atf_ci::_db.modfiles to the current snapshot.  Without
    // this delta, every test after the first dirtying one reports the
    // same pre-existing dirt and gets charged for it.
    // Return TRUE if no new modifications.  On drift, also set exit_code
    // (like CheckNoDir) so a caller that discards the return -- e.g. a
    // sandboxed citest whose per-test gate is skipped -- still fails the run.
    bool CheckCleanDirs();

    // Check that DIR is not generated by the test
    void CheckNoDir(strptr dir);
    algo::UnTime FileAtime(algo::strptr fname);

    // Return max. access time of all files in directory DIRNAME, as recursively
    // calculated. This is different from the directory access time.
    algo::UnTime DirAtime(algo::strptr dirname);

    // Compare contents of file `outfname` with the reference file.
    // Any difference = error
    void CompareOutput(strptr outfname);

    // Two modes: -cleanup removes the credentials this checkout installed, and
    // anything else runs the selected citests.
    //
    // The scrub takes none of the citest prologue, and -check_clean is the reason
    // that matters.  A run of the tests refuses to start on a dirty tree, because a
    // test may then overwrite work the operator has not committed.  The scrub reads
    // its own record under temp/ and writes nothing to the tree, so the same refusal
    // would only mean that a job which modified the tree -- which is most of them --
    // leaves its credential behind on the runner.  Both modes take the lockfile, so
    // a scrub cannot land while a run that installed a credential is still using it.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:atf_ci

    // -------------------------------------------------------------------
    // cpp/atf_ci/memcheck.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_atf_comp_mem(); // gstatic/atfdb.citest:atf_comp_mem

    // -------------------------------------------------------------------
    // cpp/atf_ci/normalize.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_checkclean(); // gstatic/atfdb.citest:checkclean

    // Delete files that haven't been accessed in the last couple days
    // void citest_cleantemp(); // gstatic/atfdb.citest:cleantemp
    // void citest_gitfile(); // gstatic/atfdb.citest:gitfile
    // void citest_scanreadme(); // gstatic/atfdb.citest:scanreadme
    // void citest_quickreadme(); // gstatic/atfdb.citest:quickreadme
    // void citest_ssimfile(); // gstatic/atfdb.citest:ssimfile
    // void citest_normalize_acr(); // gstatic/atfdb.citest:normalize_acr

    // source code police
    // void citest_src_lim(); // gstatic/atfdb.citest:src_lim

    // run amc
    // void citest_amc(); // gstatic/atfdb.citest:amc

    // Create a bootstrap file for each build dir
    // void citest_bootstrap(); // gstatic/atfdb.citest:bootstrap
    // void citest_shebang(); // gstatic/atfdb.citest:shebang
    // void citest_encoding(); // gstatic/atfdb.citest:encoding

    // update file headers
    // void citest_file_header(); // gstatic/atfdb.citest:file_header
    // void citest_non_copyrighted(); // gstatic/atfdb.citest:non_copyrighted
    // void citest_iffy_src(); // gstatic/atfdb.citest:iffy_src
    // void citest_stray_gen(); // gstatic/atfdb.citest:stray_gen
    // void citest_tempcode(); // gstatic/atfdb.citest:tempcode
    // void citest_lineendings(); // gstatic/atfdb.citest:lineendings
    // void citest_update_script(); // gstatic/atfdb.citest:update_script

    // indent all script files modified in the last commit
    // void citest_indent_script(); // gstatic/atfdb.citest:indent_script

    // Run static code analyzer
    // Check Linux only
    // void citest_cppcheck(); // gstatic/atfdb.citest:cppcheck

    // indent any source files modified in the last commit
    // indentation under CYGWIN is broken -- and we don't have a cross-platform
    // solution. so only try it on Linux
    // void citest_indent_srcfile(); // gstatic/atfdb.citest:indent_srcfile
    // void citest_readme(); // gstatic/atfdb.citest:readme
    // void citest_normalize_amc_vis(); // gstatic/atfdb.citest:normalize_amc_vis
    // void citest_normalize_acr_my(); // gstatic/atfdb.citest:normalize_acr_my
    // void citest_apm_check(); // gstatic/atfdb.citest:apm_check

    // Check that no package which forbids a word carries one.
    //
    // A package published downstream is read by people who have never seen the tree
    // it was published from, so a name belonging to that tree -- a namespace, a
    // tool, a host path -- is at best noise and at worst a dangling reference to
    // something they cannot look up.  Which words a package forbids is the
    // package's own statement, in dev.package.nomention, because the upstream tree
    // is the only one that knows what it is called; a downstream repository cannot
    // be asked to know about the trees that publish into it, and there may be
    // several.
    //
    // The package's evaluation is what gets checked, records and files alike, since
    // that is exactly what a push carries.  A row that must name a forbidden word
    // to do its job -- an exclusion naming the very namespace it excludes -- is
    // kept out of the package for that reason, so its absence here is the check
    // working rather than a hole in it.  The package's own dev.package row is the
    // one exception the check has to make for itself: it carries the list of
    // forbidden words, so it names every one of them by construction.
    // void citest_apm_nodownstream(); // gstatic/atfdb.citest:apm_nodownstream
    // void citest_apm_gen(); // gstatic/atfdb.citest:apm_gen
}
