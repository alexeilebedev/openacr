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
// Source: cpp/ainst/tsc_freq_khz.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the tsc_freq_khz kernel module, which exports the kernel's boot-calibrated TSC rate.
// Stock kernels never expose tsc_khz, and an EC2 instance has no cpufreq sysfs
// either, so without this module userspace has no authoritative rate to
// calibrate against and algo::CpuHz falls back to guessing.
// The source is vendored verbatim from the archived trailofbits/tsc_freq_khz
// (GPL-2.0, commit f71e4ad, 2019-09-24).  dkms rebuilds it for every future
// kernel and modules-load.d loads it at boot.
// The Makefile recipes use the one-line "target: ; command" form, which make
// accepts without the leading TAB a conventional recipe needs -- a TAB would
// not survive this repo's indent pass.
void ainst::extpkg_tsc_freq_khz(ainst::FExtpkg &extpkg) {
    AddStageFileFrom(extpkg,"/usr/src/tsc_freq_khz-0.1/tsc_freq_khz.c","conf/ainst/tsc_freq_khz/tsc_freq_khz.c");
    AddStageFileFrom(extpkg,"/usr/src/tsc_freq_khz-0.1/Makefile","conf/ainst/tsc_freq_khz/Makefile");
    AddStageFile(extpkg,"/usr/src/tsc_freq_khz-0.1/dkms.conf",
                 "PACKAGE_NAME=\"tsc_freq_khz\"\n"
                 "PACKAGE_VERSION=\"0.1\"\n"
                 "BUILT_MODULE_NAME[0]=\"tsc_freq_khz\"\n"
                 "DEST_MODULE_LOCATION[0]=\"/updates/dkms\"\n"
                 "MAKE[0]=\"make KDIR=/lib/modules/${kernelver}/build\"\n"
                 "CLEAN=\"make KDIR=/lib/modules/${kernelver}/build clean\"\n"
                 "AUTOINSTALL=\"yes\"\n");
    AddStageFile(extpkg,"/etc/modules-load.d/tsc_freq_khz.conf","tsc_freq_khz\n");
    AddPost(extpkg,"apt-get update -qq");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold dkms \"linux-headers-$(uname -r)\"");
    AddPost(extpkg,"dkms status tsc_freq_khz/0.1 | grep -q tsc_freq_khz || dkms add -m tsc_freq_khz -v 0.1");
    AddPost(extpkg,"dkms install -m tsc_freq_khz -v 0.1");
    AddPost(extpkg,"modprobe tsc_freq_khz");
}
