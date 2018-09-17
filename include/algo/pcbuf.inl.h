// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/pcbuf.inl.h
//

inline void algo::EndWrite(PCBuf &F) {
    sfence();
    F.write_ptr = F.write_next;
}

inline void algo::Write(PCBuf &F, memptr bytes) {
    u8 *v = BeginWrite(F, bytes.n_elems);
    memcpy(v, bytes.elems, bytes.n_elems);
    EndWrite(F);
}

inline void algo::EndRead(PCBuf &F) {
    F.read_ptr = F.read_next;
}

inline bool algo::AllReadQ(PCBuf &F) {
    return F.read_ptr == F.write_ptr;
}

inline algo::PCPBuf::PCPBuf() {
    write_ptr=0;
}

inline algo::PCPLine &algo::Access(algo::PCPBuf &B, u32 idx) {
    return ((PCPLine*)B.backing.elems)[idx];
}
