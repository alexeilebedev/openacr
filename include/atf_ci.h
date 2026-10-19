// Copyright (C) 2024-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
    // cpp/atf_ci/compA.cpp
    //

    // Run the comptests tagged cijob:compA (see RunCompShard for how the suite is cut).
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_atf_compA(); // atfdb.citest:atf_compA

    // -------------------------------------------------------------------
    // cpp/atf_ci/compB.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_atf_amc(); // atfdb.citest:atf_amc

    // Indent any .json files under ts/.
    // void citest_check_json(); // atfdb.citest:check_json
    // void citest_atf_unit(); // atfdb.citest:atf_unit

    // Run the comptests whose cijob field names CIJOB, one half of the component
    // tests, capturing their output instead when the run captures.
    //
    // The comptests ran as one job of twenty to thirty-eight minutes, while the jobs
    // beside it in the pipeline finished sooner.  So each comptest's cijob field names
    // one of two jobs that run side by side, compA and compB, and compB also runs the
    // job's other citests.  The tests are assigned by their measured runtime, which
    // leaves each job about a quarter hour.
    void RunCompShard(strptr cijob);

    // Run the comptests tagged cijob:compB (see RunCompShard for how the suite is cut).
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_atf_compB(); // atfdb.citest:atf_compB

    // Move a numbered stream through every shape an ams lane can take, and then
    // through the same shapes past a reader too slow to keep up with it.
    //
    // Three independent choices make a lane's shape, so the shapes are a cross
    // product rather than a list: where a message goes (one lane every reader shares,
    // or a lane per reader), how it travels (inline in those lanes, or as a reference
    // to a message in a chunk of the writer's message board), and how a party that
    // has to wait is woken (the poll loop finds the data, or a signal delivers it).
    // ams_sendtest numbers every message and checks each arrival against its
    // position, so a reference resolved to the wrong place or into a reused chunk, or
    // a ring wrapped over bytes a reader had not yet consumed, surfaces as a payload
    // mismatch rather than as a count that happens to add up.
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
    //
    // Then a shm channel per reader on the lane shapes.  A channel is a flow
    // inside the lane whose limit follows its own reader's read count, so the writer
    // is paced by each reader and not by lane space alone.
    //
    // The last pass is a correctness test of the writer's wake.  Every pass before
    // it sends from a recurring timer, which keeps the writer's loop awake, so a
    // wake that went missing would cost nothing and go unseen.  In park mode the
    // writer sleeps whenever it is refused, and a missing wake stops the run.
    // void citest_ams_sendtest(); // atfdb.citest:ams_sendtest

    // Runs in sandbox
    // void citest_acr_ed_ssimfile(); // atfdb.citest:acr_ed_ssimfile

    // Runs in sandbox
    // void citest_acr_ed_ssimdb(); // atfdb.citest:acr_ed_ssimdb

    // Runs in sandbox
    // void citest_acr_ed_unittest(); // atfdb.citest:acr_ed_unittest

    // Runs in sandbox
    // void citest_acr_ed_target(); // atfdb.citest:acr_ed_target

    // Runs in sandbox
    //
    // A table created now is listed by its namespace's page, and nothing was regenerated to
    // make that true: the list comes from dmmeta.ssimfile when the page is asked for, so the
    // page is right the moment the row exists.  The check that used to stand here asked
    // whether abt_md had written a markdown file per table and named it in a directory
    // README, which is the arrangement this replaced.
    // void citest_doc_after_ssimfile_is_added(); // atfdb.citest:doc_after_ssimfile_is_added

    // Check that each citest function lives in the file matching its cijob.
    // Expected: citest:xyz with cijob:zzz → function citest_xyz in cpp/atf_ci/zzz.cpp
    // TODO: make this table-driven, with table describing all function contraints
    // void citest_check_citest(); // atfdb.citest:check_citest

    // Evaluate the tutorials' inline commands, which the readme citest of the
    // normalize job leaves alone: they rebuild a sample program with acr_ed -write
    // and take most of a whole-tree abt_md pass.  The run takes no selection, so
    // abt_md alone says which readmes are tutorials.  A tutorial whose output moved
    // leaves the file modified, and the job fails on it.
    // void citest_readme_tut(); // atfdb.citest:readme_tut

    // -------------------------------------------------------------------
    // cpp/atf_ci/coverageA.cpp
    //

    // Build every target with the coverage cfg, as the first citest of a coverage
    // job.  atf_ci is the single place that builds the coverage binaries; the test
    // tools never build.  No -install, so bin/ keeps pointing at the release build
    // -- the citests reach the instrumented binaries through -bindir:build/coverage
    // instead.
    void BuildCoverage();

    // Directory that holds the line counts of coverage job CIJOB: what its last
    // citest writes, what CI uploads, and what covfinal merges.
    tempstr CovJobDir(strptr cijob);

    // Turn the profile data of the running citest's job into line counts, as the
    // last citest of a coverage job.  Every citest of the job marked gcov left its
    // data in temp/cov/<citest>.d; atf_cov runs gcov over each of those dirs, adds
    // the counts up, and writes one .cov.ssim per source into CovJobDir.  Beside
    // them goes the commit the counts measure, which covfinal compares with its
    // own checkout before it merges anything.  The floors are not checked here,
    // since a half measures a part of the suite and every target reads low.
    void RunCovGcov();

    // First citest of the coverageA job: build the coverage binaries.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_cov_prepA(); // atfdb.citest:cov_prepA

    // Run the C++ unit-test suite against the coverage build so unit-tested
    // library functions are credited.  atf_unit spawns no children, so naming
    // the instrumented binary directly is enough; GCC_PROFILE_DIR routes its
    // gcda.  perf_secs:0 skips the timing benchmarks.
    // void citest_atf_unit_cov(); // atfdb.citest:atf_unit_cov

    // Run every comptest against the coverage build: cfg:coverage makes
    // atf_comp's $bindir = build/coverage, and the gcda land in the covdir
    // RunCiTest exported via GCC_PROFILE_DIR.
    // void citest_atf_comp_cov(); // atfdb.citest:atf_comp_cov

    // Last citest of the coverageA job: its line counts, for covfinal.
    // void citest_cov_gcovA(); // atfdb.citest:cov_gcovA

    // -------------------------------------------------------------------
    // cpp/atf_ci/coverageB.cpp
    //

    // First citest of the coverageB job: build the coverage binaries.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_cov_prepB(); // atfdb.citest:cov_prepB

    // Last citest of the coverageB job: its line counts, for covfinal.
    // void citest_cov_gcovB(); // atfdb.citest:cov_gcovB

    // -------------------------------------------------------------------
    // cpp/atf_ci/covfinal.cpp
    //

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
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_cov_finalize(); // atfdb.citest:cov_finalize

    // -------------------------------------------------------------------
    // cpp/atf_ci/depscan.cpp
    //

    // Scan every tracked lockfile and pom with osv-scanner, naming each file so the
    // scan covers what version control holds and nothing a build installed.  The
    // report is temp/depscan/osv.json.  The citest fails on an advisory whose
    // dev.vulnsev band fails and that no dev.vulnaccept row accepts, and when
    // osv-scanner cannot run.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_depscan_lockfile(); // atfdb.citest:depscan_lockfile

    // Look up the release of each dev.vendorlib row in the NVD and print one
    // atf_ci.vuln line per CVE that the NVD marks as vulnerable in that release.
    // The NVD answers an unknown CPE name with no CVE at all, which would read as a
    // clean library.  So the CPE dictionary is asked first, and a release missing
    // from it fails the citest, as does an answer that holds no CVE list.  A CVE
    // whose dev.vulnsev band fails also fails the citest, unless a dev.vulnaccept
    // row accepts it and has not lapsed.
    // void citest_depscan_vendorlib(); // atfdb.citest:depscan_vendorlib

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
    // void Main(); // dmmeta.main:atf_ci

    // -------------------------------------------------------------------
    // cpp/atf_ci/memcheck.cpp
    //

    // First memcheck citest: build every target with the memcheck cfg, which is the
    // configuration the memcheck run drives.
    //
    // That cfg is release with valgrind's client requests compiled in and nothing
    // else changed.  The requests are what let a checker account for the records
    // inside an amc pool, and they are not free when nothing is watching -- a dozen
    // instructions on every allocation and every free -- so the configuration that
    // ships does not carry them.  Keeping the optimization identical to release is
    // the point of a separate cfg rather than reusing debug: the binary a checker
    // examines is generated the way the shipped one is, so what it finds is what the
    // shipped code would do.
    //
    // No -install, so bin/ keeps pointing at the release build; the run reaches the
    // annotated binaries through atf_comp's -cfg:memcheck instead.
    // Each memcheck shard's first citest calls this, so every shard builds its own
    // annotated tree; the build cache makes that a few seconds on a warm runner.
    void BuildMemcheck();

    // Run one shard of the comptests under valgrind memcheck against the annotated
    // build that BuildMemcheck produced.  CIJOB names the shard: atf_comp runs the
    // comptests whose memcheck field carries it.
    //
    // One job running every comptest under valgrind took 47 minutes on a CI runner
    // while every other job of the pipeline finished inside 20, so the suite is cut
    // by target into three jobs of about 15 minutes each, and each comptest's
    // memcheck attribute names the shard that runs it.
    void RunMemcheckShard(strptr cijob);

    // -------------------------------------------------------------------
    // cpp/atf_ci/memcheckA.cpp
    //

    // First citest of the memcheckA job: build every target with the memcheck cfg.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_mem_prepA(); // atfdb.citest:mem_prepA

    // Run the comptests tagged memcheck:memcheckA under valgrind (see
    // RunMemcheckShard for how the suite is cut).
    // void citest_atf_comp_memA(); // atfdb.citest:atf_comp_memA

    // -------------------------------------------------------------------
    // cpp/atf_ci/memcheckB.cpp
    //

    // First citest of the memcheckB job: build every target with the memcheck cfg.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_mem_prepB(); // atfdb.citest:mem_prepB

    // Run the comptests tagged memcheck:memcheckB under valgrind (see
    // RunMemcheckShard for how the suite is cut).
    // void citest_atf_comp_memB(); // atfdb.citest:atf_comp_memB

    // -------------------------------------------------------------------
    // cpp/atf_ci/memcheckC.cpp
    //

    // First citest of the memcheckC job: build every target with the memcheck cfg.
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_mem_prepC(); // atfdb.citest:mem_prepC

    // Run the comptests tagged memcheck:memcheckC under valgrind (see
    // RunMemcheckShard for how the suite is cut).
    // void citest_atf_comp_memC(); // atfdb.citest:atf_comp_memC

    // -------------------------------------------------------------------
    // cpp/atf_ci/normalize.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void citest_checkclean(); // atfdb.citest:checkclean

    // Delete files that haven't been accessed in the last couple days
    // void citest_cleantemp(); // atfdb.citest:cleantemp

    // Regenerate dev.gitfile from the git index, then check each tracked file against the
    // filesystem and against the dev.gitpath patterns.
    //
    // The patterns state where each kind of file lives: cpp/%.cpp, include/%.h, bin/%.  A
    // script committed as cpp/ctxledger/run.sh matches none of them, so it is reported and
    // the citest fails.  A pattern may match no file at all: the openacr package carries
    // every pattern into a tree that holds only part of this one.
    // void citest_gitfile(); // atfdb.citest:gitfile
    // void citest_scanreadme(); // atfdb.citest:scanreadme
    // void citest_quickreadme(); // atfdb.citest:quickreadme
    // void citest_ssimfile(); // atfdb.citest:ssimfile
    // void citest_normalize_acr(); // atfdb.citest:normalize_acr

    // source code police
    // void citest_src_lim(); // atfdb.citest:src_lim

    // run amc
    // void citest_amc(); // atfdb.citest:amc

    // Create a bootstrap file for each build dir
    // void citest_bootstrap(); // atfdb.citest:bootstrap
    // void citest_shebang(); // atfdb.citest:shebang
    // void citest_encoding(); // atfdb.citest:encoding

    // update file headers
    // void citest_file_header(); // atfdb.citest:file_header

    // Fail when a script or a hand-written source carries no copyright notice.
    // The check does not add a year: a holder's year stands for a change the
    // holder made, and a run of normalize changes nothing.
    // void citest_non_copyrighted(); // atfdb.citest:non_copyrighted
    // void citest_iffy_src(); // atfdb.citest:iffy_src
    // void citest_stray_gen(); // atfdb.citest:stray_gen
    // void citest_tempcode(); // atfdb.citest:tempcode
    // void citest_lineendings(); // atfdb.citest:lineendings
    // void citest_update_script(); // atfdb.citest:update_script

    // indent all script files modified in the last commit
    // void citest_indent_script(); // atfdb.citest:indent_script

    // Run static code analyzer
    // Check Linux only
    // void citest_cppcheck(); // atfdb.citest:cppcheck

    // indent any source files modified in the last commit
    // indentation under CYGWIN is broken -- and we don't have a cross-platform
    // solution. so only try it on Linux
    // void citest_indent_srcfile(); // atfdb.citest:indent_srcfile

    // Give the current copyright holder this year on every hand-written source and
    // script the last commit changed.
    // A holder's year in a notice stands for a change the holder made to the file.
    // Adding the year to every file once a year claims changes nobody made, so the
    // year goes on at the moment of the change, to the files the commit touched.  A
    // change that only reindents the file or rewrites its license header adds
    // nothing of the holder's, so it adds no year either.
    // void citest_copyright_srcfile(); // atfdb.citest:copyright_srcfile
    // void citest_readme(); // atfdb.citest:readme
    // void citest_normalize_amc_vis(); // atfdb.citest:normalize_amc_vis
    // void citest_normalize_acr_my(); // atfdb.citest:normalize_acr_my

    // Run apm -check over every package, so a key that matches nothing, a record
    // that references what its package does not carry, proprietary content in an
    // open-source package, a file split between two packages, a sync pair that
    // disagrees, or a word a package must not carry fails the normalize job.
    // void citest_apm_check(); // atfdb.citest:apm_check

    // Check that each installation function is alone in the file named after its row.
    // A reader who knows the installation knows which file to open, and an
    // installation whose steps are spread over two files has no such file.  The
    // pairing is not something the compiler can hold: amc binds a row to a function
    // by name wherever that function is written.  The script ainst writes for every
    // package, and the library form of it, must also parse: a step is shell text
    // inside C++ strings, so a stray quote compiles and breaks the script of every
    // package written with it, on every platform, with nothing else objecting.
    // void citest_check_ainst(); // atfdb.citest:check_ainst

    // Refuse a generated install script that cannot say what it installs.
    // ainst -script is inlined into the image builds, where it runs as root, so
    // whoever answers for the endpoint it reads from chooses what lands in the
    // image.  Four defects make that unanswerable, and none of them is ever needed:
    // piping a fetch into a shell, turning the certificate check off, naming latest
    // in a url, and cloning a repository without moving it to a named revision.
    // A clone is carried until a checkout or a reset moves it, so a script that
    // clones twice has to move each one, and a clone moved on its own line is
    // already satisfied.  A clone is carried no further than the installation it
    // stands in: the next one's stage function is where an unmoved clone is
    // reported, since nothing after that boundary can be the checkout it wanted.  A comment line is skipped whatever it is indented by,
    // because bash ignores it wherever the generator puts it.
    // The whole of what ainst can emit is read here, which is why this reads one
    // program rather than a directory: an installation reaches the network through
    // the generated fetch step and nowhere else, so the four checks have exactly
    // one text to cover.
    // void citest_install_script(); // atfdb.citest:install_script
}
