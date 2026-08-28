// Copyright (C) 2026 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/fcond.cpp
//
// A dmmeta.fcond row ties membership in an index to a condition on a
// field: the record (or its via target) is inserted into the ins index
// when the condition becomes true, and rem:Y removes it when the
// condition stops holding (rem:N leaves removal to the queue's
// consumer).  Which conditions a field has depends on its reftype.  A
// Val field's conditions are its values, so the value component of the
// fcond key names an fconst or a literal, and the generated _Set
// function performs the maintenance on every value transition; see
// txt/amc/fcond.md.  Every other reftype has a condition vocabulary of
// its own, declared as amcdb.tcond rows of its tclass, and its
// generator reports the condition at the point it flips -- an fbuf
// leaves cd_conn_outflow when the socket drains, which no value
// assignment could have expressed.
// This file validates the fcond records (gen_check_fcond), emits the
// membership dispatch into the Val setter (AddFcondSetBody, called from
// tfunc_Val_Set), emits the initial-membership dispatch into the
// watched ctype's XrefMaybe (AddFcondXrefMaybeBody, called from
// tfunc_Ctype_XrefMaybe) -- the record's initial value decides its
// initial membership, so the list's own xref carries inscond:false and
// creators write no manual arm -- and supplies the lookup and emit
// helpers (FindFcond, AddFcondOp, FcondInsFunc) that a tclass generator
// uses to report one of its own conditions.

#include "include/algo.h"
#include "include/amc.h"

// Return the field whose fconst records name the values FIELD can take:
// FIELD itself when fconsts attach to it directly, otherwise the
// fconst-carrying field of FIELD's arg ctype (the enum-wrapper case, e.g.
// <ns>.State.value for a field of arg <ns>.State).  NULL when FIELD's
// values are plain literals (bool, integer).
static amc::FField *FcondEnumField(amc::FField &field) {
    amc::FField *ret = c_fconst_N(field) > 0 ? &field : NULL;
    if (!ret) {
        ind_beg(amc::ctype_c_field_curs, argfield, *field.p_arg) {
            if (!ret && c_fconst_N(argfield) > 0) {
                ret = &argfield;
            }
        }ind_end;
    }
    return ret;
}

// The Upptr field FCOND redirects through (NULL when via is empty):
// the pointed-to record, not the record carrying the watched field,
// is what enters and leaves the list.
amc::FField *amc::FcondViaField(amc::FFcond &fcond) {
    return fcond.via == "" ? NULL : amc::ind_field_Find(fcond.via);
}

// The tclass condition FCOND watches on FIELD, NULL when FIELD is a Val
// whose conditions are its own values.  Every other reftype declares the
// conditions it can report as amcdb.tcond rows of its tclass, and the value
// component of the fcond key names one of them.
amc::FTcond *amc::FcondTcond(amc::FField &field, amc::FFcond &fcond) {
    amc::FTcond *ret = NULL;
    if (field.reftype != dmmeta_Reftype_reftype_Val && field.p_reftype->p_tclass) {
        tempstr key = amcdb::Tcond_Concat_tclass_name(field.p_reftype->p_tclass->tclass, value_Get(fcond));
        ret = amc::ind_tcond_Find(key);
    }
    return ret;
}

// The fcond row declaring condition TCOND on FIELD, NULL when FIELD declares
// no membership for it.  A tclass generator calls this at each point it can
// report TCOND, naming the condition by its generated symbol
// (amcdb_tcond_Fbuf_ready and friends), and emits nothing where the row is
// absent.  A generator that asks for a condition its own tclass does not
// declare is a generator bug, and fails here rather than silently generating
// nothing.
amc::FFcond *amc::FindFcond(amc::FField &field, algo::strptr tcond) {
    amc::FFcond *ret = NULL;
    amc::FTcond *cond = amc::ind_tcond_Find(tcond);
    vrfy(cond, tempstr()<<"amc.fcond_notcond"
         <<Keyval("tcond",tcond)
         <<Keyval("comment","no such condition; see amcdb.tcond"));
    vrfy(field.p_reftype->p_tclass && tclass_Get(*cond) == field.p_reftype->p_tclass->tclass, tempstr()<<"amc.fcond_tclass"
         <<Keyval("tcond",tcond)
         <<Keyval("field",field.field)
         <<Keyval("reftype",field.reftype)
         <<Keyval("comment","condition belongs to another tclass"));
    ind_beg(amc::field_c_fcond_curs, fcond, field) {
        if (value_Get(fcond) == name_Get(*cond)) {
            ret = &fcond;
        }
    }ind_end;
    return ret;
}

// The namespace-qualified name of the function that OPs (Insert or Remove)
// a record on FCOND's index.
tempstr amc::FcondOpFunc(amc::FFcond &fcond, algo::strptr op) {
    return tempstr()<<ns_Get(*fcond.p_ins->p_ctype)<<"::"<<name_Get(*fcond.p_ins)<<"_"<<op;
}

// The insert function of FCOND's index, for a generator that needs the
// function itself rather than a call to it -- an fbuf hands it to
// callback_Set1 as the file descriptor's readiness hook.  The function takes
// the record carrying the watched field, so a via redirection has no way
// through; FcondNocallbackQ rejects that combination up front.
tempstr amc::FcondInsFunc(amc::FFcond &fcond) {
    return amc::FcondOpFunc(fcond, "Insert");
}

// True when FCOND cannot be installed as a plain callback on the record
// carrying the watched field, because it redirects the membership elsewhere.
bool amc::FcondNocallbackQ(amc::FFcond &fcond) {
    return fcond.via != "";
}

// Emit into BODY the list operation FCOND declares -- OP is Insert or
// Remove -- performed on the record that carries the watched field, or on
// the record its via pointer names.  The generated Insert and Remove are
// self-guarding, so repeating the operation is a no-op, and a NULL via
// pointer skips it.
void amc::AddFcondOp(algo::cstring &body, amc::FFcond &fcond, algo::strptr op) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField *viafld = amc::FcondViaField(fcond);
    Set(R, "$condcall", amc::FcondOpFunc(fcond, op));
    if (viafld) {
        Set(R, "$condvia", name_Get(*viafld));
        Ins(&R, body, "if ($parname.$condvia) {");
        Ins(&R, body, "    $condcall(*$parname.$condvia);");
        Ins(&R, body, "}");
    } else {
        Ins(&R, body, "$condcall($pararg);");
    }
}

// Resolve FCOND's value component to the C++ expression the generated
// setter compares against: the fconst symbol when FIELD's values are
// named by fconsts, the literal itself for bool and integer fields.
// An unresolvable value fails with an error naming the record.
tempstr amc::FcondCppValue(amc::FField &field, amc::FFcond &fcond) {
    tempstr ret;
    amc::FField *enumfld = FcondEnumField(field);
    algo::strptr value = value_Get(fcond);
    if (enumfld) {
        amc::FFconst *fconst = amc::ind_fconst_Find(tempstr()<<enumfld->field<<"/"<<value);
        vrfy(fconst, tempstr()<<"amc.fcond_value"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("enumfield",enumfld->field)
             <<Keyval("comment","value is not an fconst of the watched field"));
        ret << fconst->cpp_name;
    } else if (field.arg == "bool") {
        vrfy(value == "true" || value == "false", tempstr()<<"amc.fcond_value"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("comment","bool field: value must be true or false"));
        ret << value;
    } else {
        i64 num = 0;
        vrfy(i64_ReadStrptrMaybe(num, value), tempstr()<<"amc.fcond_value"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("arg",field.arg)
             <<Keyval("comment","value must be an fconst name or an integer literal"));
        ret << num;
    }
    return ret;
}

// Check fcond records: the condition belongs to the watched field's domain
// -- a value of a Val field, or a condition its reftype's tclass declares --
// the ins index is intrusive and lives on the namespace global of the same
// namespace, its xref does not compete for insertion, and the type of
// the inserted record (the watched ctype, or the via target) matches
// the index's element type.
void amc::gen_check_fcond() {
    ind_beg(amc::_db_fcond_curs, fcond, amc::_db) {
        amc::FField *field = amc::ind_field_Find(field_Get(fcond));
        vrfy(field, tempstr()<<"amc.fcond_field"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("comment","watched field not found"));
        if (field->reftype == dmmeta_Reftype_reftype_Val) {
            vrfy(!FldfuncQ(*field), tempstr()<<"amc.fcond_fldfunc"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("comment","fcond cannot watch a computed field"));
            vrfy(!field->c_fbigend && c_pmaskfld_member_N(*field) == 0, tempstr()<<"amc.fcond_combo"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("comment","fcond cannot combine with fbigend or pmask"));
            (void)amc::FcondCppValue(*field, fcond);
        } else {
            vrfy(amc::FcondTcond(*field, fcond), tempstr()<<"amc.fcond_tcond"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("reftype",field->reftype)
                 <<Keyval("comment","reftype reports no condition by this name; see amcdb.tcond"));
        }
        amc::FField &ins = *fcond.p_ins;
        vrfy(ins.reftype == dmmeta_Reftype_reftype_Llist || ins.reftype == dmmeta_Reftype_reftype_Bheap
             , tempstr()<<"amc.fcond_ins"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("reftype",ins.reftype)
             <<Keyval("comment","ins must name an Llist or Bheap"));
        vrfy(GlobalQ(*ins.p_ctype), tempstr()<<"amc.fcond_ins"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("ins",ins.field)
             <<Keyval("comment","ins index must live on the namespace global (FDb)"));
        vrfy(ins.p_ctype->p_ns == field->p_ctype->p_ns, tempstr()<<"amc.fcond_ns"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("ins",ins.field)
             <<Keyval("comment","ins index must live in the watched field's namespace"));
        amc::FXref *insxref = ins.c_xref;
        vrfy(!insxref || insxref->inscond.value == "false", tempstr()<<"amc.fcond_inscond"
             <<Keyval("fcond",fcond.fcond)
             <<Keyval("ins",ins.field)
             <<Keyval("inscond",insxref->inscond.value)
             <<Keyval("comment","fcond owns insertion into the index; set inscond:false on its xref"));
        amc::FField *viafld = amc::FcondViaField(fcond);
        if (fcond.via != "") {
            vrfy(viafld, tempstr()<<"amc.fcond_via"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("via",fcond.via)
                 <<Keyval("comment","via field not found"));
            vrfy(viafld->p_ctype == field->p_ctype, tempstr()<<"amc.fcond_via"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("via",fcond.via)
                 <<Keyval("comment","via must be a field of the watched ctype"));
            vrfy(viafld->reftype == dmmeta_Reftype_reftype_Upptr || viafld->reftype == dmmeta_Reftype_reftype_Ptr
                 , tempstr()<<"amc.fcond_via"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("reftype",viafld->reftype)
                 <<Keyval("comment","via must be a pointer field"));
            vrfy(viafld->p_arg == ins.p_arg, tempstr()<<"amc.fcond_via"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("via_arg",viafld->arg)
                 <<Keyval("ins_arg",ins.arg)
                 <<Keyval("comment","via target type must match the ins index's element type"));
        } else {
            vrfy(field->p_ctype == ins.p_arg, tempstr()<<"amc.fcond_arg"
                 <<Keyval("fcond",fcond.fcond)
                 <<Keyval("ctype",field->p_ctype->ctype)
                 <<Keyval("ins_arg",ins.arg)
                 <<Keyval("comment","watched ctype must match the ins index's element type (or set via)"));
        }
    }ind_end;
}

// Emit one guarded list operation of the membership dispatch into SET:
// compare EXPR (the old value for removes, the incoming value for
// inserts) against FCOND's value, and OP the target record on the list.
// The generated Insert/Remove are self-guarding, so a record already in
// (or absent from) the list is a no-op, and a NULL via pointer skips.
static void EmitFcondOp(amc::FFunc &set, amc::FField &field, amc::FFcond &fcond, algo::strptr expr, algo::strptr op) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField *viafld = amc::FcondViaField(fcond);
    Set(R, "$condval", amc::FcondCppValue(field, fcond));
    Set(R, "$condexpr", expr);
    Set(R, "$condcall", amc::FcondOpFunc(fcond, op));
    if (viafld) {
        Set(R, "$condvia", name_Get(*viafld));
        Ins(&R, set.body, "    if ($condexpr == $condval && $parname.$condvia) {");
        Ins(&R, set.body, "        $condcall(*$parname.$condvia);");
        Ins(&R, set.body, "    }");
    } else {
        Ins(&R, set.body, "    if ($condexpr == $condval) {");
        Ins(&R, set.body, "        $condcall($pararg);");
        Ins(&R, set.body, "    }");
    }
}

// Emit the fcond membership dispatch as the body of SET, the generated
// setter of FIELD: on a value change, rows matching the old value remove
// (rem:Y), the assignment happens, and rows matching the new value
// insert.  A same-value Set performs no list operation -- conditions
// are level-triggered, and the level did not change.
void amc::AddFcondSetBody(amc::FFunc &set, amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    Ins(&R, set.body, "if (!($parname.$name == rhs)) {");
    ind_beg(amc::field_c_fcond_curs, fcond, field) {
        if (fcond.rem) {
            EmitFcondOp(set, field, fcond, "$parname.$name", "Remove");
        }
    }ind_end;
    Ins(&R, set.body, "    $parname.$name = rhs;");
    ind_beg(amc::field_c_fcond_curs, fcond, field) {
        EmitFcondOp(set, field, fcond, "rhs", "Insert");
    }ind_end;
    Ins(&R, set.body, "}");
}

// Emit the initial-membership dispatch into XREFMAYBE, the generated
// x-reference function of CTYPE: each fcond row on a Val field inserts the
// record (or its via target) when the field holds the row's value at xref
// time.  The initial value thereby decides the initial membership -- a
// record that must start registered leaves the field at its default (or
// sets it before XrefMaybe); no manual arm is written.  Emitted after
// the xref inserts, so via pointers assigned by the x-reference step
// are already in place.  A condition of another reftype has no initial
// value to read: its tclass reports it when it flips, and a buffer that
// starts empty simply never flipped.
void amc::AddFcondXrefMaybeBody(amc::FFunc &xrefmaybe, amc::FCtype &ctype) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    Set(R, "$condns", ctype.p_ns->ns);
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.reftype == dmmeta_Reftype_reftype_Val) {
        ind_beg(amc::field_c_fcond_curs, fcond, field) {
            amc::FField *viafld = amc::FcondViaField(fcond);
            Set(R, "$condkey", fcond.fcond);
            Set(R, "$condname", name_Get(field));
            Set(R, "$condval", amc::FcondCppValue(field, fcond));
            Set(R, "$condcall", tempstr()<<name_Get(*fcond.p_ins)<<"_Insert");
            Ins(&R, xrefmaybe.body, "// fcond $condkey: the initial value decides initial membership");
            if (viafld) {
                Set(R, "$condvia", name_Get(*viafld));
                Ins(&R, xrefmaybe.body, "if (row.$condname == $condval && row.$condvia) {");
                Ins(&R, xrefmaybe.body, "    $condns::$condcall(*row.$condvia);");
                Ins(&R, xrefmaybe.body, "}");
            } else {
                Ins(&R, xrefmaybe.body, "if (row.$condname == $condval) {");
                Ins(&R, xrefmaybe.body, "    $condns::$condcall(row);");
                Ins(&R, xrefmaybe.body, "}");
            }
        }ind_end;
    }ind_end;
}
