// Copyright (C) 2020-2023 Astra
// Copyright (C) 2023 AlgoRND
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
// Source: cpp/atf/ci/acr_ed.cpp
//

#include "include/atf_ci.h"
#include "include/define.h"

// -----------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_ssimdb() {
    // create a new ssimdb
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.target="ssimdb";
    acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ExecX(acr);
}

// -----------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_ssimfile() {
    // create a new ssimdb
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.target="ssimdb";
        acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // create a new ssimfile
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="ssimdb.xyz";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // insert a tuple
    vrfy_(SysCmd("echo dev.xyz xyz:blah | acr -insert -write")==0);
    // query it
    vrfy_(SysCmd("acr xyz")==0);

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ExecX(acr);

    // build everything
    command::abt_proc abt;
    abt.cmd.target.expr="abt";
    abt_ExecX(abt);
}

// --------------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_target() {
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.target="acr_test";
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);
}
