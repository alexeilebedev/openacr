## dev.gitfile - File managed by git


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Gitfile

* file:[data/dev/gitfile.ssim](/data/dev/gitfile.ssim)

italicised fields: *ext* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gitfile|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ext*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>/RR.LR.RR of gitfile*|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dev.gitfile

* [dev.Covfile](/txt/ssimdb/dev/covfile.md) -  
* [dev.Linelim](/txt/ssimdb/dev/linelim.md) -  
* [dev.Msgfile](/txt/ssimdb/dev/msgfile.md) - File in repo containing messages that should be normalized during CI 
* [dev.Noindent](/txt/ssimdb/dev/noindent.md) - Indicates that a file should not be automatically indented 
* [dev.Readme](/txt/ssimdb/dev/readme.md) - File containing documentation 
* [dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md) - Known script file 
* [dev.Srcfile](/txt/ssimdb/dev/srcfile.md) - Source file or header (key is pathname) 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dev.gitfile

* [dev.covline via src](/txt/ssimdb/dev/covline.md) - Source file 
* [dev.targsrc via src](/txt/ssimdb/dev/targsrc.md) - List of sources for target 
* [gclidb.grepossh via sshid](/txt/ssimdb/gclidb/grepossh.md) - ssh id key name for ssh config 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [abt -srcfile](/txt/exe/abt/README.md) - Build/disassemble/preprocess specific file 
* [atf_cov -exclude](/txt/exe/atf_cov/README.md) - Exclude gitfiles (external, generated) 
* [src_func -nextfile](/txt/exe/src_func/README.md) - Print name of next srcfile in targsrc list 
* [src_lim -srcfile](/txt/exe/src_lim/README.md) - Filter for source files to process 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FGitfile](/txt/exe/acr_ed/internals.md#acr_ed-fgitfile)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FGitfile](/txt/exe/atf_ci/internals.md#atf_ci-fgitfile)
* [atf_cov](/txt/exe/atf_cov/internals.md) as [atf_cov.FGitfile](/txt/exe/atf_cov/internals.md#atf_cov-fgitfile)
* [src_lim](/txt/exe/src_lim/internals.md) as [src_lim.FGitfile](/txt/exe/src_lim/internals.md#src_lim-fgitfile)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->

