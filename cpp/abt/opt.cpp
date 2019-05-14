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
// Source: cpp/abt/opt.cpp -- Calculate compiler options
//
// Created By: ara.aslyan alexei.lebedev
// Recent Changes: ara.aslyan alexei.lebedev
//

#include "include/abt.h"

// check if given opt value is in the set
bool abt::Matches(abt::FToolOpt& opt) {
    bool ok = true;
    ok &= !ch_N(opt.uname)    || opt.uname    == abt::_db.cmdline.uname;
    ok &= !ch_N(opt.compiler) || opt.compiler == abt::_db.cmdline.compiler;
    ok &= !ch_N(opt.cfg)      || opt.cfg      == abt::_db.cmdline.cfg;
    ok &= !ch_N(opt.arch)     || opt.arch     == abt::_db.cmdline.arch;
    return ok;
}

// -----------------------------------------------------------------------------

tempstr abt::EvalSrcfileOpts(abt::FTarget &target, abt::FSrcfile &srcfile) {
    tempstr opts;

    // Detect source file language here
    if (abt::_db.cmdline.compiler == "g++" || abt::_db.cmdline.compiler == "clang++") {
        if (ext_Get(srcfile) =="c") {
            opts << " -x c";
        } else if (ext_Get(srcfile) =="cpp") {
            opts << " -x c++";
            opts << " -Wno-invalid-offsetof";
        }
    }

    // collect compiler options from tool_opt table
    // Ideally, ALL options are specified via tool_opt, but negative regexes are
    // a problem
    ind_beg(abt::_db_tool_opt_curs, opt,abt::_db) {
        bool ok = opt.opt_type == "CC_OPTS";
        ok     |= opt.opt_type == "INCL";
        ok     &= Matches(opt);
        ok     &= !ch_N(opt.target) || opt.target == target.target;
        //skip Werror when compiling an h file with clang.
        ok     &= ext_Get(srcfile) != "h" || opt.opt != "-Werror" || abt::_db.cmdline.compiler != "clang++";

        if (ok) {
            if (opt.opt_type == "INCL") {
                opts << " -iquote";
            }
            opts << " " << opt.opt;
        }
    }ind_end;

    // use precompiled header in this target
    ind_beg(abt::target_c_targdep_curs, dep,target) {
        if (dep.p_parent->p_ns->nstype == dmmeta_Nstype_nstype_pch) {
            // e.g.
            // src.srcfile == include/algo_pch.hpp
            // src.objkey == build/Linux-g++.release-x86_64/include.algo_pch.hpp.gch
            abt::FSrcfile& src = *c_srcfile_Find(*dep.p_parent, 0);
            if (abt::_db.cmdline.compiler == dev_Compiler_compiler_clangPP) {
                opts << " -include-pch "<<src.objpath;
            } else {
                opts << " -iquote "<<GetDirName(src.objpath);
                opts << " -include "<<src.objkey;
            }
            // This is a hack to make coverity happy, because coverity doesn't recognize gcc pch.
            if (abt::_db.cmdline.coverity && PchQ(src)) {
                opts << " -include "<<src.srcfile;
            }
        }
    }ind_end;

    // Enable pedantic builds on all c++ code that we control
    // TODO add the same level of pedantism for clang
    if (abt::_db.cmdline.compiler == "g++") {
        bool extrn = StartsWithQ(srcfile.srcfile, "extern/");
        if (!extrn) {
            opts << " -Wpedantic";
        }
    }

    // Don't allow $-s to leak in from tool_opt
    vrfy(!range_N(ch_FindFirst(opts,'$')), "$-expressions not allowed");

    return opts;
}

// -----------------------------------------------------------------------------

// Build global linked list zs_libdep.
// zs_libdep_visited is used in between calls.
static void ComputeLibdep(abt::FTarget &target) {
    if (target.p_ns->nstype != dmmeta_Nstype_nstype_pch && !zsl_libdep_visited_InLlistQ(target)) {
        zsl_libdep_visited_Insert(target);
        // targdeps have to be scanned in reverse order because we're building
        // a stack zs_libdep; absent any dependencies between targdep targets,
        // their libs appear on the command line in targdpe file order
        for (int i=c_targdep_N(target)-1; i>=0; --i) {
            ComputeLibdep(*c_targdep_Find(target,i)->p_parent);
        }
        if (target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
            zsl_libdep_Insert(target);
        }
    }
}


// -----------------------------------------------------------------------------
tempstr abt::EvalLinkCmdline(abt::FTarget &target) {
    tempstr cmd;
    cmd << abt::_db.c_compiler->compiler <<" -o " <<target.outfile;            // create executable

    ind_beg(abt::target_c_srcfile_curs, srcfile, target) {
        if (!HeaderExtQ(GetFileExt(srcfile.srcfile))) {
            cmd << " "<< srcfile.objpath;
        }
    }ind_end;

    // Calculate list of libs for this target.
    // Destructively scan the list and add lib filenames to command line
    abt::zsl_libdep_visited_RemoveAll();
    ComputeLibdep(target);
    while (abt::zsl_libdep_First())  {
        cmd << " "<< abt::zsl_libdep_RemoveFirst()->outfile;
    }
    //put algo_lib after gen_lib for exe targets.
    //solves the interdependency issue between gen_lib and algo_lib.
    cmd<<" "<<abt::_db.cmdline.out_dir<<"/algo_lib-"<< abt::_db.cmdline.arch<<".a";
    // collect link options
    ind_beg(abt::_db_tool_opt_curs, opt,abt::_db) {
        bool ok = opt.opt_type == "LINK_OPTS";
        ok     &= Matches(opt);
        ok     &= !ch_N(opt.target) || opt.target == target.target;
        if (ok) {
            cmd << " " << opt.opt;
        }
    }ind_end;

    // attach targsyslibs
    ind_beg(abt::target_c_alllib_curs, syslib, target) {
        cmd << " -l" << syslib.syslib;
    }ind_end;

    return cmd;
}
