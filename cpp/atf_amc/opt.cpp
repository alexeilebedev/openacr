// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/opt.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

#if GCC_VERSION >= 4
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

void atf_amc::amctest_OptG() {
    {
        u8 buf[1024];
        memset(buf, 0xff, sizeof(buf));
        atf_amc::OptG *optg = new(buf) atf_amc::OptG;
        atf_amc::TypeG *typeg = (atf_amc::TypeG*)((u8*)optg + sizeof(atf_amc::OptG));
        optg->length = sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
        typeg->typeg = 0x234234;
        vrfyeq_((u64)typeg_Get(*optg), (u64)typeg);
        vrfyeq_((u64)(*typeg_Get(*optg)).typeg, (u64)0x234234);
    }
    {
        // allocate an FOptG, which is derived from a type with an Opt field
        u8 buf[1024];
        memset(buf, 0xff, sizeof(buf));
        atf_amc::OptG *optg = new(buf) atf_amc::OptG;
        optg->length = 1024;
        atf_amc::FOptG *foptg = atf_amc::optg_InsertMaybe(*optg);
        vrfy_(foptg);
        vrfy_(memcmp(optg,foptg,optg->length)==0);
        vrfy_(foptg->length == optg->length);
        optg_Delete(*foptg);
    }
}

void atf_amc::amctest_OptG2() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptG *optg = new(buf) atf_amc::OptG;
    optg->length = sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG) - 1;
    vrfyeq_((u64)typeg_Get(*optg), (u64)NULL);
}

void atf_amc::amctest_OptOptG3() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptOptG *optoptg = new(buf) atf_amc::OptOptG;
    atf_amc::OptG *optg = (atf_amc::OptG*)((u8*)optoptg + sizeof(atf_amc::OptOptG));
    optoptg->length = sizeof(atf_amc::OptOptG) + sizeof(atf_amc::OptG);
    optg->length = sizeof(atf_amc::OptG);
    vrfyeq_((u64)optg_Get(*optoptg), (u64)optg);
}

void atf_amc::amctest_OptOptG4() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptOptG *optoptg = new(buf) atf_amc::OptOptG;
    atf_amc::OptG *optg = (atf_amc::OptG*)((u8*)optoptg + sizeof(atf_amc::OptOptG));
    optoptg->length = sizeof(atf_amc::OptOptG) + sizeof(atf_amc::OptG) - 1;
    optg->length = sizeof(atf_amc::OptG);
    vrfyeq_((u64)optg_Get(*optoptg), (u64)NULL);
}

void atf_amc::amctest_OptOptG5() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptOptG *optoptg = new(buf) atf_amc::OptOptG;
    atf_amc::OptG *optg = (atf_amc::OptG*)((u8*)optoptg + sizeof(atf_amc::OptOptG));
    optoptg->length = sizeof(atf_amc::OptOptG) + sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
    optg->length = sizeof(atf_amc::OptG) - 1;
    vrfyeq_((u64)optg_Get(*optoptg), (u64)NULL);
}

void atf_amc::amctest_OptOptG6() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptOptG *optoptg = new(buf) atf_amc::OptOptG;
    atf_amc::OptG *optg = (atf_amc::OptG*)((u8*)optoptg + sizeof(atf_amc::OptOptG));
    optoptg->length = sizeof(atf_amc::OptOptG) + sizeof(atf_amc::OptG);
    optg->length = sizeof(atf_amc::OptG) + 1;
    vrfyeq_((u64)optg_Get(*optoptg), (u64)NULL);
}

void atf_amc::amctest_OptOptG7() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptOptG *optoptg = new(buf) atf_amc::OptOptG;
    atf_amc::OptG *optg = (atf_amc::OptG*)((u8*)optoptg + sizeof(atf_amc::OptOptG));
    atf_amc::TypeG *typeg = (atf_amc::TypeG*)((u8*)optg + sizeof(atf_amc::OptG));
    optoptg->length = sizeof(atf_amc::OptOptG) + sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
    optg->length = sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
    typeg->typeg = 0x234234;
    vrfyeq_((u64)typeg_Get(*optg_Get(*optoptg))->typeg, u64(0x234234));
}

void atf_amc::amctest_OptG8() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    algo::memptr mem(buf,sizeof(buf));

    atf_amc::TypeG typeg;
    typeg.typeg = 0x12341234;

    atf_amc::OptG *optg = OptG_FmtMemptr(mem, &typeg, sizeof(atf_amc::TypeG));

    vrfyeq_((u64)optg->length, (u64)(sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG)));
    vrfyeq_((u64)typeg_Get(*optg)->typeg, (u64)0x12341234);
}

void atf_amc::amctest_OptOptG8() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    algo::memptr mem(buf,sizeof(buf));

    atf_amc::TypeG typeg;
    typeg.typeg = 0xabcabc;

    atf_amc::OptG *optg = OptG_FmtMemptr(mem, &typeg, sizeof(atf_amc::TypeG));
    optg->length = sizeof(atf_amc::TypeG) + sizeof(atf_amc::OptG);

    atf_amc::OptOptG *optoptg = OptOptG_FmtMemptr(mem, optg);

    vrfyeq_((u64)optoptg->length, (u64)(sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG) + sizeof(atf_amc::OptOptG)));
    vrfyeq_((u64)(*typeg_Get(*optg_Get(*optoptg))).typeg, (u64)0xabcabc);
}

void atf_amc::amctest_OptG9() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));

    atf_amc::OptG *optg = new(buf) atf_amc::OptG;

    atf_amc::TypeG *typeg = (atf_amc::TypeG*)((u8*)optg + sizeof(atf_amc::OptG));
    optg->length = sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
    typeg->typeg = 12345;

    vrfyeq_((u64)typeg_Get(*optg), (u64)typeg);
    vrfyeq_((u64)(*typeg_Get(*optg)).typeg, (u64)12345);
    // due to the hidden length field, an opt field is always printed with a name
    // is this a bug?
    tempstr str;
    str << *optg;
    vrfyeq_(str, "atf_amc.OptG  typeg:12345");
}

void atf_amc::amctest_OptOptG9() {
    u8 buf[1024];
    memset(buf, 0xff, sizeof(buf));
    atf_amc::OptOptG *optoptg = new(buf) atf_amc::OptOptG;
    atf_amc::OptG *optg = (atf_amc::OptG*)((u8*)optoptg + sizeof(atf_amc::OptOptG));
    atf_amc::TypeG *typeg = (atf_amc::TypeG*)((u8*)optg + sizeof(atf_amc::OptG));
    optoptg->length = sizeof(atf_amc::OptOptG) + sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
    optg->length = sizeof(atf_amc::OptG) + sizeof(atf_amc::TypeG);
    typeg->typeg = 12345;
    // due to the hidden length field, an opt field is always printed with a name
    // is this a bug?
    tempstr str;
    str << *optoptg;
    //prerr(str);
    vrfyeq_(str, "atf_amc.OptOptG  optg:\"atf_amc.OptG  typeg:12345\"");
}

void atf_amc::amctest_OptOptG10() {
    algo::ByteAry buf;
    algo::ByteAry temp;
    algo_lib::_db.varlenbuf = &temp;
    atf_amc::OptOptG *optoptg = new(ary_AllocN(buf, sizeof(atf_amc::OptOptG)).elems) atf_amc::OptOptG;
    atf_amc::OptOptG_ReadStrptrMaybe(*optoptg, "atf_amc.OptOptG  optg:'atf_amc.OptG'");
    optoptg->length = sizeof(atf_amc::OptOptG) + ary_N(temp);
    ary_Addary(buf,ary_Getary(temp));
    // the append may have moved the array, so the record is found again at its
    // start rather than read through the pointer taken before it
    optoptg = (atf_amc::OptOptG*)buf.ary_elems;
    tempstr str;
    str << *optoptg;
    vrfyeq_(str, "atf_amc.OptOptG  optg:atf_amc.OptG");
}

// -----------------------------------------------------------------------------

static void CheckOpt(atf_amc::OptAlloc *rec, int n) {
    vrfy_(rec && rec->length == sizeof(atf_amc::OptAlloc) + n*sizeof(int));
    int *data = (int*)typeg_Get(*rec);
    for (int i=0; i<n; i++) {
        vrfy_(data[i] == i+1);
    }
}

void atf_amc::amctest_OptAlloc() {
    int temp[256];
    for (int i=0; i<256; i++) {
        temp[i]=i+1;
    }
    // alloc and delete, to cover exception-throwing code
    {
        atf_amc::OptAlloc *rec = &atf_amc::optalloc_AllocExtra(temp,sizeof(temp));
        CheckOpt(rec,256);
        optalloc_Delete(*rec);
    }
    // alloc using Extra
    {
        atf_amc::OptAlloc *rec = atf_amc::optalloc_AllocExtraMaybe(temp,sizeof(temp));
        CheckOpt(rec,256);
        optalloc_Delete(*rec);
    }
    // check that too-small payload is not returned
    {
        atf_amc::OptAlloc *rec=&atf_amc::optalloc_AllocExtra(NULL,1); // 1 extra byte
        vrfy_(typeg_Get(*rec)==NULL); // not enough room to get at it
        optalloc_Delete(*rec);
    }
}
