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
// Source: cpp/lib/algo/file.cpp -- File functions
//
// AL: used sendfile() here previously, but don't like the random
// failures; using read/write instead.

#include "include/algo.h"

#ifndef WIN32
#include <fnmatch.h>
#include <sys/mman.h>
#endif

#ifdef WIN32
#include <aclapi.h>
#endif

// -----------------------------------------------------------------------------

inline char *ToCstr(char *to, const strptr &x) {
    memcpy(to,x.elems,x.n_elems);
    to[x.n_elems]=0;
    return to;
}

// function to convert strptr X to a nul-terminated char*
// uses alloca
#define TOCSTR(x) ToCstr((char*)alloca((x).n_elems+1),(x))

// -----------------------------------------------------------------------------

#if defined(__MACH__) || __FreeBSD__>0 || defined(__CYGWIN__) || defined(WIN32)
// replacement for missing lseek64
static i64 lseek64(int fd, i64 off, int whence) {
#ifdef WIN32
    return _lseeki64(fd,off,whence);
#else
    return lseek(fd,off,whence);
#endif
}
#endif

// -----------------------------------------------------------------------------

// Copy file FROM to file TO, creating the file if necessary
// with mode MODE.
// Throw exception on failure.
void algo::CopyFileX(strptr from, strptr to, int mode) {
    algo_lib::FFildes in;
    in.fd = OpenRead(from, algo_FileFlags__throw);
    algo_lib::FFildes out;
    // make sure to supply O_TRUNC.
    out.fd = Fildes(open(TOCSTR(to), O_WRONLY | O_CREAT | O_TRUNC, mode));
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

// Test whether FNAME refers to a valid filesystem entity (file, directory, or special file)
// If FNAME is a soft link, then TRUE is returned even if the link points to
// a non-existent location.
bool algo::FileObjectExistsQ(strptr fname) NOTHROW {
    StatStruct fst;
    return 0==lstat(TOCSTR(fname), &fst);
}

// -----------------------------------------------------------------------------

// Test whether PATH is an existing directory
bool algo::DirectoryQ(strptr path) NOTHROW {
    StatStruct fst;
    return 0==stat(TOCSTR(path), &fst) && S_ISDIR(fst.st_mode);
}

// -----------------------------------------------------------------------------

// Test if F refers to an existing regular file (i.e. not a special file or directory)
bool algo::FileQ(strptr fname) NOTHROW {
    StatStruct fst;
    int rc = stat(TOCSTR(fname), &fst);
    return rc==0 && S_ISREG(fst.st_mode);
}

// -----------------------------------------------------------------------------

// Wrapper for c library realpath function.
// On Windows: read path and expand all soft links along the way; Also eat ..'s.
// The following table shows successive values of LEFT and RIGHT as the loop executes
// ExpandLinks("bin/amc")
// LEFT                                   RIGHT
// bin                                    amc
// bin/amc
//    link=../build/release/amc
// bin                                    ../build/release/amc
//                                        build/release/amc
// build                                  release/amc
// build/release                          amc
//    link=CYGWIN_NT-cl.release-x86_64
// build                                  CYGWIN_NT-cl.release-x86_64/amc
// build/CYGWIN_NT-cl.release-x86_64      amc
// build/CYGWIN_NT-cl.release-x86_64/amc
tempstr algo::GetFullPath(strptr path) NOTHROW {
    tempstr ret;
#ifdef WIN32
    tempstr link;
    tempstr newlink;
    while (path.n_elems) {
        strptr nextcomp = Pathcomp(path,"/LL");
        path=Pathcomp(path,"/LR");
        if (nextcomp == "..") {
            ret.ch_n = TRevFind(ret, '/').beg;
        } else if (nextcomp != "") {
            ret << algo::MaybeDirSep << nextcomp;
            newlink = algo::ReadLink(ret);
            if (newlink != "") {
                if (AbsolutePathQ(newlink)) {
                    ret = newlink;
                } else {
                    if (path.n_elems) {
                        newlink << algo::MaybeDirSep << path;
                    }
                    link = newlink;
                    path = link;
                    ret.ch_n = TRevFind(ret, '/').beg;
                }
            }
        }
    }
#else
    char buf[PATH_MAX];
    if (NULL != realpath(TOCSTR(path),buf)) {
        ret << strptr(buf);
    }
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// Delete file F.
// Return success code.
bool algo::DeleteFile(strptr f) NOTHROW {
    bool ret = elems_N(f) > 0;
    tempstr fname(f);
    // Dubious work-around for Windows -- many files are created with special
    // attributes (read-only) that prevent deletion. These these now
    // Once these files have been deleted, the line is no longer needed
#ifdef WIN32
    (void)SetFileAttributes(Zeroterm(fname), FILE_ATTRIBUTE_NORMAL);
#endif
    ret = ret && unlink(Zeroterm(fname)) == 0;
    return ret;
}

// -----------------------------------------------------------------------------

// Remove everything up to and including the last slash in A,
// and return that. This is equivalent to Pathcomp(A,"/RR");
strptr algo::StripDirName(strptr a) NOTHROW {
    int i = elems_N(a);
    for ( ; i>0 && !algo_lib::DirSepQ(a[i-1]); i--) {
    }
    return RestFrom(a,i);
}

// -----------------------------------------------------------------------------

// Strip last slash and everything after it in A,
// and return that. This is equivalent to Pathcomp(A,"/RL");
strptr algo::GetDirName(strptr a) NOTHROW {
    int i=elems_N(a);
    for (; i>0 && !algo_lib::DirSepQ(a[i-1]); i--) {
    }
    return FirstN(a,i);
}

// -----------------------------------------------------------------------------

// Recursively create directory S and all of its components,
// using mode MODE for newly created directories.
// if DO_THROW is specified, throw exceptions on failure.
// If DO_THROW is false, return success value.
// TODO: test on windows
bool algo::CreateDirRecurse(strptr s, bool do_throw, u32 mode) {
    if (!elems_N(s))           return false;
    if (DirectoryQ(s)) return true;
    strptr base = StripDirComponent(s);
    if (elems_N(base) && !CreateDirRecurse(base, do_throw, mode)) return false;
    bool ok = mkdir(TOCSTR(s),mode)==0;
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
    while (len>0 && algo_lib::DirSepQ(a[len-1])) len--;
    int i=len;
    for (; i>0 && !algo_lib::DirSepQ(a[i-1]); i--) {}
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
        if (algo_lib::DirSepQ(a[i])) {
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
        if (algo_lib::DirSepQ(a[i])) {
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
// On windows, the same test is in force, but in addition any pathname
// where the second character is a : (e.g. c:\blah) is recognized as an absolute
// path name
bool algo::AbsolutePathQ(strptr path) NOTHROW {
    bool ret = elems_N(path) > 0 && (path[0]=='/' || path[0]=='~');
#ifdef WIN32
    ret = ret || (elems_N(path) > 1 && (path[1]==':'));
#endif
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
#ifdef __linux__
    char *p = get_current_dir_name();
    ret<<p;
    free(p);
#else
    char buf[BUFSIZ];
    ret << strptr(getcwd(buf,sizeof(buf)-1));
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// Change current directory to DIR and return success status
// errno is set as witih chdir() call
bool algo::SetCurDir(strptr in_dir) NOTHROW {
    return chdir(TOCSTR(in_dir))==0;
}

// -----------------------------------------------------------------------------

// Calculate size of file referred to by FILENAME.
// If file is not found or an error occurs, 0 is returned.
i64 algo::GetFileSize(strptr filename) NOTHROW {
    i64 ret = 0;
    StatStruct s;
    if (stat(TOCSTR(filename),&s) == 0) {
        ret = s.st_size;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return size of file referred to by FD.
// On error, return zero.
i64 algo::GetFileSize(Fildes fd) NOTHROW {
    StatStruct buf;
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
    str << MaybeDirSep;
    str << StripDirName(b) << GetFileExt(a);
    return str;
}

// -----------------------------------------------------------------------------

algo::UnTime algo::ModTime(strptr filename) {
    StatStruct S;
    UnTime ret;
    if (stat(TOCSTR(filename), &S)==0) {
        ret = ToUnTime(UnixTime(S.st_mtime));
    }
    return ret;
}

// -----------------------------------------------------------------------------

bool algo::RemDir(strptr name) {
    bool ok = rmdir(TOCSTR(name))==0;
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
                (void)algo::DeleteFile(E.pathname);
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

// -----------------------------------------------------------------------------

void algo_lib::fildes_Cleanup(algo_lib::FLockfile &lockfile) {
    if (ValidQ(lockfile.fildes.fd)) {
        (void)flock(lockfile.fildes.fd.value, LOCK_UN);
        algo::Refurbish(lockfile.fildes);
        // delete file, but only if it was successfully locked.
        unlink(Zeroterm(lockfile.filename));
    }
}

// -----------------------------------------------------------------------------

// On Windows, open() does not respect POSIX permissions well.
// Moreover, premissions inherit from parent apply.
// Thus we need to clear ACL, and establish new one with correct owner, group,
// and world permissions, and drop inheritance.
// This is best-effort function, silently exits on any error.
#ifdef WIN32
static void WinChmod644(char *filename) {
    // get owner and group
    PSID owner(NULL);
    PSID group(NULL);
    PSECURITY_DESCRIPTOR sd(NULL); // used as storage for owner and group SID
    if (GetNamedSecurityInfo(filename, SE_FILE_OBJECT,
                             OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
                             &owner, &group, NULL, NULL, &sd)!=ERROR_SUCCESS && sd) {
        LocalFree(sd);
        sd = NULL;
        owner = NULL;
        group = NULL;
    }

    // create new ACL
    PACL acl(NULL);
    if (owner && group) {
        // create explicit access for owner
        EXPLICIT_ACCESS ea[3];
        ZeroMemory(&ea,sizeof ea);
        // owner
        ea[0].grfAccessPermissions = STANDARD_RIGHTS_ALL | GENERIC_READ | GENERIC_WRITE;
        ea[0].grfAccessMode = SET_ACCESS;
        ea[0].grfInheritance = NO_INHERITANCE;
        BuildTrusteeWithSid(&ea[0].Trustee,owner);
        // group
        ea[1].grfAccessPermissions = GENERIC_READ;
        ea[1].grfAccessMode = SET_ACCESS;
        ea[1].grfInheritance = NO_INHERITANCE;
        BuildTrusteeWithSid(&ea[1].Trustee,group);
        // world
        ea[2].grfAccessPermissions = GENERIC_READ;
        ea[2].grfAccessMode = SET_ACCESS;
        ea[2].grfInheritance = NO_INHERITANCE;
        BuildTrusteeWithName(&ea[2].Trustee,"Everyone");

        if (SetEntriesInAcl(_array_count(ea),ea,NULL,&acl)!=ERROR_SUCCESS && acl) {
            LocalFree(acl);
            acl = NULL;
        }
    }

    // apply ACL
    if (acl) {
        SetNamedSecurityInfo(filename, SE_FILE_OBJECT,
                             DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
                             NULL, NULL, acl, NULL);
    }

    // cleanup
    if (acl) {
        LocalFree(acl);
    }
    if (sd) {
        LocalFree(sd);
    }
}
#endif

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
    // append implies write
    if (append_Get(flags)) {
        write_Set(flags, true);
    }
    if (append_Get(flags)) {
        // #AL# without O_APPEND, 2 processes cannot reliably append to the same file
        // #AL# warning: O_APPEND does not work on NFS systems (see man 2 open)
        //os_flags |= O_APPEND;
        os_flags = O_RDWR | O_CREAT;
    } else if (write_Get(flags)) {
        os_flags = O_RDWR | O_CREAT | O_TRUNC;
    } else if (read_Get(flags)) {
        os_flags = O_RDONLY;
    }
#ifdef WIN32
    // windows: prevent interpretation of file contents!:
    (void)_set_fmode(_O_BINARY);
    if (linear_Get(flags)) {
        os_flags |= O_SEQUENTIAL;
    }
    os_flags |= _O_BINARY;
    // if (algo::temp_Get(flags)) {
    //     os_flags |= _O_TEMPORARY;
    // }
    // O_TMPFILE doesn't behave as you would expect --
    // it creates a an anonymous file in directory specified by path
    // so we ignore it here

    // we need to know if the file was newly created,
    // in order to apply win ACL.
    // O_EXCL will be indication of it.
    if ((os_flags & O_CREAT)) {
        os_flags |= O_EXCL;
    }
#endif
    Fildes fd(open(Zeroterm(fn), os_flags, 0644));
#ifdef WIN32
    if (!ValidQ(fd) && (os_flags & O_EXCL)) {
        // drop O_EXCL and reopen file
        os_flags &= ~int(O_EXCL);
        fd = Fildes(open(Zeroterm(fn), os_flags, 0644));
    }
#endif
    if (!ValidQ(fd) && printerr_Get(flags)) {
        prerr("open ["<<filename<<"]: "<<strerror(errno));
    }
    errno_vrfy(ValidQ(fd) || !(_throw_Get(flags))
               ,tempstr()<< "open ["<<filename<<"]");
#ifdef WIN32
    if (ValidQ(fd) && (os_flags & O_EXCL)) {
        // file was created, apply ACL
        WinChmod644(Zeroterm(fn));
    }
#endif
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
bool algo::SeekFile(Fildes fd, i64 off) {
    bool ok = lseek64(fd.value, off, SEEK_SET) == (i64)off;
    return ok;
}

// -----------------------------------------------------------------------------

// Return current file position on FD
// There is no provision to return an error code; only the offset is returned
// (zero on failure?)
i64 algo::GetPos(Fildes fd) {
    return lseek64(fd.value, 0, SEEK_CUR);
}

// -----------------------------------------------------------------------------

// Truncate file indicated by FD to size SIZE.
// Return success status
// TODO: Test on windows
bool algo::TruncateFile(Fildes fd, i64 size) {
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
// Todo: test on windows
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
            errno_vrfy(nread>=0 || !(flags & algo_FileFlags__throw),tempstr()<< "read "<<fname);
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
    return FileToString(fname,algo_FileFlags__throw);
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
    StatStruct st;
    algo::ZeroBytes(st);
    int rc=stat(fname,&st);
    return rc==-1 ? dflt : st.st_mode;
}

// -----------------------------------------------------------------------------

static int CreateReplacementFile(cstring &oldfname, cstring &newfname, int dfltmode) {
    ch_RemoveAll(newfname);
    newfname << oldfname << "-XXXXXX";
    mode_t old_mask = umask(033);
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
#ifdef WIN32
            // Windows is unable to rename a file on top of an existing file
            (void)unlink(Zeroterm(oldfile));
#endif
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
// FLAGS may specify algo_FileFlags__throw, in which case an exception is thrown on error
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
        errno_vrfy(!(flags & algo_FileFlags__throw) || write_ok, "write");
    }
    return dowrite;
}

// Short version of StringToFile: compares file contents before writing,
// throws exception on error.
bool algo::StringToFile(const strptr& str, const strptr& filename) {
    return StringToFile(str,filename,algo_FileFlags__throw,true);
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
    (void)curs;
    do {
        algo::DirEntry &E = curs.E;
        struct dirent *dir_ent = readdir(curs.E.dir_handle);
        curs.E.eof     = dir_ent == NULL;
        bool match = false;
        if (!E.eof) {
            E.filename = strptr(dir_ent->d_name);
            E.pathname.ch_n =  ch_N(E.dirname);
            E.pathname << algo::MaybeDirSep << E.filename;
            StatStruct st;
            if (0 == lstat(Zeroterm(E.pathname), &st)) {
                E.is_dir = S_ISDIR(st.st_mode) != 0;
                E.mode   = st.st_mode;
                // can we grab the nanosecs too?
                E.mtime  = ToUnTime(algo::UnixTime(st.st_mtime));
                E.ctime  = ToUnTime(algo::UnixTime(st.st_ctime));
                E.size   = st.st_size;
                match    = E.filename != "." && E.filename != ".."; // never pass these back to user
                match = match && fnmatch(Zeroterm(E.pattern), Zeroterm(E.filename), FNM_FILE_NAME)==0;
            }
        }
        E.match = match;
    } while (!curs.E.eof && !curs.E.match);
}

// -----------------------------------------------------------------------------

// Begin scanning files matching shell pattern PATTERN.
void algo::Dir_curs_Reset(Dir_curs &curs, strptr pattern) NOTHROW {
    strptr dirname = GetDirName(pattern);
    if (!elems_N(dirname)) {
        dirname =".";
    }
    strptr mask = StripDirName(pattern);
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
    int ret = -1;
#ifdef WIN32
    // todo: move this to win32.cpp and implement fcntl
    HANDLE h = ValidQ(fildes) ? (HANDLE)_get_osfhandle(fildes.value) : NULL;
    if (h && GetFileType(h) == FILE_TYPE_PIPE) {
        DWORD flags = blocking ? PIPE_WAIT : PIPE_NOWAIT;
        if (SetNamedPipeHandleState(h, &flags, NULL, NULL)) {
            ret=0;
        }
    }
#else
    int flags = fcntl(fildes.value, F_GETFL);
    int fl    = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    ret = fcntl(fildes.value, F_SETFL, fl);
#endif
    return ret;
}

// -----------------------------------------------------------------------------

// If PATH is an existing path, leave it unchanged
// On Windows, If PATH.EXE is an existing path, return that
// Return true if file exists
bool algo_lib::TryExeSuffix(algo::cstring &path) {
    bool ret = FileQ(path);
#ifdef WIN32
    if (!ret && ReadLink(path)=="") {
        int n = path.ch_n;
        path << ".exe";
        ret = FileQ(path) && ReadLink(path)=="";
        if (!ret) {
            path.ch_n=n;
        }
    }
#endif
    return ret;
}

// -----------------------------------------------------------------------------

static tempstr PathSearch(strptr fname) {
    strptr path_first = ":LL";
    strptr path_rest  = ":LR";
#ifdef WIN32
    path_first = ";LL";
    path_rest  = ";LR";
#endif
    tempstr ret(fname);
    for (strptr path=getenv("PATH"); path != ""; path = Pathcomp(path,path_rest)) {
        strptr left = Pathcomp(path,path_first);
        tempstr candidate(DirFileJoin(left,fname));
        if (algo_lib::TryExeSuffix(candidate)) {
            ret=candidate;
            break;
        }
    }
    return ret;
}

// Update FNAME to be a filename that can be passed to Unix exec call.
// If FNAME is an absolute path, don't perform a search
// If FNAME is a relative path, perform a search using the PATH environment
// the first executable file that's found is the result.
void algo_lib::ResolveExecFname(algo::cstring &fname) {
    if (!AbsolutePathQ(fname)) {
        if (algo_lib::TryExeSuffix(fname)) {
            // all good
        } else {
            fname = PathSearch(fname);
        }
    }
}

// -----------------------------------------------------------------------------

// Read soft link and return resulting path.
// If PATH is not a soft link, return empty string
// This is not the function to resolve symlinks (if link temp/x points to y, then
// this function will return string "y", not "temp/y"), use GetFullPath for
// full service
tempstr algo::ReadLink(strptr path) {
    tempstr ret;
    char buf[4096];
    int n=readlink(TOCSTR(path),buf,sizeof(buf));
    if (n>0) {
        ret << strptr(buf,n);
    }
    return ret;
}
