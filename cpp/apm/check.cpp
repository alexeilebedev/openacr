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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/check.cpp
//

#include "include/algo.h"
#include "include/apm.h"

void apm::Main_Check() {
    if (CheckLicense() > 0) {
        algo_lib::_db.exit_code=1;
    }
    if (CheckDangling() > 0) {
        algo_lib::_db.exit_code=1;
    }
    if (CheckMixedFile() > 0) {
        algo_lib::_db.exit_code=1;
    }
    if (CheckSyncPair() > 0) {
        algo_lib::_db.exit_code=1;
    }
    if (CheckKeyword() > 0) {
        algo_lib::_db.exit_code=1;
    }
    if (CheckLink() > 0) {
        algo_lib::_db.exit_code=1;
    }
    // Each pkgkey must match at least one record.  The test is what the key's
    // query matched, before closure, exclusion or subtraction: an exclusion
    // keeps no pkgrec by construction, and a key whose records an extender
    // claims keeps none either, though both name records that exist.
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        ind_beg(package_zd_pkgkey_curs,pkgkey,package) {
            if (pkgkey.n_explicit==0) {
                prlog("apm.emptykey"
                      <<Keyval("pkgkey",pkgkey.pkgkey)
                      <<Keyval("comment","this key doesn't match any records"));
                dev::Pkgkey out;
                pkgkey_CopyOut(pkgkey,out);
                prlog("acr.delete  "<<out);
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }ind_end;

    // each namespace must belong to a package

    // ssimfiles under data/ should not be included in pkgkey, since ssim records
    // are picked up by the corresponding namespace
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        ind_beg(package_zd_pkgkey_curs,pkgkey,package) {
            if (StartsWithQ(key_Get(pkgkey),"dmmeta.ssimfile:data/")) {
                prlog("apm.badkey"
                      <<Keyval("pkgkey",pkgkey.pkgkey)
                      <<Keyval("comment","ssimfiles should not be included in package definition directly"));
                dev::Pkgkey out;
                pkgkey_CopyOut(pkgkey,out);
                prlog("acr.delete  "<<out);
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }ind_end;

    // any pkgkey that's dominated by another pkgkey can be eliminated

    // if any record belongs to more than one package, then
    // there must be a package dependency between a later package and an earlier package

    // A package and the packages extending it must not claim the same namespace.
    //
    // A downstream package claiming a namespace the base publishes is claiming
    // something it merely depends on.  With the exclusions written out by hand
    // nothing noticed, because a record is allowed to belong to two packages and
    // the base's list simply did not exclude that namespace.  Once a package's
    // content is reduced by what its extenders capture, the same double claim
    // silently takes every record of that namespace out of the distribution,
    // which is a change nobody asked for and no test names.
    //
    // The report is per namespace rather than per record, because the namespace
    // is where the decision is: one of the two packages does not own it.
    ind_beg(_db_pkgdep_curs,pkgdep,_db) if (pkgdep.pkgdeptype == dev_pkgdeptype_extend) {
        ind_beg(package_zd_pkgkey_curs,pkgkey,*pkgdep.p_package) if (!pkgkey.exclude) {
            tempstr ns(Pathcomp(key_Get(pkgkey),":LR"));
            tempstr parent_key(dev::Pkgkey_Concat_package_key(pkgdep.p_parent->package,tempstr()<<"dmmeta.ns:"<<ns));
            apm::FPkgkey *found = StartsWithQ(key_Get(pkgkey),"dmmeta.ns:") ? ind_pkgkey_Find(parent_key) : NULL;
            if (found && !found->exclude) {
                prlog("apm.doubleclaim"
                      <<Keyval("ns",ns)
                      <<Keyval("package",pkgdep.p_package->package)
                      <<Keyval("parent",pkgdep.p_parent->package)
                      <<Keyval("comment","both claim this namespace; the extender's claim removes the parent's records"));
                algo_lib::_db.exit_code=1;
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Return the dev.license record that REC references, or NULL when it references none.
// A record with a license field (a namespace, a script file, a system library)
// references its license directly.  A source file of a target falls under the
// license of the target's namespace, and a target's link to a system library
// under the license of that library.  Any other row falls under the license of
// its table's namespace: an amcdb.tclass row is amcdb's data wherever it
// travels.
static apm::FRec *GetLicenseRec(apm::FRec &rec) {
    apm::FRec *owner = &rec;
    apm::FRec *ret = NULL;
    if (rec.p_ssimfile->ssimfile == "dev.targsrc") {
        owner = apm::ind_rec_Find(tempstr()<<"dmmeta.ns:"<<Pathcomp(attrs_Find(rec.tuple,0)->value,"/LL"));
    } else if (rec.p_ssimfile->ssimfile == "dev.targsyslib") {
        owner = NULL;
        ind_beg(apm::rec_c_parent_curs, parentrec, rec) {
            if (parentrec.p_ssimfile->ssimfile == "dev.syslib") {
                owner = &parentrec;
            }
        }ind_end;
    }
    if (owner) {
        ind_beg(apm::rec_c_parent_curs, parentrec, *owner) {
            if (parentrec.p_ssimfile->ssimfile == "dev.license") {
                ret = &parentrec;
            }
        }ind_end;
    }
    apm::FRec *tablens = ret ? NULL : apm::ind_rec_Find(tempstr() << "dmmeta.ns:" << Pathcomp(rec.p_ssimfile->ssimfile,".LL"));
    if (tablens && tablens != owner) {
        ind_beg(apm::rec_c_parent_curs, parentrec, *tablens) {
            if (parentrec.p_ssimfile->ssimfile == "dev.license") {
                ret = &parentrec;
            }
        }ind_end;
    }
    return ret;
}

// Report every record of each selected open-source package whose license
// forbids it there, and return how many were found.
// An open-source package is published outside the company, so a namespace or a
// file under a proprietary license must never be part of it.  A target the
// package builds must not link a system library under a proprietary license
// either, nor one under a copyleft license such as the GPL: say acr_compl links
// GNU readline, then every acr_compl binary built from the package is a GPL program,
// whatever license its own sources carry.  Each finding names the package, the
// record and the license, so the fix is a key that excludes the record, a
// license change in the record's namespace, or a target that stops linking the
// library.
int apm::CheckLicense() {
    int ret = 0;
    ind_beg(_db_zd_sel_package_curs,package,_db) if (package.opensource) {
        zd_selrec_RemoveAll();
        SelectPkgRecs(package);
        ind_beg(_db_zd_selrec_curs,rec,_db) {
            apm::FRec *license = GetLicenseRec(rec);
            bool proprietary = license && attr_GetString(license->tuple,"proprietary") == "Y";
            bool copyleft = license
                && rec.p_ssimfile->ssimfile == "dev.targsyslib"
                && attr_GetString(license->tuple,"copyleft") == "Y";
            if (proprietary || copyleft) {
                prerr((proprietary ? "apm.proprietary" : "apm.copyleft")
                      <<Keyval("package",package.package)
                      <<Keyval("rec",rec.rec)
                      <<Keyval("license",license->rec));
                ret++;
            }
        }ind_end;
        zd_selrec_RemoveAll();
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every record of each selected package that references a record the
// package does not carry, and return how many were found.
// A package installed into an empty repository holds exactly its own records and
// those of the packages it builds on, so a reference to anything else is one
// that repository cannot resolve: acr -check fails there, and amc stops on the
// first such reference it needs.  Each finding names the record and its target,
// so the fix is to bring the target into the package or to move the record out.
int apm::CheckDangling() {
    int ret = 0;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        SetCarrier(&package);
        zd_selrec_RemoveAll();
        SelectPkgRecs(package);
        ind_beg(_db_zd_selrec_curs,rec,_db) {
            ind_beg(rec_c_parent_curs,parentrec,rec) {
                if (!CarriedQ(parentrec)) {
                    prerr("apm.dangling"
                          <<Keyval("package",package.package)
                          <<Keyval("rec",rec.rec)
                          <<Keyval("references",parentrec.rec));
                    ret++;
                }
            }ind_end;
        }ind_end;
        zd_selrec_RemoveAll();
    }ind_end;
    SetCarrier(NULL);
    return ret;
}

// -----------------------------------------------------------------------------

// Report every source file that defines user functions for records a selected
// package carries and for records it does not, and return how many were found.
// Take a comptest driver that defines one test of openacr's and one of an
// extending package's.  openacr cannot carry the file, since the other test has
// no row there and amc would
// generate no prototype for its function.  Nor can openacr drop it, since its own
// test row would then name a function nothing defines, and the origin fails to
// link.  apm publishes a file whole, so the only fix is to split it, one owner per
// file.  Only a package that builds the file's target is asked, since a package
// that carries a target's data and not its namespace never compiles the file.
// Each finding names the file, one record on each side, and how many there are of
// each.
int apm::CheckMixedFile() {
    int ret = 0;
    apm::FSsimfile *targsrc_ssimfile = ind_ssimfile_Find("dev.targsrc");
    ind_beg(_db_zd_sel_package_curs,package,_db) if (targsrc_ssimfile) {
        SetCarrier(&package);
        ind_beg(ssimfile_zd_ssimfile_rec_curs,targsrc,*targsrc_ssimfile) {
            algo::strptr key = attrs_Find(targsrc.tuple,0)->value;
            apm::FRec *file = ind_rec_Find(tempstr() << "dev.gitfile:" << Pathcomp(key,"/LR"));
            apm::FRec *ns = ind_rec_Find(tempstr() << "dmmeta.ns:" << Pathcomp(key,"/LL"));
            apm::FRec *carried = NULL;
            apm::FRec *missing = NULL;
            int ncarried = 0;
            int nmissing = 0;
            if (file && ns && CarriedQ(*ns)) {
                ind_beg(rec_c_extrn_curs,target,*file) {
                    if (CarriedQ(target)) {
                        carried = &target;
                        ncarried++;
                    } else {
                        missing = &target;
                        nmissing++;
                    }
                }ind_end;
            }
            if (carried && missing) {
                prerr("apm.mixedfile"
                      <<Keyval("package",package.package)
                      <<Keyval("file",file->rec)
                      <<Keyval("carried",carried->rec)
                      <<Keyval("missing",missing->rec)
                      <<Keyval("ncarried",ncarried)
                      <<Keyval("nmissing",nmissing)
                      <<Keyval("comment","the file defines functions for records on both sides; split it, one owner per file"));
                ret++;
            }
        }ind_end;
    }ind_end;
    SetCarrier(NULL);
    return ret;
}

// -----------------------------------------------------------------------------

// Report every file on which the projection of this tree at a selected package's
// localref and the origin at its baseref disagree, and return how many there are.
// A sync is a pair of commits, one per repository, and the origin's commit is
// the projection of this tree's: that is what lets a push take the whole origin
// as its base.  A file only one side holds means the pair does not correspond,
// and the next push would treat the difference as a change nobody made.  File
// contents are not compared, since each tree regenerates its own generated files;
// ssimfiles are records, and data/ is left out.
int apm::CheckSyncPair() {
    int ret = 0;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        ind_beg(package_c_pkgupstream_curs,pkgupstream,package) if (pkgupstream.localref != "" && pkgupstream.baseref != "") {
            tempstr base = FetchPackageOrigin(pkgupstream.pkgupstream,pkgupstream.origin,pkgupstream.baseref);
            vrfy(base != "", tempstr() << "apm.syncpair  comment:'the baseref could not be fetched'" << Keyval("baseref",pkgupstream.baseref));
            vrfy(CreatePackageSandbox(_db.base_sandbox,pkgupstream.localref)==0, "apm.syncpair  comment:'the localref could not be checked out'");
            mergefile_RemoveAll();
            // the push publishes the package with the packages it contains and the
            // ones it builds on, and the carrier set is exactly those
            SetCarrier(&package);
            algo::ListSep ls("|");
            tempstr expr;
            ind_beg(_db_package_curs,carrier,_db) if (carrier.visited) {
                expr << ls << carrier.package;
            }ind_end;
            SetCarrier(NULL);
            command::apm_proc apm;
            apm.path = DirFileJoin(algo::GetCurDir(),"bin/apm");
            apm.cmd.package.expr = tempstr() << "(" << expr << ")";
            apm.cmd.showfile = true;
            apm.fstdout = "|";
            algo_lib::PushDir(algo_lib::WtDir(_db.base_sandbox));
            apm_Start(apm);
            algo_lib::PopDir();
            ind_beg(algo::FileLine_curs,line,apm.from_stdout) {
                dev::Gitfile gitfile;
                if (Gitfile_ReadStrptrMaybe(gitfile,line) && !StartsWithQ(gitfile.gitfile,"data/")) {
                    ind_mergefile_GetOrCreate(gitfile.gitfile).ours_mode = 1;
                }
            }ind_end;
            apm_Wait(apm);
            vrfy(apm.status == 0, "apm.syncpair  comment:'the projection at localref could not be evaluated'");
            tempstr tree(SysEval(tempstr() << "git ls-tree -r --name-only " << strptr_ToBash(base), FailokQ(false), 1024*1024*64));
            ind_beg(algo::Line_curs,path,tree) if (ch_N(path) > 0 && !StartsWithQ(path,"data/")) {
                ind_mergefile_GetOrCreate(path).base_mode = 1;
            }ind_end;
            ind_beg(_db_mergefile_curs,mergefile,_db) if ((mergefile.ours_mode != 0) != (mergefile.base_mode != 0)) {
                prerr("apm.syncdrift"
                      <<Keyval("pkgupstream",pkgupstream.pkgupstream)
                      <<Keyval("file",mergefile.mergefile)
                      <<Keyval("only_in",mergefile.ours_mode ? "projection" : "origin")
                      <<Keyval("comment","the origin at baseref is not the projection of localref"));
                ret++;
            }ind_end;
            mergefile_RemoveAll();
        }ind_end;
    }ind_end;
    return ret;
}
