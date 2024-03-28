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
// Target: abt_md (exe) -- Tool to generate markdown documentation
// Exceptions: yes
// Source: cpp/abt_md/main.cpp
//

#include "include/algo.h"
#include "include/abt_md.h"

// -----------------------------------------------------------------------------

// Return true if readme file README needs section MDSECTION
bool abt_md::NeedSectionQ(abt_md::FMdsection &mdsection, abt_md::FReadme &readme) {
    return Regx_Match(mdsection.regx_path,readme.gitfile);
}

// Extract words from line up until first dash
// E.g. LineKey("#### Some Text - blah") -> "some-text"
// This is similar to the algorithm used by other markdown processors
// to generate anchors, but we allow '.' in anchor name
tempstr abt_md::LineKey(algo::strptr line) {
    strptr str=Pathcomp(line," LR");
    int i=FindStr(str," -- ");
    if (i==-1) {
        i=FindStr(str," - ");
    }
    if (i!=-1) {
        str=ch_FirstN(str,i);
    }
    str=Trimmed(str);
    return MdEscape(str);
}

// Translate characters to create a markdown link
tempstr abt_md::MdEscape(algo::strptr str) {
    tempstr ret;
    for (int i=0; i < str.n_elems; i++) {
        char c = algo::ToLower(str.elems[i]);
        if (c == ':') {
            // skip
        } else {
            if (!algo_lib::IdentCharQ(c)) {
                c = '-';
            }
            ret << c;
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

tempstr abt_md::Preformatted(algo::strptr text) {
    tempstr ret;
    ret << "```\n"<<text;
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

static tempstr NewlineToBr(strptr s) {
    tempstr ret(s);
    Replace(ret,"\n","<br>");
    return ret;
}

void abt_md::AddRow(cstring &out, strptr text1, strptr text2, strptr text3 DFLTVAL(""), strptr text4 DFLTVAL(""), strptr text5 DFLTVAL("")) {
    out <<"|"<<NewlineToBr(text1);
    out<<"|"<<NewlineToBr(text2);
    if (text5!="" || text4!="" || text3!="") {
        out<<"|"<<NewlineToBr(text3);
    }
    if (text5!= "" || text4!="") {
        out<<"|"<<NewlineToBr(text4);
    }
    if (text5!="") {
        out<<"|"<<NewlineToBr(text5);
    }
    out<<"|"<<eol;
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

// -----------------------------------------------------------------------------

// Construct a link to file FNAME
// using the first line of FNAME as the link text
tempstr abt_md::LinkToMd(strptr fname) {
    tempstr ret;
    ind_beg(algo::FileLine_curs,line,fname) {// take first line
        strptr rest=Pathcomp(line," LR");// usually rest of the line
        ret << "["<<rest<<"](/"<<fname<<")"<<eol;
        break;
    }ind_end;
    return ret;
}

// Link within repo to a given file
// The URL is given as a relative pathname with respect to the root
tempstr abt_md::LinkToFileAbs(algo::strptr name, algo::strptr url, algo::strptr anchor DFLTVAL("")) {
    return Link(name, tempstr()<<"/"<<url, anchor);
}

// Link to the md file for given ssimfile
tempstr abt_md::LinkToSsimfile(algo::strptr name, algo::strptr ssimfile, algo::strptr anchor DFLTVAL("")) {
    return LinkToFileAbs(name, tempstr()<<"txt/ssimdb/"<<Pathcomp(ssimfile,".LL")<<"/"<<Pathcomp(ssimfile,".LR")<<".md", anchor);
}

// Link to documentation for given namespace (could be lib,protocol,exe,ssimdb)
tempstr abt_md::LinkToNs(strptr ns, strptr nstype, algo::strptr anchor DFLTVAL("")) {
    return LinkToFileAbs(ns, tempstr()<<"txt/"<<nstype<<"/"<<ns<<"/README.md", anchor);
}

// --------------------------------------------------------------------------------

// Create table of contents link from string
// The string should be a section heading for which markdown generates a link target;
// We just repeat its algorithm and generate a link to that target
tempstr abt_md::LinkToSection(strptr str) {
    str = Trimmed(str);
    tempstr ret;
    ret << "[" << str << "](#";
    for (int i=0; i < str.n_elems; i++) {
        char c = algo::ToLower(str.elems[i]);
        if (c == ':') {
            // skip
        } else {
            if (!algo_lib::IdentCharQ(c)) {
                c = '-';
            }
            ret << c;
        }
    }
    ret << ")";
    return ret;
}

// -----------------------------------------------------------------------------

// Populate global table DIRENT with a directory listing
// matching PATTERN, that can be read in sorted order
// (Dir_curs does not provide sorted order)
void abt_md::PopulateDirent(strptr pattern) {
    dirent_RemoveAll();
    ind_beg(algo::Dir_curs,ent,pattern) {
        abt_md::FDirent &dirent=dirent_Alloc();
        dirent.filename=ent.filename;
        dirent.pathname=ent.pathname;
        dirent.is_dir=ent.is_dir;
        vrfy_(dirent_XrefMaybe(dirent));
    }ind_end;
}

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
// - save readme to disk
void abt_md::UpdateReadme() {
    abt_md::FReadme &readme = *_db.c_readme;
    // create missing file sections
    ind_beg(abt_md::_db_mdsection_curs,mdsection,_db) {
        if (zd_file_section_EmptyQ(mdsection) && NeedSectionQ(mdsection,readme)) {
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

    if (readme.sandbox) {
        command::sandbox_proc sandbox;
        sandbox.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
        sandbox.cmd.reset = true;
        sandbox_ExecX(sandbox);
    }

    // process sections in 2 passes
    // process table of contents on the second pass only
    for (int pass=0; pass<2; pass++) {
        ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
            if (file_section.select && (TocQ(file_section) == pass)) {
                UpdateSection(file_section);
            }
        }ind_end;
    }

    cstring out;
    PrintSections(out);
    if (!_db.cmdline.dry_run) {
        verblog("save "<<readme.gitfile<<eol);
        SafeStringToFile(out,readme.gitfile);
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
    ind_beg(_db_readme_curs,readme,_db) {
        // PATHNAME                    DIR1     DIR2     FILENAME
        // txt/exe/acr/main.md         exe      ""       main
        // txt/script/abc.md           script   script   abc
        // txt/ssimdb/ns/name.md       ssimdb   ssimdb   name
        algo::strptr dir1=Pathcomp(readme.gitfile, "/LR/LL");
        algo::strptr dir2=Pathcomp(readme.gitfile, "/LR/LR/LL");
        algo::strptr filename = Pathcomp(readme.gitfile, "/RR.RL");
        tempstr stray_error;
        if (dir1 == "script" && filename != "README") {// manually check script existence
            if (abt_md::FScriptfile *scriptfile=ind_scriptfile_Find(DirFileJoin("bin",filename))) {
                readme.p_scriptfile=scriptfile;
            } else {
                stray_error="no such script";
            }
        } else if (abt_md::FNs *ns=ind_ns_Find(dir2)) {
            if (filename == "README") {
                if (ns->ns != "") {
                    readme.p_ns=ns;
                    ns->c_readme=&readme;
                }
            } else if (dir1==dmmeta_Nstype_nstype_ssimdb) {
                if (abt_md::FSsimfile *ssimfile =ind_ssimfile_Find(tempstr()<<dir2<<"."<<filename)) {
                    readme.p_ssimfile=ssimfile;
                    readme.p_ctype=ssimfile->p_ctype;
                } else {
                    stray_error="no such ssimfile";
                }
            } else if (dir1==dmmeta_Nstype_nstype_lib || dir1==dmmeta_Nstype_nstype_protocol) {
                if (abt_md::FCtype *ctype =ind_ctype_Find(tempstr()<<dir2<<"."<<filename)) {
                    readme.p_ctype=ctype;
                } else {
                    stray_error="no such ctype";
                }
            } else {
                // other file such as txt/exe/amc/xyz.md
            }
        }
        if (stray_error!="") {
            prerr("abt_md.unrecognized_readme"
                  <<Keyval("readme",readme.gitfile)
                  <<Keyval("comment",stray_error));
            algo_lib::_db.exit_code=1;
        }
        if (readme.select) {
            if (readme.p_scriptfile) {
                verblog("abt_md.readme_scriptfile"
                        <<Keyval("readme",readme.gitfile)
                        <<Keyval("scriptfile",readme.p_scriptfile->gitfile));
            } else if (readme.p_ns) {
                verblog("abt_md.readme_ns"
                        <<Keyval("readme",readme.gitfile)
                        <<Keyval("ns",readme.p_ns->ns));
            } else if (readme.p_ssimfile) {
                verblog("abt_md.readme_ssimfile"
                        <<Keyval("readme",readme.gitfile)
                        <<Keyval("ssimfile",readme.p_ssimfile->ssimfile));
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void abt_md::CheckLinks() {
    ind_beg(_db_link_curs,link,_db) {
        bool good=true;
        // link.target might be #abcd, or file.md#abcd, or /txt/file.md#abcd
        // in the first case, path is empty and anchor is abcd
        // empty path referes to the same file, and is ok
        // in the second case, path is file.md, and is interpreted relative to the
        // document containing the link
        // in the third case, the path is interpreted relative to the repo root
        tempstr path;
        if (StartsWithQ(link.target,"https://") || StartsWithQ(link.target,"http://")) {
            good=true;// don't check URLs
        } else if (StartsWithQ(link.target,"/")) {
            // /txt/file.md#abcd -> txt/file.md
            path=Pathcomp(link.target,"/LR#LL");
        } else if (path =="") {
            // link.location is filename.md:22, this yields filename.md
            path=Pathcomp(link.location,":RL");
        } else {
            // if link.locataion is txt/file.md:22
            // and link.target is anotherfile.md#anchor,
            // path is txt/anotherfile.md
            path=DirFileJoin(link.target,DirFileJoin(GetDirName(link.location),Pathcomp(link.target,"#LL")));
        }
        if (path != "") {
            if (!FileQ(path)) {
                good=false;
                prlog(link.location<<": "
                      <<Keyval("target",link.target)
                      <<Keyval("path",path)
                      <<Keyval("comment","target file doesn't exist"));
            }
            tempstr anchor(Pathcomp(link.target,"#LR"));
            tempstr fullanchor = tempstr() << path << "#"<<anchor;
            if (good && anchor!="" && !ind_anchor_Find(fullanchor)) {
                good=false;
                prlog(link.location<<": "
                      <<Keyval("target",link.target)
                      <<Keyval("anchor",anchor)
                      <<Keyval("comment","link path is OK, but the anchor doesn't exist"));
            }
        }
        if (!good) {
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void abt_md::Main() {
    if (_db.cmdline.check || _db.cmdline.print) {
        _db.cmdline.update=false;
    }

    // initialize regx for mdsection
    ind_beg(_db_mdsection_curs,mdsection,_db) {
        Regx_ReadSql(mdsection.regx_match,mdsection.match,true);
        Regx_ReadSql(mdsection.regx_path,mdsection.path,true);
    }ind_end;

    // select md files by regex or by namespace
    ind_beg(_db_readme_curs,readme,_db) {
        readme.select = _db.cmdline.ns.expr != ""
            ? Regx_Match(_db.cmdline.ns,Pathcomp(readme.gitfile,"/LR/LR/LL"))
            : Regx_Match(_db.cmdline.readme,readme.gitfile);

        if (readme.select) {
            verblog("abt_md: select "<<readme.gitfile);
        }
    }ind_end;

    // x-reference readme and ns
    Main_XrefNs();

    ind_beg(_db_ctype_curs,ctype,_db) {
        if (abt_md::FField *field=c_field_Find(ctype,0)) {
            field->ispkey=true;
        }
    }ind_end;

    // count number of selected md files
    int nselect=0;
    ind_beg(_db_readme_curs,readme,_db) {
        nselect += readme.select;
    }ind_end;

    // process selected readmes
    // if none are selected, it is an error, unless the selection
    // was an empty string, in which case just update the top-level readme
    if (algo_lib::_db.exit_code==0 && (_db.cmdline.readme.expr != "" || _db.cmdline.ns.expr != "")) {
        int nmatch=0;
        ind_beg(_db_readme_curs,readme,_db) if (readme.select) {
            nmatch++;
            verblog("processing "<<readme.gitfile);
            _db.c_readme = &readme;
            LoadSections(readme);
            if (_db.cmdline.update) {
                UpdateReadme();
            }
            // scan sections for links and anchors
            ScanLinksAnchors();
            if (_db.cmdline.print && !_db.cmdline.link && !_db.cmdline.anchor) {
                cstring out;
                PrintSections(out);
                prlog(out);
            }
        }ind_end;

        if (!nmatch) {
            prlog("abt_md.nomatch"
                  <<Keyval("comment","no readmes matched selection. see `acr readme` for the full list"));
            algo_lib::_db.exit_code=1;
        }
    }
    // can't check links if not all files were loaded
    // -update implies -check
    if (_db.cmdline.check || _db.cmdline.update) {
        if (nselect < readme_N()) {
            verblog("abt_md: disable link checking, not all files being loaded");
        } else {
            CheckLinks();
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
}
