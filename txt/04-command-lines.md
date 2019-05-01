## Command Lines

All tools treat space after option and `:` interchangeably:
`-option:value` is the same as `-option value`.
I don't particularly like Java, but the first variant is preferrable,
because it's clear at the syntax level, without knowing what the command
expects, that `value` is a value and not a stand-alone option or a positional argument.

All tools accept standard options -verbose, -help, -debug
Verbosity is cumulative, each child process inherits a lower verbosity level; it's a form of tracing.
Running a process with -v -v will execute most immediate subprocesses with -v.
The command line of any tool is represented by ctype in the command namespace. Try this:

    acr ctype:cmdline.acr -t

## Bash Command Completion

To configure completion of all commands, do

    eval $(acr_compl -install)

Then, try auto-completing acr command-line options:

    $ acr -pr<tab>
    -pretty:<flag>  -print:<flag>

This auto-completes works with ssimfiles, and can auto-complete from any table.

    $ acr ctype:acr.FC<tab>
    acr.FCdflt    acr.FCheck    acr.FCppfunc  acr.FCtype

### Example: Create New Ssimfile And Test Command Completion

In fact, let's create and populate a brand new table, and add a dummy acr 
command line option referencing that table, just to see if auto-complete works.

* Step 1: create new ssimfile

    acr_ed -create -ssimfile dmmeta.mytable -write

* Step 2: populate it

    for X in abcd{10..99}; do echo "dmmeta.mytable mytable:$X"; done | acr -insert -write

* Step 3: create a new command line option

    acr_ed -create -field command.acr.mytable -arg dmmeta.Mytable -reftype Pkey -comment "A very useful option" -write

* Step 4: test

    $ acr -mytable:abcd2<tab>
    abcd20  abcd21  abcd22  abcd23  abcd24  abcd25  abcd26  abcd27  abcd28  abcd29

Inside acr, the field is accessible as _db.cmdline.mytable. Of course, manually
editing acr source code would be required to do something useful with this new option.

Since we don't really need these changes, we can undo them:

    $ git reset --hard

Or

    $ acr_ed -del -ssimfile dmmeta.mytable -write
    $ acr field:command.acr.mytable -del -write

