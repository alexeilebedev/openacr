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

#if defined(__MACH__) || __FreeBSD__>0
#include <sys/sysctl.h>
#endif

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
    double mhz = 0;
    ind_beg(Line_curs,line,cpuinfo) {
        if (StartsWithQ(line, "cpu MHz\t", false)) {
            algo::StringIter iter(Trimmed(Pathcomp(line,":LR")));
            if (algo::TryParseDouble(iter,mhz)) {
                break;
            }
        }
    }ind_end;
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

void algo_lib::InitCpuHz() {
    double hz = 0;
#if __FreeBSD__>0
    uint64_t freq = 0;
    size_t size = sizeof(freq);
    if (sysctlbyname("machdep.tsc_freq", &freq, &size, NULL, 0) == 0) {
        hz = freq;
    }
    (void)CheckConstantTsc;
#elif defined(__MACH__)
    uint64_t freq = 0;
    size_t size = sizeof(freq);
    if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) == 0) {
        hz = freq;
    }
    (void)CheckConstantTsc;
#else
    tempstr cpuinfo(FileToString("/proc/cpuinfo", FileFlags()));
    hz = GetCpuHzSysdev();
    if (hz == 0) {
        hz = GetCpuHzCpuinfo(cpuinfo);
    }
    CheckConstantTsc(cpuinfo);
    if (!(hz>10000000 && hz<10000000000ULL)) {
        FatalErrorExit(Zeroterm(tempstr()<<"algo_lib.bad_hz"
                                <<Keyval("hz",hz)));
    }
#endif
    algo_lib::_db.hz = hz;//double
    algo_lib::_db.cpu_hz = hz;//int
    algo_lib::_db.clocks_to_ms = 1000.0 / algo_lib::_db.hz;
    algo_lib::_db.clocks_to_ns = 1000000000.0 / algo_lib::_db.hz;
    algo_lib::_db.clock.value  = get_cycles();
    algo_lib::_db.start_clock  = algo_lib::_db.clock;
}
