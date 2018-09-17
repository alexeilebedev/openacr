// (C) AlgoEngineering LLC 2008-2012
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/thash.cpp -- Hash tables
//
// Created By: alexei.lebedev jeffrey.wang
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan jeffrey.wang
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool EarlierPoolQ(amc::FField &field, amc::FField &first_inst) {
    bool same_scope = field.p_ctype == first_inst.p_ctype;
    bool glob = GlobalQ(*field.p_ctype);
    bool pool_earlier = first_inst.rowid < field.rowid;
    bool ret = !glob && same_scope && !field.c_cascdel;
    ret = ret && pool_earlier;
    ret = ret && (first_inst.reftype == dmmeta_Reftype_reftype_Lary
                  || first_inst.reftype == dmmeta_Reftype_reftype_Tary);
    return ret;
}

static void Thash_Check(amc::FField &field) {
    // check if the field is an index for an item whose pool is in the same
    // scope but declared earlier (e.g. Lary, Tary).
    // #AL# -- one way to avoid this is if we call RemoveAll on the index
    // in the destructor.
    // #AL# What if the child element has Upptr to the parent element?
    amc::FField *first_inst = amc::FirstInst(*field.p_arg);
    if (first_inst && EarlierPoolQ(field,*first_inst)) {
        prerr("amc.hash_order"
              <<Keyval("index_field",field.field)
              <<Keyval("element_field",first_inst->field)
              <<Keyval("element_type",field.p_arg->ctype)
              <<Keyval("comment","Index field is declared in the same scope"
                       " and later than the pool of its element field"
                       " but this index is not cascdel. This will cause a crash during destruction."
                       " Either add cascdel to this index, or reorder index and pool"));
        algo_lib::_db.exit_code=1;
    }
    amc::FThash &thash = *field.c_thash;
    amc::FField &hashfld = *thash.p_hashfld;
    // check that a unique global hash defined on a relational type
    // is defined over a provably unique field
    //
    if (thash.unique && hashfld.p_ctype->c_ssimfile && GlobalQ(*field.p_ctype)) {
        amc::FField *pkey = c_field_Find(*hashfld.p_ctype,0);
        if (!hashfld.c_funique && pkey != &hashfld) {
            prerr("amc.iffyhash"
                  <<Keyval("thash",field.field)
                  <<Keyval("hashfld",thash.hashfld)
                  <<Keyval("comment","Hash is marked unique, but hash field is not marked funique, and isn't a pkey"));
            prlog("dmmeta.thash"
                  <<Keyval("field",field.field)
                  <<Keyval("unique","N"));
            //algo_lib::_db.exit_code=1;
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Thash() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FThash &thash = *field.c_thash;
    amc::FField        *hashfld   = thash.p_hashfld;

    Set(R, "$Hashfldarg" , Argtype(*hashfld));
    Set(R, "$Hashfldtype", name_Get(*hashfld->p_arg));
    Set(R, "$rethashfld" , FieldvalExpr(field.p_arg, *hashfld,"(*ret)"));
    Set(R, "$gethashfld" , FieldvalExpr(field.p_arg, *hashfld,"row"));

    InsVar(R, field.p_ctype     , "$Cpptype**", "$name_buckets_elems", "", "pointer to bucket array");
    InsVar(R, field.p_ctype     , "i32", "$name_buckets_n", "", "number of elements in bucket array");
    InsVar(R, field.p_ctype     , "i32", "$name_n", "", "number of elements in the hash table");
    InsVar(R, field.p_arg       , "$Cpptype*", "$name_next", "", "hash next");

    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_arg);
    Set(R, "$fname"     , Refname(*field.p_arg));
    Ins(&R, child_init->body, "$fname.$name_next = ($Cpptype*)-1; // ($field) not-in-hash");

    Thash_Check(field);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Thash_Find() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& find = amc::CreateCurFunc();
    Ins(&R, find.ret  , "$Cpptype*", false);
    Ins(&R, find.proto, "$name_Find($Parent, $Hashfldarg key)", false);
    Ins(&R, find.body, "u32 index = $Hashfldtype_Hash(0, key) & ($parname.$name_buckets_n - 1);");
    Ins(&R, find.body, "$Cpptype* *e = &$parname.$name_buckets_elems[index];");
    Ins(&R, find.body, "$Cpptype* ret=NULL;");
    Ins(&R, find.body, "do {");
    Ins(&R, find.body, "    ret       = *e;");
    Ins(&R, find.body, "    bool done = !ret || $rethashfld == key;");
    Ins(&R, find.body, "    if (done) break;");
    Ins(&R, find.body, "    e         = &ret->$name_next;");
    Ins(&R, find.body, "} while (true);");
    Ins(&R, find.body, "return ret;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Thash_FindX() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& findx = amc::CreateCurFunc();
    Ins(&R, findx.ret  , "$Cpptype&", false);
    Ins(&R, findx.proto, "$name_FindX($Parent, $Hashfldarg key)", false);
    Ins(&R, findx.body, "$Cpptype* ret = $name_Find($pararg, key);");
    Ins(&R, findx.body, "vrfy(ret, tempstr() << \"$ns.key_error  table:$name  key:'\"<<key<<\"'  comment:'key not found'\");");
    Ins(&R, findx.body, "return *ret;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Thash_Reserve() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& reserve = amc::CreateCurFunc();
    Ins(&R, reserve.ret  , "void", false);
    Ins(&R, reserve.proto, "$name_Reserve($Parent, int n)", false);
    Ins(&R, reserve.body, "u32 old_nbuckets = $parname.$name_buckets_n;");
    Ins(&R, reserve.body, "u32 new_nelems   = $parname.$name_n + n;");
    Ins(&R, reserve.body, "// # of elements has to be roughly equal to the number of buckets");
    Ins(&R, reserve.body, "if (new_nelems > old_nbuckets) {");
    Ins(&R, reserve.body, "    int new_nbuckets = i32_Max(BumpToPow2(new_nelems), u32(4));");
    Ins(&R, reserve.body, "    u32 old_size = old_nbuckets * sizeof($Cpptype*);");
    Ins(&R, reserve.body, "    u32 new_size = new_nbuckets * sizeof($Cpptype*);");
    Ins(&R, reserve.body, "    // allocate new array. we don't use Realloc since copying is not needed and factor of 2 probably");
    Ins(&R, reserve.body, "    // means new memory will have to be allocated anyway");
    Ins(&R, reserve.body, "    $Cpptype* *new_buckets = ($Cpptype**)$basepool_AllocMem(new_size);");
    Ins(&R, reserve.body, "    if (UNLIKELY(!new_buckets)) {");
    Ins(&R, reserve.body, "        FatalErrorExit(\"$ns.out_of_memory  field:$field\");");
    Ins(&R, reserve.body, "    }");
    Ins(&R, reserve.body, "    memset(new_buckets, 0, new_size); // clear pointers");
    Ins(&R, reserve.body, "    // rehash all entries");
    Ins(&R, reserve.body, "    for (int i = 0; i < $parname.$name_buckets_n; i++) {");
    Ins(&R, reserve.body, "        $Cpptype* elem = $parname.$name_buckets_elems[i];");
    Ins(&R, reserve.body, "        while (elem) {");
    Ins(&R, reserve.body, "            $Cpptype &row        = *elem;");
    Ins(&R, reserve.body, "            $Cpptype* next       = row.$name_next;");
    Ins(&R, reserve.body, "            u32 index          = $Hashfldtype_Hash(0, $gethashfld) & (new_nbuckets-1);");
    Ins(&R, reserve.body, "            row.$name_next     = new_buckets[index];");
    Ins(&R, reserve.body, "            new_buckets[index] = &row;");
    Ins(&R, reserve.body, "            elem               = next;");
    Ins(&R, reserve.body, "        }");
    Ins(&R, reserve.body, "    }");
    Ins(&R, reserve.body, "    // free old array");
    Ins(&R, reserve.body, "    $basepool_FreeMem($parname.$name_buckets_elems, old_size);");
    Ins(&R, reserve.body, "    $parname.$name_buckets_elems = new_buckets;");
    Ins(&R, reserve.body, "    $parname.$name_buckets_n = new_nbuckets;");
    Ins(&R, reserve.body, "}");
}

// -----------------------------------------------------------------------------

// Check if GetOrCreate can be generated for hash FIELD
// The target type is FIELD.ARG
static bool CanGetOrCreateQ(amc::FField &field) {
    amc::FCtype &ctype = *field.p_arg;
    amc::FThash &thash = *field.c_thash;
    bool correct_getorcreate = ctype.p_ns->c_nsx && ctype.p_ns->c_nsx->correct_getorcreate;
    int n_iffy_path = 0;
    int max_iffy_path = correct_getorcreate ? 0 : 1;
    ind_beg(amc::ctype_zs_xref_curs,xref,ctype) {
        bool always_succeeds = true;
        if (xref.p_field == &field) {
            if (correct_getorcreate) {
                // xref via this hash succeeds only if there is no via,
                // or the key field is actually the hash field and thus initialized
                // by the time the function is called
                always_succeeds = keyfld_Get(xref) == thash.hashfld || xref.via=="";
            }
        } else {
            // a global llist always succeeds...
            always_succeeds = (xref.p_field->reftype == dmmeta_Reftype_reftype_Llist
                               && GlobalQ(*xref.p_field->p_ctype))
                || xref.p_field->p_reftype->isval;
        }
        bool ignore = correct_getorcreate
            ? xref.inscond.value =="false"
            : xref.inscond.value !="false";// trying to right an old wrong

        if (Regx_Match(amc::_db.cmdline.trace, ctype.ctype)) {
            prlog("amc.trace_getorcreate"
                  <<Keyval("thash",thash.field)
                  <<Keyval("hashfld",thash.hashfld)
                  <<Keyval("xref",xref.field)
                  <<Keyval("via",xref.via)
                  <<Keyval("always_succeeds",always_succeeds)
                  <<Keyval("ignore",Bool(ignore))
                  <<Keyval("iffy",Bool(!ignore && !always_succeeds))
                  );
        }
        n_iffy_path += !ignore && !always_succeeds;
    }ind_end;
    return n_iffy_path <= max_iffy_path
        && !ctype.c_varlenfld
        && !ctype.c_optfld
        && FirstInst(ctype)!=NULL;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Thash_GetOrCreate() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FField *inst = FirstInst(*field.p_arg);
    amc::FThash &thash = *field.c_thash;
    // this function must always succeed!
    if (CanGetOrCreateQ(field) && field.c_xref && !FldfuncQ(*thash.p_hashfld)) {
        Set(R, "$impool"     , strptr(name_Get(*inst)));
        amc::FFunc& goc = amc::CreateCurFunc();
        Ins(&R, goc.ret  , "$Cpptype&", false);
        Ins(&R, goc.proto, "$name_GetOrCreate($Parent, $Hashfldarg key)", false);
        Ins(&R, goc.body,         "$Cpptype* ret = $name_Find($pararg, key);");
        Ins(&R, goc.body,         "if (!ret) { //  if memory alloc fails, process dies; if insert fails, function returns NULL.");
        if (inst && GlobalQ(*inst->p_ctype)) {
            Ins(&R, goc.body,     "    ret         = &$impool_Alloc();");// quick fix
        } else {
            Ins(&R, goc.body,     "    ret         = &$impool_Alloc($pararg);");
        }
        Ins(&R, goc.body,         "    $rethashfld = key;");
        Ins(&R, goc.body,         "    bool good = $impool_XrefMaybe(*ret);");
        Ins(&R, goc.body,         "    if (!good) {");
        if (inst && GlobalQ(*inst->p_ctype)) {
            if (HasRemoveLastQ(*inst->p_reftype)) {
                Ins(&R, goc.body, "        $impool_RemoveLast(); // delete offending row, any existing xrefs are cleared");
            } else {
                Ins(&R, goc.body, "        $impool_Delete(*ret); // delete offending row, any existin xrefs are cleared");
            }
        } else {
            if (inst && HasRemoveLastQ(*inst->p_reftype)) {
                Ins(&R, goc.body, "        $impool_RemoveLast($pararg); // delete offending row, any existing xrefs are cleared");
            } else {
                Ins(&R, goc.body, "        $impool_Delete($pararg, *ret); // delete offending row, any existin xrefs are cleared");
            }
        }
        Ins(&R, goc.body,         "        ret = NULL;");
        Ins(&R, goc.body,         "    }");
        Ins(&R, goc.body,         "}");
        Ins(&R, goc.body,         "return *ret;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Thash_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& nitems = amc::CreateCurFunc();
    Ins(&R, nitems.ret  , "i32", false);
    Ins(&R, nitems.proto, "$name_N($Cparent)", false);
    Ins(&R, nitems.body, "return $parname.$name_n;");
}

void amc::tfunc_Thash_EmptyQ() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& emptyq = amc::CreateCurFunc();
    Ins(&R, emptyq.ret  , "bool", false);
    Ins(&R, emptyq.proto, "$name_EmptyQ($Parent)", false);
    Ins(&R, emptyq.body, "return $parname.$name_n == 0;");
}

void amc::tfunc_Thash_InsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FThash &thash = *field.c_thash;

    amc::FFunc& ins = amc::CreateCurFunc();
    Ins(&R, ins.comment, "Insert row into hash table. Return true if row is reachable through the hash after the function completes.");
    Ins(&R, ins.ret  , "bool", false);
    Ins(&R, ins.proto, "$name_InsertMaybe($Parent, $Cpptype& row)", false);
    Ins(&R, ins.body    , "$name_Reserve($pararg, 1);");
    Ins(&R, ins.body    , "bool retval = true; // if already in hash, InsertMaybe returns true");
    Ins(&R, ins.body    , "if (LIKELY(row.$name_next == ($Cpptype*)-1)) {// check if in hash already");
    Ins(&R, ins.body    , "    u32 index = $Hashfldtype_Hash(0, $gethashfld) & ($parname.$name_buckets_n - 1);");
    Ins(&R, ins.body    , "    $Cpptype* *prev = &$parname.$name_buckets_elems[index];");
    if (thash.unique) {
        Ins(&R, ins.body, "    do {");
        Ins(&R, ins.body, "        $Cpptype* ret = *prev;");
        Ins(&R, ins.body, "        if (!ret) { // exit condition 1: reached the end of the list");
        Ins(&R, ins.body, "            break;");
        Ins(&R, ins.body, "        }");
        Ins(&R, ins.body, "        if ($rethashfld == $gethashfld) { // exit condition 2: found matching key");
        Ins(&R, ins.body, "            retval = false;");
        Ins(&R, ins.body, "            break;");
        Ins(&R, ins.body, "        }");
        Ins(&R, ins.body, "        prev = &ret->$name_next;");
        Ins(&R, ins.body, "    } while (true);");
    }
    Ins(&R, ins.body    , "    if (retval) {");
    Ins(&R, ins.body    , "        row.$name_next = *prev;");
    Ins(&R, ins.body    , "        $parname.$name_n++;");
    Ins(&R, ins.body    , "        *prev = &row;");
    Ins(&R, ins.body    , "    }");
    Ins(&R, ins.body    , "}");
    Ins(&R, ins.body    , "return retval;");
}

void amc::tfunc_Thash_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();// already exists!
        Ins(&R, cascdel.body, "if ($parname.$name_n) {");
        Ins(&R, cascdel.body, "    for (int i = 0; i < $parname.$name_buckets_n; i++) {");
        Ins(&R, cascdel.body, "        $Cpptype *elem = $parname.$name_buckets_elems[i];");
        Ins(&R, cascdel.body, "        while (elem) {");
        Ins(&R, cascdel.body, "            $Cpptype *next = elem->$name_next;");
        Ins(&R, cascdel.body, DeleteExpr(field, "$pararg", "*elem") << ";");
        Ins(&R, cascdel.body, "            elem = next;");
        Ins(&R, cascdel.body, "        }");
        Ins(&R, cascdel.body, "    }");
        Ins(&R, cascdel.body, "}");
    }
}

void amc::tfunc_Thash_Remove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& rem = amc::CreateCurFunc();
    Ins(&R, rem.ret  , "void", false);
    Ins(&R, rem.proto, "$name_Remove($Parent, $Cpptype& row)", false);
    Ins(&R, rem.body, "if (LIKELY(row.$name_next != ($Cpptype*)-1)) {// check if in hash already");
    Ins(&R, rem.body, "    u32 index = $Hashfldtype_Hash(0, $gethashfld) & ($parname.$name_buckets_n - 1);");
    Ins(&R, rem.body, "    $Cpptype* *prev = &$parname.$name_buckets_elems[index]; // addr of pointer to current element");
    Ins(&R, rem.body, "    while ($Cpptype *next = *prev) {                          // scan the collision chain for our element");
    Ins(&R, rem.body, "        if (next == &row) {        // found it?");
    Ins(&R, rem.body, "            *prev = next->$name_next; // unlink (singly linked list)");
    Ins(&R, rem.body, "            $parname.$name_n--;");
    Ins(&R, rem.body, "            row.$name_next = ($Cpptype*)-1;// not-in-hash");
    Ins(&R, rem.body, "            break;");
    Ins(&R, rem.body, "        }");
    Ins(&R, rem.body, "        prev = &next->$name_next;");
    Ins(&R, rem.body, "    }");
    Ins(&R, rem.body, "}");
}

void amc::tfunc_Thash_FindRemove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.c_findrem) {
        amc::FFunc& findrem = amc::CreateCurFunc();
        Ins(&R, findrem.ret  , "$Cpptype*", false);
        Ins(&R, findrem.proto, "$name_FindRemove($Parent, $Hashfldarg key)", false);
        Ins(&R, findrem.body, "u32 index = $Hashfldtype_Hash(0, key) & ($parname.$name_buckets_n - 1);");
        Ins(&R, findrem.body, "$Cpptype* *prev = &$parname.$name_buckets_elems[index];");
        Ins(&R, findrem.body, "$Cpptype* ret=NULL;");
        Ins(&R, findrem.body, "do {");
        Ins(&R, findrem.body, "    ret       = *prev;");
        Ins(&R, findrem.body, "    bool done = !ret || $rethashfld == key;");
        Ins(&R, findrem.body, "    if (done) break;");
        Ins(&R, findrem.body, "    prev         = &ret->$name_next;");
        Ins(&R, findrem.body, "} while (true);");
        Ins(&R, findrem.body, "if (ret) {");
        Ins(&R, findrem.body, "    *prev = ret->$name_next;");
        Ins(&R, findrem.body, "    ret->$name_next = ($Cpptype*)-1; // not-in-hash");
        Ins(&R, findrem.body, "    $parname.$name_n--;");
        Ins(&R, findrem.body, "}");
        Ins(&R, findrem.body, "return ret;");
    }
}

void amc::tfunc_Thash_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = false;
    init.priv = true;// static
    Ins(&R, init.body, "// initialize hash table for $Cpptype;");
    Ins(&R, init.body, "$parname.$name_n             \t= 0; // ($field)");
    Ins(&R, init.body, "$parname.$name_buckets_n     \t= 4; // ($field)");
    Ins(&R, init.body, "$parname.$name_buckets_elems \t= ($Cpptype**)$basepool_AllocMem(sizeof($Cpptype*)*$parname.$name_buckets_n); // initial buckets ($field)");
    Ins(&R, init.body, "if (!$parname.$name_buckets_elems) {");
    Ins(&R, init.body, "    FatalErrorExit(\"out of memory\"); // ($field)");
    Ins(&R, init.body, "}");
    Ins(&R, init.body, "memset($parname.$name_buckets_elems, 0, sizeof($Cpptype*)*$parname.$name_buckets_n); // ($field)");
}

void amc::tfunc_Thash_Uninit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& uninit = amc::CreateCurFunc();
    if (GlobalQ(*field.p_ctype)) {
        Ins(&R, uninit.body, "// skip destruction of $name in global scope");
    } else {
        Ins(&R, uninit.body, "$basepool_FreeMem($parname.$name_buckets_elems, sizeof($Cpptype*)*$parname.$name_buckets_n); // ($field)");
    }
}

// -----------------------------------------------------------------------------

static bool CursN(amc::FField &field) {
    int n=0;
    ind_beg(amc::ctype_c_fcurs_curs,fcurs,*field.p_ctype) {
        n += field_Get(fcurs) == field.field;
    }ind_end;
    return n;
}

void amc::tfunc_Thash_curs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    amc::FField &field = *amc::_db.genfield.p_field;
    Set(R, "$curspararg", ch_N(Subst(R,"$pararg")) ? "*curs.$parname" : "");

    // zero-density hash shouldn't need a cursor
    if (CursN(field)>0) {
        Ins(&R, ns.curstext    , "");
        Ins(&R, ns.curstext    , "struct $Parname_$name_curs {// cursor");
        Ins(&R, ns.curstext    , "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext    , "    $Partype *parent;");
        Ins(&R, ns.curstext    , "    int bucket;");
        Ins(&R, ns.curstext    , "    $Cpptype **prow;");
        Ins(&R, ns.curstext    , "    $Parname_$name_curs() { parent=NULL; bucket=0; prow=NULL; }");
        Ins(&R, ns.curstext    , "};");
        Ins(&R, ns.curstext    , "");

        {
            amc::FFunc& reset = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Reset"));
            Ins(&R, reset.ret  , "void", false);
            Ins(&R, reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
            Ins(&R, reset.body, "curs.bucket = 0;");
            Ins(&R, reset.body, "curs.parent = &parent;");
            Ins(&R, reset.body, "curs.prow = &parent.$name_buckets_elems[0]; // hash never has zero buckets");
            Ins(&R, reset.body, "while (!*curs.prow) {");
            Ins(&R, reset.body, "    curs.bucket += 1;");
            Ins(&R, reset.body, "    if (curs.bucket == parent.$name_buckets_n) break;");
            Ins(&R, reset.body, "    curs.prow = &parent.$name_buckets_elems[curs.bucket];");
            Ins(&R, reset.body, "}");
        }

        {
            amc::FFunc& curs_validq = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.ValidQ"));
            curs_validq.inl = true;
            Ins(&R, curs_validq.comment, "cursor points to valid item");
            Ins(&R, curs_validq.ret  , "bool", false);
            Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
            Ins(&R, curs_validq.body, "return *curs.prow != NULL;");
        }

        {
            amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Next"));
            curs_next.inl = true;
            Ins(&R, curs_next.comment, "proceed to next item");
            Ins(&R, curs_next.ret  , "void", false);
            Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
            Ins(&R, curs_next.body, "curs.prow = &(*curs.prow)->$name_next;");
            Ins(&R, curs_next.body, "while (!*curs.prow) {");
            Ins(&R, curs_next.body, "    curs.bucket += 1;");
            Ins(&R, curs_next.body, "    if (curs.bucket >= curs.parent->$name_buckets_n) break;");
            Ins(&R, curs_next.body, "    curs.prow = &curs.parent->$name_buckets_elems[curs.bucket];");
            Ins(&R, curs_next.body, "}");
        }

        amc::FFunc& curs_access = amc::ind_func_GetOrCreate(Subst(R,"$field_curs.Access"));
        curs_access.inl = true;
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "$Cpptype&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
        Ins(&R, curs_access.body, "return **curs.prow;");
    }
}
