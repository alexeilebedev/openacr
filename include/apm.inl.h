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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Header: include/apm.inl.h
//

#pragma once

// Return the origin of PACKAGE's destination: the URL it was installed from, or "." for a
// package that has no dev.pkgupstream row and so lives in this tree.
inline algo::strptr apm::GetOrigin(apm::FPackage &package) {
    apm::FPkgupstream *pkgupstream = GetPkgupstream(package);
    return pkgupstream ? algo::strptr(pkgupstream->origin) : algo::strptr(".");
}

// Return the baseref of PACKAGE's destination: the commit of its origin this tree last merged
// or published, or HEAD for a package that lives in this tree.
inline algo::strptr apm::GetBaseref(apm::FPackage &package) {
    apm::FPkgupstream *pkgupstream = GetPkgupstream(package);
    return pkgupstream ? algo::strptr(pkgupstream->baseref) : algo::strptr("HEAD");
}
