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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/lib_ws.cpp
//
// Unit tests for lib_ws. Exercise frame encode/decode at the three
// payload-length encodings RFC 6455 §5.2 defines:
// payload_len  in  [0..125]    -> 7-bit length in byte1
// payload_len  in  [126..65535] -> byte1=126(unmasked)/254(masked), u16 BE
// payload_len  >=  65536       -> byte1=127(unmasked)/255(masked), u64 BE
// The existing component test (x2.NatsWs) only covers the small path,
// so a regression in the u16/u64 dispatch would go undetected.

#include "include/algo.h"
#include "include/atf_unit.h"
#include "include/lib_ws.h"

// Build a payload of N bytes with a deterministic, easy-to-eyeball pattern.
static algo::tempstr MakePayload(u32 n) {
    algo::tempstr out;
    u32 i = 0;
    while (i < n) {
        out << char('A' + (i % 26));
        i += 1;
    }
    return out;
}

// Compare two byte ranges of equal length without using memcmp (keeps SESE simple).
static bool BytesEq(algo::strptr a, algo::strptr b) {
    bool eq = a.n_elems == b.n_elems;
    i64 i = 0;
    while (eq && i < a.n_elems) {
        eq = a.elems[i] == b.elems[i];
        i += 1;
    }
    return eq;
}

// Roundtrip small frame (payload <= 125): byte1's low 7 bits hold the length
// directly. Covers both unmasked (byte1 < 0x80) and masked (byte1 >= 0x80).
void atf_unit::unittest_lib_ws_SmallFrame() {
    const u32 sizes[] = {0, 1, 125};
    u32 si = 0;
    while (si < 3) {
        u32 n = sizes[si];
        algo::tempstr payload = MakePayload(n);
        // unmasked
        {
            algo::ByteAry buf;
            ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                    buf, true, false, false, false, ws::Opcode(ws_Opcode_binary),
                                                                    false, 0, payload);
            vrfy_(frame != NULL);
            vrfy_(buf.ary_n == u64(sizeof(ws::Frame)) + n);
            vrfy_(buf.ary_elems[1] == n); // mask bit clear, length direct
            vrfy_(lib_ws::GetMsgLen(algo::memptr(buf.ary_elems, i64(buf.ary_n))) == i32(buf.ary_n));
            strptr got = lib_ws::payload_Getary(*frame);
            vrfy_(BytesEq(got, payload));
        }
        // masked
        {
            algo::ByteAry buf;
            ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                    buf, true, false, false, false, ws::Opcode(ws_Opcode_binary),
                                                                    true, 0xdeadbeef, payload);
            vrfy_(frame != NULL);
            vrfy_(buf.ary_n == u64(sizeof(ws::FrameMasked)) + n);
            vrfy_(buf.ary_elems[1] == (n | 0x80)); // mask bit set
            vrfy_(lib_ws::GetMsgLen(algo::memptr(buf.ary_elems, i64(buf.ary_n))) == i32(buf.ary_n));
            // Payload bytes on the wire are masked; after toggling, must equal plaintext.
            lib_ws::ToggleMasking(*frame, false);
            strptr got = lib_ws::payload_Getary(*frame);
            vrfy_(BytesEq(got, payload));
        }
        si += 1;
    }
}

// Roundtrip Frame16 / FrameMasked16 (payload in [126..65535]):
// byte1 == 126 (unmasked) or 254 (masked), followed by u16 big-endian length.
void atf_unit::unittest_lib_ws_Frame16() {
    const u32 sizes[] = {126, 1000, 65535};
    u32 si = 0;
    while (si < 3) {
        u32 n = sizes[si];
        algo::tempstr payload = MakePayload(n);
        // unmasked
        {
            algo::ByteAry buf;
            ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                    buf, true, false, false, false, ws::Opcode(ws_Opcode_binary),
                                                                    false, 0, payload);
            vrfy_(frame != NULL);
            vrfy_(buf.ary_n == u64(sizeof(ws::Frame16)) + n);
            vrfy_(buf.ary_elems[1] == 126);
            u32 be = (u32(buf.ary_elems[2]) << 8) | u32(buf.ary_elems[3]);
            vrfy_(be == n);
            vrfy_(lib_ws::GetMsgLen(algo::memptr(buf.ary_elems, i64(buf.ary_n))) == i32(buf.ary_n));
            strptr got = lib_ws::payload_Getary(*frame);
            vrfy_(BytesEq(got, payload));
        }
        // masked
        {
            algo::ByteAry buf;
            ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                    buf, true, false, false, false, ws::Opcode(ws_Opcode_binary),
                                                                    true, 0xcafef00d, payload);
            vrfy_(frame != NULL);
            vrfy_(buf.ary_n == u64(sizeof(ws::FrameMasked16)) + n);
            vrfy_(buf.ary_elems[1] == 254);
            u32 be = (u32(buf.ary_elems[2]) << 8) | u32(buf.ary_elems[3]);
            vrfy_(be == n);
            vrfy_(lib_ws::GetMsgLen(algo::memptr(buf.ary_elems, i64(buf.ary_n))) == i32(buf.ary_n));
            lib_ws::ToggleMasking(*frame, false);
            strptr got = lib_ws::payload_Getary(*frame);
            vrfy_(BytesEq(got, payload));
        }
        si += 1;
    }
}

// Roundtrip Frame64 / FrameMasked64 (payload >= 65536):
// byte1 == 127 (unmasked) or 255 (masked), followed by u64 big-endian length.
void atf_unit::unittest_lib_ws_Frame64() {
    const u32 sizes[] = {65536, 100000};
    u32 si = 0;
    while (si < 2) {
        u32 n = sizes[si];
        algo::tempstr payload = MakePayload(n);
        // unmasked
        {
            algo::ByteAry buf;
            ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                    buf, true, false, false, false, ws::Opcode(ws_Opcode_binary),
                                                                    false, 0, payload);
            vrfy_(frame != NULL);
            vrfy_(buf.ary_n == u64(sizeof(ws::Frame64)) + n);
            vrfy_(buf.ary_elems[1] == 127);
            u64 be = 0;
            u32 k = 0;
            while (k < 8) {
                be = (be << 8) | u64(buf.ary_elems[2 + k]);
                k += 1;
            }
            vrfy_(be == u64(n));
            vrfy_(lib_ws::GetMsgLen(algo::memptr(buf.ary_elems, i64(buf.ary_n))) == i32(buf.ary_n));
            strptr got = lib_ws::payload_Getary(*frame);
            vrfy_(BytesEq(got, payload));
        }
        // masked
        {
            algo::ByteAry buf;
            ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                    buf, true, false, false, false, ws::Opcode(ws_Opcode_binary),
                                                                    true, 0x12345678, payload);
            vrfy_(frame != NULL);
            vrfy_(buf.ary_n == u64(sizeof(ws::FrameMasked64)) + n);
            vrfy_(buf.ary_elems[1] == 255);
            u64 be = 0;
            u32 k = 0;
            while (k < 8) {
                be = (be << 8) | u64(buf.ary_elems[2 + k]);
                k += 1;
            }
            vrfy_(be == u64(n));
            vrfy_(lib_ws::GetMsgLen(algo::memptr(buf.ary_elems, i64(buf.ary_n))) == i32(buf.ary_n));
            lib_ws::ToggleMasking(*frame, false);
            strptr got = lib_ws::payload_Getary(*frame);
            vrfy_(BytesEq(got, payload));
        }
        si += 1;
    }
}

// Confirm rsv1/rsv2/rsv3 setters land in byte0 at the documented bit positions
// (RFC 6455 §5.2: RSV1=bit6, RSV2=bit5, RSV3=bit4). The x2gw parser uses these
// getters to reject extensions we don't support; this keeps the encoding stable.
void atf_unit::unittest_lib_ws_RsvBits() {
    const bool flags[3][3] = {
        {true,  false, false},  // RSV1 -> 0x40
        {false, true,  false},  // RSV2 -> 0x20
        {false, false, true},   // RSV3 -> 0x10
    };
    const u8 expect_byte0[3] = {0x40 | u8(ws_Opcode_binary),
                                0x20 | u8(ws_Opcode_binary),
                                0x10 | u8(ws_Opcode_binary)};
    u32 i = 0;
    while (i < 3) {
        algo::tempstr payload = MakePayload(4);
        algo::ByteAry buf;
        ws::FrameHeader *frame = lib_ws::FrameHeader_FmtByteAry(
                                                                buf, false, flags[i][0], flags[i][1], flags[i][2],
                                                                ws::Opcode(ws_Opcode_binary), false, 0, payload);
        vrfy_(frame != NULL);
        vrfy_(buf.ary_elems[0] == expect_byte0[i]);
        vrfy_(rsv1_Get(*frame) == flags[i][0]);
        vrfy_(rsv2_Get(*frame) == flags[i][1]);
        vrfy_(rsv3_Get(*frame) == flags[i][2]);
        i += 1;
    }
}
