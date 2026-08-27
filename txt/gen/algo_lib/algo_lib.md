## algo_lib - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>
<!-- abt_md.toc_end -->

### Description
<a href="#description"></a>
for usage, see [algo_lib - Support library for all executables](/txt/lib/algo_lib/README.md)

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
inline bool operator !=(const algo::strptr &a, const algo::strptr &b)
```

```c++
U128 U128::operator %(const U128 &val) const
```

```c++
U128 U128::operator &(const U128 &rhs) const
```

```c++
U128 U128::operator *(const U128 &rhs) const
```

```c++
inline char *ToCstr(char *to, const strptr &x)
```

```c++
void *mmap(void *base_addr, i64 size, MmapProt prot, MmapFlags mapflags, int fd, int flags2)
```

```c++
DIR *opendir(const char *path)
```

```c++
char *strerror_r(int code, char *buf, int bufsize)
```

```c++
const char *strsignal(int sig)
```

```c++
// https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
void *sync_lock_test_and_set(void **addr, void *val)
```

```c++
// https://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
void *sync_val_compare_and_swap(void **addr, void *oldval, void *newval)
```

```c++
U128 U128::operator +(const U128 &val) const
```

```c++
U128 U128::operator -(const U128 &val) const
```

```c++
inline algo::SchedTime operator +  (const algo::SchedTime &a, algo::SchedTime d)
```

```c++
inline algo::SchedTime operator -  (const algo::SchedTime &a, algo::SchedTime d)
```

```c++
inline algo::UnDiff operator *  (const algo::UnDiff &a, i64 d)
```

```c++
inline algo::UnDiff operator +    (const algo::UnDiff &a, algo::UnDiff d)
```

```c++
inline algo::UnDiff operator += (algo::UnDiff &a, const algo::UnDiff d)
```

```c++
inline algo::UnDiff operator -    (algo::UnTime a, algo::UnTime b)
```

```c++
inline algo::UnDiff operator -    (const algo::UnDiff &a, algo::UnDiff d)
```

```c++
inline algo::UnDiff operator -    (const algo::UnDiff a)
```

```c++
inline algo::UnDiff operator /  (const algo::UnDiff &a, i64 d)
```

```c++
inline algo::UnTime operator + (algo::UnTime a, algo::UnDiff d)
```

```c++
inline algo::UnTime operator += (algo::UnTime &a, const algo::UnDiff d)
```

```c++
inline algo::UnTime operator -    (algo::UnTime a, algo::UnDiff d)
```

```c++
inline algo::UnixDiff operator *  (const algo::UnixDiff &a, i64 d)
```

```c++
inline algo::UnixDiff operator +  (const algo::UnixDiff &a, algo::UnixDiff d)
```

```c++
inline algo::UnixDiff operator -  (const algo::UnixDiff &a, algo::UnixDiff d)
```

```c++
inline algo::UnixDiff operator - (const algo::UnixTime &a, algo::UnixTime b)
```

```c++
inline algo::UnixDiff operator /  (const algo::UnixDiff &a, i64 d)
```

```c++
inline algo::UnixTime operator +  (const algo::UnixTime &a, algo::UnixDiff d)
```

```c++
inline algo::UnixTime operator -  (const algo::UnixTime &a, algo::UnixDiff d)
```

```c++
U128 U128::operator /(const U128 &val) const
```

```c++
inline bool operator <(const algo::strptr &a, const algo::strptr &b)
```

```c++
U128 U128::operator <<(u64 count) const
```

```c++
template<class T> inline tempstr &algo::operator <<(const tempstr &lhs, const T &rhs)
```

```c++
inline bool operator ==(const algo::strptr &a, const algo::strptr &b)
```

```c++
U128 U128::operator >>(u64 count) const
```

```c++
algo::cstring &algo_lib::AddCell(algo_lib::FTxttbl &txttbl)
```

```c++
// Add new cell to the given row.
// TEXT specifies the text for the cell
algo_lib::FTxtcell &algo_lib::AddCell(algo_lib::FTxtrow &txtrow, algo::strptr text, algo_TextJustEnum justify = algo_TextJust_j_auto)
```

```c++
// Add column to the last row of table and return reference to it
// The justification for the column is copied from the header row
algo_lib::FTxtcell &algo_lib::AddCellEx(algo_lib::FTxttbl &txttbl)
```

```c++
// Add a column with specified text TEXT to the last row of the table
// If optional JUSTIFY is provided, the justification is set. Otherwise, justification is automatically
// determined from the header cell.
void algo_lib::AddCol(algo_lib::FTxttbl &txttbl, algo::strptr text, algo_TextJustEnum justify = algo_TextJust_j_auto)
```

```c++
// Add a comma-separated list of columns to the table
void algo_lib::AddCols(algo_lib::FTxttbl &txttbl, algo::strptr csv, algo_TextJustEnum justify = algo_TextJust_j_left)
```

```c++
// Add new row to the table and return a reference to it
algo_lib::FTxtrow &algo_lib::AddRow(algo_lib::FTxttbl &txttbl)
```

```c++
// Add key-value pair to algo_lib::_db.errtext
// Error text beyond a reasonable limit is discarded -- keep errors short!
void algo_lib::AppendErrtext(const strptr &name, const strptr &value)
```

```c++
// Install HZ as the process's cycles<->seconds calibration: refuse an
// implausible value, set the conversion constants, and re-anchor the
// scheduler clock so elapsed time counts from the calibration point.
void algo_lib::ApplyCpuHz(double hz)
```

```c++
// Interpret redirect string and make DST_FD consistent with
// the intended state. Return 0 on success, -1 on failure
// This function is usually called in the child process right after fork
// See CreateRedirect for interpretation of redirect string
// The pipe token "|" makes DST_FD a copy of PIPE_FD (a pipe end set up by the
// caller before fork). PIPE_FD is not closed here -- the caller owns both pipe
// ends. "|" with PIPE_FD<0 is a misuse and returns -1. To merge stderr into the
// stdout pipe, set fstdout="|" and fstderr=">&1" (the child applies stdout
// before stderr, so >&1 duplicates the already-redirected stdout pipe).
int algo_lib::ApplyRedirect(strptr redirect, int dst_fd, int pipe_fd = -1)
```

```c++
// Enable/disable log category NAME with filter FILTER.
// FILTER is a regex
// If FILTER starts with !, it is a negative filter (any matching lines are omitted)
// Return number of logcats affected.
// Changed logcats are marked, attribute changed = algo_lib::_db.clock
int algo_lib::ApplyTrace(algo::strptr name, algo::strptr filter, bool enable = true)
```

```c++
// Enable or disable logcat tracing based on traace expression WHAT
// WHAT is a comma-separated list of logcat regexes, e.g. a,b,c
// Each component can be a key-value pair, e.g. a:<filter>,b,c
// <filter> is an optional regex; Regex can be prefixed with ! to indicate a negative match.
// Timestamps can be controlled with 'timestamps'
// Verbose can be controlled with 'verbose'
// Debug can be controlled with 'debug'
int algo_lib::ApplyTrace(algo::strptr what, bool enable = true)
```

```c++
// Close FD if it holds a valid descriptor (value>=0), then mark it invalid.
// No-op when FD is already invalid.
void algo_lib::Close(algo::Fildes &fd)
```

```c++
// Close specified handle if it open
// Set value to NULL
void CloseHandleSafe(HANDLE &handle)
```

```c++
// Close every descriptor above stderr, leaving an exec'd program its three
// standard streams and nothing else.  Call this in the child, after the fork
// and before the exec.
//
// A command that inherits a descriptor holds what the descriptor holds for as
// long as it runs, without knowing it: a listening socket keeps its port
// bound, an unlinked file keeps its blocks, a lock keeps its holder alive.  A
// gateway's port outliving the gateway and reappearing as "address already in
// use" under an unrelated process name is this leak read from the far end.
// The child inherits whatever the parent had open, so the close belongs in one
// place rather than at each parent that happens to hold something.
//
// close_range does it in one syscall and skips no descriptor; walking the
// table one close at a time stops at the first hole unless it is bounded by
// the limit instead, which is what a kernel without the syscall gets.
void algo_lib::CloseInheritedFd()
```

```c++
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
int algo_lib::CreateRedirect(strptr redirect)
```

```c++
// Retrieve whatever bad tags were saved with AppendErrtext,
// and clear the state.
// AppendErrtext is typically called by string read functions that encounter
// something unreadable. This is the only way to retrieve that
// additional information
tempstr algo_lib::DetachBadTags()
```

```c++
// Die when parent process dies
void algo_lib::DieWithParent()
```

```c++
// Return name of user executing this command.
// If the command is being executed under sudo, return name
// of original user.
tempstr algo_lib::EffectiveUser()
```

```c++
// For InlineOnce and TimeHookOnce steps, break
// out of the enclosing while loop (over timeouts).
// Calling this function may be necessary when it becomes known that
// no further progress can be made by the step function.
void algo_lib::EndStep()
```

```c++
void algo_lib::ErrorX_Print(algo_lib::ErrorX &row, algo::cstring &str)
```

```c++
algo_TextJustEnum algo_lib::EvalJust(algo_lib::FTxttbl &txttbl, u32 col, algo_TextJustEnum just)
```

```c++
// Record child wait status STATUS as this process's own exit facts: exit_code
// receives the shell-convention exit code (exit N -> N, death by signal ->
// 128+signal), and exit_signal separately receives the terminating signal --
// 0 when the child exited -- so the fold into the exit code never loses which
// case it was.  The one site that decomposes a wait status for export; a
// caller that assigned the raw status directly would hand the C runtime a
// value whose low eight bits are zero for every exit(N) child.
void algo_lib::ExportWaitStatus(int status)
```

```c++
// Print table TXTTBL using markdown, appending to string STR.
// First row of the table is assumed to be the header.
// Newlines in cells are converted to '<br>'.
void algo_lib::FTxttbl_Markdown(algo_lib::FTxttbl &txttbl, algo::cstring &str)
```

```c++
void algo_lib::FTxttbl_Print(algo_lib::FTxttbl &txttbl, algo::cstring &str)
```

```c++
// Look up cell with specific coordinates.
algo_lib::FTxtcell *algo_lib::FindCell(algo_lib::FTxttbl &txttbl, int row, int col)
```

```c++
// Signature of dispatch DISPSIG as this binary was compiled with -- the digest
// itself, zero when the binary carries no such dispatch.  Every executable loads
// its own namespace's rows into this table at startup, so the answer is a fact
// about the running program and not about any data it has read.
//
// The digest is what a comparison wants and what a binary protocol carries, so
// it is what this returns; the 40-character hex form is a rendering of it, which
// `tempstr() << GetDispsig(...)` produces where a reader or a text field needs
// one.
algo::Signature algo_lib::GetDispsig(algo::strptr dispsig)
```

```c++
// Return name of temp directory.
// If it's not already initialized, it is set to the default value:
// temp/<procname> where <procname> is taken from argv[0].
// The directory is created as needed.
algo::strptr algo_lib::GetTempDir()
```

```c++
void algo_lib::InitCpuHz()
```

```c++
// Append TEXT to OUT, performing $-substitution using variables from SCOPE (must be non-NULL)
// if EOL is set, then new line is appended at the end.
void algo_lib::Ins(algo_lib::Replscope *scope, algo::cstring &out, strptr text, bool eol = true)
```

```c++
// Register IOHOOK to be called whenever an IO operation is possible.
// Add iohook to epoll in read, write or read/write mode, edge triggered.
// Re-registering the same iohook replaces its flag set, so a caller may widen
// or narrow one hook's subscription at any time; a descriptor carries exactly
// one registration, so two hooks may never claim the same fd.
void algo_lib::IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) NOTHROW
```

```c++
void algo_lib::IohookInit()
```

```c++
// De-register interest in iohook
void algo_lib::IohookRemove(algo_lib::FIohook& iohook) NOTHROW
```

```c++
bool algo_lib::IpmaskValidQ(const strptr ipmask)
```

```c++
// Walk child process tree for parent process pid, in post-order traversal way,
// and send signal sig to each process. Kill_topmost is an option whether
// to send signal to parent process itself. Return value - number of processes
// to whose the signal has been actually sent.
// Does not throw exceptions, just prints error message if kill() fails.
// Linux only.
int algo_lib::KillRecurse(int pid, int sig, bool kill_topmost)
```

```c++
// Attempt to lock the file named NAME, updating state of LOCKFILE.
// Return TRUE if lock is successfully applied.
// If FAIL_OK is false, an error causes an exception (function always returns true)
// If an exception is thrown, the text contains error description.
// Otherwise, algo_lib::_db.errtext (DetachBadTags()) contains human-readable description of error.
// If WAIT_TIMEOUT is non-zero, block up to WAIT_TIMEOUT seconds before failing
// Write pid to file specified in NAME, and lock file using flock().
bool algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok = algo::FailokQ(false), algo::UnDiff wait_timeout = algo::UnDiff())
```

```c++
// Read contents of lock file FNAME, extracting the pid that's stored in there.
// On failure, return 0
pid_t algo_lib::LockFileRead(strptr fname)
```

```c++
// Filter string STR for output on LOGCAT.
// The string must match FILTER and not match NEGFILTER.
// The initial state is that filter is empty (uninitialized), so empty filter
// is interpreted as "match all"
// Filter and negfilter are configured with ApplyTrace which parses a trace expression
// In addition, if throttling is enabled on LOGCAT, block message
// if more than MAXMSG are being printed within WINDOW secs. The counter
// is reset every WINDOW secs.
// The window's count is of messages this category *matched*, not of messages it
// printed, which is what makes the suppressed tally at window end a real number.
// Counting only what printed freezes the count at the cap -- nothing increments
// it once suppression is on -- so the overflow reads as zero and an operator is
// never told the trace they are reading is lossy.  TOTMSG stays the count of
// messages that went out.
bool algo_lib::LogcatFilterQ(algo_lib::FLogcat &logcat, algo::strptr str)
```

```c++
// TRUE when LOGCAT may emit right now: the operator has it on and the throttle
// is not suppressing the rest of its window.
//
// The two are separate fields because they are separate facts with separate
// owners -- enabled is what the operator asked for, suppress is what the
// throttle is doing about the current window -- and this is the one place that
// combines them.  A caller testing only ENABLED would print through a throttle;
// one testing only SUPPRESS would print a category nobody asked for.
inline bool algo_lib::LogcatOnQ(algo_lib::FLogcat &logcat)
```

```c++
// Tell the memory checker that a pool has just handed the program the SIZE bytes
// at MEM, so that the checker accounts for that block from here on and reports it
// if the program drops its last pointer to it.  MEM is the address the pool
// returned and SIZE the size the caller asked for; a failed allocation passes
// NULL, which the checker ignores.  In a build without the annotations the call
// is nothing at all, so an allocator carries no trace of it.
//
// A pool takes one big block from its base pool and carves its records out of it,
// so a checker that knows only about the big block sees a single live allocation
// no matter which record leaked.  Marking each record moves the accounting to the
// granularity the program allocates at, and the stack the checker then reports is
// the one that asked for the record.
//
// The block is marked defined rather than undefined, which is the state pool
// memory has anyway: a fresh block from the base pool is zeroed, and a recycled
// record still holds whatever was last written into it.  Marking it undefined
// would report a field read before anything wrote it, which is a real defect and
// a different one from a leak, so it belongs to a change that goes looking for it.
inline void algo_lib::MemcheckAlloc(void *mem, u64 size)
```

```c++
// Tell the memory checker that the block at MEM, SIZE bytes long, is no longer one
// the program holds -- either the pool has taken it back, or the pool is about to
// carve it into records of its own and will mark those instead.  MEM is the
// address a matching MemcheckAlloc marked, SIZE the size it marked; NULL is
// ignored.  Nothing at all in a build without the annotations.
//
// The block stays readable and writable afterwards, and that is what lets the pool
// go on using it.  A pool threads its free list through the memory it is not
// lending out, so a freed record holds the pointer to the next free one; a checker
// told to treat the block as unmapped would report the pool's own bookkeeping as
// an invalid write, once per free.
inline void algo_lib::MemcheckFree(void *mem, u64 size)
```

```c++
// Attach mmapfile MMAPFILE to FNAME
// Return success code.
bool algo_lib::MmapFile_Load(MmapFile &mmapfile, strptr fname)
```

```c++
// Attach mmapfile MMAPFILE to FD.
// Return success code.
//
// The mapping is MAP_SHARED, so it tracks the file and a write another handle
// makes shows through it.  MAP_PRIVATE would leave that unsaid: the standard
// does not decide whether a later write to the file reaches a private mapping,
// Linux lets it through and Darwin does not, so the same call would answer two
// different things.  Nothing is written through the mapping -- it is PROT_READ
// -- so sharing costs nothing and the behavior is the same everywhere.
bool algo_lib::MmapFile_LoadFd(MmapFile &mmapfile, algo::Fildes fd)
```

```c++
// Increment algo_lib::_db.trace.tot_insert_err
// And print accumulated 'bad tags' using prerr.
// if SetShowInsertErrLim was previously called.
void algo_lib::NoteInsertErr(strptr tuple)
```

```c++
// Change to the directory that was current before sandbox mode
// Must be balanced with PushDir
void algo_lib::PopDir()
```

```c++
// Start the subprocess and wait for it; return its wait() status.
int algo_lib::ProcExec(algo_lib::FProc &proc)
```

```c++
// Execute the subprocess; throw a human-readable algo_lib.exec error when the
// child exits non-zero (uses DescribeWaitStatus for the comment).
void algo_lib::ProcExecX(algo_lib::FProc &proc)
```

```c++
// Decode the last wait() status into a child exit code, or -1 if the child was
// killed by a signal or has not been reaped.
int algo_lib::ProcExitCode(algo_lib::FProc &proc)
```

```c++
// Kill the subprocess with SIGKILL and reap it. No-op when not running.
// A pgroup child is killed as a whole group (its descendants with it).
void algo_lib::ProcKill(algo_lib::FProc &proc)
```

```c++
// Start the subprocess if not already running. For each redirect set to "|",
// create a pipe before fork and expose the parent-side fd on proc
// (to_stdin / from_stdout / from_stderr). To merge stderr into the stdout pipe,
// set fstdout="|" and fstderr=">&1". Returns 0, or errno on fork failure.
int algo_lib::ProcStart(algo_lib::FProc &proc)
```

```c++
// Build a shell-legible rendering of the command line and redirects, for logging.
// Args are separated by two spaces so a value that itself contains a space stays
// visually distinguishable from the gap between args.
tempstr algo_lib::ProcToCmdline(algo_lib::FProc &proc)
```

```c++
// Wait for the subprocess to exit. Close to_stdin first so the child sees EOF,
// waitpid (restarting on EINTR), store status, clear pid, then close the read
// ends. Drain from_stdout / from_stderr before calling to avoid a deadlock.
void algo_lib::ProcWait(algo_lib::FProc &proc)
```

```c++
// Enter sandbox directory remember previous directory
void algo_lib::PushDir(algo::strptr dir)
```

```c++
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
bool algo_lib::ReadLine(algo_lib::InTextFile &file, algo::strptr &result)
```

```c++
// True if REDIRECT names a filesystem path (worth showing in a command line),
// as opposed to a pipe "|" or an fd dup "<&N"/">&N" -- those are internal
// plumbing (temporary fds), so _ToCmdline/ProcToCmdline omit them.
bool algo_lib::RedirectFileQ(strptr redirect)
```

```c++
void algo_lib::RegxState_Print(algo_lib::RegxState &state, algo::cstring &lhs)
```

```c++
// Find REGX in TEXT and return range of chars which match.
// If the regx is not found, return empty range.
// For this to work, REGX must be parsed with "full:N".
algo::i32_Range algo_lib::Regx_Find(algo_lib::Regx &regx, algo::strptr text, int start = 0)
```

```c++
// Check if REGX matches TEXT, return result
bool algo_lib::Regx_Match(algo_lib::Regx &regx, algo::strptr text)
```

```c++
// there is not enough information in a regx expression to fully specify it.
// sql vs shell vs classic regx, vs acr, partial vs full.
// we print back the original expression that was read in, but the information
// about what function read it is lost.
void algo_lib::Regx_Print(algo_lib::Regx &regx, algo::cstring &lhs)
```

```c++
void algo_lib::Regx_ReadAcr(algo_lib::Regx &regx, algo::strptr input, bool full)
```

```c++
// Parse string INPUT as regex and store in REGX.
// Supported features:
// \n,\r,\t,\a,\e,\v,\f     regular escaped chars
// [..]                     char range, possibly with negation
// $, ^                     anchors
// .                        any char
// *                        repetition
// +                        one or more
// (..)                     precedence grouping and capture groups
// \d,\D,\w,\W,\s,\S        macros
void algo_lib::Regx_ReadDflt(algo_lib::Regx &regx, algo::strptr input)
```

```c++
// Set REGX to match string INPUT literally
void algo_lib::Regx_ReadLiteral(algo_lib::Regx &regx, algo::strptr input)
```

```c++
void algo_lib::Regx_ReadShell(algo_lib::Regx &regx, algo::strptr input, bool full)
```

```c++
void algo_lib::Regx_ReadSql(algo_lib::Regx &regx, algo::strptr input, bool full)
```

```c++
bool algo_lib::Regx_ReadStrptrMaybe(algo_lib::Regx &regx, algo::strptr input)
```

```c++
void algo_lib::Regx_ReadStyle(algo_lib::Regx &regx, algo::strptr input, algo_lib::RegxStyle style, bool full)
```

```c++
// Set exit time of main loop to current time.
void algo_lib::ReqExitMainLoop()
```

```c++
// Demand the kernel's calibrated TSC rate, for a process that schedules on
// the counter: without the export nothing states the rate,
// so the process exits rather than starting on a figure that is merely
// plausible.  A plain tool never calls this and lives with the P-state
// figure InitCpuHz settled for.
//
// The alternative would be to time the counter against a wall clock at
// startup, and that measurement cannot be made trustworthy.  Its window is
// a few tens of milliseconds of ordinary scheduling, and a stall inside the
// window adds cycles the wall clock never saw: a 122ms stall on an EPYC
// 9R14 yields 1.8e10 Hz for a counter that ticks at 2.6GHz.  Repeating the
// window and keeping the best trial does not rescue it, because the stall
// is invisible to every criterion the trials can be compared on.  Worse
// than the absurd value is the plausible one -- a stall of a few tens of
// milliseconds produces a rate that passes every range test and then
// mis-scales every cycles<->seconds conversion in the process for the rest
// of its life.
//
// So the rate is read and never measured, which is why a rate nobody states
// is fatal here rather than a fallback.  InitCpuHz has already installed the
// kernel's figure where the export exists; what this demands is that the
// figure came from a source that states the counter's rate rather than from a
// P-state file.
//
// Two further sources exist because some kernels cannot carry the export at
// all.  A WSL2 guest is the case in hand: its kernel calibrates the counter
// exactly, having been told the rate by the hypervisor, but ships no header
// package to build the tsc_freq_khz module against and no /lib/modules to
// install it into, and it publishes the figure through no other interface --
// not cpufreq, not CPUID, not the MSR device.  The rate on such a host is
// known and merely unreadable, so the host states it directly, in the kHz the
// export would have carried.  Either way the figure is read rather than
// measured, so the rule above holds; and both are consulted only when the
// export is absent, which leaves a machine that has the export unable to be
// retuned by a stray statement.
//
// The two differ in how the statement reaches a process, not in what it means.
// A file on the host reaches every process running there whatever started it,
// because nothing has to inherit an environment for a file to be readable --
// which is why a shell profile does not serve: it reaches a login shell and
// misses a service, a job runner and a cron entry.  The variable covers the
// case where the filesystem a process sees is not the host's: a container is
// handed the rate as an ordinary environment entry when it starts.  The
// variable wins where both are present, being the narrower statement of the
// two.
//
// Both are named by the caller, because which file and which variable a
// deployment states its rate through is that deployment's convention and not
// this library's.
//
// Taking the rate from either is reported as a verbose line rather than as
// plain output.  Every process of a cluster reads the rate, so on such a host
// the plain form would announce it once per process, and a comptest compares a
// captured stdout byte for byte -- the announcement would fail every test that
// starts a cluster, on exactly the hosts these sources exist to serve.
// `atf_tsc` reports the calibration sources unconditionally, which is where an
// operator confirms which one a process would take.
void algo_lib::RequireKernelCpuHz(strptr tscfreq_path, strptr tscfreq_env)
```

```c++
// Reset value of algo_lib::_db.errtext and return it for further editing
// Usage:
// algo_lib::ResetBadTags() << ...errors...
algo::cstring &algo_lib::ResetErrtext()
```

```c++
// Update FNAME to be a filename that can be passed to Unix exec call.
// If FNAME is an absolute path, don't perform a search
// If FNAME is a relative path, perform a search using the PATH environment
// the first executable file that's found is the result.
void algo_lib::ResolveExecFname(algo::cstring &fname)
```

```c++
// Return TRUE if current user is root.
// #AL# is this more correct than checking that effective uid is 0?
bool algo_lib::RootQ()
```

```c++
void algo_lib::RunCsvParse(algo_lib::CsvParse &csvparse)
```

```c++
// Set value of key KEY value VALUE
// KEY        string to replace
// VALUE      value to replace it with
// SUBST      If set, $-expand the VALUE parameter
// NOTES
// With 'strict' Replscope, avoid expressions like this:
// Set(R, "$var", field.comment);
// This will trigger an error when field.comment contains a $ sign and the substitution fails.
// Use
// Set(R, "$var", field.comment, false);
//
void algo_lib::Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst = true)
```

```c++
// Perform $-substitutions in TEXT and return new value.
tempstr algo_lib::Subst(algo_lib::Replscope &scope, strptr text)
```

```c++
// Create temporary file
// tempfile.fildes points to the new temp file after this
// temp file is created under _db.tempdir
// prefix is a namespace-unique name, such as "amc.xyz"
// Actual file that's created becomes "temp/amc/xyz.XXXXXX"
// Computed filename is saved to tempfile.filename
void algo_lib::TempfileInitX(algo_lib::FTempfile &tempfile, strptr prefix)
```

```c++
// Initialize a recurrent time hook TH to execute on the next scheduling cycle,
// and after that, every DELAY clocks
// NOTE: 'delay' field of a recurrent timehook is used when automatically rescheduling it.
void algo_lib::ThInitRecur(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW
```

```c++
// Initialize a non-recurrent time hook TH to execute after DELAY clock cycles with
// respect to current time
// NOTE: 'delay' field of non-recurrent timehook is ignored
// NOTE: this function updates scheduling clock to the most current value
void algo_lib::ThScheduleIn(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW
```

```c++
// Schedule a time hook TH to execute on the next scheduling cycle,
// and after that, every DELAY clocks
void algo_lib::ThScheduleRecur(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW
```

```c++
algo::tempstr algo_lib::ToDbgString(algo_lib::Bitset &bitset)
```

```c++
algo::tempstr algo_lib::ToDbgString(algo_lib::Regx &regx)
```

```c++
algo::tempstr algo_lib::ToDbgString(algo_lib::RegxState &state, int index)
```

```c++
tempstr algo_lib::ToDbgString(algo_lib::RegxExpr *expr)
```

```c++
tempstr algo_lib::ToDbgString(algo_lib::RegxParse &regxparse)
```

```c++
// If PATH is an existing path, leave it unchanged
// On Windows, If PATH.EXE is an existing path, return that
// Return true if file exists
bool algo_lib::TryExeSuffix(algo::cstring &path)
```

```c++
// read TEXT into a tuple.
// perform variable substitution on key-value pairs.
// write tuple back.
// return resulting string
tempstr algo_lib::Tuple_Subst(algo_lib::Replscope &R, strptr text)
```

```c++
void algo_lib::UpdateRate(algo::I64Rate &rate, i64 val)
```

```c++
void algo_lib::Userinit()
```

```c++
int WCOREDUMP(int status)
```

```c++
int WEXITSTATUS(int status)
```

```c++
int WIFEXITED(int status)
```

```c++
int WIFSIGNALED(int status)
```

```c++
int WIFSTOPPED(int status)
```

```c++
int WSTOPSIG(int status)
```

```c++
int WTERMSIG(int status)
```

```c++
// True when the current directory lies inside a worktree rather than the
// main checkout.  A linked worktree's root has .git as a gitdir-pointer
// file, where the main checkout has a directory; a cow-farm sandbox has
// its own .git directory, so it is recognized by the wt/ path component
// every wt-managed checkout lives under.
bool algo_lib::WorktreeQ()
```

```c++
// Write pid to lockfile, separate function to update pid after fork().
// Sets error text in case of error, and return false.
bool algo_lib::WritePid(algo_lib::FLockfile &lockfile)
```

```c++
// Return directory of the worktree named NAME (empty name -> empty result)
tempstr algo_lib::WtDir(algo::strptr name)
```

```c++
template<class T> inline T &algo::aryptr<T>::operator [](u64 idx) const
```

```c++
int alarm(int sec)
```

```c++
template<class T> inline algo::aryptr<T>::aryptr()
```

```c++
template<class T> inline algo::aryptr<T>::aryptr(const T *e, i64 in_n) : elems(const_cast<T*>(e)) , n_elems(in_n)
```

```c++
inline u16 be16toh(u16 val)
```

```c++
inline u32 be32toh(u32 val)
```

```c++
inline u64 be64toh(u64 val)
```

```c++
void algo_lib::bh_timehook_Step()
```

```c++
void bool_Print(bool row, algo::cstring &str)
```

```c++
bool bool_ReadStrptrMaybe(bool &row, algo::strptr str)
```

```c++
void char_Print(char c, algo::cstring &str)
```

```c++
bool char_ReadStrptrMaybe(char &row, algo::strptr str)
```

```c++
void closedir(DIR *dir)
```

```c++
// Check signature on incoming data
bool algo_lib::dispsigcheck_InputMaybe(dmmeta::Dispsigcheck &dispsigcheck)
```

```c++
int dospawn(char *path, char *cmdline, int timeout, strptr fstdin, strptr fstdout, strptr fstderr)
```

```c++
void double_Print(double d, algo::cstring &str)
```

```c++
bool double_ReadStrptrMaybe(double &row, algo::strptr str)
```

```c++
// Enable comma-eating (default true)
void algo_lib::eatcomma_Set(algo_lib::Replscope &scope, bool enable)
```

```c++
// Global initializer, called from algo_lib::FDb_Init
void algo_lib::errns_Userinit()
```

```c++
// not too important
int fchmod(int fd, int mode)
```

```c++
void algo_lib::fd_Cleanup(algo_lib::FFildes &fildes)
```

```c++
void algo_lib::fildes_Cleanup(algo_lib::FIohook &iohook)
```

```c++
// User-defined cleanup trigger fildes field of ctype:algo_lib.FLockfile
// With KEEP set, the file survives: the flock alone was the lock, and the
// file's content remains behind as a durable record for later readers.
// The mtime is refreshed on release, so a reader can judge how long ago
// the record's holder let go of it.
void algo_lib::fildes_Cleanup(algo_lib::FLockfile &lockfile)
```

```c++
void algo_lib::fildes_Cleanup(algo_lib::FTempfile &tempfile)
```

```c++
// if OWN_FD is cleared, clean up file descriptor before it is closed
void algo_lib::file_Cleanup(algo_lib::InTextFile &file)
```

```c++
void float_Print(float d, algo::cstring &str)
```

```c++
bool float_ReadStrptrMaybe(float &row, algo::strptr str)
```

```c++
int flock(int fd, int flags)
```

```c++
int fnmatch(const char *pattern, const char *filename, int flags)
```

```c++
int fork()
```

```c++
int fstat(int fd, StatStruct *s)
```

```c++
int ftruncate(int fd, u64 size)
```

```c++
int getdomainname(char *buf, int bufsize)
```

```c++
int getrlimit(RlimitEnum what, rlimit *r)
```

```c++
void gettimeofday(struct timeval *tv, struct timezone *)
```

```c++
// give up unused time to the OS.
// Compute number of clocks to sleep before next scheduling cycle
// If there was no sleep on the previous cycle, the sleep is zero.
// This last bit is important because it prevents deadlocks
// when one step implicitly creates work for another step
// that occurs before it in the main loop.
// Sleep will not extend beyond algo_lib::_db.limit
void algo_lib::giveup_time_Step()
```

```c++
inline u16 htobe16(u16 val)
```

```c++
inline u32 htobe32(u32 val)
```

```c++
inline u64 htobe64(u64 val)
```

```c++
inline u16 htole16(u16 val)
```

```c++
inline u32 htole32(u32 val)
```

```c++
inline u64 htole64(u64 val)
```

```c++
void i8_Print(i8 i, algo::cstring &str)
```

```c++
void i16_Print(i16 i, algo::cstring &str)
```

```c++
void i32_Print(i32 i, algo::cstring &str)
```

```c++
void i64_Print(i64 i, algo::cstring &str)
```

```c++
int isatty(int fd)
```

```c++
int kill(pid_t pid, int signal)
```

```c++
int link(const char *target, const char *path)
```

```c++
// Windows doesn't support soft links.
// So implement lstat via stat
int lstat(const char *path, StatStruct *buf)
```

```c++
// User-defined cleanup function for MMAP.MEM
void algo_lib::mem_Cleanup(algo_lib::Mmap &mmap)
```

```c++
int mkdir(const char *path, int mode)
```

```c++
int mkstemp(char *tmpl)
```

```c++
int munmap(void *base_addr, i64 size)
```

```c++
inline algo::WDiff operator+(const algo::WDiff &t, const algo::WDiff &t2)
```

```c++
inline algo::WTime operator+(const algo::WTime &t, const algo::WDiff &t2)
```

```c++
inline algo::WDiff operator-(const algo::WDiff &t, const algo::WDiff &t2)
```

```c++
inline algo::WDiff operator-(const algo::WTime &t, const algo::WTime &t2)
```

```c++
inline algo::WTime operator-(const algo::WTime &t, const algo::WDiff &t2)
```

```c++
// On destruction of an FProc, kill and reap the child for forward progress.
void algo_lib::pid_Cleanup(algo_lib::FProc &proc)
```

```c++
// redirect to MS implementation
int pipe(int fd[2])
```

```c++
// Create a pipe whose two ends already carry FLAGS, the way Linux's pipe2 does.
//
// A caller that wants O_CLOEXEC on a pipe has to get it before the next fork,
// and `pipe` followed by `fcntl` leaves a window in between: a thread that forks
// there hands the child a copy of the write end, and the child then holds the
// reader open after the writer is gone, so a read that should have seen eof
// blocks forever.  Darwin has no atomic form of the call, so the window cannot
// be closed here -- what it can do is stop being the caller's problem, and shrink
// to the two fcntl calls below.
//
// FD receives the two descriptors on success, read end first.  FLAGS takes
// O_CLOEXEC and O_NONBLOCK, each applied to both ends; any other bit is refused
// with EINVAL, since silently dropping a flag the caller asked for is how the
// window above reopens.  Returns 0, or -1 with errno set and no descriptor left
// open.
int pipe2(int fd[2], int flags)
```

```c++
int pthread_create(pthread_t *thread, pthread_attr_t *attr, ThreadFunc func, void *arg)
```

```c++
pthread_t pthread_self()
```

```c++
int random()
```

```c++
int readlink(const char *path, char *out, int bufsize)
```

```c++
int setenv(const char *name, const char *value, int overwrite)
```

```c++
int setrlimit(RlimitEnum what, const rlimit *r)
```

```c++
// no sigaciton on windows
int sigaction(int signal, struct sigaction *sa, void*)
```

```c++
void sigemptyset(u32 *mask)
```

```c++
void sleep(int sec)
```

```c++
void srandom(int val)
```

```c++
// retrieve random double in 0..1 range
double algo_lib::srng_GetDouble(algo_lib::Srng &srng)
```

```c++
// retrieve random u32
u32 algo_lib::srng_GetU32(algo_lib::Srng &srng)
```

```c++
// This is a low-quality random number generator suitable for simple tasks...
// Set seed for srng state
void algo_lib::srng_SetSeed(algo_lib::Srng &srng, int z, int w)
```

```c++
int stat(const char *path, StatStruct *s)
```

```c++
// Create linux-type soft link
// Return 0 if success
int symlink(const char *target, const char *path)
```

```c++
int sysconf(int selector)
```

```c++
void u8_Print(u8 i, algo::cstring &str)
```

```c++
void u16_Print(u16 i, algo::cstring &str)
```

```c++
void u32_Print(u32 i, algo::cstring &str)
```

```c++
void u64_Print(u64 i, algo::cstring &str)
```

```c++
void u128_Print(u128 num, algo::cstring &out)
```

```c++
void usleep(int microsec)
```

```c++
pid_t waitpid(pid_t pid, int *status, int options)
```

```c++
U128 U128::operator |(const U128 &rhs) const
```

```c++
inline algo_FileFlagsEnum operator |(algo_FileFlagsEnum lhs, algo_FileFlagsEnum rhs)
```

### Inputs
<a href="#inputs"></a>
`algo_lib` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/_gen.cpp](/cpp/gen/_gen.cpp)||
|[cpp/gen/algo_gen.cpp](/cpp/gen/algo_gen.cpp)||
|[cpp/gen/algo_lib_gen.cpp](/cpp/gen/algo_lib_gen.cpp)||
|[cpp/gen/command_gen.cpp](/cpp/gen/command_gen.cpp)||
|[cpp/gen/dev_gen.cpp](/cpp/gen/dev_gen.cpp)||
|[cpp/gen/dmmeta_gen.cpp](/cpp/gen/dmmeta_gen.cpp)||
|[cpp/gen/report_gen.cpp](/cpp/gen/report_gen.cpp)||
|[cpp/lib/algo/arg.cpp](/cpp/lib/algo/arg.cpp)|Parse command-line|
|[cpp/lib/algo/backtrace.cpp](/cpp/lib/algo/backtrace.cpp)|Print stack trace in case of crash|
|[cpp/lib/algo/base64.cpp](/cpp/lib/algo/base64.cpp)|base64 encoding|
|[cpp/lib/algo/bin_decode.cpp](/cpp/lib/algo/bin_decode.cpp)||
|[cpp/lib/algo/bin_encode.cpp](/cpp/lib/algo/bin_encode.cpp)||
|[cpp/lib/algo/cpu_hz.cpp](/cpp/lib/algo/cpu_hz.cpp)|Obtain cpu_hz from a source that states it, never by measuring|
|[cpp/lib/algo/crc32.cpp](/cpp/lib/algo/crc32.cpp)|Software-based CRC32|
|[cpp/lib/algo/decimal.cpp](/cpp/lib/algo/decimal.cpp)||
|[cpp/lib/algo/errtext.cpp](/cpp/lib/algo/errtext.cpp)||
|[cpp/lib/algo/file.cpp](/cpp/lib/algo/file.cpp)|File functions|
|[cpp/lib/algo/fmt.cpp](/cpp/lib/algo/fmt.cpp)|Print to string / Read from string|
|[cpp/lib/algo/iohook.cpp](/cpp/lib/algo/iohook.cpp)||
|[cpp/lib/algo/lib.cpp](/cpp/lib/algo/lib.cpp)|Main file|
|[cpp/lib/algo/line.cpp](/cpp/lib/algo/line.cpp)|Line processing|
|[cpp/lib/algo/lockfile.cpp](/cpp/lib/algo/lockfile.cpp)|Lock file|
|[cpp/lib/algo/macos.cpp](/cpp/lib/algo/macos.cpp)|macOS adaptation layer|
|[cpp/lib/algo/mmap.cpp](/cpp/lib/algo/mmap.cpp)|Mmap wrapper|
|[cpp/lib/algo/prlog.cpp](/cpp/lib/algo/prlog.cpp)|prlog macro|
|[cpp/lib/algo/proc.cpp](/cpp/lib/algo/proc.cpp)||
|[cpp/lib/algo/regx.cpp](/cpp/lib/algo/regx.cpp)|Sql Regx implementation|
|[cpp/lib/algo/replscope.cpp](/cpp/lib/algo/replscope.cpp)||
|[cpp/lib/algo/retry.cpp](/cpp/lib/algo/retry.cpp)|retry_curs|
|[cpp/lib/algo/string.cpp](/cpp/lib/algo/string.cpp)|cstring functions|
|[cpp/lib/algo/time.cpp](/cpp/lib/algo/time.cpp)|UnTime / UnDiff functions|
|[cpp/lib/algo/timehook.cpp](/cpp/lib/algo/timehook.cpp)||
|[cpp/lib/algo/tstamp.cpp](/cpp/lib/algo/tstamp.cpp)|timestamp cache|
|[cpp/lib/algo/txttbl.cpp](/cpp/lib/algo/txttbl.cpp)|Ascii table|
|[cpp/lib/algo/u128.cpp](/cpp/lib/algo/u128.cpp)||
|[cpp/lib/algo/url.cpp](/cpp/lib/algo/url.cpp)||
|[cpp/lib/algo/uuid.cpp](/cpp/lib/algo/uuid.cpp)||
|[cpp/lib/algo/win32.cpp](/cpp/lib/algo/win32.cpp)||
|[include/algo.h](/include/algo.h)|Main header|
|[include/algo.inl.h](/include/algo.inl.h)|Inline functions|
|[include/define.h](/include/define.h)|Macros|
|[include/gen/_gen.h](/include/gen/_gen.h)||
|[include/gen/_gen.inl.h](/include/gen/_gen.inl.h)||
|[include/gen/algo_gen.h](/include/gen/algo_gen.h)||
|[include/gen/algo_gen.inl.h](/include/gen/algo_gen.inl.h)||
|[include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)||
|[include/gen/algo_lib_gen.inl.h](/include/gen/algo_lib_gen.inl.h)||
|[include/gen/command_gen.h](/include/gen/command_gen.h)||
|[include/gen/command_gen.inl.h](/include/gen/command_gen.inl.h)||
|[include/gen/dev_gen.h](/include/gen/dev_gen.h)||
|[include/gen/dev_gen.inl.h](/include/gen/dev_gen.inl.h)||
|[include/gen/report_gen.h](/include/gen/report_gen.h)||
|[include/gen/report_gen.inl.h](/include/gen/report_gen.inl.h)||
|[include/sha.h](/include/sha.h)||
|[include/sysincl.h](/include/sysincl.h)|System header includes|
|[include/typedef.h](/include/typedef.h)|Typedefs|
|[include/u128.h](/include/u128.h)||
|[include/win32.h](/include/win32.h)||

### In Memory DB
<a href="#in-memory-db"></a>
`algo_lib` generated code creates the tables below.
All allocations are done through global `algo_lib::_db` [algo_lib.FDb](#algo_lib-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[algo_lib.Bitset](#algo_lib-bitset)||
|[algo_lib.Cmdline](#algo_lib-cmdline)||
|[algo_lib.CsvParse](#algo_lib-csvparse)||FDb.csvparse (Cppstack)|
|[algo_lib.ErrorX](#algo_lib-errorx)||FDb.error (Cppstack)|
|[algo_lib.RegxM](#algo_lib-regxm)||
|[algo_lib.FFildes](#algo_lib-ffildes)||FDb.fildes (Cppstack)|
|[algo_lib.FLockfile](#algo_lib-flockfile)||
|[algo_lib.FTimehook](#algo_lib-ftimehook)||FDb.timehook (Cppstack)|bh_timehook (Bheap, sort field time)|c_timehook (Ptr)|
||||FTimehook.hook (Hook)|
|[algo_lib.FImdb](#algo_lib-fimdb)||FDb.imdb (Inlary)|ind_imdb (Thash, hash field imdb)|
|[algo_lib.RegxFlags](#algo_lib-regxflags)||
|[algo_lib.RegxStyle](#algo_lib-regxstyle)||
|[algo_lib.Regx](#algo_lib-regx)||FDb.regx (Cppstack)|
||||RegxParse.p_regx (Upptr)|
|[algo_lib.FLogcat](#algo_lib-flogcat)|[dmmeta.logcat](/txt/ssimdb/dmmeta/logcat.md)|FDb.logcat (Inlary)|**static**|ind_logcat (Thash, hash field logcat)|
|[algo_lib.FErrns](#algo_lib-ferrns)||FDb.errns (Inlary)|
|[algo_lib.FDb](#algo_lib-fdb)||FDb._db (Global)|
|[algo_lib.FDispsigcheck](#algo_lib-fdispsigcheck)|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|FDb.dispsigcheck (Lary)|dispsigcheck (Lary, by rowid)|ind_dispsigcheck (Thash, hash field dispsig)|
|[algo_lib.FImtable](#algo_lib-fimtable)||FDb.imtable (Lary)|imtable (Lary, by rowid)|ind_imtable (Thash, hash field imtable)|
|[algo_lib.FIohook](#algo_lib-fiohook)||FDb.iohook (Cppstack)|
||||FIohook.callback (Hook)|
|[algo_lib.FProc](#algo_lib-fproc)||
|[algo_lib.FReplvar](#algo_lib-freplvar)||FDb.replvar (Tpool)|
||||Replscope.ind_replvar (Thash)|
|[algo_lib.FTempfile](#algo_lib-ftempfile)||
|[algo_lib.FTxtcell](#algo_lib-ftxtcell)||FDb.txtcell (Tpool)|
||||FTxtrow.c_txtcell (Ptrary)|
|[algo_lib.FTxtrow](#algo_lib-ftxtrow)||FDb.txtrow (Tpool)|
||||FTxtcell.p_txtrow (Upptr)|
||||FTxttbl.c_txtrow (Ptrary)|
|[algo_lib.FTxttbl](#algo_lib-ftxttbl)||FDb.txttbl (Cppstack)|
||||FTxtrow.p_txttbl (Upptr)|
|[algo_lib.InTextFile](#algo_lib-intextfile)||
|[algo_lib.Mmap](#algo_lib-mmap)||
|[algo_lib.MmapFile](#algo_lib-mmapfile)||
|[algo_lib.RecSortkey](#algo_lib-recsortkey)||
|[algo_lib.RegxToken](#algo_lib-regxtoken)||
|[algo_lib.RegxExpr](#algo_lib-regxexpr)||RegxParse.ary_expr (Tary)|
|[algo_lib.RegxOp](#algo_lib-regxop)||
|[algo_lib.RegxParse](#algo_lib-regxparse)||FDb.regxparse (Cppstack)|
|[algo_lib.RegxState](#algo_lib-regxstate)||Regx.state (Tary)|
|[algo_lib.Replscope](#algo_lib-replscope)||FDb.replscope (Cppstack)|
||||FReplvar.p_replscope (Upptr)|
|[algo_lib.ShHdr](#algo_lib-shhdr)||
|[algo_lib.Srng](#algo_lib-srng)||
|[algo_lib.Tabulate](#algo_lib-tabulate)||FDb.tabulate (Cppstack)|

#### algo_lib.Bitset
<a href="#algo_lib-bitset"></a>

#### algo_lib.Bitset Fields
<a href="#algo_lib-bitset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Bitset.ary|u64|[Tary](/txt/exe/amc/reftype/Tary.md)|||

#### Struct Bitset
<a href="#struct-bitset"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Bitset { // algo_lib.Bitset
    u64*   ary_elems;   // pointer to elements
    u64    ary_n;       // number of elements in array
    u64    ary_max;     // max. capacity of array before realloc
    // Copy from aryptr (operator=)
    // func:algo_lib.Bitset.ary.AssignAryptr
    inline void          operator =(const algo::aryptr<u64> &rhs) __attribute__((nothrow));
    // func:algo_lib.Bitset.ary.CtorAryptr
    explicit inline               Bitset(const algo::aryptr<u64> &rhs) __attribute__((nothrow));
    // func:algo_lib.Bitset..AssignOp
    algo_lib::Bitset&    operator =(const algo_lib::Bitset &rhs) __attribute__((nothrow));
    // func:algo_lib.Bitset..Ctor
    inline               Bitset() __attribute__((nothrow));
    // func:algo_lib.Bitset..Dtor
    inline               ~Bitset() __attribute__((nothrow));
    // func:algo_lib.Bitset..CopyCtor
    Bitset(const algo_lib::Bitset &rhs) __attribute__((nothrow));
};
```

#### algo_lib.Cmdline - *can't move this to command namespace because of circular dependency*
<a href="#algo_lib-cmdline"></a>

#### algo_lib.Cmdline Fields
<a href="#algo_lib-cmdline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Cmdline.verbose|u8|[Val](/txt/exe/amc/reftype/Val.md)||Verbosity level (0..255)|
|algo_lib.Cmdline.debug|u8|[Val](/txt/exe/amc/reftype/Val.md)||Debug level (0..255)|
|algo_lib.Cmdline.help|bool|[Val](/txt/exe/amc/reftype/Val.md)||Print help and exit|
|algo_lib.Cmdline.version|bool|[Val](/txt/exe/amc/reftype/Val.md)||Print version and exit|
|algo_lib.Cmdline.signature|bool|[Val](/txt/exe/amc/reftype/Val.md)||Show signatures and exit|
|algo_lib.Cmdline.v|u8|[Alias](/txt/exe/amc/reftype/Alias.md)||Alias for verbose|
|algo_lib.Cmdline.d|u8|[Alias](/txt/exe/amc/reftype/Alias.md)||Alias for debug|
|algo_lib.Cmdline.sig|bool|[Alias](/txt/exe/amc/reftype/Alias.md)||Alias for signature|
|algo_lib.Cmdline.h|bool|[Alias](/txt/exe/amc/reftype/Alias.md)||Alias for help|

#### Struct Cmdline
<a href="#struct-cmdline"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Cmdline { // algo_lib.Cmdline: *can't move this to command namespace because of circular dependency*
    u8     verbose;     //   0  Verbosity level (0..255)
    u8     debug;       //   0  Debug level (0..255)
    bool   help;        //   false  Print help and exit
    bool   version;     //   false  Print version and exit
    bool   signature;   //   false  Show signatures and exit
    // func:algo_lib.Cmdline..Ctor
    inline               Cmdline() __attribute__((nothrow));
};
```

#### algo_lib.CsvParse
<a href="#algo_lib-csvparse"></a>

#### algo_lib.CsvParse Fields
<a href="#algo_lib-csvparse-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.CsvParse.input|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftype/Val.md)||Input string|
|algo_lib.CsvParse.sep|char|[Val](/txt/exe/amc/reftype/Val.md)|','|Input: separator|
|algo_lib.CsvParse.quotechar1|char|[Val](/txt/exe/amc/reftype/Val.md)|'\"'|Allow this quote|
|algo_lib.CsvParse.quotechar2|char|[Val](/txt/exe/amc/reftype/Val.md)|'\''|Allow this quote as well|
|algo_lib.CsvParse.ary_tok|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype/Tary.md)||Output: array of tokens|
|algo_lib.CsvParse.openquote|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|On output: set if unbalanced quote found|

#### Struct CsvParse
<a href="#struct-csvparse"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct CsvParse { // algo_lib.CsvParse
    algo::strptr     input;           // Input string
    char             sep;             //   ','  Input: separator
    char             quotechar1;      //   '\"'  Allow this quote
    char             quotechar2;      //   '\''  Allow this quote as well
    algo::cstring*   ary_tok_elems;   // pointer to elements
    u64              ary_tok_n;       // number of elements in array
    u64              ary_tok_max;     // max. capacity of array before realloc
    bool             openquote;       //   true  On output: set if unbalanced quote found
    // func:algo_lib.CsvParse..AssignOp
    algo_lib::CsvParse&  operator =(const algo_lib::CsvParse &rhs) __attribute__((nothrow));
    // func:algo_lib.CsvParse..Ctor
    inline               CsvParse() __attribute__((nothrow));
    // func:algo_lib.CsvParse..Dtor
    inline               ~CsvParse() __attribute__((nothrow));
    // func:algo_lib.CsvParse..CopyCtor
    CsvParse(const algo_lib::CsvParse &rhs) __attribute__((nothrow));
    // func:algo_lib.CsvParse..FieldwiseCtor
    explicit inline               CsvParse(algo::strptr in_input, char in_sep, char in_quotechar1, char in_quotechar2, bool in_openquote) __attribute__((nothrow));
};
```

#### algo_lib.ErrorX
<a href="#algo_lib-errorx"></a>

#### algo_lib.ErrorX Fields
<a href="#algo_lib-errorx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.ErrorX.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct ErrorX
<a href="#struct-errorx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct ErrorX { // algo_lib.ErrorX
    algo::cstring   str;   //
    // func:algo_lib.ErrorX..Ctor
    inline               ErrorX() __attribute__((nothrow));
    // func:algo_lib.ErrorX..FieldwiseCtor
    explicit inline               ErrorX(const algo::strptr& in_str) __attribute__((nothrow));
};
```

#### algo_lib.RegxM - Matching context for regex
<a href="#algo_lib-regxm"></a>

#### algo_lib.RegxM Fields
<a href="#algo_lib-regxm-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxM.front|[algo_lib.Bitset](/txt/gen/algo_lib/algo_lib.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftype/Val.md)||Temporary front (for matching)|
|algo_lib.RegxM.this_char|[algo_lib.Bitset](/txt/gen/algo_lib/algo_lib.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftype/Val.md)||States to test on this char|
|algo_lib.RegxM.next_char|[algo_lib.Bitset](/txt/gen/algo_lib/algo_lib.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftype/Val.md)||States to test on next char|
|algo_lib.RegxM.matchrange|[algo.I32RangeAry](/txt/protocol/algo/README.md#algo-i32rangeary)|[Val](/txt/exe/amc/reftype/Val.md)||List of match char ranges|
|algo_lib.RegxM.visited|[algo_lib.Bitset](/txt/gen/algo_lib/algo_lib.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftype/Val.md)||States already closed for this char|

#### Struct RegxM
<a href="#struct-regxm"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxM { // algo_lib.RegxM: Matching context for regex
    algo_lib::Bitset    front;        // Temporary front (for matching)
    algo_lib::Bitset    this_char;    // States to test on this char
    algo_lib::Bitset    next_char;    // States to test on next char
    algo::I32RangeAry   matchrange;   // List of match char ranges
    algo_lib::Bitset    visited;      // States already closed for this char
    // func:algo_lib.RegxM..Ctor
    inline               RegxM() __attribute__((nothrow));
};
```

#### algo_lib.FFildes - Wrapper for unix file descritor, call close() on Uninit
<a href="#algo_lib-ffildes"></a>

#### algo_lib.FFildes Fields
<a href="#algo_lib-ffildes-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FFildes.fd|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FFildes
<a href="#struct-ffildes"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FFildes { // algo_lib.FFildes: Wrapper for unix file descritor, call close() on Uninit
    algo::Fildes   fd;   //
    // func:algo_lib.FFildes..Ctor
    inline               FFildes() __attribute__((nothrow));
    // func:algo_lib.FFildes..Dtor
    inline               ~FFildes() __attribute__((nothrow));
};
```

#### algo_lib.FLockfile
<a href="#algo_lib-flockfile"></a>

#### algo_lib.FLockfile Fields
<a href="#algo_lib-flockfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FLockfile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FLockfile.fildes|[algo_lib.FFildes](/txt/gen/algo_lib/algo_lib.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FLockfile.keep|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Leave the file on cleanup: its content is a durable record beyond the lock|

#### Struct FLockfile
<a href="#struct-flockfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FLockfile { // algo_lib.FLockfile
    algo::cstring       filename;   //
    algo_lib::FFildes   fildes;     //
    bool                keep;       //   false  Leave the file on cleanup: its content is a durable record beyond the lock
    // func:algo_lib.FLockfile..Ctor
    inline               FLockfile() __attribute__((nothrow));
    // func:algo_lib.FLockfile..Dtor
    inline               ~FLockfile() __attribute__((nothrow));
};
```

#### algo_lib.FTimehook
<a href="#algo_lib-ftimehook"></a>

#### algo_lib.FTimehook Fields
<a href="#algo_lib-ftimehook-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTimehook.time|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||Time the hook is scheduled to expire|
|algo_lib.FTimehook.delay|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||Minimum delay between iterations|
|algo_lib.FTimehook.hook|[algo_lib.FTimehook](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftimehook)|[Hook](/txt/exe/amc/reftype/Hook.md)||Function to call|
|algo_lib.FTimehook.recurrent|bool|[Val](/txt/exe/amc/reftype/Val.md)||If true, automatically reschedule|

#### Struct FTimehook
<a href="#struct-ftimehook"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTimehook { // algo_lib.FTimehook
    algo::SchedTime                time;              // Time the hook is scheduled to expire
    algo::SchedTime                delay;             // Minimum delay between iterations
    algo_lib::timehook_hook_hook   hook;              //   NULL  Pointer to a function
    u64                            hook_ctx;          //   0  Callback context
    bool                           recurrent;         //   false  If true, automatically reschedule
    i32                            bh_timehook_idx;   // index in heap; -1 means not-in-heap
    // reftype Hook of algo_lib.FTimehook.hook prohibits copy
    // func:algo_lib.FTimehook..AssignOp
    inline algo_lib::FTimehook& operator =(const algo_lib::FTimehook &rhs) = delete;
    // func:algo_lib.FTimehook..Ctor
    inline               FTimehook() __attribute__((nothrow));
    // func:algo_lib.FTimehook..Dtor
    inline               ~FTimehook() __attribute__((nothrow));
    // reftype Hook of algo_lib.FTimehook.hook prohibits copy
    // func:algo_lib.FTimehook..CopyCtor
    inline               FTimehook(const algo_lib::FTimehook &rhs) = delete;
};
```

#### algo_lib.FImdb - In-memory database descriptor (reflection)
<a href="#algo_lib-fimdb"></a>

#### algo_lib.FImdb Fields
<a href="#algo_lib-fimdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FImdb.msghdr|[algo.Imdb](/txt/protocol/algo/README.md#algo-imdb)|[Base](#algo-imdb-fields)|||
#### algo.Imdb Fields
<a href="#algo-imdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo.Imdb.imdb|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Database name|
|algo.Imdb.InsertStrptrMaybe|[algo.ImdbInsertStrptrMaybeFcn](/txt/protocol/algo/README.md#algo-imdbinsertstrptrmaybefcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Insert new element given a string|
|algo.Imdb.RemoveStrptrMaybe|[algo.ImdbRemoveStrptrMaybeFcn](/txt/protocol/algo/README.md#algo-imdbremovestrptrmaybefcn)|[Val](/txt/exe/amc/reftype/Val.md)||Remove element by primary key parsed from string (NULL if unsupported)|
|algo.Imdb.Step|[algo.ImdbStepFcn](/txt/protocol/algo/README.md#algo-imdbstepfcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Perform one step (may be NULL)|
|algo.Imdb.MainLoop|[algo.ImdbMainLoopFcn](/txt/protocol/algo/README.md#algo-imdbmainloopfcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Main Loop|
|algo.Imdb.GetTrace|[algo.ImdbGetTraceFcn](/txt/protocol/algo/README.md#algo-imdbgettracefcn)|[Val](/txt/exe/amc/reftype/Val.md)|0||
|algo.Imdb.comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FImdb
<a href="#struct-fimdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FImdb { // algo_lib.FImdb
    algo::Smallstr50                 imdb;                // Database name
    algo::ImdbInsertStrptrMaybeFcn   InsertStrptrMaybe;   //   0  Insert new element given a string
    algo::ImdbRemoveStrptrMaybeFcn   RemoveStrptrMaybe;   //   0  Remove element by primary key parsed from string (NULL if unsupported)
    algo::ImdbStepFcn                Step;                //   0  Perform one step (may be NULL)
    algo::ImdbMainLoopFcn            MainLoop;            //   0  Main Loop
    algo::ImdbGetTraceFcn            GetTrace;            //   0
    algo::cstring                    comment;             //
    algo_lib::FImdb*                 ind_imdb_next;       // hash next
    u32                              ind_imdb_hashval;    // hash value
    // func:algo_lib.FImdb..AssignOp
    inline algo_lib::FImdb& operator =(const algo_lib::FImdb &rhs) = delete;
    // func:algo_lib.FImdb..Ctor
    inline               FImdb() __attribute__((nothrow));
    // func:algo_lib.FImdb..Dtor
    inline               ~FImdb() __attribute__((nothrow));
    // func:algo_lib.FImdb..CopyCtor
    inline               FImdb(const algo_lib::FImdb &rhs) = delete;
};
```

#### algo_lib.RegxFlags
<a href="#algo_lib-regxflags"></a>

#### algo_lib.RegxFlags Fields
<a href="#algo_lib-regxflags-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxFlags.value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.RegxFlags.trace|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||Enable tracing|
|algo_lib.RegxFlags.capture|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||Enable capture groups|
|algo_lib.RegxFlags.valid|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||The regx parsed successfully|
|algo_lib.RegxFlags.literal|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||The regx expression is a literal string|
|algo_lib.RegxFlags.accepts_all|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||Compiled regx accepts all intputs|
|algo_lib.RegxFlags.fullmatch|bool|[Bitfld](/txt/exe/amc/reftype/Bitfld.md)||Regx expression is a substring|

#### Struct RegxFlags
<a href="#struct-regxflags"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
#pragma pack(push,1)
struct RegxFlags { // algo_lib.RegxFlags
    u8   value;   //   0
    // func:algo_lib.RegxFlags..Ctor
    inline               RegxFlags() __attribute__((nothrow));
    // func:algo_lib.RegxFlags..FieldwiseCtor
    explicit inline               RegxFlags(u8 in_value) __attribute__((nothrow));
    // func:algo_lib.RegxFlags..EnumCtor
    inline               RegxFlags(algo_lib_RegxFlagsEnum arg) __attribute__((nothrow));
};
```

#### algo_lib.RegxStyle - Regex encoding style (Acr, Sql, Classic, Shell etc)
<a href="#algo_lib-regxstyle"></a>

#### algo_lib.RegxStyle Fields
<a href="#algo_lib-regxstyle-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxStyle.value|u8|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct RegxStyle
<a href="#struct-regxstyle"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxStyle { // algo_lib.RegxStyle: Regex encoding style (Acr, Sql, Classic, Shell etc)
    u8   value;   //   0
    // func:algo_lib.RegxStyle.value.Cast
    inline               operator algo_lib_RegxStyleEnum() const __attribute__((nothrow));
    // func:algo_lib.RegxStyle..Ctor
    inline               RegxStyle() __attribute__((nothrow));
    // func:algo_lib.RegxStyle..FieldwiseCtor
    explicit inline               RegxStyle(u8 in_value) __attribute__((nothrow));
    // func:algo_lib.RegxStyle..EnumCtor
    inline               RegxStyle(algo_lib_RegxStyleEnum arg) __attribute__((nothrow));
};
```

#### algo_lib.Regx - Parsed regular expression
<a href="#algo_lib-regx"></a>

#### algo_lib.Regx Fields
<a href="#algo_lib-regx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Regx.expr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Original string expression|
|algo_lib.Regx.state|[algo_lib.RegxState](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxstate)|[Tary](/txt/exe/amc/reftype/Tary.md)||Array of states|
|algo_lib.Regx.flags|[algo_lib.RegxFlags](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxflags)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.Regx.style|[algo_lib.RegxStyle](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxstyle)|[Val](/txt/exe/amc/reftype/Val.md)||Regx style according to which EXPR was parsed|

#### Struct Regx
<a href="#struct-regx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Regx { // algo_lib.Regx: Parsed regular expression
    algo::cstring          expr;          // Original string expression
    algo_lib::RegxState*   state_elems;   // pointer to elements
    u64                    state_n;       // number of elements in array
    u64                    state_max;     // max. capacity of array before realloc
    algo_lib::RegxFlags    flags;         //
    algo_lib::RegxStyle    style;         // Regx style according to which EXPR was parsed
    // func:algo_lib.Regx..AssignOp
    algo_lib::Regx&      operator =(const algo_lib::Regx &rhs) __attribute__((nothrow));
    // func:algo_lib.Regx..Ctor
    inline               Regx() __attribute__((nothrow));
    // func:algo_lib.Regx..Dtor
    inline               ~Regx() __attribute__((nothrow));
    // func:algo_lib.Regx..CopyCtor
    Regx(const algo_lib::Regx &rhs) __attribute__((nothrow));
};
```

#### algo_lib.FLogcat - Log category
<a href="#algo_lib-flogcat"></a>

#### algo_lib.FLogcat Fields
<a href="#algo_lib-flogcat-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FLogcat.base|[dmmeta.Logcat](/txt/ssimdb/dmmeta/logcat.md)|[Base](/txt/ssimdb/dmmeta/logcat.md)|||
|algo_lib.FLogcat.filter|[algo_lib.Regx](/txt/gen/algo_lib/algo_lib.md#algo_lib-regx)|[Val](/txt/exe/amc/reftype/Val.md)||Output filter|
|algo_lib.FLogcat.negfilter|[algo_lib.Regx](/txt/gen/algo_lib/algo_lib.md#algo_lib-regx)|[Val](/txt/exe/amc/reftype/Val.md)||Negative output filter|
|algo_lib.FLogcat.nmsg|i64|[Val](/txt/exe/amc/reftype/Val.md)||Number of messages printed inside current window|
|algo_lib.FLogcat.th_throttle|[algo_lib.FTimehook](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftimehook)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FLogcat.changed|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FLogcat.totmsg|u64|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FLogcat
<a href="#struct-flogcat"></a>
*Note:* field ``algo_lib.FLogcat.base`` has reftype ``base`` so the fields of [dmmeta.Logcat](/txt/ssimdb/dmmeta/logcat.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FLogcat { // algo_lib.FLogcat
    algo::Smallstr50      logcat;               //
    bool                  enabled;              //   false
    bool                  builtin;              //   false
    bool                  stdout;               //   false
    i32                   maxmsg;               //   0
    i32                   window;               //   0
    bool                  suppress;             //   false  Throttle is suppressing this category for the rest of its window; enabled is the operator's switch
    algo::cstring         comment;              //
    algo_lib::Regx        filter;               // Output filter
    algo_lib::Regx        negfilter;            // Negative output filter
    i64                   nmsg;                 //   0  Number of messages printed inside current window
    algo_lib::FTimehook   th_throttle;          //
    algo::SchedTime       changed;              //
    u64                   totmsg;               //   0
    algo_lib::FLogcat*    ind_logcat_next;      // hash next
    u32                   ind_logcat_hashval;   // hash value
    // value field algo_lib.FLogcat.th_throttle is not copiable
    // func:algo_lib.FLogcat..AssignOp
    algo_lib::FLogcat&   operator =(const algo_lib::FLogcat &rhs) = delete;
    // func:algo_lib.FLogcat..Ctor
    inline               FLogcat() __attribute__((nothrow));
    // func:algo_lib.FLogcat..Dtor
    inline               ~FLogcat() __attribute__((nothrow));
    // value field algo_lib.FLogcat.th_throttle is not copiable
    // func:algo_lib.FLogcat..CopyCtor
    FLogcat(const algo_lib::FLogcat &rhs) = delete;
};
```

#### algo_lib.FErrns
<a href="#algo_lib-ferrns"></a>

#### algo_lib.FErrns Fields
<a href="#algo_lib-ferrns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FErrns.errns|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FErrns.decode|i32|[Hook](/txt/exe/amc/reftype/Hook.md)|||
|algo_lib.FErrns.outstr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Output of decode operation|

#### Struct FErrns
<a href="#struct-ferrns"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FErrns { // algo_lib.FErrns
    i32                           errns;        //   0
    algo_lib::errns_decode_hook   decode;       //   NULL  Pointer to a function
    u64                           decode_ctx;   //   0  Callback context
    algo::cstring                 outstr;       // Output of decode operation
    // func:algo_lib.FErrns..Ctor
    inline               FErrns() __attribute__((nothrow));
};
```

#### algo_lib.FDb - In-memory database for algo_lib
<a href="#algo_lib-fdb"></a>

#### algo_lib.FDb Fields
<a href="#algo_lib-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FDb.cstring|[algo.cstring](/txt/protocol/algo/cstring.md)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)|||
|algo_lib.FDb.sbrk|u8|[Sbrk](/txt/exe/amc/reftype/Sbrk.md)||Base allocator for everything|
|algo_lib.FDb.lpool|u8|[Lpool](/txt/exe/amc/reftype/Lpool.md)||private memory pool|
|algo_lib.FDb.next_loop|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.limit|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)|0x7fffffffffffffff|Main loop clock limit|
|algo_lib.FDb.clocks_to_ms|double|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.n_iohook|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of iohooks in epoll|
|algo_lib.FDb.clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||Most recent cpu clock value|
|algo_lib.FDb.step_limit|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||Execution limit for current step|
|algo_lib.FDb.start_clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||cpu clock value at startup|
|algo_lib.FDb.hz|double|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.t_last_signal|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||Time last async signal was processed|
|algo_lib.FDb.exit_code|i32|[Val](/txt/exe/amc/reftype/Val.md)|0|Unix exit code from main program. 0 = success|
|algo_lib.FDb.clocks_to_ns|double|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.n_temp|u32|[Val](/txt/exe/amc/reftype/Val.md)|0||
|algo_lib.FDb.last_signal|u32|[Val](/txt/exe/amc/reftype/Val.md)||Value of last signal (used by SetupExitSignals)|
|algo_lib.FDb.cpu_hz|u64|[Val](/txt/exe/amc/reftype/Val.md)||Cpu HZ, determined at startup|
|algo_lib.FDb.fildes|[algo_lib.FFildes](/txt/gen/algo_lib/algo_lib.md#algo_lib-ffildes)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.temp_strings|[algo.cstring](/txt/protocol/algo/cstring.md)|[Inlary](/txt/exe/amc/reftype/Inlary.md)|||
|algo_lib.FDb.ArgvIdent|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.BashQuotesafe|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.RegxSqlSpecial|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.SsimBreakName|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.SsimBreakValue|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.SsimQuotesafe|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.bh_timehook|[algo_lib.FTimehook](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftimehook)|[Bheap](/txt/exe/amc/reftype/Bheap.md)||Binary heap of time-based callbacks|
|algo_lib.FDb._db|[algo_lib.FDb](/txt/gen/algo_lib/algo_lib.md#algo_lib-fdb)|[Global](/txt/exe/amc/reftype/Global.md)||* initialization order is important *|
|algo_lib.FDb.imtable|[algo_lib.FImtable](/txt/gen/algo_lib/algo_lib.md#algo_lib-fimtable)|[Lary](/txt/exe/amc/reftype/Lary.md)||Array of all in-memory tables linked into this process|
|algo_lib.FDb.ind_imtable|[algo_lib.FImtable](/txt/gen/algo_lib/algo_lib.md#algo_lib-fimtable)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|algo_lib.FDb.iohook|[algo_lib.FIohook](/txt/lib/algo_lib/FIohook.md)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.timehook|[algo_lib.FTimehook](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftimehook)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.replscope|[algo_lib.Replscope](/txt/gen/algo_lib/algo_lib.md#algo_lib-replscope)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.error|[algo_lib.ErrorX](/txt/gen/algo_lib/algo_lib.md#algo_lib-errorx)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)|||
|algo_lib.FDb.csvparse|[algo_lib.CsvParse](/txt/gen/algo_lib/algo_lib.md#algo_lib-csvparse)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.regxparse|[algo_lib.RegxParse](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxparse)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.regx|[algo_lib.Regx](/txt/gen/algo_lib/algo_lib.md#algo_lib-regx)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.regxm|[algo_lib.RegxM](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxm)|[Val](/txt/exe/amc/reftype/Val.md)||Context for Regx matching|
|algo_lib.FDb.tabulate|[algo_lib.Tabulate](/txt/gen/algo_lib/algo_lib.md#algo_lib-tabulate)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.log_str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.epoll_fd|i32|[Val](/txt/exe/amc/reftype/Val.md)|-1||
|algo_lib.FDb.lock_core|[algo_lib.FLockfile](/txt/gen/algo_lib/algo_lib.md#algo_lib-flockfile)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.c_timehook|[algo_lib.FTimehook](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftimehook)|[Ptr](/txt/exe/amc/reftype/Ptr.md)||TEMP: here only for dependency reasons|
|algo_lib.FDb._timehook|[algo_lib.FTimehook](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftimehook)|[Val](/txt/exe/amc/reftype/Val.md)||Keep me here i'm special|
|algo_lib.FDb.dispsigcheck|[algo_lib.FDispsigcheck](/txt/gen/algo_lib/algo_lib.md#algo_lib-fdispsigcheck)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|algo_lib.FDb.ind_dispsigcheck|[algo_lib.FDispsigcheck](/txt/gen/algo_lib/algo_lib.md#algo_lib-fdispsigcheck)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|algo_lib.FDb.imdb|[algo_lib.FImdb](/txt/gen/algo_lib/algo_lib.md#algo_lib-fimdb)|[Inlary](/txt/exe/amc/reftype/Inlary.md)|||
|algo_lib.FDb.ind_imdb|[algo_lib.FImdb](/txt/gen/algo_lib/algo_lib.md#algo_lib-fimdb)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|algo_lib.FDb.malloc|u8|[Malloc](/txt/exe/amc/reftype/Malloc.md)||Pool for everything else|
|algo_lib.FDb.txtcell|[algo_lib.FTxtcell](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxtcell)|[Tpool](/txt/exe/amc/reftype/Tpool.md)|||
|algo_lib.FDb.txtrow|[algo_lib.FTxtrow](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxtrow)|[Tpool](/txt/exe/amc/reftype/Tpool.md)|||
|algo_lib.FDb.txttbl|[algo_lib.FTxttbl](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxttbl)|[Cppstack](/txt/exe/amc/reftype/Cppstack.md)||Provides default name for variables of this type|
|algo_lib.FDb.argc|i32|[Val](/txt/exe/amc/reftype/Val.md)||Argc from main|
|algo_lib.FDb.argv|char*|[Ptr](/txt/exe/amc/reftype/Ptr.md)||Argv from main|
|algo_lib.FDb.xref_error|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.errtext|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.varlenbuf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||
|algo_lib.FDb.replvar|[algo_lib.FReplvar](/txt/gen/algo_lib/algo_lib.md#algo_lib-freplvar)|[Tpool](/txt/exe/amc/reftype/Tpool.md)|||
|algo_lib.FDb.cmdline|[algo_lib.Cmdline](/txt/gen/algo_lib/algo_lib.md#algo_lib-cmdline)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.h_fatalerror||[Hook](/txt/exe/amc/reftype/Hook.md)|||
|algo_lib.FDb.giveup_count|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.fatalerr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.stringtofile_nwrite|u32|[Val](/txt/exe/amc/reftype/Val.md)||Global counter of # of files written|
|algo_lib.FDb.last_sleep_clocks|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.msgtemp|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.DigitChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.NewLineChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.WhiteChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.DirSep|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.IdentChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.IdentStart|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.AlphaChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.HexChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.UpperChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.CmdLineNameBreak|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.CmdLineValueBreak|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.WordSeparator|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.LowerChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.Urlsafe|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype/Charset.md)|||
|algo_lib.FDb.winjob|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.Prlog|[algo.PrlogFcn](/txt/protocol/algo/PrlogFcn.md)|[Val](/txt/exe/amc/reftype/Val.md)|algo::Prlog||
|algo_lib.FDb.logcat|[algo_lib.FLogcat](/txt/gen/algo_lib/algo_lib.md#algo_lib-flogcat)|[Inlary](/txt/exe/amc/reftype/Inlary.md)|||
|algo_lib.FDb.ind_logcat|[algo_lib.FLogcat](/txt/gen/algo_lib/algo_lib.md#algo_lib-flogcat)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|algo_lib.FDb.tstamp_fmt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|"%Y/%m/%dT%H:%M:%S.%.6X "||
|algo_lib.FDb.exec_args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype/Tary.md)|||
|algo_lib.FDb.dirstack|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype/Tary.md)||Directory stack for PushDir/PopDir|
|algo_lib.FDb.errns|[algo_lib.FErrns](/txt/gen/algo_lib/algo_lib.md#algo_lib-ferrns)|[Inlary](/txt/exe/amc/reftype/Inlary.md)||Fixed table of errns decoders|
|algo_lib.FDb.tempdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.use_epoll_pwait2|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Use epoll_pwait2 system call|
|algo_lib.FDb.pending_eol|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.giveup_time|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Trigger for giveup_time loop|
|algo_lib.FDb.show_tstamp|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FDb.exit_signal|i32|[Val](/txt/exe/amc/reftype/Val.md)|0|Signal that terminated the last exported child; 0 when it exited|
|algo_lib.FDb.fatalerr_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Path a fail-stop writes its report to; empty = stderr only|
|algo_lib.FDb.in_fatalerr|bool|[Val](/txt/exe/amc/reftype/Val.md)||A fail-stop is already reporting; a second one only states its cause|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FDb { // algo_lib.FDb: In-memory database for algo_lib
    u64                               sbrk_huge_limit;                               // Huge page limit in bytes -- set to 0 with fork!
    u64                               sbrk_huge_alloc;                               // Huge page bytes allocated
    bool                              sbrk_zeromem;                                  // Zero allocated memory
    lpool_Lpblock*                    lpool_free[36];                                // Lpool levels
    lpool_Lpblk*                      lpool_blk[11];                                 // Dedicated blks with free space, per class
    algo::SchedTime                   next_loop;                                     //
    algo::SchedTime                   limit;                                         //   0x7fffffffffffffff  Main loop clock limit
    double                            clocks_to_ms;                                  //   0.0
    u32                               n_iohook;                                      //   0  Number of iohooks in epoll
    algo::SchedTime                   clock;                                         // Most recent cpu clock value
    algo::SchedTime                   step_limit;                                    // Execution limit for current step
    algo::SchedTime                   start_clock;                                   // cpu clock value at startup
    double                            hz;                                            //   0.0
    algo::SchedTime                   t_last_signal;                                 // Time last async signal was processed
    i32                               exit_code;                                     //   0  Unix exit code from main program. 0 = success
    double                            clocks_to_ns;                                  //   0.0
    u32                               n_temp;                                        //   0
    u32                               last_signal;                                   //   0  Value of last signal (used by SetupExitSignals)
    u64                               cpu_hz;                                        //   0  Cpu HZ, determined at startup
    algo::cstring                     temp_strings_elems[8];                         // fixed array
    algo::Charset                     ArgvIdent;                                     //
    algo::Charset                     BashQuotesafe;                                 //
    algo::Charset                     RegxSqlSpecial;                                //
    algo::Charset                     SsimBreakName;                                 //
    algo::Charset                     SsimBreakValue;                                //
    algo::Charset                     SsimQuotesafe;                                 //
    algo_lib::FTimehook**             bh_timehook_elems;                             // binary heap by time
    i32                               bh_timehook_n;                                 // number of elements in the heap
    i32                               bh_timehook_max;                               // max elements in bh_timehook_elems
    algo_lib::FImtable*               imtable_lary[36];                              // level array
    i64                               imtable_n;                                     // number of elements in array
    algo_lib::FImtable**              ind_imtable_buckets_elems;                     // pointer to bucket array
    i32                               ind_imtable_buckets_n;                         // number of elements in bucket array
    i32                               ind_imtable_n;                                 // number of elements in the hash table
    algo_lib::RegxM                   regxm;                                         // Context for Regx matching
    algo::cstring                     log_str;                                       //
    i32                               epoll_fd;                                      //   -1
    algo_lib::FLockfile               lock_core;                                     //
    algo_lib::FTimehook*              c_timehook;                                    // TEMP: here only for dependency reasons. optional pointer
    algo_lib::FTimehook               _timehook;                                     // Keep me here i'm special
    algo_lib::FDispsigcheck*          dispsigcheck_lary[36];                         // level array
    i64                               dispsigcheck_n;                                // number of elements in array
    algo_lib::FDispsigcheck**         ind_dispsigcheck_buckets_elems;                // pointer to bucket array
    i32                               ind_dispsigcheck_buckets_n;                    // number of elements in bucket array
    i32                               ind_dispsigcheck_n;                            // number of elements in the hash table
    alignas(algo_lib::FImdb) u8       imdb_data[sizeof(algo_lib::FImdb) * 32];       // place for data
    i32                               imdb_n;                                        // number of elems current in existence
    enum { imdb_max = 32 };
    algo_lib::FImdb**                 ind_imdb_buckets_elems;                        // pointer to bucket array
    i32                               ind_imdb_buckets_n;                            // number of elements in bucket array
    i32                               ind_imdb_n;                                    // number of elements in the hash table
    u64                               txtcell_blocksize;                             // # bytes per block
    algo_lib::FTxtcell*               txtcell_free;                                  //
    u64                               txtrow_blocksize;                              // # bytes per block
    algo_lib::FTxtrow*                txtrow_free;                                   //
    i32                               argc;                                          //   0  Argc from main
    char**                            argv;                                          // Argv from main. optional pointer
    algo::cstring                     xref_error;                                    //
    algo::cstring                     errtext;                                       //
    algo::ByteAry*                    varlenbuf;                                     // optional pointer
    u64                               replvar_blocksize;                             // # bytes per block
    algo_lib::FReplvar*               replvar_free;                                  //
    algo_lib::Cmdline                 cmdline;                                       //
    algo_lib::_db_h_fatalerror_hook   h_fatalerror;                                  //   NULL  Pointer to a function
    u64                               h_fatalerror_ctx;                              //   0  Callback context
    u64                               giveup_count;                                  //   0
    algo::cstring                     fatalerr;                                      //
    u32                               stringtofile_nwrite;                           //   0  Global counter of # of files written
    u64                               last_sleep_clocks;                             //   0
    algo::ByteAry                     msgtemp;                                       //
    algo::Charset                     Urlsafe;                                       //
    u64                               winjob;                                        //   0
    algo::PrlogFcn                    Prlog;                                         //   algo::Prlog
    alignas(algo_lib::FLogcat) u8     logcat_data[sizeof(algo_lib::FLogcat) * 12];   // place for data
    i32                               logcat_n;                                      // number of elems current in existence
    enum { logcat_max = 12 };
    algo_lib::FLogcat**               ind_logcat_buckets_elems;                      // pointer to bucket array
    i32                               ind_logcat_buckets_n;                          // number of elements in bucket array
    i32                               ind_logcat_n;                                  // number of elements in the hash table
    algo::cstring                     tstamp_fmt;                                    //   "%Y/%m/%dT%H:%M:%S.%.6X "
    algo::cstring*                    exec_args_elems;                               // pointer to elements
    u64                               exec_args_n;                                   // number of elements in array
    u64                               exec_args_max;                                 // max. capacity of array before realloc
    algo::cstring*                    dirstack_elems;                                // pointer to elements
    u64                               dirstack_n;                                    // number of elements in array
    u64                               dirstack_max;                                  // max. capacity of array before realloc
    algo_lib::FErrns                  errns_elems[8];                                // fixed array
    algo::cstring                     tempdir;                                       //
    bool                              use_epoll_pwait2;                              //   false  Use epoll_pwait2 system call
    bool                              pending_eol;                                   //   false
    bool                              giveup_time;                                   //   true  Trigger for giveup_time loop
    bool                              show_tstamp;                                   //   false
    i32                               exit_signal;                                   //   0  Signal that terminated the last exported child; 0 when it exited
    algo::cstring                     fatalerr_file;                                 // Path a fail-stop writes its report to; empty = stderr only
    bool                              in_fatalerr;                                   //   false  A fail-stop is already reporting; a second one only states its cause
    algo_lib::trace                   trace;                                         //
};
```

#### algo_lib.FDispsigcheck - Check signature of input data against executable's version
<a href="#algo_lib-fdispsigcheck"></a>

#### algo_lib.FDispsigcheck Fields
<a href="#algo_lib-fdispsigcheck-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FDispsigcheck.msghdr|[dmmeta.Dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|[Base](/txt/ssimdb/dmmeta/dispsigcheck.md)|||

#### Struct FDispsigcheck
<a href="#struct-fdispsigcheck"></a>
*Note:* field ``algo_lib.FDispsigcheck.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FDispsigcheck { // algo_lib.FDispsigcheck
    algo_lib::FDispsigcheck*   ind_dispsigcheck_next;      // hash next
    u32                        ind_dispsigcheck_hashval;   // hash value
    algo::Smallstr50           dispsig;                    //
    algo::Signature            signature;                  //
    // func:algo_lib.FDispsigcheck..AssignOp
    inline algo_lib::FDispsigcheck& operator =(const algo_lib::FDispsigcheck &rhs) = delete;
    // func:algo_lib.FDispsigcheck..CopyCtor
    inline               FDispsigcheck(const algo_lib::FDispsigcheck &rhs) = delete;
private:
    // func:algo_lib.FDispsigcheck..Ctor
    inline               FDispsigcheck() __attribute__((nothrow));
    // func:algo_lib.FDispsigcheck..Dtor
    inline               ~FDispsigcheck() __attribute__((nothrow));
    friend algo_lib::FDispsigcheck& dispsigcheck_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend algo_lib::FDispsigcheck* dispsigcheck_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispsigcheck_RemoveLast() __attribute__((nothrow));
};
```

#### algo_lib.FImtable - In-memory table descriptor
<a href="#algo_lib-fimtable"></a>

#### algo_lib.FImtable Fields
<a href="#algo_lib-fimtable-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FImtable.msghdr|[algo.Imtable](/txt/protocol/algo/Imtable.md)|[Base](#algo-imtable-fields)|||
#### algo.Imtable Fields
<a href="#algo-imtable-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo.Imtable.imtable|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Table name|
|algo.Imtable.elem_type|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Element type name|
|algo.Imtable.c_RowidFind|[algo.ImrowRowidFindFcn](/txt/protocol/algo/README.md#algo-imrowrowidfindfcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Function to find element by rowid (may be NULL)|
|algo.Imtable.XrefX|[algo.ImrowXrefXFcn](/txt/protocol/algo/README.md#algo-imrowxrefxfcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Function to x-reference an element (may be NULL)|
|algo.Imtable.NItems|[algo.ImrowNItemsFcn](/txt/protocol/algo/README.md#algo-imrownitemsfcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Return number of elements in the table|
|algo.Imtable.Print|[algo.ImrowPrintFcn](/txt/protocol/algo/README.md#algo-imrowprintfcn)|[Val](/txt/exe/amc/reftype/Val.md)|0|Convert specified element to string (may be NULL)|
|algo.Imtable.size|i32|[Val](/txt/exe/amc/reftype/Val.md)||Size of one element (for fixed-width elements only)|
|algo.Imtable.ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Ssimfile name (if associated)|
|algo.Imtable.comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FImtable
<a href="#struct-fimtable"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FImtable { // algo_lib.FImtable
    algo_lib::FImtable*       ind_imtable_next;      // hash next
    u32                       ind_imtable_hashval;   // hash value
    algo::Smallstr50          imtable;               // Table name
    algo::Smallstr100         elem_type;             // Element type name
    algo::ImrowRowidFindFcn   c_RowidFind;           //   0  Function to find element by rowid (may be NULL)
    algo::ImrowXrefXFcn       XrefX;                 //   0  Function to x-reference an element (may be NULL)
    algo::ImrowNItemsFcn      NItems;                //   0  Return number of elements in the table
    algo::ImrowPrintFcn       Print;                 //   0  Convert specified element to string (may be NULL)
    i32                       size;                  //   0  Size of one element (for fixed-width elements only)
    algo::Smallstr50          ssimfile;              // Ssimfile name (if associated)
    algo::cstring             comment;               //
    // func:algo_lib.FImtable..AssignOp
    algo_lib::FImtable&  operator =(const algo_lib::FImtable &rhs) = delete;
    // func:algo_lib.FImtable..CopyCtor
    FImtable(const algo_lib::FImtable &rhs) = delete;
private:
    // func:algo_lib.FImtable..Ctor
    inline               FImtable() __attribute__((nothrow));
    // func:algo_lib.FImtable..Dtor
    inline               ~FImtable() __attribute__((nothrow));
    friend algo_lib::FImtable&  imtable_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend algo_lib::FImtable*  imtable_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 imtable_RemoveAll() __attribute__((nothrow));
    friend void                 imtable_RemoveLast() __attribute__((nothrow));
};
```

#### algo_lib.FIohook
<a href="#algo_lib-fiohook"></a>

#### algo_lib.FIohook Fields
<a href="#algo_lib-fiohook-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FIohook.callback|[algo_lib.FIohook](/txt/lib/algo_lib/FIohook.md)|[Hook](/txt/exe/amc/reftype/Hook.md)|||
|algo_lib.FIohook.fildes|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftype/Val.md)||File descriptor, possibly in epoll|
|algo_lib.FIohook.evt_flags|[algo.IOEvtFlags](/txt/protocol/algo/IOEvtFlags.md)|[Val](/txt/exe/amc/reftype/Val.md)||Flags subscribed to|
|algo_lib.FIohook.flags|[algo.IOEvtFlags](/txt/protocol/algo/IOEvtFlags.md)|[Val](/txt/exe/amc/reftype/Val.md)||Flags during callback|
|algo_lib.FIohook.in_epoll|bool|[Val](/txt/exe/amc/reftype/Val.md)||Registered in epoll?|
|algo_lib.FIohook.nodelete|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|File descriptor is shared -- do not close()|

#### Struct FIohook
<a href="#struct-fiohook"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FIohook { // algo_lib.FIohook
    algo_lib::iohook_callback_hook   callback;       //   NULL  Pointer to a function
    u64                              callback_ctx;   //   0  Callback context
    algo::Fildes                     fildes;         // File descriptor, possibly in epoll
    algo::IOEvtFlags                 evt_flags;      // Flags subscribed to
    algo::IOEvtFlags                 flags;          // Flags during callback
    bool                             in_epoll;       //   false  Registered in epoll?
    bool                             nodelete;       //   false  File descriptor is shared -- do not close()
    // func:algo_lib.FIohook..Ctor
    inline               FIohook() __attribute__((nothrow));
    // func:algo_lib.FIohook..Dtor
    inline               ~FIohook() __attribute__((nothrow));
};
```

#### algo_lib.FProc
<a href="#algo_lib-fproc"></a>

#### algo_lib.FProc Fields
<a href="#algo_lib-fproc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FProc.args|[algo.StringAry](/txt/protocol/algo/README.md#algo-stringary)|[Val](/txt/exe/amc/reftype/Val.md)||argv; args[0] is the executable path|
|algo_lib.FProc.fstdin|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||stdin redirect (<file, <&fd, |)|
|algo_lib.FProc.fstdout|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||stdout redirect (>file, >>file, >&fd, |)|
|algo_lib.FProc.fstderr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||stderr redirect (>file, >&fd, |; >&1 merges into stdout)|
|algo_lib.FProc.to_stdin|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftype/Val.md)||write end of stdin pipe when fstdin==|; closed by ProcWait|
|algo_lib.FProc.from_stdout|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftype/Val.md)||read end of stdout pipe when fstdout==|; closed by ProcWait|
|algo_lib.FProc.from_stderr|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftype/Val.md)||read end of stderr pipe when fstderr==|; closed by ProcWait|
|algo_lib.FProc.pid|pid_t|[Val](/txt/exe/amc/reftype/Val.md)||pid of running child; 0 when not running|
|algo_lib.FProc.timeout|i32|[Val](/txt/exe/amc/reftype/Val.md)||alarm seconds before SIGALRM; 0 = none|
|algo_lib.FProc.status|i32|[Val](/txt/exe/amc/reftype/Val.md)||last wait() status|
|algo_lib.FProc.cloexec|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|set O_CLOEXEC on the pipe ends so they don't leak into later-spawned children|
|algo_lib.FProc.pgroup|bool|[Val](/txt/exe/amc/reftype/Val.md)||run the child in its own process group; kills then target the whole group|

#### Struct FProc
<a href="#struct-fproc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FProc { // algo_lib.FProc
    algo::StringAry   args;          // argv; args[0] is the executable path
    algo::cstring     fstdin;        // stdin redirect (<file, <&fd, |)
    algo::cstring     fstdout;       // stdout redirect (>file, >>file, >&fd, |)
    algo::cstring     fstderr;       // stderr redirect (>file, >&fd, |; >&1 merges into stdout)
    algo::Fildes      to_stdin;      // write end of stdin pipe when fstdin==|; closed by ProcWait
    algo::Fildes      from_stdout;   // read end of stdout pipe when fstdout==|; closed by ProcWait
    algo::Fildes      from_stderr;   // read end of stderr pipe when fstderr==|; closed by ProcWait
    pid_t             pid;           //   0  pid of running child; 0 when not running
    i32               timeout;       //   0  alarm seconds before SIGALRM; 0 = none
    i32               status;        //   0  last wait() status
    bool              cloexec;       //   true  set O_CLOEXEC on the pipe ends so they don't leak into later-spawned children
    bool              pgroup;        //   false  run the child in its own process group; kills then target the whole group
    // func:algo_lib.FProc..Ctor
    inline               FProc() __attribute__((nothrow));
    // func:algo_lib.FProc..Dtor
    inline               ~FProc() __attribute__((nothrow));
};
```

#### algo_lib.FReplvar
<a href="#algo_lib-freplvar"></a>

#### algo_lib.FReplvar Fields
<a href="#algo_lib-freplvar-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FReplvar.p_replscope|[algo_lib.Replscope](/txt/gen/algo_lib/algo_lib.md#algo_lib-replscope)|[Upptr](/txt/exe/amc/reftype/Upptr.md)||Parent|
|algo_lib.FReplvar.key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Key|
|algo_lib.FReplvar.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Value|
|algo_lib.FReplvar.nsubst|i32|[Val](/txt/exe/amc/reftype/Val.md)||Number of times variable accessed|
|algo_lib.FReplvar.partial|bool|[Val](/txt/exe/amc/reftype/Val.md)||This is a partial match on another variable|

#### Struct FReplvar
<a href="#struct-freplvar"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FReplvar { // algo_lib.FReplvar
    algo_lib::FReplvar*    replvar_next;                    // Pointer to next free element int tpool
    algo_lib::Replscope*   p_replscope;                     // reference to parent row
    algo::cstring          key;                             // Key
    algo::cstring          value;                           // Value
    i32                    nsubst;                          //   0  Number of times variable accessed
    bool                   partial;                         //   false  This is a partial match on another variable
    algo_lib::FReplvar*    replscope_ind_replvar_next;      // hash next
    u32                    replscope_ind_replvar_hashval;   // hash value
    // func:algo_lib.FReplvar..AssignOp
    inline algo_lib::FReplvar& operator =(const algo_lib::FReplvar &rhs) = delete;
    // func:algo_lib.FReplvar..CopyCtor
    inline               FReplvar(const algo_lib::FReplvar &rhs) = delete;
private:
    // func:algo_lib.FReplvar..Ctor
    inline               FReplvar() __attribute__((nothrow));
    // func:algo_lib.FReplvar..Dtor
    inline               ~FReplvar() __attribute__((nothrow));
    friend algo_lib::FReplvar&  replvar_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend algo_lib::FReplvar*  replvar_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 replvar_Delete(algo_lib::FReplvar &row) __attribute__((nothrow));
};
```

#### algo_lib.FTempfile
<a href="#algo_lib-ftempfile"></a>

#### algo_lib.FTempfile Fields
<a href="#algo_lib-ftempfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTempfile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.FTempfile.fildes|[algo_lib.FFildes](/txt/gen/algo_lib/algo_lib.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FTempfile
<a href="#struct-ftempfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTempfile { // algo_lib.FTempfile
    algo::cstring       filename;   //
    algo_lib::FFildes   fildes;     //
    // func:algo_lib.FTempfile..Ctor
    inline               FTempfile() __attribute__((nothrow));
    // func:algo_lib.FTempfile..Dtor
    inline               ~FTempfile() __attribute__((nothrow));
};
```

#### algo_lib.FTxtcell
<a href="#algo_lib-ftxtcell"></a>

#### algo_lib.FTxtcell Fields
<a href="#algo_lib-ftxtcell-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTxtcell.p_txtrow|[algo_lib.FTxtrow](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxtrow)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|algo_lib.FTxtcell.justify|[algo.TextJust](/txt/protocol/algo/TextJust.md)|[Val](/txt/exe/amc/reftype/Val.md)|algo_TextJust_j_left|Justification of text within cell|
|algo_lib.FTxtcell.style|[algo.TermStyle](/txt/protocol/algo/TermStyle.md)|[Val](/txt/exe/amc/reftype/Val.md)||Text style|
|algo_lib.FTxtcell.span|i32|[Val](/txt/exe/amc/reftype/Val.md)|1|Column span of this cell|
|algo_lib.FTxtcell.width|i32|[Val](/txt/exe/amc/reftype/Val.md)|0|Width in chars|
|algo_lib.FTxtcell.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Cell contents|
|algo_lib.FTxtcell.rsep|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Right separator|

#### Struct FTxtcell
<a href="#struct-ftxtcell"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTxtcell { // algo_lib.FTxtcell
    algo_lib::FTxtcell*   txtcell_next;              // Pointer to next free element int tpool
    algo_lib::FTxtrow*    p_txtrow;                  // reference to parent row
    algo::TextJust        justify;                   //   algo_TextJust_j_left  Justification of text within cell
    algo::TermStyle       style;                     // Text style
    i32                   span;                      //   1  Column span of this cell
    i32                   width;                     //   0  Width in chars
    algo::cstring         text;                      // Cell contents
    algo::cstring         rsep;                      // Right separator
    bool                  txtrow_c_txtcell_in_ary;   //   false  membership flag
    // func:algo_lib.FTxtcell..AssignOp
    inline algo_lib::FTxtcell& operator =(const algo_lib::FTxtcell &rhs) = delete;
    // func:algo_lib.FTxtcell..CopyCtor
    inline               FTxtcell(const algo_lib::FTxtcell &rhs) = delete;
    // func:algo_lib.FTxtcell..FieldwiseCtor
    explicit inline               FTxtcell(algo_lib::FTxtrow* in_p_txtrow, algo::TextJust in_justify, algo::TermStyle in_style, i32 in_span, i32 in_width, const algo::strptr& in_text, const algo::strptr& in_rsep, bool in_txtrow_c_txtcell_in_ary) __attribute__((nothrow));
private:
    // func:algo_lib.FTxtcell..Ctor
    inline               FTxtcell() __attribute__((nothrow));
    // func:algo_lib.FTxtcell..Dtor
    inline               ~FTxtcell() __attribute__((nothrow));
    friend algo_lib::FTxtcell&  txtcell_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend algo_lib::FTxtcell*  txtcell_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 txtcell_Delete(algo_lib::FTxtcell &row) __attribute__((nothrow));
};
```

#### algo_lib.FTxtrow - Table row. Todo: absolute index for cells?
<a href="#algo_lib-ftxtrow"></a>

#### algo_lib.FTxtrow Fields
<a href="#algo_lib-ftxtrow-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTxtrow.p_txttbl|[algo_lib.FTxttbl](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxttbl)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|algo_lib.FTxtrow.select|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Select for processing|
|algo_lib.FTxtrow.ishdr|bool|[Val](/txt/exe/amc/reftype/Val.md)|false|Is header row|
|algo_lib.FTxtrow.sortkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Sort key|
|algo_lib.FTxtrow.c_txtcell|[algo_lib.FTxtcell](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxtcell)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||

#### Struct FTxtrow
<a href="#struct-ftxtrow"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTxtrow { // algo_lib.FTxtrow: Table row. Todo: absolute index for cells?
    algo_lib::FTxtrow*     txtrow_next;              // Pointer to next free element int tpool
    algo_lib::FTxttbl*     p_txttbl;                 // reference to parent row
    bool                   select;                   //   true  Select for processing
    bool                   ishdr;                    //   false  Is header row
    algo::cstring          sortkey;                  // Sort key
    algo_lib::FTxtcell**   c_txtcell_elems;          // array of pointers
    u64                    c_txtcell_n;              // current size
    u64                    c_txtcell_max;            // capacity of allocated array
    bool                   txttbl_c_txtrow_in_ary;   //   false  membership flag
    // reftype Ptrary of algo_lib.FTxtrow.c_txtcell prohibits copy
    // func:algo_lib.FTxtrow..AssignOp
    inline algo_lib::FTxtrow& operator =(const algo_lib::FTxtrow &rhs) = delete;
    // reftype Ptrary of algo_lib.FTxtrow.c_txtcell prohibits copy
    // func:algo_lib.FTxtrow..CopyCtor
    inline               FTxtrow(const algo_lib::FTxtrow &rhs) = delete;
private:
    // func:algo_lib.FTxtrow..Ctor
    inline               FTxtrow() __attribute__((nothrow));
    // func:algo_lib.FTxtrow..Dtor
    inline               ~FTxtrow() __attribute__((nothrow));
    friend algo_lib::FTxtrow&   txtrow_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend algo_lib::FTxtrow*   txtrow_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 txtrow_Delete(algo_lib::FTxtrow &row) __attribute__((nothrow));
};
```

#### algo_lib.FTxttbl - Table row. Todo: absolute index for cells?
<a href="#algo_lib-ftxttbl"></a>

#### algo_lib.FTxttbl Fields
<a href="#algo_lib-ftxttbl-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTxttbl.c_txtrow|[algo_lib.FTxtrow](/txt/gen/algo_lib/algo_lib.md#algo_lib-ftxtrow)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)||Array of rows|
|algo_lib.FTxttbl.col_space|i32|[Val](/txt/exe/amc/reftype/Val.md)|2|Default extra space between columns|
|algo_lib.FTxttbl.normalized|bool|[Val](/txt/exe/amc/reftype/Val.md)||Cell widths computed|
|algo_lib.FTxttbl.style|bool|[Val](/txt/exe/amc/reftype/Val.md)||Emit cell color styles even when stdout is not a terminal (e.g. output relayed to a remote terminal)|

#### Struct FTxttbl
<a href="#struct-ftxttbl"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTxttbl { // algo_lib.FTxttbl: Table row. Todo: absolute index for cells?
    algo_lib::FTxtrow**   c_txtrow_elems;   // array of pointers
    u64                   c_txtrow_n;       // current size
    u64                   c_txtrow_max;     // capacity of allocated array
    i32                   col_space;        //   2  Default extra space between columns
    bool                  normalized;       //   false  Cell widths computed
    bool                  style;            //   false  Emit cell color styles even when stdout is not a terminal (e.g. output relayed to a remote terminal)
    // func:algo_lib.FTxttbl..Ctor
    inline               FTxttbl() __attribute__((nothrow));
    // func:algo_lib.FTxttbl..Dtor
    inline               ~FTxttbl() __attribute__((nothrow));
};
```

#### algo_lib.InTextFile
<a href="#algo_lib-intextfile"></a>

#### algo_lib.InTextFile Fields
<a href="#algo_lib-intextfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.InTextFile.file|[algo_lib.FFildes](/txt/gen/algo_lib/algo_lib.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.InTextFile.own_fd|bool|[Val](/txt/exe/amc/reftype/Val.md)|true||
|algo_lib.InTextFile.line_buf|[algo.LineBuf](/txt/protocol/algo/LineBuf.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.InTextFile.temp_buf|u8|[Inlary](/txt/exe/amc/reftype/Inlary.md)|||

#### Struct InTextFile
<a href="#struct-intextfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct InTextFile { // algo_lib.InTextFile
    algo_lib::FFildes   file;                               //
    bool                own_fd;                             //   true
    algo::LineBuf       line_buf;                           //
    alignas(u8) u8      temp_buf_data[sizeof(u8) * 8192];   // place for data
    i32                 temp_buf_n;                         // number of elems current in existence
    enum { temp_buf_max = 8192 };
    // func:algo_lib.InTextFile..AssignOp
    inline algo_lib::InTextFile& operator =(const algo_lib::InTextFile &rhs) __attribute__((nothrow));
    // func:algo_lib.InTextFile..Ctor
    inline               InTextFile() __attribute__((nothrow));
    // func:algo_lib.InTextFile..Dtor
    inline               ~InTextFile() __attribute__((nothrow));
    // func:algo_lib.InTextFile..CopyCtor
    inline               InTextFile(const algo_lib::InTextFile &rhs) __attribute__((nothrow));
};
```

#### algo_lib.Mmap
<a href="#algo_lib-mmap"></a>

#### algo_lib.Mmap Fields
<a href="#algo_lib-mmap-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Mmap.mem|[algo.memptr](/txt/protocol/algo/memptr.md)|[Val](/txt/exe/amc/reftype/Val.md)||Memory that has been mmap()ed|

#### Struct Mmap
<a href="#struct-mmap"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Mmap { // algo_lib.Mmap
    algo::memptr   mem;   // Memory that has been mmap()ed
    // func:algo_lib.Mmap..Ctor
    inline               Mmap() __attribute__((nothrow));
    // func:algo_lib.Mmap..Dtor
    inline               ~Mmap() __attribute__((nothrow));
};
```

#### algo_lib.MmapFile
<a href="#algo_lib-mmapfile"></a>

#### algo_lib.MmapFile Fields
<a href="#algo_lib-mmapfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.MmapFile.map|[algo_lib.Mmap](/txt/gen/algo_lib/algo_lib.md#algo_lib-mmap)|[Val](/txt/exe/amc/reftype/Val.md)||Pointer to shared memory|
|algo_lib.MmapFile.fd|[algo_lib.FFildes](/txt/gen/algo_lib/algo_lib.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftype/Val.md)||Associated file descriptor|
|algo_lib.MmapFile.text|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftype/Val.md)||Alias to map.mem, accessible as text|

#### Struct MmapFile
<a href="#struct-mmapfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct MmapFile { // algo_lib.MmapFile
    algo_lib::Mmap      map;    // Pointer to shared memory
    algo_lib::FFildes   fd;     // Associated file descriptor
    algo::strptr        text;   // Alias to map.mem, accessible as text
    // func:algo_lib.MmapFile..Ctor
    inline               MmapFile() __attribute__((nothrow));
};
```

#### algo_lib.RecSortkey - One record
<a href="#algo_lib-recsortkey"></a>

#### algo_lib.RecSortkey Fields
<a href="#algo_lib-recsortkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RecSortkey.ctype|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype/Val.md)||Type name|
|algo_lib.RecSortkey.num|double|[Val](/txt/exe/amc/reftype/Val.md)||Numeric key (if present)|
|algo_lib.RecSortkey.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||String key (sort key)|
|algo_lib.RecSortkey.rowid|float|[Val](/txt/exe/amc/reftype/Val.md)||row id|

#### Struct RecSortkey
<a href="#struct-recsortkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RecSortkey { // algo_lib.RecSortkey: One record
    algo::Smallstr150   ctype;   // Type name
    double              num;     //   0.0  Numeric key (if present)
    algo::cstring       str;     // String key (sort key)
    float               rowid;   //   0.f  row id
    // func:algo_lib.RecSortkey..EqOp
    inline bool          operator ==(const algo_lib::RecSortkey &rhs) const __attribute__((nothrow));
    // func:algo_lib.RecSortkey..NeOp
    inline bool          operator !=(const algo_lib::RecSortkey &rhs) const __attribute__((nothrow));
    // func:algo_lib.RecSortkey..LtOp
    inline bool          operator <(const algo_lib::RecSortkey &rhs) const __attribute__((nothrow));
    // func:algo_lib.RecSortkey..GtOp
    inline bool          operator >(const algo_lib::RecSortkey &rhs) const __attribute__((nothrow));
    // func:algo_lib.RecSortkey..LeOp
    inline bool          operator <=(const algo_lib::RecSortkey &rhs) const __attribute__((nothrow));
    // func:algo_lib.RecSortkey..GeOp
    inline bool          operator >=(const algo_lib::RecSortkey &rhs) const __attribute__((nothrow));
    // func:algo_lib.RecSortkey..Ctor
    inline               RecSortkey() __attribute__((nothrow));
};
```

#### algo_lib.RegxToken - Used when parsing
<a href="#algo_lib-regxtoken"></a>

#### algo_lib.RegxToken Fields
<a href="#algo_lib-regxtoken-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxToken.type|i32|[Val](/txt/exe/amc/reftype/Val.md)||State|

#### Struct RegxToken
<a href="#struct-regxtoken"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxToken { // algo_lib.RegxToken: Used when parsing
    i32   type;   //   0  State
    // func:algo_lib.RegxToken.type.Cast
    inline               operator algo_lib_RegxToken_type_Enum() const __attribute__((nothrow));
    // func:algo_lib.RegxToken..Ctor
    inline               RegxToken() __attribute__((nothrow));
    // func:algo_lib.RegxToken..FieldwiseCtor
    explicit inline               RegxToken(i32 in_type) __attribute__((nothrow));
    // func:algo_lib.RegxToken..EnumCtor
    inline               RegxToken(algo_lib_RegxToken_type_Enum arg) __attribute__((nothrow));
};
```

#### algo_lib.RegxExpr - Expression during parsing
<a href="#algo_lib-regxexpr"></a>

#### algo_lib.RegxExpr Fields
<a href="#algo_lib-regxexpr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxExpr.type|[algo_lib.RegxToken](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxtoken)|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.RegxExpr.first|i32|[Val](/txt/exe/amc/reftype/Val.md)||First character|
|algo_lib.RegxExpr.last|[algo_lib.Bitset](/txt/gen/algo_lib/algo_lib.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftype/Val.md)||Last character(s)|

#### Struct RegxExpr
<a href="#struct-regxexpr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxExpr { // algo_lib.RegxExpr: Expression during parsing
    algo_lib::RegxToken   type;    //
    i32                   first;   //   0  First character
    algo_lib::Bitset      last;    // Last character(s)
    // func:algo_lib.RegxExpr..Ctor
    inline               RegxExpr() __attribute__((nothrow));
};
```

#### algo_lib.RegxOp - A single instruction for Regex NFA
<a href="#algo_lib-regxop"></a>

#### algo_lib.RegxOp Fields
<a href="#algo_lib-regxop-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxOp.op|u8|[Val](/txt/exe/amc/reftype/Val.md)|0|What test to perform|
|algo_lib.RegxOp.consume|u8|[Val](/txt/exe/amc/reftype/Val.md)|1|Number of characters to consume|
|algo_lib.RegxOp.imm|u16|[Val](/txt/exe/amc/reftype/Val.md)|0|immediate value|

#### Struct RegxOp
<a href="#struct-regxop"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxOp { // algo_lib.RegxOp: A single instruction for Regex NFA
    u8    op;        //   0  What test to perform
    u8    consume;   //   1  Number of characters to consume
    u16   imm;       //   0  immediate value
    // func:algo_lib.RegxOp..Ctor
    inline               RegxOp() __attribute__((nothrow));
    // func:algo_lib.RegxOp..FieldwiseCtor
    explicit inline               RegxOp(u8 in_op, u8 in_consume, u16 in_imm) __attribute__((nothrow));
};
```

#### algo_lib.RegxParse - Function to parse regx
<a href="#algo_lib-regxparse"></a>

#### algo_lib.RegxParse Fields
<a href="#algo_lib-regxparse-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxParse.input|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftype/Val.md)||Input string|
|algo_lib.RegxParse.nextgroup|i32|[Val](/txt/exe/amc/reftype/Val.md)||Next capture group #|
|algo_lib.RegxParse.p_regx|[algo_lib.Regx](/txt/gen/algo_lib/algo_lib.md#algo_lib-regx)|[Upptr](/txt/exe/amc/reftype/Upptr.md)||Regx being compiled|
|algo_lib.RegxParse.ary_expr|[algo_lib.RegxExpr](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxexpr)|[Tary](/txt/exe/amc/reftype/Tary.md)||Expression stack|

#### Struct RegxParse
<a href="#struct-regxparse"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxParse { // algo_lib.RegxParse: Function to parse regx
    algo::strptr          input;            // Input string
    i32                   nextgroup;        //   0  Next capture group #
    algo_lib::Regx*       p_regx;           // reference to parent row
    algo_lib::RegxExpr*   ary_expr_elems;   // pointer to elements
    u64                   ary_expr_n;       // number of elements in array
    u64                   ary_expr_max;     // max. capacity of array before realloc
    // func:algo_lib.RegxParse..AssignOp
    algo_lib::RegxParse& operator =(const algo_lib::RegxParse &rhs) __attribute__((nothrow));
    // func:algo_lib.RegxParse..Ctor
    inline               RegxParse() __attribute__((nothrow));
    // func:algo_lib.RegxParse..Dtor
    inline               ~RegxParse() __attribute__((nothrow));
    // func:algo_lib.RegxParse..CopyCtor
    RegxParse(const algo_lib::RegxParse &rhs) __attribute__((nothrow));
};
```

#### algo_lib.RegxState - Instruction + jumps
<a href="#algo_lib-regxstate"></a>

#### algo_lib.RegxState Fields
<a href="#algo_lib-regxstate-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxState.ch_class|[algo.U16Ary](/txt/protocol/algo/README.md#algo-u16ary)|[Val](/txt/exe/amc/reftype/Val.md)||What to match|
|algo_lib.RegxState.op|[algo_lib.RegxOp](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxop)|[Val](/txt/exe/amc/reftype/Val.md)||Operation to perform|
|algo_lib.RegxState.lparen|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.RegxState.next|[algo_lib.Bitset](/txt/gen/algo_lib/algo_lib.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftype/Val.md)||Where to go on a match|

#### Struct RegxState
<a href="#struct-regxstate"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxState { // algo_lib.RegxState: Instruction + jumps
    algo::U16Ary       ch_class;   // What to match
    algo_lib::RegxOp   op;         // Operation to perform
    i32                lparen;     //   0
    algo_lib::Bitset   next;       // Where to go on a match
    // func:algo_lib.RegxState..Ctor
    inline               RegxState() __attribute__((nothrow));
};
```

#### algo_lib.Replscope
<a href="#algo_lib-replscope"></a>

#### algo_lib.Replscope Fields
<a href="#algo_lib-replscope-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Replscope.eatcomma|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Delete comma+space after substitution|
|algo_lib.Replscope.strict|u8|[Val](/txt/exe/amc/reftype/Val.md)||1=warnings; 2=throw exception on error|
|algo_lib.Replscope.ind_replvar|[algo_lib.FReplvar](/txt/gen/algo_lib/algo_lib.md#algo_lib-freplvar)|[Thash](/txt/exe/amc/reftype/Thash.md)|||

#### Struct Replscope
<a href="#struct-replscope"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Replscope { // algo_lib.Replscope
    bool                   eatcomma;                    //   true  Delete comma+space after substitution
    u8                     strict;                      //   0  1=warnings; 2=throw exception on error
    algo_lib::FReplvar**   ind_replvar_buckets_elems;   // pointer to bucket array
    i32                    ind_replvar_buckets_n;       // number of elements in bucket array
    i32                    ind_replvar_n;               // number of elements in the hash table
    // func:algo_lib.Replscope..Ctor
    inline               Replscope() __attribute__((nothrow));
    // func:algo_lib.Replscope..Dtor
    inline               ~Replscope() __attribute__((nothrow));
};
```

#### algo_lib.ShHdr
<a href="#algo_lib-shhdr"></a>

#### algo_lib.ShHdr Fields
<a href="#algo_lib-shhdr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.ShHdr.magic|u32|[Val](/txt/exe/amc/reftype/Val.md)|0x09202017|Signature|
|algo_lib.ShHdr.name|[algo.RspaceStr32](/txt/protocol/algo/README.md#algo-rspacestr32)|[Val](/txt/exe/amc/reftype/Val.md)||User defined name|
|algo_lib.ShHdr.dataoffset|u64|[Val](/txt/exe/amc/reftype/Val.md)|4096|Offset to beginning of data|
|algo_lib.ShHdr.eof|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.ShHdr.sof|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.ShHdr.bufsize|u64|[Val](/txt/exe/amc/reftype/Val.md)|||
|algo_lib.ShHdr.pad|u64|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct ShHdr
<a href="#struct-shhdr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct ShHdr { // algo_lib.ShHdr
    u32                 magic;        //   0x09202017  Signature
    algo::RspaceStr32   name;         // User defined name
    u64                 dataoffset;   //   4096  Offset to beginning of data
    u64                 eof;          //   0
    u64                 sof;          //   0
    u64                 bufsize;      //   0
    u64                 pad;          //   0
    // func:algo_lib.ShHdr..Ctor
    inline               ShHdr() __attribute__((nothrow));
};
```

#### algo_lib.Srng - Command function, a single word
<a href="#algo_lib-srng"></a>

#### algo_lib.Srng Fields
<a href="#algo_lib-srng-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Srng.z|u32|[Val](/txt/exe/amc/reftype/Val.md)|123||
|algo_lib.Srng.w|u32|[Val](/txt/exe/amc/reftype/Val.md)|456||

#### Struct Srng
<a href="#struct-srng"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Srng { // algo_lib.Srng: Command function, a single word
    u32   z;   //   123
    u32   w;   //   456
    // func:algo_lib.Srng..Ctor
    inline               Srng() __attribute__((nothrow));
};
```

#### algo_lib.Tabulate - Function to tabulate a string
<a href="#algo_lib-tabulate"></a>

#### algo_lib.Tabulate Fields
<a href="#algo_lib-tabulate-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Tabulate.width|i32|[Tary](/txt/exe/amc/reftype/Tary.md)|||
|algo_lib.Tabulate.temp|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct Tabulate
<a href="#struct-tabulate"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Tabulate { // algo_lib.Tabulate: Function to tabulate a string
    i32*            width_elems;   // pointer to elements
    u64             width_n;       // number of elements in array
    u64             width_max;     // max. capacity of array before realloc
    algo::cstring   temp;          //
    // func:algo_lib.Tabulate..AssignOp
    algo_lib::Tabulate&  operator =(const algo_lib::Tabulate &rhs) __attribute__((nothrow));
    // func:algo_lib.Tabulate..Ctor
    inline               Tabulate() __attribute__((nothrow));
    // func:algo_lib.Tabulate..Dtor
    inline               ~Tabulate() __attribute__((nothrow));
    // func:algo_lib.Tabulate..CopyCtor
    Tabulate(const algo_lib::Tabulate &rhs) __attribute__((nothrow));
};
```
