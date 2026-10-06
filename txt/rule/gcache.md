## gcache: Rules
<a href="#gcache-rules"></a>

gcache is not a module.  It runs once per compile, in the build tool's process
tree, and holds no shared memory, so it carries no Steps section; what it
carries are the decisions its source cannot state.  The usage is in
[/txt/exe/gcache/README.md](/txt/exe/gcache/README.md).

### Invariants
<a href="#invariants"></a>

The key of a coverage compile carries the directory the compile runs in.  An
object compiled for coverage names the profile file it writes at run time, and
gcc anchors that name at the working directory: the object's absolute path with
`.gcda` for `.o`, or that path mangled into one component under `-fprofile-dir`.
The notes file records the directory as well.  Two checkouts of one commit
preprocess to the same text under the same relative command line, so a key of
those two alone hands the second checkout an object that writes its profile
under the first checkout's path, and gcov measures nothing for it.  A coverage
entry therefore serves the checkout that wrote it, and a bare object entry
serves every checkout.  `gcache.CoverageCwd` pins this.  gcache's own
precompiled header entry carries no directory under the same flags: a header
compiles to no object and names no profile file, so the `.gch` holds no `.gcda`
name and the object compiled against it is what carries the name.

`-fprofile-prefix-path=<cwd>` with `-fprofile-dir` makes the embedded name
relative to the checkout, which is what keeps atf_cov's gcov command lines
short and the same on every machine.  It does not make the entry shareable:
the flag's own value differs per checkout, and the notes still record the
directory.  A shared coverage entry would need gcache to recognize that
combination of flags, and that special case is declined.

A coverage entry is one file holding the object and its notes, written by one
rename, so a hit restores both halves from one compile or neither.  Which
compiles are coverage compiles is decided from the flags alone, over every
spelling gcc accepts, and `gcache.CoverageSpelling` pins that decision through a
driver that runs no compiler.

Every step of `-install`, `-enable` and `-disable` has its status read.  A
failure is reported once, as a `gcache.error`, and reaches the exit code; `done`
is printed only after every step has succeeded.  A request that cannot be
honored leaves the `.gcache` link as it was, so a mistyped `-dir` does not
disable a working cache on its way to the error.  A publish that fails costs
the next build a miss and is a `gcache.warning` that leaves the exit code alone;
a restore that fails costs the build its object and is a `gcache.error` that
raises it.

An entry is a hit only when it is a regular file with bytes in it.  A build that
ran out of disk leaves an entry of no bytes behind.  An earlier per-file format
wrote a directory where this one writes a single file.  A hit writes nothing
back, so an unusable entry treated as a hit would answer every later compile the
same way.  A compile that finds one compiles, and its publish replaces whatever
stood at the path.  The same rule governs a publish: an object of no bytes is
not published.

The exit code is a sum.  The wrapped compiler's status is one term, and each
failure that keeps the run from delivering its object adds one.  So a compile
that exits 1 beside a failed restore exits 2, and neither hides the other.

The cleanup keeps an entry by moving its modification time to now, because the
age pass reads that time and not the log.  It moves the time through the path,
which fails on an entry that is gone, so a log line naming an entry that the
budget or an earlier pass already deleted creates nothing.  A line whose
compile used a precompiled header refreshes the `.gch` as well: the compiles
that hit a header only read it, and its time would otherwise stand still at the
moment it was built.

A comptest of gcache asserts what holds on every driver the tree builds with.
gcc's own long forms of the coverage flags and its precompiled header mechanism
each hold on one platform, so a test that needs one of them reaches it through
`test/gcache/cc`, a driver script that runs no compiler: it takes any flag,
expands one level of `#include` with gcc's line markers under `-E`, writes an
object and notes for a compile and a `.gch` for an output named as one.  The
cleanup's refresh of a header entry is pinned by a forged log line, since the
cleanup reads the log and nothing else.  A file's freshness is read against the
time it was backdated to, so no stage waits out a clock, and an errno that
differs by platform is masked in the golden.

### Decisions
<a href="#decisions"></a>

The precompiled header marker is a function prototype,
`void __gcache_pragma_pch_preprocess();`, that nothing defines.  gcache finds
the header to precompile by reading the preprocessed text, where a macro no
longer exists.  A prototype passes through the preprocessor as it is, and it
can repeat without harm, so any number of headers can carry it.  gcache splices the
precompiled header in by replacing the header's text with
`#pragma GCC pch_preprocess "<file>.gch"` at the top of the preprocessed file.

Age alone does not bound the cache in bytes.  A machine whose builds span
several configurations can write tens of gigabytes inside the one-week window,
and that fills the disk the builds share.  So the cleanup evicts entries in
last-use order until the cache fits `-maxmb`.  An entry an active compile just
wrote carries a fresh time, so it is the last candidate.
