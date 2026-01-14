## sampdb.target - samp_make targets list


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)sampdb.Target

* file:[data/sampdb/target.ssim](/data/sampdb/target.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|dflt|bool|[Val](/txt/exe/amc/reftypes.md#val)||make first target - default target|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of sampdb.target

* [sampdb.Targrec](/txt/ssimdb/sampdb/targrec.md) - samp_make recipe for a target 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference sampdb.target

* [sampdb.targdep via target](/txt/ssimdb/sampdb/targdep.md) - Child target 
* [sampdb.targdep via parent](/txt/ssimdb/sampdb/targdep.md) - Parent target 
* [sampdb.targsrc via target](/txt/ssimdb/sampdb/targsrc.md) - samp_make targets source dependencies 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [samp_make -target](/txt/exe/samp_make/README.md) - Create Makefile for selected targets 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [samp_make](/txt/exe/samp_make/internals.md) as [samp_make.FTarget](/txt/exe/samp_make/internals.md#samp_make-ftarget)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

