// Copyright (C) 2024,2026 AlgoRND
//
// License: ARND
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.
//
// Target: lib_netio (lib) -- Network I/O library
// Exceptions: yes
// Header: include/lib_netio.inl.h
//
// Check whether IP address is multicast group

inline bool lib_netio::MulticastQ(ietf::Ipv4 addr) {
    return (addr.ipv4 & 0xf0000000) == 0xe0000000;
}

// Wrapper for setsockopt
template<typename T> bool lib_netio::SetSocketOption(algo::Fildes sock, int level, int option, const T &value) {
    return setsockopt(sock.value, level, option, &const_cast<T&>(value), sizeof value) == 0;
}
