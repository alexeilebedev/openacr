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
// Source: cpp/ainst/grpcurl.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install grpcurl, a grpc client for the command line.
// The release is a deb carrying one binary, so it is unpacked rather than given
// to dpkg: the file lands under the staging root either way, and nothing here
// needs a package manager's database.
void ainst::extpkg_grpcurl(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,"extpkg=$(mktemp -d)");
    AddStage(extpkg,tempstr()<<"dpkg-deb -x "<<GetExtpkgsrcVar(extpkg,"deb")<<" \"$extpkg\"");
    AddStage(extpkg,"ainst_install 755 \"$extpkg/usr/bin/grpcurl\" /usr/local/bin/grpcurl");
}
