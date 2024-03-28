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

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|expr|algo.Smallstr100|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFcast

