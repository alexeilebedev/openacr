// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/hook.cpp
//
// TODO: use hook arg to provide a parameter to the hook
// add parentptr ability (make it all type safe)

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Determine if hook is static (i.e. parent ctype uses gstatic
// to assign all hook values at start time).
// Non-static hooks get a Set function.
bool amc::StaticQ(amc::FHook &hook) {
    amc::FField *inst = amc::FirstInst(*hook.p_field->p_ctype);
    return inst && inst->c_gstatic;
}

// -----------------------------------------------------------------------------

// Create a new ctype for the hook
// Name construct is <ctype>_<field>_hook, similar in style to
// that of cursors.
void amc::NewFieldHook(amc::FHook &hook) {
    amc::FField &field = *hook.p_field;

    dmmeta::Ctype ctype;
    ctype.ctype=tempstr() << ns_Get(*field.p_ctype) << "." << Refname(*field.p_ctype) << "_" << name_Get(field) << "_hook";
    hook.p_funcptr = amc::ctype_InsertMaybe(ctype);
    vrfy(hook.p_funcptr, "internal error processing hook");

    dmmeta::Cdflt cdflt;
    cdflt.ctype=ctype.ctype;
    cdflt.cppdflt.value="NULL";
    amc::cdflt_InsertMaybe(cdflt);

    dmmeta::Csize csize;
    csize.ctype = ctype.ctype;
    csize.size = sizeof(u64);
    csize.alignment = csize.size;
    amc::csize_InsertMaybe(csize);

    dmmeta::Cextern cextern;
    cextern.ctype = ctype.ctype;
    amc::cextern_InsertMaybe(cextern);
}

// -----------------------------------------------------------------------------

void amc::gen_newfield_hook() {
    ind_beg(amc::_db_hook_curs, hook, amc::_db) {
        NewFieldHook(hook);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Create typedef for the function pointer matching the hook
static void FuncPtrTypedef(algo_lib::Replscope &R, amc::FField &field) {
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FHook &hook = *field.c_hook;
    Ins(&R, *ns.hdr, tempstr()<<"typedef void (*"<<name_Get(*hook.p_funcptr)<<")();");
    if (!StaticQ(hook)) {// user context (any reference -- assigned via template)
        amc::AddArg(*ns.hdr, tempstr() << "void*" << " userctx");
    }
    if (field.arg != "") {// additional argument
        amc::AddArg(*ns.hdr, tempstr() << amc::Refto(field.p_arg->cpp_type) << " arg");
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Hook() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FField &field = *amc::_db.genfield.p_field;
    vrfy(field.c_hook, "hook record required");
    amc::FHook &hook = *field.c_hook;

    Set(R, "$HookCtype", hook.p_funcptr->ctype);
    Set(R, "$HookCpptype", hook.p_funcptr->cpp_type);

    // typedef for this function pointer
    FuncPtrTypedef(R, field);

    InsVar(R, field.p_ctype, "$HookCpptype", "$name", "NULL", "Pointer to a function");
    if (!StaticQ(hook)) {
        InsVar(R, field.p_ctype, "u64", "$name_ctx", "0", "Callback context");
    }

    // add initializer
    amc::FFunc *child_init = amc::init_GetOrCreate(*field.p_ctype);
    Ins(&R, child_init->body, "$parname.$name = NULL;");
    if (!StaticQ(hook)) {
        Ins(&R, child_init->body, "$parname.$name_ctx = 0;");
    }
}

// -----------------------------------------------------------------------------

// Generate Set function for callback
// with user callback accepting at most N arguments
// First argument is a template argument.
static void _tfunc_Hook_Set(algo_lib::Replscope &R, amc::FField &field, int n) {
    amc::FHook &hook = *field.c_hook;
    if (!StaticQ(hook)) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            func.inl=true; // templates must be inline
            AddRetval(func, "void", "", "");
            if (n>0) {
                AddTypearg(func, "T");
                AddProtoArg(func, "T&", "ctx");
            }
            tempstr funcarg;
            funcarg<<"void (*fcn)()";
            if (n>0) {// user context
                amc::AddArg(funcarg, "T&");
            }
            if (field.arg != "" && n>1) {// additional argument
                amc::AddArg(funcarg, tempstr() << amc::Refto(field.p_arg->cpp_type) << " arg");
            }
            AddProtoArg(func, funcarg, "");
            if (n>0) {
                Ins(&R, func.body, "$parname.$name_ctx = (u64)&ctx;");
            } else {
                // clear garbage out
                Ins(&R, func.body, "$parname.$name_ctx = 0;");
            }
            Ins(&R, func.body, "$parname.$name = ($HookCpptype)fcn;");
        }
    }
}

void amc::tfunc_Hook_Set0() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    _tfunc_Hook_Set(R,field,0);
}

void amc::tfunc_Hook_Set1() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    _tfunc_Hook_Set(R,field,1);
}

void amc::tfunc_Hook_Set2() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    _tfunc_Hook_Set(R,field,2);
}

// -----------------------------------------------------------------------------

// Implement _Call function for the hook
void amc::tfunc_Hook_Call() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FHook &hook = *field.c_hook;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        func.inl = true;
        if (field.arg != "") {
            AddProtoArg(func, amc::Refto(field.p_arg->cpp_type), "arg");
        }
        AddRetval(func, "void", "", "");
        Ins(&R, func.body    , "if ($parname.$name) {");
        Ins(&R, func.body    , "    $parname.$name();");
        if (!StaticQ(hook)) {// user-defined context
            amc::AddArg(func.body, Subst(R,"(void*)$parname.$name_ctx"));
        }
        if (field.arg != "") {// additional argument
            amc::AddArg(func.body, "arg");
        }
        Ins(&R, func.body    , "}");
    }
}
