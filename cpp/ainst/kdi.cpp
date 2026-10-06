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
// Source: cpp/ainst/kdi.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install kafka-delta-ingest as /usr/bin/kdi, built from the commit its artifact names.
// The project publishes no tag and no release, so a commit is the only pin
// available and it is a stronger one than a tag, which upstream can move.
// The rust toolchain is fetched, used, and removed again: nothing downstream of
// this installation wants cargo on the machine.
void ainst::extpkg_kdi(ainst::FExtpkg &extpkg) {
    AddStage(extpkg,"src=$(mktemp -d)");
    AddStage(extpkg,tempstr()<<"cp "<<GetExtpkgsrcVar(extpkg,"rustup")<<" \"$src/rustup-init\"");
    AddStage(extpkg,"chmod +x \"$src/rustup-init\"");
    AddStage(extpkg,"CARGO_HOME=\"$src/cargo\" RUSTUP_HOME=\"$src/rustup\" \"$src/rustup-init\" -y --no-modify-path");
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"src")<<")/kafka-delta-ingest-c6d42def1f65badaac417203591d2d47daf5f0d8");
    AddStage(extpkg,"(cd \"$dir\" && CARGO_HOME=\"$src/cargo\" RUSTUP_HOME=\"$src/rustup\""
             " \"$src/cargo/bin/cargo\" build --release --features s3)");
    AddStage(extpkg,"strip \"$dir/target/release/kafka-delta-ingest\" 2>/dev/null || true");
    AddStage(extpkg,"ainst_install 755 \"$dir/target/release/kafka-delta-ingest\" /usr/bin/kdi");
}
