## atf_cov: Rules
<a href="#atf_cov-rules"></a>

`atf_cov` measures line coverage from the profile data a GCC-instrumented binary
writes, and it guards each target's coverage against the floor recorded in
`dev.tgtcov`.  The usage is in [/txt/exe/atf_cov/README.md](/txt/exe/atf_cov/README.md).
What follows are the decisions its source cannot state.

### Invariants
<a href="#invariants"></a>

**A run is judged whole before any target is judged.**  A coverage figure comes
from two files the run has to find: the program graph beside each object and
the profile data the binary wrote when it exited.  When some of them go missing,
the arithmetic still works.  The lines nobody counted are absent, and what comes
out is an accurate figure for a smaller program.  The tests whose data went
missing are the same tests that exercise the rest of the tree, so every target
the run did reach also reads low.  Judged target by target, such a run prints
dozens of floor breaches, and each one names a target of the branch under test.
So `-check` fails once with `atf_cov.coverage_lost` when a merge directory came
back empty or a target with a floor above zero produced no data, and it compares
no target against its floor.  A floor of zero says nothing exercises the target
today, so such a target is allowed to produce no data.

**A capture never writes a run that lost data.**  `-capture` writes each
measurement into `dev.tgtcov` as the new floor.  Capturing a run that lost a
directory would lower the gate by exactly what went missing, and nothing would
say so.  `-capture` therefore refuses with `atf_cov.capture_refused` on the same
test `-check` uses.

**A floor comes down only by a hand edit.**  The one way to lower a floor is a
changed row in `data/dev/tgtcov.ssim`, in a commit somebody reads.  A target
whose last test was deleted keeps failing `-check` until that edit is made.

**The merge directory is the unit of loss.**  Each citest of the coverage cijob
writes its profile data into a directory of its own.  One directory that comes
back empty takes with it every target only that citest exercises, and those
targets look exactly like untested code.  So every directory reports itself on
an `atf_cov.merge` line, which names the citest whose data went missing.

**Profile files are named relative to the checkout.**  GCC names a `.gcda` file
after its object's path, with each `/` spelled `#`.  The coverage build passes
`-fprofile-prefix-path` so that path is relative:
`build/coverage/cpp.acr.check.o` writes `build#coverage#cpp.acr.check.gcda`.
`atf_cov` reads the name backwards to find the `.gcno`, and a `build` symlink in
the coverage directory makes the recovered path resolve.  With absolute names,
the same tree measured in two checkouts produced two sets of names.  The
recovered graph path then pointed into whichever checkout compiled the object,
and every name grew by the length of the checkout path.

**No `gcov` command grows without bound.**  A full run leaves about a thousand
`.gcda` files in a directory.  A shell command reaches the kernel as one
argument, and the kernel refuses one longer than 128K.  A single command naming
every file sat close enough to that ceiling to cross it on a machine with a
longer checkout path.  `atf_cov` cuts the list into commands of about 64K each.

**Progress goes to the logfile, and verdicts go to stderr.**  A merge logs every
directory it reads and every ssimfile it writes, thousands of lines in all, and
`-logfile` exists to keep them off the console.  A verdict is the answer the
caller asked for.  A CI job keeps its log and may lose the logfile, so
`atf_cov.merge`, `atf_cov.gcov_fail` and the check results stay on stderr.

**Code only a kill test reaches scores zero.**  An instrumented binary keeps its
counters in memory and writes them as it exits, and `SIGKILL` gives it no exit.
Recovery code that exists for a process dying without warning can be driven
only by a test that kills the process, and that test contributes nothing to the
measurement.  A floor far below its neighbors is then a fact about how the code
is reached.  The `dev.tgtcov` row's comment says which targets are low for this
reason.  Raising such a floor with a test that lets the process exit cleanly
exercises the ordinary shutdown path, which is a different test.
