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
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/pkgfile.cpp
//
// Files a package ships at another path of its projection.  A dev.pkgfile row
// <package>/<dest> names a file of this tree, and the push writes that file's
// content at <dest> in the origin.  A package's readmefile is the same mapping
// for the path txt/README.md, which abt_md copies to the projection's README.md.
// A push, and every comparison of this tree's projection with the origin, applies
// the mappings going out.  An update applies them coming back, so a mapped file
// in the origin is merged with its source here, and never with this tree's own
// file at the same path.

#include "include/algo.h"
#include "include/apm.h"

// -----------------------------------------------------------------------------

// Add to table pkgfile the mapping that each package's readmefile implies.  A
// package whose front page is R ships R as its projection's txt/README.md, so its
// readmefile stands for the row <package>/txt/README.md naming R, and the rest of
// apm reads the two forms the same way.  A readmefile of txt/README.md, the
// default, names the file at its own path and implies nothing.  A row the definition already holds
// for that path stays, and a row naming another file contradicts the readmefile:
// the run reports it as apm.readmefile and fails.
void apm::DerivePkgfile() {
    ind_beg(_db_package_curs, package, _db) if (package.readmefile != "" && package.readmefile != "txt/README.md") {
        dev::Pkgfile pkgfile;
        pkgfile.pkgfile = dev::Pkgfile_Concat_package_dest(package.package, "txt/README.md");
        pkgfile.gitfile = package.readmefile;
        pkgfile.comment.value = "the package's readmefile";
        apm::FPkgfile *held = ind_pkgfile_Find(pkgfile.pkgfile);
        if (!held) {
            vrfy(pkgfile_InsertMaybe(pkgfile), algo_lib::_db.errtext);
        } else if (held->gitfile != pkgfile.gitfile) {
            prerr("apm.readmefile"
                  <<Keyval("package",package.package)
                  <<Keyval("readmefile",package.readmefile)
                  <<Keyval("pkgfile",held->pkgfile)
                  <<Keyval("gitfile",held->gitfile)
                  <<Keyval("comment","the package's readmefile and its pkgfile row name different files for txt/README.md"));
            algo_lib::_db.exit_code = 1;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// True when PACKAGE is a package the action is for: NAMED when there is one, else
// every package the command line's -package names.
static bool NamedQ(algo::strptr package, apm::FPackage *named) {
    return named ? package == named->package : Regx_Match(apm::_db.cmdline.package, package);
}

// Fill table pkgdest with the paths that the mappings of the carriers fill, the
// packages SetCarrier marked, each with the mapping that wins it, and return the
// number of conflicts found.
//
// Take a push of an SDK package that builds on openacr, so the push carries
// openacr as well.  Say both packages map a file to txt/README.md: the SDK its
// front page, openacr a page of its own.  The projection has one file at that
// path, so one of the two mappings has to lose.  The push is about the SDK, and
// openacr comes along because the SDK needs it, so the SDK's mapping is the one
// the push means.  So a mapping of a package the action is for, NAMED or else
// the command line's -package, wins over a mapping of a package that is merely
// carried, and the named packages are resolved first.  Two mappings that name
// the same file agree.  Two mappings of one path that name different files, of
// packages of the same rank, cannot be ordered, and each such pair is reported
// as apm.pkgfile_conflict.
int apm::ResolvePkgfile(apm::FPackage *named) {
    int ret = 0;
    pkgdest_RemoveAll();
    for (int pass = 0; pass < 2; pass++) {
        ind_beg(_db_package_curs, package, _db) if (package.visited && NamedQ(package.package, named) == (pass == 0)) {
            ind_beg(package_c_pkgfile_curs, pkgfile, package) {
                apm::FPkgdest &pkgdest = ind_pkgdest_GetOrCreate(dest_Get(pkgfile));
                apm::FPkgfile *held = pkgdest.p_pkgfile;
                bool rival = held && held->gitfile != pkgfile.gitfile && NamedQ(package_Get(*held), named) == (pass == 0);
                if (!held) {
                    pkgdest.p_pkgfile = &pkgfile;
                } else if (rival) {
                    prerr("apm.pkgfile_conflict"
                          <<Keyval("dest",pkgdest.pkgdest)
                          <<Keyval("pkgfile",held->pkgfile)
                          <<Keyval("other",pkgfile.pkgfile)
                          <<Keyval("comment","two packages map different files to one path of the projection"));
                    ret++;
                }
            }ind_end;
        }ind_end;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Make each path of table pkgdest a file of the projection, so that a push writes
// the content of its mapping's source there.  The path replaces whatever file of a
// carried package the projection held at it: an SDK's front page replaces
// openacr's txt/README.md.  The source still goes out at its own path when a key
// of the package carries it.  A source that is not in this tree stops the push
// with apm.pkgfile_nosrc, since the push would otherwise delete the path in the
// origin.
void apm::ShipPkgdest() {
    ind_beg(_db_pkgdest_curs, pkgdest, _db) {
        algo::strptr src = pkgdest.p_pkgfile->gitfile;
        vrfy(GetFileMode(src) != 0, tempstr() << "apm.pkgfile_nosrc"
             << Keyval("pkgfile", pkgdest.p_pkgfile->pkgfile)
             << Keyval("gitfile", src)
             << Keyval("comment", "the file the package maps is not in this tree"));
        SetMergeSide(ind_mergefile_GetOrCreate(pkgdest.pkgdest), 0, src);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Make the origin's copy of each mapped file side SIDE of the source's merge
// entry, where SIDE is 1 for the base and 2 for the incoming version, and DIR is
// that version's checkout.  The source here is the entry's own side, whether or
// not a key of the package carries it.  The merge then compares the origin's
// copy with the source, and this tree's own file at the mapped path stays out of
// it: openacr's txt/README.md is never merged with an SDK's front page.
//
// The origin holds the mapped file twice when the package also carries the
// source at its own path.  An SDK holds its front page as txt/README.md and at
// the path of its source, and a contributor may edit either copy.  The base holds
// the two copies equal, since one push wrote both.  So the incoming side takes
// the copy at the mapped path when that copy changed since the base, and
// otherwise the copy at the source's own path, which carries whatever change was
// made there.  When both copies changed, differently, the copy at the mapped path
// is merged and the other is reported as apm.pkgfile_diverge.
void apm::MapOriginCopy(int side, algo::strptr dir) {
    ind_beg(_db_pkgdest_curs, pkgdest, _db) {
        tempstr dest_file(DirFileJoin(dir, pkgdest.pkgdest));
        apm::FMergefile &mergefile = ind_mergefile_GetOrCreate(pkgdest.p_pkgfile->gitfile);
        algo::strptr src_file = side == 1 ? algo::strptr(mergefile.base_file) : algo::strptr(mergefile.theirs_file);
        bool own = (side == 1 ? mergefile.base_mode : mergefile.theirs_mode) != 0;
        tempstr base(mergefile.base_mode != 0 ? algo::FileToString(mergefile.base_file) : tempstr());
        tempstr dest(GetFileMode(dest_file) != 0 ? algo::FileToString(dest_file) : tempstr());
        tempstr src(own ? algo::FileToString(src_file) : tempstr());
        bool moved = side == 2 && dest != base;
        bool take = GetFileMode(dest_file) != 0 && (side == 1 || !own || moved);
        if (take && own && moved && src != base && src != dest) {
            prerr("apm.pkgfile_diverge"
                  <<Keyval("file",dest_file)
                  <<Keyval("other",src_file)
                  <<Keyval("comment","both copies of a mapped file changed; the copy at the mapped path is merged"));
        }
        SetMergeSide(mergefile, 0, pkgdest.p_pkgfile->gitfile);
        if (take) {
            SetMergeSide(mergefile, side, dest_file);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// True when PACKAGE carries REC whole through a key of its own.
static bool HoldsWholeQ(apm::FRec &rec, apm::FPackage &package) {
    bool ret = false;
    ind_beg(apm::rec_zd_rec_pkgrec_curs, pkgrec, rec) {
        ret = ret || (pkgrec.p_package == &package && !pkgrec.stub);
    }ind_end;
    return ret;
}

// Report every mapping of a selected package that a push of it cannot carry out,
// and return how many were found.  The mappings of a package and the packages it
// carries must not collide (apm.pkgfile_conflict).  A mapping must name a file of
// this tree (apm.pkgfile_nosrc).  And the package must not carry the mapped path
// whole from its own keys, since the projection would then hold two different
// files at one path (apm.pkgfile_ambiguous).
int apm::CheckPkgfile() {
    int ret = 0;
    ind_beg(_db_zd_sel_package_curs, package, _db) {
        SetCarrier(&package);
        ret += ResolvePkgfile(&package);
        SetCarrier(NULL);
        ind_beg(package_c_pkgfile_curs, pkgfile, package) {
            apm::FRec *src = ind_rec_Find(tempstr() << "dev.gitfile:" << pkgfile.gitfile);
            apm::FRec *dest = ind_rec_Find(tempstr() << "dev.gitfile:" << dest_Get(pkgfile));
            if (!src || GetFileMode(pkgfile.gitfile) == 0) {
                prerr("apm.pkgfile_nosrc"
                      <<Keyval("pkgfile",pkgfile.pkgfile)
                      <<Keyval("gitfile",pkgfile.gitfile)
                      <<Keyval("comment","the file the package maps is not in this tree"));
                ret++;
            }
            if (dest && HoldsWholeQ(*dest, package)) {
                prerr("apm.pkgfile_ambiguous"
                      <<Keyval("pkgfile",pkgfile.pkgfile)
                      <<Keyval("gitfile",pkgfile.gitfile)
                      <<Keyval("comment","the package also carries the mapped path from its own keys; drop the key or the mapping"));
                ret++;
            }
        }ind_end;
    }ind_end;
    return ret;
}
