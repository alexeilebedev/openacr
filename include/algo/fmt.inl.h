// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/fmt.inl.h
//

inline u32 algo::u16_FmtBuf(u32 value, u8 *buf) {
    return u32_FmtBufSmall(value,buf);
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const char* t) {
    strptr_Print (strptr(t),out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const ListSep &t) {
    ListSep_Print(t,out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const memptr &t) {
    memptr_Print (t,out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const strptr &t) {
    strptr_Print (t,out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, void (*t)(algo::cstring &)) {
    t(out);
    return out;
}
