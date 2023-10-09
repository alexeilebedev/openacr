// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2023 AlgoRND
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
// Exceptions: NO
// Source: cpp/atf/amc/bigend.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

void atf_amc::amctest_BigEndian() {
    // print iba.Ipv6Addr -- MOVE TO SOMEWHERE
    // {
    //     u8 a[16] = {0x12,0x34,0x56,0x78,0x9A,0xbc,0xde,0xf0,0,0,0,1,0xff,1,1,0xff};
    //     ietf::Ipv6Addr addr;
    //     memcpy(&addr, a, sizeof(addr));
    //     cstring addr_str;
    //     Ipv6Addr_Print(addr, addr_str);
    //     vrfy_(addr_str == "1234:5678:9abc:def0:0000:0001:ff01:01ff");
    // }

    // Big-endian u16
    {
        const u16 val    = 0xfedc;
        const u16 val_be = 0xdcfe;
        atf_amc::TypeBE16 x;
        value_Set(x,val);
        vrfyeq_(val_be, x.value_be);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u32
    {
        const u32 val    = 0xfedcba98;
        const u32 val_be = 0x98badcfe;
        atf_amc::TypeBE32 x;
        value_Set(x,val);
        vrfyeq_(val_be, x.value_be);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u64
    {
        const u64 val    = 0xfedcba9876543210;
        const u64 val_be = 0x1032547698badcfe;
        atf_amc::TypeBE64 x;
        value_Set(x,val);
        vrfyeq_(val_be, x.value_be);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u64 dflt ctor
    {
        const u64 val    = 0xfedcba9876543210;
        const u64 val_be = 0x1032547698badcfe;
        atf_amc::TypeBE64dflt x;
        vrfyeq_(val_be, x.value_be);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u64 ctor
    {
        const u64 val    = 0xfedcba9876543210;
        const u64 val_be = 0x1032547698badcfe;
        atf_amc::TypeBE64 x(val);
        vrfyeq_(val_be, x.value_be);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u32 enum
    prlog("big-endian u32 enum") {
        prlog("with explicit ctor");
        atf_amc::TypeBE32en x(atf_amc_TypeBE32en_value_val1);
        // bytes are reversed so in the string representation, pairs of hex digits are reversed
        vrfyeq_(0x78563412, x.value_be);
        vrfyeq_(atf_amc_TypeBE32en_value_val1, value_GetEnum(x));

        prlog("with SetEnum");
        atf_amc::TypeBE32en y;
        value_SetEnum(y,atf_amc_TypeBE32en_value_val1);
        prlog(x.value_be);
        vrfyeq_(0x78563412, y.value_be);
        vrfyeq_(atf_amc_TypeBE32en_value_val1, value_GetEnum(y));
    }
    // Big-endian u64 -- print
    {
        const u64 val    = 0xfedcba9876543210;
        tempstr str_val;
        str_val << val;
        atf_amc::TypeBE64 x(val) ;
        tempstr str_valx;
        str_valx << x;
        vrfyeq_(str_val, str_valx);
    }
    // Big-endian u64 -- read
    {
        const u64 val    = 0xfedcba987654321;
        tempstr str_val;
        str_val << val;
        atf_amc::TypeBE64 x;
        TypeBE64_ReadStrptrMaybe(x,str_val);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u64 -- read tuple
    {
        const u64 val    = 0xfedcba987654321;
        Tuple t;
        attr_Add(t, "value", tempstr()<<val);
        atf_amc::TypeBE64 x;
        TypeBE64_ReadTupleMaybe(x,t);
        vrfyeq_(val, value_Get(x));
    }
    // Big-endian u32 -- print enum
    {
        atf_amc::TypeBE32en x(atf_amc_TypeBE32en_value_val1);
        tempstr str_valx;
        str_valx << x;
        vrfyeq_(str_valx, "val1");
    }
    // Big-endian u32 -- read enum
    {
        atf_amc::TypeBE32en x;
        TypeBE32en_ReadStrptrMaybe(x, "val1");
        vrfyeq_(atf_amc_TypeBE32en_value_val1, value_GetEnum(x));
    }
    // Big-endian u32 -- read enum tuple
    {
        Tuple t;
        attr_Add(t, "value", "val1");
        atf_amc::TypeBE32en x;
        TypeBE32en_ReadTupleMaybe(x,t);
        vrfyeq_(atf_amc_TypeBE32en_value_val1, value_GetEnum(x));
    }
    // Big-endian u64 -- bitfield
    {
        atf_amc::TypeBE64sf x;
        struct {
            int offset;
            int width;
            u64 (*getfcn)(const atf_amc::TypeBE64sf&);
            void (*setfcn)(atf_amc::TypeBE64sf&, u64);
        } range[8] = {
            {  63,  1, atf_amc::bit63_Get    , atf_amc::bit63_Set      }
            , {  61,  2, atf_amc::bits62_61_Get, atf_amc::bits62_61_Set  }
            , {  58,  3, atf_amc::bits60_58_Get, atf_amc::bits60_58_Set  }
            , {  53,  5, atf_amc::bits57_53_Get, atf_amc::bits57_53_Set  }
            , {  45,  8, atf_amc::bits52_45_Get, atf_amc::bits52_45_Set  }
            , {  32, 13, atf_amc::bits44_32_Get, atf_amc::bits44_32_Set  }
            , {  11, 21, atf_amc::bits31_11_Get, atf_amc::bits31_11_Set  }
            , {   0, 11, atf_amc::bits10_0_Get , atf_amc::bits10_0_Set   }
        };

        // running 'one'
        for (u64 run1 = 1; run1; run1<<=1) {
            frep_(r,8) {
                u64 msk = 0;
                frep_(i,range[r].width) {
                    msk |= (1ULL<<i);
                }
                u64 val = (run1 >> range[r].offset )&msk;
                if (val) {
                    range[r].setfcn(x,val);
                    vrfy_(range[r].getfcn(x) == val);
                    vrfy_(value_Get(x)       == run1);
                    vrfy_(x.value_be         == be64toh(run1));
                    range[r].setfcn(x,0);
                    vrfy_(range[r].getfcn(x) == 0);
                }
            }
        }
        // running 'zero'
        value_Set(x,~0ULL);
        for (u64 run1 = 1; run1; run1<<=1) {
            u64 run0 = ~run1;
            frep_(r,8) {
                u64 msk = 0;
                frep_(i,range[r].width) {
                    msk |= (1ULL<<i);
                }
                u64 val = (run1 >> range[r].offset)&msk;
                if (val) {
                    u64 val0 = ~val & msk;
                    range[r].setfcn(x,val0);
                    vrfy_(range[r].getfcn(x) == val0);
                    vrfy_(value_Get(x)       == run0);
                    vrfy_(x.value_be         == be64toh(run0));
                    range[r].setfcn(x,msk);
                    vrfy_(range[r].getfcn(x) == msk);
                }
            }
        }
    }

    // Big-endian u64 Hash field
    {
        const u64 val    = 0xfedcba9876543210;
        const u64 val_be = 0x1032547698badcfe;
        atf_amc::TypeBE64 x1,x2;
        value_Set(x1,val);
        value_Set(x2,val_be);
        vrfy_(u64_Hash(0,val)     == TypeBE64_Hash(0,x1));
        vrfy_(u64_Hash(0,val_be)  == TypeBE64_Hash(0,x2));
    }
    // Big-endian u64 ==
    {
        const u64 val    = 0xfedcba9876543210;
        const u64 val_be = 0x1032547698badcfe;
        atf_amc::TypeBE64 x1,x2;
        value_Set(x1,val);
        value_Set(x2,val_be);
        vrfy_(  x1 == x1 );
        vrfy_(  x2 == x2 );
        vrfy_(!(x1 == x2));
        vrfy_(!(x2 == x1));
    }
    // Big-endian u64 <, >
    {
        const u64 val    = 0xfedcba9876543210;
        const u64 val_be = 0x1032547698badcfe;
        atf_amc::TypeBE64 x1,x2;
        value_Set(x1,val);
        value_Set(x2,val_be);
        vrfy_(!(x1 < x1));
        vrfy_(!(x2 < x2));
        vrfy_(  x2 < x1 );
        vrfy_(!(x1 < x2));

        vrfy_(!(x1 > x1));
        vrfy_(!(x2 > x2));
        vrfy_(!(x2 > x1));
        vrfy_(x1 > x2);
        vrfy_(x1 >= x1);
        vrfy_(x1 <= x1);
        vrfy_(x1 >= x2);
        vrfy_(x2 <= x2);
    }
}

// --------------------------------------------------------------------------------

void atf_amc::amctest_BigendFconst() {
    atf_amc::PooledBE64 &be64 = atf_amc::pooledbe64_Alloc();
    value_Set(be64,atf_amc_PooledBE64_value_A);
    cstring str;
    str << be64;
    vrfyeq_(value_Get(be64), 3U);
    vrfyeq_(str, "atf_amc.PooledBE64  value:A");

    // read back
    value_Set(be64,0);
    PooledBE64_ReadStrptrMaybe(be64,str);
    vrfyeq_(value_Get(be64), 3U);
    vrfyeq_(str, "atf_amc.PooledBE64  value:A");
    pooledbe64_Delete(be64);
}
