## txt/protocol/ams/UdpFrame.md


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.UdpFrame

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|header|ams.MsgHeader|Base|
|timestamp|algo.UnTime|Val||Timestamp|
|src_ip|ietf.Ipv4|Val||Source IP|
|dst_ip|ietf.Ipv4|Val||Destination IP|
|src_port|u16|Val||Source UDP port|
|dst_port|u16|Val||Destination UDP port|
|payload|u8|Varlen||Payload|

