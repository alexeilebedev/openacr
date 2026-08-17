## Tutorial 4: Command lines
<a href="#tutorial-4-command-lines"></a>

> **Run this in a sandbox.**  The commands below create a
> target, add fields to it, create an ssimfile and delete an
> index again, so they change the schema and the source tree of
> whichever checkout runs them.  `wt tut_make -reset "bash -l"`
> gives you a shell in a sandbox, reset to match your current
> directory, and nothing you do there reaches your working tree
> — see [wt](/txt/exe/wt/README.md).

Every amc-generated program comes with full command-line
support: a parsed-argv struct, a usage screen, shell
completion, and a printer that turns the struct back into an
argv.  Command-line options are declared as fields on a ctype
in the `command` namespace, and the fields can be of any type
— including types you define yourself, since amc treats
built-in types the same way it treats new ones.

For every executable, amc generates:

- code to read the command line from `argc`/`argv` into the
  `command.<exe>` struct,
- a printer that turns the struct back into an argv array
  (used by [Exec](/txt/exe/amc/reftype/Exec.md) to drive
  child processes),
- the `-help` usage screen,
- shell completion entries for `acr_compl`.

For the full reference see
[Command-line parsing](/txt/exe/amc/cmdline.md).  This tutorial
walks through declaring a couple of flags and seeing them
appear in `-help`.

To begin, create a new executable from a fresh commit.

    $ acr_ed -create -target sample -write
    ...

This will create a new program called `sample`:

    $ sample
    Hello, World!

The program already comes with some built-in options.

    $ sample -h

    Usage: sample [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

Amc knows that the ctype `command.sample` is the command line for sample because of the `ccmdline`
record inserted by `acr_ed`.

    $ acr ccmdline:command.sample
    dmmeta.ccmdline  ctype:command.sample  read:Y  comment:""

The command line itself, as created by acr_ed, is a simple ctype:

    dmmeta.ctype  ctype:command.sample  comment:""
      dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"
                  
Let's add a flag.

    $ acr_ed -create -field command.sample.flag -arg bool -write -comment "An important flag"
    ...
    $ ai sample
    ...
    $ sample -h

    Usage: sample [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -flag             An important flag. default: false
        ^^^^^ it was added!
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

The help screen has been updated.  Let's modify `sample`'s
Main to print the flag.  OpenACR convention avoids displaying
raw values — raw output is an injection vector — and prefers
machine-readable key/value form via the `Keyval` helper.

    void sample::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
    }

    $ ai sample
    ...
    $ sample -flag
    flag:Y

### Boolean options
<a href="#boolean-options"></a>

A boolean flag accepts several true forms — `-flag`, `-flag:Y`,
`-flag:true`, `-flag:yes`, `-flag:on` — and several false forms
— omit the option entirely, or write `-flag:N`, `-flag:false`,
`-flag:no`, `-flag:off`.  OpenACR prints booleans in
database-friendly form, using `Y` and `N`.

If a boolean option appears more than once on the command line,
its in-memory value is incremented each time.  That's how `-v
-v -v` counts up verbosity and `-d -d` counts up debug level.

There is no flag bundling.  Each option is a separate argv
entry.

### Default values
<a href="#default-values"></a>

If the `dflt` attribute of the field describing a command-line
argument is non-empty, that option may be omitted; otherwise
it is required.  Boolean options can always be omitted —
omitting a flag is never an error.

Here is an example of the default value, in this case `"data"`:

    $ acr field:command.%.in | head -1
    dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"

### Integer options
<a href="#integer-options"></a>

Add an integer-valued option:

    $ acr_ed -create -field command.sample.val -dflt 0 -arg u32 -write -comment "A value"
    ...
    $ ai sample

Let's update the source code to print the value:

    void sample::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
        prlog(Keyval("val",_db.cmdline.val));
    }

    ...
    $ sample -val:4
    flag:N
    val:4

When parsing the command line, a space after the option name
and a `:` are interchangeable: `-val:4` is the same as `-val 4`.
The `:` form has a stylistic edge — `value` is unambiguously
part of the previous option without the reader needing to know
what the command expects, which improves error messages when
things go wrong.

### Anonymous (positional) options
<a href="#anonymous-positional-options"></a>
To allow `sample 4` (positional) as a synonym for `sample -val:4`,
add a `dmmeta.anonfld` row for the field and rebuild:

    $ echo dmmeta.anonfld field:command.sample.val | acr -insert -write
    $ amc && abt -install sample
    $ sample 5
    flag:N
    val:5

### Other data types
<a href="#other-data-types"></a>

Any type that can be read from a string is a valid option type
— there's nothing special about built-ins.  For an IPv4 option:

    $ acr_ed -create -field command.sample.ip -arg ietf.Ipv4 -comment "Enter this IP" -write

Reading ctypes from strings is controlled by `dmmeta.cfmt`,
where a user-supplied parser can be plugged in with `extrn:Y`.
See [cfmt](/txt/exe/amc/cfmt.md) for the full story.

### Bash completion
<a href="#bash-completion"></a>

OpenACR offers shell completion for any program in its domain
via `acr_compl`.  Tell bash to call `acr_compl` whenever a
command line starts with a known program name; `sample` is now
known, so `acr_compl` recognizes it:

    $ abt -install acr_compl
    $ acr_compl -install
    # This is a script suitable for inclusion in .bash_profile
    complete -o default -o nospace -C acr_compl sample abt acr acr_compl acr_ed acr_in acr_my amc ...
                                                ^^^ -- it knows!

Let's manually hook it up:

    complete -o default -o nospace -C acr_compl sample

And test it:

    $ sample -v<tab>
    -val:<int>  -verbose    -version

### Completing from a table
<a href="#completing-from-a-table"></a>

Completion can pull values from any ssim table on disk,
loading it on demand:

    $ acr ctype:acr.FC<tab>
    acr.FCdflt    acr.FCheck    acr.FCppfunc  acr.FCtype

Let's test it on our program. We'll create and populate a brand new table, and add an option to sample
which will be completed with values from that table.

    $ acr_ed -create -ssimfile dmmeta.mytable -write
    $ for X in sampled{10..99}; do echo "dmmeta.mytable mytable:$X"; done | acr -insert -write
    ...
    $ acr mytable -print:N
    report.acr  n_select:90  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

So far so good, we have 90 values in our table.
Now let's create a new command line option:

    $ acr_ed -create -field command.sample.mytable -arg dmmeta.Mytable -reftype Pkey -dflt '""' -comment "A very useful option" -write
    ...
    $ ai sample

And now we're ready to test it:

    $ sample -mytable:abcd2<tab>
    abcd20  abcd21  abcd22  abcd23  abcd24  abcd25  abcd26  abcd27  abcd28  abcd29

It works! Bash now magically knows to fetch values from `mytable` whenever `sample` is being invoked.
As expected, inside sample this value is accessible as _db.cmdline.mytable. It has the type `algo::Smallstr50`,
because that's the type of the primary key of `mytable`.

### Loading a table as program input
<a href="#loading-a-table-as-program-input"></a>

Now that `sample` accepts a `-mytable` option, the program
should be able to look up the corresponding record at runtime.
To do that, we declare a `finput` in the `sample` namespace,
backed by the `dmmeta.mytable` ssimfile:

    $ acr_ed -create -finput -target sample -ssimfile dmmeta.mytable -indexed -write

We verify that there are two access paths from `sample` to `sample.FMytable`, one via the memory pool `sample.FDb.mytable`,
the other via hash table `sample.FDb.ind_mytable`:

    / sample.FDb
    |
    |Lary mytable------->/ sample.FMytable
    |Thash ind_mytable-->|
    -                    |
                         |
                         -

We modify `sample` as follows:

    void sample::Main() {
        sample::FMytable *mytable=ind_mytable_Find(_db.cmdline.mytable);
        prlog((mytable ? "Yes, record found" : "No such record"));
    }

Then build and run:

    $ sample -mytable:tt
    No such record
    $ sample -mytable:abcd51
    Yes, record found

### Regx options
<a href="#regx-options"></a>

Convert `-mytable` to a `RegxSql` option and have `sample` scan
the `_db.mytable` array, printing every matching record:

    $ echo dmmeta.field field:command.sample.mytable reftype:RegxSql | acr -merge -write
    $ amc

    ...
    void sample::Main() {
        ind_beg(_db_mytable_curs,mytable,_db) {
            if (Regx_Match(_db.cmdline.mytable,mytable.mytable)) {
                prlog(mytable.mytable);
            }
        }ind_end;
    }

Now we run the new sample to check the result:

    $ sample -mytable:abcd5%
    abcd50
    abcd51
    abcd52
    abcd53
    abcd54
    abcd55
    abcd56
    abcd57
    abcd58
    abcd59

Yes, it finds and prints the keys of the records it found.

Notice that we no longer need the hash table `sample.FDb.ind_mytable`, since we 
converted from a hash lookup to a linear scan. Let's delete the hash table:

    $ acr_ed -del -field sample.FDb.ind_mytable -write
    $ amc_vis sample.%

    / sample.FDb
    |
    |Lary mytable-->/ sample.FMytable
    -               |
                    |
                    -

Indeed, only one access path remains.

### `-version`
<a href="#-version-"></a>

Every executable carries an embedded `dev.gitinfo` record —
build date, the leading characters of the last commit, the
last committer's email, and the toolchain/config string.  Run
`-version` to print it.

    $ sample -version

The same data is recoverable from the binary directly with
`strings <binary> | grep gitinfo:`.

### `-sig`
<a href="#-sig-"></a>

For each target that loads ssim input tables, amc computes a
SHA-1 signature hash of the input schema.  `-sig` prints the
signatures.  Peers exchange them at startup; a mismatch means
the schemas have diverged and decoding is unsafe.  See
[Dispsigcheck](/txt/exe/amc/dispatch.md#signatures-dispsig-dispsigcheck).

### Printing command lines
<a href="#printing-command-lines"></a>

amc generates a printer alongside the parser — turning a
`command.<exe>` struct back into a Bash-escaped argv string.
Modify `sample`'s Main:

    void sample::Main() {
        prlog(sample_ToCmdline(_db.cmdline));
    }

Now let's run sample with some options:

    $ sample
    bin/sample  0

The output is a properly quoted shell command:

    $ sample -val:33 -mytable:$'\nblah'
    bin/sample  33 -mytable:$'\nblah'

Since `-val` is an anonymous field, `sample_ToCmdline` omits the name.

### Typed subprocesses
<a href="#typed-subprocesses"></a>

One more feature: invoking a subprocess in a strictly typed
way.  amc emits a `command::sample_proc` helper that wraps
`fork`/`exec` without going through `system()`.  `system()` is
undesirable because it disables SIGINT and SIGTERM, making
processes built on it effectively unkillable from the
terminal.  amc's spawner doesn't trap interrupts, and uses
`DieWithParent` so the child is reaped if the parent exits.

Modify the source code of sample.cpp as follows:

    #include "include/gen/command_gen.h"
    #include "include/gen/command_gen.inl.h"

    void sample::Main() {
        prlog(_db.cmdline.val);
        if (_db.cmdline.val>0) {
            command::sample_proc sample;
            sample.cmd.val = _db.cmdline.val-1;
            sample_Exec(sample);
        }
    }

This will recursively invoke the `sample` subprocess (don't try it with large values!)

    $ sample 3
    3
    2
    1
    0

Here is the code amc generated for `amc_proc`:

    $ amc command.sample_proc

    // --- command.sample_proc
    struct sample_proc { // command.sample_proc: Subprocess:
        algo::cstring   path;      //   "bin/sample"  path for executable
        command::sample    cmd;       // command line for child process
        algo::cstring   stdin;     // redirect for stdin
        algo::cstring   stdout;    // redirect for stdout
        algo::cstring   stderr;    // redirect for stderr
        pid_t           pid;       //   0  pid of running child process
        i32             timeout;   //   0  optional timeout for child process
        i32             status;    //   0  last exit status of child process
        sample_proc();
        ~sample_proc();
    private:
        // reftype of command.sample_proc.sample prohibits copy
        sample_proc(const sample_proc&){ /*disallow copy constructor */}
        void operator =(const sample_proc&){ /*disallow direct assignment */}
    };
    ...

### Verbosity
<a href="#verbosity"></a>

`prlog` is a macro that opens an implicit `tempstr()`, so
`prlog(a << b << c)` works without ceremony.  Companion macros:

- `prerr(...)` — same idea, but stderr.
- `prcat(category, ...)` — print only when the named log
  category is enabled.  `prcat(debug, ...)` requires `-d`;
  `prcat(verbose, ...)` requires `-v`.  Categories can also
  be selected with `-trace:verbose,debug`.

All macros live in `include/define.h`.

When a command line is rendered with `<ns>_ToCmdline` for a
child process, the child inherits a lower verbosity level than
its parent.  Adding `-v`s on the outer command thus controls
how deep verbose logging penetrates a tool tree.

Don't forget to `git reset --hard` to clean up any local changes.
