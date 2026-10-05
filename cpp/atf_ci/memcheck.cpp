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
// Each memcheck shard's first citest calls this, so every shard builds its own
// annotated tree; the build cache makes that a few seconds on a warm runner.
void atf_ci::BuildMemcheck() {
    SetupMemcheckLink();
    command::abt_proc abt;
    Regx_ReadSql(abt.cmd.target, "%", true);
    abt.cmd.cfg.expr = dev_Cfg_cfg_memcheck;
    abt.cmd.install = false;
    abt_ExecX(abt);
}

// -----------------------------------------------------------------------------

// Run one shard of the comptests under valgrind memcheck against the annotated
// build that BuildMemcheck produced.  CIJOB names the shard: atf_comp runs the
// comptests whose memcheck field carries it.
//
// One job running every comptest under valgrind took 47 minutes on a CI runner
// while every other job of the pipeline finished inside 20, so the suite is cut
// by target into three jobs of about 15 minutes each, and each comptest's
// memcheck attribute names the shard that runs it.
void atf_ci::RunMemcheckShard(strptr cijob) {
    command::atf_comp_proc atf_comp;
    atf_comp.cmd.maxerr = 3;
    atf_comp.cmd.cfg = dev_Cfg_cfg_memcheck;
    atf_comp.cmd.mode = command_atf_comp_mode_memcheck;
    Regx_ReadSql(atf_comp.cmd.cijob, cijob, true);
    atf_comp_ExecX(atf_comp);
}
