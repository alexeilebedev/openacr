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
// Source: cpp/ainst/nats.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the nats command line client.
// The release is a deb and it is unpacked rather than installed, because
// dpkg-deb is present in every image this runs in while unzip is not, and
// because the package would otherwise land the binary in /usr/local/bin.
void ainst::extpkg_nats(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,"extpkg=$(mktemp -d)");
    AddStage(extpkg,tempstr()<<"dpkg-deb -x "<<GetExtpkgsrcVar(extpkg,"deb")<<" \"$extpkg\"");
    AddStage(extpkg,"ainst_install 755 \"$extpkg/usr/local/bin/nats\" /usr/bin/nats");
}
