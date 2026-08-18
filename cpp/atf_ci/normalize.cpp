// Copyright (C) 2026 AlgoRND
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
// Target: atf_ci (exe) -- Normalization tests (see citest table)
// Exceptions: yes
// Source: cpp/atf_ci/normalize.cpp
//

#include "include/algo.h"
#include "include/atf_ci.h"

// -----------------------------------------------------------------------------

void atf_ci::citest_checkclean() {
    // do nothing - atf_ci will check clean dirs
    // after this test
}

// -----------------------------------------------------------------------------

// Delete files that haven't been accessed in the last couple days
void atf_ci::citest_cleantemp() {
    algo::UnTime thresh = algo::CurrUnTime() - algo::UnDiffHMS(48,0,0);
    ind_beg(algo::Dir_curs,entry,"temp/*") if (!StartsWithQ(entry.filename, ".")) {
        algo::UnTime atime = entry.is_dir ? DirAtime(entry.pathname) : FileAtime(entry.pathname);
        // empty dir will yield atime = 0, don't delete it
        if (atime.value && atime < thresh){
            prlog("cleanup: "<<entry.pathname);
            if (entry.is_dir) {
                RemDirRecurse(entry.pathname,true);
            } else {
                DeleteFile(entry.pathname);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_ci::citest_gitfile() {
    SysCmd("bin/update-gitfile >/dev/null",FailokQ(false));
    ind_beg(algo::FileLine_curs,line,SsimFname(atf_ci::_db.cmdline.in,dmmeta_Ssimfile_ssimfile_dev_gitfile)) {
        dev::Gitfile gitfile;
        if (Gitfile_ReadStrptrMaybe(gitfile,line)) {
            if (!FileObjectExistsQ(gitfile.gitfile)) {
                prlog("atf_ci.missing_file"
                      <<Keyval("success","N")
                      <<Keyval("gitfile",gitfile.gitfile)
                      <<Keyval("comment","File missing from filesystem"));
                algo_lib::_db.exit_code=1;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_ci::citest_scanreadme() {
    // update list of readmes
    cstring out;
    ind_beg(_db_gitfile_curs,gitfile,_db) if (StartsWithQ(gitfile.gitfile,"txt/")) {
        // print just the pkey so that other attrs don't get overwritten
        out << "dev.readmefile gitfile:"<<gitfile.gitfile<<eol;
    }ind_end;
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"readme");
    StringToFile(out,tempfile.filename);
    command::acr_proc acr;
    acr.cmd.write   = true;
    acr.cmd.merge   = true;
    acr.fstdin << "<"<<tempfile.filename;
    acr.cmd.print   = true;
    acr.cmd.report  = false;
    acr_ExecX(acr);
}

// -----------------------------------------------------------------------------

void atf_ci::citest_quickreadme() {
    command::abt_md_proc abt_md;
    abt_md.cmd.evalcmd=false;
    abt_md_ExecX(abt_md);
}

// -----------------------------------------------------------------------------

void atf_ci::citest_ssimfile() {
    ind_beg(algo::Dir_curs,dir,"data/*") {
        ind_beg(algo::Dir_curs,file,tempstr()<<dir.pathname<<"/*.ssim") {
            tempstr ssimfile(Pathcomp(file.pathname,"/LR.RL"));// data/acmdb/device.ssim -> acmdb/device
            Replace(ssimfile,"/",".");
            vrfy(atf_ci::ind_ssimfile_Find(ssimfile)
                 ,tempstr()<<"atf_ci.stray_ssimfile"
                 <<Keyval("fname",file.pathname)
                 <<Keyval("ssimfile",ssimfile)
                 <<Keyval("comment","No ssimfile entry exists for this file"));
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_ci::citest_normalize_acr() {
    command::acr_proc acr;
    acr.cmd.check = true;
    acr.cmd.x     = true;
    acr.cmd.write = true;
    acr.cmd.query = "%";
    acr.cmd.print = false;
    acr.cmd.report = false;
    acr_ExecX(acr);
}

// -----------------------------------------------------------------------------

// source code police
void atf_ci::citest_src_lim() {
    command::src_lim src_lim;
    src_lim.strayfile=true;
    // #AL# disabling line limit check
    // because nobody is using it
    src_lim.linelim=false;
    src_lim.badline.expr="%";
    SysCmd(src_lim_ToCmdline(src_lim), FailokQ(false));
}

// -----------------------------------------------------------------------------

// run amc
void atf_ci::citest_amc() {
    command::amc amc;
    amc.report = false;
    SysCmd(amc_ToCmdline(amc),FailokQ(false));
}

// -----------------------------------------------------------------------------

static void InsertCopyright(cstring &out) {
    cstring text = FileToString(atf_ci::dev_gitfile_conf_copyright_txt);
    ind_beg(Line_curs,line,text) {
        out<<"#";
        if (ch_N(line)) {
            out<<" "<<line;
        }
        out<<eol;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Create a bootstrap file for each build dir
void atf_ci::citest_bootstrap() {
    // Check that every entry in the bin/bootstrap directory is a valid 'builddir'
    ind_beg(algo::Dir_curs,entry,"bin/bootstrap/*") {
        vrfy(ind_builddir_Find(entry.filename)
             , tempstr()<<"Missing entry in dev.builddir table for file "
             << entry.pathname<<" in bin/bootstrap");
    }ind_end;

    ind_beg(atf_ci::_db_builddir_curs,builddir,atf_ci::_db) {
        // bootstrap exists only for release
        if (cfg_Get(builddir) == dev_Cfg_cfg_release) {
            // abt build directory
            tempstr outdir;
            outdir << "build/" << builddir.builddir;
            CreateDirRecurse(outdir);

            // bootstrap filename
            cstring bsfile;
            bsfile<<"bin/bootstrap/"<<builddir.builddir;

            // temporary location
            algo_lib::FTempfile tempfile;
            TempfileInitX(tempfile,"bootstrap");

            // invoke abt in bootstrap mode
            cstring text;
            algo_lib::Replscope R;
            Ins(&R, text, "#!/usr/bin/env bash");
            InsertCopyright(text);
            Ins(&R, text, "echo '# this script has been created using atf_ci bootstrap'");
            Ins(&R, text, "echo '# now building an abt executable which will build the rest'");
            Ins(&R, text, "mkdir -p build");
            Ins(&R, text, "set -e");
            Ins(&R, text, "if [ ! -f .ffroot ]; then echo 'Missing .ffroot. Wrong directory?'; exit 1; fi");

            // create soft links that look like this
            // lrwxr-xr-x    1 alexei  staff    29 May 16 11:02 release -> Darwin-clang++.release-x86_64
            // lrwxr-xr-x    1 alexei  staff    27 May 16 11:02 debug -> Darwin-clang++.debug-x86_64
            ind_beg(atf_ci::_db_cfg_curs,cfg,atf_ci::_db) if (cfg.cfg != "") {
                Set(R,"$thiscfg",cfg.cfg);
                Set(R,"$thisdir",dev::Builddir_Concat_uname_compiler_cfg_arch(uname_Get(builddir)
                                                                              ,compiler_Get(builddir)
                                                                              ,cfg.cfg
                                                                              ,arch_Get(builddir)));
                Ins(&R, text, "");
                Ins(&R, text, "echo '# setting up soft link build/$thiscfg (default)'");
                Ins(&R, text, "rm -f build/$thiscfg");
                Ins(&R, text, "mkdir -p build/$thisdir # create target directory");
                Ins(&R, text, "ln -s $thisdir build/$thiscfg # and a soft link to it");
            }ind_end;

            command::abt_proc abt;
            abt.cmd.uname       = uname_Get(builddir);
            abt.cmd.compiler    = compiler_Get(builddir);
            abt.cmd.arch        = arch_Get(builddir);
            abt.cmd.cfg.expr    = cfg_Get(builddir);
            abt.cmd.install     = true;
            abt.cmd.shortlink   = false;// use explicit soft links
            abt.cmd.target.expr = tempstr()<<dmmeta_Ns_ns_abt
                                           <<"|"<<dmmeta_Ns_ns_src_hdr
                                           <<"|"<<dmmeta_Ns_ns_gcache
                                           <<"|"<<dmmeta_Ns_ns_src_func;
            abt.cmd.printcmd    = true;
            abt.cmd.report      = false;// stay quiet
            abt.cmd.build       = false;
            algo::StringToFile(text,tempfile.filename);
            abt.fstdout << ">>" << tempfile.filename;// append
            abt_ExecX(abt);

            // atomically move new bootstrap file into final location
            verblog("# create "<<bsfile);
            int rc=rename(Zeroterm(tempfile.filename),Zeroterm(bsfile));
            if (rc!=0) {
                algo_lib::_db.exit_code=1;
                prlog("atf_ci.bootstrap"
                      <<Keyval("error",errno)
                      <<Keyval("comment","rename failed"));
            }
            (void)chmod(Zeroterm(bsfile),0755);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_ci::citest_shebang() {
    ind_beg(_db_scriptfile_curs,scriptfile,_db) {
        ind_beg(algo::FileLine_curs,line,scriptfile.gitfile) {
            if (StartsWithQ(line,"#!")) {
                strptr interpreter = Pathcomp(line,"!LR LL");
                if (interpreter != "/bin/sh" && interpreter != "/usr/bin/env") {
                    prerr(scriptfile.gitfile<<":1: Non-portable interpreter "<<interpreter<<" use /usr/bin/env");
                    algo_lib::_db.exit_code=1;
                }
            }
            break;// first line only
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Validate that a string contains only valid UTF-8 sequences
// Uses algo::Utf8SeqLen for multibyte validation
static bool IsValidUTF8(strptr s) {
    for (int i = 0; i < s.n_elems; ) {
        u8 c = s[i];
        if (c < 0x80) {
            // ASCII - single byte
            i++;
        } else {
            int seq_len = algo::Utf8SeqLen(s, i);
            if (seq_len == 0) {
                return false; // Invalid UTF-8 sequence
            }
            i += seq_len;
        }
    }
    return true;
}

void atf_ci::citest_encoding() {
    ind_beg(_db_gitfile_curs, gitfile, _db) {
        const strptr ext = GetFileExt(gitfile.gitfile);
        if (ext == ".cpp" || ext == ".cc" || ext == ".h") {
            ind_beg(algo::FileLine_curs, line, gitfile.gitfile) {
                if (!IsValidUTF8(line)) {
                    prlog(gitfile.gitfile <<":" << ind_curs(line).i+1 << ": invalid UTF-8 sequence in line: " << line);
                    algo_lib::_db.exit_code = 1;
                }
            }ind_end;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// update file headers
void atf_ci::citest_file_header() {
    command::src_hdr src_hdr;
    src_hdr.write=true;
    SysCmd(src_hdr_ToCmdline(src_hdr),FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_ci::citest_non_copyrighted() {
    SysCmd("bin/src_hdr -update_copyright -scriptfile bin/%");
    tempstr bad;
    ind_beg(atf_ci::_db_scriptfile_curs, sf, atf_ci::_db) {
        tempstr contents(algo::FileToString(sf.gitfile));
        if (algo::FindStr(contents,"Copyright") < 0) {
            bad << sf.gitfile << eol;
        }
    }ind_end;
    ind_beg(atf_ci::_db_targsrc_curs, ts, atf_ci::_db) {
        algo::Smallstr200 src(src_Get(ts));
        if (!algo::StartsWithQ(src,"extern")
            && !algo::StartsWithQ(src,"cpp/gen")
            && !algo::StartsWithQ(src,"include/gen")) {
            tempstr contents(algo::FileToString(src));
            if (algo::FindStr(contents,"Copyright") < 0) {
                bad << src << eol;
            }
        }
    }ind_end;
    if (bad != "") {
        vrfy(0,tempstr()<<"Each C++ file, and each non-trivial script file shall be copyrighted."
             " Put copyright notice to the following files:\n"<<bad);
    }
}

// -----------------------------------------------------------------------------

void atf_ci::citest_iffy_src() {
    command::src_func src_func;
    src_func.func.expr = "%.%";
    src_func.iffy = true;
    src_func.baddecl = true;// gripe about declarations that src_func doesn't like
    src_func.list = true;
    src_func.report = false;
    cstring output(Trimmed(SysEval(src_func_ToCmdline(src_func),FailokQ(false),1024*1024)));
    if (output != "") {
        prlog(output);
        vrfy(0,"Please fix above instances and retry");
    }
}

// -----------------------------------------------------------------------------

static void GenCheck(strptr dir) {
    ind_beg(algo::Dir_curs,file,DirFileJoin(dir,"*")) {
        int idx=FindStr(file.filename,"_gen.");
        if (idx!=-1) {
            atf_ci::FNs *ns=atf_ci::ind_ns_Find(ch_FirstN(file.filename,idx));
            if (!ns) {
                prlog("# success:N file doesn't appear to be generated by amc (to fix: pipe to  |grep ^acr_ed|sh)");
                prlog("acr_ed -del -srcfile:"<<file.pathname<<" -write");
                algo_lib::_db.exit_code=1;
            }
        }
    }ind_end;
}

void atf_ci::citest_stray_gen() {
    GenCheck("include/gen");
    GenCheck("cpp/gen");
}

// -----------------------------------------------------------------------------

void atf_ci::citest_tempcode() {
    // extra double-quote needed to avoid this check
    // from failing on this file
    int rc=SysCmd("acr dev.targsrc -field:src | xargs -L100 grep -RHn TEMP""CODE");
    if (rc == 0) {
        prerr("SCALPEL LEFT IN PATIENT");
        prerr("It looks like some testing code made its way into the commit.");
        prerr("Please examine the found instances above carefully.");
        prerr("Diallowed code is indicated by the presence of the words TEMP""CODE.");
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

void atf_ci::citest_lineendings() {
    algo_lib::Regx regx;
    Regx_ReadSql(regx, "txt/%",true);
    cstring files(SysEval("git ls-files",FailokQ(true),1024*1024*100));
    ind_beg(Line_curs,fname,files) {
        if (Regx_Match(regx,fname) && FileQ(fname)) {
            SysCmd(tempstr() << "sed -i 's/\\r$//;s/\\r/\\n/g' "<<fname);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_ci::citest_update_script() {
    tempstr newfiles;
    ind_beg(algo::Dir_curs,entry,"bin/*") if (ind_gitfile_Find(entry.pathname)) {
        struct stat buf;
        int rc=lstat(Zeroterm(entry.pathname), &buf);
        bool script=false;
        if (rc==0 && S_ISREG(buf.st_mode) && (buf.st_mode & S_IXUSR)!=0) {
            ind_beg(algo::FileLine_curs,line,entry.pathname) {
                if (StartsWithQ(line,"#!/")) {
                    script=true;
                }
                break;// first line only
            }ind_end;
        }
        if (script) {
            if (!ind_scriptfile_Find(entry.pathname)) {
                dev::Scriptfile out;
                out.gitfile=entry.pathname;
                newfiles<<out<<eol;
            }
            tempstr doc=tempstr()<<"txt/script/"<<entry.filename<<".md";
            if (!algo::FileQ(doc)) {
                StringToFile("",doc);
                SysCmd(tempstr()<<"git add "<<doc);
            }
            dev::Readmefile out;
            out.gitfile=doc;
            newfiles<<out<<eol;
        }
    }ind_end;
    algo::strptr txnfile("temp/update-script");
    StringToFile(newfiles,txnfile);
    command::acr_proc acr;
    acr.cmd.insert=true;
    acr.cmd.write=true;
    acr.cmd.report=false;
    acr.fstdin<<"<"<<txnfile;
    acr_Exec(acr);
}

// -----------------------------------------------------------------------------

// indent all script files modified in the last commit
void atf_ci::citest_indent_script() {
    tempstr modfiles(SysEval("git diff-tree --name-only  HEAD -r --no-commit-id",FailokQ(true),1024*1024*10));
    ind_beg(Line_curs,line,modfiles) {
        if (atf_ci::FGitfile *gitfile = ind_gitfile_Find(line)) {
            if (gitfile->c_scriptfile && !gitfile->c_noindent) {
                // indent script files -- there are few of them,
                // so it takes no time to indent them all.
                SysCmd(tempstr()<<"bin/cpp-indent "<<gitfile->gitfile
                       <<" >> temp/atf_ci_indent.log 2>&1",FailokQ(false));
                // eliminate windows line endings from script files
                SysCmd(tempstr()<<"sed -i 's/\\r$//' "<<gitfile->gitfile);
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Cppcheck(strptr uname, strptr compiler, strptr platform) {
    cstring builddir = tempstr()<<"temp/cppcheck."<<uname<<"."<<compiler;
    cstring project  = tempstr()<<builddir<<"/project.json";
    CreateDirRecurse(builddir);

    // create json compile database
    command::abt_proc abt;
    Regx_ReadSql(abt.cmd.target,"%",true);
    abt.cmd.uname       = uname;
    abt.cmd.cfg.expr    = dev_Cfg_cfg_release;
    abt.cmd.compiler    = compiler;
    abt.cmd.arch        = "x86_64";
    abt.cmd.jcdb        = project;
    abt_ExecX(abt);

    // run cppcheck
    // interesting results with addons, but execution time greatly increases
    SysCmd(tempstr()
           << "cppcheck --error-exitcode=1"
           << " --quiet" // suppress too verbose progress
           //<< " --addon=cert"
           //<< " --addon=threadsafety"
           << " --std=c++03"
           << " --platform="<<platform
           << " --project="<<project
           << " -i extern" // ignore third party code
           << " --cppcheck-build-dir="<<builddir
           << " --suppressions-list=test/cppcheck-suppressions.txt"
           ,FailokQ(false));
}

// Run static code analyzer
// Check Linux only
void atf_ci::citest_cppcheck() {
    Cppcheck("Linux","g++","unix64");
}

// -----------------------------------------------------------------------------

#ifndef __CYGWIN__
// Answer whether every line of a header's body is text that update-hdr
// produced.  A target header such as include/atf_comp.h declares one namespace
// per group of sources and marks it `// update-hdr`; src_hdr fills everything
// inside those braces by scanning those sources, so the text there is the
// generator's own output, and indenting it can only reproduce what the
// generator already emits.  That reproduction is not free: the largest of
// these headers dominate the indent pass, so a commit that touches many
// sources at once spends the citest's whole time budget re-deriving generated
// formatting.
//
// The marker on a namespace line says nothing about the rest of the file,
// though, and a header that carries one marked namespace can still hold
// hand-written text outside it: include/atf_snf.h declares three packed
// structs above its marked namespace, and include/atf_unit.h a multi-line
// macro.  Skipping such a file would drop its hand-written part out of the
// indent pass with nothing to report that it had gone unchecked.  So the
// answer is yes only when the file is a header, it opens at least one marked
// namespace, no namespace lacks the marker, every marked namespace reaches
// its closing brace, and every line outside those namespaces is blank, a
// `//` comment, or a preprocessor directive.
//
// The walk reads a section off the trimmed line: a line beginning with
// `namespace` and carrying `// update-hdr` opens one, the next line beginning
// with `}` closes it, and a section still open at EOF is one update-hdr
// reports and refuses to rewrite, so its body is not generated text and the
// answer is no.  Outside a section only the three recognized shapes are
// passed over; every other line counts as hand-written, which is what keeps a
// file the walk does not understand in the indent pass -- an unmarked or
// commented-out namespace, a declaration sharing its line with a block
// comment, a declaration at file scope.  A `/* ... */` block outside a
// section counts as hand-written too, which costs the file the skip but never
// the check.
//
// That reading is simpler than update-hdr's own, which tracks comment and
// literal state across lines and locates a section's close by brace depth.
// Where the two disagree this walk is the permissive one: a marker line
// quoted inside a block comment opens a section here and does not there, so
// the lines it swallows are counted as generated and a file holding them is
// skipped although update-hdr leaves them hand-written.  The two readings
// agree on the plain `namespace X { // update-hdr` form that the headers in
// this tree are written with, and what a disagreement costs is one header's
// hand-written part going unindented -- a formatting pass, not a wrong
// header -- which is the price of deciding the skip from a second parser
// rather than from update-hdr itself.
static bool GeneratedHdrQ(strptr fname) {
    int ngen = 0;
    int nhand = 0;
    bool inblock = false;
    if (EndsWithQ(fname, ".h")) {
        ind_beg(algo::FileLine_curs,line,fname) {
            strptr text = Trimmed(line);
            if (!inblock && StartsWithQ(text, "namespace")) {
                inblock = FindStr(text, "// update-hdr") != -1;
                if (inblock) {
                    ngen++;
                } else {
                    nhand++;
                }
            } else if (inblock) {
                inblock = !StartsWithQ(text, "}");
            } else if (ch_N(text) > 0
                       && !StartsWithQ(text, "//")
                       && !StartsWithQ(text, "#")) {
                nhand++;
            }
        }ind_end;
    }
    return ngen > 0 && nhand == 0 && !inblock;
}

static void IndentCPP() {
    algo_lib::FProc proc;
    ary_Alloc(proc.args) = "git";
    ary_Alloc(proc.args) = "diff-tree";
    ary_Alloc(proc.args) = "--name-only";
    ary_Alloc(proc.args) = "HEAD";
    ary_Alloc(proc.args) = "-r";
    ary_Alloc(proc.args) = "--no-commit-id";
    ary_Alloc(proc.args) = "cpp";
    ary_Alloc(proc.args) = "include";
    proc.fstdin = "</dev/null";// disable any prompting
    proc.fstdout = "|";
    algo_lib::ProcStart(proc);
    ind_beg(algo::FileLine_curs,fname,proc.from_stdout)  {
        atf_ci::FGitfile *gitfile = atf_ci::ind_gitfile_Find(fname);
        bool noindent = gitfile && gitfile->c_noindent;
        bool ourfile = FindStr(fname,"/gen/") == -1 && FindStr(fname,"extern/") == -1;
        if  (FileQ(fname) && ourfile && !noindent && !GeneratedHdrQ(fname)) {
            // the citest's verdict is the set of files the pass modified, and
            // an indenter that never ran modifies nothing, which reads exactly
            // like a tree that was already indented.  bin/cpp-indent runs the
            // indentation through emacs, and exits nonzero when emacs is
            // missing or its elisp fails, so that status is the only evidence
            // the file was checked at all: the run stops on it instead of
            // reporting a pass it did not compute.  The output goes to the log
            // the script pass writes, out of the citest's own output.
            SysCmd(tempstr()<<"bin/cpp-indent "<<fname
                   <<" >> temp/atf_ci_indent.log 2>&1",FailokQ(false));
            prlog_("*");
        }
    }ind_end;
}
#endif

// indent any source files modified in the last commit
// indentation under CYGWIN is broken -- and we don't have a cross-platform
// solution. so only try it on Linux
void atf_ci::citest_indent_srcfile() {
#ifndef __CYGWIN__
    prlog_("indenting ... ");
    IndentCPP();
    prlog(" done");
#endif
}

// -----------------------------------------------------------------------------

void atf_ci::citest_readme() {
    command::abt_md_proc abt_md;
    abt_md_ExecX(abt_md);
}

// -----------------------------------------------------------------------------

void atf_ci::citest_normalize_amc_vis() {
    command::amc_vis amc_vis;
    amc_vis.ctype.expr = "%";
    amc_vis.check      = true;
    SysCmd(amc_vis_ToCmdline(amc_vis),FailokQ(false));
}

// -----------------------------------------------------------------------------

void atf_ci::citest_normalize_acr_my() {
#if defined(__CYGWIN__)
    prlog("cygwin doesn't have a working mariadb install. skipping acr_my test");
#else
    command::acr_my_proc acr_my;
    acr_my.cmd.abort = true;
    acr_my_ExecX(acr_my); //return to known state
    command::acr_my_proc acr_my2;
    acr_my2.cmd.start = true;
    acr_my2.cmd.stop  = true;
    acr_my2.cmd.nsdb.expr   = "%";
    acr_my_ExecX(acr_my2);//# round trip all data through mysql
#endif
}

// -----------------------------------------------------------------------------

void atf_ci::citest_apm_check() {
    command::apm_proc apm;
    apm.cmd.check=true;
    apm_ExecX(apm);
}

// TRUE when LINE carries a word REGX matches.
//
// The test is per word rather than per line because the words that must not
// appear are prefixes -- a namespace, a tool -- and a substring search on a
// prefix answers yes to any word that merely contains it: `fix2` and the hex
// `0x28` both hold "x2" without being it.  A word here is what a name is made
// of, letters, digits and underscore, and every other character ends one.
static bool MentionQ(algo_lib::Regx &regx, strptr line) {
    bool ret = false;
    int beg = 0;
    for (int i = 0; i <= line.n_elems; i++) {
        char c = i < line.n_elems ? line[i] : ' ';
        bool wordchar = (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_';
        if (!wordchar) {
            if (i > beg && Regx_Match(regx, strptr(line.elems + beg, i - beg))) {
                ret = true;
            }
            beg = i + 1;
        }
    }
    return ret;
}

// Check that no package which forbids a word carries one.
//
// A package published downstream is read by people who have never seen the tree
// it was published from, so a name belonging to that tree -- a namespace, a
// tool, a host path -- is at best noise and at worst a dangling reference to
// something they cannot look up.  Which words a package forbids is the
// package's own statement, in dev.package.nomention, because the upstream tree
// is the only one that knows what it is called; a downstream repository cannot
// be asked to know about the trees that publish into it, and there may be
// several.
//
// The package's evaluation is what gets checked, records and files alike, since
// that is exactly what a push carries.  A row that must name a forbidden word
// to do its job -- an exclusion naming the very namespace it excludes -- is
// kept out of the package for that reason, so its absence here is the check
// working rather than a hole in it.  The package's own dev.package row is the
// one exception the check has to make for itself: it carries the list of
// forbidden words, so it names every one of them by construction.
void atf_ci::citest_apm_nodownstream() {
    ind_beg(atf_ci::_db_package_curs,package,atf_ci::_db) if (ch_N(package.nomention)) {
        algo_lib::Regx regx;
        vrfy(Regx_ReadStrptrMaybe(regx, package.nomention), tempstr()<<"atf_ci.bad_nomention"
             <<Keyval("package",package.package)
             <<Keyval("nomention",package.nomention));
        int nbad = 0;
        int nline = 0;
        // apm takes one action per run, so the records and the files are two
        for (int showfile = 0; showfile <= 1; showfile++) {
            command::apm_proc apm;
            apm.cmd.package.expr = package.package;
            apm.cmd.l            = true;
            apm.cmd.showrec      = showfile == 0;
            apm.cmd.showfile     = showfile == 1;
            apm.fstdout          = "|";
            apm_Start(apm);
            tempstr self;
            self << "dev.package  package:" << package.package << "  ";
            ind_beg(algo::FileLine_curs,line,apm.from_stdout) {
                nline++;
                if (!StartsWithQ(line, self) && MentionQ(regx, line) && nbad < 10) {
                    prerr("atf_ci.package_mentions"
                          <<Keyval("package",package.package)
                          <<Keyval("nomention",package.nomention)
                          <<Keyval("record",line));
                    nbad++;
                }
            }ind_end;
            apm_Wait(apm);
            vrfy(apm.status == 0, tempstr()<<"atf_ci.package_eval"
                 <<Keyval("package",package.package)
                 <<Keyval("status",algo::DescribeWaitStatus(apm.status))
                 <<Keyval("comment","the package could not be evaluated"));
        }
        // an evaluation that produced nothing would pass this check whatever
        // the package carried, so the emptiness is the failure, not the pass
        vrfy(nline > 0, tempstr()<<"atf_ci.package_empty"
             <<Keyval("package",package.package)
             <<Keyval("comment","the package evaluated to no records and no files"));
        if (nbad > 0) {
            prerr("atf_ci.package_nomention"
                  <<Keyval("package",package.package)
                  <<Keyval("comment","the package carries words it forbids; exclude the keys above from it"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

void atf_ci::citest_apm_gen() {
    ind_beg(atf_ci::_db_pkggen_curs, pkggen, atf_ci::_db) {
        command::apm_proc apm;
        apm.cmd.package.expr = pkggen.package;
        apm.cmd.generate     = true;
        apm_ExecX(apm);
    }ind_end;
}
