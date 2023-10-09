// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 Astra
// Copyright (C) 2023 AlgoRND
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
// Header: include/algo/prlog.h
//

namespace algo { // update-hdr srcfile:%/algo/prlog.%
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/prlog.cpp -- prlog macro
    //

    // Default implementation of prlog handler
    //
    // Notes on WriteFile use:
    // some tools set fd 0 to nonblocking mode,
    // which in case of a terminal makes output non-blocking too (bug in gnome terminal?)
    // in any case it causes EAGAIN during fast writes, so we use WriteFile to
    // write all bytes out.
    void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str);
}
