// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2024 AlgoRND
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
// Target: abt (exe) -- Algo Build Tool - build & link C++ targets
// Exceptions: NO
// Source: cpp/abt/ood.cpp
//

#include "include/algo.h"
#include "include/abt.h"

// -----------------------------------------------------------------------------

static void ComputeCumMod(abt::FSrcfile &srcfile) {
    if (bool_Update(srcfile.cum_mod_visited,true)) {
        zd_inclstack_Insert(srcfile);
        ind_beg(abt::srcfile_zd_include_curs, include, srcfile) {
            ComputeCumMod(*include.p_header);
            i64_UpdateMax(srcfile.cum_modtime.value, include.p_header->cum_modtime.value);
        }ind_end;
        zd_inclstack_Remove(srcfile);
    } else {
        if (zd_inclstack_InLlistQ(srcfile) & !StartsWithQ(srcfile.srcfile,"extern/")) {
            cstring out;
            strptr sep("\n    ");
            ind_beg(abt::_db_zd_inclstack_curs,s,abt::_db) {
                out<<sep<<s.srcfile;
            }ind_end;
            out<<sep<<srcfile.srcfile;
            prlog(srcfile.srcfile<<":1: abt.circular_include  "<<out);
            algo_lib::_db.exit_code=1;
        }
    }
}

// Compute cumulative modification timestamp for source files
// and targets.
// This calculates SRCFILE.CUM_MODTIME, TARGET.CUM_MODTIME
void abt::ComputeTimestamps() {
    ind_beg(abt::_db_srcfile_curs, srcfile, abt::_db) {
        srcfile.cum_modtime = GetFilestat(srcfile.srcfile).modtime;
    }ind_end;
    // compute cumulative modification time for each file
    ind_beg(abt::_db_srcfile_curs, srcfile, abt::_db) {
        ComputeCumMod(srcfile);
    }ind_end;
    // compute target cumulative modification time
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        ind_beg(abt::target_c_srcfile_curs, src,target) {
            target.cum_modtime.value = i64_Max(target.cum_modtime.value, src.cum_modtime.value);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Recalculate SRCFILE.OOD, TARGET.OOD, TARGET.OUT_MODTIME
// for given build directory BUILDDIR
void abt::ComputeOod(abt::FBuilddir &builddir) {
    ind_beg(abt::_db_zs_sel_target_curs,target,abt::_db) {
        target.ood=false;
        target.ood_visited=false;
    }ind_end;

    // compute out-of-date flag for each file
    ind_beg(abt::_db_srcfile_curs, srcfile, abt::_db) {
        abt::FFilestat &filestat = abt::GetFilestat(GetObjpath(builddir,srcfile));
        srcfile.ood  = abt::_db.cmdline.force
            || UnTime_Lt(filestat.modtime, srcfile.cum_modtime)
            || filestat.size == 0;// #AL# zero-sized file is out of date
    }ind_end;

    // compute out-of-date flag for each target (collect it from source files)
    // if a source file filter was specified, don't mark targets as out-of-date
    if (accepts_all_Get(_db.cmdline.srcfile.flags)) {
        ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
            ind_beg(abt::target_c_srcfile_curs, src,target) {
                target.ood |= src.ood;
            }ind_end;
            target.ood |= abt::_db.cmdline.force;
            tempstr outfile=GetOutfile(builddir,target);
            if (ch_N(outfile)) {
                abt::FFilestat &filestat = abt::GetFilestat(outfile);
                target.out_modtime = filestat.modtime;
                target.ood |= UnTime_Lt(filestat.modtime, target.cum_modtime)
                    || filestat.size ==0;
            }
        }ind_end;

        // mark target as out-of-date if any parent is out-of-date
        ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
            ind_beg(abt::target_c_alldep_curs, parent, target) {
                target.ood = target.ood
                    || parent.ood
                    || UnTime_Lt(target.out_modtime, parent.out_modtime);
            }ind_end;
        }ind_end;
    }

    // Show out-of-date files for current build directory
    bool realexec = !abt::_db.cmdline.dry_run && !abt::_db.cmdline.printcmd;
    int ood_src=0, ood_target=0;
    ind_beg(abt::_db_zs_sel_target_curs, target,abt::_db) {
        ood_target += target.ood;
        ind_beg(abt::target_c_srcfile_curs, srcfile, target) {
            ood_src += srcfile.ood;
        }ind_end;
    }ind_end;
    if (realexec && abt::zs_sel_target_N() > 0 && abt::_db.cmdline.report) {
        prlog(abt::config(builddir.builddir
                          ,ood_src
                          ,ood_target
                          ,(abt::_db.ccache ? "ccache" : abt::_db.gcache ? "gcache" : "none")));
    }
}
