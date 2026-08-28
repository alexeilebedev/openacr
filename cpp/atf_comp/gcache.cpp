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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/gcache.cpp -- Comptests for gcache
//
// Comptests for gcache, the compiler cache. Each test builds its own cache
// directory inside the test's tempdir and wraps a real g++ invocation, so the
// .gcache link, the cache contents and the compiler's output files all stay
// inside that directory.

#include "include/algo.h"
#include "include/atf_comp.h"

// gcache asked to enable a cache directory that does not exist, wrapping a
// command that succeeds: the missing directory is reported and the run fails.
// A wrapped command's status adds to the run's exit code instead of replacing
// it, so a successful compile cannot mask the setup failure reported ahead of
// it.
void atf_comp::comptest_gcache_CacheDirFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && $$OLDPWD/$bindir/gcache -enable -dir:nosuchdir -- true'");
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
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && mkdir cache && printf \"int f(){return 1;}\\n\" > x.cpp && $$OLDPWD/$bindir/gcache -install -dir:cache > /dev/null && $$OLDPWD/$bindir/gcache -- g++ --coverage -c x.cpp -o x.o && rm -f x.o x.gcno && mkdir x.o; $$OLDPWD/$bindir/gcache -- g++ --coverage -c x.cpp -o x.o; echo ofail_code:$$?; test -e x.gcno && echo ofail_gcno:present || echo ofail_gcno:absent; rmdir x.o; rm -f x.gcno; mkdir x.gcno; $$OLDPWD/$bindir/gcache -- g++ --coverage -c x.cpp -o x.o; echo gcnofail_code:$$?; test -e x.o && echo gcnofail_o:present || echo gcnofail_o:absent; rmdir x.gcno; rm -f x.o; $$OLDPWD/$bindir/gcache -- g++ --coverage -c x.cpp -o x.o; echo hit_code:$$?; test -e x.o -a -e x.gcno && echo hit_pair:present || echo hit_pair:incomplete'");
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
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && mkdir cache && printf \"int f(){return 1;}\\n\" > x.cpp && G=$$OLDPWD/$bindir/gcache && $$G -install -dir:cache > /dev/null && $$G -- g++ --coverage -c x.cpp -o x.o; BLOB=$$(find cache -mindepth 3 -type f); try(){ rm -f x.o; printf \"stalenotes\\n\" > x.gcno; $$G -report -- g++ --coverage -c x.cpp -o x.o > rep.txt; echo $$1_code:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_/\"; test -s x.o && echo $$1_o:present || echo $$1_o:empty; grep -q stalenotes x.gcno && echo $$1_gcno:stale || echo $$1_gcno:fresh; }; try whole; truncate -s -$$(( $$(wc -c < x.gcno) )) $$BLOB; try nogcno; printf \"\\010\\000\\000\\000\\000\\000\\000\\000\" > $$BLOB; cat x.gcno >> $$BLOB; try noo; cp x.o $$BLOB; try legacy; printf \"abc\" > $$BLOB; try short; rm -f $$BLOB; mkdir $$BLOB; try dir; try again'");
}

// Which compiler flags select the coverage cache format, over every spelling
// gcc accepts for them. A coverage entry is one blob holding the object and its
// coverage notes, so the format fits exactly the compiles that write notes:
// -ftest-coverage writes them, --test-coverage is the driver's long form of it,
// --coverage asks for instrumentation and notes together and -coverage is that
// flag with one dash, while -fprofile-arcs and its long form --profile-arcs
// instrument the object and write no notes file at all. A compile that has no
// notes to publish must still be cached as a bare object; asking it for a blob
// leaves it with one half, unpublishable, and so uncached forever -- a
// recompile, a warning, and the same miss on the next build. Each shape is
// compiled twice with the object and any notes removed in between, so the first
// run has to miss and the second has to hit, and the notes have to come back
// exactly for the shapes that produce them. The plain compile with no coverage
// flag is the control for the bare-object format, and the two spellings of
// -fprofile-arcs are the controls that keep the rule from reading as a rule
// about how many dashes a flag carries.
// The name of the object is the format's other input, because the notes are the
// object's own name with its extension replaced: an output named x.obj is
// answered with x.gcno and one named foo with foo.gcno, so a compile whose object
// does not end in .o writes notes all the same and needs the same blob. Both
// names are compiled twice the way the flag shapes are, and the notes have to
// come back for each.
// An output named the way a precompiled header is named is the third name and the
// one that looks like an exception: gcc answers an output named h.h.gch with
// h.h.gcno under -c, and with h.h.gch-h.gcno without it. gcache caches only
// compiles that carry -c, so the name it derives is the name gcc writes, and the
// GCHNAME stage pins the notes coming back from a hit on such an output as well.
// What that stage leaves at h.h.gch is an object and not a precompiled header,
// because gcache compiles the preprocessed text of the translation unit rather
// than the header it was handed, so the stage pins the derivation from the name
// and says nothing about how a precompiled header is cached.
void atf_comp::comptest_gcache_CoverageFlag() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && mkdir cache && printf \"int f(){return 1;}\\n\" > x.cpp && printf \"inline int g(){return 2;}\\n\" > h.h && G=$$OLDPWD/$bindir/gcache && $$G -install -dir:cache > /dev/null; try(){ rm -f x.o x.gcno; $$G -report -- g++ $$2 -c x.cpp -o x.o > rep.txt; echo $$1_first:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_first_/\"; rm -f x.o x.gcno; $$G -report -- g++ $$2 -c x.cpp -o x.o > rep.txt; echo $$1_second:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_second_/\"; test -s x.o && echo $$1_o:present || echo $$1_o:empty; test -e x.gcno && echo $$1_gcno:present || echo $$1_gcno:absent; }; tryname(){ rm -f $$2 $$3; $$G -report -- g++ --coverage -c x.cpp -o $$2 > rep.txt; echo $$1_first:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_first_/\"; rm -f $$2 $$3; $$G -report -- g++ --coverage -c x.cpp -o $$2 > rep.txt; echo $$1_second:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_second_/\"; test -s $$2 && echo $$1_o:present || echo $$1_o:empty; test -e $$3 && echo $$1_gcno:present || echo $$1_gcno:absent; }; trygchname(){ rm -f h.h.gch h.h.gcno; $$G -report -- g++ --coverage -c h.h -o h.h.gch > rep.txt; echo $$1_first:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_first_/\"; rm -f h.h.gch h.h.gcno; $$G -report -- g++ --coverage -c h.h -o h.h.gch > rep.txt; echo $$1_second:$$?; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_second_/\"; test -s h.h.gch && echo $$1_o:present || echo $$1_o:empty; test -e h.h.gcno && echo $$1_gcno:present || echo $$1_gcno:absent; }; try both \"--coverage\"; try dashboth \"-coverage\"; try notesonly \"-ftest-coverage\"; try dashnotesonly \"--test-coverage\"; try arcsonly \"-fprofile-arcs\"; try dasharcsonly \"--profile-arcs\"; try plain \"\"; tryname obj x.obj x.gcno; tryname noext foo foo.gcno; trygchname gchname'");
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
// 2020, records MARKER, and runs the compile: the report has to say hit:Y,
// which is what makes the times meaningful, and each file the hit writes has to
// come out newer than MARKER. BAREABSENT is the control for a target that is
// not there at all, and BARESAME is the bare-object format's answer to the same
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
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && mkdir cache && printf \"int f(){return 1;}\\n\" > x.cpp && G=$$OLDPWD/$bindir/gcache && $$G -install -dir:cache > /dev/null; stamp(){ if test ! -e $$2; then echo $$1:absent; elif test $$2 -nt marker; then echo $$1:fresh; else echo $$1:stale; fi; }; try(){ touch marker; $$G -report -- g++ $$2 -c x.cpp -o x.o > rep.txt; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/$$1_/\"; stamp $$1_o x.o; stamp $$1_gcno x.gcno; }; rm -f x.o x.gcno; $$G -- g++ -c x.cpp -o x.o; rm -f x.o; try bareabsent \"\"; touch -t 202001010000 x.o; try baresame \"\"; rm -f x.o x.gcno; $$G -- g++ --coverage -c x.cpp -o x.o; touch -t 202001010000 x.o; rm -f x.gcno; try covnonotes \"--coverage\"; touch -t 202001010000 x.o x.gcno; try covsame \"--coverage\"; printf \"int g(){return 2;}\\n\" > y.cpp; touch marker0; rm -f y.o y.gcno; $$G -- g++ --coverage -frandom-seed=t -c y.cpp -o y.o; BLOB=$$(find cache -mindepth 3 -type f -newer marker0); cp $$BLOB before; touch -t 202001010000 $$BLOB; touch marker; $$G -report -force -- g++ --coverage -frandom-seed=t -c y.cpp -o y.o > rep.txt; grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/pubsame_/\"; cmp -s $$BLOB before && echo pubsame_bytes:same || echo pubsame_bytes:changed; stamp pubsame_entry $$BLOB'");
}

// A run's exit code sums what it has to report: the wrapped command's own exit
// status, plus one for each failure that keeps the run from delivering the object
// it was asked for. So such a failure is not lost behind the command's status or
// behind another one of its kind. A failure that costs only a future cache miss
// is reported as a warning and adds nothing, so an entry that cannot be published
// leaves the code where the compile left it.
// The failures combined here are independent: the -install marker write, the
// wrapped command's own status, and a cache hit that cannot write its object
// file. Each is first shown alone (MARKER_CODE, CMD_CODE, and the object-file
// case of gcache.CoverageRestoreFail), then paired with the marker failure. The
// wrapped command in the paired cases is `false`, whose status is 1, so those
// codes have to reach 2; a command exiting 5 beside the same marker failure
// exits 6. The marker path is the one setup failure that
// leaves the cache usable, which is what lets a second, independent failure
// happen in the same run: the marker is a directory, so writing it fails while
// the cache directory around it still serves hits. HIT_CODE and CLEAN_CODE are
// the controls -- a run with nothing to report exits 0.
void atf_comp::comptest_gcache_ExitCodeCount() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && mkdir cache && printf \"int f(){return 1;}\\n\" > x.cpp && $$OLDPWD/$bindir/gcache -install -dir:cache > /dev/null && $$OLDPWD/$bindir/gcache -- g++ --coverage -c x.cpp -o x.o; echo clean_code:$$?; rm -f cache/.keep; mkdir cache/.keep; $$OLDPWD/$bindir/gcache -install -dir:cache -- true > /dev/null; echo marker_code:$$?; $$OLDPWD/$bindir/gcache -install -dir:cache -- false > /dev/null; echo marker_cmd_code:$$?; rm -f x.o x.gcno; mkdir x.o; $$OLDPWD/$bindir/gcache -install -dir:cache -- g++ --coverage -c x.cpp -o x.o > /dev/null; echo marker_restore_code:$$?; rmdir x.o; rmdir cache/.keep; $$OLDPWD/$bindir/gcache -install -dir:cache -- false > /dev/null; echo cmd_code:$$?; rm -f x.o x.gcno; $$OLDPWD/$bindir/gcache -install -dir:cache -- g++ --coverage -c x.cpp -o x.o > /dev/null; echo hit_code:$$?'");
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
// stage backdates a freshly built precompiled header past the retention window and
// runs the cleanup, which must keep it;
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
    atf_comp::ProcStart("bash -c 'cd $tempdir && ln -s $$OLDPWD/data data && mkdir cache"
                        " && printf \"int f(){return 1;}\\n\" > x.cpp"
                        " && G=$$OLDPWD/$bindir/gcache && $$G -install -dir:cache > /dev/null;"
                        " ent(){ grep -o \"cached_file:[^ ]*\" rep.txt | sed \"s/cached_file://\"; };"
                        " stamp(){ if test ! -e $$2; then echo $$1:absent; elif test $$2 -nt marker; then echo $$1:fresh; else echo $$1:stale; fi; };"
                        " pub(){ rm -f x.o x.gcno; $$G -report -- g++ $$1 -c x.cpp -o x.o > rep.txt; };"
                        " pub \"\"; E=$$(ent); rm -f $$E; $$G -gc > /dev/null;"
                        " test -e $$E && echo recreate:present || echo recreate:absent;"
                        " pub \"\"; E=$$(ent); touch -t 202001010000 $$E; touch marker; $$G -gc > /dev/null; stamp refresh $$E;"
                        " printf \"void __gcache_pragma_pch_preprocess();\\ninline int g(){return 2;}\\n\" > h.h;"
                        " printf \"#include <h.h>\\nint f(){return 1;}\\n\" > p.cpp;"
                        " $$G -report -- g++ -I. -c p.cpp -o p.o > rep.txt;"
                        " P=$$(grep -o \"pch_file:[^ ]*\" rep.txt | sed \"s/pch_file://\");"
                        " touch -t 202001010000 $$P; touch marker; $$G -gc > /dev/null; stamp pchrefresh $$P;"
                        " pub \"\"; E=$$(ent); : > $$E; rm -f x.o;"
                        " $$G -report -- g++ -c x.cpp -o x.o > rep.txt; echo emptyhit_code:$$?;"
                        " grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/emptyhit_/\";"
                        " test -s x.o && echo emptyhit_o:present || echo emptyhit_o:empty;"
                        " test -s $$E && echo emptyhit_entry:present || echo emptyhit_entry:empty;"
                        " pub \"--coverage\"; E=$$(ent); : > $$E; rm -f x.o x.gcno;"
                        " $$G -report -- g++ --coverage -c x.cpp -o x.o > rep.txt; echo emptycov_code:$$?;"
                        " grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/emptycov_/\";"
                        " test -s x.o && echo emptycov_o:present || echo emptycov_o:empty;"
                        " test -s x.gcno && echo emptycov_gcno:present || echo emptycov_gcno:empty;"
                        " pub \"\"; E=$$(ent); rm -f $$E; mkdir $$E; rm -f x.o;"
                        " $$G -report -- g++ -c x.cpp -o x.o > rep.txt; echo dirbare_code:$$?;"
                        " grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/dirbare_/\";"
                        " test -s x.o && echo dirbare_o:present || echo dirbare_o:empty;"
                        " test -f $$E && echo dirbare_entry:file || echo dirbare_entry:dir;"
                        " rm -f x.o; $$G -report -- g++ -c x.cpp -o x.o > rep.txt;"
                        " grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/dirbare2_/\";"
                        " pub \"--coverage\"; E=$$(ent); rm -f $$E; mkdir $$E; rm -f x.o x.gcno;"
                        " $$G -report -- g++ --coverage -c x.cpp -o x.o > rep.txt; echo dircov_code:$$?;"
                        " grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/dircov_/\";"
                        " test -s x.gcno && echo dircov_gcno:present || echo dircov_gcno:empty;"
                        " test -f $$E && echo dircov_entry:file || echo dircov_entry:dir;"
                        " rm -f e.o; ln -s /dev/null e.o;"
                        " $$G -report -- g++ -c x.cpp -o e.o > rep.txt; echo nopublish_code:$$?;"
                        " test -e $$(ent) && echo nopublish_entry:present || echo nopublish_entry:absent;"
                        " pub \"\"; rm -f x.o;"
                        " $$G -report -- g++ -c x.cpp -o x.o > rep.txt; echo control_code:$$?;"
                        " grep -o \"\\bhit:[YN]\" rep.txt | sed \"s/^/control_/\";"
                        " test -s x.o && echo control_o:present || echo control_o:empty'");
}
