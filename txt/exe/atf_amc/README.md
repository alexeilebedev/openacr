## atf_amc - Unit tests for amc (see amctest table)


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
atf_amc: Unit tests for amc (see amctest table)
Usage: atf_amc [[-amctest:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [amctest]   regx    "%"     SQL regex, selecting test to run
    -dofork             Y       Use fork
    -q                          Quiet mode
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -amctest -- SQL regex, selecting test to run
<a href="#-amctest"></a>

#### -dofork -- Use fork
<a href="#-dofork"></a>

#### -q -- Quiet mode
<a href="#-q"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf_amc/atree.cpp](/cpp/atf_amc/atree.cpp)||
|[cpp/atf_amc/bheap.cpp](/cpp/atf_amc/bheap.cpp)||
|[cpp/atf_amc/bigend.cpp](/cpp/atf_amc/bigend.cpp)||
|[cpp/atf_amc/bitfld.cpp](/cpp/atf_amc/bitfld.cpp)||
|[cpp/atf_amc/bitset.cpp](/cpp/atf_amc/bitset.cpp)||
|[cpp/atf_amc/cascdel.cpp](/cpp/atf_amc/cascdel.cpp)||
|[cpp/atf_amc/cdlist.cpp](/cpp/atf_amc/cdlist.cpp)||
|[cpp/atf_amc/cleanup.cpp](/cpp/atf_amc/cleanup.cpp)||
|[cpp/atf_amc/cmp.cpp](/cpp/atf_amc/cmp.cpp)||
|[cpp/atf_amc/cslist.cpp](/cpp/atf_amc/cslist.cpp)||
|[cpp/atf_amc/delptr.cpp](/cpp/atf_amc/delptr.cpp)||
|[cpp/atf_amc/dispatch.cpp](/cpp/atf_amc/dispatch.cpp)||
|[cpp/atf_amc/exec.cpp](/cpp/atf_amc/exec.cpp)||
|[cpp/atf_amc/fbuf.cpp](/cpp/atf_amc/fbuf.cpp)||
|[cpp/atf_amc/fconst.cpp](/cpp/atf_amc/fconst.cpp)||
|[cpp/atf_amc/fstep.cpp](/cpp/atf_amc/fstep.cpp)||
|[cpp/atf_amc/gsymbol.cpp](/cpp/atf_amc/gsymbol.cpp)||
|[cpp/atf_amc/hook.cpp](/cpp/atf_amc/hook.cpp)||
|[cpp/atf_amc/inlary.cpp](/cpp/atf_amc/inlary.cpp)||
|[cpp/atf_amc/lary.cpp](/cpp/atf_amc/lary.cpp)||
|[cpp/atf_amc/lineiter.cpp](/cpp/atf_amc/lineiter.cpp)||
|[cpp/atf_amc/lpool.cpp](/cpp/atf_amc/lpool.cpp)||
|[cpp/atf_amc/main.cpp](/cpp/atf_amc/main.cpp)||
|[cpp/atf_amc/msgcurs.cpp](/cpp/atf_amc/msgcurs.cpp)||
|[cpp/atf_amc/numstr.cpp](/cpp/atf_amc/numstr.cpp)||
|[cpp/atf_amc/opt.cpp](/cpp/atf_amc/opt.cpp)||
|[cpp/atf_amc/pmask.cpp](/cpp/atf_amc/pmask.cpp)||
|[cpp/atf_amc/ptrary.cpp](/cpp/atf_amc/ptrary.cpp)||
|[cpp/atf_amc/readstr.cpp](/cpp/atf_amc/readstr.cpp)||
|[cpp/atf_amc/sort.cpp](/cpp/atf_amc/sort.cpp)||
|[cpp/atf_amc/strconv.cpp](/cpp/atf_amc/strconv.cpp)||
|[cpp/atf_amc/tary.cpp](/cpp/atf_amc/tary.cpp)||
|[cpp/atf_amc/thash.cpp](/cpp/atf_amc/thash.cpp)||
|[cpp/atf_amc/varlen.cpp](/cpp/atf_amc/varlen.cpp)||
|[cpp/atf_amc/zdlist.cpp](/cpp/atf_amc/zdlist.cpp)||
|[cpp/atf_amc/zslist.cpp](/cpp/atf_amc/zslist.cpp)||
|[cpp/gen/atf_amc_gen.cpp](/cpp/gen/atf_amc_gen.cpp)||
|[include/atf_amc.h](/include/atf_amc.h)||
|[include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)||
|[include/gen/atf_amc_gen.inl.h](/include/gen/atf_amc_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`atf_amc` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

