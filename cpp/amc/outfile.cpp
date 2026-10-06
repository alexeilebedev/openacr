// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/outfile.cpp -- Output functions
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Insert TEXT into OUT as a comment, each line opened by MARK and a space, and a
// line of its own where TEXT has a blank one.
static void InsertComment(cstring &out, strptr text, strptr mark) {
    ind_beg(Line_curs,line,text) {
        out<<mark;
        if (ch_N(line)) {
            out<<" "<<line;
        }
        out<<eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Number of newline characters in TEXT: the number of lines in a text that ends
// with one.
int amc::CountLines(strptr text) {
    int n = 0;
    const char *p = text.elems;
    const char *end = text.elems + text.n_elems;
    while (p < end) {
        const char *nl = (const char*)memchr(p, '\n', end - p);
        if (nl) {
            n++;
            p = nl + 1;
        } else {
            p = end;
        }
    }
    return n;
}

// -----------------------------------------------------------------------------

// Write output file to disk
// and deallocate memory associated with it
void amc::gen_ns_write() {
    amc::FNs &ns=*amc::_db.c_ns;
    // A generation error must suppress all output: a partial write would
    // leave a half-regenerated tree that looks up to date. But exit_code
    // alone cannot serve as the gate: each failed write below increments it
    // too, so the first namespace's failed write would silently skip every
    // later namespace's writes, and the error report would name only the
    // first namespace's paths. Generation is complete when this phase runs
    // (ns_write is the last per-namespace gen), so exit_code counts pure
    // generation errors exactly until the first write -- capture it then,
    // and gate every namespace's output on the captured value.
    if (_db.n_generr == -1) {
        _db.n_generr = algo_lib::_db.exit_code;
    }
    // -derive writes the amc-owned tables and no source.  A gstatic that
    // carries one of those tables is compiled from the table's ssim file,
    // which is read back near the start of the run, so a run whose
    // derivation changes such a table generates its source from the file as
    // it stood before the change.  Deriving in a run of its own leaves the
    // files correct for the run that generates from them.
    if (!amc::QueryModeQ() && ch_N(amc::_db.cmdline.out_dir) && !amc::_db.cmdline.derive && _db.n_generr==0) {
        int nbefore=algo_lib::_db.stringtofile_nwrite;
        ind_beg(amc::ns_c_outfile_curs, outfile,ns) {
            amc::_db.report.n_cppfile++;
            // save to preassigned filename, or out dir if overridden.  The
            // file's directory is created first, since a tree that has never
            // been generated (a fresh clone of a published package) holds no
            // gen directory for a language the package projects into.  A write
            // that still fails (permission, a file where the directory goes)
            // fails the run: exiting 0 with the generated code silently missing
            // would leave a stale tree that looks up to date
            tempstr fname(DirFileJoin(amc::_db.cmdline.out_dir, outfile.outfile));
            (void)algo::CreateDirRecurse(GetDirName(fname), false);
            if (!algo::SaveFile(outfile.text, fname, "amc.outfile_write", "output file could not be written")) {
                algo_lib::_db.exit_code++;
            }
            amc::_db.report.n_cppline += amc::CountLines(outfile.text);
        }ind_end;
        amc::_db.report.n_filemod += algo_lib::_db.stringtofile_nwrite - nbefore;
        c_outfile_Cascdel(ns);
        ns.inl=NULL;
        ns.hdr=NULL;
        ns.cpp=NULL;
    }
}

// -----------------------------------------------------------------------------

// Create outfile record for specified filename
// T here is one outfile per generated output file
amc::FOutfile &amc::outfile_Create(strptr filename) {
    amc::FOutfile &outfile = amc::outfile_Alloc();
    outfile.outfile=filename;
    (void)outfile_XrefMaybe(outfile);
    strptr license = outfile.p_ns->p_license->text;
    if (!ch_N(outfile.text)) {
        cstring comment = tempstr()
            << eol
            << filename << eol
            << "Generated by AMC" << eol
            << eol
            << _db.copyright
            << eol
            << license;
        // a language says what opens a comment in its files, and how far its
        // formatter leaves the header from the first declaration; a file of a
        // language with no row here is C++, which takes the defaults
        algo::strptr linecomment("//");
        int nblank = 2;
        amc::FLang *lang = amc::ind_lang_Find(algo::Pathcomp(filename,".RR"));
        if (lang) {
            linecomment = lang->linecomment;
            nblank = lang->nblank;
        }
        InsertComment(outfile.text,comment,linecomment);
        for (int i = 0; i < nblank; i++) {
            outfile.text << eol;
        }
    }
    return outfile;
}
