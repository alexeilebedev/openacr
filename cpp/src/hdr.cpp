// (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/gen/src_hdr_gen.h"
#include "include/gen/src_hdr_gen.inl.h"

static const char *license =
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
    "\n";

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

static void InsertCommentMaybe(strptr name, strptr value, cstring &out) {
    if (ch_N(value)) {
        InsertComment(tempstr()<<name<<": "<<value, out);
    }
}

// -----------------------------------------------------------------------------

static void Authors(src_hdr::FSrc &src, cstring &out) {
    int n=ch_N(out);
    InsertCommentMaybe("Created By", src.created_by, out);
    InsertCommentMaybe("Authors", src.authors, out);
    InsertCommentMaybe("Recent Changes", src.recent_changes, out);
    if (ch_N(out)>n) {
        InsertComment("\n",out);
    }
}

// -----------------------------------------------------------------------------

static void Save(src_hdr::FSrc &src) {
    cstring out;
    AddCopyright(src,out);
    InsertComment(strptr(license),out);
    // if copyright came from ICE, add contact info
    if (FindStr(src.copyright, "Intercontinental Exchange")!=-1) {
        InsertComment("Contacting ICE: <https://www.theice.com/contact>\n\n",out);
    }
    src_hdr::FTargsrc &targsrc=*src.p_targsrc;
    // add namespace info
    DescribeTarget(*targsrc.p_ns,out,targsrc);
    Authors(src,out);
    InsertComment(src.comment,out);
    out<<eol;
    out<<src.body;

    (void)StringToFile(out,src_Get(*src.p_targsrc));
}

// -----------------------------------------------------------------------------

// a "tag" is a header line of the form "A: B"
static tempstr GetTagValue(strptr line) {
    return tempstr(Trimmed(Pathcomp(line,":LR")));
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
    } else if (StartsWithQ(line,"Created By:")) {
        src.created_by = GetTagValue(line);
    } else if (StartsWithQ(line,"Authors:")) {
        src.authors = GetTagValue(line);
    } else if (StartsWithQ(line,"Recent Changes:")) {
        src.recent_changes = GetTagValue(line);
    } else if (StartsWithQ(line,"Exceptions:")) {
        // ignore, we re-generate this
    } else if (StartsWithQ(line,"(C)")) {
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

static void UpdateAuthors(src_hdr::FSrc &src) {
    tempstr cmd=tempstr()<<"bin/git-authors "<<src_Get(*src.p_targsrc);
    tempstr data=SysEval(cmd,FailokQ(true),1024*1024);
    ind_beg(Line_curs,line,data) {
        ReadTagLine(src,Trimmed(line));
    }ind_end;
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
    if (src_hdr::_db.cmdline.update_authors) {
        UpdateAuthors(src);
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
    MmapFile file;
    if (MmapFile_Load(file,src_Get(targsrc))) {
        src_hdr::FSrc src;
        src.p_targsrc=&targsrc;
        src.text=file.text;
        RebuildHeader(src);
    }
}

// -----------------------------------------------------------------------------

void src_hdr::Main() {
    algo_lib::Regx exclude;
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
