// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: atf_cov (exe) -- Line coverage
// Exceptions: yes
// Source: cpp/atf/atf_cov.cpp
//

#include "include/algo.h"
#include "include/atf_cov.h"

static void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) {
    (void)logcat;
    (void)tstamp;
    WriteFileX(atf_cov::_db.logfd.fd,strptr_ToMemptr(str));
}

static tempstr GetGcovDstDir(strptr name) {
    tempstr dir(".");
    int pos = FindStr(name,"##");
    if (pos >=0) {
        dir = ch_FirstN(name,pos);
        Translate(dir,"#","/");
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

static void DeleteFiles(strptr dir, strptr pattern) {
    ind_beg(algo::Dir_curs,ent,DirFileJoin(dir,pattern)) {
        DeleteFile(ent.pathname);
    }ind_end;
}

static void CleanupDirPhase(strptr covdir, atf_cov_Phase_value_Enum phase) {
    if (phase == atf_cov_Phase_value_runcmd) {
        DeleteFiles(covdir,"*.gcda");
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
    // cleanup current dir from .gcov files
    ind_beg(algo::Dir_curs,ent,"*.gcov") {
        cstring dst_dir = GetGcovDstDir(ent.filename);
        cstring dst_dir_full = GetFullPath(dst_dir);
        if (dst_dir_full == covdir_full) {
            DeleteFile(ent.filename);
        }
    }ind_end;
    // walk .gcda, prepare .gcno, run gcov - produce .gcov
    ind_beg(algo::Dir_curs,ent,DirFileJoin(covdir,"*.gcda")) {
        cstring gcno_dst_path = ReplaceExt(ent.pathname,".gcno");
        cstring gcno_src_path = ReplaceExt(ent.filename,".gcno"); // mangled
        Translate(gcno_src_path,"#","/"); // demangle
        errno_vrfy_(symlink(Zeroterm(gcno_src_path),Zeroterm(gcno_dst_path))==0);
        tempstr cmd;
        cmd << "gcov -p -l " << ent.pathname;
        if (ValidQ(_db.logfd.fd)) {
            cmd << " >&" << _db.logfd.fd.value << " 2>&" << _db.logfd.fd.value;
        }
        SysCmd(cmd);
    }ind_end;
    // walk .ccov in current directory, move relevant to covdir
    ind_beg(algo::Dir_curs,ent,"*.gcov") {
        prlog(ent.filename);
        cstring dst_dir = GetGcovDstDir(ent.filename);
        cstring dst_dir_full = GetFullPath(dst_dir);
        cstring dst_path = DirFileJoin(covdir,ent.filename);
        if (dst_dir_full == covdir_full && dst_dir_full != algo::GetCurDir()) {
            prlog(dst_path);
            errno_vrfy_(rename(Zeroterm(ent.pathname),Zeroterm(dst_path))==0);
        }
    }ind_end;
    // walk gcov files in covdir, parse and fill in database, merge lines if needed
    ind_beg(algo::Dir_curs,ent,DirFileJoin(covdir,"*.gcov")) {
        cstring src;
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

void atf_cov::Main_Check() {
    ind_beg(_db_target_curs,target,_db) if (target.c_tgtcov) {
        algo::U32Dec2 cov = target.c_covtarget ? target.c_covtarget->cov : algo::U32Dec2(0);
        algo::U32Dec2 cov_plus_maxerr(cov + target.c_tgtcov->maxerr);
        if (cov_plus_maxerr < target.c_tgtcov->cov_min) {
            prlog("atf_cov.coverage_lowered"
                  <<Keyval("target",target.target)
                  <<Keyval("success","N")
                  <<Keyval("cov_min",target.c_tgtcov->cov_min)
                  <<Keyval("maxerr",target.c_tgtcov->maxerr)
                  <<Keyval("cov_measured",cov)
                  <<Keyval("comment",""));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

void atf_cov::Main_Capture() {
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
        setenv("GCC_PROFILE_DIR",Zeroterm(_db.cmdline.covdir),1);
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
    if (_db.cmdline.report) {
        CleanupDirPhase(_db.cmdline.covdir,atf_cov_Phase_value_report);
        GenerateSsimReport();
        GenerateTxtReport();
        GenerateCoberturaReport();
    }
    Summary();
    if (_db.cmdline.check) {
        Main_Check();
    }
    if (_db.cmdline.capture) {
        Main_Capture();
        SaveCov();
    }
}
