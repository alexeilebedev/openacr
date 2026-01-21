## sandbox - Sandbox - sandbox management tool
<a href="#sandbox"></a>

The sandbox tool can be used to create, delete and manage sandboxes, which are copies of
the current directory that include any currently modified files.
The list of sandboxes is in `dev.sandbox`.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Operation](#operation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tools that use sandbox:](#tools-that-use-sandbox)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: create a new sandbox](#example-create-a-new-sandbox)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: run command in sandbox](#example-run-command-in-sandbox)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: show difference from baseline](#example-show-difference-from-baseline)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list sandboxes and their current state](#example-list-sandboxes-and-their-current-state)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: reset all sandboxes](#example-reset-all-sandboxes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: test amc changes inside sandbox](#example-test-amc-changes-inside-sandbox)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: run interactive shell inside sandbox](#example-run-interactive-shell-inside-sandbox)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: run sandbox inside sandbox](#example-run-sandbox-inside-sandbox)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [sandbox - Internals](/txt/exe/sandbox/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
sandbox: Sandbox - sandbox management tool
Usage: sandbox [-name:]<regx> [[-cmd:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [name]      regx            Sandbox name
    -create                     Create new sandbox and register in dev.sandbox
    -list                       List existing sandboxes
    -reset                      Reset sandbox to match current directory
    -clean                      Remove sandbox contents to save space
    -shell                      Open interactive shell inside sandbox
    -del                        Permanently delete sandbox
    -gc                         Run git gc in target dir
    [cmd]...    string          Command to execute in sandbox
    -diff                       Show diff after running command
    -files...   string          Shell regx to diff
    -refs       string  "HEAD"  Additional list of refs to fetch into sandbox
    -ref        string  "HEAD"  Reset to this ref
    -q                          Quiet mode
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Operation
<a href="#operation"></a>

With `-create`, a new entry is added to `dev.sandbox` table and this implies `-reset`.
`-reset` brings the sandbox in correspondence with the current directory.

Sandbox performs actions in the following order: `-create`, `-reset`, `-clean`,
execute comand in sandbox, `-diff`, `-gc`, `-del`. This means you can create a sandbox, run a command
inside it, show the difference, and delete the sandbox in one line.

### Tools that use sandbox:
<a href="#tools-that-use-sandbox"></a>

* `amc_gc` uses sandbox to test changes to process in-memory database composition, and to clean up
includes
* `acr_ed` uses sandbox when invoked with `-sandbox` option
* `atf_fuzz` uses sandbox
* `atf_ci` uses sandbox to run tests marked `sandbox:Y

### Example: create a new sandbox
<a href="#example-create-a-new-sandbox"></a>

```
sandbox test -create
```

This initializes a new sandbox. The sandbox directory is `temp/sandbox.test`
and includes any files that are modified in the current directory. But these changes
are committed with the comment 'baseline'. In addition, any files listed in `dev.sbpath` table
are added to the sandbox. Importantly, this includes any executables (`build/` directory)
and the build cache link (`.gcache`)
The sandbox can be reset to match the current directory with `sandbox test -reset`.
It can be brought back to this baseline state with `sandbox test -clean`.
Sandboxes are built with `git clone --local` so they actually use little space.

### Example: run command in sandbox
<a href="#example-run-command-in-sandbox"></a>

```
$ sandbox test "acr sandbox:test"
dev.sandbox  sandbox:test  comment:""
```

### Example: show difference from baseline
<a href="#example-show-difference-from-baseline"></a>

```
$ sandbox amc -reset "echo test >> cpp/sandbox.cpp" -diff
sandbox.reset  sandbox:amc  dir:temp/sandbox.amc
diff --git a/cpp/sandbox.cpp b/cpp/sandbox.cpp
index 0889b61..20d42ef 100644
--- a/cpp/sandbox.cpp
+++ b/cpp/sandbox.cpp
@@ -173,3 +173,4 @@ void sandbox::Main() {
         }ind_end;
    }
}
+test
```

### Example: list sandboxes and their current state
<a href="#example-list-sandboxes-and-their-current-state"></a>

```
$ sandbox % -list
Sandbox   Size  Clean  Path                   Comment
acr_ed    656M  Y      temp/sandbox.acr_ed    sandbox for testing acr_ed changes
amc       659M  Y      temp/sandbox.amc       sandbox for running amc commands
amc_gc    664M  N      temp/sandbox.amc_gc    sandbox for garbage collection tool
atf_ci    664M  N      temp/sandbox.atf_ci    sandbox for CI testing
atf_fuzz  1.3G  Y      temp/sandbox.atf_fuzz  sandbox for fuzzing
```

### Example: reset all sandboxes
<a href="#example-reset-all-sandboxes"></a>

```
$ sandbox % -reset
```

### Example: test amc changes inside sandbox
<a href="#example-test-amc-changes-inside-sandbox"></a>

This resets the sandbox to match current directory, runs amc in it, and rebuilds everything.
It can be used to test changes that might break compilation and put you in a dead-end situation.

```
$ sandbox amc -reset "amc && ai"
...
```

### Example: run interactive shell inside sandbox
<a href="#example-run-interactive-shell-inside-sandbox"></a>

```
$ sandbox amc -reset "bash -l"
```

### Example: run sandbox inside sandbox
<a href="#example-run-sandbox-inside-sandbox"></a>

Sandboxes can be nested without limit, which is possibly only useful when testing sandbox itself.
Note that since `git clone --local` is used, packed git files, while read-only, are still
shared with the parent directory, so overwriting them (which is only possible after changing
the mode) will corrupt the original git directory.

```
$ sandbox test -create "sandbox test2 -create 'command'" -del
```

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -name -- Sandbox name
<a href="#-name"></a>

#### -create -- Create new sandbox and register in dev.sandbox
<a href="#-create"></a>

#### -list -- List existing sandboxes
<a href="#-list"></a>

#### -reset -- Reset sandbox to match current directory
<a href="#-reset"></a>

#### -clean -- Remove sandbox contents to save space
<a href="#-clean"></a>

#### -shell -- Open interactive shell inside sandbox
<a href="#-shell"></a>

#### -del -- Permanently delete sandbox
<a href="#-del"></a>

#### -gc -- Run git gc in target dir
<a href="#-gc"></a>

#### -cmd -- Command to execute in sandbox
<a href="#-cmd"></a>

#### -diff -- Show diff after running command
<a href="#-diff"></a>

#### -files -- Shell regx to diff
<a href="#-files"></a>

#### -refs -- Additional list of refs to fetch into sandbox
<a href="#-refs"></a>

#### -ref -- Reset to this ref
<a href="#-ref"></a>

#### -q -- Quiet mode
<a href="#-q"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`sandbox` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.sandbox](/txt/ssimdb/dev/sandbox.md)|Registered sandbox|
|[dev.sbpath](/txt/ssimdb/dev/sbpath.md)|Extra files to copy into the sandbox|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

