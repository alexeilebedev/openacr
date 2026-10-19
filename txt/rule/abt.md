## abt: Rules
<a href="#abt-rules"></a>
abt builds the targets a regex selects, and every target they depend on, in each
build directory that `-cfg` selects.  A build directory is a row of
`dev.builddir`: a platform, a compiler, a configuration and an architecture.
Each one holds one object per source, an archive per library and an output per
executable or shared object.  The user documentation is
[/txt/exe/abt/README.md](/txt/exe/abt/README.md).

### Invariants
<a href="#invariants"></a>
**An executable is compiled position-dependent, and every object a shared object
links is compiled position-independent.**  Say the command line names an
executable and a Python module, and both link one library.  ld refuses an object
compiled position-dependent inside a shared object, and `-fPIC` costs a few
percent on an executable's hot path: a throughput benchmark measured 2-3% on a
shared-memory fanout.  So each
build directory runs two passes.  The plain pass builds what a target the command
line named that is no shared object needs.  The so pass builds what a shared
object the command line named needs, under the build directory's `so/`, and adds
the `dev.tool_opt` rows of type `SOCC` to every compile.  A library both need is
built once in each pass.  `Main_MarkPass` marks each target with its passes, and
every phase that walks the selection walks one pass's targets (`InPassQ`).

**A pass owns its directory.**  Objects, archives and links of the so pass go to
`<path>/so` (`GetPassDir`), and `-install` links `lib/<target>.so` there.  A
dependency of a target in a pass is in the same pass, so a link reads only the
archives its own pass built.
