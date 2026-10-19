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
// Source: cpp/ainst/tla.cpp
//
// The TLA+ tools: SANY, the parser, and TLC, the model checker, in one jar.

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the TLA+ tools jar as /opt/tla/tla2tools-<version>.jar and point
// /opt/tla/tla2tools.jar at it, so atla finds the pinned release at one path
// and the probe tells one release from another.  The jar runs on the runtime of
// the java package.
void ainst::extpkg_tla(ainst::FExtpkg &extpkg) {
    tempstr jar;
    jar << "tla2tools-" << GetExtpkgVersion(extpkg) << ".jar";
    AddStage(extpkg,tempstr()<<"ainst_install 644 "<<GetExtpkgsrcVar(extpkg,"jar")<<" /opt/tla/"<<jar);
    AddStage(extpkg,tempstr()<<"ln -sfn "<<jar<<" \"$(ainst_root /opt/tla/tla2tools.jar)\"");
}
