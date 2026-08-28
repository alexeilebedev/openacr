// Copyright (C) 2024,2026 AlgoRND
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
// - For namespace, pull namespace name and comment from ns table
// - For script, use script name and comment from scriptfile table
// For all other cases, leave title as-is
// Section contents are user-defined
void abt_md::mdsection_Title(abt_md::FFileSection &section) {
    abt_md::FReadmefile *readmefile =_db.c_readmefile;
    if (readmefile->p_scriptfile) {
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

// Append every C string literal found on `text` to ns->help.
// Skips leading whitespace, then walks each "..." literal with
// algo::cstring_ReadCmdarg (which decodes C escapes via UnescapeC).
// Stops at the first non-quote token (the trailing `;` on the last
// line of a block, or end of line).
static void AccumHelpString(abt_md::FNs *ns, algo::strptr text) {
    if (ns) {
        algo::StringIter iter(text);
        bool more = true;
        while (more) {
            iter.Ws();
            if (iter.Peek() == '"') {
                algo::cstring part;
                if (algo::cstring_ReadCmdarg(part, iter, true)) {
                    ns->help << part;
                } else {
                    more = false;
                }
            } else {
                more = false;
            }
        }
    }
}

// One-shot scan of cpp/gen/command_gen.cpp.  amc emits every exe's
// help text as `const char *command::<ns>_help = "…" "…" …;`,
// possibly spanning multiple lines.  Walk line-by-line: on the
// header line capture the ns and consume the rest-of-line; on each
// continuation line accumulate the literals; release cur_ns when
// the line ends with `;`.  After this every executable FNs has
// `.help` populated and mdsection_Syntax becomes a hash lookup.
// Return the namespace whose help string the line LINE declares, and the empty string when
// it declares none.
//
// The declaration reads `const char *command::<ns>_help = "..."`, so the name is what sits
// between the qualifier and the suffix.  It cannot be taken as the text before the last
// underscore: a tool's help opens by naming the tool, so `atf_unit_help = "atf_unit: ...`
// has its last underscore inside the string and the name came back as
// `atf_unit_help = "atf`.  Every tool whose name carries an underscore was therefore read
// as a namespace that does not exist, and forty-one of the tool READMEs had no Syntax
// section because of it.
static tempstr Helpns(algo::strptr line) {
    algo::strptr open("command::");
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

static void LoadHelpStrings() {
    abt_md::FNs *cur_ns = NULL;
    ind_beg(algo::FileLine_curs, line, "cpp/gen/command_gen.cpp") {
        if (!cur_ns && StartsWithQ(line, "const char *command::") && FindStr(line, "_help = ") != -1) {
            cur_ns = abt_md::ind_ns_Find(Helpns(line));
            AccumHelpString(cur_ns, Pathcomp(line, "=LR"));
        } else if (cur_ns) {
            AccumHelpString(cur_ns, line);
            if (EndsWithQ(line, ";")) {
                cur_ns = NULL;
            }
        }
    }ind_end;
}

// Update syntax section from FNs.help (pre-populated by
// LoadHelpStrings on the first call).  No fork; no built binary
// required.  evalcmd:N now only gates inline `cmd:…` blocks
// elsewhere — the Syntax section is always refreshed because its
// source is the generated file, not the binary.
void abt_md::mdsection_Syntax(abt_md::FFileSection &section) {
    if (!_db.help_loaded) {
        LoadHelpStrings();
        _db.help_loaded = true;
    }
    if (_db.c_readmefile->p_ns && _db.c_readmefile->p_ns->nstype == dmmeta_Nstype_nstype_exe) {
        section.text = "";
        algo::strptr help = _db.c_readmefile->p_ns->help;
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

// Update copyright section
void abt_md::mdsection_Copyright(abt_md::FFileSection &) {
}

//
