## dev.scriptfile - Known script file


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Scriptfile

* file:[data/dev/scriptfile.ssim](/data/dev/scriptfile.ssim)

italicised fields: *name* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>/RR of gitfile*|
|license|[dev.License](/txt/ssimdb/dev/license.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [src_hdr -scriptfile](/txt/exe/src_hdr/README.md) - Regx of scripts to update header 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FScriptfile](/txt/exe/abt_md/internals.md#abt_md-fscriptfile)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FScriptfile](/txt/exe/atf_ci/internals.md#atf_ci-fscriptfile)
* [src_hdr](/txt/exe/src_hdr/internals.md) as [src_hdr.FScriptfile](/txt/exe/src_hdr/internals.md#src_hdr-fscriptfile)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

