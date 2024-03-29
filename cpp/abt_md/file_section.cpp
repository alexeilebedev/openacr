// Copyright (C) 2024 AlgoRND
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
// Source: cpp/abt_md/file_section.cpp
//

#include "include/algo.h"
#include "include/abt_md.h"

// -----------------------------------------------------------------------------

// - Check that section doesn't have nested code-blocks (~~~) and preformatted blocks (```).
// - Check that section doesn't have unbalanced code blocks and preformatted blocks
// - Check that section doesn't contain headers of higher significance than the title line
// (i.e. title line is ### but inside there is a ##)
void abt_md::CheckSection(abt_md::FFileSection &file_section) {
    int title_level=GetHeaderLevel(file_section.title);
    int backticks_start_line=0;
    int codeblock_start_line=0;
    ind_beg(algo::Line_curs,line,file_section.text) {
        int lineno = file_section.firstline + ind_curs(line).i;
        vrfy_(lineno>0);
        if (StartsWithQ(line,"```")) {
            backticks_start_line = backticks_start_line ? 0 : lineno;
        } else if (StartsWithQ(line,"~~~")) {
            codeblock_start_line = codeblock_start_line ? 0 : lineno;
        }
        if (codeblock_start_line && backticks_start_line) {
            prlog(_db.c_readme->gitfile<<":"<<lineno
                  <<": "<<"Interleaved code block & performatted block");
            prlog("    "<<line);
        }
        if (!backticks_start_line && !codeblock_start_line) {
            int level=GetHeaderLevel(line);
            if (level > 0 && level <= title_level) {
                prlog(_db.c_readme->gitfile<<":"<<lineno
                      <<": "<<"Bad title indentation "<<level<<"; header has "<<title_level);
                prlog("    "<<line);
                algo_lib::_db.exit_code=1;
            }
        }
    }ind_end;
    if (backticks_start_line) {
        prlog(_db.c_readme->gitfile<<":"<<backticks_start_line<<": "<<"Unterminated preformatted block (```)");
        algo_lib::_db.exit_code=1;
    }
    if (codeblock_start_line) {
        prlog(_db.c_readme->gitfile<<":"<<codeblock_start_line<<": "<<"Unterminated code block (~~~)");
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

// Scan section and save human-entered text into hash ind_human_text
// The sections where human text begins needs preserving are given by attribute dev.mdsection.genlist
// Any line that matches genlist header string is the beginning of new human text
// The key for the entry is given by Pathcomp(line,<key pathcomp>).
void abt_md::SaveHumanText(abt_md::FFileSection &file_section) {
    abt_md::FMdsection &mdsection = *file_section.p_mdsection;
    ind_human_text_Cascdel();
    if (mdsection.genlist != "") {
        abt_md::FHumanText *cur_human_text=&abt_md::ind_human_text_GetOrCreate("");// initial tet
        ind_beg(Line_curs,line,file_section.text) {
            if (StartsWithQ(line,mdsection.genlist)) {
                tempstr name=LineKey(line);
                abt_md::FHumanText &ht=abt_md::ind_human_text_GetOrCreate(name);
                cur_human_text=&ht;
            } else {
                cur_human_text->text << line << eol;
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Scan section and restore any human text that has been saved before
// This allows abt_md to easily re-generate section content headings
// and re-apply human-written annotations from before.
void abt_md::RestoreHumanText(abt_md::FFileSection &file_section) {
    abt_md::FMdsection &mdsection = *file_section.p_mdsection;
    if (mdsection.genlist != "") {
        cstring newtext;
        if (abt_md::FHumanText *front=abt_md::ind_human_text_Find("")) {
            newtext << front->text << eol;
        }
        ind_beg(Line_curs,line,file_section.text) {
            newtext << line << eol;
            if (StartsWithQ(line,mdsection.genlist)) {
                tempstr name=LineKey(line);
                if (abt_md::FHumanText *ht=abt_md::ind_human_text_Find(name)) {
                    newtext << Trimmed(ht->text) << eol << eol;
                    ht->used=true;
                }
            }
        }ind_end;
        file_section.text = newtext;
    }
}

// -----------------------------------------------------------------------------

// Insert anchors in FILE_SECTION, one for each section / subsection
// (The anchors are stripped when loading the section for updating)
// Delete any double blank links outside of code/preformatted blocks
// In the end, if the section body doesn't contain any non-empty lines except anchors,
// leave the section body empty (it will not be printed back to the file).
void abt_md::RewriteAnchors(abt_md::FFileSection &file_section) {
    cstring out;
    int nanchor=0;
    AddAnchor(LineKey(file_section.title),out);
    nanchor++;
    bool backticks=false;
    bool codeblock=false;
    int nlines=0;
    int n_conseq_empty=0;// run of empty lines
    ind_beg(algo::Line_curs,line,file_section.text) {
        int nbefore=out.ch_n;
        bool empty=Trimmed(line)=="";
        n_conseq_empty = !empty ? 0 : n_conseq_empty+1;// count empty lines
        out << line << eol;
        nlines += !empty;
        if (StartsWithQ(line,"```")) {
            backticks=!backticks;
        } else if (StartsWithQ(line,"~~~")) {
            codeblock=!codeblock;
        } else if (!backticks && !codeblock) {
            if (n_conseq_empty>1 || StartsWithQ(line, "<a href=\"#") || StartsWithQ(line,"<a name=\"")) {
                out.ch_n=nbefore;
                nlines -= !empty;
                // strip anchors -- we are regenerating them
            } else {
                int level=GetHeaderLevel(line);
                if (level>=1 && level<=4) {
                    AddAnchor(tempstr()<<LineKey(line), out);
                    nanchor++;
                }
            }
        }
    }ind_end;
    verblog("abt_md.rewrite_anchors"
            <<Keyval("title",file_section.title)
            <<Keyval("nlines",nlines)
            <<Keyval("nanchor",nanchor));
    file_section.text = nlines>0 ? out : cstring();
}

// -----------------------------------------------------------------------------

// Scan for links and anchors in section SECTION
void abt_md::ScanLinksAnchors() {
    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        bool backticks=false;
        bool codeblock=false;
        ind_beg(algo::Line_curs,line,file_section.text) {
            if (StartsWithQ(line,"```")) {
                backticks=!backticks;
            } else if (StartsWithQ(line,"~~~")) {
                codeblock=!codeblock;
            } else if (!backticks && !codeblock) {
                int bracket_start=-1;
                int bracket_end=-1;
                int paren_start=-1;
                int paren_end=-1;
                for (int i=0; i<line.n_elems; i++) {
                    if (line.elems[i]=='[') {
                        bracket_start=i;
                    } else if (line.elems[i]==']') {
                        bracket_end=i;
                    } else if (line.elems[i]=='(') {
                        paren_start=i;
                    } else if (line.elems[i]==')') {
                        paren_end=i;
                        if (bracket_start!=-1 && bracket_end!=-1 && paren_start==bracket_end+1) {
                            abt_md::FLink &link=link_Alloc();
                            link.location << _db.c_readme->gitfile<<":"<<(file_section.firstline + 1 + ind_curs(line).i);
                            link.text=ch_GetRegion(line,bracket_start+1,bracket_end-bracket_start-1);
                            link.target=ch_GetRegion(line,paren_start+1,paren_end-paren_start-1);
                        }
                    }
                }
                if (StartsWithQ(line,"<a href=\"")) {
                    strptr name=Pathcomp(line,"\"LR#LR\"LL");
                    abt_md::FAnchor &anchor=anchor_Alloc();
                    anchor.anchor << _db.c_readme->gitfile<<"#"<<name;
                    if (!anchor_XrefMaybe(anchor)) {
                        anchor_RemoveLast();
                    }
                }
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Update/evaluate specified section
// The algorithm is as follows:
// - First, check if the generator needs to be run. This is true if the template's (mdsection's) regx
// matches the readme pathname. If so, run the generator.
// The generator typically replaces the entire section contents with something computed,
// but a special case is allowed where the generator produces a list of subsections (e.g. one entry
// per file in some directory, or one entry per row in some table), and these subsections
// have human-entered text. The human-entered text is saved into a side table and later restored.
// See functions SaveHumanText / RestoreHumanText
// - Then, evaluate inline-command inside the section by running the command (possibly in sandbox)
// - Then, scan the resulting section text again and generate anchors for the main
//   section and for any subsection. Save the anchor to a global anchor table.
//   Find any links ([link text](link target)) and save them to the global link table
// - Finally, check that the resulting section body doesn't have unbalanced backticks
// (it's possible since the inline-command output might contain backticks).
void abt_md::UpdateSection(abt_md::FFileSection &file_section) {
    // evaluate section content if it's not fully generated
    abt_md::FReadme &readme = *_db.c_readme;
    abt_md::FMdsection &mdsection = *file_section.p_mdsection;
    verblog("abt_md.update_section_begin"
            <<Keyval("mdsection",mdsection.mdsection)
            <<Keyval("title",file_section.title)
            <<Keyval("sortkey",file_section.sortkey)
            <<Keyval("length",ch_N(file_section.text)));
    if (NeedSectionQ(mdsection,readme)) {
        // run generator function on section --
        // this may generate content and/or replace file section fields
        SaveHumanText(file_section);
        file_section.p_mdsection->step(file_section);
        RestoreHumanText(file_section);
    }
    cstring out;
    bool inlinecommand=false;
    bool backticks=false;// inside backticks block
    bool codeblock=false;// inside code block
    bool leadempty=true;
    // expand inline-commands
    ind_beg(algo::Line_curs,line,file_section.text) {
        int lineno = file_section.firstline + ind_curs(line).i + 1/*skip title*/;
        if (line=="" && leadempty) {
            // ignore leading empty line
        } else if ((codeblock || backticks) && StartsWithQ(line,"inline-command: ")) {
            out << line << eol;
            verblog(readme.gitfile<<": eval "<<line);
            tempstr cmd(Pathcomp(line," LR"));
            int rc=0;
            if (readme.sandbox) {
                command::sandbox_proc sandbox;
                sandbox.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
                cmd_Alloc(sandbox.cmd) << "bash";
                cmd_Alloc(sandbox.cmd) << "-c";
                cmd_Alloc(sandbox.cmd) << cmd;
                sandbox.fstdout = ">temp/abt_md.out";
                sandbox.fstderr = ">&1";
                rc = sandbox_Exec(sandbox);
            } else {
                command::bash_proc bash;
                bash.cmd.c = cmd;
                bash.fstdout = ">temp/abt_md.out";
                bash.fstderr = ">&1";
                rc = bash_Exec(bash);
            }
            if (rc) {
                prerr(readme.gitfile<<":"<<lineno<<": command failed: "<<cmd);
                algo_lib::_db.exit_code=rc;
            }
            if (readme.filter == "") {
                readme.filter = "cat";
            }
            cmd = tempstr()<<readme.filter<<" temp/abt_md.out";
            try {
                out << SysEval(cmd,FailokQ(true),1024*1024*10);
            } catch (algo_lib::ErrorX &x) {
                prerr(readme.gitfile<<":"<<lineno<<": command failed: "<<cmd);
                algo_lib::_db.exit_code=1;
            }
            inlinecommand=true;
        } else if (StartsWithQ(line, "```")) {
            out << line << eol;
            inlinecommand=false;
            backticks = !backticks;
        } else if (StartsWithQ(line, "~~~")) {
            out << line << eol;
            inlinecommand=false;
            codeblock = !codeblock;
        } else {
            // regular line : output unless it's part of command line output
            // that's being replaced
            if (!inlinecommand) {
                out << line << eol;
            }
        }
        if (line != "") {
            leadempty =false;
        }
    }ind_end;
    file_section.text=out;
    RewriteAnchors(file_section);
    CheckSection(file_section);
    verblog("abt_md.update_section_end"
            <<Keyval("mdsection",mdsection.mdsection)
            <<Keyval("title",file_section.title)
            <<Keyval("length",ch_N(file_section.text)));
}

// -----------------------------------------------------------------------------

bool abt_md::TocQ(abt_md::FFileSection &section) {
    return section.p_mdsection && section.p_mdsection->mdsection == abt_md_mdsection_Toc.mdsection;
}

// -----------------------------------------------------------------------------

bool abt_md::TitleQ(abt_md::FFileSection &section) {
    return section.p_mdsection && section.p_mdsection->mdsection == abt_md_mdsection_Title.mdsection;
}

// -----------------------------------------------------------------------------

// Load specified readme into memory as FILE_SECTION records
// the section table is global and is wiped on every readme.
// Only lines starting with ## or ### are considered FILE_SECTIONS
void abt_md::LoadSections(abt_md::FReadme &readme) {
    ind_replvar_Cascdel(_db.R);// clean up
    if (readme.p_ns) {
        Set(_db.R,"$ns",readme.p_ns->ns);
    }
    file_section_RemoveAll();
    abt_md::FFileSection *cur_section=NULL;
    bool backticks=false;// inside backticks block
    bool codeblock=false;// inside code block
    ind_beg(algo::FileLine_curs,line,readme.gitfile) {
        bool skip=false;
        if (StartsWithQ(line,"```")) {
            backticks=!backticks;
        } else if (StartsWithQ(line,"~~~")) {
            codeblock=!codeblock;
        } else if (!backticks && !codeblock) {
            int level=GetHeaderLevel(line);
            if (level>=1 && level<=3) {
                cur_section=&file_section_Alloc();
                cur_section->title=line;
                cur_section->p_mdsection = &abt_md_mdsection_Content;
                cur_section->firstline = ind_curs(line).i+1;
                ind_beg(_db_mdsection_curs,mdsection,_db) {
                    // don't match title unless it's the first ##
                    if (&mdsection == &abt_md_mdsection_Title && zd_file_section_N(abt_md_mdsection_Title)>0) {
                        continue;
                    }
                    if (Regx_Match(mdsection.regx_match,cur_section->title)) {
                        cur_section->p_mdsection=&mdsection;
                    }
                }ind_end;
                verblog(readme.gitfile<<":"<<cur_section->firstline
                        <<Keyval("section",line)
                        <<Keyval("mdsection",(cur_section->p_mdsection ? cur_section->p_mdsection->mdsection : dev::MdsectionPkey())));
                cur_section->sortkey=Sortkey(*cur_section->p_mdsection, file_section_N());
                cur_section->select=Regx_Match(_db.cmdline.section,line);
                vrfy_(file_section_XrefMaybe(*cur_section));// index by mdsection
                skip=true;
            }
        }
        if (!skip) {
            if (cur_section) {
                cur_section->text << line << eol;
            } else {
                verblog("# discard "<<line);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print a single section to string
void abt_md::PrintSection(abt_md::FFileSection &file_section, cstring &out) {
    out << TrimmedRight(file_section.title) << eol;
    out << TrimmedRight(file_section.text) << eol << eol;
}

// -----------------------------------------------------------------------------

// Print all sections to string OUT
// Sections with empty text are deleted, except for title
// (Note that contents of generated sections are not considered sections, so they may be empty
//   and will not be deleted)
void abt_md::PrintSections(cstring &out) {
    ind_beg(abt_md::_db_bh_file_section_curs,file_section,_db) {
        bool select = file_section.select || !_db.cmdline.dry_run;
        select = select && (file_section.p_mdsection == &abt_md_mdsection_Title || Trimmed(file_section.text) != "");
        if (select) {
            PrintSection(file_section,out);
        }
    }ind_end;
}
