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
// Source: cpp/ainst/pnpm.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------
// Install pnpm into $PREFIX with the npm of the nodejs package.  pnpm switches
// itself to the version each ts package's packageManager field names, so this
// copy only bootstraps it.  npm writes outside any staging root, so the install
// runs as a post step on the target system.  An earlier install put pnpm under
// /usr/local on Linux too, where it would stay first on PATH and answer the
// probe with its old version, so on a host whose prefix is elsewhere that copy
// is removed.
void ainst::extpkg_pnpm(ainst::FExtpkg &extpkg) {
    AddPost(extpkg,tempstr()<<"npm install -g pnpm@"<<GetExtpkgVersion(extpkg)<<" --prefix \"$PREFIX\"");
    AddPost(extpkg,"if [[ $PREFIX != /usr/local ]]; then rm -rf /usr/local/bin/pnpm /usr/local/bin/pnpx /usr/local/lib/node_modules/pnpm; fi");
}
