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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/nextfile.cpp -- Find next file in target
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

// Return name of next/previous file in the list of sources
// (used to implement rotation across source files -- see top of this source file)
static tempstr Main_NextSrcFile(strptr srcfile) {
    tempstr ret;
    ind_beg(src_func::_db_targsrc_curs,targsrc,src_func::_db) {
        if (src_Get(targsrc)==srcfile) {
            if (src_func::_db.cmdline.other) {
                ret=src_Get(*src_func::target_cd_targsrc_Prev(targsrc));// previous circular list
            } else {
                ret=src_Get(*src_func::target_cd_targsrc_Next(targsrc));// next in circular list
            }
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Locate root directory with respect to which all targsources can be found.
static tempstr FindRootDir(strptr fname) {
    fname=GetDirName(fname);
    while (ch_N(fname)>1 && !FileQ(DirFileJoin(fname,".ffroot"))) {
        fname=StripDirComponent(fname);
    }
    return tempstr(fname)<<algo::MaybeDirSep;
}

// -----------------------------------------------------------------------------

// Find and print next/previous file
void src_func::Main_Nextfile() {
    strptr nextfile = src_func::_db.cmdline.nextfile;
    tempstr dir = FindRootDir(nextfile);
    // full pathname
    cstring srcfile(RestFrom(nextfile,ch_N(dir)));
    if (ch_N(srcfile)) {
        tempstr next;
        next=Main_NextSrcFile(srcfile);
        prlog_(DirFileJoin(dir,next));
    }
}
