// Copyright (C) 2025,2026 AlgoRND
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/lib_netio.cpp
//
// --------------------------------------------------------------------------------

#include "include/atf_unit.h"
#include "include/lib_netio.h"

void atf_unit::unittest_lib_netio_GetHostAddr(){
    vrfy(lib_netio::GetHostAddr("example.com")!="","example.com resolution failed");
    vrfy(lib_netio::GetHostAddr("8.8.8.8")=="8.8.8.8","8.8.8.8 resolution failed");
    vrfy(lib_netio::GetHostAddr("0.0.0.0")=="0.0.0.0","0.0.0.0 resolution failed");
    vrfy(lib_netio::GetHostAddr("example.invalid")=="","example.invalid is resolved - wrong!");
    vrfy(lib_netio::GetHostAddr("")=="","empty string is resolved - wrong!");
    vrfy(lib_netio::GetHostAddr("0.0.0.0/24")=="","badly formed ip is resoved");
}
