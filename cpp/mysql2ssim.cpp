// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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
// Target: mysql2ssim (exe) -- mysql -> ssim conversion tool
// Exceptions: NO
// Source: cpp/mysql2ssim.cpp
//
// Dump contents of a mysql database, or some set of tables as an ssim file.
// Optionally dump just the database schema.

#include "include/algo.h"
#include "include/gen/mysql2ssim_gen.h"
#include "include/gen/mysql2ssim_gen.inl.h"
#include "include/lib_mysql.h"
#include <mariadb/mysqld_error.h> // for ER_BAD_DB_ERROR

// -----------------------------------------------------------------------------

static dmmeta::CtypePkey MapDecimal(mysql2ssim::FTobltin &tobltin, strptr param) {
    algo::i32_Range DR = TFind(param,',');
    i32 fxsize;
    i32 flsize;
    dmmeta::CtypePkey ret;

    vrfy(i32_ReadStrptrMaybe(fxsize, FirstN(param,DR.beg)), algo_lib::_db.errtext);
    vrfy(i32_ReadStrptrMaybe(flsize, RestFrom(param,DR.end)), algo_lib::_db.errtext);

    // i32 can store 9 digits, i64 - 18 digits (signed)
    if (flsize == 0) {
        ret = "i64";
        tobltin.warn = true;
    } else if ((fxsize+flsize < 9) && (flsize <= 4)) {
        ret = tempstr() << "I32Dec" << flsize;
    } else {
        if (flsize > 7) {
            tobltin.warn = true; // Precision will be limited
            flsize = 7;
        }
        ret = tempstr() << "I64Dec" << flsize;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void CvtEnum(mysql2ssim::FTobltin &tobltin, dmmeta::Field &field) {
    int max = 0;
    frep_(i,vals_N(tobltin)) {
        max = i32_Max(max, ch_N(vals_qFind(tobltin,i)));
    }
    if (   (vals_qFind(tobltin,0)=="'N'" && vals_qFind(tobltin,1)=="'Y'")
           || (vals_qFind(tobltin,0)=="'Y'" && vals_qFind(tobltin,1)=="'N'")) {
        field.arg = "Bool";
        field.dflt.value        = "false";
    } else {
        tobltin.warn = true;
        field.arg = tempstr() << "Smallstr" << max;
    }
}

// -----------------------------------------------------------------------------

static void CvtChar(mysql2ssim::FTobltin &tobltin, dmmeta::Field &field) {
    vrfy(vals_N(tobltin), "no values defined");
    int width;
    vrfy(i32_ReadStrptrMaybe(width, vals_qFind(tobltin,0)), algo_lib::_db.errtext);
    field.arg = width == 1 ? strptr("char") : strptr(tempstr() << "RspaceStr"<<width);
    field.dflt.value  = "' '";
}

// -----------------------------------------------------------------------------

static void CvtVarchar(mysql2ssim::FTobltin &tobltin, dmmeta::Field &field) {
    vrfy(vals_N(tobltin), "no values defined");
    int  width;
    vrfy(i32_ReadStrptrMaybe(width, vals_qFind(tobltin,0)), algo_lib::_db.errtext);
    field.arg = width<=253 ? strptr(tempstr() << "Smallstr" << width) : strptr("algo.cstring");
}

// -----------------------------------------------------------------------------

//
// STR          input MYSQL type
// FIELD        target TFC "field" record
// Convert MYSQL type as best we can to a TFC type.
//
static void ToBltin(strptr str, dmmeta::Field &field) {
    algo::i32_Range  L      = TFind(str, '(');
    algo::i32_Range  R      = TFind(str, ')');
    strptr type   = FirstN(str, L.beg);
    strptr suffix = Trimmed(RestFrom(str, R.end));
    bool   uns    = range_N(substr_FindFirst(suffix, strptr("unsigned"))) > 0;
    strptr param  = qGetRegion(str, L.end, R.beg-L.end);
    mysql2ssim::FTobltin  tobltin;
    ind_beg(algo::Sep_curs,tok,param,',') {
        vals_Alloc(tobltin) = tok;
    }ind_end;
    if (type == "tinyint") {
        field.arg = uns ? "u8" : "i8";
    } else if (type == "mediumint" || type == "int") {
        field.arg = uns ? "u32" : "i32";
    } else if (type == "smallint") {
        field.arg = uns ? "u16" : "i16";
    } else if (type == "bigint") {
        field.arg = uns ? "u64" : "i64";
    } else if (type == "decimal" || type == "numeric") {
        field.arg = MapDecimal(tobltin,param);
    } else if (type == "float") {
        field.arg = "float";
        tobltin.warn = vals_N(tobltin) > 0;
    } else if (type == "text") {
        field.arg = "algo.cstring";
    } else if (type == "double") {
        field.arg = "double";
        tobltin.warn = vals_N(tobltin) > 0;
    } else if (type == "char") {
        CvtChar(tobltin,field);
    } else if (type == "varchar") {
        CvtVarchar(tobltin,field);
    } else if (type == "enum") {
        CvtEnum(tobltin,field);
    } else if (type == "datetime") {
        field.arg = "UnTime";
        field.dflt.value = "";
    } else if (type == "date") {
        field.arg = "UnixTime";
        field.dflt.value = "";
    } else if (type == "time") {
        field.arg = "UnDiff";
        field.dflt.value = "";
    } else {
        tobltin.err=true;
    }
    vrfy(!tobltin.err,tempstr()<< "cannot convert type "<<str);
    if (tobltin.warn) {
        prerr("mysql2ssim.conversion_warning"
              <<Keyval("type",str)
              <<Keyval("target",field.arg)
              <<Keyval("comment","Cannot convert to target type"));
    }
}

// -----------------------------------------------------------------------------

static tempstr TableList() {
    tempstr ret;
    mysql2ssim::in_tables_RemoveAll();
    strptr tables = mysql2ssim::_db.cmdline.tables;
    while (elems_N(tables)) {
        algo::i32_Range r2   = TFind(tables,',');
        mysql2ssim::in_tables_Alloc() = FirstN(tables,r2.beg);
        tables = RestFrom(tables,r2.end);
    }
    algo::ListSep ls(",");
    ind_beg(mysql2ssim::_db_in_tables_curs, E, mysql2ssim::_db) {
        ret << ls;
        lib_mysql::MPrintQuoted(ret, lib_mysql::_db.mysql, E, '\'');
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

static void LoadList(strptr db) {
    cstring query;
    query<<"SELECT table_name FROM information_schema.tables WHERE table_schema=?";
    lib_mysql::MBind(lib_mysql::_db.mysql,query,db,'\'');
    tempstr tables(TableList());
    if (ch_N(tables)) {
        query << " AND TABLE_NAME IN (" << tables << ")";
    }
    lib_mysql::Res    res;
    MQuery(lib_mysql::_db.mysql, query, res);
    MYSQL_ROW            row;
    while ((row = mysql_fetch_row(res.res)) != NULL){
        mysql2ssim::table_names_Alloc() = row[0];
    }
}

// -----------------------------------------------------------------------------

static void ReadSchema(strptr db, strptr table_name) {
    tempstr ctype_out;
    ctype_out << db;
    ctype_out << ".";
    strptr_PrintCamel(table_name, ctype_out);

    dmmeta::Ctype ctype;
    ctype.ctype  = ctype_out;
    prlog(ctype);

    // type came from database -- therefore link it back.
    dmmeta::Ssimfile ssimfile;
    ssimfile.ssimfile = tempstr() << db << "." << table_name;
    ssimfile.ctype = ctype.ctype;
    prlog(ssimfile);

    cstring query;
    query = "SELECT"
        " table_schema AS `db`" // 0
        " ,table_name AS `table`" // 1
        " ,column_name AS `column`" // 2
        " ,column_type AS `type`"   // 3
        " ,is_nullable AS `null`"   // 4
        " ,column_key AS `key`"     // 5
        " ,column_default AS `dflt`" // 6
        " FROM information_schema.columns where table_schema=? and table_name=?";
    lib_mysql::MBind(lib_mysql::_db.mysql, query, db, '\'');
    lib_mysql::MBind(lib_mysql::_db.mysql, query, table_name, '\'');

    lib_mysql::Res res;
    MQuery(lib_mysql::_db.mysql, query, res);
    int    nfields       = mysql_field_count(lib_mysql::_db.mysql);
    algo::aryptr<MYSQL_FIELD>  fields(mysql_fetch_fields(res.res), nfields);
    MYSQL_ROW            mysql_row;
    int idx              = 0;

    while ((mysql_row = mysql_fetch_row(res.res)) != NULL) {
        prlog_("*");
        dmmeta::Field field;
        field.field         = tempstr() << ctype.ctype<<"." << mysql_row[2];
        field.dflt.value  = mysql_row[6];
        ToBltin(mysql_row[3], field);
        idx++;
        prlog(field);
    }
}

// -----------------------------------------------------------------------------

static void ReadData_Ssim(Attr &head, lib_mysql::Res &res, int nfields, int skipfield, algo::aryptr<MYSQL_FIELD> fields) {
    cstring str;
    MYSQL_ROW            mysql_row;
    while ((mysql_row = mysql_fetch_row(res.res)) != NULL) {
        PrintAttr(str, head.name, head.value);
        frep_(i,nfields) {
            if (mysql_row[i] && i!=skipfield) {
                PrintAttrSpace(str, fields[i].name, mysql_row[i]);
            }
        }
        if (mysql2ssim::_db.cmdline.writessimfile) {
            str << eol;
        } else {
            //print string and flush
            str << '\n';
            algo::Prlog(&algo_lib_logcat_stdout,algo::SchedTime(),str);
            ch_RemoveAll(str);
        }
    }
    if (mysql2ssim::_db.cmdline.writessimfile) {
        // append data to file
        StringToFile(str, mysql2ssim::_db.tempfile.filename, algo_FileFlags_append);
    }
}

// -----------------------------------------------------------------------------

static void ReadData_Table(Attr &head, lib_mysql::Res &res, int nfields, algo::aryptr<MYSQL_FIELD> fields) {
    // print aligned rectangular blocks
    int screenful = 100;
    algo_lib::FTxttbl table;
    MYSQL_ROW            mysql_row;
    while ((mysql_row = mysql_fetch_row(res.res)) != NULL) {
        AddRow(table);
        PrintAttr(AddCell(table), head.name, head.value);
        frep_(i,nfields) {
            cstring &cell = AddCell(table);
            if (!mysql_row[i]) {
                continue;
            }
            PrintAttr(cell, fields[i].name, mysql_row[i]);
        }
        if (c_txtrow_N(table) >= screenful) {
            prlog(table);
            algo::Refurbish(table);
        }
    }
    if (c_txtrow_N(table) > 0) {
        prlog(table);
    }
}

// -----------------------------------------------------------------------------

static void ReadData(strptr db, strptr table_name) {
    cstring query("SELECT * FROM ?");
    Attr head;
    lib_mysql::MBind(lib_mysql::_db.mysql, query, table_name, '`');
    ch_RemoveAll(head.value);
    head.value<< db << "." << table_name;
    lib_mysql::Res res;
    lib_mysql::MQuery(lib_mysql::_db.mysql, query, res);
    int    nfields       = mysql_field_count(lib_mysql::_db.mysql);
    algo::aryptr<MYSQL_FIELD>  fields(mysql_fetch_fields(res.res), nfields);
    // on the way back, skip the bad column
    int skipfield=-1;
    frep_(i,nfields) {
        if (strptr(fields[i].name) == strptr("extra_column_for_roundtrip")) {
            skipfield=i;
        }
    }
    //
    // print tuples
    //
    if (!mysql2ssim::_db.cmdline.pretty) {
        ReadData_Ssim(head,res,nfields,skipfield,fields);
    } else {
        ReadData_Table(head,res,nfields,fields);
    }
}

// -----------------------------------------------------------------------------

static void Logo() {
    // identify ourselves
    cstring revision;
    revision << "# Copyright (C) 2013-2014 NYSE|Intercontinental Exchange\n";
    revision << "# generated by mysql2ssim $Revision:: 2519$\n";
    Replace(revision, "$", "");
    if (!mysql2ssim::_db.cmdline.nologo) {
        prlog(revision);
    }
}

// -----------------------------------------------------------------------------

static void Connect(URL &url, strptr db) {
    strptr sock = Pathcomp(url.dir,"/RL");
    bool issock = url.protocol == "sock";
    bool connect_ok = mysql_real_connect(lib_mysql::_db.mysql
                                         , (issock ? NULL : Zeroterm(tempstr() << url.server))
                                         , Zeroterm(tempstr() << url.username)
                                         , Zeroterm(tempstr() << url.password)
                                         , Zeroterm(tempstr() << db)
                                         , 0
                                         , (issock ? Zeroterm(tempstr() << sock) : NULL)
                                         , 0);
    if (!connect_ok && ER_BAD_DB_ERROR == mysql_errno(lib_mysql::_db.mysql) && mysql2ssim::_db.cmdline.baddbok) {
        // quietly finish with success, nothing to do
        _exit(0);
    }
    vrfy(connect_ok,mysql_error(lib_mysql::_db.mysql));
}

// -----------------------------------------------------------------------------

void mysql2ssim::Main() {
    lib_mysql::_db.mysql = mysql_init(NULL);

    if (mysql2ssim::_db.cmdline.writessimfile) {
        TempfileInitX(mysql2ssim::_db.tempfile,"mysql2ssim");
    }


    URL url;
    vrfy(URL_ReadStrptrMaybe(url, mysql2ssim::_db.cmdline.url), algo_lib::_db.errtext);

    Logo();
    strptr db = Pathcomp(url.dir, "/RR");
    Connect(url,db);

    // load list of tables
    LoadList(db);

    ind_beg(mysql2ssim::_db_table_names_curs, table_name, mysql2ssim::_db) {
        if (mysql2ssim::_db.cmdline.schema) {
            ReadSchema(db,table_name);
        } else {
            ReadData(db,table_name);
        }
    }ind_end;

    // use acr to append this data to tables
    if (mysql2ssim::_db.cmdline.writessimfile) {
        command::acr acr;
        acr.trunc=true;
        acr.replace=true;
        acr.write=true;
        acr.report=false;
        acr.print=false;
        acr.in = mysql2ssim::_db.cmdline.in;
        int rc=SysCmd(acr_ToCmdline(acr)<<"< "<<mysql2ssim::_db.tempfile.filename);
        algo_lib::_db.exit_code += algo::Abs(rc);
    }
}
