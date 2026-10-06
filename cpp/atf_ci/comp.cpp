// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
// run prints, and check that it moved all of its data.  Returns the writer's
// report row, whose refusal counts -- by the lane's room and by a write
// channel's limit -- let a caller say whether the shape reached the backpressure
// it was meant to.
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
static report::ams_sendtest RunAmsSendtest(command::ams_sendtest_proc &ams_sendtest) {
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
    report::ams_sendtest writer;
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
                writer = row;
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
    return writer;
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
//
// Then a shm channel per reader on the lane shapes.  A channel is a flow
// inside the lane whose limit follows its own reader's read count, so the writer
// is paced by each reader and not by lane space alone.
//
// The last pass is a correctness test of the writer's wake.  Every pass before
// it sends from a recurring timer, which keeps the writer's loop awake, so a
// wake that went missing would cost nothing and go unseen.  In park mode the
// writer sleeps whenever it is refused, and a missing wake stops the run.
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
        u64 n_write_wait = RunAmsSendtest(ams_sendtest).n_write_wait;
        vrfy(n_write_wait > 0, tempstr()<<"atf_ci.sendtest_nobackpressure"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("comment","a lane smaller than its stream must refuse a writer whose reader is behind"));
    }
    // Fan-out over a lane per reader with one reader that cannot keep up: the
    // board must keep delivering to the readers that can, and the lane sizing
    // must still refuse the writer when the slow one is behind.
    {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.uc = true;
        ams_sendtest.cmd.board = true;
        ams_sendtest.cmd.slowreader = true;
        ams_sendtest.cmd.recvdelay_ns = 200000;
        ams_sendtest.cmd.bufsize = 32768;
        u64 n_write_wait = RunAmsSendtest(ams_sendtest).n_write_wait;
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
        u64 n_write_wait = RunAmsSendtest(ams_sendtest).n_write_wait;
        vrfy(n_write_wait == 0, tempstr()<<"atf_ci.sendtest_blocking_refused"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("n_write_wait",n_write_wait)
             <<Keyval("comment","a blocking send waits for room instead of being refused"));
    }
    // The lane shapes again with a shm channel per reader: shared or one lane
    // per reader, woken by polling or by signal.  Each reader sets its channel a
    // window past what it has read, smaller than the lane, so the channels bind
    // before the lane does: the writer must be refused on its limit and still
    // deliver every message, and ams_sendtest checks that each channel ends with
    // everything written read and nothing written past its limit.  A board
    // message travels by the board's own path and writes on no channel.
    for (int shape = 0; shape < 4; shape++) {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.uc = (shape & 1) != 0;
        ams_sendtest.cmd.signaled = (shape & 2) != 0;
        ams_sendtest.cmd.channel = true;
        ams_sendtest.cmd.channel_window = 1024;
        u64 n_limit_wait = RunAmsSendtest(ams_sendtest).n_limit_wait;
        vrfy(n_limit_wait > 0, tempstr()<<"atf_ci.sendtest_nolimit"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("comment","a channel window smaller than the lane must refuse the writer on its limit"));
    }
    // A window no larger than the ring's largest message would leave the writer
    // refused forever once its reader has read everything, so the parent refuses
    // the run before any process starts, naming the cause.  The test asks for the
    // refusal by name: a run that started and ran out its time limit also exits
    // non-zero, and so does a missing binary.
    {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.channel = true;
        ams_sendtest.cmd.channel_window = 300;
        ams_sendtest.cmd.timeout = 10;
        int status = 0;
        tempstr out(SysEval(tempstr() << ams_sendtest_ToCmdline(ams_sendtest) << " 2>&1", FailokQ(true), 1024*64, false, &status));
        vrfy(status != 0 && algo::FindStr(out, "ams_sendtest.badwindow") != -1, tempstr()<<"atf_ci.sendtest_badwindow_accepted"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("status",status)
             <<Keyval("comment","a channel window no larger than a message must be refused by name"));
    }
    // A writer that sleeps.  In park mode the parent sends from a step that holds
    // its loop awake only while it makes progress; refused, it parks and sleeps,
    // and only its readers' wake signals run it again.  Behind a slow reader the
    // writer runs out of room many times, on the lane's space or on a channel's
    // limit, so every lost wake would leave it asleep until the tool's time
    // limit, and the run would fail.  The run must park, and must still deliver
    // every message.  The wake is a real-time signal read through a signalfd,
    // which only Linux has; elsewhere signaled mode keeps busy-polling and a
    // parked writer has nothing to wake it, so the shapes run on Linux alone.
#ifdef __linux__
    int nshape = 4;
#else
    int nshape = 0;
#endif
    for (int shape = 0; shape < nshape; shape++) {
        command::ams_sendtest_proc ams_sendtest;
        ams_sendtest.cmd.uc = (shape & 1) != 0;
        ams_sendtest.cmd.channel = (shape & 2) != 0;
        ams_sendtest.cmd.signaled = true;
        ams_sendtest.cmd.parkwrite = true;
        ams_sendtest.cmd.recvdelay_ns = 200000;
        ams_sendtest.cmd.bufsize = 32768;
        ams_sendtest.cmd.channel_window = 1024;
        u64 n_writer_park = RunAmsSendtest(ams_sendtest).n_writer_park;
        vrfy(n_writer_park > 0, tempstr()<<"atf_ci.sendtest_nopark"
             <<Keyval("cmd",ams_sendtest_ToCmdline(ams_sendtest))
             <<Keyval("comment","a writer behind a slow reader must park, and a reader's wake must resume it"));
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
//
// A table created now is listed by its namespace's page, and nothing was regenerated to
// make that true: the list comes from dmmeta.ssimfile when the page is asked for, so the
// page is right the moment the row exists.  The check that used to stand here asked
// whether abt_md had written a markdown file per table and named it in a directory
// README, which is the arrangement this replaced.
void atf_ci::citest_doc_after_ssimfile_is_added() {
    // create a new ssimfile
    {
        command::acr_ed_proc acr_ed;
        acr_ed.cmd.create=true;
        acr_ed.cmd.ssimfile="dev.xyz";
        acr_ed.cmd.write=true;
        acr_ed_ExecX(acr_ed);
    }

    vrfy_(SysCmd("bin/doc txt/ssimdb/dev -pager:N -color:N -width:100 | grep 'dev.xyz'")==0);
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
    // a tree extending openacr has, and those are kept out of the openacr package,
    // which names whole files rather than functions.  So each cijob may also have
    // a file of the extender's holding exactly that part, named after the job with
    // the extender's suffix, and the name still says which job runs it.
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

// -----------------------------------------------------------------------------

// Evaluate the tutorials' inline commands, which the readme citest of the
// normalize job leaves alone: they rebuild a sample program with acr_ed -write
// and take most of a whole-tree abt_md pass.  The run takes no selection, so
// abt_md alone says which readmes are tutorials.  A tutorial whose output moved
// leaves the file modified, and the job fails on it.
void atf_ci::citest_readme_tut() {
    command::abt_md_proc abt_md;
    abt_md.cmd.tut = true;
    abt_md_ExecX(abt_md);
}
