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
// The DOCA-OFED user space stack, and the apt repository that both it and
// ofed_kernel install from.

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Add NVIDIA's DOCA apt repository for the release that EXTPKG's dev.extpkgver row
// pins, keyed by the keyring EXTPKG's "key" source fetches and checks.
// The repository path carries the release, so the version reaches apt from the
// table and from nowhere else; ofed and ofed_kernel each pin it, and a host that
// installs both reads one repository when the two rows agree.  An aarch64 machine
// reads NVIDIA's arm64-sbsa tree, which publishes the same packages.
void ainst::AddDocaRepo(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"gpg --dearmor --yes --batch -o \"$(ainst_root /etc/apt/keyrings/doca.gpg)\" < "<<GetExtpkgsrcVar(extpkg,"key"));
    AddStage(extpkg,tempstr()<<"doca_version="<<strptr_ToBash(extpkg.c_extpkgver->version));
    AddStage(extpkg,"doca_arch=$([[ $(uname -m) == aarch64 ]] && echo arm64-sbsa || echo x86_64)");
    AddStage(extpkg,"ainst_write /etc/apt/sources.list.d/doca.list <<< \"deb [signed-by=/etc/apt/keyrings/doca.gpg] https://linux.mellanox.com/public/repo/doca/$doca_version/ubuntu24.04/$doca_arch/ ./\"");
}

// -----------------------------------------------------------------------------

// Install the DOCA-OFED user space stack and xlio from NVIDIA's apt repository:
// rdma-core, the verbs providers and tools, and libxlio.  Every image that talks
// to a Mellanox card takes this package, a container as well as a host, since it
// carries no kernel driver; a host that runs OFED's drivers adds ofed_kernel.
void ainst::extpkg_ofed(ainst::FExtpkg &extpkg) {
    AddDocaRepo(extpkg);
    AddPost(extpkg,"apt-get update -qq");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold doca-ofed-userspace libxlio");
}
