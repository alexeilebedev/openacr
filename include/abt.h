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
// Header: include/abt.h -- Main header
//

#include "include/algo.h"
#include "include/gen/abt_gen.h"
#include "include/gen/abt_gen.inl.h"

namespace abt { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/abt/build.cpp -- Build dag execution
    //
    abt::FSyscmd* NewCmd(abt::FSyscmd *start, abt::FSyscmd *end);
    void Main_Build();

    // -------------------------------------------------------------------
    // cpp/abt/disas.cpp -- Disassemble
    //

    // Show disassembly of function matching regex _db.cmdline.disas
    // in all selected targets & build directories
    void Main_Disas();

    // -------------------------------------------------------------------
    // cpp/abt/gitinfo.cpp
    //

    // Write build/gitinfo.ssim and build/gitinfo.h, the dev.gitinfo tuple that
    // algo::gitinfo_Get() returns.  Runs before the header scan, so the include
    // arg.cpp carries always resolves.  The tuple names the commit and the moment
    // of the build and nothing about the build directory: one pair of files serves
    // every config, so a per-config value would make alternating configs restamp
    // each other's builds.
    void WriteGitinfo();

    // -------------------------------------------------------------------
    // cpp/abt/main.cpp -- Algo Build Tool - Main file
    //
    bool HeaderExtQ(strptr ext);

    // Access cached 'stat' results for FNAME
    abt::FFilestat &GetFilestat(strptr fname);

    // how are we using this execkey???
    //     (user-implemented function, prototype is in amc-generated header)
    // i64 execkey_Get(abt::FSyscmd &cmd);
    bool SourceQ(abt::FTargsrc &targsrc);

    // Return true when the pass BUILDDIR is running builds TARGET (Main_MarkPass).
    bool InPassQ(abt::FBuilddir &builddir, abt::FTarget &target);

    // Return the directory the pass BUILDDIR is running writes its objects, archives
    // and links to: the build directory, or its so/ for the so pass.
    tempstr GetPassDir(abt::FBuilddir &builddir);

    // compute obj key by replacing path components
    // with .
    // So, cpp/abt/main.cpp becomes cpp.abt.main.cpp
    // Next step will be to replace the extension
    tempstr GetObjkey(strptr source);

    // Compute object pathname by combinindg builddir path,
    // objkey, and extension.
    tempstr GetObjpath(abt::FBuilddir &builddir, abt::FSrcfile &srcfile);
    void DeleteFileV(strptr path);
    tempstr GetOutfile(abt::FBuilddir &builddir, abt::FTarget &target);
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:abt

    // -------------------------------------------------------------------
    // cpp/abt/ood.cpp
    //

    // Compute cumulative modification timestamp for source files
    // and targets.
    // This calculates SRCFILE.CUM_MODTIME, TARGET.CUM_MODTIME
    void ComputeTimestamps();

    // Recalculate SRCFILE.OOD, TARGET.OOD, TARGET.OUT_MODTIME for the pass of the
    // build directory BUILDDIR, and report how many of the pass's sources and targets
    // are out of date
    void ComputeOod(abt::FBuilddir &builddir);

    // -------------------------------------------------------------------
    // cpp/abt/opt.cpp -- Calculate compiler options
    //

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
    tempstr GetSyslibDefines(abt::FBuilddir &builddir, abt::FTarget &target);

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
    tempstr GetSyslibCflags(abt::FBuilddir &builddir, abt::FTarget &target);
    tempstr EvalSrcfileCmdline(abt::FBuilddir &builddir, abt::FTarget &target, abt::FSrcfile &srcfile);

    // Return the row of dev.syslibuname that says how the platform of BUILDDIR provides
    // the system library TARGSYSLIB names, or NULL when the platform does not provide
    // it, or provides it and this host lacks it (FSyslibuname.missing).  A target with
    // such a need cannot be built in BUILDDIR.
    abt::FSyslibuname *FindSyslibuname(abt::FBuilddir &builddir, abt::FTargsyslib &targsyslib);

    // Return list of object file pathnames and library pathnames for target TARGET
    // into output variables OBJS and LIBS.  A system library that TARGET or a
    // dependency needs is linked under the name the platform of BUILDDIR gives it, and
    // not at all when the platform's C library already holds it.
    void DepsObjList(abt::FBuilddir &builddir, abt::FTarget &target, cstring &objs, cstring &libs);
    tempstr EvalLinkCmdline(abt::FBuilddir &builddir, abt::FTarget &target);

    // -------------------------------------------------------------------
    // cpp/abt/scan.cpp
    //

    // This function always scans the entire graph of all sources
    void ScanSrcfile();
}
