// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 Astra
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
// Source: cpp/lib/algo/prlog.cpp -- prlog macro
//
// -----------------------------------------------------------------------------

#include "include/algo.h"

// Default implementation of prlog handler
//
// Notes on WriteFile use:
// some tools set fd 0 to nonblocking mode,
// which in case of a terminal makes output non-blocking too (bug in gnome terminal?)
// in any case it causes EAGAIN during fast writes, so we use WriteFile to
// write all bytes out.
void algo::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) {
    try {
        algo::Fildes fildes = logcat == &algo_lib_logcat_stdout
            ? algo_lib::_db.fildes_stdout
            : algo_lib::_db.fildes_stderr;
        if (algo_lib::_db.show_tstamp || (logcat && !logcat->builtin)) {
            // rich and slow case
            algo::UnTime time = algo::CurrUnTime() - algo::UnDiffSecs(double(get_cycles() - tstamp) / get_cpu_hz());
            i32 pos(0);
            i32 start(0);
            while(pos<ch_N(str)) {
                if (str[pos++] == '\n') {
                    // get whole line including eol
                    strptr line = ch_GetRegion(str,start,pos-start);
                    start = pos; // advance to next line
                    if (algo_lib::_db.pending_eol) {
                        WriteFile(fildes,(u8*)line.elems,line.n_elems);
                        algo_lib::_db.pending_eol = false;
                    } else {
                        // format log line
                        // time logcat: text
                        tempstr out;
                        if (algo_lib::_db.show_tstamp) {
                            UnTime_PrintSpec(time,out,algo_lib::_db.tstamp_fmt);
                        }
                        if (logcat && !logcat->builtin) {
                            out << logcat->logcat <<": ";
                        }
                        out << line;
                        WriteFile(fildes,(u8*)out.ch_elems,out.ch_n);
                    }
                }
            }
            algo_lib::_db.pending_eol = start != pos;
        } else {
            // simple and fast case
            WriteFile(fildes, (u8*)str.elems, i32(str.n_elems));
        }
    } catch (...) {
        // coverity UNCAUGHT_EXCEPT --
        // do not allow this function to throw exception.
    }
}
