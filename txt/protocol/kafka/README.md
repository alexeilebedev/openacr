## kafka - Kafka protocol


### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### kafka.AclOperationType - AclOperation type
<a href="#kafka-acloperationtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.AclOperations - ACL operations
<a href="#kafka-acloperations"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|UNKNOWN|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|ANY|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|ALL|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|READ|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|WRITE|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|CREATE|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|DELETE|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|ALTER|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|DESCRIBE|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|CLUSTER_ACTION|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|DESCRIBE_CONFIGS|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|ALTER_CONFIGS|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|IDEMPOTENT_WRITE|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|CREATE_TOKENS|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|DESCRIBE_TOKENS|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|OMITTED|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||

#### kafka.AclPermissionType - AclPermission type
<a href="#kafka-aclpermissiontype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.CompressionType - 
<a href="#kafka-compressiontype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.ConfigSource - Config type
<a href="#kafka-configsource"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.ConfigType - Config type
<a href="#kafka-configtype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.Error - 
<a href="#kafka-error"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|i16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.Frame - Frame of binary Kafka protocol
<a href="#kafka-frame"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|size|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|payload|u8|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### kafka.GroupRecordKeyHeader - 
<a href="#kafka-grouprecordkeyheader"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|type|i16|[Val](/txt/exe/amc/reftypes.md#val)|||
|version|i16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.GroupRecordValueHeader - 
<a href="#kafka-grouprecordvalueheader"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|type|i16|[Val](/txt/exe/amc/reftypes.md#val)|||
|version|i16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.GroupState - Group state
<a href="#kafka-groupstate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|5||

#### kafka.Header - 
<a href="#kafka-header"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.OffsetCommitKey - 
<a href="#kafka-offsetcommitkey"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[kafka.GroupRecordKeyHeader](/txt/protocol/kafka/README.md#kafka-grouprecordkeyheader)|[Base](/txt/exe/amc/reftypes.md#base)|||
|group|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|topic|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|partition|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.OffsetCommitValue - 
<a href="#kafka-offsetcommitvalue"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[kafka.GroupRecordValueHeader](/txt/protocol/kafka/README.md#kafka-grouprecordvalueheader)|[Base](/txt/exe/amc/reftypes.md#base)|||
|offset|i64|[Val](/txt/exe/amc/reftypes.md#val)|||
|leader_epoch|i32|[Val](/txt/exe/amc/reftypes.md#val)|-1||
|metadata|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|commit_timestamp|i64|[Val](/txt/exe/amc/reftypes.md#val)|||
|expire_timestamp|i64|[Val](/txt/exe/amc/reftypes.md#val)|-1||

#### kafka.PatternType - Pattern type
<a href="#kafka-patterntype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.Record - 
<a href="#kafka-record"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|attributes|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|pmask|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|timestamp_delta|i64|[Val](/txt/exe/amc/reftypes.md#val)|||
|offset_delta|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|headers|[kafka.Header](/txt/protocol/kafka/README.md#kafka-header)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### kafka.RecordBatch - 
<a href="#kafka-recordbatch"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base_offset|i64|[Val](/txt/exe/amc/reftypes.md#val)|||
|partition_leader_epoch|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|magic|i8|[Val](/txt/exe/amc/reftypes.md#val)|2||
|crc|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|attributes|i16|[Val](/txt/exe/amc/reftypes.md#val)|||
|last_offset_delta|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|base_timestamp|i64|[Val](/txt/exe/amc/reftypes.md#val)|||
|max_timestamp|i64|[Val](/txt/exe/amc/reftypes.md#val)|||
|producer_id|i64|[Val](/txt/exe/amc/reftypes.md#val)|-1||
|producer_epoch|i16|[Val](/txt/exe/amc/reftypes.md#val)|-1||
|base_sequence|i32|[Val](/txt/exe/amc/reftypes.md#val)|-1||
|records|[kafka.Record](/txt/protocol/kafka/README.md#kafka-record)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### kafka.ResourceType - Resource type
<a href="#kafka-resourcetype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### kafka.TimestampType - 
<a href="#kafka-timestamptype"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->

