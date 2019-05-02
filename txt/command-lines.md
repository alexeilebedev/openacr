## Command Lines

All tools created with amc come with full command line support. A command line is just a struct,
and options are described as fields. All one needs to do is keep adding fields, and amc and the rest
of tools will take care of all chores.

To start this tutorial, let's start with a fresh commit, and create a new executable.

    $ acr_ed -create -target abc -write
    ...

This will create a new tool called `abc`:

    $ abc
    Hello, World!

The tool already comes with some built-in options.

    $ abc -h


    Usage: abc [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

Let's ignore these for now and proceed to fun stuff. First, let's add a flag. Because what kind of
command doesn't have a flag?

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

As you can see, the new flag is now recognized. Let's modify `abc`'s main to print it.
It is an OpenACR convention to avoid ever displaying raw values. Output should be machine
readable and never susceptible to an injection attack. Even though it may not matter
with a bool, we'll print it as a key-value pair. `Keyval` is a little C++ template that
helps do that:

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

All these interchangeable symbols come from the fconst values for `algo.Bool`.
We can query them:

    $ acr fconst:algo.Bool.%
    dmmeta.fconst  fconst:algo.Bool.value/N      value:0  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/Y      value:1  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/true   value:1  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/false  value:0  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/0      value:0  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/1      value:1  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/off    value:0  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/on     value:1  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/no     value:0  comment:""
    dmmeta.fconst  fconst:algo.Bool.value/yes    value:1  comment:""
    ...

We can also query the resulting code:

    $ amc algo.Bool

    // --- algo_BoolEnum

    enum algo_BoolEnum {        // algo.Bool.value
         algo_Bool_N       = 0
        ,algo_Bool_Y       = 1
        ,algo_Bool_true    = 1
        ,algo_Bool_false   = 0
        ,algo_Bool_0       = 0
        ,algo_Bool_1       = 1
        ,algo_Bool_off     = 0
        ,algo_Bool_on      = 1
        ,algo_Bool_no      = 0
        ,algo_Bool_yes     = 1
    };

    enum { algo_BoolEnum_N = 10 };

    // --- algo.Bool
    struct Bool { // algo.Bool
        u8   value;   //   false
        inline operator algo_BoolEnum() const;
        explicit Bool(u8                             in_value);
        Bool(algo_BoolEnum arg);
        Bool();
    };

    ...

I'm showing these queries to encourage you to query anything and everything in OpenACR
as a method of discovery. With few exceptions, most of the things normally considered
built-ins are in OpenACR being generated.

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

### Anonymous Values

Let's say we want abc to be able to write `abc 4` without specifying the parameter name `-val`.
We can use `anonfld` for this. We add the appropriate record to the anonfld table, then re-generate
and rebuild.

    $ echo dmmeta.anonfld field:command.abc.val | acr -insert -write
    $ amc && abt -install abc
    $ abc 5
    flag:N
    val:5

### Bash Command Completion

OpenACR offers completion of command line parameters for any tool in its domain.
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

Let's test it on our tool. We'll create and populate a brand new table, and add an option to abc
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

Now that we have our `abc` tool and we're taking a `mytable` option, it would be nice to be able
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
to a Regx option, andmodify abc to scan the `_db.mytable` array instead of using the hash table,
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

### Printing Command Lines

Just like there is support for reading command lines, amc generates the necessary code to print command lines.
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

Don't forget to `git reset --hard` to clean up any local changes.
