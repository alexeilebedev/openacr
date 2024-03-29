## bin/ff - Find files and inside files
<a href="#bin-ff"></a>

`Ff` is a file search utility.
The simplest usage is

    ff <term1> <term2>
    
Ff recursively searches for all text files starting from from the starting
point (default is `.`), and then searches inside each for for the specified pattern.
All terms must occur on the same, in effect `ff term1 term2` is equivalent to `ff term1.*term2`.

All terms are perl regexes.

#### Selecting Files And Directories
<a href="#selecting-files-and-directories"></a>

Ff can search inside a particular directory with `-d`; Multiple directories can be specified:

    ff -d /usr/include -d cpp <term>
    
The `-n` option excludes generated files (files with `/gen/` in the path).
The `-s` option adds `/usr/include` to the list of starting directories.

The `-g` option starts the search in the `ffroot` directory. This directory is determined
by walking upward from the current directory until encountering a directory with the file `.ffroot` in it.

`ff` automatically skips directory `temp/`.
The `-f` option can be used to select which files `ff` selects for searching.
There can be multiple `-f` options. They act as "OR".

#### Negative Patterns
<a href="#negative-patterns"></a>

Negative patterns can be preceded with `!`.
Negative patterns are available both for filenames (`-f`) and file conrtents.
For instance, to exclude headers, you might use `ff -f '!\.h$'`. Make sure to quote `!` 
so it's not interpeted by the command shell as a history item

#### Matching Options
<a href="#matching-options"></a>

The `-t` option looks for typedefs or struct definitions (in a very loose sense).

The `-w` option matches whole words only. This is equivalent to wrapping terms with `\b...\b`
since all terms are Perl regexes.

The `-i` option enables case-insensitive search for all terms.

#### Editing Matches
<a href="#editing-matches"></a>

With `-l` option, only the names of files that matched the terms are printed.

The `-c` option adds extra empty lines between multiple matches in the same file; for visual grouping.

The `-a` option prints all matches, even if there was more than match per line.
This can be useful with emacs macros, in order to visit all match locations and perform some action.

The output of `ff` is ready to consume as compilation buffer output in emacs, or via errlist.
To open the list of matches in an editor, do `errlist ff <term1>...`.
Integration of `ff` with emacs is provided in `conf/emacs.el`.

#### Highlighting
<a href="#highlighting"></a>

By default, `ff` highlights output. But if `-notty` is specified, or the output
is not a terminal, highlighting is skipped.

`ff` is similar to `grep -RHn`, but more specialized.

### Sources
<a href="#sources"></a>
The source code license is GPL

Source file: [bin/ff](/bin/ff)

