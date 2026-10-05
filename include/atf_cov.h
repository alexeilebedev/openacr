// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: atf_cov (exe) -- Line coverage
// Exceptions: yes
// Header: include/atf_cov.h
//

#include "include/algo.h"
#include "include/gen/atf_cov_gen.h"
#include "include/gen/atf_cov_gen.inl.h"

namespace atf_cov { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_cov/atf_cov.cpp
    //
    void MergeCovline(dev::Covline &covline_in);
    void RunGcov(strptr covdir);
    void WriteCovSsim();
    void ComputeCoverage();
    void GenerateSsimReport();
    void GenerateTxtReport();
    void Summary();
    void XmlIndent(algo::cstring &out, strptr text, int indent);
    void GenerateCoberturaReport();

    // Judge the run first, and its targets only if the run is whole.
    //
    // A run that lost data measures the targets it did reach at less than their
    // real coverage, because the tests whose data went missing are the same tests
    // that exercise the rest of the tree.  Judging such a run target by target
    // prints one floor breach per target -- fifty of them on a bad day -- and every
    // line of that names a target of the branch under test, so the author reads a
    // lost merge directory as fifty regressions they caused.  So a run that lost
    // data fails once, as one fact about the run, naming what went missing; the
    // answer to it is to run the job again, not to read the diff.
    //
    // A whole run judges each target against its floor, and a measurement that came
    // in under one is a regression the diff under test explains.
    void Main_Check();

    // Write each target's measurement into dev.tgtcov as its new floor, and the
    // functions no test reached into dev.uncovfunc.
    //
    // A capture is worth no more than the run beneath it.  A run that lost a merge
    // directory measures every target that directory exercised at a fraction of its
    // real coverage, and capturing those figures writes the loss into the floors:
    // the gate comes down by exactly the amount that went missing, nothing in the
    // output says so, and the next run passes against the lowered bar.  A capture
    // is also the one operation here with no undo short of a revert.  So a run
    // showing any sign of loss is refused, and the floors keep the values an
    // earlier whole run put there.
    void Main_Capture();
    void SaveCov();

    // Build the dev.uncovfunc backlog: every in-scope function whose
    // executable lines are all unhit across the suite.  Function extents
    // (source file, begin and end line) come from src_func -printssim;
    // per-line hit data from the Covline pool.  Scoped like the coverage
    // report -- only functions in target sources are considered, which
    // excludes generated/external the same way RunGcov already filters.
    void ComputeUncovfunc();

    // Dump the uncovfunc pool to PATH in dev.uncovfunc ssim format.
    void WriteUncovfunc(strptr path);

    // Persist the uncovfunc pool to its committed ssimfile, replacing prior
    // contents -- the same acr -replace -trunc path SaveCov uses for tgtcov.
    void SaveUncovfunc();
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // dmmeta.main:atf_cov
}
