## dmmeta.func -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Func

* file:[data/dmmeta/func.ssim](/data/dmmeta/func.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|func|algo.Smallstr100|Val||Primary key|
|inl|bool|Val||Make inline?|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||** doesn't always work<br>.RL of func|
|name|algo.Smallstr50|Val||** doesn't always work<br>.RR of func|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||** doesn't always work<br>.RL.RL of func|
|proto|algo.cstring|Val||Prototype|
|body|algo.cstring|Val||Body of function|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.RL.RL.RL of func|
|deprecate|bool|Val||Make deprecated?|
|ismacro|bool|Val||Always inline.|
|extrn|bool|Val||Implemented externally by user|
|glob|bool|Val||Function is not attached to a field (EXPLAIN!!)|
|priv|bool|Val||Private to translateion unit (static)|
|ret|algo.cstring|Val||Return type (c++ expression)|
|comment|algo.cstring|Val||Description|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFunc

