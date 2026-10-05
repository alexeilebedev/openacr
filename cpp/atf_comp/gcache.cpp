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

// gcache asked to enable a cache directory that does not exist, wrapping a
// command that succeeds: the missing directory is reported and the run fails.
// A wrapped command's status adds to the run's exit code instead of replacing
// it, so a successful compile cannot mask the setup failure reported ahead of
// it.
void atf_comp::comptest_gcache_CacheDirFail() {
    LinkData("data");
    report::gcache report;
    RunGcache("enable", "", "-enable -dir:nosuchdir -- true", report, 1);
}

// A coverage cache hit whose object file cannot be written, and the mirror
// case where its coverage notes cannot be written. A cached coverage entry is
// one blob holding both halves, so that a hit can never pair a fresh .o with a
// stale .gcno; the restore has to keep that promise when only one half is
// writable. Each half's path is made unwritable in turn by holding it with a
// directory. Neither half may be left fresh on its own, and the run fails
// either way. The last run is the control: with both paths free the pair
// restores and the run succeeds.
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

// A cache entry that is not a whole coverage blob counts as a miss, so the
// compile runs again and republishes the entry.
// The cache directory outlives the builds that write it: another tool, an
// older gcache, an interrupted write can all leave a file under a coverage key
// that is not two non-empty halves. Restoring from one of those puts a fresh
// object next to whatever .gcno the working directory happens to hold, which
// gcov then rejects on a stamp mismatch and reports as 0% coverage for that
// object -- and the build sees no error, because a restore that was asked for
// half a pair and delivered it reports success. So validity is decided from
// the bytes: an entry is used only when the offset it starts with leaves bytes
// on both sides of itself.
// Each stage forges one kind of entry, marks x.gcno with a recognizable string,
// removes x.o, and runs the compile: WHOLE and AGAIN are the controls that must
// report a hit, while the rejected forms -- notes half truncated away, offset
// describing an empty object half, a bare .o left under the coverage key, fewer
// bytes than the offset itself, and the key held by a directory -- must all
// report a miss. In every stage the object has to come out non-empty and the
// marked .gcno has to be gone, because the pair the compile leaves behind is
// always a pair from one compile.
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

// Which compiler flags select the coverage cache format, through a real
// compile, over the spellings every driver the tree builds with accepts. A
// coverage entry is one blob holding the object and its coverage notes, so the
// format fits exactly the compiles that write notes: -ftest-coverage writes
// them, --coverage asks for instrumentation and notes together and -coverage
// is that flag with one dash, while -fprofile-arcs instruments the object and
// writes no notes file at all. A compile that has no notes to publish must
// still be cached as a bare object; asking it for a blob leaves it with one
// half, unpublishable, and so uncached forever -- a recompile, a warning, and
// the same miss on the next build. Each shape is compiled twice with the object
// and any notes removed in between, so the first run has to miss and the second
// has to hit, and the notes have to come back exactly for the shapes that
// produce them. The plain compile with no coverage flag is the control for the
// bare-object format, and -fprofile-arcs is the control that keeps the rule
// from reading as a rule about how many dashes a flag carries. The long forms
// gcc alone accepts, --test-coverage and --profile-arcs, are pinned by
// gcache.CoverageSpelling through a driver that runs no compiler.
// The name of the object is the format's other input, because the notes are the
// object's own name with its extension replaced: an output named x.obj is
// answered with x.gcno and one named foo with foo.gcno, so a compile whose object
// does not end in .o writes notes all the same and needs the same blob. Both
// names are compiled twice the way the flag shapes are, and the notes have to
// come back for each.
// An output named the way a precompiled header is named is the third name and the
// one that looks like an exception: gcc answers an output named h.hpp.gch with
// h.hpp.gcno under -c, and with h.hpp.gch-h.gcno without it. gcache caches only
// compiles that carry -c, so the name it derives is the name gcc writes, and the
// GCHNAME stage pins the notes coming back from a hit on such an output as well.
// What that stage leaves at h.hpp.gch is an object and not a precompiled header,
// because gcache compiles the preprocessed text of the translation unit rather
// than the header it was handed, so the stage pins the derivation from the name
// and says nothing about how a precompiled header is cached.
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

// The modification time of every file a cache hit writes, in both cache
// formats. A build tool decides that an object is out of date by comparing its
// modification time against its sources, so an object a hit puts in place has
// to carry the time of that hit; an object left with an older time is asked for
// again on the next build, and asked for again after that, because the restore
// does not clear the condition that triggered it. The case that reaches this is
// a hit whose cached bytes equal the bytes already at the target -- a header
// whose touch moved no preprocessed text, so the key still hits while the
// build tool considers the object stale. Each stage backdates the target to
// 2020 and runs the compile: the report has to say hit:Y, which is what makes
// the times meaningful, and each file the hit writes has to come out newer
// than the backdate. BAREABSENT is the control for a target that is not there
// at all, and BARESAME is the bare-object format's answer to the same
// identical-bytes case the coverage format faces in COVSAME.
// The cache entry the publish writes carries a modification time of its own, and
// the cleanup reads it: an entry older than the retention window is deleted, and
// the byte budget evicts in oldest-last-use order. A publish over an entry whose
// bytes have not changed therefore has to move that time as well, or an entry a
// build republished every day looks a week old. PUBSAME is that stage: the entry is
// backdated to 2020 and the same translation unit is published again under -force,
// with -frandom-seed fixing the coverage notes so the bytes really are identical --
// which the stage pins next to the time, since a publish that wrote different bytes
// would freshen the entry whatever rule it used.
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

// A run's exit code sums what it has to report: the wrapped command's own exit
// status, plus one for each failure that keeps the run from delivering the object
// it was asked for. So such a failure is not lost behind the command's status or
// behind another one of its kind. A failure that costs only a future cache miss
// is reported as a warning and adds nothing, so an entry that cannot be published
// leaves the code where the compile left it.
// The failures combined here are independent: the -install marker write, the
// wrapped command's own status, and a cache hit that cannot write its object
// file. Each is first shown alone (MARKER, CMD, and the object-file case of
// gcache.CoverageRestoreFail), then paired with the marker failure. The
// wrapped command in the paired cases is `false`, whose status is 1, so those
// codes have to reach 2; a command exiting 5 beside the same marker failure
// exits 6. The marker path is the one setup failure that
// leaves the cache usable, which is what lets a second, independent failure
// happen in the same run: the marker is a directory, so writing it fails while
// the cache directory around it still serves hits. HIT and CLEAN are
// the controls -- a run with nothing to report exits 0.
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

// An entry no compile can be served from -- one of no bytes, or a directory
// standing at an entry's path -- on the two sides of the cache and in both
// formats, next to the cleanup pass that plants one.
// A cache entry can end up empty without anyone noticing. The cleanup keeps an
// entry a recent log line names by refreshing that entry's modification time, and
// an entry the byte budget or an earlier age pass already deleted is named by a
// log line all the same, so a refresh that opens the path for writing recreates
// it with no bytes in it. The next compile with that key finds a file, calls it a
// hit, renames the empty file over its object and exits 0; the link that follows
// is what fails, naming the object as a file of an unrecognized format. Each such
// hit writes a log line of its own, so the log line stays young and the next
// cleanup recreates the file the age pass just deleted, and the translation unit
// stays wedged.
// A directory standing at an entry's path is the other shape a compile cannot be
// served from, and the two formats read it differently: the coverage format finds
// no offset in it and misses, while the bare format took its size for an object's
// and called it a hit, which fails the copy and leaves the run with no object at
// all. That one also outlives the build that met it, since a hit publishes
// nothing, so the key stays wedged until something removes the directory.
// So an entry has to be a regular file with bytes to be used, and the cleanup has
// to refresh without creating. The stages follow the chain: RECREATE deletes an
// entry and runs the cleanup, which must not put the path back; REFRESH backdates an entry
// past the retention window and runs the cleanup, which must move the entry's
// time up so the age pass in the same run keeps it, where a refresh that moves no
// time leaves the entry to be deleted; EMPTYHIT truncates an entry and asks for
// the compile, which must miss, compile, and republish an entry with bytes;
// PCHREFRESH is the refresh reaching the other entry a log line can name: a compile
// that used a precompiled header names the header beside its object, and nothing
// else moves that header's time, since the compiles that hit it only read it. The
// stage plants a file where a precompiled header would sit, rewrites the log so
// its lines name that file, backdates it past the retention window and runs the
// cleanup, which must keep it. The log is rewritten rather than earned through a
// compile because the cleanup reads the log and nothing else, so what built the
// header is not its concern; gcache.Pch is where the header gets built.
// EMPTYCOV is the same case in the coverage format, which reads the same entry as
// a blob whose offset leaves no bytes on either side of itself and has always
// counted it a miss; DIRBARE puts a directory at a bare entry's path and pins that
// the compile misses, that the publish takes the path back, and that the next build
// with the same key hits; DIRCOV is the control for that shape in the coverage
// format, which reached the same verdict from the blob's own bytes; NOPUBLISH
// compiles to an object that keeps no bytes -- a target symlinked to /dev/null --
// and pins that nothing is published for it. The
// last stage is the control, an ordinary hit off an entry that has bytes.
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

// Which compiler flags select the coverage cache format, over every spelling
// gcache recognizes, through a driver that runs no compiler.
// gcache.CoverageFlag pins the rule against what a real compiler writes, and
// so can use only the spellings every driver accepts. The two remaining ones,
// --test-coverage and --profile-arcs, are gcc's long forms of -ftest-coverage
// and -fprofile-arcs, and clang refuses them, so this test reads the
// classification through test/gcache/cc, which takes any flag, writes an
// object of a few bytes and writes notes beside it every time. The notes then
// come back from a hit exactly when gcache filed the entry as a blob, which is
// the classification and nothing else. Each spelling is compiled twice with the
// object and notes removed in between: the first run misses, the second hits,
// and the notes are present after the hit for the four spellings that write
// notes and absent for the two spellings of -fprofile-arcs and for the plain
// compile.
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

// A coverage entry serves the directory that wrote it, and a bare entry serves
// every directory.
// An object compiled for coverage carries the name of the profile file it
// writes at run time, and gcc anchors that name at the directory the compile
// runs in. The key of a cache entry is the preprocessing command and the
// preprocessed text, and both hold only the paths the command line spelled, so
// the same source compiled in two directories with a shared cache computes the
// same key. Served across directories, the second directory's object writes its
// profile under the first directory's path and gcov finds nothing to measure
// for it. So the key of a coverage compile carries the directory. The stages
// compile one source in directory A, then in B, then in A again, sharing one
// cache: without a coverage flag B hits off A's entry, and with --coverage B
// misses while A's second compile hits. The object B compiled names B's own
// directory in its profile name, which is the fact the miss exists to keep.
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

// The setup steps of -install, -enable and -disable, each with its status
// read, `done` printed only after all of them, and the link left as it was by
// a request that cannot be honored.
// One cache is shared per machine, so a cache that -install left disabled shows
// up only as build wall clock nobody attributes to gcache; the run has to say
// so and exit non-zero, and it must not say `done` first. NODIR asks for a
// cache directory under a path that is a regular file, so the directory cannot
// be created: the marker write reports it, nothing else reports it a second
// time, and the run exits 1. LINKDIR puts a directory where the .gcache link
// goes, so the directory is set up but the link cannot be created: the link
// failure is reported with its errno and the run exits 1. DISABLEDIR asks
// -disable to remove that directory, which unlink cannot: one gcache.error is
// reported and the run exits 1. The errno differs by platform, EISDIR on Linux
// and EPERM on macOS, and the golden masks it.
// CONTROL is the same install with the way clear, which says `done`, exits 0
// and leaves the link. BADDIR then asks -enable for a directory that does not
// exist: the run exits 1 and the working link is still there. DISABLE removes
// the link and exits 0, and REDISABLE on the link already gone is the state
// asked for, so it exits 0.
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

// A header marked with __gcache_pragma_pch_preprocess is precompiled once and
// reused by every translation unit that includes it first.
// gcache walks the preprocessed text for gcc's line markers, cuts the marked
// header out, compiles it to a .gch keyed on its text, and rewrites the unit
// to read that .gch through a pch_preprocess pragma. gcc alone builds such a
// header, so the test reads the walk through test/gcache/cc, which expands one
// level of #include with the same markers under -E and writes a .gch when the
// output is named for one. BUILD compiles p.cpp on an empty cache: the object
// misses, the header is built (pch_hit:N), pch_source names it and the .gch
// the report names is present. REUSE compiles q.cpp, a different unit with
// the same header first: the object misses and the header hits. HIT compiles
// p.cpp again and is served the object, so no header is consulted. FORCE
// compiles p.cpp under -force, which rebuilds the header and reads it back as
// built rather than hit, into the same entry BUILD wrote, since every build of
// the same header text lands on the same entry.
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
