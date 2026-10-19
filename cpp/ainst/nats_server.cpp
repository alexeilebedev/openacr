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
// Source: cpp/ainst/nats_server.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------

// Install the nats-server broker into $PREFIX/bin from the upstream release:
// a deb on Linux, a tarball on macOS.  The deb is unpacked rather than
// installed, because dpkg-deb is present in every image this runs in while
// unzip is not, and because the package also carries a /usr/local/bin symlink
// of the same binary.
// natsusr owns the running broker: omcli reaches the node over ssh as that
// account, and nats-server keeps its JetStream store under that home.
void ainst::extpkg_nats_server(ainst::FExtpkg &extpkg) {
    tempstr rel(GetExtpkgsrcVar(extpkg,"rel"));
    AddStage(extpkg,"if [[ $DARWIN == Y ]]; then");
    // the archive's one directory is named after the release, which the
    // script does not spell, so a release bump stays one row of data
    AddStage(extpkg,tempstr()<<"    bin=$(echo \"$(ainst_unpack "<<rel<<")\"/nats-server-*-darwin-$(uname -m)/nats-server)");
    AddStage(extpkg,"else");
    AddStage(extpkg,"    extpkg=$(ainst_tmpdir)");
    AddStage(extpkg,tempstr()<<"    dpkg-deb -x "<<rel<<" \"$extpkg\"");
    AddStage(extpkg,"    bin=\"$extpkg/usr/bin/nats-server\"");
    AddStage(extpkg,"fi");
    AddStage(extpkg,"ainst_install 755 \"$bin\" \"$PREFIX/bin/nats-server\"");
    // the broker's account is a Linux host's; a Mac runs the server only in tests
    AddPost(extpkg,"if [[ $DARWIN == N ]]; then useradd natsusr -u 1102 -U -m -s /bin/bash || true; fi");
}
