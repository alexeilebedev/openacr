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
// Target: src_hdr (exe) -- Manage source copyright+license header in source files and scripts
// Exceptions: yes
// Source: cpp/src_hdr/hdr.cpp
//

#include "include/algo.h"
#include "include/gen/src_hdr_gen.h"
#include "include/gen/src_hdr_gen.inl.h"
#include "include/gen/dev_gen.h"
#include "include/gen/dev_gen.inl.h"

// -----------------------------------------------------------------------------

static void InsertComment(src_hdr::FSrc &src, strptr text, cstring &out) {
    if (src.cmtstring != "") {
        ind_beg(Line_curs,line,text) {
            out<<src.cmtstring;
            if (ch_N(line)) {
                out<<" "<<line;
            }
            out<<eol;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static tempstr Tag(strptr key, strptr val) {
    return tempstr() << key << ": " << val;
}

// -----------------------------------------------------------------------------

static tempstr DemarcatedComment(strptr comment) {
    tempstr ret;
    if (ch_N(comment)) {
        ret << " -- " << comment;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void DescribeTarget(src_hdr::FSrc &src, src_hdr::FNs &ns, cstring &out) {
    tempstr hdr = Tag("Target", tempstr()<<ns.ns<<" ("<<ns.nstype<<")");
    if (ch_N(ns.comment.value)) {
        hdr<<" -- "<<ns.comment;
    }
    InsertComment(src,hdr,out);
    if (ns.c_nsx) {
        InsertComment(src,Tag("Exceptions", ns.c_nsx->genthrow?"yes":"NO"), out);
    }
    if (src.p_targsrc) {
        bool header = GetFileExt(src.src)==".h";
        strptr tagname = header ? "Header" : "Source";
        InsertComment(src,Tag(tagname
                              , tempstr()<<src.src<<DemarcatedComment(src.p_targsrc->comment))
                      ,out);
        InsertComment(src,"\n",out);
    }
}

// -----------------------------------------------------------------------------

// Write file contents using the following layout:
// <SHEBANG>
// <COPYRIGHTS>
// <LICENSE>
// <TARGSRC DESCRIPTION>
static void Save(src_hdr::FSrc &src) {
    cstring out;
    verblog("src_hdr.file"
            <<Keyval("file",src.src)
            <<Keyval("shebang",src.shebang)
            <<Keyval("license",src.p_license->text)
            <<Keyval("comment",src.comment)
            <<Keyval("cmtstring",src.cmtstring)
            <<Keyval("copyright",src.copyright));
    out << src.shebang;
    if (src.copyright != "") {
        InsertComment(src,src.copyright,out);
        InsertComment(src,"\n",out);
    }
    if (src.p_license) {
        strptr license = src.p_license->text;
        if (license != "") {
            InsertComment(src,Tag("License",src.p_license->license),out);
            InsertComment(src,strptr(license),out);
        }
    }
    if (src.contact_ice != "") {
        InsertComment(src,Tag("Contacting ICE",src.contact_ice),out);
    }
    if (src_hdr::FTargsrc *targsrc=src.p_targsrc) {
        DescribeTarget(src,*targsrc->p_target->p_ns,out);
    }
    InsertComment(src,src.comment,out);
    out<<eol;
    out<<src.body;

    if (src_hdr::_db.cmdline.write) {
        (void)SafeStringToFile(out,src.src,algo::FileFlags());
    }
}

// -----------------------------------------------------------------------------

static void ReadTagLine(src_hdr::FSrc &src, strptr line) {
    if (StartsWithQ(line,"Target:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"License:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"Comment:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"File:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"Source:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"Header:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"Exceptions:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"Contacting ICE:")) {
        src.contact_ice=Trimmed(Pathcomp(line,":LR"));
    } else if (StartsWithQ(line,"(C)")) {
        src.copyright<<line<<eol;
    } else if (StartsWithQ(line,"Copyright")) {
        src.copyright<<line<<eol;
    } else if (src_hdr::ind_fcopyline_Find(Trimmed(line))) {// copyright line - ignore
    } else {
        src.comment<<line<<eol;
    }
}

// -----------------------------------------------------------------------------

// YEARS: a string like
//    2023
//    2021-2023
//    1998,2001,2014-2016
//
static void UpdateCopyrightYear(algo::cstring &years, int year) {
    strptr prior_ranges = Trimmed(Pathcomp(years,",RL"));
    strptr last_range = Trimmed(Pathcomp(years,",RR"));
    strptr first_year = Pathcomp(last_range,"-RL");
    strptr last_year = Pathcomp(last_range,"-RR");
    i32 last_year_num = ParseU32(last_year,0);
    if (last_year_num < year) {
        tempstr new_years;
        if (ch_N(prior_ranges)) {
            new_years << prior_ranges << ",";
        }
        if (last_year_num+1 == year) {
            new_years << (ch_N(first_year)? first_year: last_year)<< "-";
        } else if (last_range != "") {
            new_years << last_range << ",";
        }
        new_years << year;
        years = new_years;
    }
}

// -----------------------------------------------------------------------------

// detect lines in the following format:
// Copyright (C) Company Name 2021-2013
// Copyright (C) 2021-2013 Company Name
static void ParseCopyright(algo::strptr text, algo::strptr &years, algo::strptr &company) {
    if (text != "" && algo_lib::DigitCharQ(text.elems[0])) {
        years=Pathcomp(text, " LL");
        company=Trimmed(Pathcomp(text, " LR"));
    } else {
        years=Pathcomp(text, " RR");
        company=Trimmed(Pathcomp(text, " RL"));
    }
}

// -----------------------------------------------------------------------------

// put current year copyright of company specified as -update_copyright arg
static void UpdateCopyright(src_hdr::FSrc &src) {
    u32 year = GetLocalTimeStruct(algo::CurrUnTime()).tm_year + 1900;
    ind_beg(src_hdr::_db_copyright_curs,copyright,src_hdr::_db) {
        copyright.years="";
    }ind_end;
    src_hdr::bh_copyright_RemoveAll();
    ind_beg(Line_curs,line,src.copyright) {
        algo::strptr text=Pathcomp(line, ")LR LR");// skip copyright part
        algo::strptr years, company;
        ParseCopyright(text,years,company);
        src_hdr::FCopyright *copyright=src_hdr::ind_copyright_Find(company);
        if (copyright) {
            copyright->years = years;
        }
    }ind_end;
    // insert default copyright
    if (src_hdr::_db.cmdline.update_copyright && src_hdr::_db.c_dflt_copyright) {
        UpdateCopyrightYear(src_hdr::_db.c_dflt_copyright->years, year);
    }
    // sort copyrights by last year
    ind_beg(src_hdr::_db_copyright_curs,copyright,src_hdr::_db) {
        copyright.sortkey = -ParseI32(Pathcomp(copyright.years,",RR-RR"),0);
        bh_copyright_Insert(copyright);
    }ind_end;
    tempstr new_copyright;
    // print copyrights back, print by descending year
    ind_beg(src_hdr::_db_bh_copyright_curs,copyright,src_hdr::_db) {
        if (copyright.years != "") {
            new_copyright << "Copyright (C) " << copyright.years << " "<< copyright.copyright << eol;
        }
    }ind_end;
    src.copyright = new_copyright;
}

// -----------------------------------------------------------------------------

static strptr GuessCmtString(strptr shebang) {
    shebang=Trimmed(shebang);
    strptr executable=Pathcomp(shebang," LR LL");
    strptr ret = executable == "node" ? "//"
        : "#";
    return ret;
}

// -----------------------------------------------------------------------------

static void RebuildHeader(src_hdr::FSrc &src) {
    algo_lib::MmapFile file;
    if (MmapFile_Load(file,src.src)) {
        src.text=file.text;
        bool inhdr=true;
        ind_beg(Line_curs,line,src.text) {
            if (ind_curs(line).i==0 && StartsWithQ(line,"#!")) {
                src.shebang << line << eol;
                // if the file had no extension, guess comment string from shebang line
                if (src.cmtstring == "") {
                    src.cmtstring = GuessCmtString(src.shebang);
                }
            } else {
                // empty lines following header are part of the header
                inhdr = inhdr && (StartsWithQ(line,src.cmtstring) || !ch_N(Trimmed(line)));
                if (inhdr) {
                    ReadTagLine(src,Trimmed(RestFrom(line,ch_N(src.cmtstring))));
                } else {
                    src.body<<line<<eol;
                }
            }
        }ind_end;
        UpdateCopyright(src);
        if (src_hdr::_db.cmdline.indent) {
            tempstr out;
            algo::InsertIndent(out, src.body,0);
            src.body=out;
        }
        Save(src);
    }
}

// -----------------------------------------------------------------------------

static void LoadLicense() {
    ind_beg(src_hdr::_db_license_curs,license,src_hdr::_db) {
        if (license.license != "") {
            license.text = FileToString(tempstr()<<"conf/"<<license.license<<".license.txt",algo::FileFlags());
            if (ch_N(license.text)) {
                license.text << eol;
            }
            ind_beg(algo::Line_curs,line,license.text) {
                src_hdr::ind_fcopyline_GetOrCreate(Trimmed(line));
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void src_hdr::Main() {
    algo_lib::Regx exclude;
    LoadLicense();
    vrfy (!src_hdr::_db.cmdline.update_copyright || src_hdr::_db.c_dflt_copyright,
          "src_hdr: default copyright not defined (please update dev.copyright table");
    (void)Regx_ReadStrptrMaybe(exclude,"(include/gen/%|cpp/gen/%|extern/%|bin/bootstrap/%)");
    if (_db.cmdline.update_copyright) {
        _db.cmdline.write=true;
    }
    ind_beg(src_hdr::_db_targsrc_curs,targsrc,src_hdr::_db) {
        if (Regx_Match(_db.cmdline.targsrc,targsrc.targsrc) && !Regx_Match(exclude,src_Get(targsrc))) {
            src_hdr::FSrc src;
            src.src=src_Get(targsrc);
            src.p_targsrc=&targsrc;
            src.p_license=targsrc.p_target->p_ns->p_license;
            src.cmtstring="//";
            RebuildHeader(src);
        }
    }ind_end;
    ind_beg(src_hdr::_db_scriptfile_curs,scriptfile,src_hdr::_db) {
        if (Regx_Match(_db.cmdline.scriptfile,scriptfile.gitfile) && !Regx_Match(exclude,scriptfile.gitfile)) {
            algo::strptr ext = GetFileExt(scriptfile.gitfile);
            src_hdr::FSrc src;
            src.src=scriptfile.gitfile;
            src.p_targsrc=NULL;
            src.p_license=scriptfile.p_license;
            src.cmtstring = ext == "" ? ""
                : ext == ".mjs" || ext == ".js" || ext == ".jsx" || ext == ".ts" || ext == ".tsx" ? "//"
                : ext == ".json" ? ""
                : "#";
            RebuildHeader(src);
        }
    }ind_end;

    // Update function prototypes
    if (src_hdr::_db.cmdline.write) {
        command::src_func src_func;
        src_func.updateproto=true;
        SysCmd(src_func_ToCmdline(src_func));
    }
}
