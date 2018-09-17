// (C) AlgoEngineering LLC 2008-2012
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
// Header: include/sysincl.h -- System header includes
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once

#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <memory.h>
#include <locale.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <new>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#include <byteswap.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#include <netinet/in.h>

#ifdef AOS_SSE42
#include <xmmintrin.h>
#include <popcntintrin.h>
#include <nmmintrin.h>
#endif
