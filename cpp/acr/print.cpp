// (C) AlgoEngineering LLC 2008-2013
// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/print.cpp
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//
// Print records in normalized order.
// Absent of any dependencies between record keys, print records
// in the dependency order of their relations.
// Example: if ns:abc and ctype:xyz.def were selected, then the normal order is
// ns:abc
// ctype:xyz.def
// Because ctype uses ns
// In tree mode, group all items that share the primary key under
// the parent item, and indent them.
// All "uses" of a record (i.e. references to its key from non-pkey attribute)
// are printed un-indented.
// Implementation:
// The records are printed by putting them in a heap and then scanning the heap.
// First, we calculate the relationship between ctypes, computing a "rank" for each.
// If no other constraints exist, this will be the only key for the record.
// Next, we scan all selected records once and construct Plines (print-line-rec) out of them
// For each pline, we create a set of Pdeps: these are all the up-links from
// a given pline.
// An inverse index (of down-links) is built as well, so each line knows who the children are.
// Then we enter plines into a heap index. The key is (#up-links, relation rank)
// We take a record off the top of the heap, print it, then scan the down-links and reduce their
// up-link count by 1. This is like a topological sort.

#include "include/acr.h"

// -----------------------------------------------------------------------------

static int GetDepth(acr::PlineKey &key) {
    return -key.negdepth;
}

static void SetDepth(acr::PlineKey &key, int depth) {
    key.negdepth = -depth;
}

// -----------------------------------------------------------------------------

// Show comment for specified ctype (only one)
static void PrintCtypeComments1(acr::FPrint &, algo_lib::FTxttbl &txttbl, acr::FCtype &ctype) {
    if (!ctype.cmt_printed && ch_N(ctype.comment.value) > 0) {
        ctype.cmt_printed = true;
        AddRow(txttbl);
        AddCell(txttbl) << ctype.ctype;
        AddCell(txttbl) << ctype.comment;
    }
}

// Show comments for all ctypes
// touched by given print context
static void PrintCtypeComments(acr::FPrint &print) {
    algo_lib::FTxttbl txttbl;
    AddRow(txttbl);
    AddCol(txttbl, "Ctype");
    AddCol(txttbl, "Comment");
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        rec.p_ctype->cmt_printed = false;
    }ind_end;

    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        acr::FCtype &ctype = *rec.p_ctype;
        if (!ctype.cmt_printed) {
            PrintCtypeComments1(print,txttbl,ctype);
            // recurse one level down -- print comments for fields' args.
            ind_beg(acr::ctype_c_field_curs, field, ctype) {
                PrintCtypeComments1(print, txttbl, *field.p_arg);
            }ind_end;
        }
    }ind_end;
    if (c_txtrow_N(txttbl) > 1) {
        tempstr out;
        FTxttbl_Print(txttbl,out);
        ind_beg(Line_curs,lineiter,out) {
            print.out << "# "<<lineiter << eol;
        }ind_end;
        print.out << eol; // skip a line
    }
}

// -----------------------------------------------------------------------------

// Show comments for all fields touched by specified print
// context
static void PrintFieldComments(acr::FPrint &print) {
    algo_lib::FTxttbl txttbl;
    AddRow(txttbl);
    AddCol(txttbl, "Field");
    AddCol(txttbl, "Arg");
    AddCol(txttbl, "Reftype");
    AddCol(txttbl, "Comment");
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        rec.p_ctype->cmt_printed = false;
    }ind_end;
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        acr::FCtype &ctype = *rec.p_ctype;
        if (!ctype.cmt_printed) {
            ctype.cmt_printed = true;
            ind_beg(acr::ctype_c_field_curs, field, ctype) if (ch_N(field.comment.value) > 0) {
                AddRow(txttbl);
                AddCell(txttbl) << field.field;
                AddCell(txttbl) << field.arg;
                AddCell(txttbl) << field.reftype;
                AddCell(txttbl) << field.comment;
            }ind_end;
        }
    }ind_end;
    if (c_txtrow_N(txttbl) > 1) {
        tempstr out;
        FTxttbl_Print(txttbl,out);
        print.out << eol; // skip a line
        ind_beg(Line_curs,lineiter,out) {
            print.out << "# "<<lineiter<<eol;
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

static int GetAttrWidth(acr::FPrint &print, acr::FField &field) {
    int ret = 0;
    if (print.pretty) {
        algo::Smallstr50 name = name_Get(field);
        acr::FPrintAttr *printattr = acr::ind_printattr_Find(print, name);
        if (printattr) {
            ret = printattr->width;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Print all attributes of record REC using schema provided by REC.P_CTYPE
// Show include acr.rowid attribute if PRINT.ROWID is specified
// and records do not sorted by primary key
static void PrintFields(acr::FPrint &print, cstring &line, acr::FRec &rec)  {
    // print attrs according to schema
    ind_beg(acr::ctype_c_field_curs, field, *rec.p_ctype) {
        if (!field.isfldfunc || (field.c_substr && print.fldfunc)) {
            bool first_field = ind_curs(field).index == 0;
            if (first_field) {
                PrintAttr(line, rec.tuple.head.name, rec.tuple.head.value);
                line << "  ";
            }
            if (!first_field) {
                line << "  ";
            }
            u32 width = GetAttrWidth(print,field);
            int start_col = ch_N(line);
            tempstr attr;
            EvalAttrDflt(rec.tuple, field, attr);
            PrintAttr(line, name_Get(field), attr);
            char_PrintNTimes(' ', line,  width - (ch_N(line) - start_col));
            if (print.rowid && rec.p_ctype->show_rowid && ind_curs(field).index == 0) {
                int idx = ch_N(line);
                PrintAttrSpace(line, "acr.rowid", tempstr()<<rec.sortkey.rowid);
                char_PrintNTimes(' ', line,  18 - (ch_N(line) - idx));
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static acr::FPdep &CreatePdep(acr::FPline &parent, acr::FPline &child, int lindex) {
    acr::FPdep& pdep = acr::pdep_Alloc();
    pdep.p_parent         = &parent;
    pdep.p_child          = &child;
    pdep.weight = 1;
    pdep.lindex = lindex;
    child.key.alldep    += pdep.weight;
    (void)acr::pdep_XrefMaybe(pdep);
    return pdep;
}

// -----------------------------------------------------------------------------

// Release dependencies held by PLINE on its children.
// I.e. allow printing records that refer to PLINE.
//
static void ReleaseChildren(acr::FPline &pline) {
    ind_beg(acr::pline_zd_child_curs, pdep,pline) {
        if (bh_pline_InBheapQ(*pdep.p_child)) {
            pdep.p_child->key.alldep    -= pdep.weight;
            if (pdep.lindex==0) {
                SetDepth(pdep.p_child->key, GetDepth(pline.key)+1);
            }
            bh_pline_Reheap(*pdep.p_child);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Initialize RANK field for every known ctype
static void CreateCtypeRank() {
    // sort key is:
    // - # of unresolved dependencies
    // - rowid of ctype
    // So all things being equal, the ctypes are output alphabetically
    ind_beg(acr::_db_ctype_curs, ctype, acr::_db) {
        ctype.topokey.alldep = c_field_N(ctype);
        ctype.topokey.rowid = ctype.rowid;
        bh_ctype_topo_Reheap(ctype);
    }ind_end;

    // now read them off in topological order, starting with ctypes
    // that don't refer to anything
    int rank = 0;
    while (acr::FCtype *ctype = acr::bh_ctype_topo_RemoveFirst()) {
        ctype->rank = rank;
        rank++;
        ind_beg(acr::ctype_zd_arg_curs, field, *ctype) {
            field.p_ctype->topokey.alldep--;
            if (bh_ctype_topo_InBheapQ(*field.p_ctype)) {
                bh_ctype_topo_Reheap(*field.p_ctype);
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Create a heap of all currently selected records
static void CreateRecPline(acr::FPrint &print) {
    ind_beg(acr::_db_zd_all_selrec_curs, rec,acr::_db) {
        // skip deleted records if showstatus==false
        if (print.showstatus || !rec.del) {
            acr::FPline& pline   = acr::pline_Alloc();
            // global rank
            pline.key.ctype_rank = rec.p_ctype->rank;
            // sort by line id
            pline.key.rowid      = rec.sortkey.rowid;
            pline.p_rec          = &rec;
            (void)acr::pline_XrefMaybe(pline);
        }
    }ind_end;

    // compute record dependencies (for tree display only)
    // with -loose, enforce referential order by pkey only,
    // otherwise never print record until all of its dependent records
    // are printed
    // if there is no tree display, the topological sort of ctypes
    // ensures dependencies between records.
    if (print.tree) {
        ind_beg(acr::_db_zd_pline_curs, pline,acr::_db) {
            int lindex=0;// index of how far "left" the reference is ... aesthetic purposes
            ind_beg(acr::ctype_c_field_curs, field, *pline.p_rec->p_ctype) {
                acr::FRec *parent_rec = acr::ind_rec_Find(*field.p_arg, EvalAttr(pline.p_rec->tuple, field));
                if (parent_rec && zd_all_selrec_InLlistQ(*parent_rec)) {
                    CreatePdep(*parent_rec->c_pline, pline, lindex);
                    if (print.loose) break;
                }
                lindex += field.p_arg->c_ssimfile != NULL;
            }ind_end;
        }ind_end;
    }

    // compute heap.
    ind_beg(acr::_db_zd_pline_curs, pline,acr::_db) {
        acr::bh_pline_Reheap(pline);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Update attr width for pretty printing
static void UpdateAttrWidth(acr::FPline &pline, acr::FPrint &print) {
    acr::FRec& rec = *pline.p_rec;
    tempstr line;
    ind_beg(acr::ctype_c_field_curs, field, *rec.p_ctype) {
        if (!field.isfldfunc || (field.c_substr && print.fldfunc)) {
            ch_RemoveAll(line);
            algo::Smallstr50 name = name_Get(field);
            acr::FPrintAttr *printattr = acr::ind_printattr_Find(print, name);
            if (!printattr) {
                printattr = &acr::printattr_Alloc();
                printattr->p_print = &print;
                printattr->field = name;
                (void)printattr_XrefMaybe(*printattr);
            }
            PrintAttr(line, name, EvalAttr(rec.tuple, field));
            printattr->width = u32_Max(printattr->width, ch_N(line));
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Print all lines held in c_pline and prepare for new line group.
static void EndGroup(acr::FPrint &print) {
    ind_beg(acr::print_c_pline_curs, pline, print) {
        //prerr(pline.key<<" "<<pline.p_rec->tuple);
        u32 start = print.out.ch_n;
        acr::FRec& rec = *pline.p_rec;
        for(; print.skip_line>0; print.skip_line--) {
            print.out << eol;
        }
        int indent = GetDepth(rec.c_pline->key) * 2;
        // indent
        char_PrintNTimes(' ', print.out,  indent);
        if (print.showstatus) {
            if (rec.del) {
                print.out << "acr.delete  ";
            } else if (rec.isnew) {
                print.out << "acr.insert  ";
            } else if (rec.mod) {
                print.out << "acr.update  ";
            }
        }
        PrintFields(print,print.out,rec);
        // strip whitespace from the end of the line (but don't merge lines!)
        while (print.out.ch_n > start && ch_qLast(print.out) == ' ') {
            print.out.ch_n--;
        }
        print.out << eol;
    }ind_end;
    if (print.tree) {
        // tree mode -- add space when de-denting.
        print.skip_line = print.this_depth < print.prev_depth;
    } else {
        // block mode -- add space between groups of same type
        print.skip_line = c_pline_N(print)>0;
    }
    acr::c_pline_RemoveAll(print);
    acr::ind_printattr_Cascdel(print);
}

// -----------------------------------------------------------------------------

static bool DetectNewGroup(acr::FPrint &print, acr::FPline *pline) {
    acr::FPline *prevrec = c_pline_N(print) > 0 ? c_pline_Find(print, c_pline_N(print)-1) : NULL;
    print.this_depth = 0;
    print.prev_depth = 0;
    bool end_group = pline == NULL || c_pline_N(print) > print.maxgroup;
    // detect group boundaries.
    // group boundaries are used for alignment.
    // tree depth (key.depth) is negated, so we negate it back to check depth
    // if depth is the same, but ctype_rank changes (-pretty mode), this is also a group change.
    // it means we've switched to printing records of a different type at the same tree depth.
    // if the new record is indented with respect to the previous record, then it doesn't
    // matter if ctype_rank is different since there is already a visual indication of change.
    // if the depth lessens, it is a different group only if the pkey is different
    if (prevrec && pline) {
        print.this_depth = GetDepth(pline->key);
        print.prev_depth = GetDepth(prevrec->key);
        end_group = end_group
            || (print.this_depth < print.prev_depth && pline->p_rec->pkey != prevrec->p_rec->pkey)
            || (print.this_depth == print.prev_depth && pline->key.ctype_rank != prevrec->key.ctype_rank);
    }
    return end_group;
}

// -----------------------------------------------------------------------------

// Print selected records using formatting options
// specified on command line.
void acr::Print(acr::FPrint &print) {
    // Print a comment for each ctype
    if (print.cmt) {
        PrintCtypeComments(print);
    }

    if (bool_Update(acr::_db.ctype_rank_computed,true)) {
        CreateCtypeRank();
    }

    // create pline for each selected record
    CreateRecPline(print);

    // print in groups.
    // align with each group
    acr::FPline *pline = NULL;
    int iter = 0;

    do {
        pline = acr::bh_pline_RemoveFirst();
        bool end_group = DetectNewGroup(print,pline);
        if (end_group) {
            EndGroup(print);
        }
        // update alignment -- compute width of all attributes in the tuple
        if (!end_group && print.pretty) {
            UpdateAttrWidth(*pline,print);
        }
        // add pline to c_pline list, release its children
        if (pline) {
            ReleaseChildren(*pline);
            c_pline_Insert(print, *pline);
        }
        if (iter++ % 32 == 0) {
            FlushStdout(print);
        }
    } while (pline);

    // Scan all selected records. Print something about each field.
    if (print.cmt) {
        PrintFieldComments(print);
    }
    FlushStdout(print);

    // clean up.
    acr::zd_pdep_Cascdel();
    acr::zd_pline_Cascdel();
}

// -----------------------------------------------------------------------------

void acr::FlushStdout(acr::FPrint &print) {
    if (print.stdout) {
        algo::Prlog(1/*stdout*/, print.out, 0/*reset to char*/, false/*eol*/);
    }
}

// -----------------------------------------------------------------------------

// Print selected records to fd
void acr::PrintToFd(acr::FPrint &print, algo::Fildes fd) {
    int old_out = dup(1);
    dup2(fd.value,1);
    lseek(fd.value,0,SEEK_SET);
    int rc = ftruncate(fd.value,0);
    (void)rc;
    fsync(fd.value);
    acr::Print(print);
    if (old_out >= 0) {// mostly for coverity. dup shouldn't fail
        dup2(old_out,1);
        close(old_out);
    }
}
