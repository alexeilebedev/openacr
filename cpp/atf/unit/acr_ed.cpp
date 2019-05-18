// (C) 2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/acr_ed.cpp
//

#include "include/atf_unit.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

// --------------------------------------------------------------------------------

void atf_unit::unittest_acr_ed_CreateTarget() {
    // create a new executable in a sandbox.
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.target="acr_test";
    acr_ed.cmd.sandbox=true;
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);
}
