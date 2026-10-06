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
