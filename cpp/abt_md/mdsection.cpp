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
// Source: cpp/abt_md/mdsection.cpp
//

#include "include/algo.h"
#include "include/abt_md.h"

void abt_md::DescribeCtype(abt_md::FCtype *ctype, cstring &out) {
    if (ctype->c_ssimfile) {
        tempstr ssimfname = SsimFname("data",ctype->c_ssimfile->ssimfile);
        out << "* file:"<<LinkToFileAbs(ssimfname,ssimfname)<<eol;
    }
    if (c_field_N(*ctype)) {
        out<<eol;
        AddRow(out
               , "Name"
               , LinkToSsimfile("Type","dmmeta.ctype")
               , LinkToSsimfile("Reftype","dmmeta.reftype")
               , "Default"
               , "Comment");
        AddRow(out, "---", "---", "---", "---", "---");
        ind_beg(ctype_c_field_curs,field,*ctype) {
            tempstr comment(field.comment);
            if (field.c_substr) {
                comment << "\n" << field.c_substr->expr <<" of "<<name_Get(*field.c_substr->p_srcfield);
            }
            tempstr arg(field.arg);
            // for ssimdb only, provide cross-links
            if (field.p_arg->c_ssimfile) {
                arg = LinkToSsimfile(field.arg,field.p_arg->c_ssimfile->ssimfile);
            }
            AddRow(out
                   , name_Get(field)
                   , arg
                   , field.reftype
                   , field.dflt.value
                   , comment);
        }ind_end;
        out<<eol<<eol;
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Tables(abt_md::FFileSection &section) {
    section.text = "";
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Attributes(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    if (abt_md::FCtype *ctype = _db.c_readme->p_ctype) {
        section.text << "* "<<LinkToSsimfile(tempstr()<<"ctype:","dmmeta.ctype") << ctype->ctype << eol<<eol;
        DescribeCtype(ctype,section.text);
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Inputs(abt_md::FFileSection &section) {
    if (_db.c_readme->p_ns) {
        section.text = "";// replace section text
        tempstr inputs = SysEval(Subst(_db.R,"acr_in $ns"),FailokQ(true),1024*1024);
        algo_lib::FFildes read;
        command::acr_in_proc acr_in;
        acr_in.cmd.ns.expr=_db.c_readme->p_ns->ns;
        acr_in.cmd.sigcheck=false;
        cstring text;
        Ins(&_db.R,text, "`$ns` takes the following tables on input:");
        AddRow(text, "ssimfile", "comment");
        AddRow(text, "---", "---");
        int nctype=0;
        ind_beg(algo::FileLine_curs,line,acr_in_StartRead(acr_in,read)) {
            dmmeta::Ssimfile ssimfile;
            if (Ssimfile_ReadStrptrMaybe(ssimfile,line)) {
                if (abt_md::FSsimfile *fssimfile=ind_ssimfile_Find(ssimfile.ssimfile)) {
                    tempstr fname = tempstr() << "txt/ssimdb/"<<ns_Get(*fssimfile)<<"/"<<name_Get(*fssimfile)<<".md";
                    AddRow(text, LinkToFileAbs(fssimfile->ssimfile,fname), fssimfile->p_ctype->comment);
                    nctype++;
                }
            }
        }ind_end;
        // omit section text if no inputs
        if (nctype) {
            section.text << Tabulated(text, "\t", "ll", 2);
        }
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Options(abt_md::FFileSection &section) {
    if (_db.c_readme->p_ns) {
        abt_md::FCtype *ctype = ind_ctype_Find(tempstr()<<"command."<<_db.c_readme->p_ns->ns);
        section.text = "";
        if (ctype) {
            ind_beg(ctype_c_field_curs,field,*ctype) {
                section.text << "#### -"<<name_Get(field)<<" -- "<<field.comment << eol;
            }ind_end;
        }
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Ctypes(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    if (abt_md::FNs *ns = _db.c_readme->p_ns) {
        cstring out;
        ind_beg(ns_c_ctype_curs,ctype,*ns) {
            if (!FileQ(tempstr()<<DirFileJoin(GetDirName(_db.c_readme->gitfile),name_Get(ctype))<<".md")) {
                tempstr comment(ctype.comment);
                if (comment == "" && ns_Get(ctype) == "report" && ind_ns_Find(name_Get(ctype))) {
                    comment << "Report line for "<< LinkToNs(name_Get(ctype), dmmeta_Nstype_nstype_exe);
                } else if (comment == "" && ns_Get(ctype) == "command" && ind_ns_Find(name_Get(ctype))) {
                    comment << "Command line for "<< LinkToNs(name_Get(ctype), dmmeta_Nstype_nstype_exe);
                }
                out << "#### "<< ctype.ctype << " - " << comment << eol<<eol;
                DescribeCtype(&ctype,out);
            }
        }ind_end;
        if (out != "") {
            section.text << "Other ctypes in this namespace which don't have own readme files" << eol<<eol;
            section.text << out;
        }
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Functions(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    if (_db.c_readme->p_ns) {
        command::src_func_proc src_func;
        src_func.cmd.target.expr=_db.c_readme->p_ns->ns;
        src_func.cmd.proto=true;
        src_func.cmd.showloc=false;
        src_func.cmd.showstatic=false;
        algo_lib::FFildes read;
        cstring comment;
        cstring out;
        ind_beg(algo::FileLine_curs,line,src_func_StartRead(src_func,read)) {
            if (StartsWithQ(line,"// ")) {
                comment<<line<<eol;
            } else {
                out<<eol;
                out << CodeBlock(tempstr()<<comment<<line) << eol;
                comment="";
            }
        }ind_end;
        if (out!="") {
            section.text<<"Functions exported from this namespace:"<<eol<<eol;
        }
        section.text << out;
    }
}

// -----------------------------------------------------------------------------

// Update title of document
// - For namespace, pull namespace name and comment from ns table
// - For script, use script name and comment from scriptfile table
// For all other cases, leave title as-is
void abt_md::mdsection_Title(abt_md::FFileSection &section) {
    abt_md::FReadme *readme =_db.c_readme;
    if (readme->p_ns) {
        section.title = tempstr()<< "## "<<readme->p_ns->ns<<" - "<<readme->p_ns->comment;
    } else if (readme->p_scriptfile) {
        section.title = tempstr()<< "## "<<readme->p_scriptfile->gitfile<<" - "<<readme->p_scriptfile->comment;
    } else if (readme->p_ssimfile) {
        section.title = tempstr()<< "## "<<readme->p_ssimfile->ssimfile<<" - "<<readme->p_ssimfile->p_ctype->comment;
    }
}

// -----------------------------------------------------------------------------

// Update syntax string
// Invoke command with -h flag and substitute output into section body
void abt_md::mdsection_Syntax(abt_md::FFileSection &section) {
    if (_db.c_readme->p_ns) {
        section.text="";
        tempstr out = SysEval(Subst(_db.R,"$ns -h 2>&1"),FailokQ(true),1024*1024);
        if (Trimmed(out)!="") {
            section.text << Preformatted(out);
        }
    }
}

// -----------------------------------------------------------------------------

// Table of contents
// for README file, create links to subdirectories
// for non-README file, create links to sections inside file
void abt_md::mdsection_Toc(abt_md::FFileSection &section) {
    section.text = "";
    abt_md::FReadme *readme = _db.c_readme;
    tempstr dirname(GetDirName(readme->gitfile));
    // the top-level README.md cannot be a soft link, or it won't be displayed
    // so if we detect that the directory is top-level, we begin our search under txt/
    if (dirname =="") {
        dirname = "txt";
    }
    verblog("toc "<<dirname);

    // scan remainder of this file (except ToC itself)
    // and add links to sections
    ind_beg(_db_file_section_curs,other_section,_db) {
        if (!TitleQ(other_section) && &other_section != &section && other_section.text != "") {
            strptr line = other_section.title;
            int level = GetHeaderLevel(line);
            if (level>1 && level<=3 && line[level]==' ') {
                tempstr toclink = Link(RestFrom(line,level+1),"",LineKey(line));
                section.text << "* " << toclink << eol;
            }
        }
    }ind_end;

    // create links to subdirectories
    bool mainfile = StripDirName(readme->gitfile)=="README.md";
    if (mainfile) {
        // create links to sub-directories
        PopulateDirent(DirFileJoin(dirname,"*"));
        ind_beg(abt_md::_db_bh_dirent_curs,ent,_db) {
            tempstr subfile = DirFileJoin(ent.pathname,"README.md");
            if (!FileQ(subfile)) {
                subfile = DirFileJoin(ent.pathname,tempstr()<<ent.filename<<".md");// recognize $ns/$ns.md
            }
            if (ent.is_dir && FileQ(subfile)) {
                section.text << "* "<<LinkToMd(subfile)<<eol;
            }
        }ind_end;
        // create links to individual files
        // PopulateDirent(DirFileJoin(dirname,"*.md"));
        // ind_beg(abt_md::_db_bh_dirent_curs,ent,_db) {
        //     if (!ent.is_dir && ent.pathname != readme->gitfile) {
        //         section.text << "* "<<ent.filename<<" - "<<LinkToMd(ent.pathname)<<eol;
        //     }
        // }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Create links to other files in the same directory
void abt_md::mdsection_Chapters(abt_md::FFileSection &section) {
    section.text = "";
    PopulateDirent(tempstr()<<GetDirName(_db.c_readme->gitfile)<<"*.md");
    ind_beg(abt_md::_db_bh_dirent_curs,ent,_db) {
        if (ent.pathname != _db.c_readme->gitfile) {
            if (StripExt(ent.filename) != "") {// ignore the stupid ".md" file
                section.text << "* "<<LinkToMd(ent.pathname)<<eol;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Sources(abt_md::FFileSection &section) {
    abt_md::FReadme *readme = _db.c_readme;
    section.text = "";// replace section text
    if (readme->p_ns) {
        abt_md::FNs *ns = readme->p_ns;
        section.text << "The source code license is "<<ns->license<<eol;
        section.text << "The following source files are part of this tool:" << eol<<eol;
        AddRow(section.text,"Source File","Comment");
        AddRow(section.text, "---", "---");
        ind_beg(ns_c_targsrc_curs,targsrc,*ns) {
            AddRow(section.text, LinkToFileAbs(src_Get(targsrc),src_Get(targsrc)), targsrc.comment.value);
        }ind_end;
    } else if (StartsWithQ(readme->gitfile, "txt/script/")) {
        // see if this file matches a scriptfile
        tempstr fname = tempstr() << "bin/"<<StripExt(GetFileName(readme->gitfile));
        if (abt_md::FScriptfile *scriptfile = ind_scriptfile_Find(fname)) {
            section.text << "The source code license is "<<scriptfile->license<<eol<<eol;
            section.text << "Source file: "<<LinkToFileAbs(fname,fname) << eol;
        }
    }
}

void abt_md::mdsection_Description(abt_md::FFileSection &) {
}

void abt_md::mdsection_Content(abt_md::FFileSection &) {
}

void abt_md::mdsection_Limitations(abt_md::FFileSection &) {
}

void abt_md::mdsection_Example(abt_md::FFileSection &) {
}

// Update tests section
// Scan component tests for this namespace and print a table
void abt_md::mdsection_Tests(abt_md::FFileSection &section) {
    if (_db.c_readme->p_ns) {
        section.text = "";// replace section text
        if (c_comptest_N(*_db.c_readme->p_ns)) {
            Ins(&_db.R,section.text, "The following component tests are defined for `$ns`.");
            Ins(&_db.R,section.text, "These can be executed with `atf_comp <comptest> -v`");
            AddRow(section.text, "COMPTEST", "COMMENT");
            AddRow(section.text, "---", "---");
            ind_beg(ns_c_comptest_curs,comptest,*_db.c_readme->p_ns) {
                AddRow(section.text, comptest.comptest, comptest.comment);
            }ind_end;
        }
    }
}

// Update copyright section
void abt_md::mdsection_Copyright(abt_md::FFileSection &) {
}

void abt_md::mdsection_Reftypes(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    ind_beg(_db_reftype_curs,reftype,_db) {
        section.text << eol;
        section.text << "#### "<< reftype.reftype << " - " << reftype.p_tclass->comment << eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Subsets(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    cstring out;
    if (abt_md::FSsimfile *ssimfile = _db.c_readme->p_ssimfile) {
        ind_beg(ctype_c_field_arg_curs,field,*ssimfile->p_ctype) if (field.ispkey) {
            abt_md::FCtype &childtype=*field.p_ctype;
            if (childtype.c_ssimfile) {
                out << "* "<<LinkToSsimfile(childtype.ctype,childtype.c_ssimfile->ssimfile) << " - "<<childtype.comment<<" "<<eol;
            }
        }ind_end;
    }
    if (out != "") {
        section.text << "These ssimfiles are subsets of "<<_db.c_readme->p_ssimfile->ssimfile<<eol;
        section.text <<eol;
        section.text << out;
    }
}

// -----------------------------------------------------------------------------

// Show related ssimfiles (those that reference this ssimfile NOT through pkey)
void abt_md::mdsection_Related(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    cstring out;
    if (abt_md::FSsimfile *ssimfile = _db.c_readme->p_ssimfile) {
        ind_beg(ctype_c_field_arg_curs,field,*ssimfile->p_ctype) if (!field.ispkey) {
            abt_md::FCtype &childtype=*field.p_ctype;
            if (childtype.c_ssimfile) {
                tempstr comment(field.comment);
                if (comment == "") {
                    comment = childtype.comment;
                }
                out << "* "<<LinkToSsimfile(tempstr() << childtype.c_ssimfile->ssimfile<<" via "<<name_Get(field),childtype.c_ssimfile->ssimfile)
                    << " - "<<comment<<" "<<eol;
            }
        }ind_end;
    }
    if (out != "") {
        section.text << "These ssimfiles reference "<<_db.c_readme->p_ssimfile->ssimfile<<eol;
        section.text <<eol;
        section.text << out;
    }
}

// -----------------------------------------------------------------------------

// Show related ssimfiles (those that reference this ssimfile NOT through pkey)
void abt_md::mdsection_CmdlineUses(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    cstring out;
    if (abt_md::FSsimfile *ssimfile = _db.c_readme->p_ssimfile) {
        ind_beg(ctype_c_field_arg_curs,field,*ssimfile->p_ctype) {
            abt_md::FCtype &childtype=*field.p_ctype;
            if (ns_Get(childtype) == "command") {
                out << "* "<<LinkToFileAbs(tempstr()<<name_Get(childtype)<<" -"<<name_Get(field)
                                           , tempstr()<<"txt/exe/"<<name_Get(childtype)<<"/README.md")
                    << " - "<<Trimmed(field.comment)<<" "<<eol;
            }
        }ind_end;
    }
    if (out != "") {
        section.text <<eol;
        section.text << out;
    }
}

// -----------------------------------------------------------------------------

// Show related ssimfiles (those that reference this ssimfile NOT through pkey)
void abt_md::mdsection_ImdbUses(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    cstring out;
    if (abt_md::FSsimfile *ssimfile = _db.c_readme->p_ssimfile) {
        ind_beg(ctype_c_field_arg_curs,field,*ssimfile->p_ctype) if (field.reftype == dmmeta_Reftype_reftype_Base) {
            abt_md::FCtype &childtype=*field.p_ctype;
            if (childtype.p_ns->nstype == dmmeta_Nstype_nstype_lib || childtype.p_ns->nstype == dmmeta_Nstype_nstype_exe) {
                out << "* "<<LinkToNs(childtype.p_ns->ns,childtype.p_ns->nstype)<<" as "<<childtype.ctype << eol;
            }
        }ind_end;
    }
    if (out != "") {
        section.text << eol << out <<eol;
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Constants(abt_md::FFileSection &section) {
    section.text = "";
    if (abt_md::FSsimfile *ssimfile = _db.c_readme->p_ssimfile) {
        ind_beg(ctype_c_field_arg_curs,field,*ssimfile->p_ctype) {
            if (c_fconst_N(field)) {
                AddRow(section.text, "NAME", "VALUE", "COMMENT");
                AddRow(section.text, "---", "---", "---");
                ind_beg(field_c_fconst_curs,fconst,field) {
                    AddRow(section.text, name_Get(fconst), fconst.value.value, fconst.comment);
                }ind_end;
                section.text << eol;
            }
        }ind_end;
    }
}

//
