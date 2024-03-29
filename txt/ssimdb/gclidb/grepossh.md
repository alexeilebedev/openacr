## gclidb.grepossh - Ssh key table


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Grepossh

* file:[data/gclidb/grepossh.ssim](/data/gclidb/grepossh.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|grepossh|algo.Smallstr50|Val||grepo project/repo name|
|sshid|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey||ssh id key name for ssh config|
|name|algo.cstring|Val||ssh id key name for ssh config<br>/RR of sshid|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FGrepossh

