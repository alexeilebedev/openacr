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
// Source: cpp/ainst/spdk.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the storage performance development kit, built from the release
// dev.extpkgver pins, so -update moves the clone with the version it records.
// This is the one installation that clones rather than fetching an artifact:
// spdk carries its dependencies as git submodules, and a release tarball from
// github carries none of them.  The build dependencies -- libaio-dev,
// libcunit1-dev, libnuma-dev, libssl-dev, uuid-dev, meson, python3-pyelftools
// -- are debs of the image that names this package, as they are for nginx and
// vma, so a host that builds it declares them the same way.  The clone and its
// build tree are removed once installed: they run to two gigabytes, and in an
// image build they would otherwise ride along in the layer.
void ainst::extpkg_spdk(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,"src=$(mktemp -d)");
    AddStage(extpkg,tempstr()<<"git clone --branch v"<<GetExtpkgVersion(extpkg)<<" --depth 1 https://github.com/spdk/spdk.git \"$src/spdk\"");
    AddStage(extpkg,"(cd \"$src/spdk\" && git submodule update --init)");
    AddStage(extpkg,"(cd \"$src/spdk\" && ./configure && make -j\"$(nproc)\")");
    AddStage(extpkg,"(cd \"$src/spdk\" && make install DESTDIR=\"$ROOT\")");
    AddStage(extpkg,"rm -rf \"$src\"");
}
