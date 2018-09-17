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
// Source: cpp/lib/algo/rand.cpp
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/algo.h"

// This is a low-quality random number generator suitable for simple tasks...
// Set seed for srng state
void algo_lib::srng_SetSeed(algo_lib::Srng &srng, int z, int w) {
    srng.z=z + (z==0);
    srng.w=w + (w==0);
}

// retrieve random u32
u32 algo_lib::srng_GetU32(algo_lib::Srng &srng) {
    srng.z = 36969 * (srng.z & 65535) + (srng.z >> 16);
    srng.w = 18000 * (srng.w & 65535) + (srng.w >> 16);
    return (srng.z << 16) + srng.w;
}

// retrieve random double in 0..1 range
double algo_lib::srng_GetDouble(algo_lib::Srng &srng) {
    double norm = 2.328306435454494e-10; // 1/(2^32 + 2)
    return (srng_GetU32(srng) + 1.0) * norm;
}
