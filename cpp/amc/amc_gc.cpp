// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: amc_gc (exe) -- Garbage collector for in-memory databases
// Exceptions: NO
// Source: cpp/amc/amc_gc.cpp
//

#include "include/algo.h"
#include "include/gen/amc_gc_gen.h"
#include "include/gen/amc_gc_gen.inl.h"
#include "include/gen/dev_gen.h"

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
        rc=SysCmd(tempstr()
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
    algo_lib::PushDir(algo_lib::SandboxDir(dev_Sandbox_sandbox_amc_gc));
    command::amc_proc amc;
    command::abt_proc abt;

    // redirect all to the build log
    amc.fstdout     << ">>"<<DirFileJoin(algo_lib::dirstack_qLast(), amc_gc::_db.buildlog);
    amc.fstderr     = amc.fstdout;
    abt.fstdout     = amc.fstdout;
    abt.fstderr     = amc.fstdout;

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
    algo_lib::PopDir();
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
        command::sandbox_proc sandbox;
        sandbox.cmd.name.expr = dev_Sandbox_sandbox_amc_gc;
        sandbox.cmd.clean = true;
        sandbox_ExecX(sandbox);
        StringToFile("", amc_gc::_db.buildlog);

        int rc=SandboxDeleteRec(line,pkey);
        if (rc==0) {
            (void)DeleteRec(line,pkey);// delete in parent directory
            tempstr logcontents(FileToString(amc_gc::_db.buildlog,algo::FileFlags()));
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
    tempstr recs = QueryRecords();
    Begin(recs);
    command::sandbox_proc sandbox;
    sandbox.cmd.name.expr = dev_Sandbox_sandbox_amc_gc;
    sandbox.cmd.reset = true;
    sandbox_ExecX(sandbox);

    ind_beg(Line_curs,line,recs) {
        Analyze(line);
        amc_gc::_db.n_cur++;
    }ind_end;

    PrintReport();
}
