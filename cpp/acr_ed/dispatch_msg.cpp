// Copyright (C) 2026 AlgoX2 Corp
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
