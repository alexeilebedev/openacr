// Copyright (C) 2026 AlgoX2 Corp
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
// Target: ainst (exe) -- Install third-party software described by dev.extpkg
// Exceptions: yes
// Source: cpp/ainst/rsync.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------
// Install rsync into $PREFIX, built from source.  The optional checksum,
// compression and OpenSSL libraries are left out, so the build needs none of
// their headers and comes out the same on every system.
void ainst::extpkg_rsync(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"src")<<")/rsync-3.4.4");
    AddStage(extpkg,"(cd \"$dir\" && ./configure --prefix=\"$PREFIX\" --disable-md2man --disable-xxhash --disable-zstd --disable-lz4 --disable-openssl"
             " && make -j\"$(ainst_nproc)\")");
    AddStage(extpkg,"(cd \"$dir\" && make install DESTDIR=\"$ROOT\")");
}
