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
// Source: cpp/ainst/vma.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install mellanox libvma, built from the release tag named in its artifact url.
// The script this replaced fetched refs/heads/master.zip, which names a
// different tree every day; 9.8.84 is the newest release upstream publishes and
// is newer than the OFED beside it.
void ainst::extpkg_vma(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"src")<<")/libvma-9.8.84");
    AddStage(extpkg,"(cd \"$dir\" && ./autogen.sh)");
    AddStage(extpkg,"(cd \"$dir\" && ./configure --with-ofed=/usr --prefix=/usr --libdir=/usr/lib"
             " --includedir=/usr/include --docdir=/usr/share/doc/libvma --sysconfdir=/etc)");
    AddStage(extpkg,"(cd \"$dir\" && make -j\"$(nproc)\")");
    AddStage(extpkg,"(cd \"$dir\" && make install DESTDIR=\"$ROOT\")");
    AddPost(extpkg,"ldconfig");
    AddPost(extpkg,"chmod u+s /usr/lib/libvma*");
    AddPost(extpkg,"chmod u+s /sbin/sysctl");
}
