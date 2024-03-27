// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/acr/verb.cpp -- Command-line verbs
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
        tempstr out;
        algo::ListSep ls("\t");
        ind_beg(command::acr_field_curs,field,_db.cmdline) {
            ind_beg(algo::Sep_curs,tok,field,',') {
                acr::FField *ffield=acr::ind_field_Find(tempstr()<<rec.p_ctype->ctype<<"."<<tok);
                if (ffield) {
                    out<<ls;
                    out << EvalAttr(rec.tuple, *ffield);
                }
            }ind_end;
        }ind_end;
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
    acr_my.cmd.fldfunc = true;
    acr_my.cmd.in = acr::_db.cmdline.in;
    int rc = acr_my_Exec(acr_my);
    if (rc!=0) {
        algo_lib::_db.exit_code=rc;
    }
    acr::_db.cmdline.print  = false;
    acr::_db.cmdline.report = false;
}

// -----------------------------------------------------------------------------

// Add ctype and its transitive closure to the list of selected records
// This produces a new query but doesn't run it
void acr::ScheduleSelectCtype(acr::FCtype &ctype_ctype, acr::FCtype &ctype) {
    if (bool_Update(ctype.mark_sel,true)) {
        acr::FRec *ctype_rec=ind_ctype_rec_Find(ctype_ctype,ctype.ctype);
        if (ctype_rec && !zd_all_selrec_InLlistQ(*ctype_rec)) {
            acr::FQuery& next  = acr::query_Alloc();
            next.queryop = acr_Queryop_value_select;
            algo_lib::Regx_ReadLiteral(next.ssimfile, "dmmeta.ctype");
            Regx_ReadLiteral(next.query.value, ctype.ctype);
            next.comment = "ctype";
            (void)acr::query_XrefMaybe(next);
        }
    }
}

// -----------------------------------------------------------------------------

// Select ctypes of selected records, deselect records themselves
void acr::Main_SelectMeta() {
    // Find data record of 'ctype'
    acr::FCtype *ctype_ctype = acr::ind_ctype_Find("dmmeta.Ctype");
    vrfy(ctype_ctype, "acr.broken_metadata");
    LoadRecords(*ctype_ctype);
    // mark selected records for metaselection and deselect them
    ind_beg(acr::_db_zd_all_selrec_curs,rec,acr::_db) {
        rec.metasel=true;
    }ind_end;
    acr::Rec_DeselectAll();
    // add ctypes of selected records to the selected list
    ind_beg(acr::_db_file_curs, file, acr::_db) {
        ind_beg(acr::file_zd_frec_curs,rec,file) {
            if (rec.metasel) {
                ScheduleSelectCtype(*ctype_ctype,*rec.p_ctype);
            }
        }ind_end;
    }ind_end;
    // add ctypes of empty ssimfiles to the selected list
    ind_beg(acr::_db_ssimfile_curs, ssimfile, acr::_db) {
        if (ssimfile.c_file && zd_frec_EmptyQ(*ssimfile.c_file)) {
            ScheduleSelectCtype(*ctype_ctype, *ssimfile.p_ctype);
        }
    }ind_end;
    // clear metasel flag on all records
    ind_beg(acr::_db_file_curs, file, acr::_db) {
        ind_beg(acr::file_zd_frec_curs,rec,file) {
            rec.metasel=false;
        }ind_end;
    }ind_end;
    RunAllQueries();
    while (Main_SelectDown(false)) {// recursive
    }
}

// -----------------------------------------------------------------------------

void acr::Main_SelectUp() {
    // run through selected records, add new ones to the front
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        if (rec.seldist >= 0 && rec.seldist < _db.cmdline.nup) {
            ind_beg(acr::ctype_c_field_curs,  field, *rec.p_ctype) if (field.p_arg->c_ssimfile) {
                LoadRecords(*field.p_arg);
                // look up item in the parent record.
                // if found -- add that record to the match set
                tempstr val(EvalAttr(rec.tuple, field));
                acr::FRec *parrec = acr::ind_ctype_rec_Find(*field.p_arg, val);
                if (parrec && Rec_Select(*parrec)) {
                    parrec->seldist = rec.seldist + 1;
                }
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Check if field FIELD of type CHILD is the pkey of CHILD
// or the leftmost prefix of pkey of CHILD
bool acr::LeftCheck(acr::FCtype &child, acr::FField &field) {
    bool ret=true;
    if (_db.cmdline.l) {
        acr::FField *pkey =c_field_Find(child,0);
        if (pkey) {
            ret = pkey==&field
                || (field.c_substr && field.c_substr->srcfield==pkey->field && algo::LeftPathcompQ(field.c_substr->expr.value));
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// extend selected front down
// the search starts with all selected records, where we clear the visit flag
// we then create a list C_CTYPE_FRONT of all potential ctypes that might reference these selected records,
// we then scan records for these ctypes, and add new records to the selected list
// The function returns the number of records added.
// The function performs one iteration of the downward transitive closure. Looping until
// SelectDown returns 0 finds all downward references.
int acr::Main_SelectDown(bool unused) {
    int nmatch=0;
    c_ctype_front_RemoveAll();
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        if (rec.seldist <= 0) {// go down only from "baseline" records
            ind_beg(acr::ctype_c_child_curs, child, *rec.p_ctype) {
                c_ctype_front_Insert(child);
            }ind_end;
        }
        if (acr::_db.cmdline.x) {
            ind_beg(acr::ctype_c_ssimreq_curs,ssimreq,*rec.p_ctype) {
                if (Regx_Match(ssimreq.regx_value,EvalAttr(rec.tuple, *ssimreq.p_parent_field))) {
                    algo_lib::Replscope R;
                    R.eatcomma=false;
                    acr::FCtype *child = ssimreq.p_child_ssimfile->p_ctype;
                    LoadRecords(*child);
                    if (acr::FRec *child_rec = ind_ctype_rec_Find(*child,GetChildKey(rec,ssimreq,rec.tuple,R))) {
                        if (Rec_Select(*child_rec)) {
                            child_rec->seldist = rec.seldist -1;
                            nmatch++;
                        }
                    }
                }
            }ind_end;
        }
    }ind_end;
    // walk child ssimfiles
    // walk all records of each ssimfile
    // add records which reference one of selected records
    ind_beg(acr::_db_c_ctype_front_curs, child, _db) {
        LoadRecords(child);
        ind_beg(acr::ctype_zd_ctype_rec_curs, rec, child) {
            ind_beg(acr::ctype_c_field_curs,  field, child) if (field.p_arg->c_ssimfile && LeftCheck(child,field)) {
                tempstr val(EvalAttr(rec.tuple, field));
                acr::FRec *parrec = acr::ind_ctype_rec_Find(*field.p_arg, val);
                bool good = parrec;
                good = good && parrec->seldist <= 0;
                good = good && acr::zd_all_selrec_InLlistQ(*parrec);
                if (unused) {
                    // "unused" option -- instead of selecting child,
                    // deselect parent. this effectively selects only those records
                    // which have no one pointing to them.
                    // cmdline.nup=0, cmdline.ndown=1, otherwise this algorithm doesn't
                    // work.
                    if (good) {
                        zd_all_selrec_Remove(*parrec);
                        zd_ctype_selrec_Remove(*parrec->p_ctype, *parrec);
                    }
                } else {
                    good = good && Rec_Select(rec);
                    if (good) {
                        rec.seldist = parrec->seldist - 1;
                        nmatch++;
                        break; // already selected, no need to continue search
                    }
                }
            }ind_end;
        }ind_end;
    }ind_end;
    return nmatch;
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
    print.showstatus=true;// annotate records as del,insert,update
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
    // de-select all records
    acr::Rec_DeselectAll();
    // with -in:<filename> -e, any newly inserted records
    // appear to originate from the input file.
    acr::FFile &file = acr::ind_file_GetOrCreate(FileInputQ() ? strptr(acr::_db.cmdline.in) : strptr(fname));
    algo_lib::FFildes in;
    in.fd =  OpenRead(fname);
    // insert records from file in replace mode
    ReadLines(file,in.fd,acr_ReadMode_acr_replace);
    // give up the flock
    close(fd.value);
    // if no exception is thrown above, unlink file.
    // otherwise it stays around.
    unlink(Zeroterm(fname));
}

// -----------------------------------------------------------------------------

// Mark SSIMREQ child records for deletion
void acr::DelChildRecords(acr::FRec &rec) {
    ind_beg(acr::ctype_c_ssimreq_curs,ssimreq,*rec.p_ctype) {
        if (Regx_Match(ssimreq.regx_value,EvalAttr(rec.tuple, *ssimreq.p_parent_field))) {
            algo_lib::Replscope R;
            R.eatcomma=false;
            acr::FCtype *child = ssimreq.p_child_ssimfile->p_ctype;
            LoadRecords(*child);
            if (acr::FRec *child_rec = ind_ctype_rec_Find(*child,GetChildKey(rec,ssimreq,rec.tuple,R))) {
                child_rec->del=true;
                Rec_Select(*child_rec);
            }
        }
    }ind_end;
}

// Start with selected records
// Find all dependent records and delete them as well
// In the end, de-select records that were both inserted and deleted
void acr::CascadeDelete() {
    for (acr::FRec *rec=zd_all_selrec_First(); rec; rec=zd_all_selrec_Next(*rec)) {
        c_ctype_front_RemoveAll();
        // scan all selected records since the last one
        // collect a list of child tables to scan
        for (; rec; rec=zd_all_selrec_Next(*rec)) {
            if (rec->del) {
                if (_db.cmdline.x) {
                    DelChildRecords(*rec);
                }
                ind_beg(acr::ctype_c_child_curs, child, *rec->p_ctype) {
                    c_ctype_front_Insert(child);
                }ind_end;
            }
            if (rec==zd_all_selrec_Last()) {
                break;
            }
        }
        // scan child tables
        // makr any references to a deleted record as deleted
        ind_beg(acr::_db_c_ctype_front_curs, child, _db) {
            LoadRecords(child);
            ind_beg(acr::ctype_zd_ctype_rec_curs, childrec, child) {
                ind_beg(acr::ctype_c_field_curs, field, child) if (field.p_arg->c_ssimfile) {
                    acr::FRec *parrec = acr::ind_ctype_rec_Find(*field.p_arg, EvalAttr(childrec.tuple, field));
                    if (parrec && parrec->del) {
                        Rec_Select(childrec);
                        childrec.del=true;
                    }
                }ind_end;
            }ind_end;
        }ind_end;
    }
    // de-select records that were both inserted and deleted
    // since there is no point in showing them
    ind_beg(acr::_db_zd_all_selrec_delcurs, delrec, acr::_db) {
        if (delrec.isnew && delrec.del) {
            acr::Rec_Deselect(delrec);
        }
    }ind_end;
}
