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
// Target: amsspy (exe) -- List ams sessions and monitor traffic on host
// Exceptions: yes
// Header: include/amsspy.h
//

#include "include/gen/amsspy_gen.h"
#include "include/gen/amsspy_gen.inl.h"

namespace amsspy { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/amsspy/amsspy.cpp
    //

    // Inline step: called each main-loop iteration.
    // Uses RotateFirst for round-robin polling across all watched segments.
    //     (user-implemented function, prototype is in amc-generated header)
    // void cd_shm_poll_Step(); // dmmeta.fstep:amsspy.FDb.cd_shm_poll
    // void Main(); // dmmeta.main:amsspy
}
