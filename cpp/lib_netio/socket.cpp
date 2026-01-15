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
// Source: cpp/lib_netio/socket.cpp
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

#include "include/algo.h"
#include "include/lib_netio.h"
#include "include/lib_netio.inl.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

//------------------------------------------------------------------------------

// Create TCP socket
algo::Fildes lib_netio::CreateTcpSocket() {
    return algo::Fildes(socket(AF_INET, SOCK_STREAM, 0));
}

//------------------------------------------------------------------------------

// Create UDP socket
algo::Fildes lib_netio::CreateUdpSocket() {
    return algo::Fildes(socket(AF_INET, SOCK_DGRAM, 0));
}

//------------------------------------------------------------------------------

// Create Netlink socket
algo::Fildes lib_netio::CreateNetlinkSocket() {
    return algo::Fildes(socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE));
}

//------------------------------------------------------------------------------

//  Wrapper for bind() -- Ipport
bool lib_netio::Bind(algo::Fildes sock, ietf::Ipport ipport) {
    sockaddr_in sa;
    algo::ZeroBytes(sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(ipport.ip.ipv4);
    sa.sin_port = htons(ipport.port);
    return bind(sock.value, (sockaddr *)&sa, sizeof sa) == 0;
}

//------------------------------------------------------------------------------

//  Wrapper for bind() -- strptr
bool lib_netio::Bind(algo::Fildes sock, strptr addr) {
    int ret(false);
    ietf::Ipport ipport;
    if (ietf::Ipport_ReadStrptrMaybe(ipport, addr)) {
        ret = Bind(sock, ipport);
    } else {
        errno = EINVAL;
    }
    return ret;
}

//------------------------------------------------------------------------------

//  Wrapper for bind to netlink
bool lib_netio::BindNetlink(algo::Fildes sock) {
    sockaddr_nl sa;
    algo::ZeroBytes(sa);
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK;
    return bind(sock.value, (sockaddr *)&sa, sizeof sa) == 0;
};

// send GETLINK netlink request
bool lib_netio::RequestLinkDump(algo::Fildes sock) {
    // address
    sockaddr_nl sa;
    algo::ZeroBytes(sa);
    sa.nl_family = AF_NETLINK;
    // data
    u8 buf[NLMSG_SPACE(sizeof(ifinfomsg))]; // with alignment padding
    algo::ZeroBytes(buf);
    struct Msg {
        nlmsghdr nlh;
        ifinfomsg ifm;
    } &req = *(Msg*)buf;
    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof req.ifm);
    req.nlh.nlmsg_type = RTM_GETLINK;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.ifm.ifi_family = AF_UNSPEC;
    return sendto(sock.value, buf, sizeof buf, 0, (sockaddr*)&sa, sizeof sa) == sizeof buf;
}

//------------------------------------------------------------------------------

//  Wrapper for listen() -- strptr
bool lib_netio::Listen(algo::Fildes sock, int backlog) {
    return listen(sock.value,backlog) == 0;
}

//------------------------------------------------------------------------------

// Wrapper for connect() -- ipport
bool lib_netio::Connect(algo::Fildes sock, ietf::Ipport ipport) {
    sockaddr_in sa;
    algo::ZeroBytes(sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(ipport.ip.ipv4);
    sa.sin_port = htons(ipport.port);
    return connect(sock.value, (sockaddr *)&sa, sizeof(sa)) == 0;
}

//------------------------------------------------------------------------------

// Wrapper for connect() -- strptr
int lib_netio::Connect(algo::Fildes sock, strptr addr) {
    int ret(false);
    ietf::Ipport ipport;
    if (ietf::Ipport_ReadStrptrMaybe(ipport, addr)) {
        ret = Connect(sock, ipport);
    } else {
        errno = EINVAL;
    }
    return ret;
}

//------------------------------------------------------------------------------

// accept remote connection:
// return connection socket and fills ipport with client address/port
algo::Fildes lib_netio::Accept(algo::Fildes listen_sock, ietf::Ipport &ipport) {
    algo::Fildes sock;
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof client_addr;
    algo::ZeroBytes(client_addr);
    sock.value = accept(listen_sock.value, (sockaddr *)&client_addr, &client_addr_len);
    ipport.ip.ipv4 = ntohl(client_addr.sin_addr.s_addr);
    ipport.port = ntohs(client_addr.sin_port);
    return sock;
}

//------------------------------------------------------------------------------

// Get socket error -- getsockopt(SO_ERROR);
// in case of getsockopt failure, return errno
int lib_netio::GetSocketError(algo::Fildes sock) {
    int err(0);
    socklen_t errlen = sizeof(err);
    if (getsockopt(sock.value, SOL_SOCKET, SO_ERROR, &err, &errlen) != 0) {
        err = errno;
    }
    return err;
}

//------------------------------------------------------------------------------

// Wrapper for setsockopt(SO_REUSEADDR)
bool lib_netio::SetReuseAddress(algo::Fildes sock, bool value DFLTVAL(true)) {
    return SetSocketOption<int>(sock, SOL_SOCKET, SO_REUSEADDR, value);
}

//------------------------------------------------------------------------------

// Wrapper for setsockopt(TCP_NODELAY)
bool lib_netio::SetTcpNoDelay(algo::Fildes sock, bool value DFLTVAL(true)) {
    return SetSocketOption<int>(sock, IPPROTO_TCP, TCP_NODELAY, value);
}

//------------------------------------------------------------------------------

// Wrapper for setsockopt(SO_LINGER) - timeout in seconds to drain output buffers before close().
// set 0 to drop connection with RST.
bool lib_netio::SetLinger(algo::Fildes sock, algo::UnixDiff timeout DFLTVAL(algo::UnixDiff())) {
    linger arg;
    algo::ZeroBytes(arg);
    arg.l_linger = timeout.value;
    arg.l_onoff = 1;
    return SetSocketOption<linger>(sock, SOL_SOCKET, SO_LINGER, arg) == 0;
}

//------------------------------------------------------------------------------

// Set TCP keepalive
bool lib_netio::SetTcpKeepalive(algo::Fildes sock, bool on DFLTVAL(true), algo::UnixDiff idle DFLTVAL(algo::UnixDiff(2)), algo::UnixDiff interval DFLTVAL(algo::UnixDiff(2)), int max_probes DFLTVAL(5)) {
    return SetSocketOption<int>(sock, SOL_SOCKET, SO_KEEPALIVE, on)
        && SetSocketOption<int>(sock, IPPROTO_TCP, TCP_KEEPIDLE, idle.value)
        && SetSocketOption<int>(sock, IPPROTO_TCP, TCP_KEEPINTVL, interval.value)
        && SetSocketOption<int>(sock, IPPROTO_TCP, TCP_KEEPCNT, max_probes);
}

//------------------------------------------------------------------------------

// Set send buffer size -- setsockopt(SO_SNDBUF)
bool lib_netio::SetSendBufferSize(algo::Fildes sock, int size) {
    return SetSocketOption<int>(sock, SOL_SOCKET, SO_SNDBUF, size);
}

//------------------------------------------------------------------------------

// Set receive buffer size -- setsockopt(SO_RCVBUF)
bool lib_netio::SetReceiveBufferSize(algo::Fildes sock, int size) {
    return SetSocketOption<int>(sock, SOL_SOCKET, SO_RCVBUF, size);
}

//------------------------------------------------------------------------------

// Set multicast loop (enabled by defult)
bool lib_netio::SetMulticastLoop(algo::Fildes sock, bool loop) {
    return SetSocketOption<u8>(sock, IPPROTO_IP, IP_MULTICAST_LOOP, loop);
}

//------------------------------------------------------------------------------

// Set multicast TTL
// TTL     Scope
//    0    Restricted to the same host. Won't be output by any interface.
//    1    Restricted to the same subnet. Won't be forwarded by a router. This is the default.
//  <32    Restricted to the same site, organization or department.
//  <64    Restricted to the same region.
// <128    Restricted to the same continent.
// <255    Unrestricted in scope. Global.
bool lib_netio::SetMulticastTtl(algo::Fildes sock, u8 ttl) {
    return SetSocketOption<u8>(sock, IPPROTO_IP, IP_MULTICAST_TTL, ttl);
}

//------------------------------------------------------------------------------

// Join or leave multicast group (on the interface basis!)
bool lib_netio::SetMulticastMembership(algo::Fildes sock, ietf::Ipv4 group, ietf::Ipv4 interface, bool membership) {
    ip_mreq mreq;
    algo::ZeroBytes(mreq);
    mreq.imr_multiaddr.s_addr = htonl(group.ipv4);
    mreq.imr_interface.s_addr = htonl(interface.ipv4);
    int option = membership ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP;
    return SetSocketOption<ip_mreq>(sock, IPPROTO_IP, option, mreq);
}

//------------------------------------------------------------------------------

// Set multicast send interface (default specified by system administrator)
bool lib_netio::SetMulticastInterface(algo::Fildes sock, ietf::Ipv4 interface) {
    in_addr addr;
    algo::ZeroBytes(addr);
    addr.s_addr = htonl(interface.ipv4);
    return SetSocketOption<in_addr>(sock, IPPROTO_IP, IP_MULTICAST_IF, addr);
}

//------------------------------------------------------------------------------

// Get local ip/port -- getsockname()
bool lib_netio::GetIpportLocal(algo::Fildes sock, ietf::Ipport &ipport) {
    sockaddr_in addr;
    algo::ZeroBytes(addr);
    socklen_t addr_len = sizeof addr;
    bool ok = getsockname(sock.value, (sockaddr *)&addr, &addr_len) == 0;
    ipport.ip.ipv4 = ntohl(addr.sin_addr.s_addr);
    ipport.port = ntohs(addr.sin_port);
    return ok;
}

//------------------------------------------------------------------------------

// Get remote ip/port -- getpeername()
bool lib_netio::GetIpportRemote(algo::Fildes sock, ietf::Ipport &ipport) {
    sockaddr_in addr;
    algo::ZeroBytes(addr);
    socklen_t addr_len = sizeof addr;
    bool ok = getpeername(sock.value, (sockaddr *)&addr, &addr_len) == 0;
    ipport.ip.ipv4 = ntohl(addr.sin_addr.s_addr);
    ipport.port = ntohs(addr.sin_port);
    return ok;
}

//------------------------------------------------------------------------------

// get list of interface names as space-separated string
tempstr lib_netio::GetInterfaces(algo::Fildes sock) {
    tempstr ret;
    algo::ListSep ls(" ");
    char buf[4096]; // FIXME
    ifconf ifc;
    ifc.ifc_len = sizeof buf;
    ifc.ifc_buf = buf;
    bool ok = ioctl(sock.value, SIOCGIFCONF, &ifc)==0;
    if (ok) {
        frep_(i, ifc.ifc_len / sizeof(ifreq)) {
            ret << ls << ifc.ifc_req[i].ifr_name;
        }
    }
    return ret;
}

// get list of ips for a give hostname as space-separated string
//------------------------------------------------------------------------------
tempstr lib_netio::GetHostAddr(strptr hostname) {
    tempstr ret;
    tempstr host(hostname);
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; // Force TCP protocol
    int ret_code=getaddrinfo(Zeroterm(host), nullptr, &hints, &res);
    if (!ret_code){
        algo::ListSep ls(" ");
        for (auto p = res; p; p = p->ai_next) {
            char ip[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &((sockaddr_in*)p->ai_addr)->sin_addr, ip, sizeof(ip))){
                ret << ls << ip;
            }
        }
        freeaddrinfo(res);
    } else {
        gai_strerror(ret_code);
    }
    return tempstr()<<Trimmed(ret);
}
//------------------------------------------------------------------------------

// ioctl with ifreq
bool lib_netio::Ioctl(algo::Fildes sock, strptr name, u32 request, ifreq &ifr) {
    memset(&ifr, 0, sizeof ifr);
    memcpy(ifr.ifr_name, name.elems, u32_Min(name.n_elems, IFNAMSIZ));
    return ioctl(sock.value, request, &ifr)==0;
}

// get hardware address for intrface
bool lib_netio::GetHwAddrFamily(algo::Fildes sock, strptr name, sa_family_t &result) {
    ifreq ifr;
    bool ok = Ioctl(sock, name, SIOCGIFHWADDR, ifr);
    result = ok ? ifr.ifr_hwaddr.sa_family : ARPHRD_VOID;
    return ok;
}

// get ip address of interface
bool lib_netio::GetIpv4(algo::Fildes sock, strptr name, ietf::Ipv4 &result) {
    ifreq ifr;
    bool ok = Ioctl(sock, name, SIOCGIFADDR, ifr);
    result = ietf::Ipv4(ntohl(ok ? ((sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr : 0));
    return ok;
}

// find interface name for known ip
tempstr lib_netio::FindInterfaceByIpv4(algo::Fildes sock, ietf::Ipv4 &ip) {
    tempstr name;
    cstring interfaces = lib_netio::GetInterfaces(sock);
    ind_beg(Word_curs,intf,interfaces) {
        ietf::Ipv4 intf_ip;
        if (GetIpv4(sock,intf,intf_ip) && intf_ip == ip) {
            name = intf;
            break;
        }
    }ind_end;
    return name;
}

// -----------------------------------------------------------------------------

// Resolve ip:port or <dnsname>:<port> to an Ipport struct
ietf::Ipport lib_netio::Resolve(algo::strptr addr) {
    tempstr host(lib_netio::GetHostAddr(Pathcomp(addr,":RL")));
    tempstr port(Pathcomp(addr,":RR"));
    ietf::Ipport ret;
    ietf::Ipport_ReadStrptrMaybe(ret,tempstr()<<host<<":"<<port);
    return ret;
}
