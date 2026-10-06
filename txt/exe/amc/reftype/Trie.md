## amc Reftype: Trie
<a href="#amc-reftype-trie"></a>

`Trie` holds values keyed by a dense integer, in a tree of fixed-size nodes.
Take an index of a stream's records by position, one slot per position.  In a
`Tary`, the append that fills the array copies every entry into one twice the
size, and at millions of entries that one append costs milliseconds.  A trie
never copies: each level branches on `nbit` bits of the key, so the tree grows
one fixed node at a time, and a removed range gives whole leaves back.

```ssim
dmmeta.field  field:atf_amc.TrieU64.trie  arg:u64  reftype:Trie  dflt:44  comment:""
  dmmeta.trie  field:atf_amc.TrieU64.trie  linfld:algo.SeqType.value  nbit:8  comment:""
```

`linfld` names the integer the key reduces to, through any single-field wrappers,
and the key type is the ctype that holds it.  A node has 2^`nbit` children and a
leaf holds 2^`nbit` value slots, with an occupancy bit for each, so a slot is
empty or holds a value whatever the value is.  The key is absolute: the root
covers keys below 2^(`nbit`·(height+1)), and a larger key adds a level on top.
The parent caches the leaf the last lookup found, so consecutive keys find
their slot with one compare.

### Operations
<a href="#operations"></a>

h is the trie's height and F = 2^`nbit`.

| function | effect | cost |
|---|---|---|
| `<name>_Find(parent, key)` | the value at KEY, or NULL when the slot is empty | 1 compare on the cached leaf, else h+1 loads |
| `<name>_Alloc(parent, key)` | the value at KEY, constructed with the field's `dflt` in an empty slot | as Find; a new leaf is 2 allocations and a new level 1 |
| `<name>_Remove(parent, key)` | destroy the value at KEY | as RemoveRange |
| `<name>_RemoveRange(parent, lo, hi)` | destroy the values at LO..HI−1; a covered leaf or subtree is freed whole | (HI−LO)/F leaf frees, plus 2 edge leaves slot by slot |
| `<name>_NextKey(parent, from, out)` | the lowest held key at or above FROM | as Find, plus a scan of occupancy words |
| `<name>_PrevKey(parent, from, out)` | the highest held key at or below FROM | as NextKey |
| `<name>_RemoveAll(parent)` | destroy every value and free every node | O(nodes) |
| `<name>_N`, `<name>_EmptyQ` | the number of held values, and whether it is zero | O(1) |
| `<name>_Bytes(parent)` | the memory its resident leaves and interior nodes hold | O(1) |
| cursor (`dmmeta.fcurs <field>/curs`) | the held values in key order | O(1) amortized per value |

The parent carries `<name>_nleaf` and `<name>_nnode`, the resident leaves and
interior nodes, from which the trie's memory follows.  A parent holding a trie
cannot be copied.

### Memory
<a href="#memory"></a>

A leaf is two allocations, a small header and an array of F value slots, and an
interior node is F pointers, which at `nbit:8` is 2 KB each.  A non-empty trie
holds at least one leaf.  Keys that are not dense cost a leaf each, so a trie over
scattered keys is the wrong reftype; use [Thash](/txt/exe/amc/reftype/Thash.md).

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Blkhash](/txt/exe/amc/reftype/Blkhash.md), the hash index over dense keys it shares `linfld` with
- Source: `cpp/amc/trie.cpp`
- Tfunc records: `acr 'tfunc:Trie.%'`
