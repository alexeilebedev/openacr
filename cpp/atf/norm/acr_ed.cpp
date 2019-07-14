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
// Source: cpp/atf/norm/acr_ed.cpp
//

#include "include/atf_norm.h"
#include "include/define.h"

// -----------------------------------------------------------------------------

void atf_norm::normcheck_acr_ed_ssimdb() {
    // create a new ssimdb inside a sandbox
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.target="ssimdb";
    acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
    acr_ed.cmd.sandbox=true;
    acr_ed.cmd.sandbox_build=true;
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.in = "temp/acr_ed";
    acr.cmd.schema = "temp/acr_ed";
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ed_ExecX(acr_ed);
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_acr_ed_ssimfile() {
    // create a new ssimdb inside a sandbox
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.target="ssimdb";
        acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
        acr_ed.cmd.sandbox=true;
        acr_ed.cmd.sandbox_build=false;
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // go inside sandbox
    errno_vrfy(chdir("temp/acr_ed")==0, "chdir");
    
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="ssimdb.xyz";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // insert a tuple
    SysCmd("echo dev.xyz xyz:blah | acr -insert -write");
    // query it
    SysCmd("acr xyz");
    // build something
    SysCmd("abt 'abt'");

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ExecX(acr);
    errno_vrfy(chdir("../../")==0, "chdir");
}

// --------------------------------------------------------------------------------

void atf_norm::normcheck_acr_ed_target() {
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.target="acr_test";
    acr_ed.cmd.sandbox=true;
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);
}
