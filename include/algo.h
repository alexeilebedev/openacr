// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Header: include/algo.h -- Main header
//

#pragma once

#include "include/sysincl.h"
#include "include/typedef.h"
#include "include/u128.h"
#include "include/define.h"
#include "include/gen/_gen.h"
#include "include/gen/algo_gen.h"
#include "include/gen/algo_lib_gen.h"

#include "include/algo/lib.h"
#include "include/algo/string.h"
#include "include/algo/tstamp.h"
#include "include/algo/file.h"
#include "include/algo/arg.h"
#include "include/algo/prlog.h"
#include "include/algo/backtrace.h"

#include "include/algo/time.h"
#include "include/algo/fmt.h"
#include "include/gen/report_gen.h"
#include "include/gen/ietf_gen.h"
#include "algo/using.h"
#include "include/algo/win32.h" // windows compatibility api

// INLINE FILE SECTION

#include "include/algo/aryptr.inl.h"

#include "include/gen/_gen.inl.h"

#include "include/algo.inl.h"
#include "include/algo/time.inl.h"
#include "include/algo/string.inl.h"
#include "include/algo/fmt.inl.h"
#include "include/gen/report_gen.inl.h"
#include "include/gen/ietf_gen.inl.h"
#include "include/gen/algo_gen.inl.h"
#include "include/gen/algo_lib_gen.inl.h"

// presently cannot be avoided
#include "include/lib/lib_json.h"
