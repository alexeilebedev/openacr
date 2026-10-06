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
// Source: cpp/ainst/ofed.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the mellanox ofed user-space stack from the vendor bundle.
// The bundle's own installer runs as root and writes across the live system, so
// it cannot be staged; what the sha256 buys is that the bundle is identified
// before any of it runs.
void ainst::extpkg_ofed(ainst::FExtpkg &extpkg) {
    AddPost(extpkg,"apt-get update");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold perl perl-modules");
    AddPost(extpkg,tempstr()<<"ofeddir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"tgz")<<")/MLNX_OFED_LINUX-24.10-1.1.4.0-ubuntu24.04-x86_64");
    AddPost(extpkg,"(cd \"$ofeddir\" && ./mlnxofedinstall --user-space-only --without-fw-update --without-mlnx-dpdk --all --xlio --force)");
}
