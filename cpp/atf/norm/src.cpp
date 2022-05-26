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
// Target: atf_norm (exe) -- Normalization tests (see normcheck table)
// Exceptions: yes
// Source: cpp/atf/norm/src.cpp -- source file (cpp/, include/) normalizations including amc
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_norm.h"

// -----------------------------------------------------------------------------

#ifndef __CYGWIN__
static void IndentCPP() {
    command::bash_proc bash;
    bash.cmd.c << "git diff-tree --name-only HEAD -r --no-commit-id cpp include";
    bash.fstdin << "</dev/null";// disable any prompting
    algo_lib::FFildes read;
    ind_beg(algo::FileLine_curs,fname,bash_StartRead(bash,read))  {
        atf_norm::FGitfile *gitfile = atf_norm::ind_gitfile_Find(fname);
        bool noindent = gitfile && gitfile->c_noindent;
        bool ourfile = FindStr(fname,"/gen/") == -1 && FindStr(fname,"extern/") == -1;
        if  (FileQ(fname) && ourfile && !noindent) {
            SysCmd(tempstr()<<"bin/cpp-indent "<<fname<<" >/dev/null 2>&1");
            prlog_("*");
        }
    }ind_end;
    atf_norm::CheckCleanDirs("cpp include");
}
#endif

// indent any source files modified in the last commit
// indentation under CYGWIN is broken -- and we don't have a cross-platform
// solution. so only try it on Linux
void atf_norm::normcheck_indent_srcfile() {
#ifndef __CYGWIN__
    prlog_("indenting ... ");
    IndentCPP();
    prlog(" done");
#endif
}

// -----------------------------------------------------------------------------

// indent all script files modified in the last commit
void atf_norm::normcheck_indent_script() {
    SysCmd("update-scriptfile");
    CheckCleanDirs(SsimFname(atf_norm::_db.cmdline.in, dmmeta_Ssimfile_ssimfile_dev_scriptfile));
    tempstr modfiles(SysEval("git diff-tree --name-only  HEAD -r --no-commit-id",FailokQ(true),1024*1024*10));
    ind_beg(Line_curs,line,modfiles) {
        if (atf_norm::FGitfile *gitfile = ind_gitfile_Find(line)) {
            if (gitfile->c_scriptfile && !gitfile->c_noindent) {
                // indent script files -- there are few of them,
                // so it takes no time to indent them all.
                SysCmd(tempstr()<<"bin/cpp-indent "<<gitfile->gitfile
                       <<" >> temp/atf_norm_indent.log 2>&1",FailokQ(false));
                // eliminate windows line endings from script files
                SysCmd(tempstr()<<"sed -i 's/\\r$//' "<<gitfile->gitfile);
                CheckCleanDirs(gitfile->gitfile);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// update copyright info in source files
void atf_norm::normcheck_copyright() {
    command::src_hdr src_hdr;
    src_hdr.write=true;
    SysCmd(src_hdr_ToCmdline(src_hdr),FailokQ(false));
    CheckCleanDirs("cpp include");
}

// -----------------------------------------------------------------------------

// source code police
void atf_norm::normcheck_src_lim() {
    command::src_lim src_lim;
    src_lim.strayfile=true;
    // #AL# temporarrily disabling line limit check
    // because nobody is using it
    src_lim.linelim=false;
    src_lim.badline.expr="%";
    SysCmd(src_lim_ToCmdline(src_lim), FailokQ(false));
}

// -----------------------------------------------------------------------------

// run amc
void atf_norm::normcheck_amc() {
    command::amc amc;
    amc.report = false;
    SysCmd(amc_ToCmdline(amc),FailokQ(false));
    CheckCleanDirs("include/gen cpp/gen");
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_tempcode() {
    // extra double-quote needed to avoid this check
    // from failing on this file
    int rc=SysCmd("acr targsrc -field:src | xargs -L100 grep -RHn TEMP""CODE");
    if (rc == 0) {
        prerr("SCALPEL LEFT IN PATIENT");
        prerr("It looks like some testing code made its way into the commit.");
        prerr("Please examine the found instances above carefully.");
        prerr("Diallowed code is indicated by the presence of the words TEMP""CODE.");
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

static void GenCheck(strptr dir) {
    ind_beg(algo::Dir_curs,file,DirFileJoin(dir,"*")) {
        int idx=FindStr(file.filename,"_gen.");
        if (idx!=-1) {
            atf_norm::FNs *ns=atf_norm::ind_ns_Find(ch_FirstN(file.filename,idx));
            if (!ns) {
                prlog("# success:N file doesn't appear to be generated by amc (to fix: pipe to  |grep ^acr_ed|sh)");
                prlog("acr_ed -del -srcfile:"<<file.pathname<<" -write");
                algo_lib::_db.exit_code=1;
            }
        }
    }ind_end;
}

void atf_norm::normcheck_stray_gen() {
    GenCheck("include/gen");
    GenCheck("cpp/gen");
}

// -----------------------------------------------------------------------------

static void BuildWith(strptr compiler) {
    if (SysEval(tempstr() << compiler << " --version",FailokQ(true),1024*10) != "") {
        prlog("----- building everything with "<<compiler<<"  cfg:release -----");
        command::abt_proc abt;
        abt.cmd.compiler = compiler;
        abt.cmd.cfg = dev_Cfg_cfg_release;
        abt.cmd.target.expr = "%";
        abt_ExecX(abt);
    } else {
        prlog("# "<<compiler<<" not installed, skipping build");
    }
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_build_clang() {
    (void)BuildWith;
    //BuildWith(dev_Compiler_compiler_clangPP);
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_build_gcc9() {
    //BuildWith(dev_Compiler_compiler_gPP_9);
}

// -----------------------------------------------------------------------------

static bool BadCharQ(unsigned char c) {
    return c >= 0x80
        && c != 0x80 // Windows-1252 EUR symbol
        && c != 0xA3 // Windows-1252 GBP symbol
        && c != 0xA4 // Windows-1252 currency sign
        && c != 0xA5 // Windows-1252 JPY symbol
        ;
}

static bool HasBadCharQ(strptr s) {
    bool ret=false;
    for (int i=0; i<s.n_elems; i++) {
        if (BadCharQ(s[i])) {
            ret=true;
            break;
        }
    }
    return ret;
}

void atf_norm::normcheck_encoding() {
    ind_beg(_db_gitfile_curs, gitfile, _db) {
        const strptr ext = GetFileExt(gitfile.gitfile);
        if (ext == ".cpp" || ext == ".h") {
            ind_beg(algo::FileLine_curs, line, gitfile.gitfile) {
                if (HasBadCharQ(line)) {
                    prlog(gitfile.gitfile <<":" << ind_curs(line).i+1 << ": bad char in line: " << line);
                    algo_lib::_db.exit_code = 1;
                }
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_norm::normcheck_iffy_src() {
    command::src_func src_func;
    src_func.iffy = true;
    src_func.listfunc = true;
    src_func.proto = true;
    src_func.report = false;
    cstring output(Trimmed(SysEval(src_func_ToCmdline(src_func),FailokQ(false),1024*1024)));
    if (output != "") {
        prlog(output);
        prerr("Please fix above instances of iffy code and retry");
    }
}
