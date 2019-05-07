// (C) AlgoEngineering LLC 2008-2013
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/lib.cpp -- Main file
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include <arpa/inet.h>// inet_pton
#include <sys/mman.h>// mmap,mlockall
#include <sys/prctl.h>
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

// -----------------------------------------------------------------------------

// Execute unix command and return output.
tempstr algo::SysEval(strptr cmd, FailokQ fail_ok, int max_output, bool echo) {
    tempstr result;
    bool ok = true;
    int start_code=0;
    // empty command succeeds immediately
    if (cmd != "") {
        if (echo) {
            prlog(cmd);
        } else {
            verblog(cmd);
        }
        command::bash_proc bash;
        bash.cmd.c = cmd;
        algo_lib::FFildes readpipe;
        {
            int pipefd[2];
            int rc=pipe(pipefd);
            (void)rc;
            readpipe.fd.value = pipefd[0];
            bash.stdout  << ">&" << pipefd[1];
            start_code=bash_Start(bash);
            (void)close(pipefd[1]);
        }
        vrfy(start_code==0 || fail_ok
             ,tempstr("algo_lib.exec_error")
             <<Keyval("code",start_code)
             <<Keyval("cmd",cmd));
        // read from fd
        if (ok) {
            char buf[4096];
            size_t nread;
            while ((nread = read(readpipe.fd.value, buf, sizeof(buf)))!=0) {
                result << strptr(buf,(i32)nread);
                if (ch_N(result) > max_output) {
                    bash_Kill(bash);// terminate subprocess
                    vrfy(fail_ok,tempstr("algo_lib.max_output_exceeded")
                         << Keyval("max_n",max_output)
                         << Keyval("cmd",cmd));
                    ok = false;
                    break;
                }
            }
        }
        // wait for subprocess to finish
        bash_Wait(bash);
        if (ok) {
            errno_vrfy(bash.status==0 || fail_ok
                       ,tempstr("algo_lib.cmd_status_error")
                       << Keyval("status",bash.status)
                       << Keyval("cmd",cmd));
        }
    }
    if (!ok) {
        result="";
    }
    return result;
}

// -----------------------------------------------------------------------------

// Execute unix command and return status code.
// Exception is thrown if FAIL_OK is false, and subprocess does not exit normally.
int algo::SysCmd(strptr cmd, FailokQ fail_ok, DryrunQ dry_run, EchoQ echo) {
    int ret = 0;
    if (dry_run) {
        prlog("dry_run: "<<cmd);// make it clear that the command is not executing
    } else {
        // _Exec will print command in verbose mode
        // so don't do it twice
        if (echo && !algo_lib::_db.cmdline.verbose) {
            prlog(cmd);
        }
        // empty command will cause a long-living shell; don't do it
        if (cmd.n_elems > 0) {
            command::bash_proc bash;
            bash.cmd.c = cmd;
            if (fail_ok) {
                ret = bash_Exec(bash);
            } else {
                bash_ExecX(bash); // let amc-generated code throw the exception
            }
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return a human-readable description of STATUS as returned by wait() / waitpid()
tempstr algo::DescribeWaitStatus(int status) {
    tempstr ret;
    if (WIFEXITED(status)) {
        ret << "exit code "<<WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        ret << "signal "<<WTERMSIG(status);
        if (WCOREDUMP(status)) {
            ret << " (core dumped)";
        }
    } else {
        ret << "status "<<status;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// The several variants of Throw are all the same -- the goal is to get as many
// setup instructions out of the execution path as possible, to avoid polluting instruction
// cache.
void algo::Throw(strptr text, Errcode err) NORETURN {
    throw algo_lib::ErrorX(text,err);
}

void algo::Throw() NORETURN {
    Throw("", Errcode());
}

// -----------------------------------------------------------------------------

// This function uses vertical distance to simplify an input curve,
// which is assumed to be uniformly sampled.
// Returned is array $backidx, which lists indices of input points.
// Parameter tol specifies maximum vertical distance.
// "arrsim" stands for "Array Simplify".
void algo::RunArrsimp(algo::Arrsimp &arrsimp, aryptr<double> in) {
    out_Reserve(arrsimp, elems_N(in));
    if (!elems_N(in) || arrsimp.tol <= 0) {
        frep_(i,elems_N(in)) {
            out_Alloc(arrsimp) = i;
        }
    } else {
        stack_Alloc(arrsimp) = i32_Range(0,elems_N(in));
        while (stack_N(arrsimp) > 0) {
            i32_Range r = stack_qLast(arrsimp);
            stack_RemoveLast(arrsimp);
            double lo = in[r.beg], hi = in[r.end-1], diff = (hi-lo)/(r.end-1-r.beg);
            double maxd = arrsimp.tol;
            int imax=-1;
            for (int i = r.beg+1; i < r.end-1; i++) {
                double this_diff = Abs(in[i] - (lo + diff * (i-r.beg)));
                if (this_diff > maxd) {
                    maxd = this_diff;
                    imax=i;
                }
            }
            if (imax != -1) {
                stack_Alloc(arrsimp) = i32_Range(imax, r.end);
                stack_Alloc(arrsimp) = i32_Range(r.beg, imax);
            } else {
                out_Alloc(arrsimp) = r.beg;
            }
        }
        out_Alloc(arrsimp) = elems_N(in)-1;
    }
}

// Set exit time of main loop to current time.
void algo_lib::ReqExitMainLoop() {
    algo_lib::_db.limit = algo_lib::_db.clock;
}

void algo_lib::fd_Cleanup(algo_lib::FFildes &fildes) {
    if (ValidQ(fildes.fd)) {
        (void)close(fildes.fd.value);
        fildes.fd = algo::Fildes();
    }
}

bool algo::Tuple_EqualQ(Tuple &t1, Tuple &t2, bool ignore_comment) {
    bool ret = true;
    if (!(t1.head == t2.head)) {
        ret = false;
    } else if (attrs_N(t1) != attrs_N(t2)) {
        ret = false;
    } else {
        ind_beg(Tuple_attrs_curs,attr1,t1) {
            if (ignore_comment && attr1.name == "comment") {
                continue;
            }
            Attr &attr2 = attrs_qFind(t2, ind_curs(attr1).index);
            if (!(attr1 == attr2)) {
                ret = false;
                break;
            }
        }ind_end;
    }
    return ret;
}

Attr* algo::attr_Find(Tuple &tuple, strptr name, int occurence) {
    Attr *ret = NULL;
    frep_(i,attrs_N(tuple)) {
        if (attrs_qFind(tuple,i).name == name) {
            if (occurence <= 0) {
                ret = &attrs_qFind(tuple,i);
                break;
            }
            occurence--;
        }
    }
    return ret;
}

strptr algo::attr_GetString(Tuple &T, strptr name, strptr dflt) {
    Attr *attr = attr_Find(T, name, 0);
    return attr ? strptr(attr->value) : dflt;
}

// Append new attribute with specified name and value.
void algo::attr_Add(Tuple &T, strptr name, strptr value) {
    Attr &attr = attrs_Alloc(T);
    attr.name = name;
    attr.value = value;
}

// -----------------------------------------------------------------------------

void algo_lib::fildes_Cleanup(algo_lib::FIohook &iohook) {
    if (iohook.in_epoll) {
        IoHookRemove(iohook);
    }
    if (ValidQ(iohook.fildes) && !iohook.nodelete) {
        close(iohook.fildes.value);
    }
    iohook.fildes=algo::Fildes();// strip
}

// -----------------------------------------------------------------------------

// Call sleep/usleep, giving up MS milliseconds
void algo::SleepMsec(int ms) {
    if (ms>=1000) {
        sleep(ms/1000);
        ms -= (ms/1000)*1000;
    }
    usleep(ms*1000);
}

// -----------------------------------------------------------------------------

Errcode algo::FromErrno(i64 val) {
    Errcode ret;
    code_Set(ret, val);
    type_Set(ret, algo_Errns_unix);
    return ret;
}

Errcode algo::FromWinErr(i64 val) {
    Errcode ret;
    code_Set(ret, val);
    type_Set(ret, algo_Errns_win);
    return ret;
}

i64 algo::I64Power10(u32 power) {
    switch(power) {
    case  0: return 1;
    case  1: return 10;
    case  2: return 100;
    case  3: return 1000;
    case  4: return 10000;
    case  5: return 100000;
    case  6: return 1000000;
    case  7: return 10000000;
    case  8: return 100000000;
    case  9: return 1000000000;
    case 10: return 10000000000;
    case 11: return 100000000000;
    case 12: return 1000000000000;
    case 13: return 10000000000000;
    case 14: return 100000000000000;
    case 15: return 1000000000000000;
    case 16: return 10000000000000000;
    case 17: return 100000000000000000;
    default: return LLONG_MAX; // LLONG_MAX = 9223372036854775807LL
    }
}

// Copy bytes from one location to another in blocks of 16 bytes.
void algo::StreamFromToB16(void *from, void *to, u32 n) {
#ifdef AOS_SSE42
    n = (n+15)/16;
    __m128i *F    = (__m128i*)from;
    __m128i *T    = (__m128i*)to;
    for (u32 i=0; i<n; i++) {
        __m128i v0 = F[0];
        _mm_stream_si128(T, v0);
        F++;
        T++;
    }
#else
    memcpy(to,from,n);
#endif
}

void algo::CopyFromToB16(void *from, void *to, u32 n) {
#ifdef AOS_SSE42
    n = (n+15)/16;
    __m128i *F    = (__m128i*)from;
    __m128i *T    = (__m128i*)to;
    for (u32 i=0; i<n; i++) {
        __m128i v0 = F[0];
        T[0] = v0;
        F++;
        T++;
    }
#else
    memcpy(to,from,n);
#endif
}


// SSE implementation of fast exponentiation idea from
// http//nic.linotune.com/pubs/Schraudolph99.pdf
// ExpFast(x) goes bad at x < -709 (exp doesn't)
// Where it works, ExpFast makes a steady relative error of about 2%.
double algo::ExpFast(double y) {
#ifdef AOS_SSE42
    __m128d x = _mm_setr_pd((1<<20) / M_LN2, 1072693248 - 60801);
    // how do we get rid of the resulting movddup??
    __m128d ry = _mm_set1_pd(y);
    x = _mm_mul_sd(x,ry);
    x = _mm_hadd_pd(x,x);
    __m128i z = _mm_cvtpd_epi32(x);
    z = _mm_shuffle_epi32(z,0x62);
    double ret;
    _mm_store_sd(&ret,_mm_castsi128_pd(z));
    return ret;
#else
    return exp(y);
#endif
}

// -----------------------------------------------------------------------------

// lock all presently, and future allocated bytes in physical memory.
// Return success value. Errno provides more info.
bool algo::LockAllMemory() {
    return mlockall(MCL_CURRENT | MCL_FUTURE)==0;
}

// -----------------------------------------------------------------------------

static void SignalHandler(int sig) {
    algo_lib::_db.last_signal = sig;
    i64 t_now = get_cycles();
    // 2 interrupts within 2 second -- die.
    // otherwise the process becomes unkillable
    i64 thresh = algo_lib::_db.t_last_signal + get_cpu_hz_int()*2;
    if (thresh > t_now) {
        kill(getpid(), SIGKILL);
    }
    algo_lib::_db.t_last_signal.value = t_now;
    algo_lib::_db.limit = algo_lib::_db.clock; // interrupt
}

void algo::SetupExitSignals(bool sigint) {
    struct sigaction sigact;
    sigact.sa_handler = SignalHandler;
    sigemptyset( &sigact.sa_mask );
    sigact.sa_flags = SA_RESTART;
    errno_vrfy(sigaction( SIGHUP, &sigact, 0 )==0,"sigaction");
    if (sigint) {
        errno_vrfy(sigaction( SIGINT, &sigact, 0 )==0,"sigaction");
    }
    errno_vrfy(sigaction(SIGTERM, &sigact, 0 )==0,"sigaction");
    // SIGPIPE, which can occur on sockets, should result in an error (EPIPE)
    // not in killed processes
    signal(SIGPIPE, SIG_IGN);
}

void algo_lib::bh_timehook_Step() {
    // execute all expired time hooks (but no time hook gets executed twice in this loop)
    algo_lib::FTimehook *timehook = algo_lib::bh_timehook_First();
    u64 clock_thresh = algo_lib::_db.clock;// process all expired timehooks; no timehook gets 2 callbacks
    while (timehook && timehook->time < clock_thresh) {
        // advance timehook to the future (even if is non-recurrent).
        // this means that before the next time it's scheduled with bh_timehook_Reheap,
        // it will wait at least DELAY clocks.
        timehook->time.value = algo_lib::_db.clock + timehook->delay;
        if (timehook->recurrent) {
            algo_lib::bh_timehook_ReheapFirst();
        } else {
            algo_lib::bh_timehook_RemoveFirst();
        }
        hook_Call(*timehook,*timehook);
        algo_lib::_db.next_loop = algo_lib::_db.clock; // callback may have created work to do
        algo_lib::bh_timehook_UpdateCycles();
        timehook = algo_lib::bh_timehook_First();
        clock_thresh = u64_Max(clock_thresh,3000)-3000;// lower threshold by 1 microsecond to avoid timehook pileup
    }
    if (timehook) {
        algo_lib::_db.next_loop.value = u64_Min(timehook->time.value, algo_lib::_db.next_loop);
    }
}

static void SleepClocks(u64 clocks) {
    if (clocks > 10000) {
        u64 sleep_nsec = u64(clocks * algo_lib::_db.clocks_to_ns);
        struct timespec t;
        t.tv_sec = sleep_nsec / 1000000000;
        t.tv_nsec = sleep_nsec % 1000000000;
        nanosleep(&t,&t);
    }
}

// -----------------------------------------------------------------------------

static void DoEpoll(u64 sleep_clocks) {
    // this is a heuristic -- do not call epoll on every cycle.
    // epolls are expensive (650 cycles/call) with vma_lib, while main steps are cheap
    // (200 cycles/step).
    if ((sleep_clocks > 0) || (algo_lib::_db.giveup_count & 1) == 0) {
        i32 sleep_ms = i32(i64_Min(sleep_clocks * algo_lib::_db.clocks_to_ms, 60000));
        epoll_event events[20];
        if (UNLIKELY(algo_lib::_db.sleep_roundup)) {
            // Avoid taking 100% cpu when sleep is < 1 msec
            sleep_ms += sleep_clocks > 0 && sleep_ms == 0;
        }
        int n = epoll_wait(algo_lib::_db.epoll_fd, events, _array_count(events), sleep_ms);
        for (int i=0; i<n; i++) {
            epoll_event &ev = events[i];
            algo_lib::FIohook *iohook = (algo_lib::FIohook*)ev.data.ptr;
            iohook->flags.value = 0;
            if (ev.events & EPOLLIN) read_Set(iohook->flags, true);
            if (ev.events & EPOLLOUT) write_Set(iohook->flags, true);
            if (ev.events & EPOLLHUP) eof_Set(iohook->flags, true);
            if (ev.events & EPOLLERR) err_Set(iohook->flags, true);
            callback_Call(*iohook,*iohook);// immediate callback
        }
    }
}

// -----------------------------------------------------------------------------

// give up unused time to the OS.
void algo_lib::giveup_time_Step() {
    // compute number of clocks to sleep before next scheduling cycle
    // If there was no sleep on the previous cycle, the sleep is zero.
    //   This last bit is important because it prevents deadlocks
    //   when one step implicitly creates work for another step
    //   that occurs before it in the main loop.
    // Sleep will not extend beyond algo_lib::_db.limit
    u64 limit = algo_lib::_db.limit;
    u64 slack = u64_SubClip(u64_Min(algo_lib::_db.next_loop, limit), algo_lib::_db.clock);
    u64 sleep_clocks = algo_lib::_db.last_sleep_clocks == 0 ? 0 : slack;
    if (algo_lib::_db.n_iohook > 0) {
        algo_lib::_db.giveup_count++;
        DoEpoll(sleep_clocks);
        // at this point, next_loop could be equal to _db.limit
        // which would cause the main loop to exit; since we just executed
        // an io wait, prevent that
        algo_lib::_db.next_loop = algo_lib::_db.clock;
    } else {
        // no i/o is in progress, just give up time using nanosleep.
        // however if next_loop is equal to limit, the loop is about to exit,
        // so do nothing.
        if (algo_lib::_db.next_loop < limit) {
            SleepClocks(sleep_clocks);
        }
    }
    algo_lib::_db.last_sleep_clocks = slack;
    algo_lib::giveup_time_UpdateCycles();
}

// -----------------------------------------------------------------------------

// Check signature on incoming data
bool algo_lib::dispsigcheck_InputMaybe(dmmeta::Dispsigcheck &dispsigcheck) {
    bool retval=true;
    algo_lib::FDispsigcheck *current = algo_lib::ind_dispsigcheck_Find(dispsigcheck.dispsig);
    if (current && !(dispsigcheck.signature==current->signature)) {
        tempstr sig1 (ch_FirstN( tempstr() << current->signature , 8));
        tempstr sig2 (ch_FirstN( tempstr() << dispsigcheck.signature ,8));
        prerr("algo_lib.dispsigcheck"
              <<Keyval("dispsig",dispsigcheck.dispsig)
              <<Keyval("compiled_sig",sig1)
              <<Keyval("incoming_sig",sig2)
              <<Keyval("comment","signature mismatch -- this program is not compatible with incoming data"));
        _exit(1);// immediately exit
    }
    if (!current) {
        retval = algo_lib::dispsigcheck_InsertMaybe(dispsigcheck);
    }
    return retval;
}

const tempstr algo::GetHostname() {
    tempstr str;
    ch_Reserve(str,128);
    int ret = gethostname(str.ch_elems,str.ch_max);
    vrfy(!ret,"gethostname() failed");
    str.ch_n = strlen(str.ch_elems);
    return str;
}

const tempstr algo::GetDomainname() {
    tempstr str;
    ch_Reserve(str,128);
    int ret = getdomainname(str.ch_elems,str.ch_max);
    vrfy(!ret,"getdomainname() failed");
    str.ch_n = strlen(str.ch_elems);
    return str;
}

// Die when parent process dies
void algo_lib::DieWithParent() {
    prctl(PR_SET_PDEATHSIG, SIGKILL);
}

// Create temporary file
// tempfile.fildes points to the new temp file after this
// temp file is created under temp/
// prefix is a namespace-unique name, such as "amc.xyz"
// Actual file that's created becomes "temp/amc.xyz.XXXXXX"
// Computed filename is saved to tempfile.filename
void algo_lib::TempfileInitX(algo_lib::FTempfile &tempfile, strptr prefix) {
    if (tempfile.fildes.fd == algo::Fildes()) {
        tempfile.filename << "temp/"<<prefix<<".XXXXXX";
        mode_t mask = umask(S_IXUSR | S_IRWXG | S_IWOTH | S_IXOTH);
        tempfile.fildes.fd = algo::Fildes(mkstemp((char*)Zeroterm(tempfile.filename)));
        umask(mask);
        vrfy(fildes_XrefMaybe(tempfile.fildes), algo_lib::_db.errtext);
        errno_vrfy(!(tempfile.fildes.fd == algo::Fildes())
                   , tempstr()<<"algo_lib.tempfile"
                   <<Keyval("filename",tempfile.filename));
    }
}

void algo_lib::fildes_Cleanup(algo_lib::FTempfile &tempfile) {
    if (ch_N(tempfile.filename)) {
        unlink(Zeroterm(tempfile.filename));
    }
}

double algo::double_WeakRandom(double scale) {
    // #AL# annotation doesn't seem to work. why?
    // coverity[dont_call]
    return (double(random()) / double((1u<<31)-1)) * scale;
}

i32 algo::i32_WeakRandom(i32 modulo) {
    // #AL# annotation doesn't seem to work. why?
    // coverity[dont_call]
    return random() % modulo;
}

// -----------------------------------------------------------------------------

// Apply redirect for given fd, _exit on error
// This function is used to implement amc's Exec
// Supported redirects:
// >filename  -- on exit, dst_fd is writing to a file
// <filename  -- on exit, dst_fd is reading from a file
// >>filename -- on exit, dst_fd is appending to a file
// >&fd       -- on exit, dst_fd is pointing to fd
// <&fd       -- on exit, dst_fd is pointing to fd
void algo_lib::ApplyRedirect(strptr redirect, int dst_fd) {
    StringIter iter(Trimmed(redirect));
    algo::FileFlags flags;
    int rc = 0;
    int src_fd = -1;
    bool try_dup = false;
    if (SkipChar(iter,'<')) {// <
        flags = flags | algo_FileFlags_read;
        try_dup = true;
    } else if (SkipChar(iter, '>')) {// > or >>
        flags = flags | algo_FileFlags_write;
        if (SkipChar(iter,'>')) {
            flags = flags | algo_FileFlags_append;
        }
        try_dup = true;
    }
    if (try_dup && SkipChar(iter,'&')) {// >&, <&, >>& forms
        src_fd = ParseI32(iter,-1);
        if (src_fd == -1) {
            rc=EINVAL;
        }
    }
    if (src_fd == -1 && !iter.EofQ()) {// try filename
        tempstr fname;
        fname << Trimmed(iter.Rest());
        src_fd = algo::OpenFile(Zeroterm(fname), flags).value;
        if (src_fd == -1) {
            rc=errno;
        }
    }
    if (src_fd >=0 && src_fd != dst_fd) {
        (void)dup2(src_fd,dst_fd);
    }
    if (rc!=0) {
        _exit(rc);
    }
}

// -----------------------------------------------------------------------------

bool algo_lib::IpmaskValidQ(const strptr ipmask) {
    bool valid = true;
    ietf::Ipv4 ip;
    strptr ip_str = Pathcomp(ipmask, "/LL");
    valid &= ietf::Ipv4_ReadStrptrMaybe(ip, ip_str);
    i32 i = Find(ipmask,'/');
    if (valid && i != -1) {
        strptr subnet_mask = Pathcomp(ipmask, "/RR");
        StringIter iter(subnet_mask);
        u32 mask_len = 0;
        valid  = ch_N(subnet_mask) > 0 && TryParseU32(iter, mask_len);
        valid  = valid && ((ip.ipv4 == 0 && mask_len == 0) || (mask_len > 0 && mask_len <=32)); // 0.0.0.0/0 is valid
        valid  = valid && (mask_len == 32 || (ip.ipv4 << mask_len == 0));  // /32 is valid for any addr

    }
    return valid;
}

// -----------------------------------------------------------------------------

// Read tuples from specified file descriptor.
// FNAME is used to report errors
// LoadFunc is called for each line.
// if STRICT is specified, function exits on first error.
// Return success status
// This function is used by amc for process startup
// If this function is passed a non-blocking file descriptor,
// it is temporarily changed to a blocking one.
bool algo_lib::LoadTuplesFd(algo::Fildes fd, strptr fname, bool (*LoadFunc)(strptr), bool strict) {
    bool retval = true;
    ind_beg(algo::FileLine_curs,line,fd) {
        bool ok = LoadFunc(line);
        if (!ok) {
            algo_lib::_db.errtext << eol << fname << ":" << (ind_curs(line).i+1) << ": " << line << eol;
            retval = false;
            if (strict) {
                break;
            }
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

// Like LoadTuplesFd but load from specified file
bool algo_lib::LoadTuplesFile(strptr fname, bool (*LoadFunc)(strptr), bool strict) {
    algo_lib::FFildes fildes;
    fildes.fd = OpenRead(fname);
    bool ret = LoadTuplesFd(fildes.fd, fname, LoadFunc, strict);
    return ret;
}

// -----------------------------------------------------------------------------

// root = "-" -- read tuples from stdin
// root = filename -- read tuples from filename, return success code
// root = directory -- load ssimfiles specified with SSIMFILES from dataset, return success code
// all tuples are passed to LoadFunc.
// if STRICT is specified, function exits on first error.
// Otherwise, it runs until the end.
// This function is used by amc for process startup
bool algo_lib::DoLoadTuples(strptr root, bool (*LoadFunc)(strptr), const char **ssimfiles, bool strict) {
    bool retval = true;
    if (FileQ(root)) {
        retval = algo_lib::LoadTuplesFile(root, LoadFunc, strict);
    } else if (root == "-") {
        retval = LoadTuplesFd(algo::Fildes(0), "-", LoadFunc, strict);
    } else if (DirectoryQ(root)) {
        for (int i=0; ssimfiles && ssimfiles[i]; i++) {
            if (!algo_lib::LoadTuplesFile(SsimFname(root, ssimfiles[i]), LoadFunc, strict)) {
                retval=false;
                if (strict) {
                    break;
                }
            }
        }
    } else {
        algo_lib::SaveBadTag("path", root);
        algo_lib::SaveBadTag("comment", "Wrong working directory?");
        retval = false;
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Return TRUE if current user is root.
// #AL# is this more correct than checking that effective uid is 0?
bool algo_lib::RootQ() {
    //return geteuid()==0;
    return getenv("USER")==strptr("root");
}

// -----------------------------------------------------------------------------

// Return name of user executing this command.
// If the command is being executed under sudo, return name
// of original user.
tempstr algo_lib::EffectiveUser() {
    tempstr user(getenv("USER"));
    tempstr susr(getenv("SUDO_USER"));
    if (RootQ() && ch_N(susr)) {
        user = susr;
    }
    return user;
}

// -----------------------------------------------------------------------------

// if OWN_FD is cleared, clean up file descriptor before it is closed
void algo_lib::file_Cleanup(algo_lib::InTextFile &file) {
    if (!file.own_fd) {
        file.file.fd = algo::Fildes();
    }
}
