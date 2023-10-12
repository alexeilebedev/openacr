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
// Target: acr_my (exe) -- ACR <-> MariaDB adaptor
// Exceptions: yes
// Source: cpp/acr/acr_my.cpp
//

#include "include/algo.h"
#include "include/gen/acr_my_gen.h"
#include "include/gen/acr_my_gen.inl.h"
#include <sys/utsname.h> // for uname()

// -----------------------------------------------------------------------------
static tempstr MysqldSafeCmd(){
    tempstr cmd;
    cmd << Subst(acr_my::_db.R, "mysqld_safe"
                 " --no-defaults"
                 " --log-error=$data_logdir/mysqld.log"
                 " --pid-file=$data_logdir/mysql_pid"
                 " --socket=$data_dir/mysql.sock"
                 " --datadir=$data_dir"
                 " --skip-grant-tables"
                 );
    if (!acr_my::_db.cmdline.serv) {
        cmd << " --skip-networking";
    }
    return cmd;
}
// -----------------------------------------------------------------------------

static void Main_Start() {
    bool started=false;
    // check if dir exist, and initialize if not
    tempstr cmd;
    if (!DirectoryQ(acr_my::_db.data_dir)){
        // create mysql directory
        CreateDirRecurse(acr_my::_db.data_dir);
        // **VP** if logdir dir is the same as data_dir
        // --initialize fails because it puts a record into data_dir
        // and mysqld sees the dir as contaminated!
        CreateDirRecurse(acr_my::_db.data_logdir);
        // assume mysqld_safe is in the path
        cmd << MysqldSafeCmd()
            << " --initialize"
            << " 1>/dev/null";
        prlog("waiting for server init...");
        SysCmd(cmd, FailokQ(false));
    }
    // assume mysqld_safe is in the path
    cmd = MysqldSafeCmd();
    // In RH7, we use mariadb which requires innodb to be enabled.
    struct utsname my_uname;
    uname(&my_uname);
    cmd << " 1>/dev/null &";
    SysCmd(cmd, FailokQ(false));
    // wait for it to start -- wait for a few seconds, then print the error
    // file and exit
    int step = 0;
    tempstr waitcmd = Subst(acr_my::_db.R
                            , "mysqladmin"
                            " --no-defaults"
                            " --protocol=socket"
                            " --socket=$data_dir/mysql.sock"
                            " ping 1>/dev/null 2>&1");
    algo::SchedTime t0=algo::CurrSchedTime();
    do {
        started = SysCmd(Zeroterm(waitcmd))==0;
        if (step % 10 == 9) {// print message every few seconds
            prlog("waiting for server startup...");
        }
        algo::SleepMsec(100);
        step++;
    } while (!started && algo::ElapsedSecs(t0,algo::CurrSchedTime()) < 5.0);

    // oops -- start failed. proceed straight to abort.
    if (!started) {
        prerr("acr_my.server_start_timeout"
              <<Keyval("comment","here are the last few lines of mysqld.log. perhaps it will help:"));
        SysCmd(Subst(acr_my::_db.R,"tail -20 $data_logdir/mysqld.log"));
        vrfy(0, "start failed");
    }
}

// -----------------------------------------------------------------------------

static void Main_UploadNs(acr_my::FNsdb &nsdb) {
    command::ssim2mysql ssim2mysql;
    ssim2mysql.url      = Subst(acr_my::_db.R, "sock:///$data_dir/mysql.sock");
    ssim2mysql.db       = nsdb.ns;
    ssim2mysql.fkey     = acr_my::_db.cmdline.fkey;
    ssim2mysql.trunc    = true;
    ssim2mysql.createdb = true;
    ssim2mysql.fldfunc  = acr_my::_db.cmdline.fldfunc;

    tempstr cmd;
    cmd << "cat ";
    int nfile = 0;
    if (algo::FileQ(acr_my::_db.cmdline.in)) {
        cmd << acr_my::_db.cmdline.in;
        nfile++;
    } else {
        ind_beg(acr_my::_db_ssimfile_curs, ssimfile, acr_my::_db) if (ns_Get(ssimfile) == nsdb.ns) {
            tempstr fname(SsimFname(acr_my::_db.cmdline.in, ssimfile.ssimfile));
            // avoid 'file not found' error -- ssimfiles are not supposed
            // to always exist.
            if (FileQ(fname)) {
                cmd << " " << fname;
                nfile++;
            }
        }ind_end;
    }
    // cat will hang if it is not passed a list of filenames!
    if (nfile > 0) {
        cmd << " | " << ssim2mysql_ToCmdline(ssim2mysql);
        SysCmd(cmd, FailokQ(false),DryrunQ(false),EchoQ(algo_lib::_db.cmdline.verbose));
    }
}

static void Main_Upload() {
    ind_beg(acr_my::_db_nsdb_curs, nsdb, acr_my::_db) if (nsdb.select) {
        Main_UploadNs(nsdb);
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Main_DownloadNs(acr_my::FNsdb &nsdb) {
    command::mysql2ssim mysql2ssim;
    mysql2ssim.in = acr_my::_db.cmdline.in;
    mysql2ssim.url << Subst(acr_my::_db.R, "sock:///$data_dir/mysql.sock/") << nsdb.ns;
    mysql2ssim.baddbok = true; // missing database is OK, keep calm and carry on
    // write directly to ssimfile (but not if fldfunc)
    mysql2ssim.writessimfile = !acr_my::_db.cmdline.fldfunc;
    mysql2ssim.nologo = true; // write directly to ssimfile
    tempstr cmd;
    cmd << mysql2ssim_ToCmdline(mysql2ssim);
    if (acr_my::_db.cmdline.fldfunc) {
        command::acr acr;
        acr.replace=true;
        acr.write=true;
        acr.report=false;
        acr.print=false;
        acr.in = acr_my::_db.cmdline.in;
        cmd << " | " << command::acr_ToCmdline(acr);
    }
    SysCmd(cmd, FailokQ(false));
}

// -----------------------------------------------------------------------------
// returns 0 if mysqld is down
// or its pid if it is up
static int MysqlUpQ() {
    int pid=0;
    // recover pid from file
    tempstr file_pid(Subst(acr_my::_db.R, "$data_logdir/mysql_pid"));
    if (FileQ(file_pid)){
        pid=ParseI32(algo::FileToString(file_pid,algo::FileFlags()),0);
    }
    // check if the process still active
    if (!DirectoryQ(tempstr()<<"/proc/"<<pid<<"/fd")){
        pid=0;
    }
    return pid;
}
// -----------------------------------------------------------------------------

static void Main_Download() {
    if (MysqlUpQ()){
        ind_beg(acr_my::_db_nsdb_curs, nsdb, acr_my::_db) if (nsdb.select) {
            Main_DownloadNs(nsdb);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static void Main_Stop() {
    int step=0;
    tempstr cmd = Subst(acr_my::_db.R
                        , "mysqladmin"
                        " --no-defaults"
                        " --protocol=socket"
                        " --socket=$data_dir/mysql.sock"
                        " shutdown"
                        " >/dev/null"
                        " 2>&1");
    while (MysqlUpQ()) {
        if (step%20==19) {
            prlog("acr_my.wait_shutdown"
                  <<Keyval("pid",MysqlUpQ()));
        }
        SysCmd(Zeroterm(cmd),FailokQ(true),DryrunQ(false));
        algo::SleepMsec(50);
        step++;
        vrfy(step<20*10, "server stop timeout");
    }

    algo::RemDirRecurse("mysql", true);
}

// -----------------------------------------------------------------------------

static void Main_Shell() {
    tempstr lastns;
    ind_beg(acr_my::_db_nsdb_curs,nsdb,acr_my::_db) if (nsdb.select) {
        lastns=nsdb.ns;
    }ind_end;
    Set(acr_my::_db.R, "$db", lastns);
    // entering interactive shell here --
    // when command returns, the user is done.
    SysCmd(Subst(acr_my::_db.R, "mysql -S $data_dir/mysql.sock $db"), FailokQ(false));
}

// -----------------------------------------------------------------------------

void acr_my::Main() {
    // mysql integration: start, stop, abort, enter mysql shell
    if (acr_my::_db.cmdline.e) {
        acr_my::_db.cmdline.start=true;
        acr_my::_db.cmdline.stop =true;
        acr_my::_db.cmdline.shell=true;
    }

    // until a work-around to the "permission denied binding socket" error,
    // prevent the user from running this command to avoid surprises.
    if (getuid()==0) {
        vrfy(0,"mysqld_safe is known not to work properly from root account. try another account");
    }

    // has to be a full pathname, otherwise mysql fails.
    acr_my::_db.data_dir << algo::GetCurDir() << "/temp/mysql";
    Set(acr_my::_db.R, "$data_dir", acr_my::_db.data_dir);
    // log has to go to a different dir, otherwise mysql --initialise fails
    acr_my::_db.data_logdir << algo::GetCurDir() << "/temp/mysql_log";
    Set(acr_my::_db.R, "$data_logdir", acr_my::_db.data_logdir);

    ind_beg(acr_my::_db_nsdb_curs, nsdb,acr_my::_db) {
        bool match = Regx_Match(acr_my::_db.cmdline.nsdb, tempstr()<<nsdb.ns<<".");// allow dmmeta.% to match dmmeta
        match |= Regx_Match(acr_my::_db.cmdline.nsdb, nsdb.ns);// allow dmm%a to match dmmeta
        if (match) {
            nsdb.select=true;
        }
    }ind_end;

    // start mysql
    if (acr_my::_db.cmdline.start) {
        Main_Start();
    }

    // we upload one namespace at a time -- for all selected ssimfiles.
    if (acr_my::_db.cmdline.start) {
        Main_Upload();
    }

    if (acr_my::_db.cmdline.shell) {
        Main_Shell();
    }

    if (acr_my::_db.cmdline.shell && !(acr_my::_db.cmdline.stop || acr_my::_db.cmdline.abort)) {
        prlog("use acr_my -stop to save changes, -abort to discard.");
    }

    // TODO: connect to mysql and download data directly
    if (acr_my::_db.cmdline.stop) {
        Main_Download();
        // make sure the report comes from the download step, not from us.
    }

    // wait for server exit
    // delete mysql directory
    if (acr_my::_db.cmdline.stop || acr_my::_db.cmdline.abort) {
        Main_Stop();
    }
}
