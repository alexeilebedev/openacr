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
// Target: atf_norm (exe) -- Run normalization tests (see normcheck table)
// Exceptions: yes
// Source: cpp/atf/norm/bootstrap.cpp
//

#include "include/atf_norm.h"

// -----------------------------------------------------------------------------

// Create a bootstrap file for each build dir
void atf_norm::normcheck_bootstrap() {
    ind_beg(atf_norm::_db_builddir_curs,builddir,atf_norm::_db) {
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
            Ins(&R, text, "echo '# this script has been created using 'atf_norm bootstra''");
            Ins(&R, text, "echo '# now building an abt executable which will build the rest'");
            Ins(&R, text, "mkdir -p build");
            Ins(&R, text, "set -e");
            Ins(&R, text, "if [ ! -f .ffroot ]; then echo 'Missing .ffroot. Wrong directory?'; fi");

            // create soft links that look like this
            // lrwxr-xr-x    1 alexei  staff    29 May 16 11:02 release -> Darwin-clang++.release-x86_64
            // lrwxr-xr-x    1 alexei  staff    27 May 16 11:02 debug -> Darwin-clang++.debug-x86_64
            ind_beg(atf_norm::_db_cfg_curs,cfg,atf_norm::_db) if (cfg.cfg != "") {
                Set(R,"$thiscfg",cfg.cfg);
                Set(R,"$thisdir",dev::Builddir_Concat_uname_compiler_cfg_arch(uname_Get(builddir)
                                                                              ,compiler_Get(builddir)
                                                                              ,cfg.cfg
                                                                              ,arch_Get(builddir)));
                Ins(&R, text, "");
                Ins(&R, text, "echo '# setting up soft link build/$thiscfg (default)'");
                Ins(&R, text, "rm -f build/$thiscfg");
                Ins(&R, text, "ln -s $thisdir build/$thiscfg");
            }ind_end;

            command::abt_proc abt;
            abt.cmd.uname       = uname_Get(builddir);
            abt.cmd.compiler    = compiler_Get(builddir);
            abt.cmd.arch        = arch_Get(builddir);
            abt.cmd.cfg         = cfg_Get(builddir);
            abt.cmd.install     = true;
            abt.cmd.target.expr = dmmeta_Ns_ns_abt;
            abt.cmd.printcmd    = true;
            abt.cmd.report      = false;// stay quiet
            abt.cmd.build       = false;
            algo::StringToFile(text,tempfile.filename);
            abt.stdout << ">>" << tempfile.filename;// append
            abt_ExecX(abt);

            // atomically move new bootstrap file into final location
            prlog("# create "<<bsfile);
            (void)rename(Zeroterm(tempfile.filename),Zeroterm(bsfile));
            (void)chmod(Zeroterm(bsfile),0755);
        }
    }ind_end;
}
