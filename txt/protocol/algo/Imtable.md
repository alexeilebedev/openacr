## txt/protocol/algo/Imtable.md


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)algo.Imtable

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|imtable|algo.Smallstr50|Val||Table name|
|elem_type|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||Element type name|
|c_RowidFind|algo.ImrowRowidFindFcn|Val|0|Function to find element by rowid (may be NULL)|
|XrefX|algo.ImrowXrefXFcn|Val|0|Function to x-reference an element (may be NULL)|
|NItems|algo.ImrowNItemsFcn|Val|0|Return number of elements in the table|
|Print|algo.ImrowPrintFcn|Val|0|Convert specified element to string (may be NULL)|
|size|i32|Val||Size of one element (for fixed-width elements only)|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|Pkey||Ssimfile name (if associated)|
|comment|algo.Comment|Val|

