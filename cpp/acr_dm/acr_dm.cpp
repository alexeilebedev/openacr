// Copyright (C) 2023 AlgoRND
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
// Target: acr_dm (exe) -- ACR Diff/Merge
// Exceptions: yes
// Source: cpp/acr_dm/acr_dm.cpp
//

#include "include/algo.h"
#include "include/acr_dm.h"

//------------------------------------------------------------------------------

// Get or create tuple attribute
acr_dm::FAttr &acr_dm::zs_attr_GetOrCreate(FTuple &tuple, strptr name) {
    FAttr *ret = NULL;
    ind_beg(tuple_zs_attr_curs,attr,tuple) {
        if (attr.name == name) {
            ret = &attr;
            break;
        }
    }ind_end;
    if (!ret) {
        ret = &attr_Alloc();
        ret->p_tuple = &tuple;
        ret->name = name;
        attr_XrefMaybe(*ret);
    }
    return *ret;
}

//------------------------------------------------------------------------------

// Get or create attribute value
acr_dm::FValue &acr_dm::zs_value_GetOrCreate(FAttr &attr, strptr val) {
    FValue *ret = NULL;
    ind_beg(attr_zs_value_curs,value,attr) {
        if (value.value == val) {
            ret = &value;
            break;
        }
    }ind_end;
    if (!ret) {
        ret = &value_Alloc();
        ret->p_attr = &attr;
        ret->value = val;
        value_XrefMaybe(*ret);
    }
    return *ret;
}

//------------------------------------------------------------------------------

// Load all files
void acr_dm::Main_LoadFiles() {
    vrfy(arg_N(_db.cmdline)<ssizeof(Source)*8,"Too many args");
    ind_beg(command::acr_dm_arg_curs,arg,_db.cmdline) {
        acr_dm::Rowid next_rowid(0,0);
        verblog("-- reading file "<<arg<<". next rowid "<<next_rowid);
        ind_beg(algo::FileLine_curs,line,arg) {
            Tuple tuple;
            if (Tuple_ReadStrptrMaybe(tuple,line)) {
                vrfy(attrs_N(tuple)>=1,"Missing key attribute");
                cstring tuple_key = tempstr()
                    << tuple.head.value
                    << Keyval(tuple.attrs_elems->name,tuple.attrs_elems->value);
                FTuple &ftuple = ind_tuple_GetOrCreate(tuple_key);
                source_SetBit(ftuple.source,ind_curs(arg).index);
                if (ftuple.rowid.f1 == 0) {// new entry
                    if (ind_curs(arg).index==0) {// first file
                        next_rowid.f1++;
                    } else {
                        next_rowid.f2++;
                    }
                    ftuple.rowid = next_rowid;
                } else {
                    next_rowid = ftuple.rowid;
                }
                bh_tuple_Insert(ftuple);// sort it
                verblog("# tuple "<<tuple_key<<" gets "<<ftuple.rowid<<". next rowid "<<next_rowid);
                ind_beg(algo::Tuple_attrs_curs,attr,tuple) if (ind_curs(attr).index) {
                    FAttr &fattr = zs_attr_GetOrCreate(ftuple,attr.name);
                    FValue &fvalue = zs_value_GetOrCreate(fattr,attr.value);
                    source_SetBit(fvalue.source,ind_curs(arg).index);
                }ind_end;
            }
        }ind_end;
    }ind_end;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Print list of sources (for dump only)
void acr_dm::PrintSource(acr_dm::Source &source, cstring &out) {
    algo::ListSep ls(",");
    out << "(";
    frep_(i,source_Nbits(source)) {
        if (source_GetBit(source,i)) {
            out << ls << i;
        }
    }
    out << ")";
}

//------------------------------------------------------------------------------

// Dump
void acr_dm::Main_Dump() {
    if (algo_lib::_db.cmdline.debug) {
        cstring out;
        ind_beg(_db_bh_tuple_curs,tuple,_db) {
            out << tuple.key;
            PrintSource(tuple.source,out);
            ind_beg(tuple_zs_attr_curs,attr,tuple) {
                ind_beg(attr_zs_value_curs,value,attr) {
                    out << Keyval(attr.name,value.value);
                    PrintSource(value.source,out);
                }ind_end;
            }ind_end;
            out << eol;
        }ind_end;
        dbglog(out);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// whether an item has been removed in some source
inline bool acr_dm::RemovedQ(acr_dm::Source source) {
    return source_GetBit(source,0)
        && int(source_Sum1s(source))!=arg_N(_db.cmdline);
}

//------------------------------------------------------------------------------

// print conflict marker
void acr_dm::PrintConflictMarker(char mark, strptr source_name, cstring &out) {
    char_PrintNTimes(mark,out,_db.cmdline.msize);
    if (ch_N(source_name)) {
        out << " " << source_name;
    }
    out << eol;
}

//------------------------------------------------------------------------------

// Print tuple for given source
void acr_dm::PrintSourceTuple(FTuple &tuple, int source, cstring &out) {
    tempstr temp;
    if (source_GetBit(tuple.source,source)) {
        temp << tuple.key;
        ind_beg(tuple_zs_attr_curs,attr,tuple) {
            ind_beg(attr_zs_value_curs,value,attr) {
                if (source_GetBit(value.source,source)) {
                    temp << Keyval(attr.name,value.value);
                    break;
                }
            }ind_end;
        }ind_end;
    }
    if (ch_N(temp)) {
        out << temp << eol;
    }
}

//------------------------------------------------------------------------------

// Print merged tuple, return false in case of conflict
bool acr_dm::MergeTuple(FTuple &tuple, cstring &out) {
    bool conflict(false);
    tempstr temp;
    temp << tuple.key;
    ind_beg(tuple_zs_attr_curs,attr,tuple) {
        bool do_print(false);
        switch (zs_value_N(attr)) {
        case 0:
            // impossible
            break;
        case 1:
            // single unique value - print if not removed
            do_print = !RemovedQ(zs_value_First(attr)->source);
            break;
        case 2:
            // two unique values -
            // older(1) - ours or theirs(2) - changed value
            // ours(1) - theirs(2) - distinct values - conflict
            conflict = !source_GetBit(zs_value_First(attr)->source,0);
            do_print = !conflict;
            break;
        default:
            // more than two unique values - conflict
            conflict = true;
            break;
        }
        if (do_print) {
            temp << Keyval(attr.name,zs_value_Last(attr)->value);
        }
        if (conflict) {
            break;
        }
    } ind_end;
    if (!RemovedQ(tuple.source) && !conflict) {
        out << temp << eol;
    }
    return !conflict;
}

//------------------------------------------------------------------------------

// Merge
void acr_dm::Main_Merge() {
    cstring out;
    ind_beg(_db_bh_tuple_curs,tuple,_db) {
        cstring merged;
        if (MergeTuple(tuple,merged)) {
            // no conflict
            // print tuple (may be empty when deleted)
            out << merged;
        } else {
            // conflict
            PrintConflictMarker('<',*arg_Find(_db.cmdline,1),out);
            PrintSourceTuple(tuple,1,out);
            PrintConflictMarker('=',"",out);
            for (int i=2; i<arg_N(_db.cmdline); ++i) {
                PrintSourceTuple(tuple,i,out);
                PrintConflictMarker('>',*arg_Find(_db.cmdline,i),out);
            }
            algo_lib::_db.exit_code = 1;
        }
    }ind_end;
    // print or save
    if (arg_N(_db.cmdline)>=3 && _db.cmdline.write_ours && FileQ(*arg_Find(_db.cmdline,1))) {
        StringToFile(out,*arg_Find(_db.cmdline,1));
    } else {
        prlog_(out);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Main routine
void acr_dm::Main() {
    Main_LoadFiles();
    Main_Dump();
    Main_Merge();
}
