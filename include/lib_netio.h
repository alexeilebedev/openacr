// Copyright (C) 2024,2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: lib_netio (lib) -- Network I/O library
// Exceptions: yes
// Header: include/lib_netio.h
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

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

    // Create TCP socket
    algo::Fildes CreateTcpSocket();

    // Create UDP socket
    algo::Fildes CreateUdpSocket();

    // Create Netlink socket
    algo::Fildes CreateNetlinkSocket();

    // Wrapper for bind() -- Ipport
    bool Bind(algo::Fildes sock, ietf::Ipport ipport);

    // Wrapper for bind() -- strptr
    bool Bind(algo::Fildes sock, strptr addr);

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

    // accept remote connection:
    // return connection socket and fills ipport with client address/port
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

    // Get local ip/port -- getsockname()
    bool GetIpportLocal(algo::Fildes sock, ietf::Ipport &ipport);

    // Get remote ip/port -- getpeername()
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

    // find interface name for known ip
    tempstr FindInterfaceByIpv4(algo::Fildes sock, ietf::Ipv4 &ip);

    // Resolve ip:port or <dnsname>:<port> to an Ipport struct
    ietf::Ipport Resolve(algo::strptr addr);

    // -------------------------------------------------------------------
    // include/lib_netio.inl.h
    //
    inline bool MulticastQ(ietf::Ipv4 addr);

    // Wrapper for setsockopt
    template<typename T> bool SetSocketOption(algo::Fildes sock, int level, int option, const T &value);
}
