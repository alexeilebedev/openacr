## ietf - Some IETF structs


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Ctypes](#ctypes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### ietf.Ipport - IP+Port: host format
<a href="#ietf-ipport"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ip|[ietf.Ipv4](/txt/protocol/ietf/README.md#ietf-ipv4)|[Val](/txt/exe/amc/reftype.md#val)|||
|port|u16|[Val](/txt/exe/amc/reftype.md#val)|||

#### ietf.Ipv4
<a href="#ietf-ipv4"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ipv4|u32|[Val](/txt/exe/amc/reftype.md#val)||IP address. First octet in bits 24..32|

#### ietf.Ipv4Addr
<a href="#ietf-ipv4addr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|addr|u32|[Val](/txt/exe/amc/reftype.md#val)|||

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
void ietf::Ipport_Print(ietf::Ipport row, algo::cstring &str)
```

```c++
bool ietf::Ipport_ReadStrptrMaybe(ietf::Ipport &row, strptr in)
```

```c++
void ietf::Ipv4Addr_Print(ietf::Ipv4Addr row, algo::cstring &str)
```

```c++
// Parse an IpV4 address from STR to IP
// Return success value. If not successful, output value is not modified.
bool ietf::Ipv4Addr_ReadStrptrMaybe(ietf::Ipv4Addr &ip, algo::strptr str)
```

```c++
void ietf::Ipv4_Print(ietf::Ipv4 row, algo::cstring &str)
```

```c++
bool ietf::Ipv4_ReadStrptrMaybe(ietf::Ipv4 &ip, algo::strptr str)
```
