// (C) AlgoEngineering LLC 2008-2013
// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: lib_iconv (lib) -- Iconv wrapper
// Exceptions: NO
// Source: cpp/lib/lib_iconv.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//
// Odbc/Iconvlib wrappers
// close conversion descriptor

#include "include/lib/lib_mysql.h"
#include "include/lib/lib_iconv.h"

void lib_iconv::Close(lib_iconv::Icd &icd) {
    if (icd.icd != (iconv_t)-1) {
        iconv_close(icd.icd);
        icd.icd = (iconv_t)-1;
    }
}

// open conversion descriptor
void lib_iconv::Open(lib_iconv::Icd& icd, strptr tocode, strptr fromcode) {
    tempstr _tocode(tocode);
    tempstr _fromcode(fromcode);
    lib_iconv::Close(icd);
    icd.icd = iconv_open(Zeroterm(_tocode),Zeroterm(_fromcode));
    vrfy((iconv_t)-1 != icd.icd ||  errno != EINVAL
         ,tempstr()<<"The conversion from "<<fromcode<<" to "<<tocode<<" is not supported by the implementation");
    errno_vrfy((iconv_t)-1 != icd.icd, "iconv");
}

// convert multibyte sequence
tempstr lib_iconv::Convert(lib_iconv::Icd &icd, strptr in, bool no_throw) {
    tempstr out;
    // first reserve exact number of bytes (assume zero-conversion)
    ch_Reserve(out,elems_N(in));
    size_t inoffset  = 0;
    size_t outoffset = 0;
    while (true) {
        char *inbuf         = in.elems + inoffset;
        char *outbuf        = out.ch_elems + outoffset;
        size_t inbytesleft  = elems_N(in) - inoffset;
        size_t outbytesleft = out.ch_max - outoffset;

        // do conversion
        size_t ret  = iconv(icd.icd,&inbuf,&inbytesleft,&outbuf,&outbytesleft);

        inoffset    = inbuf  - in.elems;
        outoffset   = outbuf - out.ch_elems;
        out.ch_n = outoffset;
        if ((size_t)-1 == ret) {
            switch(errno) {
            case E2BIG: {
                u32 reserve = 0;
                if (inoffset) {
                    // estimate mean conversion ratio
                    double ratio = double(outoffset)/double(inoffset) + 0.2; // 20% extra
                    reserve = static_cast<u32>(ratio*inbytesleft);
                }
                // minumum 6 bytes for widest encoding (UTF-8)
                reserve = reserve > 6 ? reserve : 6 ;
                ch_Reserve(out,reserve);
                continue;
            }
            case EILSEQ: {
                vrfy(no_throw,tempstr()<<"Invalid multibyte sequence (first 6 bytes shown): "
                     << strptr_ToMemptr(FirstN(strptr(inbuf,inbytesleft),6)));
                break;
            }
            case EINVAL:
                vrfy(no_throw,tempstr()<<"Incomplete multibyte sequence: "
                     << strptr_ToMemptr(strptr(inbuf,inbytesleft)));
                break;
            default:
                errno_vrfy(no_throw,"iconv");
                break;
            }
        }
        break;
    }
    return out;
}

void lib_iconv::icd_Cleanup(lib_iconv::Icd &icd) {
    lib_iconv::Close(icd);
}
