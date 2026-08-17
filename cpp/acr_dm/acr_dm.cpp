// Copyright (C) 2023-2024,2026 AlgoRND
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

// Attach ROW under ANCHOR as element SEQ of the run named by RUNKEY.
// Rows a branch added come out before the base file's own row at that anchor, which is
// what puts an insertion between the two rows it was written between.  Two branches
// that both inserted at one anchor are separated by RUNKEY, the key of the first row
// of each run: the only comparison ever made between two branches is a comparison of
// keys, and a key cannot tell which branch git passed second.
void acr_dm::AnchorTuple(acr_dm::FTuple &row, acr_dm::FTuple &anchor, bool basefile, strptr runkey, int seq) {
    row.p_anchor = &anchor;
    row.sortkey.base = basefile ? 1 : 0;
    row.sortkey.runkey = runkey;
    row.sortkey.seq = seq;
    bh_child_Insert(anchor,row);
}

//------------------------------------------------------------------------------

// Load all files
void acr_dm::Main_LoadFiles() {
    vrfy(arg_N(_db.cmdline)<ssizeof(Source)*8,"Too many args");
    ind_beg(command::acr_dm_arg_curs,arg,_db.cmdline) {
        bool basefile = ind_curs(arg).index==0;
        // Rows this file adds one after another form a run, and the whole run hangs off
        // the last row the file had in common with what the merge already knew.  The
        // run, not the individual row, is what gets placed, which is what keeps a
        // branch's additions together and stops one branch's run from being split by
        // another's.  Before the first shared row there is no such row, so a run at the
        // top of a file hangs off the virtual root and lands ahead of the base file.
        //
        // Hanging a run off one anchor is also what keeps the output walk shallow.
        // Anchoring each row on the row above it would make a file added whole by one
        // branch into a chain as long as the file, and walking it would recurse once
        // per row until the stack ran out.
        FTuple *anchor = _db.p_root;
        cstring runkey;
        int runseq = 0;
        verblog("-- reading file "<<arg);
        ind_beg(algo::FileLine_curs,line,arg) {
            Tuple tuple;
            if (line != "" && Tuple_ReadStrptrMaybe(tuple,line)) {
                vrfy(attrs_N(tuple)>=1,"Missing key attribute");
                cstring tuple_key = tempstr()
                    << tuple.head.value
                    << Keyval(tuple.attrs_elems->name,tuple.attrs_elems->value);
                int nbefore=ind_tuple_N();
                FTuple &ftuple = ind_tuple_GetOrCreate(tuple_key);
                bool isnew=ind_tuple_N() > nbefore;
                source_SetBit(ftuple.source,ind_curs(arg).index);
                if (isnew) {
                    if (runseq == 0) {
                        runkey = tuple_key;// this row opens the run and gives it its name
                    }
                    AnchorTuple(ftuple, *anchor, basefile, runkey, runseq);
                    runseq++;
                    verblog("# tuple "<<tuple_key<<" follows "<<ftuple.p_anchor->key);
                } else {
                    // A row the merge already knew ends the run and becomes the anchor
                    // that the next run hangs off.
                    anchor = &ftuple;
                    runseq = 0;
                }
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
        ind_beg(_db_tuple_curs,tuple,_db) if (&tuple != _db.p_root) {
            out << tuple.key;
            PrintSource(tuple.source,out);
            out << Keyval("follows",tuple.p_anchor->key);
            ind_beg(tuple_zs_attr_curs,attr,tuple) {
                ind_beg(attr_zs_value_curs,value,attr) {
                    out << Keyval(attr.name,value.value);
                    PrintSource(value.source,out);
                }ind_end;
            }ind_end;
            out << eol;
        }ind_end;
        prcat(debug,out);
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

// Print whatever the caller asked to see beside ROW, which came out at position POS.
// `acr.rowid` is the position itself, because that is what a reader of the merged file
// needs: acr orders the rows of one sortkey group by rowid, and a number that only
// rises keeps every group in the order the merge put it in.  `acr_dm.follows` is the
// anchor, which is the merge's own account of where it decided the row goes.
void acr_dm::PrintPos(acr_dm::FTuple &row, int pos, cstring &out) {
    if (_db.cmdline.rowid) {
        out << Keyval("acr.rowid", pos);
    }
    if (_db.cmdline.anchor) {
        out << Keyval("acr_dm.follows", row.p_anchor->key);
    }
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
void acr_dm::PrintSourceTuple(FTuple &tuple, int source, int pos, cstring &out) {
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
        out << temp;
        PrintPos(tuple,pos,out);
        out << eol;
    }
}

//------------------------------------------------------------------------------

// Print merged tuple, return false in case of conflict
bool acr_dm::MergeTuple(FTuple &tuple, int pos, cstring &out) {
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
        out << temp;
        PrintPos(tuple,pos,out);
        out << eol;
    }
    return !conflict;
}

//------------------------------------------------------------------------------

// Print every row that came after TUPLE, each one followed in turn by the rows that
// came after it.  The bheap hands the rows back in sortkey order, so the rows a branch
// added come out ordered by key and the rows of the base file come out in the order
// the base file had them, the added ones ahead of the base row they were placed before.
void acr_dm::PrintSubtree(acr_dm::FTuple &tuple, int &pos, cstring &out) {
    ind_beg(tuple_bh_child_curs,child,tuple) {
        cstring merged;
        if (MergeTuple(child,pos,merged)) {
            // no conflict
            // print tuple (may be empty when deleted)
            out << merged;
        } else {
            // conflict
            PrintConflictMarker('<',*arg_Find(_db.cmdline,1),out);
            PrintSourceTuple(child,1,pos,out);
            PrintConflictMarker('=',"",out);
            for (int i=2; i<arg_N(_db.cmdline); ++i) {
                PrintSourceTuple(child,i,pos,out);
                PrintConflictMarker('>',*arg_Find(_db.cmdline,i),out);
            }
            algo_lib::_db.exit_code = 1;
        }
        pos++;
        PrintSubtree(child,pos,out);
    }ind_end;
}

//------------------------------------------------------------------------------

// Merge
void acr_dm::Main_Merge() {
    cstring out;
    int pos=0;
    PrintSubtree(*_db.p_root,pos,out);
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
    // Every file's first line comes after nothing, and this row is that nothing.
    // Giving it a record means no row is a special case: each one has an anchor, and
    // the whole result is the rows that came after this one.  It is never printed.
    _db.p_root = &tuple_Alloc();
    Main_LoadFiles();
    Main_Dump();
    Main_Merge();
}
