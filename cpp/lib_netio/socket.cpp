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
// Source: cpp/lib_netio/socket.cpp
//

#include "include/algo.h"
#include "include/lib_netio.h"
#include "include/lib_netio.inl.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

#ifdef __linux__
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif
#include <poll.h>
#include <sys/un.h>

//------------------------------------------------------------------------------

// Fill IPPORT from the socket address SA of ADDR_LEN bytes: the ip and port
// when SA is an IPv4 address, zero otherwise.  A unix-domain socket has a
// path for an address and no ip, and reading its bytes as an IPv4 address
// would report a garbage peer, so the family decides.
static void IpportFromSockaddr(sockaddr_storage &sa, ietf::Ipport &ipport) {
    ipport = ietf::Ipport();
    if (sa.ss_family == AF_INET) {
        sockaddr_in &sin = *(sockaddr_in*)&sa;
        ipport.ip.ipv4 = ntohl(sin.sin_addr.s_addr);
        ipport.port = ntohs(sin.sin_port);
    }
}

// Fill SA with the unix-domain address of socket file PATH, and return the
// length to pass to bind or connect.  Zero when PATH does not fit the address,
// with errno ENAMETOOLONG, so the caller reports it the way it reports a bind
// that failed.
static socklen_t SockaddrUn(sockaddr_un &sa, algo::strptr path) {
    socklen_t ret = 0;
    algo::ZeroBytes(sa);
    if (path.n_elems < (int)sizeof(sa.sun_path)) {
        sa.sun_family = AF_UNIX;
        memcpy(sa.sun_path, path.elems, path.n_elems);
        ret = socklen_t(offsetof(sockaddr_un, sun_path) + path.n_elems + 1);
    } else {
        errno = ENAMETOOLONG;
    }
    return ret;
}

//------------------------------------------------------------------------------

// True when PATH fits a unix-domain socket address, so a bind or connect on it
// can reach the kernel at all.  The address holds a fixed-size path with room
// for a terminator, and a longer PATH is refused by SockaddrUn with
// ENAMETOOLONG however many times it is tried.  A caller that retries a failed
// bind asks this first, so it reports the path once and stops rather than
// retrying a length that cannot change.
bool lib_netio::SockPathFitsQ(algo::strptr path) {
    sockaddr_un sa;
    return path.n_elems < (int)sizeof(sa.sun_path);
}

//------------------------------------------------------------------------------

// Return FD with close-on-exec set, so it does not survive an exec.  The flag
// is set after the descriptor exists, which is portable: Darwin has neither
// SOCK_CLOEXEC nor accept4.  A fork between the two calls would leak the
// descriptor, and none can happen, since these programs are single-threaded.
static algo::Fildes SetCloexec(algo::Fildes fd) {
    if (fd.value >= 0) {
        (void)fcntl(fd.value, F_SETFD, FD_CLOEXEC);
    }
    return fd;
}

//------------------------------------------------------------------------------

// Create TCP socket, close-on-exec.
// A descriptor that survives exec keeps what it holds for the whole life of
// the exec'd program, and that program knows nothing about it: a listening
// socket carried into an unrelated command keeps its port bound long after
// the process that opened the port is gone, so the port reads as in use with
// no owner that explains it.  A child that is meant to receive a socket gets
// it as one of its standard streams, which the exec preserves by itself.
algo::Fildes lib_netio::CreateTcpSocket() {
    return SetCloexec(algo::Fildes(socket(AF_INET, SOCK_STREAM, 0)));
}

//------------------------------------------------------------------------------

// Create UDP socket, close-on-exec (see CreateTcpSocket)
algo::Fildes lib_netio::CreateUdpSocket() {
    return SetCloexec(algo::Fildes(socket(AF_INET, SOCK_DGRAM, 0)));
}

//------------------------------------------------------------------------------

// Create a unix-domain stream socket, close-on-exec (see CreateTcpSocket).
// It is addressed by a path on the host (BindUnix, ConnectUnix) and carries
// the same byte stream a TCP socket does.
algo::Fildes lib_netio::CreateUnixSocket() {
    return SetCloexec(algo::Fildes(socket(AF_UNIX, SOCK_STREAM, 0)));
}

//------------------------------------------------------------------------------

// Create Netlink socket, close-on-exec (see CreateTcpSocket).
// Netlink exists on Linux only; elsewhere the socket is invalid and errno is ENOTSUP.
algo::Fildes lib_netio::CreateNetlinkSocket() {
#ifdef __linux__
    return SetCloexec(algo::Fildes(socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)));
#else
    errno = ENOTSUP;
    return algo::Fildes(-1);
#endif
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

// Bind unix-domain socket SOCK to the socket file PATH, and return TRUE when
// it is bound.  A socket file outlives the process that bound it, so the one a
// predecessor left is removed first; the bind then creates the file anew.
// Failure leaves errno set, ENAMETOOLONG when PATH does not fit a socket
// address.
bool lib_netio::BindUnix(algo::Fildes sock, strptr path) {
    sockaddr_un sa;
    socklen_t len = SockaddrUn(sa, path);
    bool ret = false;
    if (len > 0) {
        (void)unlink(sa.sun_path);
        ret = bind(sock.value, (sockaddr *)&sa, len) == 0;
    }
    return ret;
}

//------------------------------------------------------------------------------

//  Wrapper for bind to netlink
bool lib_netio::BindNetlink(algo::Fildes sock) {
#ifdef __linux__
    sockaddr_nl sa;
    algo::ZeroBytes(sa);
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK;
    return bind(sock.value, (sockaddr *)&sa, sizeof sa) == 0;
#else
    (void)sock;
    errno = ENOTSUP;
    return false;
#endif
};

// send GETLINK netlink request
bool lib_netio::RequestLinkDump(algo::Fildes sock) {
#ifdef __linux__
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
#else
    (void)sock;
    errno = ENOTSUP;
    return false;
#endif
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

// Connect unix-domain socket SOCK to the socket file PATH, and return TRUE
// when the connect completed.  On a non-blocking socket a connect to a
// listening socket file completes at once; failure leaves errno set, ENOENT
// when nothing has bound PATH.
bool lib_netio::ConnectUnix(algo::Fildes sock, strptr path) {
    sockaddr_un sa;
    socklen_t len = SockaddrUn(sa, path);
    bool ret = false;
    if (len > 0) {
        ret = connect(sock.value, (sockaddr *)&sa, len) == 0;
    }
    return ret;
}

//------------------------------------------------------------------------------

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
bool lib_netio::TcpUpQ(ietf::Ipport ipport, int msec) {
    bool ret = false;
    algo::Fildes sock = CreateTcpSocket();
    if (algo::ValidQ(sock)) {
        algo::SetBlockingMode(sock, false);
        ret = Connect(sock, ipport);
        if (!ret && errno == EINPROGRESS) {
            struct pollfd pfd;
            pfd.fd = sock.value;
            pfd.events = POLLOUT;
            pfd.revents = 0;
            ret = poll(&pfd, 1, msec) == 1 && GetSocketError(sock) == 0;
        }
        (void)close(sock.value);
    }
    return ret;
}

//------------------------------------------------------------------------------

// accept remote connection on LISTEN_SOCK:
// return connection socket and fill IPPORT with client address/port, zero
// for a client on a unix-domain socket, which has no ip.
// The accepted socket is close-on-exec for the reason the listening socket is
// (see CreateTcpSocket); accept does not inherit the flag, so it is set again.
algo::Fildes lib_netio::Accept(algo::Fildes listen_sock, ietf::Ipport &ipport) {
    algo::Fildes sock;
    sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof client_addr;
    algo::ZeroBytes(client_addr);
    sock = SetCloexec(algo::Fildes(accept(listen_sock.value, (sockaddr *)&client_addr, &client_addr_len)));
    IpportFromSockaddr(client_addr, ipport);
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
#ifdef __APPLE__
        && SetSocketOption<int>(sock, IPPROTO_TCP, TCP_KEEPALIVE, idle.value)
#else
        && SetSocketOption<int>(sock, IPPROTO_TCP, TCP_KEEPIDLE, idle.value)
#endif
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

// Get local ip/port of SOCK into IPPORT -- getsockname(); zero for a
// unix-domain socket.  Returns TRUE when the call succeeded.
bool lib_netio::GetIpportLocal(algo::Fildes sock, ietf::Ipport &ipport) {
    sockaddr_storage addr;
    algo::ZeroBytes(addr);
    socklen_t addr_len = sizeof addr;
    bool ok = getsockname(sock.value, (sockaddr *)&addr, &addr_len) == 0;
    IpportFromSockaddr(addr, ipport);
    return ok;
}

//------------------------------------------------------------------------------

// Get remote ip/port of SOCK into IPPORT -- getpeername(); zero for a
// unix-domain socket.  Returns TRUE when the call succeeded.
bool lib_netio::GetIpportRemote(algo::Fildes sock, ietf::Ipport &ipport) {
    sockaddr_storage addr;
    algo::ZeroBytes(addr);
    socklen_t addr_len = sizeof addr;
    bool ok = getpeername(sock.value, (sockaddr *)&addr, &addr_len) == 0;
    IpportFromSockaddr(addr, ipport);
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
#ifdef __linux__
    ifreq ifr;
    bool ok = Ioctl(sock, name, SIOCGIFHWADDR, ifr);
    result = ok ? ifr.ifr_hwaddr.sa_family : ARPHRD_VOID;
    return ok;
#else
    (void)sock;
    (void)name;
    result = AF_UNSPEC;
    errno = ENOTSUP;
    return false;
#endif
}

// get ip address of interface
bool lib_netio::GetIpv4(algo::Fildes sock, strptr name, ietf::Ipv4 &result) {
    ifreq ifr;
    bool ok = Ioctl(sock, name, SIOCGIFADDR, ifr);
    result = ietf::Ipv4(ntohl(ok ? ((sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr : 0));
    return ok;
}

// get link MTU (bytes) of interface NAME; RESULT is 0 on failure
bool lib_netio::GetMtu(algo::Fildes sock, strptr name, int &result) {
    ifreq ifr;
    bool ok = Ioctl(sock, name, SIOCGIFMTU, ifr);
    result = ok ? ifr.ifr_mtu : 0;
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
    //prlog("resolve "<<addr<<" -> "<<ret);
    return ret;
}

//  Wrapper for sendto -- Ipport
bool lib_netio::Sendto(algo::Fildes sock, u8 *buf, u32 len, ietf::Ipport ipport) {
    sockaddr_in sa;
    algo::ZeroBytes(sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(ipport.ip.ipv4);
    sa.sin_port = htons(ipport.port);
    // sendto returns the number of bytes queued on success (== len for a
    // datagram), or -1 on error; it is never 0 for a non-empty payload.
    return sendto(sock.value, buf, len, 0, (sockaddr *)&sa, sizeof sa) == (ssize_t)len;
}

//------------------------------------------------------------------------------

// Send the same datagram BUF (LEN bytes) to N destinations DST using as few
// syscalls as possible: sendmmsg() on Linux, one sendto() each elsewhere.  This
// is the unicast replacement for a
// multicast send: one logical broadcast becomes N datagrams but only
// ceil(N/BATCH) syscalls instead of N.  Returns the number of datagrams the
// kernel accepted (a short count means the send buffer filled; not retried).
int lib_netio::SendtoMulti(algo::Fildes sock, u8 *buf, u32 len, ietf::Ipport *dst, int n) {
    int sent = 0;
#ifdef __linux__
    enum { BATCH = 64 };
    mmsghdr     msgs[BATCH];
    iovec       iov[BATCH];
    sockaddr_in sa[BATCH];
    int off = 0;
    bool stop = false;
    while (off < n && !stop) {
        int cnt = i32_Min((int)BATCH, n - off);
        for (int i = 0; i < cnt; i++) {
            algo::ZeroBytes(sa[i]);
            sa[i].sin_family = AF_INET;
            sa[i].sin_addr.s_addr = htonl(dst[off+i].ip.ipv4);
            sa[i].sin_port = htons(dst[off+i].port);
            iov[i].iov_base = buf;
            iov[i].iov_len = len;
            algo::ZeroBytes(msgs[i]);
            msgs[i].msg_hdr.msg_name = &sa[i];
            msgs[i].msg_hdr.msg_namelen = sizeof(sa[i]);
            msgs[i].msg_hdr.msg_iov = &iov[i];
            msgs[i].msg_hdr.msg_iovlen = 1;
        }
        int rc = sendmmsg(sock.value, msgs, cnt, 0);
        if (rc < 0) {
            stop = true;
        } else {
            sent += rc;
            off += cnt;
            if (rc < cnt) {
                stop = true;
            }
        }
    }
#else
    // sendmmsg is Linux's, so elsewhere each destination costs one sendto.
    bool stop = false;
    for (int i = 0; i < n && !stop; i++) {
        sockaddr_in sa;
        algo::ZeroBytes(sa);
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(dst[i].ip.ipv4);
        sa.sin_port = htons(dst[i].port);
        stop = sendto(sock.value, buf, len, 0, (sockaddr *)&sa, sizeof sa) != (ssize_t)len;
        sent += !stop;
    }
#endif
    return sent;
}
