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
    // void In_SampMengTextMsg(ams::SampMengTextMsg &); // dispatch_msg:samp_meng.In/ams.SampMengTextMsg

    // Create new symbol
    // void In_SampMengNewSymbolReqMsg(ams::SampMengNewSymbolReqMsg &msg); // dispatch_msg:samp_meng.In/ams.SampMengNewSymbolReqMsg

    // Create new user
    // void In_SampMengNewUserReqMsg(ams::SampMengNewUserReqMsg &msg); // dispatch_msg:samp_meng.In/ams.SampMengNewUserReqMsg

    // Add an order to the orderbook.
    // Publish any resulting trades; If order is IOC, cancel the remainder back,
    // otherwise post it to the book
    // void In_SampMengNewOrderReqMsg(ams::SampMengNewOrderReqMsg &msg); // dispatch_msg:samp_meng.In/ams.SampMengNewOrderReqMsg

    // Cancel a single order
    void CancelOrder(samp_meng::FOrder *order, bool notify);
    //     (user-implemented function, prototype is in amc-generated header)
    // void In_SampMengCancelReqMsg(ams::SampMengCancelReqMsg &msg); // dispatch_msg:samp_meng.In/ams.SampMengCancelReqMsg
    // void In_SampMengMassCancelReqMsg(ams::SampMengMassCancelReqMsg &msg); // dispatch_msg:samp_meng.In/ams.SampMengMassCancelReqMsg
    // void Main(); // main:samp_meng
}
