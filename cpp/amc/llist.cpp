// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/llist.cpp -- Linked lists
//

#include "include/amc.h"

void amc::tclass_Llist() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;
    amc::FListtype &listtype = *llist->p_listtype;

    if (!(listtype.circular || !listtype.instail || llist->havetail)) {
        prerr("amc.notail"
              <<Keyval("field",field.field)
              <<Keyval("comment","instail on zero-terminated list  implies havetail"));
        algo_lib::_db.exit_code++;
    }
    if (!(!listtype.circular  || (llist->havetail ^ listtype.haveprev))) {
        prerr("amc.badcirc"
              <<Keyval("field",field.field)
              <<Keyval("comment","circular list implies either haveprev or havetail but not both"));
        algo_lib::_db.exit_code++;
    }

    // helper fields
    Set(R, "$desc", listtype.haveprev ? "zero-terminated doubly linked list" : "zero-terminated singly linked list");

    // if (field.p_arg->c_varlenfld && field.p_arg->c_varlenfld->processed) {
    //     prerr("amc.gen_llist"
    //           <<Keyval("field",field.field)
    //           <<Keyval("comment","no field cannot appear after varlen field"));
    //     algo_lib::_db.exit_code++;
    // }

    InsVar(R, field.p_ctype           , "$Cpptype*", "$name_head", "", "$desc");
    if (llist->havecount) {
        InsVar(R, field.p_ctype       , "i32", "$name_n", "", "$desc");
    }
    if (llist->havetail) {
        InsVar(R, field.p_ctype       , "$Cpptype*", "$name_tail", "", "pointer to last element");
    }
    InsVar(R, field.p_arg             , "$Cpptype*", "$name_next", "", "zslist link; -1 means not-in-list");
    if (listtype.haveprev) {
        InsVar(R, field.p_arg         , "$Cpptype*", "$name_prev", "", "previous element");
    }
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname"     , Refname(*field.p_arg));
    Ins(&R, child_init->body    , "$fname.$name_next = ($Cpptype*)-1; // ($field) not-in-list");
    if (listtype.haveprev) {
        Ins(&R, child_init->body, "$fname.$name_prev = NULL; // ($field)");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Llist_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // cascading delete
    if (field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();
        Ins(&R, cascdel.body, "while ($Cpptype *$name_first = $name_First($pararg)) {");
        Ins(&R, cascdel.body, DeleteExpr(field,"$pararg","*$name_first")<<";");
        Ins(&R, cascdel.body, "}");
    }
}

void amc::tfunc_Llist_DestructiveFirst() {
}

void amc::tfunc_Llist_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_head == NULL;");
}

void amc::tfunc_Llist_First() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& first = amc::CreateCurFunc();
    Ins(&R, first.ret  , "$Cpptype*", false);
    Ins(&R, first.proto, "$name_First($Parent)", false);
    Ins(&R, first.body, "$Cpptype *row = NULL;");
    Ins(&R, first.body, "row = $parname.$name_head;");
    Ins(&R, first.body, "return row;");
}

void amc::tfunc_Llist_InLlistQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& inlist = amc::CreateCurFunc();
    Ins(&R, inlist.ret  , "bool", false);
    Ins(&R, inlist.proto, "$name_InLlistQ($Cpptype& row)", false);
    Ins(&R, inlist.body, "bool result = false;");
    Ins(&R, inlist.body, "result = !(row.$name_next == ($Cpptype*)-1);");
    Ins(&R, inlist.body, "return result;");
}

void amc::tfunc_Llist_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    amc::FFunc& init = amc::CreateCurFunc();
    Ins(&R, init.body                 , "$parname.$name_head = NULL; // ($field)");
    if (llist->havecount) {
        Ins(&R, init.body             , "$parname.$name_n = 0; // ($field)");
    }
    if (llist->havetail) {
        Ins(&R, init.body             , "$parname.$name_tail = NULL; // ($field)");
    }
}

void amc::tfunc_Llist_Last() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    if (llist->havetail) {
        amc::FFunc& last = amc::CreateCurFunc();
        Ins(&R, last.ret  , "$Cpptype*", false);
        Ins(&R, last.proto, "$name_Last($Parent)", false);
        Ins(&R, last.body, "$Cpptype *row = NULL;");
        Ins(&R, last.body, "row = $parname.$name_tail;");
        Ins(&R, last.body, "return row;");
    }
    if (llist->p_listtype->circular && !llist->havetail && llist->p_listtype->haveprev) {
        amc::FFunc& last = amc::CreateCurFunc();
        Ins(&R, last.ret  , "$Cpptype*", false);
        Ins(&R, last.proto, "$name_Last($Parent)", false);
        Ins(&R, last.body, "$Cpptype *row = NULL;");
        Ins(&R, last.body, "row = $parname.$name_head ? $parname.$name_head->$name_prev : NULL;");
        Ins(&R, last.body, "return row;");
    }
}

void amc::tfunc_Llist_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    if (llist->havecount) {
        amc::FFunc& nitems = amc::CreateCurFunc();
        Ins(&R, nitems.ret  , "i32", false);
        Ins(&R, nitems.proto, "$name_N($Cparent)", false);
        Ins(&R, nitems.body, "return $parname.$name_n;");
    }
}

void amc::tfunc_Llist_Next() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& next = amc::CreateCurFunc();
    Ins(&R, next.ret  , "$Cpptype*", false);
    Ins(&R, next.proto, "$name_Next($Cpptype &row)", false);
    Ins(&R, next.body, "return row.$name_next;");
}

void amc::tfunc_Llist_Prev() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    if (llist->p_listtype->haveprev) {
        amc::FFunc& prev = amc::CreateCurFunc();
        Ins(&R, prev.ret  , "$Cpptype*", false);
        Ins(&R, prev.proto, "$name_Prev($Cpptype &row)", false);
        Ins(&R, prev.body, "return row.$name_prev;");
    }
}

void amc::tfunc_Llist_Remove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    amc::FFunc& remove = amc::CreateCurFunc();
    Ins(&R,             remove.ret  , "void", false);
    Ins(&R,             remove.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R,             remove.body, "if ($name_InLlistQ(row)) {");
    Ins(&R,             remove.body, "    $Cpptype* old_head       = $parname.$name_head;");
    Ins(&R,             remove.body, "    (void)old_head; // in case it's not used");

    if (!llist->p_listtype->circular) {
        // this logic is somewhat convoluted. the llist->p_listtype->circular section seems much cleaner.
        // are there really that many cases involved???
        if (llist->p_listtype->haveprev) {
            Ins(&R,         remove.body, "    $Cpptype* prev = row.$name_prev;");
            Ins(&R,         remove.body, "    $Cpptype* next = row.$name_next;");
            Ins(&R,         remove.body, "    // if element is first, adjust list head; otherwise, adjust previous element's next");
            Ins(&R,         remove.body, "    $Cpptype **new_next_a = &prev->$name_next;");
            Ins(&R,         remove.body, "    $Cpptype **new_next_b = &$parname.$name_head;");
            Ins(&R,         remove.body, "    $Cpptype **new_next = prev ? new_next_a : new_next_b;");
            Ins(&R,         remove.body, "    *new_next = next;");
            if (llist->havetail) {
                Ins(&R,     remove.body, "    // if element is last, adjust list tail; otherwise, adjust next element's prev");
                Ins(&R,     remove.body, "    $Cpptype **new_prev_a = &next->$name_prev;");
                Ins(&R,     remove.body, "    $Cpptype **new_prev_b = &$parname.$name_tail;");
                Ins(&R,     remove.body, "    $Cpptype **new_prev = next ? new_prev_a : new_prev_b;");
                Ins(&R,     remove.body, "    *new_prev = prev;");
            } else {
                Ins(&R,     remove.body, "    // if element is not last, adjust next element's prev");
                Ins(&R,     remove.body, "    if (next != NULL) {");
                Ins(&R,     remove.body, "        next->$name_prev = prev;");
                Ins(&R,     remove.body, "    }");
            }
            if (llist->havecount) {
                Ins(&R,     remove.body, "    $parname.$name_n--;");
            }
            Ins(&R,         remove.body, "    row.$name_next=($Cpptype*)-1; // not-in-list");
        } else {
            Ins(&R,         remove.comment, "Since the list is singly-linked, use linear search to locate the element.");
            Ins(&R,         remove.body, "    $Cpptype* prev=NULL;");
            Ins(&R,         remove.body, "    $Cpptype* cur     = $parname.$name_head;");
            Ins(&R,         remove.body, "    while (cur) {  // search for element by pointer");
            Ins(&R,         remove.body, "        $Cpptype* next = cur->$name_next;");
            Ins(&R,         remove.body, "        if (cur == &row) {");
            if (llist->havecount) {
                Ins(&R,     remove.body, "            $parname.$name_n--;  // adjust count");
            }
            if (llist->havetail) {
                Ins(&R,     remove.body, "            ");
                Ins(&R,     remove.body, "            if (!next) {");
                Ins(&R,     remove.body, "                $parname.$name_tail = prev;  // adjust tail pointer");
                Ins(&R,     remove.body, "            }");
            }
            Ins(&R,         remove.body, "            // disconnect element from linked list");
            Ins(&R,         remove.body, "            if (prev) {");
            Ins(&R,         remove.body, "                prev->$name_next = next;");
            Ins(&R,         remove.body, "            } else {");
            Ins(&R,         remove.body, "                $parname.$name_head = next;");
            Ins(&R,         remove.body, "            }");
            Ins(&R,         remove.body, "            row.$name_next = ($Cpptype*)-1; // not-in-list");
            Ins(&R,         remove.body, "            break;");
            Ins(&R,         remove.body, "        }");
            Ins(&R,         remove.body, "        prev = cur;");
            Ins(&R,         remove.body, "        cur  = next;");
            Ins(&R,         remove.body, "    }");
        }
    } else { // llist->p_listtype->circular
        Ins(&R,             remove.body, "    $Cpptype *oldnext = row.$name_next;");
        if (llist->p_listtype->haveprev) {
            Ins(&R,         remove.body, "    $Cpptype *oldprev = row.$name_prev;");
        } else {
            Ins(&R,         remove.comment, "Since the list is singly-linked, use linear search to locate the element.");
            Ins(&R,         remove.body, "    // find previous element with linear search -- always scans the entire list");
            Ins(&R,         remove.body, "    $Cpptype *oldprev = row.$name_next;");
            Ins(&R,         remove.body, "    while (oldprev->$name_next != &row) {");
            Ins(&R,         remove.body, "        oldprev = oldprev->$name_next;");
            Ins(&R,         remove.body, "    }");
        }
        if (llist->p_listtype->haveprev) {
            Ins(&R,         remove.body, "    oldnext->$name_prev = oldprev; // remove element from list");
        }
        Ins(&R,             remove.body, "    oldprev->$name_next = oldnext;");
        if (llist->havecount) {
            Ins(&R,         remove.body, "    $parname.$name_n--;  // adjust count");
        }
        Ins(&R,             remove.body, "    if (&row == $parname.$name_head) {");
        Ins(&R,             remove.body, "        $parname.$name_head = oldnext==&row ? NULL : oldnext; // adjust list head");
        Ins(&R,             remove.body, "    }");
        if (llist->havetail) {
            Ins(&R,         remove.body, "    if (&row == $parname.$name_tail) {");
            Ins(&R,         remove.body, "        $parname.$name_tail = oldprev==&row ? NULL : oldprev; // adjust list tail");
            Ins(&R,         remove.body, "    }");
        }
        Ins(&R,             remove.body, "    row.$name_next = ($Cpptype*)-1; // mark element as not-in-list);");
        if (llist->p_listtype->haveprev) {
            Ins(&R,         remove.body, "    row.$name_prev = NULL; // clear back-pointer");
        }
    }
    if (field.need_firstchanged) {
        Ins(&R, remove.body, "    if (old_head != $parname.$name_head) {");
        Ins(&R, remove.body, "        $name_FirstChanged($pararg);");
        Ins(&R, remove.body, "    }");
    }
    Ins(&R,             remove.body, "}");
}

void amc::tfunc_Llist_RemoveAll() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    amc::FFunc& flush = amc::CreateCurFunc();
    Ins(&R, flush.ret  , "void", false);
    Ins(&R, flush.proto, "$name_RemoveAll($Parent)", false);
    Ins(&R, flush.body, "$Cpptype* row = $parname.$name_head;");
    if (llist->p_listtype->circular) {
        Ins(&R, flush.body, "$Cpptype* head = $parname.$name_head;");
    }
    Ins(&R, flush.body, "$parname.$name_head = NULL;");
    if (llist->havetail) {
        Ins(&R, flush.body, "$parname.$name_tail = NULL;");
    }
    if (llist->havecount) {
        Ins(&R, flush.body, "$parname.$name_n = 0;");
    }
    if (field.need_firstchanged) {
        Ins(&R, flush.body, "bool do_fire = (NULL != row);");
    }
    Ins(&R, flush.body, "while (row) {");
    Ins(&R, flush.body, "    $Cpptype* row_next = row->$name_next;");
    Ins(&R, flush.body, "    row->$name_next  = ($Cpptype*)-1;");
    if (llist->p_listtype->haveprev) {
        Ins(&R, flush.body, "    row->$name_prev  = NULL;");
    }
    if (!llist->p_listtype->circular) {
        Ins(&R, flush.body, "    row = row_next;");
    } else {
        Ins(&R, flush.body, "    row = row_next != head  ? row_next : NULL;");
    }
    Ins(&R, flush.body, "}");
    if (field.need_firstchanged) {
        Ins(&R, flush.body, "if (do_fire) {");
        Ins(&R, flush.body, "    $name_FirstChanged($pararg);");
        Ins(&R, flush.body, "}");
    }
}

void amc::tfunc_Llist_RemoveFirst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    amc::FFunc& remfirst = amc::CreateCurFunc();
    Ins(&R,         remfirst.ret  , "$Cpptype*", false);
    Ins(&R,         remfirst.proto, "$name_RemoveFirst($Parent)", false);
    Ins(&R,         remfirst.body, "$Cpptype *row = NULL;");
    Ins(&R,         remfirst.body, "row = $parname.$name_head;");
    Ins(&R,         remfirst.body, "if (row) {");
    if (!llist->p_listtype->circular) {
        Ins(&R,         remfirst.body, "    $Cpptype *next = row->$name_next;");
        Ins(&R,         remfirst.body, "    $parname.$name_head = next;");
        if (llist->havetail) {
            if (llist->p_listtype->haveprev) {
                Ins(&R, remfirst.body, "    $Cpptype **new_end_a = &next->$name_prev;");
                Ins(&R, remfirst.body, "    $Cpptype **new_end_b = &$parname.$name_tail;");
                Ins(&R, remfirst.body, "    $Cpptype **new_end = next ? new_end_a : new_end_b;");
                Ins(&R, remfirst.body, "    *new_end = NULL;");
            } else {
                Ins(&R, remfirst.body, "    // clear list's tail pointer if list is empty.");
                Ins(&R, remfirst.body, "    if (!next) {");
                Ins(&R, remfirst.body, "        $parname.$name_tail = NULL;");
                Ins(&R, remfirst.body, "    }");
            }
        }
    } else {
        Ins(&R, remfirst.body,         "    bool hasmore = row!=row->$name_next;");
        Ins(&R, remfirst.body,         "    $parname.$name_head = hasmore ? row->$name_next : NULL;");
        if (llist->p_listtype->haveprev) {
            Ins(&R, remfirst.body,     "    row->$name_next->$name_prev = row->$name_prev;");
            Ins(&R, remfirst.body,     "    row->$name_prev->$name_next = row->$name_next;");
            Ins(&R, remfirst.body,     "    row->$name_prev = NULL;");
        } else if (llist->havetail) {
            Ins(&R, remfirst.body,     "    $parname.$name_tail->$name_next = row->$name_next;");
            Ins(&R, remfirst.body,     "    // clear list's tail pointer if list is empty.");
            Ins(&R, remfirst.body,     "    if (!hasmore) {");
            Ins(&R, remfirst.body,     "        $parname.$name_tail = NULL;");
            Ins(&R, remfirst.body,     "    }");
        } else {
            prerr("amc.bad_llist");
            algo_lib::_db.exit_code++;
        }
    }
    if (llist->havecount) {
        Ins(&R,     remfirst.body, "    $parname.$name_n--;");
    }
    Ins(&R,         remfirst.body, "    row->$name_next = ($Cpptype*)-1; // mark as not-in-list");
    if (field.need_firstchanged) {
        Ins(&R,     remfirst.comment, "Call FirstChanged trigger.");
        Ins(&R,     remfirst.body, "    $name_FirstChanged($pararg);");
    }
    Ins(&R,         remfirst.body, "}");
    Ins(&R,         remfirst.body, "return row;");
}

void amc::tfunc_Llist_RotateFirst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;

    if (llist->p_listtype->circular) {
        amc::FFunc& rotfirst = amc::CreateCurFunc();
        Ins(&R,         rotfirst.ret  , "$Cpptype*", false);
        Ins(&R,         rotfirst.proto, "$name_RotateFirst($Parent)", false);
        Ins(&R,         rotfirst.body, "$Cpptype *row = NULL;");
        Ins(&R,         rotfirst.body, "row = $parname.$name_head;");
        Ins(&R,         rotfirst.body, "if (row) {");
        if (llist->havetail) {
            Ins(&R,     rotfirst.body, "    $parname.$name_tail = $parname.$name_head;");
        }
        Ins(&R,         rotfirst.body, "    $parname.$name_head = row->$name_next;");
        Ins(&R,         rotfirst.body, "}");
        Ins(&R,         rotfirst.body, "return row;");
    }
}

void amc::tfunc_Llist_Insert() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;
    amc::FListtype &listtype = *llist->p_listtype;

    // private function to update the linked list.
    amc::FFunc& ins = amc::CreateCurFunc();
    Ins(&R,             ins.proto, "$name_Insert($Parent, $Cpptype& row)", false);
    Ins(&R,             ins.body, "if (!$name_InLlistQ(row)) {");
    Ins(&R,             ins.ret  , "void", false);
    if (!listtype.circular) {
        // doubly linked list case
        if (listtype.haveprev) {
            if (listtype.instail) {// doubly linked, tail insertion
                Ins(&R,     ins.body, "    $Cpptype* old_tail = $parname.$name_tail;");
                Ins(&R,     ins.body, "    row.$name_next = NULL;");
                Ins(&R,     ins.body, "    row.$name_prev = old_tail;");
                Ins(&R,     ins.body, "    $parname.$name_tail = &row;");
                Ins(&R,     ins.body, "    $Cpptype **new_row_a = &old_tail->$name_next;");
                Ins(&R,     ins.body, "    $Cpptype **new_row_b = &$parname.$name_head;");
                Ins(&R,     ins.body, "    $Cpptype **new_row = old_tail ? new_row_a : new_row_b;");
                Ins(&R,     ins.body, "    *new_row = &row;");
            } else { // doubly linked, head insertion
                Ins(&R,     ins.body, "    $Cpptype* old_head = $parname.$name_head;");
                Ins(&R,     ins.body, "    row.$name_prev = NULL;");
                Ins(&R,     ins.body, "    row.$name_next = old_head;");
                Ins(&R,     ins.body, "    $parname.$name_head  = &row;");
                if (llist->havetail) {
                    Ins(&R, ins.body, "    $Cpptype **new_row_a = &old_head->$name_prev;");
                    Ins(&R, ins.body, "    $Cpptype **new_row_b = &$parname.$name_tail;");
                    Ins(&R, ins.body, "    $Cpptype **new_row = old_head ? new_row_a : new_row_b;");
                    Ins(&R, ins.body, "    *new_row = &row;");
                } else {
                    Ins(&R, ins.body, "    if (old_head) {");
                    Ins(&R, ins.body, "        old_head->$name_prev = &row;");
                    Ins(&R, ins.body, "    }");
                }
            }
        } else {
            if (listtype.instail) { // singly linked, tail insertion
                Ins(&R,     ins.body, "    $Cpptype* old_tail       = $parname.$name_tail;");
                Ins(&R,     ins.body, "    row.$name_next  = NULL;");
                Ins(&R,     ins.body, "    $parname.$name_tail = &row;");
                Ins(&R,     ins.body, "    $Cpptype **new_row_a = &old_tail->$name_next;");
                Ins(&R,     ins.body, "    $Cpptype **new_row_b = &$parname.$name_head;");
                Ins(&R,     ins.body, "    $Cpptype **new_row = old_tail ? new_row_a : new_row_b;");
                Ins(&R,     ins.body, "    *new_row = &row;");
            } else { // singly linked, head insertion
                Ins(&R,     ins.body, "    $Cpptype* old_head = $parname.$name_head;");
                Ins(&R,     ins.body, "    row.$name_next  = old_head;");
                Ins(&R,     ins.body, "    $parname.$name_head = &row;");
                if (llist->havetail) {
                    Ins(&R, ins.body, "    if (!old_head) {");
                    Ins(&R, ins.body, "        $parname.$name_tail = &row;");
                    Ins(&R, ins.body, "    }");
                }
            }
        }
    } else { // llist->p_listtype->circular
        Ins(&R,             ins.body, "    if ($parname.$name_head) {");
        Ins(&R,             ins.body, "        row.$name_next = $parname.$name_head;");
        if (listtype.haveprev) {
            Ins(&R,         ins.body, "        row.$name_prev = $parname.$name_head->$name_prev;");
            Ins(&R,         ins.body, "        row.$name_prev->$name_next = &row;");
            Ins(&R,         ins.body, "        row.$name_next->$name_prev = &row;");
        } else if (llist->havetail) {
            Ins(&R,         ins.body, "        $parname.$name_tail->$name_next = &row;");
            if (listtype.instail) {
                Ins(&R,     ins.body, "        $parname.$name_tail = &row;");
            }
        } else {
            prerr("amc.bad_llist");
            algo_lib::_db.exit_code++;
        }
        if (!listtype.instail) {
            Ins(&R,         ins.body, "        $parname.$name_head = &row;");
        }
        Ins(&R,             ins.body, "    } else {");
        Ins(&R,             ins.body, "        row.$name_next = &row;");
        if (listtype.haveprev) {
            Ins(&R,         ins.body, "        row.$name_prev = &row;");
        }
        Ins(&R,             ins.body, "        $parname.$name_head = &row;");
        if (llist->havetail) {
            Ins(&R,         ins.body, "        $parname.$name_tail = &row;");
        }
        Ins(&R,             ins.body, "    }");
    }
    if (llist->havecount) {
        Ins(&R,         ins.body, "    $parname.$name_n++;");
    }
    // detect change in the head element
    if (field.need_firstchanged) {
        Ins(&R, ins.body, "    if ($parname.$name_head == &row) {");
        Ins(&R, ins.body, "        $name_FirstChanged($pararg);");
        Ins(&R, ins.body, "    }");
    }
    Ins(&R, ins.body, "}");
}

void amc::tfunc_Llist_qLast() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FLlist *llist = field.c_llist;
    amc::FListtype &listtype = *llist->p_listtype;

    if (llist->havetail) {
        amc::FFunc& qlast = amc::CreateCurFunc();
        qlast.inl = true;
        Ins(&R, qlast.ret  , "$Cpptype&", false);
        Ins(&R, qlast.proto, "$name_qLast($Parent)", false);
        Ins(&R, qlast.body, "$Cpptype *row = NULL;");
        Ins(&R, qlast.body, "row = $parname.$name_tail;");
        Ins(&R, qlast.body, "return *row;");
    }

    if (listtype.circular && !llist->havetail && listtype.haveprev) {
        amc::FFunc& qlast = amc::CreateCurFunc();
        qlast.inl = true;
        Ins(&R, qlast.ret  , "$Cpptype&", false);
        Ins(&R, qlast.proto, "$name_qLast($Parent)", false);
        Ins(&R, qlast.body, "$Cpptype *row = NULL;");
        Ins(&R, qlast.body, "row = $parname.$name_head ? $parname.$name_head->$name_prev : NULL;");
        Ins(&R, qlast.body, "return *row;");
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_ZSListMT() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);

    // helper fields
    Set(R, "$fname"     , Refname(*field.p_arg));
    InsStruct(R, field.p_ctype, "");
    InsVar(R, field.p_ctype, "$Cpptype*", "$name_head", "", "ZSListMT - zero-terminated singly linked list -- read side");
    InsVar(R, field.p_ctype, "$Cpptype*", "$name_mt", "", "elements inserted here from another thread -- write side");
    InsVar(R, field.p_arg, "$Cpptype*", "$name_next", "", "zslist link; -1 means not-in-list");
    Ins(&R, child_init->body  , "");
    Ins(&R, child_init->body  , "// ($field) not-in-list");
    Ins(&R, child_init->body  , "$fname.$name_next = ($Cpptype*)-1;");
}

void amc::tfunc_ZSListMT_DestructiveFirst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& first = amc::CreateCurFunc();
    Ins(&R,     first.comment, "If index is empty, return NULL");
    Ins(&R,     first.comment, "Otherwise return first key in index.");
    Ins(&R,     first.comment, "NOTE: this function must be called from the reading thread.");
    Ins(&R,     first.comment, "NOTE: if called from the writing thread, it might result in loss of data.");
    Ins(&R,     first.ret  , "$Cpptype*", false);
    Ins(&R,     first.proto, "$name_DestructiveFirst($Parent)", false);
    Ins(&R,     first.body, "$Cpptype *row = NULL;");
    Ins(&R,     first.body, "row = $parname.$name_head;");
    Ins(&R,     first.body, "if (row == NULL && $parname.$name_mt != NULL) {");
    Ins(&R,     first.body, "    // grab elements from mt-safe portion of the linked list and reverse them.");
    Ins(&R,     first.body, "    // due to the above if statement being not quite safe, must check if the fetched");
    Ins(&R,     first.body, "    // value is NULL or not.");
    Ins(&R,     first.body, "    $Cpptype* temp = __sync_lock_test_and_set(&$parname.$name_mt, NULL);");
    Ins(&R,     first.body, "    if (temp) {");
    Ins(&R,     first.body, "        do {");
    Ins(&R,     first.body, "            $Cpptype* next     = temp->$name_next;");
    Ins(&R,     first.body, "            temp->$name_next = row;");
    Ins(&R,     first.body, "            row              = temp;");
    Ins(&R,     first.body, "            temp             = next;");
    Ins(&R,     first.body, "        } while (temp);");
    Ins(&R,     first.body, "    }");
    Ins(&R,     first.body, "    // update pointer to first element.");
    Ins(&R,     first.body, "    $parname.$name_head = row;");
    Ins(&R,     first.body, "}");
    Ins(&R,     first.body, "return row;");
}

void amc::tfunc_ZSListMT_RemoveFirst() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& remfirst = amc::CreateCurFunc();
    Ins(&R,     remfirst.ret  , "$Cpptype*", false);
    Ins(&R,     remfirst.proto, "$name_RemoveFirst($Parent)", false);
    Ins(&R,     remfirst.body, "$Cpptype *row = $name_DestructiveFirst($pararg);");
    Ins(&R,     remfirst.body, "if (row) {");
    Ins(&R,     remfirst.body, "    $Cpptype *next = row->$name_next;");
    Ins(&R,     remfirst.body, "    $parname.$name_head = next;");
    Ins(&R,     remfirst.body, "    row->$name_next = ($Cpptype*)-1; // mark as not-in-list");
    Ins(&R,     remfirst.body, "}");
    Ins(&R,     remfirst.body, "return row;");
}

void amc::tfunc_ZSListMT_InLlistQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& inlist = amc::CreateCurFunc();
    inlist.inl=true;
    Ins(&R,     inlist.ret  , "bool", false);
    Ins(&R,     inlist.proto, "$name_InLlistQ($Cpptype& row)", false);
    Ins(&R,     inlist.body, "bool result = false;");
    Ins(&R,     inlist.body, "result = !(row.$name_next == ($Cpptype*)-1);");
    Ins(&R,     inlist.body, "return result;");
}

void amc::tfunc_ZSListMT_Insert() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& insert = amc::CreateCurFunc();
    Ins(&R,     insert.ret  , "void", false);
    Ins(&R,     insert.proto, "$name_Insert($Parent, $Cpptype& row)", false);
    Ins(&R,     insert.body, "if (!$name_InLlistQ(row)) {");
    Ins(&R,     insert.body, "    // insertion into thread-safe linked list.");
    Ins(&R,     insert.body, "    $Cpptype *temp = NULL;");
    Ins(&R,     insert.body, "    $Cpptype *oldval = $parname.$name_mt;");
    Ins(&R,     insert.body, "    do {");
    Ins(&R,     insert.body, "        temp = oldval;");
    Ins(&R,     insert.body, "        row.$name_next = temp;");
    Ins(&R,     insert.body, "        oldval = __sync_val_compare_and_swap(&$parname.$name_mt, temp, &row);");
    Ins(&R,     insert.body, "    } while (oldval != temp);");
    Ins(&R,     insert.body, "}");
}

void amc::tfunc_ZSListMT_Remove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& remove = amc::CreateCurFunc();
    Ins(&R,     remove.comment, "If element is not in index, do nothing.");
    Ins(&R,     remove.comment, "Otherwise, throw exception: ZSListMT elements cannot");
    Ins(&R,     remove.comment, "be removed from the list except via RemoveFIrst.");
    Ins(&R,     remove.ret  , "void",false);
    Ins(&R,     remove.proto, "$name_Remove($Parent, $Cpptype& row)",false);
    Ins(&R,     remove.body, "// cannot remove from ZSListMT");
    Ins(&R,     remove.body, "// should the process die here?");
    MaybeUnused(remove, Subst(R,"$parname"));
    MaybeUnused(remove, "row");
}

void amc::tfunc_ZSListMT_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    Ins(&R, init.body     , "$parname.$name_head = NULL; // ($field)");
    Ins(&R, init.body     , "$parname.$name_mt = NULL; // ($field)");
}

// Generate cursor for llist
void amc::Llist_curs(bool needdel) {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    amc::FLlist *llist = field.c_llist;
    amc::FListtype &listtype = *llist->p_listtype;
    bool circular  = listtype.circular;

    Ins(&R, ns.curstext    , "");
    Ins(&R, ns.curstext    , "struct $Parname_$name_$curstype {// fcurs:$fcurs");
    Ins(&R, ns.curstext    , "    typedef $Cpptype ChildType;");
    Ins(&R, ns.curstext    , "    $Cpptype* row;");
    if (circular) {
        Ins(&R, ns.curstext, "    $Cpptype** head; // address of head element");
    }
    if (needdel) {
        Ins(&R, ns.curstext, "    $Cpptype *next;");
    }
    Ins(&R, ns.curstext    , "    $Parname_$name_$curstype() {");
    Ins(&R, ns.curstext    , "        row = NULL;");
    if (circular) {
        Ins(&R, ns.curstext, "        head = NULL;");
    }
    if (needdel) {
        Ins(&R, ns.curstext, "        next = NULL;");
    }
    Ins(&R, ns.curstext    , "    }");
    Ins(&R, ns.curstext    , "};");
    Ins(&R, ns.curstext    , "");

    {
        amc::FFunc& func = amc::ind_func_GetOrCreate(Subst(R,"$field_$curstype.Reset"));
        func.inl = true;
        Ins(&R, func.comment, "cursor points to valid item");
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_Reset($Parname_$name_$curstype &curs, $Partype &parent)", false);
        Ins(&R, func.body, "curs.row = parent.$name_head;");
        if (circular) {
            Ins(&R, func.body, "curs.head = &parent.$name_head;");
        }
        // prefetch next element
        // for a delcurs on a circular list of size 1, next will point to the
        // same element (curs.row == *curs.head)
        // so it must be ignored.
        if (needdel) {
            Ins(&R, func.body, "if (curs.row) {");
            Ins(&R, func.body, "    curs.next = (*curs.row).$name_next;");
            if (circular) {
                Ins(&R, func.body, "    if (curs.next == *curs.head) {");
                Ins(&R, func.body, "        curs.next = NULL;");
                Ins(&R, func.body, "    }");
            }
            Ins(&R, func.body, "}");
        }
    }

    {
        amc::FFunc& func = amc::ind_func_GetOrCreate(Subst(R,"$field_$curstype.ValidQ"));
        func.inl = true;
        Ins(&R, func.comment, "cursor points to valid item");
        Ins(&R, func.ret  , "bool", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_ValidQ($Parname_$name_$curstype &curs)", false);
        Ins(&R, func.body, "return curs.row != NULL;");
    }

    {
        amc::FFunc& func = amc::ind_func_GetOrCreate(Subst(R,"$field_$curstype.Next"));
        func.inl = true;
        Ins(&R, func.comment, "proceed to next item");
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_Next($Parname_$name_$curstype &curs)", false);
        if (needdel) {
            Ins(&R, func.body, "$Cpptype *next = curs.next;");
        } else {
            Ins(&R, func.body, "$Cpptype *next = (*curs.row).$name_next;");
        }
        Ins(&R, func.body, "curs.row = next;");
        if (circular && !needdel) {
            Ins(&R, func.body, "if (curs.row == *curs.head) {");
            Ins(&R, func.body, "    curs.row = NULL;");
            Ins(&R, func.body, "}");
        }
        if (needdel) {
            Ins(&R, func.body, "if (curs.row) {");
            Ins(&R, func.body, "    curs.next = (*curs.row).$name_next;");
            if (circular) {
                Ins(&R, func.body, "if (curs.next == *curs.head) {");
                Ins(&R, func.body, "    curs.next = NULL;");
                Ins(&R, func.body, "}");
            }
            Ins(&R, func.body, "}");
        }
    }

    {
        amc::FFunc& func = amc::ind_func_GetOrCreate(Subst(R,"$field_$curstype.Access"));
        func.inl = true;
        Ins(&R, func.comment, "item access");
        Ins(&R, func.ret  , "$Cpptype&", false);
        Ins(&R, func.proto, "$Parname_$name_$curstype_Access($Parname_$name_$curstype &curs)", false);
        Ins(&R, func.body, "return *curs.row;");
    }
}

void amc::tfunc_Llist_curs() {
    bool needdel=false;
    Llist_curs(needdel);
}

void amc::tfunc_Llist_delcurs() {
    bool needdel=true;
    Llist_curs(needdel);
}
