// Copyright (C) 2023-2024 AlgoRND
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
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/disas.cpp -- Disassemble
//

#include "include/abt.h"

static bool Disas_Prefilter(cstring &objfile) {
    algo_lib::Regx text_regx;
    //0000000000000000 g     F .text._Z15Main_ReserveMemv0000000000000178 Main_ReserveMem()
    Regx_ReadSql(text_regx, "%.text%", true);
    tempstr syms_fname = ReplaceExt(objfile,".syms");
    DeleteFile(syms_fname);
    int rc=SysCmd(tempstr()<<"objdump --syms -C "<<objfile<<"> "<<syms_fname<<" 2>&1",FailokQ(true),DryrunQ(false));
    bool retval=false;
    if (rc==0) {
        algo_lib::MmapFile syms_file;
        MmapFile_Load(syms_file, syms_fname);
        ind_beg(algo::Line_curs, line, syms_file.text) if (Regx_Match(text_regx,line)) { // Insert tuples
            if (Regx_Match(abt::_db.cmdline.disas,line)) {
                retval=true;
                break;
            }
        }ind_end;
    }
    return retval;
}

static void FlushSection(bool interesting_section, cstring &section_text, algo::strptr srcfile, algo::cstring &objfile) {
    if (interesting_section) {
        prlog("# abt.disas"
              <<Keyval("srcfile",srcfile)
              <<Keyval("objfile",objfile));
        prlog(section_text);
        section_text="";
    }
}

static void Disas_Show(algo::strptr srcfile, cstring &objfile) {
    algo_lib::Regx ignore_regx;
    Regx_ReadSql(ignore_regx, "Disassembly of section%", true);
    algo_lib::Regx section_regx;
    //0000000000000000 <FuncName(u32, void*)>:
    Regx_ReadSql(section_regx, "0000000000000000 <%>:", true);
    tempstr disas_fname = algo::ReplaceExt(objfile,".s");
    DeleteFile(disas_fname);
    int rc=SysCmd(tempstr()<<"objdump --disassemble -C "<<objfile<<"> "<<disas_fname<<" 2>&1",FailokQ(true),DryrunQ(false));
    if (rc==0) {
        algo_lib::MmapFile disas_file;
        MmapFile_Load(disas_file, disas_fname);
        bool interesting_section=false;
        cstring section_text;
        ind_beg(algo::Line_curs, line, disas_file.text) if (!Regx_Match(ignore_regx,line)) {        // Insert tuples
            if (Regx_Match(section_regx,line)) {
                FlushSection(interesting_section,section_text,srcfile,objfile);
                interesting_section=Regx_Match(abt::_db.cmdline.disas,line);
            }
            if (interesting_section) {
                section_text<<line<<eol;
            }
        }ind_end;
        FlushSection(interesting_section,section_text,srcfile,objfile);
    }
}

// Show disassembly of function matching regex _db.cmdline.disas
// in all selected targets & build directories
void abt::Main_Disas() {
    ind_beg(abt::_db_builddir_curs, builddir, abt::_db) if (builddir.select) {
        ind_beg(abt::_db_zs_sel_target_curs, target, abt::_db) {
            ind_beg(abt::target_c_srcfile_curs, srcfile,target) if (Regx_Match(_db.cmdline.srcfile,srcfile.srcfile)) {
                tempstr objpath = GetObjpath(builddir,srcfile);
                if (FileQ(objpath) && Disas_Prefilter(objpath)) {
                    Disas_Show(srcfile.srcfile,objpath);
                }
            }ind_end;
        }ind_end;
    }ind_end;
}
