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
// Target: abt_md (exe)
// Exceptions: yes
// Source: cpp/abt/abt_md.cpp
//

#include "include/algo.h"
#include "include/abt_md.h"

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

void abt_md::InlineReadme(abt_md::FReadme &readme) {
    cstring out;
    bool inblock=false;
    if (readme.sandbox) {
        command::sandbox_proc sandbox;
        sandbox.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
        sandbox.cmd.reset = true;
        sandbox_ExecX(sandbox);
    }
    ind_beg(algo::FileLine_curs,line,readme.gitfile) {
        if (StartsWithQ(line,"inline-command: ")) {
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
    }ind_end;
    SafeStringToFile(out,readme.gitfile);
}

// -----------------------------------------------------------------------------

void abt_md::Main() {
    ind_beg(_db_readme_curs,readme,_db) {
        if (Regx_Match(_db.cmdline.readme,readme.gitfile)) {
            verblog("processing "<<readme.gitfile);
            InlineReadme(readme);
        }
    }ind_end;

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
