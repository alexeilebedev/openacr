## dev.syscmddep - Dependency between two commands


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Syscmddep

* file:[data/dev/syscmddep.ssim](/data/dev/syscmddep.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|syscmddep|algo.RspaceStr16|Val|
|child|[dev.Syscmd](/txt/ssimdb/dev/syscmd.md)|Pkey|
|parent|[dev.Syscmd](/txt/ssimdb/dev/syscmd.md)|Pkey|

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FSyscmddep
* [lib_exec](/txt/lib/lib_exec/README.md) as lib_exec.FSyscmddep

