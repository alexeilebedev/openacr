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
// Source: cpp/ainst/certbot.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Install certbot and its nginx plugin into a virtualenv of their own.
// A virtualenv records the absolute path it was created at in every script it
// writes, so it cannot be built in a buildroot and moved.  The whole
// installation is therefore a post step, and its rpm is that scriptlet.
void ainst::extpkg_certbot(ainst::FExtpkg &extpkg) {
    AddPost(extpkg,"python3 -m venv /opt/certbot/");
    AddPost(extpkg,"/opt/certbot/bin/pip install --upgrade pip");
    AddPost(extpkg,"/opt/certbot/bin/pip install certbot certbot-nginx");
    AddPost(extpkg,"ln -sfr /opt/certbot/bin/certbot /usr/bin/certbot");
}
