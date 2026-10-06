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
// Source: cpp/ainst/redpanda.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the redpanda broker from the vendor's apt repository.
void ainst::extpkg_redpanda(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"gpg --dearmor --yes --batch -o \"$(ainst_root /usr/share/keyrings/redpanda-redpanda-archive-keyring.gpg)\" < "<<GetExtpkgsrcVar(extpkg,"key"));
    AddStage(extpkg,"chmod 644 \"$(ainst_root /usr/share/keyrings/redpanda-redpanda-archive-keyring.gpg)\"");
    AddStageFile(extpkg,"/etc/apt/sources.list.d/redpanda-redpanda.list",
                 "deb [signed-by=/usr/share/keyrings/redpanda-redpanda-archive-keyring.gpg]"
                 " https://dl.redpanda.com/public/redpanda/deb/ubuntu noble main\n");
    AddPost(extpkg,"useradd rdpusr -u 1082 -d /opt/redpanda -U -m -s /bin/bash || true");
    AddPost(extpkg,"apt-get update -q");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold redpanda");
    AddPost(extpkg,"ln -sfn /home/rdpusr/.ssh /opt/redpanda/.ssh");
    AddPost(extpkg,"chown -hLR rdpusr:rdpusr /opt/redpanda");
}
