// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/step.cpp
//

#include "include/amc.h"

void amc::tclass_Step() {
}

void amc::tfunc_Step_UpdateCycles() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;
    amc::FFunc& func = amc::CreateCurFunc();
    func.priv        = !amc::ExternStepQ(fstep);
    func.inl = amc::DirectStepQ(fstep) && func.priv;
    Ins(&R, func.comment, "Update cycles count from previous clock capture");
    Ins(&R, func.ret     , "void",false);
    Ins(&R, func.proto   , "$name_UpdateCycles()",false);
    Ins(&R, func.body    , "u64 cur_cycles                      = algo::get_cycles();");
    if (field.c_ftrace) {
        Ins(&R, func.body, "u64 prev_cycles                     = algo_lib::_db.clock.value;");
        Ins(&R, func.body, "++$ns::_db.trace.step_$name;");
        Ins(&R, func.body, "$ns::_db.trace.step_$name_cycles  += cur_cycles - prev_cycles;");
    }
    Ins(&R, func.body    , "algo_lib::_db.clock                 = algo::SchedTime(cur_cycles);");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Step_Step() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& step = amc::CreateCurFunc();
    Ins(&R, step.proto  , "$name_Step()",false);
    Ins(&R, step.ret  , "void",false);
    step.extrn=true;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Step_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& init = amc::CreateCurFunc();
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;
    init.inl = false;

    if (fstep.steptype == dmmeta_Steptype_steptype_InlineRecur) {
        if (fstep.c_fdelay) {
            Set(R, "$delay", tempstr()<< value_GetDouble(fstep.c_fdelay->delay));
            Ins(&R, init.body, "$ns::_db.$name_delay = algo::ToSchedTime($delay); // initialize fstep delay ($field)");
        }
    } else if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur) {
        Ins(&R, init.body, "// initialize fstep timehook ($field)");
        Ins(&R, init.body, "// timehook is recurrent with initial frequency=max.");
        Ins(&R, init.body, "hook_Set0($parname.th_$name, $ns::$name_Call);");
        Ins(&R, init.body, "ThInitRecur($parname.th_$name, algo::SchedTime());");
        if (fstep.c_fdelay) {
            vrfy(!fstep.c_fdelay->scale, "Scalable delay is only supported for InlineRecur step");
            Set(R, "$delay", tempstr()<< fstep.c_fdelay->delay);
            Ins(&R, init.body, "$ns::_db.th_$name.delay = algo::ToSchedTime($delay); // initialize fstep delay ($field)");
        }
    }
}

// -----------------------------------------------------------------------------

static void GenInlineRecur(algo_lib::Replscope &R, amc::FField &field, amc::FFunc& call) {
    amc::FFstep &fstep = *field.c_fstep;
    InsVar(R, field.p_ctype, "algo::SchedTime", "$name_next", "", "$field \tNext invocation time");
    InsVar(R, field.p_ctype, "algo::SchedTime", "$name_delay", "", "$field \tDelay between invocations");

    if (fstep.c_fdelay) {
        Set(R, "$delay", tempstr()<< value_GetDouble(fstep.c_fdelay->delay));
    }

    Ins(&R, call.body, "if ($LoopCond) { // fstep:$field");
    Ins(&R, call.body, "    if ($ns::_db.$name_next < algo_lib::_db.clock) {");
    if (fstep.c_fdelay && fstep.c_fdelay->scale) {
        Ins(&R, call.body, "        u64 effective_delay = $ns::_db.$name_delay / u64_Max(1,$ns::$name_N());");
        Ins(&R, call.body, "        $ns::_db.$name_next = algo_lib::_db.clock + algo::SchedTime(effective_delay);");
    } else {
        Ins(&R, call.body, "        $ns::_db.$name_next = algo_lib::_db.clock + $ns::_db.$name_delay;");
    }
    Ins(&R, call.body, "        $ns::$name_Step(); // steptype:InlineRecur: call function every N clock cycles");
    Ins(&R, call.body, "        $name_UpdateCycles();");
    Ins(&R, call.body, "    }");
    Ins(&R, call.body, "    algo_lib::_db.next_loop.value = u64_Min($ns::_db.$name_next, algo_lib::_db.next_loop);");
    Ins(&R, call.body, "}");
}

// -----------------------------------------------------------------------------

static tempstr GetStepCond(amc::FField &field, amc::FFstep &fstep) {
    tempstr ret;
    // special work-around for ZSListMT -- EmptyQ  cannot be defined, DestructiveFirst must be used.
    if (field.reftype == dmmeta_Reftype_reftype_ZSListMT) {
        ret="$ns::$name_DestructiveFirst() != NULL";
        vrfy(fstep.steptype == dmmeta_Steptype_steptype_Inline
             || fstep.steptype == dmmeta_Steptype_steptype_InlineRecur
             , "ZSListMT can only be used with fstep of type Inline or InlineRecur");
    } else if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
        ret= "!$ns::$name_EmptyQ()";
    } else if (ValQ(field)
               || field.reftype == dmmeta_Reftype_reftype_Ptr
               || field.reftype == dmmeta_Reftype_reftype_Upptr) {
        ret= "$ns::_db.$name";
    } else if (field.reftype == dmmeta_Reftype_reftype_Global) {
        ret= "true";
    } else {
        ret= "!$ns::$name_EmptyQ()";
    }
    return ret;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Step_Call() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;

    amc::FFunc& call = amc::CreateCurFunc();
    Ins(&R, call.proto  , "$name_Call()",false);
    Ins(&R, call.ret  , "void",false);
    call.inl = amc::DirectStepQ(fstep);
    call.priv = !amc::ExternStepQ(fstep);
    Set(R, "$LoopCond", GetStepCond(field,fstep));

    vrfy(!(fstep.steptype == dmmeta_Steptype_steptype_InlineOnce)
         || field.reftype == dmmeta_Reftype_reftype_Bheap, "TimeHookOnce requires bheap");
    vrfy(!fstep.c_fdelay || (fstep.steptype == dmmeta_Steptype_steptype_InlineRecur
                             || fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur)
         , "fdelay only applies to step type InlineRecur and TimeHookRecur");

    if (fstep.steptype == dmmeta_Steptype_steptype_InlineRecur) {
        GenInlineRecur(R, field, call);
    } else if (fstep.steptype == dmmeta_Steptype_steptype_InlineOnce) {
        Set(R, "$sortval" , FieldvalExpr(field.p_arg, *field.c_sortfld->p_sortfld, "(*$name)"));
        Set(R, "$Cpptype"  , field.p_arg->cpp_type);
        Ins(&R, call.body, "if ($Cpptype *$name = $ns::$name_First()) { // fstep:$field");
        Ins(&R, call.body, "    algo::SchedTime expire = $sortval;");
        Ins(&R, call.body, "    // if step occurs, next cycle comes with no delay.");
        Ins(&R, call.body, "    // if step isn't occuring yet, calculate delay.");
        Ins(&R, call.body, "    if (expire < algo_lib::_db.clock) {");
        Ins(&R, call.body, "        $ns::$name_Step(); // steptype:InlineOnce: call function at specified time");
        Ins(&R, call.body, "        $name_UpdateCycles();");
        Ins(&R, call.body, "    }");
        Ins(&R, call.body, "    algo_lib::_db.next_loop.value = u64_Min(expire, algo_lib::_db.next_loop);");
        Ins(&R, call.body, "}");
    } else if (fstep.steptype == dmmeta_Steptype_steptype_Inline) {
        Ins(&R, call.body, "if ($LoopCond) { // fstep:$field");
        Ins(&R, call.body, "    $ns::$name_Step(); // steptype:Inline: call function on every step");
        Ins(&R, call.body, "    $name_UpdateCycles();");
        Ins(&R, call.body, "    algo_lib::_db.next_loop = algo_lib::_db.clock;");
        Ins(&R, call.body, "}");
    } else if (fstep.steptype == dmmeta_Steptype_steptype_Callback) {
        Ins(&R, call.body, "if ($LoopCond) { // fstep:$field");
        Ins(&R, call.body, "    $ns::$name_Step(); // steptype:Callback: user calls call _UpdateCycles");
        Ins(&R, call.body, "}");
    } else if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur) {
        InsVar(R, field.p_ctype, "algo_lib::FTimehook", "th_$name", "", "$field \tfstep time hook for $field");
        Ins(&R, call.body   , "$ns::$name_Step();");
        Ins(&R, call.body   , "$name_UpdateCycles();");
    } else if (fstep.steptype == dmmeta_Steptype_steptype_Extern) {
        Ins(&R, call.body   , "$ns::$name_Step();");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Step_FirstChanged() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;
    if (field.need_firstchanged) {
        amc::FFunc& chg = amc::CreateCurFunc();
        Ins(&R, chg.comment, "First element of index changed.");
        Ins(&R, chg.ret  , "void", false);
        Ins(&R, chg.proto, "$name_FirstChanged($Parent)", false);
        bool is_extern = fstep.steptype == dmmeta_Steptype_steptype_Extern;
        chg.extrn = is_extern;
        if (!is_extern && !GlobalQ(*field.p_ctype)) {
            Ins(&R, chg.body, "(void)$pararg;");
        }
        if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur) {
            chg.priv = true;
            Ins(&R, chg.body, "$Ctype* row = $name_First($pararg);");
            Ins(&R, chg.comment, "If index $name is empty, deschedule time hook $parname.th_$name.");
            Ins(&R, chg.comment, "If index is non-empty, and time hook is not scheduled,");
            Ins(&R, chg.comment, "    schedule it after $parname.th_$name.delay clocks.");
            Ins(&R, chg.comment, "If index is non-empty, and time hook is already scheduled, do nothing");
            Ins(&R, chg.body, "if (row) {");
            Ins(&R, chg.body, "    bh_timehook_Reheap($parname.th_$name); // ($field) TimeHookRecur");
            Ins(&R, chg.body, "} else {");
            Ins(&R, chg.body, "    bh_timehook_Remove($parname.th_$name);");
            Ins(&R, chg.body, "}");
        } else if (is_extern) {
            Ins(&R, chg.comment, "Forward-declaration for user-provided function.");
        } else if (fstep.steptype == dmmeta_Steptype_steptype_Inline) {
            // inline -- do nothing
            chg.priv = true;
        }
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Step_SetDelay() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;
    if (fstep.steptype == dmmeta_Steptype_steptype_InlineRecur) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Set inter-step delay to specified value.");
        Ins(&R, func.comment, "The difference between new delay and current delay is added to the next scheduled time.");
        Ins(&R, func.ret  , "void", false);
        Ins(&R, func.proto, "$name_SetDelay($Parent)", false);
        AddProtoArg(func, "algo::SchedTime", "delay");
        Ins(&R, func.body, "i64 diff = delay.value - $ns::_db.$name_delay.value;");
        Ins(&R, func.body, "$ns::_db.$name_delay = delay;");
        Ins(&R, func.body, "if (diff > 0) {");
        Ins(&R, func.body, "    $ns::_db.$name_next.value += diff;");
        Ins(&R, func.body, "} else {");
        Ins(&R, func.body, "    $ns::_db.$name_next.value = algo::u64_SubClip($ns::_db.$name_next.value,-diff);");
        Ins(&R, func.body, "}");
    }
}
