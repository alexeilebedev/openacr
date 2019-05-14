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
// Target: atf_norm (exe) -- Run normalization tests (see normcheck table)
// Exceptions: yes
// Header: include/atf_norm.h
//
// Created By: alexei.lebedev vladimir.parizhsky
// Recent Changes: alexei.lebedev vladimir.parizhsky
//

#include "include/gen/atf_norm_gen.h"
#include "include/gen/atf_norm_gen.inl.h"

#define normcheck(x,y) { if (!(x)) { prerr(y); atf_norm::_db.c_normcheck->nerr++; } }


namespace atf_norm { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf/norm/bootstrap.cpp
    //

    // Create a bootstrap file for each build dir
    //     (user-implemented function, prototype is in amc-generated header)
    // void normcheck_bootstrap();

    // -------------------------------------------------------------------
    // cpp/atf/norm/main.cpp
    //

    // Check that a list of directories specified with DIRS is clean
    // Nothing is done if an error exit code is already set, since
    // this error might have been caused by the previous error.
    void CheckCleanDirs(strptr dirs);
    //     (user-implemented function, prototype is in amc-generated header)
    // void normcheck_testamc();
    // void normcheck_unit();
    // void normcheck_iffy_src();
    // void normcheck_normalize_amc_vis();
    // void normcheck_gitfile();
    void Main();

    // -------------------------------------------------------------------
    // cpp/atf/norm/readme.cpp
    //

    // Generate README.md by scanning the readme table
    // for instructions
    //     (user-implemented function, prototype is in amc-generated header)
    // void normcheck_readme();

    // -------------------------------------------------------------------
    // cpp/atf/norm/src.cpp -- source file (cpp/, include/) normalizations including amc
    //

    // indent any source files modified in the last commit
    //     (user-implemented function, prototype is in amc-generated header)
    // void normcheck_indent_srcfile();

    // indent all script files modified in the last commit
    // void normcheck_indent_script();

    // update copyright info in source files
    // void normcheck_copyright();

    // source code police
    // void normcheck_src_lim();

    // run amc
    // void normcheck_amc();
    // void normcheck_tempcode();
    // void normcheck_stray_gen();
    // void normcheck_build_clang();
    // void normcheck_build_gcc9();

    // -------------------------------------------------------------------
    // cpp/atf/norm/ssim.cpp -- ssim database normalizations
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void normcheck_normalize_acr();
    // void normcheck_normalize_acr_my();
    // void normcheck_ssimfile();
}
