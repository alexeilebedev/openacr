## acr_in - ACR Input - compute set of ssimfiles or tuples used by a specific target


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Description](#description)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)
* [Tests](#tests)

### Syntax
<a href="#syntax"></a>
```
acr_in: ACR Input - compute set of ssimfiles or tuples used by a specific target
Usage: acr_in [[-ns:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    [ns]          regx    ""      Regx of matching namespace
    -data                         List ssimfile contents
    -sigcheck             Y       Output sigcheck records for schema version mismatch detection
    -list                         List ssimfile names
    -t                            (with -list) Tree mode
    -data_dir     string  "data"  Directory with ssimfiles
    -schema       string  "data"
    -related      string  ""      Select only tuples related to specified acr key
    -notssimfile  regx    ""      Exclude ssimfiles matching regx
    -checkable                    Ensure output passes acr -check
    -r            regx    ""      Reverse lookup of target by ssimfile
    -verbose      int             Verbosity level (0..255); alias -v; cumulative
    -debug        int             Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig

```

### Description
<a href="#description"></a>

acr_in computes the names and the order of ssimfiles
which constitute target's declared input.

To illustrate, let's create a new program and make it read a table.

    $ acr_ed -create -target sample -write
    ...
    $ acr_ed -create -finput -target sample -ssimfile dmmeta.ctype -write
    ...
    $ acr finput:sample.%
    dmmeta.finput  field:sample.FDb.gitfile  extrn:N  update:N  strict:Y  comment:""
    ...
    
For this target, finput is `dmmeta.ctype`. Let's see if acr_in knows that:

    $ acr_in sample
    dmmeta.Dispsigcheck  dispsig:sample.Input  signature:f162f70f9895c41909c2192722172e6d21fe5679
    dmmeta.Dispsigcheck  dispsig:algo_lib.Input  signature:ddc07e859e7056e1a824df1ad0e6d08e12e89849
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype

The output is the list of ssimfiles needed by the target, plus any signatures
used by the target. We can ignore the signatures for now, but they can be used 
to detect schema changes between the compiled version of a program and the version 
of data set on which `acr_in` operates. The `-sigcheck` option can be used to omit these.

Let's now add, as an `finput` for `sample`, the `dmmeta.ns` table, which is lexicographically
after `dmmeta.ctype`, but logically before (since ctype depends on ns).

    $ acr_ed -create -finput -target sample -ssimfile dmmeta.ns -write
    
    $ acr_in sample -sigcheck:N
    dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype
    
We see that `acr_in` has printed ns and ctype in the order of Pkey dependencies between them.
`acr_in` operates recursively over all libraries that are target uses.

The order of ssimfiles is determined as a transitive closure on Pkey
references, and is independent of the target itself. This means that
`acr_in` can be called with an regex of target names (e.g. %), and the
resulting input can be fed into any one of the targets implied by the
regex, without error.

### Options
<a href="#options"></a>

#### -ns -- Regx of matching namespace
<a href="#-ns"></a>

#### -data -- List ssimfile contents
<a href="#-data"></a>
With -data argument, acr_in also loads the specified ssimfiles in memory
and prints out their contents.

This can be used to create canned input files:

    acr_in sample -data > tempfile
    sample -in:tempfile
    # this is exactly the same as running sample -in:data

#### -sigcheck -- Output sigcheck records for schema version mismatch detection
<a href="#-sigcheck"></a>

#### -list -- List ssimfile names
<a href="#-list"></a>

#### -t -- (with -list) Tree mode
<a href="#-t"></a>

This option outputs a tree, grouped by namespace first.
A ssimfile can appear in the output multiple times, once for each
namespace that requires it

#### -data_dir -- Directory with ssimfiles
<a href="#-data_dir"></a>

#### -schema -- 
<a href="#-schema"></a>

#### -related -- Select only tuples related to specified acr key
<a href="#-related"></a>

`acr_in` can optionally include only those tuples which are transitively
reachable from a certain set. For this, specify `-related`.
Here is an example where we constrain `sample`'s input to the `sample` namespace itself.

    $ acr_in sample -data -related:dmmeta.ns:sample -sigcheck:N
    dmmeta.ns  ns:sample  nstype:exe  comment:""
    dmmeta.ctype  ctype:sample.FCtype  comment:""
    dmmeta.ctype  ctype:sample.FDb  comment:""
    dmmeta.ctype  ctype:sample.FGitfile  comment:""
    dmmeta.ctype  ctype:sample.FNs  comment:""

In contrast, if we didn't specify `-related`, `-data` would fetch all records:

    $ acr_in sample -data | wc -l
    864

#### -notssimfile -- Exclude ssimfiles matching regx
<a href="#-notssimfile"></a>

#### -checkable -- Ensure output passes acr -check
<a href="#-checkable"></a>

If we take the data from ssmifiles `ns` and `ctype`, they are now sufficient
to serve as inputs to the newly created `sample`. However, `acr -check` will fail on
this resulting dataset, because `ns` also depends on `nstype`, and in general there
may be unresolved Pkey references in the resulting output.

To recursively include any dependent ssimfiles, specify `-checkable`:

    $ acr_in sample -checkable -sigcheck:N
    dmmeta.ssimfile  ssimfile:dmmeta.nstype  ctype:dmmeta.Nstype
    dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
    dmmeta.ssimfile  ssimfile:dmmeta.dispsig  ctype:dmmeta.Dispsig
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype

#### -r -- Reverse lookup of target by ssimfile
<a href="#-r"></a>

With the `-r` option, one can supply a regex of a ssimfile and get a list
of all namespaces that require the ssimfile. This includes any dependent namespaces
via the targdep table.

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_in/acr_in.cpp](/cpp/acr_in/acr_in.cpp)||
|[cpp/acr_in/data.cpp](/cpp/acr_in/data.cpp)||
|[cpp/gen/acr_in_gen.cpp](/cpp/gen/acr_in_gen.cpp)||
|[include/acr_in.h](/include/acr_in.h)||
|[include/gen/acr_in_gen.h](/include/gen/acr_in_gen.h)||
|[include/gen/acr_in_gen.inl.h](/include/gen/acr_in_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`acr_in` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.dispsig](/txt/ssimdb/dmmeta/dispsig.md)|Cryptographic signature of all dispatches|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targdep](/txt/ssimdb/dev/targdep.md)|Dependency between targets|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|Describe input table of a program|

### Tests
<a href="#tests"></a>
The following component tests are defined for `acr_in`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|acr_in.Reverse||
|acr_in.Simple||
|acr_in.Tree||

