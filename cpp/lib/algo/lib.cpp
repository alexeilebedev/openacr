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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/lib.cpp -- Main file
//

#include "include/algo.h"

#if !defined(WIN32)
#include <arpa/inet.h>// inet_pton
#include <sys/mman.h>// mmap,mlockall
#endif

#if defined(__linux__)
#include <sys/prctl.h>
#endif

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
            bash.fstdout  << ">&" << pipefd[1];
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
    tempstr str;
    str << text;
    if (code_Get(err)!=0) {
        str << ": "
            <<Keyval("errno", code_Get(err))
            <<Keyval("errtext", err);
    }
    throw algo_lib::ErrorX(str);
}

void algo::Throw() NORETURN {
    Throw("", Errcode());
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
        IohookRemove(iohook);
    }
    if (ValidQ(iohook.fildes) && !iohook.nodelete) {
        close(iohook.fildes.value);
    }
    iohook.fildes=algo::Fildes();// strip
}

// -----------------------------------------------------------------------------

// Call sleep/usleep, giving up MS milliseconds
void algo::SleepMsec(int ms) {
#ifdef WIN32
    Sleep(ms);
#else
    if (ms>=1000) {
        sleep(ms/1000);
        ms -= (ms/1000)*1000;
    }
    if (ms>0) {
        usleep(ms*1000);
    }
#endif
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
#if defined(AOS_SSE42) && !defined(WIN32)
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
#if defined(WIN32) || defined(__CYGWIN__)
    // not supported
    return false;
#else
    return mlockall(MCL_CURRENT | MCL_FUTURE)==0;
#endif
}

// -----------------------------------------------------------------------------

static void SignalHandler(int sig) {
    algo_lib::_db.last_signal = sig;
    i64 t_now = algo::get_cycles();
    // 2 interrupts within 2 second -- die.
    // otherwise the process becomes unkillable
    i64 thresh = algo_lib::_db.t_last_signal + algo::get_cpu_hz_int()*2;
    if (thresh > t_now) {
        kill(getpid(), SIGKILL);
    }
    algo_lib::_db.t_last_signal.value = t_now;
    algo_lib::_db.limit = algo_lib::_db.clock; // interrupt
}

void algo::SetupExitSignals(bool sigint) {
    struct sigaction sigact;
    sigact.sa_handler = SignalHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    (void)sigaction( SIGHUP, &sigact, 0);
    if (sigint) {
        (void)sigaction( SIGINT, &sigact, 0);
    }
    (void)sigaction(SIGTERM, &sigact, 0);
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
    if (ret == 0) {
        str.ch_n = u32(strlen(str.ch_elems));
    }
    return str;
}

const tempstr algo::GetDomainname() {
    tempstr str;
    ch_Reserve(str,128);
    int ret = getdomainname(str.ch_elems,str.ch_max);
    if (ret == 0) {
        str.ch_n = u32(strlen(str.ch_elems));
    }
    return str;
}

// Die when parent process dies
void algo_lib::DieWithParent() {
#if defined(__MACH__) || __FreeBSD__>0 || defined(__CYGWIN__) || defined(WIN32)
    // sadly, there is no way to make a child exit
    // when a parent dies, so structured concurrency
    // cannot be implemented
    // Even if we use kqueue to monitor for parent process exit,
    //  this works only for processes that
    // a) do not block, i.e. process events frequently, and
    // b) for processes which are implemented with algo_lib.
    //  Anything exec()'ed on top won't work.
#else
    prctl(PR_SET_PDEATHSIG, SIGKILL);
#endif
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

// Interpret redirect string, return resulting fd
// If no redirect applies, return -1
// If a valid fd is returned, it is unique and may be closd with close()
// Supported redirects:
// >filename  -- on exit, dst_fd is writing to a file
// <filename  -- on exit, dst_fd is reading from a file
// >>filename -- on exit, dst_fd is appending to a file
// >&fd       -- on exit, dst_fd is pointing to fd
// <&fd       -- on exit, dst_fd is pointing to fd
// This function could be called openex
int algo_lib::CreateRedirect(strptr redirect) {
    algo::StringIter iter(Trimmed(redirect));
    algo::FileFlags flags;
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
    // return existing fd
    if (try_dup && SkipChar(iter,'&')) {// >&, <&, >>& forms
        src_fd = dup(ParseI32(iter,-1));
    }
    // open file with flags
    if (src_fd == -1 && !iter.EofQ()) {// try filename
        tempstr fname;
        fname << Trimmed(iter.Rest());
        src_fd = algo::OpenFile(Zeroterm(fname), flags).value;
    }
    return src_fd;
}

// -----------------------------------------------------------------------------

// Interpret redirect string and make DST_FD consistent with
// the intended state. Return 0 on success, -1 on failure
// This function is usually called in the child process right after fork
// See CreateRedirect for interpretation of redirect string
int algo_lib::ApplyRedirect(strptr redirect, int dst_fd) {
    int rc=0;
    if (redirect != "") {
        int src_fd = CreateRedirect(redirect);
        if (src_fd < 0) {
            rc=-1;
        }
        if (src_fd >=0 && src_fd != dst_fd) {
            (void)dup2(src_fd,dst_fd);
            (void)close(src_fd);
        }
    }
    return rc;
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
        algo::StringIter iter(subnet_mask);
        u32 mask_len = 0;
        valid  = ch_N(subnet_mask) > 0 && TryParseU32(iter, mask_len);
        // 0.0.0.0/0 is valid
        valid  = valid && ((ip.ipv4 == 0 && mask_len == 0) || (mask_len > 0 && mask_len <=32));
        // /32 is valid for any addr
        valid  = valid && (mask_len == 32 || (ip.ipv4 << mask_len == 0));
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

// -----------------------------------------------------------------------------

// Walk child process tree for parent process pid, in post-order traversal way,
// and send signal sig to each process. Kill_topmost is an option whether
// to send signal to parent process itself. Return value - number of processes
// to whose the signal has been actually sent.
// Does not throw exceptions, just prints error message if kill() fails.
// Linux only.
int algo_lib::KillRecurse(int pid, int sig, bool kill_topmost) {
    int ret(0);
#ifdef __linux__
    if (pid>0) {
        ind_beg(algo::Dir_curs,T,tempstr()<<"/proc/"<<pid<<"/task/*") {
            cstring ch_file(algo::DirFileJoin(T.pathname,"children"));
            cstring ch(algo::FileToString(ch_file,algo::FileFlags()));
            algo::StringIter iter(ch);
            i64 ch_pid;
            while (TryParseI64(iter, ch_pid)) {
                ret += KillRecurse(ch_pid,sig,true);
            }
        }ind_end;
        if (kill_topmost) {
            int err = kill(pid,sig)==0 ? 0 : errno;
            if (err && err != ESRCH) {
                prerr("kill "<<pid<<" "<<sig<<": "<<strerror(err));
            }
            ret += err ? 0 : 1;
        }
    }
#else
    (void)pid;
    (void)sig;
    (void)kill_topmost;
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// Return computed name for sandbox SANDBOX
tempstr algo_lib::SandboxDir(algo::strptr sandbox) {
    tempstr ret;
    if (sandbox != "") {
        ret << "temp/sandbox." << sandbox;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Enter sandbox and remember previous directory
void algo_lib::SandboxEnter(algo::strptr sandbox) {
    algo_lib::_db.sandbox_orig_dir=algo::GetCurDir();
    verblog("sandbox.begin"
            <<Keyval("dir",sandbox));
    errno_vrfy(chdir(Zeroterm(algo_lib::SandboxDir(sandbox)))==0, "chdir sandbox");
}

// -----------------------------------------------------------------------------

// Change to the directory that was current before sandbox mode
// Must be balanced with SandboxEnter
void algo_lib::SandboxExit() {
    verblog("sandbox.end");
    errno_vrfy(chdir(Zeroterm(algo_lib::_db.sandbox_orig_dir))==0, "chdir");
}
