// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: abt (exe) -- Algo Build Tool (build system)
// Exceptions: NO
// Source: cpp/abt/disas.cpp -- Disassemble
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/abt.h"

static bool Disas_Prefilter(cstring &objfile) {
    algo_lib::Regx text_regx;
    //0000000000000000 g     F .text._Z15Main_ReserveMemv0000000000000178 Main_ReserveMem()
    Regx_ReadSql(text_regx, "%.text%", true);
    unlink("temp/abt.syms");
    int rc=SysCmd(tempstr()<<"objdump --syms -C "<<objfile<<"> temp/abt.syms 2>&1",FailokQ(true),DryrunQ(false));
    bool retval=false;
    if (rc==0) {
        MmapFile syms_file;
        MmapFile_Load(syms_file, "temp/abt.syms");
        ind_beg(algo::Line_curs, line, syms_file.text) if (Regx_Match(text_regx,line)) {        // Insert tuples
            if (Regx_Match(abt::_db.cmdline.disas,line)) {
                retval=true;
                break;
            }
        }ind_end;
    }
    return retval;
}

static void Disas_Show(cstring &objfile) {
    algo_lib::Regx ignore_regx;
    Regx_ReadSql(ignore_regx, "Disassembly of section%", true);
    algo_lib::Regx section_regx;
    //0000000000000000 <FuncName(u32, void*)>:
    Regx_ReadSql(section_regx, "0000000000000000 <%>:", true);
    unlink("temp/abt.s");
    int rc=SysCmd(tempstr()<<"objdump --disassemble -C "<<objfile<<"> temp/abt.s 2>&1",FailokQ(true),DryrunQ(false));
    if (rc==0) {
        MmapFile disas_file;
        MmapFile_Load(disas_file, "temp/abt.s");
        bool interesting_section=false;
        cstring section_text;
        ind_beg(algo::Line_curs, line, disas_file.text) if (!Regx_Match(ignore_regx,line)) {        // Insert tuples
            if (Regx_Match(section_regx,line)) {
                if (interesting_section) {
                    prlog(section_text);
                }
                interesting_section=Regx_Match(abt::_db.cmdline.disas,line);
                ch_RemoveAll(section_text);
            }
            if (interesting_section) {
                section_text<<line<<eol;
            }
        }ind_end;
        if (interesting_section) {
            prlog(section_text);
        }
    }
}

void abt::Main_Disas() {
    ind_beg(abt::_db_zs_origsel_target_curs, target,abt::_db) {// scan initial target set
        ind_beg(abt::target_c_srcfile_curs, srcfile,target) if (FileQ(srcfile.objpath)) {
            if (Disas_Prefilter(srcfile.objpath)) {
                Disas_Show(srcfile.objpath);
            }
        }ind_end;
    }ind_end;
}
