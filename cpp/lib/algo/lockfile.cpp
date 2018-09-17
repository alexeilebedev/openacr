// (C) 2019 NYSE | Intercontinental Exchange
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/lockfile.cpp -- Lock file
//

#include "include/algo.h"

// -----------------------------------------------------------------------------

// Attempt to lock FD for up to WAIT_TIMEOUT seconds
// Return codes are same as the flock() system call
static int FlockTimeout(int fd, algo::UnDiff wait_timeout) {
    // always attempt non-blocking lock
    int flags = LOCK_EX | LOCK_NB;
    algo_lib::giveup_time_UpdateCycles();
    // calculate timeout
    SchedTime time_limit = algo_lib::_db.clock + ToSchedTime(ToSecs(wait_timeout));
    int rc = -1;
    do {
        rc = flock(fd, flags);
        if (rc != 0 && algo_lib::_db.clock < time_limit) {
            // 10 millisecond sleep should be sufficient
            double sleeptime = double_Min(ElapsedSecs(algo_lib::_db.clock,time_limit), 0.01);
            usleep(u32(sleeptime * 1e6));
        }
        algo_lib::giveup_time_UpdateCycles();
    } while (rc != 0 && algo_lib::_db.clock < time_limit);
    return rc;
}

// -----------------------------------------------------------------------------

// Attempt to lock the file named NAME, updating state of LOCKFILE.
// Return TRUE if lock is successfully applied.
// If FAIL_OK is false, an error causes an exception (function always returns true)
//     If an exception is thrown, the text contains error description.
// Otherwise, algo_lib::_db.errtext (DetachBadTags()) contains human-readable description of error.
// If WAIT_TIMEOUT is non-zero, block up to WAIT_TIMEOUT seconds before failing
// Write pid to file specified in NAME, and lock file using flock().
bool algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok, algo::UnDiff wait_timeout) {
    cstring &err = algo_lib::ResetErrtext();
    bool was_open = ValidQ(lockfile.fildes.fd);
    if (was_open) {
        err << "algo_lib.flock_reentrant"
            <<Keyval("filename",lockfile.filename)
            <<Keyval("comment","lock already taken");
    }
    if (err == "") {
        lockfile.filename = name;
        int old_mask = umask(0);
        //mode 666 so that different users can try to lock the resource
        int os_flags = O_RDWR | O_CREAT | O_CLOEXEC;
        lockfile.fildes.fd.value = open(Zeroterm(lockfile.filename), os_flags, 0666);
        umask(old_mask);
    }
    if (lockfile.fildes.fd.value == -1) {
        err << "algo_lib.flock_open"
            <<Keyval("filename",lockfile.filename)
            <<Keyval("comment",strerror(errno));
    }
    if (err == "") {
        // attempt to lock the file
        if (FlockTimeout(lockfile.fildes.fd.value, wait_timeout) != 0) {
            err << "algo_lib.flock"
                <<Keyval("filename",lockfile.filename)
                <<Keyval("comment",strerror(errno));
        }
    }
    if (err == "") {
        // set the mode for the case where the file already existed
        // and had a bad mode
        fchmod(lockfile.fildes.fd.value, 0666);
        TruncateFile(lockfile.fildes.fd, 0); // truncate file -- only after successful lock
        tempstr str = tempstr() << getpid();
        // write pid to file
        if (!WriteFile(lockfile.fildes.fd, (u8*)str.ch_elems, str.ch_n)) {
            err << "algo_lib.write_lockfile"
                <<Keyval("filename",lockfile.filename)
                <<Keyval("comment",strerror(errno));
        }
    }
    if (err != "") {
        // if we opened a file and experienced an error, close it.
        // if the file was already open, then leave it alone (error is reentrancy)
        if (!was_open && ValidQ(lockfile.fildes.fd)) {
            close(lockfile.fildes.fd.value);
            lockfile.fildes.fd = algo::Fildes();
        }
        vrfy(fail_ok, err);
    }
    return err == "";
}

// -----------------------------------------------------------------------------

// Non-blocking attempt to lock LOCKFILE
// Return success status
// If FAIL_OK is FALSE, throw exception on error (must succeed)
bool algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok) {
    return LockFileInit(lockfile, name, fail_ok, algo::UnDiff());
}

// -----------------------------------------------------------------------------

// Non-blocking attempt to lock LOCKFILE.
// Throw exception on error.
// If the function returns, it has succeeded.
void algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name) {
    (void)LockFileInit(lockfile, name, algo::FailokQ(false), algo::UnDiff());
}

// -----------------------------------------------------------------------------

// Read contents of lock file FNAME, extracting the pid that's stored in there.
// On failure, return 0
pid_t algo_lib::LockFileRead(strptr fname) {
    return algo::ParseU32(FileToString(fname), 0);
}
