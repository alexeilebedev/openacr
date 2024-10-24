## ams.MsgBlock - A block of messages from ams commit file


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.MsgBlock

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|header|ams.MsgHeader|Base|||
|first_seqno|u64|Val||First sequence number |
|n_messages|u32|Val||Number of messages|
|original_length|u32|Val||Original (uncompressed length) of messages|
|messages|u8|Varlen||LZ4 compressed messages|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

