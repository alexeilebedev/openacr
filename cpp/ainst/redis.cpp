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
// Source: cpp/ainst/redis.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------
// Install redis-cli and redis-benchmark into $PREFIX/bin, built from source with
// TLS.  The server is left out.
void ainst::extpkg_redis(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"src")<<")/redis-7.0.15");
    AddStage(extpkg,"opt=BUILD_TLS=yes");
    AddStage(extpkg,"if [[ $DARWIN == Y ]]; then opt=\"$opt OPENSSL_PREFIX=/opt/homebrew/opt/openssl@3\"; fi");
    AddStage(extpkg,"(cd \"$dir\" && make -C src $opt -j\"$(ainst_nproc)\" redis-cli redis-benchmark)");
    AddStage(extpkg,"ainst_mkdir \"$PREFIX/bin\"");
    AddStage(extpkg,"for cmd in redis-cli redis-benchmark; do ainst_install 755 \"$dir/src/$cmd\" \"$PREFIX/bin/$cmd\"; done");
}
