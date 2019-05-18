// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/lockfile.cpp
//

#include "include/atf_unit.h"

// --------------------------------------------------------------------------------

static void CheckPid() {
    const char *fname = "temp/lock1";
    tempstr contents = FileToString(fname,algo_FileFlags_none);
    tempstr shouldbe = tempstr()<<getpid();
    vrfyeq_(contents, shouldbe);
}

// -----------------------------------------------------------------------------

static void CheckFdLeak() {
    int fd = open("/dev/null",O_RDWR);
    vrfy_(fd<50);// check that FDs didn't leak
    if (fd >= 0) {
        close(fd);
    }
}

// -----------------------------------------------------------------------------

static void Lock1() {
    const char *fname = "temp/lock1";
    algo_lib::FLockfile lockfile1;
    algo_lib::FLockfile lockfile2;

    bool ok1 = LockFileInit(lockfile1, fname, FailokQ(true));
    vrfyeq_(ok1, true);
    CheckPid();

    bool ok2 = LockFileInit(lockfile2, fname, FailokQ(true));
    vrfyeq_(ok2, false);

    Refurbish(lockfile1);// give it up
    vrfyeq_(FileQ(fname), false);

    ok2 = LockFileInit(lockfile1, fname, FailokQ(true));
    vrfyeq_(ok2, true);
    CheckPid();
}

// -----------------------------------------------------------------------------

// Check that exception is thrown
static void Lock2() {
    const char *fname = "temp/lock1";
    algo_lib::FLockfile lockfile1;
    algo_lib::FLockfile lockfile2;

    bool ok1 = LockFileInit(lockfile1, fname, FailokQ(false));
    bool ok2 = false;
    bool thrown=false;
    try {
        ok2 = LockFileInit(lockfile2, fname, FailokQ(false));
    } catch(...) {
    }
    vrfyeq_(ok1, true);
    CheckPid();
    vrfyeq_(ok2, false);
    vrfyeq_(thrown, false);
}

// -----------------------------------------------------------------------------

// Repeated locking with exception thrown
static void Lock3() {
    const char *fname = "temp/lock1";
    algo_lib::FLockfile lockfile1;
    int ntry=0;
    int nsuccess=0;
    for (int i=0; i<100; i++) {
        try {
            ntry++;
            nsuccess += LockFileInit(lockfile1, fname, FailokQ(false));
        } catch(...) {
        }
    }
    CheckFdLeak();
    vrfy_(ntry==100);
    vrfy_(nsuccess==1);
}

// -----------------------------------------------------------------------------

// Repeated locking with bad path
static void Lock4() {
    const char *fname = "/non/existent/path";
    algo_lib::FLockfile lockfile1;
    int ntry=0;
    int nsuccess=0;
    for (int i=0; i<100; i++) {
        try {
            ntry++;
            nsuccess += LockFileInit(lockfile1, fname, FailokQ(true));
        } catch(...) {
        }
    }
    CheckFdLeak();
    vrfy_(ntry==100);
    vrfy_(nsuccess==0);
}

// -----------------------------------------------------------------------------

static void Lock5() {// version with timeout
    const char *fname = "temp/lock1";
    algo_lib::FLockfile lockfile1;
    algo_lib::FLockfile lockfile2;

    bool ok1 = LockFileInit(lockfile1, fname, FailokQ(true));
    vrfyeq_(ok1, true);
    CheckPid();

    SchedTime start = CurrSchedTime();
    bool ok1_2 = LockFileInit(lockfile2, fname, FailokQ(true), UnDiffSecs(1));
    vrfyeq_(ok1_2, false); // must timeout
    SchedTime end = CurrSchedTime();
    vrfy_(ElapsedSecs(start,end) >= 0.9); /* at least 90% of time elapsed */
}

// -----------------------------------------------------------------------------

static void Lock6() {// lock twice
    const char *fname = "temp/lock1";
    algo_lib::FLockfile lockfile1;

    bool ok1 = LockFileInit(lockfile1, fname, FailokQ(true));
    vrfyeq_(ok1, true);
    CheckPid();

    // same file
    bool ok2 = LockFileInit(lockfile1, fname, FailokQ(true));
    vrfyeq_(ok2, false);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Lockfile() {
    Lock1();
    Lock2();
    Lock3();
    Lock4();
    Lock5();
    Lock6();
}
