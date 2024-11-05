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
#include "include/lib_ctype.h"
#include "include/lib_sqlite.h"


static tempstr CreateCtypeTable(lib_ctype::FCtype& ctype) {
    auto cmd = tempstr();
    cmd << "CREATE TABLE x (";
    algo::ListSep ls(",");
    ind_beg(lib_ctype::ctype_c_field_curs, field, ctype) {
        auto sqltype = field.p_arg->c_sqltype ? strptr(field.p_arg->c_sqltype->expr) : "TEXT";
        if (algo::FindStr(sqltype, "enum") >= 0) {
            sqltype = "TEXT";
        }
        cmd << ls << "'" << name_Get(field) << "' " << sqltype;
    }ind_end;
    cmd << ")";
    return cmd;
}

static bool CtypeTuple_ReadMaybe(algo::Tuple& row, lib_ctype::FCtype& ctype, strptr line) {
    algo::Refurbish(row);
    algo::Tuple _row;
    auto retval = algo::Tuple_ReadStrptrMaybe(_row, line);
    row.head = _row.head;
    ind_beg(lib_ctype::ctype_c_field_curs,field,ctype) {
        auto& attr = attrs_Alloc(row);
        attr.name = name_Get(field);
        if (auto* sub = field.c_substr) {
            if (auto* src = attr_Find(_row,dmmeta::Field_name_Get(sub->srcfield))) {
                attr.value = Pathcomp(src->value, sub->expr.value);
            }
        } else {
            if (auto* src = attr_Find(_row,name_Get(field))) {
                attr.value = src->value;
            }
        }
    }ind_end;
    return retval;
}

static bool MatchTag(lib_sqlite::Vtab& tab, strptr tag) {
    auto res = false;
    if (tab.c_ssimfile && tag == tab.c_ssimfile->ssimfile) {
        res = true;
    } else {
        res = tab.p_ctype->ctype == tag;
    }
    return res;
}
static void SsimCursor_Next(lib_sqlite::VtabCurs& curs) {
    auto& tab = *(lib_sqlite::Vtab*)curs.base.pVtab;
    curs.eof = !ReadLine(curs.file, curs.line);
    while (!curs.eof && !MatchTag(tab, algo::GetTypeTag(curs.line))) {
        curs.eof = !ReadLine(curs.file, curs.line);
    }
    curs.i++;
    CtypeTuple_ReadMaybe(curs.row,*tab.p_ctype,curs.line);
}

static void SsimCursor_Reset(lib_sqlite::VtabCurs& curs) {
    const auto& tab = *(lib_sqlite::Vtab*)curs.base.pVtab;
    algo::Refurbish(curs.file);
    algo::Refurbish(curs.row);
    curs.file.file.fd = OpenRead(tab.filename);
    curs.file.own_fd = true;
    curs.line = strptr();
    curs.i=0;
    SsimCursor_Next(curs);
}

static bool SsimCursor_ValidQ(lib_sqlite::VtabCurs& curs) {
    return !curs.eof;
}

static algo::Tuple& SsimCursor_Access(lib_sqlite::VtabCurs& curs) {
    return curs.row;
}

static int xDisconnect(sqlite3_vtab* pVtab) {
    verblog(__PRETTY_FUNCTION__);
    auto* tab = (lib_sqlite::Vtab*)pVtab;
    delete tab;
    return SQLITE_OK;
}
static int xConnect(sqlite3 *db, void *, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr){
    verblog(__PRETTY_FUNCTION__);
    if (argc != 5) {
        *pzErr = sqlite3_mprintf("%s", "invalid number of arguments");
        return SQLITE_ERROR;
    }

    auto data = algo::Trimmed(strptr(argv[3]));
    auto type_tag = algo::Trimmed(argv[4]);
    auto* ctype = lib_ctype::TagToCtype(type_tag);
    if (!ctype) {
        *pzErr = sqlite3_mprintf("create ctype:%s comment:ctype not found", type_tag);
        return SQLITE_ERROR;
    }

    auto rc = SQLITE_OK;
    auto* ctx = new lib_sqlite::Vtab();
    *ppVtab = (sqlite3_vtab*)ctx;
    if (FileQ(data)) {
        ctx->filename = data;
    } else if (data == "-") {
        ctx->filename = "/dev/stdin";
    } else if (DirectoryQ(data)) {
        ctx->filename = algo::SsimFname(data, type_tag);
    }
    ctx->p_ctype = ctype;
    ctx->c_ssimfile = lib_ctype::ind_ssimfile_Find(type_tag);
    char * schmea = sqlite3_mprintf("%s", algo::Zeroterm(CreateCtypeTable(*ctype)));
    verblog(schmea);
    rc = sqlite3_declare_vtab(db, schmea);
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
static int xCreate(sqlite3 *db, void *pAux, int argc, const char *const*argv, sqlite3_vtab **ppVtab, char **pzErr){
    verblog(__PRETTY_FUNCTION__);
    return xConnect(db, pAux, argc, argv, ppVtab, pzErr);
}
static int xBestIndex(sqlite3_vtab*,sqlite3_index_info* pIdxInfo) {
    verblog(__PRETTY_FUNCTION__);
    pIdxInfo->estimatedCost = 1000000;
    return SQLITE_OK;
}
static int xOpen(sqlite3_vtab *, sqlite3_vtab_cursor **ppCursor){
    verblog(__PRETTY_FUNCTION__);
    auto* curs = new lib_sqlite::VtabCurs();
    *ppCursor = &curs->base;
    return SQLITE_OK;
}
static int xClose(sqlite3_vtab_cursor* pCurs) {
    verblog(__PRETTY_FUNCTION__);
    auto* curs = (lib_sqlite::VtabCurs*)pCurs;
    delete curs;
    return SQLITE_OK;
}
static int xFilter(sqlite3_vtab_cursor* pCurs, int, const char *, int, sqlite3_value **) {
    verblog(__PRETTY_FUNCTION__);
    auto& curs = *(lib_sqlite::VtabCurs*)pCurs;
    SsimCursor_Reset(curs);
    return SQLITE_OK;
}
static int xNext(sqlite3_vtab_cursor* pCurs) {
    verblog(__PRETTY_FUNCTION__);
    auto& curs = *(lib_sqlite::VtabCurs*)pCurs;
    SsimCursor_Next(curs);
    return SQLITE_OK;
}
static int xEof(sqlite3_vtab_cursor* pCurs) {
    verblog(__PRETTY_FUNCTION__);
    auto& curs = *(lib_sqlite::VtabCurs*)pCurs;
    return SsimCursor_ValidQ(curs) ? SQLITE_OK : SQLITE_ERROR;
}
static int xColumn(sqlite3_vtab_cursor* pCurs,sqlite3_context* ctx,int i) {
    verblog(__PRETTY_FUNCTION__);
    auto& curs = *(lib_sqlite::VtabCurs*)pCurs;
    auto& row = SsimCursor_Access(curs);
    if (i>=0 && i<attrs_N(row)) {
        auto& value = attrs_qFind(row, i).value;
        sqlite3_result_text(ctx, value.ch_elems, value.ch_n, SQLITE_STATIC);
    }
    return SQLITE_OK;
}
static int xRowid(sqlite3_vtab_cursor *pCurs, sqlite_int64 *pRowid) {
    verblog(__PRETTY_FUNCTION__);
    auto& curs = *(lib_sqlite::VtabCurs*)pCurs;
    *pRowid = curs.i;
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
    0,             /* xUpdate */
    0,             /* xBegin */
    0,             /* xSync */
    0,             /* xCommit */
    0,             /* xRollback */
    0,             /* xFindFunction */
    0,             /* xRename */
    0,             /* xSavepoint */
    0,             /* xRelease */
    0,             /* xRollbackTo */
    0,             /* xShadowName */
    0,             /* xIntegrity */
};
