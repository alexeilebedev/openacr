// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Header: include/sysincl.h -- System header includes
//

#pragma once

// necessary defines before we include any MS headers
#ifdef WIN32
// thanks Microsoft, but we won't switch to your proprietary versions
// of standard C functions.
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1
// 0x0600 = vista
// required for CreateSymbolicLink function
// (otherwise, 0x0500 could be used)
#undef WINVER
#define WINVER 0x0600
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#undef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0600
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define STRICT
#endif

#ifdef WIN32
#include <windows.h>
// functions like _open, _close
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
// stat
#include <sys/stat.h>
// chdir?
#include <direct.h>
// getpid
#include <process.h>
// gethostname
#include <winsock2.h>
#endif

#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <memory.h>
#include <locale.h>
#include <string.h>

#ifdef __linux__
#include <malloc.h>
#endif

#include <ctype.h>
#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#endif
#include <time.h>
#include <new>
#include <errno.h>
#include <signal.h>

#if _linux__
#include <byteswap.h>
#endif

#ifdef __MACH__
#include <libkern/OSByteOrder.h>
#endif

#ifndef WIN32
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <netinet/in.h>
#endif

#ifdef __linux__
#include <sys/epoll.h>
#endif


#ifdef AOS_SSE42
#ifdef WIN32
#include <intrin.h>
#else
#include <xmmintrin.h>
#include <popcntintrin.h>
#include <nmmintrin.h>
#endif
#endif

#ifndef WIN32
#include <sys/wait.h>
#include <sys/mman.h>
#endif

#ifdef WIN32
#include <winnt.h> // temporary, for Interlocked*
#endif
