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
// Source: cpp/lib/algo/line.cpp -- Line processing
//

#include "include/algo.h"

// -----------------------------------------------------------------------------

// Accept data block DATA as input for the line buffer
// LinebufNext will attempt to return pointers to a subregion of DATA until
// it returns false, so this block must be valid until then.
// If IMPLIED_EOF is set, then this is the last call to LinebufBegin.
// LinebufNext will know to return a partial line (with no \n) and set the
// EOF flag
void algo::LinebufBegin(LineBuf &L, memptr data, bool implied_eof) {
    L.incoming    = ToStrPtr(data);
    L.implied_eof = implied_eof;
    L.eof = false;
}

// -----------------------------------------------------------------------------

// Scan memory provided by LinebufBegin for line separator (hard-coded to be '\n')
// If the function returns FALSE, and there is a partial line remaining,
// it is saved to an internal buffer in LineBuf (called BUF).
// It will be subsequently recombined with incoming data to form a contiguous line,
// but copying memory is avoided whenever possible.
// Usage:
// LinebufBegin(linebuf,data,true);
// for (strptr line; LinebufNext(linebuf,line); ) {
//     <process line>
// }
bool algo::LinebufNext(LineBuf &L, strptr &result) {
    // beg,end indicate location of eol inside the incoming string
    // search for \n, or to end of incoming string
    char *incoming=L.incoming.elems;
    int beg=0,end=L.incoming.n_elems;
    for (; beg<end; beg++) {
        if (L.incoming.elems[beg]=='\n') {
            end=beg+1;
            break;
        }
    }
    // line is returned when \n is found, or when a non-empty partial line is found at the end
    bool ret = (end>beg) || (L.implied_eof && bool_Update(L.eof,true) && (beg + L.buf_n>0));
    if (!ret || L.buf_n) {
        buf_Addary(L, strptr(incoming,beg));
    }
    if (ret) {
        result = L.buf_n ? strptr(L.buf_elems, L.buf_n) : strptr(incoming,beg);
        result.n_elems -= (end>beg) && result.n_elems && result.elems[result.n_elems-1] == '\r';
        L.buf_n=0;
    }
    L.incoming = strptr(incoming+end,L.incoming.n_elems-end); // skip processed bytes
    return ret;
}

// -----------------------------------------------------------------------------

// Read characters up to newline or end of file.  Return
// resulting substring, skipping the newline
// (which is either \n or \r\n).
strptr algo::GetLine(StringIter &S) {
    strptr ret(S.Rest());
    for (int i=0; i<ret.n_elems; i++) {
        S.index++;// skip character in S
        if (ret[i]=='\n') {
            ret.n_elems = i-(i>0 && ret[i-1]=='\r');
            break;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Line_curs uses strptr to hold on to the string being scanned
// (to avoid copying a potentially huge string)
// The use of "strptr&" prevents passing a temporary.
// Line_curs handling of newlines is identical to that of FileLine_curs
void algo::Line_curs_Reset(Line_curs &curs, algo::strptr &text) {
    curs.contents.expr = text;
    curs.contents.index = 0;
    curs.i = -1; // line number
    Line_curs_Next(curs);
}

// -----------------------------------------------------------------------------

void algo::Line_curs_Reset(Line_curs &curs, algo::cstring &text) {
    strptr temps(text);
    Line_curs_Reset(curs,temps);
}

// -----------------------------------------------------------------------------

// Scan for next line.
void algo::Line_curs_Next(Line_curs &curs) {
    curs.eof=curs.contents.EofQ();
    curs.line=GetLine(curs.contents);
    curs.i++;
}

// -----------------------------------------------------------------------------

// Read next line from FILE into RESULT
// When end of file is reached, return FALSE.
// If error occurs during reading, return false.
// Typical usage:
// InTextFile file;
// file.file = OpenRead(filename);
// for (strptr line; ReadLine(file,line); ) {
//     <do something with LINE>
// }
// A partial line at the end of the file is returned.
bool algo_lib::ReadLine(algo_lib::InTextFile &file, algo::strptr &result) {
    // attempt to grab a line from in-memory buffer
    bool ret = LinebufNext(file.line_buf,result);
    while (!ret && !file.line_buf.eof) {
        ssize_t len = read(file.file.fd.value, file.temp_buf_data, ssizeof(file.temp_buf_data));
        // pipes can return a few bytes at a time, without filling a whole line.
        // keep reading until we get a full line, or end of file is detected
        // if we were passed a non-blocking file descriptor, keep going. it wastes CPU,
        // but that's not our problem, it's the caller's problem.
        if (len >= 0) {
            bool implied_eof = len==0;
            LinebufBegin(file.line_buf, memptr(file.temp_buf_data,len), implied_eof);
            ret = LinebufNext(file.line_buf, result);
        } else if (errno != EAGAIN) {
            ret = false;
            break;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void _FileLine_curs_Reset(algo::FileLine_curs &curs, algo::Fildes fd, bool own) {
    curs.file.file.fd = fd;
    curs.file.own_fd = own;
    curs.line = strptr();
    curs.i=0;// first line is zero, buyer beware
    curs.eof.value = !ReadLine(curs.file, curs.line);
}

// -----------------------------------------------------------------------------

void algo::FileLine_curs_Reset(algo::FileLine_curs &curs, strptr filename) {
    _FileLine_curs_Reset(curs,OpenRead(filename),true);
}

// -----------------------------------------------------------------------------

void algo::FileLine_curs_Reset(algo::FileLine_curs &curs, algo::Fildes fd) {
    _FileLine_curs_Reset(curs,fd,false);
}

// -----------------------------------------------------------------------------

void algo::FileLine_curs_Next(algo::FileLine_curs &curs) {
    curs.eof.value = !ReadLine(curs.file, curs.line);
    curs.i++;
}
