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
prlog_cat(algo_lib_logcat_xyz, "some output");
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
inline-command: acr logcat
dmmeta.logcat  logcat:expect  enabled:N  builtin:N  comment:"lib_ams expect implementation"
dmmeta.logcat  logcat:stderr  enabled:Y  builtin:Y  comment:"Standard error (cannot be disabled)"
dmmeta.logcat  logcat:stdout  enabled:Y  builtin:Y  comment:"Standard output (cannot be disabled)"
report.acr  n_select:3  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
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
strptr algo::gitinfo_Get() 
```

```c++
void algo::ShowStackTrace(uintptr_t start_ip, cstring &out) 
```

```c++
void algo::FatalErrorExit(const char *a) NORETURN 
```

```c++
// catch fatal signals and show backtrace
void algo::SetupFatalSignals() 
```

```c++
// print base64-encoded string
void algo::strptr_PrintBase64(strptr str, cstring &out) 
```

```c++
void algo_lib::InitCpuHz() 
```

```c++
= 
```

```c++
u32 algo::CRC32Step(u32 old, const u8 *data, size_t len) 
```

```c++
// This is a low-quality random number generator suitable for simple tasks...
// Set seed for srng state
void algo_lib::srng_SetSeed(algo_lib::Srng &srng, int z, int w) 
```

```c++
// retrieve random u32
u32 algo_lib::srng_GetU32(algo_lib::Srng &srng) 
```

```c++
// retrieve random double in 0..1 range
double algo_lib::srng_GetDouble(algo_lib::Srng &srng) 
```

```c++
// Normalize decimal, so mantissa does not contain tail 0
void algo::Decimal_Normalize(algo::Decimal &parent) 
```

```c++
// Try parse Decimal [+-][0-9]*[.][0-9]*
// Stop after first non-conforming char, error if no digit has been met.
bool algo::TryParseDecimal(algo::StringIter &iter, algo::Decimal &result) 
```

```c++
// Print Decimal
void algo::Decimal_Print(algo::Decimal parent, algo::cstring &str) 
```

```c++
// Read Decimal from string
bool algo::Decimal_ReadStrptrMaybe(algo::Decimal &parent, algo::strptr in_str) 
```

```c++
// Convert Decimal to double
double algo::Decimal_GetDouble(algo::Decimal parent) 
```

```c++
// Convert double to Decimal
void algo::Decimal_SetDouble(algo::Decimal &parent, double value) 
```

```c++
void algo_lib::SaveBadField(strptr name, strptr value) 
```

```c++
// Reset value of algo_lib::_db.errtext and return it for further editing
// Usage:
// algo_lib::ResetBadTags() << ...errors...
algo::cstring &algo_lib::ResetErrtext() 
```

```c++
// Add key-value pair to algo_lib::_db.errtext
// Error text beyond a reasonable limit is discarded -- keep errors short!
void algo_lib::SaveBadTag(const strptr &name, const strptr &value) 
```

```c++
// Better name than SaveBadTAg
void algo_lib::AppendErrtext(const strptr &name, const strptr &value) 
```

```c++
// Retrieve whatever bad tags were saved with SaveBadTag,
// and clear the state.
// SaveBadTag is typically called by string read functions that encounter
// something unreadable. This is the only way to retrieve that
// additional information
tempstr algo_lib::DetachBadTags() 
```

```c++
// Increment algo_lib::_db.trace.tot_insert_err
// And print accumulated 'bad tags' using prerr.
// if SetShowInsertErrLim was previously called.
// Note: printing is disabled by default, and must be explicitly enabled.
void algo_lib::NoteInsertErr(strptr tuple) 
```

```c++
// Specify how many times NoteInsertErr can use prerr
// to output.
void algo_lib::SetShowInsertErrLim(int n) 
```

```c++
inline char *ToCstr(char *to, const strptr &x) 
```

```c++
// Copy file FROM to file TO, creating the file if necessary
// with mode MODE.
// Throw exception on failure.
void algo::CopyFileX(strptr from, strptr to, int mode) 
```

```c++
// Read all bytes from IN_FD and copy them to OUT_FD, using read/write.
// Return success status.
bool algo::CopyFd(Fildes in_fd, Fildes out_fd) NOTHROW 
```

```c++
// Test whether FNAME refers to a valid filesystem entity (file, directory, or special file)
// If FNAME is a soft link, then TRUE is returned even if the link points to
// a non-existent location.
bool algo::FileObjectExistsQ(strptr fname) NOTHROW 
```

```c++
// Test whether PATH is an existing directory
bool algo::DirectoryQ(strptr path) NOTHROW 
```

```c++
// Test if F refers to an existing regular file (i.e. not a special file or directory)
bool algo::FileQ(strptr fname) NOTHROW 
```

```c++
// Wrapper for c library realpath function.
// On Windows: read path and expand all soft links along the way; Also eat ..'s.
// The following table shows successive values of LEFT and RIGHT as the loop executes
// ExpandLinks("bin/amc")
// LEFT                                   RIGHT
// bin                                    amc
// bin/amc
// link=../build/release/amc
// bin                                    ../build/release/amc
// build/release/amc
// build                                  release/amc
// build/release                          amc
// link=CYGWIN_NT-cl.release-x86_64
// build                                  CYGWIN_NT-cl.release-x86_64/amc
// build/CYGWIN_NT-cl.release-x86_64      amc
// build/CYGWIN_NT-cl.release-x86_64/amc
tempstr algo::GetFullPath(strptr path) NOTHROW 
```

```c++
// Delete file F.
// Return success code.
bool algo::DeleteFile(strptr f) NOTHROW 
```

```c++
// Remove everything up to and including the last slash in A,
// and return that. This is equivalent to Pathcomp(A,"/RR");
strptr algo::StripDirName(strptr a) NOTHROW 
```

```c++
// Strip last slash and everything after it in A,
// and return that. This is equivalent to Pathcomp(A,"/RL");
strptr algo::GetDirName(strptr a) NOTHROW 
```

```c++
// Recursively create directory S and all of its components,
// using mode MODE for newly created directories.
// if DO_THROW is specified, throw exceptions on failure.
// If DO_THROW is false, return success value.
// TODO: test on windows
bool algo::CreateDirRecurse(strptr s, bool do_throw = true, u32 mode = 0755) 
```

```c++
// Strip last component of a path.
// If the path ends in a slash, the slash is stripped as well.
// So, StripDirComponent("abcde/d/") -> abcde/
// So, StripDirComponent("abcde/d") -> abcde/
strptr algo::StripDirComponent(strptr a) NOTHROW 
```

```c++
// Combine paths A and B, placing a slash in the middle if necessary.
// If B is already an absolute pathname, just return that.
tempstr algo::DirFileJoin(strptr a, strptr b) NOTHROW 
```

```c++
// Remove pathname A with any extension stripped.
// StripExt("abcd/e.txt") -> "abcd/e"
// StripExt("abcd/e.txt/f") -> "abcd/e/f"
// (The last slash shields the extension, so this function has no corresponding
// Pathcomp expression)
strptr algo::StripExt(strptr a) NOTHROW 
```

```c++
// Return extension of pathname A, including the dot.
// GetFileExt("abcd/e.txt") -> ".txt"
// GetFileExt("abcd/e.txt/f") -> ""
// This function is equivalent to Pathcomp(A,"/RR.RR")
strptr algo::GetFileExt(strptr a) NOTHROW 
```

```c++
// Check if path PATH is an absolute pathname,
// meaning that it starts with / or ~
// On windows, the same test is in force, but in addition any pathname
// where the second character is a : (e.g. c:\blah) is recognized as an absolute
// path name
bool algo::AbsolutePathQ(strptr path) NOTHROW 
```

```c++
// Replace extension in pathname A with string B.
tempstr algo::ReplaceExt(strptr a, strptr b) NOTHROW 
```

```c++
// Return current directory name.
tempstr algo::GetCurDir() NOTHROW 
```

```c++
// Change current directory to DIR and return success status
// errno is set as witih chdir() call
bool algo::SetCurDir(strptr in_dir) NOTHROW 
```

```c++
// Calculate size of file referred to by FILENAME.
// If file is not found or an error occurs, 0 is returned.
i64 algo::GetFileSize(strptr filename) NOTHROW 
```

```c++
// Return size of file referred to by FD.
// On error, return zero.
i64 algo::GetFileSize(Fildes fd) NOTHROW 
```

```c++
// Strip extension from PATH, then strip directory name from PATH,
// and return the remainder.
// c.d      -> c
// /a/b/c.d -> c
strptr algo::GetFileName(const strptr& path) NOTHROW 
```

```c++
// F  ("a.txt","b") -> "b.txt";
// F  ("a.txt","/b.jpg") -> "b.jpg.txt"
tempstr algo::ReplaceFileName(const strptr& a, const strptr& b) 
```

```c++
// Return modification time of file FILENAME
// If file is not found or is not accessible, return 0.
algo::UnTime algo::ModTime(strptr filename) 
```

```c++
// Remove directory.
// Return TRUE if operaiton succeeds
// The function will fail if the directory is not empty. For that case,
// use RemDirRecurse.
bool algo::RemDir(strptr name) 
```

```c++
// Destroy directory NAME, and any subdirectories.
// If REMOVE_TOPMOST is specified, delete the directory as well,
// (otherwise, only the contents are deleted).
// If filesystem entry exists and is NOT a directory, return true.
bool algo::RemDirRecurse(strptr name, bool remove_topmost) 
```

```c++
// User-defined cleanup trigger for dir_handle field of ctype:algo.DirEntry
void algo::dir_handle_Cleanup(algo::DirEntry &dir_entry) 
```

```c++
// User-defined cleanup trigger fildes field of ctype:algo_lib.FLockfile
void algo_lib::fildes_Cleanup(algo_lib::FLockfile &lockfile) 
```

```c++
// Open file FILENAME with flags FLAGS, return resulting file descriptor
// Possible flags:
// write   -> open file for writing, create file if missing
// if write is specified, and append is not, then file is truncated upon opening
// append  -> open file in append mode (automatically sets 'write' flag)
// in append mode, seek to end of file after opening
// read    -> open flag in read-only mode
// _throw  -> throw exception if an error occurs
// NOTE: this function doesn't set O_APPEND flag, because it doesn't work'
// on NFS mounted filesystems.
// Without O_APPEND, two processes cannot reliably append to the same file.
algo::Fildes algo::OpenFile(const strptr& filename, algo::FileFlags flags) 
```

```c++
// Write BYTES to file FD;
// If FD is non-blocking, the function may fail.
// Throw exception on error. Upon completion, the function has succeeded
void algo::WriteFileX(Fildes fd, memptr bytes) 
```

```c++
// Set file position of FD to OFF
// Return success status
bool algo::SeekFile(Fildes fd, i64 off) 
```

```c++
// Return current file position on FD
// There is no provision to return an error code; only the offset is returned
// (zero on failure?)
i64 algo::GetPos(Fildes fd) 
```

```c++
// Truncate file indicated by FD to size SIZE.
// Return success status
// TODO: Test on windows
bool algo::TruncateFile(Fildes fd, i64 size) 
```

```c++
// Return contents of file as string.
// File is trimmed at first NULL character
// Implementation note:
// linux files in /proc directory lie about their size
// and are sometimes zero-terminated.
// File is read using a "safe" method of succesively calling read.
// relying on reported file size or using mmap does not work in all cases
// Todo: test on windows
const tempstr algo::FileToString(const strptr& fname, algo::FileFlags flags = algo_FileFlags__throw) 
```

```c++
// Read all bytes from IN_FD as a string.
// NUL characters in file will appear in the string --
// TODO: add option to sanitize string
// If FD is non-blocking, reading will terminate at first EAGAIN.
// If an error occurs, all data prior to occurrence of the error is returned.
const tempstr algo::FdToString(Fildes in_fd) NOTHROW 
```

```c++
algo::Fildes algo::CreateReplacementFile(cstring &oldfname, cstring &newfname, int dfltmode) 
```

```c++
// Compare STR to contents of FILENAME.
// If they differ, use a tempfile construction to atomically UPDATE file
// with contents of STR.
// NOTE: This will result in a new inode for file!
// No exceptions are thrown. If the function fails, check errno.
// Default mode for new file is provided by MODE.
// If the file is being replaced, MODE is ignored and copied from the old file.
bool algo::SafeStringToFile(const strptr& str, const strptr& filename, int dfltmode)
```

```c++
bool algo::SafeStringToFile(const strptr& str, const strptr& filename) 
```

```c++
// Replace contents of file FILENAME with string STR.
// If CHECK_SAME is specified, first compare contents and do not perform a write
// if the contents are the same.
// FLAGS may specify algo_FileFlags__throw, in which case an exception is thrown on error
bool algo::StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags = algo_FileFlags__throw, bool check_same = true)
```

```c++
algo::Fildes algo::OpenWrite(const strptr& filename, algo::FileFlags flags = algo::FileFlags()) 
```

```c++
algo::Fildes algo::OpenRead(const strptr& filename, algo::FileFlags flags = algo::FileFlags()) 
```

```c++
// Write NWRITE bytes starting at START to file descriptor FILDES.
// Go until all bytes are written on an error occurs.
// If FILDES is non-blocking, spin indefinitely until bytes do get through.
// At the end, return success status (TRUE if all bytes written)
bool algo::WriteFile(algo::Fildes fildes, u8 *start, int nwrite) 
```

```c++
void algo::Dir_curs_Next(Dir_curs &curs) NOTHROW 
```

```c++
// Begin scanning files matching shell pattern PATTERN.
void algo::Dir_curs_Reset(Dir_curs &curs, strptr pattern) NOTHROW 
```

```c++
bool algo::Dir_curs_ValidQ(Dir_curs &curs) NOTHROW 
```

```c++
algo::DirEntry &algo::Dir_curs_Access(Dir_curs &curs) NOTHROW 
```

```c++
// Set blocking mode on file descriptor FD to BLOCKING
// Return result of FCNTL (0==success)
int algo::SetBlockingMode(Fildes fildes, bool blocking) NOTHROW 
```

```c++
// If PATH is an existing path, leave it unchanged
// On Windows, If PATH.EXE is an existing path, return that
// Return true if file exists
bool algo_lib::TryExeSuffix(algo::cstring &path) 
```

```c++
// Update FNAME to be a filename that can be passed to Unix exec call.
// If FNAME is an absolute path, don't perform a search
// If FNAME is a relative path, perform a search using the PATH environment
// the first executable file that's found is the result.
void algo_lib::ResolveExecFname(algo::cstring &fname) 
```

```c++
// Read soft link and return resulting path.
// If PATH is not a soft link, return empty string
// This is not the function to resolve symlinks (if link temp/x points to y, then
// this function will return string "y", not "temp/y"), use GetFullPath for
// full service
tempstr algo::ReadLink(strptr path) 
```

```c++
// Examine string VALUE
// Determine if it needs quoting as an ssim key/value,
// and return appropriate quote character. If no quote character is needed,
// return NUL.
char algo::PickSsimQuoteChar(strptr value) 
```

```c++
bool char_ReadStrptrMaybe(char &row, algo::strptr str) 
```

```c++
bool float_ReadStrptrMaybe(float &row, algo::strptr str) 
```

```c++
bool double_ReadStrptrMaybe(double &row, algo::strptr str) 
```

```c++
bool bool_ReadStrptrMaybe(bool &row, algo::strptr str) 
```

```c++
// Read time from STR to ROW
// Return success code.
// If funciton does not succeed, ROW is not modified
// Several formats are supported:
// %Y-%m-%dT%T
// %Y-%m-%d %T
// %Y/%m/%d %T
// %Y-%m-%d
// %Y/%m/%d
// Where %T is %H:%M:%S.%X
// And %X is the nanosecond portion
bool algo::UnTime_ReadStrptrMaybe(algo::UnTime &row, algo::strptr str) 
```

```c++
bool algo::UnDiff_ReadStrptrMaybe(UnDiff &row, algo::strptr str) 
```

```c++
bool algo::UnixTime_ReadStrptrMaybe(algo::UnixTime &row, algo::strptr str) 
```

```c++
bool algo::cstring_ReadStrptrMaybe(algo::cstring &row, algo::strptr str) 
```

```c++
// Parse a URL from string STR to OUT.
// The format of a URL is
// protocol://user:password@somehost:port<-source_interface/dir1/dir2
// URL fields are
// protocol, username, password, server, port, host, source_addr_host, dir
// 
// 
// Windows pathnames are supported, e.g.
// when parsing
// file://c:/dir/dir2
// c: will not be parsed as a username, but as part of the pathname.
// 
bool algo::URL_ReadStrptrMaybe(URL &out, algo::strptr str) 
```

```c++
bool ietf::Ipv4_ReadStrptrMaybe(ietf::Ipv4 &ip, algo::strptr str) 
```

```c++
// Parse an IpV4 address from STR to IP
// Return success value. If not successful, output value is not modified.
bool ietf::Ipv4Addr_ReadStrptrMaybe(ietf::Ipv4Addr &ip, algo::strptr str) 
```

```c++
bool algo::Ipmask_ReadStrptrMaybe(Ipmask &row, algo::strptr str) 
```

```c++
void algo::Ptr_Print(void *ptr, algo::cstring &out) 
```

```c++
// pads with zeros on the left so that at least 'atleast' characters are output.
void algo::i64_PrintPadLeft(i64 num, algo::cstring &out, int atleast) 
```

```c++
// Assuming SRC is a number, Transfer SRC to OUT, inserting
// commas between groups of 3 numbers.
// 0 -> 0
// 11 -> 11
// 222 -> 222
// 3333 -> 3,333
// 4567.3 -> 4,567.3
// 1.11111 -> 1.11111
void algo::strptr_PrintWithCommas(strptr src, algo::cstring &out) 
```

```c++
// Assuming STR is a number, remove any unnecessary characters from the right of it.
// Unnecessary characters are trailing zeros after a dot.
// If the string contains no dot, nothing is done.
// If a trailing '.' or a single '-' remains, it is removed as well.
// If the resulting string is empty, a single zero is returned.
// 
// 0         -> 0
// 0.1       -> 0.1
// 0.0       -> 0
// 12345.000 -> 12345
// -0        -> -0
// .0        -> 0
// -.0       -> 0
// -0.0      -> -0
// -10.0     -> -10
// ->            empty string is not touched
// Since the string may be edited, you can't pass compile-time constants
// to this function. In fact the only reason this function is not private to algo_lib
// is because of unit testing.
// If the initial string is empty, nothing is done.
// BEWARE: this function will happily convert 1e+60 to 1e+6
// 
void algo::strptr_TrimZerosRight(strptr &str) 
```

```c++
// Print double D into string OUT with PRECISION digits
// after the decimal point.
// If OMIT_ZEROS is specified, trailing zeros that are safe to omit are omitted.
// If COMMAS is specified, the large numbers are printed in groups of 3 digits
// with commas between them.
void algo::double_PrintPrec(double d, algo::cstring &out, int precision, bool omit_zeros, bool commas) 
```

```c++
// Print string, replacing '_x' with 'X', and capitalizing first word.
void algo::strptr_PrintCamel(algo::strptr str, algo::cstring& out) 
```

```c++
// Convert CamelCase to lower_under.
// A consecutive run of caps gets a single leading underscore.
// Leading cap doesn't get an underscore
// Example: "abcd testString"  -> "abcd test_string"
// Example: "Abcd" -> "abcd"
// Example: "BBOPrice" -> "bboprice"
// Example: "SymbolID" -> "symbol_id"
// Example: "A_B" -> "a_b"
void algo::strptr_PrintLowerUnder(algo::strptr str, algo::cstring &out) 
```

```c++
// Output string STR to OUT changing all characters to uppercase.
void algo::strptr_ToUpper(algo::strptr str, algo::cstring& out) 
```

```c++
void algo::u128_PrintHex(u128 value, algo::cstring &out, bool prefix) 
```

```c++
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix,  bool caps) 
```

```c++
// Print value VALUE to string OUT,
// producing least ATLEAST hex characters.
// Include prefix 0x.
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast) 
```

```c++
// Print value VALUE to string OUT,
// producing least ATLEAST hex characters.
// if PREFIX is specified, include prefix 0x.
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix) 
```

```c++
// print character as c++ character, surrounded by single quotes
// and properly escaped according to c++ rules
void algo::char_PrintCppSingleQuote(int c, algo::cstring &out) 
```

```c++
// prints a character suitable for appearance inside an XML string.
// character is either printed as-is, or is escaped so that its meaning does not change.
void algo::char_PrintXml(char c, algo::cstring &out, bool quoted) 
```

```c++
// Print string NAME as a TeX identifier to OUT, converting digits to letters
// (0 -> zero, etc) and squashing other characters not belonging to TeX
// identifier character class
void algo::strptr_PrintTexIdent(strptr name, algo::cstring &out) 
```

```c++
// print STR to OUT, escaping TeX characters that switch modes
// (_, #, &, $)
void algo::strptr_PrintTexString(strptr str, algo::cstring &out) 
```

```c++
// Print string using XML rules.
void algo::strptr_PrintXml(algo::strptr s, algo::cstring &out, bool quoted) 
```

```c++
// Append N instances of character C to string OUT.
void algo::char_PrintNTimes(char c, algo::cstring &out, int n) 
```

```c++
void algo::strptr_PrintAligned(algo::strptr str, algo::cstring &out, int nplaces, algo::TextJust align, char c) 
```

```c++
void i32_Print(i32 i, algo::cstring &str) 
```

```c++
void i64_Print(i64 i, algo::cstring &str) 
```

```c++
void i16_Print(i16 i, algo::cstring &str) 
```

```c++
void u16_Print(u16 i, algo::cstring &str) 
```

```c++
void u8_Print(u8 i, algo::cstring &str) 
```

```c++
void i8_Print(i8 i, algo::cstring &str) 
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
void algo::cstring_Print(algo::cstring &row, algo::cstring &str) 
```

```c++
void algo::strptr_Print(const algo::strptr &row, algo::cstring &str) 
```

```c++
// Append a space unless the string already ends with a space
void  algo::MaybeSpace(cstring &str) 
```

```c++
// print value*100
// Example:
// double_PrintPercent(0.334, str, 1) -> "33.4%"
void algo::double_PrintPercent(double value, algo::cstring &str, int prec) 
```

```c++
void algo::i32_Range_Print(algo::i32_Range &r, algo::cstring &o) 
```

```c++
void double_Print(double d, algo::cstring &str) 
```

```c++
void float_Print(float d, algo::cstring &str) 
```

```c++
void algo::double_PrintWithCommas(double value, algo::cstring &str, int prec) 
```

```c++
// ignore:bigret
algo::cstring algo::double_PrintWithCommas(double value, int prec) 
```

```c++
// Print scaled decimal.
// VALUE           integer value
// SCALE           log10 to get the real value: real value = value / pow(10,scale)
// BUF             target buffer. buffer must be large enough.
// FIXED           if true, make sure there are exactly SCALE characters after decimal point (and always emit decimal point)
// RETURN          number of characters printed.
// Example:
// u64_FmtBufDec(0    , 2, .. false ) -> "0"
// u64_FmtBufDec(0    , 2, .. true  ) -> "0.00"
// u64_FmtBufDec(123  , 1, .. false ) -> "12.3"
// u64_FmtBufDec(123  , 2, .. false ) -> "1.23"
// u64_FmtBufDec(123  , 2, .. true  ) -> "1.23"
// u64_FmtBufDec(1230 , 3, .. false ) -> "1.23"
// u64_FmtBufDec(1230 , 3, .. true  ) -> "1.230"
u32 algo::u64_FmtBufDec(u64 num, int scale, u8 *buf, bool fixed) 
```

```c++
u32 algo::u64_FmtBufDec(u64 num, int scale, u8 *buf) 
```

```c++
// print TIME to STR, according to spec SPEC
// The following control characters are supported in SPEC.
// %Y     Year printed as 4 digits, e.g. 2002
// %y     Last 2 digits of year printed as 2 digits
// %B     Long month name (February)
// %b     Short month name (Feb)
// %a     Week day name (Monday)
// %m     Month printed as 2 digits, with January being 01
// %d     Day of month printed as at least 2 digits (or WIDTH)
// %.X    where X is a number -- set width
// %I     Hour using 12-hour format
// %H     Hour using 24-hour format, printed as 2 digits
// %M     Minute, printed as 2 digits
// %S     Second, printed as 2 digits
// %T     Shortcut for %H:%M:%S.%.9X
// %X     Print fractions of a second according to width
// %x     Print milliseconds with 3 decimal places
// %-     Print - sign if timespec is negative
// %%     Print % sign
void algo::TimeStruct_Print(const TimeStruct &time, algo::cstring &str, const algo::strptr &spec) 
```

```c++
void algo::Tuple_Print(algo::Tuple &row_, algo::cstring &str) 
```

```c++
void char_Print(char c, algo::cstring &str) 
```

```c++
void bool_Print(bool row, algo::cstring &str) 
```

```c++
void algo::Bool_Print(algo::Bool row, algo::cstring &str) 
```

```c++
void algo::UnDiff_Print(UnDiff row, algo::cstring &str) 
```

```c++
void algo::UnixDiff_Print(UnixDiff row, algo::cstring &str) 
```

```c++
void algo::UnTime_Print(algo::UnTime row, algo::cstring &str) 
```

```c++
void algo::UnixTime_Print(algo::UnixTime row, algo::cstring &str) 
```

```c++
void algo::UnTime_PrintSpec(UnTime t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
void algo::UnixTime_PrintSpec(UnixTime t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
void algo::UnDiff_PrintSpec(UnDiff   t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
void algo::UnixDiff_PrintSpec(UnixDiff t, algo::cstring &out, const algo::strptr &spec) 
```

```c++
void ietf::Ipv4_Print(ietf::Ipv4 row, algo::cstring &str) 
```

```c++
void ietf::Ipv4Addr_Print(ietf::Ipv4Addr row, algo::cstring &str) 
```

```c++
void algo::Ipmask_Print(algo::Ipmask &row, algo::cstring &str) 
```

```c++
void algo::Errcode_Print(algo::Errcode &row, algo::cstring &str) 
```

```c++
void algo_lib::ErrorX_Print(algo_lib::ErrorX &row, algo::cstring &str) 
```

```c++
void ietf::Ipport_Print(ietf::Ipport row, algo::cstring &str) 
```

```c++
bool ietf::Ipport_ReadStrptrMaybe(ietf::Ipport &row, strptr in) 
```

```c++
// Append STR to OUT, using comma-separated-values encoding
// If QUOTE is 0, the need for quotes and the type of quote is determined automatically.
// If it is any other value, then that type of quote is used
// According to the CSV standard, the quote itself is the escape character.
void algo::strptr_PrintCsv(algo::strptr str, algo::cstring &out, char quote) 
```

```c++
// Print CSV field, auto-determine quotes
void algo::strptr_PrintCsv(algo::strptr str, algo::cstring &out) 
```

```c++
void algo_lib::FTxttbl_Print(algo_lib::FTxttbl &T_, algo::cstring &str) 
```

```c++
void algo_lib::FTxttbl_Markdown(algo_lib::FTxttbl &T_, algo::cstring &str) 
```

```c++
void algo::URL_Print(algo::URL &url, algo::cstring &str) 
```

```c++
// Append STR to OUT, and pad remainder with character FILL
void algo::strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces, char fill) 
```

```c++
// Append STR to OUT, and pad remainder with space characters
void algo::strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces) 
```

```c++
// Append STR to OUT, padding start with character FILL
void algo::strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces, char fill) 
```

```c++
// Append STR to OUT, padding start with spaces.
void algo::strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces) 
```

```c++
// Append STR to OUT as an SQL string using quote character Q.
// Escaped characters:
// '\' (backslash)
// ' quoted with '' if q is ' and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
// " quoted with "" if q is " and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
// NUL (ascii zero)
// \n
// \r
// \t
void algo::strptr_PrintSql(algo::strptr str, algo::cstring &out, char q) 
```

```c++
void algo::strptr_PrintSql(algo::strptr str, algo::cstring &out) 
```

```c++
void algo::Attr_Print(algo::Attr &attr, algo::cstring &str) 
```

```c++
// Print a string suitable for parsing with Tuple
// If the string doesn't need quotes, it is printed as-is.
// If quotes are needed, first determine whether to use single or double quotes to minimize
// escaping. Then print the string using char_PrintCppEsc for quoting.
// Empty string is printed as ""
void algo::PrintAttrSpace(cstring &str, algo::strptr name, const strptr &value) 
```

```c++
void algo::ListSep_Print(const algo::ListSep &t, algo::cstring &str) 
```

```c++
// copy string B to TO, using ORIG as a case template
// I.e. PrintCopyCase("AbcD", to, "somestring") -> "SomEstring"
void algo::strptr_PrintCopyCase(const algo::strptr &orig, algo::cstring &to, const algo::strptr &b) 
```

```c++
bool algo::Tuple_ReadStrptrMaybe(Tuple &row, algo::strptr s) 
```

```c++
// T             target tuple. the tuple is not emptied before parsing.
// STR           source string
// ATTRONLY        if set, all loaded attrs are appended to the ATTRS
// array. otherwise, the first attr becomes HEAD.
// CMT_CHAR      character at which to stop parsing.
// Parse sequence of attrs (name-value pairs) into tuple T.
// Roughly:
// ATTR       -> VALUE | VALUE ':' VALUE
// VALUE      -> IDENTIFIER | C++-STRING
// Return value: FALSE if unterminated string or other badness found in data.
bool algo::Tuple_ReadStrptr(algo::Tuple &tuple, strptr str, bool attronly) 
```

```c++
// Implementations of built-in read functions for AMC.
// basis function for reading tuples.
// scan ITER for identifier, or quoted string.
// return FALSE if attribute is malformed (i.e. unterminated string)
bool algo::cstring_ReadCmdarg(cstring &out, algo::StringIter &S, bool is_value) 
```

```c++
// Read Charset from list of chars.
// Every character in RHS is simply added to the bitset
void algo::Charset_ReadStrptrPlain(algo::Charset &lhs, strptr desc) 
```

```c++
bool algo::Charset_ReadStrptrMaybe(algo::Charset &lhs, strptr rhs) 
```

```c++
// Print STR to OUT in a way that's acceptable as input for bash.
void algo::strptr_PrintBash(strptr str, algo::cstring &out) 
```

```c++
// Escape S according to bash rules and return result
tempstr algo::strptr_ToBash(strptr str) 
```

```c++
// Escape S according to ssim rules and return result
tempstr algo::strptr_ToSsim(strptr str) 
```

```c++
// encode uri component.
// before ? use plus=false (no + substitution by space)
void algo::strptr_PrintUri(strptr str, algo::cstring &out, bool plus) 
```

```c++
// Print string, escaping characters in a way suitable for graphviz dot format
void algo::strptr_PrintDot(strptr s, algo::cstring &out) 
```

```c++
// print binary octet string as hex
void algo::Sha1sig_Print(algo::Sha1sig &sha1sig, algo::cstring &out) 
```

```c++
bool algo::Sha1sig_ReadStrptrMaybe(algo::Sha1sig &sha1sig, algo::strptr str) 
```

```c++
// Write character C into string OUT, using C++ character escapement rules
// QUOTE_CHAR also gets escaped.
void algo::char_PrintCppEsc(char c, algo::cstring &out, char quote_char) 
```

```c++
// Print STR, surrounded by quotes as C++ string
// surrounded by QUOTE_CHAR quotes, to buffer OUT.
// All string characters are escaped using char_PrintCppEsc.
void algo::strptr_PrintCppQuoted(algo::strptr str, algo::cstring &out, char quote_char) 
```

```c++
// Print STR as a C++ string to OUT.
void algo::strptr_PrintCpp(algo::strptr str, algo::cstring &out) 
```

```c++
// Print STR as a valid SSIM key/value to OUT.
// SSIM tokens use quotes only when the printed value contains
// characters outside of the 'safe' set a-zA-Z0-9_;&*^%$@.!:,+/-
void algo::strptr_PrintSsim(algo::strptr str, algo::cstring &out) 
```

```c++
// Begin append key/value to string LHS
int algo::BeginKeyval(cstring &lhs, const strptr &name) 
```

```c++
// End append key/value to string LHS
cstring &algo::EndKeyval(cstring &lhs, int start) 
```

```c++
// Append key-value pair NAME:VALUE to string STR
void algo::PrintAttr(cstring &str, const algo::strptr &name, const strptr &value) 
```

```c++
// Append key-value pair NAME:VALUE to string STR, and remove all
// characters from VALUE (used by amc)
void algo::PrintAttrSpaceReset(cstring &str, const algo::strptr &name, algo::cstring &value) 
```

```c++
// Append key-value pair NAME:VALUE to string STR,
// and remove all characters from VALUE (used by amc)
void algo::PrintAttrSpaceReset(cstring &str, const char *name, algo::cstring &value) 
```

```c++
// Convert VALUE to a string, encoded as a u64.
// Value must be >=0 and <100000000.
// Resulting u64 is little-endian -- MSB contains first byte
// of the resulting string. To be used as an ascii string, value
// 0x3030303030303030 (8 ascii '0's) must be added to the result.
// Don't use this function. Use u32_FmtBufSmall, and u64_FmtBuf.
u64 algo::u32_CvtLestr8Small(u32 value) 
```

```c++
// Format VALUE, which should be >=0 and <100000000,
// as a string and write it to buffer BUF. Return length of resulting string.
// Buffer must be big enough.
u32 algo::u32_FmtBufSmall(u32 value, u8 *buf) 
```

```c++
// Format VALUE, which is an arbitrary u64,
// as a string and write it to buffer BUF.
// Return length of resulting string.
// Buffer must be big enough.
u32 algo::u64_FmtBuf(u64 value, u8 *buf) 
```

```c++
// Format VALUE, which is an arbitrary u32,
// as a string and write it to buffer BUF.
// Return length of resulting string.
// Buffer must be big enough.
u32 algo::u32_FmtBuf(u32 value, u8 *buf) 
```

```c++
u64 algo::aParseNum16(strptr s, u32 &ok) 
```

```c++
// STR         little-endian string up to 2 chars in length
// LEN         length of STR
// RESULT      value of hex number encoded in STR
// RETURN      number of characters to skip (0 - unsuccessful)
int algo::ParseHex2(u32 str, int len, u8 &result) 
```

```c++
// STR        little-endian string up to 4 chars
// LEN        length of STR
// RESULT     Octal value encoded in str
// RETURN     number of characters to skip (0 - unsuccessful)
int algo::ParseOct3(u32 str, int len, u8 &result) 
```

```c++
u32 algo::ParseNum4(u32 x, u32 len, u32 &ok) 
```

```c++
u32 algo::ParseNum8(u64 x, u32 len, u32 &ok) 
```

```c++
// Print a string as a classic regex, escaping all special
// characters. This regex will only match the specified string.
void algo::strptr_PrintRegxSql(algo::strptr value, algo::cstring &str) 
```

```c++
void algo::WDiff_Print(algo::WDiff row, algo::cstring &str) 
```

```c++
void algo::WTime_Print(algo::WTime row, algo::cstring &str) 
```

```c++
bool algo::WDiff_ReadStrptrMaybe(algo::WDiff &parent, algo::strptr in_str) 
```

```c++
bool algo::WTime_ReadStrptrMaybe(algo::WTime &parent, algo::strptr in_str) 
```

```c++
void algo::u64_PrintBase32(u64 k, algo::cstring &str) 
```

```c++
void algo::Uuid_Print(algo::Uuid &parent, algo::cstring &str) 
```

```c++
void algo_lib::IohookInit() 
```

```c++
// Register IOHOOK to be called whenever an IO operation is possible.
// OK to add an fd twice with different flags. Subsequent calls override previous ones.
// Add iohook to epoll in read, write or read/write mode
// Optionally, add as edge triggered
void algo_lib::IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) NOTHROW 
```

```c++
// De-register interest in iohook
void algo_lib::IohookRemove(algo_lib::FIohook& iohook) NOTHROW 
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
// Execute unix command and return output.
tempstr algo::SysEval(strptr cmd, FailokQ fail_ok, int max_output, bool echo = false) 
```

```c++
// Execute unix command and return status code.
// Execute command using system().
// fail_ok: if true, command must succeed or throw exception
// dry_run: if true, do not run command, just print
// echo: print command. command alwyas echoed if verbose mode is on
int algo::SysCmd(strptr cmd, FailokQ fail_ok = FailokQ(true), DryrunQ dry_run = DryrunQ(false), EchoQ echo = EchoQ(false)) 
```

```c++
// Return a human-readable description of STATUS as returned by wait() / waitpid()
tempstr algo::DescribeWaitStatus(int status) 
```

```c++
// The several variants of Throw are all the same -- the goal is to get as many
// setup instructions out of the execution path as possible, to avoid polluting instruction
// cache.
void algo::Throw(strptr text, Errcode err) NORETURN 
```

```c++
void algo::Throw() NORETURN 
```

```c++
// Set exit time of main loop to current time.
void algo_lib::ReqExitMainLoop() 
```

```c++
void algo_lib::fd_Cleanup(algo_lib::FFildes &fildes) 
```

```c++
Attr* algo::attr_Find(Tuple &tuple, strptr name, int occurence = 0) 
```

```c++
strptr algo::attr_GetString(Tuple &T, strptr name, strptr dflt = strptr()) 
```

```c++
// Append new attribute with specified name and value.
void algo::attr_Add(Tuple &T, strptr name, strptr value) 
```

```c++
void algo_lib::fildes_Cleanup(algo_lib::FIohook &iohook) 
```

```c++
// Call sleep/usleep, giving up MS milliseconds
void algo::SleepMsec(int ms) 
```

```c++
Errcode algo::FromErrno(i64 val) 
```

```c++
Errcode algo::FromWinErr(i64 val) 
```

```c++
i64 algo::I64Power10(u32 power) 
```

```c++
// Copy bytes from one location to another in blocks of 16 bytes.
void algo::StreamFromToB16(void *from, void *to, u32 n) 
```

```c++
void algo::CopyFromToB16(void *from, void *to, u32 n) 
```

```c++
// SSE implementation of fast exponentiation idea from
// http//nic.linotune.com/pubs/Schraudolph99.pdf
// ExpFast(x) goes bad at x < -709 (exp doesn't)
// Where it works, ExpFast makes a steady relative error of about 2%.
double algo::ExpFast(double y) 
```

```c++
// lock all presently, and future allocated bytes in physical memory.
// Return success value. Errno provides more info.
bool algo::LockAllMemory() 
```

```c++
void algo::SetupExitSignals(bool sigint = true) 
```

```c++
void algo_lib::bh_timehook_Step() 
```

```c++
// Check signature on incoming data
bool algo_lib::dispsigcheck_InputMaybe(dmmeta::Dispsigcheck &dispsigcheck) 
```

```c++
const tempstr algo::GetHostname() 
```

```c++
const tempstr algo::GetDomainname() 
```

```c++
// Die when parent process dies
void algo_lib::DieWithParent() 
```

```c++
// Create temporary file
// tempfile.fildes points to the new temp file after this
// temp file is created under temp/
// prefix is a namespace-unique name, such as "amc.xyz"
// Actual file that's created becomes "temp/amc.xyz.XXXXXX"
// Computed filename is saved to tempfile.filename
void algo_lib::TempfileInitX(algo_lib::FTempfile &tempfile, strptr prefix) 
```

```c++
void algo_lib::fildes_Cleanup(algo_lib::FTempfile &tempfile) 
```

```c++
double algo::double_WeakRandom(double scale) 
```

```c++
i32 algo::i32_WeakRandom(i32 modulo) 
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
// Interpret redirect string and make DST_FD consistent with
// the intended state. Return 0 on success, -1 on failure
// This function is usually called in the child process right after fork
// See CreateRedirect for interpretation of redirect string
int algo_lib::ApplyRedirect(strptr redirect, int dst_fd) 
```

```c++
bool algo_lib::IpmaskValidQ(const strptr ipmask) 
```

```c++
// Return TRUE if current user is root.
// #AL# is this more correct than checking that effective uid is 0?
bool algo_lib::RootQ() 
```

```c++
// Return name of user executing this command.
// If the command is being executed under sudo, return name
// of original user.
tempstr algo_lib::EffectiveUser() 
```

```c++
// if OWN_FD is cleared, clean up file descriptor before it is closed
void algo_lib::file_Cleanup(algo_lib::InTextFile &file) 
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
// Return computed name for sandbox SANDBOX
tempstr algo_lib::SandboxDir(algo::strptr sandbox) 
```

```c++
// Enter sandbox directory remember previous directory
void algo_lib::PushDir(algo::strptr dir) 
```

```c++
// Change to the directory that was current before sandbox mode
// Must be balanced with PushDir
void algo_lib::PopDir() 
```

```c++
// Accept data block DATA as input for the line buffer
// LinebufNext will attempt to return pointers to a subregion of DATA until
// it returns false, so this block must be valid until then.
// If IMPLIED_EOF is set, then this is the last call to LinebufBegin.
// LinebufNext will know to return a partial line (with no \n) and set the
// EOF flag
void algo::LinebufBegin(LineBuf &L, memptr data, bool implied_eof) 
```

```c++
// Scan memory provided by LinebufBegin for line separator (hard-coded to be '\n')
// If the function returns FALSE, and there is a partial line remaining,
// it is saved to an internal buffer in LineBuf (called BUF).
// It will be subsequently recombined with incoming data to form a contiguous line,
// but copying memory is avoided whenever possible.
// Usage:
// LinebufBegin(linebuf,data,true);
// for (strptr line; LinebufNext(linebuf,line); ) {
// <process line>
// }
bool algo::LinebufNext(LineBuf &L, strptr &result) 
```

```c++
// Read characters up to newline or end of file.  Return
// resulting substring, skipping the newline
// (which is either \n or \r\n).
strptr algo::GetLine(StringIter &S) 
```

```c++
// Line_curs uses strptr to hold on to the string being scanned
// (to avoid copying a potentially huge string)
// The use of "strptr&" prevents passing a temporary.
// Line_curs handling of newlines is identical to that of FileLine_curs
void algo::Line_curs_Reset(Line_curs &curs, algo::strptr &text) 
```

```c++
void algo::Line_curs_Reset(Line_curs &curs, algo::cstring &text) 
```

```c++
// Scan for next line.
void algo::Line_curs_Next(Line_curs &curs) 
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
void algo::FileLine_curs_Reset(algo::FileLine_curs &curs, strptr filename) 
```

```c++
void algo::FileLine_curs_Reset(algo::FileLine_curs &curs, algo::Fildes fd) 
```

```c++
void algo::FileLine_curs_Next(algo::FileLine_curs &curs) 
```

```c++
// Attempt to lock the file named NAME, updating state of LOCKFILE.
// Return TRUE if lock is successfully applied.
// If FAIL_OK is false, an error causes an exception (function always returns true)
// If an exception is thrown, the text contains error description.
// Otherwise, algo_lib::_db.errtext (DetachBadTags()) contains human-readable description of error.
// If WAIT_TIMEOUT is non-zero, block up to WAIT_TIMEOUT seconds before failing
// Write pid to file specified in NAME, and lock file using flock().
bool algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok, algo::UnDiff wait_timeout) 
```

```c++
// Write pid to lockfile, separate function to update pid after fork().
// Sets error text in case of error, and return false.
bool algo_lib::WritePid(algo_lib::FLockfile &lockfile) 
```

```c++
// Non-blocking attempt to lock LOCKFILE
// Return success status
// If FAIL_OK is FALSE, throw exception on error (must succeed)
bool algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok) 
```

```c++
// Non-blocking attempt to lock LOCKFILE.
// Throw exception on error.
// If the function returns, it has succeeded.
void algo_lib::LockFileInit(algo_lib::FLockfile &lockfile, strptr name) 
```

```c++
// Read contents of lock file FNAME, extracting the pid that's stored in there.
// On failure, return 0
pid_t algo_lib::LockFileRead(strptr fname) 
```

```c++
// User-defined cleanup function for MMAP.MEM
void algo_lib::mem_Cleanup(algo_lib::Mmap &mmap) 
```

```c++
// Attach mmapfile MMAPFILE to FD.
// Return success code.
bool algo_lib::MmapFile_LoadFd(MmapFile &mmapfile, algo::Fildes fd) 
```

```c++
// Attach mmapfile MMAPFILE to FNAME
// Return success code.
bool algo_lib::MmapFile_Load(MmapFile &mmapfile, strptr fname) 
```

```c++
// Default implementation of prlog handler
// 
// Notes on WriteFile use:
// some tools set fd 1 to nonblocking mode, which causes EAGAIN during fast writes,
// so we must use WriteFile (which contains a loop) to write all the bytes out.
void algo::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) 
```

```c++
// there is not enough information in a regx expression to fully specify it.
// sql vs shell vs classic regx, vs acr, partial vs full.
// we print back the original expression that was read in, but the information
// about what function read it is lost.
void algo_lib::Regx_Print(algo_lib::Regx &regx, algo::cstring &lhs) 
```

```c++
// Check if REGX matches S, return result
bool algo_lib::Regx_Match(algo_lib::Regx &regx, algo::strptr text) 
```

```c++
// Parse string INPUT as regex and store in REGX.
// Supported features:
// \n,\r,\t,\a,\e,\v,\f, [..], $, ^,
// ., *, +, (..), {..}, \d,\D,\w,\W,\s,\S
void algo_lib::Regx_ReadDflt(algo_lib::Regx &regx, algo::strptr input) 
```

```c++
// Parse bash-style regx:
// * -> .*
// ? -> .?
// All other regx chars are escaped away
// if FULL is set to false, input is treated as ".*input.*"
void algo_lib::Regx_ReadShell(algo_lib::Regx &regx, algo::strptr input, bool full) 
```

```c++
// Parse SQL-style regx:
// % is rewritten as .*
// _ is rewritten as .
// (, ), [, ] are passed through
// ., *, ?, + are escaped
// if FULL is set to false, input is treated as ".*input.*"
void algo_lib::Regx_ReadSql(algo_lib::Regx &regx, algo::strptr input, bool full) 
```

```c++
// Parse ACR-style regx:
// % is rewritten as .*
// (, ), [, ], _ are passed through
// ., *, ?, + are escaped
// if FULL is set to false, input is treated as ".*input.*"
// If the input expression can be matched as a string, set REGX.LITERAL to true
void algo_lib::Regx_ReadAcr(algo_lib::Regx &regx, algo::strptr input, bool full) 
```

```c++
// Set REGX to match string INPUT literally
void algo_lib::Regx_ReadLiteral(algo_lib::Regx &regx, algo::strptr input) 
```

```c++
bool algo_lib::Regx_ReadStrptrMaybe(algo_lib::Regx &regx, algo::strptr input) 
```

```c++
// S         source string
// EXPR      string in the form (XYZ)*
// - X is the character to search
// - Y is L to search from left, R to search from right
// - Z is which site to pick: L=left, R=right
// If the character to search is not found, it assumed to be found
// at the far end of the string with respect to search direction.
// Function processes characters from expr in groups of 3.
// Example:
// s = "a.b.c.d"
// expr = ".LR.LL"
// first, search for . from left; pick right hand side (b.c.d)
// then, search for . from left. pick left hand side (b)
// result: b
// More examples:
// s = "abcd"; expr = ".LL"; result = "abcd"
// s = "abcd"; expr = ".LR"; result = ""
strptr algo::Pathcomp(strptr s, strptr expr) 
```

```c++
// Check if the Pathcomp expression PATHCOMP
// refers to the leftmost part of the key
// (The third character of each Pathcomp step must be 'L')
bool algo::LeftPathcompQ(algo::strptr pathcomp) 
```

```c++
// Append NUL character to the end of the string and return a
// pointer to the C string.
// There is no guarantee that there are no NUL characters in the middle
// of STR, that is up to the user.
// cstring is normally NOT null terminated (it is length-based),
// so this function is necessary when passing strings to C library functions
// or the OS
char *algo::Zeroterm(cstring &rhs) 
```

```c++
// Same thing but with tempstr.
// Typical usage is to take a strptr
// expression and pass it to some unix call
// some_unix_call(Zeroterm(tempstr(some_strptr)))
// 
char *algo::Zeroterm(const tempstr &rhs) 
```

```c++
// Construct ssim filename from key
// SsimFname("data", "abcd") -> data/abcd.ssim
// SsimFname("data/", "abcd.ef") -> data/abcd/ef.ssim
tempstr algo::SsimFname(strptr root, strptr rel) 
```

```c++
// Extract Namespace name from "ns.name" format.
// This is equivalent to Pathcomp(s,".LL")
strptr algo::GetNs(strptr s) 
```

```c++
// if IDENT starts with NS_NAME., return ident without prefix
// otherwise return ident
// if NS_NAME is empty, removes any dotted prefix from IDENT.
// NS_NAME      name of 'current' namespace (optional)
// IDENT        identifier in question
// 
// StripNs("abc", "abc.def") -> "def"
// StripNs(""   , "abc.def") -> "def"
// StripNs("abc", "def.ghi") -> "def.ghi"
strptr algo::StripNs(strptr ns_name, strptr ident) 
```

```c++
// This function echoes the corresponding MySQL function.
// Treat STR as an array of tokens separated by C.
// Locate and return tokens 1..IDX. Negative numbers cause scanning from the right.
// Indexes are 1-based
// Example:
// SubstringIndex("a.b.c", '.',  1) -> "a"
// SubstringIndex("a.b.c", '.',  2) -> "a.b"
// SubstringIndex("a.b.c", '.',  3) -> "a.b.c"
// SubstringIndex("a.b.c", '.',  4) -> "a.b.c"
// SubstringIndex("a.b.c", '.', -1) -> "c"
// SubstringIndex("a.b.c", '.', -2) -> "b.c"
// SubstringIndex("a.b.c", '.', -3) -> "a.b.c"
strptr algo::SubstringIndex(strptr str, char c, int idx) 
```

```c++
int algo::FindFrom(strptr s, strptr t, int from, bool case_sensitive) 
```

```c++
int algo::FindFrom(strptr s, strptr t, int from) 
```

```c++
int algo::FindFrom(strptr s, char c, int from) 
```

```c++
// Search for character/string from left to right
// If found, return index where match occurs.
// If not found, return -1
int algo::FindChar(strptr lhs, char c) 
```

```c++
int algo::FindStr(strptr lhs, strptr t, bool case_sensitive) 
```

```c++
int algo::FindStr(strptr lhs, strptr t) 
```

```c++
int algo::CompareNoCase(strptr lhs, strptr s) 
```

```c++
// Check strings for equality, optionally in a case-insensitive way
bool algo::StrEqual(strptr a, strptr b, bool case_sens) 
```

```c++
bool algo::StartsWithQ(strptr s, strptr sstr, bool case_sensitive = true) 
```

```c++
bool algo::EndsWithQ(strptr s, strptr sstr) 
```

```c++
void algo::MakeLower(strptr s) 
```

```c++
void algo::MakeUpper(strptr s) 
```

```c++
// Locate all occurences of SEP in string IN, scanning line-by-line.
// Compute column widths; Scan string IN again.
// Expand each column to the required width, leaving COLSPACE characters between
// columns.
// String FMT specifies, for each column, whether to align its contents left,center,or right
// ('l','c','r'). Default is 'l'.
// Example text
// aa\tcc\tbb
// asdfasdf\t\tyy
// Tabulated(text, "\t", "rl", 2) ->
// aa  cc  bb
// asdfasdf      yy
tempstr algo::Tabulated(strptr in, strptr sep, strptr fmt, int colspace) 
```

```c++
tempstr algo::Tabulated(strptr in, strptr sep) 
```

```c++
// Given a string of up to 4 characters encoded in u32 STR
// (LSB = first char, etc.)
// Decode character into RESULT, returning number of characters read.
// The character is C++-escaped: \a, \b, \f, \n, \r, \t, \v, \', \", \\, \? are supported
// as well as octal (\OOO) and hex (\xHH) sequences.
int algo::UnescapeC(u32 str, int len, u8 &result) 
```

```c++
int algo::Replace(cstring &str, const strptr& from, const strptr& to, bool case_sensitive = true, bool preserve_case = true) 
```

```c++
// Scan S. For any character in S that occurs in string FROM, replace with corresponding
// character from string TO.
// Example:
// Translate("aabcd", "bd", "xy") -> "aaxcy"
void algo::Translate(strptr s, strptr from, strptr to) 
```

```c++
void algo_lib::ind_replvar_Cleanup(algo_lib::Replscope &replscope) 
```

```c++
// Set value of key KEY value VALUE
// KEY        string to replace
// VALUE      value to replace it with
// SUBST      If set, $-expand the VALUE parameter
void algo_lib::Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst = true) 
```

```c++
// Append TEXT to OUT, performing $-substitution using variables from SCOPE (must be non-NULL)
// if EOL is set, then new line is appended at the end.
void algo_lib::Ins(algo_lib::Replscope *scope, algo::cstring &out, strptr text, bool eol = true) 
```

```c++
// Enable comma-eating (default true)
void algo_lib::eatcomma_Set(algo_lib::Replscope &scope, bool enable) 
```

```c++
// Enable strict mode (default true -- any failed substitution kills process)
// If strict mode is off, failed substitution acts as if there was no substitution
void algo_lib::fatal_Set(algo_lib::Replscope &scope, bool enable) 
```

```c++
// Perform $-substitutions in TEXT and return new value.
tempstr algo_lib::Subst(algo_lib::Replscope &scope, strptr text) 
```

```c++
// read TEXT into a tuple.
// perform variable substitution on key-value pairs.
// write tuple back.
// return resulting string
tempstr algo_lib::Tuple_Subst(algo_lib::Replscope &R, strptr text) 
```

```c++
algo::tempstr::tempstr() 
```

```c++
algo::tempstr::~tempstr() 
```

```c++
// Return a run of characters up to next occurence of SEP (or to end of string)
// Skip any characters equal to SEP that follow the run
// sep="|", string = "a||b|"; return value -> "a", rest -> "b|";
// sep="|", string = "a";     return value -> "a", rest -> "";
// sep="|", string = "|x";    return value -> "" , rest -> "x";
strptr algo::GetTokenChar(algo::StringIter &S, char sep) 
```

```c++
// Skip leading characters matching SEP
// Return run of characters up to next matching SEP, or EOF.
// Do not skip trailing separators.
strptr algo::GetWordCharf(algo::StringIter &iter, bool (*sep)(u32) = algo_lib::WhiteCharQ) 
```

```c++
// Skip any leading whitespace in STR.
// Read and return next word.
strptr algo::GetTypeTag(strptr str) 
```

```c++
bool algo::TryParseI32(algo::StringIter &iter, i32 &result) 
```

```c++
bool algo::TryParseI64(algo::StringIter &iter, i64 &result) 
```

```c++
bool algo::TryParseU32(algo::StringIter &iter, u32 &result) 
```

```c++
bool algo::TryParseU64(algo::StringIter &iter, u64 &result) 
```

```c++
bool algo::TryParseU128(algo::StringIter &iter, u128 &result) 
```

```c++
// Read a series of digits and return resulting number.
// Return success code
bool algo::TryParseDigits(algo::StringIter &S, double &result) 
```

```c++
// TODO: document these more carefully
// Read a series of digits N, returning N / pow(10, length(N))
// If successful, advance index. Otherwise, leave index where it was.
bool algo::TryParseFraction(algo::StringIter &S, double &result) 
```

```c++
bool algo::TryParseDouble(algo::StringIter &iter, double &result) 
```

```c++
// Search for the next occurence of SEP. If not found, assume it occurs at EOF
// Return any characters between current position and the occurence of SEP
// Skip SEP, if needed.
strptr algo::GetTokenStrptr(StringIter &iter, const strptr& separator) 
```

```c++
// if the next characters match STR, skip and return true
// optionally do case sensitive comparison
bool algo::SkipStrptr(StringIter &iter, strptr str, bool case_sens) 
```

```c++
// Case-sensitive version
bool algo::SkipStrptr(StringIter &iter, strptr str) 
```

```c++
i64 algo::ParseI64(StringIter &iter, i64 dflt) 
```

```c++
// Convert string to numeric type.
// If there is an error, or string is empty, return DFLT.
// If there is junk at the end of the string, silently ignore it.
// Use StringIter version to check for junk, or use -X version to throw exception.
u64 algo::ParseU64(StringIter &iter, u64 dflt) 
```

```c++
int algo::ParseI32(StringIter &iter, int dflt) 
```

```c++
u32 algo::ParseU32(StringIter &iter, u32 dflt) 
```

```c++
int algo::StringIter::GetDigit(int dflt) 
```

```c++
void algo_lib::RunCsvParse(algo_lib::CsvParse &csvparse) 
```

```c++
// Find first occurence of MATCH in S
// If found, return range corresponding to the match.
// If not found, return range (S.n_elems,S.n_elems) -- an empty range positioned at
// the end of S
algo::i32_Range algo::substr_FindFirst(const aryptr<char> &s, const aryptr<char> &match) 
```

```c++
// Same as above but search right-to-left.
// In case of failure, return range (0,0) -- empty range positioned at start of S.
algo::i32_Range algo::substr_FindLast(const aryptr<char> &s, const aryptr<char> &match) 
```

```c++
// Strip leading whitespace, return new strptr.
strptr algo::TrimmedLeft(strptr s) 
```

```c++
// Strip trailing whitespace, return new strptr.
strptr algo::TrimmedRight(strptr s) 
```

```c++
// TrimmedLeft + TrimmedRight
strptr algo::Trimmed(strptr s) 
```

```c++
// scan STR until character SEP is found.
// if SEP is not found, it is assumed to be beyond the right side of the string
// set LEFT to point to the characters to the left of found location.
// set STR to point to the characters to the right of found location.
// Example:
// str = "abc.def", sep='.'; After NextSep, str="def", left="abc"
// str = "abc-def", sep='.'; After NextSep, str="", left="abc-def"
void algo::NextSep(algo::strptr &str, char sep, algo::strptr &left) 
```

```c++
void algo::Word_curs_Reset(Word_curs &curs, strptr text) 
```

```c++
void algo::Word_curs_Next(Word_curs &curs) 
```

```c++
// Skip any leading whitespace.
// Read next word (GetWordCharf) and compare to typetag.
// Skip any subsequent whitespace as well.
// Return true if type typetag matches
// if typetag doesn't match, input string remains unchanged
// " a  b " -> typetag is "a", rest is "b "
// "a    b" -> typetag is "a", rest is "b"
bool algo::StripTypeTag(strptr &in_str, strptr typetag) 
```

```c++
// Limit length of string S ot at most LEN characters
// If S is trimmed, append "..." to the end
tempstr algo::LimitLengthEllipsis(strptr s, int len) 
```

```c++
algo::i32_Range algo::ch_FindFirst(const algo::strptr &s, char match) 
```

```c++
algo::i32_Range algo::ch_FindLast(const algo::strptr &s, char match) 
```

```c++
bool algo::strptr_ReadStrptrMaybe(strptr , strptr )
```

```c++
// Append / to string STR unless STR already ends in one.
// str << dirname << MaybeDirSep << filename.
// The separator is always /. To support windows-specific pathnames,
// use ToWindows path where appropriate.
void algo::MaybeDirSep(cstring &str) 
```

```c++
algo::Attr_curs::Attr_curs() 
```

```c++
void algo::Attr_curs_Reset(Attr_curs &curs, strptr line) 
```

```c++
void algo::Attr_curs_Next(Attr_curs &curs) 
```

```c++
// Compare two strings whose start address is 16-byte aligned
// Do not use this function --
bool algo::AlignedEqual(strptr a, strptr b) 
```

```c++
// insert TEXT into OUT, indenting as necessary;
// Initial indentation is INDENT, it's adjusted as necessary as { and } are found
// in the TEXT.
// Each indent is 4 spaces.
// Trailing //-sytle comments are stripped
// /* */-style comments are not supported
// Lines beginning with # (#ifdef, etc) are printed at column zero.
void algo::InsertIndent(algo::cstring &out, strptr text, int indent) 
```

```c++
// Convert unix path to windows path
// This replaces slashes with backslashes
tempstr algo::ToWindowsPath(strptr path) 
```

```c++
// compatibility
void algo::reset(algo::cstring &lhs) 
```

```c++
void algo::DayRange_curs_Reset(algo::DayRange_curs &curs, const algo::DayRange& range) 
```

```c++
void algo::DayRange_curs_Next(algo::DayRange_curs &curs) 
```

```c++
bool algo::DayRange_curs_ValidQ(algo::DayRange_curs &curs) 
```

```c++
algo::UnTime& algo::DayRange_curs_Access(algo::DayRange_curs &curs) 
```

```c++
algo::u64_Ranges_curs::u64_Ranges_curs() : cur(0), end(0), valid(false) 
```

```c++
void algo::u64_Ranges_curs_Reset(algo::u64_Ranges_curs &curs, strptr str) 
```

```c++
void algo::u64_Ranges_curs_Next(algo::u64_Ranges_curs &curs) 
```

```c++
bool algo::u64_Ranges_curs_ValidQ(algo::u64_Ranges_curs &curs) 
```

```c++
u64 &algo::u64_Ranges_curs_Access(algo::u64_Ranges_curs &curs) 
```

```c++
void algo::Sep_curs_Reset(algo::Sep_curs &curs, strptr line, char sep) 
```

```c++
void algo::Sep_curs_Next(algo::Sep_curs &curs) 
```

```c++
bool algo::TimeStruct_Read(TimeStruct &out, algo::StringIter &iter, const strptr& spec) 
```

```c++
TimeStruct algo::ToTimeStruct(UnDiff   U) 
```

```c++
TimeStruct algo::ToTimeStruct(UnixDiff U) 
```

```c++
algo::UnixDiff algo::ToUnixDiff(const TimeStruct &S) 
```

```c++
algo::UnDiff algo::ToUnDiff(const TimeStruct &S) 
```

```c++
const algo::UnixTime algo::LocalDate(UnixTime in) 
```

```c++
TimeStruct algo::GetLocalTimeStruct(UnixTime U) 
```

```c++
TimeStruct algo::GetLocalTimeStruct(UnTime U) 
```

```c++
const algo::UnTime algo::LocalDate(UnTime in) 
```

```c++
algo::UnDiff algo::StripDate(UnTime t) 
```

```c++
algo::TimeStruct algo::GetGMTimeStruct(algo::UnTime U) 
```

```c++
algo::UnixTime algo::ToUnixTime(const TimeStruct &S) 
```

```c++
algo::UnTime algo::ToUnTime(const TimeStruct &S) 
```

```c++
// empty string -> 0
// invalid weekday -> -1
// All other weeks days map to 0..6, numbers compatible with struct tm's tm_wday field
int algo::GetWeekday(strptr wday)
```

```c++
const strptr algo::GetWeekdayName(int index)
```

```c++
int algo::GetMonthZeroBased(strptr month) 
```

```c++
int algo::GetWholeMonthZeroBased(strptr s) 
```

```c++
// Inverse mapping to abbreviated month names (Jan, Feb, etc.)
// If short_name==false, returns full name (January, February, etc.)
const strptr algo::GetMonthNameZeroBased(int index) 
```

```c++
const strptr algo::GetMonthNameZeroBasedShort(int index) 
```

```c++
// DateCache -- Roughly 200x faster LocalDate
const algo::UnTime algo::DateCache_LocalDate(algo::DateCache &dc, UnTime in) 
```

```c++
algo::UnTime algo::ToUnTime(WTime s) 
```

```c++
algo::UnTime algo::ParseUnTime(StringIter &s, const algo::strptr spec) 
```

```c++
algo::UnTime algo::ParseUnTime(const algo::strptr& s, const algo::strptr spec) 
```

```c++
// Change TZ environment variable to specified value
// and notify C runtime lib of the change
void algo::SetTz(strptr zone) 
```

```c++
// Initialize time hook TH as non-recurrent, with delay DELAY.
// Usage:
// ThInit(th, SchedTime());     // schedule at current time
// hook_Set0(th, myfunction);   // set callback
// bh_timehook_Reheap(th);      // insert into timehook heap
// ... eventually algo_lib::Step() will call the hook
void algo_lib::ThInit(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW 
```

```c++
// Similar to the above, but recurrent.
void algo_lib::ThInitRecur(algo_lib::FTimehook& th, algo::SchedTime delay) NOTHROW 
```

```c++
// Allowing only for fixed length formats.
void algo::tstamp_cache_Init(TstampCache& cache, strptr spec, bool gmt)
```

```c++
void algo::tstamp_cache_Print(algo::TstampCache& cache, UnTime t, cstring& out)
```

```c++
// Add new cell to the given row.
algo_lib::FTxtcell &algo_lib::AddCell(algo_lib::FTxtrow &txtrow, algo::strptr title, algo_TextJustEnum justify, int span) 
```

```c++
algo_lib::FTxtcell &algo_lib::AddCell(algo_lib::FTxtrow &txtrow, algo::strptr title, algo_TextJustEnum justify) 
```

```c++
// Add new cell to the last row of of table and return references to its text
algo_lib::FTxtrow &algo_lib::AddRow(algo_lib::FTxttbl &txttbl) 
```

```c++
// Add column to the last row of table and return reference to it
algo_lib::FTxtcell &algo_lib::AddCellEx(algo_lib::FTxttbl &txttbl) 
```

```c++
// Text table.
// Text table is a table with rows; Each row has an array of cells.
// Each call has text, col span, text justification, and optional style (color).
// Add column to the last row of table.
void algo_lib::AddCol(algo_lib::FTxttbl &txttbl, algo::strptr col, algo_TextJustEnum justify) 
```

```c++
void algo_lib::AddCol(algo_lib::FTxttbl &txttbl, algo::strptr col) 
```

```c++
void algo_lib::AddCols(algo_lib::FTxttbl &txttbl, algo::strptr csv, algo_TextJustEnum justify) 
```

```c++
void algo_lib::AddCols(algo_lib::FTxttbl &txttbl, algo::strptr csv) 
```

```c++
algo::cstring &algo_lib::AddCell(algo_lib::FTxttbl &txttbl) 
```

```c++
// Use prlog(txttbl) to print.
void algo_lib::TxttblCsv(algo_lib::FTxttbl &tbl) 
```

```c++
bool algo::SaneTerminalQ() 
```

```c++
U128 U128::operator >>(u64 count) const 
```

```c++
U128 U128::operator <<(u64 count) const 
```

```c++
U128 U128::operator &(const U128 &rhs) const 
```

```c++
U128 U128::operator |(const U128 &rhs) const 
```

```c++
U128 U128::operator -(const U128 &val) const 
```

```c++
U128 U128::operator +(const U128 &val) const 
```

```c++
U128 U128::operator %(const U128 &val) const 
```

```c++
U128 U128::operator /(const U128 &val) const 
```

```c++
U128 U128::operator *(const U128 &rhs) const 
```

```c++
int fork() 
```

```c++
int alarm(int sec) 
```

```c++
// redirect to MS implementation
int pipe(int fd[2]) 
```

```c++
int kill(pid_t pid, int signal) 
```

```c++
// not too important
int fchmod(int fd, int mode) 
```

```c++
pid_t waitpid(pid_t pid, int *status, int options) 
```

```c++
void srandom(int val) 
```

```c++
int random() 
```

```c++
int mkdir(const char *path, int mode) 
```

```c++
int mkstemp(char *tmpl) 
```

```c++
int isatty(int fd) 
```

```c++
int flock(int fd, int flags) 
```

```c++
void usleep(int microsec) 
```

```c++
void sleep(int sec) 
```

```c++
int setenv(const char *name, const char *value, int overwrite) 
```

```c++
const char *strsignal(int sig) 
```

```c++
int WIFSIGNALED(int status) 
```

```c++
int WCOREDUMP(int status) 
```

```c++
int WIFEXITED(int status) 
```

```c++
int WEXITSTATUS(int status) 
```

```c++
int WTERMSIG(int status) 
```

```c++
int WIFSTOPPED(int status) 
```

```c++
int WSTOPSIG(int status) 
```

```c++
int getrlimit(RlimitEnum what, rlimit *r) 
```

```c++
int setrlimit(RlimitEnum what, const rlimit *r) 
```

```c++
// Create linux-type soft link
// Return 0 if success
int symlink(const char *target, const char *path)
```

```c++
int link(const char *target, const char *path) 
```

```c++
void sigemptyset(u32 *mask) 
```

```c++
// no sigaciton on windows
int sigaction(int signal, struct sigaction *sa, void*) 
```

```c++
char *strerror_r(int code, char *buf, int bufsize) 
```

```c++
int getdomainname(char *buf, int bufsize) 
```

```c++
DIR *opendir(const char *path) 
```

```c++
void closedir(DIR *dir) 
```

```c++
int fnmatch(const char *pattern, const char *filename, int flags) 
```

```c++
// Windows doesn't support soft links.
// So implement lstat via stat
int lstat(const char *path, StatStruct *buf) 
```

```c++
int stat(const char *path, StatStruct *s) 
```

```c++
int fstat(int fd, StatStruct *s) 
```

```c++
void gettimeofday(struct timeval *tv, struct timezone *) 
```

```c++
int ftruncate(int fd, u64 size) 
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
// Close specified handle if it open
// Set value to NULL
void CloseHandleSafe(HANDLE &handle) 
```

```c++
int dospawn(char *path, char *cmdline, int timeout, strptr fstdin, strptr fstdout, strptr fstderr) 
```

```c++
int readlink(const char *path, char *out, int bufsize) 
```

```c++
int sysconf(int selector) 
```

```c++
void *mmap(void *base_addr, i64 size, MmapProt prot, MmapFlags mapflags, int fd, int flags2) 
```

```c++
int munmap(void *base_addr, i64 size) 
```

```c++
int pthread_create(pthread_t *thread, pthread_attr_t *attr, ThreadFunc func, void *arg) 
```

```c++
pthread_t pthread_self() 
```

```c++
template<class T> inline tempstr &algo::operator <<(const tempstr &lhs, const T &rhs) 
```

```c++
// Refurbish: destroy and re-create object in place.
// This function is exception-safe: if ~T throws an exception, T::T still gets
// called.
// Implementation note: explicit calls to destructor are checked for NULL pointer
// by GCC, so the nonnull attribute is required.
template<class T> inline void algo::Refurbish(T &t) F_NONNULL 
```

```c++
template<class T> inline int algo::elems_N(const aryptr<T> &ary) 
```

```c++
template<class T> inline int algo::ch_N(const aryptr<T> &ary) 
```

```c++
template<class T> inline void algo::Fill(const aryptr<T> &lhs, const T &t) 
```

```c++
template<class T> inline algo::aryptr<T>::aryptr() 
```

```c++
template<class T> inline T &algo::aryptr<T>::operator [](u32 idx) const 
```

```c++
template<class T, class U> inline int algo::Find(const algo::aryptr<T> &lhs, const U&t) 
```

```c++
template<class T> inline algo::aryptr<T> algo::FirstN(const algo::aryptr<T> &lhs, u32 n) 
```

```c++
template<class T> inline algo::aryptr<T> algo::LastN(const algo::aryptr<T> &lhs, u32 n) 
```

```c++
template<class T> inline algo::aryptr<T> algo::RestFrom(const algo::aryptr<T> &lhs, u32 n) 
```

```c++
template<class T> inline algo::aryptr<T> algo::qGetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) 
```

```c++
template<class T> inline algo::aryptr<T> algo::GetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) 
```

```c++
template<class T> inline algo::aryptr<u8> algo::BytesOf(const T &t) 
```

```c++
template<class T> inline algo::aryptr<T>::aryptr(const T *e, i32 in_n) : elems(const_cast<T*>(e)) , n_elems(in_n) 
```

```c++
template<class T> inline algo::aryptr<T>::aryptr(const char *e) : elems(const_cast<T*>(e)) , n_elems(algo::ImpliedLength((T*)NULL,e)) 
```

```c++
template<class T> inline T &algo::qLast(const algo::aryptr<T> &ary) 
```

```c++
// Taylor series expansion. For x around 0.01, this produces 2 digits per
// iteration, so with N=4 will be reasonably fast.
inline double algo::ExpTaylor(double x, int n) 
```

```c++
inline u32 algo::ParseNum8(u64 x, u32 &ok) 
```

```c++
inline u32 algo::ParseNum4(u32 x, u32 &ok) 
```

```c++
inline u32 algo::aParseNum8(strptr s, u32 &ok) 
```

```c++
inline u32 algo::aParseNum4(strptr s, u32 &ok) 
```

```c++
inline int algo::ParseOct1(u32 c, u8 &result) 
```

```c++
// C           single hex character, upper or lowercase
// RETURN      value of hex character
// RESULT      value of hex digit
// RETURN      success code
inline int algo::ParseHex1(u32 c, u8 &result) 
```

```c++
inline void algo::PageBufInit(PageBuf &F, u64 n, u64 align) 
```

```c++
inline u32 u8_Hash (u32 prev, u8  val) 
```

```c++
inline u32 u16_Hash(u32 prev, u16 val) 
```

```c++
inline u32 u32_Hash(u32 prev, u32 val) 
```

```c++
inline u32 u64_Hash(u32 prev, u64 val) 
```

```c++
inline u32 i8_Hash (u32 prev, i8  val) 
```

```c++
inline u32 i16_Hash(u32 prev, i16 val) 
```

```c++
inline u32 i32_Hash(u32 prev, i32 val) 
```

```c++
inline u32 i64_Hash(u32 prev, i64 val) 
```

```c++
inline u32 bool_Hash(u32 prev, bool  val) 
```

```c++
inline u32 char_Hash(u32 prev, char  val) 
```

```c++
inline u32 float_Hash(u32 prev, float val) 
```

```c++
inline u32 double_Hash(u32 prev, double  val) 
```

```c++
inline u32 u128_Hash(u32 prev, u128 t) 
```

```c++
inline u32 algo::strptr_Hash(u32 prev, algo::strptr val) 
```

```c++
inline u32 algo::cstring_Hash(u32 prev, const algo::strptr &val) 
```

```c++
inline u32 algo::cstring_Hash(u32 prev, algo::cstring &val) 
```

```c++
inline u32 algo::cstring_Hash(u32 prev, const algo::cstring &val) 
```

```c++
inline i32 u8_Cmp (u8 a, u8 b) 
```

```c++
inline i32 u16_Cmp(u16 a, u16 b) 
```

```c++
inline i32 u32_Cmp(u32 a, u32 b) 
```

```c++
inline i32 u64_Cmp(u64 a, u64 b) 
```

```c++
inline i32 i8_Cmp (i8 a, i8 b) 
```

```c++
inline i32 i16_Cmp(i16 a, i16 b) 
```

```c++
inline i32 i32_Cmp(i32 a, i32 b) 
```

```c++
inline i32 i64_Cmp(i64 a, i64 b) 
```

```c++
inline i32 bool_Cmp(bool a, bool b) 
```

```c++
inline i32 char_Cmp(char a, char b) 
```

```c++
inline i32 float_Cmp(float a, float b) 
```

```c++
inline i32 double_Cmp(double a, double b) 
```

```c++
inline i32 u128_Cmp(u128 a, u128 b) 
```

```c++
inline bool u8_Lt (u8 a, u8 b) 
```

```c++
inline bool u16_Lt(u16 a, u16 b) 
```

```c++
inline bool u32_Lt(u32 a, u32 b) 
```

```c++
inline bool u64_Lt(u64 a, u64 b) 
```

```c++
inline bool i8_Lt (i8 a, i8 b) 
```

```c++
inline bool i16_Lt(i16 a, i16 b) 
```

```c++
inline bool i32_Lt(i32 a, i32 b) 
```

```c++
inline bool i64_Lt(i64 a, i64 b) 
```

```c++
inline bool bool_Lt(bool a, bool b) 
```

```c++
inline bool char_Lt(char a, char b) 
```

```c++
inline bool float_Lt(float a, float b) 
```

```c++
inline bool double_Lt(double a, double b) 
```

```c++
inline bool u128_Lt(u128 a, u128 b) 
```

```c++
inline bool u8_Eq (u8 a, u8 b) 
```

```c++
inline bool u16_Eq(u16 a, u16 b) 
```

```c++
inline bool u32_Eq(u32 a, u32 b) 
```

```c++
inline bool u64_Eq(u64 a, u64 b) 
```

```c++
inline bool i8_Eq (i8 a, i8 b) 
```

```c++
inline bool i16_Eq(i16 a, i16 b) 
```

```c++
inline bool i32_Eq(i32 a, i32 b) 
```

```c++
inline bool i64_Eq(i64 a, i64 b) 
```

```c++
inline bool bool_Eq(bool a, bool b) 
```

```c++
inline bool char_Eq(char a, char b) 
```

```c++
inline bool float_Eq(float a, float b) 
```

```c++
inline bool double_Eq(double a, double b) 
```

```c++
inline bool u128_Eq(u128 a, u128 b) 
```

```c++
// this is inefficient because it would be nice to be able to overrun strings.
// however, we don't check byte alignment at the beginning,
// so a 2-byte string that lies 4 bytes before the end of a page could fault.
inline u32 algo::CRC32Step(u32 old, const u8 *x, size_t len) 
```

```c++
inline u32 _mm_crc32_u64(u32 prev, u64 val) 
```

```c++
inline u32 _mm_crc32_u32(u32 prev, u32 val) 
```

```c++
inline u32 _mm_crc32_u16(u32 prev, u16 val) 
```

```c++
inline u32 _mm_crc32_u8 (u32 prev, u8  val) 
```

```c++
inline int algo::P1Mod(int a, int b)  
```

```c++
inline bool operator ==(const algo::strptr &a, const algo::strptr &b) 
```

```c++
inline bool operator !=(const algo::strptr &a, const algo::strptr &b) 
```

```c++
inline bool operator <(const algo::strptr &a, const algo::strptr &b) 
```

```c++
inline algo::UnixTime operator +  (const algo::UnixTime &a, algo::UnixDiff d) 
```

```c++
inline algo::UnixTime operator -  (const algo::UnixTime &a, algo::UnixDiff d) 
```

```c++
inline algo::UnixDiff operator - (const algo::UnixTime &a, algo::UnixTime b) 
```

```c++
inline algo::UnTime operator + (algo::UnTime a, algo::UnDiff d) 
```

```c++
inline algo::UnTime operator += (algo::UnTime &a, const algo::UnDiff d) 
```

```c++
inline algo::UnDiff operator += (algo::UnDiff &a, const algo::UnDiff d) 
```

```c++
inline algo::UnTime operator -    (algo::UnTime a, algo::UnDiff d) 
```

```c++
inline algo::UnDiff operator -    (algo::UnTime a, algo::UnTime b) 
```

```c++
inline algo::UnDiff operator -    (const algo::UnDiff a) 
```

```c++
inline algo::UnixDiff operator *  (const algo::UnixDiff &a, i64 d) 
```

```c++
inline algo::UnixDiff operator /  (const algo::UnixDiff &a, i64 d) 
```

```c++
inline algo::UnixDiff operator +  (const algo::UnixDiff &a, algo::UnixDiff d) 
```

```c++
inline algo::UnixDiff operator -  (const algo::UnixDiff &a, algo::UnixDiff d) 
```

```c++
inline algo::UnDiff operator +    (const algo::UnDiff &a, algo::UnDiff d) 
```

```c++
inline algo::UnDiff operator -    (const algo::UnDiff &a, algo::UnDiff d) 
```

```c++
inline algo::UnDiff operator *  (const algo::UnDiff &a, i64 d) 
```

```c++
inline algo::UnDiff operator /  (const algo::UnDiff &a, i64 d) 
```

```c++
inline algo::WTime operator+(const algo::WTime &t, const algo::WDiff &t2) 
```

```c++
inline algo::WDiff operator+(const algo::WDiff &t, const algo::WDiff &t2) 
```

```c++
inline algo::WTime operator-(const algo::WTime &t, const algo::WDiff &t2) 
```

```c++
inline algo::WDiff operator-(const algo::WTime &t, const algo::WTime &t2) 
```

```c++
inline algo::WDiff operator-(const algo::WDiff &t, const algo::WDiff &t2) 
```

```c++
inline algo::SchedTime operator +  (const algo::SchedTime &a, algo::SchedTime d) 
```

```c++
inline algo::SchedTime operator -  (const algo::SchedTime &a, algo::SchedTime d) 
```

```c++
// move ctor
inline algo::tempstr::tempstr(const tempstr &rhs) : algo::cstring(rhs) 
```

```c++
inline u32 algo::u32_Count1s(u32 x) 
```

```c++
inline u32 algo::u8_Count1s(u8 x) 
```

```c++
inline u32 algo::u16_Count1s(u16 x) 
```

```c++
inline u32 algo::u64_Count1s(u64 x) 
```

```c++
inline u32 algo::u128_Count1s(u128 x) 
```

```c++
template<class T> inline void algo::ZeroBytes(T &t) 
```

```c++
template<class T, class U> inline T algo::PtrAdd(U *ptr, int_ptr offset) 
```

```c++
template<class T> inline void algo::TSwap(T &a, T &b)  
```

```c++
// Default, and invalid, value for Fildes is -1. 0 is a valid value (stdin)!
inline bool algo::ValidQ(Fildes fd) 
```

```c++
inline int algo::i32_NegateIf(int x, bool i) 
```

```c++
inline i64 algo::i64_NegateIf(i64 x, bool i) 
```

```c++
inline float algo::float_NegateIf(float x, bool i) 
```

```c++
inline double algo::double_NegateIf(double x, bool i) 
```

```c++
inline double algo::DRound(double a) 
```

```c++
inline double algo::DCeiling(double a) 
```

```c++
inline double algo::DFloor(double a) 
```

```c++
inline double algo::DTrunc(double a) 
```

```c++
inline algo_FileFlagsEnum operator |(algo_FileFlagsEnum lhs, algo_FileFlagsEnum rhs) 
```

```c++
// Clipping to an open interval (exclisive upper limit) [0..hi-1] or [lo..hi-1]
inline int algo::Clipped(int a, int hi) 
```

```c++
inline int algo::Clipped(int a, int lo, int hi) 
```

```c++
// Clipping to a closed interval lo..hi
inline double algo::Clippedf(double a, double lo, double hi) 
```

```c++
// unsigned subtract with clipping
inline u64 algo::u64_SubClip( u64 a, u64 b) 
```

```c++
inline u32 algo::u32_SubClip( u32 a, u32 b) 
```

```c++
inline u16 algo::u16_SubClip( u16 a, u16 b) 
```

```c++
inline int algo::Abs(i8  a) 
```

```c++
inline int algo::Abs(i16 a) 
```

```c++
inline i32 algo::Abs(i32 a) 
```

```c++
inline i64 algo::Abs(i64 a) 
```

```c++
inline f32 algo::Abs(f32 a) 
```

```c++
inline f64 algo::Abs(f64 a) 
```

```c++
inline f80 algo::Abs(f80 a) 
```

```c++
// Read big-endian word
inline u16 algo::ReadBE16(const void *val) 
```

```c++
inline u32 algo::ReadBE32(const void *val) 
```

```c++
inline u64 algo::ReadBE64(const void *val) 
```

```c++
// Read little-endian word
inline u16 algo::ReadLE16(const void *val) 
```

```c++
inline u32 algo::ReadLE32(const void *val) 
```

```c++
inline u64 algo::ReadLE64(const void *val) 
```

```c++
// Forward:  returns 0-based index of least significant bit that is set
// Reverse:  returns 0-based index of most  significant bit that is set.
// input argument must not be zero.
// input result in 0 is undefined (see Intel manual)
// http://www.intel.com/content/dam/doc/manual/64-ia-32-architectures-software-developer-vol-2a-2b-instruction-set-a-z-manual.pdf
inline u32 algo::u32_BitScanForward(u32 v) 
```

```c++
inline u64 algo::u64_BitScanForward(u64 v) 
```

```c++
inline u32 algo::u32_BitScanReverse(u32 v) 
```

```c++
inline u64 algo::u64_BitScanReverse(u64 v) 
```

```c++
inline u32 algo::u16_BitScanForward(u16 v) 
```

```c++
inline u32 algo::u16_BitScanReverse(u16 v) 
```

```c++
inline u32 algo::u8_BitScanForward(u8 v) 
```

```c++
inline u32 algo::u8_BitScanReverse(u8 v) 
```

```c++
inline u32  algo::CeilingLog2(u32 orig) 
```

```c++
inline u64  algo::CeilingLog2(u64 orig) 
```

```c++
inline u32  algo::FloorLog2(u32 i) 
```

```c++
inline u64  algo::FloorLog2(u64 i) 
```

```c++
inline u32 algo::BumpToPow2(u32 i) 
```

```c++
inline u64 algo::BumpToPow2(u64 i) 
```

```c++
inline u64   algo::u64_CeilPow2(u64 a, u64 b) 
```

```c++
inline u32   algo::u32_CeilPow2(u32 a, u32 b) 
```

```c++
inline u64    algo::u64_ModPow2(u64 a, u64 b) 
```

```c++
inline u32    algo::u32_ModPow2(u32 a, u32 b) 
```

```c++
inline bool     algo::u64_Pow2Q(u64 i) 
```

```c++
inline bool     algo::u32_Pow2Q(u32 i) 
```

```c++
inline algo::PageBuf::~PageBuf() 
```

```c++
// get CPU HZ value as u64
inline u64 algo::get_cpu_hz_int() 
```

```c++
// get CPU HZ value as double
inline double algo::get_cpu_hz() 
```

```c++
// cpu_hz (untyped SchedTime)
// use this for timestamps.
// these calls may be pipelined and reordered, so measuring instruction
// latency with these is not possible. for that, use rdtscp
inline u64 algo::get_cycles() 
```

```c++
// Convert scheduler time units to seconds.
inline double algo::ToSecs(SchedTime ticks) 
```

```c++
inline algo::UnDiff algo::ToUnDiff(SchedTime ticks) 
```

```c++
inline algo::SchedTime algo::ToSchedTime(double secs) 
```

```c++
inline algo::UnTime algo::ToUnTime(UnixTime t) 
```

```c++
inline algo::UnixTime algo::ToUnixTime(UnTime t) 
```

```c++
inline double algo::ToSecs(UnDiff t) 
```

```c++
inline double algo::ToSecs(UnTime t) 
```

```c++
inline algo::UnixTime algo::CurrUnixTime()
```

```c++
// use this for performance measurements.
// according to Intel software manual, lfence followed by rdtsc
// is the beez knees.
inline u64 algo::rdtscp() 
```

```c++
inline algo::UnixDiff algo::UnixDiffHMS(i64 h, int m, int s) 
```

```c++
inline algo::UnDiff algo::UnDiffSecs(double d) 
```

```c++
inline algo::UnDiff algo::UnDiffMsecs(const i64 i) 
```

```c++
inline algo::UnDiff algo::UnDiffUsecs(const double d) 
```

```c++
inline algo::UnTime algo::UnTimeSecs(double d) 
```

```c++
inline algo::UnDiff algo::UnDiffSecs(i64 i) 
```

```c++
inline algo::UnDiff algo::UnDiffSecs(i32 i) 
```

```c++
inline algo::UnDiff algo::UnDiffHMS(int h, int m, int s) 
```

```c++
// Current value of get_cycles();
inline algo::SchedTime algo::CurrSchedTime() 
```

```c++
// Elapsed time in seconds between two SchedTimes.
inline double algo::ElapsedSecs(algo::SchedTime start, algo::SchedTime end) 
```

```c++
inline algo::TimeStruct::TimeStruct() 
```

```c++
inline algo::TimeStruct::TimeStruct(const struct tm &t) : tm(t) 
```

```c++
inline algo::WDiff algo::ToWDiff(algo::UnixDiff d) 
```

```c++
inline algo::WDiff algo::ToWDiff(algo::UnDiff d) 
```

```c++
inline algo::UnixDiff algo::ToUnixDiff(algo::WDiff d) 
```

```c++
inline algo::WTime algo::ToWTime(algo::UnTime s) 
```

```c++
inline algo::UnDiff algo::ToUnDiff(algo::WDiff d) 
```

```c++
inline algo::UnixTime algo::ToUnixTime(algo::WTime nt) 
```

```c++
inline algo::WTime algo::ToWTime(algo::UnixTime t) 
```

```c++
inline double algo::ToSecs(algo::WDiff t) 
```

```c++
// Append and end-of-line sequence to string S.
inline void algo::eol(cstring &s) 
```

```c++
inline algo::strptr algo::ToStrPtr(memptr buf)  
```

```c++
// Note: cstring << algo::ToLower(int) will output an *integer*, not a character
inline int algo::ToLower(int i)  
```

```c++
// Note: cstring << algo::ToUpper(int) will output an *integer*, not a character
inline int algo::ToUpper(int i)  
```

```c++
inline char algo::ToLower(char i)  
```

```c++
inline char algo::ToUpper(char i)  
```

```c++
inline bool algo::StringIter::EofQ() const 
```

```c++
inline char algo::StringIter::Peek() 
```

```c++
inline char algo::StringIter::GetChar() 
```

```c++
inline algo::StringIter &algo::StringIter::Ws() 
```

```c++
inline algo::i32_Range algo::TFind(const strptr &s, char match) 
```

```c++
inline algo::i32_Range algo::TRevFind(const strptr &s, char match) 
```

```c++
inline algo::aryptr<char> algo::ch_FirstN(const strptr &lhs, u32 n) 
```

```c++
inline algo::aryptr<char> algo::ch_LastN(const strptr &lhs, u32 n) 
```

```c++
inline algo::aryptr<char> algo::ch_RestFrom(const strptr &lhs, u32 n) 
```

```c++
inline algo::aryptr<char> algo::ch_GetRegion(const strptr &lhs, u32 lo, u32 n) 
```

```c++
inline int algo::ImpliedLength(char *, const char *c) 
```

```c++
inline int algo::ImpliedLength(const char *, const char *c) 
```

```c++
inline int algo::ch_N(const strptr &s) 
```

```c++
inline int algo::ch_First(const strptr &s, int dflt = 0) 
```

```c++
inline int algo::ch_Last(const strptr &s, int dflt = 0) 
```

```c++
inline int algo::ch_N(const tempstr &str) 
```

```c++
inline int algo::range_N(const i32_Range &rhs) 
```

```c++
inline algo::aryptr<u8> algo::strptr_ToMemptr(algo::aryptr<char> rhs) 
```

```c++
inline algo::aryptr<char> algo::memptr_ToStrptr(algo::aryptr<u8> rhs) 
```

```c++
// if next character matches WHAT, skip and return true
// otherwise return false
inline bool algo::SkipChar(StringIter &iter, char what) 
```

```c++
// Convert string to numeric type.
// If there is an error, or string is empty, return DFLT.
// If there is junk at the end of the string, silently ignore it.
// Use StringIter version to check for junk, or use -X version to throw exception.
inline int algo::ParseI32(strptr str, int dflt) 
```

```c++
inline u32 algo::ParseU32(strptr str, u32 dflt) 
```

```c++
inline i64 algo::ParseI64(strptr str, i64 dflt) 
```

```c++
inline u64 algo::ParseU64(strptr str, u64 dflt) 
```

```c++
inline strptr algo::StringIter::Rest() 
```

```c++
inline algo::Attr_curs &algo::Attr_curs_Access(Attr_curs &curs) 
```

```c++
inline bool algo::Attr_curs_ValidQ(Attr_curs &curs) 
```

```c++
inline bool algo::Line_curs_ValidQ(Line_curs &curs) 
```

```c++
inline strptr &algo::Line_curs_Access(Line_curs &curs) 
```

```c++
inline bool algo::Word_curs_ValidQ(Word_curs &curs) 
```

```c++
inline strptr &algo::Word_curs_Access(Word_curs &curs) 
```

```c++
template<class T> inline void algo::Init_Set(StringDesc &desc, void(*fcn)(T&)) 
```

```c++
template<class T> inline void algo::SetnumMaybe_Set(StringDesc &desc, bool(*fcn)(T&, i64 num)) 
```

```c++
template<class T> inline void algo::Geti64_Set(StringDesc &desc, i64(*fcn)(T&, bool &)) 
```

```c++
template<class T> inline void algo::Getary_Set(StringDesc &desc, algo::aryptr<char>(*fcn)(T&)) 
```

```c++
inline strptr &algo::FileLine_curs_Access(algo::FileLine_curs &curs) 
```

```c++
inline bool algo::FileLine_curs_ValidQ(algo::FileLine_curs &curs) 
```

```c++
inline algo::ListSep::ListSep(strptr sep_) : sep(sep_), iter(0) 
```

```c++
inline algo::Line_curs::Line_curs() : eof(true),i(-1)
```

```c++
inline algo::Word_curs::Word_curs() : index(0)
```

```c++
inline bool algo::Sep_curs_ValidQ(algo::Sep_curs &curs) 
```

```c++
inline strptr &algo::Sep_curs_Access(algo::Sep_curs &curs) 
```

```c++
inline algo::Sep_curs::Sep_curs() : sep('\0'), index(-1), state(algo::Sep_curs::State::invalid)
```

```c++
inline u32 algo::u16_FmtBuf(u32 value, u8 *buf) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, const char* t) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, const ListSep &t) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, const strptr &t) 
```

```c++
inline algo::cstring &algo::operator<<(algo::cstring &out, void (*t)(algo::cstring &)) 
```

```c++
// SameSign returns false if one of the arguments is zero.
template<typename T> inline bool algo::SameSignQ(T a, T b) 
```

```c++
// Compare two strings for equality, case-sensitively
inline bool algo::strptr_Eq(algo::strptr a, algo::strptr b) 
```

```c++
// Test if string A is lexicographically less than string B.
inline bool algo::strptr_Lt(algo::strptr a, algo::strptr b) 
```

```c++
// reverse bit order in a byte
inline u8 algo::u8_ReverseBits(u8 b) 
```

```c++
// Return unix-epoch time with nanosecond resolution
// On Linux, this function calls clock_gettime() which takes about
// 30ns and uses rdtsc() to increase underlying clock resolution
inline algo::UnTime algo::CurrUnTime() 
```

```c++
inline i32 algo::strptr_Cmp(algo::strptr a, algo::strptr b) 
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
|[cpp/lib/algo/string.cpp](/cpp/lib/algo/string.cpp)|cstring functions|
|[cpp/lib/algo/time.cpp](/cpp/lib/algo/time.cpp)|UnTime / UnDiff functions|
|[cpp/lib/algo/timehook.cpp](/cpp/lib/algo/timehook.cpp)||
|[cpp/lib/algo/tstamp.cpp](/cpp/lib/algo/tstamp.cpp)|timestamp cache|
|[cpp/lib/algo/txttbl.cpp](/cpp/lib/algo/txttbl.cpp)|Ascii table|
|[cpp/lib/algo/u128.cpp](/cpp/lib/algo/u128.cpp)||
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
|[algo_lib.FFildes](#algo_lib-ffildes)||FDb.fildes (Cppstack)|
|[algo_lib.FLockfile](#algo_lib-flockfile)||
|[algo_lib.FTimehook](#algo_lib-ftimehook)||FDb.timehook (Cppstack)|bh_timehook (Bheap, sort field time)|c_timehook (Ptr)|
||||FTimehook.hook (Hook)|
|[algo_lib.FImdb](#algo_lib-fimdb)||FDb.imdb (Inlary)|ind_imdb (Thash, hash field imdb)|
|[algo_lib.FLogcat](#algo_lib-flogcat)|[dmmeta.logcat](/txt/ssimdb/dmmeta/logcat.md)|FDb.logcat (Inlary)|**static**|ind_logcat (Thash, hash field logcat)|
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
|[algo_lib.Regx](#algo_lib-regx)||FDb.regx (Cppstack)|
||||RegxParse.p_regx (Upptr)|
|[algo_lib.RegxToken](#algo_lib-regxtoken)||
|[algo_lib.RegxExpr](#algo_lib-regxexpr)||RegxParse.ary_expr (Tary)|
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

#### algo_lib.FLogcat - Log category
<a href="#algo_lib-flogcat"></a>

#### algo_lib.FLogcat Fields
<a href="#algo_lib-flogcat-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.FLogcat.base|[dmmeta.Logcat](/txt/ssimdb/dmmeta/logcat.md)|[Base](/txt/ssimdb/dmmeta/logcat.md)|||

#### Struct FLogcat
<a href="#struct-flogcat"></a>
*Note:* field ``algo_lib.FLogcat.base`` has reftype ``base`` so the fields of [dmmeta.Logcat](/txt/ssimdb/dmmeta/logcat.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FLogcat { // algo_lib.FLogcat
    algo::Smallstr50     logcat;            //
    bool                 enabled;           //   false
    bool                 builtin;           //   false
    algo::Comment        comment;           //
    algo_lib::FLogcat*   ind_logcat_next;   // hash next
    // func:algo_lib.FLogcat..AssignOp
    inline algo_lib::FLogcat& operator =(const algo_lib::FLogcat &rhs) = delete;
    // func:algo_lib.FLogcat..Ctor
    inline               FLogcat() __attribute__((nothrow));
    // func:algo_lib.FLogcat..Dtor
    inline               ~FLogcat() __attribute__((nothrow));
    // func:algo_lib.FLogcat..CopyCtor
    inline               FLogcat(const algo_lib::FLogcat &rhs) = delete;
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
|algo_lib.FDb.limit|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|0x7fffffffffffffff||
|algo_lib.FDb.clocks_to_ms|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.n_iohook|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of iohooks in epoll|
|algo_lib.FDb.clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Current cpu clock value|
|algo_lib.FDb.mainloop_clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Mainloop cpu clock value|
|algo_lib.FDb.start_clock|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||cpu clock value at startup|
|algo_lib.FDb.hz|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.t_last_signal|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time last async signal was processed|
|algo_lib.FDb.exit_code|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Exit code from main program. 0 = success (this is the unix exit code!)|
|algo_lib.FDb.clocks_to_ns|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.n_temp|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|* initialization order is important *|
|algo_lib.FDb.last_signal|u32|[Val](/txt/exe/amc/reftypes.md#val)||Value of last signal (used by SetupExitSignals)|
|algo_lib.FDb.eol|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|algo_lib.FDb.cpu_hz|u64|[Val](/txt/exe/amc/reftypes.md#val)||Cpu HZ, determined at startup|
|algo_lib.FDb.fildes|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|algo_lib.FDb.temp_strings|[algo.cstring](/txt/protocol/algo/cstring.md)|[Inlary](/txt/exe/amc/reftypes.md#inlary)||* initialization order is important *|
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
|algo_lib.FDb.giveup_time|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Trigger for giveup_time loop|
|algo_lib.FDb.sleep_roundup|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.last_sleep_clocks|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.msgtemp|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.show_insert_err_lim|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
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
|algo_lib.FDb.show_tstamp|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.tstamp_fmt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"%Y/%m/%dT%H:%M:%S.%.6X "||
|algo_lib.FDb.fildes_stdout|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftypes.md#val)|1||
|algo_lib.FDb.fildes_stderr|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftypes.md#val)|2||
|algo_lib.FDb.pending_eol|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.FDb.exec_args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|algo_lib.FDb.dirstack|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Directory stack for PushDir/PopDir|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FDb { // algo_lib.FDb: In-memory database for algo_lib
    u64                               sbrk_huge_limit;                              // Huge page limit in bytes -- set to 0 with fork!
    u64                               sbrk_huge_alloc;                              // Huge page bytes allocated
    bool                              sbrk_zeromem;                                 // Zero allocated memory
    lpool_Lpblock*                    lpool_free[36];                               // Lpool levels
    algo::SchedTime                   next_loop;                                    //
    algo::SchedTime                   limit;                                        //   0x7fffffffffffffff
    double                            clocks_to_ms;                                 //   0.0
    u32                               n_iohook;                                     //   0  Number of iohooks in epoll
    algo::SchedTime                   clock;                                        // Current cpu clock value
    algo::SchedTime                   mainloop_clock;                               // Mainloop cpu clock value
    algo::SchedTime                   start_clock;                                  // cpu clock value at startup
    double                            hz;                                           //   0.0
    algo::SchedTime                   t_last_signal;                                // Time last async signal was processed
    i32                               exit_code;                                    //   0  Exit code from main program. 0 = success (this is the unix exit code!)
    double                            clocks_to_ns;                                 //   0.0
    u32                               n_temp;                                       //   0  * initialization order is important *
    u32                               last_signal;                                  //   0  Value of last signal (used by SetupExitSignals)
    bool                              eol;                                          //   false
    u64                               cpu_hz;                                       //   0  Cpu HZ, determined at startup
    algo::cstring                     temp_strings_elems[8];                        // fixed array
    algo::Charset                     ArgvIdent;                                    //
    algo::Charset                     BashQuotesafe;                                //
    algo::Charset                     RegxSqlSpecial;                               //
    algo::Charset                     SsimBreakName;                                //
    algo::Charset                     SsimBreakValue;                               //
    algo::Charset                     SsimQuotesafe;                                //
    algo_lib::FImtable*               imtable_lary[32];                             // level array
    i32                               imtable_n;                                    // number of elements in array
    algo_lib::FImtable**              ind_imtable_buckets_elems;                    // pointer to bucket array
    i32                               ind_imtable_buckets_n;                        // number of elements in bucket array
    i32                               ind_imtable_n;                                // number of elements in the hash table
    algo::cstring                     log_str;                                      //
    algo_lib::FTimehook**             bh_timehook_elems;                            // binary heap by time
    i32                               bh_timehook_n;                                // number of elements in the heap
    i32                               bh_timehook_max;                              // max elements in bh_timehook_elems
    i32                               epoll_fd;                                     //   -1
    algo_lib::FLockfile               lock_core;                                    //
    algo_lib::FTimehook*              c_timehook;                                   // TEMP: here only for dependency reasons. optional pointer
    algo_lib::FTimehook               _timehook;                                    // Keep me here i'm special
    algo_lib::FDispsigcheck*          dispsigcheck_lary[32];                        // level array
    i32                               dispsigcheck_n;                               // number of elements in array
    algo_lib::FDispsigcheck**         ind_dispsigcheck_buckets_elems;               // pointer to bucket array
    i32                               ind_dispsigcheck_buckets_n;                   // number of elements in bucket array
    i32                               ind_dispsigcheck_n;                           // number of elements in the hash table
    u128                              imdb_data[sizeu128(algo_lib::FImdb,32)];      // place for data
    i32                               imdb_n;                                       // number of elems current in existence
    enum { imdb_max = 32 };
    algo_lib::FImdb**                 ind_imdb_buckets_elems;                       // pointer to bucket array
    i32                               ind_imdb_buckets_n;                           // number of elements in bucket array
    i32                               ind_imdb_n;                                   // number of elements in the hash table
    u64                               txtcell_blocksize;                            // # bytes per block
    algo_lib::FTxtcell*               txtcell_free;                                 //
    u64                               txtrow_blocksize;                             // # bytes per block
    algo_lib::FTxtrow*                txtrow_free;                                  //
    i32                               argc;                                         //   0  Argc from main
    char**                            argv;                                         // Argv from main. optional pointer
    algo::cstring                     xref_error;                                   //
    algo::cstring                     errtext;                                      //
    algo::ByteAry*                    varlenbuf;                                    // optional pointer
    u64                               replvar_blocksize;                            // # bytes per block
    algo_lib::FReplvar*               replvar_free;                                 //
    algo_lib::Cmdline                 cmdline;                                      //
    algo_lib::_db_h_fatalerror_hook   h_fatalerror;                                 //   NULL  Pointer to a function
    u64                               h_fatalerror_ctx;                             //   0  Callback context
    u64                               giveup_count;                                 //   0
    algo::cstring                     fatalerr;                                     //
    u32                               stringtofile_nwrite;                          //   0  Global counter of # of files written
    bool                              giveup_time;                                  //   true  Trigger for giveup_time loop
    bool                              sleep_roundup;                                //   false
    u64                               last_sleep_clocks;                            //   0
    algo::ByteAry                     msgtemp;                                      //
    u32                               show_insert_err_lim;                          //   0
    algo::Charset                     Urlsafe;                                      //
    u64                               winjob;                                       //   0
    algo::PrlogFcn                    Prlog;                                        //   algo::Prlog
    u128                              logcat_data[sizeu128(algo_lib::FLogcat,3)];   // place for data
    i32                               logcat_n;                                     // number of elems current in existence
    enum { logcat_max = 3 };
    algo_lib::FLogcat**               ind_logcat_buckets_elems;                     // pointer to bucket array
    i32                               ind_logcat_buckets_n;                         // number of elements in bucket array
    i32                               ind_logcat_n;                                 // number of elements in the hash table
    bool                              show_tstamp;                                  //   false
    algo::cstring                     tstamp_fmt;                                   //   "%Y/%m/%dT%H:%M:%S.%.6X "
    algo::Fildes                      fildes_stdout;                                //   1
    algo::Fildes                      fildes_stderr;                                //   2
    bool                              pending_eol;                                  //   false
    algo::cstring*                    exec_args_elems;                              // pointer to elements
    u32                               exec_args_n;                                  // number of elements in array
    u32                               exec_args_max;                                // max. capacity of array before realloc
    algo::cstring*                    dirstack_elems;                               // pointer to elements
    u32                               dirstack_n;                                   // number of elements in array
    u32                               dirstack_max;                                 // max. capacity of array before realloc
    algo_lib::trace                   trace;                                        //
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
    algo_lib::FDispsigcheck*   ind_dispsigcheck_next;   // hash next
    algo::Smallstr50           dispsig;                 //
    algo::Sha1sig              signature;               //
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
    algo_lib::FImtable*       ind_imtable_next;   // hash next
    algo::Smallstr50          imtable;            // Table name
    algo::Smallstr100         elem_type;          // Element type name
    algo::ImrowRowidFindFcn   c_RowidFind;        //   0  Function to find element by rowid (may be NULL)
    algo::ImrowXrefXFcn       XrefX;              //   0  Function to x-reference an element (may be NULL)
    algo::ImrowNItemsFcn      NItems;             //   0  Return number of elements in the table
    algo::ImrowPrintFcn       Print;              //   0  Convert specified element to string (may be NULL)
    i32                       size;               //   0  Size of one element (for fixed-width elements only)
    algo::Smallstr50          ssimfile;           // Ssimfile name (if associated)
    algo::Comment             comment;            //
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

#### Struct FReplvar
<a href="#struct-freplvar"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FReplvar { // algo_lib.FReplvar
    algo_lib::FReplvar*    replvar_next;       // Pointer to next free element int tpool
    algo_lib::Replscope*   p_replscope;        // reference to parent row
    algo::cstring          key;                // Key
    algo::cstring          value;              // Value
    i32                    nsubst;             //   0  Number of times variable accessed
    algo_lib::FReplvar*    ind_replvar_next;   // hash next
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
|algo_lib.FTxtcell.span|i32|[Val](/txt/exe/amc/reftypes.md#val)|1|Span in columns|
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
    i32                   span;                      //   1  Span in columns
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
|algo_lib.FTxttbl.c_txtrow|[algo_lib.FTxtrow](/txt/lib/algo_lib/README.md#algo_lib-ftxtrow)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Array of cells|
|algo_lib.FTxttbl.col_space|i32|[Val](/txt/exe/amc/reftypes.md#val)|2|Extra space between columns|
|algo_lib.FTxttbl.hdr_row|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Index of header row (default -1)|
|algo_lib.FTxttbl.normalized|bool|[Val](/txt/exe/amc/reftypes.md#val)||Cell widths computed|

#### Struct FTxttbl
<a href="#struct-ftxttbl"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct FTxttbl { // algo_lib.FTxttbl: Table row. Todo: absolute index for cells?
    algo_lib::FTxtrow**   c_txtrow_elems;   // array of pointers
    u32                   c_txtrow_n;       // array of pointers
    u32                   c_txtrow_max;     // capacity of allocated array
    i32                   col_space;        //   2  Extra space between columns
    i32                   hdr_row;          //   0  Index of header row (default -1)
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

#### algo_lib.Regx - Parsed regular expression
<a href="#algo_lib-regx"></a>

#### algo_lib.Regx Fields
<a href="#algo_lib-regx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Regx.expr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Original string expression in some unspecified regx dialect|
|algo_lib.Regx.state|[algo_lib.RegxState](/txt/lib/algo_lib/README.md#algo_lib-regxstate)|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of states|
|algo_lib.Regx.front|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Temporary front (for matching)|
|algo_lib.Regx.next_front|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Next front (for matching)|
|algo_lib.Regx.start|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Set of starting states|
|algo_lib.Regx.accept|i32|[Val](/txt/exe/amc/reftypes.md#val)||Accept state|
|algo_lib.Regx.parseerror|bool|[Val](/txt/exe/amc/reftypes.md#val)||Non-fatal error while parsing|
|algo_lib.Regx.accepts_all|bool|[Val](/txt/exe/amc/reftypes.md#val)||True if this regx matches anything|
|algo_lib.Regx.literal|bool|[Val](/txt/exe/amc/reftypes.md#val)||True if expr may be matched literally (set during translation)|

#### Struct Regx
<a href="#struct-regx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Regx { // algo_lib.Regx: Parsed regular expression
    algo::cstring          expr;          // Original string expression in some unspecified regx dialect
    algo_lib::RegxState*   state_elems;   // pointer to elements
    u32                    state_n;       // number of elements in array
    u32                    state_max;     // max. capacity of array before realloc
    algo_lib::Bitset       front;         // Temporary front (for matching)
    algo_lib::Bitset       next_front;    // Next front (for matching)
    algo_lib::Bitset       start;         // Set of starting states
    i32                    accept;        //   0  Accept state
    bool                   parseerror;    //   false  Non-fatal error while parsing
    bool                   accepts_all;   //   false  True if this regx matches anything
    bool                   literal;       //   false  True if expr may be matched literally (set during translation)
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

#### algo_lib.RegxToken - 
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
struct RegxToken { // algo_lib.RegxToken
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

#### algo_lib.RegxExpr - 
<a href="#algo_lib-regxexpr"></a>

#### algo_lib.RegxExpr Fields
<a href="#algo_lib-regxexpr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxExpr.type|[algo_lib.RegxToken](/txt/lib/algo_lib/README.md#algo_lib-regxtoken)|[Val](/txt/exe/amc/reftypes.md#val)|||
|algo_lib.RegxExpr.in|i32|[Val](/txt/exe/amc/reftypes.md#val)||Input state|
|algo_lib.RegxExpr.out|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Output states|

#### Struct RegxExpr
<a href="#struct-regxexpr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxExpr { // algo_lib.RegxExpr
    algo_lib::RegxToken   type;   //
    i32                   in;     //   0  Input state
    algo_lib::Bitset      out;    // Output states
    // func:algo_lib.RegxExpr..Ctor
    inline               RegxExpr() __attribute__((nothrow));
};
```

#### algo_lib.RegxParse - Function to parse regx
<a href="#algo_lib-regxparse"></a>

#### algo_lib.RegxParse Fields
<a href="#algo_lib-regxparse-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxParse.input|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Input string|
|algo_lib.RegxParse.p_regx|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Output regx -- by reference|
|algo_lib.RegxParse.ary_expr|[algo_lib.RegxExpr](/txt/lib/algo_lib/README.md#algo_lib-regxexpr)|[Tary](/txt/exe/amc/reftypes.md#tary)||Output expression array|

#### Struct RegxParse
<a href="#struct-regxparse"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxParse { // algo_lib.RegxParse: Function to parse regx
    algo::strptr          input;            // Input string
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

#### algo_lib.RegxState - 
<a href="#algo_lib-regxstate"></a>

#### algo_lib.RegxState Fields
<a href="#algo_lib-regxstate-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.RegxState.ch_class|[algo.i32_Range](/txt/protocol/algo/README.md#algo-i32_range)|[Tary](/txt/exe/amc/reftypes.md#tary)||What to match|
|algo_lib.RegxState.out|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Where to go on a match|
|algo_lib.RegxState.accept_all|bool|[Val](/txt/exe/amc/reftypes.md#val)||Regx always succeeds from here|

#### Struct RegxState
<a href="#struct-regxstate"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct RegxState { // algo_lib.RegxState
    algo::i32_Range*   ch_class_elems;   // pointer to elements
    u32                ch_class_n;       // number of elements in array
    u32                ch_class_max;     // max. capacity of array before realloc
    algo_lib::Bitset   out;              // Where to go on a match
    bool               accept_all;       //   false  Regx always succeeds from here
    // func:algo_lib.RegxState..AssignOp
    algo_lib::RegxState& operator =(const algo_lib::RegxState &rhs) __attribute__((nothrow));
    // func:algo_lib.RegxState..Ctor
    inline               RegxState() __attribute__((nothrow));
    // func:algo_lib.RegxState..Dtor
    inline               ~RegxState() __attribute__((nothrow));
    // func:algo_lib.RegxState..CopyCtor
    RegxState(const algo_lib::RegxState &rhs) __attribute__((nothrow));
};
```

#### algo_lib.Replscope - 
<a href="#algo_lib-replscope"></a>

#### algo_lib.Replscope Fields
<a href="#algo_lib-replscope-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|algo_lib.Replscope.eatcomma|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||
|algo_lib.Replscope.fatal|bool|[Val](/txt/exe/amc/reftypes.md#val)||Kill process on bad substitution|
|algo_lib.Replscope.ind_replvar|[algo_lib.FReplvar](/txt/lib/algo_lib/README.md#algo_lib-freplvar)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct Replscope
<a href="#struct-replscope"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/algo_lib_gen.h](/include/gen/algo_lib_gen.h)
```
struct Replscope { // algo_lib.Replscope
    bool                   eatcomma;                    //   true
    bool                   fatal;                       //   false  Kill process on bad substitution
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

