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
// Source: cpp/ainst/ofed_kernel.cpp
//
// The DOCA-OFED kernel drivers, which a host runs in place of the kernel's own mlx5.

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the DOCA-OFED kernel drivers on a host: mlx5, the RDMA core, srp, iser,
// xpmem and the mft driver, built by DKMS for every kernel whose headers the host
// holds, together with the user space they are released with.
// A container never takes this package, since it runs on its host's kernel.  A
// host with a Mellanox HCA runs OFED's drivers rather than the kernel's own, and
// a kernel the drivers do not build for fails this install, so a host's kernel
// moves only to a release whose DOCA-OFED builds for it.
void ainst::extpkg_ofed_kernel(ainst::FExtpkg &extpkg) {
    AddDocaRepo(extpkg);
    AddPost(extpkg,"apt-get update -qq");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold doca-ofed");
    AddPost(extpkg,"dkms status mlnx-ofed-kernel -k \"$(uname -r)\" | grep -q installed");
}
