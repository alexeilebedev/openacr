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
// Source: cpp/atf/norm/ssim.cpp -- ssim database normalizations
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_norm.h"

// -----------------------------------------------------------------------------

void atf_norm::normcheck_normalize_acr() {
    command::acr_proc acr;
    acr.cmd.check = true;
    acr.cmd.write = true;
    acr.cmd.query = "%";
    acr.cmd.print = false;
    acr.cmd.report = false;
    acr_ExecX(acr);
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_normalize_acr_my() {
    command::acr_my_proc acr_my;
    acr_my.cmd.abort = true;
    acr_my_ExecX(acr_my); //return to known state
    command::acr_my_proc acr_my2;
    acr_my2.cmd.start = true;
    acr_my2.cmd.stop  = true;
    acr_my2.cmd.nsdb.expr   = "%";
    acr_my_ExecX(acr_my2);//# round trip all data through mysql
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_ssimfile() {
    ind_beg(Dir_curs,dir,"data/*") {
        ind_beg(Dir_curs,file,tempstr()<<dir.pathname<<"/*.ssim") {
            tempstr ssimfile(Pathcomp(file.pathname,"/LR.RL"));// data/acmdb/device.ssim -> acmdb/device
            Replace(ssimfile,"/",".");
            normcheck(atf_norm::ind_ssimfile_Find(ssimfile)
                      ,"atf_norm.stray_ssimfile"
                      <<Keyval("fname",file.pathname)
                      <<Keyval("ssimfile",ssimfile)
                      <<Keyval("comment","No ssimfile entry exists for this file"));
        }ind_end;
    }ind_end;
}
