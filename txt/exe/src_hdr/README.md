## src_hdr - Manage source copyright+license header in source files and scripts
<a href="#src_hdr"></a>
src_hdr writes the comment header at the top of every source file and script
in the tree.  The header carries the copyright lines, the license text, and,
for a C++ source, the target and the file it belongs to.  `update-hdr` runs it
over the whole tree, and it then refreshes the prototypes in hand-written
headers through [src_func](/txt/exe/src_func/README.md).

### Syntax
<a href="#syntax"></a>
```usage
src_hdr: Manage source copyright+license header in source files and scripts
Usage: src_hdr [options]
    OPTION             TYPE    DFLT    COMMENT
    -in                string  "data"  Input directory or filename, - for stdin
    -targsrc           regx    ""      Regx of targsrc to update
    -write                             Update files in-place
    -indent                            Indent source files
    -update_copyright                  Update copyright year for current company
    -scriptfile        regx    ""      Regx of scripts to update header
    -verbose           flag            Verbosity level (0..255); alias -v; cumulative
    -debug             flag            Debug level (0..255); alias -d; cumulative
    -trace             string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                              Print help and exit; alias -h
    -version                           Print version and exit
    -signature                         Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

src_hdr reads `dev.targsrc`, `dev.target`, `dmmeta.ns`, `dmmeta.nsx`,
`dev.scriptfile`, `dev.license` and `dev.copyright` from the `-in` directory.
The text of each license comes from `conf/<license>.license.txt`.

It visits every `dev.targsrc` row that matches `-targsrc` and every
`dev.scriptfile` row that matches `-scriptfile`.  Both patterns are empty by
default, so a run selects no file until you name some.  src_hdr never touches
generated files, anything under `extern/`, or the scripts in `bin/bootstrap/`.

For each file it reads the leading block of comment lines, keeps the parts it
cannot regenerate, and writes a new header in this order:

- the `#!` line of a script;
- one `Copyright (C) <years> <company>` line per company in `dev.copyright`,
  the most recent years first;
- a `License:` line and the license text of the file's namespace, or of the
  script's `dev.scriptfile` row;
- for a C++ source, a `Target:` line with the namespace, its type and its
  comment, an `Exceptions:` line from `dmmeta.nsx`, and a `Source:` or `Header:`
  line with the path and the `dev.targsrc` comment;
- every other comment line the old header carried.

A C++ source such as `cpp/acr/main.cpp` comes out like this:

```
// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// ...
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// ...
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/main.cpp -- Main file
//
// ACR: Algo Cross-Reference
// ...
```

A C++ file takes `//` as its comment string.  A script takes `//` when its
extension is `.js`, `.mjs`, `.jsx`, `.ts` or `.tsx`, and `#` for any other
extension.  A script with no extension takes the comment string its `#!` line
implies: `//` for node and `#` for everything else.

With `-write`, src_hdr saves each file and then runs `src_func -updateproto`
over the tree.  A run that failed to read or write a file stops before that
step.

### Examples
<a href="#examples"></a>

```bash
src_hdr -targsrc:acr/cpp/acr/main.cpp -v     # parse one header and print its parts, writing nothing
src_hdr -write -targsrc:acr/%                # rewrite the headers of acr's sources, then refresh prototypes
src_hdr -write -scriptfile:bin/%             # rewrite the headers of the scripts under bin/
src_hdr -update_copyright -targsrc:acr/%     # add this year to the default company's copyright in acr
update-hdr                                   # rewrite every header in the tree, then refresh prototypes
```

### Caveats
<a href="#caveats"></a>

- Without `-write`, src_hdr prints nothing and changes nothing.  It still
  reports a file it cannot read, and `-v` prints the parts it parsed from each
  header.
- `-update_copyright` turns on `-write`, so it always saves the files it
  selects.
- The rewrite drops a copyright line whose company has no `dev.copyright` row.
  Insert the company into `dev.copyright` before you run src_hdr on a file
  that names it.
- src_hdr regenerates the `Target:`, `License:`, `Source:`, `Header:` and
  `Exceptions:` lines, so you lose any text you add to them.  Write a note on a line of its
  own, and src_hdr keeps it.
- A scriptfile with no comment syntax, such as a `.json` file or a file with no
  extension and no `#!` line, fails the run with `src_hdr.no_cmtstring`.
  src_hdr leaves that file untouched.
- The header ends at the first line that is neither a comment nor blank.  A
  comment block that follows a line of code belongs to the body, and src_hdr
  keeps it there.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Read the ssim tables from this directory, which is `data` by default.  Pass an
ssim file, or `-` for stdin, to run against a hand-built set of rows.

#### -targsrc -- Regx of targsrc to update
<a href="#-targsrc"></a>

Select the `dev.targsrc` rows whose key matches this sql regx.  The key is
`<target>/<path>`, so `acr/%` selects every source of acr and `%` selects the
whole tree.  The default is empty and selects nothing.

#### -write -- Update files in-place
<a href="#-write"></a>

Save the regenerated headers, and then run `src_func -updateproto` over the
tree.  Without it the run writes nothing.

#### -indent -- Indent source files
<a href="#-indent"></a>

Reindent the body of each selected file by its braces, four spaces a level.
A line starting with `#` goes to column zero.  The rule suits C++ and
brace-delimited languages, so keep the flag away from shell and perl scripts.

#### -update_copyright -- Update copyright year for current company
<a href="#-update_copyright"></a>

Add the current year to the copyright of the `dev.copyright` row marked
`dflt:Y`, and turn on `-write`.  A file that already names the current year
for any company stays as it is.  The run fails when no row carries `dflt:Y`.

#### -scriptfile -- Regx of scripts to update header
<a href="#-scriptfile"></a>

Select the `dev.scriptfile` rows whose path matches this sql regx, such as
`bin/%`.  The default is empty and selects nothing.
