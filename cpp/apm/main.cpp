// Copyright (C) 2023-2024 AlgoRND
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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/main.cpp
//

#include "include/algo.h"
#include "include/lib_git.h"
#include "include/apm.h"

// Initialize zd_sel_package list based on the command line regex
// For -update, -install -- select parent packages as well
// For -remove -- select dependent packages
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
    bool sel_parent=_db.cmdline.t;
    bool sel_child=_db.cmdline.remove;
    if (sel_parent) {
        ind_beg(_db_zd_sel_package_curs,package,_db) {
            ind_beg(package_c_pkgdep_curs,pkgdep,package) if (!pkgdep.soft) {
                // newly selected package will be visited by enclosing iteration
                zd_sel_package_Insert(*pkgdep.p_parent);
            }ind_end;
        }ind_end;
    }
    if (sel_child) {
        ind_beg(_db_zd_sel_package_curs,package,_db) {
            ind_beg(package_c_pkgdep_parent_curs,pkgdep,package) {
                // newly selected package will be visited by enclosing iteration
                zd_sel_package_Insert(*pkgdep.p_package);
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
// This is the pristine version of the package (as specified with the gitref)
// If BASEREF is an empty string, then the entire current directory, with whatever
// local changes, is copied to the sandbox instead
int apm::CreatePackageSandbox(algo::strptr sandbox_name, algo::strptr baseref) {
    int rc=0;
    command::sandbox_proc sandbox;
    sandbox.cmd.create=true;
    sandbox.cmd.q=true;
    sandbox.cmd.name.expr=sandbox_name;
    if (baseref == "") {
        sandbox.cmd.reset=true;
    } else {
        // add desired ref to the fetch set, otherwise it might not be cloned
        // into the sandbox
        sandbox.cmd.refs<<" "<<baseref;
        cmd_Alloc(sandbox.cmd) << "git";
        cmd_Alloc(sandbox.cmd) << "reset";
        cmd_Alloc(sandbox.cmd) << "-q";
        cmd_Alloc(sandbox.cmd) << "--hard";
        cmd_Alloc(sandbox.cmd) << baseref;
    }
    rc= sandbox_Exec(sandbox);
    return rc;
}

// -----------------------------------------------------------------------------

tempstr apm::FetchPackageOrigin(algo::strptr origin, algo::strptr ref) {
    tempstr gitref;
    if (origin == ".") {
        if (ref == "HEAD") {
            gitref=ref;
        } else {
            gitref=Trimmed(SysEval(tempstr()<<"git rev-parse "<<ref,FailokQ(true),1024));
        }
    } else {
        int rc=SysCmd(tempstr()<<"git fetch -q "<<origin<<" "<<ref);
        if (rc==0) {
            gitref=algo::Pathcomp(algo::FileToString(".git/FETCH_HEAD"),"\tLL");
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
    SafeStringToFile(_db.script,_db.scriptfile,0755);
    command::bash_proc bash;
    bash.cmd.c=_db.scriptfile;
    int rc=0;
    if (!_db.cmdline.dry_run) {
        rc=bash_Exec(bash);
    } else {
        prlog(_db.script);
    }
    _db.script="";
    if (rc!=0) {
        algo_lib::_db.exit_code=rc;
    }
}

int apm::GetFileMode(algo::strptr filename) {
    struct stat struct_stat;
    algo::ZeroBytes(struct_stat);
    int rc=stat(Zeroterm(tempstr()<<filename),&struct_stat);
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
                package.baseref="HEAD";
                package.origin=".";
                package.comment.value=tempstr()<<"package for namespace "<<ns.ns;
                if (package_InsertMaybe(package)) {
                    dev::Pkgdep pkgdep;
                    pkgdep.pkgdep=dev::Pkgdep_Concat_package_parent(ns.ns,"openacr");
                    pkgdep.soft=true;
                    pkgdep_InsertMaybe(pkgdep);

                    dev::Pkgkey pkgkey;
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
    vrfy(!((_db.cmdline.install || _db.cmdline.update) && _db.cmdline.remove),
         "-install/-update is incompatible with -remove, please select just one option");
    vrfy(_db.cmdline.install + _db.cmdline.update + _db.cmdline.push + _db.cmdline.diff + _db.cmdline.showrec
         + _db.cmdline.showfile + _db.cmdline.e + _db.cmdline.remove + _db.cmdline.reset <= 1,
         "more than one action selected");
    if (_db.cmdline.annotate != "" || _db.cmdline.check || _db.cmdline.diff || _db.cmdline.showrec || _db.cmdline.push
        || _db.cmdline.showfile || _db.cmdline.update || _db.cmdline.remove || _db.cmdline.install) {
        LoadRecs();
    }
    bool needlock=_db.cmdline.update || _db.cmdline.remove || _db.cmdline.install;
    algo_lib::FLockfile lockfile;
    if (needlock) {
        LockFileInit(lockfile, "temp/apm.lock");
    }
    if (algo_lib::_db.cmdline.verbose) {
        _db.script << "set -x" << eol;
    }
    int actions=0;
    // check package definition before removing
    if (_db.cmdline.remove) {
        _db.cmdline.check=true;
    }
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
    } else if (_db.cmdline.e) {
        Main_Edit();
        actions++;
    } else if (_db.cmdline.remove) {
        Main_Remove();
        actions++;
    } else if (_db.cmdline.reset) {
        Main_Reset();
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
