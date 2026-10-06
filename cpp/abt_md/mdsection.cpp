// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2024 AlgoRND
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
// Target: abt_md (exe) -- Tool to generate markdown documentation
// Exceptions: yes
// Source: cpp/abt_md/mdsection.cpp -- Markdown section handlers - toc, inline command, links
//

#include "include/algo.h"
#include "include/abt_md.h"


// -----------------------------------------------------------------------------
static void HeaderCtype(algo_lib::FTxttbl &txttbl){
    AddRow(txttbl);
    AddCol(txttbl,"Field");
    AddCol(txttbl,abt_md::LinkToSsimfile("Type","dmmeta.ctype"));
    AddCol(txttbl,abt_md::LinkToSsimfile("Reftype","dmmeta.reftype"));
    AddCol(txttbl,"Default");
    AddCol(txttbl,"Comment");
}
// -----------------------------------------------------------------------------
static tempstr SetItalics(strptr text,bool is_substr){
    tempstr italics;
    if (text!="" && is_substr) {
        italics="*";
    }
    return tempstr()<<italics<<text<<italics;
}
// -----------------------------------------------------------------------------
void abt_md::DescribeCtype(abt_md::FCtype *ctype, cstring &out) {
    if (ctype->c_ssimfile) {
        tempstr ssimfname = SsimFname("data",ctype->c_ssimfile->ssimfile);
        out << "* file:"<<LinkToFileAbs(ssimfname,ssimfname)<<eol;
    }
    tempstr fldfunc;
    algo::ListSep ls(", ");
    if (c_field_N(*ctype)) {
        out<<eol;
        algo_lib::FTxttbl txttbl;
        HeaderCtype(txttbl);
        ind_beg(ctype_c_field_curs,field,*ctype) {
            bool is_substr=field.c_substr;
            tempstr comment(field.comment);
            tempstr arg = LinkToCtype(*field.p_arg);
            if (is_substr) {
                fldfunc << ls << name_Get(field);
                comment << "\n" << field.c_substr->expr <<" of "<<name_Get(*field.c_substr->p_srcfield);
                // mark fldfunc by italics
            }
            AddRow(txttbl);
            AddCol(txttbl,SetItalics(name_Get(field),is_substr));
            AddCol(txttbl,SetItalics(arg,is_substr));
            AddCol(txttbl,SetItalics(LinkToReftype(field.reftype),is_substr));
            AddCol(txttbl,SetItalics(field.dflt.value,is_substr));
            AddCol(txttbl,SetItalics(comment,is_substr));
        }ind_end;
        if (fldfunc!=""){
            out << "italicised fields: "
                << "*" << fldfunc << "*"
                << " are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields"
                << eol
                << eol;
        }
        FTxttbl_Markdown(txttbl,out);
        out<<eol;
    }
}


// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void abt_md::mdsection_Options(abt_md::FFileSection &section) {
    if (_db.c_readmefile->p_ns) {
        abt_md::FCtype *ctype = ind_ctype_Find(tempstr()<<"command."<<_db.c_readmefile->p_ns->ns);
        section.text = "";
        if (ctype) {
            ind_beg(ctype_c_field_curs,field,*ctype) {
                section.text << "#### -"<<name_Get(field)<<" -- "<<field.comment << eol;
            }ind_end;
        }
    }
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Update title of document
// - For a page whose subject set a title, use that title
// - For namespace, pull namespace name and comment from ns table
// - For script, use script name and comment from scriptfile table
// For all other cases, leave title as-is
// Section contents are user-defined
//
// A title the subject set comes first, ahead of the ctype the page also names.  A
// command's page documents the command a person types, so it is titled with the
// command; the ctype that parses the command's options is a fact about the
// implementation and not what the reader came for.
void abt_md::mdsection_Title(abt_md::FFileSection &section) {
    abt_md::FReadmefile *readmefile =_db.c_readmefile;
    if (ch_N(readmefile->title)) {
        section.title = tempstr()<< "## "<<readmefile->title;
    } else if (readmefile->p_scriptfile) {
        section.title = tempstr()<< "## "<<readmefile->p_scriptfile->gitfile<<" - "<<readmefile->p_scriptfile->comment;
    } else if (readmefile->p_ssimfile) {
        section.title = tempstr()<< "## "<<readmefile->p_ssimfile->ssimfile<<" - "<<readmefile->p_ssimfile->p_ctype->comment;
    } else if (readmefile->p_ctype) {
        section.title = tempstr()<< "## "<<readmefile->p_ctype->ctype<<" - "<<readmefile->p_ctype->comment;
    } else if (readmefile->p_ns) {
        tempstr fname(StripDirName(readmefile->gitfile));
        if (fname == "README.md") {
            section.title = tempstr()<< "## "<<readmefile->p_ns->ns<<" - "<<readmefile->p_ns->comment;
        } else {
            // don't change -- could be some other chapter
        }
    }
}

// -----------------------------------------------------------------------------

// Append every C string literal found on TEXT to HELP.
// Skips leading whitespace, then walks each "..." literal with
// algo::cstring_ReadCmdarg (which decodes C escapes via UnescapeC).
// Stops at the first non-quote token (the trailing `;` on the last
// line of a block, or end of line).
// HELP is NULL for a declaration naming something this tree does not have, and
// the literals are then dropped.
static void AccumHelpString(algo::cstring *help, algo::strptr text) {
    if (help) {
        algo::StringIter iter(text);
        bool more = true;
        while (more) {
            iter.Ws();
            if (iter.Peek() == '"') {
                algo::cstring part;
                if (algo::cstring_ReadCmdarg(part, iter, true)) {
                    *help << part;
                } else {
                    more = false;
                }
            } else {
                more = false;
            }
        }
    }
}

// Return the name whose help string the line LINE declares, and the empty string when
// it declares none.  QUALIFIER is the namespace the declaration is qualified by.
//
// The declaration reads `const char *<qualifier>::<name>_help = "..."`, so the name is what
// sits between the qualifier and the suffix.  It cannot be taken as the text before the last
// underscore: a tool's help opens by naming the tool, so `atf_unit_help = "atf_unit: ...`
// has its last underscore inside the string and the name came back as
// `atf_unit_help = "atf`.  Every tool whose name carries an underscore was therefore read
// as a namespace that does not exist, and forty-one of the tool READMEs had no Syntax
// section because of it.
static tempstr Helpname(algo::strptr line, algo::strptr qualifier) {
    tempstr open;
    open << qualifier << "::";
    algo::strptr shut("_help = ");
    int at = FindStr(line, open);
    int end = FindStr(line, shut);
    int begin = at >= 0 ? at + ch_N(open) : 0;
    tempstr ret;
    if (at >= 0 && end > begin) {
        ret << algo::strptr(line.elems + begin, end - begin);
    }
    return ret;
}

// One-shot scan of the generated file PATH, copying every help string it declares
// into the help table under <QUALIFIER>.<name>.  amc emits one declaration per entity, as
// `const char *<qualifier>::<name>_help = "…" "…" …;`, and the literals may span
// several lines: the declaration line names the entity and carries the first of
// them, each continuation line carries more, and the line ending in `;` closes the
// declaration.
// A tree that does not build PATH has none of these entities either, so a missing
// file leaves the table without them and reports nothing.
static void LoadHelpFile(algo::strptr path, algo::strptr qualifier) {
    tempstr open;
    open << "const char *" << qualifier << "::";
    algo::cstring *help = NULL;
    if (algo::FileQ(path)) {
        ind_beg(algo::FileLine_curs, line, path) {
            if (!help && StartsWithQ(line, open) && FindStr(line, "_help = ") != -1) {
                help = &abt_md::ind_help_GetOrCreate(tempstr() << qualifier << "." << Helpname(line, qualifier)).text;
                AccumHelpString(help, Pathcomp(line, "=LR"));
            } else if (help) {
                AccumHelpString(help, line);
                if (EndsWithQ(line, ";")) {
                    help = NULL;
                }
            }
        }ind_end;
    }
}

// Return the help text amc generated for entity NAME, declared in C++ namespace
// QUALIFIER (command, for a tool), and empty when there is none.  The first ask
// for a qualifier reads cpp/gen/<QUALIFIER>_gen.cpp, where amc declares every
// help string of that qualifier, so the file is read once.
algo::strptr abt_md::GetHelp(algo::strptr qualifier, algo::strptr name) {
    if (!abt_md::ind_helpfile_Find(qualifier)) {
        abt_md::ind_helpfile_GetOrCreate(qualifier);
        LoadHelpFile(tempstr() << "cpp/gen/" << qualifier << "_gen.cpp", qualifier);
    }
    abt_md::FHelp *help = abt_md::ind_help_Find(tempstr() << qualifier << "." << name);
    return help ? algo::strptr(help->text) : algo::strptr();
}

// -----------------------------------------------------------------------------

// Update the syntax section from the help text of the page's subject.  A tool
// README's subject is its namespace, whose help is in cpp/gen/command_gen.cpp,
// and a page whose subject set its own help text uses that.  The text is in
// both cases exactly what the user sees on -h, and its source is the generated
// file rather than the binary, so the section is refreshed whatever -evalcmd says.
void abt_md::mdsection_Syntax(abt_md::FFileSection &section) {
    abt_md::FReadmefile &readmefile = *_db.c_readmefile;
    bool exe = readmefile.p_ns && readmefile.p_ns->nstype == dmmeta_Nstype_nstype_exe;
    if (ch_N(readmefile.help) || exe) {
        algo::strptr help = ch_N(readmefile.help) ? algo::strptr(readmefile.help) : GetHelp("command", readmefile.p_ns->ns);
        section.text = "";
        if (Trimmed(help) != "") {
            section.text << Preformatted(help, "usage");
        }
    }
}
// A Description is prose somebody wrote, and nothing here generates one.
void abt_md::mdsection_Description(abt_md::FFileSection &) {
}

void abt_md::mdsection_Content(abt_md::FFileSection &) {
}

void abt_md::mdsection_Limitations(abt_md::FFileSection &) {
}

void abt_md::mdsection_Example(abt_md::FFileSection &) {
}

// Caveats are written by hand; the row exists so they sort after the examples.
void abt_md::mdsection_Caveats(abt_md::FFileSection &) {
}

// Update copyright section
void abt_md::mdsection_Copyright(abt_md::FFileSection &) {
}

//
