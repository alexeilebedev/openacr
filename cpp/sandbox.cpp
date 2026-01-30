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
// Target: sandbox (exe) -- Sandbox - sandbox management tool
// Exceptions: yes
// Source: cpp/sandbox.cpp
//

#include "include/sandbox.h"

// -----------------------------------------------------------------------------

// Generate shell helper functions (called once at script start)
void sandbox::AddResetFunc() {
    Ins(&_db.R, _db.script, R"BASH(
clone_sandbox() {
  local dir=$1
  if git rev-parse --is-shallow-repository | grep -q true; then
    git clone . "$dir" 2>/dev/null
  else  # ignore:hanging_else
    git clone --local . "$dir" 2>/dev/null
  fi
}
sync_sandbox() {
  local dir=$1
  git -C "$dir" fetch -q "$curdir" "$ref_sha" $extra_sha
  git -C "$dir" reset --hard -q
  git -C "$dir" clean --exclude=build --exclude=.gcache -fxq .
  git -C "$dir" checkout -B baseline "$ref_sha" -q
}
reset_sandbox() {
  local dir=$1 name=$2 quiet=${3:-}
  [ -z "$quiet" ] && echo "sandbox.reset  sandbox:$name  dir:$dir" >&2
  [ ! -d "$dir" ] && clone_sandbox "$dir"
  if ! sync_sandbox "$dir"; then
    rm -rf "$dir"
    clone_sandbox "$dir"
  fi
  rsync --relative --ignore-missing-args -ac $gitfiles "$dir/"
  git -C "$dir" commit --allow-empty -m baseline -q 2>/dev/null
}
)BASH");
}

// -----------------------------------------------------------------------------

// Add list output to script (uses column -t for formatting)
// Assumes Set was already called for each sandbox
void sandbox::ListSandbox() {
    Ins(&_db.R, _db.script, "(printf 'Sandbox\\tSize\\tClean\\tPath\\tComment\\n'");
    ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
        Ins(&sandbox.R, _db.script, R"BASH(
if pushd "$dir" >/dev/null 2>&1; then
  size=$(du -sh . 2>/dev/null | awk '{print $1}')
  clean=$(git diff --quiet 2>/dev/null && echo Y || echo N)
  popd >/dev/null || exit 1
else
  size=N/A
  clean=Y
fi
printf '%s\t%s\t%s\t%s\t%s\n' "$sandbox" "$size" "$clean" "$dir" "$comment")BASH");
    }ind_end;
    Ins(&_db.R, _db.script, ") | column -t -s '\t'");
}

// -----------------------------------------------------------------------------

void sandbox::Main() {
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
    if (_db.cmdline.create) {
        _db.cmdline.reset=true;
    }
    if (_db.cmdline.shell) {
        cmd_RemoveAll(_db.cmdline);
        cmd_Alloc(_db.cmdline)="bash";
        cmd_Alloc(_db.cmdline)="-l";
    }
    if (_db.cmdline.create) {
        dev::Sandbox sandbox;
        sandbox.sandbox=_db.cmdline.name.expr;
        sandbox_InsertMaybe(sandbox);
    }
    int nsel=0;
    ind_beg(_db_sandbox_curs,sandbox,_db) {
        sandbox.select=Regx_Match(_db.cmdline.name,sandbox.sandbox);
        sandbox.dir = algo_lib::SandboxDir(sandbox.sandbox);
        nsel += sandbox.select;
    }ind_end;
    // Anonymous sandbox: if name provided but no match, create
    // ephemeral sandbox
    if (nsel == 0 && _db.cmdline.name.expr != "") {
        auto& sandbox = ind_sandbox_GetOrCreate(_db.cmdline.name.expr);
        sandbox.select = true;
        sandbox.dir = algo_lib::SandboxDir(sandbox.sandbox);
        if (!_db.cmdline.q) {
            prlog("sandbox.anon"
                  <<Keyval("sandbox",sandbox.sandbox)
                  <<Keyval("dir",sandbox.dir));
        }
    }
    // sbpaths is same for all sandboxes
    tempstr sbpaths;
    ind_beg(_db_sbpath_curs,sbpath,_db) {
        sbpaths<<" "<<sbpath.sbpath;
    }ind_end;
    Set(_db.R, "$sbpaths", sbpaths);
    // Set variables once for all sandboxes before any script generation
    ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
        Set(sandbox.R, "$dir", sandbox.dir);
        Set(sandbox.R, "$sandbox", sandbox.sandbox);
        Set(sandbox.R, "$comment", sandbox.comment);
        Set(sandbox.R, "$ref", _db.cmdline.ref);
        Set(sandbox.R, "$extra", _db.cmdline.refs);
        Set(sandbox.R, "$sbpaths", sbpaths);
        dev::Sandbox out;
        sandbox_CopyOut(sandbox, out);
        Set(sandbox.R, "$row", tempstr() << out);
    }ind_end;
    // Shell options: exit on error, undefined vars, pipe failures
    Ins(&_db.R, _db.script, "set -eu -o pipefail");
    if (algo_lib::_db.cmdline.verbose) {
        Ins(&_db.R, _db.script, "set -x");
    }
    // Check if any operation needs the sandbox
    bool has_operation = _db.cmdline.reset || _db.cmdline.clean || _db.cmdline.del
        || _db.cmdline.gc || _db.cmdline.diff || _db.cmdline.pull
        || _db.cmdline.create || _db.cmdline.shell || cmd_N(_db.cmdline) > 0;
    // Build script for operations (skip if list-only)
    if (has_operation) {
        // Capture values from main repo (once, before any sandbox operations)
        Ins(&_db.R, _db.script, "curdir=$(pwd)");
        // Resolve refs to SHAs (must happen before entering sandbox)
        Ins(&_db.R, _db.script, tempstr() << "ref_sha=$(git rev-parse " << _db.cmdline.ref << ")");
        Ins(&_db.R, _db.script, tempstr() << "extra_sha=$(git rev-parse " << _db.cmdline.refs << " | tr '\\n' ' ')");
        Ins(&_db.R, _db.script, tempstr() << "gitfiles=\"$(git ls-files -m)$sbpaths\"");
        // Define reset function once
        AddResetFunc();
        // Set quiet flag for all sandboxes
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            Set(sandbox.R, "$quiet", _db.cmdline.q ? "1" : "");
        }ind_end;
        ind_beg(_db_sandbox_curs,sandbox,_db) if (sandbox.select) {
            // Reset: always if -reset flag, or auto-reset if dir doesn't exist (runtime check)
            if (_db.cmdline.reset) {
                Ins(&sandbox.R, _db.script, R"(reset_sandbox "$dir" "$sandbox" "$quiet")");
            } else {
                Ins(&sandbox.R, _db.script, R"([ ! -d "$dir" ] && reset_sandbox "$dir" "$sandbox" "$quiet")");
            }
            Ins(&sandbox.R, _db.script, R"(pushd "$dir" >/dev/null || exit 1)");
            ind_beg(command::sandbox_cmd_curs,cmd,_db.cmdline) {
                strptr_PrintBash(cmd, _db.script);
                Ins(&sandbox.R, _db.script, " ", false);
            }ind_end;
            Ins(&sandbox.R, _db.script, "");
            if (_db.cmdline.diff) {
                Ins(&sandbox.R, _db.script, "git diff");
            }
            if (_db.cmdline.clean) {
                Ins(&sandbox.R, _db.script, "git reset --hard -q");
                Ins(&sandbox.R, _db.script, "git clean -df . -q");
            }
            if (_db.cmdline.gc) {
                Ins(&sandbox.R, _db.script, "git gc");
            }
            Ins(&sandbox.R, _db.script, "popd >/dev/null || exit 1");
            if (_db.cmdline.pull) {
                Ins(&sandbox.R, _db.script, R"(git pull --no-edit "$dir" HEAD)");
            }
            if (_db.cmdline.create) {
                Ins(&sandbox.R, _db.script, "echo '$row' | acr -insert -write");
            }
            if (_db.cmdline.del) {
                Ins(&sandbox.R, _db.script, R"(rm -rf "$dir")");
                Ins(&sandbox.R, _db.script, "echo 'acr.delete $row' | acr -insert -write");
            }
        }ind_end;
    }
    // List sandboxes (after operations so state reflects changes)
    if (_db.cmdline.list) {
        ListSandbox();
    }
    // Execute script once at the end
    if (ch_N(_db.script) > 0) {
        SysCmd(_db.script, FailokQ(true), DryrunQ(_db.cmdline.dry_run));
    }
}
