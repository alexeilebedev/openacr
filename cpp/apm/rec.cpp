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
// Source: cpp/apm/rec.cpp
//

#include "include/algo.h"
#include "include/apm.h"
#include "include/lib_ctype.h"

static float GetRowId(algo::Tuple& tuple) {
    float ret = 0;
    algo::Attr* attr = attr_Find(tuple, "acr.rowid");
    if (attr) {
        float_ReadStrptrMaybe(ret, attr->value);
    }
    return ret;
}

static tempstr EvalAttr(algo::Tuple& tuple, apm::FField& field) {
    tempstr ret;
    algo::Attr* attr = attr_Find(tuple, name_Get(field.c_substr ? *field.c_substr->p_srcfield : field));
    if (attr) {
        ret << (field.c_substr ? Pathcomp(attr->value, field.c_substr->expr.value) : attr->value);
    }
    return ret;
}

static void SetSortkey(apm::FRec& rec) {
    rec.sortkey.ctype = rec.p_ssimfile->p_ctype->ctype;
    rec.sortkey.num = 0;
    if (rec.p_ssimfile->c_ssimsort) {
        apm::FField* sortfld = apm::ind_field_Find(rec.p_ssimfile->c_ssimsort->sortfld);
        if (sortfld) {
            rec.sortkey.str = EvalAttr(rec.tuple, *sortfld);
        }
        if (sortfld && sortfld->p_arg->c_bltin) { // numeric check
            double_ReadStrptrMaybe(rec.sortkey.num, rec.sortkey.str);
            ch_RemoveAll(rec.sortkey.str);
        }
    }
    rec.sortkey.rowid = GetRowId(rec.tuple);
}

// Load tuples from FILENAME into REC table.
// A dev.pkgupstream row is not loaded.  It records where this tree's copy of a
// package came from, a commit of another repository, and the same row in any
// other tree would name a history that tree does not have.  Leaving it out here
// keeps it out of every projection, on every side of a merge or a push.
static void LoadRecsFile(algo::strptr filename) {
    ind_beg(algo::FileLine_curs, line, filename) {
        apm::FRec& rec = apm::rec_Alloc();
        bool good = false;
        if (Tuple_ReadStrptrMaybe(rec.tuple, line) && attrs_N(rec.tuple) > 0) {
            rec.p_ssimfile = apm::ind_ssimfile_Find(rec.tuple.head.value);
            if (rec.p_ssimfile && algo::strptr(rec.p_ssimfile->ssimfile) != apm::dmmeta_ssimfile_dev_pkgupstream) {
                rec.rec = tempstr() << rec.p_ssimfile->ssimfile << ":" << attrs_Find(rec.tuple, 0)->value;
                SetSortkey(rec);
                good = rec_XrefMaybe(rec);
            }
        }
        if (!good) {
            rec_Delete(rec);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Add any records from ssimfile SSIMFILE matching regx VALUE_REGX
// to ZD_CHOOSEREC (initial selection).
static void ChooseRec(apm::FSsimfile &ssimfile, algo_lib::Regx &value_regx) {
    if (literal_Get(value_regx.flags)) {
        tempstr key=tempstr()<<ssimfile.ssimfile<<":"<<value_regx.expr;
        if (apm::FRec *rec=apm::ind_rec_Find(key)) {
            zd_chooserec_Insert(*rec);
        }
    } else {
        ind_beg(apm::ssimfile_zd_ssimfile_rec_curs,rec,ssimfile) {
            if (Regx_Match(value_regx,attrs_Find(rec.tuple,0)->value)) {
                zd_chooserec_Insert(rec);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Return the dmmeta.ssimfile record of ctype record REC, or NULL when REC is not
// a ctype or the ctype is not a table's row.
static apm::FRec *GetCtypeSsimfileRec(apm::FRec &rec) {
    apm::FCtype *ctype = rec.p_ssimfile->ssimfile == "dmmeta.ctype" ? apm::ind_ctype_Find(Pathcomp(rec.rec,":LR")) : NULL;
    return ctype && ctype->c_ssimfile ? apm::ind_rec_Find(tempstr() << "dmmeta.ssimfile:" << ctype->c_ssimfile->ssimfile) : NULL;
}

// -----------------------------------------------------------------------------

// Add REC to ZD_CHOOSEREC together with every record it owns, transitively.
// A ctype also brings its ssimfile, which amc requires of a table's row type.
// The rows of a table that a program loads or compiles in do not come with it.
// They belong to whichever package owns each row: a package that extends
// openacr adds citests to atfdb.citest by claiming its own rows, and openacr
// leaves them behind.  A
// row the program's code names comes through the symbol scan instead.
// A record that is already whole was expanded before, so the walk stops there.
// The depth is the nesting of primary keys (ns, ctype, field, ...) plus one
// step, a small constant.
static void SelectWhole(apm::FRec &rec) {
    if (!rec.whole) {
        rec.whole = true;
        zd_chooserec_Insert(rec);
        ind_beg(apm::rec_c_child_curs, childrec, rec) {
            SelectWhole(childrec);
        }ind_end;
        apm::FRec *ssimfile_rec = GetCtypeSsimfileRec(rec);
        if (ssimfile_rec) {
            SelectWhole(*ssimfile_rec);
        }
    }
}

// -----------------------------------------------------------------------------

// Add owner REC to ZD_CHOOSEREC with the records that extend it.  A ctype is
// the exception and comes whole: its fields are its layout, so a ctype missing
// one is a different struct, and generated code over it no longer compiles.
// An extension is a child in the owner's namespace keyed exactly like the
// owner: dmmeta.nscpp:amc and dmmeta.nsx:amc extend dmmeta.ns:amc, while
// dmmeta.ctype:amc.FCtype is a separate entity the namespace owns.  A row of
// another namespace that shares the key, such as dev.target:amc beside
// dmmeta.ns:amc, is a separate table's entity and stays behind.
static void SelectOwner(apm::FRec &rec) {
    algo::strptr key = Pathcomp(rec.rec,":LR");
    algo::strptr ns = Pathcomp(rec.rec,".LL");
    if (rec.p_ssimfile->ssimfile == "dmmeta.ctype") {
        SelectWhole(rec);
    } else {
        zd_chooserec_Insert(rec);
        ind_beg(apm::rec_c_child_curs, childrec, rec) {
            if (Pathcomp(childrec.rec,":LR") == key && Pathcomp(childrec.rec,".LL") == ns) {
                zd_chooserec_Insert(childrec);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Evaluate regx in pkgkey, compute transitive closure according to pkgkey.up, pkgkey.down, pkgkey.ref, pkgkey.exclude
// and add any selected records to global zd_selrec table
static void SelectPkgkeyRecs(apm::FPkgkey &pkgkey) {
    tempstr key(key_Get(pkgkey));
    algo_lib::Regx ssimfile_regx;
    algo_lib::Regx value_regx;
    Regx_ReadAcr(ssimfile_regx,Pathcomp(key,":LL"),true);
    Regx_ReadAcr(value_regx,Pathcomp(key,":LR"),true);
    vrfy_(!apm::zd_chooserec_N());
    // produce initial chooserec selection
    if (literal_Get(ssimfile_regx.flags)) {
        if (apm::FSsimfile *ssimfile=apm::ind_ssimfile_Find(ssimfile_regx.expr)) {
            ChooseRec(*ssimfile,value_regx);
        }
    } else {
        ind_beg(apm::_db_ssimfile_curs,ssimfile,apm::_db) if (Regx_Match(ssimfile_regx,ssimfile.ssimfile)) {
            ChooseRec(ssimfile,value_regx);
        }ind_end;
    }

    // explicit pkgkey selection
    ind_beg(apm::_db_zd_chooserec_curs,rec,apm::_db) {
        zd_selrec_Insert(rec);
    }ind_end;
    pkgkey.n_explicit = apm::zd_selrec_N();

    apm::zd_chooserec_RemoveAll();

    // closure down
    if (pkgkey.down) {
        // restore original zd_chooserec list
        apm::FRec* r = apm::zd_selrec_First();
        for(u32 i = 0; i < pkgkey.n_explicit; ++i) {
            apm::zd_chooserec_Insert(*r);
            r = apm::zd_selrec_Next(*r);
        }

        // add referencing records in steps
        // the zd_chooserec list grows as we scan it, each new record is added with level=parent.level+1
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            ind_beg(apm::rec_c_child_curs, childrec, rec) {
                if (!zd_chooserec_InLlistQ(childrec)) {
                    prcat(verbose2, pkgkey.pkgkey << ": adding child " << childrec.rec << ", level " << rec.level + 1);
                    zd_chooserec_Insert(childrec);
                    childrec.level = rec.level + 1;
                }
            }ind_end;
        }ind_end;

        // reset level, and add choosen records to zd_selrec
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            zd_selrec_Insert(rec);
            rec.level = 0;
        }ind_end;
        // clear zd_chooserec
        apm::zd_chooserec_RemoveAll();

        pkgkey.n_down = apm::zd_selrec_N() - pkgkey.n_explicit;
    }

    // closure up
    if (pkgkey.up) {
        // restore original zd_chooserec list
        apm::FRec* r = apm::zd_selrec_First();
        for(u32 i = 0; i < pkgkey.n_explicit; ++i) {
            apm::zd_chooserec_Insert(*r);
            r = apm::zd_selrec_Next(*r);
        }

        // add referenced records in steps
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            ind_beg(apm::rec_c_parent_curs, parentrec, rec) {
                if (!zd_chooserec_InLlistQ(parentrec)) {
                    prcat(verbose2, pkgkey.pkgkey << ": adding parent " << parentrec.rec << ", level " << rec.level + 1);
                    zd_chooserec_Insert(parentrec);
                    parentrec.level = rec.level + 1;
                }
            }ind_end;
        }ind_end;

        // reset level, and add choosen records to zd_selrec
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            zd_selrec_Insert(rec);
            rec.level = 0;
        }ind_end;
        // clear zd_chooserec
        apm::zd_chooserec_RemoveAll();

        pkgkey.n_up = apm::zd_selrec_N() - pkgkey.n_explicit - pkgkey.n_down;
    }

    // closure over references
    // Take a field abt.FTarget.msghdr whose arg is dev.Target.  The package
    // needs that ctype, and the ctype is useless without its fields, which it
    // owns.  It also needs the ctype's owner, dmmeta.ns:dev, and must not take
    // what the owner owns, which is every other dev ctype.  So a referenced record comes
    // whole, and an owner comes alone.  Either one's own references are
    // followed in turn.
    if (pkgkey.ref) {
        u32 n_before = apm::zd_selrec_N();
        ind_beg(apm::_db_zd_selrec_curs, rec, apm::_db) {
            zd_chooserec_Insert(rec);
        }ind_end;
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            ind_beg(apm::rec_c_parent_curs, parentrec, rec) {
                SelectOwner(parentrec);
            }ind_end;
            ind_beg(apm::rec_c_ref_curs, refrec, rec) {
                SelectWhole(refrec);
            }ind_end;
        }ind_end;
        ind_beg(apm::_db_zd_chooserec_curs, rec, apm::_db) {
            zd_selrec_Insert(rec);
            rec.whole = false;
        }ind_end;
        apm::zd_chooserec_RemoveAll();
        pkgkey.n_ref = apm::zd_selrec_N() - n_before;
    }

    // clear zd_chooserec
    apm::zd_chooserec_RemoveAll();
}

// -----------------------------------------------------------------------------

// return TRUE if the field is a valid edge for transitive closure.
// The field is chosen if it's the pkey, or a leftmost subtring of pkey
static bool LeftCheckQ(apm::FField &field) {
    apm::FCtype &ctype=*field.p_ctype;
    bool ret=field.reftype == dmmeta_Reftype_reftype_Pkey;
    apm::FField *pkey =c_field_Find(ctype,0);
    if (pkey) {
        ret = pkey==&field
            || (field.c_substr && field.c_substr->srcfield==pkey->field && algo::LeftPathcompQ(field.c_substr->expr.value));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Set the visited flag on PACKAGE and on every package whose content it carries:
// the packages it builds on (its pkgdep parents, transitively) and the packages
// it distributes (its contain children).  The depth is that of the package
// graph, a handful.
static void MarkCarrier(apm::FPackage &package) {
    if (!package.visited) {
        package.visited = true;
        ind_beg(apm::package_c_pkgdep_curs, pkgdep, package) {
            MarkCarrier(*pkgdep.p_parent);
        }ind_end;
        ind_beg(apm::package_c_pkgdep_parent_curs, pkgdep, package) {
            if (pkgdep.pkgdeptype == apm::dev_pkgdeptype_contain) {
                MarkCarrier(*pkgdep.p_package);
            }
        }ind_end;
    }
}

// Make PACKAGE and the packages whose content it carries the carriers that
// CarriedQ asks about, and every other package not one.
// With PACKAGE NULL there is no carrier.
void apm::SetCarrier(apm::FPackage *package) {
    ind_beg(apm::_db_package_curs, other, apm::_db) {
        other.visited = false;
    }ind_end;
    if (package) {
        MarkCarrier(*package);
    }
}

// Return true if a carrier set by SetCarrier holds REC.
bool apm::CarriedQ(apm::FRec &rec) {
    bool ret = false;
    ind_beg(apm::rec_zd_rec_pkgrec_curs, pkgrec, rec) {
        ret = ret || pkgrec.p_package->visited;
    }ind_end;
    return ret;
}

// Remove from PACKAGE every record whose required parent the package does not
// carry, then every record that references a removed one, until none is left.
// Take cpp/gen/sampdb_gen.cpp.  The file exists because namespace sampdb
// exists, so a package without sampdb cannot regenerate it, and shipping it leaks what
// the namespace holds.  The data says so: the file requires its namespace
// (a bidir ssimreq), and the lib_prot source row references the file.  So the
// file leaves, and the next round takes the source row.  Only a removal spreads:
// a record that references something the package never held is left alone,
// which is how a package carries a source row whose file the origin keeps.
static void DropUnmet(apm::FPackage &package) {
    apm::SetCarrier(&package);
    apm::zd_selrec_RemoveAll();
    ind_beg(apm::package_zd_pkgrec_curs, pkgrec, package) {
        ind_beg(apm::rec_c_req_curs, reqrec, *pkgrec.p_rec) {
            if (!apm::CarriedQ(reqrec)) {
                prcat(verbose, "apm.unmet"
                      <<Keyval("package",package.package)
                      <<Keyval("rec",pkgrec.p_rec->rec)
                      <<Keyval("requires",reqrec.rec));
                apm::zd_selrec_Insert(*pkgrec.p_rec);
            }
        }ind_end;
    }ind_end;
    while (apm::zd_selrec_N() > 0) {
        ind_beg(apm::_db_zd_selrec_curs, rec, apm::_db) {
            apm::zd_droprec_Insert(rec);
        }ind_end;
        apm::DropSelectedPkgrec(package);
        apm::zd_selrec_RemoveAll();
        ind_beg(apm::package_zd_pkgrec_curs, pkgrec, package) {
            ind_beg(apm::rec_c_parent_curs, parentrec, *pkgrec.p_rec) {
                if (apm::zd_droprec_InLlistQ(parentrec) && !apm::CarriedQ(parentrec)) {
                    prcat(verbose, "apm.unmet"
                          <<Keyval("package",package.package)
                          <<Keyval("rec",pkgrec.p_rec->rec)
                          <<Keyval("references",parentrec.rec));
                    apm::zd_selrec_Insert(*pkgrec.p_rec);
                }
            }ind_end;
        }ind_end;
    }
    apm::zd_droprec_RemoveAll();
    apm::SetCarrier(NULL);
}

// -----------------------------------------------------------------------------

// Take out of zd_selrec every record that exists only with records BASE carries.
// Take cpp/gen/kafka_gen.cpp, which exists because namespace kafka has C++
// output (a bidir ssimreq), and openacr carries that namespace.  A package that
// extends openacr and claims lib_kafka, which compiles the file, reaches it
// through the closure of that claim, so subtracting the extender's records would
// take the file from the package that owns its namespace.  A record that exists only with its requirements belongs where they
// are, so a record whose every requirement BASE carries stays with BASE.  A source
// file's requirement on the records of the user functions it defines is not of
// that kind, so a file that defines one stays subject to the subtraction.
static void KeepBaseOwned(apm::FPackage &base) {
    apm::SetCarrier(&base);
    apm::zd_droprec_RemoveAll();
    ind_beg(apm::_db_zd_selrec_curs, rec, apm::_db) {
        bool owned = c_req_N(rec) > 0 && c_extrn_N(rec) == 0;
        ind_beg(apm::rec_c_req_curs, reqrec, rec) {
            owned = owned && apm::CarriedQ(reqrec);
        }ind_end;
        if (owned) {
            apm::zd_droprec_Insert(rec);
        }
    }ind_end;
    ind_beg(apm::_db_zd_droprec_curs, rec, apm::_db) {
        apm::zd_selrec_Remove(rec);
    }ind_end;
    apm::zd_droprec_RemoveAll();
    apm::SetCarrier(NULL);
}

// -----------------------------------------------------------------------------

// Load all records (FRec) from dataset _db.cmdline.data_in)
// For each record (FRec), compute p_ssimfile, pkey, tuple
// Populate global zd_rec index
// Populate zd_ssimfile_rec for each ssimfile (records grouped by ssimfile)
// Populate c_child and c_left_child arrays for each record (these are records referring
//    to choosen records)
// For each record, evaluate ssimreq rules. If there is a match, find corresponding
// record and add it as a "match" to this key.
//
// For each match between FPkgkey and FRec, Create an FPkgrec record,
// and group FPkgrec by FRec (zd_rec_pkgrec) and by FPackage (zd_rec)
// This structure allows full analysis of package composition and checking
void apm::LoadRecs() {
    if (DirectoryQ(_db.cmdline.data_in)) {
        // load all ssimfile records
        // use acr with `-rowid` option to capture correct rowids
        command::acr_proc acr;
        acr.cmd.in=_db.cmdline.data_in;
        acr.cmd.query="%";
        acr.cmd.loose=true;
        acr.cmd.rowid=true;
        algo_lib::FTempfile tempfile;
        TempfileInitX(tempfile, "apm.recs");
        acr.fstdout << ">"<<tempfile.filename;
        // The file is read back on the next line, and the redirect truncated it
        // before the child ran, so a child that fails hands the run an empty
        // database rather than an error. Every action apm can take is computed
        // from these records, and an action computed from none of them looks
        // like an action on a package that holds nothing: the checking variant
        // stops the run instead.
        acr_ExecX(acr);
        LoadRecsFile(tempfile.filename);
    } else {
        LoadRecsFile(_db.cmdline.data_in);
    }
    verblog("loaded "<<ind_rec_N()<<" records");
    // build graph of all records
    // compute c_child, c_leftchild
    ind_beg(_db_zd_rec_curs, rec, _db) {
        ind_beg(ctype_c_field_curs, field, *rec.p_ssimfile->p_ctype) if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
            algo::Attr *attr = attr_Find(rec.tuple,name_Get(field.c_substr ? *field.c_substr->p_srcfield:field));
            if (attr && field.p_arg->c_ssimfile) {
                algo::strptr value = field.c_substr ? Pathcomp(attr->value,field.c_substr->expr.value) : attr->value;
                tempstr parent_key=tempstr()<<field.p_arg->c_ssimfile->ssimfile<<":"<<value;
                if (apm::FRec *parent = apm::ind_rec_Find(parent_key)) {
                    if (LeftCheckQ(field)) {
                        c_child_Insert(*parent, rec);
                    } else {
                        c_ref_Insert(rec, *parent);
                    }
                    c_parent_Insert(rec, *parent);
                }
            }
        }ind_end;
        // evaluate ssimreq, add child record to the set
        ind_beg(apm::ctype_c_ssimreq_curs,ssimreq,*rec.p_ssimfile->p_ctype) if (!ssimreq.exclude) {
            apm::FField &reqfield=*ssimreq.p_field;
            algo::Attr *attr=attr_Find(rec.tuple,name_Get(reqfield.c_substr ? *reqfield.c_substr->p_srcfield:reqfield));
            if (attr) {
                algo::strptr value = reqfield.c_substr ? Pathcomp(attr->value,reqfield.c_substr->expr.value) : attr->value;
                if (Regx_Match(ssimreq.regx_value,value)) {
                    algo_lib::Replscope R;
                    lib_ctype::FillReplscope(R, rec.tuple);
                    tempstr child_key = Subst(R,ssimreq.ssimreq);
                    apm::FRec *child = apm::ind_rec_Find(child_key);
                    if (child && child != &rec) {
                        prcat(verbose2,child->rec<<" now child of "<<rec.rec);
                        c_child_Insert(rec,*child);
                        if (ssimreq.bidir) {
                            c_req_Insert(*child,rec);
                        }
                    }
                }
            }
        }ind_end;
    }ind_end;

    // A hand-written source references the records its code names.
    ScanSources();

    // Evaluate each package's pkgkeys, and create lists
    // package.zd_pkgrec
    // rec.zd_rec_pkgrec
    ind_beg(_db_zd_topo_package_curs, package, _db) {
        // included records
        ind_beg(package_zd_pkgkey_curs, pkgkey, package) if (!pkgkey.exclude) {
            zd_selrec_RemoveAll();
            SelectPkgkeyRecs(pkgkey);
            ind_beg(_db_zd_selrec_curs, rec, _db) {
                apm::FPkgrec& pkgrec = pkgrec_Alloc();
                pkgrec.p_package    = &package;
                pkgrec.p_rec        = &rec;
                pkgrec.p_pkgkey     = &pkgkey;
                vrfy(pkgrec_XrefMaybe(pkgrec), algo_lib::_db.errtext);
            }ind_end;
        }ind_end;

        // excluded records; a record the package names outright stays, so
        // openacr/dev.package:openacr survives openacr/dev.package:%
        ind_beg(package_zd_pkgkey_curs, pkgkey, package) if (pkgkey.exclude) {
            zd_selrec_RemoveAll();
            SelectPkgkeyRecs(pkgkey);
            DropSelectedPkgrec(package,true);
        }ind_end;
    }ind_end;

    // Subtract from each package the records the packages extending it capture.
    //
    // A package that extends another is not part of it: a downstream package
    // builds on a base distribution without shipping inside it, so nothing the
    // downstream package captures may go out as the base.  Written as regxes,
    // that fact costs one negative key per leaked record, and a base package
    // whose downstream is a whole platform carries thousands of them.  They sit
    // in the base package rather than in the package the records belong to, and
    // each says only that something does not belong, never whose it is.
    // Written as a relation it is one `dev.pkgdep` row of type `extend`, and
    // the records follow from whatever the extending package claims.
    //
    // The pass runs after every package has been evaluated rather than inside
    // the loop above.  That loop visits a package before the packages that
    // extend it -- a package is evaluated after its parents -- so an extender's
    // records do not exist yet at the moment its parent is being built.
    ind_beg(_db_pkgdep_curs,pkgdep,_db) if (pkgdep.pkgdeptype == dev_pkgdeptype_extend) {
        zd_selrec_RemoveAll();
        SelectPkgRecs(*pkgdep.p_package);
        KeepBaseOwned(*pkgdep.p_parent);
        DropSelectedPkgrec(*pkgdep.p_parent,true);
    }ind_end;

    // A package carries no record whose requirement it lacks.  This runs last,
    // since an exclusion or a subtraction can take away a requirement.
    ind_beg(_db_package_curs, package, _db) {
        DropUnmet(package);
    }ind_end;
    zd_selrec_RemoveAll();
}

// -----------------------------------------------------------------------------

// Whether PKGKEY names REC outright, rather than matching it.
// A pkgkey and a record are spelled the same way, `<ssimfile>:<pkey>`, so the
// test is that the two strings agree and that the key holds no pattern.
// Reaching a record through the reference closure of a literal key does not
// count: `dmmeta.ns:abt_md` names a namespace and nothing else, whatever its
// closure goes on to visit.
bool apm::NamesRecQ(apm::FPkgkey &pkgkey, apm::FRec &rec) {
    bool ret = key_Get(pkgkey) == rec.rec;
    if (ret) {
        algo_lib::Regx value_regx;
        Regx_ReadAcr(value_regx,Pathcomp(key_Get(pkgkey),":LR"),true);
        ret = literal_Get(value_regx.flags);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Remove from PACKAGE every pkgrec whose record is currently in zd_selrec.
// With KEEP_LITERAL, a record the package names outright is kept.
//
// A record a package names outright stays with it against both of the package's
// statements about what is not its own: an exclusion, which is a blanket such as
// `dev.package:%` beside the literal `dev.package:openacr`, and the subtraction a
// relation derives, where an extender's reference closure reaches a table the
// base owns.  Naming a record outright is how a package says the record is its
// regardless, and a blanket like `dev.%:%` does not.  A record whose requirement
// the package lacks goes whatever named it, so DropUnmet passes KEEP_LITERAL false.
//
// The walk is by hand rather than by cursor because it deletes the rows it
// visits, and a cursor over a list may not outlive the removal of its own node.
void apm::DropSelectedPkgrec(apm::FPackage &package, bool keep_literal DFLTVAL(false)) {
    apm::FPkgrec* cur = apm::zd_pkgrec_First(package);
    while (cur) {
        apm::FPkgrec* next = apm::package_zd_pkgrec_Next(*cur);
        bool named = keep_literal && NamesRecQ(*cur->p_pkgkey,*cur->p_rec);
        if (zd_selrec_InLlistQ(*cur->p_rec) && !named) {
            pkgrec_Delete(*cur);
        }
        cur = next;
    }
}

// -----------------------------------------------------------------------------

// Select records belonging to package PACKAGE by adding them to zd_selrec.
// These are all the records that the package references via zd_pkgrec.
void apm::SelectPkgRecs(apm::FPackage &package) {
    ind_beg(package_zd_pkgrec_curs, pkgrec, package) {
        zd_selrec_Insert(*pkgrec.p_rec);
    }ind_end;
}
