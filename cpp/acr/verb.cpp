// (C) AlgoEngineering LLC 2008-2013
// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/verb.cpp -- Command-line verbs
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

void acr::Main_Cmd() {
    acr::_db.cmdline.print = false;
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        tempstr cmd;
        acr::FCtype &ctype = *rec.p_ctype;

        // show whole line
        cmd << "acr_tuple=";
        tempstr temp;
        Tuple_Print(rec.tuple, temp);
        strptr_PrintBash(temp, cmd);
        cmd<< eol;

        // show tuple head
        cmd << "acr_head=";
        strptr_PrintBash(Typetag(*rec.p_ctype), cmd);
        cmd<<eol;

        // show rowid
        cmd << "acr_rowid=" << rec.sortkey.rowid;
        cmd<<eol;

        // print the command
        ind_beg(acr::ctype_c_field_curs, field,ctype) {
            cmd << name_Get(field) << "=";
            strptr_PrintBash(EvalAttr(rec.tuple, field), cmd);
            cmd<< eol;
        }ind_end;

        cmd << acr::_db.cmdline.cmd<<eol;
        prlog(cmd);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print fields in a column
void acr::Main_Field() {
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        strptr echo=acr::_db.cmdline.field;
        tempstr out;
        algo::ListSep ls("\t");
        while (elems_N(echo)>0) {
            strptr tok=Pathcomp(echo,",LL");// list of fields
            echo=Pathcomp(echo,",LR");
            acr::FField *field=acr::ind_field_Find(tempstr()<<rec.p_ctype->ctype<<"."<<tok);
            if (field) {
                out<<ls;
                out << EvalAttr(rec.tuple, *field);
            }
        }
        if (ch_N(out)) {
            prlog(out);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print fields in a column
void acr::Main_Regxof() {
    cstring out;
    int n=0;
    algo::ListSep ls("|");
    acr::FCtype *ctype=NULL;
    acr::FField *field=NULL;
    acr::tempkey_RemoveAll();
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        LookupField(rec,acr::_db.cmdline.regxof,ctype,field);
        if (field) {
            acr::ind_tempkey_GetOrCreate(EvalAttr(rec.tuple, *field));
        }
    }ind_end;
    ind_beg(acr::_db_tempkey_curs, tempkey,acr::_db) if (ch_N(tempkey.tempkey)) {
        n++;
        out<<ls;
        strptr_PrintRegxSql(tempkey.tempkey,out);
    }ind_end;
    if (n>0) {
        out = tempstr()<<"("<<out<<")";
    }
    prlog(out);
}

// -----------------------------------------------------------------------------

void acr::Main_Mysql() {
    command::acr_my_proc acr_my;
    vrfy(DirectoryQ(acr::_db.cmdline.in), tempstr()
         <<"acr.mydir"<<Keyval("comment","-in must be a directory"));
    acr_my.cmd.nsdb.expr = acr::_db.cmdline.query;
    acr_my.cmd.e = true;
    acr_my.cmd.fldfunc = acr::_db.cmdline.fldfunc;
    acr_my.cmd.in = acr::_db.cmdline.in;
    int rc = acr_my_Exec(acr_my);
    if (rc!=0) {
        algo_lib::_db.exit_code=rc;
    }
    acr::_db.cmdline.print  = false;
    acr::_db.cmdline.report = false;
}

// -----------------------------------------------------------------------------

void acr::Main_Browser() {
    strptr browser = getenv("BROWSER");
    if (!elems_N(browser)) {
        prerr("WARNING: BROWSER environment variable not set");
    }
    if (elems_N(browser)) {
        tempstr cmd;
        cmd << browser;
        cmd << " http://localhost:6769/?q=";
        strptr_PrintUri(acr::_db.cmdline.query, cmd, true);
        cmd << " & ";// background it
        int rc = system(Zeroterm(cmd));
        (void)rc;
    }
}

// -----------------------------------------------------------------------------

static void SelectCtype(acr::FCtype &ctype_ctype, acr::FCtype &ctype) {
    acr::FRec *ctype_rec=ind_rec_Find(ctype_ctype,ctype.ctype);
    if (ctype_rec && !zd_all_selrec_InLlistQ(*ctype_rec)) {
        acr::FQuery& next  = acr::query_Alloc();
        next.queryop = acr_Queryop_value_select;
        next.ctype = "dmmeta.Ctype";
        next.pk    = true;
        next.value = ctype.ctype;
        next.ndown = 100;
        (void)acr::query_XrefMaybe(next);
        acr::RunAllQueries();
    }
}

// Select ctypes of selected records, deselect records themselves
void acr::Main_SelectMeta() {
    // Find data record of 'ctype'
    acr::FCtype *ctype_ctype = acr::ind_ctype_Find("dmmeta.Ctype");
    vrfy(ctype_ctype, "acr.broken_metadata");
    if (ctype_ctype->c_ssimfile && !ctype_ctype->c_ssimfile->c_file) {
        acr::LoadSsimfile(*ctype_ctype->c_ssimfile);
    }
    ind_beg(acr::_db_zd_all_selrec_curs,rec,acr::_db) {
        rec.metasel=true;
    }ind_end;
    acr::Rec_DeselectAll();
    ind_beg(acr::_db_file_curs, file, acr::_db) {
        ind_beg(acr::file_zd_frec_curs,rec,file) {
            if (rec.metasel) {
                SelectCtype(*ctype_ctype,*rec.p_ctype);
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void CheckTimestamps(strptr fname) {
    // check timestamps
    bool timestamps_ok = true;
    ind_beg(acr::_db_file_curs, file,acr::_db) {
        algo::UnTime t = ModTime(file.file);
        if (!UnTime_Eq(t, file.modtime)) {
            prerr("acr_util.file_changed"
                  <<Keyval("file",file.file)
                  <<Keyval("comment","The file has been changed since reading it"));
            timestamps_ok = false;
            break;
        }
    }ind_end;
    vrfy(timestamps_ok,tempstr()<<"No write performed, your modifications preserved in "<<fname);
}

// -----------------------------------------------------------------------------

void acr::Main_AcrEdit() {
    tempstr fname("temp/acr.ssim");
    vrfy(!FileQ(fname),tempstr()<<"acr_util.tempfile"
         <<Keyval("filename",fname)
         <<Keyval("comment","other acr instance is running, or file may contain backup data"));
    algo::Fildes fd = OpenWrite(fname, algo_FileFlags__throw|algo_FileFlags_append);
    if (flock(fd.value, LOCK_EX | LOCK_NB)!=0) {
        int err=errno;
        prerr("Cannot lock ["<<fname<<"]");
        prerr("Is another instance of 'acr -e' running?");
        vrfy(0, tempstr()<<algo::FromErrno(err));
    }
    acr::FPrint print;
    print.tree     = acr::_db.cmdline.tree;
    print.fldfunc  = acr::_db.cmdline.fldfunc;
    print.pretty   = acr::_db.cmdline.pretty;
    print.maxgroup = acr::_db.cmdline.maxgroup;
    print.cmt      = acr::_db.cmdline.cmt;
    print.loose    = acr::_db.cmdline.loose;
    PrintToFd(print, fd);
    strptr editor = getenv("EDITOR");
    vrfy(elems_N(editor), "EDITOR environment variable not set");
    tempstr cmd;
    cmd << editor << " " << fname;
    //
    // this command can take a verrrrrry long time -- hours.
    // during this time, it is possible for user to open files
    // and modify them.
    //
    int rc = system(Zeroterm(cmd));
    vrfy(rc==0,tempstr()<< cmd);
    CheckTimestamps(fname);

    acr::_db.cmdline.replace = true;
    acr::_db.cmdline.write = true;
    // mark all selected records for deletion
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        rec.del = true;
    }ind_end;
    // with -in:<filename> -e, any newly inserted records
    // appear to originate from the input file.
    acr::FFile &file = acr::ind_file_GetOrCreate(FileInputQ() ? strptr(acr::_db.cmdline.in) : strptr(fname));
    file.deselect=true;
    algo_lib::InTextFile in;
    in.file.fd =  OpenRead(fname);
    ReadLines(file,in);
    // give up the flock
    close(fd.value);
    // if no exception is thrown above, unlink file.
    // otherwise it stays around.
    unlink(Zeroterm(fname));
}
