## atfdb.tmsg -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)atfdb.Tmsg

* file:[data/atfdb/tmsg.ssim](/data/atfdb/tmsg.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|tmsg|algo.Smallstr50|Val|
|comptest|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|Pkey||<br>/LL of tmsg|
|rank|i32|Val||<br>/LR.LL of tmsg|
|dir|[atfdb.Msgdir](/txt/ssimdb/atfdb/msgdir.md)|Pkey||<br>/LR.LR of tmsg|
|istuple|bool|Val|
|msg|algo.cstring|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [atf_comp](/txt/exe/atf_comp/README.md) as atf_comp.FTmsg

