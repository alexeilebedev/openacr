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
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/gitinfo.cpp
//
// A binary has to be able to name the commit it was built from and the moment
// it was built, because what it creates outlives it and records its identity --
// a filesystem stamps both into its genesis log.  Neither fact is in the
// source tree; they belong to the act of building.  So abt writes them, as
// build/gitinfo.h, and cpp/lib/algo/arg.cpp picks the file up when it is there
// and reports "unversioned" when it is not.
// Both files are build products under the gitignored build/ directory, and they
// are rewritten only when the identity they name changes.  That is what keeps
// the stamp from churning the build: within one commit the header's timestamp
// never moves, so abt finds nothing out of date, and the object that carries the
// stamp is recompiled on the first build after the ref changes and not again.
// Because arg.cpp includes the header, abt's own scan makes the dependency real,
// and gcache keys on preprocessed text, so neither can serve a stale stamp.
// The identity cannot be read back out of the header -- the tuple sits in there
// as an escaped C literal -- and it has to be read back, because the build date
// belongs to the build that first saw a given ref and must survive every later
// build of it.  Comparing the whole text would defeat the purpose: the date is
// part of it, so the text differs on every run and the stamp would churn on
// every build.  So the tuple is also written plainly, as build/gitinfo.ssim,
// where the generated reader can take it back as a record and the comparison is
// over the identity fields alone.

#include "include/algo.h"
#include "include/abt.h"

// Read the two git facts the stamp carries, in one invocation: the abbreviated
// "<commit-date>.<short-hash>" label first, the full ref second.  Failure here
// is ordinary rather than exceptional -- a release tree unpacked without .git,
// or a machine with no git -- and leaves both empty, which is what makes the
// resulting stamp read as unversioned.
static void ReadGitfact(tempstr &shortref, tempstr &gitref) {
    tempstr out = SysEval("git log -1 --date=short --format=%ad.%h%n%H 2>/dev/null", algo::FailokQ(true), 1024);
    int line_n = 0;
    ind_beg(algo::Line_curs,line,out) {
        if (line_n == 0) {
            shortref = line;
        } else if (line_n == 1) {
            gitref = line;
        }
        line_n++;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Write build/gitinfo.ssim and build/gitinfo.h, the dev.gitinfo tuple that
// algo::gitinfo_Get() returns.  Runs before the header scan, so the include
// arg.cpp carries always resolves.  The tuple names the commit and the moment
// of the build and nothing about the build directory: one pair of files serves
// every config, so a per-config value would make alternating configs restamp
// each other's builds.
void abt::WriteGitinfo() {
    tempstr shortref, gitref;
    ReadGitfact(shortref, gitref);
    dev::Gitinfo gitinfo;
    gitinfo.gitinfo = ch_N(shortref) ? tempstr() << shortref << ".abt" : tempstr("unversioned");
    gitinfo.gitref  = gitref;
    gitinfo.package = abt::_db.cmdline.package;
    tempstr ssimpath("build/gitinfo.ssim");
    dev::Gitinfo prev;
    bool same = dev::Gitinfo_ReadStrptrMaybe(prev, FileToString(ssimpath, algo::FileFlags()))
        && prev.gitinfo == gitinfo.gitinfo
        && prev.gitref == gitinfo.gitref
        && prev.package == gitinfo.package;
    if (!same) {
        // to the second: everything that shows this value shows it to a reader,
        // and the nanoseconds of a build's start are noise in all of them
        gitinfo.builddate = algo::UnTime(algo::CurrUnTime().value / algo::UNTIME_PER_SEC * algo::UNTIME_PER_SEC);
        cstring tuple;
        dev::Gitinfo_Print(gitinfo, tuple);
        // -package accepts any tag, so the tuple reaches the header as a quoted
        // C++ literal rather than spliced between hand-written quotes
        cstring literal;
        strptr_PrintCpp(tuple, literal);
        cstring text;
        algo_lib::Replscope R;
        Set(R, "$literal", literal);
        Ins(&R, text, "// Build identity stamped by abt.  A build product, not a source file.");
        Ins(&R, text, "#define ALGO_GITINFO $literal");
        tempstr hdrpath("build/gitinfo.h");
        algo::CreateDirRecurse("build", false);
        StringToFile(tempstr() << tuple << eol, ssimpath, algo::FileFlags());
        StringToFile(text, hdrpath, algo::FileFlags());
        if (FileToString(hdrpath, algo::FileFlags()) != text) {
            prerr("abt.gitinfo_write"<<Keyval("path",hdrpath)<<Keyval("comment","cannot stamp the build identity"));
            algo_lib::_db.exit_code = 1;
        }
    }
}
