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
// Target: lib_ws (lib) -- WebSocket library
// Exceptions: yes
// Header: include/lib_ws.h
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

#include "include/gen/lib_ws_gen.h"
#include "include/gen/lib_ws_gen.inl.h"

#include "include/gen/ws_gen.h"
#include "include/gen/ws_gen.inl.h"

#include "include/gen/http_gen.h"
#include "include/gen/http_gen.inl.h"

namespace ws { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_ws/lib_ws.cpp
    //
    cstring &operator<<(cstring &lhs, const ws::FrameHeader &rhs);
    //     (user-implemented function, prototype is in amc-generated header)
    // u32 payload_N(const ws::Frame& parent); // field:ws.FrameMasked.payload
    // u32 payload_N(const ws::FrameMasked& parent); // field:ws.FrameMasked.payload
    ws::Frame *Frame_Castdown(ws::FrameHeader &hdr);
    ws::FrameMasked *FrameMasked_Castdown(ws::FrameHeader &hdr);
    bool FrameHeaderMsgs_ReadStrptrMaybe2(algo::strptr str, algo::ByteAry &buf);
    void FrameHeaderMsgs_Print2(algo::cstring &str, ws::FrameHeader &msg);
}

namespace lib_ws { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_ws/lib_ws.cpp
    //
    ws::FrameHeader *FrameHeader_FmtByteAry(algo::ByteAry &buf, bool fin, bool rsv1, bool rsv2, bool rsv3,  ws::Opcode opcode, bool mask, u32 masking_key, strptr payload);
    //     (user-implemented function, prototype is in amc-generated header)
    // void FrameLen_Frame16(i32 &len, ws::Frame16 &frame, u32 msg_len); // dispatch_msg:lib_ws.FrameLen/ws.Frame16
    // void FrameLen_FrameMasked16(i32 &len, ws::FrameMasked16 &frame, u32 msg_len); // dispatch_msg:lib_ws.FrameLen/ws.FrameMasked16
    // void FrameLen_Frame64(i32 &len, ws::Frame64 &frame, u32 msg_len); // dispatch_msg:lib_ws.FrameLen/ws.Frame64
    // void FrameLen_FrameMasked64(i32 &len, ws::FrameMasked64 &frame, u32 msg_len); // dispatch_msg:lib_ws.FrameLen/ws.FrameMasked64
    // int FrameLen_Unkmsg(i32 &len, ws::FrameHeader &hdr, u32 msg_len); // dispatch:lib_ws.FrameLen
    i32 GetMsgLen(algo::memptr buf);
    void ToggleMasking(strptr payload, u32 masking_key);
    //     (user-implemented function, prototype is in amc-generated header)
    // void FrameIdx_Frame16(lib_ws::FrameIdx &idx, ws::Frame16 &frame, u32); // dispatch_msg:lib_ws.FrameIdx/ws.Frame16
    // void FrameIdx_FrameMasked16(lib_ws::FrameIdx &idx, ws::FrameMasked16 &frame, u32); // dispatch_msg:lib_ws.FrameIdx/ws.FrameMasked16
    // void FrameIdx_Frame64(lib_ws::FrameIdx &idx, ws::Frame64 &frame, u32); // dispatch_msg:lib_ws.FrameIdx/ws.Frame64
    // void FrameIdx_FrameMasked64(lib_ws::FrameIdx &idx, ws::FrameMasked64 &frame, u32); // dispatch_msg:lib_ws.FrameIdx/ws.FrameMasked64
    // int FrameIdx_Unkmsg(lib_ws::FrameIdx &idx, ws::FrameHeader &hdr, u32); // dispatch:lib_ws.FrameIdx
    void ToggleMasking(ws::FrameHeader &frame, bool reset_key);
    strptr payload_Getary(ws::FrameHeader &frame);

    // Compute Sec-WebSocket-Accept from Sec-WebSocket-Key
    tempstr ComputeSecWebSocketAccept(strptr sec_websocket_key);
}
