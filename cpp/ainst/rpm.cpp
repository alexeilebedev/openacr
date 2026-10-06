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
// Source: cpp/ainst/rpm.cpp
//

#include "include/algo.h"
#include "include/ainst.h"

// -----------------------------------------------------------------------------

// Version of INST in the form rpm accepts, which forbids a dash.
// A release tag such as RELEASE.2025-08-13T08-35-41Z is a version like any
// other once its dashes are underscores.  An installation that pins nothing
// answers 0, because rpmbuild refuses a spec whose Version tag is empty.
static algo::tempstr GetRpmVersion(ainst::FExtpkg &extpkg) {
    algo::tempstr ret;
    algo::strptr version(ainst::GetExtpkgVersion(extpkg));
    frep_(i,version.n_elems) {
        char c = version[i];
        ret << (c == '-' ? '_' : c);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Write the lines that produce INST's spec file into the script being assembled.
// The staged tree supplies %files, listed by walking the buildroot at build
// time rather than named here, and the installation's own post lines supply
// %post.  Only a directory the walk finds empty is listed: rpm creates the
// leading directories of every file it carries, so listing them again would
// claim ownership of /usr and /usr/local on the way to one binary.  An installation that stages no file still packages: what it does to
// the system it does from the scriptlet, which is where it did it anyway.
static void WriteSpec(ainst::FExtpkg &extpkg, algo::cstring &out) {
    tempstr summary;
    if (ch_N(extpkg.comment)) {
        summary << extpkg.comment;
    } else {
        summary << extpkg.extpkg;
    }
    out << "cat > \"$specdir/" << extpkg.extpkg << ".spec\" << 'AINST_SPEC_EOF'" << eol;
    out << "%define _build_id_links none" << eol;
    out << "Name: " << extpkg.extpkg << eol;
    out << "Version: " << GetRpmVersion(extpkg) << eol;
    out << "Release: 1" << eol;
    out << "Summary: " << summary << eol;
    out << "License: unspecified" << eol;
    out << "BuildArch: x86_64" << eol;
    out << "AutoReqProv: no" << eol;
    out << "%description" << eol;
    out << summary << eol;
    out << "%files" << eol;
    out << "AINST_SPEC_EOF" << eol;
    out << "(cd \"$buildroot\" && find . -mindepth 1 \\( -type f -o -type l \\) -printf '/%P\\n'"
        << " && find . -mindepth 1 -type d -empty -printf '%%dir /%P\\n')"
        << " >> \"$specdir/" << extpkg.extpkg << ".spec\"" << eol;
    out << "cat >> \"$specdir/" << extpkg.extpkg << ".spec\" << 'AINST_SPEC_EOF'" << eol;
    out << "%post" << eol;
    ainst::WritePreamble(out);
    // The preamble's cache default is a path inside the checkout, which is right
    // for a developer and meaningless on the machine installing an rpm.
    out << "AINST_CACHE=$(mktemp -d)" << eol;
    ainst::WriteFetch(extpkg,out);
    out << extpkg.post;
    out << "AINST_SPEC_EOF" << eol;
}

// -----------------------------------------------------------------------------

// Assemble the script that packages every selected installation as an rpm.
// Each one stages into a buildroot of its own and rpmbuild turns that directory
// into a package, so an rpm carries exactly the files a live install would have
// placed -- the same shell function produced both, called under a different
// root.
void ainst::WriteRpmScript(algo::cstring &out) {
    ainst::WriteFunc(out);
    out << eol;
    out << "# -----------------------------------------------------------------------------" << eol;
    out << "specdir=" << ainst::_db.cmdline.outdir << "/spec" << eol;
    out << "rpmdir=$(readlink -f " << algo::strptr_ToBash(ainst::_db.cmdline.outdir) << ")/rpm" << eol;
    out << "mkdir -p \"$specdir\" \"$rpmdir\"" << eol;
    ind_beg(ainst::_db_zd_sel_extpkg_curs,extpkg,ainst::_db) {
        out << eol;
        out << "buildroot=$(readlink -f " << algo::strptr_ToBash(ainst::_db.cmdline.outdir)
            << ")/buildroot/" << extpkg.extpkg << eol;
        out << "rm -rf \"$buildroot\"" << eol;
        out << "mkdir -p \"$buildroot\"" << eol;
        out << "ROOT=$buildroot inst_" << ainst::GetExtpkgSymbol(extpkg) << "_stage" << eol;
        WriteSpec(extpkg,out);
        out << "rpmbuild -bb \"$specdir/" << extpkg.extpkg << ".spec\""
            << " --buildroot \"$buildroot\" --define \"_rpmdir $rpmdir\"" << eol;
    }ind_end;
}
