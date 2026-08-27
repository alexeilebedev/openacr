## dev.scriptfile - Known script file


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Scriptfile

* file:[data/dev/scriptfile.ssim](/data/dev/scriptfile.ssim)

italicised fields: *name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>/RR of gitfile*|
|license|[dev.License](/txt/ssimdb/dev/license.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [src_hdr -scriptfile](/txt/exe/src_hdr/README.md) - Regx of scripts to update header

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FScriptfile](/txt/gen/abt_md/abt_md.md#abt_md-fscriptfile)
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FScriptfile](/txt/gen/atf_ci/atf_ci.md#atf_ci-fscriptfile)
* [src_hdr](/txt/gen/src_hdr/src_hdr.md) as [src_hdr.FScriptfile](/txt/gen/src_hdr/src_hdr.md#src_hdr-fscriptfile)
