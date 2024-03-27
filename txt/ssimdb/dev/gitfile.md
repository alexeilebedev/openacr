## dev.gitfile - File managed by git


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Gitfile

* file:[data/dev/gitfile.ssim](/data/dev/gitfile.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|algo.Smallstr200|Val|
|ext|algo.Smallstr50|Val||<br>/RR.LR.RR of gitfile|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dev.gitfile

* [dev.Covfile](/txt/ssimdb/dev/covfile.md) -  
* [dev.Linelim](/txt/ssimdb/dev/linelim.md) -  
* [dev.Msgfile](/txt/ssimdb/dev/msgfile.md) - File in repo containing messages that should be normalized during CI 
* [dev.Noindent](/txt/ssimdb/dev/noindent.md) - Indicates that a file should not be automatically indented 
* [dev.Readme](/txt/ssimdb/dev/readme.md) - File containing documentation 
* [dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md) - Known script file 
* [dev.Srcfile](/txt/ssimdb/dev/srcfile.md) - Source file or header (key is pathname)

### Related
<a href="#related"></a>
These ssimfiles reference dev.gitfile

* [dev.covline via src](/txt/ssimdb/dev/covline.md) - Source file 
* [dev.targsrc via src](/txt/ssimdb/dev/targsrc.md) - List of sources for target 
* [gclidb.grepossh via sshid](/txt/ssimdb/gclidb/grepossh.md) - ssh id key name for ssh config

### Used In Commands
<a href="#used-in-commands"></a>
* [atf_cov -exclude](/txt/exe/atf_cov/README.md) - Exclude gitfiles (external, generated) 
* [src_func -nextfile](/txt/exe/src_func/README.md) - Print name of next srcfile in targsrc list 
* [src_lim -srcfile](/txt/exe/src_lim/README.md) - Filter for source files to process

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FGitfile
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FGitfile
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FGitfile
* [src_lim](/txt/exe/src_lim/README.md) as src_lim.FGitfile

