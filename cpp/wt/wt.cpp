// Copyright (C) 2023-2024,2026 AlgoRND
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
// branch, registered only in git.
// Every operation executes its commands directly, in order; -verbose
// echoes each command as it runs. The exit code of a command executed
// inside a sandbox becomes wt's exit code.

#include "include/algo.h"
#include "include/wt.h"

// -----------------------------------------------------------------------------

// Execute a shell command; return true if it exited 0
static bool Run(strptr cmd) {
    return algo::SysCmd(cmd, FailokQ(true)) == 0;
}

// -----------------------------------------------------------------------------

// Return the SHA that -ref resolves to
static tempstr RefSha() {
    return tempstr() << Trimmed(algo::SysEval(tempstr()<<"git rev-parse "<<wt::_db.cmdline.ref, FailokQ(false), 1024));
}

// -----------------------------------------------------------------------------

// Return the path of the libcowdancer preload library (COWDANCER_SO overrides)
static tempstr FarmSo() {
    const char *env = getenv("COWDANCER_SO");
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
    if (!ok) {
        ok = Run(tempstr()<<"rm -rf "<<algo::strptr_ToBash(sandbox.dir));
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

// Seed DIR's build directories: local empty build/<cfg> with abt and gcache
// linked from the main checkout, and the compiler cache enabled, so the
// first build inside the worktree is cache-served
static bool WireBuild(strptr dir) {
    tempstr script;
    script << "dir=" << algo::strptr_ToBash(dir) << "\n";
    script << R"BASH(for cfg in release coverage debug profile; do
  link=$(readlink "build/$cfg" 2>/dev/null) || continue
  name=$(basename "$link")
  [ -d "build/$name" ] || continue
  mkdir -p "$dir/build/$name"
  for tool in abt gcache; do
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

// Create SANDBOX's branch worktree at DIR: new branch named after it, .branch
// symlink to the shared branch-control directory, isolated cache-backed build
static bool AddBranch(wt::FSandbox &sandbox) {
    if (!wt::_db.cmdline.q) {
        prlog("wt.branch"<<Keyval("branch",sandbox.sandbox)<<Keyval("dir",sandbox.dir));
    }
    tempstr dirq(algo::strptr_ToBash(sandbox.dir));
    tempstr nameq(algo::strptr_ToBash(sandbox.sandbox));
    bool ok = AddParentDir(sandbox.dir);
    ok = ok && Run(tempstr()<<"git worktree add -q "<<dirq<<" -b "<<nameq<<" "<<RefSha()<<" 2>/dev/null"
                   <<" || git worktree add -q "<<dirq<<" "<<nameq);
    ok = ok && Run(tempstr()<<"[ ! -e .branch ] || ln -sfn ../../.branch "<<dirq<<"/.branch");
    ok = ok && WireBuild(sandbox.dir);
    return ok;
}

// -----------------------------------------------------------------------------

// Spawn a background claude session inside SANDBOX's worktree, named after
// it; the session boots by reading the agent instructions and the branch
// control file, then stands by for instructions (attach with
// claude --resume <name>). The .branch symlink points outside the worktree,
// so the resolved branch-control directory is granted with --add-dir;
// without the grant the boot turn stalls on a read-permission prompt.
static bool StartClaudeSess(wt::FSandbox &sandbox) {
    if (!wt::_db.cmdline.q) {
        prlog("wt.claudesess"<<Keyval("session",sandbox.sandbox)<<Keyval("dir",sandbox.dir));
    }
    tempstr prompt;
    prompt << "Read CLAUDE.md and .branch/" << sandbox.sandbox << ".md and stand by.";
    tempstr branchdir(Trimmed(algo::SysEval("readlink -f .branch", FailokQ(true), 1024)));
    tempstr script;
    script << "cd " << algo::strptr_ToBash(sandbox.dir);
    // the prompt precedes --add-dir: the flag is variadic and would
    // otherwise consume the prompt as one more directory
    script << " && claude --bg " << algo::strptr_ToBash(prompt);
    script << " --name " << algo::strptr_ToBash(sandbox.sandbox);
    if (DirectoryQ(branchdir)) {
        script << " --add-dir " << algo::strptr_ToBash(branchdir);
    }
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

// Print selected sandboxes: name, materialization, size, cleanliness, path
static void ListSandbox() {
    prlog("Sandbox\tCow\tSize\tClean\tPath\tComment");
    ind_beg(wt::_db_sandbox_curs,sandbox,wt::_db) if (sandbox.select) {
        tempstr size("N/A");
        tempstr clean("Y");
        if (DirectoryQ(sandbox.dir)) {
            tempstr dirq(algo::strptr_ToBash(sandbox.dir));
            size = Trimmed(algo::SysEval(tempstr()<<"du -sh "<<dirq<<" 2>/dev/null | awk '{print $1}'", FailokQ(true), 1024));
            clean = Run(tempstr()<<"git -C "<<dirq<<" diff --quiet 2>/dev/null") ? "Y" : "N";
        }
        prlog(sandbox.sandbox<<"\t"<<(sandbox.cow ? "Y" : "N")<<"\t"<<size<<"\t"<<clean<<"\t"<<sandbox.dir<<"\t"<<sandbox.comment);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Perform the requested actions on SANDBOX, in the documented order:
// -create (register), -reset (or reset on first use), -claudesess, -clean,
// execute command, -diff, -del. Returns false when an action fails; the
// command's exit code is recorded on algo_lib exit_code.
static bool DoSandbox(wt::FSandbox &sandbox) {
    bool ok = true;
    bool run = cmd_N(wt::_db.cmdline) > 0;
    bool enter = wt::_db.cmdline.reset || wt::_db.cmdline.create || wt::_db.cmdline.clean
        || wt::_db.cmdline.diff || wt::_db.cmdline.pull || wt::_db.cmdline.claudesess || run;
    if (wt::_db.cmdline.create && !wt::_db.cmdline.b) {
        dev::Sandbox out;
        sandbox_CopyOut(sandbox, out);
        ok = Run(tempstr()<<"echo "<<algo::strptr_ToBash(tempstr()<<out)<<" | acr -insert -write");
    }
    if (ok && enter) {
        // Materialize: branch worktree (-b) is created once, never auto-reset;
        // a sandbox resets on -reset or on first use
        if (wt::_db.cmdline.b) {
            if (!DirectoryQ(sandbox.dir)) {
                ok = AddBranch(sandbox);
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

// Select sandboxes by the name regex and perform the requested actions on
// each; -list prints the surviving state afterwards
void wt::Main() {
    // If no cmd and stdin is not tty, read stdin as script
    if (cmd_N(_db.cmdline) == 0 && !_db.cmdline.shell) {
        if (!isatty(0)) {
            algo::ListSep sep("\n");
            tempstr script;
            ind_beg(algo::FileLine_curs, line, algo::Fildes(0)) {
                script << sep << line;
            }ind_end;
            if (ch_N(script)) {
                cmd_Alloc(_db.cmdline) = script;
            }
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
    int nsel=0;
    ind_beg(_db_sandbox_curs,sandbox,_db) {
        sandbox.select=Regx_Match(_db.cmdline.name,sandbox.sandbox);
        sandbox.dir = algo_lib::WtDir(sandbox.sandbox);
        nsel += sandbox.select;
    }ind_end;
    // Anonymous sandbox: if name provided but no match, create
    // ephemeral sandbox; -cow selects its materialization
    if (nsel == 0 && _db.cmdline.name.expr != "") {
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
    bool ok = true;
    ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select && ok) {
        ok = DoSandbox(sandbox);
    }ind_end;
    if (!ok) {
        algo_lib::_db.exit_code = i32_Max(algo_lib::_db.exit_code, 1);
    }
    // List worktrees (after operations so state reflects changes):
    // with -b, all git-registered worktrees; otherwise the dev.sandbox set
    if (_db.cmdline.list) {
        if (_db.cmdline.b) {
            SysCmd("git worktree list", FailokQ(true));
        } else {
            ListSandbox();
        }
    }
}
