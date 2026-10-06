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
// Source: cpp/ainst/linux_tools.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------

// Install cpupower and the other kernel tools for the running kernel.
// The generic metapackage linux-tools-generic tracks Ubuntu's newest generic
// kernel and moves linux-image-generic with it, so installing it on a host that
// runs another kernel -- a vendor's HWE kernel, say -- installs a second kernel,
// rebuilds every initramfs and dkms module, and makes the next boot run a kernel
// nobody chose.  The tools of the kernel that is running are the ones cpupower
// execs, so that package is the one installed.
void ainst::extpkg_linux_tools(ainst::FExtpkg &extpkg) {
    AddPost(extpkg,"apt-get update -qq");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold linux-tools-common \"linux-tools-$(uname -r)\"");
}
