// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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

    // TRUE when every lane the next message goes to has ring room for it.  Asked
    // before the message is built, so a send refused for lack of room costs neither
    // the padding nor the format; in board mode the lane needs room for a reference,
    // and whether it may reference the chunk is known only once the message has a
    // chunk, so BoardSendAll asks that half.
    bool RoomQ();

    // TRUE when the next message is refused by its owner's channel limit alone:
    // every lane it goes to has ring room for it, and the owner's channel has no room
    // for it under the write limit.  A send the ring refuses is not the limit's.
    bool LimitRefusedQ();

    // Send MSG, formatted into the board, to every lane the run delivers on; TRUE
    // when every lane took a reference.  Every lane is asked for room first,
    // because a reader that missed one message of a numbered stream has a gap it
    // cannot ask to have filled, so a message reaches every lane or none.  A
    // refused message stays in the board for the caller to give back.
    bool BoardSendAll(ams::MsgHeader &msg);

    // Compose TEXT as a LogMsg and deliver it to every reader; TRUE when all of them
    // got it.  Delivery is all or nothing, because a reader that missed one message
    // of a numbered stream has a gap it cannot ask to have filled; RoomQ has said
    // the lanes have room, so a refusal here is a race with a reader, or the board
    // out of chunks, and is retried.
    //
    // Two independent choices decide the shape.  The message goes to one ring every
    // reader shares, or to a ring per reader; and it travels inline in those rings
    // or as a reference to a payload formatted once into the board.  Inline
    // delivery to N rings is N copies of the payload -- that is the cost the board
    // exists to remove, and the only arrangement in which the two paths can be told
    // apart, since a shared ring is one write however many readers consume it.
    bool SendText(algo::strptr text);

    // Return the child the next message belongs to in channel mode: message n goes
    // to child n % nchild + 1, whose channel paces it.
    ams_sendtest::FChild &MsgChild();

    // Return the channel the next message goes on in the lane of CHILD, in channel
    // mode: the channel of the child it belongs to on that child's lane, and the
    // lane's base channel on any other lane, since a message belongs to one channel.
    lib_ams::FChannel &LaneChannel(ams_sendtest::FChild &child);

    // Park the writer on every target the next message has no room on, and return
    // true when one of them had room after all, so the next pass should try again.
    // The targets are the ones RoomQ asks: the message's channel on each lane in
    // channel mode, each child's lane with one lane per reader, and the shared lane
    // otherwise.  A park raises the ring's writer_sleeping flag and re-checks under a
    // barrier, so a reader freeing room either sees the flag and signals, or the
    // re-check sees the room.
    bool ParkRefused();

    // Send messages until one is refused, in park mode.  A pass that sent something,
    // or is still waiting for the children to start, keeps the loop awake.  A pass
    // refused for room parks the writer on what refused it and lets the loop sleep,
    // so the next pass runs only when a reader's wake signal arrives: a wake that is
    // never sent leaves the parent asleep until its time limit, and the run fails.
    //     (user-implemented function, prototype is in amc-generated header)
    // void send_Step(); // dmmeta.fstep:ams_sendtest.FDb.send
    void SendMsg();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:ams_sendtest
}
