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
// Target: sv2ssim (exe) -- sv2ssim - Separated Value file processor
// Exceptions: yes
// Source: cpp/sv2ssim.cpp
//

#include "include/algo.h"
#include "include/sv2ssim.h"

// Add a new field with name NAME
// The name of the field is stripped of all non-identifier characters
// Field is indexed and added to the selected set (zd_selfield) if it matches _db.cmdline.field regx
void sv2ssim::AllocField(strptr name) {
    FField &field=field_Alloc();
    tempstr fldname;
    for (int i=0; i<ch_N(name); i++) {
        if (algo_lib::IdentCharQ(name.elems[i])) {
            fldname << name.elems[i];
        }
    }
    // handle empty fields
    if (fldname == "") {
        fldname = "field";
    }
    // avoid all-numeric field names
    if (!algo_lib::IdentStartQ(fldname.ch_elems[0])) {
        fldname = tempstr() << "_" << fldname;
    }
    // make field name unique
    int suffix=1;
    do {
        field.name = fldname;
        if (suffix>1) {
            field.name << suffix;
        }
        suffix++;
    } while (ind_field_Find(field.name) != NULL);
    // this will succeed
    vrfy_(field_XrefMaybe(field));
    // add to selected set
    if (Regx_Match(_db.cmdline.field,field.name)) {
        zd_selfield_Insert(field);
    }
}

// Convert line into token array _db.linetok
// If the file has a headerline, the remove any fields beyond those defined in the header
// If the file has no headerline, create fields on the fly
void sv2ssim::TokenizeLine(strptr line) {
    linetok_RemoveAll();
    algo_lib::CsvParse csvparse;
    csvparse.sep=_db.cmdline.separator;
    if (_db.cmdline.separator != ',') {
        csvparse.quotechar1=0;
        csvparse.quotechar2=0;
    }
    csvparse.input=line;
    RunCsvParse(csvparse);
    ind_beg(algo_lib::csvparse_ary_tok_curs,tok,csvparse) {
        linetok_Alloc()=tok;
    }ind_end;

    if (_db.cmdline.header) {
        if (field_N()>0 && linetok_N()>field_N()) {
            _db.n_wideline++;
            while (linetok_N() > field_N()) {
                linetok_RemoveLast();
            }
        }
    } else {
        while (field_N() < linetok_N()) {
            AllocField(tempstr()<<"field"<<field_N());
        }
    }
}

// Create FIELD records out of current tokens
void sv2ssim::CreateFields() {
    ind_beg(_db_linetok_curs,linetok,_db) {
        AllocField(linetok);
    }ind_end;
}

// Update FIELD records based on current tokens
void sv2ssim::UpdateFields() {
    ind_beg(_db_linetok_curs,linetok,_db) {
        FField &field=field_qFind(ind_curs(linetok).index);
        i32_UpdateMax(field.maxwid, ch_N(linetok));
        if (field.couldbe_bool) {
            bool b;
            if (!bool_ReadStrptrMaybe(b,linetok)) {
                verblog("field "<<field.name<<" tok "<<linetok<<" -> not bool");
                field.couldbe_bool=false;
            } else {
                double_UpdateMin(field.minval,b);
                double_UpdateMax(field.maxval,b);
            }
        }
        if (field.couldbe_int) {
            i64 val;
            if (!i64_ReadStrptrMaybe(val,linetok)) {
                verblog("field "<<field.name<<" tok "<<linetok<<" -> not integer");
                field.couldbe_int=false;
            } else {
                double_UpdateMin(field.minval,val);
                double_UpdateMax(field.maxval,val);
            }
        }
        if (field.couldbe_double) {
            double val;
            if (!double_ReadStrptrMaybe(val,linetok)) {
                verblog("field "<<field.name<<" tok "<<linetok<<" -> not double");
                field.couldbe_double=false;
            } else {
                double_UpdateMin(field.minval,val);
                double_UpdateMax(field.maxval,val);
            }
        }
        if (field.couldbe_fixwid) {
            int ndot=0;
            for (int i=0; i<ch_N(linetok); i++) {
                char ch=ch_qFind(linetok,i);
                if (!algo_lib::DigitCharQ(ch)) {
                    field.couldbe_fixwid=false;
                    break;
                }
                if (ch=='.') {
                    ndot++;
                    if (ndot==1) {
                        i32_UpdateMin(field.minwid_fix1,i);
                        i32_UpdateMax(field.minwid_fix1,i);
                        i32_UpdateMax(field.maxwid_fix2,ch_N(linetok)-1-i);
                        i32_UpdateMax(field.maxwid_fix2,ch_N(linetok)-1-i);
                    } else if (ndot>1) {
                        field.couldbe_fixwid=false;
                        break;
                    }
                }
            }
        }
    }ind_end;
}

// Pick types for each field and print ctype definition
void sv2ssim::OutputSchema() {
    dmmeta::Ctype ctype;
    ctype.ctype=_db.cmdline.ctype;
    prlog(ctype);
    if (_db.cmdline.ssimfile != "") {
        dmmeta::Ssimfile ssimfile;
        ssimfile.ssimfile=_db.cmdline.ssimfile;
        ssimfile.ctype=ctype.ctype;
        prlog(ssimfile);
    }
    dmmeta::Cfmt cfmt;
    cfmt.cfmt = dmmeta::Cfmt_Concat_ctype_strfmt(ctype.ctype, dmmeta_Strfmt_strfmt_String);
    cfmt.printfmt = dmmeta_Printfmt_printfmt_Tuple;
    cfmt.print = true;
    cfmt.read = true;
    cfmt.genop = true;
    prlog(cfmt);
    ind_beg(_db_field_curs,field,_db) if (zd_selfield_InLlistQ(field)) {
        sv2ssim::FSvtype *bestmatch=NULL;
        // search for best matching svtype
        ind_beg(_db_svtype_curs,svtype,_db) {
            sv2ssim::FBltin *bltin=ind_bltin_Find(svtype.ctype);
            bool fldnumeric = field.couldbe_int || field.couldbe_double;
            bool numeric = bltin && bltin->likeu64;
            bool issigned = bltin && bltin->issigned;
            bool match=false;
            if (svtype.ctype == "bool") {
                match=field.couldbe_bool;
            } else if (numeric) {
                u64 maxval = u64(1)<<svtype.maxwid;
                match = field.couldbe_int && field.maxval <= maxval;
                match = match && _db.cmdline.prefer_signed ? issigned : (issigned == (field.minval < 0));
            } else if (svtype.ctype == "double") {
                match = field.couldbe_double;
            } else {
                match = !fldnumeric && !numeric && field.maxwid <= svtype.maxwid;
            }
            if (match && (!bestmatch || svtype.maxwid < bestmatch->maxwid)) {
                bestmatch=&svtype;
            }
        }ind_end;
        if (bestmatch) {
            field.ctype=bestmatch->ctype;
        }
        verblog("#"<<field);
        // print field
        dmmeta::Field outfld;
        outfld.field = dmmeta::Field_Concat_ctype_name(ctype.ctype,field.name);
        outfld.reftype="Val";
        outfld.arg=bestmatch ? bestmatch->ctype : dmmeta::CtypePkey("algo.cstring");
        prlog(outfld);
    }ind_end;
}

void sv2ssim::ValidateCmdline() {
    if (_db.cmdline.schema && _db.cmdline.ctype == "") {
        vrfy(0, "-schema requires -ctype to be specified");
    }
}

// Print current tuple according to output format
// If -outseparator is set, use separator
//   With , -> use csv output files
//   With other separators, strip separator character from token before printing
tempstr sv2ssim::CurlineToString() {
    tempstr out;
    if (_db.cmdline.outseparator == "") {// ssim field
        out << _db.cmdline.ctype;// typetag
    }
    ind_beg(_db_zd_selfield_curs,field,_db) {
        cstring *tok = linetok_Find(field.rowid);
        strptr tokval = tok ? strptr(*tok) : strptr();
        if (out != "") {
            out << _db.cmdline.outseparator;
        }
        if (_db.cmdline.outseparator == "") {// ssim field
            out << Keyval(field.name,tokval);
        } else if (_db.cmdline.outseparator == ",") {// csv field
            strptr_PrintCsv(tokval,out,0);
        } else {// other separators: strip separator char from field
            tempstr str;
            str << tokval;
            Replace(str,_db.cmdline.outseparator,"");
            out << str;
        }
    }ind_end;
    return out;
}

void sv2ssim::Main() {
    algo_lib::FFildes fildes;
    if (_db.cmdline.fname=="-") {
        fildes.fd.value=0;
    } else {
        fildes.fd=OpenRead(_db.cmdline.fname);
    }
    ValidateCmdline();
    int n_nonempty_line=0;// number of non-empty lines read
    int n_line=0;
    ind_beg(algo::FileLine_curs,line,fildes.fd) {
        n_line++;
        if (line != "") {// skip empty lines
            n_nonempty_line++;
            TokenizeLine(line);
            bool isheader = n_nonempty_line==1 && _db.cmdline.header;
            if (isheader) {
                CreateFields();
            } else {
                UpdateFields();
            }
            if (_db.cmdline.schema && algo_lib::_db.cmdline.verbose) {
                verblog("#"<<CurlineToString());
            }
            if (_db.cmdline.data && !isheader) {
                prlog(CurlineToString());
            }
        }
    }ind_end;
    if (_db.cmdline.report) {
        prlog("sv2ssim.report"
              <<Keyval("n_line",n_line)
              <<Keyval("n_nonempty_line",n_nonempty_line)
              <<Keyval("n_field",field_N())
              <<Keyval("n_wideline",_db.n_wideline));
    }
    if (_db.cmdline.schema) {
        OutputSchema();
    }
}
