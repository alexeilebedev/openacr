// Copyright (C) 2024-2026 AlgoRND
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
// Source: cpp/atf_ci/apm.cpp
//

#include "include/atf_ci.h"

static void UpdatePackage(algo::strptr name) {
    command::apm_proc apm;
    apm.cmd.package.expr=name;
    apm.cmd.checkclean=false;
    apm.cmd.update=true;
    vrfy_(apm_Exec(apm)==0);
}

static void InstallPackage(algo::strptr name) {
    command::apm_proc apm;
    apm.cmd.package.expr=name;
    apm.cmd.checkclean=false;
    apm.cmd.origin=algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm);
    apm.cmd.ref="HEAD";
    apm.cmd.install=true;
    vrfy_(apm_Exec(apm)==0);
}

static void RemovePackage(algo::strptr name) {
    command::apm_proc apm;
    apm.cmd.package.expr=name;
    apm.cmd.checkclean=false;
    apm.cmd.remove=true;
    vrfy_(apm_Exec(apm)==0);
}

static void PushPackage(algo::strptr name) {
    command::apm_proc apm;
    apm.cmd.package.expr=name;
    apm.cmd.checkclean=false;
    apm.cmd.push=true;
    apm.cmd.origin=algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm);
    vrfy_(apm_Exec(apm)==0);
}

static void ResetPackage(algo::strptr name, algo::strptr ref) {
    command::apm_proc apm;
    apm.cmd.package.expr=name;
    apm.cmd.reset=true;
    apm.cmd.ref=ref;
    apm.cmd.origin=algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm);
    vrfy_(apm_Exec(apm)==0);
}

// Return the diffstat apm reports for package NAME, without apm's own chatter.
// A diff run prints the sandbox path it worked in and a closing hint, so its
// output is never empty and cannot be compared against nothing.  Git's own
// summary line is what says whether anything differs, and it names a count of
// files: "1 file changed, 2 insertions(+)".  No such line means no difference.
static tempstr GetPackageDiffstat(algo::strptr name) {
    tempstr ret;
    tempstr out(SysEval(tempstr()<<"apm "<<name<<" -diff -stat",FailokQ(true),1024*100));
    ind_beg(algo::Line_curs,line,out) {
        if (FindStr(line," changed")!=-1) {
            ret << line << eol;
        }
    }ind_end;
    return ret;
}

// Insert specified line into acr database
static void AcrInsert(algo::strptr str) {
    strptr filename="temp/atf_ci-apm.ssim";
    StringToFile(str,filename);
    command::acr_proc acr;
    acr.cmd.insert=true;
    acr.cmd.write=true;
    acr.cmd.report=false;
    acr.fstdin<<"<"<<filename;
    vrfy(acr_Exec(acr)==0, acr_ToCmdline(acr));
}

static void AddPackageKey(algo::strptr package, algo::strptr key, bool up = false, bool down = true, bool exclude = false) {
    dev::Pkgkey pkgkey;
    pkgkey.pkgkey  = dev::Pkgkey_Concat_package_key(package,key);
    pkgkey.up      = up;
    pkgkey.down    = down;
    pkgkey.exclude = exclude;
    AcrInsert(tempstr()<<pkgkey);
}

static void RemovePackageKey(algo::strptr package, algo::strptr key) {
    dev::Pkgkey pkgkey;
    pkgkey.pkgkey = dev::Pkgkey_Concat_package_key(package,key);
    AcrInsert(tempstr()<<"acr.delete  "<<pkgkey);
}

static void AddPackageDep(algo::strptr package, algo::strptr parent, bool soft = true) {
    dev::Pkgdep pkgdep;
    pkgdep.pkgdep = dev::Pkgdep_Concat_package_parent(package,parent);
    pkgdep.soft = soft;
    // pkgdeptype keeps its default of `require`, which is ordering alone: the
    // sample packages are checked for what their own keys capture, and an
    // extend relation would subtract one of them from the other
    AcrInsert(tempstr()<<pkgdep);
}

static void CreatePackage(algo::strptr name) {
    dev::Package package;
    package.package=name;
    package.origin=".";
    package.baseref="HEAD";
    AcrInsert(tempstr()<<package);
    AddPackageKey(name, tempstr()<< "dev.package:"<<name);
}

static void CreateFile(algo::strptr package, algo::strptr filename, algo::strptr text) {
    vrfy_(algo::StringToFile(text, filename));
    vrfy_(SysCmd(tempstr()<<"git add "<<filename)==0);
    dev::Gitfile gitfile;
    gitfile.gitfile=filename;
    AcrInsert(tempstr()<<gitfile);
    AddPackageKey(package, tempstr()<<"dev.gitfile:"<<filename);
}

static void RemoveFile(algo::strptr package, algo::strptr filename) {
    vrfy_(SysCmd(tempstr()<<"git rm --force "<<filename)==0);
    dev::Gitfile gitfile;
    gitfile.gitfile=filename;
    AcrInsert(tempstr()<<"acr.delete "<<gitfile);
    RemovePackageKey(package, tempstr()<<"dev.gitfile:"<<filename);
}

static void Stage(algo::strptr notice) {
    prlog("--------------------------------------------------------------------------------");
    prlog("BEGIN: "<<notice);
}

// Check that an acr query for KEY returns a selection with N items
// TRUE if that's the case
static bool KeyPresent(algo::strptr key, int n) {
    command::acr_proc acr;
    acr.cmd.query=key;
    bool ret=false;
    acr.fstdout = "|";
    acr_Start(acr);
    ind_beg(algo::FileLine_curs,line,acr.from_stdout) {
        report::acr report;
        if (report::acr_ReadStrptrMaybe(report,line)) {
            ret=report.n_select==u32(n);
            break;
        }
    }ind_end;
    return ret;
}

void atf_ci::CitestApm() {

    // -----------------------------------------------------------------------------
    Stage("clone repo into a sandbox");
    {
        command::wt_proc wt;
        wt.cmd.name.expr=dev_Sandbox_sandbox_atf_ci_apm;
        wt.cmd.create=true;
        wt.cmd.reset=true;
        vrfy_(wt_Exec(wt)==0);
    }

    // -----------------------------------------------------------------------------
    Stage("create new package inside sandbox. add a file and a record");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        CreatePackage("sample");
        AddPackageDep("sample", "apm"); // extends apm.FDb
        CreateFile("sample", "sample_file", "some text\n");
        AcrInsert("dmmeta.field field:apm.FDb.sample_field arg:i32 reftype:Val");
        AddPackageKey("sample", "dmmeta.field:apm.FDb.sample_field%");
        vrfy_(SysCmd("git commit -am 'created sample_file'")==0);
        algo_lib::PopDir();
    }

    // -----------------------------------------------------------------------------
    Stage("install package from sandbox");
    InstallPackage("sample");
    // verify that package was installed
    vrfy_(algo::FileQ("sample_file"));
    vrfy_(algo::FileToString("sample_file")=="some text\n");
    vrfy_(KeyPresent("dev.package:sample",1));
    vrfy_(KeyPresent("dev.gitfile:sample_file",1));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_field",1));

    // -----------------------------------------------------------------------------
    Stage("add file in sandbox. perform package update");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        CreateFile("sample", "sample_file2", "more text\n");
        vrfy_(SysCmd("git commit -am 'created sample_file2'")==0);
        algo_lib::PopDir();
        UpdatePackage("sample");
    }
    vrfy_(algo::FileQ("sample_file"));
    vrfy_(algo::FileQ("sample_file2"));
    vrfy_(algo::FileToString("sample_file")=="some text\n");
    vrfy_(algo::FileToString("sample_file2")=="more text\n");

    // -----------------------------------------------------------------------------
    Stage("remove file from our version of package; add another file to package. update package - changes should stay");
    {
        RemoveFile("sample", "sample_file2");
        CreateFile("sample", "sample_file_local", "our modified file\n");
        UpdatePackage("sample");
    }
    vrfy_( algo::FileQ("sample_file"));
    vrfy_(!algo::FileQ("sample_file2"));
    vrfy_( algo::FileQ("sample_file_local"));

    // -----------------------------------------------------------------------------
    Stage("remove file from sandbox. update package - changes should propagate but local changes remain");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        RemoveFile("sample", "sample_file2");
        vrfy_(SysCmd("git commit -am 'deleted sample_file2'")==0);
        algo_lib::PopDir();
        UpdatePackage("sample");
    }
    vrfy_( algo::FileQ("sample_file"));
    vrfy_(!algo::FileQ("sample_file2"));
    vrfy_( algo::FileQ("sample_file_local"));

    // -----------------------------------------------------------------------------
    Stage("modify file in sandbox; update package - modification should show up");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        algo::StringToFile("extra line\n","sample_file",algo_FileFlags_append);
        vrfy_(SysCmd("git commit -am 'appended lines to sample_file1'")==0);
        algo_lib::PopDir();
        UpdatePackage("sample");
    }
    vrfy_( algo::FileToString("sample_file") == "some text\nextra line\n");

    // -----------------------------------------------------------------------------
    Stage("make local change in file; update package - change should stay");
    {
        algo::StringToFile("another line\n","sample_file",algo_FileFlags_append);
        UpdatePackage("sample");
    }
    vrfy_( algo::FileToString("sample_file") == "some text\nextra line\nanother line\n");

    // -----------------------------------------------------------------------------
    Stage("create a conflict in sample_file");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        algo::StringToFile("package line\n","sample_file",algo_FileFlags_append);
        vrfy_(SysCmd("git commit -am 'appended lines to sample_file1'")==0);
        algo_lib::PopDir();
        UpdatePackage("sample");
    }
    vrfy_( algo::FileToString("sample_file") == "some text\n"
           "extra line\n"
           "<<<<<<< sample_file\n"
           "another line\n"
           "=======\n"
           "package line\n"
           ">>>>>>> package\n");

    // -----------------------------------------------------------------------------
    Stage("add record in sandbox; update package - it should show up");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        AcrInsert("dmmeta.field field:apm.FDb.sample_field2 arg:algo.cstring reftype:Val");
        vrfy_(SysCmd("git commit -am 'add record'")==0);
        algo_lib::PopDir();
        UpdatePackage("sample");
    }
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_field2",1));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_fi%",2));

    // -----------------------------------------------------------------------------
    Stage("add local record; update package - record should stay");
    {
        AcrInsert("dmmeta.field field:apm.FDb.sample_field_local arg:u32 reftype:Val");
        UpdatePackage("sample");
    }
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_field_local",1));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_fi%",3));

    // -----------------------------------------------------------------------------
    Stage("show diff");
    SysCmd("apm sample -diff | hilite -d");

    // -----------------------------------------------------------------------------
    Stage("modify different attributes of local and sandbox record with same key; changes should merge");
    {
        AcrInsert("acr.merge dmmeta.field field:apm.FDb.sample_field arg:dmmeta.Ns");
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        AcrInsert("acr.merge dmmeta.field field:apm.FDb.sample_field comment:\"xyz\"");
        vrfy_(SysCmd("git commit -am 'modify reftype attribute'")==0);
        algo_lib::PopDir();
        UpdatePackage("sample");
    }
    vrfy_(SysEval("acr -report:N field:apm.FDb.sample_field",FailokQ(true),1024)
          == "dmmeta.field  field:apm.FDb.sample_field  arg:dmmeta.Ns  reftype:Val  dflt:\"\"  comment:xyz\n");

    // -----------------------------------------------------------------------------
    Stage("push local changes to the origin, adopt its commit, and read the package as synced");
    {
        // The package here holds local additions the origin has never seen: a
        // file, a record, and edits to a file the origin also edited.  Pushing
        // sends all of it, the origin commits it, and the baseref moves to that
        // commit.
        vrfy(GetPackageDiffstat("sample")!="", "the package should differ from its base before the push");
        PushPackage("sample");
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        vrfy_(SysCmd("git add -A")==0);
        vrfy_(SysCmd("git commit -q -m 'take the local version of sample'")==0);
        algo_lib::PopDir();
        ResetPackage("sample","HEAD");
    }
    // The baseref now names the commit the origin made of this tree's
    // projection, so the two sides carry the same package and the diff has
    // nothing left to report.  This is the whole point of moving the baseref
    // after a push: a baseref that stays behind describes a version the tree
    // does not match, every later merge is computed against the wrong base, and
    // nothing anywhere says so.  An empty diff straight after a sync is the one
    // observation that catches it.
    vrfy(GetPackageDiffstat("sample")=="", "the package should match its base after the push and reset");
    // and the record carries a commit id rather than the word HEAD, which
    // names no fixed version and resolves differently in every repo that reads it
    {
        tempstr out(SysEval("acr -report:N package:sample",FailokQ(true),1024));
        tempstr line(Trimmed(out));
        dev::Package package;
        vrfy_(dev::Package_ReadStrptrMaybe(package,line));
        vrfy(ch_N(package.baseref)==40, tempstr()<<"baseref should be a commit id, got "<<package.baseref);
    }

    // -----------------------------------------------------------------------------
    Stage("create sample2 package with sample as parent");
    {
        algo_lib::PushDir(algo_lib::WtDir(dev_Sandbox_sandbox_atf_ci_apm));
        CreatePackage("sample2");
        CreateFile("sample2", "sample2_file", "some text for sample2 package");
        AddPackageKey("sample2","dmmeta.field:apm.FDb.sample2%");
        AcrInsert("dmmeta.field field:apm.FDb.sample2_field arg:i32 reftype:Val");
        AddPackageDep("sample2","sample",false);
        vrfy_(SysCmd("git commit -am 'created sample2'")==0);
        algo_lib::PopDir();
        InstallPackage("sample2");
    }
    // check that new package was installed
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample2_field",1));
    vrfy_(algo::FileQ("sample2_file"));

    // -----------------------------------------------------------------------------
    Stage("remove base package -- all files, fields, and local changes go away");
    {
        RemovePackage("sample");
    }
    vrfy_(!algo::FileQ("sample_file"));
    vrfy_(!algo::FileQ("sample_file2"));
    vrfy_(!algo::FileQ("sample2_file"));
    vrfy_(!algo::FileQ("sample_file_local"));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_field%",0));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample2_field",0));

    // -----------------------------------------------------------------------------
    Stage("reinstall sample2 (child package) and check that the base package was installed as well");
    {
        InstallPackage("sample2");
    }
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_field",1));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample2_field",1));
    vrfy_( algo::FileQ("sample_file"));
    vrfy_( algo::FileQ("sample2_file"));

    // -----------------------------------------------------------------------------
    Stage("remove packages one by one");
    {
        RemovePackage("sample2");
        RemovePackage("sample");
    }
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample_field",0));
    vrfy_(KeyPresent("dmmeta.field:apm.FDb.sample2_field",0));
    vrfy_(!algo::FileQ("sample_file"));
    vrfy_(!algo::FileQ("sample2_file"));

    // -----------------------------------------------------------------------------

    Stage("final check - back to original state");
    algo::SysCmd("git add -u");
    vrfy(Trimmed(algo::SysEval("git ls-files -m",FailokQ(false),100))=="", "not all files were removed");
}

// -----------------------------------------------------------------------------

void atf_ci::CitestApmReinstall() {
    // for each package, remove it, check that build succeeds, add it back (from current commit), and rebuild again
    ind_beg(_db_cipackage_curs,cipackage,_db) {
        // remove package
        if (cipackage.remove) {
            prlog("# --- remove/reinstall "<<cipackage.package<<" ---");
            command::apm_proc apm;
            apm.cmd.package.expr=cipackage.package;
            apm.cmd.checkclean=false;
            apm.cmd.remove=true;
            apm_ExecX(apm);

            if (cipackage.build) {
                command::abt_proc abt;
                abt.cmd.target.expr="%";
                abt.cmd.build=true;
                abt_ExecX(abt);
            }

            if (cipackage.reinstall != "") {
                // reinstall package
                apm.cmd.remove=false;
                apm.cmd.package.expr=cipackage.reinstall;
                apm.cmd.install=true;
                apm.cmd.checkclean=false;
                apm.cmd.origin=".";
                apm.cmd.ref="HEAD";
                apm_ExecX(apm);
            }
        }

        // should round trip cleanly
        atf_ci::CheckCleanDirs();
    }ind_end;
}
