## Command Lines

All programs created with amc come with full command line support. A command line is just a struct,
and options are described as fields. All one needs to do is keep adding fields, and amc and the rest
of programs will take care of all chores.

To start this tutorial, let's start with a fresh commit, and create a new executable.

    $ acr_ed -create -target abc -write
    ...

This will create a new program called `abc`:

    $ abc
    Hello, World!

The program already comes with some built-in options.

    $ abc -h


    Usage: abc [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

Amc knows that the ctype `command.abc` is the command line for abc because of the `fcmdline`
record inserted by `acr_ed`.

    $ acr fcmdline:abc.%
    dmmeta.fcmdline  field:abc.FDb.cmdline  read:Y  comment:""

The command line itself, as created by acr_ed, is a simple ctype:

    dmmeta.ctype  ctype:command.abc  comment:""
      dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"
                  
Let's do something func. First, let's add a flag. Because what kind of command doesn't have a flag?

    $ acr_ed -create -field command.abc.flag -arg bool -write -comment "An important flag"
    ...
    $ ai abc
    ...
    $ abc -h

    Usage: abc [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -flag             An important flag. default: false
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

As you can see, the help screen has been updated. Let's modify `abc`'s main to print the flag.
It is an OpenACR convention to avoid displaying raw values. Output should be machine
readable and never susceptible to an injection attack. So we'll print the value
as a key-value pair (`Keyval` is a C++ template helper):

    void abc::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
    }

    $ ai abc
    ...
    $ abc -flag
    flag:Y

### Boolean Options

There are several ways to specify a flag on a command line: `abc -flag`, `abc -flag:Y`,
`abc -flag:true` as well as `-flag:yes` and `-flag:on` all work.
To specify a false value, we could either omit the option
altogether, or write `abc -flag:N` or `abc -flag:false` or `-flag:no` or `-flag:off`.
The OpenACR convention is to
print booleans in a datatabase-friendly way, using `Y` and `N`.

If a boolean option is specified more than one on the command line, the value
in memory is incremented by one each time. This allows counting how many times the value
was specified, which is useful for controlling verbosity and debug levels.

There is no bundling of options. Each option must be specified as a separate 
argument.

### Default Values

If the `dflt` attribute of the field describing a command line argumentn is non-empty,
that option may be omitted. Otherwise it is required.
Boolean options can always be omitted (i.e. omitting a boolean option, a.k.a. a flag
from the command line is never an error).

Here is an example of the default value, in this case `"data"`:

    $ acr field:command.%.in | head -1
    dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"


### Integer Options

Let's continue. It's time to add an integer-valued option:

    $ acr_ed -create -field command.abc.val -dflt 0 -arg u32 -write -comment "A value"
    ...
    $ ai abc

Let's update the source code to print the value:

    void abc::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
        prlog(Keyval("val",_db.cmdline.val));
    }

    ...
    $ abc -val:4
    flag:N
    val:4

When parsing the command line, space after option and `:` are treated interchangeably:
`-val:4` is the same as `-val value`.

It may be a stylistic point, but when the option and its value form one word, it's clear at the syntax level,
without knowing what the command expects, that `value` is not a stand-alone option or a positional argument.
It can make error messages more legible when things go wrong.

### Anonymous Options

Let's say we want abc to be able to write `abc 4` without specifying the parameter name `-val`.
We can use `anonfld` for this. We add the appropriate record to the anonfld table, then re-generate
and rebuild.

    $ echo dmmeta.anonfld field:command.abc.val | acr -insert -write
    $ amc && abt -install abc
    $ abc 5
    flag:N
    val:5

### Other Data Types

A command line option can be of any data type whatsoever. Any type that can be read from 
a string is a valid type. For instance, to input an IPv4 address, use

    $ acr_ed -create -field command.abc.ip -arg ietf.Ipv4 -comment "Enter this IP" -write

Reading ctypes from a string is controlled by the `dmmeta.cfmt` table, where a user-provided
function may be specified with `extrn:Y`. This is described in depth in the chapter on string conversion.
    
### Bash Command Completion

OpenACR offers completion of command line parameters for any program in its domain.
The magic is implemented in the command `acr_compl`. To use, we instruct bash
to call `acr_compl` whenever a command line start with one of known command names.
In this case, abc is now a known name, so `acr_compl` already knows about it:

    $ abt -install acr_compl
    $ acr_compl -install
    # This is a script suitable for inclusion in .bash_profile
    complete -o default -o nospace -C acr_compl abc abt acr acr_compl acr_ed acr_in acr_my amc ...
                                                ^^^ -- it knows!

Let's manually hook it up:

    complete -o default -o nospace -C acr_compl abc

And test it:

    $ abc -v<tab>
    -val:<int>  -verbose    -version

### Completing From Table

Auto-complete works with the contents of any table, loading it as necessary to provide completions.

    $ acr ctype:acr.FC<tab>
    acr.FCdflt    acr.FCheck    acr.FCppfunc  acr.FCtype

Let's test it on our program. We'll create and populate a brand new table, and add an option to abc
which will be completed with values from that table.

    $ acr_ed -create -ssimfile dmmeta.mytable -write
    $ for X in abcd{10..99}; do echo "dmmeta.mytable mytable:$X"; done | acr -insert -write
    ...
    $ acr mytable -print:N
    report.acr  n_select:90  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

So far so good, we have 90 values in our table.
Now let's create a new command line option:

    $ acr_ed -create -field command.abc.mytable -arg dmmeta.Mytable -reftype Pkey -dflt '""' -comment "A very useful option" -write
    ...
    $ ai abc

And now we're ready to test it:

    $ abc -mytable:abcd2<tab>
    abcd20  abcd21  abcd22  abcd23  abcd24  abcd25  abcd26  abcd27  abcd28  abcd29

It works! Bash now magically knows to fetch values from `mytable` whenever `abc` is being invoked.
As expected, inside abc this value is accessible as _db.cmdline.mytable. It has the type `algo::Smallstr50`,
because that's the type of the primary key of `mytable`.

### Inputting A Table

Now that we have our `abc` program and we're taking a `mytable` option, it would be nice to be able
to do a lookup and find the appropriate record.

To do that, we first create a table in the `abc` namespace, which is based on the `dmmeta.mytable` table.

    $ acr_ed -create -finput -target abc -ssimfile dmmeta.mytable -indexed -write

We verify that there are two access paths from `abc` to `abc.FMytable`, one via the memory pool `abc.FDb.mytable`,
the other via hash table `abc.FDb.ind_mytable`:

    / abc.FDb
    |
    |Lary mytable------->/ abc.FMytable
    |Thash ind_mytable-->|
    -                    |
                         |
                         -

We modify `abc` as follows:

    void abc::Main() {
        abc::FMytable *mytable=ind_mytable_Find(_db.cmdline.mytable);
        prlog((mytable ? "Yes, record found" : "No such record"));
    }

Then build and run:

    $ abc -mytable:tt
    No such record
    $ abc -mytable:abcd51
    Yes, record found

### Regx Options

We will go into more detail into these operations later, but first let's convert `-mytable`
to a Regx option, and modify abc to scan the `_db.mytable` array instead of using the hash table,
and print all matching records:

    $ echo dmmeta.field field:command.abc.mytable reftype:RegxSql | acr -merge -write
    $ amc

    ...
    void abc::Main() {
        ind_beg(_db_mytable_curs,mytable,_db) {
            if (Regx_Match(_db.cmdline.mytable,mytable.mytable)) {
                prlog(mytable.mytable);
            }
        }ind_end;
    }

Now we run the new abc to check the result:

    $ abc -mytable:abcd5%
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

Notice that we no longer need the hash table `abc.FDb.ind_mytable`, since we 
converted from a hash lookup to a linear scan. Let's delete the hash table:

    $ acr_ed -del -field abc.FDb.ind_mytable -write
    $ amc_vis abc.%

    / abc.FDb
    |
    |Lary mytable-->/ abc.FMytable
    -               |
                    |
                    -

Indeed, only one access path remains.

### The -version flag

All commands compiled with abt contain a special string called `gitinfo`. abt populates it
with the build date, first 6 characters of the last known git commit, last git author's e-mail,
and current configuration string: compiler, compiler version, config (e.g. release), OS name (e.g. Linux)
and architecture string (e.g. x86). All this is intended to help with debugging.
This information can be retrived with `strings` or by running the command with `-version` argument:

    $ abc -version
    dev.gitinfo  gitinfo:2019-05-02.309c6ba  author:alexei@lebe.dev  cfg:g++/4.8.5/release.Linux-x86_64  package:""

    $ strings dflt.release-x86_64/abc | grep gitinfo:
    dev.gitinfo  gitinfo:2019-05-02.309c6ba  author:alexei@lebe.dev  cfg:g++/4.8.5/release.Linux-x86_64  package:""

~AL~: this is no longer valid as gitinfo support via loader mechanism was non-portable.
Will be re-implemented via `amc` in a portable way

### The -sig flag

For each target that inputs some tables, amc computes a signature hash of that program's inputs.
This allows detecting schema changes and avoiding incompatibilities that might be dangerous.
The `-sig` option prints the target's signatures. A signature can be viewed as a de-facto version.

### Printing Command Lines

Just like there is support for reading command lines, amc generates the necessary code to convert
a command line struct to a properly escaped Bash command.
Let's modify `abc`'s source as follows:

    void abc::Main() {
        prlog(abc_ToCmdline(_db.cmdline));
    }

Now let's run abc with some options:

    $ abc
    bin/abc  0

abc converts its command line back to a string that can be passed to bash, and any strings get
correctly quoted and escaped as necessary:

    $ abc -val:33 -mytable:$'\nblah'
    bin/abc  33 -mytable:$'\nblah'

Since `-val` is an anonymous field, `abc_ToCmdline` omits the name.

### Subprocesses With Command Line

Before we end this tutorial, let's show one more feature, namely invoking a subprocess
in a strictly typed way. `amc` generates a subprocess invocation helper `command::abc_proc`
which we can use to avoid calling system(). system() is undesirable, mainly because
it disables SIGINT and SIGTERM, making processes built around system() essentially unkillable
from the command line. `amc`s subprocess implementation doesn't capture interrupts, and
uses `DieWithParent` to make sure the child process is killed whenever the parent dies.

Modify the source code of abc.cpp as follows:

    #include "include/gen/command_gen.h"
    #include "include/gen/command_gen.inl.h"

    void abc::Main() {
        prlog(_db.cmdline.val);
        if (_db.cmdline.val>0) {
            command::abc_proc abc;
            abc.cmd.val = _db.cmdline.val-1;
            abc_Exec(abc);
        }
    }

This will recursively invoke the `abc` subprocess (don't try it with large values!)

    $ abc 3
    3
    2
    1
    0

Here is the code amc generated for `amc_proc`:

    $ amc command.abc_proc

    // --- command.abc_proc
    struct abc_proc { // command.abc_proc: Subprocess:
        algo::cstring   path;      //   "bin/abc"  path for executable
        command::abc    cmd;       // command line for child process
        algo::cstring   stdin;     // redirect for stdin
        algo::cstring   stdout;    // redirect for stdout
        algo::cstring   stderr;    // redirect for stderr
        pid_t           pid;       //   0  pid of running child process
        i32             timeout;   //   0  optional timeout for child process
        i32             status;    //   0  last exit status of child process
        abc_proc();
        ~abc_proc();
    private:
        // reftype of command.abc_proc.abc prohibits copy
        abc_proc(const abc_proc&){ /*disallow copy constructor */}
        void operator =(const abc_proc&){ /*disallow direct assignment */}
    };
    ...

### Verbosity

You may have noticed the use of `prlog` to print things. `prlog` is one of a few C++ macros in OpenACR.
(All macros are defined in [include/define.h]).
Along with `prlog`, there is `prerr` that prints to stderr, `verblog`, which prints only
if the command was invoked with `-v` or `-verbose`, and `dbglog`, which prints if the command was
invoked with `-d` or `-debug`.

Whenever a command line is converted to a string with the `_ToCmdline` function,
it inherits a lower verbosity level than the parent. This allows tracing the process tree
by using an appropriate number of `-v`'s on the command line.

Don't forget to `git reset --hard` to clean up any local changes.
