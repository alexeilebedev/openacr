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
// Source: cpp/ainst/nodejs.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install node and pnpm from the nodesource apt repository.
// pnpm self-switches to the version each ts package's packageManager field
// names, so the global install only has to bootstrap the right major.
void ainst::extpkg_nodejs(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"gpg --dearmor --yes --batch -o \"$(ainst_root /etc/apt/keyrings/nodesource.gpg)\" < "<<GetExtpkgsrcVar(extpkg,"key"));
    AddStageFile(extpkg,"/etc/apt/sources.list.d/nodesource.list",
                 "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_24.x nodistro main\n");
    AddPost(extpkg,"apt-get update -q");
    AddPost(extpkg,"apt-get install -y -o Dpkg::Options::=--force-confold nodejs");
    AddPost(extpkg,"npm install -g pnpm@11 --prefix /usr/local");
}
