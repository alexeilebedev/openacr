## dev.gitfile - File managed by git


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Gitfile

* file:[data/dev/gitfile.ssim](/data/dev/gitfile.ssim)

italicised fields: *ext* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)|||
|*ext*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>/RR.LR.RR of gitfile*|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dev.gitfile

* [dev.Covfile](/txt/ssimdb/dev/covfile.md) -  
* [dev.Linelim](/txt/ssimdb/dev/linelim.md) -  
* [dev.Msgfile](/txt/ssimdb/dev/msgfile.md) - File in repo containing messages that should be normalized during CI 
* [dev.Noindent](/txt/ssimdb/dev/noindent.md) - Indicates that a file should not be automatically indented 
* [dev.Readmefile](/txt/ssimdb/dev/readmefile.md) - File containing documentation 
* [dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md) - Known script file 
* [dev.Srcfile](/txt/ssimdb/dev/srcfile.md) - Source file or header (key is pathname)

### Related
<a href="#related"></a>
These ssimfiles reference dev.gitfile

* [dev.covline via src](/txt/ssimdb/dev/covline.md) - Source file 
* [dev.targsrc via src](/txt/ssimdb/dev/targsrc.md) - List of sources for target

### Used In Commands
<a href="#used-in-commands"></a>
* [abt -srcfile](/txt/exe/abt/README.md) - Build/disassemble/preprocess specific file 
* [atf_cov -exclude](/txt/exe/atf_cov/README.md) - Exclude gitfiles (external, generated) 
* [src_func -nextfile](/txt/exe/src_func/README.md) - (action) Print name of next srcfile in targsrc list 
* [src_lim -srcfile](/txt/exe/src_lim/README.md) - Filter for source files to process

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FGitfile](/txt/gen/acr_ed/acr_ed.md#acr_ed-fgitfile)
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FGitfile](/txt/gen/atf_cov/atf_cov.md#atf_cov-fgitfile)
* [src_func](/txt/gen/src_func/src_func.md) as [src_func.FGitfile](/txt/gen/src_func/src_func.md#src_func-fgitfile)
* [src_lim](/txt/gen/src_lim/src_lim.md) as [src_lim.FGitfile](/txt/gen/src_lim/src_lim.md#src_lim-fgitfile)
