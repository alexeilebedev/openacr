// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

// Regenerate dev.gitfile from the git index, then check each tracked file against the
// filesystem and against the dev.gitpath patterns.
//
// The patterns state where each kind of file lives: cpp/%.cpp, include/%.h, bin/%.  A
// script committed as cpp/ctxledger/run.sh matches none of them, so it is reported and
// the citest fails.  A pattern may match no file at all: the openacr package carries
// every pattern into a tree that holds only part of this one.
void atf_ci::citest_gitfile() {
    SysCmd("bin/update-gitfile >/dev/null",FailokQ(false));
    ind_beg(atf_ci::_db_gitpath_curs,gitpath,atf_ci::_db) {
        Regx_ReadSql(gitpath.regx,gitpath.gitpath,true);
    }ind_end;
    ind_beg(algo::FileLine_curs,line,SsimFname(atf_ci::_db.cmdline.in,dmmeta_Ssimfile_ssimfile_dev_gitfile)) {
        dev::Gitfile gitfile;
        if (Gitfile_ReadStrptrMaybe(gitfile,line)) {
            bool placed=false;
            ind_beg(atf_ci::_db_gitpath_curs,gitpath,atf_ci::_db) {
                if (Regx_Match(gitpath.regx,gitfile.gitfile)) {
                    placed=true;
                    break;// search loop, no mutation
                }
            }ind_end;
            bool exist=FileObjectExistsQ(gitfile.gitfile);
            if (!exist || !placed) {
                prlog((!exist ? "atf_ci.missing_file" : "atf_ci.stray_file")
                      <<Keyval("success","N")
                      <<Keyval("gitfile",gitfile.gitfile)
                      <<Keyval("comment",!exist ? "File missing from filesystem" : "No dev.gitpath pattern matches this file"));
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

// Fail when a script or a hand-written source carries no copyright notice.
// The check does not add a year: a holder's year stands for a change the
// holder made, and a run of normalize changes nothing.
void atf_ci::citest_non_copyrighted() {
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
            SysCmd(tempstr() << "perl -pi -e 's/\\r$//;s/\\r/\\n/g' "<<algo::strptr_ToBash(fname),FailokQ(false));
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
                SysCmd(tempstr()<<"perl -pi -e 's/\\r$//' "<<algo::strptr_ToBash(gitfile->gitfile),FailokQ(false));
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

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
    // The build dir caches each file's result under a hash of its preprocessed
    // tokens, so a warm run re-analyzes only what changed.  A cold run analyzes
    // every file, and one thread takes a minute over the tree where the cores
    // the process may use take a quarter of that.  -j turns off only
    // unusedFunction, which no --enable asks for.
    SysCmd(tempstr()
           << "cppcheck --error-exitcode=1"
           << " --quiet" // suppress too verbose progress
           << " -j" << algo::GetNcore()
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

// Indent each hand-written C++ file the last commit changed, one emacs per
// file and as many at once as the host has CPUs.
//
// The indenter costs about half a second a file, so a commit touching a thousand
// files takes ten minutes in a row and overruns the citest's budget.  The files
// are independent, so they run side by side.  One file per run keeps
// bin/cpp-indent's bound per file, so a file it cannot finish is skipped alone.
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
    tempstr list;
    ind_beg(algo::FileLine_curs,fname,proc.from_stdout)  {
        atf_ci::FGitfile *gitfile = atf_ci::ind_gitfile_Find(fname);
        bool noindent = gitfile && gitfile->c_noindent;
        bool ourfile = FindStr(fname,"/gen/") == -1 && FindStr(fname,"extern/") == -1;
        if  (FileQ(fname) && ourfile && !noindent && !GeneratedHdrQ(fname)) {
            list << fname << eol;
            prlog_("*");
        }
    }ind_end;
    // the citest's verdict is the set of files the pass modified, and an
    // indenter that never ran modifies nothing, which reads exactly like a tree
    // that was already indented.  bin/cpp-indent runs the indentation through
    // emacs, and exits nonzero when emacs is missing or its elisp fails; xargs
    // passes a failure on as its own status, which is the only evidence the
    // files were checked at all, so the run stops on it instead of reporting a
    // pass it did not compute.  The output goes to the log the script pass
    // writes, out of the citest's own output.
    //
    // A commit can leave the list empty: it touches no C++, or the checkout is
    // shallow, as GitHub's is, and HEAD then has no parent to diff against.  GNU
    // xargs runs its command once even on empty input, so the indenter would
    // start with no file and fail on a host without emacs.  So an empty list
    // runs nothing.
    StringToFile(list, "temp/atf_ci_indent.lst");
    if (list != "") {
        SysCmd("xargs -P $(getconf _NPROCESSORS_ONLN) -n 1 bin/cpp-indent < temp/atf_ci_indent.lst"
               " >> temp/atf_ci_indent.log 2>&1",FailokQ(false));
    }
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

// Return true if LINE of a source file is copyright or license boilerplate: a
// blank line, a Copyright line, a header field that src_hdr writes, or a whole
// line of one of the license texts.  LICENSETEXT holds those lines, trimmed,
// each between two newlines.
static bool BoilerplateQ(algo::strptr line, algo::strptr licensetext) {
    algo::strptr text = Trimmed(line);
    text = StartsWithQ(text,"//") ? Trimmed(RestFrom(text,2))
        : StartsWithQ(text,"#") ? Trimmed(RestFrom(text,1))
        : text;
    return text == ""
        || StartsWithQ(text,"Copyright (C)")
        || StartsWithQ(text,"License:")
        || StartsWithQ(text,"Target:")
        || StartsWithQ(text,"Exceptions:")
        || StartsWithQ(text,"Source:")
        || StartsWithQ(text,"Header:")
        || StartsWithQ(text,"Contacting ICE")
        || FindStr(licensetext,tempstr() << eol << text << eol) != -1;
}

// Return true if DIFF, a whitespace-blind git diff of one file, adds or removes
// a line that is not boilerplate (see BoilerplateQ, given LICENSETEXT).
static bool RealChangeQ(algo::strptr diff, algo::strptr licensetext) {
    bool ret = false;
    ind_beg(algo::Line_curs,line,diff) {
        bool changed = (StartsWithQ(line,"+") && !StartsWithQ(line,"+++"))
            || (StartsWithQ(line,"-") && !StartsWithQ(line,"---"));
        ret = ret || (changed && !BoilerplateQ(RestFrom(line,1),licensetext));
    }ind_end;
    return ret;
}

// Give the current copyright holder this year on every hand-written source and
// script the last commit changed.
// A holder's year in a notice stands for a change the holder made to the file.
// Adding the year to every file once a year claims changes nobody made, so the
// year goes on at the moment of the change, to the files the commit touched.  A
// change that only reindents the file or rewrites its license header adds
// nothing of the holder's, so it adds no year either.
void atf_ci::citest_copyright_srcfile() {
    cstring licensetext;
    licensetext << eol;
    ind_beg(algo::Dir_curs,file,"conf/*.license.txt") {
        ind_beg(algo::FileLine_curs,line,file.pathname) {
            licensetext << Trimmed(line) << eol;
        }ind_end;
    }ind_end;
    tempstr modfiles(SysEval("git diff-tree --name-only HEAD -r --no-commit-id",FailokQ(true),1024*1024*10));
    ind_beg(algo::Line_curs,fname,modfiles) {
        atf_ci::FGitfile *gitfile = atf_ci::ind_gitfile_Find(fname);
        atf_ci::FTargsrc *targsrc = gitfile ? gitfile->c_targsrc : NULL;
        atf_ci::FScriptfile *scriptfile = gitfile ? gitfile->c_scriptfile : NULL;
        bool ourfile = FindStr(fname,"/gen/") == -1 && FindStr(fname,"extern/") == -1;
        if (FileQ(fname) && ourfile && (targsrc || scriptfile)) {
            tempstr diff(SysEval(tempstr()<<"git diff-tree -p -w --no-commit-id HEAD -- "<<strptr_ToBash(fname)
                                 ,FailokQ(true),1024*1024*64));
            if (RealChangeQ(diff,licensetext)) {
                command::src_hdr src_hdr;
                src_hdr.update_copyright = true;
                src_hdr.write = true;
                if (targsrc) {
                    src_hdr.targsrc.expr = targsrc->targsrc;
                } else {
                    src_hdr.scriptfile.expr = fname;
                }
                SysCmd(src_hdr_ToCmdline(src_hdr),FailokQ(false));
            }
        }
    }ind_end;
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

// Run apm -check over every package, so a key that matches nothing, a record
// that references what its package does not carry, proprietary content in an
// open-source package, a file split between two packages, a sync pair that
// disagrees, or a word a package must not carry fails the normalize job.
void atf_ci::citest_apm_check() {
    command::apm_proc apm;
    apm.cmd.package.expr="%";
    apm.cmd.check=true;
    apm_ExecX(apm);
}

// -----------------------------------------------------------------------------

// The word with any shell punctuation stripped off its front, so that curl is
// still recognised inside $(curl ...) or "curl or api=$(curl.
static strptr BareWord(strptr word) {
    strptr ret = word;
    for (int i = 0; i < word.n_elems; i++) {
        char c = word[i];
        if (c=='$' || c=='(' || c=='`' || c=='"' || c=='\'' || c==';' || c=='&' || c=='=' || c=='{') {
            ret = strptr(word.elems + i + 1, word.n_elems - i - 1);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// True when the word names a shell, which is what makes handing it a fetch the
// same thing as running whatever the endpoint served.
static bool ShellNameQ(strptr word) {
    return word == "sh" || word == "bash" || word == "/bin/sh" || word == "/bin/bash";
}

// -----------------------------------------------------------------------------

// True when the word names a fetch.
static bool FetchWordQ(strptr word) {
    strptr name = BareWord(word);
    return name == "curl" || name == "wget";
}

// -----------------------------------------------------------------------------

// True when the word is a short option cluster carrying this letter, so that
// curl -k and curl -sSLk read as the same request.  Every character after the
// dash has to be a letter, which keeps an attached value such as -okernel.tgz
// from being read as a cluster that carries k.
static bool ShortFlagQ(strptr word, char flag) {
    bool ret = false;
    if (word.n_elems > 1 && word[0] == '-' && word[1] != '-') {
        bool alpha = true;
        bool found = false;
        for (int i = 1; i < word.n_elems; i++) {
            char c = word[i];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
                alpha = false;
            }
            if (c == flag) {
                found = true;
            }
        }
        ret = alpha && found;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// The command a pipeline segment runs, with a sudo or env prefix and any
// options or assignments ahead of it skipped, or empty when it runs nothing.
static strptr SegmentCmd(strptr segment) {
    strptr ret;
    ind_beg(algo::Word_curs,word,segment) {
        bool prefix = word == "sudo" || word == "env" || word == "command" || word == "exec";
        bool option = StartsWithQ(word,"-") || algo::FindStr(word,"=") >= 0;
        if (ch_N(ret) == 0 && !prefix && !option) {
            ret = word;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// True when the line fetches bytes over the network.  The name is matched as a
// whole word, because a substring search for curl answers yes to grpcurl.
static bool FetchQ(strptr line) {
    bool ret = false;
    ind_beg(algo::Word_curs,word,line) {
        if (FetchWordQ(word)) {
            ret = true;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// True when the line hands a fetch to a shell, so that what runs is whatever
// the endpoint served and nobody has read it.  The line is read as a pipeline:
// a segment counts only when a fetch stands to its left and the segment's own
// command is a shell, which tells "| sudo -E bash" apart from "| sha256sum -c -"
// and from a pipe that has nothing to do with the fetch beside it.
static bool PipeToShellQ(strptr line) {
    bool ret = false;
    bool fetch = false;
    ind_beg(algo::Sep_curs,segment,line,'|') {
        if (fetch && ShellNameQ(SegmentCmd(segment))) {
            ret = true;
        }
        if (FetchQ(segment)) {
            fetch = true;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// True when the line switches off the check that decides whether the far end is
// who it claims to be, which is what leaves a fetch open to anyone on its path.
// Only the options standing after the fetch itself count, so another command's
// flags on the same line are left alone.
static bool UnverifiedTlsQ(strptr line) {
    bool ret = false;
    ind_beg(algo::Sep_curs,segment,line,'|') {
        bool fetch = false;
        ind_beg(algo::Word_curs,word,segment) {
            bool off = word == "--insecure" || word == "--no-check-certificate" || ShortFlagQ(word,'k');
            if (FetchWordQ(word)) {
                fetch = true;
            } else if (fetch && off) {
                ret = true;
            }
        }ind_end;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// True when the line names a url that carries no version, so that what arrives
// depends on the day rather than on anything this repository states.  The word
// tested has to be the url itself, which keeps a sentence that merely uses the
// word latest out of it.
static bool LatestUrlQ(strptr line) {
    bool ret = false;
    ind_beg(algo::Word_curs,word,line) {
        if (algo::FindStr(word,"//") >= 0 && algo::FindStr(word,"latest",false) >= 0) {
            ret = true;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// True when the line runs git with this subcommand.  Both words are matched
// anywhere on the line rather than side by side, so that git -C <dir> clone is
// seen as the clone it is.
static bool GitWordQ(strptr line, strptr verb) {
    bool git = false;
    bool found = false;
    ind_beg(algo::Word_curs,word,line) {
        if (word == "git") {
            git = true;
        }
        if (word == verb) {
            found = true;
        }
    }ind_end;
    return git && found;
}

// -----------------------------------------------------------------------------

// True when the clone on this line already names the revision it wants, which
// -b and --branch do on the clone's own command line.
static bool ClonePinnedQ(strptr line) {
    bool ret = false;
    ind_beg(algo::Word_curs,word,line) {
        if (word == "--branch" || ShortFlagQ(word,'b')) {
            ret = true;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Check that each installation function is alone in the file named after its row.
// A reader who knows the installation knows which file to open, and an
// installation whose steps are spread over two files has no such file.  The
// pairing is not something the compiler can hold: amc binds a row to a function
// by name wherever that function is written.
void atf_ci::citest_check_ainst() {
    command::src_func_proc src_func;
    src_func.cmd.func.expr = "ainst.pkg_%";
    src_func.cmd.showloc = true;
    src_func.fstdout = "|";
    src_func_Start(src_func);
    strptr prefix = "ainst::extpkg_";
    int n_err = 0;
    ind_beg(algo::FileLine_curs, line, src_func.from_stdout) {
        strptr trimmed = Trimmed(line);
        strptr file = Pathcomp(trimmed, ":LL");
        strptr func = Pathcomp(trimmed, "(RL RR");
        if (StartsWithQ(func, prefix)) {
            strptr name = RestFrom(func, prefix.n_elems);
            tempstr expected = tempstr() << "cpp/ainst/" << name << ".cpp";
            if (file != expected) {
                prlog("atf_ci.badloc"
                      <<Keyval("pkg",name)
                      <<Keyval("actual",file)
                      <<Keyval("expected",expected));
                n_err++;
            }
        }
    }ind_end;
    src_func_Wait(src_func);
    vrfy(src_func.status == 0, tempstr()<<"atf_ci.src_func_fail"
         <<Keyval("status",src_func.status)
         <<Keyval("comment","src_func could not list the package functions, so nothing was checked"));
    // Every package pins a release, and a missing dev.extpkgver row is how that
    // stops being true with nothing objecting: the version reads as 0, the
    // install still works because each url carries its version literally, and
    // the only place it shows is the Version tag of an rpm built months later.
    ind_beg(atf_ci::_db_extpkg_curs,pkg,atf_ci::_db) {
        if (!atf_ci::ind_extpkgver_Find(pkg.extpkg)) {
            prlog("atf_ci.nopkgver"
                  <<Keyval("pkg",pkg.extpkg)
                  <<Keyval("comment","package pins no release; add an dev.extpkgver row"));
            n_err++;
        }
    }ind_end;
    vrfy(n_err == 0, tempstr() << n_err << " package(s) misplaced or unpinned");
}

// -----------------------------------------------------------------------------

// Report one defect found in an install script, and fail the citest.
static void ReportInstallScript(strptr inst, int lineno, strptr comment) {
    prlog("atf_ci.install_script"
          <<Keyval("inst",inst)
          <<Keyval("line",lineno)
          <<Keyval("comment",comment));
    algo_lib::_db.exit_code=1;
}

// -----------------------------------------------------------------------------

// Refuse a generated install script that cannot say what it installs.
// ainst -script is inlined into the image builds, where it runs as root, so
// whoever answers for the endpoint it reads from chooses what lands in the
// image.  Four defects make that unanswerable, and none of them is ever needed:
// piping a fetch into a shell, turning the certificate check off, naming latest
// in a url, and cloning a repository without moving it to a named revision.
// A clone is carried until a checkout or a reset moves it, so a script that
// clones twice has to move each one, and a clone moved on its own line is
// already satisfied.  A clone is carried no further than the installation it
// stands in: the next one's stage function is where an unmoved clone is
// reported, since nothing after that boundary can be the checkout it wanted.  A comment line is skipped whatever it is indented by,
// because bash ignores it wherever the generator puts it.
// The whole of what ainst can emit is read here, which is why this reads one
// program rather than a directory: an installation reaches the network through
// the generated fetch step and nowhere else, so the four checks have exactly
// one text to cover.
void atf_ci::citest_install_script() {
    command::ainst_proc ainst;
    ainst.cmd.script = true;
    ainst.fstdout = "|";
    ainst_Start(ainst);
    tempstr inst("preamble");
    int clone_line = 0;
    ind_beg(algo::FileLine_curs,line,ainst.from_stdout) {
        int lineno = ind_curs(line).i + 1;
        strptr trimmed = algo::Trimmed(line);
        if (StartsWithQ(trimmed,"inst_") && EndsWithQ(trimmed,"_stage() {")) {
            if (clone_line > 0) {
                ReportInstallScript(inst, clone_line, "git clone is not moved to a named revision");
            }
            // Pathcomp reads its expression in groups of three characters, so
            // "_stage() {LL" is four searches rather than one separator and the
            // name loses everything from its first "a" on.  Both ends are known
            // from the test above, so the name is what lies between them.
            strptr body = algo::RestFrom(trimmed,5);
            inst = algo::qGetRegion(body,0,body.n_elems-10);
            clone_line = 0;
        }
        if (!StartsWithQ(trimmed,"#")) {
            bool moved = GitWordQ(line,"checkout") || GitWordQ(line,"reset");
            if (PipeToShellQ(line)) {
                ReportInstallScript(inst, lineno, "fetch piped into a shell");
            }
            if (UnverifiedTlsQ(line)) {
                ReportInstallScript(inst, lineno, "certificate verification disabled");
            }
            if (LatestUrlQ(line)) {
                ReportInstallScript(inst, lineno, "url names latest instead of a version");
            }
            if (GitWordQ(line,"clone")) {
                if (clone_line > 0) {
                    ReportInstallScript(inst, clone_line, "git clone is not moved to a named revision");
                }
                clone_line = moved || ClonePinnedQ(line) ? 0 : lineno;
            } else if (moved) {
                clone_line = 0;
            }
        }
    }ind_end;
    if (clone_line > 0) {
        ReportInstallScript(inst, clone_line, "git clone is not moved to a named revision");
    }
    ainst_Wait(ainst);
    vrfy(ainst.status == 0, tempstr()<<"atf_ci.ainst_fail"
         <<Keyval("status",ainst.status)
         <<Keyval("comment","ainst could not produce the install script"));
}
