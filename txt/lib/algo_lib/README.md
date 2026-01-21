## algo_lib - Support library for all executables


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Printing / Process output](#printing---process-output)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Printing to stderr](#printing-to-stderr)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Log categories](#log-categories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Verblog](#verblog)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dbglog](#dbglog)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Environment variables](#environment-variables)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Printing / Process output
<a href="#printing---process-output"></a>

The standard way to print something to `stdout` is with the use of `prlog` macro.
Example
```
prlog("hello, world");
```

Prlog uses a temporary string, so that convenient `<<` expressions are possible:

```
prlog("hello, world, my name is "<< _db.cmdline.name);
```

prlog automatically appends a newline; to omit the newline, use the variant `prlog_`.
Prlog will automatically prefix each printed line with a nanosecond-level timestamp,
if the variable `algo_lib::_db.show_tstamp` is set.

Additionally, all `*log` macros ultimately call a function through the global pointer
`algo_lib::_db.Prlog`, which can be overridden to redirect output to some other channel.

### Printing to stderr
<a href="#printing-to-stderr"></a>

The prlog equivalent for stderr output is `prerr`.
prerr automatically appends a newline; to omit the newline, use the variant `prerr_`.

### Log categories
<a href="#log-categories"></a>

`algo_lib` has a convenient notion of `log category`, controlled by table `dev.logcat`.
Each log category has a name and an `enable` flag, which can be switched at runtime.
Printing to a log category xyz is performed with

```
prcat(xyz, "some output");
```

And is equivalent to

```
if (algo_lib_logcat.xyz.enable) {
   prlog("some output");
}
```

Stdout and stderr are generalized as log categories.
The table of log categories is given below:

```
inline-command: acr logcat:'(stdout|stderr|debug|verbose|verbose2)' -report:N | ssimfilt -t
LOGCAT    ENABLED  BUILTIN  STDOUT  MAXMSG  WINDOW  COMMENT
debug     N        N        N       0       0       Debug messages
stderr    Y        Y        N       0       0       Standard error (cannot be disabled)
stdout    Y        Y        Y       0       0       Standard output (cannot be disabled)
verbose   N        N        N       0       0       Verbose messages go here
verbose2  N        N        N       0       0       Extra verbose messages go here

```

### Verblog
<a href="#verblog"></a>

The macro `verblog(x)` is equivlent to `if (algo_lib::_db.cmdline.verbose) { prlog(x); }`

Verbose mode is enabled with flags `-v` or `-verbose`.
Multiple `-v` flags increment the verbose level to be higher.

When spawning a known amc-compatible subprocess using amc-generated code, the verbosity level
is automatically decremented by one. Thus, running a process with `-v -v` passes `-v` to each spawned
subprocess, which can be very useful to look "deeper" into the run.

### Dbglog
<a href="#dbglog"></a>

The macro `dbglog(x)` is equivlent to `if (algo_lib::_db.cmdline.debug) { prlog(x); }`

Debug mode is enabled with flags `-d` or `-debug
Multiple `-d` flags increment the debug level to be higher.
Debug level is similar to verbose level and is passed down to child processes on command line,
without being decremented.

### Environment variables
<a href="#environment-variables"></a>

We specifically avoid using the environment block to pass verbose/debug flags, in order to maintain
an exact correspondence between the command line and the process behavior.

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
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
inline u32 _mm_crc32_u8 (u32 prev, u8  val) 
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
// Interpret redirect string and make DST_FD consistent with
// the intended state. Return 0 on success, -1 on failure
// This function is usually called in the child process right after fork
// See CreateRedirect for interpretation of redirect string
int algo_lib::ApplyRedirect(strptr redirect, int dst_fd) 
```

```c++
// Enable/disable log category NAME with filter FILTER.
// If NAME is prefixed with +, logging is enabled
// If NAME is prefixed with -, logging is disabled
// If NAME is an empty string. current state is printed
// FILTER is a regex
// If FILTER starts with !, it is a negative filter (any matching lines are omitted)
// Return number of logcats affected.
int algo_lib::ApplyTrace(algo::strptr name, algo::strptr filter) 
```

```c++
// Enable or disable logcat tracing based on traace expression WHAT
// WHAT is a comma-separated list of logcat regexes, e.g. a,b,c
// Each component can be prefixed with + or -, e.g. +a,-b etc.
// Finally, each component can be a key-value pair, e.g. +a:<filter>,-b,+c
// <filter> is an optional regex; Regex can be prefixed with ! to indicate a negative match.
// Timestamps can be enabled with 'timestamps', disabled with '-timestamps'
// Verbose can be enabled with 'verbose', disabled with '-verbose'
// Debug can be enabled with 'debug', disabled with '-debug'
int algo_lib::ApplyTrace(algo::strptr what) 
```

```c++
// Close specified handle if it open
// Set value to NULL
void CloseHandleSafe(HANDLE &handle) 
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
// OK to add an fd twice with different flags. Subsequent calls override previous ones.
// Add iohook to epoll in read, write or read/write mode
// Optionally, add as edge triggered
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
bool algo_lib::LogcatFilterQ(algo_lib::FLogcat &logcat, algo::strptr str) 
```

```c++
// Attach mmapfile MMAPFILE to FNAME
// Return success code.
bool algo_lib::MmapFile_Load(MmapFile &mmapfile, strptr fname) 
```

```c++
// Attach mmapfile MMAPFILE to FD.
// Return success code.
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
// Return computed name for sandbox SANDBOX
tempstr algo_lib::SandboxDir(algo::strptr sandbox) 
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
void algo_lib::ShowTrace() 
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
// Write pid to lockfile, separate function to update pid after fork().
// Sets error text in case of error, and return false.
bool algo_lib::WritePid(algo_lib::FLockfile &lockfile) 
```

```c++
template<class T> inline T &algo::aryptr<T>::operator [](u32 idx) const 
```

```c++
void algo_lib::_db_Userinit() 
```

```c++
inline u32 _mm_crc32_u16(u32 prev, u16 val) 
```

```c++
inline u32 _mm_crc32_u32(u32 prev, u32 val) 
```

```c++
inline u32 _mm_crc32_u64(u32 prev, u64 val) 
```

```c++
int alarm(int sec) 
```

```c++
template<class T> inline algo::aryptr<T>::aryptr() 
```

```c++
template<class T> inline algo::aryptr<T>::aryptr(const T *e, i32 in_n) : elems(const_cast<T*>(e)) , n_elems(in_n) 
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
// redirect to MS implementation
int pipe(int fd[2]) 
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`algo_lib` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
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
|[cpp/lib/algo/cpu_hz.cpp](/cpp/lib/algo/cpu_hz.cpp)|Grab cpu_hz from OS|
|[cpp/lib/algo/crc32.cpp](/cpp/lib/algo/crc32.cpp)|Software-based CRC32|
|[cpp/lib/algo/decimal.cpp](/cpp/lib/algo/decimal.cpp)||
|[cpp/lib/algo/errtext.cpp](/cpp/lib/algo/errtext.cpp)||
|[cpp/lib/algo/file.cpp](/cpp/lib/algo/file.cpp)|File functions|
|[cpp/lib/algo/fmt.cpp](/cpp/lib/algo/fmt.cpp)|Print to string / Read from string|
|[cpp/lib/algo/iohook.cpp](/cpp/lib/algo/iohook.cpp)||
|[cpp/lib/algo/lib.cpp](/cpp/lib/algo/lib.cpp)|Main file|
|[cpp/lib/algo/line.cpp](/cpp/lib/algo/line.cpp)|Line processing|
|[cpp/lib/algo/lockfile.cpp](/cpp/lib/algo/lockfile.cpp)|Lock file|
|[cpp/lib/algo/mmap.cpp](/cpp/lib/algo/mmap.cpp)|Mmap wrapper|
|[cpp/lib/algo/prlog.cpp](/cpp/lib/algo/prlog.cpp)|prlog macro|
|[cpp/lib/algo/regx.cpp](/cpp/lib/algo/regx.cpp)|Sql Regx implementation|
|[cpp/lib/algo/replscope.cpp](/cpp/lib/algo/replscope.cpp)||
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[lib_json](/txt/lib/lib_json/README.md)|Full json support library|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
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

#### algo_lib.Bitset - 
<a href="#algo_lib-bitset"></a>

#### algo_lib.Bitset Fields
<a href="#algo_lib-bitset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Bitset.ary|u64|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### Struct Bitset
<a href="#struct-bitset"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Bitset { // algo_lib.Bitset
    u64*   ary_elems;   // pointer to elements
    u32    ary_n;       // number of elements in array
    u32    ary_max;     // max. capacity of array before realloc
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
|algo_lib.Cmdline.verbose|u8|[Val](/txt/exe/amc/reftypes.md#val)||Verbosity level (0..255)|
|algo_lib.Cmdline.debug|u8|[Val](/txt/exe/amc/reftypes.md#val)||Debug level (0..255)|
|algo_lib.Cmdline.help|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print help and exit|
|algo_lib.Cmdline.version|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print version and exit|
|algo_lib.Cmdline.signature|bool|[Val](/txt/exe/amc/reftypes.md#val)||Show signatures and exit|
|algo_lib.Cmdline.v|u8|[Alias](/txt/exe/amc/reftypes.md#alias)||Alias for verbose|
|algo_lib.Cmdline.d|u8|[Alias](/txt/exe/amc/reftypes.md#alias)||Alias for debug|
|algo_lib.Cmdline.sig|bool|[Alias](/txt/exe/amc/reftypes.md#alias)||Alias for signature|
|algo_lib.Cmdline.h|bool|[Alias](/txt/exe/amc/reftypes.md#alias)||Alias for help|

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

#### algo_lib.CsvParse - 
<a href="#algo_lib-csvparse"></a>

#### algo_lib.CsvParse Fields
<a href="#algo_lib-csvparse-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.CsvParse.input|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Input string|
|algo_lib.CsvParse.sep|char|[Val](/txt/exe/amc/reftypes.md#val)|','|Input: separator|
|algo_lib.CsvParse.quotechar1|char|[Val](/txt/exe/amc/reftypes.md#val)|'\"'|Allow this quote|
|algo_lib.CsvParse.quotechar2|char|[Val](/txt/exe/amc/reftypes.md#val)|'\''|Allow this quote as well|
|algo_lib.CsvParse.ary_tok|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Output: array of tokens|
|algo_lib.CsvParse.openquote|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|On output: set if unbalanced quote found|

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
    u32              ary_tok_n;       // number of elements in array
    u32              ary_tok_max;     // max. capacity of array before realloc
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

#### algo_lib.ErrorX - 
<a href="#algo_lib-errorx"></a>

#### algo_lib.ErrorX Fields
<a href="#algo_lib-errorx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.ErrorX.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

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
|algo_lib.RegxM.front|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Temporary front (for matching)|
|algo_lib.RegxM.this_char|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||States to test on this char|
|algo_lib.RegxM.next_char|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||States to test on next char|
|algo_lib.RegxM.matchrange|[algo.I32RangeAry](/txt/protocol/algo/README.md#algo-i32rangeary)|[Val](/txt/exe/amc/reftypes.md#val)||List of match char ranges|

#### Struct RegxM
<a href="#struct-regxm"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxM { // algo_lib.RegxM: Matching context for regex
    algo_lib::Bitset    front;        // Temporary front (for matching)
    algo_lib::Bitset    this_char;    // States to test on this char
    algo_lib::Bitset    next_char;    // States to test on next char
    algo::I32RangeAry   matchrange;   // List of match char ranges
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
|algo_lib.FFildes.fd|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

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

#### algo_lib.FLockfile - 
<a href="#algo_lib-flockfile"></a>

#### algo_lib.FLockfile Fields
<a href="#algo_lib-flockfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FLockfile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FLockfile.fildes|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FLockfile
<a href="#struct-flockfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FLockfile { // algo_lib.FLockfile
    algo::cstring       filename;   //
    algo_lib::FFildes   fildes;     //
    // func:algo_lib.FLockfile..Ctor
    inline               FLockfile() __attribute__((nothrow));
    // func:algo_lib.FLockfile..Dtor
    inline               ~FLockfile() __attribute__((nothrow));
};
```

#### algo_lib.FTimehook - 
<a href="#algo_lib-ftimehook"></a>

#### algo_lib.FTimehook Fields
<a href="#algo_lib-ftimehook-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTimehook.time|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time the hook is scheduled to expire|
|algo_lib.FTimehook.delay|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Minimum delay between iterations|
|algo_lib.FTimehook.hook|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Hook](/txt/exe/amc/reftypes.md#hook)||Function to call|
|algo_lib.FTimehook.recurrent|bool|[Val](/txt/exe/amc/reftypes.md#val)||If true, automatically reschedule|

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
|algo.Imdb.imdb|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Database name|
|algo.Imdb.InsertStrptrMaybe|[algo.ImdbInsertStrptrMaybeFcn](/txt/protocol/algo/README.md#algo-imdbinsertstrptrmaybefcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Insert new element given a string|
|algo.Imdb.Step|[algo.ImdbStepFcn](/txt/protocol/algo/README.md#algo-imdbstepfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Perform one step (may be NULL)|
|algo.Imdb.MainLoop|[algo.ImdbMainLoopFcn](/txt/protocol/algo/README.md#algo-imdbmainloopfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Loop!|
|algo.Imdb.GetTrace|[algo.ImdbGetTraceFcn](/txt/protocol/algo/README.md#algo-imdbgettracefcn)|[Val](/txt/exe/amc/reftypes.md#val)|0||
|algo.Imdb.comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FImdb
<a href="#struct-fimdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FImdb { // algo_lib.FImdb
    algo::Smallstr50                 imdb;                // Database name
    algo::ImdbInsertStrptrMaybeFcn   InsertStrptrMaybe;   //   0  Insert new element given a string
    algo::ImdbStepFcn                Step;                //   0  Perform one step (may be NULL)
    algo::ImdbMainLoopFcn            MainLoop;            //   0  Loop!
    algo::ImdbGetTraceFcn            GetTrace;            //   0
    algo::Comment                    comment;             //
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

#### algo_lib.RegxFlags - 
<a href="#algo_lib-regxflags"></a>

#### algo_lib.RegxFlags Fields
<a href="#algo_lib-regxflags-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxFlags.value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.RegxFlags.trace|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||Enable tracing|
|algo_lib.RegxFlags.capture|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||Enable capture groups|
|algo_lib.RegxFlags.valid|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||The regx parsed successfully|
|algo_lib.RegxFlags.literal|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||The regx expression is a literal string|
|algo_lib.RegxFlags.accepts_all|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||Compiled regx accepts all intputs|
|algo_lib.RegxFlags.fullmatch|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)||Regx expression is a substring|

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
|algo_lib.RegxStyle.value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

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
|algo_lib.Regx.expr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Original string expression|
|algo_lib.Regx.state|[algo_lib.RegxState](/txt/lib/algo_lib/README.md#algo_lib-regxstate)|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of states|
|algo_lib.Regx.flags|[algo_lib.RegxFlags](/txt/lib/algo_lib/README.md#algo_lib-regxflags)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.Regx.style|[algo_lib.RegxStyle](/txt/lib/algo_lib/README.md#algo_lib-regxstyle)|[Val](/txt/exe/amc/reftypes.md#val)||Regx style according to which EXPR was parsed|

#### Struct Regx
<a href="#struct-regx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Regx { // algo_lib.Regx: Parsed regular expression
    algo::cstring          expr;          // Original string expression
    algo_lib::RegxState*   state_elems;   // pointer to elements
    u32                    state_n;       // number of elements in array
    u32                    state_max;     // max. capacity of array before realloc
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
|algo_lib.FLogcat.filter|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Val](/txt/exe/amc/reftypes.md#val)||Output filter|
|algo_lib.FLogcat.negfilter|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Val](/txt/exe/amc/reftypes.md#val)||Negative output filter|
|algo_lib.FLogcat.nmsg|i64|[Val](/txt/exe/amc/reftypes.md#val)||Number of messages printed inside current window|
|algo_lib.FLogcat.th_throttle|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Val](/txt/exe/amc/reftypes.md#val)|||

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
    algo::Comment         comment;              //
    algo_lib::Regx        filter;               // Output filter
    algo_lib::Regx        negfilter;            // Negative output filter
    i64                   nmsg;                 //   0  Number of messages printed inside current window
    algo_lib::FTimehook   th_throttle;          //
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

#### algo_lib.FErrns - 
<a href="#algo_lib-ferrns"></a>

#### algo_lib.FErrns Fields
<a href="#algo_lib-ferrns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FErrns.errns|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FErrns.decode|i32|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|algo_lib.FErrns.outstr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Output of decode operation|

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
|algo_lib.FDb.cstring|[algo.cstring](/txt/protocol/algo/cstring.md)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)|||
|algo_lib.FDb.sbrk|u8|[Sbrk](/txt/exe/amc/reftypes.md#sbrk)||Base allocator for everything|
|algo_lib.FDb.lpool|u8|[Lpool](/txt/exe/amc/reftypes.md#lpool)||private memory pool|
|algo_lib.FDb.next_loop|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.limit|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|0x7fffffffffffffff|Main loop clock limit|
|algo_lib.FDb.clocks_to_ms|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.n_iohook|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of iohooks in epoll|
|algo_lib.FDb.clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Most recent cpu clock value|
|algo_lib.FDb.step_limit|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Execution limit for current step|
|algo_lib.FDb.start_clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||cpu clock value at startup|
|algo_lib.FDb.hz|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.t_last_signal|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time last async signal was processed|
|algo_lib.FDb.exit_code|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Unix exit code from main program. 0 = success|
|algo_lib.FDb.clocks_to_ns|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.n_temp|u32|[Val](/txt/exe/amc/reftypes.md#val)|0||
|algo_lib.FDb.last_signal|u32|[Val](/txt/exe/amc/reftypes.md#val)||Value of last signal (used by SetupExitSignals)|
|algo_lib.FDb.cpu_hz|u64|[Val](/txt/exe/amc/reftypes.md#val)||Cpu HZ, determined at startup|
|algo_lib.FDb.fildes|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.temp_strings|[algo.cstring](/txt/protocol/algo/cstring.md)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|algo_lib.FDb.ArgvIdent|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.BashQuotesafe|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.RegxSqlSpecial|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.SsimBreakName|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.SsimBreakValue|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.SsimQuotesafe|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb._db|[algo_lib.FDb](/txt/lib/algo_lib/README.md#algo_lib-fdb)|[Global](/txt/exe/amc/reftypes.md#global)||* initialization order is important *|
|algo_lib.FDb.imtable|[algo_lib.FImtable](/txt/lib/algo_lib/README.md#algo_lib-fimtable)|[Lary](/txt/exe/amc/reftypes.md#lary)||Array of all in-memory tables linked into this process|
|algo_lib.FDb.ind_imtable|[algo_lib.FImtable](/txt/lib/algo_lib/README.md#algo_lib-fimtable)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|algo_lib.FDb.iohook|[algo_lib.FIohook](/txt/lib/algo_lib/README.md#algo_lib-fiohook)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.timehook|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.replscope|[algo_lib.Replscope](/txt/lib/algo_lib/README.md#algo_lib-replscope)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.error|[algo_lib.ErrorX](/txt/lib/algo_lib/README.md#algo_lib-errorx)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)|||
|algo_lib.FDb.csvparse|[algo_lib.CsvParse](/txt/lib/algo_lib/README.md#algo_lib-csvparse)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.regxparse|[algo_lib.RegxParse](/txt/lib/algo_lib/README.md#algo_lib-regxparse)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.regx|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.regxm|[algo_lib.RegxM](/txt/lib/algo_lib/README.md#algo_lib-regxm)|[Val](/txt/exe/amc/reftypes.md#val)||Context for Regx matching|
|algo_lib.FDb.tabulate|[algo_lib.Tabulate](/txt/lib/algo_lib/README.md#algo_lib-tabulate)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.log_str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.bh_timehook|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Bheap](/txt/exe/amc/reftypes.md#bheap)||Binary heap of time-based callbacks|
|algo_lib.FDb.epoll_fd|i32|[Val](/txt/exe/amc/reftypes.md#val)|-1||
|algo_lib.FDb.lock_core|[algo_lib.FLockfile](/txt/lib/algo_lib/README.md#algo_lib-flockfile)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.c_timehook|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||TEMP: here only for dependency reasons|
|algo_lib.FDb._timehook|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Val](/txt/exe/amc/reftypes.md#val)||Keep me here i'm special|
|algo_lib.FDb.dispsigcheck|[algo_lib.FDispsigcheck](/txt/lib/algo_lib/README.md#algo_lib-fdispsigcheck)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|algo_lib.FDb.ind_dispsigcheck|[algo_lib.FDispsigcheck](/txt/lib/algo_lib/README.md#algo_lib-fdispsigcheck)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|algo_lib.FDb.imdb|[algo_lib.FImdb](/txt/lib/algo_lib/README.md#algo_lib-fimdb)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|algo_lib.FDb.ind_imdb|[algo_lib.FImdb](/txt/lib/algo_lib/README.md#algo_lib-fimdb)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|algo_lib.FDb.malloc|u8|[Malloc](/txt/exe/amc/reftypes.md#malloc)||Pool for everything else|
|algo_lib.FDb.txtcell|[algo_lib.FTxtcell](/txt/lib/algo_lib/README.md#algo_lib-ftxtcell)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|algo_lib.FDb.txtrow|[algo_lib.FTxtrow](/txt/lib/algo_lib/README.md#algo_lib-ftxtrow)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|algo_lib.FDb.txttbl|[algo_lib.FTxttbl](/txt/lib/algo_lib/README.md#algo_lib-ftxttbl)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.argc|i32|[Val](/txt/exe/amc/reftypes.md#val)||Argc from main|
|algo_lib.FDb.argv|char*|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Argv from main|
|algo_lib.FDb.xref_error|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.errtext|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.varlenbuf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|algo_lib.FDb.replvar|[algo_lib.FReplvar](/txt/lib/algo_lib/README.md#algo_lib-freplvar)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|algo_lib.FDb.cmdline|[algo_lib.Cmdline](/txt/lib/algo_lib/README.md#algo_lib-cmdline)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.h_fatalerror||[Hook](/txt/exe/amc/reftypes.md#hook)|||
|algo_lib.FDb.giveup_count|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.fatalerr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.stringtofile_nwrite|u32|[Val](/txt/exe/amc/reftypes.md#val)||Global counter of # of files written|
|algo_lib.FDb.last_sleep_clocks|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.msgtemp|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.DigitChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.NewLineChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.WhiteChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.DirSep|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.IdentChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.IdentStart|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.AlphaChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.HexChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.UpperChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.CmdLineNameBreak|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.CmdLineValueBreak|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.WordSeparator|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.LowerChar|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.Urlsafe|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|algo_lib.FDb.winjob|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.Prlog|[algo.PrlogFcn](/txt/protocol/algo/PrlogFcn.md)|[Val](/txt/exe/amc/reftypes.md#val)|algo::Prlog||
|algo_lib.FDb.logcat|[algo_lib.FLogcat](/txt/lib/algo_lib/README.md#algo_lib-flogcat)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|algo_lib.FDb.ind_logcat|[algo_lib.FLogcat](/txt/lib/algo_lib/README.md#algo_lib-flogcat)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|algo_lib.FDb.tstamp_fmt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"%Y/%m/%dT%H:%M:%S.%.6X "||
|algo_lib.FDb.exec_args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|algo_lib.FDb.dirstack|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Directory stack for PushDir/PopDir|
|algo_lib.FDb.errns|[algo_lib.FErrns](/txt/lib/algo_lib/README.md#algo_lib-ferrns)|[Inlary](/txt/exe/amc/reftypes.md#inlary)||Fixed table of errns decoders|
|algo_lib.FDb.tempdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.use_epoll_pwait2|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Use epoll_pwait2 system call|
|algo_lib.FDb.pending_eol|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.giveup_time|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Trigger for giveup_time loop|
|algo_lib.FDb.show_tstamp|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FDb { // algo_lib.FDb: In-memory database for algo_lib
    u64                               sbrk_huge_limit;                               // Huge page limit in bytes -- set to 0 with fork!
    u64                               sbrk_huge_alloc;                               // Huge page bytes allocated
    bool                              sbrk_zeromem;                                  // Zero allocated memory
    lpool_Lpblock*                    lpool_free[36];                                // Lpool levels
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
    algo_lib::FImtable*               imtable_lary[32];                              // level array
    i32                               imtable_n;                                     // number of elements in array
    algo_lib::FImtable**              ind_imtable_buckets_elems;                     // pointer to bucket array
    i32                               ind_imtable_buckets_n;                         // number of elements in bucket array
    i32                               ind_imtable_n;                                 // number of elements in the hash table
    algo_lib::RegxM                   regxm;                                         // Context for Regx matching
    algo::cstring                     log_str;                                       //
    algo_lib::FTimehook**             bh_timehook_elems;                             // binary heap by time
    i32                               bh_timehook_n;                                 // number of elements in the heap
    i32                               bh_timehook_max;                               // max elements in bh_timehook_elems
    i32                               epoll_fd;                                      //   -1
    algo_lib::FLockfile               lock_core;                                     //
    algo_lib::FTimehook*              c_timehook;                                    // TEMP: here only for dependency reasons. optional pointer
    algo_lib::FTimehook               _timehook;                                     // Keep me here i'm special
    algo_lib::FDispsigcheck*          dispsigcheck_lary[32];                         // level array
    i32                               dispsigcheck_n;                                // number of elements in array
    algo_lib::FDispsigcheck**         ind_dispsigcheck_buckets_elems;                // pointer to bucket array
    i32                               ind_dispsigcheck_buckets_n;                    // number of elements in bucket array
    i32                               ind_dispsigcheck_n;                            // number of elements in the hash table
    u128                              imdb_data[sizeu128(algo_lib::FImdb,32)];       // place for data
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
    u128                              logcat_data[sizeu128(algo_lib::FLogcat,10)];   // place for data
    i32                               logcat_n;                                      // number of elems current in existence
    enum { logcat_max = 10 };
    algo_lib::FLogcat**               ind_logcat_buckets_elems;                      // pointer to bucket array
    i32                               ind_logcat_buckets_n;                          // number of elements in bucket array
    i32                               ind_logcat_n;                                  // number of elements in the hash table
    algo::cstring                     tstamp_fmt;                                    //   "%Y/%m/%dT%H:%M:%S.%.6X "
    algo::cstring*                    exec_args_elems;                               // pointer to elements
    u32                               exec_args_n;                                   // number of elements in array
    u32                               exec_args_max;                                 // max. capacity of array before realloc
    algo::cstring*                    dirstack_elems;                                // pointer to elements
    u32                               dirstack_n;                                    // number of elements in array
    u32                               dirstack_max;                                  // max. capacity of array before realloc
    algo_lib::FErrns                  errns_elems[8];                                // fixed array
    algo::cstring                     tempdir;                                       //
    bool                              use_epoll_pwait2;                              //   false  Use epoll_pwait2 system call
    bool                              pending_eol;                                   //   false
    bool                              giveup_time;                                   //   true  Trigger for giveup_time loop
    bool                              show_tstamp;                                   //   false
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
    algo::Sha1sig              signature;                  //
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
|algo.Imtable.imtable|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Table name|
|algo.Imtable.elem_type|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Element type name|
|algo.Imtable.c_RowidFind|[algo.ImrowRowidFindFcn](/txt/protocol/algo/README.md#algo-imrowrowidfindfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Function to find element by rowid (may be NULL)|
|algo.Imtable.XrefX|[algo.ImrowXrefXFcn](/txt/protocol/algo/README.md#algo-imrowxrefxfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Function to x-reference an element (may be NULL)|
|algo.Imtable.NItems|[algo.ImrowNItemsFcn](/txt/protocol/algo/README.md#algo-imrownitemsfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Return number of elements in the table|
|algo.Imtable.Print|[algo.ImrowPrintFcn](/txt/protocol/algo/README.md#algo-imrowprintfcn)|[Val](/txt/exe/amc/reftypes.md#val)|0|Convert specified element to string (may be NULL)|
|algo.Imtable.size|i32|[Val](/txt/exe/amc/reftypes.md#val)||Size of one element (for fixed-width elements only)|
|algo.Imtable.ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Ssimfile name (if associated)|
|algo.Imtable.comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

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
    algo::Comment             comment;               //
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

#### algo_lib.FIohook - 
<a href="#algo_lib-fiohook"></a>

#### algo_lib.FIohook Fields
<a href="#algo_lib-fiohook-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FIohook.callback|[algo_lib.FIohook](/txt/lib/algo_lib/README.md#algo_lib-fiohook)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|algo_lib.FIohook.fildes|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftypes.md#val)||File descriptor, possibly in epoll|
|algo_lib.FIohook.evt_flags|[algo.IOEvtFlags](/txt/protocol/algo/IOEvtFlags.md)|[Val](/txt/exe/amc/reftypes.md#val)||Flags subscribed to|
|algo_lib.FIohook.flags|[algo.IOEvtFlags](/txt/protocol/algo/IOEvtFlags.md)|[Val](/txt/exe/amc/reftypes.md#val)||Flags during callback|
|algo_lib.FIohook.in_epoll|bool|[Val](/txt/exe/amc/reftypes.md#val)||Registered in epoll?|
|algo_lib.FIohook.nodelete|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|File descriptor is shared -- do not close()|

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

#### algo_lib.FReplvar - 
<a href="#algo_lib-freplvar"></a>

#### algo_lib.FReplvar Fields
<a href="#algo_lib-freplvar-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FReplvar.p_replscope|[algo_lib.Replscope](/txt/lib/algo_lib/README.md#algo_lib-replscope)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Parent|
|algo_lib.FReplvar.key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Key|
|algo_lib.FReplvar.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Value|
|algo_lib.FReplvar.nsubst|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of times variable accessed|
|algo_lib.FReplvar.partial|bool|[Val](/txt/exe/amc/reftypes.md#val)||This is a partial match on another variable|

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

#### algo_lib.FTempfile - 
<a href="#algo_lib-ftempfile"></a>

#### algo_lib.FTempfile Fields
<a href="#algo_lib-ftempfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTempfile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FTempfile.fildes|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)|||

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

#### algo_lib.FTxtcell - 
<a href="#algo_lib-ftxtcell"></a>

#### algo_lib.FTxtcell Fields
<a href="#algo_lib-ftxtcell-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FTxtcell.p_txtrow|[algo_lib.FTxtrow](/txt/lib/algo_lib/README.md#algo_lib-ftxtrow)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|algo_lib.FTxtcell.justify|[algo.TextJust](/txt/protocol/algo/TextJust.md)|[Val](/txt/exe/amc/reftypes.md#val)|algo_TextJust_j_left|Justification of text within cell|
|algo_lib.FTxtcell.style|[algo.TermStyle](/txt/protocol/algo/TermStyle.md)|[Val](/txt/exe/amc/reftypes.md#val)||Text style|
|algo_lib.FTxtcell.span|i32|[Val](/txt/exe/amc/reftypes.md#val)|1|Column span of this cell|
|algo_lib.FTxtcell.width|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Width in chars|
|algo_lib.FTxtcell.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Cell contents|
|algo_lib.FTxtcell.rsep|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Right separator|

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
|algo_lib.FTxtrow.p_txttbl|[algo_lib.FTxttbl](/txt/lib/algo_lib/README.md#algo_lib-ftxttbl)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|algo_lib.FTxtrow.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Select for processing|
|algo_lib.FTxtrow.ishdr|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Is header row|
|algo_lib.FTxtrow.sortkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Sort key|
|algo_lib.FTxtrow.c_txtcell|[algo_lib.FTxtcell](/txt/lib/algo_lib/README.md#algo_lib-ftxtcell)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

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
    u32                    c_txtcell_n;              // array of pointers
    u32                    c_txtcell_max;            // capacity of allocated array
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
|algo_lib.FTxttbl.c_txtrow|[algo_lib.FTxtrow](/txt/lib/algo_lib/README.md#algo_lib-ftxtrow)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Array of rows|
|algo_lib.FTxttbl.col_space|i32|[Val](/txt/exe/amc/reftypes.md#val)|2|Default extra space between columns|
|algo_lib.FTxttbl.normalized|bool|[Val](/txt/exe/amc/reftypes.md#val)||Cell widths computed|

#### Struct FTxttbl
<a href="#struct-ftxttbl"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTxttbl { // algo_lib.FTxttbl: Table row. Todo: absolute index for cells?
    algo_lib::FTxtrow**   c_txtrow_elems;   // array of pointers
    u32                   c_txtrow_n;       // array of pointers
    u32                   c_txtrow_max;     // capacity of allocated array
    i32                   col_space;        //   2  Default extra space between columns
    bool                  normalized;       //   false  Cell widths computed
    // func:algo_lib.FTxttbl..Ctor
    inline               FTxttbl() __attribute__((nothrow));
    // func:algo_lib.FTxttbl..Dtor
    inline               ~FTxttbl() __attribute__((nothrow));
};
```

#### algo_lib.InTextFile - 
<a href="#algo_lib-intextfile"></a>

#### algo_lib.InTextFile Fields
<a href="#algo_lib-intextfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.InTextFile.file|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.InTextFile.own_fd|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||
|algo_lib.InTextFile.line_buf|[algo.LineBuf](/txt/protocol/algo/LineBuf.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.InTextFile.temp_buf|u8|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||

#### Struct InTextFile
<a href="#struct-intextfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct InTextFile { // algo_lib.InTextFile
    algo_lib::FFildes   file;                               //
    bool                own_fd;                             //   true
    algo::LineBuf       line_buf;                           //
    u8                  temp_buf_data[sizeof(u8) * 8192];   // place for data
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

#### algo_lib.Mmap - 
<a href="#algo_lib-mmap"></a>

#### algo_lib.Mmap Fields
<a href="#algo_lib-mmap-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Mmap.mem|[algo.memptr](/txt/protocol/algo/memptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Memory that has been mmap()ed|

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

#### algo_lib.MmapFile - 
<a href="#algo_lib-mmapfile"></a>

#### algo_lib.MmapFile Fields
<a href="#algo_lib-mmapfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.MmapFile.map|[algo_lib.Mmap](/txt/lib/algo_lib/README.md#algo_lib-mmap)|[Val](/txt/exe/amc/reftypes.md#val)||Pointer to shared memory|
|algo_lib.MmapFile.fd|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)||Associated file descriptor|
|algo_lib.MmapFile.text|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Alias to map.mem, accessible as text|

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

#### algo_lib.RegxToken - Used when parsing
<a href="#algo_lib-regxtoken"></a>

#### algo_lib.RegxToken Fields
<a href="#algo_lib-regxtoken-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxToken.type|i32|[Val](/txt/exe/amc/reftypes.md#val)||State|

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
|algo_lib.RegxExpr.type|[algo_lib.RegxToken](/txt/lib/algo_lib/README.md#algo_lib-regxtoken)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.RegxExpr.first|i32|[Val](/txt/exe/amc/reftypes.md#val)||First character|
|algo_lib.RegxExpr.last|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Last character(s)|

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
|algo_lib.RegxOp.op|u8|[Val](/txt/exe/amc/reftypes.md#val)|0|What test to perform|
|algo_lib.RegxOp.consume|u8|[Val](/txt/exe/amc/reftypes.md#val)|1|Number of characters to consume|
|algo_lib.RegxOp.imm|u16|[Val](/txt/exe/amc/reftypes.md#val)|0|immediate value|

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
|algo_lib.RegxParse.input|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Input string|
|algo_lib.RegxParse.nextgroup|i32|[Val](/txt/exe/amc/reftypes.md#val)||Next capture group #|
|algo_lib.RegxParse.p_regx|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Regx being compiled|
|algo_lib.RegxParse.ary_expr|[algo_lib.RegxExpr](/txt/lib/algo_lib/README.md#algo_lib-regxexpr)|[Tary](/txt/exe/amc/reftypes.md#tary)||Expression stack|

#### Struct RegxParse
<a href="#struct-regxparse"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxParse { // algo_lib.RegxParse: Function to parse regx
    algo::strptr          input;            // Input string
    i32                   nextgroup;        //   0  Next capture group #
    algo_lib::Regx*       p_regx;           // reference to parent row
    algo_lib::RegxExpr*   ary_expr_elems;   // pointer to elements
    u32                   ary_expr_n;       // number of elements in array
    u32                   ary_expr_max;     // max. capacity of array before realloc
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
|algo_lib.RegxState.ch_class|[algo.U16Ary](/txt/protocol/algo/README.md#algo-u16ary)|[Val](/txt/exe/amc/reftypes.md#val)||What to match|
|algo_lib.RegxState.op|[algo_lib.RegxOp](/txt/lib/algo_lib/README.md#algo_lib-regxop)|[Val](/txt/exe/amc/reftypes.md#val)||Operation to perform|
|algo_lib.RegxState.lparen|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.RegxState.next|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Where to go on a match|

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

#### algo_lib.Replscope - 
<a href="#algo_lib-replscope"></a>

#### algo_lib.Replscope Fields
<a href="#algo_lib-replscope-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Replscope.eatcomma|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Delete comma+space after substitution|
|algo_lib.Replscope.strict|u8|[Val](/txt/exe/amc/reftypes.md#val)||1=warnings; 2=throw exception on error|
|algo_lib.Replscope.ind_replvar|[algo_lib.FReplvar](/txt/lib/algo_lib/README.md#algo_lib-freplvar)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

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

#### algo_lib.ShHdr - 
<a href="#algo_lib-shhdr"></a>

#### algo_lib.ShHdr Fields
<a href="#algo_lib-shhdr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.ShHdr.magic|u32|[Val](/txt/exe/amc/reftypes.md#val)|0x09202017|Signature|
|algo_lib.ShHdr.name|[algo.RspaceStr32](/txt/protocol/algo/README.md#algo-rspacestr32)|[Val](/txt/exe/amc/reftypes.md#val)||User defined name|
|algo_lib.ShHdr.dataoffset|u64|[Val](/txt/exe/amc/reftypes.md#val)|4096|Offset to beginning of data|
|algo_lib.ShHdr.eof|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.ShHdr.sof|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.ShHdr.bufsize|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.ShHdr.pad|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

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
|algo_lib.Srng.z|u32|[Val](/txt/exe/amc/reftypes.md#val)|123||
|algo_lib.Srng.w|u32|[Val](/txt/exe/amc/reftypes.md#val)|456||

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
|algo_lib.Tabulate.width|i32|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|algo_lib.Tabulate.temp|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Tabulate
<a href="#struct-tabulate"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Tabulate { // algo_lib.Tabulate: Function to tabulate a string
    i32*            width_elems;   // pointer to elements
    u32             width_n;       // number of elements in array
    u32             width_max;     // max. capacity of array before realloc
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

