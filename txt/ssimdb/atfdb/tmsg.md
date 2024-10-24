## atfdb.tmsg -


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Tmsg

* file:[data/atfdb/tmsg.ssim](/data/atfdb/tmsg.ssim)

italicised fields: *comptest, rank, dir* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tmsg|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*comptest*|*[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LL of tmsg*|
|*rank*|*i32*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>/LR.LL of tmsg*|
|*dir*|*[atfdb.Msgdir](/txt/ssimdb/atfdb/msgdir.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LR.LR of tmsg*|
|istuple|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|msg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [atf_comp](/txt/exe/atf_comp/internals.md) as [atf_comp.FTmsg](/txt/exe/atf_comp/internals.md#atf_comp-ftmsg)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

