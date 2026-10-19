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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/gcache.cpp -- Comptests for gcache
//
// Comptests for gcache, the compiler cache.  Each test builds its own cache
// directory inside the test's tempdir and runs gcache there once per stage, so
// the .gcache link, the cache contents and the compiler's output files all stay
// inside that directory.  The fixture -- the source files, a backdated object, a
// forged cache entry -- is made in C++, the report gcache prints is read with
// its generated reader, and what a stage established is recorded with Check.

#include "include/algo.h"
#include "include/atf_comp.h"
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// Whether the bytes of NEEDLE occur in HAY, either of which may hold NUL bytes.
static bool ContainsBytesQ(strptr hay, strptr needle) {
    return memmem(hay.elems, hay.n_elems, needle.elems, needle.n_elems) != NULL;
}

// Write TEXT to FILE under the tempdir, creating or replacing it.
static void WriteTemp(strptr file, strptr text) {
    StringToFile(text, atf_comp::TempPath(file));
}

// Return every byte of FILE under the tempdir, NUL bytes included, since an
// object file or a cache entry is what gets read here.
static tempstr ReadTemp(strptr file) {
    algo_lib::FFildes fd;
    fd.fd = OpenRead(atf_comp::TempPath(file));
    return algo::FdToString(fd.fd);
}

// Remove FILE under the tempdir, a file or a directory tree; a path that is
// not there is left alone.
static void RemoveTemp(strptr file) {
    tempstr path = atf_comp::TempPath(file);
    if (DirectoryQ(path)) {
        RemDirRecurse(path, true);
    } else {
        DeleteFile(path);
    }
}

// Create directory DIR under the tempdir, and the directories leading to it.
static void MkdirTemp(strptr dir) {
    CreateDirRecurse(atf_comp::TempPath(dir));
}

// Link NAME under the tempdir to the tree's data directory, which gcache reads
// its tables from.
static void LinkData(strptr name) {
    errno_vrfy_(symlink(algo::Zeroterm(algo::GetFullPath("data")), algo::Zeroterm(atf_comp::TempPath(name))) == 0);
}

// Return the moment every backdated file is set to, the start of 2020: a
// file a run wrote carries a later time, and one it left alone still carries this.
static algo::UnTime Backdate() {
    algo::UnixTime t;
    t.value = 1577836800;
    return algo::ToUnTime(t);
}

// Set the modification time of FILE under the tempdir to Backdate().
static void BackdateTemp(strptr file) {
    struct timeval tv[2];
    tv[0].tv_sec = algo::ToUnixTime(Backdate()).value;
    tv[0].tv_usec = 0;
    tv[1] = tv[0];
    (void)utimes(Zeroterm(atf_comp::TempPath(file)), tv);
}

// Record whether FILE under the tempdir was written since it was backdated, as
// `# check NAME:fresh` when its time is past Backdate(), `stale` when it is not,
// and `absent` when there is no such file.
static void CheckFresh(strptr name, strptr file) {
    tempstr path = atf_comp::TempPath(file);
    strptr state = "absent";
    if (FileQ(path)) {
        state = ModTime(path) > Backdate() ? "fresh" : "stale";
    }
    atf_comp::Check(name, state);
}

// Run gcache with ARGS from directory DIR under the tempdir (the tempdir itself
// when DIR is empty), as test stage STAGE, and wait for it to exit with code
// EXPECTED.  REPORT receives the report.gcache line the run printed, and is
// left blank when it printed none.
static void RunGcache(strptr stage, strptr dir, strptr args, report::gcache &report, int expected = 0) {
    atf_comp::Stage(stage);
    tempstr cmd;
    cmd << "cd $tempdir";
    if (ch_N(dir)) {
        cmd << "/" << dir;
    }
    cmd << " && $$OLDPWD/$bindir/gcache " << args;
    atf_comp::FProc &proc = atf_comp::ProcStart(cmd);
    tempstr out = atf_comp::ProcRead(proc, "");
    atf_comp::ProcWait(proc, expected);
    report = report::gcache();
    ind_beg(algo::Line_curs, line, out) {
        (void)report::gcache_ReadStrptrMaybe(report, line);
    }ind_end;
}

// Return the gcache arguments compiling SOURCE to OBJECT with COMPILER under
// FLAGS, the report requested.
static tempstr CompileArgs(strptr compiler, strptr flags, strptr source, strptr object) {
    tempstr args;
    algo::ListSep ls(" ");
    args << "-report -- " << compiler;
    if (ch_N(flags)) {
        args << " " << flags;
    }
    args << " -c " << source << " -o " << object;
    return args;
}

// Set up the cache every test starts from: the data link and gcache -install
// pointed at a directory named cache under the tempdir.
static void InstallCache() {
    LinkData("data");
    report::gcache report;
    RunGcache("install", "", "-install -dir:cache", report);
}

// Compile SOURCE to OBJECT twice with COMPILER under FLAGS, removing OBJECT and
// NOTES in between, as stages STAGE_first and STAGE_second, then record the
// state of both files.
static void TryCompile(strptr stage, strptr compiler, strptr flags, strptr source, strptr object, strptr notes) {
    report::gcache report;
    RemoveTemp(object);
    RemoveTemp(notes);
    RunGcache(tempstr() << stage << "_first", "", CompileArgs(compiler, flags, source, object), report);
    RemoveTemp(object);
    RemoveTemp(notes);
    RunGcache(tempstr() << stage << "_second", "", CompileArgs(compiler, flags, source, object), report);
    atf_comp::CheckFile(tempstr() << stage << "_o", object);
    atf_comp::CheckFile(tempstr() << stage << "_gcno", notes);
}

// Run gcache -enable on a cache directory that does not exist, wrapping the command
// true, and check that gcache reports the missing directory and exits 1.
// A wrapped command's status adds to the run's exit code, so a successful compile
// never masks the setup failure reported ahead of it.
void atf_comp::comptest_gcache_CacheDirFail() {
    LinkData("data");
    report::gcache report;
    RunGcache("enable", "", "-enable -dir:nosuchdir -- true", report, 1);
}

// Restore a coverage cache hit with one half of the pair unwritable, and check that
// gcache leaves neither half fresh on its own and fails the run:
// 1. Publish x.cpp compiled with --coverage.
// 2. Hold x.o with a directory, and compile again.  Check that the run exits 1, and
//    check the state of x.gcno.
// 3. Hold x.gcno with a directory, and compile again.  Check that the run exits 1,
//    and check the state of x.o.
// 4. Free both paths and compile again, the control.  Check that the pair restores
//    and the run succeeds.
// A cached coverage entry is one blob that holds both halves, so that a hit never
// pairs a fresh .o with a stale .gcno.  The restore has to keep that promise when
// only one half is writable.
void atf_comp::comptest_gcache_CoverageRestoreFail() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    report::gcache report;
    strptr compile = "-- g++ --coverage -c x.cpp -o x.o";
    RunGcache("publish", "", compile, report);
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    MkdirTemp("x.o");
    RunGcache("ofail", "", compile, report, 1);
    atf_comp::CheckFile("ofail_gcno", "x.gcno");
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    MkdirTemp("x.gcno");
    RunGcache("gcnofail", "", compile, report, 1);
    atf_comp::CheckFile("gcnofail_o", "x.o");
    RemoveTemp("x.gcno");
    RemoveTemp("x.o");
    RunGcache("hit", "", compile, report);
    atf_comp::CheckFile("hit_o", "x.o");
    atf_comp::CheckFile("hit_gcno", "x.gcno");
}

// One stage of gcache.CoverageBlobMiss, named STAGE: the object is removed,
// the notes are marked with a recognizable string, and the coverage compile
// runs.  The object has to come out with bytes, and the marked notes have to be
// gone, whatever the entry held.
static void TryBlob(strptr stage) {
    report::gcache report;
    RemoveTemp("x.o");
    WriteTemp("x.gcno", "stalenotes\n");
    RunGcache(stage, "", "-report -- g++ --coverage -c x.cpp -o x.o", report);
    atf_comp::CheckFile(tempstr() << stage << "_o", "x.o");
    bool stale = ContainsBytesQ(ReadTemp("x.gcno"), "stalenotes");
    atf_comp::Check(tempstr() << stage << "_gcno", stale ? "stale" : "fresh");
}

// Forge each kind of broken coverage entry, and check that gcache counts it as a
// miss, compiles again and republishes the entry:
// 1. Publish x.cpp compiled with --coverage.
// 2. Run the stages below in order.  Each forges one kind of entry, marks x.gcno
//    with a recognizable string, removes x.o and compiles.  Check in every stage
//    that the object comes out non-empty and that the marked x.gcno is gone.
// 3. WHOLE: leave the entry as published, the first control, and check that it
//    reports a hit.
// 4. Forge the rejected forms: the notes half truncated away, an offset that
//    describes an empty object half, a bare .o left under the coverage key, fewer
//    bytes than the offset itself, and the key held by a directory.  Check that
//    each reports a miss.
// 5. AGAIN: compile once more, the second control, and check that it reports a hit.
// The cache directory outlives the builds that write it.  Another tool, an older
// gcache or an interrupted write can leave a file under a coverage key that is not
// two non-empty halves.  Restoring from one puts a fresh object next to whatever
// .gcno the working directory holds.  gcov then rejects the pair on a stamp
// mismatch and reports 0% coverage for that object.  The build sees no error,
// because a restore asked for half a pair delivers that half and reports success.
// gcache therefore judges an entry from its bytes.  It uses an entry only when the
// offset that the entry starts with leaves bytes on both sides of itself.  The pair
// the compile leaves behind always comes from one compile.
void atf_comp::comptest_gcache_CoverageBlobMiss() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    report::gcache report;
    RunGcache("publish", "", "-report -- g++ --coverage -c x.cpp -o x.o", report);
    tempstr blob(report.cached_file);
    TryBlob("whole");
    tempstr whole = ReadTemp(blob);
    WriteTemp(blob, ch_FirstN(whole, ch_N(whole) - GetFileSize(atf_comp::TempPath("x.gcno"))));
    TryBlob("nogcno");
    tempstr noo;
    noo << strptr("\010\0\0\0\0\0\0\0", 8) << ReadTemp("x.gcno");
    WriteTemp(blob, noo);
    TryBlob("noo");
    WriteTemp(blob, ReadTemp("x.o"));
    TryBlob("legacy");
    WriteTemp(blob, "abc");
    TryBlob("short");
    RemoveTemp(blob);
    MkdirTemp(blob);
    TryBlob("dir");
    TryBlob("again");
}

// Compile through gcache with g++ under each spelling of the coverage flags and to
// each kind of object name, and check which compiles get the coverage cache format:
// 1. Compile x.cpp under --coverage, -coverage, -ftest-coverage, -fprofile-arcs and
//    no coverage flag.
// 2. Compile x.cpp with --coverage to x.obj and to foo.
// 3. GCHNAME: compile h.hpp with --coverage to h.hpp.gch.
// Each compile runs twice, with the object and any notes removed in between.  Check
// that the first run misses and the second hits, and that the notes come back
// exactly for the compiles that produce them.
//
// A coverage entry is one blob that holds the object and its coverage notes, so the
// format fits exactly the compiles that write notes.  -ftest-coverage writes them.
// --coverage asks for instrumentation and notes together, and -coverage is that
// flag with one dash.  -fprofile-arcs instruments the object and writes no notes
// file at all.  gcache must still cache a compile with no notes to publish, as a
// bare object.  Asking it for a blob leaves it with one half, unpublishable and so
// uncached forever: a recompile, a warning, and the same miss on the next build.
// The plain compile is the control for the bare-object format.  -fprofile-arcs is
// the control that keeps the rule from reading as a rule about how many dashes a
// flag carries.  gcache.CoverageSpelling pins the long forms that gcc alone
// accepts, --test-coverage and --profile-arcs, through a driver that runs no
// compiler.
//
// The name of the object is the format's other input, because the notes take the
// object's own name with its extension replaced.  gcc answers an output named x.obj
// with x.gcno, and one named foo with foo.gcno.  A compile whose object lacks the
// .o ending therefore writes notes all the same and needs the same blob.
//
// An output named like a precompiled header looks like an exception.  gcc answers
// an output named h.hpp.gch with h.hpp.gcno under -c, and with h.hpp.gch-h.gcno
// without it.  gcache caches only compiles that carry -c, so the name it derives is
// the name gcc writes.  The GCHNAME stage pins the notes that come back from a hit
// on such an output.  What the stage leaves at h.hpp.gch is an object, because
// gcache compiles the preprocessed text of the translation unit.  The stage pins
// the derivation from the name, and leaves open how gcache caches a precompiled
// header.
void atf_comp::comptest_gcache_CoverageFlag() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    WriteTemp("h.hpp", "inline int g(){return 2;}\n");
    TryCompile("both", "g++", "--coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("dashboth", "g++", "-coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("notesonly", "g++", "-ftest-coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("arcsonly", "g++", "-fprofile-arcs", "x.cpp", "x.o", "x.gcno");
    TryCompile("plain", "g++", "", "x.cpp", "x.o", "x.gcno");
    TryCompile("obj", "g++", "--coverage", "x.cpp", "x.obj", "x.gcno");
    TryCompile("noext", "g++", "--coverage", "x.cpp", "foo", "foo.gcno");
    TryCompile("gchname", "g++", "--coverage", "h.hpp", "h.hpp.gch", "h.hpp.gcno");
}

// One stage of gcache.HitMtime, named STAGE: compile x.cpp under FLAGS and
// record whether the object and the notes were written since their backdate.
static void TryMtime(strptr stage, strptr flags) {
    report::gcache report;
    RunGcache(stage, "", CompileArgs("g++", flags, "x.cpp", "x.o"), report);
    CheckFresh(tempstr() << stage << "_o", "x.o");
    CheckFresh(tempstr() << stage << "_gcno", "x.gcno");
}

// Hit the cache in both formats over targets that are absent, backdated or
// identical, and check that every file a hit writes carries the time of that hit:
// 1. Publish x.cpp as a bare object.
// 2. BAREABSENT: remove x.o and compile, the control for an absent target.
// 3. BARESAME: backdate x.o to 2020 and compile over the same bytes.
// 4. Publish x.cpp with --coverage.
// 5. COVNONOTES: backdate x.o, remove x.gcno and compile.
// 6. COVSAME: backdate x.o and x.gcno to 2020 and compile over the same bytes.
// 7. PUBSAME: publish y.cpp with -frandom-seed=t, backdate its entry to 2020, and
//    publish it again under -force.  Check that the entry's bytes stay the same and
//    that its time moves past the backdate.
// In steps 2, 3, 5 and 6 check that the report says hit:Y, and that each file the
// hit writes comes out newer than the backdate.
//
// A build tool decides that an object is out of date by comparing its modification
// time against its sources.  An object that a hit puts in place therefore has to
// carry the time of that hit.  The build tool asks again for an object left with an
// older time, on every later build, because the restore leaves the triggering
// condition in place.  A hit whose cached bytes equal the bytes already at the
// target reaches this case.  A header whose touch moved no preprocessed text keeps
// the key hitting while the build tool considers the object stale.  hit:Y makes the
// times meaningful.  BARESAME is the bare-object format's answer to the
// identical-bytes case that the coverage format faces in COVSAME.
//
// The cleanup reads the modification time of each cache entry.  It deletes an entry
// older than the retention window, and the byte budget evicts in oldest-last-use
// order.  A publish over an entry with unchanged bytes therefore has to move that
// time as well.  Otherwise an entry that a build republished every day looks a week
// old.  -frandom-seed fixes the coverage notes so the bytes really are identical.
// PUBSAME pins the bytes next to the time, because a publish that wrote different
// bytes would freshen the entry whatever rule it used.
void atf_comp::comptest_gcache_HitMtime() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    report::gcache report;
    RunGcache("publish_bare", "", "-- g++ -c x.cpp -o x.o", report);
    RemoveTemp("x.o");
    TryMtime("bareabsent", "");
    BackdateTemp("x.o");
    TryMtime("baresame", "");
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    RunGcache("publish_cov", "", "-- g++ --coverage -c x.cpp -o x.o", report);
    BackdateTemp("x.o");
    RemoveTemp("x.gcno");
    TryMtime("covnonotes", "--coverage");
    BackdateTemp("x.o");
    BackdateTemp("x.gcno");
    TryMtime("covsame", "--coverage");
    WriteTemp("y.cpp", "int g(){return 2;}\n");
    RunGcache("publish_seeded", "", "-report -- g++ --coverage -frandom-seed=t -c y.cpp -o y.o", report);
    tempstr blob(report.cached_file);
    tempstr before = ReadTemp(blob);
    BackdateTemp(blob);
    RunGcache("pubsame", "", "-report -force -- g++ --coverage -frandom-seed=t -c y.cpp -o y.o", report);
    atf_comp::Check("pubsame_bytes", ReadTemp(blob) == before ? "same" : "changed");
    CheckFresh("pubsame_entry", blob);
}

// Combine independent failures in one gcache run, and check that the exit code sums
// them:
// 1. CLEAN: compile x.cpp with --coverage, the first control, and check that the
//    run exits 0.
// 2. Hold the -install marker cache/.keep with a directory.
// 3. MARKER: run -install with the command true, and check that the run exits 1.
// 4. MARKER_CMD: run -install with the command `false`, whose status is 1, and
//    check that the run exits 2.
// 5. MARKER_RESTORE: hold x.o with a directory, and compile with -install.  Check
//    that the run exits 2, for the marker and for the hit that fails to write its
//    object file.
// 6. CMD: free the marker, run -install with the command `false`, and check that
//    the run exits 1.
// 7. HIT: compile again, the second control, and check that the run exits 0.
// A run's exit code is the wrapped command's own exit status, plus one for each
// failure that keeps the run from delivering the object that its caller asked for.
// Such a failure thus stays visible behind the command's status and behind another
// of its kind.  A failure that costs only a future cache miss is a warning and
// leaves the exit code alone.  An entry that fails to publish leaves the code where
// the compile left it.  gcache.CoverageRestoreFail shows the object-file failure
// alone.
// The marker write is the one setup failure that leaves the cache usable, which lets
// a second, independent failure happen in the same run.  The marker is a directory,
// so writing it fails while the cache directory around it still serves hits.
void atf_comp::comptest_gcache_ExitCodeCount() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    report::gcache report;
    RunGcache("clean", "", "-- g++ --coverage -c x.cpp -o x.o", report);
    RemoveTemp("cache/.keep");
    MkdirTemp("cache/.keep");
    RunGcache("marker", "", "-install -dir:cache -- true", report, 1);
    RunGcache("marker_cmd", "", "-install -dir:cache -- false", report, 2);
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    MkdirTemp("x.o");
    RunGcache("marker_restore", "", "-install -dir:cache -- g++ --coverage -c x.cpp -o x.o", report, 2);
    RemoveTemp("x.o");
    RemoveTemp("cache/.keep");
    RunGcache("cmd", "", "-install -dir:cache -- false", report, 1);
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    RunGcache("hit", "", "-install -dir:cache -- g++ --coverage -c x.cpp -o x.o", report);
}

// Publish x.cpp under FLAGS as stage STAGE, with the object and notes removed
// first so the compile runs, and return the path of the entry it wrote.
static tempstr Publish(strptr stage, strptr flags) {
    report::gcache report;
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    RunGcache(stage, "", CompileArgs("g++", flags, "x.cpp", "x.o"), report);
    return tempstr(report.cached_file);
}

// Rewrite the cache log under the tempdir so every line names PCH_FILE as the
// precompiled header the compile used.
static void ForgePchLog(strptr pch_file) {
    tempstr log = atf_comp::TempPath("cache/log.ssim");
    tempstr out;
    ind_beg(algo::FileLine_curs, line, log) {
        report::gcache report;
        if (report::gcache_ReadStrptrMaybe(report, line)) {
            report.pch_file = pch_file;
            out << report << eol;
        } else {
            out << line << eol;
        }
    }ind_end;
    StringToFile(out, log);
}

// Plant entries that no compile can use, an entry of no bytes or a directory at an
// entry's path, and check that gcache misses on them in both formats and that the
// cleanup plants none:
// 1. RECREATE: publish an entry, delete it and run -gc.  Check that the cleanup
//    leaves the path absent.
// 2. REFRESH: publish an entry, backdate it past the retention window and run -gc.
//    Check that the cleanup moves the entry's time up, so the age pass in the same
//    run keeps it.
// 3. PCHREFRESH: plant a file where a precompiled header would sit, and rewrite the
//    log so its lines name that file.  Backdate the file past the retention window
//    and run -gc, and check that the cleanup keeps it.
// 4. EMPTYHIT: publish an entry, truncate it and compile.  Check that the compile
//    misses, compiles, and republishes an entry with bytes.
// 5. EMPTYCOV: do the same in the coverage format, and check that the compile
//    misses.
// 6. DIRBARE: put a directory at a bare entry's path and compile.  Check that the
//    compile misses and that the publish takes the path back.  Compile again, and
//    check that the next build with the same key hits.
// 7. DIRCOV: do the same in the coverage format, the control for that shape.
// 8. NOPUBLISH: compile to e.o, a symlink to /dev/null that keeps no bytes, and
//    check that gcache publishes no entry for it.
// 9. CONTROL: hit an ordinary entry that has bytes.
//
// A cache entry can end up empty without anyone noticing.  The cleanup keeps an
// entry that a recent log line names by refreshing that entry's modification time.
// A log line still names an entry that the byte budget or an earlier age pass
// already deleted.  A refresh that opens the path for writing then recreates it with
// no bytes in it.  The next compile with that key finds a file, calls it a hit,
// renames the empty file over its object and exits 0.  The link that follows fails,
// naming the object as a file of an unrecognized format.  Each such hit writes a
// log line of its own, so the log line stays young.  The next cleanup recreates the
// file that the age pass just deleted, and the translation unit stays wedged.
//
// A directory at an entry's path is the other shape that no compile can use, and
// the two formats read it differently.  The coverage format finds no offset in it
// and misses.  A bare format that took the directory's size for an object's would
// call it a hit.  The copy would then fail and leave the run with no object at all.
// A hit publishes no entry, so the key would stay wedged past the build that met it,
// until someone removed the directory.
//
// gcache therefore uses an entry only when it is a regular file with bytes, and the
// cleanup has to refresh without creating.  A refresh that moves no time leaves the
// entry for the age pass to delete.  A compile that used a precompiled header names
// the header beside its object in the log.  The compiles that hit the header only
// read it, so the refresh alone moves its time.  PCHREFRESH rewrites the log because
// the cleanup reads only the log, and what built the header is outside its concern.
// gcache.Pch builds the header.  The coverage format reads an empty entry as a blob
// whose offset leaves no bytes on either side of itself, and counts it a miss.
void atf_comp::comptest_gcache_EmptyEntry() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    report::gcache report;
    tempstr entry = Publish("recreate_publish", "");
    RemoveTemp(entry);
    RunGcache("recreate_gc", "", "-gc", report);
    atf_comp::CheckFile("recreate", entry);
    entry = Publish("refresh_publish", "");
    BackdateTemp(entry);
    RunGcache("refresh_gc", "", "-gc", report);
    CheckFresh("refresh", entry);
    strptr pch = "cache/ab/cd/abcd.gch";
    MkdirTemp("cache/ab/cd");
    WriteTemp(pch, "header\n");
    ForgePchLog(pch);
    BackdateTemp(pch);
    RunGcache("pchrefresh_gc", "", "-gc", report);
    CheckFresh("pchrefresh", pch);
    entry = Publish("emptyhit_publish", "");
    WriteTemp(entry, "");
    RemoveTemp("x.o");
    RunGcache("emptyhit", "", "-report -- g++ -c x.cpp -o x.o", report);
    atf_comp::CheckFile("emptyhit_o", "x.o");
    atf_comp::CheckFile("emptyhit_entry", entry);
    entry = Publish("emptycov_publish", "--coverage");
    WriteTemp(entry, "");
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    RunGcache("emptycov", "", "-report -- g++ --coverage -c x.cpp -o x.o", report);
    atf_comp::CheckFile("emptycov_o", "x.o");
    atf_comp::CheckFile("emptycov_gcno", "x.gcno");
    entry = Publish("dirbare_publish", "");
    RemoveTemp(entry);
    MkdirTemp(entry);
    RemoveTemp("x.o");
    RunGcache("dirbare", "", "-report -- g++ -c x.cpp -o x.o", report);
    atf_comp::CheckFile("dirbare_o", "x.o");
    atf_comp::CheckFile("dirbare_entry", entry);
    RemoveTemp("x.o");
    RunGcache("dirbare_again", "", "-report -- g++ -c x.cpp -o x.o", report);
    entry = Publish("dircov_publish", "--coverage");
    RemoveTemp(entry);
    MkdirTemp(entry);
    RemoveTemp("x.o");
    RemoveTemp("x.gcno");
    RunGcache("dircov", "", "-report -- g++ --coverage -c x.cpp -o x.o", report);
    atf_comp::CheckFile("dircov_gcno", "x.gcno");
    atf_comp::CheckFile("dircov_entry", entry);
    RemoveTemp("e.o");
    errno_vrfy_(symlink("/dev/null", Zeroterm(atf_comp::TempPath("e.o"))) == 0);
    RunGcache("nopublish", "", "-report -- g++ -c x.cpp -o e.o", report);
    atf_comp::CheckFile("nopublish_entry", report.cached_file);
    entry = Publish("control_publish", "");
    RemoveTemp("x.o");
    RunGcache("control", "", "-report -- g++ -c x.cpp -o x.o", report);
    atf_comp::CheckFile("control_o", "x.o");
}

// Compile through test/gcache/cc under every spelling of the coverage flags that
// gcache recognizes, and check which spellings select the coverage cache format.
// Compile each spelling twice, with the object and notes removed in between.  Check
// that the first run misses and the second hits.  Check that the notes are present
// after the hit for the four spellings that write notes, and absent for the two
// spellings of -fprofile-arcs and for the plain compile.
// gcache.CoverageFlag pins the rule against what a real compiler writes, so it uses
// only the spellings every driver accepts.  The two remaining ones, --test-coverage
// and --profile-arcs, are gcc's long forms of -ftest-coverage and -fprofile-arcs.
// clang refuses them.  test/gcache/cc runs no compiler.  It takes any flag, writes
// an object of a few bytes, and writes notes beside it every time.  The notes
// therefore come back from a hit exactly when gcache filed the entry as a blob, so
// the test reads the classification alone.
void atf_comp::comptest_gcache_CoverageSpelling() {
    InstallCache();
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    strptr cc = "$$OLDPWD/test/gcache/cc";
    TryCompile("both", cc, "--coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("dashboth", cc, "-coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("notesonly", cc, "-ftest-coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("dashnotesonly", cc, "--test-coverage", "x.cpp", "x.o", "x.gcno");
    TryCompile("arcsonly", cc, "-fprofile-arcs", "x.cpp", "x.o", "x.gcno");
    TryCompile("dasharcsonly", cc, "--profile-arcs", "x.cpp", "x.o", "x.gcno");
    TryCompile("plain", cc, "", "x.cpp", "x.o", "x.gcno");
}

// One stage of gcache.CoverageCwd, named STAGE: compile x.cpp under FLAGS in
// directory DIR, with its object and notes removed first.
static void TryCwd(strptr stage, strptr dir, strptr flags) {
    report::gcache report;
    RemoveTemp(tempstr() << dir << "/x.o");
    RemoveTemp(tempstr() << dir << "/x.gcno");
    RunGcache(stage, dir, CompileArgs("g++", flags, "x.cpp", "x.o"), report);
}

// Compile one source in directory a, then in b, sharing one cache, and check that a
// coverage entry serves the directory that wrote it while a bare entry serves every
// directory:
// 1. Install the cache from a and from b.
// 2. Compile x.cpp without a coverage flag in a and then in b, and check that b
//    hits off a's entry.
// 3. Compile x.cpp with --coverage in a and then in b, and check that b misses.
// 4. Check that b's object names b's own directory in its profile name
//    (cov_b_name).  The miss exists to keep this fact.
// 5. Compile with --coverage in a again, and check that it hits.
// An object compiled for coverage carries the name of the profile file it writes at
// run time.  gcc anchors that name at the directory the compile runs in.  The key of
// a cache entry is the preprocessing command and the preprocessed text.  Both hold
// only the paths that the command line spelled, so the same source compiled in two
// directories with a shared cache computes the same key.  If gcache served the
// entry across directories, the second directory's object would write its profile
// under the first directory's path.  gcov would then find no data to measure for
// it.  The key of a coverage compile therefore carries the directory.
void atf_comp::comptest_gcache_CoverageCwd() {
    report::gcache report;
    MkdirTemp("a");
    MkdirTemp("b");
    WriteTemp("a/x.cpp", "int f(){return 1;}\n");
    WriteTemp("b/x.cpp", "int f(){return 1;}\n");
    LinkData("a/data");
    LinkData("b/data");
    RunGcache("install_a", "a", "-install -dir:../cache", report);
    RunGcache("install_b", "b", "-install -dir:../cache", report);
    TryCwd("bare_a", "a", "");
    TryCwd("bare_b", "b", "");
    TryCwd("cov_a", "a", "--coverage");
    TryCwd("cov_b", "b", "--coverage");
    bool own = ContainsBytesQ(ReadTemp("b/x.o"), "b/x.gcda");
    atf_comp::Check("cov_b_name", own ? "own" : "foreign");
    TryCwd("cov_a2", "a", "--coverage");
}

// Record whether the .gcache link under the tempdir is there, as `# check NAME:present|absent`.
static void CheckLink(strptr name) {
    tempstr path = atf_comp::TempPath(".gcache");
    bool present = ch_N(ReadLink(path)) > 0 || DirectoryQ(path) || FileQ(path);
    atf_comp::Check(name, present ? "present" : "absent");
}

// Fail each setup step of -install, -enable and -disable, and check that gcache
// reads each step's status, prints `done` only after all of them, and leaves the
// link as it was when it refuses a request:
// 1. NODIR: run -install with a cache directory under nofile, a regular file.
//    Check that the marker write reports the failure, that no other step reports it
//    a second time, and that the run exits 1.
// 2. LINKDIR: put a directory where the .gcache link goes, and run -install.  Check
//    that gcache sets up the cache directory, reports the link failure with its
//    errno, and exits 1.
// 3. DISABLEDIR: run -disable on that directory, which unlink fails to remove.
//    Check that gcache reports one gcache.error and exits 1.
// 4. CONTROL: clear the way and run -install.  Check that it says `done`, exits 0
//    and leaves the link.
// 5. BADDIR: run -enable on a directory that does not exist.  Check that the run
//    exits 1 and that the working link is still there.
// 6. DISABLE: run -disable, and check that it removes the link and exits 0.
// 7. REDISABLE: run -disable again, and check that it exits 0, because a link
//    already gone is the state asked for.
// A machine shares one cache, so a cache that -install left disabled shows up
// only as build wall clock that nobody attributes to gcache.  The run has to say so
// and exit non-zero, and it must not say `done` first.  The errno in step 3 differs
// by platform, EISDIR on Linux and EPERM on macOS, and the golden masks it.
void atf_comp::comptest_gcache_InstallFail() {
    LinkData("data");
    report::gcache report;
    WriteTemp("nofile", "");
    RunGcache("nodir", "", "-install -dir:nofile/cache", report, 1);
    MkdirTemp(".gcache");
    RunGcache("linkdir", "", "-install -dir:cache", report, 1);
    RunGcache("disabledir", "", "-disable", report, 1);
    RemoveTemp(".gcache");
    RunGcache("control", "", "-install -dir:cache", report);
    CheckLink("control_link");
    RunGcache("baddir", "", "-enable -dir:missing", report, 1);
    CheckLink("baddir_link");
    RunGcache("disable", "", "-disable", report);
    CheckLink("disable_link");
    RunGcache("redisable", "", "-disable", report);
}

// One stage of gcache.Pch, named STAGE: compile UNIT.cpp through the driver
// that runs no compiler, with EXTRA gcache options ahead of the report, and
// record the state of the precompiled header the report names and of the
// object.  Return the header's path, blank when the run named none.
static tempstr TryPch(strptr stage, strptr unit, strptr extra) {
    report::gcache report;
    tempstr object;
    object << unit << ".o";
    RemoveTemp(object);
    RemoveTemp(tempstr() << unit << ".gcno");
    tempstr args;
    if (ch_N(extra)) {
        args << extra << " ";
    }
    args << CompileArgs("$$OLDPWD/test/gcache/cc", "", tempstr() << unit << ".cpp", object);
    RunGcache(stage, "", args, report);
    if (ch_N(report.pch_file)) {
        atf_comp::CheckFile(tempstr() << stage << "_gch", report.pch_file);
    } else {
        atf_comp::Check(tempstr() << stage << "_gch", "none");
    }
    atf_comp::CheckFile(tempstr() << stage << "_o", object);
    return tempstr(report.pch_file);
}

// Compile two units that include a header marked with __gcache_pragma_pch_preprocess
// first, and check that gcache precompiles the header once and every such unit
// reuses it:
// 1. BUILD: compile p.cpp on an empty cache.  Check that the object misses, that
//    gcache builds the header (pch_hit:N) and pch_source names it, and that the .gch
//    the report names is present.
// 2. REUSE: compile q.cpp, a different unit with the same header first.  Check that
//    the object misses and the header hits.
// 3. HIT: compile p.cpp again, and check that gcache serves the object and consults
//    no header.
// 4. FORCE: compile p.cpp under -force.  Check that gcache rebuilds the header and
//    reads it back as built, into the same entry that BUILD wrote (force_entry).
// gcache walks the preprocessed text for gcc's line markers and cuts the marked
// header out.  It compiles the header to a .gch keyed on its text, and rewrites the
// unit to read that .gch through a pch_preprocess pragma.  gcc alone builds such a
// header, so the test reads the walk through test/gcache/cc.  Under -E that driver
// expands one level of #include with the same markers.  It writes a .gch when the
// output carries a .gch name.  Every build of the same header text lands on the
// same entry.
void atf_comp::comptest_gcache_Pch() {
    InstallCache();
    WriteTemp("h.h", "void __gcache_pragma_pch_preprocess();\ninline int g(){return 2;}\n");
    WriteTemp("p.cpp", "#include <h.h>\nint f(){return 1;}\n");
    WriteTemp("q.cpp", "#include <h.h>\nint k(){return 3;}\n");
    tempstr built = TryPch("build", "p", "");
    TryPch("reuse", "q", "");
    TryPch("hit", "p", "");
    tempstr forced = TryPch("force", "p", "-force");
    atf_comp::Check("force_entry", forced == built ? "same" : "other");
}

// -----------------------------------------------------------------------------

// gcache under clang, through test/gcache/cc run as a link named clang++, which
// answers gcache's probe with __clang__ and writes an object naming its source.
// A miss compiles x.cpp from the source, so the object names x.cpp, and the
// second compile hits on the stored probe answer and the filed object.  The
// driver edits edit.cpp while compiling it, so the source no longer reads as the
// text the key was taken from.  gcache files no object under that key, and the
// second compile of edit.cpp misses again: an object filed there would be the
// edited source's, served for text that never produced it.
void atf_comp::comptest_gcache_ClangSource() {
    InstallCache();
    vrfy_(symlink(Zeroterm(algo::DirFileJoin(algo::GetCurDir(), "test/gcache/cc")), Zeroterm(atf_comp::TempPath("clang++"))) == 0);
    WriteTemp("x.cpp", "int f(){return 1;}\n");
    report::gcache report;
    RunGcache("miss", "", CompileArgs("./clang++", "", "x.cpp", "x.o"), report);
    atf_comp::Check("object", Trimmed(ReadTemp("x.o")));
    RemoveTemp("x.o");
    RunGcache("hit", "", CompileArgs("./clang++", "", "x.cpp", "x.o"), report);
    atf_comp::Check("hit", report.hit ? "Y" : "N");
    WriteTemp("edit.cpp", "int g(){return 2;}\n");
    RunGcache("edited", "", CompileArgs("./clang++", "", "edit.cpp", "edit.o"), report);
    WriteTemp("edit.cpp", "int g(){return 2;}\n");
    RemoveTemp("edit.o");
    RunGcache("edited_again", "", CompileArgs("./clang++", "", "edit.cpp", "edit.o"), report);
    atf_comp::Check("edited_hit", report.hit ? "Y" : "N");
}
