// Copyright (C) 2026 AlgoRND
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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/comp.cpp
//

#include "include/algo.h"
#include "include/atf_ci.h"

// -----------------------------------------------------------------------------

void atf_ci::citest_atf_amc() {
    command::atf_amc_proc atf_amc;
    atf_amc_ExecX(atf_amc);
}

// -----------------------------------------------------------------------------

// Indent any .json files under ts/.
void atf_ci::citest_check_json() {
    ind_beg(_db_gitfile_curs,gitfile,_db) {
        if (StartsWithQ(gitfile.gitfile,"ts/") && GetFileExt(gitfile.gitfile) == ".json") {
            SysCmd(tempstr()<<"bin/check-json.mjs "<<algo::strptr_ToBash(gitfile.gitfile),FailokQ(false));
        }
    }ind_end;
}



// -----------------------------------------------------------------------------

void atf_ci::citest_atf_unit() {
    command::atf_unit_proc atf_unit;
    atf_unit.cmd.capture = CaptureQ();
    atf_unit.cmd.perf_secs=0;
    atf_unit_ExecX(atf_unit);
}

// -----------------------------------------------------------------------------

void atf_ci::citest_atf_comp() {
    command::atf_comp_proc atf_comp;
    atf_comp.cmd.capture = CaptureQ() ? command_atf_comp_mode_capture : command_atf_comp_mode_run;
    atf_comp.cmd.maxerr = 3;
    atf_comp_ExecX(atf_comp);
}

// -----------------------------------------------------------------------------

// Run ams_sendtest once in the shape AMS_SENDTEST describes, echo everything the
// run prints, and check that it moved all of its data.  Answers how often the
// writer was refused, so a caller can also say whether the shape was meant to
// reach backpressure at all.
//
// The knobs every shape shares are set here.  Three readers, so that a shared
// lane and a lane per reader are genuinely different arrangements.  Board mode
// asks for messages of 8 to 16 kilobytes, because the board exists for messages
// a ring cannot hold and carries nothing otherwise.  And two time limits: the
// tool's own, which ends a stalled run as a failure, and the subprocess alarm
// behind it, because a blocking send waits for room without consulting any
// deadline -- a reader that stops for good would hang it, and the alarm turns
// that into one failed run instead of a CI job killed at its own timeout.
//
// The verdict arrives as one report.ams_sendtest row per process -- the writer's
// says how many messages it sent and whether every reader exited zero, each
// reader's says how many it received and that the lane's read offset accounts for
// them.  Reading the rows rather than only the exit status is what makes a
// truncated run visible: a lane that carried half the stream and a lane that
// carried all of it differ in the rows, and not in the exit code of a process
// that decided its own time was up.
static u64 RunAmsSendtest(command::ams_sendtest_proc &ams_sendtest) {
    ams_sendtest.cmd.nchild = 3;
    ams_sendtest.cmd.nmsg = 2000;
    ams_sendtest.cmd.timeout = 60;
    ams_sendtest.timeout = 120;
    if (ams_sendtest.cmd.board) {
        ams_sendtest.cmd.msgsize_min = 8192;
        ams_sendtest.cmd.msgsize_max = 16384;
    }
    tempstr cmdline = ams_sendtest_ToCmdline(ams_sendtest);
    prlog("atf_ci.sendtest"<<Keyval("cmd",cmdline));
    ams_sendtest.fstdout = "|";
    u64 n_write_wait = 0;
    int n_writer = 0;
    int n_reader = 0;
    int n_fail = 0;
    ams_sendtest_Start(ams_sendtest);
    ind_beg(algo::FileLine_curs,line,ams_sendtest.from_stdout) {
        prlog(line);
        report::ams_sendtest row;
        if (report::ams_sendtest_ReadStrptrMaybe(row,line)) {
            n_fail += !row.success;
            // the writer is the one process in the run that sent anything
            if (row.n_msg_send > 0) {
                n_write_wait = row.n_write_wait;
                n_writer += row.n_msg_send == row.n_msg;
            } else {
                n_reader += row.n_msg_recv == row.n_msg;
            }
        }
    }ind_end;
    ams_sendtest_Wait(ams_sendtest);
    vrfy(ams_sendtest.status == 0, tempstr()<<"atf_ci.sendtest_exit"<<Keyval("cmd",cmdline)
         <<Keyval("comment",algo::DescribeWaitStatus(ams_sendtest.status)));
    vrfy(n_fail == 0, tempstr()<<"atf_ci.sendtest_verdict"<<Keyval("cmd",cmdline)
         <<Keyval("n_fail",n_fail)<<Keyval("comment","a process reported success:N"));
    vrfy(n_writer == 1, tempstr()<<"atf_ci.sendtest_writer"<<Keyval("cmd",cmdline)
         <<Keyval("n_writer",n_writer)<<Keyval("comment","one writer must report every message sent"));
    vrfy(n_reader == ams_sendtest.cmd.nchild, tempstr()<<"atf_ci.sendtest_reader"<<Keyval("cmd",cmdline)
         <<Keyval("n_reader",n_reader)<<Keyval("nchild",ams_sendtest.cmd.nchild)
         <<Keyval("comment","every reader must report every message received"));
    return n_write_wait;
}

// Move a numbered stream through every shape an ams lane can take, and then
// through the same shapes past a reader too slow to keep up with it.
//
// Three independent choices make a lane's shape, so the shapes are a cross
// product rather than a list: where a message goes (one lane every reader shares,
// or a lane per reader), how it travels (inline in those lanes, or as a reference
// to a slot on the writer's message board), and how a party that has to wait is
// woken (the poll loop finds the data, or a signal delivers it).  ams_sendtest
// numbers every message and checks each arrival against its position, so a
// reference resolved to the wrong slot, or a ring wrapped over bytes a reader had
// not yet consumed, surfaces as a payload mismatch rather than as a count that
// happens to add up.
//
// A reader that pauses 200 microseconds per message is three orders slower than
// the writer, so it falls behind and stays there.  The lane it reads is an order
// smaller than the stream, so the writer runs out of room to put its next
// message.  What the writer does about that is the contract worth checking, and
// it differs by send mode.  A
// non-blocking send is refused, and the refusal has to be counted and retried
// rather than dropped, so the run must report refusals and still deliver every
// message to every reader.  A blocking send waits for room instead, so it must
// never be refused at all.  Both end on the message count rather than on the
// tool's time limit, which is the property a slow reader threatens: the lane
// slows to the reader's rate, and it does not stop.
void atf_ci::citest_ams_sendtest() {
    // bit 0 is a lane per reader, bit 1 is the board, bit 2 is signaled wakeup
    for (int shape = 0; shape < 8; shape++) {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.uc = (shape & 1) != 0;
        ams_sendtest.cmd.board = (shape & 2) != 0;
        ams_sendtest.cmd.signaled = (shape & 4) != 0;
        RunAmsSendtest(ams_sendtest);
    }
    // every shape again, this time behind a reader that cannot keep up, on a lane
    // an order smaller than the stream it has to carry
    for (int shape = 0; shape < 8; shape++) {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.uc = (shape & 1) != 0;
        ams_sendtest.cmd.board = (shape & 2) != 0;
        ams_sendtest.cmd.signaled = (shape & 4) != 0;
        ams_sendtest.cmd.recvdelay_ns = 200000;
        ams_sendtest.cmd.bufsize = 32768;
        u64 n_write_wait = RunAmsSendtest(ams_sendtest);
        vrfy(n_write_wait > 0, tempstr()<<"atf_ci.sendtest_nobackpressure"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("comment","a lane smaller than its stream must refuse a writer whose reader is behind"));
    }
    // The blocking send waits for room on the shared lane rather than taking a
    // refusal, so the same slow reader must produce no refusal at all.
    {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.blocking = true;
        ams_sendtest.cmd.signaled = true;
        ams_sendtest.cmd.recvdelay_ns = 200000;
        ams_sendtest.cmd.bufsize = 32768;
        u64 n_write_wait = RunAmsSendtest(ams_sendtest);
        vrfy(n_write_wait == 0, tempstr()<<"atf_ci.sendtest_blocking_refused"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("n_write_wait",n_write_wait)
             <<Keyval("comment","a blocking send waits for room instead of being refused"));
    }
}

// -----------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_ssimfile() {
    // create a new ssimdb
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.target="ssimdb";
        acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // create a new ssimfile
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="ssimdb.xyz";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // insert a tuple
    vrfy_(SysCmd("echo dev.xyz xyz:blah | acr -insert -write")==0);
    // query it
    vrfy_(SysCmd("acr xyz")==0);

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ExecX(acr);

    // build everything
    command::abt_proc abt;
    abt.cmd.target.expr="abt";
    abt_ExecX(abt);
}

// -----------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_ssimdb() {
    // create a new ssimdb
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.target="ssimdb";
    acr_ed.cmd.nstype=dmmeta_Nstype_nstype_ssimdb;
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ExecX(acr);
}

// -----------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_unittest() {
    // create a new ssimdb
    command::acr_ed_proc acr_ed;
    acr_ed.cmd.create=true;
    acr_ed.cmd.unittest="algo_lib.SomeTest";
    acr_ed.cmd.write=true;
    acr_ed_ExecX(acr_ed);

    // check that everything is ok
    command::acr_proc acr;
    acr.cmd.query = "%";
    acr.cmd.check=true;
    acr_ExecX(acr);

    command::abt_proc abt;
    abt.cmd.target.expr = "%";
    abt.cmd.build=true;
    abt_ExecX(abt);

    command::atf_unit_proc atf_unit;
    atf_unit.cmd.unittest.expr = "algo_lib.SomeTest";
    atf_unit_ExecX(atf_unit);
}

// --------------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_acr_ed_target() {
    // create a new target
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.target="acr_test";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
        command::abt_proc abt;
        abt.cmd.target.expr = "acr_test";
        abt_ExecX(abt);
    }
    // create 2 new ssimfiles
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="dev.test1";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);

        acr_ed.cmd.ssimfile="dev.test2";
        acr_ed.cmd.subset="dev.Test1";
        acr_ed_ExecX(acr_ed);
    }
    // create finputs
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.finput=true;
        acr_ed.cmd.target="acr_test";
        acr_ed.cmd.ssimfile="dev.test1";
        acr_ed.cmd.indexed=true;
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);

        acr_ed.cmd.ssimfile="dev.test2";
        acr_ed.cmd.indexed=false;
        acr_ed_ExecX(acr_ed);
    }
    // create xrefs
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.field="acr_test.FTest1.c_test2";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);

        acr_ed.cmd.field="acr_test.FTest2.p_test1";
        acr_ed_ExecX(acr_ed);

        command::amc_vis_proc amc_vis;
        amc_vis.cmd.ctype.expr="acr_test.%";
        amc_vis_ExecX(amc_vis);
    }
    // rename this target and check that everything compiles
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.target="acr_test";
        acr_ed.cmd.rename="samp_test";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
        command::abt_proc abt;
        abt.cmd.target.expr = "samp_test";
        abt_ExecX(abt);
    }
}

// -----------------------------------------------------------------------------

// Runs in sandbox
void atf_ci::citest_abt_md_after_ssimfile_is_added() {
    // create a new ssimfile
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="dev.xyz";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    // run abt_md to produce xyz.md and add it into README.md
    command::abt_md_proc abt_md;
    abt_md.cmd.readmefile.expr = "txt/ssimdb/dev/%";
    abt_md_ExecX(abt_md);

    vrfy_(SysCmd("grep 'dev.xyz' txt/ssimdb/dev/README.md")==0);
}

void atf_ci::citest_apm() {
    CitestApm();
}

void atf_ci::citest_apm_reinstall() {
    CitestApmReinstall();
}

// Check that each citest function lives in the file matching its cijob.
// Expected: citest:xyz with cijob:zzz → function citest_xyz in cpp/atf_ci/zzz.cpp
// TODO: make this table-driven, with table describing all function contraints
void atf_ci::citest_check_citest() {
    // load all function locations in one call
    command::src_func_proc src_func;
    src_func.cmd.func.expr = "atf_ci.citest_%";
    src_func.cmd.showloc = true;
    // build map: function name (e.g. "citest_xyz") → source file
    // src_func output: "cpp/atf_ci/comp.cpp:27: void atf_ci::citest_atf_amc()"
    algo::strptr prefix = "atf_ci::citest_";
    src_func.fstdout = "|";
    src_func_Start(src_func);
    ind_beg(algo::FileLine_curs, line, src_func.from_stdout) {
        strptr trimmed = Trimmed(line);
        strptr file = Pathcomp(trimmed, ":LL");
        strptr func = Pathcomp(trimmed, "(RL RR");// "cpp/atf_ci/comp.cpp:27: void atf_ci::citest_atf_amc"
        if (StartsWithQ(func, prefix)) {
            strptr name = RestFrom(func, prefix.n_elems);// "citest_xyz"
            atf_ci::FCitest *citest = atf_ci::ind_citest_Find(name);
            if (citest) {
                citest->srcfile = file;
            }
        }
    }ind_end;
    // check each citest record.
    //
    // A citest lives in the file named after the cijob that runs it, so a reader
    // who knows the job knows where to look.  Some citests drive tools that only
    // this tree has -- x2img, atf_x2aws, lsttool -- and those are kept out of the
    // openacr package, which names whole files rather than functions.  So each
    // cijob may also have an _x2 file holding exactly that part, and the name
    // still says which job runs it.
    int n_err = 0;
    ind_beg(atf_ci::_db_citest_curs, citest, atf_ci::_db) {
        tempstr expected = tempstr() << "cpp/atf_ci/" << citest.cijob << ".cpp";
        tempstr expected_x2 = tempstr() << "cpp/atf_ci/" << citest.cijob << "_x2.cpp";
        if (citest.srcfile != expected && citest.srcfile != expected_x2) {
            prlog("atf_ci.badloc"
                  <<Keyval("citest",citest.citest)
                  <<Keyval("actual",citest.srcfile)
                  <<Keyval("expected",expected));
            n_err++;
        }
    }ind_end;
    vrfy(n_err == 0, tempstr() << n_err << " citest function(s) in wrong file");
}
