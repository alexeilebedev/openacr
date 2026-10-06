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
// Source: cpp/ainst/buildkit.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the buildkit daemon and its client, and the unit that runs the daemon.
// buildkit is versioned by moby and nerdctl by containerd, so they are two
// packages: one package carries one version, and a package holding both could
// be moved to a new release of neither.
// The unit wants containerd and is PartOf it.  A containerd restart, which the
// kubelet routine performs, then restarts buildkitd with it.  On a rebooted host
// containerd's first start loses a race with loopback and the retry five
// seconds later succeeds.  With Wants that first failure leaves buildkit's own
// start job to run; Requires would fail it and leave buildkit stopped.
void ainst::extpkg_buildkit(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"tgz")<<")");
    AddStage(extpkg,"ainst_install 755 \"$dir/bin/buildkitd\" /usr/local/bin/buildkitd");
    AddStage(extpkg,"ainst_install 755 \"$dir/bin/buildctl\" /usr/local/bin/buildctl");
    AddStageFileFrom(extpkg,"/etc/systemd/system/buildkit.service","conf/ainst/buildkit/buildkit.service");
    AddPost(extpkg,"systemctl daemon-reload");
    AddPost(extpkg,"systemctl enable --now buildkit");
}
