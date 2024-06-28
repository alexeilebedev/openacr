## ams.Member - Process/Stream/Mode - primary key for reader/writer of stream


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.Member

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc_id|ams.ProcId|Val||Proces ID|
|stream_id|ams.StreamId|Val||Stream ID|
|mode|u8|Val|0|0=read, 1=write (2 or more =read)|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

