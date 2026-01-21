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
// Header: include/lib_netio.inl.h
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.
// Check whether IP address is multicast group

inline bool lib_netio::MulticastQ(ietf::Ipv4 addr) {
    return (addr.ipv4 & 0xf0000000) == 0xe0000000;
}

// Wrapper for setsockopt
template<typename T> bool lib_netio::SetSocketOption(algo::Fildes sock, int level, int option, const T &value) {
    return setsockopt(sock.value, level, option, &const_cast<T&>(value), sizeof value) == 0;
}
