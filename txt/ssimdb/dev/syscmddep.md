## dev.syscmddep - Dependency between two commands


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Syscmddep

* file:[data/dev/syscmddep.ssim](/data/dev/syscmddep.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|child|[dev.Syscmd](/txt/ssimdb/dev/syscmd.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|parent|[dev.Syscmd](/txt/ssimdb/dev/syscmd.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FSyscmddep](/txt/gen/abt/abt.md#abt-fsyscmddep)
* [lib_exec](/txt/gen/lib_exec/lib_exec.md) as [lib_exec.FSyscmddep](/txt/gen/lib_exec/lib_exec.md#lib_exec-fsyscmddep)
