// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/step.cpp -- Step functions
//
// A step is a user-implemented function that gets called periodically from the amc-generated MainLoop.
// With ftrace, the number of invocations and total clock cycles spent in the user function will be recorded.
// A step is associated with a field. It must be a global field in FDb.
// If you want to associate a step with a record, you have to first place that record in some suitable index,
// such as a linked list, or a binary heap (two most common cases). Circular linked lists are very convenient
// because you can call cd_mylist_RotateFirst() inside the step function and thus go over all records eventually.
// Amc main loop switches smoothly between "blocking" and "hot-polling" modes and in between. All file descriptors
// are added to an epoll object, and waiting on epoll is a step. The amount of sleep we allow epoll is a function
// of how soon any of the steps will need cpu time. This is controlled by algo_lib::_db.next_loop variable.
// At the beginning of the scheduling cycle, next_loop is set to infinity. Each step has a next_loop variable
// associated with it, which lowers the global value. At the end, all the time until next_loop is given up to the OS.
// A step is declared on a global field whose reftype offers a loop condition: an index with an
// emptiness test (Atree, Bheap, Blkhash, Lary, Llist, Ptrary, Tary, Thash, or a variable Inlary),
// a value read directly (Val of type bool, Ptr, Upptr), ZSListMT (tested through DestructiveFirst,
// steptype Inline or InlineRecur only), or Global (unconditional).
// The step function is called if the index is non-empty, or the bool value is true.
// With the simplest step type Inline, the function is called on every scheduler cycle until the index becomes empty
// or the controlling variable becomes false.
// With the InlineOnce step, a field from the record that's part of the step list is designated as the expiration time.
// At every scheduling step, the scheduler looks at the expiration field; if algo_lib::_db.clock has advanced past the
// expiration time, the step is called.
// (When the user function is called, it is guaranteed the index is non-empty, and the expiration condition is met.
// The user must manually dequeue / remove element from the index to avoid hot polling.)
// With the InlineRecur step, instead of controlling the specific time when callback occurs, the user controls delay
// between steps. Each next step is called after a fixed delay with respect to the previous step.
// Delay is controlled with dmmeta.fdelay record. With fdelay and scale:Y you can go through the entire list of
// records in 1 unit of time, at uniform intervals. This is useful for sending heartbeats etc.
// TimeHookRecur step type is like InlineRecur, but callback to the step function occurs through a TimeHook, which adds tiny overhead to
// scheduling/descheduling, and some scheduling non-determinism, but in return this doesn't waste any precious scheduler
// cycles on every loop. The hook is armed by the index's FirstChanged calls, which only the Llist and
// Bheap generators emit, so a TimeHookRecur step field must be an Llist or a Bheap.
// TimeHookOnce is similar to InlineOnce - a global TimeHook is created which is responsible for calling the
// step function; the time hook is scheduled for the time given by the first element of the fstep's index
// (usually a Bheap).
// Other (auxiliary) step types are Callback and Extern:
// Callback: the function is simply called on every scheduler cycle. This doesn't cause hot polling, next_loop is not updated.
// Extern: the _FirstChanged function is marked extern and also implemented by the user
// A library emits no step function of its own.  An executable's Steps() calls every direct step of
// every namespace it links, in dependency order, so the whole main loop is written out in one place.
// A step may also be declared on another namespace's list.  The step field is then an Alias on the
// stepping namespace's FDb whose srcfield is the other namespace's global list, and the generated step
// tests that list for pending work and calls the stepping namespace's own $name_Step.  When the list
// has a step of its own, the alias step overrides it: Steps() calls the alias step in the library
// step's slot and never calls the library's; the overriding step may call lib::$name_Step() itself.
// Only Inline, InlineRecur and Callback apply to an alias step, because a TimeHook step is armed by
// the list's FirstChanged, which is generated in the list's own namespace, and for the same reason a
// TimeHook step cannot be overridden.  At most one alias step per list may share a process.

#include "include/amc.h"

// True if FSTEP sits in the idle band: it runs in a pass that has time to give
// up, as well as when its rate allows.
static bool IdleStepQ(amc::FFstep &fstep) {
    return fstep.stepband == amc::amcdb_stepband_idle;
}

// True if the calls of FSTEP are counted against a rate: it has an fsteprate
// row, or it is an idle Inline step, whose rate defaults to one call in eight
// passes.
static bool RatedStepQ(amc::FFstep &fstep) {
    return fstep.c_fsteprate != NULL || (IdleStepQ(fstep) && fstep.steptype == dmmeta_Steptype_steptype_Inline);
}

// True if FSTEP is called from a time hook rather than from Steps().
static bool TimehookStepQ(amc::FFstep &fstep) {
    return fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur
        || fstep.steptype == dmmeta_Steptype_steptype_TimeHookOnce;
}

// Name of the algo_lib heap that holds the time hook of FSTEP.  algo_lib
// keeps one heap per band a time hook may fire in: bh_timehook, stepped in
// the work band, and bh_timehook_idle, stepped in the idle band.
static tempstr TimehookHeap(amc::FFstep &fstep) {
    return tempstr() << (IdleStepQ(fstep) ? "bh_timehook_idle" : "bh_timehook");
}

// Check the fstep on alias FIELD, which steps another namespace's list: the
// list is a global field of another namespace, a step of its own that this
// one overrides runs from Steps() and shares the override's band, and the
// steptype needs nothing from the list's namespace.  Two alias steps on one list within one process are
// refused where the process's Steps() is generated.  Each defect is reported
// as a generation error
static void CheckAliasStep(amc::FField &field) {
    amc::FFstep &fstep = *field.c_fstep;
    amc::FField &list = *field.c_falias->p_srcfield;
    if (!amc::GlobalQ(*list.p_ctype) || list.p_ctype->p_ns == field.p_ctype->p_ns) {
        prerr("amc.fstep_alias_list"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("srcfield",list.field)
              <<Keyval("comment","an alias step names a global field of another namespace"));
        algo_lib::_db.exit_code++;
    }
    if (list.c_fstep && !amc::DirectStepQ(*list.c_fstep)) {
        prerr("amc.fstep_alias_timehook"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("srcfield",list.field)
              <<Keyval("comment","the list's own step runs from a time hook, not from Steps(), so it cannot be overridden"));
        algo_lib::_db.exit_code++;
    }
    if (list.c_fstep && fstep.p_stepband != list.c_fstep->p_stepband) {
        prerr("amc.fstep_alias_band"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("stepband",fstep.stepband)
              <<Keyval("slot",list.c_fstep->stepband)
              <<Keyval("comment","an override runs in the slot of the step it replaces, so it declares that step's band"));
        algo_lib::_db.exit_code++;
    }
    if (fstep.steptype != dmmeta_Steptype_steptype_Inline
        && fstep.steptype != dmmeta_Steptype_steptype_InlineRecur
        && fstep.steptype != dmmeta_Steptype_steptype_Callback) {
        prerr("amc.fstep_alias_steptype"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("comment","an alias step is Inline, InlineRecur or Callback"));
        algo_lib::_db.exit_code++;
    }
}

// Validate the fstep against the contract stated at the top of this file
// and add the step's state fields (next/delay for InlineRecur, the time
// hook for TimeHook steps); every schema shape the Step tfuncs cannot
// serve is reported here as a generation error, and the run continues so
// one pass names every defect -- the error count withholds all output
void amc::tclass_Step() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;
    // Every generated step accesses its field through the namespace global
    // ($ns::_db.$name), so an fstep on a field of a non-global ctype names a
    // member the FDb struct does not have; enforce the FDb-field contract
    // stated at the top of this file
    if (!GlobalQ(*field.p_ctype)) {
        prerr("amc.fstep_global"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("ctype",field.p_ctype->ctype)
              <<Keyval("comment","fstep requires a global field (a field of the namespace FDb)"));
        algo_lib::_db.exit_code++;
    }
    // An alias step tests another namespace's list; every check below that
    // reads the step field's shape reads the list's
    amc::FField &list = ListAliasQ(field) ? *field.c_falias->p_srcfield : field;
    if (ListAliasQ(field)) {
        CheckAliasStep(field);
    }
    // The step's loop condition tests the field for pending work
    // (GetStepCond): an index is tested with its EmptyQ, a Val/Ptr/Upptr
    // with its value, ZSListMT with DestructiveFirst. A reftype outside
    // this set -- Count, Fbuf, Smallstr, a memory pool -- has no such
    // test, and the generated condition calls an EmptyQ that does not
    // exist; reject the schema instead of shipping the compile error.
    // An Inlary counts only when variable: a fixed Inlary always holds
    // max elements and generates no EmptyQ.
    bool steppable = list.reftype == dmmeta_Reftype_reftype_Val
        || list.reftype == dmmeta_Reftype_reftype_Ptr
        || list.reftype == dmmeta_Reftype_reftype_Upptr
        || list.reftype == dmmeta_Reftype_reftype_Global
        || list.reftype == dmmeta_Reftype_reftype_ZSListMT
        || (list.reftype == dmmeta_Reftype_reftype_Inlary && !amc::FixaryQ(list))
        || list.reftype == dmmeta_Reftype_reftype_Atree
        || list.reftype == dmmeta_Reftype_reftype_Bheap
        || list.reftype == dmmeta_Reftype_reftype_Blkhash
        || list.reftype == dmmeta_Reftype_reftype_Lary
        || list.reftype == dmmeta_Reftype_reftype_Llist
        || list.reftype == dmmeta_Reftype_reftype_Ptrary
        || list.reftype == dmmeta_Reftype_reftype_Tary
        || list.reftype == dmmeta_Reftype_reftype_Thash;
    if (!steppable) {
        prerr("amc.fstep_reftype"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("reftype",list.reftype)
              <<Keyval("comment","fstep needs an emptiness test for the loop condition; use an index with EmptyQ (for Inlary: min<max), a Val/Ptr/Upptr, or ZSListMT"));
        algo_lib::_db.exit_code++;
    }
    // InlineOnce and TimeHookOnce both read the expiration time from the first row's
    // sort field, which only a Bheap step field provides; checked here in the tclass
    // function so the rejection precedes every Step tfunc
    vrfy(!(fstep.steptype == dmmeta_Steptype_steptype_InlineOnce || fstep.steptype == dmmeta_Steptype_steptype_TimeHookOnce)
         || list.reftype == dmmeta_Reftype_reftype_Bheap
         , tempstr()<<"amc.fstep_bheap"
         <<Keyval("fstep",fstep.fstep)
         <<Keyval("steptype",fstep.steptype)
         <<Keyval("reftype",list.reftype)
         <<Keyval("comment","steptype InlineOnce/TimeHookOnce requires the step field to be a Bheap"));
    // TimeHookRecur arms and disarms its time hook from the index's first row:
    // $name_FirstChanged reheaps or removes the hook, and only the Llist and
    // Bheap generators call $name_FirstChanged from their inserts and removes.
    // On a step field of any other shape nothing ever arms the hook, and the
    // step compiles but never fires
    if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur
        && list.reftype != dmmeta_Reftype_reftype_Llist
        && list.reftype != dmmeta_Reftype_reftype_Bheap) {
        prerr("amc.fstep_first"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("reftype",list.reftype)
              <<Keyval("comment","steptype TimeHookRecur requires an Llist or Bheap step field"));
        algo_lib::_db.exit_code++;
    }
    // ZSListMT has no EmptyQ (the list is concurrent); its loop condition
    // tests DestructiveFirst, which only the Inline and InlineRecur call
    // shapes embed
    if (list.reftype == dmmeta_Reftype_reftype_ZSListMT
        && fstep.steptype != dmmeta_Steptype_steptype_Inline
        && fstep.steptype != dmmeta_Steptype_steptype_InlineRecur) {
        prerr("amc.fstep_zslistmt"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("comment","a ZSListMT step field requires steptype Inline or InlineRecur"));
        algo_lib::_db.exit_code++;
    }
    // fdelay names the delay between invocations, which only the InlineRecur
    // and TimeHookRecur call shapes read; on any other steptype the row
    // configures nothing
    if (fstep.c_fdelay
        && fstep.steptype != dmmeta_Steptype_steptype_InlineRecur
        && fstep.steptype != dmmeta_Steptype_steptype_TimeHookRecur) {
        prerr("amc.fstep_fdelay"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("comment","fdelay applies only to steptype InlineRecur and TimeHookRecur"));
        algo_lib::_db.exit_code++;
    }
    // fdelay scale:Y divides the delay by the number of rows in the step
    // index ($name_N), spreading a full sweep over one delay unit; only the
    // InlineRecur call shape computes the scaled delay, and on a step field
    // with no row count the generated code calls an N function that does
    // not exist. The countable set is every step-field reftype whose N
    // function amc generates unconditionally, plus Llist, whose N exists
    // only with havecount.
    bool countable = list.reftype == dmmeta_Reftype_reftype_Bheap
        || list.reftype == dmmeta_Reftype_reftype_Blkhash
        || list.reftype == dmmeta_Reftype_reftype_Thash
        || list.reftype == dmmeta_Reftype_reftype_Tary
        || list.reftype == dmmeta_Reftype_reftype_Lary
        || list.reftype == dmmeta_Reftype_reftype_Ptrary
        || list.reftype == dmmeta_Reftype_reftype_Inlary
        || (list.reftype == dmmeta_Reftype_reftype_Llist && list.c_llist && list.c_llist->havecount);
    if (fstep.c_fdelay && fstep.c_fdelay->scale
        && !(fstep.steptype == dmmeta_Steptype_steptype_InlineRecur && countable)) {
        prerr("amc.fstep_scale"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("reftype",list.reftype)
              <<Keyval("comment","fdelay scale:Y requires steptype InlineRecur and a counted step field (Bheap, Blkhash, Thash, Tary, Lary, Ptrary, Inlary, or Llist with havecount)"));
        algo_lib::_db.exit_code++;
    }
    // A rate counts calls per pass, which belongs to a step called on every
    // pass it has work: the Inline shape.  The other steptypes keep their own
    // clocks, and in the idle band they only run late in the pass.
    if (fstep.c_fsteprate && fstep.steptype != dmmeta_Steptype_steptype_Inline) {
        prerr("amc.fstep_rate_steptype"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("comment","an fsteprate row applies to steptype Inline"));
        algo_lib::_db.exit_code++;
    }
    // A time hook step fires from the algo_lib heap of its band, and algo_lib
    // has a heap for the work band and one for the idle band.
    if (TimehookStepQ(fstep) && !(fstep.stepband == amc::amcdb_stepband_work) && !IdleStepQ(fstep)) {
        prerr("amc.fstep_timehook_band"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("steptype",fstep.steptype)
              <<Keyval("stepband",fstep.stepband)
              <<Keyval("comment","a time hook step fires in stepband work or idle"));
        algo_lib::_db.exit_code++;
    }
    if (fstep.c_fsteprate && (fstep.c_fsteprate->ncall == 0 || fstep.c_fsteprate->npass == 0)) {
        prerr("amc.fstep_rate_zero"
              <<Keyval("fstep",fstep.fstep)
              <<Keyval("comment","an fsteprate names at least one call over at least one pass"));
        algo_lib::_db.exit_code++;
    }
    if (RatedStepQ(fstep)) {
        InsVar(R, field.p_ctype, "u32", "$name_credit", "", "$field \tCalls the step's rate has earned and not spent");
    }
    if (fstep.steptype == dmmeta_Steptype_steptype_InlineRecur) {
        InsVar(R, field.p_ctype, "algo::SchedTime", "$name_next", "", "$field \tNext invocation time");
        InsVar(R, field.p_ctype, "algo::SchedTime", "$name_delay", "", "$field \tDelay between invocations");
    }
    if (TimehookStepQ(fstep)) {
        InsVar(R, field.p_ctype, "algo_lib::FTimehook", "th_$name", "", "$field \tfstep time hook for $field");
    }
}

void amc::tfunc_Step_UpdateCycles() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;
    amc::FFunc& func = amc::CreateCurFunc();
    // an executable's Steps() calls a library's steps, so a library's are public
    bool exe = amc::ExeQ(*field.p_ctype->p_ns);
    func.priv        = !amc::ExternStepQ(fstep) && exe;
    func.inl = amc::DirectStepQ(fstep) && !amc::ExternStepQ(fstep);
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
    step.acrkey <<"fstep:"<<amc::_db.genctx.p_field->field;
    step.extrn=true;
}

// -----------------------------------------------------------------------------

// Generate the step's Init statements: the delay variable for
// InlineRecur, the time-hook setup (and its delay) for the TimeHook
// steptypes
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
            Set(R, "$delay", tempstr()<< fstep.c_fdelay->delay);
            Ins(&R, init.body, "$ns::_db.th_$name.delay = algo::ToSchedTime($delay); // initialize fstep delay ($field)");
        }
    } else if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookOnce) {
        Ins(&R, init.body, "// initialize fstep timehook ($field)");
        Ins(&R, init.body, "hook_Set0($parname.th_$name, $ns::$name_Call);");
    }
}

// -----------------------------------------------------------------------------

// Return the loop condition of the step on FIELD: the expression that is
// true while the stepped list holds pending work.  For an alias step the
// expression tests the other namespace's list
static tempstr GetStepCond(amc::FField &field) {
    amc::FField &list = amc::ListAliasQ(field) ? *field.c_falias->p_srcfield : field;
    tempstr ref = tempstr() << list.p_ctype->p_ns->ns << "::" << name_Get(list);
    tempstr ret;
    // special work-around for ZSListMT -- EmptyQ  cannot be defined, DestructiveFirst must be used.
    if (list.reftype == dmmeta_Reftype_reftype_ZSListMT) {
        ret = tempstr() << ref << "_DestructiveFirst() != NULL";
    } else if (list.reftype == dmmeta_Reftype_reftype_Inlary) {
        ret = tempstr() << "!" << ref << "_EmptyQ()";
    } else if (ValQ(list)
               || list.reftype == dmmeta_Reftype_reftype_Ptr
               || list.reftype == dmmeta_Reftype_reftype_Upptr) {
        ret = tempstr() << list.p_ctype->p_ns->ns << "::_db." << name_Get(list);
    } else if (list.reftype == dmmeta_Reftype_reftype_Global) {
        ret= "true";
    } else {
        ret = tempstr() << "!" << ref << "_EmptyQ()";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Generate $name_Call: invoke $name_Step on the steptype's schedule --
// delay-gated for InlineRecur, expiration-driven off the first row's
// sort field for the Once steptypes, every pass for Inline, bare for
// the hook- and caller-driven steptypes
void amc::tfunc_Step_Call() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFstep &fstep = *field.c_fstep;

    amc::FFunc& call = amc::CreateCurFunc();
    Ins(&R, call.proto  , "$name_Call()",false);
    Ins(&R, call.ret  , "void",false);
    // an alias step tests another namespace's list, whose header a library's
    // inline header may not see, so its call wrapper stays out of line
    call.inl = amc::DirectStepQ(fstep) && !ListAliasQ(field);
    call.priv = !amc::ExternStepQ(fstep) && amc::ExeQ(*field.p_ctype->p_ns);
    Set(R, "$LoopCond", GetStepCond(field));

    if (fstep.steptype == dmmeta_Steptype_steptype_InlineRecur) {
        if (fstep.c_fdelay) {
            Set(R, "$delay", tempstr()<< value_GetDouble(fstep.c_fdelay->delay));
        }

        Ins(&R, call.body, "if ($LoopCond) { // fstep:$field");
        Ins(&R, call.body, "    if ($ns::_db.$name_next < algo_lib::_db.clock) {");
        if (fstep.c_fdelay && fstep.c_fdelay->scale) {
            amc::FField &list = ListAliasQ(field) ? *field.c_falias->p_srcfield : field;
            Set(R, "$listN", tempstr() << list.p_ctype->p_ns->ns << "::" << name_Get(list) << "_N()");
            Ins(&R, call.body, "        u64 effective_delay = $ns::_db.$name_delay / u64_Max(1,$listN);");
            Ins(&R, call.body, "        $ns::_db.$name_next = algo_lib::_db.clock + algo::SchedTime(effective_delay);");
        } else {
            Ins(&R, call.body, "        $ns::_db.$name_next = algo_lib::_db.clock + $ns::_db.$name_delay;");
        }
        Ins(&R, call.body, "        $ns::$name_Step(); // steptype:InlineRecur: call function every N clock cycles");
        Ins(&R, call.body, "        $name_UpdateCycles();");
        Ins(&R, call.body, "    }");
        Ins(&R, call.body, "    algo_lib::_db.next_loop.value = u64_Min($ns::_db.$name_next, algo_lib::_db.next_loop);");
        Ins(&R, call.body, "}");
    } else if (fstep.steptype == dmmeta_Steptype_steptype_InlineOnce || fstep.steptype == dmmeta_Steptype_steptype_TimeHookOnce) {
        Set(R, "$sortval" , FieldvalExpr(field.p_arg, *field.c_sortfld->p_sortfld, "(*$name)"));
        Set(R, "$Cpptype"  , field.p_arg->cpp_type);
        Ins(&R, call.body, "// Call Step for all entries expired by this time.");
        Ins(&R, call.body, "// (_db.clock may get updated during this loop, but only those entries");
        Ins(&R, call.body, "// that expired prior will be processed.)");
        Ins(&R, call.body, "algo_lib::_db.step_limit = algo_lib::_db.clock;");
        Ins(&R, call.body, "while ($Cpptype *$name = $ns::$name_First()) { // fstep:$field");
        Ins(&R, call.body, "    algo::SchedTime expire = $sortval;");
        Ins(&R, call.body, "    if (expire < algo_lib::_db.step_limit) {");
        Ins(&R, call.body, "        $ns::$name_Step(); // steptype:InlineOnce: call function at specified time");
        Ins(&R, call.body, "        $name_UpdateCycles();");
        if (fstep.steptype == dmmeta_Steptype_steptype_InlineOnce) {
            Ins(&R, call.body, "    algo_lib::_db.next_loop.value = algo_lib::_db.step_limit;");
        }
        Ins(&R, call.body, "    } else {");
        if (fstep.steptype == dmmeta_Steptype_steptype_InlineOnce) {
            Ins(&R, call.body, "    algo_lib::_db.next_loop.value = u64_Min(expire, algo_lib::_db.next_loop);");
        }
        Ins(&R, call.body, "        break;");
        Ins(&R, call.body, "    }");
        Ins(&R, call.body, "}");
    } else if (fstep.steptype == dmmeta_Steptype_steptype_Inline && RatedStepQ(fstep)) {
        // A rate NCALL/NPASS is a for-loop whose limit may be a fraction: each
        // pass earns NCALL credit and each call spends NPASS.  A pass that has
        // spent what it could keeps less than NPASS, so it holds at most
        // NCALL+NPASS-1 after earning; capping there loses no earned credit
        // while the step has work, and an empty stretch banks less than one
        // call.  A cap below that drops credit at 3/2 or 2/3.  An idle step is
        // also called once in a pass that earned it no call, when the pass has
        // time to give up.  With no fsteprate row the step is idle, and its
        // rate is 1/8: a busy loop gives background work an eighth of its
        // passes, and an idle one gives it every pass.
        u32 ncall = fstep.c_fsteprate ? fstep.c_fsteprate->ncall : 1;
        u32 npass = fstep.c_fsteprate ? fstep.c_fsteprate->npass : 8;
        Set(R, "$ncall", tempstr() << ncall);
        Set(R, "$npass", tempstr() << npass);
        Set(R, "$ncap", tempstr() << (ncall + npass - 1));
        Ins(&R, call.body, "$ns::_db.$name_credit = u32_Min($ns::_db.$name_credit + $ncall, $ncap); // fstep:$field  rate:$ncall/$npass");
        Ins(&R, call.body, "bool called = false;");
        Ins(&R, call.body, "while ($LoopCond && $ns::_db.$name_credit >= $npass) {");
        Ins(&R, call.body, "    $ns::_db.$name_credit -= $npass;");
        Ins(&R, call.body, "    $ns::$name_Step();");
        Ins(&R, call.body, "    $name_UpdateCycles();");
        Ins(&R, call.body, "    called = true;");
        Ins(&R, call.body, "}");
        if (IdleStepQ(fstep)) {
            Ins(&R, call.body, "if (!called && $LoopCond && algo_lib::_db.next_loop > algo_lib::_db.clock) {");
            Ins(&R, call.body, "    $ns::$name_Step(); // stepband:idle: the pass has time to give up");
            Ins(&R, call.body, "    $name_UpdateCycles();");
            Ins(&R, call.body, "}");
        } else {
            Ins(&R, call.body, "(void)called;");
        }
        Ins(&R, call.body, "if ($LoopCond) {");
        Ins(&R, call.body, "    algo_lib::_db.next_loop = algo_lib::_db.clock;");
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
        Set(R, "$heap", TimehookHeap(fstep));
        if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookRecur) {
            chg.priv = true;
            Ins(&R, chg.body, "$Ctype* row = $name_First($pararg);");
            Ins(&R, chg.comment, "If index $name is empty, deschedule time hook $parname.th_$name.");
            Ins(&R, chg.comment, "If index is non-empty, and time hook is not scheduled,");
            Ins(&R, chg.comment, "    schedule it after $parname.th_$name.delay clocks.");
            Ins(&R, chg.comment, "If index is non-empty, and time hook is already scheduled, do nothing");
            Ins(&R, chg.body, "if (row) {");
            Ins(&R, chg.body, "    $heap_Reheap($parname.th_$name); // ($field) TimeHookRecur");
            Ins(&R, chg.body, "} else {");
            Ins(&R, chg.body, "    $heap_Remove($parname.th_$name);");
            Ins(&R, chg.body, "}");
        } else if (fstep.steptype == dmmeta_Steptype_steptype_TimeHookOnce) {
            chg.priv = true;
            Ins(&R, chg.body, "$Ctype* row = $name_First($pararg);");
            Ins(&R, chg.comment, "If index $name is empty, deschedule time hook $parname.th_$name.");
            Ins(&R, chg.comment, "If index is non-empty, update time hook to fire at specified time.");
            Ins(&R, chg.body, "if (row) {");
            Ins(&R, chg.body, "    $parname.th_$name.time = row->$sortfld;");
            Ins(&R, chg.body, "    $heap_Reheap($parname.th_$name); // ($field) TimeHookOnce");
            Ins(&R, chg.body, "} else {");
            Ins(&R, chg.body, "    $heap_Remove($parname.th_$name);");
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
