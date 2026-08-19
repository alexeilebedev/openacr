## dmmeta.pbsyntax - Protobuf syntax variant selecting default-value and presence semantics
<a href="#dmmeta-pbsyntax"></a>
The protobuf syntax a message follows, named by
[dmmeta.cpbuf](/txt/ssimdb/dmmeta/cpbuf.md).  The two variants differ in how a
field's absence is expressed on the wire, which is what the generated encoder
has to decide for every field it writes.

The syntax governs one decision only: whether a scalar equal to its zero value
is skipped.  That is what `implicit_presence` names, and it is the attribute
the encoder reads — the syntax name itself does not appear in the generator, so
a further variant is a row here rather than a code change.  A syntax that has
no row is rejected: `acr -check` refuses the `cpbuf` row, and `amc` refuses the
schema it was handed, since a schema read straight out of a directory does not
pass through the reference check.  A field that carries a presence mask
([dmmeta.pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)) is written when its present
bit is set, under either syntax, so `syntax:proto2` on a ctype without a
presence mask writes every field on every encode.

```
inline-command: acr pbsyntax | ssimfilt ^ -t
PBSYNTAX  IMPLICIT_PRESENCE  COMMENT
proto2    N                  Explicit presence: a scalar is written even when equal to its zero value
proto3    Y                  Implicit presence: a scalar equal to its zero value is not written

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Pbsyntax

* file:[data/dmmeta/pbsyntax.ssim](/data/dmmeta/pbsyntax.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pbsyntax|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|implicit_presence|bool|[Val](/txt/exe/amc/reftype.md#val)||A scalar equal to its zero value is not written to the wire|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.pbsyntax

* [dmmeta.cpbuf via syntax](/txt/ssimdb/dmmeta/cpbuf.md) - Protobuf syntax variant this message follows

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FPbsyntax](/txt/gen/amc/amc.md#amc-fpbsyntax)
