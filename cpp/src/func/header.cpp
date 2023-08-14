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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src/func/header.cpp -- Generate prototypes in header
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

// Load body of file
// Scan lines in file
// If found begin marker (namespace XYZ .. // update-hdr)
// Collect includes for namespace
// Find end marker
// Replace middle with prototypes for namespace XYZ
// When finished, save file
static void UpdateHeader(src_func::FTarget &target, strptr fname) {
    algo_lib::MmapFile file;
    MmapFile_Load(file,fname);
    cstring newfile;
    cstring ns;
    cstring srcfilter;
    bool inside=false;
    int nupdate=0;
    ind_beg(Line_curs,line,file.text) {
        strptr trimmed=Trimmed(line);
        if (!inside) {
            newfile << line << eol;
        }
        if (!inside && StartsWithQ(trimmed,"namespace") && FindStr(trimmed,"// update-hdr")!=-1) {
            ns = src_func::Nsline_GetNamespace(trimmed);
            srcfilter = src_func::Nsline_GetSrcfile(trimmed);
            inside=true;
        }
        if (inside && StartsWithQ(trimmed,"}")) {
            PrintGlobalProtos(target,ns,srcfilter,newfile);
            newfile << line << eol;
            inside=false;
            nupdate++;
        }
    }ind_end;
    // no update if nothing is found
    if (nupdate>0) {
        algo::Refurbish(file);
        verblog2("src_func.update"
                 <<Keyval("file",fname));
        int nbefore = algo_lib::_db.stringtofile_nwrite;
        SafeStringToFile(newfile,fname);
        src_func::_db.report.n_filemod += algo_lib::_db.stringtofile_nwrite - nbefore;
    }
}

// -----------------------------------------------------------------------------

static bool UpdateFileQ(src_func::FTargsrc &targsrc) {
    strptr src=src_Get(targsrc);
    return FindStr(src,"/gen/")==-1;
}

// -----------------------------------------------------------------------------

// Go over all sources (includes & sources!)
// Look for 'update-hdr' sections and insert function prototypes there.
void src_func::Main_UpdateHeader() {
    ind_beg(src_func::_db_target_curs,target,src_func::_db) {
        if (target.select) {
            ind_beg(src_func::target_cd_targsrc_curs,targsrc,target) {
                if (UpdateFileQ(targsrc)) {
                    UpdateHeader(target,src_Get(targsrc));
                }
            }ind_end;
        }
    }ind_end;
}
