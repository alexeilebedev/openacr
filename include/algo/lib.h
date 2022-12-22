// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Header: include/algo/lib.h
//
// Created By: alexei.lebedev shreejith.lokegowda
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev shreejith.lokegowda luke.huang
//
// This file has 3 sections:
// - manual function declarations for namespace algo (not doable with amc or src_func)
// - auto extracted prototypes for namespace algo
// - auto extracted prototypes for namespace algo_lib

namespace algo {
    // Refurbish: destroy and re-create object in place.
    // This function is exception-safe: if ~T throws an exception, T::T still gets
    // called.
    // Implementation note: explicit calls to destructor are checked for NULL pointer
    // by GCC, so the nonnull attribute is required.
    template<class T> inline void Refurbish(T &t) F_NONNULL;

    // Set all bytes to zero.
    template<class T> inline void ZeroBytes(T &t);

    // Shift pointer and cast.
    template<class T, class U> inline T      PtrAdd(U *ptr, int_ptr offset);

    algo::Errcode FromErrno(i64 val);
    algo::Errcode FromWinErr(i64 val);

    struct PageBuf : memptr {
        PageBuf(){}
        ~PageBuf();
    private:
        PageBuf(const PageBuf&);
        void operator =(const PageBuf&){}
    };

    struct RegxMacro {
        char c;
        const char *crange;
    };

    // -----------------------------------------------------------------------------

    template<class T>
    struct Renew {
        T &t;
        Renew(T &in_t) : t(in_t){
        }
        ~Renew() {
            new (&t)T;
        }
    };

    template<class T> inline void Refurbish(T &t) {
        Renew<T> r(t);
        t.~T();
    }

    // -----------------------------------------------------------------------------
    // Tuple functions
    // Search for matching attribute by name and occurence number. If found, return pointer to it.
    // If name is empty, this searches for an anonymous attribute.
    Attr     *attr_Find(Tuple &tuple, strptr name, int occurence = 0);
    // Find attribute by name and return value.
    // If attribute not found, return DFLT.
    strptr  attr_GetString(Tuple &tuple, strptr name, strptr dflt=strptr());

    // Compare tuples t1 & t2, return true if they are equal.
    bool          Tuple_EqualQ(Tuple &t1, Tuple &t2, bool ignore_comment=false);

    // See strconv.h for functions Tuple_ReadStrptr,Tuple_Print,PrintAttr,PrintAttrSpace.
    // (appending name-value pairs to string)
    // Execute command and return output.
    // MAX_OUTPUT   throw exception if command output exceeds this size.
    // CMD_FAIL_OK  if set, throw exception if can't execute command
    //              or if command returns false.
    tempstr SysEval(strptr cmd_, FailokQ fail_ok, int max_output, bool echo = false);
    // Execute command using system().
    // fail_ok: if true, command must succeed or throw exception
    // dry_run: if true, do not run command, just print
    // echo: print command
    int SysCmd(strptr cmd, FailokQ fail_ok=FailokQ(true), DryrunQ dry_run=DryrunQ(false), EchoQ echo=algo_EchoQ_false);

    // -----------------------------------------------------------------------------
    // Cursor support for directory scanning:
    // ind_beg(Dir_curs,entry,"*.gcov") {
    // }ind_end;
    //
    // ind_beg(Dir_curs,entry,".atf_ci/covbuild/*.xml.errors") {
    // }ind_end;
    struct Dir_curs {
        typedef DirEntry ChildType;
        DirEntry E;
    };

    // -----------------------------------------------------------------------------
    // Cursor for reading a text file line by line
    // ind_beg(FileLine_curs,line,filename") {
    // }ind_end;
    // Access current line number with ind_curs(line).i+1
    struct FileLine_curs {
        typedef strptr ChildType;
        algo_lib::InTextFile file;// current file
        strptr line;// current line
        int i;// current line number - 1 (for compatibility with array iteration)
        Bool eof;
    };

    // THREAD_ID        target thread
    // CORE_ID          cpu number
    // LOCK             if set, create a CoreLock. the core lock
    //                  is released when thread exits. There is no way
    //                  to kill it early.
    void MoveToCore(pthread_t thread_id, int coreID, bool lock = true);
    void SetupExitSignals(bool sigint=true);
    bool SaneTerminalQ();
}

// -----------------------------------------------------------------------------

namespace algo { // update-hdr srcfile:'(%/algo/lib.%|include/algo.inl.h|include/algo/aryptr.inl.h)'
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/lib.cpp -- Main file
    //

    // Execute unix command and return output.
    tempstr SysEval(strptr cmd, FailokQ fail_ok, int max_output, bool echo);

    // Execute unix command and return status code.
    // Exception is thrown if FAIL_OK is false, and subprocess does not exit normally.
    int SysCmd(strptr cmd, FailokQ fail_ok, DryrunQ dry_run, EchoQ echo);

    // Return a human-readable description of STATUS as returned by wait() / waitpid()
    tempstr DescribeWaitStatus(int status);

    // The several variants of Throw are all the same -- the goal is to get as many
    // setup instructions out of the execution path as possible, to avoid polluting instruction
    // cache.
    void Throw(strptr text, Errcode err) __attribute__((noreturn));
    void Throw() __attribute__((noreturn));
    bool Tuple_EqualQ(Tuple &t1, Tuple &t2, bool ignore_comment);
    Attr* attr_Find(Tuple &tuple, strptr name, int occurence);
    strptr attr_GetString(Tuple &T, strptr name, strptr dflt);

    // Append new attribute with specified name and value.
    void attr_Add(Tuple &T, strptr name, strptr value);

    // Call sleep/usleep, giving up MS milliseconds
    void SleepMsec(int ms);
    Errcode FromErrno(i64 val);
    Errcode FromWinErr(i64 val);
    i64 I64Power10(u32 power);

    // Copy bytes from one location to another in blocks of 16 bytes.
    void StreamFromToB16(void *from, void *to, u32 n);
    void CopyFromToB16(void *from, void *to, u32 n);

    // SSE implementation of fast exponentiation idea from
    // http//nic.linotune.com/pubs/Schraudolph99.pdf
    // ExpFast(x) goes bad at x < -709 (exp doesn't)
    // Where it works, ExpFast makes a steady relative error of about 2%.
    double ExpFast(double y);

    // lock all presently, and future allocated bytes in physical memory.
    // Return success value. Errno provides more info.
    bool LockAllMemory();
    void SetupExitSignals(bool sigint);
    const tempstr GetHostname();
    const tempstr GetDomainname();
    double double_WeakRandom(double scale);
    i32 i32_WeakRandom(i32 modulo);

    // -------------------------------------------------------------------
    // include/algo.inl.h -- Inline functions
    //

    // Taylor series expansion. For x around 0.01, this produces 2 digits per
    // iteration, so with N=4 will be reasonably fast.
    inline double ExpTaylor(double x, int n);
    inline u32 ParseNum8(u64 x, u32 &ok);
    inline u32 ParseNum4(u32 x, u32 &ok);
    inline u32 aParseNum8(strptr s, u32 &ok);
    inline u32 aParseNum4(strptr s, u32 &ok);
    inline int ParseOct1(u32 c, u8 &result);

    // C           single hex character, upper or lowercase
    // RETURN      value of hex character
    // RESULT      value of hex digit
    // RETURN      success code
    inline int ParseHex1(u32 c, u8 &result);
    inline void PageBufInit(PageBuf &F, u64 n, u64 align);
    inline u32 strptr_Hash(u32 prev, algo::strptr val);
    inline u32 cstring_Hash(u32 prev, const algo::strptr &val);
    inline u32 cstring_Hash(u32 prev, algo::cstring &val);
    inline u32 cstring_Hash(u32 prev, const algo::cstring &val);

    // this is inefficient because it would be nice to be able to overrun strings.
    // however, we don't check byte alignment at the beginning,
    // so a 2-byte string that lies 4 bytes before the end of a page could fault.
    inline u32 CRC32Step(u32 old, const u8 *x, size_t len);
    inline int P1Mod(int a, int b);
    inline u32 u32_Count1s(u32 x);
    inline u32 u8_Count1s(u8 x);
    inline u32 u16_Count1s(u16 x);
    inline u32 u64_Count1s(u64 x);
    inline u32 u128_Count1s(u128 x);
    template<class T> inline void ZeroBytes(T &t);
    template<class T, class U> inline T PtrAdd(U *ptr, int_ptr offset);
    template<class T> inline void TSwap(T &a, T &b);

    // Default, and invalid, value for Fildes is -1. 0 is a valid value (stdin)!
    inline bool ValidQ(Fildes fd);
    inline int i32_NegateIf(int x, bool i);
    inline i64 i64_NegateIf(i64 x, bool i);
    inline float float_NegateIf(float x, bool i);
    inline double double_NegateIf(double x, bool i);
    inline double DRound(double a);
    inline double DCeiling(double a);
    inline double DFloor(double a);
    inline double DTrunc(double a);

    // Clipping to an open interval (exclisive upper limit) [0..hi-1] or [lo..hi-1]
    inline int Clipped(int a, int hi);
    inline int Clipped(int a, int lo, int hi);

    // Clipping to a closed interval lo..hi
    inline double Clippedf(double a, double lo, double hi);

    // unsigned subtract with clipping
    inline u64 u64_SubClip( u64 a, u64 b);
    inline u32 u32_SubClip( u32 a, u32 b);
    inline u16 u16_SubClip( u16 a, u16 b);
    inline int Abs(i8  a);
    inline int Abs(i16 a);
    inline i32 Abs(i32 a);
    inline i64 Abs(i64 a);
    inline f32 Abs(f32 a);
    inline f64 Abs(f64 a);
    inline f80 Abs(f80 a);

    // Read big-endian word
    inline u16 ReadBE16(const void *val);
    inline u32 ReadBE32(const void *val);
    inline u64 ReadBE64(const void *val);

    // Read little-endian word
    inline u16 ReadLE16(const void *val);
    inline u32 ReadLE32(const void *val);
    inline u64 ReadLE64(const void *val);

    // Forward:  returns 0-based index of least significant bit that is set
    // Reverse:  returns 0-based index of most  significant bit that is set.
    // input argument must not be zero.
    // input result in 0 is undefined (see Intel manual)
    // http://www.intel.com/content/dam/doc/manual/64-ia-32-architectures-software-developer-vol-2a-2b-instruction-set-a-z-manual.pdf
    inline u32 u32_BitScanForward(u32 v);
    inline u64 u64_BitScanForward(u64 v);
    inline u32 u32_BitScanReverse(u32 v);
    inline u64 u64_BitScanReverse(u64 v);
    inline u32 u16_BitScanForward(u16 v);
    inline u32 u16_BitScanReverse(u16 v);
    inline u32 u8_BitScanForward(u8 v);
    inline u32 u8_BitScanReverse(u8 v);
    inline u32  CeilingLog2(u32 orig);
    inline u64  CeilingLog2(u64 orig);
    inline u32  FloorLog2(u32 i);
    inline u64  FloorLog2(u64 i);
    inline u32 BumpToPow2(u32 i);
    inline u64 BumpToPow2(u64 i);
    inline u64   u64_CeilPow2(u64 a, u64 b);
    inline u32   u32_CeilPow2(u32 a, u32 b);
    inline u64    u64_ModPow2(u64 a, u64 b);
    inline u32    u32_ModPow2(u32 a, u32 b);
    inline bool     u64_Pow2Q(u64 i);
    inline bool     u32_Pow2Q(u32 i);

    // -------------------------------------------------------------------
    // include/algo/aryptr.inl.h
    //
    template<class T> inline int elems_N(const aryptr<T> &ary);
    template<class T> inline int ch_N(const aryptr<T> &ary);
    template<class T> inline void Fill(const aryptr<T> &lhs, const T &t);
    template<class T, class U> inline int Find(const algo::aryptr<T> &lhs, const U&t);
    template<class T> inline algo::aryptr<T> FirstN(const algo::aryptr<T> &lhs, u32 n);
    template<class T> inline algo::aryptr<T> LastN(const algo::aryptr<T> &lhs, u32 n);
    template<class T> inline algo::aryptr<T> RestFrom(const algo::aryptr<T> &lhs, u32 n);
    template<class T> inline algo::aryptr<T> qGetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n);
    template<class T> inline algo::aryptr<T> GetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n);
    template<class T> inline algo::aryptr<u8> BytesOf(const T &t);
    template<class T> inline T &qLast(const algo::aryptr<T> &ary);
}

// -----------------------------------------------------------------------------

namespace algo_lib { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/cpu_hz.cpp -- Grab cpu_hz from OS
    //
    void InitCpuHz();

    // -------------------------------------------------------------------
    // cpp/lib/algo/crc32.cpp -- Software-based CRC32
    //

    // This is a low-quality random number generator suitable for simple tasks...
    // Set seed for srng state
    void srng_SetSeed(algo_lib::Srng &srng, int z, int w);

    // retrieve random u32
    u32 srng_GetU32(algo_lib::Srng &srng);

    // retrieve random double in 0..1 range
    double srng_GetDouble(algo_lib::Srng &srng);

    // -------------------------------------------------------------------
    // cpp/lib/algo/errtext.cpp
    //
    void SaveBadField(strptr name, strptr value);

    // Reset value of algo_lib::_db.errtext and return it for further editing
    // Usage:
    // algo_lib::ResetBadTags() << ...errors...
    cstring &ResetErrtext();

    // Add key-value pair to algo_lib::_db.errtext
    // Error text beyond a reasonable limit is discarded -- keep errors short!
    void SaveBadTag(const strptr &name, const strptr &value);

    // Better name than SaveBadTAg
    void AppendErrtext(const strptr &name, const strptr &value);

    // Retrieve whatever bad tags were saved with SaveBadTag,
    // and clear the state.
    // SaveBadTag is typically called by string read functions that encounter
    // something unreadable. This is the only way to retrieve that
    // additional information
    tempstr DetachBadTags();

    // Increment algo_lib::_db.trace.tot_insert_err
    // And print accumulated 'bad tags' using prerr.
    // if SetShowInsertErrLim was previously called.
    // Note: printing is disabled by default, and must be explicitly enabled.
    void NoteInsertErr(strptr tuple);

    // Specify how many times NoteInsertErr can use prerr
    // to output.
    void SetShowInsertErrLim(int n);

    // -------------------------------------------------------------------
    // cpp/lib/algo/file.cpp -- File functions
    //
    void fildes_Cleanup(algo_lib::FLockfile &lockfile);

    // If PATH is an existing path, leave it unchanged
    // On Windows, If PATH.EXE is an existing path, return that
    // Return true if file exists
    bool TryExeSuffix(cstring &path);

    // Update FNAME to be a filename that can be passed to Unix exec call.
    // If FNAME is an absolute path, don't perform a search
    // If FNAME is a relative path, perform a search using the PATH environment
    // the first executable file that's found is the result.
    void ResolveExecFname(cstring &fname);

    // -------------------------------------------------------------------
    // cpp/lib/algo/fmt.cpp -- Print to string / Read from string
    //
    void ErrorX_Print(algo_lib::ErrorX &row, algo::cstring &str);
    void FTxttbl_Print(algo_lib::FTxttbl &T_, cstring &str);

    // -------------------------------------------------------------------
    // cpp/lib/algo/iohook.cpp
    //
    void IohookInit();

    // Register IOHOOK to be called whenever an IO operation is possible.
    // OK to add an fd twice with different flags. Subsequent calls override previous ones.
    // Add iohook to epoll in read, write or read/write mode
    // Optionally, add as edge triggered
    void IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) __attribute__((nothrow));

    // De-register interest in iohook
    void IohookRemove(algo_lib::FIohook& iohook) __attribute__((nothrow));

    // give up unused time to the OS.
    // Compute number of clocks to sleep before next scheduling cycle
    // If there was no sleep on the previous cycle, the sleep is zero.
    // This last bit is important because it prevents deadlocks
    // when one step implicitly creates work for another step
    // that occurs before it in the main loop.
    // Sleep will not extend beyond algo_lib::_db.limit
    //     (user-implemented function, prototype is in amc-generated header)
    // void giveup_time_Step();

    // -------------------------------------------------------------------
    // cpp/lib/algo/lib.cpp -- Main file
    //

    // Set exit time of main loop to current time.
    void ReqExitMainLoop();
    void fd_Cleanup(algo_lib::FFildes &fildes);
    void fildes_Cleanup(algo_lib::FIohook &iohook);
    //     (user-implemented function, prototype is in amc-generated header)
    // void bh_timehook_Step();

    // Check signature on incoming data
    bool dispsigcheck_InputMaybe(dmmeta::Dispsigcheck &dispsigcheck);

    // Die when parent process dies
    void DieWithParent();

    // Create temporary file
    // tempfile.fildes points to the new temp file after this
    // temp file is created under temp/
    // prefix is a namespace-unique name, such as "amc.xyz"
    // Actual file that's created becomes "temp/amc.xyz.XXXXXX"
    // Computed filename is saved to tempfile.filename
    void TempfileInitX(algo_lib::FTempfile &tempfile, strptr prefix);
    void fildes_Cleanup(algo_lib::FTempfile &tempfile);

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
    int CreateRedirect(strptr redirect);

    // Interpret redirect string and make DST_FD consistent with
    // the intended state. Return 0 on success, -1 on failure
    // This function is usually called in the child process right after fork
    // See CreateRedirect for interpretation of redirect string
    int ApplyRedirect(strptr redirect, int dst_fd);
    bool IpmaskValidQ(const strptr ipmask);

    // Read tuples from specified file descriptor.
    // FNAME is used to report errors
    // LoadFunc is called for each line.
    // if STRICT is specified, function exits on first error.
    // Return success status
    // This function is used by amc for process startup
    // If this function is passed a non-blocking file descriptor,
    // it is temporarily changed to a blocking one.
    bool LoadTuplesFd(algo::Fildes fd, strptr fname, bool (*LoadFunc)(strptr), bool strict);

    // Like LoadTuplesFd but load from specified file
    bool LoadTuplesFile(strptr fname, bool (*LoadFunc)(strptr), bool strict);

    // root = "-" -- read tuples from stdin
    // root = filename -- read tuples from filename, return success code
    // root = directory -- load ssimfiles specified with SSIMFILES from dataset, return success code
    // all tuples are passed to LoadFunc.
    // if STRICT is specified, function exits on first error.
    // Otherwise, it runs until the end.
    // This function is used by amc for process startup
    bool DoLoadTuples(strptr root, bool (*LoadFunc)(strptr), const char **ssimfiles, bool strict);

    // Return TRUE if current user is root.
    // #AL# is this more correct than checking that effective uid is 0?
    bool RootQ();

    // Return name of user executing this command.
    // If the command is being executed under sudo, return name
    // of original user.
    tempstr EffectiveUser();

    // if OWN_FD is cleared, clean up file descriptor before it is closed
    void file_Cleanup(algo_lib::InTextFile &file);

    // Walk child process tree for parent process pid, in post-order traversal way,
    // and send signal sig to each process. Kill_topmost is an option whether
    // to send signal to parent process itself. Return value - number of processes
    // to whose the signal has been actually sent.
    // Does not throw exceptions, just prints error message if kill() fails.
    // Linux only.
    int KillRecurse(int pid, int sig, bool kill_topmost);

    // -------------------------------------------------------------------
    // cpp/lib/algo/line.cpp -- Line processing
    //

    // Read next line from FILE into RESULT
    // When end of file is reached, return FALSE.
    // If error occurs during reading, return false.
    // Typical usage:
    // InTextFile file;
    // file.file = OpenRead(filename);
    // for (strptr line; ReadLine(file,line); ) {
    // <do something with LINE>
    // }
    // A partial line at the end of the file is returned.
    bool ReadLine(algo_lib::InTextFile &file, algo::strptr &result);

    // -------------------------------------------------------------------
    // cpp/lib/algo/lockfile.cpp -- Lock file
    //

    // Attempt to lock the file named NAME, updating state of LOCKFILE.
    // Return TRUE if lock is successfully applied.
    // If FAIL_OK is false, an error causes an exception (function always returns true)
    // If an exception is thrown, the text contains error description.
    // Otherwise, algo_lib::_db.errtext (DetachBadTags()) contains human-readable description of error.
    // If WAIT_TIMEOUT is non-zero, block up to WAIT_TIMEOUT seconds before failing
    // Write pid to file specified in NAME, and lock file using flock().
    bool LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok, algo::UnDiff wait_timeout);

    // Write pid to lockfile, separate function to update pid after fork().
    // Sets error text in case of error, and return false.
    bool WritePid(algo_lib::FLockfile &lockfile);

    // Non-blocking attempt to lock LOCKFILE
    // Return success status
    // If FAIL_OK is FALSE, throw exception on error (must succeed)
    bool LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok);

    // Non-blocking attempt to lock LOCKFILE.
    // Throw exception on error.
    // If the function returns, it has succeeded.
    void LockFileInit(algo_lib::FLockfile &lockfile, strptr name);

    // Read contents of lock file FNAME, extracting the pid that's stored in there.
    // On failure, return 0
    pid_t LockFileRead(strptr fname);

    // -------------------------------------------------------------------
    // cpp/lib/algo/mmap.cpp -- Mmap wrapper
    //

    // User-defined cleanup function for MMAP.MEM
    void mem_Cleanup(algo_lib::Mmap &mmap);

    // Attach mmapfile MMAPFILE to FD.
    // Return success code.
    bool MmapFile_LoadFd(MmapFile &mmapfile, algo::Fildes fd);

    // Attach mmapfile MMAPFILE to FNAME
    // Return success code.
    bool MmapFile_Load(MmapFile &mmapfile, strptr fname);

    // -------------------------------------------------------------------
    // cpp/lib/algo/regx.cpp -- Sql Regx implementation
    //
    void Regx_Print(algo_lib::Regx &regx, cstring &lhs);

    // Check if REGX matches S, return result
    bool Regx_Match(algo_lib::Regx &regx, strptr text);

    // Parse string INPUT as regex and store in REGX.
    // Supported features:
    // \n,\r,\t,\a,\e,\v,\f, [..], $, ^,
    // ., *, +, (..), {..}, \d,\D,\w,\W,\s,\S
    void Regx_ReadDflt(algo_lib::Regx &regx, strptr input);

    // Parse bash-style regx:
    // * -> .*
    // ? -> .?
    // All other regx chars are escaped away
    // if FULL is set to false, input is treated as ".*input.*"
    void Regx_ReadShell(algo_lib::Regx &regx, strptr input, bool full);

    // Parse SQL-style regx:
    // % -> .*
    // _ -> .
    // All other regx chars are escaped away
    // if FULL is set to false, input is treated as ".*input.*"
    void Regx_ReadSql(algo_lib::Regx &regx, strptr input, bool full);
    bool Regx_ReadStrptrMaybe(algo_lib::Regx &regx, strptr input);

    // Check if string contains a SQL regular expression
    bool SqlRegxQ(strptr s);

    // -------------------------------------------------------------------
    // cpp/lib/algo/string.cpp -- cstring functions
    //
    void ind_replvar_Cleanup(algo_lib::Replscope &replscope);

    // Set value of key KEY value VALUE
    // KEY        string to replace
    // VALUE      value to replace it with
    // SUBST      If set, $-expand the VALUE parameter
    void Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst);
    void Set(algo_lib::Replscope &scope, strptr from, strptr to);

    // Append TEXT to OUT, performing $-substitution
    // There are no separators between $-parameters and rest
    // of the text. The earliest possible match is replaced.
    //
    // EOL      append end-of-line (default)
    // SCOPE    if not NULL, replace any $-string in TEXT with corresponding value.
    // it is an error if any $-string does not expand.
    void Ins(algo_lib::Replscope *scope, cstring &out, strptr text, bool eol);
    void Ins(algo_lib::Replscope *scope, cstring &out, strptr text);

    // Enable comma-eating (default true)
    void SetEatComma(algo_lib::Replscope &scope, bool enable);

    // Enable strict mode (default true -- any failed substitution kills process)
    // If strict mode is off, failed substitution acts as if there was no substitution
    void SetStrictMode(algo_lib::Replscope &scope, bool enable);

    // Perform $-substitutions in TEXT and return new value.
    tempstr Subst(algo_lib::Replscope &scope, strptr text);

    // read TEXT into a tuple.
    // perform variable substitution on key-value pairs.
    // write tuple back.
    // return resulting string
    tempstr Tuple_Subst(algo_lib::Replscope &R, strptr text);
    void RunCsvParse(algo_lib::CsvParse &parsecsv);

    // -------------------------------------------------------------------
    // cpp/lib/algo/timehook.cpp
    //

    // Initialize time hook TH as non-recurrent, with delay DELAY.
    // Usage:
    // ThInit(th, SchedTime());     // schedule at current time
    // hook_Set0(th, myfunction);   // set callback
    // bh_timehook_Reheap(th);      // insert into timehook heap
    // ... eventually algo_lib::Step() will call the hook
    void ThInit(algo_lib::FTimehook& th, algo::SchedTime delay) __attribute__((nothrow));

    // Similar to the above, but recurrent.
    void ThInitRecur(algo_lib::FTimehook& th, algo::SchedTime delay) __attribute__((nothrow));

    // -------------------------------------------------------------------
    // cpp/lib/algo/txttbl.cpp -- Ascii table
    //

    // Add new cell to the given row.
    algo_lib::FTxtcell &AddCell(algo_lib::FTxtrow &txtrow, strptr title, algo_TextJustEnum justify, int span);
    algo_lib::FTxtcell &AddCell(algo_lib::FTxtrow &txtrow, strptr title, algo_TextJustEnum justify);

    // Add new cell to the last row of of table and return references to its text
    algo_lib::FTxtrow &AddRow(algo_lib::FTxttbl &txttbl);

    // Add column to the last row of table and return reference to it
    algo_lib::FTxtcell &AddCellEx(algo_lib::FTxttbl &txttbl);

    // Text table.
    // Text table is a table with rows; Each row has an array of cells.
    // Each call has text, col span, text justification, and optional style (color).
    // Add column to the last row of table.
    void AddCol(algo_lib::FTxttbl &txttbl, strptr col, algo_TextJustEnum justify);
    void AddCol(algo_lib::FTxttbl &txttbl, strptr col);
    cstring &AddCell(algo_lib::FTxttbl &txttbl);

    // Use prlog(txttbl) to print.
    void TxttblCsv(algo_lib::FTxttbl &tbl);
}
