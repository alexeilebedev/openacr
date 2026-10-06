// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/lib/algo/mmap.cpp -- Mmap wrapper
//

#include "include/algo.h"
#ifdef WIN32
#else
#include <sys/mman.h>// mmap,mlockall
#endif

// -----------------------------------------------------------------------------

// User-defined cleanup function for MMAP.MEM
void algo_lib::mem_Cleanup(algo_lib::Mmap &mmap) {
    if (mmap.mem.elems) {
        (void)munmap(mmap.mem.elems, elems_N(mmap.mem));
    }
}

// -----------------------------------------------------------------------------

// Attach mmapfile MMAPFILE to FD.
// Return success code.
//
// The mapping is MAP_SHARED, so it tracks the file and a write another handle
// makes shows through it.  MAP_PRIVATE would leave that unsaid: the standard
// does not decide whether a later write to the file reaches a private mapping,
// Linux lets it through and Darwin does not, so the same call would answer two
// different things.  Nothing is written through the mapping -- it is PROT_READ
// -- so sharing costs nothing and the behavior is the same everywhere.
bool algo_lib::MmapFile_LoadFd(MmapFile &mmapfile, algo::Fildes fd) {
    mmapfile.fd.fd = fd;
    i64 n = GetFileSize(mmapfile.fd.fd);
    // Linux: do not attempt to map zero bytes -- it will fail.
    void *addr = NULL;
    if (n > 0) {
        addr = mmap(NULL,n,PROT_READ,MAP_SHARED,mmapfile.fd.fd.value,0);
    }
    if (addr == (void*)-1) {
        addr = NULL;
    }
    if (addr != NULL) {        // success
        mmapfile.map.mem.n_elems = n;
        mmapfile.map.mem.elems   = (u8*)addr;
        mmapfile.text            = ToStrPtr(mmapfile.map.mem);
    }
    return (n == 0) || (addr != NULL);
}

// -----------------------------------------------------------------------------

// Attach mmapfile MMAPFILE to FNAME
// Return success code.
bool algo_lib::MmapFile_Load(MmapFile &mmapfile, strptr fname) {
    algo::Fildes fildes = OpenRead(fname,algo::FileFlags());
    return ValidQ(fildes) ? MmapFile_LoadFd(mmapfile,fildes) : false;
}
