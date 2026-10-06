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
// Source: cpp/ainst/chrome.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install google chrome from the vendor apt repository.
// The distro's own chromium and firefox on 24.04 are stub packages that
// pre-depend on snapd and install a snap, which a container has no daemon to
// run, so neither is a browser anywhere this image is used.  Chrome ships a
// real deb, and headless is a flag on it rather than a separate build.
void ainst::extpkg_chrome(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"gpg --dearmor --yes --batch -o \"$(ainst_root /etc/apt/keyrings/google-chrome.gpg)\" < "<<GetExtpkgsrcVar(extpkg,"key"));
    AddStageFile(extpkg,"/etc/apt/sources.list.d/google-chrome.list",
                 "deb [arch=amd64 signed-by=/etc/apt/keyrings/google-chrome.gpg] https://dl.google.com/linux/chrome/deb/ stable main\n");
    AddPost(extpkg,"apt-get update -q");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold google-chrome-stable");
}
