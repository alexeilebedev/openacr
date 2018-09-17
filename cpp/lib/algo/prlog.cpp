// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/prlog.cpp -- prlog macro
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//
// -----------------------------------------------------------------------------

void algo::Prlog(int fd, cstring &str, int start, bool eol) {
    try {
        if (eol) {
            str << '\n';
        }
        // some tools set fd 0 to nonblocking mode,
        // which in case of a terminal makes output non-blocking too (bug in gnome terminal?)
        // in any case it causes EAGAIN during fast writes, so we use WriteFile to
        // write all bytes out.
        WriteFile(algo::Fildes(fd), (u8*)str.ch_elems + start, i32(str.ch_n - start));
        str.ch_n = start;
    } catch (...) {
        // coverity UNCAUGHT_EXCEPT --
        // do not allow this function to throw exception.
    }
}
