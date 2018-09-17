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
// Source: cpp/lib/algo/cpu_hz.cpp -- Grab cpu_hz from OS
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//
// -----------------------------------------------------------------------------

static double GetCpuHzSysdev(void) {
    i32 freq_khz = 0;
    tempstr value = FileToString("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", FileFlags());
    StringIter iter(value);
    (void)TryParseI32(iter, freq_khz);
    return double(freq_khz)*1e3;
}

// -----------------------------------------------------------------------------

static double GetCpuHzCpuinfo(strptr cpuinfo) {
    // look in /proc/cpuinfo
    StringIter iter(cpuinfo);
    double mhz = 0;
    while (true) {
        // on some linux systems, the spelling is MHz, others it's Mhz
        strptr text = "cpu Mhz";
        int idx = FindFrom(iter.expr, text, iter.index, false /*case-insensitive */);
        if (idx !=-1) {
            iter.index = idx + elems_N(text);
        }
        iter.Ws();
        if (idx != -1 && SkipChar(iter, ':') && TryParseDouble(iter, mhz)) {
            break;
        }
    }
    return mhz * 1e6;
}

// -----------------------------------------------------------------------------

static void CheckConstantTsc(strptr cpuinfo) {
    bool constant_tsc= true;
    ind_beg(Line_curs,line,cpuinfo) {
        if (StartsWithQ(line,"flags")) {
            if (0 > FindStr(line,"constant_tsc",true)) {
                constant_tsc=false;
                break;
            }
        }
    }ind_end;
    // without constant TSC, the tsc clock is worthless.
    if (!constant_tsc) {
        FatalErrorExit("CPU does not support constant_tsc.");
    }
}

// -----------------------------------------------------------------------------

static double ComputeCpuHz() {
    tempstr cpuinfo(FileToString("/proc/cpuinfo", FileFlags()));
    double hz = GetCpuHzSysdev();
    if (hz == 0) {
        hz = GetCpuHzCpuinfo(cpuinfo);
    }
    CheckConstantTsc(cpuinfo);
    if (!(hz>10000000 && hz<10000000000ULL)) {
        FatalErrorExit(Zeroterm(tempstr()<<"Unlikely value for cpu_hz: "<<hz));
    }
    return hz;
}

// -----------------------------------------------------------------------------

u64 algo::get_cpu_hz_int(void) {
    if (UNLIKELY(!algo_lib::_db.cpu_hz)) {// cache the result
        algo_lib::_db.cpu_hz = ComputeCpuHz();
    }
    return algo_lib::_db.cpu_hz;
}

// -----------------------------------------------------------------------------

double algo::get_cpu_hz(void) {
    return (double)get_cpu_hz_int();
}
