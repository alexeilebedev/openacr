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
// Source: cpp/lib_ws/lib_ws.cpp
//
// This source code constitutes confidential information and trade secrets
// of AlgoRND. Unauthorized copying, distribution or sharing of this file,
// via any medium, is strictly prohibited.

#include "include/algo.h"
#include "include/lib_ws.h"
#include "include/sha.h"

cstring &ws::operator<<(cstring &lhs, const ws::FrameHeader &rhs) {
    ws::FrameHeaderMsgs_Print2(lhs,const_cast<ws::FrameHeader &>(rhs));
    return lhs;
}

// safe cast for cppcheck
template<typename T> ws::FrameHeader *CastbasePtr(T *ptr) {
    ws::FrameHeader *ret = NULL;
    if (ptr) {
        ret  = &Castbase(*ptr);
    }
    return ret;
}

u32 ws::payload_N(const ws::Frame& parent) {
    return payload_len_Get(parent);
}

u32 ws::payload_N(const ws::FrameMasked& parent) {
    return payload_len_Get(parent);
}

ws::FrameHeader *lib_ws::FrameHeader_FmtByteAry(algo::ByteAry &buf, bool fin, bool rsv1, bool rsv2, bool rsv3,  ws::Opcode opcode, bool mask, u32 masking_key, strptr payload) {
    ws::FrameHeader header;
    fin_Set(header,fin);
    rsv1_Set(header,rsv1);
    rsv2_Set(header,rsv2);
    rsv3_Set(header,rsv3);
    opcode_Set(header,opcode);
    ws::FrameHeader *frame(NULL);
    if (ch_N(payload) > UINT16_MAX) {
        if (mask) {
            frame = CastbasePtr(FrameMasked64_FmtByteAry(buf,header.byte0,masking_key,payload));
            ToggleMasking(*frame,false);
        } else {
            frame = CastbasePtr(Frame64_FmtByteAry(buf,header.byte0,payload));
        }
    } else if (ch_N(payload) > 125) {
        if (mask) {
            frame = CastbasePtr(FrameMasked16_FmtByteAry(buf,header.byte0,masking_key,payload));
            ToggleMasking(*frame,false);
        } else {
            frame = CastbasePtr(Frame16_FmtByteAry(buf,header.byte0,payload));
        }
    } else {
        mask_Set(header,mask);
        payload_len_Set(header,ch_N(payload));
        if (mask) {
            frame = CastbasePtr(FrameMasked_FmtByteAry(buf,header.byte0,header.byte1,masking_key,payload));
            ToggleMasking(*frame,false);
        } else {
            frame = CastbasePtr(Frame_FmtByteAry(buf,header.byte0,header.byte1,payload));
        }
    }
    return frame;
}

void lib_ws::FrameLen_Frame16(i32 &len, ws::Frame16 &frame, u32 msg_len) {
    len = GetMsgLength(frame);
    len = msg_len >= u32(len) ? len : 0;
}

void lib_ws::FrameLen_FrameMasked16(i32 &len, ws::FrameMasked16 &frame, u32 msg_len) {
    len = GetMsgLength(frame);
    len = msg_len >= u32(len) ? len : 0;
}

void lib_ws::FrameLen_Frame64(i32 &len, ws::Frame64 &frame, u32 msg_len) {
    len = GetMsgLength(frame);
    len = msg_len >= u32(len) ? len : 0;
}

void lib_ws::FrameLen_FrameMasked64(i32 &len, ws::FrameMasked64 &frame, u32 msg_len) {
    len = GetMsgLength(frame);
    len = msg_len >= u32(len) ? len : 0;
}

ws::Frame *ws::Frame_Castdown(ws::FrameHeader &hdr) {
    return !mask_Get(hdr) && payload_len_Get(hdr) <= 125 ? (ws::Frame*)&hdr : NULL;
}

ws::FrameMasked *ws::FrameMasked_Castdown(ws::FrameHeader &hdr) {
    return mask_Get(hdr) && payload_len_Get(hdr) <= 125 ? (ws::FrameMasked*)&hdr : NULL;
}

int lib_ws::FrameLen_Unkmsg(i32 &len, ws::FrameHeader &hdr, u32 msg_len) {
    ws::Frame *frame(NULL);
    ws::FrameMasked *frame_masked(NULL);
    u32 needlen = 0;
    if (msg_len >= sizeof(ws::Frame) && (frame = ws::Frame_Castdown(hdr))) {
        needlen = sizeof(ws::Frame) + payload_len_Get(*frame);
    } else if (msg_len >= sizeof(ws::FrameMasked) && (frame_masked = ws::FrameMasked_Castdown(hdr))) {
        needlen = sizeof(ws::FrameMasked) + payload_len_Get(*frame_masked);
    }
    len =  msg_len >= u32(needlen) ? needlen : 0;
    return len;
}

i32 lib_ws::GetMsgLen(algo::memptr buf) {
    ws::FrameHeader &hdr = *(ws::FrameHeader*)buf.elems;
    i32 len = 0;
    if (ch_N(buf) >= ssizeof(ws::FrameHeader)) {
        FrameLenDispatch(len,hdr,ch_N(buf));
    }
    return len;
}

bool ws::FrameHeaderMsgs_ReadStrptrMaybe2(algo::strptr str, algo::ByteAry &buf) {
    bool ok = ws::FrameHeaderMsgs_ReadStrptrMaybe(str, buf);
    if (!ok) {
        tempstr msgtype_str;
        algo::StringIter iter(str);
        cstring_ReadCmdarg(msgtype_str, iter, false);
        algo::ByteAry varlenbuf;
        algo::ByteAry *varlenbuf_save = algo_lib::_db.varlenbuf;
        algo_lib::_db.varlenbuf = &varlenbuf;
        if (msgtype_str == "ws.Frame") {
            int len = sizeof(ws::Frame);
            ws::Frame *ctype = new(ary_AllocN(buf, len).elems) ws::Frame; // default values
            ok = Frame_ReadStrptrMaybe(*ctype, str); // now read attributes
            varlenbuf.ary_n = varlenbuf.ary_n > 125 ? 125 : varlenbuf.ary_n;
            len += ary_N(varlenbuf);
            payload_len_Set(*ctype, u8(len-2));
            mask_Set(*ctype,false);
            ary_Addary(buf, ary_Getary(varlenbuf));
        } else if (msgtype_str == "ws.FrameMasked") {
            int len = sizeof(ws::FrameMasked);
            ws::FrameMasked *ctype = new(ary_AllocN(buf, len).elems) ws::FrameMasked; // default values
            ok = FrameMasked_ReadStrptrMaybe(*ctype, str); // now read attributes
            varlenbuf.ary_n = varlenbuf.ary_n > 125 ? 125 : varlenbuf.ary_n;
            len += ary_N(varlenbuf);
            payload_len_Set(*ctype, u8(len-6));
            mask_Set(*ctype,true);
            ary_Addary(buf, ary_Getary(varlenbuf));
        } else {
            FrameHeaderMsgs_ReadStrptrMaybe(str,buf);
        }
        algo_lib::_db.varlenbuf = varlenbuf_save;
    }
    return ok;
}

void ws::FrameHeaderMsgs_Print2(algo::cstring &str, ws::FrameHeader &msg) {
    if (!ws::FrameHeaderMsgs_Print(str,msg,UINT32_MAX)) {
        if (ws::Frame *frame = Frame_Castdown(msg)) {
            Frame_Print(*frame,str);
        } else if (ws::FrameMasked *masked = FrameMasked_Castdown(msg)) {
            FrameMasked_Print(*masked,str);
        }
    }
}

void lib_ws::ToggleMasking(strptr payload, u32 masking_key) {
    if (masking_key) {
        char *key_bytes = (char*)&masking_key;
        frep_(i,ch_N(payload)) {
            payload[i] ^= key_bytes[i%4];
        }
    }
}

void lib_ws::FrameIdx_Frame16(lib_ws::FrameIdx &idx, ws::Frame16 &frame, u32) {
    idx.masking_key = NULL;
    idx.payload = payload_Getary(frame);
}

void lib_ws::FrameIdx_FrameMasked16(lib_ws::FrameIdx &idx, ws::FrameMasked16 &frame, u32) {
    idx.masking_key = &frame.masking_key;
    idx.payload = payload_Getary(frame);
}

void lib_ws::FrameIdx_Frame64(lib_ws::FrameIdx &idx, ws::Frame64 &frame, u32) {
    idx.masking_key = NULL;
    idx.payload = payload_Getary(frame);
}

void lib_ws::FrameIdx_FrameMasked64(lib_ws::FrameIdx &idx, ws::FrameMasked64 &frame, u32) {
    idx.masking_key = &frame.masking_key;
    idx.payload = payload_Getary(frame);
}

int lib_ws::FrameIdx_Unkmsg(lib_ws::FrameIdx &idx, ws::FrameHeader &hdr, u32) {
    if (ws::Frame *frame = Frame_Castdown(hdr)) {
        idx.masking_key = NULL;
        idx.payload = payload_Getary(*frame);
    } else if (ws::FrameMasked *masked = FrameMasked_Castdown(hdr)) {
        idx.masking_key = &masked->masking_key;
        idx.payload = payload_Getary(*masked);
    }
    return 0;
}

void lib_ws::ToggleMasking(ws::FrameHeader &frame, bool reset_key) {
    FrameIdx idx;
    FrameIdxDispatch(idx,frame,UINT32_MAX);
    if (idx.masking_key) {
        ToggleMasking(idx.payload,*idx.masking_key);
        if (reset_key) {
            *idx.masking_key = 0;
        }
    }
}

strptr lib_ws::payload_Getary(ws::FrameHeader &frame) {
    FrameIdx idx;
    FrameIdxDispatch(idx,frame,UINT32_MAX);
    return idx.payload;
}

// Compute Sec-WebSocket-Accept from Sec-WebSocket-Key
tempstr lib_ws::ComputeSecWebSocketAccept(strptr sec_websocket_key) {
    tempstr out;
    Sha1Ctx sha1;
    Update(sha1,algo::strptr_ToMemptr(sec_websocket_key));
    Update(sha1,algo::strptr_ToMemptr("258EAFA5-E914-47DA-95CA-C5AB0DC85B11"));
    Finish(sha1);
    algo::Sha1sig sig = GetDigest(sha1);
    strptr_PrintBase64(algo::memptr_ToStrptr(sha1sig_Getary(sig)),out);
    return out;
}
