## Recipes


### Recipes
<a href="#recipes"></a>

|What|Command|
|---|---|
|Add new component test from input|`atf_comp -i -write`|
|Build all targets in all configurations|`abt % -cfg %`|
|Build all targets|`ai`|
|Check documentation for errors and open bad locations in editor|`errlist abt_md -check`|
|Check documentation for errors|`abt_md -check`|
|Check documentation internal & external links|`abt_md -check -external %`|
|Check ssim database for errors and delete bad records|`acr % -check -del -write`|
|Check ssim database for errors and open bad records in editor|`acr % -check -e`|
|Check ssim database for errors with extended rules|`acr % -check -x`|
|Check ssim database for errors|`acr % -check`|
|Compile and open error locations in editor|`errlist ai`|
|Compute full transitive closure of any record|`acr <table>:<key> -t`|
|Compute left transitive closure of any record|`acr <table>:<key> -t -l`|
|Create a ssimfile containing some records|`<command> \| cat > file.ssim`|
|Create conditional x-reference between two records in memory|`acr_ed -create -xref -field:target.FType.p_anothertype -inscond<c++expr>`|
|Create new ctype in target|`acr_ed -create -ctype ns.FType`|
|Create new executable|`acr_ed -create -target xyz`|
|Create new hash table|`acr_ed -create -field ns.FDb.ind_ftype`|
|Create new header file|`acr_ed -create -srcfile include/...`|
|Create new input|`acr_ed -create -finput -target <target> -ssimfile <ssimfile>`|
|Create new library|`acr_ed -create -target xyz -nstype lib`|
|Create new protocol|`acr_ed -create -target xyz -nstype protocol`|
|Create new readme file|`acr_ed -create -srcfile txt/...`|
|Create new sandbox|`sandbox <name> -create`|
|Create new source file with specific target|`acr_ed -create -srcfile cpp/... -target <target>`|
|Create new source file|`acr_ed -create -srcfile cpp/...`|
|Create new ssim database|`acr_ed -create -target xyz -nstype ssimdb`|
|Create new ssim dataset|`mkdir <dir>`|
|Create new ssimfile|`acr_ed -create -ssimfile <ns.name>`|
|Create new table with allocator and indexing|`acr_ed -create -ctype target.FType -subset i32 -pooltype Tpool -indexed`|
|Create new unit test and open in editor|`acr_ed -create -unittest <ns>.<name> -e`|
|Create new unit test|`acr_ed -create -unittest <ns>.<name>`|
|Create x-reference between two records in memory|`acr_ed -create -xref -field:target.FType.p_anothertype`|
|Debug target with breakpoint on function|`mdbg <target> -b function`|
|Debug target with breakpoint on line|`mdbg <target> -b file.cpp:33`|
|Debug target with conditional breakpoint|`mdbg <target> -b "function if <condition>"`|
|Debug target with python scripting|`mdbg <target> -py"`|
|Debug target|`mdbg <target> -- args`|
|Disassemble function in different configurations|`abt <target> -disas <function> -cfg <cfg>`|
|Disassemble function|`abt <target> -disas <function>`|
|Edit component test|`atf_comp <comptest> -e`|
|Edit records interactively|`acr <query> -e`|
|Edit target's in-memory database|`acr ns:<ns> -t -e`|
|Execute SQL (MariaDB) command on ssim database|`echo 'select * from target' \| acr -my dev.%`|
|Execute component test from input|`atf_comp -i`|
|Execute sqlite command on ssim database|`aqlite -ns:dev select * from target'`|
|Find all records a given record refers to|`acr <table>:<key> -nup 100`|
|Find all references to record|`acr <table>:<key> -ndown 100`|
|Generate c++ code for all targets|`amc`|
|Open functions matching pattern in editor|`src_func % <function> -e`|
|Open functions whose comment matches pattern in editor|`src_func -comment <regx> -e`|
|Run all component tests|`atf_comp`|
|Run all unit tests|`atf_unit`|
|Run code generator in sandbox and examine difference|`sandbox amc -reset -diff -- amc`|
|Run cppcheck on all sources|`atf_ci cppcheck`|
|Show all records in ssimfile with topological sorting and indentation|`acr -in:<filename> -tre`|
|Show all ssim records where primary key matches pattern|`acr %:%xyz%`|
|Show comments selected ssim records|`acr <query> -cmt`|
|Open selected records for editing|`acr <query> -e`|
|Show schema for selected records|`acr <query> -meta`|

