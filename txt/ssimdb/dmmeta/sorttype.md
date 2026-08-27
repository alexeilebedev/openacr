## dmmeta.sorttype - Sort type
<a href="#dmmeta-sorttype"></a>
List of supported sort types
```
inline-command: acr sorttype | ssimfilt ^ -t
SORTTYPE       COMMENT
QuickSort
InsertionSort
HeapSort

```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Sorttype

* file:[data/dmmeta/sorttype.ssim](/data/dmmeta/sorttype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|sorttype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.sorttype

* [dmmeta.fsort via sorttype](/txt/ssimdb/dmmeta/fsort.md) - Kind of sort to generate
