## acr_dm - ACR Diff/Merge
<a href="#acr_dm"></a>

ACR Diff/Merge is a tool for merging ssim files.
It is typically installed as a "merge driver" for git, and called upon by git
to merge changes made in ssimfiles. The main insight is that since ssimfile are
sets, they can be merged more efficiently than source files. Changes in nearby
lines are not conflicts, and even changes to different attributes of a record
having the same key are not conflicts either. Only change to the same attribute
are considered a conflict. Thus, the tool can easily merge changes where one version
adds a column, another removes a column.

### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Limitations](#limitations)
* [Operation](#operation)
* [Algorithm](#algorithm)
* [Git integration](#git-integration)
* [Sources](#sources)
* [Inputs](#inputs)
* [Tests](#tests)

### Syntax
<a href="#syntax"></a>
```
acr_dm: ACR Diff/Merge
Usage: acr_dm [[-arg:]<string>] [options]
    OPTION       TYPE    DFLT    COMMENT
    -in          string  "data"  Input directory or filename, - for stdin
    [arg]...     string          Files to merge: older ours theirs...
    -write_ours                  Write result to ours file
    -msize       int     7       Conflict marker size
    -rowid                       Output 'ours' rowid for merging into original ssimfiles
    -verbose     int             Verbosity level (0..255); alias -v; cumulative
    -debug       int             Debug level (0..255); alias -d; cumulative
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -arg -- Files to merge: older ours theirs...
<a href="#-arg"></a>

#### -write_ours -- Write result to ours file
<a href="#-write_ours"></a>

#### -msize -- Conflict marker size
<a href="#-msize"></a>

#### -rowid -- Output 'ours' rowid for merging into original ssimfiles
<a href="#-rowid"></a>

### Limitations
<a href="#limitations"></a>

At the moment the following limitations exist:

- No diff function, merge only;

### Operation
<a href="#operation"></a>

The tool accepts a list of files. These are the file names to merge.

- The first argument is *older* (archived) version, from which all other
versions are derived;
- second argument is *ours* version, which contains our changes to
integration stream;
- third argument is *theirs* version, which is the head of integration
stream.

In case of rebase, *ours* and *theirs* swap their places: *ours* is the
head of integration stream, and *theirs* is our changes.

There is no fundamental difference between *ours*, and *theirs*, the
only difference that git merge overwrites *ours* file.

The tool loads all input files and merges *ours* and *theirs* input
tuples, writes result to stdout or, if `-write_ours` is specified,
to *ours* file overwriting it.

The tool handles tuples and attribute addition and removal,
detecting conflicts. Conflicts are marked by standard markers
`<<<<<<<`,`=======`,`>>>>>>>`.  Marker size could be adjusted with
`-msize` option.

Here is the example.

Given four files:

```
inline-command: cat test/acr_dm/file1.ssim
garden.flower  flower:rose  color:red  thorned:Y
garden.flower  flower:dahlia  color:pink  thorned:N
garden.flower  flower:tulip  color:yellow  thorned:N
garden.flower  flower:orchid  color:white  thorned:N
garden.flower  flower:lotus  color:pink  thorned:N
garden.flower  flower:carnation color:red  thorned:N
garden.flower  flower:iris  color:yellow  thorned:N
```

```
inline-command: cat test/acr_dm/file2.ssim
garden.flower  flower:rose  color:yellow  language:romance
garden.flower  flower:tulip  color:yellow  language:friendship
garden.flower  flower:orchid  color:white  language:happiness
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:lotus  color:pink  language:purity
garden.flower  flower:iris  color:blue  language:luck
```

```
inline-command: cat test/acr_dm/file3.ssim
garden.flower  flower:rose  color:red  leaf:compound
garden.flower  flower:dahlia  color:pink  leaf:compound
garden.flower  flower:tulip  color:red  leaf:strap
garden.flower  flower:orchid  color:white  leaf:oblong
garden.flower  flower:carnation  color:red  leaf:linear
garden.flower  flower:iris  color:yellow  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

```
inline-command: cat test/acr_dm/file4.ssim
garden.flower  flower:rose  color:red  leaf:compound
garden.flower  flower:dahlia  color:pink  leaf:compound
garden.flower  flower:tulip  color:red  leaf:strap
garden.flower  flower:orchid  color:white  leaf:oblong
garden.flower  flower:carnation  color:red  leaf:linear
garden.flower  flower:iris  color:yellow  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
garden.flower  flower:lily  color:pink    leaf:bowl
```

**file1** is the common ancestor.

Let's merge **file2** with **file3**.
Both file2 anad file3 remove the column `thorned`. File2 adds a new column `language`
and file3 adds a new column 'leaf', and there are some changes to the color attribute,
but they don't conflict. The resulting changes can be merged without conflict.

```
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file3.ssim
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Now let's merge **file2** with **file4**

```
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file4.ssim; true
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
<<<<<<< test/acr_dm/file2.ssim
garden.flower  flower:lily  color:white  language:sweet
=======
garden.flower  flower:lily  color:pink  leaf:bowl
>>>>>>> test/acr_dm/file4.ssim
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Merge results in conflict.

### Algorithm
<a href="#algorithm"></a>

As `acr_dm` reads files, it assigns each tuple a 2-component rowid.
The rowids are assigned as follows:

First, look up the key in the tuple table.
- If the entry exists, update next rowid to the rowid of this entry
- If the entry doesn't exist, and we're reading the first (base) file, assign the next number.
- If the entry doesn't exist, and we're reading a subsequent file, then
take the previous existing rowid and assign the next number to the 2nd position.

The tuples are then written out in rowid order.
This has the effect of placing all new tuples after the last known location.

### Git integration
<a href="#git-integration"></a>

The tool integrates with local git repository as custom merge driver.

To install, run:
```
gitconfig-setup
```

To choose merge driver for ssim files, **.gitattributes** file is used:

```
inline-command: grep acr_dm .gitattributes
*.ssim merge=acr_dm
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_dm/acr_dm.cpp](/cpp/acr_dm/acr_dm.cpp)||
|[cpp/gen/acr_dm_gen.cpp](/cpp/gen/acr_dm_gen.cpp)||
|[include/acr_dm.h](/include/acr_dm.h)||
|[include/gen/acr_dm_gen.h](/include/gen/acr_dm_gen.h)||
|[include/gen/acr_dm_gen.inl.h](/include/gen/acr_dm_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`acr_dm` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Tests
<a href="#tests"></a>
The following component tests are defined for `acr_dm`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|acr_dm.Conflict||
|acr_dm.Merge||
|acr_dm.RenameTuple||

