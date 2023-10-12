// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/main.cpp
//
// ACR: Algo Cross-Reference
// See txt/acr.md for more information

#include "include/acr.h"

// -----------------------------------------------------------------------------

algo::UnTime acr::FdModTime(algo::Fildes fd) {
    struct stat S;
    algo::UnTime ret;
    if (fstat(fd.value, &S)==0) {
        ret = ToUnTime(algo::UnixTime(S.st_mtime));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Parse query and return object
acr::FQuery& acr::ParseQuery(strptr expr) {
    acr::FQuery& query = acr::query_Alloc();

    // query: RELNAME:VAL or NS.RELNAME:VAL  NS.RELNAME.FIELD:VAL
    strptr key    = Pathcomp(expr, ":LL");
    strptr value  = Pathcomp(expr, ":LR");
    if (value == "") {
        value = "%";
    }
    Regx_ReadAcr(query.query.value, value, true);
    query.queryop = acr_Queryop_value_select;

    // query=RELNAME:VAL            s1:RELNAME   s2:""             s3:""         s4:""
    // query=NS.RELNAME:VAL         s1:NS        s2:RELNAME        s3:RELNAME    s4:""
    // query=NS.RELNAME.FIELD:VAL   s1:NS        s2:RELNAME.FIELD  s3:RELNAME    s4:FIELD
    // so, field is always s4.
    // relname is s3 if s3 is non-empty, else s1.
    // ns is s1 if s3 is non-empty, else ""
    strptr s1    = Pathcomp(key, ".LL");
    strptr s2    = Pathcomp(key, ".LR");
    strptr s3    = Pathcomp(s2,  ".LL");
    strptr s4    = Pathcomp(s2,  ".LR");

    strptr ns      = elems_N(s3) ? s1 : strptr();
    strptr relname = elems_N(s3) ? s3 : s1;
    strptr field   = s4;

    if (!elems_N(ns)) {// omitted namespace = all namespace where this table matches
        ns = "%";
    }

    Regx_ReadAcr(query.ssimfile, tempstr() << ns << "." << relname, true);
    Regx_ReadAcr(query.query.name, field, true);
    ind_beg(command::acr_where_curs,where,_db.cmdline) {
        acr::AttrRegx &attr=where_Alloc(query);
        Regx_ReadAcr(attr.name,Pathcomp(where,":LL"),true);
        Regx_ReadAcr(attr.value,Pathcomp(where,":LR"),true);
    }ind_end;

    query.comment = "main command-line query";
    (void)acr::query_XrefMaybe(query);
    return query;
}

// -----------------------------------------------------------------------------

strptr acr::Typetag(acr::FCtype &ctype) {
    return ctype.c_ssimfile ? strptr(ctype.c_ssimfile->ssimfile) : strptr(ctype.ctype);
}

// -----------------------------------------------------------------------------

// cached lookup
// ignore:ptr_byref
void acr::LookupField(acr::FRec &rec, strptr fieldname, acr::FCtype *&prev_ctype, acr::FField *&prev_field) {
    if (rec.p_ctype != prev_ctype) {
        prev_ctype = rec.p_ctype;
        prev_field = acr::ind_field_Find(tempstr()<<rec.p_ctype->ctype<<"."<<fieldname);
    }
}

// -----------------------------------------------------------------------------

static void Main_RewriteOpts() {
    if (acr::_db.cmdline.check || acr::_db.cmdline.my) {
        acr::_db.cmdline.print = false;
    }
    if (acr::_db.cmdline.meta) {
        acr::_db.cmdline.tree = true;
    }
    if (field_N(acr::_db.cmdline) > 0) {
        acr::_db.cmdline.print = false;
        acr::_db.cmdline.report = false;
    }
    if (ch_N(acr::_db.cmdline.regxof) > 0) {
        acr::_db.cmdline.print = false;
        acr::_db.cmdline.report = false;
    }
    if (acr::_db.cmdline.merge) {
        acr::_db.cmdline.replace=true;// merge implies replace
    }
    if (acr::_db.cmdline.t) {
        acr::_db.cmdline.loose  = true;
        acr::_db.cmdline.pretty = true;
        acr::_db.cmdline.xref   = true;
        acr::_db.cmdline.tree   = true;
    }
    if (acr::_db.cmdline.e) {
        acr::_db.cmdline.cmt   = true;
        // #AL# short mode is still buggy -- don't enable yet, unless asked explicitly
        //acr::_db.cmdline.s     = true; //  short mode
        acr::_db.cmdline.rowid = true; // force rowid to avoid editing errors
    }
    if (acr::_db.cmdline.unused) {
        acr::_db.cmdline.nup = 0;
        acr::_db.cmdline.ndown = 1;
    }
    // cascading delete
    if (acr::_db.cmdline.del) {
        acr::_db.cmdline.select   = true;
        acr::_db.cmdline.nup      = 0;
        acr::_db.cmdline.ndown    = INT_MAX;
    }
    // check
    if (acr::_db.cmdline.check) {
        acr::_db.cmdline.select = true;
        acr::_db.cmdline.nup = 1;
        acr::_db.cmdline.ndown = 0;
    }
    if (acr::_db.cmdline.xref) {
        if (!acr::_db.cmdline.nup) acr::_db.cmdline.nup = 100; // set unless overridden
        if (!acr::_db.cmdline.ndown) acr::_db.cmdline.ndown = 100;
    }
}

// -----------------------------------------------------------------------------

static void Main_BuildRefmap() {
    // Build map field->target ssimfile (for going up)
    // Build the inverse map as well.
    ind_beg(acr::_db_ctype_curs, ctype,acr::_db) {
        ind_beg(acr::ctype_c_field_curs, field, ctype) {
            acr::c_child_Insert(*field.p_arg, ctype);
        }ind_end;
        // determine if ssimfile requires rowid.
        // do not show rowid if sort is enabled, and sort field is the primary key.
        ctype.show_rowid = acr::_db.cmdline.rowid;
        acr::FField *first_fld = c_field_Find(ctype, 0);
        if (ctype.show_rowid && first_fld) {
            ctype.show_rowid = !ctype.c_ssimfile || !(ctype.c_ssimfile->c_ssimsort && ctype.c_ssimfile->c_ssimsort->sortfld == first_fld->field);
        }
    }ind_end;
    // If -in is a file or stdin, load tuples from stdin.
    // Otherwise, they will be loaded on-demand during query run.
}

// -----------------------------------------------------------------------------

static void Main_SelectRename() {
    acr::FQuery& fquery = acr::ParseQuery(acr::_db.cmdline.query);
    fquery.nup    = acr::_db.cmdline.nup;
    fquery.ndown  = acr::_db.cmdline.ndown;
    fquery.unused = acr::_db.cmdline.unused;
    fquery.delrec = acr::_db.cmdline.del;
    fquery.selmeta= acr::_db.cmdline.meta;
    if (ch_N(acr::_db.cmdline.rename)) {
        fquery.new_val = acr::_db.cmdline.rename;
        fquery.queryop = acr_Queryop_value_set_attr;
    }
    fquery.comment = "command-line rename request";
    acr::RunAllQueries();    // QUERY EXECUTION HERE
}

// -----------------------------------------------------------------------------

static void PrintReport() {
    if (acr::_db.cmdline.report) {
        acr::_db.report.n_select = acr::zd_all_selrec_N();
        tempstr out;
        if (ch_N(acr::_db.cmdline.cmd)>0) {
            out <<"# ";// hide report from bash execution
        }
        out << acr::_db.report;
        prlog(out);
    }
}

// -----------------------------------------------------------------------------

static void Main_Print() {
    acr::FPrint print;
    print.tree     = acr::_db.cmdline.tree;
    print.fldfunc  = acr::_db.cmdline.fldfunc;
    print.pretty   = acr::_db.cmdline.pretty;
    print.maxgroup = acr::_db.cmdline.maxgroup;
    print.cmt      = acr::_db.cmdline.cmt;
    print.loose    = acr::_db.cmdline.loose;
    print.showstatus = true;// acr.insert, acr.update etc
    acr::Print(print);
}

// -----------------------------------------------------------------------------

static void InitFieldProps() {
    ind_beg(acr::_db_smallstr_curs,smallstr,acr::_db) {
        if (acr::FField *field=acr::ind_field_Find(smallstr.field)) {
            field->max_attr_len=smallstr.length;
        }
    }ind_end;
    ind_beg(acr::_db_substr_curs,substr,acr::_db) {
        if (acr::FField *field=acr::ind_field_Find(substr.field)) {
            field->isfldfunc=true;
        }
    }ind_end;
    ind_beg(acr::_db_cppfunc_curs,cppfunc,acr::_db) {
        if (acr::FField *field=acr::ind_field_Find(cppfunc.field)) {
            field->isfldfunc=true;
        }
    }ind_end;
    ind_beg(acr::_db_anonfld_curs,anonfld,acr::_db) {
        if (acr::FField *field=acr::ind_field_Find(anonfld.field)) {
            field->anon=true;
        }
    }ind_end;
    ind_beg(acr::_db_funique_curs,funique,acr::_db) {
        if (acr::FField *field=acr::ind_field_Find(funique.field)) {
            field->unique=true;
        }
    }ind_end;
    if (acr::FField *field=acr::ind_field_Find("algo.Comment.value")) {
        field->max_attr_len=150;
    }
}

// -----------------------------------------------------------------------------

void acr::Main() {
    Main_RewriteOpts();
    // determine input/output modes
    // see FileInputQ(), FileOutputQ(), GetOutPath()
    acr::_db.file_input = FileQ(acr::_db.cmdline.in);

    Main_BuildRefmap();
    Main_ReadIn();
    InitFieldProps();

    // create main query
    if (acr::_db.cmdline.select || ch_N(acr::_db.cmdline.rename)) {
        Main_SelectRename();
    }

    if (acr::_db.cmdline.check) {    // validate data and print suggestions.
        acr::Main_Check();
    }

    if (acr::_db.cmdline.my) {
        acr::Main_Mysql();
    }

    RunAllQueries();

    // select renamed records
    acr::Rec_SelectModified();

    // edit mode
    if (acr::_db.cmdline.e) {
        acr::Main_AcrEdit();
        acr::_db.cmdline.cmt=false;
    }
    // print command to be executed for each matching tuple
    if (ch_N(acr::_db.cmdline.cmd)>0) {
        Main_Cmd();
    }
    if (field_N(acr::_db.cmdline)>0) {
        Main_Field();
    } else if (ch_N(acr::_db.cmdline.regxof)>0) {
        Main_Regxof();
    } else if (acr::_db.cmdline.print) {
        Main_Print();
    }
    // do not write files if an error occurred
    if (acr::_db.cmdline.write && (acr::_db.cmdline.e || algo_lib::_db.exit_code==0)) {
        acr::WriteFiles();
    }
    // in interactive mode, return # of files modified
    // amc -e mode uses this flag
    if (acr::_db.cmdline.e) {
        algo_lib::_db.exit_code = acr::_db.report.n_file_mod;
    }
    PrintReport();
}
