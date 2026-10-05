// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/malloc.cpp -- Malloc allocator
//

#include "include/amc.h"

void amc::tclass_Malloc() {
    GenTclass(amc_tclass_Pool);
}

void amc::tfunc_Malloc_AllocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& allocmem = amc::CreateCurFunc();
    Ins(&R, allocmem.comment, "Allocate n bytes. If no memory available, return NULL.");
    Ins(&R, allocmem.ret  , "void*", false);
    Ins(&R, allocmem.proto, "$name_AllocMem($Parent, size_t n)", false);
    Ins(&R, allocmem.body, "void* mem = malloc(n);");
    Ins(&R, allocmem.body, "++algo_lib::_db.trace.alloc__db_malloc; // update global malloc counter");
    Ins(&R, allocmem.body, "return mem;");
}

void amc::tfunc_Malloc_ReallocMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& reallocmem = amc::CreateCurFunc();
    Ins(&R, reallocmem.comment, "Reallocate n bytes. If the call fails, return value is NULL.");
    Ins(&R, reallocmem.comment, "In this case, original MEM pointer is untouched.");
    Ins(&R, reallocmem.ret  , "void*", false);
    Ins(&R, reallocmem.proto, "$name_ReallocMem($Parent, void *mem, size_t old_size, size_t new_size)", false);
    Ins(&R, reallocmem.body, "(void)old_size;");
    Ins(&R, reallocmem.body, "algo_lib::_db.trace.alloc__db_malloc += old_size == 0; // update global malloc counter");
    Ins(&R, reallocmem.body, "algo_lib::_db.trace.del__db_malloc += new_size == 0; // update global malloc counter");
    Ins(&R, reallocmem.body, "void* new_mem = realloc(mem, new_size);");
    Ins(&R, reallocmem.body, "return new_mem;");
}

void amc::tfunc_Malloc_FreeMem() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& free = amc::CreateCurFunc();
    free.inl = false;
    Ins(&R, free.comment, "Remove mem from all global and cross indices, then deallocate mem");
    Ins(&R, free.ret  , "void", false);
    Ins(&R, free.proto, "$name_FreeMem(void *mem, size_t n)", false);
    Ins(&R, free.body, "(void)n;");
    Ins(&R, free.body, "if (mem) {");
    Ins(&R, free.body, "    ++algo_lib::_db.trace.del__db_malloc; // update global malloc counter");
    Ins(&R, free.body, "    free(mem);");
    Ins(&R, free.body, "}");
}
