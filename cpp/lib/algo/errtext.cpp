// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/errtext.cpp
//

#include "include/algo.h"

// -----------------------------------------------------------------------------

void algo_lib::SaveBadField(strptr name, strptr value) {
    SaveBadTag("attr",name);
    (void)name;
    (void)value;
}

// -----------------------------------------------------------------------------

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
void algo_lib::SaveBadTag(const strptr &name, const strptr &value) {
    if (ch_N(algo_lib::_db.errtext) < 256) {
        PrintAttrSpace(algo_lib::_db.errtext, name, value);
    }
}

// -----------------------------------------------------------------------------

// Better name than SaveBadTAg
void algo_lib::AppendErrtext(const strptr &name, const strptr &value) {
    SaveBadTag(name,value);
}

// -----------------------------------------------------------------------------

// Retrieve whatever bad tags were saved with SaveBadTag,
// and clear the state.
// SaveBadTag is typically called by string read functions that encounter
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
// Note: printing is disabled by default, and must be explicitly enabled.
void algo_lib::NoteInsertErr(strptr tuple) {
    // show first few errors (default: 0)
    if (algo_lib::_db.trace.tot_insert_err < algo_lib::_db.show_insert_err_lim) {
        prerr("algo_lib.insert_err"
              <<MaybeSpace<<LimitLengthEllipsis(DetachBadTags(), 128)
              <<MaybeSpace<<LimitLengthEllipsis(tuple,128));
    }
    algo_lib::_db.trace.tot_insert_err++;
}

// -----------------------------------------------------------------------------

// Specify how many times NoteInsertErr can use prerr
// to output.
void algo_lib::SetShowInsertErrLim(int n) {
    algo_lib::_db.show_insert_err_lim = n;
}
