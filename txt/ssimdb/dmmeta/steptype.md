## dmmeta.steptype - Type of scheduler step
<a href="#dmmeta-steptype"></a>
List of supported step types

```
inline-command: acr steptype | ssimfilt ^ -t
STEPTYPE       COMMENT
Callback       Inline, call step func always; Does not update _db.next_loop, does not affect hot polling
Extern         User provides scheduling by implementing _FirstChanged.
Inline         Inline, call step func always
InlineOnce     Inline, call step func at given time (time is given by first elemement of index)
InlineRecur    Inline, call step func with given periodicity
TimeHookOnce   TimeHook, call step func at given time (time is given by first elemement of index)
TimeHookRecur  TimeHook, call step func with given periodicity

```

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Steptype

* file:[data/dmmeta/steptype.ssim](/data/dmmeta/steptype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|steptype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.steptype

* [dmmeta.fstep via steptype](/txt/ssimdb/dmmeta/fstep.md) - Generate a main loop step to be executed whenever a field is non-empty 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FSteptype](/txt/exe/amc/internals.md#amc-fsteptype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

