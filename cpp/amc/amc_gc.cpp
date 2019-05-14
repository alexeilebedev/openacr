// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: amc_gc (exe) -- Empirically eliminate unused records
// Exceptions: NO
// Source: cpp/amc/amc_gc.cpp
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/gen/amc_gc_gen.h"
#include "include/gen/amc_gc_gen.inl.h"

// -----------------------------------------------------------------------------

// Run amc and count # generated c++ lines
static int n_cppline_Get(strptr text, int dflt) {
    int n_cppline = dflt;
    ind_beg(Line_curs,line,text) {
        report::amc amc;
        try {
            if (report::amc_ReadStrptrMaybe(amc,line)) {
                n_cppline = amc.n_cppline;
            }
        } catch (algo_lib::ErrorX &x) {
        }
    }ind_end;
    return n_cppline;
}

// -----------------------------------------------------------------------------

// retrieve list of records to garbage-collect
static tempstr QueryRecords() {
    tempstr cmd;
    if (amc_gc::_db.cmdline.include) {
        // get records in reverse order so that line numbers don't shift.
        cmd << "grep -Hn '^#include' $(acr dev.targsrc:"
            <<amc_gc::_db.cmdline.target<<"/% -field:src | egrep -v '(/gen/|extern/)') | tac";
    } else {
        command::acr acr;
        acr.query = amc_gc::_db.cmdline.key.expr;
        acr.report = false;
        cmd << acr_ToCmdline(acr);
    }
    return SysEval(cmd, FailokQ(false),1024*1024);
}

// -----------------------------------------------------------------------------

// retrieve record key
static tempstr GetPkey(strptr line) {
    tempstr ret;
    if (amc_gc::_db.cmdline.include) {
        ret << Pathcomp(line,":RL");// file + lineno
    } else {
        Tuple tuple;
        (void)Tuple_ReadStrptrMaybe(tuple,line);
        Attr* attr = attrs_Find(tuple,0);// primary key
        if (attr) {
            PrintAttr(ret,tuple.head.value,attr->value);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

static int DeleteRec(strptr line, strptr pkey) {
    int rc=0;
    if (amc_gc::_db.cmdline.include) {
        // line has the form
        // e.g. cpp/abt/abt_noterel.cpp:9:#include "include/algo.h"
        SysCmd(tempstr()
               <<"sed -i "<<Pathcomp(line,":LR:LL")<<"d"// line number
               <<" "<<Pathcomp(line,":LL")// filename
               , FailokQ(true), DryrunQ(false));
    } else {
        command::acr_proc acr;
        acr.cmd.del     = true;
        acr.cmd.write   = true;
        acr.cmd.query   = pkey;
        acr.cmd.report  = false;
        acr.cmd.print   = false;
        rc=acr_Exec(acr);
    }
    sleep(1);// must pause to avoid timestamp conflation
    return rc;
}

// -----------------------------------------------------------------------------

static int SandboxDeleteRec(strptr line, strptr pkey) {
    errno_vrfy(chdir(".testgen")==0,"chdir");// CHANGING DIRECTORY HERE
    command::amc_proc amc;
    command::abt_proc abt;

    amc.stdout     = tempstr() << ">>../" << amc_gc::_db.buildlog;
    amc.stderr     = tempstr() << ">>../" << amc_gc::_db.buildlog;

    abt.stdout     = tempstr() << ">>../" << amc_gc::_db.buildlog;
    abt.stderr     = tempstr() << ">>../" << amc_gc::_db.buildlog;

    int rc = DeleteRec(line, pkey);
    if (rc==0) {
        abt.cmd.target.expr = amc_gc::_db.cmdline.target.expr;
        abt.cmd.maxerr = 1;// fail quickly, don't try to maximize # of files built
        if (!amc_gc::_db.cmdline.include) {
            rc = amc_Exec(amc);
        }
    }
    if (rc == 0) {
        rc = abt_Exec(abt);
    }
    errno_vrfy(chdir(Zeroterm(amc_gc::_db.basedir))==0,"chdir");// CHANGING DIRECTORY BACK
    return rc;
}

// -----------------------------------------------------------------------------

static void PrintReport() {
    amc_gc::_db.n_newcppline = n_cppline_Get(SysEval("amc",FailokQ(false),1024),0);
    prlog("report.amc_gc"
          <<Keyval("key",amc_gc::_db.cmdline.key.expr)
          <<Keyval("n_match", amc_gc::_db.n_total)
          <<Keyval("n_del", amc_gc::_db.n_del)
          <<Keyval("n_cppline", amc_gc::_db.n_newcppline)
          <<Keyval("n_cppline_del", amc_gc::_db.n_cppline - amc_gc::_db.n_newcppline));
}

// -----------------------------------------------------------------------------

static void Analyze(strptr line) {
    tempstr pkey=GetPkey(line);
    if (pkey != "") {
        bool eliminate=false;
        tempstr what(amc_gc::_db.cmdline.include ? "cpp include" : "data");
        SysCmd(tempstr()<<"rsync -ac "<<what<<" .testgen"
               ,FailokQ(false),DryrunQ(false));// copy new data over
        StringToFile("", amc_gc::_db.buildlog);

        int rc=SandboxDeleteRec(line,pkey);
        if (rc==0) {
            (void)DeleteRec(line,pkey);
            tempstr logcontents(FileToString(amc_gc::_db.buildlog,algo_FileFlags_none));
            amc_gc::_db.n_newcppline = n_cppline_Get(logcontents,amc_gc::_db.n_newcppline);
            amc_gc::_db.n_del++;
            eliminate=true;
        }
        prlog("amc_gc.analyze"
              <<Keyval("query",pkey)
              <<Keyval("eliminate", Bool(eliminate))
              <<Keyval("rec_no", amc_gc::_db.n_cur)
              <<Keyval("tot_rec", amc_gc::_db.n_total)
              <<Keyval("n_del", amc_gc::_db.n_del)
              <<Keyval("n_cppline", amc_gc::_db.n_newcppline)
              <<Keyval("n_cppline_del", (amc_gc::_db.n_cppline-amc_gc::_db.n_newcppline))
              );
    }
}

// -----------------------------------------------------------------------------

static void Begin(strptr recs) {
    ind_beg(Line_curs,line,recs) {
        (void)line;
        amc_gc::_db.n_total++;
    }ind_end;
    amc_gc::_db.n_cppline = n_cppline_Get(SysEval("amc",FailokQ(false),1024),0);
    amc_gc::_db.n_newcppline = amc_gc::_db.n_cppline;
    verblog(recs);
    prlog("amc_gc.begin"
          <<Keyval("tot_rec", amc_gc::_db.n_total)
          <<Keyval("n_cppline", amc_gc::_db.n_cppline)
          <<Keyval("watch_cmd",tempstr()<<"watch head -50 "<<amc_gc::_db.buildlog));
}

// -----------------------------------------------------------------------------

void amc_gc::Main() {
    amc_gc::_db.basedir = GetCurDir();
    tempstr recs = QueryRecords();
    Begin(recs);
    SysCmd("mkdir -p .testgen .testgen/temp"); // setup!
    SysCmd("rsync --delete -ac bin cpp include extern data .testgen"
           ,FailokQ(false),DryrunQ(false));// copy everything over

    ind_beg(Line_curs,line,recs) {
        Analyze(line);
        amc_gc::_db.n_cur++;
    }ind_end;

    PrintReport();
}
