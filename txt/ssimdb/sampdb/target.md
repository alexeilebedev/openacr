## sampdb.target - samp_make targets list


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)sampdb.Target

* file:[data/sampdb/target.ssim](/data/sampdb/target.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|dflt|bool|[Val](/txt/exe/amc/reftype/Val.md)||make first target - default target|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of sampdb.target

* [sampdb.Targrec](/txt/ssimdb/sampdb/targrec.md) - samp_make recipe for a target

### Related
<a href="#related"></a>
These ssimfiles reference sampdb.target

* [sampdb.targdep via target](/txt/ssimdb/sampdb/targdep.md) - Child target 
* [sampdb.targdep via parent](/txt/ssimdb/sampdb/targdep.md) - Parent target 
* [sampdb.targsrc via target](/txt/ssimdb/sampdb/targsrc.md) - samp_make targets source dependencies

### Used In Commands
<a href="#used-in-commands"></a>
* [samp_make -target](/txt/exe/samp_make/README.md) - Create Makefile for selected targets

### Used In Executables
<a href="#used-in-executables"></a>
* [samp_make](/txt/gen/samp_make/samp_make.md) as [samp_make.FTarget](/txt/gen/samp_make/samp_make.md#samp_make-ftarget)
