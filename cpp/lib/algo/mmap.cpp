// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
bool algo_lib::MmapFile_LoadFd(MmapFile &mmapfile, algo::Fildes fd) {
    mmapfile.fd.fd = fd;
    i64 n = GetFileSize(mmapfile.fd.fd);
    // Linux: do not attempt to map zero bytes -- it will fail.
    void *addr = NULL;
    if (n > 0) {
        addr = mmap(NULL,n,PROT_READ,MAP_PRIVATE,mmapfile.fd.fd.value,0);
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
