## dmmeta.ckafka -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ckafka

* file:[data/dmmeta/ckafka.ssim](/data/dmmeta/ckafka.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|kind|[dmmeta.KafkaTypeKind](/txt/ssimdb/dmmeta/kafka_type_kind.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|root|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|valid_versions|[algo.Smallstr10](/txt/protocol/algo/README.md#algo-smallstr10)|[Val](/txt/exe/amc/reftype/Val.md)|||
|flexible_versions|[algo.Smallstr10](/txt/protocol/algo/README.md#algo-smallstr10)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FCkafka](/txt/gen/amc/amc.md#amc-fckafka)
