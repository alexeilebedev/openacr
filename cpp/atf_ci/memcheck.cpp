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
// Source: cpp/atf_ci/memcheck.cpp
//

#include "include/algo.h"
#include "include/atf_ci.h"

// -----------------------------------------------------------------------------

// Make build/memcheck name the memcheck builddir, so a run can reach the annotated binaries.
//
// atf_comp names a tool as build/<cfg>/<tool>, while abt writes that tool into
// build/<uname>-<compiler>.<cfg>-<arch>, and the soft link between the two names is what
// connects them.  A tree carries one such link per configuration its bootstrap set up, and the
// bootstrap runs only when abt is absent, so a configuration introduced afterwards has no link
// on any tree that is already built -- a developer's checkout and a long-lived CI runner alike.
// abt reads the missing link for its default uname, compiler and arch, comes away with none of
// the three, and asks dev.builddir for the key -.memcheck-, which no row carries.
//
// Those three names describe the host and its toolchain rather than the configuration, so every
// configuration's link spells them the same way and the release link is a good place to read
// them.  Planting the memcheck link from them leaves the configuration usable without a second
// bootstrap.
static void SetupMemcheckLink() {
    tempstr link(tempstr()<<"build/"<<dev_Cfg_cfg_memcheck);
    tempstr release_builddir(algo::ReadLink(tempstr()<<"build/"<<dev_Cfg_cfg_release));
    if (algo::ReadLink(link) == "" && release_builddir != "") {
        tempstr uname(dev::Builddir_uname_Get(release_builddir));
        tempstr compiler(dev::Builddir_compiler_Get(release_builddir));
        tempstr arch(dev::Builddir_arch_Get(release_builddir));
        tempstr builddir(dev::Builddir_Concat_uname_compiler_cfg_arch(uname,compiler,dev_Cfg_cfg_memcheck,arch));
        algo::CreateDirRecurse(DirFileJoin("build",builddir));
        errno_vrfy_(symlink(Zeroterm(builddir),Zeroterm(link))==0);
    }
}

// -----------------------------------------------------------------------------

// First memcheck citest: build every target with the memcheck cfg, which is the
// configuration the memcheck run drives.
//
// That cfg is release with valgrind's client requests compiled in and nothing
// else changed.  The requests are what let a checker account for the records
// inside an amc pool, and they are not free when nothing is watching -- a dozen
// instructions on every allocation and every free -- so the configuration that
// ships does not carry them.  Keeping the optimization identical to release is
// the point of a separate cfg rather than reusing debug: the binary a checker
// examines is generated the way the shipped one is, so what it finds is what the
// shipped code would do.
//
// No -install, so bin/ keeps pointing at the release build; the run reaches the
// annotated binaries through atf_comp's -cfg:memcheck instead.
void atf_ci::citest_mem_prep() {
    SetupMemcheckLink();
    command::abt_proc abt;
    Regx_ReadSql(abt.cmd.target, "%", true);
    abt.cmd.cfg.expr = dev_Cfg_cfg_memcheck;
    abt.cmd.install = false;
    abt_ExecX(abt);
}

// -----------------------------------------------------------------------------

// Run every comptest under valgrind memcheck against the annotated build that
// mem_prep produced.
void atf_ci::citest_atf_comp_mem() {
    command::atf_comp_proc atf_comp;
    atf_comp.cmd.maxerr = 3;
    atf_comp.cmd.cfg = dev_Cfg_cfg_memcheck;
    atf_comp.cmd.mode = command_atf_comp_mode_memcheck;
    atf_comp_ExecX(atf_comp);
}
