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
// Source: cpp/ainst/go.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the go toolchain under /opt/go, with go and gofmt on the path.  Each
// release unpacks into its own versioned directory and a "current" symlink names
// the one in force, so an upgrade moves that symlink.
//
// The links in /usr/local/bin need no GOROOT: go resolves the symlinks above its
// own binary and reads the root off the path it lands on.
void ainst::extpkg_go(ainst::FExtpkg &extpkg) {
    tempstr prefix;
    prefix << "/opt/go/versions/" << GetExtpkgVersion(extpkg);
    AddStage(extpkg,tempstr()<<"dir=$(ainst_unpack "<<GetExtpkgsrcVar(extpkg,"tgz")<<")");
    AddStage(extpkg,tempstr()<<"ainst_mkdir "<<prefix);
    AddStage(extpkg,tempstr()<<"cp -r \"$dir/go/.\" \"$(ainst_root "<<prefix<<")/\"");
    AddStage(extpkg,tempstr()<<"chmod -R a+rX \"$(ainst_root "<<prefix<<")\"");
    AddStage(extpkg,tempstr()<<"ln -sfn "<<prefix<<" \"$(ainst_root /opt/go/current)\"");
    AddStage(extpkg,"ln -sfn /opt/go/current/bin/go \"$(ainst_root /usr/local/bin/go)\"");
    AddStage(extpkg,"ln -sfn /opt/go/current/bin/gofmt \"$(ainst_root /usr/local/bin/gofmt)\"");
}
