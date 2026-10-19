// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/opt.cpp -- Calculate compiler options
//

#include "include/abt.h"

// -----------------------------------------------------------------------------

// Return the macro that announces SYSLIB to the compiler: rdmacm becomes
// ALGO_SYSLIB_RDMACM, with every character that is no letter or digit made an
// underscore.
static tempstr SyslibMacro(strptr syslib) {
    tempstr ret("ALGO_SYSLIB_");
    for (int i = 0; i < syslib.n_elems; i++) {
        char c = syslib[i];
        ret << char(isalnum(u8(c)) ? algo::ToUpper(c) : '_');
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return the -D options, each with a leading space, that announce to the
// compiler every optional need of TARGET or of a target it depends on that the
// platform of BUILDDIR provides.
//
// Say lib_x2net uses rdmacm where the platform provides it.  Its RDMA transport
// then has a second body for a platform without RDMA, and the source has to know
// which body to compile.  Testing the operating system would state the fact a
// second time, apart from dev.syslibuname, and a change to the table would leave
// the source deciding otherwise.  So the build states it: the platform
// provides rdmacm, and the compile line says -DALGO_SYSLIB_RDMACM to lib_x2net
// and to every target that includes its headers.
tempstr abt::GetSyslibDefines(abt::FBuilddir &builddir, abt::FTarget &target) {
    tempstr ret;
    ind_beg(abt::target_c_alldep_curs, dep, target) {
        ind_beg(abt::target_c_targsyslib_curs, targsyslib, dep) if (targsyslib.optional && abt::FindSyslibuname(builddir, targsyslib)) {
            tempstr define;
            define << " -D" << SyslibMacro(syslib_Get(targsyslib));
            if (algo::FindStr(ret, tempstr() << define << " ") == -1 && !EndsWithQ(ret, define)) {
                ret << define;
            }
        }ind_end;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return the compile options, each with a leading space, that pkg-config gave for
// every system library that TARGET or a target it depends on needs on the
// platform of BUILDDIR.
//
// Say a Python module includes Python.h.  Its directory is named for the interpreter's
// version, /usr/include/python3.12 on one host and python3.10 on another, so no
// row of dev.tool_opt can name it.  The host answers the question instead:
// dev.syslibuname names the pkg-config package, abt asks it once per run
// (Main_RunPkgconfig), and every source of a target that needs the library
// compiles with the answer.
tempstr abt::GetSyslibCflags(abt::FBuilddir &builddir, abt::FTarget &target) {
    tempstr ret;
    ind_beg(abt::target_c_alldep_curs, dep, target) {
        ind_beg(abt::target_c_targsyslib_curs, targsyslib, dep) {
            abt::FSyslibuname *syslibuname = abt::FindSyslibuname(builddir, targsyslib);
            if (syslibuname && ch_N(syslibuname->cflags) && algo::FindStr(ret, syslibuname->cflags) == -1) {
                ret << " " << syslibuname->cflags;
            }
        }ind_end;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

tempstr abt::EvalSrcfileCmdline(abt::FBuilddir &builddir, abt::FTarget &target, abt::FSrcfile &srcfile) {
    tempstr out;
    algo_lib::Replscope &R = builddir.R;
    // Detect source file language here
    bool c_file = ext_Get(srcfile) =="c";
    bool cpp_file = (ext_Get(srcfile) =="cpp" || ext_Get(srcfile) =="cc" )   ;
    bool hpp_file = ext_Get(srcfile) == "hpp";
    bool rc_file = ext_Get(srcfile) == "rc";
    if (rc_file) {
        out << builddir.p_compiler->rc;
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
        out << compiler_Get(builddir);
    }
    // collect compiler options from tool_opt table
    // Ideally, ALL options are specified via tool_opt, but negative regexes are
    // a problem
    ind_beg(abt::_db_tool_opt_curs, tool_opt,abt::_db) {
        if (Regx_Match(tool_opt.regx_opt,builddir.builddir) && Regx_Match(tool_opt.regx_target,target.target)) {
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
            } else if (opt_type_Get(tool_opt) == dev_opt_type_SOCC) {
                // an object of the so pass, which links into a shared object
                ok = builddir.so && (cpp_file || c_file || hpp_file);
            }
            if (ok) {
                out << " " << Subst(R,opt_Get(tool_opt));
            }
        }
    }ind_end;

    // the needs the platform meets, which a source with a second body tests
    if (c_file || cpp_file || hpp_file) {
        out << abt::GetSyslibDefines(builddir, target);
        out << abt::GetSyslibCflags(builddir, target);
    }

    // not doable via tool_opt
    // re-enable color output which gets squashed because output during parallel build
    // goes to a temp file.
    if (c_file || cpp_file || hpp_file) {
        if (compiler_Get(builddir) == dev_Compiler_compiler_gPP && _db.tty) {
            out << " -fdiagnostics-color=always";
        }
    }
    return out;
}

// -----------------------------------------------------------------------------

static void ComputeLibdepObjlist(abt::FBuilddir &builddir, abt::FTarget &origtarget, abt::FTarget &target, cstring &objs, cstring &libs) {
    // for lib dependencies, include the lib file as many time as needed
    if (&origtarget != &target && target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
        libs << " "<< GetOutfile(builddir,target);
    }
    if (bool_Update(target.libdep_visited,true)) {
        ind_beg(abt::target_c_targdep_curs,targdep,target) {
            ComputeLibdepObjlist(builddir,origtarget,*targdep.p_parent,objs,libs);
        }ind_end;
        if (&origtarget == &target || target.p_ns->nstype == dmmeta_Nstype_nstype_objlist) {
            ind_beg(abt::target_c_srcfile_curs, srcfile, target) {
                if (!abt::HeaderExtQ(GetFileExt(srcfile.srcfile))) {
                    objs << " "<< GetObjpath(builddir,srcfile);
                }
            }ind_end;
        }
    }
    // for lib dependencies, include the lib file as many time as needed
    if (&origtarget != &target && target.p_ns->nstype == dmmeta_Nstype_nstype_lib) {
        libs << " "<< GetOutfile(builddir,target);
    }
}

// -----------------------------------------------------------------------------

// Return the row of dev.syslibuname that says how the platform of BUILDDIR provides
// the system library TARGSYSLIB names, or NULL when the platform does not provide
// it, or provides it and this host lacks it (FSyslibuname.missing).  A target with
// such a need cannot be built in BUILDDIR.
abt::FSyslibuname *abt::FindSyslibuname(abt::FBuilddir &builddir, abt::FTargsyslib &targsyslib) {
    abt::FSyslibuname *ret = abt::ind_syslibuname_Find(dev::Syslibuname_Concat_syslib_uname(syslib_Get(targsyslib), uname_Get(builddir)));
    return ret && !ret->missing ? ret : NULL;
}

// -----------------------------------------------------------------------------

// Return list of object file pathnames and library pathnames for target TARGET
// into output variables OBJS and LIBS.  A system library that TARGET or a
// dependency needs is linked under the name the platform of BUILDDIR gives it, and
// not at all when the platform's C library already holds it.
void abt::DepsObjList(abt::FBuilddir &builddir, abt::FTarget &target, cstring &objs, cstring &libs) {
    ind_beg(abt::_db_target_curs,tgt,abt::_db) {
        tgt.libdep_visited=false;
    }ind_end;
    ComputeLibdepObjlist(builddir,target,target,objs,libs);
    ind_beg(abt::target_c_alldep_curs, dep, target) {
        ind_beg(abt::target_c_targsyslib_curs, targsyslib, dep) {
            abt::FSyslibuname *syslibuname = abt::FindSyslibuname(builddir, targsyslib);
            if (syslibuname && syslibuname->lib != "") {
                libs << " -l" << syslibuname->lib;
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

tempstr abt::EvalLinkCmdline(abt::FBuilddir &builddir, abt::FTarget &target) {
    algo_lib::Replscope &R = builddir.R;
    cstring opts;
    // collect link options: a shared object takes every option an executable
    // does, and the SOLINK ones besides
    bool so = target.p_ns->nstype == dmmeta_Nstype_nstype_so;
    ind_beg(abt::_db_tool_opt_curs, tool_opt,abt::_db) {
        if (Regx_Match(tool_opt.regx_opt,builddir.builddir)) {
            bool ok = opt_type_Get(tool_opt) == dev_opt_type_LINK
                || (so && opt_type_Get(tool_opt) == dev_opt_type_SOLINK);
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
        cmd<<" "<<abt::GetPassDir(builddir)<<Subst(R,"/algo_lib-$arch$libext");
    }

    return cmd;
}
