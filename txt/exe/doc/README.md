## doc - Render a markdown document to the terminal


### Syntax
<a href="#syntax"></a>
```usage
doc: Render a markdown document to the terminal
Usage: doc [[-key:]<string>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in           string  "data"  Input directory or filename, - for stdin
    [key]         string  ""      Document to show: a shorthand key, a regx, or a path
    -q                            Read the key as an ssim query, never as a page doc synthesizes
    -section      string  ""      Show only sections whose heading matches this regx
    -meta                         Show the schema of the selection instead of its records (acr -meta)
    -showcmd                      Show the command line behind every box, in place of what each one answered
    -vis                          Show the access paths of the ctypes on the page (amc_vis)
    -code                         Show the C++ amc generated for the ctypes on the page, and their functions
    -data                         Show the rows of the table the page is about (acr <ssimfile>)
    -list                         List matching documents instead of rendering one
    -width        int     0       Measure in columns; 0 takes it from the terminal
    -color                Y       Style the output when stdout is a terminal
    -force_color                  Style the output even when stdout is not a terminal
    -pager                Y       Page the output when stdout is a terminal
    -links                        List the page's followable links instead of showing it
    -follow       int     0       Show the document reached by following this link of the page
    -html                         Render the page as HTML instead of for a terminal
    -server       string  ""      URL prefix every generated link carries
    -bind         string  ""      Serve HTTP at this ip:port instead of showing one page
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

`doc` shows a document of this tree, or a record of the ssim database, rendered --
at the terminal, as an HTML page, or from a small HTTP server it runs itself.

### What you can name
<a href="#what-you-can-name"></a>

The argument is a key.  Any substring of a `dev.readmefile` path is a valid
selection, and so is the name of a documented thing:

| Key | Page |
|---|---|
| *nothing* | the documentation index, `txt/README.md` |
| `acr` | that tool's README |
| `txt/openacr` | that directory's README |
| `txt/openacr/` | that directory: what it holds |
| `dmmeta.ctype` | the document describing that ssimfile |
| `cpp/doc/html.cpp` | a source file, as a listing |
| `dmmeta.ctype:dmmeta.Ctype` | that record, from `acr` |
| `dmmeta.Ctype` | the same, for a key naming a ctype |
| `msg` | the protocols: every message header in the tree |
| `msg:ams.MsgHeader` | that header, and the messages that begin with it |
| `msg:ams.LogMsg` | that message, and the dispatches that read it |
| `help` | the reader's keys |

A leading `/` means the top of the repository.  A key matching several documents,
or none, is answered with what it could have meant: the documents whose path
carries it, then the tables and ctypes whose name does.

### The pages doc builds
<a href="#the-pages-doc-builds"></a>

A word with no dot in it, before a colon, is one this tool answers itself.  Every
other word is a table's name, and a table's short name expands to its full one, so
`ssimfile:dmmeta.ctype` and `dmmeta.ssimfile:dmmeta.ctype` are one page.  `-q` reaches
a table whose name is one of these words.

| Key | Page |
|---|---|
| `ns:acr` | the namespace: its ctypes, sources, dependencies, inputs, tests |
| `ctype:dmmeta.Ctype` | one ctype: its declaration, then the C++ that acts on it |
| `func:dmmeta.Ctype` | the functions that ctype appears in |
| `func:doc.NavText` | one function's source |
| `cpp/doc/ns.cpp:NavText` | the same, named by the file it lives in |
| `code:dmmeta.Ctype` | what `amc` generated for it |
| `accesspath:acr.%` | the access paths of those ctypes, drawn by `amc_vis` |

A comptest is not among them.  `comptest:doc.Ssimdb` is the short name of
`atfdb.comptest` and a key of it, so it is an ordinary page of records -- and what a
reader wants of a test beyond its row, the function that drives it and the transcript it
is checked against, arrives there as two sections read off the row itself.

`dev.docpage` is that list as a table: a row per page, its name the word a location
addresses it by, its comment what the page shows.  The rows are compiled into the tool and
each is bound to the function that draws it, so a page is a row and a function rather than
a case in two dispatch chains.

### At the terminal
<a href="#at-the-terminal"></a>

On a terminal the page opens in the tool's own reader, which pages it and follows its
links.  `?` lists the keys, and `doc help` prints the same table.

### In a browser
<a href="#in-a-browser"></a>

```bash
doc acr -html -server:/doc > acr.html
doc -bind:127.0.0.1:8080 -server:http://localhost:8080/doc
```

A URL is a location: the path after the prefix is the key itself, with a markdown
document spelled `.html`, and the display flags ride in the query string.  Every link
is a roundtrip, so a page is made from the tree as it stands.

Every page has one canonical text, and it says the whole of the state: the flags in
force, then the key.  The box along the top holds it, its words are its links, and
typing it back arrives at the same page.  The same text is the reader's status line at
the terminal.

```bash
-code dmmeta.ctype:dmmeta.Ctype
```

That text is a command line, and this tool takes it under exactly those names.  So a line
copied out of the box runs, and what it opens is the page it was copied from.

A page of records carries three tabs -- the records, the schema they are instances of, and
the command line that fetched them -- and two sections that open under a triangle: the
access paths of the ctypes it is about, and the code `amc` generated for them.  The tabs
are alternatives and travel no further than the page; a section stays open as a reader
walks, since a reader who opened the code once means it for the next page too.

What acr is asked is not among those flags.  A selection narrower than a table arrives as
its cross-reference closure, descended through pkeys and drawn as a tree, since a record
read alone says little and the rows it names are what a reader came for.  A whole table
arrives flat, because the closure of every record is the database.  So how much of a
record comes back follows from what was asked for, and there is nothing to set.

The pane holds the shape of the page: the headings of a document, the sections of a page
of records, the functions of a listing.  It is one text drawn twice -- above the page at
the terminal, beside it in a browser.

`www/doc/page.html` and `www/doc/doc.css` are the page and its style, read from the top
of the checkout and inlined, so a saved page carries its own style.

### What the server serves
<a href="#what-the-server-serves"></a>

Only files in the repo, referenced by `dev.gitfile`, are served.

One connection is served at a time, and a request that stalls is dropped.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Directory the ssim database is read from.  `doc` reads the tables that tell it where
documents live and what the database holds, so a tree other than the working one is
read by naming its `data`.

#### -key -- Document to show: a shorthand key, a regx, or a path
<a href="#-key"></a>

What to show.  It is resolved as described under [What you can name](#what-you-can-name):
a path, a substring of one, a table and a record, or one of the words this tool answers
itself.

#### -q -- Read the key as an ssim query, never as a page doc synthesizes
<a href="#-q"></a>

Read the key as a query over the ssim database, never as a page `doc` builds.  `ctype`,
`ns` and `func` are words this tool answers itself and they shadow the tables of those
names, so `-q ctype:%` is how the table is reached.  The flag expands the key and stops
-- what it produces is an ordinary query, and nothing downstream asks whether it was
set.

#### -section -- Show only sections whose heading matches this regx
<a href="#-section"></a>

Show only the sections of a document whose heading matches this regx, and nothing else
of it.

#### -meta -- Show the schema of the selection instead of its records (acr -meta)
<a href="#-meta"></a>

Show the schema of the selection instead of its records, which is `acr -meta`.  Since a
schema is records too, the page it produces is walked with the same keys as any other.

#### -showcmd -- Show the command line behind every box, in place of what each one answered
<a href="#-showcmd"></a>

#### -vis -- Show the access paths of the ctypes on the page (amc_vis)
<a href="#-vis"></a>

Open the access paths section of a page of records.  It is `amc_vis` over the ctypes the
page is about, so what it draws is which record reaches which, and through what field.
`accesspath:<regx>` is the same drawing as a page of its own, for the times a reader
wants a whole namespace's paths and nothing else.

It is off until asked for, and like `-code` it stays in force as the reader walks.

#### -code -- Show the C++ amc generated for the ctypes on the page, and their functions
<a href="#-code"></a>

Open the code section of a page of records.  It holds the chunk of the generated header
that declares each ctype the page is about -- where the records live, the struct itself,
and the accessor of every field -- and then the prototypes of the functions that take
that type, each leading to the function's own source.  The struct and the accessors are
one subject: what `amc` generated for a type is as much its interface as the fields are.

It is off until asked for, since it is one of the two questions on such a page that cost
real time to answer, and it stays in force as the reader walks -- a reader who opened the
code once means it for the next page too.

#### -data -- Show the rows of the table the page is about (acr <ssimfile>)
<a href="#-data"></a>

Open the data section, which a page about one table carries: the rows of that table.  What
a reader looking at a table's declaration wants next is what is in it, so the rows are a
section of that page rather than another location to walk to -- and closed until asked for,
since a table of ten thousand rows is not what somebody reading the declaration asked for.

#### -list -- List matching documents instead of rendering one
<a href="#-list"></a>

List the documents the key matched instead of opening one of them.

#### -width -- Measure in columns; 0 takes it from the terminal
<a href="#-width"></a>

Measure in columns.  Zero takes it from the terminal, and the measure is capped at a
hundred however wide the window is, because a line of prose past that is hard to track
back to.

#### -color -- Style the output when stdout is a terminal
<a href="#-color"></a>

Style the output when stdout is a terminal.  A pipe gets no escape sequences, which is
what makes the output diffable.

#### -force_color -- Style the output even when stdout is not a terminal
<a href="#-force_color"></a>

Style the output even when stdout is not a terminal, which is how a capture of the
styling is taken.

#### -pager -- Page the output when stdout is a terminal
<a href="#-pager"></a>

Page the output when stdout is a terminal.  Turned off, the page is written once and the
reader never opens.

#### -links -- List the page's followable links instead of showing it
<a href="#-links"></a>

List the page's followable links, numbered, instead of showing the page.  With `-follow`
it is how a walk through the documents is driven from a script, and it is the only way
the reader's decisions are testable, since the reader itself needs a terminal.

#### -follow -- Show the document reached by following this link of the page
<a href="#-follow"></a>

Show the document reached by following this link of the page, numbered as `-links`
numbers them.

#### -html -- Render the page as HTML instead of for a terminal
<a href="#-html"></a>

Render the page as HTML instead of for a terminal.  The stylesheet is inlined, so a page
written to a file carries its own style.

#### -server -- URL prefix every generated link carries
<a href="#-server"></a>

URL prefix every generated link carries, and the only path the server answers for; the
root of the host redirects to it.  A page written with `-html` and no `-server` carries
links from the root of the tree.

#### -bind -- Serve HTTP at this ip:port instead of showing one page
<a href="#-bind"></a>

Serve HTTP at this ip:port instead of showing one page.  One location is answered per
request, so every link is a roundtrip and a page is made from the tree as it stands.
One connection is served at a time.
