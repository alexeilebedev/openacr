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
// Install node, npm and npx under /opt/node from the nodejs.org release, point
// /opt/node/current at it, and link each command into $PREFIX/bin.  The release
// tarball is the same build on Linux and macOS, so both run one version.  The
// post step removes a Debian nodejs and nodesource's apt line and keyring from the
// system it installs onto: the deb owns /usr/bin/node, and an upgrade of it, or
// an apt install that pulls nodejs in, would put its own node back over the link.
// Purging the deb deletes the paths it owned, so a purge links the commands again.
void ainst::extpkg_nodejs(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(echo $(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"tgz")<<")/node-v24.21.0-*)");
    AddStage(extpkg,"ainst_mkdir /opt/node");
    AddStage(extpkg,"rm -rf \"$(ainst_root /opt/node/node-v24.21.0)\"");
    AddStage(extpkg,"cp -R \"$dir\" \"$(ainst_root /opt/node/node-v24.21.0)\"");
    AddStage(extpkg,"chmod -R a+rX \"$(ainst_root /opt/node/node-v24.21.0)\"");
    AddStage(extpkg,"ln -sfn /opt/node/node-v24.21.0 \"$(ainst_root /opt/node/current)\"");
    AddStage(extpkg,"ainst_mkdir \"$PREFIX/bin\"");
    AddStage(extpkg,"for cmd in node npm npx; do ln -sfn /opt/node/current/bin/$cmd \"$(ainst_root \"$PREFIX/bin/$cmd\")\"; done");
    AddPost(extpkg,"if [[ $DARWIN == N ]]; then rm -f /etc/apt/sources.list.d/nodesource.list /etc/apt/keyrings/nodesource.gpg; fi");
    AddPost(extpkg,"if [[ $DARWIN == N ]] && dpkg -s nodejs >/dev/null 2>&1; then");
    AddPost(extpkg,"    apt-get remove --purge -y nodejs");
    AddPost(extpkg,"    for cmd in node npm npx; do ln -sfn /opt/node/current/bin/$cmd \"$PREFIX/bin/$cmd\"; done");
    AddPost(extpkg,"fi");
}
