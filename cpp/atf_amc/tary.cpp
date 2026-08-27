// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/tary.cpp
//

#include "include/atf_amc.h"


void atf_amc::amctest_TaryInit() {
    atf_amc::TaryU32 tary_u32;
    frep_(i,100) {
        u32 &elem = tary_u32_Alloc(tary_u32);
        vrfyeq_(elem, u32(44));
    }
}

void atf_amc::amctest_TaryInit2() {
    atf_amc::TaryU32 tary_u32;
    frep_(i,100) {
        algo::aryptr<u32> elems = tary_u32_AllocN(tary_u32, i);
        frep_(j,elems_N(elems)) {
            vrfyeq_(elems[j], u32(44));
        }
    }
}

void atf_amc::amctest_TaryInit3() {
    atf_amc::TaryU8 tary_u8;
    frep_(i,100) {
        u8 &elem = ary_Alloc(tary_u8);
        vrfyeq_(elem, u8(44));
    }
}

void atf_amc::amctest_TaryInit4() {
    atf_amc::TaryU8 tary_u8;
    frep_(i,100) {
        algo::aryptr<u8> elems = ary_AllocN(tary_u8, i);
        frep_(j,elems_N(elems)) {
            vrfyeq_(elems[j], u8(44));
        }
    }
}

void atf_amc::amctest_TaryReserve() {
    atf_amc::TaryU8 tary_u8;
    vrfy_(ary_N(tary_u8) + 1 > ary_Max(tary_u8));
    ary_Reserve(tary_u8,1);
    vrfy_(ary_N(tary_u8) + 1 <= ary_Max(tary_u8));
    vrfy_(ary_N(tary_u8) + 10 > ary_Max(tary_u8));
    ary_Reserve(tary_u8,10);
    vrfy_(ary_N(tary_u8) + 10 <= ary_Max(tary_u8));
    vrfy_(ary_N(tary_u8) + 100 > ary_Max(tary_u8));
    ary_Reserve(tary_u8,100);
    vrfy_(ary_N(tary_u8) + 100 <= ary_Max(tary_u8));
    vrfy_(ary_N(tary_u8) + 1000 > ary_Max(tary_u8));
}

void atf_amc::amctest_TaryHash() {
    Hashable &h=hashable_Alloc();
    ary_Alloc(h.key)=1234;
    ary_Alloc(h.key)=5678;
    hashable_XrefMaybe(h);

    algo::U64Ary key;
    ary_Alloc(key)=1234;
    ary_Alloc(key)=5678;
    vrfy_(ind_hashable_Find(key) == &h);
    hashable_Delete(h);
}

void atf_amc::Insary(algo::StringAry &ary, const char *rhs[], int at) {
    algo::StringAry _rhs;
    for (; *rhs; ++rhs) {
        ary_Alloc(_rhs) = *rhs;
    }
    ary_Insary(ary,_rhs, at);
}

bool atf_amc::Cmpary(algo::StringAry &ary, const char **rhs) {
    return Cmpary(ary_Getary(ary),rhs);
}

bool atf_amc::Cmpary(algo::aryptr<cstring> ary, const char **rhs) {
    bool ret(true);
    int ind = 0;
    for (; ret && rhs[ind] && ind<elems_N(ary); ++ind) {
        ret &= ary[ind] == rhs[ind];
    }
    ret &= ind==elems_N(ary) && !rhs[ind];
    if (!ret) {
        cstring out;
        algo::ListSep ls1,ls2;
        out << "Array mismatch, got {";
        frep_(i,elems_N(ary)) {
            out << ls1 << ary[i];
        }
        out << "}, but shall be {";
        for (int i = 0; rhs[i]; ++i) {
            out << ls2 << rhs[i];
        }
        out << "}";
        prlog(out);
    }
    return ret;
}

void atf_amc::amctest_TaryInsary() {
    {
        // mem case
        cstring ary;
        ch_Insary(ary,"",0);
        vrfyeq_(ary,"");
        ch_Insary(ary,"ABC",0);
        vrfyeq_(ary,"ABC");
        ch_Insary(ary,"DEF",0);
        vrfyeq_(ary,"DEFABC");
        ch_Insary(ary,"GHI",6);
        vrfyeq_(ary,"DEFABCGHI");
        ch_Insary(ary,"JKL",4);
        vrfyeq_(ary,"DEFAJKLBCGHI");
    }
    {
        // ctor case
        algo::StringAry ary;
        const char *empty[] = {0};
        const char *abc[] = {"alice","bob","charlie",0};
        const char *def[] = {"dave","eve","freddy",0};
        const char *ghi[] = {"george","henry","ivy",0};
        const char *jkl[] = {"johny","ken","lisa",0};
        const char *defabc[] = {"dave","eve","freddy","alice","bob","charlie",0};
        const char *defabcghi[] = {"dave","eve","freddy","alice","bob","charlie","george","henry","ivy",0};
        const char *defajklbcghi[] = {"dave","eve","freddy","alice","johny","ken","lisa","bob","charlie","george","henry","ivy",0};

        Insary(ary,empty,0);
        vrfy_(Cmpary(ary,empty));
        Insary(ary,abc,0);
        vrfy_(Cmpary(ary,abc));
        Insary(ary,def,0);
        vrfy_(Cmpary(ary,defabc));
        Insary(ary,ghi,6);
        vrfy_(Cmpary(ary,defabcghi));
        Insary(ary,jkl,4);
        vrfy_(Cmpary(ary,defajklbcghi));
    }
}

// An out-of-range insertion index must abort via FatalErrorExit, not silently
// corrupt memory. ary_Insary's bounds check is under test; running it in a
// child lets the parent confirm the child exited with the FatalErrorExit
// status (1) rather than crashing (signal) or returning normally (status 0).
void atf_amc::amctest_TaryInsaryBadIndex() {
    tempstr path;
    path << "temp/atf_amc_insary.fatal";
    DeleteFile(path);
    int pid = fork();
    if (pid == 0) {
        algo_lib::DieWithParent();
        FILE *devnull = freopen("/dev/null", "w", stderr);// suppress the expected fatal-error banner
        (void)devnull;
        // The banner goes out on the stderr just discarded, so name a record
        // file as well: it is the only account of how far the fatal path ran,
        // and on a system where that path dies it is the only way to say which
        // part of it did.
        algo_lib::_db.fatalerr_file << path;
        algo::Zeroterm(algo_lib::_db.fatalerr_file);
        algo::StringAry ary;
        const char *abc[] = {"a", "b", "c", 0};
        const char *one[] = {"x", 0};
        Insary(ary, abc, 0);
        Insary(ary, one, 1000);
        _exit(0);
    } else {
        int status = 0;
        waitpid(pid, &status, 0);
        // Say what the child did, not merely that it was wrong: a fatal exit,
        // a crash inside the report the fatal path composes, and a bounds check
        // that never fired all fail this one assertion, and they call for three
        // different repairs.
        tempstr outcome;
        if (WIFEXITED(status)) {
            outcome << "exit:" << WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            outcome << "signal:" << WTERMSIG(status);
        } else {
            outcome << "status:" << status;
        }
        tempstr record;
        if (FileQ(path)) {
            record = FileToString(path);
        }
        if (outcome != "exit:1") {
            prlog("atf_amc.insary_fatal"<<Keyval("outcome",outcome)<<Keyval("record",record));
        }
        vrfyeq_(outcome, tempstr("exit:1"));
    }
}

void atf_amc::amctest_TaryAllocNAt() {
    {
        // mem case
        {
            cstring ary;
            vrfyeq_(ch_AllocNAt(ary,0,0),strptr());
            vrfyeq_(ary,strptr());
        }
        {
            cstring ary("abc");
            vrfyeq_(ch_AllocNAt(ary,3,0),strptr("\0\0\0",3));
            vrfyeq_(ary,strptr("\0\0\0abc",6));
        }
        if (0) {
            cstring ary("abc");
            vrfyeq_(ch_AllocNAt(ary,3,1),strptr("\0\0\0",3));
            vrfyeq_(ary,strptr("a\0\0\0bc",6));
        }
        if (0) {
            cstring ary("abc");
            vrfyeq_(ch_AllocNAt(ary,3,3),strptr("\0\0\0",3));
            vrfyeq_(ary,strptr("abc\0\0\0",6));
        }
    }
    {
        //const char *a1[] = {"","","",0};

        // ctor case
        {
            algo::StringAry ary;
            const char *empty[] = {0};
            vrfy_(Cmpary(ary_AllocNAt(ary,0,0),empty));
            vrfy_(Cmpary(ary,empty));
        }
        {
            algo::StringAry ary;
            const char *empty3[] = {"","","",0};
            const char *src[] = {"abc","def","ghi",0};
            const char *res[] = {"","","","abc","def","ghi",0};
            Insary(ary,src,0);
            vrfy_(Cmpary(ary_AllocNAt(ary,3,0),empty3));
            vrfy_(Cmpary(ary,res));
        }
        {
            algo::StringAry ary;
            const char *empty3[] = {"","","",0};
            const char *src[] = {"abc","def","ghi",0};
            const char *res[] = {"abc","","","","def","ghi",0};
            Insary(ary,src,0);
            vrfy_(Cmpary(ary_AllocNAt(ary,3,1),empty3));
            vrfy_(Cmpary(ary,res));
        }
        {
            algo::StringAry ary;
            const char *empty3[] = {"","","",0};
            const char *src[] = {"abc","def","ghi",0};
            const char *res[] = {"abc","def","ghi","","","",0};
            Insary(ary,src,0);
            vrfy_(Cmpary(ary_AllocNAt(ary,3,3),empty3));
            vrfy_(Cmpary(ary,res));
        }
    }
}

// -----------------------------------------------------------------------------

static void Append(algo::ByteAry &buf, algo::strptr str) {
    ary_Addary(buf, strptr_ToMemptr(str));
}

static algo::strptr ToStr(algo::ByteAry &buf) {
    return memptr_ToStrptr(ary_Getary(buf));
}

void atf_amc::amctest_TaryRemove() {
    algo::ByteAry buf;
    vrfyeq_(ToStr(buf),"");

    // remove from empty array
    ary_Remove(buf,1);
    vrfyeq_(ToStr(buf),"");

    Append(buf, "0123456789");
    vrfyeq_(ToStr(buf),"0123456789");

    // remove valid element
    ary_Remove(buf,1);
    vrfyeq_(ToStr(buf),"023456789");

    // remove invalid element to the left
    ary_Remove(buf,-1);
    vrfyeq_(ToStr(buf),"023456789");

    // remove invalid element to the right
    ary_Remove(buf,127);
    vrfyeq_(ToStr(buf),"023456789");

    // ---

    // remove invalid region on the left
    ary_RemRegion(buf,-1,1);
    vrfyeq_(ToStr(buf),"023456789");

    // remove region partially trimmed on the left
    ary_RemRegion(buf,-1,2);
    vrfyeq_(ToStr(buf),"23456789");

    // remove valid element
    ary_RemRegion(buf,1,1);
    vrfyeq_(ToStr(buf),"2456789");

    // remove empty region to the right of valid range
    ary_RemRegion(buf,10,1);
    vrfyeq_(ToStr(buf),"2456789");

    // remove tail, trimmed
    ary_RemRegion(buf,5,10);
    vrfyeq_(ToStr(buf),"24567");

    // remove head, trimmed
    ary_RemRegion(buf,-10,20);
    vrfyeq_(ToStr(buf),"");

    // remove from empty string
    ary_RemRegion(buf,0,0);
    vrfyeq_(ToStr(buf),"");
}

// Check that in all cases, Remove(i) == RemRegion(i,1)
void atf_amc::amctest_TaryRemove2() {
    algo::ByteAry buf;
    Append(buf, "12309817234987123");
    for (u32 i=-10; i<buf.ary_n+10; i++) {
        algo::ByteAry buf2;
        ary_Addary(buf2,ary_Getary(buf));
        ary_Remove(buf2,i);

        algo::ByteAry buf3;
        ary_Addary(buf3,ary_Getary(buf));
        ary_RemRegion(buf3,i,1);

        vrfy_(ToStr(buf2) == ToStr(buf3));
    }
}
