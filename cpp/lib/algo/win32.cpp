// Copyright (C) 2020-2021 Astra
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/win32.cpp
//
// This file contains functions for the Windows platform (compiler:cl)
// that are needed by other functions in algo_lib etc.
// It is a compatibility layer
// It is work-in-progress and not finished yet.
// Any help is appreciated.

#include "include/algo.h"
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

#ifdef WIN32

#include <winbase.h>
#include <shlwapi.h>
enum {
    SIGNAL_EXIT_CODE_BASE=10000
};

// WIN_CODE: windows API call exit function
// RETURN: unix-style exit code
// If the windows call returned a positive value, return 0.
// Otherwise, return -1.
// In all cases, copy GetLastError() to errno.
static int WinExitCode(int win_code) {
    errno=GetLastError();
    return win_code ? 0 : -1;
}

// Same as _get_osfhandle but without crash on -1 fd
static HANDLE _FdToHandle(int fd) {
    return fd >= 0 ? (HANDLE)_get_osfhandle(fd) : INVALID_HANDLE_VALUE;
}

int fork() {
    // fork cannot be implemented reliably on windows
    // in addition, fork is bad.
    // https://www.microsoft.com/en-us/research/uploads/prod/2019/04/fork-hotos19.pdf
    // use posix_spawn instead
    prerr("fork(): not implemented\n");
    return -1;
}

int alarm(int sec) {
    (void)sec;
    // tentative implementation: start new thread, have the thread
    // sleep SEC seconds, then exit process
    // for now, fail it
    return -1;
}

// redirect to MS implementation
int pipe(int fd[2]) {
    return _pipe(fd,8192,_O_BINARY);
}

int kill(pid_t pid, int signal) {
    // todo: convert signal to exit code in some better way?
    int exit_code=SIGNAL_EXIT_CODE_BASE + signal;
    int rc = 0;
    if (pid == getpid()) {
        ExitProcess(exit_code);
        // unreachable
    } else {
        HANDLE h = OpenProcess(PROCESS_TERMINATE,0,pid);
        rc = WinExitCode(TerminateProcess(h, exit_code));
        CloseHandle(h);
    }
    return rc;
}

// not too important
int fchmod(int fd, int mode) {
    (void)fd;
    (void)mode;
    return 0;
}

pid_t waitpid(pid_t pid, int *status, int options) {
    bool exited=false;
    if (pid > 0) {
        HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,0,pid);
        if (h == INVALID_HANDLE_VALUE) {
        } else {
            int timeout = (options & WNOHANG) ? 0 : INFINITE;
            exited = WaitForSingleObject( h, timeout ) == WAIT_OBJECT_0;
            DWORD exit_code=0;
            if (exited) {
                GetExitCodeProcess( h, &exit_code );
                *status = exit_code;
            }
            CloseHandle(h);
        }
    }
    return exited ? pid : -1;
}

void srandom(int val) {
    srand(val);
}

int random() {
    return rand();
}

struct tm *localtime_r(const time_t *timep, struct tm *result) {
    (void)_localtime64_s(result, timep);
    return result;
}

// convert UTC time TIMEP to Greenwich Meridian time struct RESULT
struct tm *gmtime_r(const time_t *timep, struct tm *result) {
    (void)_gmtime64_s(result, timep);
    return result;
}

int mkdir(const char *path, int mode) {
    (void)mode;
    return _mkdir(path);
}

int mkstemp(char *tmpl) {
    int len=(int)strlen(tmpl);
    int ret=-1;
    if (len>=6) {
        for (int i=len-1; i>=0 && tmpl[i]=='X'; i--) {
            // need a truly random source to avoid
            // N^2 behavior for temp files
            u64 rnd=algo::get_cycles();
            tmpl[i]="0123456789abcdefghijklmnopqrstuvwxyz"[rnd%36];
        }
    }
    ret = algo::OpenWrite(tmpl,algo_FileFlags_temp).value;
    return ret;
}

int isatty(int fd) {
    (void)fd;
    return 0;
}

// -----------------------------------------------------------------------------

int flock(int fd, int flags) {
    HANDLE h = _FdToHandle(fd);
    int rc=0;
    if (flags & LOCK_EX) {
        // is there a blocking version of LockFile?
        do  {
            rc=WinExitCode(LockFile(h, 0,0,1,0));
        } while ((flags & LOCK_NB) ? false : (rc!=0));
    }
    if (flags & LOCK_UN) {
        rc=WinExitCode(UnlockFile(h, 0,0,1,0));
    }
    return rc;
}

void usleep(int microsec) {
    // convert to rough sleep
    Sleep(microsec/10000);
}

void sleep(int sec) {
    // windows Sleep uses milliseconds
    Sleep(sec*1000);
}

int setenv(const char *name, const char *value, int overwrite) {
    (void)overwrite;
    _putenv_s(name,value);
    return 0;
}

const char *strsignal(int sig) {
    (void)sig;
    return "";
}

int WIFSIGNALED(int status) {
    return status > SIGNAL_EXIT_CODE_BASE;
}

int WCOREDUMP(int status) {
    (void)status;
    return 0;
}

int WIFEXITED(int status) {
    (void)status;
    return 0;
}

int WEXITSTATUS(int status) {
    return status;
}

int WTERMSIG(int status) {
    return status > SIGNAL_EXIT_CODE_BASE ? status-SIGNAL_EXIT_CODE_BASE : 0;
}

int WIFSTOPPED(int status) {
    (void)status;
    return 0;
}

int WSTOPSIG(int status) {
    (void)status;
    return 0;
}

int getrlimit(RlimitEnum what, rlimit *r) {
    (void)what;
    (void)r;
    errno = EINVAL;
    return -1;
}

int setrlimit(RlimitEnum what, const rlimit *r) {
    (void)what;
    (void)r;
    errno = EINVAL;
    return -1;
}

// Create linux-type soft link
// Return 0 if success
int symlink(const char *target, const char *path){
    tempstr cmd;
    cmd<<"/usr/bin/ln -sf ";
    strptr_PrintBash(target,cmd);
    cmd<<" ";
    strptr_PrintBash(path,cmd);
    command::bash_proc bash;
    bash.cmd.c = cmd;
    return bash_Exec(bash);
}

int link(const char *target, const char *path) {
    return WinExitCode(CreateHardLinkA(Zeroterm(algo::ToWindowsPath(path))
                                       ,Zeroterm(algo::ToWindowsPath(target)),0));
}

void sigemptyset(u32 *mask) {
    *mask=0;
}

// no sigaciton on windows
int sigaction(int signal, struct sigaction *sa, void*) {
    (void)signal;
    (void)sa;
    return -1;
}

char *strerror_r(int code, char *buf, int bufsize) {
    tempstr out;
    out << code;
    strncpy(buf, Zeroterm(out), bufsize);
    return buf;
}

int getdomainname(char *buf, int bufsize) {
    strncpy(buf,"",bufsize);
    return 0;
}

struct dirent *readdir(DIR *dir) {
    if (dir->iter > 0) {
        dir->eof = FindNextFile(dir->handle,&dir->find_data)==0;
    }
    strncpy(dir->dirent.d_name, dir->find_data.cFileName, sizeof(dir->dirent.d_name));
    dir->iter++;
    return dir->eof ? NULL : &dir->dirent;
}

DIR *opendir(const char *path) {
    DIR *dir=new DIR;
    tempstr _path(path);
    // WIN32: FindFirstFile argument must not end with a directory separator
    if (EndsWithQ(_path,"/")) {
        _path << "*";
    }
    dir->handle = FindFirstFile(Zeroterm(_path),&dir->find_data);
    dir->iter=0;
    dir->eof=dir->handle == INVALID_HANDLE_VALUE;
    return dir;
}

void closedir(DIR *dir) {
    if (dir) {
        FindClose(dir->handle);
    }
    delete dir;
}

int fnmatch(const char *pattern, const char *filename, int flags) {
    (void)flags;
    algo_lib::Regx reg;
    Regx_ReadShell(reg,pattern,true);
    bool ret = Regx_Match(reg,strptr(filename));
    enum {
        FNM_NOMATCH=1
    };
    return ret ? 0 : FNM_NOMATCH;
}

// Windows doesn't support soft links.
// So implement lstat via stat
int lstat(const char *path, StatStruct *buf) {
    return _stat64(path,buf);
}

int stat(const char *path, StatStruct *s) {
    return _stat64(path,s);
}

int fstat(int fd, StatStruct *s) {
    return _fstat64(fd,s);
}

void gettimeofday(struct timeval *tv, struct timezone *) {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    u64 val = (u64(ft.dwHighDateTime)<<32) | ft.dwLowDateTime;
    u64 ret = val - 0x019DB1DED53E8000;
    u32 nanotime_per_sec = 10000000;
    tv->tv_sec = ret / nanotime_per_sec;
    tv->tv_usec = ret % nanotime_per_sec;
}

int ftruncate(int fd, u64 size) {
    HANDLE h = _FdToHandle(fd);
    LONG lo = u64(size) & ~0;
    LONG hi = u64(size) >> 32;
    return SetFilePointer(h,lo,&hi,FILE_BEGIN) != INVALID_SET_FILE_POINTER
        && SetEndOfFile(h) != 0
        ? 0 : -1;
}

// https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
void *sync_lock_test_and_set(void **addr, void *val) {
    lfence();
    void *ret=*addr;
    *addr=val;
    return ret;
}

// https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
void *sync_val_compare_and_swap(void **addr, void *oldval, void *newval) {
    return InterlockedCompareExchangePointer(addr, newval, oldval);
}

static HANDLE WinCreateRedirect(strptr redirect, HANDLE dflt) {
    HANDLE ret = dflt;
    if (redirect != "") {
        int fd = algo_lib::CreateRedirect(redirect);
        ret = _FdToHandle(fd);
    }
    if (ret) {
        int rc=SetHandleInformation(ret, HANDLE_FLAG_INHERIT, 1);
        if (!rc) {
            verblog("failed to make handle inheritable");
        }
    }
    return ret;
}

// Close specified handle if it open
// Set value to NULL
void CloseHandleSafe(HANDLE &handle) {
    if (handle != INVALID_HANDLE_VALUE && handle!= NULL) {
        (void)CloseHandle(handle);
    }
    handle = NULL;
}

// -----------------------------------------------------------------------------

static HANDLE GetOrCreateParentJob() {
    if (!algo_lib::_db.winjob) {
        // create main job object. this is needed so that child processes
        // are killed automatically when the parent dies.
        // breakaways mut be enabled so that the process works on windows<7 where
        // nested job objects are not allowed.
        HANDLE job = CreateJobObject(NULL,NULL);
        if (job==INVALID_HANDLE_VALUE) {
            prerr("can't create job object");
        }
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION info = {};
        info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE| JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK;
        int sij_ok=SetInformationJobObject(job,JobObjectExtendedLimitInformation,&info, sizeof(info));
        if (!sij_ok) {
            prerr("assign child process to job: "<<int(GetLastError()));
        }
        algo_lib::_db.winjob=u64(job);
    }
    return (HANDLE)algo_lib::_db.winjob;
}

// -----------------------------------------------------------------------------

int dospawn(char *path, char *cmdline, int timeout, strptr fstdin, strptr fstdout, strptr fstderr) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    algo::ZeroBytes(si);
    algo::ZeroBytes(pi);
    HANDLE h_in = (HANDLE)_get_osfhandle(0);// existing stdout handle - do not close
    HANDLE h_out = (HANDLE)_get_osfhandle(1);// existing stdout handle - do not close
    HANDLE h_err = (HANDLE)_get_osfhandle(2);
    // child process should inherit stdin/stdout/stderr (if not overridden by a redirect)
    si.hStdInput  = WinCreateRedirect(fstdin,h_in);
    si.hStdOutput = WinCreateRedirect(fstdout,h_out);
    si.hStdError  = WinCreateRedirect(fstderr,h_err);
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.cb = sizeof(si);
    SECURITY_ATTRIBUTES *pattr = NULL;
    SECURITY_ATTRIBUTES *tattr = NULL;
    bool inherit = true;// inherit handles -- in case of redirects
    int flags = CREATE_SUSPENDED;
    void *env_block = NULL;
    char *curdir = NULL;
    // todo: apply timeout
    (void)timeout;
    // expand ..'s and CYGWIN-style soft links in the path
    tempstr cmdpath(algo::GetFullPath(path));
    // add .exe if needed (todo: .cmd, .bat??)
    algo_lib::TryExeSuffix(cmdpath);
    // convert to windows path
    char buf[MAX_PATH];
    cmdpath = strptr(_fullpath(buf,Zeroterm(cmdpath),sizeof(buf)));
    bool ok = 0!=CreateProcess(Zeroterm(cmdpath),cmdline,pattr,tattr,inherit,flags,env_block,curdir,&si,&pi);
    int retval=-1;
    if (ok) {
        HANDLE job = GetOrCreateParentJob();
        AssignProcessToJobObject(job, pi.hProcess);
        if (si.hStdInput != h_in) {
            CloseHandleSafe(si.hStdInput);
        }
        if (si.hStdOutput != h_out) {
            CloseHandleSafe(si.hStdOutput);
        }
        if (si.hStdError != h_err) {
            CloseHandleSafe(si.hStdError);
        }
        retval = GetProcessId(pi.hProcess);
        ResumeThread(pi.hThread);
        CloseHandleSafe(pi.hThread);
        CloseHandleSafe(pi.hProcess);
    } else {
        errno = GetLastError();
    }
    if (algo_lib::_db.cmdline.verbose) {
        verblog("dospawn"
                <<Keyval("path",path)
                <<Keyval("realpath",cmdpath)
                <<Keyval("cmdline",cmdline)
                <<Keyval("timeout",timeout)
                <<Keyval("fstdin",fstdin)
                <<Keyval("fstdout",fstdout)
                <<Keyval("fstderr",fstderr)
                <<Keyval("stdin_handle",u64(si.hStdInput))
                <<Keyval("stdout_handle",u64(si.hStdOutput))
                <<Keyval("stderr_handle",u64(si.hStdError))
                <<Keyval("ok",int(ok))
                <<Keyval("lasterror",int(GetLastError()))
                <<Keyval("retval",int(retval))
                );
    }
    return retval;
}

// -----------------------------------------------------------------------------

int readlink(const char *path, char *out, int bufsize) {
    algo::Fildes fd(algo::OpenRead(path,algo::FileFlags()));
    int nout=0;
    if (ValidQ(fd)) {
        u8 tempbuf[MAX_PATH];
        ssize_t nread = read(fd.value,tempbuf,sizeof(tempbuf));
        if (nread > 12 && memcmp(tempbuf,"!<symlink>",10)==0) {
            for (int i=12; i<nread && tempbuf[i]!=0; i+=2) {
                if (nout<bufsize-1) {
                    out[nout++]=tempbuf[i];
                }
            }
        }
        close(fd.value);
    }
    if (nout < bufsize) {
        out[nout]=0;
    }
    return nout;
}

int sysconf(int selector) {
    int rc=-1;
    switch (selector) {
    case _SC_NPROCESSORS_ONLN: {
        SYSTEM_INFO info;
        algo::ZeroBytes(info);
        GetSystemInfo(&info);
        rc= info.dwNumberOfProcessors;
    }break;
    default: break;
    }
    return rc;
}

void *mmap(void *base_addr, i64 size, MmapProt prot, MmapFlags mapflags, int fd, int flags2) {
    (void)mapflags;// MAP_PRIVATE
    (void)flags2;
    (void)base_addr;
    void *ret=NULL;
    if (base_addr==NULL) {
        HANDLE hfile= _FdToHandle(fd);
        int flProtect = 0;
        if (prot & PROT_READ) flProtect |= PAGE_READONLY;
        // leave remainder of flags unimplemented
        HANDLE mapping = CreateFileMapping(hfile,NULL,flProtect,0,0,NULL);
        if (mapping != INVALID_HANDLE_VALUE) {
            int access=0;
            if (prot & PROT_READ) access = FILE_MAP_READ;
            ret = MapViewOfFile(mapping,access,0,0,i64(size));
            CloseHandle(mapping);
        }
    } else {
        // don't support base address specification
        errno=-1;
    }
    return ret;
}

int munmap(void *base_addr, i64 size) {
    (void)size;
    int ret = WinExitCode(UnmapViewOfFile(base_addr));
    return ret;
}

// -----------------------------------------------------------------------------

typedef unsigned long (*WinThreadFunc)(void*);

int pthread_create(pthread_t *thread, pthread_attr_t *attr, ThreadFunc func, void *arg) {
    (void)attr;
    HANDLE h = CreateThread(NULL,8192/*staack*/,WinThreadFunc(func),arg,0,NULL);
    *thread = pthread_t(h);
    return h==INVALID_HANDLE_VALUE ? -1 : 0;
}

pthread_t pthread_self() {
    return pthread_t(GetCurrentThreadId());
}

#endif
