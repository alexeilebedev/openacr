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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/win32.h
//

#ifdef WIN32
extern "C" {
    struct ucontext_t {
        // windows TODO: use GetThreadContext
        // ucp->uc_mcontext.Eip = (unsigned long long) func;
        // ucp->uc_mcontext.Esp = (unsigned long long) (sp - 4);
    };
    struct siginfo_t {
    };
    struct rlimit {
        u64 rlim_cur;
        u64 rlim_max;
    };
    enum SaFlagsEnum {
                      SA_RESTART=1
    };
    enum SignalEnum {
                     SIGBUS=128
                     ,SIGHUP
                     ,SIGPIPE
                     ,SIGALRM
                     ,SIGKILL
    };
    typedef void (*HandlerFunc)(int signal);
    typedef void (*SignalFunc)(int signal, siginfo_t *_si, void *context);
    struct sigaction {
        HandlerFunc sa_handler;
        SignalFunc sa_sigaction;
        u32 sa_mask;
        u32 sa_flags;
    };
    struct dirent {
        char d_name[MAX_PATH];
    };
    struct DIR {
        WIN32_FIND_DATA find_data;
        HANDLE handle;
        int iter;
        bool eof;
        struct dirent dirent;
    };
    typedef HANDLE pthread_t;
    struct pthread_attr_t {
    };
    typedef void* (*ThreadFunc)(void*);
};
int pthread_create(pthread_t *thread, pthread_attr_t *attr, ThreadFunc func, void *arg);
pthread_t pthread_self();
typedef int pid_t;
int fork();
int alarm(int sec);
int pipe(int fd[2]);
int kill(pid_t pid, int signal);
typedef int mode_t;// file mode
int fchmod(int fd, int mode);
pid_t waitpid(pid_t pid, int *status, int options);
int random();
void srandom(int);
struct tm *localtime_r(const time_t *timep, struct tm *result);
struct tm *gmtime_r(const time_t *timep, struct tm *result);
int mkdir(const char *path, int mode);
int mkstemp(char *tmpl);
enum LockFlags {
                LOCK_UN=1
                ,LOCK_NB=2
                ,LOCK_EX=4
};
enum {
      STDIN_FILENO=0
      ,STDOUT_FILENO=1
      ,STDERR_FILENO=2
};
int isatty(int fd);
int flock(int fd, int flags);
void usleep(int microsec);
// no better way to do it...
#define snprintf _snprintf
int setenv(const char *name, const char *value, int overwrite);
const char *strsignal(int status);
int lstat(const char *path, StatStruct *buf);
int stat(const char *path, StatStruct *s);
int fstat(int fd, StatStruct *s);
int WIFSIGNALED(int status);
int WIFEXITED(int status);
int WEXITSTATUS(int status);
int WTERMSIG(int status);
int WIFSTOPPED(int status);
int WCOREDUMP(int status);
int WSTOPSIG(int status);
enum {
      WNOHANG=1
};
inline int S_ISDIR(int flags) {
    return flags & _S_IFDIR;
}
inline int S_ISREG(int flags) {
    return flags & _S_IFREG;
}
enum {
      S_IXUSR=1
      ,S_IRWXG=2
      ,S_IXOTH=16
};
enum {
      FNM_FILE_NAME
};
int fnmatch(const char *pattern, const char *filename, int flags);
void sleep(int sec);
enum RlimitEnum {
                 RLIMIT_CPU=1
                 ,RLIMIT_NOFILE
                 ,RLIMIT_DATA
                 ,RLIMIT_AS // address space
};
void sigemptyset(u32 *mask);
enum {
      SA_SIGINFO=1
};
int sigaction(int signal, struct sigaction *sa, void*);
enum {
      S_IWGRP=1
      ,S_IWOTH=1
};
int getrlimit(RlimitEnum what, rlimit *r);
int setrlimit(RlimitEnum what, const rlimit *r);
int symlink(const char *, const char *);
int link(const char *, const char *);
char *strerror_r(int code, char *buf, int bufsize);
int getdomainname(char *buf, int bufsize);
struct dirent *readdir(DIR *dir);
DIR *opendir(const char *path);
void closedir(DIR *dir);
void gettimeofday(struct timeval *, struct timezone*);
int ftruncate(int fd, u64 size);
void *sync_lock_test_and_set(void **addr, void *val);
void *sync_val_compare_and_swap(void **addr, void *oldval, void *newval);
template<class T> inline T *__sync_lock_test_and_set(T **addr, void *val) {
    return (T*)sync_lock_test_and_set((void**)addr,val);
}
template<class T> inline T *__sync_val_compare_and_swap(T **addr, T *oldval, void *newval) {
    return (T*)sync_val_compare_and_swap((void**)addr,oldval,newval);
}
// returns pid of child process or -1
// on error, errno is set to the error code
int dospawn(char *path, char *cmdline, int timeout, strptr fstdin, strptr fstdout, strptr fstderr);
int readlink(const char *path, char *out, int bufsize);
enum {
      _SC_NPROCESSORS_ONLN=1
};
int sysconf(int selector);
// Close specified handle if it open
// Set value to NULL
void CloseHandleSafe(HANDLE &handle);
enum MmapProt {
               PROT_READ=1
};
enum MmapFlags {
                MAP_PRIVATE=1
};
void *mmap(void *base_addr, i64 size, MmapProt prot, MmapFlags mapflags, int fd, int flags2);
int munmap(void *base_addr, i64 size);
#endif
