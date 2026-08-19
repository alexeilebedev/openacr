## amc Feature: Command-line parsing
<a href="#amc-feature-command-line-parsing"></a>

Every amc executable's argv is parsed by code amc generated
from a `command.<exename>` ctype.  You declare one
`dmmeta.ccmdline` record per executable and amc emits:

- a typed `command.<exename>` struct with one field per option,
- a `ReadArgv` function that walks `argv` and fills the struct,
- a `<exename>_help` string for `-help`,
- a `Get<Exename>Cmdline()` accessor for arbitrary lookups,
- the argv-printer used by [Exec](/txt/exe/amc/reftype/Exec.md)
  to drive child processes.

The whole CLI surface — type validation, defaults, enum
parsing, alias expansion, "`-flag:value`" vs "`-flag value`"
forms, positional arguments, repeated flags, accumulation —
is described in ssim and read for free by the generated code.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Anatomy of a command line](#anatomy-of-a-command-line)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Positional vs named arguments](#positional-vs-named-arguments)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Flags vs value arguments](#flags-vs-value-arguments)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Aliases](#aliases)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Enums on the command line](#enums-on-the-command-line)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Argvtypes](#argvtypes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The help string](#the-help-string)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Argv printing (cfmt:Argv)](#argv-printing-cfmtargv-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Anatomy of a command line
<a href="#anatomy-of-a-command-line"></a>

For every executable namespace `<ns>`:

```
dmmeta.ctype       ctype:command.<ns>
dmmeta.ccmdline    ctype:command.<ns>  read:Y  basecmdline:algo_lib.FDb.cmdline
dmmeta.field       field:command.<ns>.<flag>  arg:<type>  reftype:Val  dflt:"…"  comment:"…"
```

The `command.<ns>` struct holds one field per CLI option.
`basecmdline:algo_lib.FDb.cmdline` says "merge the standard
algo_lib flags (`-help`, `-version`, `-verbose`, `-debug`,
`-signature`) into this command line."

`read:Y` makes the generated `main()` call `ReadArgv` and pass
the parsed result to `<ns>::Main()`.  Setting `read:N` lets you
take over argv handling yourself (rare; almost everything
should let amc do it).

### Ssim inputs
<a href="#ssim-inputs"></a>

The full set:

| Record | Role |
|--------|------|
| `dmmeta.ccmdline ctype:command.<ns> read:Y/N basecmdline:<>` | Declares the cmdline; ties it to amc's main() |
| `dmmeta.field    field:command.<ns>.<flag> arg:<T> reftype:Val dflt:"…"` | One field per option |
| `dmmeta.anonfld  field:command.<ns>.<flag>` | Treat this field as positional |
| `dmmeta.fflag    field:command.<ns>.<flag> cumulative:Y emptyval:""` | Special argv treatments — see "Flags vs value arguments" |
| `dmmeta.falias   field:command.<ns>.<short> srcfield:command.<ns>.<long>` | Short alias for an option |
| `dmmeta.fconst   fconst:command.<ns>.<flag>/<value> value:N` | Allowed enum values; show in `-help` |
| `dmmeta.argvtype ctype:<T> argvtype:<T2>` | Override the parsed type for an argument (e.g., `double` for floats) |
| `dmmeta.cfmt     cfmt:command.<ns>.Argv  printfmt:Auto  print:Y` | Generate the argv printer (used by [Exec](/txt/exe/amc/reftype/Exec.md)) |

### Positional vs named arguments
<a href="#positional-vs-named-arguments"></a>

An option can be specified as `-flag value`, `-flag:value`, or
(if also marked `dmmeta.anonfld`) **just `value`** as a bare
positional argument.  Anonymous fields are positional in
declaration order:

```
dmmeta.field   field:command.foo.target  arg:dev.Target  reftype:RegxSql  dflt:""
  dmmeta.anonfld  field:command.foo.target
```

After this, `foo abt% -dryrun` and `foo -target:abt% -dryrun`
and `foo -target abt% -dryrun` all mean the same thing.

If multiple anonfld fields exist, they are filled positionally
in the order they appear in the ctype.

### Flags vs value arguments
<a href="#flags-vs-value-arguments"></a>

Each field falls into one of four ergonomic categories,
combining "is the value required?" and "is the field
positional?":

| Required value? | anonfld? | Form in help                | Example                       |
|-----------------|----------|-----------------------------|-------------------------------|
| yes             | yes      | `[-target:]<target>`        | `foo abt% …`                 |
| yes             | no       | `-target:<target>`          | `foo -target:abt% …`         |
| no              | yes      | `[[-target:]<target>]`      | `foo … (target optional)`     |
| no              | no       | `[options]`                 | `foo …` (one of many opts)    |

`dmmeta.fflag`:

| Field            | Effect |
|------------------|--------|
| `cumulative:Y`   | Each occurrence on argv accumulates (e.g., `-v -v -v` raises verbosity).  Bumps the integer field rather than overwriting. |
| `emptyval:"v"`   | If the flag is given without a value, set the field to `v`.  Used for `-debug` → `-debug:1`. |

Boolean flags use `arg:bool` plus typically `emptyval:Y`, so
`-verbose` is shorthand for `-verbose:Y`.  An i32 flag with
`cumulative:Y` increments instead of replacing — that's how
`-v -v -v` produces 3.

### Aliases
<a href="#aliases"></a>

The [Alias](/txt/exe/amc/reftype/Alias.md) reftype is the
canonical way to declare short option names:

```
dmmeta.field   field:command.foo.v  arg:i32  reftype:Alias
  dmmeta.falias  field:command.foo.v  srcfield:command.foo.verbose
```

After this, the help string lists `-v` as an alias of
`-verbose`, and the argv parser folds them together.

### Enums on the command line
<a href="#enums-on-the-command-line"></a>

When a field has `fconst` records, the help string lists every
accepted value (and its comment) in a per-flag mini-table:

```
-cfg          string  ""      Set config
    debug       (Debug build)
    release     (Release build, default)
    profile     (Profile build)
```

amc walks `field.p_arg->c_field` for fconst entries (or
`GetEnumField` for single-field wrapper ctypes) and emits the
list automatically.  At parse time the argv values are matched
against the fconst keys; an unknown value produces an
`unknown option value:` error.

### Argvtypes
<a href="#argvtypes"></a>

`dmmeta.argvtype` overrides how amc generates the parser for
specific built-in C types.  The default mappings are:

| ctype     | Effective argv type | Comment                  |
|-----------|---------------------|--------------------------|
| `bool`    | (empty — flag form) | `-verbose` ≡ `-verbose:Y` |
| `double`  | `double`            |                          |
| `float`   | `double`            |                          |

Add a row for any new built-in to override.

### The help string
<a href="#the-help-string"></a>

amc generates `<exename>_help` (a `const char *`) at code-gen
time.  Its shape:

```
<exename>: <ctype-comment>
Usage: <exename> [<positional1>] [<positional2>] [options]
    OPTION    TYPE   DFLT   COMMENT
    ...
```

Calling `<ns>::ReadArgv(<ns>_help, …)` from `main()` is what
makes `-help` print the table — the same string is referenced
from `algo_lib`'s standard `-help` handling.

### Argv printing (cfmt:Argv)
<a href="#argv-printing-cfmtargv-"></a>

The reverse direction — turning a `command.<ns>` struct back
into an `argv[]` — is generated when the ctype has a
`dmmeta.cfmt  cfmt:command.<ns>.Argv  printfmt:Auto  print:Y`
record.  `Argv` there is the strfmt, the last component of
the cfmt key; the printfmt of an Argv cfmt is `Auto`.  amc
emits:

- `void <ns>_PrintArgv(command::<ns>& cmd, cstring& out)` —
  appends amc's "`-flag:value`" tokens to OUT;
- `void <ns>_ToArgv(command::<ns>& cmd, StringAry& args)` —
  the same tokens as separate array elements, with
  `bin/<ns>` as element 0;
- `tempstr <ns>_ToCmdline(command::<ns>& cmd)` — returns
  `bin/<ns>` followed by those tokens, and passes down one
  fewer `-verbose` / `-debug` than the current process
  carries.

The two forms that build a single string bash-quote each value
they emit, so a value containing a space prints as one quoted
shell word and does survive a round trip through a shell.  The
quoting fires only where it is needed: a value whose every
character is shell-safe prints bare, and an empty value prints
as a pair of quotes.  The array form is deliberately unquoted,
since no shell ever parses it — its elements reach the child
through `execve` exactly as they stand, and a quote added there
would arrive as part of the value.

These three accessors are what [Exec](/txt/exe/amc/reftype/Exec.md)
calls when starting a subprocess — the parent and child
agree on the wire format because they share the ssim source.

### Pitfalls
<a href="#pitfalls"></a>

- **Positional ordering matters.**  Anonymous fields are
  matched in their declaration order in the ctype.  Reordering
  fields breaks every existing user script.
- **`basecmdline` shares state.**  The base ctype's fields are
  *folded into* the derived command line — they become real
  fields, not pointers.  Modifying the base after the build
  changes every derived cmdline.
- **`fflag.emptyval`** is the trick that makes `-verbose` work
  without `:Y`.  Without an `emptyval`, a flag with `arg:bool`
  *requires* a value.
- **Alias short names collide.**  amc does not check that
  short option names from `dmmeta.falias` are unique among
  themselves; first-match wins at parse time.  Manage them
  carefully.
- **`anonfld` + `RegxSql` is the standard idiom** for "target
  selector" flags (`abt`, `amc`, `acr` all use it).
- **Enum value parsing is case-sensitive** and matches against
  fconst keys exactly.  Use lowercase fconsts unless you need
  the visible distinction.

### See also
<a href="#see-also"></a>

- [Exec](/txt/exe/amc/reftype/Exec.md) — uses the argv printer for child processes
- [Alias](/txt/exe/amc/reftype/Alias.md) — short option names
- [Charsets](/txt/exe/amc/charset.md) — `CmdLineNameBreakQ` / `CmdLineValueBreakQ` separators
- [Strings](/txt/exe/amc/string.md) — printfmt families generally
- [Global](/txt/exe/amc/reftype/Global.md) — where `ReadArgv` is wired into `main()`
- Source: `cpp/amc/cmdline.cpp`, `cpp/amc/global.cpp` (`tfunc_Global_main`, `tfunc_Global_ReadArgv`)
- Spec tables: `acr 'dmmeta.ccmdline:%'`, `acr 'dmmeta.fflag:%'`, `acr 'dmmeta.anonfld:%'`, `acr 'dmmeta.argvtype:%'`

### Example
<a href="#example"></a>

Minimal exec namespace skeleton:

```
dmmeta.ns          ns:myapp   nstype:exe
dmmeta.ctype       ctype:command.myapp  comment:"Hello-world demo"

dmmeta.field       field:command.myapp.in  arg:algo.cstring  reftype:Val  dflt:'"input.dat"'
dmmeta.field       field:command.myapp.out  arg:algo.cstring  reftype:Val  dflt:'"-"'  comment:"output file; - for stdout"
dmmeta.field       field:command.myapp.verbose  arg:i32  reftype:Val  dflt:0
  dmmeta.fflag      field:command.myapp.verbose  cumulative:Y  emptyval:""

dmmeta.field       field:command.myapp.v  arg:i32  reftype:Alias
  dmmeta.falias     field:command.myapp.v  srcfield:command.myapp.verbose

dmmeta.field       field:command.myapp.target  arg:dev.Target  reftype:RegxSql  dflt:""
  dmmeta.anonfld    field:command.myapp.target
  dmmeta.fregx      field:command.myapp.target  regxtype:Sql  partial:N

dmmeta.ccmdline    ctype:command.myapp  read:Y  basecmdline:algo_lib.FDb.cmdline
dmmeta.cfmt        cfmt:command.myapp.Argv  printfmt:Auto  print:Y  read:Y  sep:""  genop:N
```

The generated help string:

```
myapp: Hello-world demo
Usage: myapp [[-target:]<target>] [options]
    OPTION    TYPE     DFLT       COMMENT
    -in       string   "input.dat"
    -out      string   "-"        output file; - for stdout
    -verbose  flag     0          ; alias -v; cumulative
    [target]  regx     ""         Sql Regx of dev::Target
    -verbose  flag                Verbosity level (0..255); alias -v; cumulative
    -debug    flag                Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

User code never touches argv directly:

```c++
void myapp::Main() {
    auto &cmd = myapp::_db.cmdline;
    prlog("target=" << cmd.target.expr << " verbose=" << cmd.verbose);
}
```
