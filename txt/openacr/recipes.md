## Recipes
<a href="#recipes"></a>

One line per task.  This page is an index rather than a manual: where a recipe
needs explaining, the explanation lives in that tool's
[README](/txt/exe/README.md) and this row is how you find it.

### Query the database
<a href="#query-the-database"></a>

Reference: [acr](/txt/exe/acr/README.md), [acr_in](/txt/exe/acr_in/README.md).

|What|Command|
|---|---|
|Show all ssim records where primary key matches pattern|`acr %:%xyz%`|
|List all ssim tables|`acr ssimfile`|
|List all namespaces|`acr ns`|
|List all build targets|`acr target`|
|Show every field of a ctype|`acr field:<ctype>.%`|
|Show the command-line options of a process|`acr field:command.<proc>.%`|
|Show the dependencies of a target|`acr targdep:<target>.%`|
|Filter on a field that is not the primary key|`acr field -where arg:u8`|
|Compute full transitive closure of any record|`acr <table>:<key> -t`|
|Compute left transitive closure of any record|`acr <table>:<key> -t -l`|
|Find all records a given record refers to|`acr <table>:<key> -nup 100`|
|Find all references to record|`acr <table>:<key> -ndown 100`|
|Show comments for selected ssim records|`acr <query> -cmt`|
|Show schema for selected records|`acr <query> -meta`|
|Select only some fields of the output|`acr <query> -field:<field>`|
|Align the output in blocks|`acr <query> -pretty`|
|Output a regx matching one field's values|`acr <query> -regxof:<field>`|
|Show all records in ssimfile with topological sorting and indentation|`acr -in:<filename> -tree`|
|Format tuples as a table|`<command> \| ssimfilt -t`|
|Keep only some fields of a tuple stream|`<command> \| ssimfilt -field:<field>`|
|Which ssimfiles a target reads|`acr_in <target>`|
|Which targets read an ssimfile|`acr_in -r <ssimfile>`|
|The actual tuples a target loads|`acr_in <ns regx> -data`|
|Execute SQL (MariaDB) command on ssim database|`echo 'select * from target' \| acr -my dev.%`|
|Execute sqlite command on ssim database|`aqlite -ns:dev 'select * from target'`|

### Edit records
<a href="#edit-records"></a>

Reference: [acr](/txt/exe/acr/README.md); the rules are in
[/txt/rule/acr.md](/txt/rule/acr.md).

|What|Command|
|---|---|
|Insert a record|`echo '<ssim tuple>' \| acr -insert -write`|
|Insert a record, refusing a bad foreign key|`echo '<ssim tuple>' \| acr -insert -check -write`|
|Upsert a record|`echo '<full ssim tuple>' \| acr -merge -write`|
|Delete a record|`acr <ssimfile>:<pkey> -del -write`|
|Delete exactly the rows a list of tuples names|`cat rows.acr \| acr -sel -del -write`|
|Rename a record's primary key|`acr <pkey> -rename:<newkey> -write`|
|Rewrite an ssimfile through the current schema|`acr '<ssimfile>:%' -write -print:N`|
|Edit records interactively|`acr <query> -e`|
|Edit a target's in-memory database|`acr ns:<ns> -t -e`|
|Create a ssimfile containing some records|`<command> \| cat > file.ssim`|
|Create new ssim dataset|`mkdir <dir>`|
|Check ssim database for errors|`acr % -check`|
|Check ssim database for errors with extended rules|`acr % -check -x`|
|Check ssim database for errors and open bad records in editor|`acr % -check -e`|
|Check ssim database for errors and delete bad records|`acr % -check -del -write`|
|Refresh dev.gitfile after a git add, remove or rename|`update-gitfile`|

### Edit the schema
<a href="#edit-the-schema"></a>

Reference: [acr_ed](/txt/exe/acr_ed/README.md); the rules are in
[/txt/rule/acr_ed.md](/txt/rule/acr_ed.md).  Leave `-write` off to print the
script instead of running it.

|What|Command|
|---|---|
|Create new executable|`acr_ed -create -target xyz`|
|Create new library|`acr_ed -create -target xyz -nstype lib`|
|Create new protocol|`acr_ed -create -target xyz -nstype protocol`|
|Create new ssim database|`acr_ed -create -target xyz -nstype ssimdb`|
|Create new ssimfile|`acr_ed -create -ssimfile <ns.name>`|
|Create new ctype in target|`acr_ed -create -ctype ns.FType`|
|Create new table with allocator and indexing|`acr_ed -create -ctype target.FType -subset i32 -reftype Tpool -indexed`|
|Create a message as a subset of a header ctype|`acr_ed -create -ctype ns.MyMsg -subset ns.MsgHeader`|
|Create new input|`acr_ed -create -finput -target <target> -ssimfile <ssimfile>`|
|Create a field|`acr_ed -create -field ns.FType.<name> -arg <ctype>`|
|Create a command-line option|`acr_ed -create -field command.<proc>.<name> -arg <ctype> -dflt '""'`|
|Create new hash table|`acr_ed -create -field ns.FDb.ind_ftype`|
|Create x-reference between two records in memory|`acr_ed -create -xref -field:target.FType.p_anothertype`|
|Create conditional x-reference between two records in memory|`acr_ed -create -xref -field:target.FType.p_anothertype -inscond:<c++expr>`|
|Place a new field before an existing one|`acr_ed -create -field <field> -arg <ctype> -before <field>`|
|Drive a field's collection from a step|`acr_ed -create -fstep <field> -steptype:<steptype>`|
|Add a cursor over a hash index|`acr_ed -create -fcurs <field>/curs`|
|Route a message type to a dispatch|`acr_ed -create -dispatch_msg <dispatch>/<msgtype>`|
|Create new source file|`acr_ed -create -srcfile cpp/...`|
|Create new source file with specific target|`acr_ed -create -srcfile cpp/... -target <target>`|
|Create new header file|`acr_ed -create -srcfile include/...`|
|Create new readme file|`acr_ed -create -srcfile txt/...`|
|Create new unit test|`acr_ed -create -unittest <ns>.<name>`|
|Create new unit test and open in editor|`acr_ed -create -unittest <ns>.<name> -e`|
|Rename a ctype|`acr_ed -ctype <ns>.Old -rename <ns>.New`|
|Rename a field|`acr_ed -field <ns>.Ct.old -rename <ns>.Ct.new`|
|Rename an ssimfile, its key field, its ctype and its file|`acr_ed -ssimfile <ns>.old -rename <ns>.new`|
|Rename a target and move its sources|`acr_ed -target <old> -rename <new>`|
|Delete a field, and the values it left in the ssimfile|`acr_ed -del -field <ns>.FType.<name>`|
|Delete a ctype|`acr_ed -del -ctype <ns>.FType`|
|Delete an ssimfile|`acr_ed -del -ssimfile <ns>.<name>`|
|Delete a source file|`acr_ed -del -srcfile cpp/...`|
|Delete a target|`acr_ed -del -target <target>`|
|See the generated code a schema change produces|`acr_ed <args> -sandbox`|

### Generate
<a href="#generate"></a>

Reference: [amc](/txt/exe/amc/README.md),
[src_func](/txt/exe/src_func/README.md).

|What|Command|
|---|---|
|Generate c++ code for all targets|`amc`|
|Print the struct amc generates for a ctype|`amc <ctype>`|
|Print the source of a generated function|`amc <function>`|
|Derive the amc-owned tables and generate no source|`amc -derive`|
|Trace code generation|`amc -trace:<regx>`|
|Run code generator in sandbox and examine difference|`wt amc -reset -diff -- amc`|
|Refresh prototypes after adding or changing a function|`update-hdr`|
|Create the functions the userfunc table declares|`src_func -createmissing`|
|Open functions matching pattern in editor|`src_func % <function> -e`|
|Open functions whose comment matches pattern in editor|`src_func -matchcomment <regx> -e`|
|List a namespace's generated API with source locations|`src_func <ns>.%<pat>% -gen -targsrc:"%/gen/%" -showloc`|

### Build and debug
<a href="#build-and-debug"></a>

Reference: [abt](/txt/exe/abt/README.md), [wt](/txt/exe/wt/README.md).

|What|Command|
|---|---|
|Build all targets|`ai`|
|Build all targets in all configurations|`abt % -cfg %`|
|Build one target and install it|`abt <target> -install`|
|Compile and open error locations in editor|`errlist ai`|
|Disassemble function|`abt <target> -disas <function>`|
|Disassemble function in different configurations|`abt <target> -disas <function> -cfg <cfg>`|
|Debug target|`mdbg <target> -- args`|
|Debug target with breakpoint on function|`mdbg <target> -b function`|
|Debug target with breakpoint on line|`mdbg <target> -b file.cpp:33`|
|Debug target with conditional breakpoint|`mdbg <target> -b "function if <condition>"`|
|Debug target with python scripting|`mdbg <target> -py`|
|Debug a whole process tree|`mdbg -nonstop <target> -- args`|
|Create new sandbox|`wt <name> -create`|

### Test
<a href="#test"></a>

Reference: [atf_unit](/txt/exe/atf_unit/README.md),
[atf_comp](/txt/exe/atf_comp/README.md), [atf_ci](/txt/exe/atf_ci/README.md).

|What|Command|
|---|---|
|Run all unit tests|`atf_unit`|
|Run all component tests|`atf_comp`|
|Execute one component test|`atf_comp <comptest>`|
|Print a component test's script|`atf_comp -mode:printinput <comptest>`|
|Edit component test|`atf_comp <comptest> -e`|
|Capture reference output for a component test|`atf_comp <comptest> -capture`|
|Run cppcheck on all sources|`atf_ci cppcheck`|
|Run one CI job|`atf_ci -cijob:<cijob>`|
|Run the pre-merge gate|`bin/normalize`|

### Document
<a href="#document"></a>

Reference: [abt_md](/txt/exe/abt_md/README.md).

|What|Command|
|---|---|
|Regenerate one markdown file|`abt_md <mdfile regx>`|
|Regenerate without running the inline commands|`abt_md -evalcmd:N`|
|Refresh the docs of one namespace|`abt_md -ns:<ns regx>`|
|Check documentation for errors|`abt_md -check`|
|Check documentation for errors and open bad locations in editor|`errlist abt_md -check`|
|Check documentation internal & external links|`abt_md -check -external %`|

### Package
<a href="#package"></a>

Reference: [apm](/txt/exe/apm/README.md).

|What|Command|
|---|---|
|List installed packages|`apm -list`|
|List the records a package carries|`apm -package:<pkg> -l -showrec`|
|List the files a package carries|`apm -package:<pkg> -l -showfile`|
|Diff a package against its installed version|`apm -package:<pkg> -diff`|
|Regenerate a package manifest|`apm -package:<pkg> -generate`|
|Push a package to its origin|`apm '<pkg regx>' -push -origin <dir>`|
