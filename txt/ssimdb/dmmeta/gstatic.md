## dmmeta.gstatic - Load entries for this table at startup time
<a href="#dmmeta-gstatic"></a>

Whenever gstatic is specified, the contents of an ssim
table are ``statically'' included into the output source file for the given namespace
The contents of the table are inserted into the in-memory database at startup,
as part of FDb's constructor.

When gstatic is combined with Inlary and the field is global (i.e. in `FDb`), the individual addresses
of elements in the table are compile-time constants. Amc celebrates this fact by declaring a C++ compiler symbol
for each row of the table. By the time the user code starts executing, all of the elements have been inserted
in the array and properly cross-referenced, and the user code can use the generated globals to access table rows.

Here is an example:
```
dmmeta.field  field:atf_amc.FDb.listtype  arg:atf_amc.FListtype  reftype:Inlary  dflt:""  comment:""
    dmmeta.gstatic  field:atf_amc.FDb.listtype  comment:""
    dmmeta.inlary  field:atf_amc.FDb.listtype  min:0  max:10  comment:""
```

This generates the following compile-time references:
```
static atf_amc::FListtype &atf_amc_listtype_cd    = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[0];
static atf_amc::FListtype &atf_amc_listtype_cdl   = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[1];
static atf_amc::FListtype &atf_amc_listtype_cs    = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[2];
static atf_amc::FListtype &atf_amc_listtype_csl   = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[3];
```

Compare `gstatic` to [finput](/txt/ssimdb/dmmeta/finput.md), which indicates that records can be dynamically loaded
into the process at runtime (and at startup, by reading an input file). By contrast, gstatic records become
part of the source code.

Tables marked `gstatic` have known contents. This enables associating entries of such tables with C functions.
Consider this example from amc:
```
  dmmeta.field  field:amc.FDb.gen  arg:amc.FGen  reftype:Lary  dflt:""  comment:""
    dmmeta.gstatic  field:amc.FDb.gen  comment:""

  dmmeta.field  field:amc.FGen.step  arg:""  reftype:Hook  dflt:""  comment:""
      dmmeta.hook  field:amc.FGen.step  comment:""
```
Here, a static table `gen` has been declared, and its contents are populated with entires from
`amcdb.gen` in amc's FDb constructor. The table can have a hash index by primary key and there
is not restriction on any other cross-references. For this pattern of declaration, amc forward-declares one
function foreach element of the table, and assigns its address to the `hook` field of `amc::FGen` upon construction.
If the function is not defined in the user process, a link error will result.

This construct is very useful for things like unit tests, consistency checks, alerts, and other situations where
an extensible list of functions is needed together with meta-information about them.
Each function has an associated record which can be looked up by name, so it is possible to invoke all
functions matching a regex passed on command line, or describe them with comments.

Example:
Show first 10 entries from amcdb.gen:

```
inline-command: acr amcdb.gen | sort -k 2 | head | ssimfilt ^ -t
GEN              PERNS  COMMENT
basepool         N      Create basepools based on defaults
bitfldenum       N      Generate fconsts from bool bitfield
cget             N      Generate helpful Q functions based on fconsts and bools
check_basefield  N      Check Base usage
check_basepool   N      Check basepool order
check_bigend     N      Check big-endians
check_bitfld     N      Check that bitfields don't overlap

```

Select first 10 functions from amc source code:
```
inline-command: src_func amc gen_% -proto -comment: | sort -k 3 | head
cpp/amc/gen.cpp:230: void amc::gen_basepool() 
cpp/amc/cget.cpp:121: void amc::gen_cget()
cpp/amc/gen.cpp:614: void amc::gen_check_bigend() 
cpp/amc/gen.cpp:310: void amc::gen_check_bitfld() 
cpp/amc/gen.cpp:110: void amc::gen_check_cascdel() 
cpp/amc/gen.cpp:496: void amc::gen_check_cpptype() 
cpp/amc/gen.cpp:1411: void amc::gen_check_fcurs() 
cpp/amc/gen.cpp:215: void amc::gen_check_prefix() 
cpp/amc/gen.cpp:432: void amc::gen_check_reftype() 
cpp/amc/gen.cpp:136: void amc::gen_check_ssimsort() 
```

In language-centric systems, one would start by writing some functions, and then use "reflection" to
scrape the functions from the code (usually at runtime) and do things with them. Starting with a plain text file
on disk, and then generating C++ functions and in-memory tables from it is a lot more elegant. No "reflection"
is needed, everything is forward-looking.

Gstatic also behaves in a special way when used with records containing `Hook` fields.
See [Hook](hook.md)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Gstatic

* file:[data/dmmeta/gstatic.ssim](/data/dmmeta/gstatic.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.LL of field|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FGstatic
* [src_func](/txt/exe/src_func/README.md) as src_func.FGstatic

