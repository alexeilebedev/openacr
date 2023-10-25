## acr_dm: ACR Diff/Merge

ACR Diff/Merge is a tool for merging ssim files.

### Syntax

```
acr_dm: ACR Diff/Merge
Usage: acr_dm [[-arg:]<string>] [options]
    OPTION       TYPE    DFLT    COMMENT
    -in          string  "data"  Input directory or filename, - for stdin
    [arg]...     string          Files to merge: older ours theirs...
    -write_ours                  Write result to ours file
    -msize       int     7       Conflict marker size
    -verbose     int             Verbosity level (0..255); alias -v; cumulative
    -debug       int             Debug level (0..255); alias -d; cumulative
    -help                        Print help an exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig

```

### Limitations

At the moment the following limitations exist:

- No diff function, merge only;
- Order of merged tuples changes, i.e. new tuples does not keep their
position, they are added to the bottom, that may cause unwanted effects
where tuple order is sensitive.

### Operation

The tool accepts a number of nameless arguments. These arguments are
file names to merge.

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

Let's merge **file2** with **file3**

```
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file3.ssim
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Now let's merge **file2** with **file4**

```
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file4.ssim
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
<<<<<<< test/acr_dm/file2.ssim
garden.flower  flower:lily  color:white  language:sweet
=======
garden.flower  flower:lily  color:pink  leaf:bowl
>>>>>>> test/acr_dm/file4.ssim
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Merge results in conflict.

### Git integration

The tool integrates to local git repository with **.git/config** as
custom merge driver. Since **.git/config** could not be versioned,
the config if kept on versioned file **.gitconfig**.

```
inline-command: cat .gitconfig
[merge "acr_dm"]
    name = Resolve conflicts in ssim files
    driver = acr_dm -write_ours -msize %L -- %O %A %B
```

To install, run:
```
gitconfig-setup
```

This will copy **.gitconfig** to **.git/config**.

To choose merge driver for ssim files, **.gitattributes** files is used:

```
inline-command: cat .gitattributes
cat: .gitattributes: No such file or directory
```

### Tests

The following component tests are defined for `acr_dm`:
```
acr_dm.Conflict	
acr_dm.Merge	
```

