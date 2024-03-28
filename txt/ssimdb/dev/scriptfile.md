## dev.scriptfile - Known script file


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Scriptfile

* file:[data/dev/scriptfile.ssim](/data/dev/scriptfile.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey|
|name|algo.Smallstr50|Val||<br>/RR of gitfile|
|license|[dev.License](/txt/ssimdb/dev/license.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Commands
<a href="#used-in-commands"></a>
* [src_hdr -scriptfile](/txt/exe/src_hdr/README.md) - Regx of scripts to update header

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FScriptfile
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FScriptfile
* [src_hdr](/txt/exe/src_hdr/README.md) as src_hdr.FScriptfile

