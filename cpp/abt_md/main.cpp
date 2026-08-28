// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: abt_md (exe) -- Tool to generate markdown documentation
// Exceptions: yes
// Source: cpp/abt_md/main.cpp -- Markdown linter and regenerator - main file
//

#include "include/algo.h"
#include "include/abt_md.h"

// -----------------------------------------------------------------------------

// Return true if readme file READMEFILE auto-generates section MDSECTION
// (Any section is allowed but not all will be auto-updated)
bool abt_md::GenSectionQ(abt_md::FMdsection &mdsection, abt_md::FReadmefile &readmefile) {
    return Regx_Match(mdsection.regx_path,readmefile.gitfile) && mdsection.path!="";
}

// Extract words from line up until first dash
// E.g. LineKey("#### Some Text - blah") -> "some-text"
// This is similar to the algorithm used by other markdown processors
// to generate anchors, but we allow '.' in anchor name
tempstr abt_md::LineKey(algo::strptr line) {
    strptr str=Pathcomp(line," LR");
    int i=FindStr(str," - ");
    int j=FindStr(str," -- ");
    if (i==-1 || (j!=-1 && j<i)) {
        i=j;
    }
    if (i!=-1) {
        str=ch_FirstN(str,i);
    }
    str=Trimmed(str);
    return MdAnchor(str);
}

// Translate characters to create a markdown link
// : is skipped
// non-identifier characters are replaced with -
// internal runs of - are collapsed to one
// (so "A & B" and "A: B" both anchor as "a-b"; "-link" stays as "-link")
// All characters are lowercased
tempstr abt_md::MdAnchor(algo::strptr str) {
    tempstr ret;
    bool prev_dash = false;
    for (int i=0; i < str.n_elems; i++) {
        char c = algo::ToLower(str.elems[i]);
        if (c != ':') {
            if (!algo_lib::IdentCharQ(c)) {
                if (!prev_dash) {
                    ret << '-';
                }
                prev_dash = true;
            } else {
                ret << c;
                prev_dash = false;
            }
        }
    }
    return ret;
}

// Print anchor to OUT and add it to a global table
void abt_md::AddAnchor(algo::strptr name, cstring &out) {
    out <<"<a href=\"#" << name << "\"></a>" << eol;
}

tempstr abt_md::Backticks(algo::strptr text) {
    return tempstr()<<"`"<<text<<"`";
}

tempstr abt_md::Preformatted(algo::strptr text, algo::strptr lang) {
    tempstr ret;
    ret << "```"<<lang<<"\n"<<text;
    if (!EndsWithQ(text,"\n")) {
        ret << "\n";
    }
    ret << "```\n";
    return ret;
}

tempstr abt_md::CodeBlock(algo::strptr text) {
    tempstr ret;
    ret << "```c++\n"<<text;
    if (!EndsWithQ(text,"\n")) {
        ret << "\n";
    }
    ret << "```\n";
    return ret;
}

// -----------------------------------------------------------------------------

// Return markdown link pointing to page URL and optional anchor ANCHOR
// The displayed string is NAME
tempstr abt_md::Link(algo::strptr name, algo::strptr url, algo::strptr anchor DFLTVAL("")) {
    tempstr ret;
    ret<<"["<<name<<"]("<<url;
    if (anchor != "") {
        ret << "#"<<anchor;
    }
    ret<<")";
    return ret;
}

// Link within repo to a given file
// The URL is given as a relative pathname with respect to the root
tempstr abt_md::LinkToFileAbs(algo::strptr name, algo::strptr url, algo::strptr anchor DFLTVAL("")) {
    return Link(name, tempstr()<<"/"<<url, anchor);
}

// Link to whatever documents SSIMFILE, with NAME as the link text.
//
// A namespace's tables are documented in one file, txt/ssimdb/<ns>/README.md, and a table
// somebody wrote about has a section there under a heading naming it -- so the anchor of
// that heading is the table's own name and the link carries it.  A table nobody wrote
// about has no such heading, and the link then names the namespace's documentation
// alone, which is where a reader of that link is going anyway.  Emitting the anchor
// regardless would make `abt_md -check` right to complain: an anchor naming no heading is
// a link that lands somewhere the writer did not mean.
//
// ANCHOR is not taken here, because the anchor is the ssimfile.
tempstr abt_md::LinkToSsimfile(algo::strptr name, algo::strptr ssimfile, algo::strptr anchor DFLTVAL("")) {
    (void)anchor;
    abt_md::FSsimfile *file = abt_md::ind_ssimfile_Find(ssimfile);
    tempstr path;
    path << "txt/ssimdb/" << Pathcomp(ssimfile,".LL") << "/README.md";
    return LinkToFileAbs(name, path, file && file->prose ? MdAnchor(ssimfile) : tempstr());
}

// Mark every ssimfile a namespace README carries a section about.
//
// The section's heading is the table's name, and that heading is what a link to the
// table anchors on, so this walk is what decides whether such a link carries an anchor
// at all.  It reads each ssimdb namespace's README once, before any document is
// generated, since a link written into one document may name a table of another.
void abt_md::Main_ScanSsimdoc() {
    ind_beg(_db_ns_curs,ns,_db) if (ns.nstype == dmmeta_Nstype_nstype_ssimdb) {
        tempstr path;
        path << "txt/ssimdb/" << ns.ns << "/README.md";
        if (FileQ(path)) {
            ind_beg(algo::FileLine_curs,line,path) {
                if (StartsWithQ(line,"### ")) {
                    if (FSsimfile *file = ind_ssimfile_Find(Trimmed(RestFrom(line,4)))) {
                        file->prose = true;
                    }
                }
            }ind_end;
        }
    }ind_end;
}

// Link to documentation for given namespace (could be lib,protocol,exe,ssimdb)
tempstr abt_md::LinkToNs(strptr ns, algo::strptr anchor DFLTVAL("")) {
    abt_md::FNs &fns = ind_ns_FindX(ns);
    return LinkToFileAbs(ns, tempstr()<<"txt/"<<fns.nstype<<"/"<<ns<<"/README.md", anchor);
}

// Link to the page documenting REFTYPE.
//
// Every reftype has a page of its own under txt/exe/amc/reftype, and the link
// goes straight to it.  The index beside them, txt/exe/amc/reftype.md, groups the
// reftypes so a reader who does not yet know which one they want can find it --
// but a reader who arrives from a field's reftype column already knows, and
// landing them on an index entry that says "see the page" costs them a click for
// nothing.
tempstr abt_md::LinkToReftype(algo::strptr reftype) {
    return LinkToFileAbs(reftype, tempstr() << "txt/exe/amc/reftype/" << reftype << ".md");
}

// Link to whatever documents CTYPE, or name it as a code span when nothing does.
//
// A ctype that is a table has the ssimdb page of that table, and a ctype with a file of
// its own is linked to that file.  Everything else is a type nothing in txt/ describes,
// and what a reader wants of one is its declaration and the code amc made of it -- which
// is the page `doc ctype:<name>` opens.
//
// A markdown link may name a path and a fragment and nothing else, so a file cannot spell
// that page as a target.  What it can do is write the name as a code span, which doc reads
// as the name of a ctype and turns into a link to that ctype's page; a browser looking at
// the file on the server renders it as the name, which is what it is.
tempstr abt_md::LinkToCtype(abt_md::FCtype &ctype) {
    abt_md::FNs &ns=*ctype.p_ns;
    tempstr ret;
    ret << "`" << ctype.ctype << "`";
    // global namespace doesn't have a readme yet
    if (ctype.c_ssimfile) {
        ret = LinkToSsimfile(ctype.ctype, ctype.c_ssimfile->ssimfile);
    } else if (ns.ns != "") {
        tempstr fname = tempstr()<<"txt/"<<ns.nstype<<"/"<<ns.ns<<"/"<<name_Get(ctype)<<".md";
        if (FileQ(fname)) {
            ret = LinkToFileAbs(ctype.ctype, fname);
        }
    }
    return ret;
}
// -----------------------------------------------------------------------------

// Compute base type, or return CTYPE if none
abt_md::FCtype *abt_md::GetBaseType(abt_md::FCtype &ctype) {
    abt_md::FCtype *ret=&ctype;
    ind_beg(abt_md::ctype_c_field_curs,field,ctype){
        if (field.reftype == dmmeta_Reftype_reftype_Base) {
            ret=field.p_arg;
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Create an HTML comment
tempstr abt_md::MdComment(algo::strptr str){
    return tempstr()<<"<!-- "<<str<<" -->";
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int abt_md::Sortkey(abt_md::FMdsection &mdsection, int i) {
    return mdsection.rowid * 10000 + i;
}

// -----------------------------------------------------------------------------

// Determine header depth level of current line by counting leading #'s
int abt_md::GetHeaderLevel(strptr line) {
    strptr word=Pathcomp(line," LL");
    bool hdr=true;
    for (int i=0; i<word.n_elems; i++) {
        if (word.elems[i]!='#') {
            hdr=false;
            break;
        }
    }
    return hdr ? word.n_elems : 0;
}

// -----------------------------------------------------------------------------

// Update loaded readme file:
// - generate missing sections
// - evaluate all commands using sandbox (if specified)
// - record the document's title on its dev.readmefile row
// - save readme to disk
void abt_md::UpdateReadme() {
    abt_md::FReadmefile &readmefile = *_db.c_readmefile;
    // create missing file sections
    ind_beg(abt_md::_db_mdsection_curs,mdsection,_db) {
        if (zd_file_section_EmptyQ(mdsection) && GenSectionQ(mdsection,readmefile)) {
            abt_md::FFileSection &section=file_section_Alloc();
            section.firstline=1;
            section.p_mdsection=&mdsection;
            section.title = mdsection.match;
            Replace(section.title,"%","");
            section.select=Regx_Match(_db.cmdline.section,section.title);
            section.sortkey = Sortkey(mdsection,1);
            file_section_XrefMaybe(section);
        }
    }ind_end;

    if (readmefile.sandbox && _db.cmdline.evalcmd) {
        command::wt_proc wt;
        wt.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
        wt.cmd.reset = true;
        wt.cmd.q = true;
        wt_ExecX(wt);
    }

    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        if (file_section.select) {
            UpdateSection(file_section);
        }
    }ind_end;

    // A listing names a document by its title rather than by its path, and finding that
    // title otherwise means opening the file: sixty files in a directory is sixty opens
    // for a string that moves only when the document's own title does.  So the title is
    // recorded where the document is recorded, and anything listing documents reads the
    // record.
    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        if (TitleQ(file_section)) {
            strptr line = file_section.title;
            readmefile.comment = Trimmed(RestFrom(line,GetHeaderLevel(line)));
        }
    }ind_end;

    cstring out;
    PrintSections(out);
    if (!_db.cmdline.dry_run && algo_lib::_db.exit_code==0) {
        verblog("save "<<readmefile.gitfile<<eol);
        // The save gate above reads exit_code as "generation failed", so a
        // write failure must not feed it: the first readme's failed write
        // would silently suppress every later readme's save, and the run
        // would report only one path. Failed writes are counted apart on
        // n_writefail (each one reported here) and fail the run at the end
        // of Main.
        bool saved = algo::SaveFile(out,readmefile.gitfile,"abt_md.file_write","readme file could not be written");
        if (!saved) {
            _db.n_writefail++;
        }
        // **VP** Hardcoded code for keeping top level README.md to be automatically visible in gitlab/github
        // Softlink README.md to txt/README.md doesn't work for gitlab - doesn't render
        // Keeping README.md in dev.readme table so abt_md generates it breaks xref with ordered dev.readmecat
        // The copy is skipped when the txt/README.md save failed: README.md
        // must never get ahead of its source.
        if (saved && readmefile.gitfile=="txt/README.md"){
            tempstr out_readme;
            out_readme<<"<!-- This file is a copy of txt/README.md -->"<<eol;
            out_readme<<"<!-- Don't edit this file, edit txt/README.md -->"<<eol;
            out_readme<<out;
            if (!algo::SaveFile(out_readme,"README.md","abt_md.file_write","readme file could not be written")) {
                _db.n_writefail++;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Enforce structure of files under txt:
// - txt/{nstype}/xyz.md implies that namespace 'xyz' must exist
// - txt/script/xyz.md implies that file bin/xyz must exist
// For namespaces, set FReadme.p_ns to point to the namespace,
//  and FNamespace.c_readme to point to the readme.
// Skip files named README.md since they are index files
// The same check is partially implemented via ssimreq, but ssimreq
// canot handle exceptions like README.md
void abt_md::Main_XrefNs() {
    ind_beg(_db_readmefile_curs,readmefile,_db) {
        // PATHNAME                              DIR1     DIR2     FILENAME
        // txt/exe/acr/main.md                   exe      ""       main
        // txt/script/abc.md                     script   script   abc
        // txt/ssimdb/ns/name.md                 ssimdb   ssimdb   name
        tempstr gitfile(readmefile.gitfile);
        algo::strptr dir1=Pathcomp(gitfile, "/LR/LL");
        algo::strptr dir2=Pathcomp(gitfile, "/LR/LR/LL");
        algo::strptr filename = Pathcomp(gitfile, "/RR.RL");
        tempstr stray_error;
        if (dir1 == "script" && filename != "README") {// manually check script existence
            tempstr scriptfile_key(DirFileJoin("bin",filename));
            if (abt_md::FScriptfile *scriptfile=ind_scriptfile_Find(scriptfile_key)) {
                readmefile.p_scriptfile=scriptfile;
            } else {
                stray_error<<Keyval("no such script",scriptfile_key);
            }
        } else if (dir1 == "gen" && ind_ns_Find(dir2)) {
            abt_md::FNs *ns = ind_ns_Find(dir2);
            bool genns = ns->nstype == dmmeta_Nstype_nstype_exe || ns->nstype == dmmeta_Nstype_nstype_lib;
            if (genns && ns->ns != "") {
                readmefile.p_ns = ns;
                // don't set ns->c_readmefile: that should point to README.md, not internals
            }
        } else if (abt_md::FNs *ns=ind_ns_Find(dir2)) {
            if (dir1 == dmmeta_Nstype_nstype_exe) {
                if (ns->ns != "") {
                    readmefile.p_ns=ns;
                    ns->c_readmefile=&readmefile;
                }
            } else if (dir1==dmmeta_Nstype_nstype_ssimdb) {
                tempstr ssimfile_key(tempstr()<<dir2<<"."<<filename);
                if (filename == "README") {
                    readmefile.p_ns = ns;
                    ns->c_readmefile=&readmefile;
                } else if (abt_md::FSsimfile *ssimfile =ind_ssimfile_Find(ssimfile_key)) {
                    readmefile.p_ssimfile=ssimfile;
                    readmefile.p_ctype=ssimfile->p_ctype;
                } else {
                    stray_error<<Keyval("no such ssimfile",ssimfile_key);
                }
            } else if (dir1==dmmeta_Nstype_nstype_lib || dir1==dmmeta_Nstype_nstype_protocol) {
                tempstr ctype_key(tempstr()<<dir2<<"."<<filename);
                if (filename == "README") {
                    readmefile.p_ns = ns;
                    ns->c_readmefile=&readmefile;
                } else if (abt_md::FCtype *ctype =ind_ctype_Find(ctype_key)) {
                    readmefile.p_ctype=ctype;
                } else {
                    stray_error<<Keyval("no such ctype",ctype_key);
                }
            } else {
                // other file such as txt/exe/amc/xyz.md
            }
        }

        if (stray_error!="") {
            prerr("abt_md.unrecognized_readme"
                  <<Keyval("readmefile",readmefile.gitfile)
                  <<Keyval("dir1",dir1)
                  <<Keyval("dir2",dir2)
                  <<Keyval("comment",stray_error));
            algo_lib::_db.exit_code++;
        }
        if (readmefile.select) {
            if (readmefile.p_scriptfile) {
                verblog("abt_md.readme_scriptfile"
                        <<Keyval("readmefile",readmefile.gitfile)
                        <<Keyval("scriptfile",readmefile.p_scriptfile->gitfile));
            } else if (readmefile.p_ns) {
                verblog("abt_md.readme_ns"
                        <<Keyval("readmefile",readmefile.gitfile)
                        <<Keyval("ns",readmefile.p_ns->ns));
            } else if (readmefile.p_ssimfile) {
                verblog("abt_md.readme_ssimfile"
                        <<Keyval("readmefile",readmefile.gitfile)
                        <<Keyval("ssimfile",readmefile.p_ssimfile->ssimfile));
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Collapse '.' and '..' components of PATH lexically ('sub/../c.md' ->
// 'c.md').  The file and anchor tables are keyed by canonical
// repo-relative gitfile, so a link target joined from a document's
// directory must lose its dot components before lookup.  Leading '..'
// components that reach above the starting directory are kept.
static tempstr CollapseDots(strptr path) {
    tempstr ret;
    ind_beg(algo::Sep_curs, comp, path, '/') {
        if (comp == "" || comp == ".") {
            // skip empty and current-directory components
        } else if (comp == ".." && ch_N(ret) && Pathcomp(ret,"/RR") != "..") {
            ret.ch_n = TRevFind(ret,'/').beg;
        } else {
            if (ch_N(ret)) {
                ret << "/";
            }
            ret << comp;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

void abt_md::CheckLinks() {
    ind_beg(_db_link_curs,link,_db) {
        bool good=true;
        // link.target might be #abcd, or file.md#abcd, or /txt/file.md#abcd
        // in the first case, the target path is empty and anchor is abcd
        // empty target path refers to the same file, and is ok
        // in the second case, the target path is file.md, and is interpreted
        // relative to the directory of the document containing the link
        // in the third case, the path is interpreted relative to the repo root
        tempstr path;
        tempstr relpath(Pathcomp(link.target,"#LL"));
        if (StartsWithQ(link.target,"https://") || StartsWithQ(link.target,"http://")) {
            if (_db.cmdline.external) {
                verblog("checking "<<link.target);
                tempstr cmd = tempstr() << "curl --output /dev/null --silent --head --fail " << strptr_ToBash(link.target);
                good=SysCmd(cmd) == 0;
            }
        } else if (StartsWithQ(link.target,"/")) {
            // /txt/file.md#abcd -> txt/file.md
            path=Pathcomp(link.target,"/LR#LL");
        } else if (algo::FindChar(Pathcomp(link.target,"/LL#LL"),':') >= 0) {
            // a colon in the first path component marks a scheme-qualified
            // URI (mailto:, ftp:, ssh:) -- not a repo file; of the external
            // targets only http(s) is checkable (above, with -external)
        } else if (relpath =="") {
            // link.location is filename.md:22, this yields filename.md
            path=Pathcomp(link.location,":RL");
        } else {
            // if link.location is txt/file.md:22
            // and link.target is anotherfile.md#anchor,
            // path is txt/anotherfile.md
            path=DirFileJoin(GetDirName(link.location),relpath);
        }
        if (path != "") {
            path = CollapseDots(path);
            if (path == ".." || StartsWithQ(path,"../")) {
                // a collapsed target that keeps a leading '..' points above
                // the repo root, so the rendered link is broken on any host
                // that serves just the repo; checking it against the local
                // filesystem would make the verdict depend on whatever sits
                // outside the checkout, so report it without looking
                good=false;
                prlog(link.location<<": "
                      <<Keyval("target",link.target)
                      <<Keyval("path",path)
                      <<Keyval("comment","target outside repository"));
            } else {
                if (!ind_gitfile_Find(path)) {
                    good=false;
                    prlog(link.location<<": "
                          <<Keyval("target",link.target)
                          <<Keyval("path",path)
                          <<Keyval("comment","target is not a file the repo tracks"));
                }
                tempstr anchor(Pathcomp(link.target,"#LR"));
                tempstr fullanchor = tempstr() << path << "#"<<anchor;
                if (good && anchor!="" && !ind_anchor_Find(fullanchor)) {
                    good=false;
                    prlog(link.location<<": "
                          <<Keyval("target",link.target)
                          <<Keyval("anchor",anchor)
                          <<Keyval("fullanchor",fullanchor)
                          <<Keyval("comment","link path is OK, but the anchor doesn't exist"));
                }
            }
        }
        if (!good) {
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

void abt_md::ProcessReadme(abt_md::FReadmefile& readmefile) {
    _db.c_readmefile = &readmefile;
    // create global table file_section, x-referenced with mdsection.
    // LoadSections also validates conflict markers; a region straddling
    // section headers corrupts the split, so refuse to rewrite the file.
    bool safe = LoadSections(readmefile);
    if (!safe) {
        prerr(readmefile.gitfile<<": "
              <<Keyval("comment","conflict markers span section headers; refusing to process -- resolve by hand"));
        algo_lib::_db.exit_code++;
    } else {
        // once a generation error is recorded the run is doomed: every
        // later save is gated off, so regenerating sections, resetting the
        // sandbox, and forking inline-commands for the remaining readmes
        // would produce output that is discarded -- skip that work and
        // leave the recorded error as the run's report
        if (_db.cmdline.update && algo_lib::_db.exit_code==0) {
            UpdateReadme();
        }
        // scan sections for links and anchors
        ScanLinksAnchors();
        // Queue the command lines the sections document; Main_RunBatchCheck
        // validates the whole queue in one fork once every readme is loaded.
        // Collecting here rather than during regeneration is what lets
        // -check report a wrong command: -check is the read-only mode, so it
        // never regenerates a section, and a check that rode along with
        // generation reported nothing in the very mode whose job is checking.
        if (_db.cmdline.check || _db.cmdline.update) {
            CheckCommandLines();
        }
        if (_db.cmdline.print && !_db.cmdline.link && !_db.cmdline.anchor) {
            cstring out;
            PrintSections(out);
            prlog(out);
        }
    }
}

// -----------------------------------------------------------------------------

// Flush queued FCheckReq rows through `acr_compl -check_batch` in a single
// fork.  Requests are streamed to acr_compl via a temp file; acr_compl emits
// one `acr_compl.checkerr` tuple per failing request on its stdout.  Each
// failure is mapped back to its source location and reported as
// `<gitfile>:<lineno>: <err>`, and the global exit code is set.  Any other
// line the subprocess writes passes through to stderr unchanged.
// A subprocess that exits abnormally has emitted no checkerr line for the
// validations it never ran, so its wait status fails the run too.
// Write the recorded titles back into the database they were read from.
//
// Every processed row is sent, not only the ones whose title moved, because acr decides
// whether the file changes: a run over a tree already up to date updates nothing and
// writes nothing, and tracking which rows moved would be a second answer to a question
// acr already answers.
//
// acr is told the same -in this run was given, so the rows land where they came from.
// An -in naming one ssimfile, or stdin, is a fragment somebody assembled by hand rather
// than a database this tool may edit, so such a run records nothing at all -- which is
// also what keeps a comptest driving abt_md from a scratch directory out of data/.
//
// A dry run sends nothing, and neither does a check: the whole of what either promises
// is that it leaves the tree alone.
void abt_md::Main_SaveReadmetitle() {
    cstring content;
    int nrow = 0;
    ind_beg(_db_readmefile_curs,readmefile,_db) {
        if (readmefile.select) {
            dev::Readmefile out;
            readmefile_CopyOut(readmefile,out);
            content << out << eol;
            nrow++;
        }
    }ind_end;
    if (nrow && _db.cmdline.update && !_db.cmdline.dry_run && DirectoryQ(_db.cmdline.in)) {
        tempstr tmpfile;
        tmpfile << "/tmp/abt_md.readmetitle." << getpid();
        vrfy(SafeStringToFile(content, tmpfile),
             tempstr() << "abt_md.tmpfile" << Keyval("path", tmpfile));
        command::acr_proc proc;
        proc.cmd.in = _db.cmdline.in;
        proc.cmd.update = true;
        proc.cmd.write = true;
        proc.fstdin = tmpfile;
        acr_ExecX(proc);
        (void)unlink(Zeroterm(tmpfile));
    }
}

void abt_md::Main_RunBatchCheck() {
    if (checkreq_N()) {
        tempstr tmpfile;
        tmpfile << "/tmp/abt_md.checkreq." << getpid();
        cstring content;
        ind_beg(_db_checkreq_curs, req, _db) {
            content << "acr_compl.checkreq  "
                    << Keyval("id", req.id)
                    << "  "
                    << Keyval("line", req.line)
                    << eol;
        }ind_end;
        vrfy(SafeStringToFile(content, tmpfile),
             tempstr() << "abt_md.tmpfile" << Keyval("path", tmpfile));

        command::acr_compl_proc proc;
        proc.cmd.check_batch = true;
        proc.fstdin          = tmpfile;
        proc.fstdout         = "|";
        proc.fstderr         = ">&1";
        acr_compl_Start(proc);
        bool any_checkerr = false;
        ind_beg(algo::FileLine_curs, line, proc.from_stdout) {
            algo::Tuple tup;
            if (algo::Tuple_ReadStrptrMaybe(tup, line)
                && tup.head.value == "acr_compl.checkerr") {
                any_checkerr = true;
                u32 id     = 0;
                strptr err = strptr();
                ind_beg(algo::Tuple_attrs_curs, attr, tup) {
                    if (attr.name == "id") {
                        (void)u32_ReadStrptrMaybe(id, attr.value);
                    } else if (attr.name == "err") {
                        err = attr.value;
                    }
                }ind_end;
                // a checkerr that maps to no queued request (garbled output,
                // a version-skewed acr_compl) is still a reported failure;
                // dropping it would pass a run whose validation failed
                abt_md::FCheckReq *req = ind_checkreq_Find(id);
                tempstr report;
                if (req) {
                    report << req->gitfile << ":" << req->lineno << ": " << err;
                } else {
                    report << "abt_md.check_batch"
                           << Keyval("id", id)
                           << Keyval("err", err)
                           << Keyval("comment", "checkerr does not match any queued request");
                }
                prerr(report);
                algo_lib::_db.exit_code++;
            } else if (ch_N(algo::Trimmed(line))) {
                // acr_compl's stderr is folded into this pipe, so anything it
                // says other than a checkerr arrives here: an
                // acr_compl.badreq naming a request that could not be parsed,
                // or a message on the way out of a failing run. Such a line
                // carries no request id to map, so it passes through verbatim
                // and adds nothing to the exit code of its own. What fails the
                // run is the batch as a whole: a checkerr reported above, or,
                // when the batch produced none, the wait-status check below,
                // which is the only place the subprocess status is consulted.
                prerr(line);
            }
        }ind_end;
        acr_compl_Wait(proc);
        // acr_compl exits nonzero when it finds failing requests; those are
        // reported above, one line per request. Nonzero status with no
        // checkerr parsed means the subprocess died before performing the
        // queued validations -- silence here would pass a run whose
        // commands were never checked.
        if (proc.status != 0 && !any_checkerr) {
            prerr("abt_md.check_batch"
                  <<Keyval("status",algo::DescribeWaitStatus(proc.status))
                  <<Keyval("comment","acr_compl -check_batch failed; queued command validations were not performed"));
            algo_lib::_db.exit_code++;
        }
        (void)unlink(Zeroterm(tmpfile));
    }
}

// -----------------------------------------------------------------------------

// Report every dev.mdsection that claims to generate a section it can never
// reach, and fail the run for each one.
//
// A row's path is a SQL-style regex over dev.readmefile.gitfile, and a
// non-empty path is how the table says abt_md writes the section itself; the
// idiom for a hand-written section is an empty path, which GenSectionQ tests
// for. So a row whose path matches no readme at all claims generation and
// delivers none, and nothing notices: the handler simply never runs, and
// acr -check, abt_md -check and normalize all pass with the row in place,
// while a reader of `acr mdsection` concludes the section is generated and
// either hand-edits it expecting abt_md to overwrite the edit, or leaves a gap
// expecting abt_md to fill it.
//
// The general fact is that a regex matching nothing looks exactly like a
// regex whose matches are all fine, so the table can misdescribe the doc set
// indefinitely. Requiring one match turns that silence into a failure at the
// normalize gate, where the next such row is cheap to fix.
//
// The comparison needs a complete readmefile pool to mean anything.
// dev.mdsection is gstatic, so every row is compiled in and present on every
// run, while dev.readmefile is a finput holding whatever -in supplied. A run
// pointed at a single fixture file loads a handful of readmes against the full
// section table, so each row carrying a path reads as dead and the run fails
// on the input rather than on the table. A directory input is a whole ssim
// database and carries the tree's readmefile set, which is the only input this
// check can draw a conclusion from.
void abt_md::Main_CheckMdsection() {
    if (DirectoryQ(_db.cmdline.in)) {
        ind_beg(_db_mdsection_curs,mdsection,_db) {
            if (mdsection.path != "") {
                int nmatch=0;
                ind_beg(_db_readmefile_curs,readmefile,_db) {
                    nmatch += Regx_Match(mdsection.regx_path,readmefile.gitfile);
                }ind_end;
                if (nmatch==0) {
                    prerr("abt_md.mdsection_nomatch"
                          <<Keyval("mdsection",mdsection.mdsection)
                          <<Keyval("path",mdsection.path)
                          <<Keyval("comment","non-empty path matches no dev.readmefile, so this section is never generated"));
                    algo_lib::_db.exit_code++;
                }
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// The ssim tuple that selects the record KEY names, or empty when KEY names no
// ssimfile this build knows or one whose pkey cannot be determined.
// A key is written `<ssimfile>:<pkey>`, and `acr -sel` reads tuples, so the
// key has to be turned into `<ssimfile>  <attr>:<pkey>`. The attribute is the
// name of the ctype's first field, which is not always the ssimfile's own last
// component: `x2db.gwproto` is keyed by `netproto`, so composing the attribute
// from the ssimfile name sends a tuple with no primary key and acr answers
// with nothing -- which reads as "no record has this key" and reports a
// correct reference as broken.
tempstr abt_md::AcrKeyTuple(algo::strptr key) {
    tempstr ret;
    strptr ssimfile = Pathcomp(key, ":LL");
    strptr value    = Pathcomp(key, ":LR");
    abt_md::FSsimfile *p_ssimfile = ind_ssimfile_Find(ssimfile);
    ind_beg(abt_md::_db_ssimfile_curs, cand, abt_md::_db) {
        if (!p_ssimfile && Pathcomp(cand.ssimfile, ".RR") == ssimfile) {
            p_ssimfile = &cand;
        }
    }ind_end;
    abt_md::FCtype *p_ctype = p_ssimfile ? p_ssimfile->p_ctype : NULL;
    abt_md::FField *p_field = p_ctype ? c_field_Find(*p_ctype, 0) : NULL;
    if (p_field && ch_N(value) > 0) {
        ret << p_ssimfile->ssimfile
            << "  "
            << Keyval(Pathcomp(p_field->field, ".RR"), value);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Resolve the queued FCheckKey rows against the ssim database in a single fork
// and report each key that names no record.
// `acr -sel` reads tuples from stdin and prints the records it found, so one
// fork answers "which of these keys exist" for the whole queue: a key is sent
// as the tuple its ssimfile takes, and the reply reconstructs the key it
// answers -- the ssimfile is the type tag and the pkey is the first attribute
// -- so the rows that came back need no correlating id. What is left over is
// the answer: a queued key acr did not return is a key no record has.
// Reading the reply is what makes the check trustworthy, because `acr` reports
// a key that resolves to nothing exactly as it reports one that resolves --
// exit code 0, an empty selection -- so a check built on its status would pass
// every key ever written.
void abt_md::Main_RunKeyCheck() {
    if (checkkey_N()) {
        cstring content;
        ind_beg(_db_checkkey_curs, checkkey, _db) {
            content << AcrKeyTuple(checkkey.key) << eol;
        }ind_end;
        tempstr tmpfile;
        tmpfile << "/tmp/abt_md.checkkey." << getpid();
        vrfy(SafeStringToFile(content, tmpfile),
             tempstr() << "abt_md.tmpfile" << Keyval("path", tmpfile));

        command::acr_proc proc;
        proc.cmd.sel    = true;
        proc.cmd.report = false;
        proc.fstdin     = tmpfile;
        proc.fstdout    = "|";
        acr_Start(proc);
        ind_beg(algo::FileLine_curs, line, proc.from_stdout) {
            algo::Tuple tup;
            if (algo::Tuple_ReadStrptrMaybe(tup, line) && attrs_N(tup) > 0) {
                tempstr key;
                key << tup.head.value << ":" << attrs_qFind(tup, 0).value;
                tempstr shortkey;
                shortkey << Pathcomp(tup.head.value, ".RR")
                         << ":" << attrs_qFind(tup, 0).value;
                // acr answers with the table's full name whichever name the
                // document used, so a key written as a table's short name is
                // looked for under that name too. Both are marked rather than
                // the first that hits, since one sentence may write the same
                // key both ways -- doc's README says `ssimfile:dmmeta.ctype`
                // and `dmmeta.ssimfile:dmmeta.ctype` are one page -- and the
                // form left unmarked would read as missing.
                abt_md::FCheckKey *full = ind_checkkey_Find(key);
                abt_md::FCheckKey *shrt = ind_checkkey_Find(shortkey);
                if (full) {
                    full->found = true;
                }
                if (shrt) {
                    shrt->found = true;
                }
            }
        }ind_end;
        acr_Wait(proc);
        // A subprocess that died returned no records, so every queued key
        // reads as missing. Reporting those as documentation errors would
        // blame the docs for a failure of the check, so the run fails naming
        // the status and no key is reported on that evidence.
        if (proc.status != 0) {
            prerr("abt_md.check_key"
                  <<Keyval("status",algo::DescribeWaitStatus(proc.status))
                  <<Keyval("comment","acr -sel failed; queued acr keys were not checked"));
            algo_lib::_db.exit_code++;
        } else {
            ind_beg(_db_checkkey_curs, checkkey, _db) {
                if (!checkkey.found) {
                    prerr(checkkey.gitfile <<":"<< checkkey.lineno
                          <<": acr.nokey"
                          <<Keyval("key",checkkey.key)
                          <<Keyval("comment","no record has this key"));
                    algo_lib::_db.exit_code++;
                }
            }ind_end;
        }
        (void)unlink(Zeroterm(tmpfile));
    }
}

// -----------------------------------------------------------------------------

// Select readme files by regex or namespace, process each one (generate
// sections, evaluate inline commands, save), then check links and flush
// the batched acr_compl validations; failed readme writes fail the run.
void abt_md::Main() {
    if (_db.cmdline.check || _db.cmdline.print) {
        _db.cmdline.update=false;
    }
    // initialize regx for mdsection
    ind_beg(_db_mdsection_curs,mdsection,_db) {
        Regx_ReadSql(mdsection.regx_match,mdsection.match,true);
        Regx_ReadSql(mdsection.regx_path,mdsection.path,true);
    }ind_end;

    // x-reference readme and ns
    Main_XrefNs();

    // which tables the ssimdb readmes write about, read before any document is generated
    Main_ScanSsimdoc();

    // select md files by regex or by namespace
    ind_beg(_db_readmefile_curs,readmefile,_db) {
        readmefile.select = _db.cmdline.ns.expr != ""
            ? (readmefile.p_ns && Regx_Match(_db.cmdline.ns,readmefile.p_ns->ns))
            : Regx_Match(_db.cmdline.readmefile,readmefile.gitfile);

        if (readmefile.select) {
            verblog("abt_md: select "<<readmefile.gitfile);
        }
    }ind_end;

    ind_beg(_db_ctype_curs,ctype,_db) {
        if (abt_md::FField *field=c_field_Find(ctype,0)) {
            field->ispkey=true;
        }
    }ind_end;

    // count number of selected md files
    int nselect=0;
    ind_beg(_db_readmefile_curs,readmefile,_db) {
        nselect += readmefile.select;
    }ind_end;

    // A link leaves the document it is written in, so checking links is checking the whole
    // graph at once: the target of a link out of the selection is a file this run never
    // read, and the anchor it names is one this run never saw.  A narrowed -check can
    // therefore only skip the link check, and a check that skips the thing it was asked
    // to look at and then exits zero is worse than no check -- a document whose links do
    // not resolve reads as verified.  So the narrowed form is refused here, before any
    // readme is processed, rather than being honoured up to the part that matters.
    // -update is a different request and keeps working on a selection: regenerating one
    // namespace's documents is the ordinary way to use this tool, and that run says on its
    // way past that it checked no links.
    bool wholetree = nselect == readmefile_N();
    if (_db.cmdline.check && !wholetree) {
        prerr("abt_md.narrow_check"
              <<Keyval("nselect",nselect)
              <<Keyval("nreadmefile",readmefile_N())
              <<Keyval("comment","-check reads every readme; drop -readmefile and -ns, or use -update to regenerate a selection"));
        algo_lib::_db.exit_code++;
    }

    // process selected readmes
    // if none are selected, it is an error, unless the selection
    // was an empty string, in which case just update the top-level readme
    if (algo_lib::_db.exit_code==0 && (_db.cmdline.readmefile.expr != "" || _db.cmdline.ns.expr != "")) {
        int nmatch=0;
        // two passes here -- one over all files except README.md,
        // another over all files named REAMDE.md
        // This is beacuse README.md contains table-of-contents references that extract some data
        // from files in the same directory.
        for (int pass=0; pass<2; pass++) {
            ind_beg(_db_readmefile_curs,readmefile,_db) {
                algo::strptr name=Pathcomp(readmefile.gitfile,"/RR");
                if (readmefile.select && ((name=="README.md") == (pass==1))) {
                    nmatch++;
                    ProcessReadme(readmefile);
                }
            }ind_end;
        }
        if (!nmatch) {
            prlog("abt_md.nomatch"
                  <<Keyval("comment","no readmes matched selection. see `acr readme` for the full list"));
            algo_lib::_db.exit_code++;
        }
    }
    // carry the titles recorded above into data/dev/readmefile.ssim.
    Main_SaveReadmetitle();
    // flush batched acr_compl validation requests collected during section
    // processing — one fork covers every queued command line.
    Main_RunBatchCheck();
    // resolve the acr keys collected during section processing — one fork
    // covers every queued key.
    Main_RunKeyCheck();
    // -update implies -check
    if (_db.cmdline.check || _db.cmdline.update) {
        // the mdsection check reads the whole readmefile pool rather than the
        // selection, so it is valid whichever readmes this run asked to process
        Main_CheckMdsection();
        if (wholetree) {
            CheckLinks();
        } else {
            // -check refused a selection above, so what reaches here is a regeneration,
            // which nobody asked to check anything: acr_ed regenerates one namespace's
            // documents as a step of creating a target, and a line about links in the
            // middle of that is noise in the output of a tool doing something else.
            verblog("abt_md: link checking needs every readme; this run checked none");
        }
    }
    // print anchors out
    if (_db.cmdline.print && _db.cmdline.anchor) {
        ind_beg(_db_anchor_curs,anchor,_db) {
            prlog("abt_md.anchor"
                  <<Keyval("anchor",anchor.anchor));
        }ind_end;
    }
    // print links out
    if (_db.cmdline.print && _db.cmdline.link) {
        ind_beg(_db_link_curs,link,_db) {
            prlog("abt_md.link"
                  <<Keyval("location",link.location)
                  <<Keyval("text",link.text)
                  <<Keyval("target",link.target));
        }ind_end;
    }
    // failed readme writes fail the run; they are counted apart from
    // exit_code during the run so the per-readme save gate reads exit_code
    // as generation status
    algo_lib::_db.exit_code += _db.n_writefail;
}
