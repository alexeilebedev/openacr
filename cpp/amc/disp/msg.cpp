// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/disp/msg.cpp -- Dispatch on message
//

#include "include/amc.h"

// Create a new dispatch collecting all messages
// using a given message header (e.g. find all xyz messages
// and create a dispatch called xyz::MsgHeaderMsgs, that
// can be used to both read and print xyz message types
void amc::Disp_CreateFromMsg() {
    // Generate a dispatch for each message header.
    ind_beg(amc::_db_typefld_curs, typefld, amc::_db) {
        tempstr key = tempstr() << ctype_Get(typefld) << "Msgs";
        amc::FDispatch *disp
            = amc::dispatch_InsertMaybe(dmmeta::Dispatch(key
                                                         , false // unk
                                                         , true // read
                                                         , true // print
                                                         , false // haslen
                                                         , false // call
                                                         , false // strict
                                                         , true // unkcount
                                                         , algo::Comment(typefld.p_ctype->comment)));

        bool kafka  = typefld.p_field->p_ctype->c_ckafka;
        disp->msgs  = true;
        disp->dyn   = kafka;
        disp->kafka = kafka;

        // loop over all messages that use this header...
        int nmsg=0;
        ind_beg(amc::_db_msgtype_curs, msgtype, amc::_db) {
            amc::FCtype *base = UltimateBaseType(msgtype.p_ctype,msgtype.p_ctype);
            if (base == typefld.p_ctype) {
                nmsg++;
                amc::dispatch_msg_InsertMaybe(dmmeta::DispatchMsg(tempstr()<<key<<"/"<<msgtype.ctype
                                                                  , algo::Comment(typefld.p_ctype->comment)));
            }
        }ind_end;
        if (nmsg==0) {
            prerr("amc.empty_typefld"
                  <<Keyval("typefld",typefld.field)
                  <<Keyval("comment","Expected to find at least 1 msgtype associated with typefld"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;

    // generate an fconst for each msgtype
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) if (ctype.c_msgtype) {
        amc::FCtype *base = UltimateBaseType(&ctype,&ctype);
        ind_beg(amc::ctype_c_field_curs, curfield, *base) if (curfield.c_typefld) {
            dmmeta::Fconst fconst;
            amc::FField &base_field = c_field_N(*curfield.p_arg) ? *c_field_Find(*curfield.p_arg,0) : curfield;
            fconst.fconst  = tempstr() << base_field.field << "/" << ctype.ctype;
            fconst.value   = ctype.c_msgtype->type;
            fconst.comment = algo::Comment(ctype.comment);
            amc::FFconst *ffconst = amc::fconst_InsertMaybe(fconst);
            if (ffconst) {
                ffconst->rec = tempstr() << "dmmeta.msgtype:" << ctype.ctype;
            }
        }ind_end;
    }ind_end;
}
