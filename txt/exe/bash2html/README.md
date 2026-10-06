## bash2html - Convert bash output and colors to html
<a href="#bash2html"></a>
bash2html turns terminal output into a web page.  It reads text carrying ANSI
color and style escapes on stdin, and writes an HTML page on stdout that shows
the same text in the same colors, in a monospace font on a black background.

### Syntax
<a href="#syntax"></a>
```usage
bash2html: Convert bash output and colors to html
Usage: bash2html [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -test                       Produce Test Output
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

bash2html reads stdin line by line and writes one page: a header, one line of
HTML per input line, and a footer.  It keeps spaces and line breaks as they are,
so columns line up the way they did in the terminal.

It understands the SGR escapes, the ones of the form `ESC [ <codes> m`.  The
styles are bold, light, italic, underline and line-through, set by 1, 2, 3, 4
and 9 and turned off by 22, 23, 24 and 29.  The foreground colors are the eight
standard ones (30 to 37) and the eight bright ones (90 to 97), and 39 restores
the default.  The same sixteen colors work as backgrounds (40 to 47 and 100 to
107), and 49 restores the default.  Code 0 resets every style.  Each style
nests inside the others, so turning one off leaves the rest in force.

### Examples
<a href="#examples"></a>

```bash
acr ns:bash2html | bash2html > ns.html          # save a command's output as a page
bash2html -test | bash2html > palette.html      # render every supported style and color
ls --color=always | bash2html > listing.html    # keep the colors a tool prints only to a terminal
```

A line with one red word comes out as a span around that word:

```
$ printf 'a \e[31mred\e[0m word\n' | bash2html
<!DOCTYPE html><html><head><meta charset="utf-8"/></head><body style="white-space: pre;...">
a&nbsp;<span style="color: Red">red</span>&nbsp;word
</span></font></body></html>
```

### Caveats
<a href="#caveats"></a>

- bash2html reads stdin whatever `-in` says, so redirect or pipe the input.
- A tool that detects it is writing to a pipe often stops emitting colors.  Ask
  it for them explicitly, as `--color=always` does for `ls` and `grep`.
- An escape that is not an SGR code, such as a cursor move or a window title,
  leaks into the page as text.
- The 256-color and true-color codes (`38;5;<n>`, `38;2;<r>;<g>;<b>`) are not
  supported, and each of their numbers shows up in the page as `ESC [<n>m`.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

bash2html accepts this flag and ignores it.  The input is always stdin.

#### -test -- Produce Test Output
<a href="#-test"></a>

Print a sample of plain text carrying every style and color bash2html supports,
and exit without reading stdin.  Pipe it back into bash2html to see how each
code renders.
