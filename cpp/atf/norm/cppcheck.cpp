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
// Target: atf_norm (exe) -- Normalization tests (see normcheck table)
// Exceptions: yes
// Source: cpp/atf/norm/cppcheck.cpp
//

#include "include/atf_norm.h"

// -----------------------------------------------------------------------------

static void Cppcheck(strptr uname, strptr compiler, strptr platform) {
    cstring builddir = tempstr()<<"temp/cppcheck."<<uname<<"."<<compiler;
    cstring project  = tempstr()<<builddir<<"/project.json";
    CreateDirRecurse(builddir);

    // create json compile database
    command::abt_proc abt;
    Regx_ReadSql(abt.cmd.target,"%",true);
    abt.cmd.uname       = uname;
    abt.cmd.cfg         = "release";
    abt.cmd.compiler    = compiler;
    abt.cmd.arch        = "x86_64";
    abt.cmd.jcdb        = project;
    abt_ExecX(abt);

    // run cppcheck
    // interesting results with addons, but execution time greatly increases
    SysCmd(tempstr()
           << "cppcheck --error-exitcode=1"
           << " --quiet" // suppress too verbose progress
           //<< " --addon=cert"
           //<< " --addon=threadsafety"
           << " --std=c++03"
           << " --platform="<<platform
           << " --project="<<project
           << " -i extern" // ignore third party code
           << " --cppcheck-build-dir="<<builddir
           << " --suppressions-list=test/cppcheck-suppress"
           ,FailokQ(false));
}

// Run static code analyzer
// Check both Linux and Windows configs on Linux only
void atf_norm::normcheck_cppcheck() {
#ifdef __linux__
    Cppcheck("Linux","g++","unix64");
#else
    (void)Cppcheck;
#endif
}
