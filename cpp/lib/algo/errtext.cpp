// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/errtext.cpp
//

#include "include/algo.h"

// Reset value of algo_lib::_db.errtext and return it for further editing
// Usage:
// algo_lib::ResetBadTags() << ...errors...
algo::cstring &algo_lib::ResetErrtext() {
    ch_RemoveAll(algo_lib::_db.errtext);
    return algo_lib::_db.errtext;
}

// -----------------------------------------------------------------------------

// Add key-value pair to algo_lib::_db.errtext
// Error text beyond a reasonable limit is discarded -- keep errors short!
void algo_lib::AppendErrtext(const strptr &name, const strptr &value) {
    if (ch_N(algo_lib::_db.errtext) < 1024) {
        PrintAttrSpace(algo_lib::_db.errtext, name, value);
    }
}

// -----------------------------------------------------------------------------

// Retrieve whatever bad tags were saved with AppendErrtext,
// and clear the state.
// AppendErrtext is typically called by string read functions that encounter
// something unreadable. This is the only way to retrieve that
// additional information
tempstr algo_lib::DetachBadTags() {
    tempstr ret;
    TSwap(algo_lib::_db.errtext,(cstring&)ret);
    return ret;
}

// -----------------------------------------------------------------------------

// Increment algo_lib::_db.trace.tot_insert_err
// And print accumulated 'bad tags' using prerr.
// if SetShowInsertErrLim was previously called.
void algo_lib::NoteInsertErr(strptr tuple) {
    algo_lib::_db.trace.tot_insert_err++;
    prcat(inserr,"algo_lib.insert_err"
          <<MaybeSpace<<LimitLengthEllipsis(DetachBadTags(), 512)
          <<MaybeSpace<<LimitLengthEllipsis(tuple, 512));
}
