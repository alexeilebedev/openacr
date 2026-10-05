// Copyright (C) 2024,2026 AlgoX2 Corp
// Copyright (C) 2024 AlgoRND
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: lib_netio (lib) -- Network I/O library
// Exceptions: yes
// Header: include/lib_netio.h
//

#include "include/gen/lib_netio_gen.h"
#include "include/gen/lib_netio_gen.inl.h"

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>

namespace lib_netio { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_netio/socket.cpp
    //

    // True when PATH fits a unix-domain socket address, so a bind or connect on it
    // can reach the kernel at all.  The address holds a fixed-size path with room
    // for a terminator, and a longer PATH is refused by SockaddrUn with
    // ENAMETOOLONG however many times it is tried.  A caller that retries a failed
    // bind asks this first, so it reports the path once and stops rather than
    // retrying a length that cannot change.
    bool SockPathFitsQ(algo::strptr path);

    // Create TCP socket, close-on-exec.
    // A descriptor that survives exec keeps what it holds for the whole life of
    // the exec'd program, and that program knows nothing about it: a listening
    // socket carried into an unrelated command keeps its port bound long after
    // the process that opened the port is gone, so the port reads as in use with
    // no owner that explains it.  A child that is meant to receive a socket gets
    // it as one of its standard streams, which the exec preserves by itself.
    algo::Fildes CreateTcpSocket();

    // Create UDP socket, close-on-exec (see CreateTcpSocket)
    algo::Fildes CreateUdpSocket();

    // Create a unix-domain stream socket, close-on-exec (see CreateTcpSocket).
    // It is addressed by a path on the host (BindUnix, ConnectUnix) and carries
    // the same byte stream a TCP socket does.
    algo::Fildes CreateUnixSocket();

    // Create Netlink socket, close-on-exec (see CreateTcpSocket).
    // Netlink exists on Linux only; elsewhere the socket is invalid and errno is ENOTSUP.
    algo::Fildes CreateNetlinkSocket();

    // Wrapper for bind() -- Ipport
    bool Bind(algo::Fildes sock, ietf::Ipport ipport);

    // Wrapper for bind() -- strptr
    bool Bind(algo::Fildes sock, strptr addr);

    // Bind unix-domain socket SOCK to the socket file PATH, and return TRUE when
    // it is bound.  A socket file outlives the process that bound it, so the one a
    // predecessor left is removed first; the bind then creates the file anew.
    // Failure leaves errno set, ENAMETOOLONG when PATH does not fit a socket
    // address.
    bool BindUnix(algo::Fildes sock, strptr path);

    // Wrapper for bind to netlink
    bool BindNetlink(algo::Fildes sock);

    // send GETLINK netlink request
    bool RequestLinkDump(algo::Fildes sock);

    // Wrapper for listen() -- strptr
    bool Listen(algo::Fildes sock, int backlog);

    // Wrapper for connect() -- ipport
    bool Connect(algo::Fildes sock, ietf::Ipport ipport);

    // Wrapper for connect() -- strptr
    int Connect(algo::Fildes sock, strptr addr);

    // Connect unix-domain socket SOCK to the socket file PATH, and return TRUE
    // when the connect completed.  On a non-blocking socket a connect to a
    // listening socket file completes at once; failure leaves errno set, ENOENT
    // when nothing has bound PATH.
    bool ConnectUnix(algo::Fildes sock, strptr path);

    // True when a TCP connection to IPPORT completes within MSEC milliseconds.
    //
    // Answering "is anything listening there" needs a deadline, because the two
    // negative answers look nothing alike on the wire.  A host whose network is up
    // but which has nothing bound to the port refuses the connection at once with
    // an RST.  A host that does not answer at all -- one whose network has not come
    // up yet, or one behind a firewall that drops rather than rejects -- leaves the
    // SYN unanswered, and the kernel then retries it for minutes.  A booting host
    // passes through both in turn: its SYN goes unanswered until the network is up,
    // and is then refused until the listener binds.  A blocking connect would park
    // the caller for the whole retry window on the unanswered case, so the connect
    // is issued non-blocking and the socket polled for writability until the
    // deadline, which reports either negative promptly and reports success only
    // once the handshake has completed.
    bool TcpUpQ(ietf::Ipport ipport, int msec);

    // accept remote connection on LISTEN_SOCK:
    // return connection socket and fill IPPORT with client address/port, zero
    // for a client on a unix-domain socket, which has no ip.
    // The accepted socket is close-on-exec for the reason the listening socket is
    // (see CreateTcpSocket); accept does not inherit the flag, so it is set again.
    algo::Fildes Accept(algo::Fildes listen_sock, ietf::Ipport &ipport);

    // Get socket error -- getsockopt(SO_ERROR);
    // in case of getsockopt failure, return errno
    int GetSocketError(algo::Fildes sock);

    // Wrapper for setsockopt(SO_REUSEADDR)
    bool SetReuseAddress(algo::Fildes sock, bool value = true);

    // Wrapper for setsockopt(TCP_NODELAY)
    bool SetTcpNoDelay(algo::Fildes sock, bool value = true);

    // Wrapper for setsockopt(SO_LINGER) - timeout in seconds to drain output buffers before close().
    // set 0 to drop connection with RST.
    bool SetLinger(algo::Fildes sock, algo::UnixDiff timeout = algo::UnixDiff());

    // Set TCP keepalive
    bool SetTcpKeepalive(algo::Fildes sock, bool on = true, algo::UnixDiff idle = algo::UnixDiff(2), algo::UnixDiff interval = algo::UnixDiff(2), int max_probes = 5);

    // Set send buffer size -- setsockopt(SO_SNDBUF)
    bool SetSendBufferSize(algo::Fildes sock, int size);

    // Set receive buffer size -- setsockopt(SO_RCVBUF)
    bool SetReceiveBufferSize(algo::Fildes sock, int size);

    // Set multicast loop (enabled by defult)
    bool SetMulticastLoop(algo::Fildes sock, bool loop);

    // Set multicast TTL
    // TTL     Scope
    // 0    Restricted to the same host. Won't be output by any interface.
    // 1    Restricted to the same subnet. Won't be forwarded by a router. This is the default.
    // <32    Restricted to the same site, organization or department.
    // <64    Restricted to the same region.
    // <128    Restricted to the same continent.
    // <255    Unrestricted in scope. Global.
    bool SetMulticastTtl(algo::Fildes sock, u8 ttl);

    // Join or leave multicast group (on the interface basis!)
    bool SetMulticastMembership(algo::Fildes sock, ietf::Ipv4 group, ietf::Ipv4 interface, bool membership);

    // Set multicast send interface (default specified by system administrator)
    bool SetMulticastInterface(algo::Fildes sock, ietf::Ipv4 interface);

    // Get local ip/port of SOCK into IPPORT -- getsockname(); zero for a
    // unix-domain socket.  Returns TRUE when the call succeeded.
    bool GetIpportLocal(algo::Fildes sock, ietf::Ipport &ipport);

    // Get remote ip/port of SOCK into IPPORT -- getpeername(); zero for a
    // unix-domain socket.  Returns TRUE when the call succeeded.
    bool GetIpportRemote(algo::Fildes sock, ietf::Ipport &ipport);

    // get list of interface names as space-separated string
    tempstr GetInterfaces(algo::Fildes sock);
    tempstr GetHostAddr(strptr hostname);

    // ioctl with ifreq
    bool Ioctl(algo::Fildes sock, strptr name, u32 request, ifreq &ifr);

    // get hardware address for intrface
    bool GetHwAddrFamily(algo::Fildes sock, strptr name, sa_family_t &result);

    // get ip address of interface
    bool GetIpv4(algo::Fildes sock, strptr name, ietf::Ipv4 &result);

    // get link MTU (bytes) of interface NAME; RESULT is 0 on failure
    bool GetMtu(algo::Fildes sock, strptr name, int &result);

    // find interface name for known ip
    tempstr FindInterfaceByIpv4(algo::Fildes sock, ietf::Ipv4 &ip);

    // Resolve ip:port or <dnsname>:<port> to an Ipport struct
    ietf::Ipport Resolve(algo::strptr addr);

    // Wrapper for sendto -- Ipport
    bool Sendto(algo::Fildes sock, u8 *buf, u32 len, ietf::Ipport ipport);

    // Send the same datagram BUF (LEN bytes) to N destinations DST using as few
    // syscalls as possible: sendmmsg() on Linux, one sendto() each elsewhere.  This
    // is the unicast replacement for a
    // multicast send: one logical broadcast becomes N datagrams but only
    // ceil(N/BATCH) syscalls instead of N.  Returns the number of datagrams the
    // kernel accepted (a short count means the send buffer filled; not retried).
    int SendtoMulti(algo::Fildes sock, u8 *buf, u32 len, ietf::Ipport *dst, int n);

    // -------------------------------------------------------------------
    // include/lib_netio.inl.h
    //
    inline bool MulticastQ(ietf::Ipv4 addr);

    // Wrapper for setsockopt
    template<typename T> bool SetSocketOption(algo::Fildes sock, int level, int option, const T &value);

    // TRUE when the gateway spec GW names a unix-domain socket file: a path, which
    // a host:port form never contains a slash to be mistaken for.
    inline bool GwPathQ(algo::strptr gw);
}
