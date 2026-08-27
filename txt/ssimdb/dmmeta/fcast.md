## dmmeta.fcast - Generate implicit conversion from field to c++ expression
<a href="#dmmeta-fcast"></a>

The fcast record adds an implicit conversion operator to the parent record.
Use it sparingly.
Here is an example:

```
    dmmeta.fcast  field:algo.Bool.value  expr:""  comment:""
```

This adds the following function to the `algo::Bool` type:

```
    inline operator algo_BoolEnum() const;
```

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcast

* file:[data/dmmeta/fcast.ssim](/data/dmmeta/fcast.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|expr|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFcast](/txt/gen/amc/amc.md#amc-ffcast)
