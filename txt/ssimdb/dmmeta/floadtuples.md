## dmmeta.floadtuples - Request that process automatically load any input tables on startup
<a href="#dmmeta-floadtuples"></a>
A row names one command-line field that holds the path of a tuple source, so
`command.acr_ed.in` says that acr_ed reads its tables from the directory `-in`
points at.  A command may name more than one, because its rows do not all come
from the same place: a tool may read the source tree at `-in` and then the
infrastructure inventory at `-invdir`, and each field gets a row of its own.

Two attributes say what a reader may do with a source.  `optional` marks a
source that need not be there — a layer nobody has attached is the ordinary
state of a fresh checkout, so the process skips it rather than failing, and the
generated startup load reads only the sources without the mark.  `autoscan`
marks a source whose contents a user may name on a command line, which is what
`acr_compl` reads when it offers completions: a node key that lives only in the
inventory is offered from there exactly as a ctype key is offered from `data`.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Floadtuples

* file:[data/dmmeta/floadtuples.ssim](/data/dmmeta/floadtuples.ssim)

italicised fields: *ctype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL of field*|
|autoscan|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|acr_compl scans this location for completion candidates|
|optional|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|a layer beyond the primary source: absent means skip, not fail|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FFloadtuples](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffloadtuples)
* [amc](/txt/gen/amc/amc.md) as [amc.FFloadtuples](/txt/gen/amc/amc.md#amc-ffloadtuples)
