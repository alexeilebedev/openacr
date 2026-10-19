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

// Install the nats command line client into $PREFIX/bin.
// The Linux release is a deb and it is unpacked rather than installed, because
// dpkg-deb is present in every image this runs in while unzip is not, and
// because the package would otherwise land the binary in /usr/local/bin.  The
// macOS release is a zip, which macOS unpacks itself.
void ainst::extpkg_nats(ainst::FExtpkg &extpkg) {
    tempstr rel(GetExtpkgsrcVar(extpkg,"rel"));
    AddStage(extpkg,"if [[ $DARWIN == Y ]]; then");
    // the archive's one directory is named after the release, which the
    // script does not spell, so a release bump stays one row of data
    AddStage(extpkg,tempstr()<<"    bin=$(echo \"$(ainst_unpack "<<rel<<")\"/nats-*-darwin-$(uname -m)/nats)");
    AddStage(extpkg,"else");
    AddStage(extpkg,"    extpkg=$(ainst_tmpdir)");
    AddStage(extpkg,tempstr()<<"    dpkg-deb -x "<<rel<<" \"$extpkg\"");
    AddStage(extpkg,"    bin=\"$extpkg/usr/local/bin/nats\"");
    AddStage(extpkg,"fi");
    AddStage(extpkg,"ainst_install 755 \"$bin\" \"$PREFIX/bin/nats\"");
}
