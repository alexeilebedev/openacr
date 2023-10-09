// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/lib/algo/cpu_hz.cpp -- Grab cpu_hz from OS
//

#include "include/algo.h"

#if defined(__MACH__) || __FreeBSD__>0
#include <sys/sysctl.h>
#endif

// -----------------------------------------------------------------------------

static double GetCpuHzSysdev(void) {
    i32 freq_khz = 0;
    tempstr value = algo::FileToString("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", algo::FileFlags());
    algo::StringIter iter(value);
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
    (void)&CheckConstantTsc;
    (void)&GetCpuHzCpuinfo;
    (void)&GetCpuHzSysdev;
    double hz = 0;
#if __FreeBSD__>0
    uint64_t freq = 0;
    size_t size = sizeof(freq);
    if (sysctlbyname("machdep.tsc_freq", &freq, &size, NULL, 0) == 0) {
        hz = freq;
    }
#elif defined(__MACH__)
    uint64_t freq = 0;
    size_t size = sizeof(freq);
    if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) == 0) {
        hz = freq;
    }
#elif defined(__CYGWIN__)
    // sampling /proc/cpuinfo on a windows machine under cygwin
    // can take a unnaturally long time, such as 13 seconds.
    // stash a copy of cpuinfo, which cannot easily change, in /etc
    if (!algo::FileQ("/etc/cpuinfo")) {
        algo::StringToFile(algo::FileToString("/proc/cpuinfo", algo::FileFlags()), "/etc/cpuinfo");
    }
    hz = GetCpuHzCpuinfo(algo::FileToString("/etc/cpuinfo", algo::FileFlags()));
#elif defined(WIN32)
    HKEY hkey=NULL;
    if (RegOpenKey(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",&hkey) == ERROR_SUCCESS) {
        DWORD buf;
        ULONG type;
        DWORD size=sizeof(buf);
        if (RegQueryValueEx(hkey,"~MHz",0,&type,(u8*)&buf,&size) == ERROR_SUCCESS) {
            hz = buf*u64(1000000);
        }
        RegCloseKey(hkey);
    }
#else
    tempstr cpuinfo(FileToString("/proc/cpuinfo", algo::FileFlags()));
    hz = GetCpuHzSysdev();
    if (hz == 0) {
        hz = GetCpuHzCpuinfo(cpuinfo);
    }
#endif
    if (!(hz>10000000 && hz<10000000000ULL)) {
        FatalErrorExit(Zeroterm(tempstr()<<"algo_lib.bad_hz"
                                <<Keyval("hz",hz)));
    }
    algo_lib::_db.hz = hz;//double
    algo_lib::_db.cpu_hz = hz;//int
    algo_lib::_db.clocks_to_ms = 1000.0 / algo_lib::_db.hz;
    algo_lib::_db.clocks_to_ns = 1000000000.0 / algo_lib::_db.hz;
    algo_lib::_db.clock.value  = algo::get_cycles();
    algo_lib::_db.start_clock  = algo_lib::_db.clock;
}
