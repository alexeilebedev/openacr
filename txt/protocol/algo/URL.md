## algo.URL - URL parsed into components


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)algo.URL

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|protocol|algo.cstring|Val|||
|username|algo.cstring|Val|||
|password|algo.cstring|Val||password|
|server|algo.cstring|Val||www.example.com or A.B.C.D|
|dir|algo.cstring|Val||/subdir/index.html|
|port|i32|Val|-1|80|
|source_addr_host|u32|Val||source address in host format (valid for multicast addresses only|
|host|algo.cstring|Val||0.0.0.0, or A.B.C.D, or 192.168.1.* or eth2|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

