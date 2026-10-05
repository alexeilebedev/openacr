// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/main.cpp
//

#include "include/algo.h"
#include "include/lib_git.h"
#include "include/apm.h"

// Initialize zd_sel_package list based on the command line regex
// For -update, -install -- select parent packages as well
// For parents, dependencies marked as 'soft' are not followed.
// These dependencies are used to establish proper package order for the purposes
// of determining which file/record belongs to which package (i.e. everything depends
// on openacr package) but not for installation. We don't automatically update base
// openacr package when a child package is updated.
void apm::Main_SelectPackage() {
    ind_beg(_db_package_curs,package,_db) {
        if (Regx_Match(_db.cmdline.package,package.package)) {
            zd_sel_package_Insert(package);
        }
    }ind_end;
    // transitively extend selection
    int n_select=zd_sel_package_N();
    // a package may have local dependencies (specified in pkgdep)
    // but these are irrelevant for the purpose of updating, because it's the
    // remote side that determines what dependencies to add in that case.
    // when installing, there are no dependencies because the package record has
    // just been created
    if (_db.cmdline.t) {
        ind_beg(_db_zd_sel_package_curs,package,_db) {
            ind_beg(package_c_pkgdep_curs,pkgdep,package) if (!pkgdep.soft) {
                // newly selected package will be visited by enclosing iteration
                zd_sel_package_Insert(*pkgdep.p_parent);
            }ind_end;
        }ind_end;
    }
    if (zd_sel_package_N()>n_select) {
        algo::ListSep ls;
        tempstr out;
        ind_beg(_db_zd_sel_package_curs,package,_db) {
            out<<ls<<package.package;
        }ind_end;
        verblog("apm: selected packages: "<<out);
    }
}

// -----------------------------------------------------------------------------

// Check out package contents into a sandbox
// Return the wait status of the checkout, zero on success
// This is the pristine version of the package (as specified with the gitref)
// If BASEREF is an empty string, then the entire current directory, with whatever
// local changes, is copied to the sandbox instead
// A checkout that fails leaves the sandbox directory absent or holding the wrong
// commit, and every reader of that directory afterwards reads a version of the
// package nobody asked for. The failure is reported here, naming the sandbox and
// the ref, and the status is returned for the caller to act on.
int apm::CreatePackageSandbox(algo::strptr sandbox_name, algo::strptr baseref) {
    int rc=0;
    command::wt_proc wt;
    wt.cmd.create=true;
    wt.cmd.q=true;
    wt.cmd.name.expr=sandbox_name;
    if (baseref == "") {
        wt.cmd.reset=true;
    } else {
        // the sandbox shares the repo's object store, so baseref is
        // resolvable inside it without any fetch
        cmd_Alloc(wt.cmd) << "git";
        cmd_Alloc(wt.cmd) << "reset";
        cmd_Alloc(wt.cmd) << "-q";
        cmd_Alloc(wt.cmd) << "--hard";
        cmd_Alloc(wt.cmd) << baseref;
    }
    rc= wt_Exec(wt);
    if (rc!=0) {
        prerr("apm.sandbox_create"
              <<Keyval("sandbox",sandbox_name)
              <<Keyval("baseref",baseref)
              <<Keyval("status",algo::DescribeWaitStatus(rc))
              <<Keyval("comment","package sandbox could not be checked out"));
    }
    return rc;
}

// -----------------------------------------------------------------------------

// Return the commit REV names, or the empty string when it names nothing here.
// Rev-parse is quiet and verifying, so an unknown name is an empty answer
// rather than a message on stderr the caller has no use for.
tempstr apm::RevParseMaybe(algo::strptr rev) {
    tempstr cmd(tempstr()<<"git rev-parse -q --verify "<<strptr_ToBash(tempstr()<<rev<<"^{commit}"));
    tempstr out(SysEval(cmd,FailokQ(true),1024));
    return tempstr(Trimmed(out));
}

// -----------------------------------------------------------------------------

// Return the destination of PACKAGE that this run acts on, or NULL when the
// package has none, which is a package that lives in this tree.  A package syncs
// with each of its dev.pkgupstream rows separately, and -dest names the row by
// its dest.  Without -dest the package's only row is the one; a package with
// several rows refuses, since picking one would merge or publish against an
// origin nobody named.
apm::FPkgupstream *apm::GetPkgupstream(apm::FPackage &package) {
    apm::FPkgupstream *ret = NULL;
    int nmatch = 0;
    ind_beg(apm::package_c_pkgupstream_curs, pkgupstream, package) {
        if (_db.cmdline.dest == "" || dest_Get(pkgupstream) == _db.cmdline.dest) {
            ret = &pkgupstream;
            nmatch++;
        }
    }ind_end;
    vrfy(nmatch <= 1, tempstr() << "apm.ambiguous_dest"
         << Keyval("package", package.package)
         << Keyval("ndest", nmatch)
         << Keyval("comment", "the package has several destinations; name one with -dest"));
    return ret;
}

// Return the namespace under refs/apm/ into which PACKAGE's origin is fetched:
// <package>/<dest> for a package with a destination, so that two origins of one
// package never overwrite each other's refs, and the package name otherwise.
tempstr apm::GetRefns(apm::FPackage &package) {
    apm::FPkgupstream *pkgupstream = GetPkgupstream(package);
    return tempstr() << (pkgupstream ? algo::strptr(pkgupstream->pkgupstream) : algo::strptr(package.package));
}

// -----------------------------------------------------------------------------

// Bring ORIGIN into this repo under refs/apm/REFNAME and resolve REF to a commit
// id there.
// Return the commit, or the empty string when the origin cannot be reached or
// REF names nothing in it.
//
// A baseref has to survive being written down, so it is a commit id rather than
// a branch name.  But `git fetch <origin> <commit>` fails: both the local
// transport and an ordinary server match a refspec by name against the refs the
// origin advertises, and a commit id is not one of them.  Fetching a stored
// baseref directly therefore dies with "couldn't find remote ref" on the one
// value the field is supposed to hold.
//
// The origin's heads come across as a set instead, into the ref namespace
// REFNAME, and REF is resolved locally afterwards.  Any commit an origin
// branch reaches is then in this repo's object store and rev-parse finds it, so
// a commit id, a branch name and HEAD all resolve through one path.  The
// namespace is consulted before the repo, because REF names something in the
// origin and a local branch of the same spelling is a different commit.
tempstr apm::FetchPackageOrigin(algo::strptr refname, algo::strptr origin, algo::strptr ref) {
    tempstr gitref;
    if (origin == ".") {
        if (ref == "HEAD") {
            gitref=ref;
        } else {
            gitref=Trimmed(SysEval(tempstr()<<"git rev-parse "<<ref,FailokQ(true),1024));
        }
    } else {
        tempstr refns(tempstr()<<"refs/apm/"<<refname);
        tempstr fetch(tempstr()<<"git fetch -q --prune "<<strptr_ToBash(origin));
        fetch << " " << strptr_ToBash(tempstr()<<"+refs/heads/*:"<<refns<<"/*");
        fetch << " " << strptr_ToBash(tempstr()<<"+HEAD:"<<refns<<"/HEAD");
        int rc=SysCmd(fetch);
        if (rc==0) {
            gitref=RevParseMaybe(tempstr()<<refns<<"/"<<ref);
            if (gitref=="") {
                gitref=RevParseMaybe(ref);
            }
        }
    }
    verblog("apm.fetch_package_origin"
            <<Keyval("origin",origin)
            <<Keyval("ref",ref)
            <<Keyval("result",gitref));
    return gitref;
}

// Execute any commands accumulated in _DB.SCRIPT
// if -dry_run, print it to the screen
// Script is reset after the run
void apm::Main_Transaction() {
    int rc=0;
    if (_db.cmdline.dry_run) {
        // A dry run reports the plan instead of running it, so it writes no
        // script: the plan is the whole output, and a temp/ that is absent
        // or unwritable must not be able to suppress it. Printing the plan
        // and then failing on a file the run never reads would leave the
        // caller with no plan and a nonzero exit.
        // What a dry run does not promise is an untouched tree. An update
        // computes its plan from a three-way merge of the package's records,
        // so a dry run of one still fetches the package, creates the two
        // sandboxes the merge compares, and leaves the record files it read
        // under temp/. The plan itself names one of those files when the
        // merge conflicts, and the reader is told to edit it, so the files
        // are part of the plan rather than a side effect of running it.
        prlog(_db.script);
    } else {
        // bash runs the file, not the string, so an unwritten script means bash
        // runs whatever a previous transaction left at that path -- or nothing at
        // all -- while the transaction reports success. The run is skipped instead.
        bool saved = algo::SaveFile(_db.script,_db.scriptfile,"apm.script_write","transaction script could not be written; the transaction was not run",0755);
        if (!saved) {
            algo_lib::_db.exit_code++;
        } else {
            command::bash_proc bash;
            bash.cmd.c=_db.scriptfile;
            rc=bash_Exec(bash);
        }
    }
    _db.script="";
    if (rc!=0) {
        // bash_Exec hands back the raw wait status of the transaction script,
        // and a wait status is not an exit code: a script that exits 1 yields
        // the status 256, whose low eight bits are zero, and a script killed
        // by SIGTERM yields 15, the same word a script calling exit(15) would
        // produce. A caller that reads apm's exit code to decide what to do
        // next needs the status decoded first, which is what ExportWaitStatus
        // does -- the script's own code for an exit, 128 plus the signal for a
        // death by signal. The table is pinned by comptest apm.TransactionExit.
        algo_lib::ExportWaitStatus(rc);
    }
}

// Return the apm binary that evaluates a package inside directory DIR.
// Evaluating a package means loading that directory's dmmeta tables, and a
// directory under sync holds tables of a different vintage than ours.  Openacr
// carries dmmeta.Cdflt.jsdflt, a field this tree does not have, so our binary
// stops on the first line of its cdflt.ssim and reads nothing at all.  A tree's
// own apm is compiled against that tree's schema, which makes it the one binary
// guaranteed to read it, so prefer it wherever DIR has one.  Ours answers for
// the two cases that leaves: a directory carrying package files but no openacr
// tooling, which is how apm serves a plain submodule, and a run under -l, where
// the package definition being evaluated is ours and only our binary parses it.
// Command line field BINPATH names the subdirectory the binaries sit in.
tempstr apm::GetApmPath(algo::strptr dir) {
    tempstr ours(DirFileJoin(algo::GetCurDir(),DirFileJoin(_db.cmdline.binpath,"apm")));
    tempstr theirs(DirFileJoin(algo::GetCurDir(),DirFileJoin(dir,DirFileJoin(_db.cmdline.binpath,"apm"))));
    return (_db.cmdline.l || !FileQ(theirs)) ? ours : theirs;
}

// Return the mode bits of FILENAME, which are zero when it does not exist.  A
// symbolic link reports its own mode, because git tracks the link: a link whose
// target is not built yet still exists, and one whose target is a binary is
// still a link.
int apm::GetFileMode(algo::strptr filename) {
    struct stat struct_stat;
    algo::ZeroBytes(struct_stat);
    int rc=lstat(Zeroterm(tempstr()<<filename),&struct_stat);
    (void)rc;
    return struct_stat.st_mode;
}

// Insert package parents, then package itself
// into zd_topo_package list
static bool VisitPackage(apm::FPackage &package) {
    bool ret=true;
    if (package.visited) {
        ret=false;
        prerr("apm.error"
              <<Keyval("package",package.package)
              <<Keyval("comment","circular dependency in package definition"));
    }
    if (ret) {
        package.visited=true;
        ind_beg(apm::package_c_pkgdep_curs,pkgdep,package) {
            ret = ret && VisitPackage(*pkgdep.p_parent);
            if (!ret) {
                prerr("    included from "<<package.package);
                break;
            }
        }ind_end;
        zd_topo_package_Insert(package);
        package.visited=false;
    }
    return ret;
}

// Retrun regx of selected packages
tempstr apm::SelPackageRegx() {
    tempstr regx_package("(");
    algo::ListSep ls("|");
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        regx_package<<ls<<package.package;
    }ind_end;
    regx_package<<")";
    return regx_package;
}

// Topologicaly sort packages by dependency into zd_topo_package list
void apm::SortPackages() {
    ind_beg(_db_package_curs,package,_db) {
        vrfy(VisitPackage(package),"package dependency check failed");
    }ind_end;
}

// Open selected package definitions for editing
void apm::Main_Edit() {
    vrfy(zd_sel_package_N()>0, "no packages selected");

    command::acr_proc acr;
    acr.cmd.query <<"dev.package:"<<SelPackageRegx();
    acr.cmd.t=true;
    acr.cmd.e=true;
    _db.script << acr_ToCmdline(acr);
}

// Definte fake packages based on 'ns' regx
void apm::DefPackages() {
    if (_db.cmdline.ns.expr != "") {
        ind_beg(apm::_db_ns_curs,ns,_db) {
            if (Regx_Match(_db.cmdline.ns,ns.ns)) {
                dev::Package package;
                package.package=ns.ns;
                package.comment.value=tempstr()<<"package for namespace "<<ns.ns;
                if (package_InsertMaybe(package)) {
                    dev::Pkgdep pkgdep;
                    pkgdep.pkgdep=dev::Pkgdep_Concat_package_parent(ns.ns,"openacr");
                    pkgdep.soft=true;
                    pkgdep_InsertMaybe(pkgdep);

                    // A namespace record on its own names nothing but itself, and the
                    // files and ctypes a caller means by "namespace algo_lib" are reached
                    // from it by following references.  Every pkgkey written by hand in
                    // the tree therefore sets down, and a fabricated one that leaves it
                    // clear selects two records and no files at all.
                    dev::Pkgkey pkgkey;
                    pkgkey.down=true;
                    pkgkey.pkgkey=tempstr()<<package.package<<"/dev.package:"<<package.package;
                    pkgkey_InsertMaybe(pkgkey);
                    pkgkey.pkgkey=tempstr()<<package.package<<"/dmmeta.ns:"<<package.package;
                    pkgkey_InsertMaybe(pkgkey);
                }
            }
        }ind_end;
        Regx_ReadSql(_db.cmdline.package,_db.cmdline.ns.expr,true);
    }
}

void apm::Main() {
    // load additional file (with package definitions presumably)
    if (_db.cmdline.pkgdata != "") {
        pkgdep_RemoveAll();
        pkgkey_RemoveAll();
        package_RemoveAll();
        bool ok = apm::LoadTuplesFile(_db.cmdline.pkgdata,true);
        vrfy(ok, tempstr()<<"apm.load_error"
             <<Keyval("pkgdata",_db.cmdline.pkgdata)
             <<Keyval("comment",algo_lib::DetachBadTags()));
    }
    // if a namespace was specified, force use of local package definition
    if (_db.cmdline.ns.expr != "") {
        _db.cmdline.l=true;
    }
    // convert -ns to a package definition
    DefPackages();
    // topologically sort packages
    SortPackages();
    // a push makes the origin one projection: the package with the packages it
    // builds on, on the record side and the file side alike
    if (_db.cmdline.push) {
        _db.cmdline.t=true;
    }
    Main_SelectPackage();
    ind_beg(_db_ssimreq_curs,ssimreq,_db) {
        Regx_ReadAcr(ssimreq.regx_value,value_Get(ssimreq),true);
    }ind_end;
    _db.scriptfile << "temp/apm.script.sh";
    _db.ours_recfile << "temp/apm.ours.ssim";
    _db.base_recfile << "temp/apm.base.ssim";
    _db.theirs_recfile << "temp/apm.theirs.ssim";
    _db.merged_recfile << "temp/apm.merged.ssim";
    _db.pkgdata_recfile << "temp/apm.pkgdata.ssim";// output
    _db.base_sandbox="apm-base";
    _db.theirs_sandbox="apm-theirs";
    if (_db.cmdline.l) {
        SavePackageDefs(_db.pkgdata_recfile);
    }
    ind_beg(_db_ssimreq_curs,ssimreq,_db) {
        ssimreq.exclude=StartsWithQ(ssimreq.ssimreq,"dev.gitfile:data/");
    }ind_end;
    vrfy(_db.cmdline.install + _db.cmdline.update + _db.cmdline.push + _db.cmdline.diff + _db.cmdline.showrec
         + _db.cmdline.showfile + _db.cmdline.generate + _db.cmdline.e + _db.cmdline.reset + _db.cmdline.publish <= 1,
         "more than one action selected");
    if (_db.cmdline.annotate != "" || _db.cmdline.check || _db.cmdline.diff || _db.cmdline.showrec || _db.cmdline.push
        || _db.cmdline.showfile || _db.cmdline.generate || _db.cmdline.update || _db.cmdline.install) {
        LoadRecs();
    }
    bool needlock=_db.cmdline.update || _db.cmdline.install;
    algo_lib::FLockfile lockfile;
    if (needlock) {
        LockFileInit(lockfile, "temp/apm.lock");
    }
    // An update deletes every base record and then re-inserts the merged ones.
    // If the re-insert fails and the script runs on, the gitfile refresh after
    // it succeeds, and the script's status is the status of that last command:
    // apm reports a package updated whose rows are gone.  So every plan stops at
    // its first failing command, and apm exits with that command's status.
    _db.script << "set -e" << eol;
    if (algo_lib::_db.cmdline.verbose) {
        _db.script << "set -x" << eol;
    }
    // A -dest that names no row would leave the package looking as though it
    // lived here, and an update would then merge against this tree itself.  So
    // every package the command names must have the destination, except where
    // -reset or -install creates it.
    if (_db.cmdline.dest != "" && !_db.cmdline.reset && !_db.cmdline.install) {
        ind_beg(_db_zd_sel_package_curs,package,_db) if (Regx_Match(_db.cmdline.package,package.package)) {
            vrfy(GetPkgupstream(package), tempstr() << "apm.nodest"
                 << Keyval("package",package.package)
                 << Keyval("dest",_db.cmdline.dest)
                 << Keyval("comment","the package has no such destination; -reset creates one"));
        }ind_end;
    }
    int actions=0;
    // -t evaluates package recursively on the remote side
    // for -install
    if (_db.cmdline.install) {
        _db.cmdline.t=true;
    }
    if (_db.cmdline.install) {
        // After install, we can't perform any other operations on the
        // package without re-initializing the tool.
        Main_Install();
        actions++;
    }
    if (_db.cmdline.annotate != "") {
        Main_Annotate();
        actions++;
    }
    if (_db.cmdline.check) {
        Main_Check();
        actions++;
    }
    if (_db.cmdline.list) {
        Main_List();
        actions++;
    }
    if (_db.cmdline.update) {
        Main_Update();
        actions++;
    } else if (_db.cmdline.push) {
        Main_Push();
        actions++;
    } else if (_db.cmdline.diff) {
        Main_Diff();
        actions++;
    } else if (_db.cmdline.showrec) {
        Main_Showrec();
        actions++;
    } else if (_db.cmdline.showfile) {
        Main_Showfile();
        actions++;
    } else if (_db.cmdline.generate) {
        Main_Generate();
        actions++;
    } else if (_db.cmdline.e) {
        Main_Edit();
        actions++;
    } else if (_db.cmdline.reset) {
        Main_Reset();
        actions++;
    } else if (_db.cmdline.publish) {
        Main_Publish();
        actions++;
    }
    if (actions==0 && _db.cmdline.package.expr != "" && zd_sel_package_N()==0) {
        prlog("apm.nomatch"<<Keyval("comment","No packages matched selection. See list of installed packages below"));
        Main_List();
        algo_lib::_db.exit_code=1;
    }
    if (algo_lib::_db.exit_code==0) {
        Main_Transaction();
    }
}
