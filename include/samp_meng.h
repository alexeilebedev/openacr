// Copyright (C) 2024,2026 AlgoRND
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
// Target: samp_meng (exe) -- Sample matching engine
// Exceptions: yes
// Header: include/samp_meng.h -- Main header
//

#include "include/gen/samp_meng_gen.h"
#include "include/gen/samp_meng_gen.inl.h"

namespace samp_meng { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/samp_meng/samp_meng.cpp -- Main source
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void In_TextMsg(samp_meng::TextMsg &); // dispatch_msg:samp_meng.In/samp_meng.TextMsg

    // Create new symbol
    // void In_NewSymbolReqMsg(samp_meng::NewSymbolReqMsg &msg); // dispatch_msg:samp_meng.In/samp_meng.NewSymbolReqMsg

    // Create new user.
    // void In_NewUserReqMsg(samp_meng::NewUserReqMsg &msg); // dispatch_msg:samp_meng.In/samp_meng.NewUserReqMsg

    // Add an order to the orderbook.
    // Publish any resulting trades; If order is IOC, cancel the remainder back,
    // otherwise post it to the book
    // USER_ID: owning user
    // SYMBOL: symbol associated with order
    // PRICE: order price
    // QTY: signed quantity; positive=buy, negative=sell
    // IOC: if TRUE, order is cancelled immediately after matching
    // void In_NewOrderReqMsg(samp_meng::NewOrderReqMsg &msg); // dispatch_msg:samp_meng.In/samp_meng.NewOrderReqMsg

    // Cancel a single order
    void CancelOrder(samp_meng::FOrder *order, bool notify);

    // Cancel a single order
    //     (user-implemented function, prototype is in amc-generated header)
    // void In_CancelReqMsg(samp_meng::CancelReqMsg &msg); // dispatch_msg:samp_meng.In/samp_meng.CancelReqMsg

    // Cancel all orders for user
    // void In_MassCancelReqMsg(samp_meng::MassCancelReqMsg &msg); // dispatch_msg:samp_meng.In/samp_meng.MassCancelReqMsg
    // void cd_fdin_eof_Step(); // fstep:samp_meng.FDb.cd_fdin_eof

    // Read next input line from stdin
    // void cd_fdin_read_Step(); // fstep:samp_meng.FDb.cd_fdin_read
    // void Main(); // main:samp_meng
}
