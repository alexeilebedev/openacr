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
// Source: cpp/lib/algo/arg.cpp -- Parse command-line
//
// Retrieve gitinfo string, e.g.
// dev.gitinfo  gitinfo:2014-10-06.afa3edc.abt  gitref:afa3edc...  builddate:2014-10-06T09:14:02

#include "include/algo.h"

// The build identity is not a source fact -- it names the commit a binary came
// from and the moment it was compiled -- so abt writes it as build/gitinfo.h at
// the start of every build (cpp/abt/gitinfo.cpp).  A tree compiled without abt
// has no such file, which is the case the bootstrap and a hand-run compiler
// take, and the binary honestly reports itself unversioned.
#if defined(__has_include)
#  if __has_include("build/gitinfo.h")
#    include "build/gitinfo.h"
#  endif
#endif

#ifndef ALGO_GITINFO
#define ALGO_GITINFO "dev.gitinfo  comment:unversioned"
#endif

strptr algo::gitinfo_Get() {
    // remove trailing \n
    return Trimmed(ALGO_GITINFO);
}
