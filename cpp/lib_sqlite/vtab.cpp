// Copyright (C) 2026 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: lib_sqlite (lib)
// Exceptions: yes
// Source: cpp/lib_sqlite/vtab.cpp
//

#include "include/algo.h"
#include "include/lib_sqlite.h"

using namespace lib_sqlite;

// Find ctype from ctype name
// Supports ctype and ssimfile lookups.
static FCtype *TagToCtype(strptr name) {
    auto *ssimfile = ind_ssimfile_Find(name);
    auto *ctype = ssimfile ? ssimfile->p_ctype : ind_ctype_Find(name);
    return ctype;
}

static tempstr CreateCtypeTable(FCtype& ctype) {
    auto cmd = tempstr();
    cmd << "CREATE TABLE x (";
    algo::ListSep ls(",");
    ind_beg(ctype_c_field_curs, field, ctype) {
        auto sqltype = field.p_arg->c_sqltype ? strptr(field.p_arg->c_sqltype->expr) : "TEXT";
        if (algo::FindStr(sqltype, "enum") >= 0) {
            sqltype = "TEXT";
        }
        cmd << ls
            << "'" << name_Get(field) << "' "
            << (field.c_substr ? " HIDDEN " : "")
            << sqltype;
    }ind_end;
    cmd << ")";
    return cmd;
}

static bool MatchTag(FCtype& ctype, strptr tag) {
    auto* ssim = ctype.c_ssimfile;
    return (ssim && tag == ssim->ssimfile) || ctype.ctype == tag;
}

static void ReadRow(VtabCurs& curs, FRow& row) {
    auto& ctype = *row.p_ctype;
    ind_beg(algo::Tuple_attrs_curs, attr, row.tuple) {
        auto* field = ind_field_name_Find(ctype, attr.name);
        if (field && !field->c_substr) {
            auto& value = attrs_qFind(curs, field->id);
            value = attr.value;
        }
    }ind_end;
    ind_beg(ctype_c_field_curs, field, ctype) if (field.c_substr) {
        auto& src = attrs_qFind(curs, field.c_substr->p_srcfield->id);
        auto& value = attrs_qFind(curs, field.id);
        value = Pathcomp(src, field.c_substr->expr.value);
    }ind_end;
}

static bool CheckRow(VtabCurs& curs, FRow& row) {
    auto mismatch = false;
    ReadRow(curs, row);
    if (auto* idx = curs.c_idx) {
        ind_beg(FIdx_cons_curs, con, *idx) {
            auto& value = attrs_qFind(curs, con.p_field->id);
            switch (con.op) {
                break; case SQLITE_INDEX_CONSTRAINT_EQ: mismatch |= value != con.value;
                break; case SQLITE_INDEX_CONSTRAINT_NE: mismatch |= value == con.value;
                break; default: ;
            }
        }ind_end;
    }
    return !mismatch;
}

static void SsimCursor_Next(VtabCurs& curs) {
    auto& ctype = *curs.p_ctype;
    if (curs.c_pkey) {
        if (!curs.c_row) {
            curs.c_row = ind_pkey_Find(ctype, curs.c_pkey->value);
            if (!curs.c_row || !CheckRow(curs, *curs.c_row)) {
                curs.c_row = nullptr;
            }
        } else {
            // already consumed
            curs.c_row = nullptr;
        }
    } else {
        // full scan
        if (!curs.c_row) {
            curs.c_row = zd_row_First(ctype);
        } else {
            curs.c_row = ctype_zd_row_Next(*curs.c_row);
        }
        while (curs.c_row) {
            if (CheckRow(curs, *curs.c_row)) {
                break;
            }
            curs.c_row = ctype_zd_row_Next(*curs.c_row);
        }
    }
    curs.eof = curs.c_row == nullptr;
    verblog(__FUNCTION__
            << Keyval("ctype", ctype.ctype)
            << Keyval("curs", curs.rowid)
            << Keyval("pkey", curs.c_pkey ? strptr(curs.c_pkey->value) : "N")
            << Keyval("row", curs.c_row ? strptr(curs.c_row->pkey) : "N")
            << Keyval("eof", curs.eof)
            );
}

static int xDisconnect(sqlite3_vtab* pVtab) {
    verblog(__FUNCTION__);
    auto* tab = (Vtab*)pVtab;
    delete tab;
    return SQLITE_OK;
}

static int xConnect(sqlite3 *db, void *, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr){
    if (argc != 5) {
        *pzErr = sqlite3_mprintf("%s", "invalid number of arguments");
        return SQLITE_ERROR;
    }

    auto data = algo::Trimmed(strptr(argv[3]));
    auto type_tag = algo::Trimmed(argv[4]);
    auto* ctype = TagToCtype(type_tag);
    if (!ctype) {
        *pzErr = sqlite3_mprintf("create ctype:%s comment:ctype not found", type_tag);
        return SQLITE_ERROR;
    }

    auto rc = SQLITE_OK;
    auto* ctx = new Vtab();
    *ppVtab = (sqlite3_vtab*)ctx;
    if (FileQ(data)) {
        ctx->filename = data;
    } else if (data == "-") {
        ctx->filename = "/dev/stdin";
    } else if (DirectoryQ(data)) {
        ctx->filename = algo::SsimFname(data, type_tag);
    }
    ctx->p_ctype = ctype;
    char * schema = sqlite3_mprintf("%s", algo::Zeroterm(CreateCtypeTable(*ctype)));
    rc = sqlite3_declare_vtab(db, schema);
    if (rc == SQLITE_OK) {
        rc = sqlite3_vtab_config(db, SQLITE_VTAB_DIRECTONLY);
    }
    if (rc != SQLITE_OK) {
        if (ctx) {
            xDisconnect(&ctx->base);
        }
    }

    return rc;
}

static void MaybeLoadTable(Vtab& tab) {
    if (!bool_Update(tab.loaded, true)) {
        return;
    }
    auto& ctype = *tab.p_ctype;
    // read the file
    algo_lib::MmapFile fmap;
    algo_lib::MmapFile_Load(fmap, tab.filename);
    ind_beg(algo::Line_curs, line, fmap.text) {
        auto row = algo::Tuple();
        if (algo::Tuple_ReadStrptrMaybe(row, line) && MatchTag(ctype, row.head.value)) {
            auto& trow = trow_Alloc();
            trow.rowid = c_row_N(ctype);
            algo::TSwap(trow.tuple, row);
            if (auto* pkey = attrs_Find(trow.tuple, 0)) {
                trow.pkey = pkey->value;
            }
            trow.p_ctype = &ctype;
            trow_XrefMaybe(trow);
        }
    }ind_end;
    verblog(__FUNCTION__
            << Keyval("ctype", ctype.ctype)
            << Keyval("n_row", zd_row_N(ctype))
            );
}

static int xCreate(sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr){
    return xConnect(db, pAux, argc, argv, ppVtab, pzErr);
}

static int xBestIndex(sqlite3_vtab* pVtab, sqlite3_index_info* pIdxInfo) {
    auto& tab = *(Vtab*)pVtab;
    auto& ctype = *tab.p_ctype;
    MaybeLoadTable(tab);
    auto& idx = bestidx_Alloc();
    idx.rowid = bestidx_N() - 1;

    // Default cost if no useful constraints
    pIdxInfo->estimatedCost = 100000;
    pIdxInfo->estimatedRows = zd_row_N(ctype);

    auto out = tempstr() << __FUNCTION__
                         << Keyval("ctype", ctype.ctype)
                         << Keyval("nConstraint", pIdxInfo->nConstraint);

    // Check for equality constraints
    for (auto i = 0; i < pIdxInfo->nConstraint; i++) {
        auto& sqlcon = pIdxInfo->aConstraint[i];
        auto& usage = pIdxInfo->aConstraintUsage[i];
        if (auto* field = c_field_Find(ctype, sqlcon.iColumn)) {
            out << Keyval(tempstr() << i << "u" << sqlcon.usable << "o" << sqlcon.op, field->field);
            if (sqlcon.usable) {
                auto& con = cons_Alloc(idx);
                con.op = sqlcon.op;
                con.p_field = field;
                con.icol = sqlcon.iColumn;
                switch (sqlcon.op) {
                    break; case SQLITE_INDEX_CONSTRAINT_EQ: {
                               usage.argvIndex = cons_N(idx);
                               // Improve cost estimates for indexed fields
                               if (sqlcon.iColumn == 0) {
                                   // Primary key - best case
                                   pIdxInfo->estimatedCost = 1;
                                   pIdxInfo->estimatedRows = 1;
                               }
                           }
                    break; case SQLITE_INDEX_CONSTRAINT_NE: usage.argvIndex = cons_N(idx);
                    break; default: cons_RemoveLast(idx); // drop if not handled
                }
            }
        }
    }

    if (cons_N(idx)) {
        pIdxInfo->idxNum = idx.rowid;
        out << Keyval("n_cons", cons_N(idx));
    } else {
        // if no constraint added drop the idx
        bestidx_RemoveLast();
    }
    out << Keyval("idxNum", pIdxInfo->idxNum);
    verblog(out);

    return SQLITE_OK;
}

static int xOpen(sqlite3_vtab *vtab, sqlite3_vtab_cursor **ppCursor){
    auto* tab = (Vtab*)vtab;
    auto* curs = new VtabCurs();
    *ppCursor = &curs->base;
    curs->p_ctype = tab->p_ctype;
    curs->p_vtab = tab;
    curs->rowid = c_curs_N(*tab);
    c_curs_Insert(*tab, *curs);
    ind_beg(ctype_c_field_curs, field, *tab->p_ctype) {
        auto &value = attrs_Alloc(*curs);
        (void)field;
        (void)value;
    }ind_end;

    return SQLITE_OK;
}

static int xClose(sqlite3_vtab_cursor* pCurs) {
    verblog(__FUNCTION__);
    auto* curs = (VtabCurs*)pCurs;
    delete curs;
    return SQLITE_OK;
}

static int xFilter(sqlite3_vtab_cursor* pCurs, int idxNum, const char* idxStr, int argc, sqlite3_value** argv) {
    (void)argv;
    (void)argc;
    (void)idxNum;
    (void)idxStr;
    auto& curs = *(VtabCurs*)pCurs;
    auto& ctype = *curs.p_ctype;
    // Reset cursor
    curs.eof = false;
    curs.c_row = nullptr;
    curs.c_idx = nullptr;

    auto* idx = bestidx_Find(idxNum);
    if (idxNum != 0) {
        curs.c_idx = idx;
        // Extract constraint values
        curs.c_pkey = nullptr;
        for (int i = 0; i < argc; i++) {
            auto& con = cons_qFind(*idx, i);
            con.value = (const char*)sqlite3_value_text(argv[i]);
            if (con.icol == 0) {
                curs.c_pkey = &con;
            }
        }
    }
    verblog(__FUNCTION__
            << Keyval("ctype", ctype.ctype)
            << Keyval("curs", curs.rowid)
            << Keyval("idxNum", idxNum)
            << Keyval("argc", argc));
    SsimCursor_Next(curs);
    return SQLITE_OK;
}

static int xNext(sqlite3_vtab_cursor* pCurs) {
    auto& curs = *(VtabCurs*)pCurs;
    SsimCursor_Next(curs);
    return SQLITE_OK;
}

static int xEof(sqlite3_vtab_cursor* pCurs) {
    auto& curs = *(VtabCurs*)pCurs;
    auto& ctype = *curs.p_ctype;
    verblog(__FUNCTION__
            << Keyval("ctype", ctype.ctype)
            << Keyval("curs", curs.rowid)
            << Keyval("eof", curs.eof));
    return curs.eof;
}

static int xColumn(sqlite3_vtab_cursor* pCurs,sqlite3_context* ctx,int i) {
    auto& curs = *(VtabCurs*)pCurs;
    auto& ctype = *curs.p_ctype;
    if (auto* field = c_field_Find(ctype, i)) {
        strptr value = algo::Zeroterm(attrs_qFind(curs, field->id));
        sqlite3_result_text(ctx, value.elems, value.n_elems, SQLITE_STATIC);
        verblog(__FUNCTION__
                << Keyval("curs", curs.rowid)
                << Keyval(field->field, value));
    }
    return SQLITE_OK;
}

static int xRowid(sqlite3_vtab_cursor *pCurs, sqlite_int64 *pRowid) {
    auto& curs = *(VtabCurs*)pCurs;
    auto& ctype = *curs.p_ctype;
    auto rowid = curs.c_row ? curs.c_row->rowid : -1;
    *pRowid = rowid;
    verblog(__FUNCTION__
            << Keyval("ctype", ctype.ctype)
            << Keyval("curs", curs.rowid)
            << Keyval("rowid", rowid)
            );
    return SQLITE_OK;
}

static int xUpdate(sqlite3_vtab *pVTab, int argc, sqlite3_value **argv, sqlite_int64 *pRowid) {
    (void)pVTab;
    (void)argc;
    (void)argv;
    (void)pRowid;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype) << Keyval("argc", argc));
    return SQLITE_OK;
}

static int xBegin(sqlite3_vtab *pVTab) {
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xSync(sqlite3_vtab *pVTab) {
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xCommit(sqlite3_vtab *pVTab) {
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xRollback(sqlite3_vtab *pVTab) {
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xFindFunction(sqlite3_vtab *pVTab, int nArg, const char *zName, void (**pxFunc)(sqlite3_context*,int,sqlite3_value**), void **ppArg) {
    (void)nArg;
    (void)zName;
    (void)pxFunc;
    (void)ppArg;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xRename(sqlite3_vtab *pVTab, const char *zName) {
    (void)zName;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xSavepoint(sqlite3_vtab *pVTab, int iSavepoint) {
    (void)iSavepoint;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xRelease(sqlite3_vtab *pVTab, int iSavepoint) {
    (void)iSavepoint;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xRollbackTo(sqlite3_vtab *pVTab, int iSavepoint) {
    (void)iSavepoint;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

static int xIntegrity(sqlite3_vtab *pVTab, const char *zSchema, const char *zTabName, int mFlags, char **pzErr) {
    (void)zSchema;
    (void)zTabName;
    (void)mFlags;
    (void)pzErr;
    auto& tab = *(Vtab*)pVTab;
    verblog(__FUNCTION__ << Keyval("ctype", tab.p_ctype->ctype));
    return SQLITE_OK;
}

sqlite3_module lib_sqlite::SsimModule = {
    0,             /* iVersion */
    xCreate,       /* xCreate */
    xConnect,      /* xConnect */
    xBestIndex,    /* xBestIndex */
    xDisconnect,   /* xDisconnect */
    xDisconnect,   /* xDestroy */
    xOpen,         /* xOpen - open a cursor */
    xClose,        /* xClose - close a cursor */
    xFilter,       /* xFilter - configure scan constraints */
    xNext,         /* xNext - advance a cursor */
    xEof,          /* xEof - check for end of scan */
    xColumn,       /* xColumn - read data */
    xRowid,        /* xRowid - read data */
    xUpdate,       /* xUpdate */
    xBegin,        /* xBegin */
    xSync,         /* xSync */
    xCommit,       /* xCommit */
    xRollback,     /* xRollback */
    xFindFunction, /* xFindFunction */
    xRename,       /* xRename */
    xSavepoint,    /* xSavepoint */
    xRelease,      /* xRelease */
    xRollbackTo,   /* xRollbackTo */
    0,   /* xShadowName */
    xIntegrity,    /* xIntegrity */
};


// Scalar function to initialize virtual tables with given data path
void lib_sqlite::VtabInitFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    if (argc != 2) {
        sqlite3_result_error(context, "init_ssim() requires 2 args : data and ns", -1);
        return;
    }

    const char* data_path = (const char*)sqlite3_value_text(argv[0]);
    if (!data_path) {
        sqlite3_result_error(context, "data path cannot be null", -1);
        return;
    }
    const char* ns_c = (const char*)sqlite3_value_text(argv[1]);
    if (!data_path) {
        sqlite3_result_error(context, "ns  cannot be null", -1);
        return;
    }
    tempstr ns_str(ns_c);
    algo_lib::Regx ns_regex;
    algo_lib::Regx_ReadSql(ns_regex, ns_str, true);

    sqlite3* db = sqlite3_context_db_handle(context);
    auto create = cstring();
    algo_lib::Replscope R;

    ind_beg(lib_sqlite::_db_ns_curs,ns,lib_sqlite::_db) if (ns.nstype == dmmeta_Nstype_nstype_ssimdb) {
        if (Regx_Match(ns_regex, ns.ns)) {
            Set(R,"$ns",ns.ns);
            Ins(&R,create, "attach ':memory:' as $ns;");
            ind_beg(lib_sqlite::ns_c_ssimfile_curs,ssimfile,ns) {
                Set(R,"$data",data_path);
                Set(R,"$ssimfile",ssimfile.ssimfile);
                Ins(&R,create, "create virtual table $ssimfile using ssimdb( $data , $ssimfile );");
            }ind_end;
        }
    }ind_end;

    int rc = sqlite3_exec(db, algo::Zeroterm(create), nullptr, nullptr, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_result_text(context, "Virtual tables initialized successfully", -1, SQLITE_STATIC);
    } else {
        sqlite3_result_error(context, sqlite3_errmsg(db), -1);
    }
}

//Entrypoint for vtab extension
int lib_sqlite::VtabInitExt(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
    (void)pzErrMsg;
    (void)pApi;
    lib_sqlite::Init();

    // Register scalar function for initializing virtual tables
    auto rc = sqlite3_create_function(db, "init_ssim", 2, SQLITE_UTF8, nullptr,
                                      VtabInitFunc, nullptr, nullptr);
    if (rc == SQLITE_OK) {
        rc = sqlite3_create_module(db, "ssimdb", &lib_sqlite::SsimModule, 0);
    }

    return rc;
}
