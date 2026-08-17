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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/fcond.cpp
//
// Tests for dmmeta.fcond: list membership tied to a field's value,
// maintained by the field's generated _Set (see txt/amc/fcond.md).
// Fixture: FCondtest.state (fconst idle/run/done) mirrors state==idle on
// zd_condtest_idle (the default value, so membership starts at birth),
// state==run on zd_condtest_run (ins:Y del:Y), and state==done on
// zd_condpar_done through p_parent; FCondtest.ready arms
// cd_condtest_ready on the rising edge only (ins:Y del:N).

#include "include/algo.h"
#include "include/atf_amc.h"

// XrefMaybe performs the initial-membership dispatch: a record whose
// field holds a watched value at xref time enters the row's list, so a
// record born in a registered state needs no manual arm.  A value set
// before XrefMaybe counts the same way (the setter already dispatched;
// the xref-time insert is an idempotent no-op).
void atf_amc::amctest_FcondInit() {
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(!atf_amc::zd_condtest_idle_InLlistQ(condtest));// not yet x-referenced
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    vrfy_(atf_amc::zd_condtest_idle_InLlistQ(condtest));// born idle: the mirror holds from birth
    vrfy_(!atf_amc::zd_condtest_run_InLlistQ(condtest));
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_run);
    vrfy_(!atf_amc::zd_condtest_idle_InLlistQ(condtest));
    vrfy_(atf_amc::zd_condtest_run_InLlistQ(condtest));
    atf_amc::condtest_Delete(condtest);
    // a record armed by the creator before XrefMaybe stays armed after it
    atf_amc::FCondtest &condtest2 = atf_amc::condtest_Alloc();
    atf_amc::state_Set(condtest2, atf_amc_FCondtest_state_run);
    vrfy_(atf_amc::condtest_XrefMaybe(condtest2));
    vrfy_(!atf_amc::zd_condtest_idle_InLlistQ(condtest2));
    vrfy_(atf_amc::zd_condtest_run_InLlistQ(condtest2));
    vrfy_(atf_amc::zd_condtest_run_N() == 1);
    atf_amc::condtest_Delete(condtest2);
}

// A mirror row (ins:Y del:Y) keeps membership equal to field==value:
// entering the value inserts, a repeated Set of the same value is a
// no-op, leaving the value removes, and deleting a member record
// unlinks it.
void atf_amc::amctest_FcondMirror() {
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    vrfy_(!atf_amc::zd_condtest_run_InLlistQ(condtest));
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_run);
    vrfy_(atf_amc::zd_condtest_run_InLlistQ(condtest));
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_run);// same value: level unchanged
    vrfy_(atf_amc::zd_condtest_run_InLlistQ(condtest));
    vrfy_(atf_amc::zd_condtest_run_N() == 1);
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_idle);
    vrfy_(!atf_amc::zd_condtest_run_InLlistQ(condtest));
    // delete unlinks a member: the pool's dtor removes from every index
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_run);
    vrfy_(atf_amc::zd_condtest_run_N() == 1);
    atf_amc::condtest_Delete(condtest);
    vrfy_(atf_amc::zd_condtest_run_N() == 0);
}

// A queue row (ins:Y del:N) inserts on the rising edge only: the falling
// edge leaves membership alone (the queue's consumer is the remover),
// and a Set that does not change the level never re-inserts.
void atf_amc::amctest_FcondQueue() {
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    atf_amc::ready_Set(condtest, true);
    vrfy_(atf_amc::cd_condtest_ready_InLlistQ(condtest));
    atf_amc::ready_Set(condtest, false);// del:N -- falling edge leaves the queue alone
    vrfy_(atf_amc::cd_condtest_ready_InLlistQ(condtest));
    atf_amc::cd_condtest_ready_Remove(condtest);// the consumer drains
    atf_amc::ready_Set(condtest, false);// same value: no level change, no insert
    vrfy_(!atf_amc::cd_condtest_ready_InLlistQ(condtest));
    atf_amc::ready_Set(condtest, true);// next rising edge re-arms
    vrfy_(atf_amc::cd_condtest_ready_InLlistQ(condtest));
    atf_amc::condtest_Delete(condtest);
}

// ReadStrptrMaybe stores through the generated Set for every spelling of
// the value: the symbolic path ('run') and the numeric fallback ('0')
// both dispatch fcond membership, so how the input spells the value
// cannot make the record's list state diverge from its field.
void atf_amc::amctest_FcondRead() {
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    vrfy_(atf_amc::state_ReadStrptrMaybe(condtest, "run"));// symbolic spelling
    vrfy_(atf_amc::zd_condtest_run_InLlistQ(condtest));
    vrfy_(!atf_amc::zd_condtest_idle_InLlistQ(condtest));
    vrfy_(atf_amc::state_ReadStrptrMaybe(condtest, "0"));// numeric spelling of idle
    vrfy_(atf_amc::zd_condtest_idle_InLlistQ(condtest));
    vrfy_(!atf_amc::zd_condtest_run_InLlistQ(condtest));
    atf_amc::condtest_Delete(condtest);
}

// A via row operates on the pointed-to record: the parent enters the
// list when the child's field becomes the value, leaves when it stops
// being the value, and a NULL pointer skips the operation.
void atf_amc::amctest_FcondVia() {
    atf_amc::FCondpar &condpar = atf_amc::condpar_Alloc();
    vrfy_(atf_amc::condpar_XrefMaybe(condpar));
    atf_amc::FCondtest &condtest = atf_amc::condtest_Alloc();
    vrfy_(atf_amc::condtest_XrefMaybe(condtest));
    // NULL parent: the transition performs no list operation
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_done);
    vrfy_(atf_amc::zd_condpar_done_N() == 0);
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_idle);
    condtest.p_parent = &condpar;
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_done);
    vrfy_(atf_amc::zd_condpar_done_InLlistQ(condpar));
    atf_amc::state_Set(condtest, atf_amc_FCondtest_state_idle);
    vrfy_(!atf_amc::zd_condpar_done_InLlistQ(condpar));
    atf_amc::condtest_Delete(condtest);
    atf_amc::condpar_Delete(condpar);
}
