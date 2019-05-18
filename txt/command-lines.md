## Command Lines

All programs created with `amc` come with full command line support. A command line is just a struct,
and options are described as fields. The command lines of all commands are described in the `command`
namespace. All one needs to do is keep adding fields, and amc will take care of conversion between
this description and
* code to read the command-line from argc & argv
* code to generate a shell command line from a command struct
* the usage screen (displayed when a program is invoked without correct parameters)
* shell auto-completion when editing commands.

To start this tutorial, let's start with a fresh commit and create a new executable.

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

Amc knows that the ctype `command.sample` is the command line for sample because of the `fcmdline`
record inserted by `acr_ed`.

    $ acr fcmdline:sample.%
    dmmeta.fcmdline  field:sample.FDb.cmdline  read:Y  comment:""

The command line itself, as created by acr_ed, is a simple ctype:

    dmmeta.ctype  ctype:command.sample  comment:""
      dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"
                  
Let's do something func. First, let's add a flag. Because what kind of command doesn't have a flag?

    $ acr_ed -create -field command.sample.flag -arg bool -write -comment "An important flag"
    ...
    $ ai sample
    ...
    $ sample -h

    Usage: sample [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -flag             An important flag. default: false
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

As you can see, the help screen has been updated. Let's modify `sample`'s main to print the flag.
It is an OpenACR convention to avoid displaying raw values. Output should be machine
readable and never susceptible to an injection attack. So we'll print the value
as a key-value pair (`Keyval` is a C++ template helper):

    void sample::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
    }

    $ ai sample
    ...
    $ sample -flag
    flag:Y

### Boolean Options

There are several ways to specify a flag on a command line: `sample -flag`, `sample -flag:Y`,
`sample -flag:true` as well as `-flag:yes` and `-flag:on` all work.
To specify a false value, we could either omit the option
altogether, or write `sample -flag:N` or `sample -flag:false` or `-flag:no` or `-flag:off`.
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

When parsing the command line, space after option and `:` are treated interchangeably:
`-val:4` is the same as `-val value`.

It may be a stylistic point, but when the option and its value form one word, it's clear at the syntax level,
without knowing what the command expects, that `value` is not a stand-alone option or a positional argument.
It can make error messages more legible when things go wrong.

### Anonymous Options

Let's say we want sample to be able to write `sample 4` without specifying the parameter name `-val`.
We can use `anonfld` for this. We add the appropriate record to the anonfld table, then re-generate
and rebuild.

    $ echo dmmeta.anonfld field:command.sample.val | acr -insert -write
    $ amc && abt -install sample
    $ sample 5
    flag:N
    val:5

### Other Data Types

A command line option can be of any data type whatsoever. Any type that can be read from 
a string is a valid type. For instance, to input an IPv4 address, use

    $ acr_ed -create -field command.sample.ip -arg ietf.Ipv4 -comment "Enter this IP" -write

Reading ctypes from a string is controlled by the `dmmeta.cfmt` table, where a user-provided
function may be specified with `extrn:Y`. This is described in depth in the chapter on string conversion.
    
### Bash Command Completion

OpenACR offers completion of command line parameters for any program in its domain.
The magic is implemented in the command `acr_compl`. To use, we instruct bash
to call `acr_compl` whenever a command line start with one of known command names.
In this case, sample is now a known name, so `acr_compl` already knows about it:

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

### Completing From Table

Auto-complete works with the contents of any table, loading it as necessary to provide completions.

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

### Inputting A Table

Now that we have our `sample` program and we're taking a `mytable` option, it would be nice to be able
to do a lookup and find the appropriate record.

To do that, we first create a table in the `sample` namespace, which is based on the `dmmeta.mytable` table.

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

### Regx Options

We will go into more detail into these operations later, but first let's convert `-mytable`
to a Regx option, and modify sample to scan the `_db.mytable` array instead of using the hash table,
and print all matching records:

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

### The -version flag

All commands compiled with abt contain a special string called `gitinfo`. abt populates it
with the build date, first 6 characters of the last known git commit, last git author's e-mail,
and current configuration string: compiler, compiler version, config (e.g. release), OS name (e.g. Linux)
and architecture string (e.g. x86). All this is intended to help with debugging.
This information can be retrived with `strings` or by running the command with `-version` argument:

    $ sample -version
    dev.gitinfo  gitinfo:2019-05-02.309c6ba  author:alexei@lebe.dev  cfg:g++/4.8.5/release.Linux-x86_64  package:""

    $ strings dflt.release-x86_64/sample | grep gitinfo:
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
Let's modify `sample`'s source as follows:

    void sample::Main() {
        prlog(sample_ToCmdline(_db.cmdline));
    }

Now let's run sample with some options:

    $ sample
    bin/sample  0

sample converts its command line back to a string that can be passed to bash, and any strings get
correctly quoted and escaped as necessary:

    $ sample -val:33 -mytable:$'\nblah'
    bin/sample  33 -mytable:$'\nblah'

Since `-val` is an anonymous field, `sample_ToCmdline` omits the name.

### Subprocesses With Command Line

Before we end this tutorial, let's show one more feature, namely invoking a subprocess
in a strictly typed way. `amc` generates a subprocess invocation helper `command::sample_proc`
which we can use to avoid calling system(). system() is undesirable, mainly because
it disables SIGINT and SIGTERM, making processes built around system() essentially unkillable
from the command line. `amc`s subprocess implementation doesn't capture interrupts, and
uses `DieWithParent` to make sure the child process is killed whenever the parent dies.

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

You may have noticed the use of `prlog` to print things. `prlog` is one of a few C++ macros in OpenACR.
(All macros are defined in [include/define.h]).
Along with `prlog`, there is `prerr` that prints to stderr, `verblog`, which prints only
if the command was invoked with `-v` or `-verbose`, and `dbglog`, which prints if the command was
invoked with `-d` or `-debug`.

Whenever a command line is converted to a string with the `_ToCmdline` function,
it inherits a lower verbosity level than the parent. This allows tracing the process tree
by using an appropriate number of `-v`'s on the command line.

Don't forget to `git reset --hard` to clean up any local changes.
