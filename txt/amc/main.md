## amc: Program Startup & Main

By convention, the command line for each target `x` is represented
by the ctype `command.x`. For instance, amc's command
line is `command.amc` as shown below:

```
inline-command: acr ctype:command.amc -t
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:command  nstype:protocol  license:GPL  comment:"Command line descriptions"
    dmmeta.ctype  ctype:command.amc  comment:""
      dmmeta.field  field:command.amc.in_dir  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Root of input ssim dir"
      dmmeta.field  field:command.amc.query   arg:algo.cstring  reftype:Val  dflt:'""'      comment:"Query mode: generate code for specified object"
        dmmeta.anonfld  field:command.amc.query   comment:""

      dmmeta.field  field:command.amc.out_dir  arg:algo.cstring  reftype:Val      dflt:'"."'  comment:"Root of output cpp dir"
      dmmeta.field  field:command.amc.proto   arg:bool          reftype:Val      dflt:false  comment:"Print prototype"
      dmmeta.field  field:command.amc.report  arg:bool          reftype:Val      dflt:true   comment:"Final report"
      dmmeta.field  field:command.amc.e       arg:bool          reftype:Val      dflt:false  comment:"Open matching records in editor"
      dmmeta.field  field:command.amc.trace   arg:algo.cstring  reftype:RegxSql  dflt:'""'   comment:"Regx of something to trace code generation"
      dmmeta.cfmt  cfmt:command.amc.Argv  printfmt:Auto  read:Y  print:Y  sep:""  genop:N  comment:""
      dmmeta.ctypelen  ctype:command.amc  len:144  alignment:8  padbytes:5

dmmeta.field  field:amc.FDb.cmdline  arg:command.amc  reftype:Val  dflt:""  comment:""
  dmmeta.fcmdline  field:amc.FDb.cmdline  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:""
report.acr  n_select:16  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

The `fcmdline` record generates code to parse the command line from argc/argv,
and generates the syntax string which can be seen below:

```
inline-command: amc -h; true
amc: Algo Model Compiler: generate code under include/gen and cpp/gen
Usage: amc [[-query:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in_dir     string  "data"  Root of input ssim dir
    [query]     string  ""      Query mode: generate code for specified object
    -out_dir    string  "."     Root of output cpp dir
    -proto                      Print prototype
    -report             Y       Final report
    -e                          Open matching records in editor
    -trace      regx    ""      Regx of something to trace code generation
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

Positional arguments are marked with `anonfld`.

When the program runs, entry function `main()` saves argc/argv to variables
`algo_lib.FDb.argc` and `algo_lib.FDb.argv` and calls `Main`:
User implements `Main`.
One can step through all of these functions with `gdb` (or use the `mdbg`)

