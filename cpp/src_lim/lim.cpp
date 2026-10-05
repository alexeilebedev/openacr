// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: src_lim (exe) -- Refuse a bad line pattern and a stray source file
// Exceptions: yes
// Source: cpp/src_lim/lim.cpp
//

#include "include/algo.h"
#include "include/gen/src_lim_gen.h"
#include "include/gen/src_lim_gen.inl.h"

// -----------------------------------------------------------------------------

static bool SrcfileQ(src_lim::FGitfile &gitfile) {
    return ext_Get(gitfile) == "cpp";
}

static void StraySrc() {
    int nerr=0;
    ind_beg(src_lim::_db_gitfile_curs,gitfile,src_lim::_db) {
        verblog("src_lim.normcheck_srcfile"
                <<Keyval("gitfile",gitfile.gitfile)
                <<Keyval("targsrc", Bool(gitfile.c_targsrc!=0))
                <<Keyval("srcfileq", SrcfileQ(gitfile)));
        if (!gitfile.c_targsrc && SrcfileQ(gitfile)) {
            prlog("src_lim.stray_srcfile"
                  <<Keyval("fname",gitfile.gitfile)
                  <<Keyval("comment","Source file not part of any target (not in targsrc table)"));
            nerr++;
        }
    }ind_end;
    vrfy(nerr==0,"src_lim.stray_files  comment:'Please delete these files or add them to targsrc table'");
}

// -----------------------------------------------------------------------------

static void CheckPerms() {
    int nbad=0;
    ind_beg(src_lim::_db_targsrc_curs,targsrc,src_lim::_db) {
        struct stat st;
        algo::ZeroBytes(st);
        (void)stat(Zeroterm(tempstr()<<src_Get(targsrc)),&st);
        if (st.st_mode & 0111) {
            nbad++;
            prerr("src_lim.badperms"
                  <<Keyval("gitfile",src_Get(targsrc))
                  <<Keyval("comment","source files should not be executable"));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void StrayInclude() {
    command::abt abt;
    abt.target.expr = "%";
    abt.listincl =true;
    cstring retval(SysEval(abt_ToCmdline(abt),FailokQ(true),1024*1024*10,EchoQ(false)));
    ind_beg(Line_curs,line,retval) {
        src_lim::InsertStrptrMaybe(line);
    }ind_end;
    prlog("src_lim.stray_include_1"
          <<Keyval("n_include", src_lim::include_N())
          <<Keyval("n_gitfile", src_lim::gitfile_N()));
    int nerr=0;
    ind_beg(src_lim::_db_include_curs,include,src_lim::_db) if (!include.sys) {    // check that every include site refers to a versioned header.
        src_lim::FGitfile *hdrfile=src_lim::ind_gitfile_Find(filename_Get(include));
        // A header under build/ is a build product rather than a source file, so
        // no checkout can carry it and this check does not apply.  The one that
        // exists is build/gitinfo.h, where abt records the commit a binary came
        // from and the moment it was built; its include site guards itself with
        // __has_include, so a tree that has never been built still compiles.
        bool product = StartsWithQ(filename_Get(include),"build/");
        if (hdrfile) {
            zd_include_Insert(*hdrfile,include);
        } else if (!product) {
            prlog("src_lim.unversioned_include"
                  <<Keyval("srcfile",srcfile_Get(include))
                  <<Keyval("hdrfile",filename_Get(include))
                  <<Keyval("comment","Header file not versioned"));
            nerr++;
        }
    }ind_end;
    // Check that every versioned include has at least one include site
    ind_beg(src_lim::_db_gitfile_curs,gitfile,src_lim::_db) {
        bool is_header = ext_Get(gitfile)=="h";
        bool no_sites = zd_include_EmptyQ(gitfile);
        bool not_extern = !StartsWithQ(gitfile.gitfile,"extern/");
        if (is_header && no_sites && not_extern) {
            prlog("src_lim.stray_include"
                  <<Keyval("fname",gitfile.gitfile)
                  <<Keyval("comment","Include file is not included anywhere."));
            nerr++;
        }
    }ind_end;
    vrfy(nerr==0,"src_lim.stray_files  comment:'Please delete these files or add them to targsrc table'");
}

// -----------------------------------------------------------------------------

// TRUE when FNAME is a plain file.  A tracked symlink, such as bin/atf_comp,
// points at a built binary, and its target is not content of the tree.
static bool RegularFileQ(strptr fname) {
    StatStruct st;
    return lstat(Zeroterm(tempstr() << fname), &st) == 0 && S_ISREG(st.st_mode);
}

// Return the text that every line EXPR matches must contain: the literal run
// that follows a leading .* in the expression, up to its first operator, or the
// empty string when there is none.  A character that a following ? or * makes
// optional is not part of the run.  Testing a line for the run is a substring
// search, which is cheap next to running the whole expression, so a line
// without it skips the expression.
static tempstr GetExprLiteral(strptr expr) {
    tempstr ret;
    int i = StartsWithQ(expr, ".*") ? 2 : 0;
    bool done = false;
    for (; i < expr.n_elems && !done; i++) {
        char c = expr[i];
        if (strchr("[](){}.*+?|\\^$", c)) {
            if ((c == '?' || c == '*') && ch_N(ret) > 0) {
                ch_RemoveLast(ret);
            }
            done = true;
        } else {
            ret << c;
        }
    }
    return ret;
}

// Check every line of FNAME against the selected badlines whose path regx
// takes FNAME and whose exempt regx does not, and print each hit with its
// location.  A hit sets a nonzero exit code; a line that carries
// ignore:<badline> is not a hit for that rule.
static void CheckBadline(strptr fname) {
    src_lim::c_badline_RemoveAll();
    ind_beg(src_lim::_db_badline_curs,badline,src_lim::_db) if (badline.select) {
        if (Regx_Match(badline._gitfile_regx,fname)
            && !(badline.exempt_regx != "" && Regx_Match(badline._exempt_regx,fname))) {
            src_lim::c_badline_Insert(badline);
        }
    }ind_end;
    if (src_lim::c_badline_N() > 0) {
        algo_lib::MmapFile file;
        MmapFile_Load(file,fname);
        ind_beg(algo::Line_curs, line, file.text) {
            ind_beg(src_lim::_db_c_badline_curs,badline,src_lim::_db) {
                if ((ch_N(badline._literal) == 0 || FindStr(line, badline._literal) != -1)
                    && Regx_Match(badline.regx, line)
                    && FindStr(line, tempstr()<<"ignore:"<<badline.badline)==-1) {
                    tempstr loc = tempstr() << fname<<":"<<ind_curs(line).i+1<<": ";
                    prlog(loc << line);
                    prlog("  "<<loc<<": src_lim.badline"
                          <<Keyval("badline",badline.badline)
                          <<Keyval("comment",tempstr()<<badline.comment));
                    algo_lib::_db.exit_code=1;
                }
            }ind_end;
        }ind_end;
    }
}

// Check the tracked files that -srcfile selects against the badlines -badline
// selects.  A badline is a line pattern that must not appear in a tracked
// file, such as a C++ idiom the tree forbids or a credential.  Each rule scopes
// itself by path, so the walk covers every dev.gitfile except generated code
// and extern/, and a rule whose path regx takes no file costs nothing.
static void CheckBadline() {
    int nselect=0;
    ind_beg(src_lim::_db_badline_curs,badline,src_lim::_db) {
        badline.select = Regx_Match(src_lim::_db.cmdline.badline,badline.badline);
        nselect += badline.select;
        (void)Regx_ReadDflt(badline.regx, badline.expr);// full regx
        badline._literal = GetExprLiteral(badline.expr);
        (void)Regx_ReadSql(badline._gitfile_regx, badline.gitfile_regx, true);// full regx
        (void)Regx_ReadSql(badline._exempt_regx, badline.exempt_regx, true);// full regx
    }ind_end;
    if (nselect) {
        algo_lib::Regx exclude;
        Regx_ReadSql(exclude, "(%/gen/%|extern/%)", true);
        ind_beg(src_lim::_db_gitfile_curs,gitfile,src_lim::_db) {
            if (Regx_Match(src_lim::_db.cmdline.srcfile, gitfile.gitfile)
                && !Regx_Match(exclude, gitfile.gitfile)
                && RegularFileQ(gitfile.gitfile)) {
                CheckBadline(gitfile.gitfile);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void src_lim::Main() {
    // select targsrc for processing
    algo_lib::Regx exclude;
    Regx_ReadSql(exclude, "(%/gen/%|extern/%)", true);
    ind_beg(src_lim::_db_targsrc_curs,targsrc,src_lim::_db) {
        targsrc.select = algo_lib::Regx_Match(src_lim::_db.cmdline.srcfile, src_Get(targsrc))
            && !algo_lib::Regx_Match(exclude, src_Get(targsrc));
    }ind_end;

    if (src_lim::_db.cmdline.strayfile) {
        StraySrc();
        StrayInclude();
        CheckPerms();
    }
    if (src_lim::_db.cmdline.badline.expr != "") {
        CheckBadline();
    }
}
