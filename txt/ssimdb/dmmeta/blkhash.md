## dmmeta.blkhash - Block hash index, required for fields with reftype Blkhash
<a href="#dmmeta-blkhash"></a>

This record is required on fields with reftype:Blkhash.

A Blkhash is a unique hash index for keys with a dense integer component.
Consider an index of sequenced messages keyed by (stream id, seqno).
A regular hash function must spread all keys uniformly, so ten million
cached messages cost a ten-million-pointer bucket array that doubles and
rehashes as it grows, and neighboring seqnos land in unrelated cache lines.
But seqnos are consecutive: only the stream id needs spreading.

The blkhash record declares the dense component (`linfld`, a field of the
key ctype, possibly wrapped in a single-field type such as algo.SeqType)
and a width `linbits`.  The low `linbits` bits of the component select a
slot directly inside a block of 2^linbits entry pointers.  The rest of the
key, with those bits masked off, identifies the block and is hashed to
find it in a small bucket array of block chains.

A matched block plus a slot determines the full key exactly, so the index
is inherently unique, needs no collision chain, and adds no intrusive
fields to the row.  Consecutive keys of one stream occupy consecutive
slots of one block.  Blocks are fixed-size and come from the basepool;
a block whose last slot clears is freed and recycled for the next block
of the same index, so a rolling window of keys (append at the tail,
retire at the head) reaches a steady state with no allocation and no
rehashing.  Only the small bucket array (one pointer per resident block)
grows with population.

A resident block costs 2^linbits pointers regardless of occupancy, so
pick linbits at or below the logarithm of the typical resident run of
consecutive keys; sparse keys degrade the index into one block per key.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Blkhash

* file:[data/dmmeta/blkhash.ssim](/data/dmmeta/blkhash.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|hashfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Key field; blocks are keyed on it with linfld low bits masked off|
|linfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Dense integer component of the key; a field of hashfld's arg|
|linbits|u8|[Val](/txt/exe/amc/reftype.md#val)||Block holds 2^linbits slots; low linfld bits select the slot directly|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FBlkhash](/txt/gen/amc/amc.md#amc-fblkhash)
