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
// Source: cpp/ainst/postgresql.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install the postgresql server and client, and the libpq headers the tree builds against.
// The auto-created "main" cluster is suppressed before the packages configure,
// because postgresql-common runs initdb from its own postinst otherwise.
// libpq-dev carries no version suffix: it tracks the distribution's libpq5, and
// cpp/ctxledger/pg.cpp includes a header that ships in neither versioned package.
void ainst::extpkg_postgresql(ainst::FExtpkg &extpkg) {
    AddStageFile(extpkg,"/etc/postgresql-common/createcluster.conf","create_main_cluster = false\n");
    AddPost(extpkg,"apt-get update");
    AddPost(extpkg,"DEBIAN_FRONTEND=noninteractive apt-get install -y -o Dpkg::Options::=--force-confdef"
            " -o Dpkg::Options::=--force-confold postgresql-16 postgresql-client-16 libpq-dev");
}
