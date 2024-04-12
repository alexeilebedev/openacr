// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/disp/msg.cpp
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
                                                         , typefld.p_ctype->comment));
        (void)disp;
        // loop over all messages that use this header...
        int nmsg=0;
        ind_beg(amc::_db_msgtype_curs, msgtype, amc::_db) {
            amc::FCtype *base = UltimateBaseType(msgtype.p_ctype,msgtype.p_ctype);
            if (base == typefld.p_ctype) {
                nmsg++;
                amc::dispatch_msg_InsertMaybe(dmmeta::DispatchMsg(tempstr()<<key<<"/"<<msgtype.ctype
                                                                  , typefld.p_ctype->comment));
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
            fconst.comment = ctype.comment;
            amc::fconst_InsertMaybe(fconst);
        }ind_end;
    }ind_end;
}
