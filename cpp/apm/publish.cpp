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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/publish.cpp
//
// apm -publish, the one way a package leaves this tree.  It produces the package
// in a fresh clone of its destination, regenerates and verifies it there, shows
// the maintainer what goes out, and pushes it only after they say yes twice.

#include "include/algo.h"
#include "include/apm.h"

#include "include/lib_cred.h"

// Print the heading of publish stage NAME, so a maintainer reading the run sees
// which step is working and which one stopped it.
static void PublishStage(algo::strptr name) {
    prlog("");
    prlog("== apm.publish  " << name);
}

// -----------------------------------------------------------------------------

// Ask the maintainer QUESTION and return true when the answer begins with y.  The
// answer is one line of standard input, and end of input is a no, so a run with
// nobody at the terminal publishes nothing.
static bool AskYes(algo::strptr question) {
    prlog(question << " [y/N]");
    char buf[256];
    bool got = fgets(buf, sizeof(buf), stdin) != NULL;
    return got && (buf[0] == 'y' || buf[0] == 'Y');
}

// -----------------------------------------------------------------------------

// Run shell command CMD in directory DIR and return true when it exits 0.
static bool RunIn(algo::strptr dir, algo::strptr cmd) {
    tempstr line;
    line << "cd " << algo::strptr_ToBash(dir) << " && " << cmd;
    return algo::SysCmd(line, algo::FailokQ(true)) == 0;
}

// -----------------------------------------------------------------------------

// Return the start of a git command line that authenticates with the token in
// APM_TOKEN.  The token reaches git through a credential helper that reads it
// from the environment, so it never appears on a command line, in a URL, in the
// clone's config or in a log.
static tempstr GitAuth() {
    return tempstr() << "git -c credential.helper= -c "
                     << algo::strptr_ToBash("credential.helper=!f() { test \"$1\" = get && echo username=apm && echo \"password=$APM_TOKEN\"; }; f");
}

// -----------------------------------------------------------------------------

// Put the token of destination PKGUPSTREAM in the environment as APM_TOKEN, and
// return false when its cred names a credential credd does not give.  A
// destination with no cred authenticates the way git already does, with an ssh
// key from the agent, and needs nothing here.
static bool LoadToken(apm::FPkgupstream &pkgupstream) {
    bool ret = true;
    if (pkgupstream.cred != "") {
        algo::cstring value;
        algo::cstring err;
        ret = lib_cred::Fetch("apm", pkgupstream.cred, value, err);
        if (ret) {
            setenv("APM_TOKEN", Zeroterm(value), 1);
        } else {
            prerr("apm.publish_cred" << Keyval("cred", pkgupstream.cred) << Keyval("err", err)
                  << Keyval("comment", "credd gives no token for this destination; start credd or add the credential"));
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Run this tree's apm on PACKAGE with the action APM names already set, and return
// true when it exits 0.  DEST names the destination it acts on.
static bool RunApm(command::apm_proc &apm, apm::FPackage &package, algo::strptr dest) {
    apm.path = DirFileJoin(algo::GetCurDir(), DirFileJoin(apm::_db.cmdline.binpath, "apm"));
    apm.cmd.package.expr = package.package;
    apm.cmd.dest = dest;
    return apm_Exec(apm) == 0;
}

// -----------------------------------------------------------------------------

// TRUE when the last report.abt line of build log LOG says n_err:0.  ai exits 0
// after a failed build, so abt's own report is the verdict.
static bool BuildOkQ(algo::strptr log) {
    bool ret = false;
    ind_beg(algo::Line_curs, line, log) {
        if (StartsWithQ(line, "report.abt")) {
            ret = FindStr(line, "n_err:0 ") != -1 || EndsWithQ(line, "n_err:0");
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Print what the commit in clone DIR would publish: the size of the tree, the
// files added, changed and deleted against the origin's head, per top directory,
// and every added file by name, since a new file is the one most likely to be a
// surprise.  Return the number of changed files.
static int ShowDelta(algo::strptr dir) {
    (void)algo::SysCmd(tempstr() << "git -C " << algo::strptr_ToBash(dir) << " add -A", algo::FailokQ(false));
    tempstr status(algo::SysEval(tempstr() << "git -C " << algo::strptr_ToBash(dir) << " diff --cached --name-status --no-renames", algo::FailokQ(false), 1024*1024*64));
    tempstr tree(algo::SysEval(tempstr() << "git -C " << algo::strptr_ToBash(dir) << " ls-files", algo::FailokQ(false), 1024*1024*64));
    int ntree = 0;
    ind_beg(algo::Line_curs, line, tree) if (ch_N(line) > 0) {
        ntree++;
    }ind_end;
    apm::delta_RemoveAll();
    int nchange = 0;
    ind_beg(algo::Line_curs, line, status) if (ch_N(line) > 2) {
        algo::strptr path = Pathcomp(line, "\tLR");
        algo::strptr top = FindChar(path, '/') == -1 ? algo::strptr(".") : Pathcomp(path, "/LL");
        apm::FDelta &delta = apm::ind_delta_GetOrCreate(top);
        delta.nadd += line[0] == 'A';
        delta.nmod += line[0] == 'M';
        delta.ndel += line[0] == 'D';
        nchange++;
    }ind_end;
    prlog("apm.publish_tree  files:" << ntree << "  delta:" << nchange);
    ind_beg(apm::_db_delta_curs, delta, apm::_db) {
        prlog("apm.publish_delta" << Keyval("dir", delta.delta) << Keyval("added", delta.nadd)
              << Keyval("changed", delta.nmod) << Keyval("deleted", delta.ndel));
    }ind_end;
    ind_beg(algo::Line_curs, line, status) if (StartsWithQ(line, "A\t")) {
        prlog("apm.publish_new" << Keyval("file", Pathcomp(line, "\tLR")));
    }ind_end;
    apm::delta_RemoveAll();
    return nchange;
}

// -----------------------------------------------------------------------------

// Publish the selected package to its destination, the dev.pkgupstream row -dest
// names: produce it in a fresh clone of the destination's origin, regenerate,
// verify, show the maintainer what goes out, and push it after two yeses.
//
// Each stage prints a heading and stops the run when it fails, so the clone under
// temp/apm-publish is left as the stage found it for the maintainer to look at.
// -dry_run stops after the delta is shown.  The stages, in order:
//
// - check: apm -check over this tree, which refuses a word the package must not
//   carry, a dangling record, or proprietary content in an open-source package;
// - clone: the origin, fresh, which must sit at the destination's baseref, since a
//   push replaces what the origin holds and anything newer there would be lost;
// - produce: apm -push of this tree's projection into the clone;
// - regenerate: amc and update-hdr in the clone, with this tree's binaries, since
//   a generated file holds the output over the database that wrote it;
// - build: ai in the clone, which builds the clone's own binaries;
// - docs: the clone's abt_md, which regenerates and checks every document;
// - secrets: the clone's secret badlines over every file of the clone;
// - comptests: the clone's atf_comp, which is what says the package works;
// - show: the size of the tree and the delta against the origin's head;
// - commit and push, each after a yes, with the destination's credd token;
// - reset: this tree records the pushed commit as the destination's baseref.
void apm::Main_Publish() {
    apm::FPackage *package = NULL;
    ind_beg(_db_zd_sel_package_curs, sel, _db) if (!package && Regx_Match(_db.cmdline.package, sel.package)) {
        package = &sel;
    }ind_end;
    vrfy(package, "apm.publish  comment:'select one package'");
    apm::FPkgupstream *pkgupstream = GetPkgupstream(*package);
    vrfy(pkgupstream, tempstr() << "apm.publish_nodest" << Keyval("package", package->package)
         << Keyval("comment", "the package has no destination; -reset -dest:<name> -origin:<url> creates one"));
    tempstr dest(dest_Get(*pkgupstream));
    tempstr dir(DirFileJoin(algo::GetCurDir(), tempstr() << "temp/apm-publish/" << pkgupstream->pkgupstream));
    prlog("apm.publish" << Keyval("package", package->package) << Keyval("dest", dest)
          << Keyval("origin", pkgupstream->origin) << Keyval("baseref", pkgupstream->baseref) << Keyval("clone", dir));
    bool ok = true;
    if (ok) {
        PublishStage("check");
        command::apm_proc apm;
        apm.cmd.check = true;
        ok = RunApm(apm, *package, dest);
    }
    if (ok) {
        PublishStage("clone");
        ok = LoadToken(*pkgupstream);
    }
    if (ok) {
        tempstr cmd;
        cmd << "rm -rf " << algo::strptr_ToBash(dir) << " && mkdir -p " << algo::strptr_ToBash(GetDirName(dir))
            << " && " << GitAuth() << " clone -q " << algo::strptr_ToBash(pkgupstream->origin) << " " << algo::strptr_ToBash(dir);
        ok = algo::SysCmd(cmd, algo::FailokQ(true)) == 0;
    }
    if (ok) {
        tempstr head(Trimmed(algo::SysEval(tempstr() << "git -C " << algo::strptr_ToBash(dir) << " rev-parse HEAD", algo::FailokQ(true), 1024)));
        ok = pkgupstream->baseref == "" || pkgupstream->baseref == head;
        if (!ok) {
            prerr("apm.origin_moved" << Keyval("baseref", pkgupstream->baseref) << Keyval("head", head)
                  << Keyval("comment", "the origin is not at the last sync; run apm -update first"));
        }
    }
    if (ok) {
        PublishStage("produce");
        command::apm_proc apm;
        apm.cmd.push = true;
        apm.cmd.l = true;
        apm.cmd.origin = dir;
        ok = RunApm(apm, *package, dest);
    }
    if (ok) {
        PublishStage("regenerate");
        // The clone has built nothing, and its bin/ entries point into its own
        // build/release.  amc and update-hdr have to run before anything builds,
        // since the generated code the push copied describes this tree's
        // database, and they call each other through bin/ (amc runs bin/acr,
        // update-hdr runs bin/src_func).  So for this stage the clone's
        // build/release is a link to this tree's, and the stage takes it down
        // again.  Every later stage runs the clone's own binaries, whose compiled
        // tables are the package's and not this tree's.
        ok = RunIn(dir, tempstr() << "mkdir -p build && rm -rf build/release && ln -s "
                   << algo::strptr_ToBash(DirFileJoin(algo::GetCurDir(), "build/release")) << " build/release");
        // acr rewrites every ssimfile in canonical form, the way the
        // destination's normalize_acr citest does.  The push leaves a
        // repo-local table's rows as the origin wrote them, and those predate
        // any field the schema has gained since.
        ok = ok && RunIn(dir, "bin/acr -query:% -write:Y -report:N -print:N");
        ok = ok && RunIn(dir, "bin/amc -report:N");
        ok = ok && RunIn(dir, "bin/update-hdr");
        (void)RunIn(dir, "rm -f build/release");
    }
    if (ok) {
        PublishStage("build");
        ok = RunIn(dir, "mkdir -p temp && bin/ai > temp/apm-publish.ai.log 2>&1; true");
        ok = ok && BuildOkQ(algo::FileToString(DirFileJoin(dir, "temp/apm-publish.ai.log")));
        if (!ok) {
            prerr("apm.publish_build" << Keyval("log", DirFileJoin(dir, "temp/apm-publish.ai.log"))
                  << Keyval("comment", "the package does not build in the clone"));
        }
    }
    if (ok) {
        PublishStage("docs");
        ok = RunIn(dir, "bin/abt_md");
    }
    if (ok) {
        PublishStage("secrets");
        ok = RunIn(dir, "bin/src_lim -badline:'secret_%'");
    }
    int nchange = 0;
    if (ok) {
        PublishStage("show");
        nchange = ShowDelta(dir);
        prlog("apm.publish_diff" << Keyval("cmd", tempstr() << "git -C " << dir << " diff HEAD~1"));
    }
    // The candidate is committed in the clone before the gate runs, because the
    // destination's CI checks a committed tree and refuses a dirty one.  The
    // commit stays local; only the push below sends anything.
    if (ok && nchange > 0) {
        tempstr msg;
        msg << "Sync " << package->package << " from " << RevParseMaybe("HEAD");
        ok = RunIn(dir, tempstr() << "git commit -q -m " << algo::strptr_ToBash(msg));
    }
    // The gate is the command the destination's own CI runs, so a push that
    // passes here passes there on the same platform.  The log goes beside the
    // clone, since the gate compares the tree with its commit.
    if (ok && nchange > 0) {
        PublishStage("ci");
        tempstr log(tempstr() << dir << ".ci.log");
        ok = RunIn(dir, tempstr() << "bin/normalize > " << algo::strptr_ToBash(log) << " 2>&1"
                   << " && bin/atf_ci -cijob:comp >> " << algo::strptr_ToBash(log) << " 2>&1");
        if (!ok) {
            prerr("apm.publish_ci" << Keyval("log", log)
                  << Keyval("comment", "the destination's CI fails on the clone"));
        }
    }
    bool go = ok && !_db.cmdline.dry_run && nchange > 0;
    tempstr pushed;
    if (go) {
        PublishStage("push");
        pushed = tempstr(Trimmed(algo::SysEval(tempstr() << "git -C " << algo::strptr_ToBash(dir) << " rev-parse HEAD", algo::FailokQ(true), 1024)));
        tempstr branch(Trimmed(algo::SysEval(tempstr() << "git -C " << algo::strptr_ToBash(dir) << " rev-parse --abbrev-ref HEAD", algo::FailokQ(true), 1024)));
        go = AskYes(tempstr() << "Push " << pushed << " to " << pkgupstream->origin << " branch " << branch << "?");
        if (go) {
            go = RunIn(dir, tempstr() << GitAuth() << " push -q origin HEAD:" << algo::strptr_ToBash(branch));
            ok = go;
        }
    }
    if (go) {
        PublishStage("reset");
        command::apm_proc apm;
        apm.cmd.reset = true;
        apm.cmd.ref = pushed;
        apm.cmd.origin = pkgupstream->origin;
        ok = RunApm(apm, *package, dest);
        prlog("apm.publish_done" << Keyval("commit", pushed)
              << Keyval("comment", "commit the dev.pkgupstream row this tree now carries"));
    }
    unsetenv("APM_TOKEN");
    if (!ok) {
        algo_lib::_db.exit_code = 1;
    }
}
