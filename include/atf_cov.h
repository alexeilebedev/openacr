//
// Target: atf_cov (exe)
// Exceptions: yes
// Header: include/atf_cov.h
//

#include "include/gen/atf_cov_gen.h"
#include "include/gen/atf_cov_gen.inl.h"

namespace atf_cov { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf/atf_cov.cpp
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
    void Main_Check();
    void Main_Capture();
    void SaveCov();
    void Main();
}
