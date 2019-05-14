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
// Source: cpp/lib/algo/file.cpp -- File functions
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev luke.huang
//
// AL: used sendfile() here previously, but don't like the random
// failures; using read/write instead.

#include <fnmatch.h>
#include <sys/mman.h>

// -----------------------------------------------------------------------------

#ifdef __MACH__
// replacement for missing lseek64
static i64 lseek64(int fd, off64_t off, int whence) {
    return lseek(fd,off,whence);
}
#endif

// -----------------------------------------------------------------------------

// Copy file FROM to file TO, creating the file if necessary
// with mode MODE.
// Throw exception on failure.
void algo::CopyFileX(strptr from, strptr to, int mode) {
    algo_lib::FFildes in;
    in.fd = OpenRead(from, algo_FileFlags_throw);
    algo_lib::FFildes out;
    // make sure to supply O_TRUNC.
    out.fd = Fildes(open(Zeroterm(tempstr() << to), O_WRONLY | O_CREAT | O_TRUNC, mode));
    (void)algo_lib::fildes_XrefMaybe(in);
    (void)algo_lib::fildes_XrefMaybe(out);
    errno_vrfy(ValidQ(out.fd),tempstr()<< "open ["<<to<<"]");
    char buf[4096];
    ssize_t nread;
    do {
        nread = read(in.fd.value, buf, sizeof(buf));
        errno_vrfy(nread >=0, "read");
        if (nread) errno_vrfy(write(out.fd.value, buf, nread)==nread, "write");
    } while (nread != 0);
}

// -----------------------------------------------------------------------------

// Read all bytes from IN_FD and copy them to OUT_FD, using read/write.
// Return success status.
bool algo::CopyFd(Fildes in_fd, Fildes out_fd) NOTHROW {
    char buf[4096];
    ssize_t nread;
    bool success = true;
    (void)SeekFile(in_fd, 0);
    do {
        nread = read(in_fd.value, buf, sizeof(buf));
        success = success && nread >=0;// -1 = error, 0=eof
        if (nread>0) {
            success = success && write(out_fd.value, buf, nread)==nread;
        }
    } while (nread > 0 && success);
    return success;
}

// -----------------------------------------------------------------------------

// Test whether C is a directory separator.
bool algo::DirSepQ(int c) NOTHROW {
    return c=='/';
}

// -----------------------------------------------------------------------------

// Test whether FNAME refers to a valid filesystem entity (file, directory, or special file)
bool algo::FileObjectExistsQ(strptr fname) NOTHROW {
    struct stat fst;
    return 0==stat(algo::Zeroterm(tempstr(fname)), &fst);
}

// -----------------------------------------------------------------------------

// Test whether PATH is an existing directory
bool algo::DirectoryQ(strptr path) NOTHROW {
    struct stat fst;
    return 0==stat(Zeroterm(tempstr(path)), &fst) && S_ISDIR(fst.st_mode);
}

// -----------------------------------------------------------------------------

// Test if F refers to an existing regular file (i.e. not a special file or directory)
bool algo::FileQ(strptr fname) NOTHROW {
    struct stat fst;
    int rc = stat(Zeroterm(tempstr(fname)), &fst);
    return rc==0 && S_ISREG(fst.st_mode);
}

// -----------------------------------------------------------------------------

// Wrapper for c library realpath function.
tempstr algo::GetFullPath(strptr path) NOTHROW {
    tempstr ret;
    char buf[PATH_MAX];
    if (NULL != realpath(Zeroterm(tempstr(path)),buf)) {
        ret << strptr(buf);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Delete file F.
// Return success code.
bool algo::DeleteFile(strptr f) NOTHROW {
    bool ret = elems_N(f) > 0;
    ret = ret && unlink(Zeroterm(tempstr() << f))==0;
    return ret;
}

// -----------------------------------------------------------------------------

// Remove everything up to and including the last slash in A,
// and return that. This is equivalent to Pathcomp(A,"/RR");
strptr algo::StripDirName(strptr a) NOTHROW {
    int i = elems_N(a);
    for ( ; i>0 && !DirSepQ(a[i-1]); i--) {
    }
    return RestFrom(a,i);
}

// -----------------------------------------------------------------------------

// Strip last slash and everything after it in A,
// and return that. This is equivalent to Pathcomp(A,"/RL");
strptr algo::GetDirName(strptr a) NOTHROW {
    int i=elems_N(a);
    for (; i>0 && !DirSepQ(a[i-1]); i--) {
    }
    return FirstN(a,i);
}

// -----------------------------------------------------------------------------

// Recursively create directory S and all of its components,
// using mode MODE for newly created directories.
// if DO_THROW is specified, throw exceptions on failure.
// If DO_THROW is false, return success value.
bool algo::CreateDirRecurse(strptr s, bool do_throw, u32 mode) {
    if (!elems_N(s))           return false;
    if (DirectoryQ(s)) return true;
    strptr base = StripDirComponent(s);
    if (elems_N(base) && !CreateDirRecurse(base, do_throw, mode)) return false;
    tempstr v(s);
    bool ok = mkdir(Zeroterm(v),mode)==0;
    ok     |= errno == EEXIST;
    errno_vrfy(!do_throw || ok,tempstr()<< "create dir ["<<s<<"]");
    return ok;
}

// Short version of CreateDirRecurse -- throws exception on error,
// new directory has mode 0755 if created
bool algo::CreateDirRecurse(strptr f) {
    return CreateDirRecurse(f, true, 0755);
}

// -----------------------------------------------------------------------------

// Strip last component of a path.
// If the path ends in a slash, the slash is stripped as well.
// So, StripDirComponent("abcde/d/") -> abcde/
// So, StripDirComponent("abcde/d") -> abcde/
strptr algo::StripDirComponent(strptr a) NOTHROW {
    int len = elems_N(a);
    while (len>0 && DirSepQ(a[len-1])) len--;
    int i=len;
    for (; i>0 && !DirSepQ(a[i-1]); i--) {}
    return FirstN(a,i);
}

// -----------------------------------------------------------------------------

// Combine paths A and B, placing a slash in the middle if necessary.
// If B is already an absolute pathname, just return that.
tempstr algo::DirFileJoin(strptr a, strptr b) NOTHROW {
    tempstr result;
    if (elems_N(a)>0 && !AbsolutePathQ(b)) {
        result<<a;
        result<<MaybeDirSep;
    }
    result<<b;
    return result;
}

// -----------------------------------------------------------------------------

// Remove pathname A with any extension stripped.
// StripExt("abcd/e.txt") -> "abcd/e"
// StripExt("abcd/e.txt/f") -> "abcd/e/f"
// (The last slash shields the extension, so this function has no corresponding
// Pathcomp expression)
strptr algo::StripExt(strptr a) NOTHROW {
    for (int i=elems_N(a)-1; i>=0; i--) {
        if (DirSepQ(a[i])) {
            return a;
        }
        if (a[i]=='.') {
            return FirstN(a,i);
        }
    }
    return a;
}

// -----------------------------------------------------------------------------

// Return extension of pathname A, including the dot.
// GetFileExt("abcd/e.txt") -> ".txt"
// GetFileExt("abcd/e.txt/f") -> ""
// This function is equivalent to Pathcomp(A,"/RR.RR")
strptr algo::GetFileExt(strptr a) NOTHROW {
    for (int i=elems_N(a)-1; i>=0; i--) {
        if (DirSepQ(a[i])) {
            return strptr();
        }
        if (a[i]=='.') {
            return RestFrom(a,i);
        }
    }
    return strptr();
}

// -----------------------------------------------------------------------------

// Check if path PATH is an absolute pathname,
// meaning that it starts with / or ~
bool algo::AbsolutePathQ(strptr path) NOTHROW {
    bool ret = elems_N(path) > 0 && (path[0]=='/' || path[0]=='~');
    return ret;
}

// -----------------------------------------------------------------------------

// Replace extension in pathname A with string B.
tempstr algo::ReplaceExt(strptr a, strptr b) NOTHROW {
    tempstr result;
    result<<StripExt(a);
    result<<StripDirName(b);
    return result;
}

// -----------------------------------------------------------------------------

// Return current directory name.
tempstr algo::GetCurDir() NOTHROW {
    tempstr ret;
#ifdef __MACH__
    char buf[BUFSIZ];
    ret << strptr(getcwd(buf,sizeof(buf)-1));
#else
    char *p = get_current_dir_name();
    ret<<p;
    free(p);
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// Change current directory to DIR and return success status
// errno is set as witih chdir() call
bool algo::SetCurDir(strptr dir) NOTHROW {
    tempstr ret(dir);
    return chdir(Zeroterm(ret))==0;
}

// -----------------------------------------------------------------------------

// Calculate size of file referred to by FILENAME.
off64_t algo::GetFileSize(strptr filename) NOTHROW {
    struct stat s;
    if (stat(Zeroterm(tempstr(filename)),&s)) {
        return -1;
    }
    return s.st_size;
}

// -----------------------------------------------------------------------------

// Return size of file referred to by FD.
// On error, return zero.
off64_t algo::GetFileSize(Fildes fd) NOTHROW {
    struct stat buf;
    ZeroBytes(buf);
    (void)fstat(fd.value, &buf);
    return buf.st_size;
}

// -----------------------------------------------------------------------------

// Strip directory name in PATH. and return the rest
// This is equivalent to Pathcomp(path,"/RR");
// c:\dir\file.txt -> file
strptr algo::GetFileName(const strptr& path) NOTHROW {
    return StripDirName(StripExt(path));
}

// -----------------------------------------------------------------------------

// F  ("a.txt","b") -> "b.txt";
// F  ("a.txt","/b.jpg") -> "b.jpg.txt"
tempstr algo::ReplaceFileName(const strptr& a, const strptr& b) {
    tempstr str;
    str << GetDirName(a);
    if (ch_N(str) && !DirSepQ(ch_qLast(str))) str<<"/";
    str << StripDirName(b) << GetFileExt(a);
    return str;
}

// -----------------------------------------------------------------------------

UnTime algo::ModTime(strptr filename) {
    struct stat S;
    UnTime ret;
    if (stat(Zeroterm(tempstr() << filename), &S)==0) {
        ret = ToUnTime(UnixTime(S.st_mtime));
    }
    return ret;
}

bool algo::RemDir(strptr name) {
    bool ok = rmdir(Zeroterm(tempstr() << name))==0;
    return ok;
}

// -----------------------------------------------------------------------------

// Destroy directory NAME, and any subdirectories.
// If REMOVE_TOPMOST is specified, delete the directory as well,
// (otherwise, only the contents are deleted).
// If filesystem entry exists and is NOT a directory, return true.
bool algo::RemDirRecurse(strptr name, bool remove_topmost) {
    bool ret = true;
    if (DirectoryQ(name)) {
        ind_beg(Dir_curs,E, DirFileJoin(name,"*")) {
            if( E.is_dir ){
                RemDirRecurse(E.pathname, true);
            } else {
                DeleteFile(E.pathname);
            }
        }ind_end;
        if (remove_topmost) {
            RemDir(name);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

void algo::dir_handle_Cleanup(algo::DirEntry &dir_entry) {
    if (dir_entry.dir_handle) {
        closedir(dir_entry.dir_handle);
    }
}

static void DirEntryFill(DirEntry &E) {
    bool match = false;
    if (!E.eof) {
        E.filename = strptr(E.dir_ent->d_name);
        E.pathname.ch_n =  ch_N(E.dirname);
        E.pathname << MaybeDirSep << E.filename;
        if (0 == lstat(Zeroterm(E.pathname), &E.stat)) {
            E.is_dir       = S_ISDIR(E.stat.st_mode) != 0;
            E.mtime        = ToUnTime(UnixTime(E.stat.st_mtime));
            E.ctime        = ToUnTime(UnixTime(E.stat.st_ctime));
            E.size         = E.stat.st_size;
            match          = E.filename != "." && E.filename != "..";// never pass these back to user
            match = match && fnmatch(Zeroterm(E.pattern), Zeroterm(E.filename), FNM_FILE_NAME)==0;
        }
    }
    E.match = match;
}

// -----------------------------------------------------------------------------

void algo_lib::fildes_Cleanup(algo_lib::FLockfile &lockfile) {
    if (ValidQ(lockfile.fildes.fd)) {
        (void)flock(lockfile.fildes.fd.value, LOCK_UN);
        Refurbish(lockfile.fildes);
        // delete file, but only if it was successfully locked.
        unlink(Zeroterm(lockfile.filename));
    }
}

// -----------------------------------------------------------------------------

// Open file FILENAME with flags FLAGS, return resulting file descriptor
// Possible flags:
// write   -> open file for writing, create file if missing
//  if write is specified, and append is not, then file is truncated upon opening
// append  -> open file in append mode (automatically sets 'write' flag)
//  in append mode, seek to end of file after opening
// read    -> open flag in read-only mode
// _throw  -> throw exception if an error occurs
// NOTE: this function doesn't set O_APPEND flag, because it doesn't work'
//   on NFS mounted filesystems.
//   Without O_APPEND, two processes cannot reliably append to the same file.
algo::Fildes algo::OpenFile(const strptr& filename, algo::FileFlags flags) {
    tempstr fn;
    fn<<filename;

    int os_flags = 0;
    if (append_Get(flags)) {
        write_Set(flags, true);
        // #AL# without O_APPEND, 2 processes cannot reliably append to the same file
        // #AL# warning: does not work on NFS systems (see man 2 open)
        //os_flags |= O_APPEND;
    }
    if (read_Get(flags)) {
        os_flags |= O_RDONLY;
    }
    if (write_Get(flags)) {
        os_flags |= O_RDWR | O_CREAT;
    }
    if (write_Get(flags) && !append_Get(flags)) {
        os_flags |= O_TRUNC;
    }

    Fildes fd(open(Zeroterm(fn), os_flags, 0644));
    errno_vrfy(ValidQ(fd) || !(_throw_Get(flags))
               ,tempstr()<< "open ["<<filename<<"]");

    if (ValidQ(fd) && append_Get(flags)) {
        (void)SeekFile(fd,GetFileSize(fd));
    }
    return fd;
}

// -----------------------------------------------------------------------------

// Write BYTES to file FD;
// If FD is non-blocking, the function may fail.
// Throw exception on error. Upon completion, the function has succeeded
void algo::WriteFileX(Fildes fd, memptr bytes) {
    while (bytes.n_elems) {
        int n=write(fd.value, bytes.elems, bytes.n_elems);
        errno_vrfy(n>0, "write()");
        bytes = RestFrom(bytes, n);
    }
}

// -----------------------------------------------------------------------------

// Set file position of FD to OFF
// Return success status
bool algo::SeekFile(Fildes fd, off64_t off) {
    bool ok = lseek64(fd.value, off, SEEK_SET) == (i64)off;
    return ok;
}

// -----------------------------------------------------------------------------

// Return current file position on FD
// There is no provision to return an error code; only the offset is returned
// (zero on failure?)
off64_t algo::GetPos(Fildes fd) {
    return lseek64(fd.value, 0, SEEK_CUR);
}

// -----------------------------------------------------------------------------

// Truncate file indicated by FD to size SIZE.
// Return success status
bool algo::TruncateFile(Fildes fd, off64_t size) {
    return ftruncate(fd.value, size)==0;
}

// -----------------------------------------------------------------------------

// Return contents of file as string.
// File is trimmed at first NULL character
// Implementation note:
//   linux files in /proc directory lie about their size
//   and are sometimes zero-terminated.
// File is read using a "safe" method of succesively calling read.
// relying on reported file size or using mmap does not work in all cases
const tempstr algo::FileToString(const strptr& fname, algo::FileFlags flags) {
    algo_lib::FFildes fd;
    tempstr ret;
    fd.fd = OpenRead(fname,flags);
    if (ValidQ(fd.fd)) {
        // linux files in /proc directory lie about their size
        // (4096) and also sometimes are zero-terminated.
        // also sometimes the reads terminate early, even though
        // not at file's end
        u8 buf[4096];
        while (true) {
            ssize_t nread = read(fd.fd.value,buf,sizeof(buf));
            errno_vrfy(nread>=0 || !(flags & algo_FileFlags_throw),tempstr()<< "read "<<fname);
            if (nread > 0) {
                ch_Addary(ret, strptr((char*)buf, nread));
            }
            // #AL# partial read cannot be used as a condition for EOF, because of /proc
            if (nread <= 0) {
                break;
            }
        }
        if (ch_N(ret) && ch_qLast(ret) == 0) {
            ret.ch_n--;
        }
        // remove everything beyond first NULL
        ret.ch_n = ch_FindFirst(ret,0).beg;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Same Short version of FileToString, throw exception on error.
const tempstr algo::FileToString(const strptr& fname) {
    return FileToString(fname,algo_FileFlags_throw);
}

// -----------------------------------------------------------------------------

// Read all bytes from IN_FD as a string.
// NUL characters in file will appear in the string --
// TODO: add option to sanitize string
// If FD is non-blocking, reading will terminate at first EAGAIN.
// If an error occurs, all data prior to occurrence of the error is returned.
const tempstr algo::FdToString(Fildes in_fd) NOTHROW {
    char buf[4096];
    ssize_t nread;
    // ignore result, file may not support it
    (void)SeekFile(in_fd, 0);
    tempstr resp_body;
    do {
        nread = read(in_fd.value, buf, sizeof(buf));
        if (nread > 0) {
            resp_body << strptr(buf,(i32)nread);
        }
    } while (nread > 0);
    resp_body << eol;
    return resp_body;
}

// -----------------------------------------------------------------------------

static i64 GetMode(char *fname, int dflt) {
    struct stat st;
    ZeroBytes(st);
    int rc=stat(fname,&st);
    return rc==-1 ? dflt : st.st_mode;
}

// -----------------------------------------------------------------------------

static int CreateReplacementFile(cstring &oldfname, cstring &newfname, int dfltmode) {
    ch_RemoveAll(newfname);
    newfname << oldfname << "-XXXXXX";
    mode_t old_mask = umask(0333);
    int fd=mkstemp((char*)Zeroterm(newfname));
    (void)umask(old_mask);
    if (fd!=-1) {
        // mkstemp creates a file with mode 0600 --
        // preserve old file's mode or use 0644
        (void)fchmod(fd,GetMode((char*)Zeroterm(oldfname),dfltmode));
    }
    return fd;
}

// -----------------------------------------------------------------------------

// Compare STR to contents of FILENAME.
// If they differ, use a tempfile construction to atomically UPDATE file
// with contents of STR.
// NOTE: This will result in a new inode for file!
// No exceptions are thrown. If the function fails, check errno.
// Default mode for new file is provided by MODE.
// If the file is being replaced, MODE is ignored and copied from the old file.
bool algo::SafeStringToFile(const strptr& str, const strptr& filename, int dfltmode){
    bool ok = true;
    bool attempt_write=false;
    if (!ch_N(filename)) {
        errno = EINVAL;
        ok = false;
    } else {
        algo_lib::MmapFile file;
        ok = MmapFile_Load(file, filename);// does not throw
        // if file does not exist, try to create it
        attempt_write = !ok || !(str == file.text);
    }
    if (attempt_write) {
        tempstr oldfile(filename);
        tempstr newfile;
        int fd = CreateReplacementFile(oldfile,newfile,dfltmode);
        if (fd==-1) {
            ok=false;
        } else {
            ok=WriteFile(algo::Fildes(fd), (u8*)str.elems, str.n_elems);// does not throw
            (void)close(fd);
        }
        if (ok) {
            ok=rename(Zeroterm(newfile),Zeroterm(oldfile))==0;
        } else {
            int savederr=errno;
            unlink(Zeroterm(newfile));
            errno=savederr;// ignore unlink error -- previous error is what counts
        }
        algo_lib::_db.stringtofile_nwrite++;// count the event
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool algo::SafeStringToFile(const strptr& str, const strptr& filename) {
    return SafeStringToFile(str,filename,0644);
}

// -----------------------------------------------------------------------------

// Replace contents of file FILENAME with string STR.
// If CHECK_SAME is specified, first compare contents and do not perform a write
// if the contents are the same.
// FLAGS may specify algo_FileFlags_throw, in which case an exception is thrown on error
bool algo::StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags, bool check_same){
    bool dowrite = elems_N(filename)>0;
    if (dowrite && check_same) {
        algo_lib::MmapFile file;
        bool ok = MmapFile_Load(file, filename);// does not throw
        dowrite = !ok || !(str == file.text);// compare
    }
    if (dowrite) {
        algo_lib::FFildes fildes;
        fildes.fd = OpenWrite(filename,flags); // file is automatically truncated
        bool write_ok=WriteFile(fildes.fd, (u8*)str.elems, str.n_elems);// does not throw
        errno_vrfy(!(flags & algo_FileFlags_throw) || write_ok, "write");
    }
    return dowrite;
}

// Short version of StringToFile: compares file contents before writing,
// throws exception on error.
bool algo::StringToFile(const strptr& str, const strptr& filename) {
    return StringToFile(str,filename,algo_FileFlags_throw,true);
}

// Short version of StringToFile: compares file contents before writing.
bool algo::StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags) {
    return StringToFile(str,filename,flags,true);
}

// -----------------------------------------------------------------------------

algo::Fildes algo::OpenWrite(const strptr& filename, algo::FileFlags flags) {
    write_Set(flags, true);
    return OpenFile(filename,flags);
}

algo::Fildes algo::OpenWrite(const strptr& filename) {
    return OpenWrite(filename,algo::FileFlags());
}

// -----------------------------------------------------------------------------

algo::Fildes algo::OpenRead(const strptr& filename, algo::FileFlags flags) {
    read_Set(flags, true);
    return OpenFile(filename,flags);
}

algo::Fildes algo::OpenRead(const strptr& filename) {
    return OpenRead(filename,algo::FileFlags());
}

// -----------------------------------------------------------------------------

// Write NWRITE bytes starting at START to file descriptor FILDES.
// Go until all bytes are written on an error occurs.
// If FILDES is non-blocking, spin indefinitely until bytes do get through.
// At the end, return success status (TRUE if all bytes written)
bool algo::WriteFile(algo::Fildes fildes, u8 *start, int nwrite) {
    bool ok = true;
    while (nwrite > 0 && ok) {
        int nwritten = write(fildes.value, start, nwrite);
        if (nwritten<0) {
            ok = errno==EAGAIN; // spin
        } else {// >=0
            nwrite -= nwritten;
            start += nwritten;
        }
    }
    return ok;
}

// -----------------------------------------------------------------------------

void algo::Dir_curs_Next(Dir_curs &curs) NOTHROW {
    do {
        curs.E.dir_ent = readdir(curs.E.dir_handle);
        curs.E.eof     = curs.E.dir_ent == NULL;
        DirEntryFill(curs.E);
    } while (!curs.E.eof && !curs.E.match);
}

// -----------------------------------------------------------------------------

// Begin scanning files matching shell pattern PATTERN.
void algo::Dir_curs_Reset(Dir_curs &curs, strptr pattern) NOTHROW {
    strptr dirname = Pathcomp(pattern,"/RL");
    if (!elems_N(dirname)) {
        dirname =".";
    }
    strptr mask = Pathcomp(pattern,"/RR");
    curs.E.dirname     = dirname;
    // empty dirname == current directory
    if (!ch_N(curs.E.dirname)) {
        curs.E.dirname << "./";
    }
    curs.E.pathname    = curs.E.dirname;
    curs.E.pattern     = mask;
    curs.E.dir_handle  = opendir(Zeroterm(curs.E.dirname));
    curs.E.eof         = curs.E.dir_handle == NULL;
    if (curs.E.dir_handle) {
        Dir_curs_Next(curs);
    }
}

bool algo::Dir_curs_ValidQ(Dir_curs &curs) NOTHROW {
    return !curs.E.eof;
}

algo::DirEntry &algo::Dir_curs_Access(Dir_curs &curs) NOTHROW {
    return curs.E;
}

// -----------------------------------------------------------------------------

// Set blocking mode on file descriptor FD to BLOCKING
// Return result of FCNTL (0==success)
int algo::SetBlockingMode(Fildes fildes, bool blocking) NOTHROW {
    int flags = fcntl(fildes.value, F_GETFL);
    int fl    = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return fcntl(fildes.value, F_SETFL, fl);
}

// -----------------------------------------------------------------------------

static tempstr PathSearch(strptr fname) {
    for (strptr path=getenv("PATH"); path != ""; path = Pathcomp(path,":LR")) {
        strptr left = Pathcomp(path,":LL");
        tempstr candidate(DirFileJoin(left,fname));
        if (FileQ(candidate)) {
            return candidate;
        }
    }
    return tempstr(fname);
}

// Update FNAME to be a filename that can be passed to Unix exec call.
// If FNAME is an absolute path, do nothing
// If FNAME is a relative path, perform a search using the PATH environment
// variable; upon finding a matching path, set FNAME to the filename found.
void algo_lib::ResolveExecFname(cstring &fname) {
    if (!AbsolutePathQ(fname)) {
        if (FileQ(fname)) {
            // all good
        } else {
            fname = PathSearch(fname);
        }
    }
}
