// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/line.cpp
//

#include "include/atf_unit.h"

// -----------------------------------------------------------------------------

static int NextFd() {
    int fd=open("/dev/null",O_RDONLY);
    if (fd>=0) {
        close(fd);
    }
    return fd;
}

// -----------------------------------------------------------------------------

static strptr TrimLeadingNewLine(strptr text) {
    if (text.n_elems>1 && text.elems[0]=='\r' && text.elems[1]=='\n') {
        text=RestFrom(text,2);
    } else if (text.n_elems>0 && text.elems[0]=='\n') {
        text=RestFrom(text,1);
    }
    return text;
}

// -----------------------------------------------------------------------------

// Check operation of FileLine_curs
static void CheckFileLine1(strptr filename, strptr text, int nline) {
    int n=0;
    ind_beg(algo::FileLine_curs,line,filename) {
        vrfy_(FirstN(text,line.n_elems)==line);
        text = TrimLeadingNewLine(RestFrom(text,line.n_elems));
        n++;
    }ind_end;
    vrfy_(n==nline);
    vrfy_(text=="");
}

// -----------------------------------------------------------------------------

// Check operation of LineCurs and MmapFile
static void CheckFileLine2(strptr filename, strptr text, int nline) {
    int n=0;
    algo_lib::MmapFile file;
    if (MmapFile_Load(file,filename)) {// pipes cannot be mmapped
        ind_beg(Line_curs,line,file.text) {
            vrfy_(FirstN(text,line.n_elems)==line);
            text = TrimLeadingNewLine(RestFrom(text,line.n_elems));
            n++;
        }ind_end;
        vrfy_(n==nline);
        vrfy_(text=="");
    }
}

// -----------------------------------------------------------------------------

// Check operation of LineCurs and raw string
static void CheckFileLine3(strptr filename, strptr text, int nline) {
    int n=0;
    (void)filename;
    tempstr str(text);
    ind_beg(Line_curs,line,str) {
        vrfy_(FirstN(text,line.n_elems)==line);
        text = TrimLeadingNewLine(RestFrom(text,line.n_elems));
        n++;
    }ind_end;
    vrfy_(n==nline);
    vrfy_(text=="");
}

// -----------------------------------------------------------------------------

// check that InTextFile can handle a pipe providing data slowly
static void CheckFileLine4(strptr text, int nline, bool blocking) {
    //strptr origtext = text;
    int pipefd[2];// read, write
    int rc=pipe(pipefd);
    errno_vrfy(rc!=-1, "pipe");
    int pid=fork();
    // non-blockinb read end
    if (!blocking) {
        (void)algo::SetBlockingMode(algo::Fildes(pipefd[0]),false);
    }
    if (pid==0) {// child
        (void)close(pipefd[0]);// cause eof on the other side
        algo_lib::DieWithParent();
        for (int i=0; i<text.n_elems; i++) {
            ssize_t ret = write(pipefd[1],text.elems+i,1);
            (void)ret;// ignore return value
            usleep(5000);// sleep 5 msec
        }
        (void)close(pipefd[1]);// cause eof on the other side
        (void)_exit(0);
    } else {
        (void)close(pipefd[1]);
        int n=0;
        ind_beg(algo::FileLine_curs,line,algo::Fildes(pipefd[0])) {
            vrfy_(FirstN(text,line.n_elems)==line);
            text = TrimLeadingNewLine(RestFrom(text,line.n_elems));
            n++;
        }ind_end;
        //prlog("pipe test passed, "<<origtext.n_elems<<" chars");
        vrfy_(n==nline);
        vrfy_(text=="");
        int status=0;
        (void)waitpid(pid, &status, 0);
        (void)close(pipefd[0]);
    }
}

// -----------------------------------------------------------------------------

static void CheckFileLine(strptr filename, strptr text, int nline) {
    StringToFile(text,filename);
    static int nextfd = NextFd();
    CheckFileLine1(filename,text,nline);
    CheckFileLine2(filename,text,nline);
    CheckFileLine3(filename,text,nline);
    // pipe test is slow, don't waste minutes on it
    if (text.n_elems < 1000) {
        // blocking
        CheckFileLine4(text,nline,true);
        // nonblocking
        CheckFileLine4(text,nline,false);
    }
    vrfy_(NextFd() == nextfd); // check no fd leaks
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_FileLine_curs() {
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"fileline");
    CheckFileLine(tempfile.filename, "", 0);
    CheckFileLine(tempfile.filename, "a", 1);
    CheckFileLine(tempfile.filename, "a\n", 1);
    CheckFileLine(tempfile.filename, "a\nb\n\nc", 4);
    CheckFileLine(tempfile.filename, "\n\n\n", 3);

    CheckFileLine(tempfile.filename, "\r", 1);// doesn't count as newlnie
    CheckFileLine(tempfile.filename, "\r\n", 1);// doesn't count as newlnie

    {
        // large line (but not too large)
        tempstr str;
        algo::char_PrintNTimes('g',str,7000);
        CheckFileLine(tempfile.filename, str, 1);
    }
    {
        // large line (really large)
        tempstr str;
        algo::char_PrintNTimes('g',str,70000);
        CheckFileLine(tempfile.filename, str, 1);
    }

    {
        algo::LineBuf L;
        int sink=0;
        algo::memptr data = algo::strptr_ToMemptr("asdfasdf\nasdfasdf\nasdfasdf\n\n\n\nasdfasdf\n");
        DO_PERF_TEST("Linebuf",{
                algo::LinebufBegin(L,data,true);
                strptr str;
                int newcount=0;
                while (algo::LinebufNext(L,str)) {
                    newcount+=str.n_elems;
                }
                sink+=newcount;
            });
        prlog(sink);
    }
}
