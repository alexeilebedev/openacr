// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Header: include/algo/file.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once
#include "include/algo.h"

namespace algo { // update-hdr srcfile:%/algo/file.%
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/file.cpp -- File functions
    //

    // Copy file FROM to file TO, creating the file if necessary
    // with mode MODE.
    // Throw exception on failure.
    void CopyFileX(strptr from, strptr to, int mode);

    // Read all bytes from IN_FD and copy them to OUT_FD, using read/write.
    // Return success status.
    bool CopyFd(Fildes in_fd, Fildes out_fd) __attribute__((nothrow));

    // Test whether C is a directory separator.
    bool DirSepQ(int c) __attribute__((nothrow));

    // Test whether FNAME refers to a valid filesystem entity (file, directory, or special file)
    bool FileObjectExistsQ(strptr fname) __attribute__((nothrow));

    // Test whether PATH is an existing directory
    bool DirectoryQ(strptr path) __attribute__((nothrow));

    // Test if F refers to an existing regular file (i.e. not a special file or directory)
    bool FileQ(strptr f) __attribute__((nothrow));

    // Wrapper for c library realpath function.
    tempstr GetFullPath(strptr path) __attribute__((nothrow));

    // Delete file F.
    // Return success code.
    bool DeleteFile(strptr f) __attribute__((nothrow));

    // Remove everything up to and including the last slash in A,
    // and return that. This is equivalent to Pathcomp(A,"/RR");
    strptr StripDirName(strptr a) __attribute__((nothrow));

    // Strip last slash and everything after it in A,
    // and return that. This is equivalent to Pathcomp(A,"/RL");
    strptr GetDirName(strptr a) __attribute__((nothrow));

    // Recursively create directory S and all of its components,
    // using mode MODE for newly created directories.
    // if DO_THROW is specified, throw exceptions on failure.
    // If DO_THROW is false, return success value.
    bool CreateDirRecurse(strptr s, bool do_throw, u32 mode);

    // Short version of CreateDirRecurse -- throws exception on error,
    // new directory has mode 0755 if created
    bool CreateDirRecurse(strptr f);

    // Strip last component of a path.
    // If the path ends in a slash, the slash is stripped as well.
    // So, StripDirComponent("abcde/d/") -> abcde/
    // So, StripDirComponent("abcde/d") -> abcde/
    strptr StripDirComponent(strptr a) __attribute__((nothrow));

    // Combine paths A and B, placing a slash in the middle if necessary.
    // If B is already an absolute pathname, just return that.
    tempstr DirFileJoin(strptr a, strptr b) __attribute__((nothrow));

    // Remove pathname A with any extension stripped.
    // StripExt("abcd/e.txt") -> "abcd/e"
    // StripExt("abcd/e.txt/f") -> "abcd/e/f"
    // (The last slash shields the extension, so this function has no corresponding
    // Pathcomp expression)
    strptr StripExt(strptr a) __attribute__((nothrow));

    // Return extension of pathname A, including the dot.
    // GetFileExt("abcd/e.txt") -> ".txt"
    // GetFileExt("abcd/e.txt/f") -> ""
    // This function is equivalent to Pathcomp(A,"/RR.RR")
    strptr GetFileExt(strptr a) __attribute__((nothrow));

    // Check if path PATH is an absolute pathname,
    // meaning that it starts with / or ~
    bool AbsolutePathQ(strptr path) __attribute__((nothrow));

    // Replace extension in pathname A with string B.
    tempstr ReplaceExt(strptr a, strptr b) __attribute__((nothrow));

    // Return current directory name.
    tempstr GetCurDir() __attribute__((nothrow));

    // Change current directory to DIR and return success status
    // errno is set as witih chdir() call
    bool SetCurDir(strptr dir) __attribute__((nothrow));

    // Calculate size of file referred to by FILENAME.
    off64_t GetFileSize(strptr filename) __attribute__((nothrow));

    // Return size of file referred to by FD.
    // On error, return zero.
    off64_t GetFileSize(Fildes fd) __attribute__((nothrow));

    // Strip directory name in PATH. and return the rest
    // This is equivalent to Pathcomp(path,"/RR");
    // c:\dir\file.txt -> file
    strptr GetFileName(const strptr& path) __attribute__((nothrow));

    // F  ("a.txt","b") -> "b.txt";
    // F  ("a.txt","/b.jpg") -> "b.jpg.txt"
    tempstr ReplaceFileName(const strptr& a, const strptr& b);
    UnTime ModTime(strptr filename);
    bool RemDir(strptr name);

    // Destroy directory NAME, and any subdirectories.
    // If REMOVE_TOPMOST is specified, delete the directory as well,
    // (otherwise, only the contents are deleted).
    // If filesystem entry exists and is NOT a directory, return true.
    bool RemDirRecurse(strptr name, bool remove_topmost);
    void dir_handle_Cleanup(algo::DirEntry &dir_entry);

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
    algo::Fildes OpenFile(const strptr& filename, algo::FileFlags flags);

    // Write BYTES to file FD;
    // If FD is non-blocking, the function may fail.
    // Throw exception on error. Upon completion, the function has succeeded
    void WriteFileX(Fildes fd, memptr bytes);

    // Set file position of FD to OFF
    // Return success status
    bool SeekFile(Fildes fd, off64_t off);

    // Return current file position on FD
    // There is no provision to return an error code; only the offset is returned
    // (zero on failure?)
    off64_t GetPos(Fildes fd);

    // Truncate file indicated by FD to size SIZE.
    // Return success status
    bool TruncateFile(Fildes fd, off64_t size);

    // Return contents of file as string.
    // File is trimmed at first NULL character
    // Implementation note:
    // linux files in /proc directory lie about their size
    // and are sometimes zero-terminated.
    // File is read using a "safe" method of succesively calling read.
    // relying on reported file size or using mmap does not work in all cases
    const tempstr FileToString(const strptr& fname, algo::FileFlags flags);

    // Same Short version of FileToString, throw exception on error.
    const tempstr FileToString(const strptr& fname);

    // Read all bytes from IN_FD as a string.
    // NUL characters in file will appear in the string --
    // TODO: add option to sanitize string
    // If FD is non-blocking, reading will terminate at first EAGAIN.
    // If an error occurs, all data prior to occurrence of the error is returned.
    const tempstr FdToString(Fildes in_fd) __attribute__((nothrow));

    // Compare STR to contents of FILENAME.
    // If they differ, use a tempfile construction to atomically UPDATE file
    // with contents of STR.
    // NOTE: This will result in a new inode for file!
    // No exceptions are thrown. If the function fails, check errno.
    // Default mode for new file is provided by MODE.
    // If the file is being replaced, MODE is ignored and copied from the old file.
    bool SafeStringToFile(const strptr& str, const strptr& filename, int dfltmode);
    bool SafeStringToFile(const strptr& str, const strptr& filename);

    // Replace contents of file FILENAME with string STR.
    // If CHECK_SAME is specified, first compare contents and do not perform a write
    // if the contents are the same.
    // FLAGS may specify algo_FileFlags_throw, in which case an exception is thrown on error
    bool StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags, bool check_same);

    // Short version of StringToFile: compares file contents before writing,
    // throws exception on error.
    bool StringToFile(const strptr& str, const strptr& filename);

    // Short version of StringToFile: compares file contents before writing.
    bool StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags);
    algo::Fildes OpenWrite(const strptr& filename, algo::FileFlags flags);
    algo::Fildes OpenWrite(const strptr& filename);
    algo::Fildes OpenRead(const strptr& filename, algo::FileFlags flags);
    algo::Fildes OpenRead(const strptr& filename);

    // Write NWRITE bytes starting at START to file descriptor FILDES.
    // Go until all bytes are written on an error occurs.
    // If FILDES is non-blocking, spin indefinitely until bytes do get through.
    // At the end, return success status (TRUE if all bytes written)
    bool WriteFile(algo::Fildes fildes, u8 *start, int nwrite);
    void Dir_curs_Next(Dir_curs &curs) __attribute__((nothrow));

    // Begin scanning files matching shell pattern PATTERN.
    void Dir_curs_Reset(Dir_curs &curs, strptr pattern) __attribute__((nothrow));
    bool Dir_curs_ValidQ(Dir_curs &curs) __attribute__((nothrow));
    algo::DirEntry &Dir_curs_Access(Dir_curs &curs) __attribute__((nothrow));

    // Set blocking mode on file descriptor FD to BLOCKING
    // Return result of FCNTL (0==success)
    int SetBlockingMode(Fildes fildes, bool blocking) __attribute__((nothrow));
}
