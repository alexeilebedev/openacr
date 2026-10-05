// Copyright (C) 2024,2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: atf_cov (exe) -- Line coverage
// Exceptions: yes
// Source: cpp/atf_cov/atf_cov.cpp
//

#include "include/algo.h"
#include "include/atf_cov.h"

// Write this run's progress to the logfile and leave its errors on stderr.
// A merge names every directory it reads and every ssimfile it writes, which is
// thousands of lines that would drown whatever console the run was started from,
// and that is the reason the logfile exists.  A verdict is not progress: it is
// the answer the caller asked for, and a caller who is handed only the logfile's
// path learns nothing when the file goes with the machine that wrote it.  So the
// sink follows the logcat -- stdout to the file, stderr to stderr.
static void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) {
    if (logcat && !logcat->stdout) {
        algo::Prlog(logcat,tstamp,str);
    } else {
        WriteFileX(atf_cov::_db.logfd.fd,strptr_ToMemptr(str));
    }
}

// The lines of gcov OUTPUT that name a profile file gcov could not use --
// `x.gcda:not a gcov data file`, `x.gcno:cannot open notes file` -- which is
// how gcov reports the file that fails a batch.  Returns at most twenty of
// them, one per line.
static tempstr GcovDiagnostic(strptr output) {
    tempstr ret;
    algo::ListSep ls("\n");
    int n = 0;
    ind_beg(algo::Line_curs,line,output) {
        strptr file = Pathcomp(line,":LL");
        if (n < 20 && (EndsWithQ(file,".gcda") || EndsWithQ(file,".gcno"))) {
            ret << ls << line;
            n++;
        }
    }ind_end;
    return ret;
}

static tempstr GetGcovDstDir(strptr covdir, strptr name) {
    tempstr dir(".");
    // Mangle covdir to match gcov filename format (/ -> #)
    tempstr mangled_covdir(covdir);
    Translate(mangled_covdir, "/", "#");
    // Check if filename starts with mangled covdir
    if (StartsWithQ(name, mangled_covdir)) {
        dir = covdir;
    }
    return dir;
}

static void flag_UpdateMax(atf_cov::FCovline &lhs, const dev::Covline &rhs) {
    lhs.flag = lhs.flag == dev_Covline_flag_E || rhs.flag == dev_Covline_flag_E
        ? dev_Covline_flag_E
        :  lhs.flag == dev_Covline_flag_P || rhs.flag == dev_Covline_flag_P
        ? dev_Covline_flag_P
        : dev_Covline_flag_N;
}

void atf_cov::MergeCovline(dev::Covline &covline_in) {
    atf_cov::FCovline *covline = atf_cov::ind_covline_Find(covline_in.covline);
    if (covline) {
        covline->hit += covline_in.hit;
        flag_UpdateMax(*covline,covline_in);
        vrfy(covline_in.text == covline->text, tempstr()
             << "covline_mismatch"
             << Keyval("covline",covline->covline)
             << Keyval("expected",covline->text)
             << Keyval("actual",covline_in.text));
    } else {
        atf_cov::covline_InsertMaybe(covline_in);
    }
}

// List the files matching PATTERN, one pathname per line.
//
// A directory is walked by asking the kernel for its entries a batch at a
// time, and the walk has no way to learn that an entry it has not reached yet
// has moved.  Creating, renaming or deleting files in a directory while
// walking it therefore drops entries -- how many depends on the order the
// filesystem happens to keep them in, so the same code reads every file on one
// machine and half of them on another.  A caller that mutates a directory
// takes this list first and walks the list.
static tempstr ListFile(strptr pattern) {
    tempstr ret;
    ind_beg(algo::Dir_curs,ent,pattern) {
        ret << ent.pathname << eol;
    }ind_end;
    return ret;
}

static void DeleteFiles(strptr dir, strptr pattern) {
    tempstr filelist = ListFile(DirFileJoin(dir,pattern));
    ind_beg(algo::Line_curs,path,filelist) {
        DeleteFile(path);
    }ind_end;
}

static void CleanupDirPhase(strptr covdir, atf_cov_Phase_value_Enum phase) {
    if (phase == atf_cov_Phase_value_runcmd) {
        if (!atf_cov::_db.cmdline.incremental) {
            DeleteFiles(covdir,"*.gcda");
        }
        phase = atf_cov_Phase_value_gcov;// grab the next phases too
    }
    if (phase == atf_cov_Phase_value_gcov) {
        DeleteFiles(covdir,"*.gcno");
        DeleteFiles(covdir,"*.gcov");
        phase = atf_cov_Phase_value_ssim;// grab the next phases too
    }
    if (phase == atf_cov_Phase_value_ssim) {
        DeleteFiles(covdir,"*.cov.ssim");
        phase = atf_cov_Phase_value_report;// grab the next phases too
    }
    if (phase == atf_cov_Phase_value_report) {
        DeleteFiles(covdir,"report.txt");
        DeleteFiles(covdir,"report.ssim");
        DeleteFiles(covdir,"cobertura.xml");
    }
}

void atf_cov::RunGcov(strptr covdir) {
    cstring covdir_full = GetFullPath(covdir);
    // Create build symlink in covdir for relative gcno paths
    {
        cstring build_target = DirFileJoin(algo::GetCurDir(), "build");
        cstring build_link = DirFileJoin(covdir, "build");
        int rc = symlink(Zeroterm(build_target), Zeroterm(build_link));
        (void)rc;
    }
    // cleanup current dir from .gcov files
    tempstr gcovlist = ListFile("*.gcov");
    ind_beg(algo::Line_curs,path,gcovlist) {
        cstring dst_dir = GetGcovDstDir(covdir, Pathcomp(path,"/RR"));
        cstring dst_dir_full = GetFullPath(dst_dir);
        if (dst_dir_full == covdir_full) {
            DeleteFile(path);
        }
    }ind_end;
    // Walk the profile data, link each file to the program graph it was written
    // from, and group the pathnames into gcov command lines.
    //
    // The command carries one pathname per object file, and a full run of the
    // suite leaves about a thousand of them in a directory.  A shell command is
    // handed to the kernel as a single argument, and the kernel refuses one
    // longer than 128K: a single command naming every file sits close enough to
    // that ceiling that it crosses it on a machine whose checkout path is a few
    // characters longer, and the exec then fails for a reason no coverage
    // figure can express.  Cutting the work into commands of bounded length
    // puts the ceiling out of reach whatever the paths look like.
    tempstr gcdalist = ListFile(DirFileJoin(covdir,"*.gcda"));
    cstring gcov_cmdlist;
    u32 n_gcda = 0;
    i32 cmd_beg = 0;
    ind_beg(algo::Line_curs,path,gcdalist) {
        cstring gcno_dst_path = ReplaceExt(path,".gcno");
        cstring gcno_src_path = ReplaceExt(Pathcomp(path,"/RR"),".gcno"); // mangled
        Translate(gcno_src_path,"#","/"); // demangle
        errno_vrfy_(symlink(Zeroterm(gcno_src_path),Zeroterm(gcno_dst_path))==0);
        n_gcda += 1;
        if (ch_N(gcov_cmdlist) == cmd_beg) {
            gcov_cmdlist << "gcov -p -l";
        }
        gcov_cmdlist << " " << path;
        if (ch_N(gcov_cmdlist) - cmd_beg > 65536) {
            gcov_cmdlist << eol;
            cmd_beg = ch_N(gcov_cmdlist);
        }
    }ind_end;
    if (ch_N(gcov_cmdlist) > cmd_beg) {
        gcov_cmdlist << eol;
    }
    // run gcov over the profile data.  gcov reports a profile it cannot read
    // as `<file>:<message>`, goes on to the next file in the command, and exits
    // non-zero at the end, so one unreadable .gcda fails a batch whose every
    // other file still produced its .gcov.  A failed command is named here with
    // those lines, since the full output goes to the logfile, which a CI job
    // keeps as an artifact and the job log does not show.
    u32 n_fail = 0;
    ind_beg(algo::Line_curs,gcov_cmd,gcov_cmdlist) {
        int status = 0;
        tempstr output = SysEval(tempstr() << gcov_cmd << " 2>&1", FailokQ(true), 1<<24, false, &status);
        if (ValidQ(_db.logfd.fd)) {
            WriteFileX(_db.logfd.fd,strptr_ToMemptr(output));
        }
        if (status != 0) {
            n_fail++;
            prerr("atf_cov.gcov_fail"
                  <<Keyval("covdir",covdir)
                  <<Keyval("exit",algo::WaitStatusToExitCode(status))
                  <<Keyval("output",GcovDiagnostic(output)));
        }
    }ind_end;
    // walk .ccov in current directory, move relevant to covdir
    tempstr outlist = ListFile("*.gcov");
    ind_beg(algo::Line_curs,line,outlist) {
        cstring src_path;
        src_path << line;
        tempstr filename(Pathcomp(src_path,"/RR"));
        cstring dst_dir = GetGcovDstDir(covdir, filename);
        cstring dst_dir_full = GetFullPath(dst_dir);
        cstring dst_path = DirFileJoin(covdir,filename);
        if (dst_dir_full == covdir_full && dst_dir_full != algo::GetCurDir()) {
            errno_vrfy_(rename(Zeroterm(src_path),Zeroterm(dst_path))==0);
        }
    }ind_end;
    // walk gcov files in covdir, parse and fill in database, merge lines if needed
    u32 n_gcov = 0;
    ind_beg(algo::Dir_curs,ent,DirFileJoin(covdir,"*.gcov")) {
        cstring src;
        n_gcov += 1;
        ind_beg(algo::FileLine_curs,gcov_line,ent.pathname) if (!StartsWithQ(gcov_line,"-")) {
            algo::StringIter line_it(gcov_line);
            strptr col1 = Trimmed(GetTokenChar(line_it,':'));
            strptr col2 = Trimmed(GetTokenChar(line_it,':'));
            i32 line;
            vrfy(i32_ReadStrptrMaybe(line,col2),tempstr()<<"Gcov paring error: Invalid line number: "<<line);
            if (line==0 && SkipStrptr(line_it,"Source:")) {
                src = line_it.Rest();
                if (!ind_gitfile_Find(src)) {
                    prlog("Gitfile not found, skipping "<<src);
                    break;
                }
            }
            if (line > 0) {
                vrfy_(src!="");
                if (Regx_Match(_db.cmdline.exclude,src)) {
                    prlog("Skipping excluded source:"<<src);
                    break;
                }
                cstring key = dev::Covline_Concat_src_line(src,line);
                dev::Covline covline;
                covline.covline = dev::Covline_Concat_src_line(src,line);
                covline.flag = col1 == "-" ? dev_Covline_flag_N
                    : EndsWithQ(col1,"*") ? dev_Covline_flag_P
                    : dev_Covline_flag_E;
                if (covline.flag != dev_Covline_flag_N) {
                    u32_ReadStrptrMaybe(covline.hit,col1);
                }
                covline.text = line_it.Rest();
                MergeCovline(covline);
            }
        }ind_end;
    }ind_end;
    // Say what this directory contributed, on stderr, where a CI job log keeps
    // it.  The directory is the unit of loss: each citest of the coverage cijob
    // writes its profile data into a directory of its own, so a directory that
    // gives nothing back takes with it every target only its citest exercises,
    // and the run ends reporting those targets as uncovered code.  A directory
    // holding profile data that yields no coverage is therefore an error here,
    // named where the cause is still visible.  A gcov command that failed over
    // a directory that still yielded coverage is reported above and counted
    // here, and what its failure cost is judged where every measurement is:
    // by each target against its floor.
    bool success = n_gcda == 0 || n_gcov > 0;
    _db.report.n_covdir += 1;
    _db.report.n_covdir_empty += !success;
    prerr("atf_cov.merge"
          <<Keyval("covdir",covdir)
          <<Keyval("n_gcda",n_gcda)
          <<Keyval("n_gcov",n_gcov)
          <<Keyval("n_fail",n_fail)
          <<Keyval("success",success ? "Y" : "N")
          <<Keyval("comment",success ? "" : "gcov read nothing here; every target this citest alone exercises is lost"));
    if (!success) {
        algo_lib::_db.exit_code=1;
    }
}

void atf_cov::WriteCovSsim() {
    // write out database to .cov.ssim files in covdir
    ind_beg(_db_gitfile_curs,gitfile,_db) if (gitfile.c_targsrc && c_covline_N(gitfile)) {
        cstring ssim_filename = tempstr() << gitfile.gitfile << ".cov.ssim";
        Translate(ssim_filename,"/","#");
        cstring ssim_pathname = DirFileJoin(_db.cmdline.covdir,ssim_filename);
        algo_lib::FFildes ssim;
        ssim.fd = OpenWrite(ssim_pathname);
        ind_beg(gitfile_c_covline_curs,covline,gitfile) {
            cstring out;
            dev::Covline covline_out;
            covline_CopyOut(covline,covline_out);
            out << covline_out << '\n';
            WriteFileX(ssim.fd,strptr_ToMemptr(out));
        }ind_end;
        prlog("Written "<<ssim_pathname);
    }ind_end;
}

// A target carries a floor above zero because some test exercises it, so a run
// that found no data for it did not measure it.  A floor of zero says the
// opposite -- nothing exercises this target today -- and such a target produces
// no data in a perfectly good run.
static bool UnmeasuredQ(atf_cov::FTarget &target) {
    return target.c_tgtcov && target.c_tgtcov->cov_min > algo::U32Dec2(0) && !target.c_covtarget;
}

// The run lost coverage data, in one of the two ways it can: a merge directory
// gave nothing back, or a target that some test exercises produced no data.
static bool LostDataQ() {
    return atf_cov::_db.report.n_covdir_empty > 0 || atf_cov::_db.report.n_unmeasured > 0;
}

void atf_cov::ComputeCoverage() {
    // compute line coverage for sources and targets
    ind_beg(_db_gitfile_curs,gitfile,_db) if (gitfile.c_targsrc && c_covline_N(gitfile)) {
        atf_cov::FCovfile &covfile = covfile_Alloc();
        covfile.covfile = gitfile.gitfile;
        covfile_XrefMaybe(covfile);
        ind_beg(gitfile_c_covline_curs,covline,gitfile) {
            ++covfile.total;
            covfile.nonexe += covline.flag == dev_Covline_flag_N ? 1 : 0;
            covfile.exe    += covline.flag != dev_Covline_flag_N ? 1 : 0;
            covfile.hit    += covline.hit > 0;
        }ind_end;
        value_qSetDouble(covfile.exer, covfile.total ? 100.0*covfile.exe/covfile.total : 0.0);
        value_qSetDouble(covfile.cov, covfile.exe ? 100.0*covfile.hit/covfile.exe : 100.0);
        if (!gitfile.c_targsrc->p_target->c_covtarget) {
            atf_cov::FCovtarget &covtarget = covtarget_Alloc();
            covtarget.covtarget = gitfile.c_targsrc->p_target->target;
            covtarget_XrefMaybe(covtarget);
        }
        atf_cov::FCovtarget &covtarget = *gitfile.c_targsrc->p_target->c_covtarget;
        covtarget.total  += covfile.total;
        covtarget.nonexe += covfile.nonexe;
        covtarget.exe    += covfile.exe;
        covtarget.hit    += covfile.hit;
        value_qSetDouble(covtarget.exer, covtarget.total ? 100.0*covtarget.exe/covtarget.total : 0.0);
        value_qSetDouble(covtarget.cov, covtarget.exe ? 100.0*covtarget.hit/covtarget.exe : 100.0);
    }ind_end;
    // compute total coverage
    _db.total.covtarget = "TOTAL";
    ind_beg(_db_target_curs,target,_db) if (target.c_covtarget) {
        _db.total.total  += target.c_covtarget->total;
        _db.total.nonexe += target.c_covtarget->nonexe;
        _db.total.exe    += target.c_covtarget->exe;
        _db.total.hit    += target.c_covtarget->hit;
        value_qSetDouble(_db.total.exer, _db.total.total ? 100.0*_db.total.exe/_db.total.total : 0.0);
        value_qSetDouble(_db.total.cov, _db.total.exe ? 100.0*_db.total.hit/_db.total.exe : 100.0);
    }ind_end;
    // Record how far this measurement reaches.  Every percentage above is correct
    // for the data the run happened to find, so a run whose instrumentation data
    // is mostly missing prints a plausible table and a healthy TOTAL.  The number
    // of targets that produced data, set beside the number that carry a floor,
    // is what tells the two apart.
    ind_beg(_db_target_curs,target,_db) {
        _db.report.n_covtarget += target.c_covtarget ? 1 : 0;
        _db.report.n_tgtcov += target.c_tgtcov ? 1 : 0;
        _db.report.n_unmeasured += UnmeasuredQ(target);
    }ind_end;
    _db.report.exe = _db.total.exe;
    _db.report.hit = _db.total.hit;
}

void atf_cov::GenerateSsimReport() {
    cstring ssim;
    ssim << _db.total << "\n";
    ind_beg(_db_target_curs,target,_db) if (target.c_covtarget) {
        dev::Covtarget covtarget_out;
        covtarget_CopyOut(*target.c_covtarget,covtarget_out);
        ssim << "    " << covtarget_out << "\n";
        ind_beg(target_c_targsrc_curs,targsrc,target) if (targsrc.p_gitfile->c_covfile) {
            dev::Covfile covfile_out;
            covfile_CopyOut(*targsrc.p_gitfile->c_covfile,covfile_out);
            ssim << "        " <<covfile_out << "\n";
        }ind_end;
        ssim << "\n";
    }ind_end;
    cstring report_ssim = DirFileJoin(_db.cmdline.covdir,"report.ssim");
    //prlog(ssim);
    StringToFile(ssim,report_ssim);
    prlog("Generated "<<report_ssim);
}

static tempstr DeltaStr(algo::U32Dec2 a, algo::U32Dec2 b) {
    tempstr ret;
    if (a > b) {
        ret << "(+" << algo::U32Dec2(a-b) << ")";
    } else if (b > a) {
        ret << "(-" << algo::U32Dec2(b-a) << ")";
    }
    return ret;
}

void atf_cov::GenerateTxtReport() {
    cstring header("ARTIFACT\tTOTAL\tNONEXE\tEXE\tEXE%\tHIT\tCOV%\n");
    cstring txt;
    ind_beg(_db_target_curs,target,_db) if (target.c_covtarget) {
        txt << header;
        ind_beg(target_c_targsrc_curs,targsrc,target) if (targsrc.p_gitfile->c_covfile) {
            txt << targsrc.p_gitfile->c_covfile->covfile
                << "\t" << targsrc.p_gitfile->c_covfile->total
                << "\t" << targsrc.p_gitfile->c_covfile->nonexe
                << "\t" << targsrc.p_gitfile->c_covfile->exe
                << "\t" << targsrc.p_gitfile->c_covfile->exer
                << "\t" << targsrc.p_gitfile->c_covfile->hit
                << "\t" << targsrc.p_gitfile->c_covfile->cov
                << "\n";
        }ind_end;
        txt << target.c_covtarget->covtarget
            << "\t" << target.c_covtarget->total
            << "\t" << target.c_covtarget->nonexe
            << "\t" << target.c_covtarget->exe
            << "\t" << target.c_covtarget->exer
            << "\t" << target.c_covtarget->hit
            << "\t" << target.c_covtarget->cov
            << "\n\n";
    }ind_end;
    txt << header;
    ind_beg(_db_target_curs,target,_db) if (target.c_covtarget) {
        txt << target.c_covtarget->covtarget
            << "\t" << target.c_covtarget->total
            << "\t" << target.c_covtarget->nonexe
            << "\t" << target.c_covtarget->exe
            << "\t" << target.c_covtarget->exer
            << "\t" << target.c_covtarget->hit
            << "\t" << target.c_covtarget->cov
            << "\n";
    }ind_end;
    txt << _db.total.covtarget
        << "\t" << _db.total.total
        << "\t" << _db.total.nonexe
        << "\t" << _db.total.exe
        << "\t" << _db.total.exer
        << "\t" << _db.total.hit
        << "\t" << _db.total.cov
        << "\n\n";
    cstring report_txt  = DirFileJoin(_db.cmdline.covdir,"report.txt");
    cstring tbl = Tabulated(txt,"\t","lrrrrrr",2);
    //prlog(tbl);
    StringToFile(tbl,report_txt);
    prlog("Generated "<<report_txt);
}

void atf_cov::Summary() {
    cstring header("ARTIFACT\tEXE\tHIT\tCOV%\n");
    cstring txt;
    txt << header;
    ind_beg(_db_target_curs,target,_db) if (target.c_covtarget) {
        txt << target.c_covtarget->covtarget
            << "\t" << target.c_covtarget->exe
            << "\t" << target.c_covtarget->hit
            << "\t" << target.c_covtarget->cov
            << "\t" << (target.c_tgtcov ? DeltaStr(target.c_covtarget->cov,target.c_tgtcov->cov_min) : strptr())
            << "\n";
    }ind_end;
    txt << _db.total.covtarget
        << "\t" << _db.total.exe
        << "\t" << _db.total.hit
        << "\t" << _db.total.cov
        << "\n\n";
    cstring tbl = Tabulated(txt,"\t","lrrrl",2);
    tbl << _db.report << "\n";
    if (_db.cmdline.report) {
        cstring report_txt  = DirFileJoin(_db.cmdline.covdir,"summary.txt");
        StringToFile(tbl,report_txt);
        prlog("Generated "<<report_txt);
    }
    if (_db.cmdline.summary) {
        prlog(tbl);
    }
}

void atf_cov::XmlIndent(algo::cstring &out, strptr text, int indent) {
    if (_db.cmdline.xmlpretty) {
        char_PrintNTimes(' ', out, indent*2);
    }
    out << text;
}

void atf_cov::GenerateCoberturaReport() {
    cstring xml;
    xml << "<?xml version=\"1.0\"?>\n";
    xml << "<!DOCTYPE coverage SYSTEM \"http://cobertura.sourceforge.net/xml/coverage-04.dtd\">\n";
    xml << "<coverage"
        " line-rate=\""<<_db.total.cov<<"\""
        " branch-rate=\"0.0\""
        " lines-covered=\""<<_db.total.hit<<"\""
        " lines-valid=\""<<_db.total.exe<<"\""
        " branches-covered=\"0\""
        " branches-valid=\"0\""
        " complexity=\"0.0\""
        " version=\"0.0\""
        " timestamp=\""<<u64(ToSecs(algo::CurrUnTime())*1000)<<"\">\n";
    XmlIndent(xml,"<sources>\n",1);
    XmlIndent(xml,"<source>./</source>\n",1);
    XmlIndent(xml,"</sources>\n",1);
    XmlIndent(xml,"<packages>\n",1);
    ind_beg(_db_target_curs,target,_db) if (target.c_covtarget) {
        cstring xpackage;
        xpackage << "<package"
            " name=\"\""
            " line-rate=\""<<target.c_covtarget->cov<<"\""
            " branch-rate=\"0\""
            " complexity=\"0\">\n";
        XmlIndent(xml,xpackage,2);
        XmlIndent(xml,"<classes>\n",3);
        ind_beg(target_c_targsrc_curs,targsrc,target) if (targsrc.p_gitfile->c_covfile) {
            cstring class_name(targsrc.p_gitfile->c_covfile->covfile);
            Translate(class_name,"/.","__");
            cstring xclass;
            xclass << "<class"
                " name=\""<<class_name<<"\""
                " filename=\""<<targsrc.p_gitfile->c_covfile->covfile<<"\""
                " line-rate=\""<<targsrc.p_gitfile->c_covfile->cov<<"\""
                " branch-rate=\"0\""
                " complexity=\"0\">\n";
            XmlIndent(xml,xclass,4);
            XmlIndent(xml,"<methods>\n",5);
            XmlIndent(xml,"</methods>\n",5);
            XmlIndent(xml,"<lines>\n",5);
            ind_beg(gitfile_c_covline_curs,covline,*targsrc.p_gitfile) if (covline.flag != dev_Covline_flag_N) {
                cstring xline;
                xline << "<line"
                    " number=\""<<line_Get(covline)<<"\""
                    " hits=\""<<covline.hit<<"\"/>\n";
                XmlIndent(xml,xline,6);
            }ind_end;
            XmlIndent(xml,"</lines>\n",5);
            XmlIndent(xml,"</class>\n",4);
        }ind_end;
        XmlIndent(xml,"</classes>\n",3);
        XmlIndent(xml,"</package>\n",2);
    }ind_end;
    XmlIndent(xml,"</packages>\n",1);
    xml << "</coverage>\n";
    cstring cobertura_xml  = DirFileJoin(_db.cmdline.covdir,"cobertura.xml");
    StringToFile(xml,cobertura_xml);
    prlog("Generated "<<cobertura_xml);
}

// Judge the run first, and its targets only if the run is whole.
//
// A run that lost data measures the targets it did reach at less than their
// real coverage, because the tests whose data went missing are the same tests
// that exercise the rest of the tree.  Judging such a run target by target
// prints one floor breach per target -- fifty of them on a bad day -- and every
// line of that names a target of the branch under test, so the author reads a
// lost merge directory as fifty regressions they caused.  So a run that lost
// data fails once, as one fact about the run, naming what went missing; the
// answer to it is to run the job again, not to read the diff.
//
// A whole run judges each target against its floor, and a measurement that came
// in under one is a regression the diff under test explains.
void atf_cov::Main_Check() {
    cstring unmeasured;
    algo::ListSep ls(" ");
    ind_beg(_db_target_curs,target,_db) if (UnmeasuredQ(target)) {
        unmeasured << ls << target.target;
    }ind_end;
    if (LostDataQ()) {
        prerr("atf_cov.coverage_lost"
              <<Keyval("n_covdir_empty",_db.report.n_covdir_empty)
              <<Keyval("n_covtarget",_db.report.n_covtarget)
              <<Keyval("n_unmeasured",_db.report.n_unmeasured)
              <<Keyval("success","N")
              <<Keyval("target",unmeasured)
              <<Keyval("comment","run lost coverage data; no target is judged against its floor"));
        algo_lib::_db.exit_code=1;
    } else {
        ind_beg(_db_target_curs,target,_db) if (target.c_tgtcov && target.c_covtarget) {
            algo::U32Dec2 cov = target.c_covtarget->cov;
            algo::U32Dec2 maxerr(500);// tolerable error: 5%, scale 1e2
            algo::U32Dec2 cov_plus_maxerr(cov + maxerr);
            if (cov_plus_maxerr < target.c_tgtcov->cov_min) {
                prerr("atf_cov.coverage_lowered"
                      <<Keyval("target",target.target)
                      <<Keyval("success","N")
                      <<Keyval("cov_min",target.c_tgtcov->cov_min)
                      <<Keyval("maxerr",maxerr)
                      <<Keyval("cov_measured",cov)
                      <<Keyval("comment",""));
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }
}

// Write each target's measurement into dev.tgtcov as its new floor, and the
// functions no test reached into dev.uncovfunc.
//
// A capture is worth no more than the run beneath it.  A run that lost a merge
// directory measures every target that directory exercised at a fraction of its
// real coverage, and capturing those figures writes the loss into the floors:
// the gate comes down by exactly the amount that went missing, nothing in the
// output says so, and the next run passes against the lowered bar.  A capture
// is also the one operation here with no undo short of a revert.  So a run
// showing any sign of loss is refused, and the floors keep the values an
// earlier whole run put there.
void atf_cov::Main_Capture() {
    if (LostDataQ()) {
        prerr("atf_cov.capture_refused"
              <<Keyval("n_covdir_empty",_db.report.n_covdir_empty)
              <<Keyval("n_covtarget",_db.report.n_covtarget)
              <<Keyval("n_unmeasured",_db.report.n_unmeasured)
              <<Keyval("success","N")
              <<Keyval("comment","run lost coverage data; capturing it would write the loss into the floors"));
        algo_lib::_db.exit_code=1;
    } else {
        ind_beg(_db_target_curs,target,_db) {
            if (!target.c_tgtcov) {
                atf_cov::FTgtcov &tgtcov  =tgtcov_Alloc();
                tgtcov.target=target.target;
                tgtcov_XrefMaybe(tgtcov);
            }
            if (target.c_covtarget) {
                target.c_tgtcov->cov_min = target.c_covtarget->cov;
            }
        }ind_end;
        SaveCov();
        SaveUncovfunc();
    }
}

void atf_cov::SaveCov() {
    cstring text;
    ind_beg(atf_cov::_db_tgtcov_curs, tgtcov, atf_cov::_db) {
        dev::Tgtcov out;
        tgtcov_CopyOut(tgtcov, out);
        text << out << eol;
    }ind_end;
    const auto acr_in = "temp/tgtcov.ssim";
    StringToFile(text, acr_in);
    command::acr acr;
    acr.replace = true;
    acr.trunc = true;
    acr.print = false;
    acr.write = true;
    SysCmd(acr_ToCmdline(acr) << " <" << acr_in);
}

// Build the dev.uncovfunc backlog: every in-scope function whose
// executable lines are all unhit across the suite.  Function extents
// (source file, begin and end line) come from src_func -printssim;
// per-line hit data from the Covline pool.  Scoped like the coverage
// report -- only functions in target sources are considered, which
// excludes generated/external the same way RunGcov already filters.
void atf_cov::ComputeUncovfunc() {
    cstring func_ssim = DirFileJoin(_db.cmdline.covdir, "func.ssim");
    command::src_func src_func;
    (void)Regx_ReadStrptrMaybe(src_func.func,"%");
    src_func.printssim = true;
    SysCmd(command::src_func_ToCmdline(src_func) << " >" << func_ssim);
    ind_beg(algo::FileLine_curs,line,func_ssim) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptrMaybe(tuple,line)) {
            strptr name = attr_GetString(tuple,"func");
            strptr src = attr_GetString(tuple,"src");
            i32 begline = 0;
            i32 endline = 0;
            i32_ReadStrptrMaybe(begline,attr_GetString(tuple,"line"));
            i32_ReadStrptrMaybe(endline,attr_GetString(tuple,"endline"));
            atf_cov::FGitfile *gitfile = ind_gitfile_Find(src);
            // skip non-pivotable parses (empty function name) and sources outside coverage scope
            if (!EndsWithQ(name,".") && gitfile && gitfile->c_targsrc && endline >= begline) {
                u32 nexe = 0;
                u32 nhit = 0;
                // point-lookup each source line of the function in the Covline index
                for (i32 ln = begline; ln <= endline; ++ln) {
                    atf_cov::FCovline *covline = ind_covline_Find(dev::Covline_Concat_src_line(src,ln));
                    if (covline && covline->flag != dev_Covline_flag_N) {
                        nexe += 1;
                        nhit += covline->hit > 0;
                    }
                }
                if (nexe > 0 && nhit == 0) {
                    strptr args = attr_GetString(tuple,"args");
                    atf_cov::FUncovfunc &uncovfunc = uncovfunc_Alloc();
                    // key is the signature: name + arglist through the closing paren (drops the body
                    // brace).  name/src/line/target are not stored -- name is a substr of this pkey.
                    uncovfunc.uncovfunc = tempstr() << name << "(" << Pathcomp(args,")RL") << ")";
                }
            }
        }
    }ind_end;
}

// Dump the uncovfunc pool to PATH in dev.uncovfunc ssim format.
void atf_cov::WriteUncovfunc(strptr path) {
    cstring text;
    ind_beg(atf_cov::_db_uncovfunc_curs,uncovfunc,atf_cov::_db) {
        dev::Uncovfunc out;
        uncovfunc_CopyOut(uncovfunc,out);
        text << out << eol;
    }ind_end;
    StringToFile(text, path);
}

// Persist the uncovfunc pool to its committed ssimfile, replacing prior
// contents -- the same acr -replace -trunc path SaveCov uses for tgtcov.
void atf_cov::SaveUncovfunc() {
    const auto acr_in = "temp/uncovfunc.ssim";
    WriteUncovfunc(acr_in);
    command::acr acr;
    acr.replace = true;
    acr.trunc = true;
    acr.print = false;
    acr.write = true;
    SysCmd(acr_ToCmdline(acr) << " <" << acr_in);
}

void atf_cov::Main() {
    // prepare output dir
    CreateDirRecurse(_db.cmdline.covdir);
    // redirect log to file
    if (_db.cmdline.logfile != "") {
        _db.logfd.fd = OpenWrite(_db.cmdline.logfile);
        algo_lib::_db.Prlog = Prlog;
    }
    // run command
    if (_db.cmdline.runcmd != "") {
        CleanupDirPhase(_db.cmdline.covdir,atf_cov_Phase_value_runcmd);
        setenv(algo_lib::dev_envvar_GCC_PROFILE_DIR,Zeroterm(_db.cmdline.covdir),1);
        _db.bash.cmd.c = _db.cmdline.runcmd;
        bash_ExecX(_db.bash);
    }
    // run gcov, or merge, or just load data
    if (_db.cmdline.mergepath != "") {
        // merge data
        algo::StringIter dir_it(_db.cmdline.mergepath);
        for (strptr dir = GetTokenChar(dir_it,':'); dir != ""; dir = GetTokenChar(dir_it,':')) {
            prlog("Merging "<<dir);
            if (_db.cmdline.gcov) {
                // load directly from gcov files
                CleanupDirPhase(dir,atf_cov_Phase_value_gcov);
                RunGcov(dir);
            } else {
                // load from ready-made ssim files
                ind_beg(algo::Dir_curs,ent,DirFileJoin(dir,"*.cov.ssim")) {
                    //prlog(ent.pathname);
                    ind_beg(algo::FileLine_curs,line,ent.pathname) {
                        dev::Covline covline;
                        if (Covline_ReadStrptrMaybe(covline,line)) {
                            MergeCovline(covline);
                        }
                    }ind_end;
                }ind_end;
            }
        }
    } else if (_db.cmdline.gcov) {
        // run gcov
        CleanupDirPhase(_db.cmdline.covdir,atf_cov_Phase_value_gcov);
        RunGcov(_db.cmdline.covdir);
    } else {
        // or just load ssim files
        ind_beg(algo::Dir_curs,ent,DirFileJoin(_db.cmdline.covdir,"*.cov.ssim")) {
            //prlog(ent.pathname);
            ind_beg(algo::FileLine_curs,line,ent.pathname) {
                dev::Covline covline;
                if (Covline_ReadStrptrMaybe(covline,line)) {
                    MergeCovline(covline);
                }
            }ind_end;
        }ind_end;
    }
    // write out ssimfiles
    if (_db.cmdline.ssim) {
        CleanupDirPhase(_db.cmdline.covdir,atf_cov_Phase_value_ssim);
        WriteCovSsim();
    }
    ComputeCoverage();
    if (_db.cmdline.report || _db.cmdline.capture) {
        ComputeUncovfunc();
    }
    if (_db.cmdline.report) {
        CleanupDirPhase(_db.cmdline.covdir,atf_cov_Phase_value_report);
        GenerateSsimReport();
        GenerateTxtReport();
        GenerateCoberturaReport();
        WriteUncovfunc(DirFileJoin(_db.cmdline.covdir,"uncovfunc.ssim"));
    }
    Summary();
    if (_db.cmdline.check) {
        Main_Check();
    }
    if (_db.cmdline.capture) {
        Main_Capture();
    }
}
