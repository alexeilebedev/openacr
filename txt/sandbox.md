## Sandbox - sandbox management tool

The sandbox tool can be used to create, delete and manage sandboxes, which are copies of
the current directory that include any currently modified files.
The list of sandboxes is in `dev.sandbox`.

### Syntax

```
inline-command: sandbox -h
Usage: sandbox [-name:]<regx> [[-cmd:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [name]      regx            Sandbox name
    -create                     Create new sandbox and register in dev.sandbox
    -list                       List existing sandboxes
    -reset                      Reset sandbox to match current directory
    -clean                      Remove sandbox contents to save space
    -del                        Permanently delete sandbox
    -gc                         Run git gc in target dir
    [cmd]...    string          Command to execute in sandbox
    -diff                       Show diff after running command
    -files...   string          Shell regx to diff
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help an exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Operation

With `-create`, a new entry is added to `dev.sandbox` table and this implies `-reset`.
`-reset` brings the sandbox in correspondence with the current directory.

Sandbox performs actions in the following order: `-create`, `-reset`, `-clean`,
execute comand in sandbox, `-diff`, `-gc`, `-del`. This means you can create a sandbox, run a command
inside it, show the difference, and delete the sandbox in one line.

### Example: create a new sandbox

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

```
$ sandbox test "acr sandbox:test"
dev.sandbox  sandbox:test  comment:""
```

### Example: show difference from baseline

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

```
$ sandbox % -reset
```

### Example: test amc changes inside sandbox

This resets the sandbox to match current directory, runs amc in it, and rebuilds everything.
It can be used to test changes that might break compilation and put you in a dead-end situation.

```
$ sandbox amc -reset "amc && ai"
...
```

### Example: run interactive shell inside sandbox

```
$ sandbox amc -reset "bash -l"
```

### Example: run sandbox inside sandbox

Sandboxes can be nested without limit, which is possibly only useful when testing sandbox itself.
Note that since `git clone --local` is used, packed git files, while read-only, are still
shared with the parent directory, so overwriting them (which is only possible after changing
the mode) will corrupt the original git directory.

```
$ sandbox test -create "sandbox test2 -create 'command'" -del
```

### Tools that use sandbox:

* `amc_gc` uses sandbox to test changes to process in-memory database composition, and to clean up
includes
* `acr_ed` uses sandbox when invoked with `-sandbox` option
* `atf_fuzz` uses sandbox
* `atf_ci` uses sandbox to run tests marked `sandbox:Y

### Library support

The following functions in `algo_lib` help use sandboxing inside a process

```
inline-command: src_func algo_lib %Sandbox% -proto
// Return computed name for sandbox SANDBOX
cpp/lib/algo/lib.cpp:734: tempstr algo_lib::SandboxDir(algo::strptr sandbox) 
// Enter sandbox and remember previous directory
cpp/lib/algo/lib.cpp:745: void algo_lib::SandboxEnter(algo::strptr sandbox) 
// Change to the directory that was current before sandbox mode
// Must be balanced with SandboxEnter
cpp/lib/algo/lib.cpp:756: void algo_lib::SandboxExit() 
```
