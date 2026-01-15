// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/abt_md.cpp
//

#include "include/atf_ci.h"
#include "include/define.h"

// Runs in sandbox
void atf_ci::citest_abt_md_after_ssimfile_is_added() {
    // create a new ssimdb
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.target="dev";
        acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // create a new ssimfile
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="dev.xyz";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // run abt_md to produce xyz.md and add it into README.md
    command::abt_md_proc abt_md;
    abt_md.cmd.readmefile.expr = "txt/ssimdb/dev/%";
    abt_md_ExecX(abt_md);

    vrfy_(SysCmd("grep 'dev.xyz' txt/ssimdb/dev/README.md")==0);
}
