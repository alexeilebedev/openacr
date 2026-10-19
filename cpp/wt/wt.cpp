// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Target: wt (exe) -- Worktree manager - reset, run, diff, delete
// Exceptions: yes
// Source: cpp/wt/wt.cpp
//
// wt manages named, resettable copies of the checkout under wt/. A sandbox
// (dev.sandbox registry) is materialized as a git worktree, or, when its
// row says cow:Y and the host has cowdancer, as a copy-on-write farm: a
// hardlink copy of the checkout whose in-place writes are deflected by
// libcowdancer. A host without cowdancer materializes every sandbox as a
// worktree. A branch worktree (-b) is a durable checkout of its own
// branch, registered only in git, and under -b the name selects among the
// branch worktrees that git lists under wt/. -merged narrows that selection
// to the worktrees whose branch has merged into the upstream branch and which
// hold nothing else, so -del can reclaim them.  The upstream branch is master@{u},
// the branch that the local master tracks, origin/master in most clones.
// Every operation executes its commands directly, in order; -verbose
// echoes each command as it runs. The exit code of a command executed
// inside a sandbox becomes wt's exit code.

#include "include/algo.h"
#include "include/wt.h"
#include "include/lib_git.h"
#include "include/lib_json.h"

// -----------------------------------------------------------------------------

// Execute a shell command; return true if it exited 0
static bool Run(strptr cmd) {
    return algo::SysCmd(cmd, FailokQ(true)) == 0;
}

// -----------------------------------------------------------------------------

// Return the SHA that -ref resolves to
static tempstr RefSha() {
    return tempstr() << Trimmed(algo::SysEval(tempstr()<<"git rev-parse "<<wt::_db.cmdline.ref, FailokQ(false)));
}

// -----------------------------------------------------------------------------

// Return the path of the libcowdancer preload library (COWDANCER_SO overrides)
static tempstr FarmSo() {
    const char *env = getenv(algo_lib::dev_envvar_COWDANCER_SO);
    return tempstr() << (env ? strptr(env) : strptr("/usr/lib/cowdancer/libcowdancer.so"));
}

// -----------------------------------------------------------------------------

// Create the parent directory of DIR (with .ffskip so ff skips it)
static bool AddParentDir(strptr dir) {
    tempstr parent(Pathcomp(dir,"/RL"));
    bool ok = Run(tempstr()<<"mkdir -p "<<algo::strptr_ToBash(parent));
    ok = ok && Run(tempstr()<<"touch "<<algo::strptr_ToBash(tempstr()<<parent<<"/.ffskip"));
    return ok;
}

// -----------------------------------------------------------------------------

// Create the sandbox worktree at DIR, detached at -ref
static bool AddSandbox(strptr dir) {
    bool ok = AddParentDir(dir);
    ok = ok && Run(tempstr()<<"git worktree add -q --detach "<<algo::strptr_ToBash(dir)<<" "<<RefSha());
    return ok;
}

// -----------------------------------------------------------------------------

// Bring the existing sandbox worktree at DIR to a clean checkout of -ref;
// false if DIR is not a usable worktree (caller then re-creates it)
static bool SyncSandbox(strptr dir) {
    tempstr dirq(algo::strptr_ToBash(dir));
    bool ok = FileQ(tempstr()<<dir<<"/.git");
    ok = ok && Run(tempstr()<<"git -C "<<dirq<<" reset --hard -q");
    ok = ok && Run(tempstr()<<"git -C "<<dirq<<" clean --exclude=build --exclude=.gcache -fxq .");
    ok = ok && Run(tempstr()<<"git -C "<<dirq<<" checkout -q --detach "<<RefSha());
    return ok;
}

// -----------------------------------------------------------------------------

// Reset SANDBOX's worktree to match the current directory: clean checkout of
// -ref, then copy in modified files and the dev.sbpath extras (build/, cache
// link), and commit the result as the baseline git diff compares against
static bool ResetSandbox(wt::FSandbox &sandbox) {
    if (!wt::_db.cmdline.q) {
        prlog("wt.reset"<<Keyval("sandbox",sandbox.sandbox)<<Keyval("dir",sandbox.dir));
    }
    bool ok = SyncSandbox(sandbox.dir);
    // A sandbox that cannot be synced is made again.  Its registration may be
    // locked -- git locks a worktree while adding it, and a job killed in that
    // window leaves the lock with no directory -- and prune keeps a locked
    // worktree, so the add that follows would refuse.  The directory is
    // wt's own, so its lock is released before the prune.
    if (!ok) {
        ok = Run(tempstr()<<"rm -rf "<<algo::strptr_ToBash(sandbox.dir));
        ok = ok && Run(tempstr()<<"git worktree unlock "<<algo::strptr_ToBash(sandbox.dir)<<" 2>/dev/null || true");
        ok = ok && Run("git worktree prune");
        ok = ok && AddSandbox(sandbox.dir);
    }
    tempstr rsync;
    rsync << "rsync --relative --ignore-missing-args -ac $(git ls-files -m)";
    ind_beg(wt::_db_sbpath_curs,sbpath,wt::_db) {
        rsync << " " << sbpath.sbpath;
    }ind_end;
    rsync << " " << algo::strptr_ToBash(tempstr()<<sandbox.dir<<"/");
    ok = ok && Run(rsync);
    ok = ok && Run(tempstr()<<"git -C "<<algo::strptr_ToBash(sandbox.dir)<<" commit --allow-empty -m baseline -q 2>/dev/null");
    return ok;
}

// -----------------------------------------------------------------------------

// Reset SANDBOX's cow farm: a copy of the current directory made of
// hardlinks (rsync --link-dest); in-place writes are deflected by
// libcowdancer at run time. wt/ is excluded (it contains the farm itself),
// .git is replaced by a fresh private repo so staged files never reach the
// real one, temp/ starts empty. .ilist, the list of inodes libcowdancer
// protects, is created here: the set of protected inodes changes only on
// reset. The caller guarantees cowdancer is present.
static bool ResetCow(wt::FSandbox &sandbox) {
    tempstr dirq(algo::strptr_ToBash(sandbox.dir));
    if (!wt::_db.cmdline.q) {
        prlog("wt.reset"<<Keyval("sandbox",sandbox.sandbox)<<Keyval("dir",sandbox.dir)<<Keyval("cow","Y"));
    }
    bool ok = AddParentDir(sandbox.dir);
    tempstr rsync;
    rsync << "rsync -a --delete --link-dest=\"$PWD\"";
    rsync << " --exclude /wt --exclude /.git --exclude /temp ./ " << algo::strptr_ToBash(tempstr()<<sandbox.dir<<"/");
    ok = ok && Run(rsync);
    ok = ok && Run(tempstr()<<"rm -rf "<<dirq<<"/temp "<<dirq<<"/.git "<<dirq<<"/.ilist");
    ok = ok && Run(tempstr()<<"mkdir -p "<<dirq<<"/temp");
    ok = ok && Run(tempstr()<<"git -C "<<dirq<<" init -q");
    tempstr ilist;
    ilist << "cd " << dirq;
    ilist << " && cowdancer-ilistcreate .ilist " << algo::strptr_ToBash("find . -xdev \\( -type l -o -type f \\) -a -links +1 -print0 | xargs -0 stat --format '%d %i '");
    ok = ok && Run(ilist);
    return ok;
}

// -----------------------------------------------------------------------------

// Reset SANDBOX using its registered materialization. cow:Y asks for a
// farm, which requires cowdancer: the ilist tool at reset, the preload
// library at run time. A host without either materializes the sandbox as
// a worktree instead -- same name, same dir, only a slower reset.
static bool Reset(wt::FSandbox &sandbox) {
    bool farm = sandbox.cow;
    farm = farm && Run("command -v cowdancer-ilistcreate >/dev/null");
    farm = farm && FileQ(FarmSo());
    return farm ? ResetCow(sandbox) : ResetSandbox(sandbox);
}

// -----------------------------------------------------------------------------

// Seed DIR's build directories: local empty build/<cfg> with abt, gcache,
// llmtool and wt linked from the main checkout, and the compiler cache
// enabled, so the first build inside the worktree is cache-served.  The
// llmtool link gives the session wt starts in DIR a status line from its
// first refresh.  The status line runs bin/llmtool -status, which prints only
// what the json Claude Code writes to its stdin says; the binary also loads
// its own checkout's price list, and -status prints nothing from it.  The wt
// link serves Claude Code's worktree hooks, which run bin/wt of the directory
// a session was launched in, so a session launched in a fresh worktree can
// make a worktree before anything is built there.  A build inside DIR
// replaces a link with a file of its own and leaves the main checkout's
// binary alone.  The cppcheck build dirs under temp/ are copied too, so the
// first normalize re-analyzes only what the branch changes.  A copy, since
// cppcheck writes into its build dir on every run and two worktrees may run
// it at once.
static bool WireBuild(strptr dir) {
    tempstr script;
    script << "dir=" << algo::strptr_ToBash(dir) << "\n";
    script << R"BASH(for cc in temp/cppcheck.*; do
  [ -d "$cc" ] || continue
  mkdir -p "$dir/temp"
  cp -r "$cc" "$dir/temp/"
done
for cfg in release coverage debug profile; do
  link=$(readlink "build/$cfg" 2>/dev/null) || continue
  name=$(basename "$link")
  [ -d "build/$name" ] || continue
  mkdir -p "$dir/build/$name"
  for tool in abt gcache llmtool wt; do
    [ -x "build/$name/$tool" ] && ln -sfn "$(pwd)/build/$name/$tool" "$dir/build/$name/$tool"
  done
  ln -sfn "$name" "$dir/build/$cfg"
done
cachedir="${GCACHE_DIR:-/tmp/gcache}"
if [ -d "$cachedir" ]; then
  (cd "$dir" && bin/gcache -enable -dir "$cachedir" >/dev/null 2>&1) || true
else  # ignore:hanging_else
  (cd "$dir" && bin/gcache -install -dir "$cachedir" >/dev/null 2>&1) || true
fi)BASH";
    return Run(script);
}

// -----------------------------------------------------------------------------

// Give DIR a link to each dataset the checkout it was cut from has attached
// under run/, so the worktree reads the same rows.  Answers true whether or not
// there was one to reproduce: a checkout with nothing attached leaves the
// worktree with nothing, and that is the state the tools already explain.
//
// The problem, by example.  A tool that reads a dataset layer reads it from a
// directory under run/, and run/ is gitignored, so the checkout carries the
// attachment and a fresh worktree does not.  Every query of that layer in the
// worktree then answers with no rows, and the first deploy refuses as
// unattached -- after the build, at the moment the work is ready to ship, and
// for want of one command the worktree could have run for itself, since the
// checkout it was cut from already knows where the dataset is.
//
// The worktree gets a symlink to each entry of the parent's run/, whichever
// shape the attachment has: a symlink to a checkout the user keeps, or a clone
// of a git url.  Both leave a directory of rows, so `readlink -f` resolves
// either shape to it and one symlink serves both.  Resolved rather than copied,
// since a relative target would name a different directory two levels down.
// run/ holds attachments and nothing else, so the walk names no dataset and a
// second one needs no change here.  An entry the worktree already holds is
// left as git checked it out: in a repository that tracks a run/ of its own,
// the branch's files are what the worktree is for.
static bool WireRun(strptr dir) {
    tempstr script;
    script << "dir=" << algo::strptr_ToBash(dir) << "\n";
    script << R"BASH(for entry in run/*; do
  target=$(readlink -f "$entry")
  if [ -n "$target" ] && [ -e "$target" ] && [ ! -e "$dir/run/$(basename "$entry")" ]; then
    mkdir -p "$dir/run"
    ln -sfn "$target" "$dir/run/$(basename "$entry")"
  fi
done)BASH";
    return Run(script);
}

// -----------------------------------------------------------------------------

// The relative path from the branch worktree named NAME up to the main
// checkout: one step for wt/, one for the worktree's own directory, and one
// per slash in the name, so that a nested name such as team/x climbs out of
// team/ too.
static tempstr UpToCheckout(strptr name) {
    tempstr ret;
    ret << "../../";
    frep_(i, ch_N(name)) {
        if (name[i] == '/') {
            ret << "../";
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the main checkout's link to the branch file of SANDBOX,
// `.branch/<name>.md`.
static tempstr GetBranchLink(wt::FSandbox &sandbox) {
    return tempstr() << ".branch/" << sandbox.sandbox << ".md";
}

// Keep SANDBOX's branch file inside its worktree, as
// `<dir>/.branch/<name>.md`, and link the main checkout's `.branch/<name>.md`
// to it, and return false when a command failed.  A checkout with no `.branch`
// gets nothing.  A branch file the main checkout already holds moves into the
// worktree, so a worktree made before this layout takes it on its next
// `-create` or `-claudesess`.
//
// The main checkout's link turns back into a file whenever something writes
// through it by rename, as `sed -i` and most editors do, and the worktree
// still holds the file it had.  Two files with different text are refused and
// both kept, since either may hold the edit that the other lacks; two with the
// same text leave the worktree's file and the link.
//
// Claude Code isolates a session it puts in a worktree, and refuses its Edit
// and Write at any path that resolves outside the worktree.  A branch file
// reached through a link out of the worktree is such a path, so every isolated
// session failed its first write of the file and fell back to the shell.  A
// file inside the worktree is written in place, the main checkout still reads
// it by its old path, and it goes with the worktree, as the file of a branch
// should.  The link is absolute, since the main checkout's `.branch` may itself
// be a link to another directory.
static bool PlaceBranchFile(wt::FSandbox &sandbox) {
    bool ok = true;
    tempstr link(GetBranchLink(sandbox));
    tempstr inner(algo::DirFileJoin(sandbox.dir, link));
    bool twofile = algo::FileQ(link) && algo::FileQ(inner)
        && algo::FileToString(link, algo::FileFlags()) != algo::FileToString(inner, algo::FileFlags());
    if (!algo::DirectoryQ(".branch")) {
        // the checkout keeps no branch files
    } else if (twofile) {
        prerr("wt.branchfile"
              <<Keyval("main",link)
              <<Keyval("worktree",inner)
              <<Keyval("comment","two branch files differ; merge them into the worktree's, remove the main checkout's, and rerun"));
        ok = false;
    } else {
        tempstr dirq(algo::strptr_ToBash(sandbox.dir));
        tempstr innerq(algo::strptr_ToBash(inner));
        tempstr linkq(algo::strptr_ToBash(link));
        tempstr target(algo::DirFileJoin(algo::GetCurDir(), inner));
        tempstr script;
        script << "{ [ ! -L " << dirq << "/.branch ] || rm -f " << dirq << "/.branch; }"
               << " && mkdir -p \"$(dirname " << innerq << ")\" \"$(dirname " << linkq << ")\""
               << " && { [ -L " << linkq << " ] || [ ! -f " << linkq << " ] || [ -e " << innerq << " ] || mv " << linkq << " " << innerq << "; }"
               << " && { [ -L " << linkq << " ] || [ ! -e " << linkq << " ] || rm -f " << linkq << "; }"
               << " && ln -sfn " << algo::strptr_ToBash(target) << " " << linkq;
        ok = Run(script);
    }
    return ok;
}

// Remove the main checkout's link to SANDBOX's branch file, when it is a link,
// and return false when the remove failed.  The file itself sits in the
// worktree and goes with it.
static bool DropBranchLink(wt::FSandbox &sandbox) {
    tempstr linkq(algo::strptr_ToBash(GetBranchLink(sandbox)));
    return Run(tempstr() << "[ ! -L " << linkq << " ] || rm -f " << linkq);
}

// Create SANDBOX's branch worktree at DIR: new branch named after it, the
// branch file inside it (see PlaceBranchFile), scratch symlink to the
// main checkout's scratch, isolated cache-backed build, and the inventory the
// parent checkout reads.
//
// scratch/ is the gitignored directory under the main checkout where a
// session keeps the files that outlive its worktree: design notes, logs, a
// fetched copy of something.  A worktree of its own would start with an empty
// one, so every worktree links the one directory and a file left there is
// read from any of them.
static bool AddBranch(wt::FSandbox &sandbox) {
    if (!wt::_db.cmdline.q) {
        prlog("wt.branch"<<Keyval("branch",sandbox.sandbox)<<Keyval("dir",sandbox.dir));
    }
    tempstr dirq(algo::strptr_ToBash(sandbox.dir));
    tempstr nameq(algo::strptr_ToBash(sandbox.sandbox));
    tempstr up(UpToCheckout(sandbox.sandbox));
    bool ok = AddParentDir(sandbox.dir);
    ok = ok && Run(tempstr()<<"git worktree add -q "<<dirq<<" -b "<<nameq<<" "<<RefSha()<<" 2>/dev/null"
                   <<" || git worktree add -q "<<dirq<<" "<<nameq);
    ok = ok && PlaceBranchFile(sandbox);
    ok = ok && Run(tempstr()<<"mkdir -p scratch && [ -e "<<dirq<<"/scratch ] || ln -sfn "<<up<<"scratch "<<dirq<<"/scratch");
    ok = ok && WireBuild(sandbox.dir);
    ok = ok && WireRun(sandbox.dir);
    return ok;
}

// -----------------------------------------------------------------------------

// Spawn a background claude session inside SANDBOX's worktree, named after
// it; the session boots by reading the agent instructions and the branch
// control file, then stands by for instructions (attach with
// claude --resume <name>).  The branch file sits inside the worktree (see
// PlaceBranchFile), so the session reads and writes it with no grant.
static bool StartClaudeSess(wt::FSandbox &sandbox) {
    if (!wt::_db.cmdline.q) {
        prlog("wt.claudesess"<<Keyval("session",sandbox.sandbox)<<Keyval("dir",sandbox.dir));
    }
    tempstr prompt;
    prompt << "Read AGENTS.md and .branch/" << sandbox.sandbox << ".md and stand by.";
    tempstr script;
    script << "cd " << algo::strptr_ToBash(sandbox.dir);
    script << " && claude --bg " << algo::strptr_ToBash(prompt);
    script << " --name " << algo::strptr_ToBash(sandbox.sandbox);
    return Run(script);
}

// -----------------------------------------------------------------------------

// Run the cmdline's command inside SANDBOX and return its exit code.
// A single cmd element is a shell command line and runs via bash -c;
// multiple elements form the argv of the command directly.
// In a hardlink farm (marked by .ilist) the command runs with libcowdancer
// preloaded; without the library such a farm is refused, since an in-place
// write would reach the checkout through the links.
static int ExecCmd(wt::FSandbox &sandbox) {
    int rc = 0;
    bool ilist = FileQ(tempstr()<<sandbox.dir<<"/.ilist");
    tempstr so(FarmSo());
    if (ilist && !FileQ(so)) {
        prerr("wt.nocowdancer"
              <<Keyval("dir",sandbox.dir)
              <<Keyval("comment","farm is a hardlink copy but cowdancer is missing; -reset rebuilds the sandbox as a worktree"));
        rc = 1;
    } else {
        tempstr script;
        script << "cd " << algo::strptr_ToBash(sandbox.dir);
        script << " &&";
        if (ilist) {
            script << " LD_PRELOAD=" << algo::strptr_ToBash(so) << " COWDANCER_ILISTFILE=\"$PWD/.ilist\"";
        }
        if (cmd_N(wt::_db.cmdline) == 1) {
            script << " bash -c " << algo::strptr_ToBash(cmd_qFind(wt::_db.cmdline,0));
        } else {
            ind_beg(command::wt_cmd_curs,cmd,wt::_db.cmdline) {
                script << " " << algo::strptr_ToBash(cmd);
            }ind_end;
        }
        int status = algo::SysCmd(script, FailokQ(true));
        rc = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    return rc;
}

// -----------------------------------------------------------------------------

// Return the disk usage of DIR in du's human-readable form, or N/A when DIR
// does not exist
static tempstr GetSize(strptr dir) {
    tempstr ret("N/A");
    if (DirectoryQ(dir)) {
        ret = Trimmed(algo::SysEval(tempstr()<<"du -sh "<<algo::strptr_ToBash(dir)<<" 2>/dev/null | awk '{print $1}'", FailokQ(true)));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Print selected sandboxes: name, materialization, size, cleanliness, path
static void ListSandbox() {
    prlog("Sandbox\tCow\tSize\tClean\tPath\tComment");
    ind_beg(wt::_db_sandbox_curs,sandbox,wt::_db) if (sandbox.select) {
        tempstr clean("Y");
        if (DirectoryQ(sandbox.dir)) {
            clean = Run(tempstr()<<"git -C "<<algo::strptr_ToBash(sandbox.dir)<<" diff --quiet 2>/dev/null") ? "Y" : "N";
        }
        prlog(sandbox.sandbox<<"\t"<<(sandbox.cow ? "Y" : "N")<<"\t"<<GetSize(sandbox.dir)<<"\t"<<clean<<"\t"<<sandbox.dir<<"\t"<<sandbox.comment);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print selected branch worktrees: name, size, whether the branch merged, path.
// Then name each process that keeps a merged worktree busy.  A session working
// in the worktree and a test process left running for days both read busy, and
// the reader tells them apart by the parent pid and the age: a process whose
// parent is pid 1 lost the shell that started it.  wt kills none of them.
static void ListBranch() {
    prlog("Branch\tSize\tMerged\tPath");
    ind_beg(wt::_db_sandbox_curs,sandbox,wt::_db) if (sandbox.select) {
        prlog(sandbox.sandbox<<"\t"<<GetSize(sandbox.dir)<<"\t"<<sandbox.merged<<"\t"<<sandbox.dir);
    }ind_end;
    ind_beg(wt::_db_sandbox_curs,sandbox,wt::_db) if (sandbox.select && sandbox.merged == "busy") {
        ind_beg(algo::Line_curs, process, sandbox.holder) {
            prlog("wt.busy"<<Keyval("worktree",sandbox.sandbox)<<"  "<<process);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Return the names of the selected sandboxes, separated by commas
static tempstr GetSelectName() {
    tempstr ret;
    algo::ListSep sep(",");
    ind_beg(wt::_db_sandbox_curs,sandbox,wt::_db) if (sandbox.select) {
        ret << sep << sandbox.sandbox;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Read the branch worktrees under wt/ from git into the sandbox table.  A
// worktree gets a row when no dev.sandbox row holds its name, and the row is
// named after the worktree's path below wt/.  Its HEAD may be on a branch or
// detached: git lists a worktree in the middle of a conflicted rebase, or one
// where somebody checked out a commit, as detached, and -del -b and -create -b
// still have to find it there.  A sandbox made without a dev.sandbox row is a
// detached worktree too, and wt makes no branch for one; so a detached
// worktree is a branch worktree only while the branch of its name exists.
static void LoadBranch() {
    tempstr top(Trimmed(algo::SysEval("git rev-parse --show-toplevel", FailokQ(true))));
    tempstr prefix(tempstr()<<top<<"/wt/");
    tempstr list(algo::SysEval("git worktree list --porcelain", FailokQ(true)));
    cstring path;
    ind_beg(algo::Line_curs, line, list) {
        if (StartsWithQ(line, "worktree ")) {
            path = Pathcomp(line, " LR");
        } else if ((StartsWithQ(line, "branch ") || line == "detached") && StartsWithQ(path, prefix)) {
            tempstr name(ch_RestFrom(path, ch_N(prefix)));
            if (wt::ind_sandbox_Find(name)) {
                // a dev.sandbox row, or an earlier entry, holds the name
            } else if (line == "detached" && !lib_git::GitBranchExistsQ(name)) {
                // a detached worktree no branch names is an anonymous sandbox
            } else {
                wt::ind_sandbox_GetOrCreate(name).branch = true;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Return one line per process that works in the worktree at DIR: its pid,
// parent pid, age and command line, as the key-value pairs pid, ppid, age and
// cmd.  Return "" when no process works there or DIR does not exist.  A
// process works there when its current directory or its executable lies
// inside DIR, so removing DIR would take its files or its binary.  With
// SKIP_CALLER, wt and every process above it are left out, since they asked
// for the removal: Claude Code runs a remove hook in the session's current
// directory, which is the worktree the hook removes, so the hook's shell and
// the session itself sit inside it.  Without it they count: a session that
// lists or reclaims merged worktrees from inside its own worktree works there,
// and a removal would take its directory.  The scan runs from /, so that the
// commands it forks sit nowhere.  It reads each process's directory and
// executable from /proc where the system has one, and from lsof on macOS, which
// has none; the Linux runner image carries no lsof.
static tempstr GetHolder(strptr dir, bool skip_caller) {
    tempstr script;
    script << "dir=" << algo::strptr_ToBash(dir) << "\n";
    script << "skip=" << (skip_caller ? "Y" : "N") << "\n";
    script << R"BASH(abs=$(cd "$dir" 2>/dev/null && pwd -P) || exit 0
  cd /
  chain=" "
  p=$$
  while [ "$skip" = Y ] && [ -n "$p" ] && [ "$p" -gt 1 ]; do
    chain="$chain$p "
    p=$(ps -o ppid= -p "$p" 2>/dev/null | tr -d ' ')
  done
  holder=$(mktemp)
  if [ -d /proc/self ]; then
    find /proc -mindepth 2 -maxdepth 2 \( -name cwd -o -name exe \) -printf '%h\t%l\n' 2>/dev/null \
      | awk -F'\t' '{ print substr($1, 7) "\t" $2 }'
  else  # ignore:hanging_else
    lsof -nP -w -d cwd,txt -Fpn 2>/dev/null \
      | awk '/^p/ { pid = substr($0, 2) } /^n/ { print pid "\t" substr($0, 2) }'
  fi > "$holder"
  pid=$(awk -F'\t' -v d="$abs" -v chain="$chain" \
          '($2 == d || index($2, d "/") == 1) && index(chain, " " $1 " ") == 0 { print $1 }' "$holder" \
      | sort -un | paste -sd, -)
  rm -f "$holder"
  [ -z "$pid" ] || ps -o pid=,ppid=,etime=,args= -p "$pid" \
      | awk '{ p = $1; q = $2; e = $3; $1 = $2 = $3 = ""; sub(/^ +/, ""); print p "\t" q "\t" e "\t" $0 }')BASH";
    tempstr ps(algo::SysEval(script, FailokQ(true)));
    tempstr ret;
    ind_beg(algo::Line_curs, line, ps) {
        tempstr process;
        process << Keyval("pid",Pathcomp(line,"\tLL"))
                << Keyval("ppid",Pathcomp(line,"\tLR\tLL"))
                << Keyval("age",Pathcomp(line,"\tLR\tLR\tLL"))
                << Keyval("cmd",Pathcomp(line,"\tLR\tLR\tLR"));
        ret << process << eol;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return Y when removing the branch worktree SANDBOX loses nothing that git
// tracks, else the reason to keep it:
// - unmerged: its HEAD is not in the upstream branch.
// - unstarted: its tip is in the upstream branch, but the tip's footer names
//   another branch.
// - dirty: it holds an untracked file, or a change that the upstream branch
//   lacks.
// - busy: a process works in it, wt and its callers included.  SANDBOX's
//   holder then names each such process.
//
// A merged branch and a branch that never committed look alike to git.  Say
// wt -create -b cut branch B from master and nobody committed on it, or B was
// reset onto master later.  B's HEAD is then a master commit, so it is in
// the upstream branch exactly as a merged branch's HEAD is, and B is absent
// from the remote like a merged branch whose source branch was removed.  What tells
// them apart is the commit footer, closes #<iid> (<branch>): a branch merges
// fast-forward, so its tip is its own commit and the footer names it, while
// B's tip names the branch that committed it.  B may be waiting for work that
// has not started, so it stays.
//
// A worktree Claude Code made for a subagent, named agent-a<16 hex digits>,
// is the one unstarted worktree that waits for nobody: Claude Code keeps a
// hook-made agent worktree when the agent ends and runs no remove hook for
// it.  So its verdict skips the unstarted case and reads dirty, busy or Y
// like a merged branch's, and the sweep takes it once it holds nothing.
static tempstr GetMerged(wt::FSandbox &sandbox) {
    tempstr script;
    script << "dir=" << algo::strptr_ToBash(sandbox.dir) << "\n";
    script << "master=" << algo::strptr_ToBash(tempstr() << wt::_db.remote << "/master") << "\n";
    script << R"BASH(branch=$(git -C "$dir" symbolic-ref -q --short HEAD)
  footer=$(git -C "$dir" log -1 --format=%B | sed '/^$/d' | tail -1)
  if ! git -C "$dir" merge-base --is-ancestor HEAD "$master" 2>/dev/null; then
    echo unmerged
  elif [ "${footer%"($branch)"}" = "$footer" ] && ! expr "$branch" : 'agent-a[0-9a-f]\{16\}$' >/dev/null; then
    echo unstarted
  elif [ -n "$(git -C "$dir" ls-files --others --exclude-standard | head -1)" ]; then
    echo dirty
  elif ! git -C "$dir" diff --name-only -z HEAD | xargs -0 -r git -C "$dir" diff --quiet "$master" --; then
    echo dirty
  else  # ignore:hanging_else
    echo Y
  fi)BASH";
    tempstr ret(Trimmed(algo::SysEval(script, FailokQ(true))));
    sandbox.holder = ret == "Y" ? GetHolder(sandbox.dir, false) : tempstr();
    if (sandbox.holder != "") {
        ret = "busy";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the first line git status prints for the worktree at DIR, which
// names an edited, staged, deleted or untracked file, or "" when the worktree
// holds none.  A git error is returned too, so a worktree git cannot read
// counts as holding work.
static tempstr GetUncommitted(strptr dir) {
    tempstr cmd;
    cmd << "git -C " << algo::strptr_ToBash(dir) << " status --porcelain 2>&1 | head -1";
    return tempstr() << Trimmed(algo::SysEval(cmd, FailokQ(true)));
}

// -----------------------------------------------------------------------------

// Return the key-value pairs that say why a remove hook keeps the worktree at
// DIR, or "" when removing it loses nothing.  An uncommitted file is named
// first; a worktree that holds none is kept while a process works in it, and
// the first such process is named by its pid, ppid, age and cmd.
static tempstr GetKeep(strptr dir) {
    tempstr ret;
    tempstr uncommitted(GetUncommitted(dir));
    tempstr holder(uncommitted == "" ? GetHolder(dir, true) : tempstr());
    if (uncommitted != "") {
        algo::PrintAttrSpace(ret, "status", uncommitted);
        algo::PrintAttrSpace(ret, "comment", "the worktree holds uncommitted work; nothing removed");
    } else if (holder != "") {
        ret << "  " << Pathcomp(holder, "\nLL");
        algo::PrintAttrSpace(ret, "comment", "a process works in the worktree; nothing removed");
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return true when the command line asks for an action inside the sandbox,
// which makes the sandbox first when its directory does not exist
static bool EnterQ() {
    return wt::_db.cmdline.reset || wt::_db.cmdline.create || wt::_db.cmdline.clean
        || wt::_db.cmdline.diff || wt::_db.cmdline.pull || wt::_db.cmdline.claudesess
        || cmd_N(wt::_db.cmdline) > 0;
}

// -----------------------------------------------------------------------------

// Perform the requested actions on SANDBOX, in the documented order:
// -create (register), -reset (or reset on first use), -claudesess, -clean,
// execute command, -diff, -del. Returns false when an action fails; the
// command's exit code is recorded on algo_lib exit_code.
static bool DoSandbox(wt::FSandbox &sandbox) {
    bool ok = true;
    bool run = cmd_N(wt::_db.cmdline) > 0;
    bool enter = EnterQ();
    if (wt::_db.cmdline.create && !wt::_db.cmdline.b) {
        dev::Sandbox out;
        sandbox_CopyOut(sandbox, out);
        ok = Run(tempstr()<<"echo "<<algo::strptr_ToBash(tempstr()<<out)<<" | acr -insert -write");
    }
    if (ok && enter) {
        // Materialize: branch worktree (-b) is created once, never auto-reset;
        // a sandbox resets on -reset or on first use.  A branch worktree
        // exists when git registers it, which LoadBranch read into the row;
        // a directory at its path that git does not know is the parent of
        // a nested worktree, a sandbox or no checkout at all, so it is
        // refused before git runs: git would make the branch and then
        // refuse the directory, leaving the branch behind.
        if (wt::_db.cmdline.b) {
            if (!sandbox.branch && DirectoryQ(sandbox.dir)) {
                prerr("wt.notworktree"
                      <<Keyval("dir",sandbox.dir)
                      <<Keyval("comment","the directory is no branch worktree that git registers; nothing created"));
                ok = false;
            } else if (!sandbox.branch) {
                ok = AddBranch(sandbox);
            } else if (wt::_db.cmdline.create || wt::_db.cmdline.claudesess) {
                // a worktree made before the branch file moved inside it
                // takes the file on its next -create, and before a session
                // starts in it, since the session reads the file
                ok = PlaceBranchFile(sandbox);
            }
        } else if (wt::_db.cmdline.reset) {
            ok = Reset(sandbox);
        } else if (!DirectoryQ(sandbox.dir)) {
            ok = Reset(sandbox);
        }
    }
    if (ok && wt::_db.cmdline.claudesess) {
        ok = StartClaudeSess(sandbox);
    }
    if (ok && wt::_db.cmdline.clean) {
        if (sandbox.cow) {
            // a farm has no baseline to restore; remove the contents
            // (the next use rebuilds the farm)
            ok = Run(tempstr()<<"rm -rf "<<algo::strptr_ToBash(sandbox.dir));
        } else {
            tempstr dirq(algo::strptr_ToBash(sandbox.dir));
            ok = Run(tempstr()<<"git -C "<<dirq<<" reset --hard -q");
            ok = ok && Run(tempstr()<<"git -C "<<dirq<<" clean -df -q");
        }
    }
    if (ok && run) {
        int rc = ExecCmd(sandbox);
        algo_lib::_db.exit_code = i32_Max(algo_lib::_db.exit_code, rc);
        ok = rc == 0;
    }
    if (ok && wt::_db.cmdline.diff) {
        if (sandbox.cow) {
            // a farm has no baseline commit to diff against
            prerr("wt.diff"<<Keyval("comment","not supported for a cow sandbox"));
        } else {
            ok = Run(tempstr()<<"git -C "<<algo::strptr_ToBash(sandbox.dir)<<" diff");
        }
    }
    if (ok && wt::_db.cmdline.pull) {
        ok = Run(tempstr()<<"git pull --no-edit "<<algo::strptr_ToBash(sandbox.dir)<<" HEAD");
    }
    if (wt::_db.cmdline.del && wt::_db.cmdline.b) {
        ok = DropBranchLink(sandbox) && ok;
    }
    if (wt::_db.cmdline.del) {
        ok = Run(tempstr()<<"rm -rf "<<algo::strptr_ToBash(sandbox.dir)) && ok;
        ok = Run("git worktree prune") && ok;
        if (!wt::_db.cmdline.b) {
            dev::Sandbox out;
            sandbox_CopyOut(sandbox, out);
            ok = Run(tempstr()<<"echo "<<algo::strptr_ToBash(tempstr()<<"acr.delete "<<out)<<" | acr -insert -write") && ok;
        }
    }
    return ok;
}

// -----------------------------------------------------------------------------

// Turn -hook:create or -hook:remove into the command line it stands for, and
// move to the main checkout, where that command line runs.
//
// Claude Code runs wt as its WorktreeCreate and WorktreeRemove hook, so that a
// worktree Claude makes -- for `EnterWorktree name:<name>`, for a subagent with
// isolation:worktree, for `claude --worktree` -- is one wt made, with the
// branch file, the scratch symlink and the isolated build in place.  The protocol
// is Claude Code's: the event's JSON arrives on stdin, `name` for a create and
// `worktree_path` for a remove, and a create answers with the worktree's
// absolute path as the last line of stdout, which Main prints.  A create is
// `wt <name> -create -b -ref:<remote>/master` after a fetch, with the remote
// that the local master tracks, so the branch starts at the upstream branch; when the worktree exists it stays as it is and the
// answer is its path, so the same call returns a session to a worktree made
// earlier.  A remove is `wt <name> -del -b`, which keeps the branch as every
// -del -b does.  The session that asks may sit in a worktree, where wt
// refuses to nest one, so the hook moves to the main checkout first.
//
// The name is the one field Claude Code passes, so a branch that starts
// elsewhere than the upstream branch names its start inside it:
// `<name>--<ref>` makes wt/<name> on branch <name> at REF, a remote branch
// such as `origin/gli-issue-files-push`, a local branch or a sha.  The ref is
// what follows the first `--`, and a branch of that remote is fetched
// first, as the upstream branch is for a bare name.
//
// The hook's name is one worktree, never a pattern, so it is read literally:
// read as SQL, `_` matched any one character and a remove of wt/a_b took
// wt/axb with its uncommitted work.  A remove names its worktree by the path
// the create answered with, and acts on that worktree only: the name is the
// path below the main checkout's wt/, slashes included, and a path elsewhere
// names nothing and is refused.
//
// Claude Code also runs wt as its SessionStart hook, so that a session started
// in a branch worktree, by `claude -w <name>` or launched there, carries the
// worktree's name as its title, the title /rename sets and the resume picker
// shows.  The name is the top of the session's `cwd` below the main
// checkout's wt/.  Only a startup with no title gets one: a title given with
// --name or /rename stays, and a resumed or forked session keeps the title it
// had, since a fork titled like its parent would make two sessions of one
// name.  A session anywhere else names no worktree, selects nothing and gets
// no title.  The name a background session answers to in `claude agents`,
// which -claudesess gives with --name, is another thing, and the hook sets
// none.
static void ReadHook() {
    bool create = wt::_db.cmdline.hook == "create";
    bool remove = wt::_db.cmdline.hook == "remove";
    bool start = wt::_db.cmdline.hook == "sessionstart";
    vrfy(create || remove || start, tempstr()<<"wt.badhook"<<Keyval("hook",wt::_db.cmdline.hook)
         <<Keyval("comment","-hook takes create, remove or sessionstart"));
    tempstr json;
    ind_beg(algo::FileLine_curs, line, algo::Fildes(0)) {
        json << line << eol;
    }ind_end;
    lib_json::FParser parser;
    bool parsed = lib_json::JsonParse(parser, json) && parser.root_node;
    vrfy(parsed, tempstr()<<"wt.badhook"<<Keyval("hook",wt::_db.cmdline.hook)
         <<Keyval("comment","the hook's JSON on stdin did not parse"));
    tempstr checkout(lib_git::ReadCheckout());
    vrfy(checkout != "", tempstr()<<"wt.badhook"<<Keyval("hook",wt::_db.cmdline.hook)
         <<Keyval("comment","the current directory is not in a git checkout"));
    tempstr prefix(tempstr()<<checkout<<"/wt/");
    tempstr name;
    cstring prefix_remote(tempstr() << wt::_db.remote << "/");
    tempstr ref(tempstr() << prefix_remote << "master");
    if (create) {
        strptr full(lib_json::strptr_Get(parser.root_node, "name"));
        int at = FindStr(full, "--");
        name << (at >= 0 ? FirstN(full, at) : full);
        if (at >= 0) {
            ref = RestFrom(full, at + 2);
        }
        vrfy(at < 0 || ref != "", tempstr()<<"wt.badhook"<<Keyval("hook",wt::_db.cmdline.hook)
             <<Keyval("name",full)<<Keyval("comment","nothing follows the -- that names the ref"));
    } else if (remove) {
        strptr path(lib_json::strptr_Get(parser.root_node, "worktree_path"));
        vrfy(StartsWithQ(path, prefix) && ch_N(path) > ch_N(prefix), tempstr()<<"wt.badhook"
             <<Keyval("hook",wt::_db.cmdline.hook)<<Keyval("worktree_path",path)
             <<Keyval("comment","not a branch worktree under wt/ of the main checkout"));
        name << ch_RestFrom(path, ch_N(prefix));
    } else if (lib_json::strptr_Get(parser.root_node, "source") == "startup"
               && lib_json::strptr_Get(parser.root_node, "session_title") == "") {
        strptr cwd(lib_json::strptr_Get(parser.root_node, "cwd"));
        tempstr top(Trimmed(algo::SysEval(tempstr()<<"git -C "<<algo::strptr_ToBash(cwd)
                                          <<" rev-parse --show-toplevel 2>/dev/null", FailokQ(true))));
        if (StartsWithQ(top, prefix) && ch_N(top) > ch_N(prefix)) {
            name << ch_RestFrom(top, ch_N(prefix));
        }
    }
    vrfy(name != "" || start, tempstr()<<"wt.badhook"<<Keyval("hook",wt::_db.cmdline.hook)
         <<Keyval("comment","the hook's JSON names no worktree"));
    algo_lib::Regx_ReadLiteral(wt::_db.cmdline.name, name);
    wt::_db.cmdline.create = create;
    wt::_db.cmdline.del = remove;
    wt::_db.cmdline.b = true;
    wt::_db.cmdline.q = true;
    wt::_db.cmdline.ref = ref;
    algo_lib::PushDir(checkout);
    // A create names a worktree that may exist already, since a session
    // returns to a worktree by the same call that makes one; the worktree
    // then stays as it is, and the fetch that sets a new branch's start is
    // skipped.  A local branch or a sha is fetched from nowhere.
    bool exists = create && DirectoryQ(algo_lib::WtDir(name));
    if (create && !exists && StartsWithQ(ref, prefix_remote)) {
        // a hook has no terminal, so a fetch that would ask for a credential
        // fails at once in place of waiting out the hook's timeout
        Run(tempstr()<<"GIT_TERMINAL_PROMPT=0 git fetch -q "<<algo::strptr_ToBash(wt::_db.remote)
            <<" "<<algo::strptr_ToBash(ch_RestFrom(ref, ch_N(prefix_remote))));
    }
}

// -----------------------------------------------------------------------------

// Select sandboxes by the name regex and perform the requested actions on
// each; -list prints the surviving state afterwards.  Under -hook the
// selection and the action come from Claude Code's hook JSON, and a create
// ends by printing the worktree's path for Claude Code to enter.
void wt::Main() {
    _db.remote = lib_git::GetMasterRemote();
    if (_db.cmdline.hook != "") {
        ReadHook();
    }
    // The name is required, and under -hook it comes from the hook's JSON
    // rather than the command line, which the schema cannot say; so the
    // requirement is checked here, once both sources have been read.  A
    // session that starts outside a worktree names none, and selects nothing.
    vrfy(_db.cmdline.name.expr != "" || _db.cmdline.hook == "sessionstart", "wt: a sandbox name is required: wt <name> ... (see -help)");
    // The script to run comes from stdin only when -i asks for it.  Reading
    // stdin on a guess -- no command given, stdin not a terminal -- hung every
    // caller that forked wt for an action while holding a stdin that never
    // closes, abt_md resetting its doc sandbox among them; the read waited for
    // a script that was not coming, for as long as the caller's timeout allowed.
    if (_db.cmdline.i) {
        algo::ListSep sep("\n");
        tempstr script;
        ind_beg(algo::FileLine_curs, line, algo::Fildes(0)) {
            script << sep << line;
        }ind_end;
        if (ch_N(script)) {
            cmd_Alloc(_db.cmdline) = script;
        }
    }
    // A branch worktree and its claude session are provisioned from the
    // main checkout; from inside a worktree the wt/<name> path would nest
    // a checkout inside a checkout.  Sandboxes stay allowed -- a test
    // driver running inside a branch worktree resets them there by design.
    if ((_db.cmdline.create && _db.cmdline.b) || _db.cmdline.claudesess) {
        vrfy(!algo_lib::WorktreeQ(), "wt: refusing to nest a worktree inside a worktree; run from the main checkout");
    }
    if (_db.cmdline.create && !_db.cmdline.b) {
        _db.cmdline.reset=true;
    }
    if (_db.cmdline.merged) {
        _db.cmdline.b=true;
    }
    if (_db.cmdline.shell) {
        cmd_RemoveAll(_db.cmdline);
        cmd_Alloc(_db.cmdline)="bash";
        cmd_Alloc(_db.cmdline)="-l";
    }
    if (_db.cmdline.create && !_db.cmdline.b) {
        dev::Sandbox sandbox;
        sandbox.sandbox=_db.cmdline.name.expr;
        sandbox.cow=_db.cmdline.cow;
        sandbox_InsertMaybe(sandbox);
    }
    // Under -b the name selects among the branch worktrees git knows, and the
    // dev.sandbox rows stay out of the selection
    if (_db.cmdline.b) {
        LoadBranch();
    }
    // A verdict compares each worktree against the upstream branch, so it is
    // taken against a fresh one; a failed fetch leaves an older master, which
    // can only make fewer worktrees count as merged
    bool verdict = _db.cmdline.b && (_db.cmdline.merged || _db.cmdline.list);
    if (verdict) {
        Run(tempstr()<<"git fetch -q "<<algo::strptr_ToBash(_db.remote)<<" master");
    }
    int nsel=0;
    ind_beg(_db_sandbox_curs,sandbox,_db) {
        sandbox.dir = algo_lib::WtDir(sandbox.sandbox);
        bool match = Regx_Match(_db.cmdline.name,sandbox.sandbox) && sandbox.branch == _db.cmdline.b;
        if (match && verdict) {
            sandbox.merged = GetMerged(sandbox);
        }
        sandbox.select = match && (!_db.cmdline.merged || sandbox.merged == "Y");
        nsel += sandbox.select;
    }ind_end;
    // Anonymous sandbox: a name that matches no row names a sandbox of its
    // own for an action that enters it or removes it; -cow selects its
    // materialization.  No table registers an anonymous sandbox, so -list
    // shows one whose directory exists.  Under -b git registers every branch
    // worktree, and -list and -merged select among those, so neither names a
    // new one; nor does -del, since a name git registers no worktree at may
    // be the parent directory of a nested worktree, and removing it would
    // take every worktree inside.
    bool act = EnterQ() || (_db.cmdline.del && !_db.cmdline.b);
    bool show = _db.cmdline.list && !_db.cmdline.b && DirectoryQ(algo_lib::WtDir(_db.cmdline.name.expr));
    if (nsel == 0 && _db.cmdline.name.expr != "" && !_db.cmdline.merged && (act || show)) {
        auto& sandbox = ind_sandbox_GetOrCreate(_db.cmdline.name.expr);
        sandbox.select = true;
        sandbox.cow = _db.cmdline.cow;
        sandbox.dir = algo_lib::WtDir(sandbox.sandbox);
        if (!_db.cmdline.q && !_db.cmdline.b) {
            prlog("wt.anon"
                  <<Keyval("sandbox",sandbox.sandbox)
                  <<Keyval("dir",sandbox.dir));
        }
    }
    // A pattern such as % selects every branch worktree, and both -del and
    // -clean discard each one's uncommitted work.  So they act on one branch
    // worktree by name, and on several at once only when -merged has kept the
    // ones with nothing to lose.
    bool discard = _db.cmdline.del || _db.cmdline.clean;
    bool refuse = discard && _db.cmdline.b && !_db.cmdline.merged && nsel > 1;
    if (refuse) {
        prerr("wt.refuse"
              <<Keyval("worktree",GetSelectName())
              <<Keyval("comment","-del -b and -clean -b act on one worktree by name, or on several with -merged"));
    }
    // A remove hook that names no branch worktree fails, so that Claude Code
    // reports the removal as failed rather than done
    bool noworktree = _db.cmdline.hook == "remove" && nsel == 0;
    if (noworktree) {
        prerr("wt.badhook"
              <<Keyval("hook",_db.cmdline.hook)
              <<Keyval("worktree",_db.cmdline.name.expr)
              <<Keyval("comment","git registers no branch worktree at this path; nothing removed"));
    }
    // A remove hook also fails on a worktree that holds uncommitted work or a
    // working process.  Say
    // an agent edits a file in a worktree the create hook made, then asks
    // ExitWorktree to remove it.  Claude Code records no head commit for a
    // worktree a hook made, so it cannot count the changes, and the remove
    // needs discard_changes, which skips Claude Code's own checks and runs this
    // hook.  The hook is then the only check.  The branch survives a remove,
    // so committed work is safe, and an edited or untracked file is not.  So
    // the hook keeps such a worktree whole, and it keeps one that another
    // process works in, such as a build or a test the session left running.
    wt::FSandbox *hooked = _db.cmdline.hook == "remove" ? ind_sandbox_Find(_db.cmdline.name.expr) : NULL;
    tempstr keep(hooked && hooked->select ? GetKeep(hooked->dir) : tempstr());
    if (keep != "") {
        prerr("wt.badhook"
              <<Keyval("hook",_db.cmdline.hook)
              <<Keyval("worktree",_db.cmdline.name.expr)
              <<keep);
    }
    bool ok = !refuse && !noworktree && keep == "";
    ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select && ok) {
        ok = DoSandbox(sandbox);
    }ind_end;
    if (!ok) {
        algo_lib::_db.exit_code = i32_Max(algo_lib::_db.exit_code, 1);
    }
    // A hook answers on stdout.  Claude Code reads the last line a create
    // prints as the worktree's path, so the path is the last thing it prints;
    // a session start that selected its branch worktree answers with JSON that
    // titles the session after it, and any other output would reach the
    // session as context.
    tempstr answer;
    if (ok && _db.cmdline.hook == "create") {
        answer << algo::DirFileJoin(algo::GetCurDir(), algo_lib::WtDir(_db.cmdline.name.expr));
    } else if (ok && _db.cmdline.hook == "sessionstart" && nsel == 1) {
        answer << "{\"hookSpecificOutput\":{\"hookEventName\":\"SessionStart\",\"sessionTitle\":";
        lib_json::JsonSerializeString(_db.cmdline.name.expr, answer);
        answer << "}}";
    }
    if (answer != "") {
        prlog(answer);
    }
    // List the selection after the operations, so the listing shows the state
    // they left
    if (_db.cmdline.list) {
        if (_db.cmdline.b) {
            ListBranch();
        } else {
            ListSandbox();
        }
    }
}
