// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023-2023 AlgoRND
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: src_hdr (exe) -- Update source file / copyright header
// Exceptions: yes
// Source: cpp/src/hdr.cpp
//

#include "include/algo.h"
#include "include/gen/src_hdr_gen.h"
#include "include/gen/src_hdr_gen.inl.h"
#include "include/gen/dev_gen.h"
#include "include/gen/dev_gen.inl.h"

// -----------------------------------------------------------------------------

static void InsertComment(strptr text, cstring &out) {
    ind_beg(Line_curs,line,text) {
        out<<"//";
        if (ch_N(line)) {
            out<<" "<<line;
        }
        out<<eol;
    }ind_end;
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

static void DescribeTarget(src_hdr::FNs &ns, cstring &out, src_hdr::FTargsrc &targsrc) {
    tempstr hdr = Tag("Target", tempstr()<<ns.ns<<" ("<<ns.nstype<<")");
    if (ch_N(ns.comment.value)) {
        hdr<<" -- "<<ns.comment;
    }
    InsertComment(hdr,out);
    if (ns.c_nsx) {
        InsertComment(Tag("Exceptions", ns.c_nsx->genthrow?"yes":"NO"), out);
    }
    bool header = ext_Get(targsrc)=="h";
    strptr tagname = header ? "Header" : "Source";
    InsertComment(Tag(tagname
                      , tempstr()<<src_Get(targsrc)<<DemarcatedComment(targsrc.comment))
                  ,out);
    InsertComment("\n",out);
}

// -----------------------------------------------------------------------------

static void AddCopyright(src_hdr::FSrc &src, cstring &out) {
    InsertComment(src.copyright,out);
    InsertComment("\n",out);
}

// -----------------------------------------------------------------------------

static void Save(src_hdr::FSrc &src) {
    cstring out;
    AddCopyright(src,out);

    strptr license = src.p_targsrc->p_target->p_license->text;
    if (license != "") {
        InsertComment(strptr(license),out);
    }
    // if copyright came from ICE, add contact info
    if (FindStr(src.copyright, "Intercontinental Exchange")!=-1) {
        InsertComment("Contacting ICE: <https://www.theice.com/contact>\n\n",out);
    }
    src_hdr::FTargsrc &targsrc=*src.p_targsrc;
    // add namespace info
    DescribeTarget(*targsrc.p_target->p_ns,out,targsrc);
    InsertComment(src.comment,out);
    out<<eol;
    out<<src.body;

    (void)SafeStringToFile(out,src_Get(*src.p_targsrc),algo::FileFlags());
}

// -----------------------------------------------------------------------------

static void ReadTagLine(src_hdr::FSrc &src, strptr line) {
    if (StartsWithQ(line,"Target:")) {
        src.saw_target = true;
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
    } else if (StartsWithQ(line,"(C)")) {
        src.copyright<<line<<eol;
    } else if (StartsWithQ(line,"Copyright")) {
        src.copyright<<line<<eol;
    } else if (ch_N(line)) {
        if (src.saw_target) {
            src.comment<<line<<eol;
        } else {
            // this line must be part of the license text that sits
            // at the top of every source file.
            // all unrecognized text prior to 'Target:' is discarded.
        }
    }
}

// -----------------------------------------------------------------------------

// put current year copyright of company specified as -update_copyright arg
static void UpdateCopyright(src_hdr::FSrc &src) {
    strptr our_company = src_hdr::_db.cmdline.update_copyright;
    u32 year = GetLocalTimeStruct(algo::CurrUnTime()).tm_year + 1900;
    bool ok(false);
    tempstr new_copyright;
    ind_beg(Line_curs,line,src.copyright) {
        algo::StringIter it(line);
        strptr copyright = GetWordCharf(it);
        strptr parenc = GetWordCharf(it);
        strptr years = GetWordCharf(it);
        strptr company = Trimmed(it.Rest());
        if (company == our_company) {
            ok = true;
            strptr prior_ranges = Pathcomp(years,",RL");
            strptr last_range = Pathcomp(years,",RR");
            strptr first_year = Pathcomp(last_range,"-RL");
            u32    last_year = ParseU32(Pathcomp(last_range,"-RR"),0);
            if (last_year < year) {
                new_copyright << copyright << " " << parenc << " ";
                if (ch_N(prior_ranges)) {
                    new_copyright << prior_ranges << ",";
                }
                if (last_year+1 == year) {
                    new_copyright << (ch_N(first_year)
                                      ? first_year
                                      : tempstr()<<last_year)
                                  << "-" << year;
                } else {
                    new_copyright << last_range << "," << year;
                }
                new_copyright << " " << our_company << eol;
            } else {
                new_copyright << line << eol;
            }
        } else {
            new_copyright << line << eol;
        }
    }ind_end;
    if (!ok) {
        new_copyright << "Copyright (C) " << year << " " << our_company;
    }
    src.copyright = new_copyright;
}

// -----------------------------------------------------------------------------

static void RebuildHeader(src_hdr::FSrc &src) {
    bool inhdr=true;
    ind_beg(Line_curs,line,src.text) {
        // empty lines following header are part of the header
        inhdr = inhdr && (StartsWithQ(line,"//") || !ch_N(Trimmed(line)));
        if (inhdr) {
            ReadTagLine(src,Trimmed(RestFrom(line,2)));
        } else {
            src.body<<line<<eol;
        }
    }ind_end;
    if (ch_N(src_hdr::_db.cmdline.update_copyright)) {
        UpdateCopyright(src);
    }
    if (src_hdr::_db.cmdline.indent) {
        tempstr out;
        algo::InsertIndent(out, src.body,0);
        src.body=out;
    }
    // prlog("src_hdr.file"
    //    <<Keyval("src",src_Get(*src.p_targsrc))
    //    <<Keyval("comment",src.comment));
    if (src_hdr::_db.cmdline.write) {
        Save(src);
    }
}

// -----------------------------------------------------------------------------

static bool ScanTargsrcQ(src_hdr::FTargsrc &targsrc, algo_lib::Regx &exclude) {
    bool match = Regx_Match(src_hdr::_db.cmdline.targsrc,targsrc.targsrc)
        && !Regx_Match(exclude,src_Get(targsrc));
    return match;
}

// -----------------------------------------------------------------------------

static void ScanTargsrc(src_hdr::FTargsrc &targsrc) {
    algo_lib::MmapFile file;
    if (MmapFile_Load(file,src_Get(targsrc))) {
        src_hdr::FSrc src;
        src.p_targsrc=&targsrc;
        src.text=file.text;
        RebuildHeader(src);
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
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void src_hdr::Main() {
    algo_lib::Regx exclude;
    LoadLicense();
    (void)Regx_ReadStrptrMaybe(exclude,"(include/gen/%|cpp/gen/%|extern/%)");
    ind_beg(src_hdr::_db_targsrc_curs,targsrc,src_hdr::_db) {
        if (ScanTargsrcQ(targsrc,exclude)) {
            ScanTargsrc(targsrc);
        }
    }ind_end;

    // Update function prototypes
    if (src_hdr::_db.cmdline.write) {
        command::src_func src_func;
        src_func.updateproto=true;
        SysCmd(src_func_ToCmdline(src_func));
    }
}
