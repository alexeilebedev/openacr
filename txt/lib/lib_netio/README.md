## lib_netio - Network I/O library


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
// accept remote connection:
// return connection socket and fills ipport with client address/port
algo::Fildes lib_netio::Accept(algo::Fildes listen_sock, ietf::Ipport &ipport) 
```

```c++
// Wrapper for bind() -- Ipport
bool lib_netio::Bind(algo::Fildes sock, ietf::Ipport ipport) 
```

```c++
// Wrapper for bind() -- strptr
bool lib_netio::Bind(algo::Fildes sock, strptr addr) 
```

```c++
// Wrapper for bind to netlink
bool lib_netio::BindNetlink(algo::Fildes sock) 
```

```c++
// Wrapper for connect() -- ipport
bool lib_netio::Connect(algo::Fildes sock, ietf::Ipport ipport) 
```

```c++
// Wrapper for connect() -- strptr
int lib_netio::Connect(algo::Fildes sock, strptr addr) 
```

```c++
// Create Netlink socket
algo::Fildes lib_netio::CreateNetlinkSocket() 
```

```c++
// Create TCP socket
algo::Fildes lib_netio::CreateTcpSocket() 
```

```c++
// Create UDP socket
algo::Fildes lib_netio::CreateUdpSocket() 
```

```c++
// find interface name for known ip
tempstr lib_netio::FindInterfaceByIpv4(algo::Fildes sock, ietf::Ipv4 &ip) 
```

```c++
tempstr lib_netio::GetHostAddr(strptr hostname) 
```

```c++
// get hardware address for intrface
bool lib_netio::GetHwAddrFamily(algo::Fildes sock, strptr name, sa_family_t &result) 
```

```c++
// get list of interface names as space-separated string
tempstr lib_netio::GetInterfaces(algo::Fildes sock) 
```

```c++
// Get local ip/port -- getsockname()
bool lib_netio::GetIpportLocal(algo::Fildes sock, ietf::Ipport &ipport) 
```

```c++
// Get remote ip/port -- getpeername()
bool lib_netio::GetIpportRemote(algo::Fildes sock, ietf::Ipport &ipport) 
```

```c++
// get ip address of interface
bool lib_netio::GetIpv4(algo::Fildes sock, strptr name, ietf::Ipv4 &result) 
```

```c++
// Get socket error -- getsockopt(SO_ERROR);
// in case of getsockopt failure, return errno
int lib_netio::GetSocketError(algo::Fildes sock) 
```

```c++
// ioctl with ifreq
bool lib_netio::Ioctl(algo::Fildes sock, strptr name, u32 request, ifreq &ifr) 
```

```c++
// Wrapper for listen() -- strptr
bool lib_netio::Listen(algo::Fildes sock, int backlog) 
```

```c++
inline bool lib_netio::MulticastQ(ietf::Ipv4 addr) 
```

```c++
// send GETLINK netlink request
bool lib_netio::RequestLinkDump(algo::Fildes sock) 
```

```c++
// Resolve ip:port or <dnsname>:<port> to an Ipport struct
ietf::Ipport lib_netio::Resolve(algo::strptr addr) 
```

```c++
// Wrapper for setsockopt(SO_LINGER) - timeout in seconds to drain output buffers before close().
// set 0 to drop connection with RST.
bool lib_netio::SetLinger(algo::Fildes sock, algo::UnixDiff timeout = algo::UnixDiff()) 
```

```c++
// Set multicast send interface (default specified by system administrator)
bool lib_netio::SetMulticastInterface(algo::Fildes sock, ietf::Ipv4 interface) 
```

```c++
// Set multicast loop (enabled by defult)
bool lib_netio::SetMulticastLoop(algo::Fildes sock, bool loop) 
```

```c++
// Join or leave multicast group (on the interface basis!)
bool lib_netio::SetMulticastMembership(algo::Fildes sock, ietf::Ipv4 group, ietf::Ipv4 interface, bool membership) 
```

```c++
// Set multicast TTL
// TTL     Scope
// 0    Restricted to the same host. Won't be output by any interface.
// 1    Restricted to the same subnet. Won't be forwarded by a router. This is the default.
// <32    Restricted to the same site, organization or department.
// <64    Restricted to the same region.
// <128    Restricted to the same continent.
// <255    Unrestricted in scope. Global.
bool lib_netio::SetMulticastTtl(algo::Fildes sock, u8 ttl) 
```

```c++
// Set receive buffer size -- setsockopt(SO_RCVBUF)
bool lib_netio::SetReceiveBufferSize(algo::Fildes sock, int size) 
```

```c++
// Wrapper for setsockopt(SO_REUSEADDR)
bool lib_netio::SetReuseAddress(algo::Fildes sock, bool value = true) 
```

```c++
// Set send buffer size -- setsockopt(SO_SNDBUF)
bool lib_netio::SetSendBufferSize(algo::Fildes sock, int size) 
```

```c++
// Wrapper for setsockopt
template<typename T> bool lib_netio::SetSocketOption(algo::Fildes sock, int level, int option, const T &value) 
```

```c++
// Set TCP keepalive
bool lib_netio::SetTcpKeepalive(algo::Fildes sock, bool on = true, algo::UnixDiff idle = algo::UnixDiff(2), algo::UnixDiff interval = algo::UnixDiff(2), int max_probes = 5) 
```

```c++
// Wrapper for setsockopt(TCP_NODELAY)
bool lib_netio::SetTcpNoDelay(algo::Fildes sock, bool value = true) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_netio` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_netio_gen.cpp](/cpp/gen/lib_netio_gen.cpp)||
|[cpp/lib_netio/socket.cpp](/cpp/lib_netio/socket.cpp)||
|[include/gen/lib_netio_gen.h](/include/gen/lib_netio_gen.h)||
|[include/gen/lib_netio_gen.inl.h](/include/gen/lib_netio_gen.inl.h)||
|[include/lib_netio.h](/include/lib_netio.h)||
|[include/lib_netio.inl.h](/include/lib_netio.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`lib_netio` generated code creates the tables below.
All allocations are done through global `lib_netio::_db` [lib_netio.FDb](#lib_netio-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_netio.FDb](#lib_netio-fdb)||FDb._db (Global)|
|[lib_netio.FVar](#lib_netio-fvar)||FDb.var (Lary)|var (Lary, by rowid)|ind_var (Thash, hash field var)|

#### lib_netio.FDb - 
<a href="#lib_netio-fdb"></a>

#### lib_netio.FDb Fields
<a href="#lib_netio-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_netio.FDb._db|[lib_netio.FDb](/txt/lib/lib_netio/README.md#lib_netio-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|lib_netio.FDb.var|[lib_netio.FVar](/txt/lib/lib_netio/README.md#lib_netio-fvar)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_netio.FDb.ind_var|[lib_netio.FVar](/txt/lib/lib_netio/README.md#lib_netio-fvar)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_netio_gen.h](/include/gen/lib_netio_gen.h)
```
struct FDb { // lib_netio.FDb
    lib_netio::FVar*    var_lary[32];            // level array
    i32                 var_n;                   // number of elements in array
    lib_netio::FVar**   ind_var_buckets_elems;   // pointer to bucket array
    i32                 ind_var_buckets_n;       // number of elements in bucket array
    i32                 ind_var_n;               // number of elements in the hash table
    lib_netio::trace    trace;                   //
};
```

#### lib_netio.FVar - 
<a href="#lib_netio-fvar"></a>

#### lib_netio.FVar Fields
<a href="#lib_netio-fvar-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_netio.FVar.var|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_netio.FVar.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FVar
<a href="#struct-fvar"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_netio_gen.h](/include/gen/lib_netio_gen.h)
```
struct FVar { // lib_netio.FVar
    lib_netio::FVar*   ind_var_next;      // hash next
    u32                ind_var_hashval;   // hash value
    algo::cstring      var;               //
    algo::cstring      value;             //
    // func:lib_netio.FVar..AssignOp
    inline lib_netio::FVar& operator =(const lib_netio::FVar &rhs) = delete;
    // func:lib_netio.FVar..CopyCtor
    inline               FVar(const lib_netio::FVar &rhs) = delete;
private:
    // func:lib_netio.FVar..Ctor
    inline               FVar() __attribute__((nothrow));
    // func:lib_netio.FVar..Dtor
    inline               ~FVar() __attribute__((nothrow));
    friend lib_netio::FVar&     var_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_netio::FVar*     var_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 var_RemoveAll() __attribute__((nothrow));
    friend void                 var_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

