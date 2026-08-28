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
// The whole tool: read the files git hands the merge driver, merge them as sets of
// rows, print the result.  A row is identified by its key, and everything else about
// it -- each attribute's value, the text of the lines above it, and the row it comes
// after -- is a thing the files may disagree about.  Every one of those is merged by
// the same rule: one value stands, two values with the base file's among them are a
// change and the changed one stands, and two values with the base file's in neither
// are a conflict that goes into the file between markers.
// Position is the part that ssim needs and a text merge cannot give.  Row order in
// data/dmmeta/field.ssim is the member order of a generated struct, so a row that
// comes out where the base file had it, in a file that moved it, is a struct laid
// out the way nobody asked for.  Position is carried as a tree: each row hangs off
// the row it follows, and the merged file is a walk of that tree.

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

// Record that file SOURCE showed ROW with TEXT above it.
// A line that names no row belongs to the row below it, which is what apm means by the
// `# SHA1 = <base64>` line it writes above each dev.gitfile row.  Held as the row's one
// nameless attribute, the text merges as every other attribute does, so a checksum that
// changed on one side is a change and one that changed on both is a conflict.
void acr_dm::AttachCmt(acr_dm::FTuple &row, strptr text, int source) {
    FAttr &fattr = zs_attr_GetOrCreate(row,"");
    FValue &fvalue = zs_value_GetOrCreate(fattr,text);
    source_SetBit(fvalue.source,source);
}

//------------------------------------------------------------------------------

// Attach ROW under ANCHOR as element SEQ of the run named by RUNKEY.
// Rows a branch added come out before the base file's own row at that anchor, which is
// what puts an insertion between the two rows it was written between.  Two branches
// that both inserted at one anchor are separated by RUNKEY, the key of the first row
// of each run: the only comparison ever made between two branches is a comparison of
// keys, and a key cannot tell which branch git passed second.
// The row does not enter its anchor's heap here.  A later file may move it, and a row
// that is placed once and never removed is a row no heap has to give back.
void acr_dm::AnchorTuple(acr_dm::FTuple &row, acr_dm::FTuple &anchor, bool basefile, strptr runkey, int seq) {
    row.p_anchor = &anchor;
    row.sortkey.base = basefile ? 1 : 0;
    row.sortkey.runkey = runkey;
    row.sortkey.seq = seq;
}

//------------------------------------------------------------------------------

// Whether ROW is CAND, or one of the rows CAND hangs off on the way to the root.
bool acr_dm::AncestorQ(acr_dm::FTuple &row, acr_dm::FTuple &cand) {
    bool ret = false;
    FTuple *cur = &cand;
    while (cur && !ret) {
        ret = cur == &row;
        cur = cur->p_anchor;
    }
    return ret;
}

//------------------------------------------------------------------------------

// Place ROW where file SOURCE puts it: after the run hanging off ANCHOR, as element SEQ
// of the run named by RUNKEY.  A row the base file also has, and which this file placed
// somewhere else, is a row this file moved, and a move is merged the way an attribute is
// -- the one file that moved it decides where it goes, and two files that moved it apart
// disagree, which is a conflict.
// A conflict still has to leave the row somewhere, and which of the two contested
// positions the file ends up holding cannot depend on which argument slot git put a
// branch in: a rebase hands over the same files a merge does with ours and theirs
// swapped.  So the two are ordered by the key of the row each would follow, a key being
// the one thing that cannot tell one branch from the other, and the conflict names them
// both either way.
// Two moves can also ask for a cycle: one file moves a after b while the other moves b
// after a.  A cycle is refused rather than merged, because the printed file is a walk
// from the root and a row on a cycle is a row the walk never reaches -- it would come
// out of the merge missing, which is the one outcome worse than a conflict.
void acr_dm::MoveTuple(acr_dm::FTuple &row, acr_dm::FTuple &anchor, strptr runkey, int seq, int source) {
    bool contested = source_Sum1s(row.moved) > 0 && row.p_anchor != &anchor;
    bool cyclic = AncestorQ(row,anchor);
    bool conflict = contested || cyclic;
    bool take = !cyclic && (!contested || anchor.key < row.p_anchor->key);
    FTuple *lose = take ? row.p_anchor : &anchor;
    source_SetBit(row.moved,source);
    row.moveconflict = row.moveconflict || conflict;
    if (conflict) {
        row.p_wanted = lose;
        prerr("acr_dm.moveconflict"
              <<Keyval("row",row.key)
              <<Keyval("follows",take ? strptr(anchor.key) : strptr(row.p_anchor->key))
              <<Keyval("wanted",lose->key)
              <<Keyval("cycle",cyclic ? "Y" : "N")
              <<Keyval("source",source));
    }
    if (take) {
        AnchorTuple(row,anchor,false,runkey,seq);
    }
}

//------------------------------------------------------------------------------

// Load all files
void acr_dm::Main_LoadFiles() {
    vrfy(arg_N(_db.cmdline)<ssizeof(Source)*8,"Too many args");
    ind_beg(command::acr_dm_arg_curs,arg,_db.cmdline) {
        int source = ind_curs(arg).index;
        bool basefile = source==0;
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
        //
        // A row the base file has can also arrive out of the order the base file had it
        // in, and that row is one this file moved.  It joins the run the same way an
        // added row does, so a block of rows moved together comes out as one run, in
        // the order this file wrote them, and adds one level to the walk rather than
        // one level per row.
        FTuple *anchor = _db.p_root;
        cstring runkey;
        cstring cmt;
        int runseq = 0;
        int nbase = 0;
        int lastbaseseq = -1;
        // A file that is not there reads as a file with no rows, and a merge whose theirs
        // holds no rows says every row was deleted.  The result is an empty file at exit
        // zero, written over the ours file when git is driving.  An empty file, on the
        // other hand, is a legitimate input: apm merges one for a package whose incoming
        // version dropped every record it had.  So the test is existence, not size.
        vrfy(FileQ(arg),tempstr()<<"no such file: "<<arg);
        verblog("-- reading file "<<arg);
        ind_beg(algo::FileLine_curs,line,arg) if (line != "") {
            Tuple tuple;
            bool parsed = Tuple_ReadStrptrMaybe(tuple,line);
            bool keyed = parsed && attrs_N(tuple)>=1;
            if (!keyed) {
                // A line the merge cannot key is text belonging to the row below it.  Held
                // rather than dropped: a comment carries the checksum of the row under it,
                // and a line nobody can key is still a line somebody wrote.
                if (ch_N(cmt)) {
                    cmt << eol;
                }
                cmt << line;
            } else {
                cstring tuple_key = tempstr()
                    << tuple.head.value
                    << Keyval(tuple.attrs_elems->name,tuple.attrs_elems->value);
                int nbefore=ind_tuple_N();
                FTuple &ftuple = ind_tuple_GetOrCreate(tuple_key);
                bool isnew=ind_tuple_N() > nbefore;
                source_SetBit(ftuple.source,source);
                if (basefile) {
                    ftuple.baseseq = nbase;
                    nbase++;
                }
                bool baserow = source_GetBit(ftuple.source,0);
                bool moved = !isnew && !basefile && baserow && ftuple.baseseq < lastbaseseq;
                if (isnew || moved) {
                    if (runseq == 0) {
                        runkey = tuple_key;// this row opens the run and gives it its name
                    }
                    if (isnew) {
                        AnchorTuple(ftuple, *anchor, basefile, runkey, runseq);
                    } else {
                        MoveTuple(ftuple, *anchor, runkey, runseq, source);
                    }
                    runseq++;
                    verblog("# tuple "<<tuple_key<<" follows "<<ftuple.p_anchor->key);
                } else {
                    // A row the merge already knew, in the order the base file has it, ends
                    // the run and becomes the anchor that the next run hangs off.
                    anchor = &ftuple;
                    runseq = 0;
                    if (baserow) {
                        lastbaseseq = ftuple.baseseq;
                    }
                }
                if (ch_N(cmt)) {
                    AttachCmt(ftuple,cmt,source);
                    cmt = "";
                }
                ind_beg(algo::Tuple_attrs_curs,attr,tuple) if (ind_curs(attr).index) {
                    FAttr &fattr = zs_attr_GetOrCreate(ftuple,attr.name);
                    FValue &fvalue = zs_value_GetOrCreate(fattr,attr.value);
                    source_SetBit(fvalue.source,source);
                }ind_end;
            }
        }ind_end;
        if (ch_N(cmt)) {
            // Lines with no row below them are the file's tail.  The root is where the tail
            // goes: it is the one row that is never printed as a row, so its own text is
            // free to stand for what comes after every row there is.
            AttachCmt(*_db.p_root,cmt,source);
        }
    }ind_end;
}

//------------------------------------------------------------------------------

// Put every row into the heap of the row it follows, which is what fixes the printed order.
// Loading decides where a row goes and may revise it, so the heaps are filled once loading
// is over and nothing is ever taken back out of one.
void acr_dm::Main_LinkTuple() {
    ind_beg(_db_tuple_curs,tuple,_db) if (&tuple != _db.p_root) {
        bh_child_Insert(*tuple.p_anchor,tuple);
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
            if (source_Sum1s(tuple.moved) > 0) {
                out << Keyval("moved","");
                PrintSource(tuple.moved,out);
            }
            ind_beg(tuple_zs_attr_curs,attr,tuple) {
                ind_beg(attr_zs_value_curs,value,attr) {
                    strptr name = ch_N(attr.name) ? strptr(attr.name) : strptr("cmt");
                    out << Keyval(name,value.value);
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

// Decide the one value ATTR keeps, and answer whether the files can agree on it at all.
// A single value stands unless some file removed it.  Two values, one of them the base
// file's, are one file's change to it, and the change stands.  Two values with the base
// file's in neither, or more than two, are files disagreeing, which is a conflict.
// PRINT says whether the value belongs in the merged row, VALUE is that value, and the
// return is false on conflict.
bool acr_dm::MergeAttr(acr_dm::FAttr &attr, bool &print, strptr &value) {
    bool conflict = false;
    print = false;
    switch (zs_value_N(attr)) {
    case 0:
        // impossible
        break;
    case 1:
        // single unique value - print if not removed
        print = !RemovedQ(zs_value_First(attr)->source);
        break;
    case 2:
        // older(1) - ours or theirs(2) - changed value
        // ours(1) - theirs(2) - distinct values - conflict
        conflict = !source_GetBit(zs_value_First(attr)->source,0);
        print = !conflict;
        break;
    default:
        // more than two unique values - conflict
        conflict = true;
        break;
    }
    if (print) {
        value = zs_value_Last(attr)->value;
    }
    return !conflict;
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
    tempstr cmt;
    tempstr temp;
    if (source_GetBit(tuple.source,source)) {
        temp << tuple.key;
        ind_beg(tuple_zs_attr_curs,attr,tuple) {
            ind_beg(attr_zs_value_curs,value,attr) {
                if (source_GetBit(value.source,source)) {
                    if (ch_N(attr.name)) {
                        temp << Keyval(attr.name,value.value);
                    } else {
                        cmt << value.value << eol;
                    }
                    break;
                }
            }ind_end;
        }ind_end;
    }
    if (ch_N(temp)) {
        out << cmt;
        out << temp;
        PrintPos(tuple,pos,out);
        out << eol;
    }
}

//------------------------------------------------------------------------------

// Print merged tuple, return false in case of conflict
bool acr_dm::MergeTuple(FTuple &tuple, int pos, cstring &out) {
    bool conflict(tuple.moveconflict);
    tempstr cmt;
    tempstr temp;
    temp << tuple.key;
    ind_beg(tuple_zs_attr_curs,attr,tuple) if (!conflict) {
        bool print = false;
        strptr value;
        conflict = !MergeAttr(attr,print,value);
        if (print && ch_N(attr.name)) {
            temp << Keyval(attr.name,value);
        } else if (print) {
            cmt << value << eol;
        }
    } ind_end;
    if (!RemovedQ(tuple.source) && !conflict) {
        out << cmt;
        out << temp;
        PrintPos(tuple,pos,out);
        out << eol;
    }
    return !conflict;
}

//------------------------------------------------------------------------------

// Say in the file why the rows between the markers below it disagree, when what they
// disagree about is where ROW goes.
// Two files that place one row differently produce a conflict whose two sides are the
// same row, printed twice, identical to the byte -- the text of a row says nothing about
// where in the file it sits.  A reader who cannot see the disagreement cannot resolve
// it, and a marker block that looks like a copy of itself is the kind of artifact that
// gets resolved by picking a side at random.  So the two positions are named above it.
void acr_dm::PrintMoveConflict(acr_dm::FTuple &row, cstring &out) {
    if (row.moveconflict) {
        out << "# acr_dm.moveconflict"
            << Keyval("row",row.key)
            << Keyval("follows",row.p_anchor->key)
            << Keyval("wanted",row.p_wanted ? strptr(row.p_wanted->key) : strptr())
            << eol;
    }
}

//------------------------------------------------------------------------------

// Print every row that came after TUPLE, each one followed in turn by the rows that
// came after it.  The bheap hands the rows back in sortkey order, so the rows a branch
// added or moved come out ordered by key and the rows of the base file come out in the
// order the base file had them, the added ones ahead of the base row they were placed
// before.
void acr_dm::PrintSubtree(acr_dm::FTuple &tuple, int &pos, cstring &out) {
    ind_beg(tuple_bh_child_curs,child,tuple) {
        cstring merged;
        if (MergeTuple(child,pos,merged)) {
            // no conflict
            // print tuple (may be empty when deleted)
            out << merged;
        } else {
            // conflict
            PrintMoveConflict(child,out);
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

// Write OUT where the caller asked for the result: over the ours file when git is driving
// the merge, and to stdout otherwise.
void acr_dm::PrintResult(algo::strptr out) {
    if (arg_N(_db.cmdline)>=3 && _db.cmdline.write_ours && FileQ(*arg_Find(_db.cmdline,1))) {
        StringToFile(out,*arg_Find(_db.cmdline,1));
    } else {
        prlog_(out);
    }
}

//------------------------------------------------------------------------------

// Merge
void acr_dm::Main_Merge() {
    cstring out;
    int pos=0;
    PrintSubtree(*_db.p_root,pos,out);
    // The tail of a file is text the root carries, and it comes after every row.
    FTuple *root = _db.p_root;
    ind_beg(tuple_zs_attr_curs,attr,*root) {
        bool print = false;
        strptr value;
        bool merged = MergeAttr(attr,print,value);
        if (print) {
            out << value << eol;
        }
        if (!merged) {
            algo_lib::_db.exit_code = 1;
        }
    }ind_end;
    PrintResult(out);
}

//------------------------------------------------------------------------------

// Print the contents of FNAME, and nothing at all when there is no such file.
void acr_dm::PrintFileMaybe(algo::strptr fname, cstring &out) {
    if (FileQ(fname)) {
        out << FileToString(fname);
    }
}

//------------------------------------------------------------------------------

// Leave a file that says the merge failed, and say why on stderr.
// A driver that exits without writing leaves git holding the ours file exactly as it was
// handed over, which during a rebase is upstream's own content: a complete, well-formed
// ssimfile carrying no marker, holding none of the rows the branch added, and offering a
// reader nothing to suggest that anything was dropped.  Staging that file by inspection
// loses the branch's work, and a resolver that reads "no markers" as "already merged"
// loses it without anyone looking.  So a failure writes both sides whole, between
// markers, which nothing can mistake for a merged result.
void acr_dm::Main_MergeFail(algo::strptr message) {
    cstring out;
    prerr("acr_dm.mergefail"<<Keyval("comment",message));
    if (arg_N(_db.cmdline)>=3) {
        PrintConflictMarker('<',*arg_Find(_db.cmdline,1),out);
        PrintFileMaybe(*arg_Find(_db.cmdline,1),out);
        PrintConflictMarker('=',"",out);
        for (int i=2; i<arg_N(_db.cmdline); ++i) {
            PrintFileMaybe(*arg_Find(_db.cmdline,i),out);
            PrintConflictMarker('>',*arg_Find(_db.cmdline,i),out);
        }
        PrintResult(out);
    }
    algo_lib::_db.exit_code = 1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Main routine
void acr_dm::Main() {
    // Every file's first line comes after nothing, and this row is that nothing.
    // Giving it a record means no row is a special case: each one has an anchor, and
    // the whole result is the rows that came after this one.  It is never printed.
    _db.p_root = &tuple_Alloc();
    try {
        Main_LoadFiles();
        Main_LinkTuple();
        Main_Dump();
        Main_Merge();
    } catch (algo_lib::ErrorX &x) {
        Main_MergeFail(x.str);
    }
}
