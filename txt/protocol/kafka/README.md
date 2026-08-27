## kafka - Kafka protocol


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### kafka.AclOperationType - AclOperation type
<a href="#kafka-acloperationtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.AclOperations - ACL operations
<a href="#kafka-acloperations"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|UNKNOWN|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|ANY|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|ALL|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|READ|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|WRITE|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|CREATE|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|DELETE|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|ALTER|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|DESCRIBE|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|CLUSTER_ACTION|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|DESCRIBE_CONFIGS|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|ALTER_CONFIGS|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|IDEMPOTENT_WRITE|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|CREATE_TOKENS|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|DESCRIBE_TOKENS|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||
|OMITTED|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)|||

#### kafka.AclPermissionType - AclPermission type
<a href="#kafka-aclpermissiontype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.CompressionType
<a href="#kafka-compressiontype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.ConfigSource - Config type
<a href="#kafka-configsource"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.ConfigType - Config type
<a href="#kafka-configtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.Error
<a href="#kafka-error"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i16|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.Frame - Frame of binary Kafka protocol
<a href="#kafka-frame"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|size|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|payload|u8|[Varlen](/txt/exe/amc/reftype/Varlen.md)|||

#### kafka.GroupRecordKeyHeader
<a href="#kafka-grouprecordkeyheader"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|type|i16|[Val](/txt/exe/amc/reftype/Val.md)|||
|version|i16|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.GroupRecordValueHeader
<a href="#kafka-grouprecordvalueheader"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|type|i16|[Val](/txt/exe/amc/reftype/Val.md)|||
|version|i16|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.GroupState - Group state
<a href="#kafka-groupstate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|5||

#### kafka.Header
<a href="#kafka-header"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.OffsetCommitKey
<a href="#kafka-offsetcommitkey"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[kafka.GroupRecordKeyHeader](/txt/protocol/kafka/README.md#kafka-grouprecordkeyheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|group|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|topic|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|partition|i32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.OffsetCommitValue
<a href="#kafka-offsetcommitvalue"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[kafka.GroupRecordValueHeader](/txt/protocol/kafka/README.md#kafka-grouprecordvalueheader)|[Base](/txt/exe/amc/reftype/Base.md)|||
|offset|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|leader_epoch|i32|[Val](/txt/exe/amc/reftype/Val.md)|-1||
|metadata|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|commit_timestamp|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|expire_timestamp|i64|[Val](/txt/exe/amc/reftype/Val.md)|-1||

#### kafka.PatternType - Pattern type
<a href="#kafka-patterntype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.Record
<a href="#kafka-record"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|attributes|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
|pmask|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
|timestamp_delta|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|offset_delta|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|headers|[kafka.Header](/txt/protocol/kafka/README.md#kafka-header)|[Tary](/txt/exe/amc/reftype/Tary.md)|||

#### kafka.RecordBatch
<a href="#kafka-recordbatch"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base_offset|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|partition_leader_epoch|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|magic|i8|[Val](/txt/exe/amc/reftype/Val.md)|2||
|crc|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|attributes|i16|[Val](/txt/exe/amc/reftype/Val.md)|||
|last_offset_delta|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|base_timestamp|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|max_timestamp|i64|[Val](/txt/exe/amc/reftype/Val.md)|||
|producer_id|i64|[Val](/txt/exe/amc/reftype/Val.md)|-1||
|producer_epoch|i16|[Val](/txt/exe/amc/reftype/Val.md)|-1||
|base_sequence|i32|[Val](/txt/exe/amc/reftype/Val.md)|-1||
|records|[kafka.Record](/txt/protocol/kafka/README.md#kafka-record)|[Tary](/txt/exe/amc/reftype/Tary.md)|||

#### kafka.ResourceType - Resource type
<a href="#kafka-resourcetype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### kafka.TimestampType
<a href="#kafka-timestamptype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
