// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/varlen.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

void atf_amc::amctest_VarlenExternLength() {
    // Allocate
    u32 ary[]={1,2,3,4,5,6,7,8,9,10};
    aryptr<u32> ary1=aryptr<u32>(ary,10);
    atf_amc::VarlenExtern &vl = atf_amc::varlen_extern_AllocVarlen(ary1);
    aryptr<u32> ary2=varlen_Getary(vl);
    vrfyeq_(elems_N(ary1),elems_N(ary2)); // test code goes here
    for (int i=0; i<ary1.n_elems; i++) {
        vrfyeq_(ary1[i], ary2[i]);
    }
    varlen_extern_Delete(vl);// get rid of it
}

void atf_amc::length_Set(atf_amc::VarlenExtern &vl, u32 n) {
    vl.n_elems = (n-sizeof(atf_amc::VarlenExtern &))/sizeof(u32);
}

u32 atf_amc::length_Get(atf_amc::VarlenExtern &vl) {
    return sizeof(atf_amc::VarlenExtern)+sizeof(u32)*vl.n_elems;
}

// -----------------------------------------------------------------------------

static void CheckVarlen(atf_amc::VarlenAlloc *rec, int n) {
    vrfy_(rec && rec->length == sizeof(atf_amc::VarlenAlloc) + n*sizeof(int));
    for (int i=0; i<n; i++) {
        vrfy_(elem_Getary(*rec)[i] == i+1);
    }
}

void atf_amc::amctest_VarlenAlloc() {
    int temp[256];
    for (int i=0; i<256; i++) {
        temp[i]=i+1;
    }
    // alloc using Alloc
    {
        atf_amc::VarlenAlloc *rec = atf_amc::varlenalloc_AllocMaybe(256);
        memcpy(elem_Getary(*rec).elems,temp,sizeof(temp));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
    // alloc using Extra
    {
        atf_amc::VarlenAlloc *rec = &atf_amc::varlenalloc_AllocExtra(temp,sizeof(temp));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
    // alloc using Varlen
    {
        atf_amc::VarlenAlloc *rec = &atf_amc::varlenalloc_AllocVarlen(aryptr<int>(temp,256));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
    // alloc using Extra
    {
        atf_amc::VarlenAlloc *rec = atf_amc::varlenalloc_AllocExtraMaybe(temp,sizeof(temp));
        vrfy_(rec && rec->length == sizeof(atf_amc::VarlenAlloc) + sizeof(temp));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
}
