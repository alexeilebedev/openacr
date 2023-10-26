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
// Source: cpp/abt/abt_md.cpp
//

#include "include/algo.h"
#include "include/abt_md.h"

// -----------------------------------------------------------------------------

static int Sortkey(abt_md::FMdsection &mdsection, int i) {
    return mdsection.rowid * 10000 + i;
}

// --------------------------------------------------------------------------------

// Create table of contents link from string
// History of SKNF -> [History of SKNF](#history-of-sknf)
static tempstr TocLink(strptr str) {
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

// Determine header depth level of current line by counting leading #'s
static int GetHeaderLevel(strptr line) {
    int i=0;
    while (i<line.n_elems && line[i]=='#') {
        i++;
    }
    return i;
}

// -----------------------------------------------------------------------------

// Scan string FROM for markdown header indicators
// (##, ###, #### etc)
// And add them as sections to the table of contents, with 3 spaces per '#'.
// At a certain level of indentation, stop wasting lines and use "; " as separator
// between chapters
static void AppendToc(strptr from, cstring &to) {
    int prevlevel=0;
    ind_beg(Line_curs,line,from) {
        int level = GetHeaderLevel(line);
        if (level>1 && level<line.n_elems && line[level]==' ') {
            tempstr toclink = TocLink(RestFrom(line,level+1));
            if (prevlevel == level && level >= 3) {
                to << "; "<< toclink;
            } else {
                if (ch_qLast(to) != '\n') {
                    to << eol;
                }
                char_PrintNTimes(' ',to,(level-1)*3);
                to << "* " << toclink << eol;
            }
            prevlevel=level;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Read contents of FILENAME and output a string
// corresponding to it.
// If INL flag is set, then the file is returned as-is.
// If INL is false, then the first line of the file is taken,
// and a markdown hyperlink pointing to the file is created instead.
static tempstr ReadmeContents(strptr filename, bool inl) {
    tempstr contents(FileToString(filename));
    if (!inl) {
        tempstr abridged;
        ind_beg(Line_curs,line,contents) {// take first line
            abridged << line << eol;
            break;
        }ind_end;
        abridged << "[See "<<filename<<"]("<<filename<<")"<<eol;
        contents = abridged;
    }
    return contents;
}

// -----------------------------------------------------------------------------

// Load specified readme into memory as FILE_SECTION records
void abt_md::LoadReadme(abt_md::FReadme &readme) {
    _db.c_readme = &readme;

    ind_replvar_Cascdel(_db.R);// clean up
    if (readme.c_readmens) {
        Set(_db.R,"$ns",readme.c_readmens->ns);
    }

    // parse file into sections, create FileSection entries
    file_section_RemoveAll();
    abt_md::FileSection *cur_section=NULL;
    ind_beg(algo::FileLine_curs,line,readme.gitfile) {
        if (StartsWithQ(line,"##") || !cur_section) {
            cur_section=&file_section_Alloc();
            cur_section->title=line;
            cur_section->p_mdsection = &abt_md_mdsection_Content;
            ind_beg(_db_mdsection_curs,mdsection,_db) {
                // don't match title unless it's the first ##
                if (&mdsection == &abt_md_mdsection_Title && zd_file_section_N(abt_md_mdsection_Title)>0) {
                    continue;
                }
                if (Regx_Match(mdsection.match_regx,cur_section->title)) {
                    cur_section->p_mdsection=&mdsection;
                }
            }ind_end;
            cur_section->sortkey=Sortkey(*cur_section->p_mdsection, file_section_N());
            cur_section->select=Regx_Match(_db.cmdline.section,line);
            file_section_XrefMaybe(*cur_section);// index by mdsection
        } else {
            if (cur_section) {
                cur_section->text << line << eol;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print a single section to string
void abt_md::PrintSection(abt_md::FileSection &file_section, cstring &out) {
    out << TrimmedRight(file_section.title) << eol << eol;
    out << TrimmedRight(file_section.text) << eol << eol;
}

// -----------------------------------------------------------------------------

// Print sections from readme matching command line regx
void abt_md::PrintReadme() {
    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        if (file_section.select) {
            tempstr out;
            PrintSection(file_section,out);
            prlog(out);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Update/evaluate specified section
void abt_md::UpdateSection(abt_md::FileSection &file_section) {
    abt_md::FReadme &readme = *_db.c_readme;
    verblog("abt_md.file_section"
            <<Keyval("mdsection",file_section.p_mdsection->mdsection)
            <<Keyval("sortkey",file_section.sortkey)
            <<Keyval("length",ch_N(file_section.text)));
    // run generator -- skip 'per-namespace' generator if
    // processing non-namespace-related readme
    if (!file_section.p_mdsection->perns || readme.c_readmens) {
        // run generator function on section --
        // this may generate content and/or replace file section fields
        file_section.p_mdsection->step(file_section);
    }
    // evaluate section content if it's not fully generated
    verblog("evaluating section "<<file_section.title);
    cstring out;
    bool inblock=false;
    bool leadempty=true;
    ind_beg(algo::Line_curs,line,file_section.text) {
        if (line=="" && leadempty) {
            // ignore leading empty line
        } else if (StartsWithQ(line,"inline-command: ")) {
            out << line << eol;
            verblog(readme.gitfile<<": eval "<<line);
            tempstr cmd(Pathcomp(line," LR"));
            if (readme.sandbox) {
                command::sandbox_proc sandbox;
                sandbox.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
                cmd_Alloc(sandbox.cmd) << "bash";
                cmd_Alloc(sandbox.cmd) << "-c";
                cmd_Alloc(sandbox.cmd) << cmd;
                sandbox.fstdout = ">temp/abt_md.out";
                sandbox.fstderr = ">&1";
                sandbox_Exec(sandbox);
                if (readme.filter == "") {
                    readme.filter = "cat";
                }
                cmd = tempstr()<<readme.filter<<" temp/abt_md.out";
            }
            cmd << " 2>&1";
            out << SysEval(cmd,FailokQ(true),1024*1024);
            inblock = true;
        } else {
            if (inblock && StartsWithQ(line, "```")) {
                inblock = false;
            }
            if (!inblock) {
                out << line << eol;
            }
        }
        if (line != "") {
            leadempty =false;
        }
    }ind_end;
    file_section.text=out;
    verblog("new content length "<<ch_N(file_section.text));
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
        if (zd_file_section_EmptyQ(mdsection) && mdsection.perns && readme.c_readmens) {
            abt_md::FileSection &section=file_section_Alloc();
            section.p_mdsection=&mdsection;
            section.title = mdsection.match;
            Replace(section.title,"%","");
            section.select=Regx_Match(_db.cmdline.section,section.title);
            section.sortkey = Sortkey(mdsection,1);
            file_section_XrefMaybe(section);
            verblog(readme.gitfile<<": creating missing section "<<section.title);
        }
    }ind_end;

    if (readme.sandbox) {
        command::sandbox_proc sandbox;
        sandbox.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
        sandbox.cmd.reset = true;
        sandbox_ExecX(sandbox);
    }

    // process sections
    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        if (file_section.select) {
            UpdateSection(file_section);
        }
    }ind_end;

    if (_db.cmdline.dry_run) {
        // just output to screen
        PrintReadme();
    } else {
        // output sections in sorted order
        // skip empty sections
        cstring out;
        ind_beg(_db_bh_file_section_curs,file_section,_db) {
            if (file_section.p_mdsection == &abt_md_mdsection_Title || Trimmed(file_section.text) != "") {
                verblog("emitting section "<<file_section.title);
                PrintSection(file_section,out);
            }
        }ind_end;

        // save file
        SafeStringToFile(out,readme.gitfile);
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Title(abt_md::FileSection &section) {
    Set(_db.R,"$comment",_db.c_readme->c_readmens->p_ns->comment);
    section.title="";
    Ins(&_db.R,section.title,"## $ns: $comment",false);
}

void abt_md::mdsection_Syntax(abt_md::FileSection &section) {
    section.text="";
    tempstr out = SysEval(Subst(_db.R,"$ns -h"),FailokQ(true),1024*1024);
    if (Trimmed(out)!="") {
        Ins(&_db.R,section.text,"```");
        section.text << out;
        Ins(&_db.R,section.text,"```");
    }
}

void abt_md::mdsection_Description(abt_md::FileSection &) {
}

void abt_md::mdsection_Content(abt_md::FileSection &) {
}

void abt_md::mdsection_Limitations(abt_md::FileSection &) {
}

void abt_md::mdsection_Example(abt_md::FileSection &) {
}

void abt_md::mdsection_Inputs(abt_md::FileSection &section) {
    section.text = "";// replace section text
    tempstr inputs = SysEval(Subst(_db.R,"acr_in $ns"),FailokQ(true),1024*1024);
    algo_lib::FFildes read;
    command::acr_in_proc acr_in;
    acr_in.cmd.ns.expr=_db.c_readme->c_readmens->ns;
    cstring text;
    Ins(&_db.R,text, "`$ns` takes the following tables on input:");
    Ins(&_db.R,text, "```");
    Ins(&_db.R,text, "CTYPE\tCOMMENT");
    int nctype=0;
    ind_beg(algo::FileLine_curs,line,acr_in_StartRead(acr_in,read)) {
        dmmeta::Ssimfile ssimfile;
        if (Ssimfile_ReadStrptrMaybe(ssimfile,line)) {
            if (abt_md::FSsimfile *fssimfile=ind_ssimfile_Find(ssimfile.ssimfile)) {
                Ins(&_db.R,text,tempstr()<<fssimfile->ctype<<"\t"<<fssimfile->p_ctype->comment);
                nctype++;
            }
        }
    }ind_end;
    Ins(&_db.R,text, "```");
    // omit section text if no inputs
    if (nctype) {
        section.text << Tabulated(text, "\t", "ll", 2);
    }
}

void abt_md::mdsection_Tests(abt_md::FileSection &section) {
    section.text = "";// replace section text
    tempstr out = SysEval(Subst(_db.R,"acr comptest:$ns.% -field comptest,comment"),FailokQ(true),1024*1024);
    if (Trimmed(out)!="") {
        Ins(&_db.R,section.text, "The following component tests are defined for `$ns`:");
        Ins(&_db.R,section.text, "```");
        section.text << out;
        Ins(&_db.R,section.text, "```");
    }
}

void abt_md::mdsection_Copyright(abt_md::FileSection &) {
}

// -----------------------------------------------------------------------------

void abt_md::Main() {
    // nothing = all
    if (_db.cmdline.readme.expr=="" && _db.cmdline.ns.expr=="") {
        Regx_ReadSql(_db.cmdline.readme,"%",true);
    }

    // initialize regx for mdsection
    ind_beg(_db_mdsection_curs,mdsection,_db) {
        Regx_ReadSql(mdsection.match_regx,mdsection.match,true);
    }ind_end;

    ind_beg(_db_readme_curs,readme,_db) {
        readme.select = Regx_Match(_db.cmdline.readme,readme.gitfile);
    }ind_end;
    ind_beg(_db_readmens_curs,readmens,_db) {
        if (Regx_Match(_db.cmdline.ns,readmens.ns)) {
            readmens.p_readme->select=true;
        }
    }ind_end;
    
    int nmatch=0;
    ind_beg(_db_readme_curs,readme,_db) if (readme.select) {
        nmatch++;
        verblog("processing "<<readme.gitfile);
        LoadReadme(readme);
        if (_db.cmdline.print) {
            PrintReadme();
        } else {
            UpdateReadme();
        }
    }ind_end;

    if (!nmatch) {
        prlog("abt_md.nomatch"
              <<Keyval("comment","no readmes matched selection. see `acr readme` for the full list"));
        algo_lib::_db.exit_code=1;
    }

    if (!_db.cmdline.dry_run && !_db.cmdline.print) {
        verblog("generating top-level readme file");
        // build top-level README.md file
        cstring text;
        cstring out;
        out << "This file was created with 'abt_md' from files in [txt/](txt/) -- *do not edit*\n\n";
        out << "## Table Of Contents\n";
        ind_beg(_db_readme_curs,readme,_db) {
            text << eol;
            text << ReadmeContents(readme.gitfile, readme.inl);
        }ind_end;
        AppendToc(text,out);
        out << eol;
        out << text;
        StringToFile(out, "README.md");
    }
}
