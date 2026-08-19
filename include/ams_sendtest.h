// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: ams_sendtest (exe) -- Algo Messaging System test tool
// Exceptions: yes
// Header: include/ams_sendtest.h
//

#include "include/algo.h"
#include "include/gen/ams_sendtest_gen.h"
#include "include/gen/ams_sendtest_gen.inl.h"

namespace ams_sendtest { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/ams_sendtest.cpp
    //
    lib_ams::FShm &GetOrCreateShm(ams::GrpId grp_id);

    // child reads parent messsage
    void ReadParentMsg(lib_ams::FShm &shm, ams::MsgHeader &msg);

    // Compose TEXT as a LogMsg and deliver it to every reader; TRUE when all of them
    // got it.  Delivery is all or nothing, because a reader that missed one message
    // of a numbered stream has a gap it cannot ask to have filled.
    //
    // Two independent choices decide the shape.  The message goes to one ring every
    // reader shares, or to a ring per reader; and it travels inline in those rings
    // or as a reference to a board slot.  Inline delivery to N rings is N copies of
    // the payload -- that is the cost the board exists to remove, and the only
    // arrangement in which the two paths can be told apart, since a shared ring is
    // one write however many readers consume it.
    bool SendText(algo::strptr text);
    void SendMsg();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:ams_sendtest
}
