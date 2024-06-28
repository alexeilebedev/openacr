## ams.UdpFrame - sniffer-captured ethernet frame


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.UdpFrame

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|header|ams.MsgHeader|Base|||
|timestamp|algo.UnTime|Val||Timestamp|
|src_ip|ietf.Ipv4|Val||Source IP|
|dst_ip|ietf.Ipv4|Val||Destination IP|
|src_port|u16|Val||Source UDP port|
|dst_port|u16|Val||Destination UDP port|
|payload|u8|Varlen||Payload|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

