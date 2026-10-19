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
// Source: cpp/ainst/mosquitto.cpp
//

#include "include/algo.h"
#include "include/ainst.h"


// -----------------------------------------------------------------------------
// Install the mosquitto command-line clients into $PREFIX/bin, built from source
// with TLS and linked against a static libmosquitto, so they need no library of
// their own at run time.  The broker is left out.
void ainst::extpkg_mosquitto(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"src")<<")/mosquitto-2.0.18");
    AddStage(extpkg,"opt=\"WITH_CJSON=no WITH_DOCS=no WITH_SHARED_LIBRARIES=no WITH_STATIC_LIBRARIES=yes\"");
    AddStage(extpkg,"(cd \"$dir\" && make -C lib $opt -j\"$(ainst_nproc)\" && make -C client $opt -j\"$(ainst_nproc)\")");
    AddStage(extpkg,"ainst_mkdir \"$PREFIX/bin\"");
    AddStage(extpkg,"for cmd in mosquitto_pub mosquitto_sub mosquitto_rr; do ainst_install 755 \"$dir/client/$cmd\" \"$PREFIX/bin/$cmd\"; done");
}
