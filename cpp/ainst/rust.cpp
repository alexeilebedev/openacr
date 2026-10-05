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
// Source: cpp/ainst/rust.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the rust toolchain under /opt/rust, with cargo, rustc, rustfmt and
// clippy on the path.  Each release unpacks into its own versioned directory and
// a "current" symlink names the one in force, so an upgrade moves that symlink.
//
// The links in /usr/local/bin need no wrapper: a rust binary reads its own
// resolved path out of /proc/self/exe and finds its toolchain above it.
void ainst::extpkg_rust(ainst::FExtpkg &extpkg) {
    tempstr prefix;
    prefix << "/opt/rust/versions/" << GetExtpkgVersion(extpkg);
    tempstr top;
    top << "rust-" << GetExtpkgVersion(extpkg) << "-x86_64-unknown-linux-gnu";
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"rust")<<")");
    AddStage(extpkg,tempstr()<<"ainst_mkdir "<<prefix);
    AddStage(extpkg,tempstr()<<"\"$dir/"<<top<<"/install.sh\" --prefix="<<prefix
             <<" --destdir=\"$ROOT\" --disable-ldconfig"
             " --components=rustc,rust-std-x86_64-unknown-linux-gnu,cargo,rustfmt-preview,clippy-preview");
    AddStage(extpkg,tempstr()<<"ln -sfn "<<prefix<<" \"$(ainst_root /opt/rust/current)\"");
    strptr bin("cargo\nrustc\nrustdoc\nrustfmt\ncargo-fmt\ncargo-clippy\nclippy-driver");
    ind_beg(algo::Line_curs,name,bin) {
        AddStage(extpkg,tempstr()<<"ln -sfn /opt/rust/current/bin/"<<name
                 <<" \"$(ainst_root /usr/local/bin/"<<name<<")\"");
    }ind_end;
}
