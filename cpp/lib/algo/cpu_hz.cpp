// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/lib/algo/cpu_hz.cpp -- Obtain cpu_hz from a source that states it, never by measuring
//

#include "include/algo.h"

#if defined(__MACH__) || __FreeBSD__>0
#include <sys/sysctl.h>
#endif

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

// The P-state boost ceiling from cpufreq's cpuinfo_max_freq, in Hz
// (0 when the file is absent, e.g. on aws guests)
static double GetCpuHzSysdev(void) {
    i32 freq_khz = 0;
    tempstr value = algo::FileToString("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", algo::FileFlags());
    algo::StringIter iter(value);
    (void)TryParseI32(iter, freq_khz);
    return double(freq_khz)*1e3;
}

// -----------------------------------------------------------------------------

// The kernel's calibrated TSC rate in Hz, or 0 when the kernel does not
// export it.  /sys/devices/system/cpu/cpu0/tsc_freq_khz states the exact
// figure the kernel calibrated at boot; kernels carrying the tsc_freq_khz
// patch (amazon linux) or the tsc_freq_khz module provide it, mainline
// kernels do not.  This file is the first of the three statements of the
// counter's rate a process that schedules on the counter will accept, the
// others being a host file and an environment variable the caller names --
// either way the rate is read, never measured.
static double GetCpuHzKernel(void) {
    i32 freq_khz = 0;
    tempstr value = algo::FileToString("/sys/devices/system/cpu/cpu0/tsc_freq_khz", algo::FileFlags());
    algo::StringIter iter(value);
    (void)TryParseI32(iter, freq_khz);
    return double(freq_khz)*1e3;
}

// -----------------------------------------------------------------------------

// The counter's rate in Hz as stated by environment variable VARNAME, or 0
// when it is unset or holds no number.  The variable carries the rate in kHz,
// the unit and the figure the kernel's export carries.  It is how the rate
// reaches a process whose filesystem is not the host's -- a container takes it
// as an ordinary environment entry at start -- and how a single run is told a
// rate for a test.
static double GetCpuHzEnv(strptr varname) {
    i32 freq_khz = 0;
    tempstr value(algo::strptr(getenv(Zeroterm(tempstr()<<varname))));
    algo::StringIter iter(value);
    (void)TryParseI32(iter, freq_khz);
    return double(freq_khz)*1e3;
}

// The counter's rate in Hz as stated by host file PATH, or 0 when it states
// none.  The file holds the rate in kHz, and every process reads it the same
// way whatever started it -- a login shell, a service, a job runner, a cron
// entry -- so the rate does not depend on an environment being inherited.
static double GetCpuHzHost(strptr path) {
    i32 freq_khz = 0;
    tempstr value = algo::FileToString(path, algo::FileFlags());
    algo::StringIter iter(value);
    (void)TryParseI32(iter, freq_khz);
    return double(freq_khz)*1e3;
}

// -----------------------------------------------------------------------------

// Install HZ as the process's cycles<->seconds calibration: refuse an
// implausible value, set the conversion constants, and re-anchor the
// scheduler clock so elapsed time counts from the calibration point.
void algo_lib::ApplyCpuHz(double hz) {
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
    // linux: refuse to run without a fixed-rate counter, then take the
    // kernel's calibrated rate when the kernel exports it.  When it does
    // not, settle for the P-state figures -- the boost ceiling, else cpu0's
    // reported frequency.  Both describe core P-states rather than the
    // counter, which on constant_tsc hardware ticks at a fixed nominal rate
    // regardless of P-state: an EPYC 7702P reports a 2.18GHz boost ceiling
    // while its counter ticks at 2.0GHz, and /proc/cpuinfo's "cpu MHz" is
    // the load-varying effective frequency, up to 1.5x below nominal on an
    // idle host.  That is tool-grade accuracy, adequate for printing
    // elapsed times and for nothing else.  A process that schedules on the
    // counter demands the kernel figure instead (RequireKernelCpuHz) and
    // does not start on a machine that exports none.
    tempstr cpuinfo(FileToString("/proc/cpuinfo", algo::FileFlags()));
    CheckConstantTsc(cpuinfo);
    hz = GetCpuHzKernel();
    if (hz == 0) {
        hz = GetCpuHzSysdev();
    }
    if (hz == 0) {
        hz = GetCpuHzCpuinfo(cpuinfo);
    }
#endif
    ApplyCpuHz(hz);
}

// -----------------------------------------------------------------------------

// Demand the kernel's calibrated TSC rate, for a process that schedules on
// the counter: without the export nothing states the rate,
// so the process exits rather than starting on a figure that is merely
// plausible.  A plain tool never calls this and lives with the P-state
// figure InitCpuHz settled for.
//
// The alternative would be to time the counter against a wall clock at
// startup, and that measurement cannot be made trustworthy.  Its window is
// a few tens of milliseconds of ordinary scheduling, and a stall inside the
// window adds cycles the wall clock never saw: a 122ms stall on an EPYC
// 9R14 yields 1.8e10 Hz for a counter that ticks at 2.6GHz.  Repeating the
// window and keeping the best trial does not rescue it, because the stall
// is invisible to every criterion the trials can be compared on.  Worse
// than the absurd value is the plausible one -- a stall of a few tens of
// milliseconds produces a rate that passes every range test and then
// mis-scales every cycles<->seconds conversion in the process for the rest
// of its life.
//
// So the rate is read and never measured, which is why a rate nobody states
// is fatal here rather than a fallback.  InitCpuHz has already installed the
// kernel's figure where the export exists; what this demands is that the
// figure came from a source that states the counter's rate rather than from a
// P-state file.
//
// Two further sources exist because some kernels cannot carry the export at
// all.  A WSL2 guest is the case in hand: its kernel calibrates the counter
// exactly, having been told the rate by the hypervisor, but ships no header
// package to build the tsc_freq_khz module against and no /lib/modules to
// install it into, and it publishes the figure through no other interface --
// not cpufreq, not CPUID, not the MSR device.  The rate on such a host is
// known and merely unreadable, so the host states it directly, in the kHz the
// export would have carried.  Either way the figure is read rather than
// measured, so the rule above holds; and both are consulted only when the
// export is absent, which leaves a machine that has the export unable to be
// retuned by a stray statement.
//
// The two differ in how the statement reaches a process, not in what it means.
// A file on the host reaches every process running there whatever started it,
// because nothing has to inherit an environment for a file to be readable --
// which is why a shell profile does not serve: it reaches a login shell and
// misses a service, a job runner and a cron entry.  The variable covers the
// case where the filesystem a process sees is not the host's: a container is
// handed the rate as an ordinary environment entry when it starts.  The
// variable wins where both are present, being the narrower statement of the
// two.
//
// Both are named by the caller, because which file and which variable a
// deployment states its rate through is that deployment's convention and not
// this library's.
//
// Taking the rate from either is reported as a verbose line rather than as
// plain output.  Every process of a cluster reads the rate, so on such a host
// the plain form would announce it once per process, and a comptest compares a
// captured stdout byte for byte -- the announcement would fail every test that
// starts a cluster, on exactly the hosts these sources exist to serve.
// `atf_tsc` reports the calibration sources unconditionally, which is where an
// operator confirms which one a process would take.
void algo_lib::RequireKernelCpuHz(strptr tscfreq_path, strptr tscfreq_env) {
    if (GetCpuHzKernel() == 0) {
        double env_hz = GetCpuHzEnv(tscfreq_env);
        double host_hz = GetCpuHzHost(tscfreq_path);
        double hz = env_hz != 0 ? env_hz : host_hz;
        strptr source = env_hz != 0 ? tscfreq_env : tscfreq_path;
        if (hz == 0) {
            FatalErrorExit(Zeroterm(tempstr()<<"algo_lib.no_tsc_freq"
                                    <<Keyval("path","/sys/devices/system/cpu/cpu0/tsc_freq_khz")
                                    <<Keyval("remedy",tempstr()<<"load the tsc_freq_khz module: bin/install-tsc-freq-khz, or state the rate in kHz in "<<tscfreq_path)));
        } else {
            ApplyCpuHz(hz);
            verblog("algo_lib.tsc_freq_stated"
                    <<Keyval("source",source)
                    <<Keyval("hz",u64(hz))
                    <<Keyval("comment","kernel exports no tsc rate; this source states it"));
        }
    }
}
