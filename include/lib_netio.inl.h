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

// TRUE when the gateway spec GW names a unix-domain socket file: a path, which
// a host:port form never contains a slash to be mistaken for.
inline bool lib_netio::GwPathQ(algo::strptr gw) {
    return algo::FindChar(gw, '/') != -1;
}
