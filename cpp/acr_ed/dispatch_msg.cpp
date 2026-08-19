// Copyright (C) 2026 AlgoRND
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
// Target: acr_ed (exe) -- Script generator for common dev tasks
// Exceptions: yes
// Source: cpp/acr_ed/dispatch_msg.cpp -- Create dispatch_msg record
//

#include "include/algo.h"
#include "include/acr_ed.h"

// Add a dmmeta.dispatch_msg record routing a message ctype to a dispatch.
// Pkey form: "<dispatch>/<msgtype-ctype>", e.g. "lib_prot.Client/ams.LogMsg".
void acr_ed::edaction_Create_DispatchMsg() {
    tempstr key(acr_ed::_db.cmdline.dispatch_msg);
    tempstr dispatch(dmmeta::DispatchMsg_dispatch_Get(key));
    tempstr ctype(dmmeta::DispatchMsg_ctype_Get(key));
    vrfy(dispatch != "" && ctype != "",
         tempstr()<<"acr_ed.bad_dispatch_msg  dispatch_msg:"<<key
         <<"  comment:'expected <dispatch>/<msgtype>'");
    vrfy(acr_ed::ind_ctype_Find(ctype),
         tempstr()<<"acr_ed.no_ctype  ctype:"<<ctype);

    dmmeta::DispatchMsg dm;
    dm.dispatch_msg = key;
    dm.comment.value = acr_ed::_db.cmdline.comment;
    acr_ed::_db.out_ssim << dm << eol;
}
