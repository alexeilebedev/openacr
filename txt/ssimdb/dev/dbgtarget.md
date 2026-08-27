## dev.dbgtarget -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Dbgtarget

* file:[data/dev/dbgtarget.ssim](/data/dev/dbgtarget.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dbgtarget|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Arguments appended to the debugged process, $-substituted|
|buildcmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Command whose output is the abt target regx to rebuild first|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [mdbg](/txt/gen/mdbg/mdbg.md) as [mdbg.FDbgtarget](/txt/gen/mdbg/mdbg.md#mdbg-fdbgtarget)
