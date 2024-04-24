// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/abt/opt.cpp -- Calculate compiler options
//

#include "include/abt.h"

// -----------------------------------------------------------------------------

tempstr abt::EvalSrcfileCmdline(algo_lib::Replscope &R, abt::FTarget &target, abt::FSrcfile &srcfile) {
    tempstr out;

    // Detect source file language here
    bool c_file = ext_Get(srcfile) =="c";
    bool cpp_file = ext_Get(srcfile) =="cpp";
    bool hpp_file = ext_Get(srcfile) == "hpp";
    bool rc_file = ext_Get(srcfile) == "rc";
    if (rc_file) {
        out << abt::_db.c_compiler->rc;
    } else {
        if (abt::_db.gcache) {
            out << "gcache ";
            if (algo_lib::_db.cmdline.verbose>1) {
                out << " -v:"<<int(algo_lib::_db.cmdline.verbose-1);
            }
            if (cache_GetEnum(_db.cmdline) == command_abt_cache_gcache_force) {
                out << " -force";
            }
            out << " -- ";
        } else if (abt::_db.ccache) {
            out << "ccache ";
        }
        out << abt::_db.cmdline.compiler;
    }
    // collect compiler options from tool_opt table
    // Ideally, ALL options are specified via tool_opt, but negative regexes are
    // a problem
    ind_beg(abt::_db_tool_opt_curs, tool_opt,abt::_db) {
        if (tool_opt.select) {
            bool ok = false;
            if (opt_type_Get(tool_opt) == dev_opt_type_CPP) {
                ok = cpp_file;
            } else if (opt_type_Get(tool_opt) == dev_opt_type_C) {
                ok = c_file;
            } else if (opt_type_Get(tool_opt) == dev_opt_type_CC) {
                ok = cpp_file || c_file || hpp_file;
            } else if (opt_type_Get(tool_opt) == dev_opt_type_HPP) {
                ok = hpp_file;
            } else if (opt_type_Get(tool_opt) == dev_opt_type_RC) {
                ok = rc_file;
            }
            if (ok && Regx_Match(tool_opt.regx_target,target.target)) {
                out << " " << Subst(R,opt_Get(tool_opt));
            }
        }
    }ind_end;

    // not doable via tool_opt
    // re-enable color output which gets squashed because output during parallel build
    // goes to a temp file.
    if (c_file || cpp_file || hpp_file) {
        if (abt::_db.cmdline.compiler == dev_Compiler_compiler_gPP && _db.tty) {
            out << " -fdiagnostics-color=always";
        }
    }
    return out;
}

// -----------------------------------------------------------------------------

static void ComputeLibdepObjlist(abt::FTarget &origtarget, abt::FTarget &target, cstring &objs, cstring &libs) {
    // for lib dependencies, include the lib file as many time as needed
    if (&origtarget != &target && target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
        libs << " "<< target.outfile;
    }
    if (bool_Update(target.libdep_visited,true)) {
        ind_beg(abt::target_c_targdep_curs,targdep,target) {
            ComputeLibdepObjlist(origtarget,*targdep.p_parent,objs,libs);
        }ind_end;
        if (&origtarget == &target || target.p_ns->nstype == dmmeta_Nstype_nstype_objlist) {
            ind_beg(abt::target_c_srcfile_curs, srcfile, target) {
                if (!abt::HeaderExtQ(GetFileExt(srcfile.srcfile))) {
                    objs << " "<< srcfile.objpath;
                }
            }ind_end;
        }
    }
    // for lib dependencies, include the lib file as many time as needed
    if (&origtarget != &target && target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
        libs << " "<< target.outfile;
    }
}

// -----------------------------------------------------------------------------

// Return list of object file pathnames and library pathnames for target TARGET
// into output variables OBJS and LIBS
void abt::DepsObjList(abt::FTarget &target, cstring &objs, cstring &libs) {
    ind_beg(abt::_db_target_curs,tgt,abt::_db) {
        tgt.libdep_visited=false;
    }ind_end;
    ComputeLibdepObjlist(target,target,objs,libs);
    ind_beg(abt::target_c_alllib_curs, syslib, target) {
        libs << " -l" << syslib.syslib;
    }ind_end;
}

// -----------------------------------------------------------------------------

tempstr abt::EvalLinkCmdline(algo_lib::Replscope &R, abt::FTarget &target) {
    cstring opts;
    // collect link options
    ind_beg(abt::_db_tool_opt_curs, tool_opt,abt::_db) {
        if (tool_opt.select) {
            bool ok = opt_type_Get(tool_opt) == dev_opt_type_LINK;
            if (ok && Regx_Match(tool_opt.regx_target,target.target)) {
                opts << " " << Subst(R,opt_Get(tool_opt));
            }
        }
    }ind_end;
    Set(R,"$opts",opts);

    tempstr cmd;
    cmd << Subst(R,"$link $objs $opts $libs");

    // hack: add another algo_lib after gen_lib for exe targets.
    //solves the interdependency issue between gen_lib and algo_lib.
    if (FindStr(Subst(R,"$libs"),"/algo_lib-")!=-1) {
        cmd<<" "<<abt::_db.cmdline.out_dir<<Subst(R,"/algo_lib-$arch$libext");
    }

    return cmd;
}
