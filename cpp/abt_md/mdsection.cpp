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
                << " are [**fldfunc**](/txt/ssim.md#fldfunc) fields"
                << eol
                << eol;
        }
        FTxttbl_Markdown(txttbl,out);
        out<<eol;
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
    // Extract loaded finputs from the generated code
    abt_md::FNs *ns=_db.c_readme->p_ns;
    if (ns) {
        section.text = "";// replace section text
        tempstr cmd;
        cmd=Subst(_db.R,"src_func $ns LoadTuplesMaybe -gen | grep -Po  -e 'root,\"\\K[^\"]*'");
        cstring slist(Trimmed(SysEval(cmd,FailokQ(true),1024*1024)));

        cstring text;
        Ins(&_db.R,text, "`$ns` takes the following tables on input:");

        algo_lib::FTxttbl txttbl;
        AddRow(txttbl);
        AddCols(txttbl,"Ssimfile,Comment");
        ind_beg(algo::Line_curs,line,slist) {
            if (abt_md::FSsimfile *fssimfile=ind_ssimfile_Find(line)) {
                LinkToSsimfile(fssimfile->ssimfile,fssimfile->p_ctype->ctype);
                AddRow(txttbl);
                AddCol(txttbl,LinkToSsimfile(fssimfile->ssimfile,fssimfile->ssimfile));
                AddCol(txttbl,fssimfile->p_ctype->comment);
            }
        }ind_end;
        FTxttbl_Markdown(txttbl,text);
        // omit section text if no inputs
        if (algo_lib::c_txtrow_N(txttbl)>1) {
            section.text << Tabulated(text, "\t", "ll", 2);
        }
        if (ns->nstype==dmmeta_Nstype_nstype_exe){
            section.text<<eol;
            section.text<<eol;
        }
    }
}
// -----------------------------------------------------------------------------

abt_md::FCtype *abt_md::GenerateFieldsTable(abt_md::FCtype &ctype, cstring &text_out, cstring &base_note){
    abt_md::FCtype *base=NULL;
    algo_lib::FTxttbl txttbl;
    HeaderCtype(txttbl);
    ind_beg(abt_md::ctype_c_field_curs,field,ctype){
        AddRow(txttbl);
        // make substr field
        if (abt_md::FSubstr *substr=field.c_substr){
            AddCol(txttbl,tempstr()<<"*"<<field.field<<" ("<<substr->expr<<(")*"));
        }else{
            AddCol(txttbl,field.field);
        }
        AddCol(txttbl,abt_md::LinkToCtype(*field.p_arg));
        if (field.reftype == dmmeta_Reftype_reftype_Base) {
            base=field.p_arg;
            if (abt_md::FSsimfile *fssimfile=base->c_ssimfile){
                base_note<<"field "
                         <<"``"<<field.field<<"``"
                         <<" has reftype ``base`` so the fields of "
                         <<abt_md::LinkToSsimfile(field.arg,fssimfile->ssimfile)
                         <<" above are included into the resulting struct."
                    ;
                AddCol(txttbl,abt_md::LinkToSsimfile(field.reftype,fssimfile->ssimfile));
                // Reference is enough
                base=NULL;
            } else {
                AddCol(txttbl,abt_md::Link(field.reftype,"",tempstr()<<abt_md::MdAnchor(base->ctype)<<"-fields"));
            }
        } else {
            AddCol(txttbl,LinkToReftype(field.reftype));
        }
        AddCol(txttbl,tempstr()<<field.dflt);
        AddCol(txttbl,field.comment);
    }ind_end;
    text_out<<"#### "<<ctype.ctype<<" Fields"<<eol;
    FTxttbl_Markdown(txttbl,text_out);
    return base;
}

// -----------------------------------------------------------------------------

// Extract generated info and combine into a table
void abt_md::mdsection_Imdb(abt_md::FFileSection &section) {
    if (_db.c_readme->p_ns) {
        section.text = "";// replace section text
        cstring text;

        algo_lib::FTxttbl txttbl;
        AddRow(txttbl);
        AddCols(txttbl,"Ctype,Ssimfile,Create,Access");
        // parsing include for the structs - much faster than individual query with amc
        // for each structure ...
        bool in_structQ(false);
        ind_beg(algo::FileLine_curs,line,Subst(abt_md::_db.R,"include/gen/$ns_gen.h")) {
            if (StartsWithQ(line,Subst(abt_md::_db.R,"// --- $ns"))){
                if (abt_md::FCtype *ctype=abt_md::ind_ctype_Find(Pathcomp(line," RR"))){
                    in_structQ=true;
                    AddRow(txttbl);
                    AddCol(txttbl,abt_md::LinkToSection(ctype->ctype));

                    abt_md::FSsimfile *fssimfile=NULL;
                    ind_beg(abt_md::ctype_c_field_curs,field,*ctype) if (field.reftype == dmmeta_Reftype_reftype_Base) {
                        fssimfile=field.p_arg->c_ssimfile;
                    }ind_end;
                    if (fssimfile){
                        AddCol(txttbl,abt_md::LinkToSsimfile(fssimfile->ssimfile,fssimfile->ssimfile));
                    } else {
                        AddCol(txttbl,"");
                    }

                    // Generate fields
                    text<<"#### "<<ctype->ctype<<" - " << TypeComment(*ctype)<<eol<<eol;
                    tempstr base_note;
                    if (abt_md::FCtype *ctype_base=GenerateFieldsTable(*ctype, text, base_note)){
                        GenerateFieldsTable(*ctype_base, text, base_note);
                    };
                    text << eol;
                    text <<"#### Struct "<<name_Get(*ctype)<<eol;
                    if (base_note!=""){
                        text<<"*Note:* "<<base_note<<eol<<eol;
                    }
                    tempstr fname(Subst(abt_md::_db.R,"include/gen/$ns_gen.h"));
                    text <<"Generated by [amc](/txt/exe/amc/README.md) into "<<LinkToFileAbs(fname,fname)<<eol;
                    text <<"```"<<eol;
                }
            };
            if (in_structQ){
                if (StartsWithQ(line,"//")){
                    tempstr key(Trimmed(Pathcomp(line,"/RR:LL")));
                    tempstr val(Trimmed(Pathcomp(line,":RR")));
                    tempstr pool_fld(Pathcomp(val," LL"));
                    Replace(val,Subst(abt_md::_db.R,"$ns."),"");
                    if (key=="create"){
                        AddCol(txttbl,val);
                        if (abt_md::ind_gstatic_Find(pool_fld)){
                            AddCol(txttbl,"**static**");
                        }
                    } else if (key == "global access"){
                        AddCol(txttbl,val);
                    } else if (key=="access"){
                        AddRow(txttbl);
                        AddCols(txttbl,tempstr()<<",,,"<<val);
                    }
                } else {
                    text << line<<eol;
                }
                if (StartsWithQ(line,"}")){
                    in_structQ=false;
                    text << "```"<<eol;
                    text << eol;
                }
            };
        }ind_end;
        if (text!=""){
            Ins(&abt_md::_db.R,section.text, "`$ns` generated code creates the tables below.");
            Ins(&abt_md::_db.R,section.text, tempstr()<<"All allocations are done through global `$ns::_db` "
                <<LinkToSection(tempstr()<<_db.c_readme->p_ns->ns<<".FDb")<<" structure");
            FTxttbl_Markdown(txttbl,section.text);
            section.text<<eol;
            section.text<<text<<eol;
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
                    comment << "Report line for "<< LinkToNs(name_Get(ctype));
                } else if (comment == "" && ns_Get(ctype) == "command" && ind_ns_Find(name_Get(ctype))) {
                    comment << "Command line for "<< LinkToNs(name_Get(ctype));
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
// Section contents are user-defined
void abt_md::mdsection_Title(abt_md::FFileSection &section) {
    abt_md::FReadme *readme =_db.c_readme;
    if (readme->p_scriptfile) {
        section.title = tempstr()<< "## "<<readme->p_scriptfile->gitfile<<" - "<<readme->p_scriptfile->comment;
    } else if (readme->p_ssimfile) {
        section.title = tempstr()<< "## "<<readme->p_ssimfile->ssimfile<<" - "<<readme->p_ssimfile->p_ctype->comment;
    } else if (readme->p_ctype) {
        section.title = tempstr()<< "## "<<readme->p_ctype->ctype<<" - "<<readme->p_ctype->comment;
    } else if (readme->p_ns) {
        tempstr fname(StripDirName(readme->gitfile));
        if (fname== "internals.md") {
            section.title = tempstr()<< "## "<<readme->p_ns->ns<<" - Internals";
        } else if (fname == "README.md") {
            section.title = tempstr()<< "## "<<readme->p_ns->ns<<" - "<<readme->p_ns->comment;
        } else {
            // don't change -- could be some other chapter
        }
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
// The README.md files must form a tree covering all files.
// Thus, non-README must not include links to other files in the same directory into ToC,
// but can include those links outside of ToC
// README.md must not include a link to internals.md on the same level (this link has to come
// from above) to avoid contaminating ToC tree with unneeded details
// for more information see spnx
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
    tempstr toctext;
    int ntoc=0;
    ind_beg(_db_file_section_curs,other_section,_db) {
        if (!TitleQ(other_section) && &other_section != &section && other_section.text != "") {
            strptr line = other_section.title;
            int level = GetHeaderLevel(line);
            if (level>1 && level<=3 && line[level]==' ') {
                tempstr toclink = Link(RestFrom(line,level+1),"",LineKey(line));
                toctext << "&nbsp;&nbsp;&bull;&nbsp;  " << toclink << "<br/>"<<eol;
                ntoc++;
            }
        }
    }ind_end;
    // skip table of contents if just 1 section
    if (ntoc>1) {
        section.text << toctext;
    }

    // create links to subdirectories
    bool mainfile = StripDirName(readme->gitfile)=="README.md";

    // Create links to other files in this directory
    cstring text;
    abt_md::FDirscan dirscan;
    PopulateDirent(dirscan, tempstr()<<GetDirName(_db.c_readme->gitfile)<<"*.md");
    ind_beg(abt_md::FDirscan_bh_dirent_curs,ent,dirscan) {
        // - don't link to this file
        // - don't link to empty file
        if (ent.pathname != _db.c_readme->gitfile && StripExt(ent.filename) != "") {
            text << FileIcon() << LinkToMd(ent.pathname) << "<br/>" << eol;
        }
    }ind_end;
    // non-README must not include links to other files in the same directory into TOC.
    // README.md must not include a link to internals.md on the same level (this link has to come
    // from above) to avoid contaminating ToC tree with unneeded details
    // for more information see spnx
    bool has_internals = FileQ(DirFileJoin(GetDirName(_db.c_readme->gitfile),"internals.md"));
    if (!mainfile || has_internals) {
        section.text << text;
        text = "";
    }

    // from README.md, create links to sub-directories
    if (mainfile) {
        PopulateDirent(dirscan,DirFileJoin(dirname,"*"));
        ind_beg(abt_md::FDirscan_bh_dirent_curs,ent,dirscan) if (ent.is_dir) {
            tempstr subfile = DirFileJoin(ent.pathname,"README.md");
            // hack: if subdirectory has internals, the internals.md file has to be
            // linked from above to avoid contaminating ToC tree
            if (FileQ(subfile) && !FileQ(DirFileJoin(ent.pathname,"internals.md"))) {
                text  << FolderIcon()<<LinkToMd(subfile)<<"<br/>"<<eol;
            } else {
                // if README not found on level below, include ALL files on level below
                abt_md::FDirscan dirscan2;
                PopulateDirent(dirscan2, DirFileJoin(ent.pathname,"*.md"));
                ind_beg(FDirscan_bh_dirent_curs,ent2,dirscan2) {
                    text  << FileIcon() <<LinkToMd(ent2.pathname)<<"<br/>"<<eol;// file icon
                }ind_end;
            }
        }ind_end;
    }

    // insert text as TOC entry
    if (text!=""){
        dev::Mdmark mdmark;
        mdmark.mdmark=dev_Mdmark_mdmark_TOC;
        mdmark.param=section.p_mdsection->mdsection;
        mdmark.state=dev_Mdmark_state_BEG_AUTO;
        section.text << abt_md::MdComment(tempstr()<<mdmark)<<eol;
        section.text << text <<eol;
        mdmark.state=dev_Mdmark_state_END_AUTO;
        section.text << abt_md::MdComment(tempstr()<<mdmark)<<eol;
    }
}

// -----------------------------------------------------------------------------

// Create links to other files in the same directory
void abt_md::mdsection_Chapters(abt_md::FFileSection &section) {
    section.text = "";
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Sources(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    abt_md::FReadme *readme = _db.c_readme;
    if (readme->p_ns) {
        abt_md::FNs *ns = readme->p_ns;
        section.text << "The source code license is "<<ns->license<<eol;
        section.text << "The following source files are part of this tool:" << eol<<eol;


        algo_lib::FTxttbl txttbl;
        AddRow(txttbl);
        AddCols(txttbl,"Source File,Comment");
        ind_beg(ns_c_targsrc_curs,targsrc,*ns) {
            AddRow(txttbl);
            AddCol(txttbl,LinkToFileAbs(src_Get(targsrc),src_Get(targsrc)));
            AddCol(txttbl,targsrc.comment.value);
        }ind_end;
        FTxttbl_Markdown(txttbl,section.text);
    } else if (StartsWithQ(readme->gitfile, "txt/script/")) {
        // see if this file matches a scriptfile
        tempstr fname = tempstr() << "bin/"<<StripExt(GetFileName(readme->gitfile));
        if (abt_md::FScriptfile *scriptfile = ind_scriptfile_Find(fname)) {
            section.text << "The source code license is "<<scriptfile->license<<eol<<eol;
            section.text << "Source file: "<<LinkToFileAbs(fname,fname) << eol;
        }
    }
}

void abt_md::mdsection_Dependencies(abt_md::FFileSection &section) {
    section.text = "";// replace section text
    abt_md::FReadme *readme = _db.c_readme;
    if (readme->p_ns && readme->p_ns->c_target && c_targdep_N(*readme->p_ns->c_target)) {
        abt_md::FTarget *target=readme->p_ns->c_target;
        section.text << "The build target depends on the following libraries"<<eol;
        algo_lib::FTxttbl txttbl;
        AddRow(txttbl);
        AddCols(txttbl,"Target,Comment");
        ind_beg(target_c_targdep_curs,targdep,*target) {
            AddRow(txttbl);
            tempstr fname = tempstr() << "txt/"<<targdep.p_parent->p_ns->nstype<<"/" << parent_Get(targdep) << "/README.md";
            bool exists = FileQ(fname);
            AddCol(txttbl,exists ? LinkToFileAbs(parent_Get(targdep),fname) : fname);
            AddCol(txttbl,targdep.p_parent->p_ns->comment.value);
        }ind_end;
        FTxttbl_Markdown(txttbl,section.text);
    }
}

void abt_md::mdsection_Description(abt_md::FFileSection &section) {
    (void)section;
    // user-defined content
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

            algo_lib::FTxttbl txttbl;
            AddRow(txttbl);
            AddCols(txttbl,"Comptest,Comment");
            ind_beg(ns_c_comptest_curs,comptest,*_db.c_readme->p_ns) {
                AddRow(txttbl);
                tempstr fname = tempstr() << "test/atf_comp/" << comptest.comptest;
                AddCol(txttbl,LinkToFileAbs(comptest.comptest,fname));
                AddCol(txttbl,comptest.comment);
            }ind_end;
            FTxttbl_Markdown(txttbl,section.text);
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
                out << "* "<<LinkToInternals(childtype.p_ns->ns,*childtype.p_ns)
                    <<" as "<<LinkToCtype(childtype)<<eol;
            }
        }ind_end;
    }
    if (out != "") {
        section.text << eol << out <<eol;
    }
}

// -----------------------------------------------------------------------------

void abt_md::mdsection_Constants(abt_md::FFileSection &section) {
    // TODO: - add to libs and protocols...
    section.text = "";
    if (abt_md::FSsimfile *ssimfile = _db.c_readme->p_ssimfile) {
        algo_lib::FTxttbl txttbl;
        AddRow(txttbl);
        AddCols(txttbl,"Field,Fconst,Value,Comment");
        ind_beg(ctype_c_field_curs,field,*ssimfile->p_ctype) {
            if (c_fconst_N(field)) {
                AddRow(txttbl);
                AddCol(txttbl,name_Get(field));
                ind_beg(field_c_fconst_curs,fconst,field) {
                    AddRow(txttbl);
                    AddCol(txttbl,"");
                    AddCol(txttbl,name_Get(fconst));
                    AddCol(txttbl,fconst.value.value);
                    AddCol(txttbl,fconst.comment);
                }ind_end;
            }
        }ind_end;
        if (c_txtrow_N(txttbl)>1){
            FTxttbl_Markdown(txttbl,section.text);
            section.text << eol;
        }
    }
}

//
