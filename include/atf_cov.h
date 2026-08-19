// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2023 Astra
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

    // Judge every target that carries a coverage floor, and name which of the two
    // things that can go wrong did.  A target whose measurement came in under its
    // floor is a regression, and the diff under test is where to look for it.  A
    // target that produced no data at all did not regress: the run never measured
    // it, and scoring that as zero coverage sends the reader hunting for a code
    // cause that does not exist.  The two need opposite responses, so they are
    // reported as different facts.
    void Main_Check();
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
    // void Main(); // main:atf_cov
}
