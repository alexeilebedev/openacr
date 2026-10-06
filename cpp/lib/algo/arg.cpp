// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
