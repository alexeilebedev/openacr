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
// Source: cpp/atf_comp/proc.cpp
//

#include "include/algo.h"
#include "include/atf_comp.h"
#include <sys/wait.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Set test variable and add to Replscope
void atf_comp::SetVar(strptr name, strptr value) {
    Set(_db.R, tempstr() << "$" << name, value);
}

// Apply $-substitution to string
//
// Ins appends an end-of-line by default, because its usual job is to add a
// line to a text being assembled.  A substituted value here is not a line: it
// is a command to spawn, a message to write to a process, or the value of an
// environment variable, and a trailing newline in any of those is wrong.  An
// environment variable is where the difference shows: a cluster told its
// instance directory is "temp/atf_comp/<test>\n" creates that directory, and
// every path a test script builds under it then breaks apart at the newline.
// Return S with every $-variable of the running test replaced by its value:
// $tempdir is the test's directory, $bindir the build under test, and `$$`
// stands for one dollar sign.
tempstr atf_comp::Subst(strptr s) {
    tempstr out;
    Ins(&atf_comp::_db.R, out, s, false);
    return out;
}

// Return the path of FILE inside the running test's tempdir; an absolute FILE
// is returned as it is.
tempstr atf_comp::TempPath(strptr file) {
    return StartsWithQ(file, "/") ? tempstr(file) : Subst(tempstr() << "$tempdir/" << file);
}

// Export the environment every test of this tree is run under, as the
// atfdb.testenv table states it.
//
// A comptest that starts a cluster needs that cluster pointed at this test's
// own directory rather than at whatever instance the host is running, and the
// name of the variable that does the pointing belongs to the platform under
// test, not to the runner.  So each row names a variable and a value, the
// value goes through the same $-substitution a test script does -- `$tempdir`
// is this test's directory -- and a tree with no such platform simply has no
// rows.  SLOW says the run is instrumented and therefore far slower than
// release, which is when a row marked slowonly applies: startup graces belong
// to that case and would otherwise put progress lines in the goldens.
void atf_comp::SetTestenv(bool slow) {
    ind_beg(atf_comp::_db_testenv_curs,testenv,atf_comp::_db) {
        if (slow || !testenv.slowonly) {
            setenv(Zeroterm(tempstr()<<testenv.testenv), Zeroterm(Subst(testenv.value)), 1);
        }
    }ind_end;
}

// Append line to test log; with -v, also print to stdout
static void Log(strptr line) {
    atf_comp::_db.log << line << "\n";
    verblog(line);
}

// The FEnv row for NAME, allocated on first use with the harness's own value
// of the variable recorded, so ClearEnv can put it back; CREATED says whether
// this call allocated it.
static atf_comp::FEnv &FindOrAllocEnv(strptr name, bool &created) {
    atf_comp::FEnv *env = atf_comp::ind_env_Find(name);
    created = !env;
    if (!env) {
        env = &atf_comp::env_Alloc();
        env->env = name;
        const char *orig = getenv(Zeroterm(tempstr(name)));
        env->had_orig = orig != NULL;
        env->orig = env->had_orig ? strptr(orig) : strptr();
        vrfy(atf_comp::env_XrefMaybe(*env), algo_lib::DetachBadTags());
    }
    return *env;
}

// Set the environment variable NAME to VALUE for every process the test starts
// from here on, and log it as `# setenv NAME:VALUE`.  VALUE is logged as the
// test wrote it, so the golden shows `$HOMEDIR` and the process sees the
// $-substituted path.  Whether the line is logged follows from the test's own
// record: the first SetEnv of a name always logs, and a second SetEnv to the
// value the test already set does nothing while the environment still holds
// its substitution, whatever the harness's environment held when it started.
// A SetVar that moves the substitution between the two makes the second set
// and log again.  The harness's own value, if it had one, is restored by
// ClearEnv.
void atf_comp::SetEnv(strptr name, strptr value) {
    bool created = false;
    atf_comp::FEnv &env = FindOrAllocEnv(name, created);
    tempstr subst = Subst(value);
    const char *cur = getenv(Zeroterm(tempstr(name)));
    bool same = env.set && env.value == value && cur && strptr(cur) == strptr(subst);
    if (!same) {
        env.value = value;
        env.set = true;
        setenv(Zeroterm(tempstr(name)), Zeroterm(subst), 1);
        Log(tempstr() << "# setenv " << name << ":" << strptr_ToSsim(value));
    }
}

// Remove the environment variable NAME from the environment of every process
// the test starts from here on, and log it as `# unsetenv NAME`.  ClearEnv
// restores the harness's own value.
void atf_comp::UnsetEnv(strptr name) {
    bool created = false;
    atf_comp::FEnv &env = FindOrAllocEnv(name, created);
    // a row the test already unset is left alone; a fresh row or a set one is unset
    if (created || env.set) {
        env.value = "";
        env.set = false;
        unsetenv(Zeroterm(tempstr(name)));
        Log(tempstr() << "# unsetenv " << name);
    }
}

// Forget every SetEnv and UnsetEnv of the test and put the harness's own
// environment back, logging `# clearenv` when there was anything to forget.
// A test calls it where its environment ends, and the harness calls it right
// after every test's step, whether the step returned or threw, so the wait,
// the comparison and the capture run in the harness's own environment.
void atf_comp::ClearEnv() {
    if (env_N() > 0) {
        ind_beg(atf_comp::_db_env_curs, env, atf_comp::_db) {
            if (env.had_orig) {
                setenv(Zeroterm(tempstr(env.env)), Zeroterm(tempstr(env.orig)), 1);
            } else {
                unsetenv(Zeroterm(tempstr(env.env)));
            }
        }ind_end;
        env_RemoveAll();
        Log("# clearenv");
    }
}

// Record the start of the test stage NAME in the log, as `# stage NAME`, so a
// reader of the golden can tell one run of the tool from the next.
void atf_comp::Stage(strptr name) {
    Log(tempstr() << "# stage " << name);
}

// Record a fact the test established in C++ in the log, as `# check NAME:VALUE`,
// beside the lines the processes printed, so the golden pins it.
void atf_comp::Check(strptr name, strptr value) {
    Log(tempstr() << "# check " << name << ":" << value);
}

// Record each distinct line of TEXT that REGX matches whole, trimmed and in
// sorted order, as `# check NAME:<line>`, and return how many lines it recorded.
// REGX is an SQL regex in the form acr reads: `%` matches any run, `_` any
// character, and `(a|b)` either branch.  This is the C++ form of
// `| grep | sort -u` on a tool's output: the test reads the output with SysEval
// and states the lines that concern it, and a test that expects none of them
// checks the count.
int atf_comp::CheckMatch(strptr name, strptr text, strptr regx) {
    algo_lib::Regx re;
    algo_lib::Regx_ReadSql(re, regx, true);
    algo::StringAry match;
    ind_beg(algo::Line_curs, rawline, text) {
        algo::strptr line = algo::Trimmed(rawline);
        if (algo_lib::Regx_Match(re, line)) {
            i64 at = 0;
            while (at < ary_N(match) && algo::strptr_Lt(ary_qFind(match, at), line)) {
                at++;
            }
            bool dup = at < ary_N(match) && algo::strptr_Eq(ary_qFind(match, at), line);
            if (!dup) {
                ary_AllocAt(match, at) = line;
            }
        }
    }ind_end;
    ind_beg(algo::StringAry_ary_curs, line, match) {
        atf_comp::Check(name, line);
    }ind_end;
    return i32(ary_N(match));
}

// Record the state of FILE, a path under the tempdir, as `# check NAME:<state>`:
// `present` for a regular file holding bytes, `empty` for one holding none,
// `directory` for a directory, and `absent`.
void atf_comp::CheckFile(strptr name, strptr file) {
    tempstr path = TempPath(file);
    strptr state = "absent";
    if (DirectoryQ(path)) {
        state = "directory";
    } else if (FileQ(path)) {
        state = GetFileSize(path) > 0 ? "present" : "empty";
    }
    Check(name, state);
}

// If stablefld is set on the current comptest, replace unstable field values with ***
// Check if attribute is unstable by looking up head.attr and %.attr.
//
// An element of a varlen field is printed under the field's name and its index,
// as `gw.0`, so a message carrying three gateways prints three attributes and no
// name a table could list covers them.  What is unstable about one element is
// what is unstable about the field, so the index is dropped and the field
// itself is looked up too: one `%.gw` row masks `gw`, `gw.0` and `gw.7` alike.
static bool UnstableAttrQ(strptr head, strptr attrname) {
    strptr base = Pathcomp(attrname, ".LL");
    bool ret = false;
    if (atf_comp::ind_unstableattr_Find(tempstr() << head << "." << attrname)) {
        ret = true;
    } else if (atf_comp::ind_unstableattr_Find(tempstr() << "%." << attrname)) {
        ret = true;
    } else if (base != attrname && atf_comp::ind_unstableattr_Find(tempstr() << "%." << base)) {
        ret = true;
    }
    return ret;
}

static tempstr StabilizeLine(strptr line) {
    atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
    if (ct && ct->stablefld) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptrMaybe(tuple, line)) {
            bool changed = false;
            ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                if (UnstableAttrQ(tuple.head.value, attr.name)) {
                    attr.value = "***";
                    changed = true;
                }
            }ind_end;
            if (changed) {
                tempstr out;
                Tuple_Print(tuple, out);
                return out;
            }
        }
    }
    return tempstr(line);
}

// Whether LINE is one the atfdb.unstableline table names, so the capture leaves
// it out.
//
// The problem, by example.  A process reports a core-shortage alarm when the
// box it starts on has fewer cores than its node's processes want.  A comptest
// that brings up an eight-process node therefore prints eight of those lines on
// an eight-core host and none at all on a thirty-two-core one, and the numbers
// on the line -- the core count itself -- belong to the host rather than to the
// test.  A golden recorded on either machine fails on the other.
//
// Masking the values, which is what atfdb.unstableattr does, cannot reach this:
// the line is either there or it is not.  So a row names the tuple head of such
// a line and the capture drops it whole, which is the same judgement the gcov
// noise above gets, expressed as data rather than as a literal.
static bool UnstableLineQ(strptr line) {
    bool ret = false;
    algo::Tuple tuple;
    if (Tuple_ReadStrptrMaybe(tuple, line)) {
        ret = atf_comp::ind_unstableline_Find(tuple.head.value) != NULL;
    }
    return ret;
}

// Emit output lines to log as "proc -> line"
// Skip gcov profiling noise from coverage-built binaries, and every line the
// atfdb.unstableline table names.
static void LogOutput(atf_comp::FProc &proc, strptr text) {
    ind_beg(algo::Line_curs, line, text) {
        if (!proc.quiet
            && ch_N(line) > 0
            && !StartsWithQ(line, "profiling:")
            && algo::FindStr(line, ".gcda:") == -1
            && !UnstableLineQ(line)) {
            Log(tempstr() << proc.proc << " -> " << StabilizeLine(line));
        }
    }ind_end;
}

// Record LINE, a line the test selected from FILE, in the log as `FILE -> LINE`,
// the form a process's output takes, with the fields atfdb.unstableattr names
// masked as a process's output has them masked, so a golden pins a fragment of
// a file the test read without pinning the whole file.
void atf_comp::Excerpt(strptr file, strptr line) {
    Log(tempstr() << file << " -> " << StabilizeLine(line));
}

// Return the leading `cd <dir> && ` of CMD, up to and including the space
// after the `&&`, or an empty string when CMD does not start that way.
static strptr CdPrefix(strptr cmd) {
    strptr prefix;
    int amp = algo::FindStr(cmd, "&&");
    if (StartsWithQ(cmd, "cd ") && amp != -1) {
        int end = amp + 2;
        while (end < ch_N(cmd) && cmd.elems[end] == ' ') {
            end++;
        }
        prefix = ch_FirstN(cmd, end);
    }
    return prefix;
}

// TRUE when WORD is a shell assignment, `NAME=value` with an identifier before
// the `=`: letters, digits and underscores, not starting with a digit.
static bool AssignmentWordQ(strptr word) {
    int eq = algo::FindChar(word, '=');
    bool ok = eq > 0 && !(word.elems[0] >= '0' && word.elems[0] <= '9');
    for (int i = 0; ok && i < eq; i++) {
        char c = word.elems[i];
        ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
    }
    return ok;
}

// Return the base name of the program CMD starts: the first word of CMD, read
// past a leading `cd <dir> &&` and past any `NAME=value` assignments that set
// the program's environment, with its directory removed.
static strptr ProgramOf(strptr cmd) {
    strptr rest = Trimmed(ch_RestFrom(cmd, ch_N(CdPrefix(cmd))));
    strptr word = Pathcomp(rest, " LL");
    while (AssignmentWordQ(word)) {
        rest = Trimmed(ch_RestFrom(rest, ch_N(word)));
        word = Pathcomp(rest, " LL");
    }
    return Pathcomp(word, "/RR");
}

// Return a process name unique within the test for the command CMD: the base
// name of the program it starts, with -N appended when that name is in use.
static tempstr DeriveProcName(strptr cmd) {
    tempstr base(ProgramOf(cmd));
    // find unique name
    tempstr name = tempstr()<< base;
    int suffix = 2;
    while (atf_comp::ind_proc_Find(name)) {
        name = tempstr() << base << "-" << suffix;
        suffix++;
    }
    return name;
}


// Whether a program name is a shell, i.e. a program whose job is to run the
// command it is handed as a child process instead of doing the work itself.
// The whole name is matched, never a suffix: a comptest can name a script
// such as kafka-console-consumer.sh, which is an ordinary program.
static bool ShellQ(strptr name) {
    return name == "sh" || name == "bash" || name == "dash" || name == "ksh" || name == "zsh";
}

// Multiplier on a comptest's wall-clock budget for the build under test.
//
// A comptest that starts a cluster and drives a client through it takes tens of
// seconds on the release build, and atfdb.comptest.timeout is the budget that
// work is given.  gcov instrumentation and valgrind each run the same work
// several times slower, so on those builds the budget expires while the test is
// still doing what it was asked to do, and the run is reported as a timeout of
// whichever process the harness happened to be reading.  The number states the
// time the test needs, so the harness scales it by what this build costs rather
// than every comptest carrying a figure tuned for the slowest one.
static double TimeoutScale() {
    bool instrumented = atf_comp::_db.cmdline.cfg == dev_Cfg_cfg_coverage
        || atf_comp::_db.cmdline.mode == command_atf_comp_mode_memcheck;
    return instrumented ? 4.0 : 1.0;
}

// Spawn subprocess with $-substitution, return reference.
atf_comp::FProc &atf_comp::ProcStart(strptr cmd) {
    tempstr name = DeriveProcName(cmd);
    Log(tempstr() << "# start " << name << " cmd:" << strptr_ToSsim(Trimmed(cmd)));
    tempstr cmd_eff = Subst(cmd);
    cmd_eff = tempstr() << algo::TrimmedRight(cmd_eff);
    atf_comp::FProc &proc = proc_Alloc();
    // wrap command based on mode.  Coverage needs no wrapping: under
    // -cfg:coverage the command already names build/coverage/<tool>, and its
    // gcda are routed by the GCC_PROFILE_DIR atf_ci exports.
    u8 mode = atf_comp::_db.cmdline.mode;
    atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
    // The first process is the program under test and the rest are auxiliary,
    // except in a test of a command-line tool, which runs the tool once per
    // stage: every process whose program is the comptest's own target -- the
    // namespace the comptest is named under -- is the program under test too.
    bool tool_proc = ct && ProgramOf(cmd_eff) == Pathcomp(ct->comptest, ".RL");
    if (mode == command_atf_comp_mode_memcheck && (proc_N() == 1 || tool_proc)) {
        // Valgrind instruments the process it execs.  Most commands name the
        // tool under test directly, so that process is the tool and memcheck
        // sees the allocations the test is about.  A command written as a
        // shell script ("bash -c 'cd ... && tool'") differs: the process
        // valgrind execs is the shell, the tool runs as its child, and a
        // memory error in the tool passes the run green.  Following children
        // instruments the tool through the shell.
        //
        // A pipeline is out of reach of child tracing.  Bash parses
        // "echo request | tool" after the prefix has been glued on, so the
        // process valgrind execs is the echo, and the tool runs beside it as
        // a sibling.  Child tracing follows a process's descendants, and a
        // sibling is not one of them, so the tool would run uninstrumented
        // and the row would pass green on the echo's allocations.  A comptest
        // that needs input on the tool's stdin writes it with ProcWrite,
        // which leaves the command a simple one naming the tool.
        //
        // Children are followed only for a shell command, because following
        // them also reaches a supervisor's children.  A cluster comptest starts
        // a supervisor, which spawns the node processes; each of those
        // would run instrumented too, and a test that takes four seconds
        // does not finish in twenty minutes.  Those node processes are what
        // the test drives, not the program it tests.
        bool shell_cmd = ShellQ(ProgramOf(cmd_eff));
        // Each traced process writes its own log (%p = pid; a shared
        // log file would be clobbered, each process truncates it on open);
        // ProcWait checks every log the run produced. The test's tempdir is
        // wiped before each run, so no stale log can contaminate the glob.
        // The path handed to valgrind must be absolute: each traced process
        // opens its log from its own working directory, and a compound
        // command typically cds into the tempdir before running the tool.
        tempstr logbase;
        logbase << "temp/atf_comp/" << (ct ? strptr(ct->comptest) : strptr("unknown")) << "/" << name << ".memcheck";
        proc.memcheck_log = logbase;
        // The log path embeds the checkout's absolute directory, and two
        // parsers read it in turn: bash (the whole command runs through
        // bash -c) and valgrind.  Valgrind reads '%' in --log-file as a
        // format escape (only %p, %q, %% are valid) and aborts at startup
        // on anything else; bash word-splits on spaces and expands '$', so
        // a checkout under a path like '/home/u/my work' hands valgrind a
        // truncated log path and the remainder as the program to run.
        // Double every '%' in the path portion so only the trailing %p
        // stays a live escape, then bash-quote the result.
        tempstr logfile = algo::DirFileJoin(algo::GetCurDir(), logbase);
        algo::Replace(logfile, "%", "%%");
        // Child tracing reaches every program the script runs, and a
        // comptest that compiles a file runs the whole C++ toolchain: the
        // driver, the compiler proper, the assembler, the linker.  Valgrind
        // finds uninitialised reads in GCC's own register allocator, so a
        // comptest whose script happens to invoke a compiler fails on errors
        // in a binary this repo does not build.  Valgrind's verdict on such
        // a binary is not a verdict about this repo, so tracing stops at the
        // toolchain.  Skipping the driver is what does the work, because the
        // stages it spawns then run outside valgrind altogether; the stages
        // are named as well, for a build that invokes one of them directly.
        // The patterns are bash-quoted: '*' is a glob character, and the
        // whole valgrind wrap is spliced into a command bash parses.
        strptr trace_child = shell_cmd ? " --trace-children=yes --trace-children-skip='*/cc,*/c++,*/cc1,*/cc1plus,*/gcc,*/gcc-*,*/g++,*/g++-*,*/clang,*/clang++,*/as,*/ld,*/collect2'" : "";
        // Ask for the leak search, which is the whole reason the pools mark what
        // they hand out: without it the run reports invalid reads and writes and
        // says nothing about a record allocated and never deleted.
        //
        // Only a definite loss is an error.  A block still reachable from a
        // global at exit is not a leak but a table a module holds until it
        // stops, and a possible loss is a block reached by an interior pointer,
        // which is what a pool's own free list looks like from outside.  Counting
        // either would report every module in the tree as leaking and leave the
        // one real finding indistinguishable from the noise.
        //
        // The suppression path is absolute for the same reason the log path is:
        // valgrind is started from whichever directory the step runs in, and a
        // compound command has usually cd'd into the tempdir by then.
        tempstr suppfile = algo::DirFileJoin(algo::GetCurDir(), "conf/memcheck.supp");
        // The wrap goes in front of the program and not in front of a leading
        // `cd <dir> &&`: valgrind asked to exec `cd` finds no such program,
        // exits 127, and the `&&` then skips the tool.
        tempstr cd_prefix(CdPrefix(cmd_eff));
        cmd_eff = tempstr() << cd_prefix << "valgrind --tool=memcheck" << trace_child
                            << " --leak-check=full --errors-for-leak-kinds=definite"
                            << " --suppressions=" << algo::strptr_ToBash(suppfile)
                            << " --log-file=" << algo::strptr_ToBash(logfile) << ".%p.log "
                            << ch_RestFrom(cmd_eff, ch_N(cd_prefix));
    } else if (mode == command_atf_comp_mode_valgrind && proc_N() == 1) {
        tempstr cd_prefix(CdPrefix(cmd_eff));
        cmd_eff = tempstr() << cd_prefix << "valgrind " << ch_RestFrom(cmd_eff, ch_N(cd_prefix));
    }
    proc.proc = name;
    proc.status = -1;
    // run the (shell) command through bash; FProc's cloexec (default) keeps the
    // pipe fds out of other procs' children, where an inherited copy would hold
    // the pipe open and block EOF.
    ary_Alloc(proc.subproc.args) = "bash";
    ary_Alloc(proc.subproc.args) = "-c";
    ary_Alloc(proc.subproc.args) = cmd_eff;
    proc.subproc.fstdin  = "|"; // write end exposed as to_stdin
    proc.subproc.fstdout = "|"; // read end exposed as from_stdout
    proc.subproc.fstderr = ">&1"; // fold stderr into the stdout pipe
    proc.subproc.pgroup = true;
    // Bound the child by the comptest's own budget, so nothing a test spawns can
    // outlive the test.  Teardown kills the run's processes, which is enough while
    // teardown gets to run; it does not run when the harness is killed outright,
    // and it has nothing to kill when a test starts a server and never asks for it
    // to stop.  glserver is the standing case of the second: it answers until it is
    // killed, so a run that does not kill it leaves it holding a localhost port and
    // its fixture in memory for as long as the machine stays up.
    //
    // A comptest's budget is the right bound because a process the test spawned has
    // no business outliving the test, and the figure here is the one the run is
    // already judged against.  It carries the same TimeoutScale the harness applies
    // to its own deadline, because the two have to expire together: gcov and
    // valgrind run the work several times slower, so a child held to the release
    // budget would be shot while the test it belongs to still had time to spend.
    //
    // The alarm is set in the child between fork and exec, so it needs nothing of
    // the parent and survives into whatever the command execs.  What it does not
    // reach is a grandchild: bash replaces itself with a simple command, so the
    // bound lands on the program itself, but a pipeline or a chain leaves bash as
    // the process holding the alarm and its children unbounded.
    if (atf_comp::_db.c_cur_comptest) {
        proc.subproc.timeout = i32(atf_comp::_db.c_cur_comptest->timeout * TimeoutScale());
    }
    algo_lib::ProcStart(proc.subproc);
    // take ownership of the parent-side pipe ends; the harness (and the fbuf
    // reader) close them, so detach from subproc to avoid a double close.
    proc.stdin_fd = proc.subproc.to_stdin;
    algo::Fildes read_fd = proc.subproc.from_stdout;
    proc.subproc.to_stdin = algo::Fildes();
    proc.subproc.from_stdout = algo::Fildes();
    algo::SetBlockingMode(read_fd, false);
    in_BeginRead(proc, read_fd);
    proc_XrefMaybe(proc);
    return proc;
}

// Write SCRIPT, one shell line per line, to NAME.sh in the test's tempdir and
// start bash on it from the tempdir; return the process.  Each line is recorded
// in the log as `# NAME.sh: <line>` before the start, with its $-variables
// unreplaced, so the golden shows the script as written and reads the same on
// every machine.
atf_comp::FProc &atf_comp::ScriptStart(strptr name, strptr script) {
    ind_beg(algo::Line_curs, line, script) {
        Log(tempstr() << "# " << name << ".sh: " << line);
    }ind_end;
    StringToFile(Subst(script), TempPath(tempstr() << name << ".sh"));
    return ProcStart(tempstr() << "cd $tempdir && bash " << name << ".sh");
}

// Connect to ADDR, a $-substituted `host:port`, and return the connection as a
// process named tcp, tcp-2 and so on.  ProcWrite sends a line over the socket,
// ProcRead reads the server's lines with each trailing carriage return
// removed, and ProcWait closes the socket at once, discarding what the test
// did not read, so the log holds exactly the lines the test read.  KEEP, when not empty, is a
// `|`-separated list of line prefixes: a line starting with none of them is
// read past and neither logged nor matched, which keeps out of the golden what
// the server sends at its own pace, such as a delivery racing a PONG.  ProcWriteEof half-closes the
// socket, and a ProcRead to end of file then reads until the server closes.
//
// A protocol client driven from a comptest has to end when the test is done
// with it.  nc does not: with the server holding the connection open it waits
// out its own timeout whatever its stdin does, and every stanza built on it pays
// that whole bound.  The harness holding the socket itself ends the
// conversation the moment the test has read what it asserts.
atf_comp::FProc &atf_comp::TcpStart(strptr addr, strptr keep DFLTVAL("")) {
    tempstr name = DeriveProcName("tcp");
    Log(tempstr() << "# connect " << name << " addr:" << strptr_ToSsim(addr));
    tempstr addr_eff = Subst(addr);
    atf_comp::FProc &proc = proc_Alloc();
    proc.proc = name;
    proc.status = -1;
    proc.tcp = true;
    proc.keep = keep;
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(u16(algo::ParseI64(Pathcomp(addr_eff, ":RR"), 0)));
    vrfy(inet_pton(AF_INET, Zeroterm(tempstr() << Pathcomp(addr_eff, ":RL")), &sa.sin_addr) == 1
         , tempstr() << "atf_comp.tcp_addr" << Keyval("addr", addr_eff));
    // SOCK_CLOEXEC is Linux's; fcntl marks the descriptor on every platform, and
    // the harness forks nothing between the two calls.
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    errno_vrfy_(fd >= 0);
    errno_vrfy_(fcntl(fd, F_SETFD, FD_CLOEXEC) == 0);
    int rc = connect(fd, (struct sockaddr*)&sa, sizeof(sa));
    vrfy(rc == 0, tempstr() << "atf_comp.tcp_connect" << Keyval("addr", addr_eff) << Keyval("err", strerror(errno)));
    proc.stdin_fd = algo::Fildes(fcntl(fd, F_DUPFD_CLOEXEC, 0));
    algo::Fildes read_fd(fd);
    algo::SetBlockingMode(read_fd, false);
    in_BeginRead(proc, read_fd);
    proc_XrefMaybe(proc);
    return proc;
}

// Keep PROC's writes and reads out of the log while QUIET holds, and log the
// switch as `# quiet PROC on` or `# quiet PROC off`.  For a probe the test
// repeats until the cluster answers it, whose number of rounds differs from
// run to run: the golden holds what the probe decided and not how often it
// asked.
void atf_comp::ProcQuiet(atf_comp::FProc &proc, bool quiet) {
    Log(tempstr() << "# quiet " << proc.proc << (quiet ? " on" : " off"));
    proc.quiet = quiet;
}

// Wait SECS seconds, scaled by the build as a comptest's budget is, and log
// `# sleep SECS`.  For a window whose length is the assertion: a test that
// requires something not to happen waits out the time in which it would have,
// and the wait is a step of its own, visible in the golden.
void atf_comp::Sleep(double secs) {
    Log(tempstr() << "# sleep " << secs);
    algo::SleepMsec(i64(secs * TimeoutScale() * 1000));
}

// Write MSG to the stdin of PROC as one line, $-substituted, trimmed and
// ended by TERMINATOR, and log it first as `<proc> <- <msg>` with MSG as the
// test wrote it, one log line per line of MSG.  A MSG of several lines goes out
// in one write, for a protocol whose reader must see them arrive together.  A CRLF protocol passes "\r\n" as TERMINATOR.
// A tool that rejects its command line exits before it reads fd 0, so the
// write can reach a pipe whose read end is already gone.  What the tool did
// with the line reaches the golden through the tool's own output and exit
// code, and the line itself is recorded either way, so a broken pipe is the
// tool's verdict rather than a harness failure and is not diagnosed as one.
// Any other write error still aborts the test.
// SIGPIPE is ignored only for the duration of the write: the disposition is
// process-global and survives exec into every child atf_comp spawns later,
// so leaving SIG_IGN set would silently change how the tools under test
// react to their own broken pipes.
void atf_comp::ProcWrite(atf_comp::FProc &proc, strptr msg, strptr terminator DFLTVAL("\n")) {
    if (!proc.quiet && ch_N(msg) == 0) {
        Log(tempstr() << proc.proc << " <- ");
    }
    ind_beg(algo::Line_curs, line, msg) {
        if (!proc.quiet) {
            Log(tempstr() << proc.proc << " <- " << algo::TrimmedRight(line));
        }
    }ind_end;
    tempstr line;
    line << Trimmed(Subst(msg)) << terminator;
    auto prior = signal(SIGPIPE, SIG_IGN);
    ssize_t nwrite = write(proc.stdin_fd.value, line.ch_elems, line.ch_n);
    int err = errno;
    signal(SIGPIPE, prior);
    errno = err;
    errno_vrfy_(nwrite == (ssize_t)line.ch_n || (nwrite == -1 && err == EPIPE));
}

// Send SIGNAL to the process group PROC leads, and report whether a live process
// received it.  The one place that decides which pid a comptest's processes are
// reached by, and usable from a signal handler because kill is its only call.
bool atf_comp::ProcSignal(atf_comp::FProc &proc, int signal) {
    // a reaped proc holds pid 0, and kill(0) signals the caller's own group
    bool sent = proc.subproc.pid > 0;
    if (sent) {
        int target = proc.subproc.pgroup ? -proc.subproc.pid : proc.subproc.pid;
        kill(target, signal);
    }
    return sent;
}

// Send SIGNAL to every process group this run created.
void atf_comp::ProcSignalAll(int signal) {
    ind_beg(atf_comp::_db_proc_curs, proc, atf_comp::_db) {
        atf_comp::ProcSignal(proc, signal);
    }ind_end;
}

// Send SIGNAL to the process group PROC leads, which holds the shell the
// command runs under together with any tool that shell forked.  Records the kill
// so ProcWait reports the status as -1 rather than an exit code.
void atf_comp::ProcKill(atf_comp::FProc &proc, int signal) {
    Log(tempstr() << "# kill " << proc.proc << " signal:" << signal);
    bool sent = atf_comp::ProcSignal(proc, signal);
    if (sent) {
        proc.killed = true;
    }
}

// Close process stdin (signal EOF)
void atf_comp::ProcWriteEof(atf_comp::FProc &proc) {
    if (ValidQ(proc.stdin_fd)) {
        Log(tempstr() << "# eof " << proc.proc);
        if (proc.tcp) {
            shutdown(proc.stdin_fd.value, SHUT_WR);
        }
        close(proc.stdin_fd.value);
        proc.stdin_fd = algo::Fildes();
    }
}

// Check if test timeout has been exceeded
static bool TimedOutQ() {
    atf_comp::FComptest *ct = atf_comp::_db.c_cur_comptest;
    double elapsed = algo::ElapsedSecs(atf_comp::_db.t0, algo::CurrSchedTime());
    return ct && elapsed >= ct->timeout * TimeoutScale();
}

// True when PROC keeps LINE: PROC keeps every line, or LINE starts with one of
// the prefixes in PROC's keep list.
static bool KeepLineQ(atf_comp::FProc &proc, strptr line) {
    bool ret = ch_N(proc.keep) == 0;
    ind_beg(algo::Sep_curs, prefix, proc.keep, '|') {
        ret = ret || algo::StartsWithQ(line, prefix);
    }ind_end;
    return ret;
}

// Read the output of PROC until the text UNTIL appears in it, and return
// everything read.  Each line is logged as `<proc> -> <line>`.  An empty UNTIL
// reads to end of file.  End of file before UNTIL fails the test with
// atf_comp.eof_before: a read the process never answered asserted nothing.  A test timeout
// during the read aborts the test; with KILL_ON_TIMEOUT it ends PROC instead --
// SIGTERM, then SIGKILL after five seconds -- and the read goes on to the end
// of file the kill produces.
tempstr atf_comp::ProcRead(atf_comp::FProc &proc, strptr until, bool kill_on_timeout DFLTVAL(false)) {
    tempstr result;
    bool matched = false;
    bool term_sent = false;
    bool kill_sent = false;
    algo::SchedTime term_time;
    while (!matched && !proc.in_eof) {
        algo::aryptr<char> msg = in_GetMsg(proc);
        if (msg.elems) {
            strptr line(msg.elems, msg.n_elems);
            if (proc.tcp && ch_N(line) > 0 && line.elems[line.n_elems - 1] == '\r') {
                line.n_elems--;
            }
            if (KeepLineQ(proc, line)) {
                result << line << "\n";
                LogOutput(proc, line);
                matched = ch_N(until) > 0 && algo::FindStr(result, until) != -1;
            }
            in_SkipMsg(proc);
        } else if (proc.in_eof) {
            // Flush remaining partial line (no trailing newline before EOF)
            i32 remaining = in_N(proc);
            if (remaining > 0) {
                strptr tail((char*)(proc.in_elems + proc.in_start), remaining);
                result << tail << "\n";
                LogOutput(proc, tail);
                in_RemoveAll(proc);
                matched = ch_N(until) > 0 && algo::FindStr(result, until) != -1;
            }
        } else {
            // No complete line yet — poll for more data or timeout
            struct pollfd pfd;
            pfd.fd = proc.in_iohook.fildes.value;
            pfd.events = POLLIN;
            int rc = poll(&pfd, 1, 1000);
            int err = errno;
            if (rc == 0) {
                if (TimedOutQ()) {
                    if (!kill_on_timeout) {
                        vrfy(false, tempstr() << "atf_comp.timeout"
                             << Keyval("proc", proc.proc)
                             << Keyval("until", until));
                    } else if (!term_sent) {
                        ProcKill(proc, SIGTERM);
                        term_time = algo::CurrSchedTime();
                        term_sent = true;
                    } else if (!kill_sent && algo::ElapsedSecs(term_time, algo::CurrSchedTime()) > 5.0) {
                        ProcKill(proc, SIGKILL);
                        kill_sent = true;
                    }
                }
            } else if (rc < 0 && err == EINTR) {
                // A signal interrupted the wait, and an interrupted wait is not
                // an end of output.  Reading it as one returns whatever the child
                // had written so far as its whole transcript, which the golden
                // comparison then reports as a mismatch -- the same verdict a
                // real behavior change produces, and one that appears only when
                // the machine is loaded enough to deliver a signal mid-poll.  So
                // the wait is retried and only end of file ends the read.
            } else if (rc < 0 || (pfd.revents & POLLNVAL)) {
                // A peer that closes a socket with bytes of ours still unread
                // resets the connection, and a poll that races the reset can
                // report POLLERR alone.  The read on the next pass returns the
                // reset as end of file, so only a failed poll or a closed
                // descriptor is an error.
                vrfy(false, tempstr() << "atf_comp.pollerror"
                     << Keyval("proc", proc.proc)
                     << Keyval("until", until)
                     << Keyval("revents", pfd.revents)
                     << Keyval("comment", rc < 0 ? strerror(err) : "pipe reported an error"));
            }
        }
    }
    vrfy(matched || ch_N(until) == 0 || kill_on_timeout, tempstr() << "atf_comp.eof_before"
         << Keyval("proc", proc.proc)
         << Keyval("until", until));
    return result;
}

// Take the variables the test's cluster wrote about itself into the replacement
// scope.
//
// A cluster picks its own ports and addresses as it starts, so a test script
// cannot name them ahead of time; the cluster writes what it chose to
// data/atfdb/var.ssim under its instance directory, and from there each becomes
// a $-substitution the script can use.  Which directory that is comes from the
// atfdb.testenv row marked vardir -- the same row that told the cluster where
// to put its instance in the first place, so the two cannot disagree.  A tree
// with no such row takes no variables.  Call it once the cluster is ready,
// which is when the file is complete.
void atf_comp::LoadClusterVar() {
    ind_beg(atf_comp::_db_testenv_curs,testenv,atf_comp::_db) if (testenv.vardir) {
        char *initdir = getenv(Zeroterm(tempstr()<<testenv.testenv));
        if (initdir != NULL) {
            ind_beg(algo::FileLine_curs, line, algo::DirFileJoin(initdir, "data/atfdb/var.ssim")) {
                atfdb::Var var;
                if (atfdb::Var_ReadStrptrMaybe(var, line)) {
                    SetVar(var.var, var.value);
                }
            }ind_end;
        }
    }ind_end;
}

// Wait for the cluster under PROC to print READY_FOR_TEST, then take the
// variables that cluster wrote about itself into the replacement scope
// (LoadClusterVar).
void atf_comp::WaitReadyForTest(atf_comp::FProc &proc) {
    tempstr out = ProcRead(proc, "READY_FOR_TEST");
    vrfy(algo::FindStr(out, "READY_FOR_TEST") != -1, tempstr() << "atf_comp.not_ready"
         << Keyval("proc", proc.proc)
         << Keyval("comment", "the cluster ended its output before announcing readiness"));
    LoadClusterVar();
}

// Wait for process to exit, drain remaining stdout into log.
// Internal helper: does the work without enforcing an expected exit code.
static void ProcWaitImpl(atf_comp::FProc &proc) {
    if (proc.tcp && proc.status == -1) {
        // A socket closed with bytes still unread in its receive buffer is
        // aborted: the kernel sends a reset and discards what it has not yet
        // transmitted.  A test that writes a burst the server is slow to take,
        // and closes with the server's greeting unread, loses the burst's tail
        // that way.  So the socket half-closes, which queues the end of the
        // stream behind every byte written, then discards what the server sent
        // and the test never read, and only then closes.
        if (ValidQ(proc.in_iohook.fildes)) {
            shutdown(proc.in_iohook.fildes.value, SHUT_WR);
            char buf[4096];
            while (read(proc.in_iohook.fildes.value, buf, sizeof(buf)) > 0) {
            }
        }
        if (ValidQ(proc.stdin_fd)) {
            close(proc.stdin_fd.value);
            proc.stdin_fd = algo::Fildes();
        }
        if (ValidQ(proc.in_iohook.fildes)) {
            close(proc.in_iohook.fildes.value);
            proc.in_iohook.fildes = algo::Fildes();
        }
        proc.status = 0;
        Log(tempstr() << "# close " << proc.proc);
    } else if (proc.subproc.pid != 0) {
        ProcWriteEof(proc);
        ProcRead(proc, "", true /*kill_on_timeout*/);
        if (ValidQ(proc.in_iohook.fildes)) {
            close(proc.in_iohook.fildes.value);
            proc.in_iohook.fildes = algo::Fildes();
        }
        algo_lib::ProcWait(proc.subproc);
        proc.status = algo_lib::ProcExitCode(proc.subproc);
        if (proc.killed) {
            proc.status = -1;
        }
        Log(tempstr() << "# exit " << proc.proc << " code:" << proc.status);
        // check the valgrind memcheck logs for errors -- one log per traced
        // pid (the wrap runs valgrind with --trace-children and a %p log).
        // An error goes into the test log, the record CheckOutput diffs
        // against the golden, so the comptest carrying the memory error
        // fails and the run's exit code becomes nonzero. No golden can be
        // taught to expect the line: capture and memcheck are two values of
        // the same -mode argument, and only a capture run writes a golden.
        //
        // Valgrind writes ERROR SUMMARY when the client shuts down, so a
        // process killed by a signal -- the drain's SIGTERM and SIGKILL, a
        // test's own ProcKill -- leaves a log with no summary, and so does
        // a log that is empty or unreadable. Such a log carries no count,
        // and reading its absence as zero would make a process valgrind
        // never finished judging indistinguishable from one it cleared. It
        // is named on the harness's own stdout instead: under
        // --trace-children a killed descendant is routine, so the notice
        // stays out of the test log, where it would fail the comptest.
        if (ch_N(proc.memcheck_log) > 0) {
            ind_beg(algo::Dir_curs, entry, tempstr() << proc.memcheck_log << ".*.log") {
                i32 nerror = 0;
                bool summary_found = false;
                ind_beg(algo::FileLine_curs, line, entry.pathname) {
                    algo::StringIter it(line);
                    GetWordCharf(it);
                    it.Ws();
                    if (SkipStrptr(it, "ERROR SUMMARY:") && TryParseI32(it, nerror)) {
                        summary_found = true;
                        break;
                    }
                }ind_end;
                if (!summary_found) {
                    prlog("# memcheck log has no ERROR SUMMARY (see " << entry.pathname << ")");
                } else if (nerror > 0) {
                    Log(tempstr() << "# memcheck errors: " << nerror << " (see " << entry.pathname << ")");
                    SysCmd(tempstr() << "cat " << entry.pathname);
                }
            }ind_end;
        }
    }
}

// Wait for process to exit; verify it exited with EXPECTED_EXIT (default 0).
// -1 means the process was killed (via ProcKill).  Procs that are never
// passed to ProcWait are not checked — ProcWaitAll drains them silently.
void atf_comp::ProcWait(atf_comp::FProc &proc, int expected_exit DFLTVAL(0)) {
    ProcWaitImpl(proc);
    vrfy(proc.status == expected_exit, tempstr()
         <<"atf_comp: "<<proc.proc<<" exited with code "<<proc.status
         <<", expected "<<expected_exit);
}

// Get process exit code
int atf_comp::ProcStatus(atf_comp::FProc &proc) {
    return proc.status;
}

// Wait for all procs in reverse order; do not enforce exit codes here.
// Tests assert exit codes with explicit ProcWait; this is the cleanup sweep.
void atf_comp::ProcWaitAll() {
    for (int i = proc_N() - 1; i >= 0; i--) {
        ProcWaitImpl(proc_qFind(i));
    }
}

// Begin part NAME of a comptest that runs several checks on one cluster: wait
// for every process the previous part started, then log `# part NAME`.
//
// A merged comptest starts its cluster once and runs one part per theme on it.
// A part that left a client connected would carry that client into the next
// part, where it could receive what the next part publishes, and its end lines
// would land wherever the test's final sweep falls.  So each part ends where the
// next begins.  The first call of a test waits for nothing, since the processes
// already running then are the cluster the parts share.
void atf_comp::Part(strptr name) {
    for (int i = proc_N() - 1; atf_comp::_db.part_nproc >= 0 && i >= atf_comp::_db.part_nproc; i--) {
        ProcWaitImpl(proc_qFind(i));
    }
    Log(tempstr() << "# part " << name);
    atf_comp::_db.part_nproc = proc_N();
}

// Compose the deny-write scaffold: a bash line that prepares the test's
// tempdir with a read-only directory ro/ and runs TOOL against it, so the
// tool's attempt to rewrite a file under ro/ fails with EACCES and the test
// can pin the failure report. SEED creates the fixture file(s) under ro/
// before the directory is locked.
//
// The mode must be restored after the run: a directory left at 0555 cannot
// be removed by the next run's tempdir cleanup, so one interrupted run would
// turn every following run of the test red. The EXIT trap restores the mode
// on normal exit and, via the TERM trap, when the harness timeout kill sends
// SIGTERM; the SIGKILL that follows 5s later still leaves the directory
// locked -- unfixable, the shell gets no chance to run anything.
//
// chmod-based write denial assumes an unprivileged runner: under
// CAP_DAC_OVERRIDE (e.g. a root container) the write succeeds and every
// deny-write golden mismatches. CI uses shell executors today, which run
// unprivileged.
tempstr atf_comp::DenyWriteCmd(strptr seed, strptr tool) {
    tempstr script;
    script << "cd $tempdir && mkdir ro && " << seed
           << " && trap \"chmod 755 ro\" EXIT && trap \"exit 1\" TERM && chmod 555 ro && " << tool;
    return tempstr() << "bash -c '" << script << "'"; // ignore:hand_quote -- quotes wrap the bash -c script, target syntax
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Return the command that runs the toolchain BIN, taken from environment
// variable ENVVAR where that is set and from the path otherwise, or an empty
// string when BIN is nowhere.
//
// A toolchain is optional, as it is to the native citests.  A comptest runs what
// needs one -- the Rust and Go clients, the bpf compiler -- only when it is
// installed, and it does so out of the test log, so the golden holds what runs on
// every machine and a machine without the toolchain passes the same golden.  What
// those steps print is checked in the test itself.
tempstr atf_comp::GetNativeTool(algo::strptr envvar, algo::strptr bin) {
    const char *value = getenv(Zeroterm(tempstr(envvar)));
    tempstr ret(value && *value ? strptr(value) : bin);
    int found = algo::SysCmd(tempstr() << "command -v " << strptr_ToBash(ret) << " >/dev/null", FailokQ(true));
    if (found != 0) {
        ret = tempstr();
    }
    return ret;
}

// Run CMD, with the test's variables substituted, out of the test log, and
// return what it printed on stdout and stderr.  A command that fails fails the
// test, naming CMD and its output.
tempstr atf_comp::EvalUnlogged(algo::strptr cmd) {
    tempstr cmd_eff = atf_comp::Subst(cmd);
    tempstr outfile = atf_comp::TempPath("unlogged.out");
    int rc = algo::SysCmd(tempstr() << "(" << cmd_eff << ") >" << strptr_ToBash(outfile) << " 2>&1", FailokQ(true));
    tempstr ret(algo::FileToString(outfile));
    vrfy(rc == 0, tempstr() << "atf_comp.unlogged_fail" << Keyval("cmd", cmd_eff) << Keyval("out", ret));
    return ret;
}
