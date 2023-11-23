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
// Target: ssim2mysql (exe) -- Ssim -> mysql
// Exceptions: yes
// Source: cpp/ssim2mysql.cpp
//
// Convert SSIM data to SQL.
// Print SQL to the screen to submit to mysql

#include "include/lib_mysql.h"
#include "include/gen/ssim2mysql_gen.h"
#include "include/gen/ssim2mysql_gen.inl.h"


static void NewCmd(strptr text) {
    ssim2mysql::FCmd &cmd = ssim2mysql::cmd_Alloc();
    cmd.cmd << text;
    vrfy(cmd_XrefMaybe(cmd), algo_lib::_db.errtext);
    verblog("Cmd: "<<text);
}

static void ProcessLine_Ssimfile(ssim2mysql::FInput *input, ssim2mysql::FSsimfile *ssimfile) {
    // check if tuple contains attributes which are not in the database.
    // warn.
    ind_beg(algo::Tuple_attrs_curs, attr, input->tuple) {
        tempstr colname;
        colname << ssimfile->ssimfile << "." << attr.name;
        if (!ssim2mysql::ind_column_Find(colname)) {
            tempstr tmps;
            tmps << "WARNING: Column [" << colname << "] not found and won't be added to MySQL table";
            if (tmps != input->warnstr) {
                prerr(tmps);
                input->warnstr = tmps;
            }
        }
    }ind_end;
    algo::ListSep field_sep(",");
    if (ssim2mysql::_db.n_cmd_rows > 0) {
        ssim2mysql::_db.c_cmd_cur->cmd << ",\n";
    }
    ssim2mysql::_db.n_cmd_rows++;
    ssim2mysql::_db.c_cmd_cur->cmd << "(" ;
    ind_beg(ssim2mysql::ssimfile_c_column_curs, column,*ssimfile) {
        strptr val;
        if (column.p_field->c_substr) {
            val = attr_GetString(input->tuple, StripNs("",column.p_field->c_substr->srcfield), column.defval);
            val = Pathcomp(val, column.p_field->c_substr->expr.value);
        } else {
            val = attr_GetString(input->tuple, name_Get(column), column.defval);
        }
        ssim2mysql::_db.c_cmd_cur->cmd << field_sep;
        if (elems_N(val) && column.is_int) {
            ssim2mysql::_db.c_cmd_cur->cmd << val;
        } else if (ssim2mysql::_db.cmdline.dry_run) {
            strptr_PrintCppQuoted(val, ssim2mysql::_db.c_cmd_cur->cmd, '\''); // some escaping
        } else {
            lib_mysql::MPrintQuoted(ssim2mysql::_db.c_cmd_cur->cmd,lib_mysql::_db.mysql,val,'\'');
        }
    }ind_end;
    ssim2mysql::_db.c_cmd_cur->cmd << ")";
}

//
// Add input tuple to current query (FCmd)
// Empty line flushes the query (submits for execution)
//
static void ProcessLine(ssim2mysql::FInput *input, strptr line) {
    vrfy(Tuple_ReadStrptrMaybe(input->tuple,line), algo_lib::_db.errtext);
    strptr table = input->tuple.head.value;
    ssim2mysql::FSsimfile *ssimfile = ssim2mysql::ind_ssimfile_Find(table);

    // table changed? end old insert statement, switch current ssimfile
    bool change = ssim2mysql::_db.c_ssimfile_cur != ssimfile;
    change |= (ssim2mysql::_db.c_cmd_cur && ch_N(ssim2mysql::_db.c_cmd_cur->cmd) >= ssim2mysql::_db.cmdline.maxpacket *3/4);

    if (ssim2mysql::_db.c_cmd_cur && change) {
        ssim2mysql::_db.c_cmd_cur->cmd  << ";\n";
        vrfy(cmd_XrefMaybe(*ssim2mysql::_db.c_cmd_cur), algo_lib::_db.errtext);// execute!
        ssim2mysql::_db.c_cmd_cur = NULL;
        ssim2mysql::_db.n_cmd_rows = 0;
    }

    ssim2mysql::_db.c_ssimfile_cur = ssimfile;

    // start new insert command
    if (ssimfile && !ssim2mysql::_db.c_cmd_cur) {
        tempstr sql_table;
        sql_table << "`" << GetNs(ssimfile->ssimfile) << "`.`" << StripNs("",table) << "`";

        // if this is the first row for a table, and truncation was specified on command line,
        // truncate.
        if (ssim2mysql::_db.cmdline.trunc && !ssimfile->truncated) {
            ssimfile->truncated = true;
            NewCmd(tempstr() << "TRUNCATE TABLE "<<sql_table<<";\n");
        }

        ssim2mysql::FCmd &cmd = ssim2mysql::cmd_Alloc();
        ssim2mysql::_db.c_cmd_cur = &cmd;
        cmd.cmd << (ssim2mysql::_db.cmdline.replace ? "REPLACE":"INSERT") << " INTO "<<sql_table;        // begin insert statement

        algo::ListSep field_sep(",");// build column list
        cmd.cmd <<" (";
        ind_beg(ssim2mysql::ssimfile_c_column_curs, column,*ssimfile) {
            cmd.cmd << field_sep << "`" << name_Get(column) << "`";
        }ind_end;
        cmd.cmd <<")";
        cmd.cmd <<" VALUES\n" ;
    }

    if (ssimfile) {
        ProcessLine_Ssimfile(input, ssimfile);
    }
}

static void EofActions() {
    if (ssim2mysql::_db.c_cmd_cur) {
        ssim2mysql::_db.c_cmd_cur->cmd  << ";\n";
        vrfy(cmd_XrefMaybe(*ssim2mysql::_db.c_cmd_cur), algo_lib::_db.errtext);// execute!
        ssim2mysql::_db.c_cmd_cur = NULL;
    }

    if (ssim2mysql::_db.cmdline.createdb && ssim2mysql::_db.cmdline.fkey) {     // Re-create database
        ssim2mysql::FNs &fns = ssim2mysql::ind_ns_FindX(ssim2mysql::_db.cmdline.db);

        // create foreign key constraints
        ind_beg(ssim2mysql::ns_c_ssimfile_curs, ssimfile, fns) if (ssimfile.select) {
            ind_beg(ssim2mysql::ctype_c_field_curs, field, *ssimfile.p_ctype) if (field.select && field.is_pkeyref) { // field, e.g. acmdb.Devchassis.chassis
                ssim2mysql::FSsimfile *tgt_ssimfile = field.p_arg->c_ssimfile;// target ssimfile, e.g. acmdb.chassis
                ssim2mysql::FField *tgt_field = c_field_Find(*field.p_arg, 0); // first field of target ctype, e.g. acmdb.Chassis.chassis
                if (tgt_ssimfile && zd_ssimfile_InLlistQ(*tgt_ssimfile)) {
                    NewCmd(tempstr()
                           << "ALTER TABLE `"<< name_Get(ssimfile)<<"`"                                              // e.g. devchassis
                           <<" ADD CONSTRAINT fk_" << name_Get(ssimfile) << "_" << name_Get(field)             // e.g. devchassis_chassis
                           << " FOREIGN KEY (`" << name_Get(field) << "`)"                                       // e.g. chassis
                           << " REFERENCES `" << name_Get(*tgt_ssimfile) << "` (`"<< name_Get(*tgt_field) <<"`);"); // e.g. chassis_chassis
                }
            } ind_end;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

//
// read lines out of input buffer.
// do not process partial lines (they could be corrupted)
//
void ssim2mysql::cd_input_line_Step() {
    ssim2mysql::FInput &input = *ssim2mysql::cd_input_line_RotateFirst();
    algo::aryptr<char> line    = in_buf_GetMsg(input);
    if (line.elems != NULL) {
        ProcessLine(&input, line);
        in_buf_SkipMsg(input);
    } else if (input.in_buf_eof) {
        EofActions();// end-of-file -- apply constraints  etc.
        if (input.in_buf_err.value != 0) {
            prerr("ssim2mysql.input"
                  <<Keyval("error",input.in_buf_err));
            algo_lib::_db.exit_code = 1;
        }
        input_Delete(input);
    }
}

// -----------------------------------------------------------------------------

static ssim2mysql::FField *RootField(ssim2mysql::FField *root_fld) {
    do {
        ssim2mysql::FCtype *ft = root_fld->p_arg;
        if (ft->c_sqltype || c_field_N(*ft) == 0) {
            break;
        }
        root_fld = c_field_Find(*ft, 0);
    } while (true);
    return root_fld;
}

// -----------------------------------------------------------------------------

static tempstr Dflt(ssim2mysql::FField *root_fld) {
    ssim2mysql::FSqltype *sqltype = root_fld->p_arg->c_sqltype; // builtin?
    bool istime = FindStr(sqltype->expr, strptr("time")) != -1;
    bool isstring = FindStr(sqltype->expr, strptr("char")) != -1 || sqltype->expr == "text";
    tempstr dflt;
    if (root_fld->arg == "Bool" || root_fld->arg == "bool") {
        dflt << (root_fld->dflt.value == "true" ? "'Y'" : "'N'");
    } else if (isstring || (istime && ("" != root_fld->dflt.value))) {
        // the default value is a c++ expression.
        // since c++ escaping is close enough to sql escaping, we will just print the string as-is,
        dflt << root_fld->dflt;
        if (!ch_N(dflt)) {
            dflt << "''";
        }
    } else if ("" != root_fld->dflt.value) {   // Numerical defaults can not be empty strings
        dflt << root_fld->dflt;
    } else {
        dflt << "0";// numeric
    }
    return dflt;
}

// -----------------------------------------------------------------------------

static bool GoodReftype(dmmeta::ReftypePkey reftype) {
    return reftype == dmmeta_Reftype_reftype_Pkey
        || reftype == dmmeta_Reftype_reftype_Val
        || reftype == dmmeta_Reftype_reftype_Regx
        || reftype == dmmeta_Reftype_reftype_RegxSql;// also a string
}

// -----------------------------------------------------------------------------

static void FixupFields(ssim2mysql::FCtype &ctype) {
    int n_field = 0;
    ind_beg(ssim2mysql::ctype_c_field_curs, fld, ctype) {
        bool good = !fld.c_substr || ssim2mysql::_db.cmdline.fldfunc;
        n_field += good;
    }ind_end;

    // a table with a single column will get sorted by MySQL.
    // must add another column to prevent this
    // a ctype with no fields gets a single field.
    if (n_field < 2) {
        vrfy(ssim2mysql::field_InsertMaybe(dmmeta::Field(tempstr()<<ctype.ctype<<".extra_column_for_roundtrip"
                                                         ,"algo.Comment"
                                                         , "Val"
                                                         ,algo::CppExpr()
                                                         , algo::Comment())), algo_lib::_db.errtext);
    }
}

// -----------------------------------------------------------------------------

//
// Generate SQL command to create a table for given ssimfile
//
static void Main_GenSchema_Table(ssim2mysql::FSsimfile &ssimfile) {
    zd_ssimfile_Insert(ssimfile); // table to upload
    strptr engine = ssim2mysql::_db.cmdline.fkey ? "InnoDB" : "MyISAM";
    bool surrogate_pkey = engine == "InnoDB";
    ssim2mysql::FCtype &ctype = *ssimfile.p_ctype;
    NewCmd(tempstr() << "DROP TABLE IF EXISTS `" << name_Get(ssimfile) << "`;");
    ssim2mysql::FCmd &createcmd = ssim2mysql::cmd_Alloc();
    createcmd.cmd << "CREATE TABLE `" << name_Get(ssimfile) << "` (\n";

    FixupFields(ctype);
    algo::ListSep ls("\n, ");

    if (surrogate_pkey) {
        createcmd.cmd << ls;
        createcmd.cmd << " `"<<name_Get(ssimfile) << "_rowid` INT PRIMARY KEY AUTO_INCREMENT";
    }

    // walk over all columns
    ind_beg(ssim2mysql::ctype_c_field_curs, fld, ctype) {
        // determine if the column should appear.
        // fldfuncs are excluded unless explicitly requested
        // (they cannot show up in the web-oriented database, and they
        // do not make it back via mysql2ssim, but they simplify writing of joins)
        bool good = !fld.c_substr || ssim2mysql::_db.cmdline.fldfunc;
        fld.select = good;
        dmmeta::ReftypePkey reftype(dmmeta_Reftype_reftype_Val);
        fld.is_pkeyref = reftype == dmmeta_Reftype_reftype_Pkey;
        if (good && GoodReftype(reftype)) {
            createcmd.cmd << ls;
            createcmd.cmd << "`" << name_Get(fld) << "` ";       // Column name
            ssim2mysql::FField *root_fld = RootField(&fld);// follow pkey chain to find out true type.
            ssim2mysql::FSqltype *sqltype = root_fld->p_arg->c_sqltype; // builtin?
            vrfy(sqltype, tempstr()<<"no sqltype defined for ctype "<<root_fld->arg);
            createcmd.cmd << sqltype->expr;
            tempstr dflt=Dflt(root_fld);
            // Default value
            createcmd.cmd << " NOT NULL ";
            if (ch_N(dflt)) createcmd.cmd << " DEFAULT "<<dflt;
            if (ind_curs(fld).index == 0) {
                createcmd.cmd << ", ";
                createcmd.cmd << (surrogate_pkey ? " UNIQUE" : " PRIMARY") << " KEY ";
                createcmd.cmd << "`" << name_Get(fld) << "`" << "(`" << name_Get(fld) << "`";
                // unique prefix key index required for text fields
                if (sqltype->expr == "text") {
                    createcmd.cmd << "(767)";
                }
                createcmd.cmd << ")";
            }
        }
    }ind_end;
    createcmd.cmd << "\n) ENGINE="<< engine
                  <<" DEFAULT CHARSET=latin1 COLLATE latin1_general_cs;\n\n";
    vrfy(ssim2mysql::cmd_XrefMaybe(createcmd), algo_lib::_db.errtext);
}

// -----------------------------------------------------------------------------

void ssim2mysql::zs_cmd_Step() {
    // execute some commands
    for(int iter = 0; ssim2mysql::zs_cmd_First() && iter < 5; iter++) {
        ssim2mysql::FCmd &exec_cmd = *ssim2mysql::zs_cmd_RemoveFirst();
        if (ssim2mysql::_db.cmdline.dry_run) {
            prlog(exec_cmd.cmd);
        }
        verblog(exec_cmd.cmd);
        if (!ssim2mysql::_db.cmdline.dry_run) {
            vrfy(mysql_query(lib_mysql::_db.mysql, Zeroterm(exec_cmd.cmd))==0
                 ,mysql_error(lib_mysql::_db.mysql));
        }
        ssim2mysql::cmd_Delete(exec_cmd);
    }
}

// -----------------------------------------------------------------------------

void ssim2mysql::Main() {
    URL url;
    vrfy(URL_ReadStrptrMaybe(url, ssim2mysql::_db.cmdline.url), algo_lib::_db.errtext);
    if (!ch_N(ssim2mysql::_db.cmdline.url)) {
        ssim2mysql::_db.cmdline.dry_run = true;
    }

    // inputect to database
    // NOTE empty server name defaults to localhost, so failure to supply
    // server causes inputectino to wrong server
    // During inputection, omit database name since it may not exist
    bool   issock = url.protocol == "sock";
    if (!ssim2mysql::_db.cmdline.dry_run) {
        lib_mysql::_db.mysql = mysql_init(NULL);
        vrfy(issock || ch_N(url.server)>0,tempstr()<< "server name must be specified");
        vrfy(mysql_real_connect(lib_mysql::_db.mysql
                                , (issock ? NULL : Zeroterm(tempstr() << url.server))
                                , (issock ? NULL : Zeroterm(tempstr() << url.username))
                                , (issock ? NULL : Zeroterm(tempstr() << url.password))
                                , NULL // omit database name
                                , 0
                                , (issock ? Zeroterm(tempstr() << url.dir) : NULL)
                                , 0)
             , mysql_error(lib_mysql::_db.mysql));
    }
    ind_beg(ssim2mysql::_db_ssimfile_curs, ssimfile, ssim2mysql::_db) {
        // work only with ssimfiles that are present
        // it is a legit case when ssimfile is absent
        tempstr fname(SsimFname(ssim2mysql::_db.cmdline.data_dir, ssimfile.ssimfile));
        ssimfile.select=FileQ(fname);
    }ind_end;

    // Re-create database
    if (ssim2mysql::_db.cmdline.createdb) {
        ssim2mysql::FNs &fns = ssim2mysql::ind_ns_FindX(ssim2mysql::_db.cmdline.db);
        NewCmd(tempstr() << "DROP DATABASE IF EXISTS `"<<ssim2mysql::_db.cmdline.db<<"`;");
        NewCmd(tempstr() << "CREATE DATABASE `"<<ssim2mysql::_db.cmdline.db<<"` DEFAULT CHARACTER SET latin1 COLLATE latin1_general_cs;");
        NewCmd(tempstr() << "USE `"<<ssim2mysql::_db.cmdline.db<<"`;");

        ind_beg(ssim2mysql::ns_c_ssimfile_curs, ssimfile,fns) if (ssimfile.select){
            Main_GenSchema_Table(ssimfile);
        } ind_end;
    }

    // Create columns and fill in their properties
    ind_beg(ssim2mysql::_db_ssimfile_curs, ssimfile, ssim2mysql::_db) if (ssimfile.select){
        ssim2mysql::FCtype &ctype = *ssimfile.p_ctype;
        ind_beg(ssim2mysql::ctype_c_field_curs, field, ctype) if (!field.c_substr || ssim2mysql::_db.cmdline.fldfunc) {
            tempstr colkey;
            colkey << ssimfile.ssimfile << "." << name_Get(field);
            ssim2mysql::FColumn   &col     = ssim2mysql::ind_column_GetOrCreate(colkey);
            col.p_field     = &field;
            col.defval      = strptr(field.dflt.value);
            col.is_int      = ctype.c_sqltype ? range_N(substr_FindFirst(ctype.c_sqltype->expr, "int"))>0 : false;
        }ind_end;
    }ind_end;

    // set up reading -- make sure to set nonblocking mode
    ssim2mysql::FInput &input = ssim2mysql::input_Alloc();
    algo::Fildes fildes((ssim2mysql::_db.cmdline.in=="-") ? Fildes(dup(0)) : OpenRead(ssim2mysql::_db.cmdline.in));
    (void)SetBlockingMode(fildes, false);
    in_buf_BeginRead(input, fildes);

    ssim2mysql::MainLoop();
}
