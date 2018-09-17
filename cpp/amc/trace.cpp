// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/trace.cpp -- Generate trace code
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void Main_Trace_Field(strptr field_key, strptr comment) {
    dmmeta::Field field;
    field.field         = field_key;
    field.arg           = "u64";
    field.reftype       = dmmeta_Reftype_reftype_Val;
    field.comment.value = comment;
    amc::InsField(field);
    amc::tracefld_InsertMaybe(dmmeta::Tracefld(field_key, field.comment));
}

// -----------------------------------------------------------------------------

void amc::gen_trace() {
    algo_lib::Replscope R;

    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ind_ctype_Find(tempstr() << ns.ns<<".FDb") != NULL) {
        tempstr ctype_key = tempstr() <<ns.ns << ".trace";
        tempstr field_key = tempstr() <<ns.ns << ".FDb.trace";
        amc::tracerec_InsertMaybe(dmmeta::Tracerec(ctype_key, algo::Comment()));
        amc::ctype_InsertMaybe(dmmeta::Ctype(ctype_key, algo::Comment()));
        amc::cfmt_InsertMaybe(dmmeta::Cfmt(tempstr() << ctype_key << "." << dmmeta_Strfmt_strfmt_String
                                           , dmmeta_Printfmt_printfmt_Tuple
                                           , false
                                           , true
                                           , ""
                                           , true // genop
                                           , algo::Comment()));
        amc::pack_InsertMaybe(dmmeta::Pack(ctype_key, algo::Comment()));
        amc::FField &field=*InsField(dmmeta::Field(field_key, ctype_key, dmmeta_Reftype_reftype_Val, dmmeta::CppExpr(), algo::Comment()));
        field.reflect=true;// look it up
    }ind_end;

    // user tracefld -- copy
    ind_beg(amc::_db_usertracefld_curs,usertracefld,_db) {
        Main_Trace_Field(usertracefld.tracefld, usertracefld.comment);
    }ind_end;

    // step/fstep
    ind_beg(amc::_db_fstep_curs,fstep,amc::_db) {
        if (fstep.p_field->c_ftrace) {
            Set(R,"$ns"  ,ns_Get(fstep));
            Set(R,"$stepname",name_Get(fstep));
            Main_Trace_Field(Subst(R,"$ns.trace.step_$stepname"), fstep.comment);
            Main_Trace_Field(Subst(R,"$ns.trace.step_$stepname_cycles"), fstep.comment);
        }
    }ind_end;

    // dispatch/dispatchmsg
    ind_beg(amc::_db_dispatch_curs,dispatch,amc::_db) if (dispatch.c_disptrace && c_dispatch_msg_N(dispatch) && dispatch.p_ns->c_globfld) {
        Set(R,"$ns", ns_Get(dispatch));
        Set(R,"$Dname", name_Get(dispatch));
        // counter fields
        ind_beg(amc::dispatch_c_dispatch_msg_curs,dispatch_msg,dispatch) {
            Set(R,"$Msgname",name_Get(*dispatch_msg.p_ctype));
            Main_Trace_Field(Subst(R,"$ns.trace.dispatch_$Dname_$Msgname"), algo::Comment(Subst(R,"Total number of $Msgname processed by $ns")));
            if (dispatch.c_disptrace && dispatch.c_disptrace->cycle) {
                Main_Trace_Field(Subst(R,"$ns.trace.dispatch_$Dname_$Msgname_cycles"), algo::Comment());
            }
        }ind_end;
        Main_Trace_Field(Subst(R,"$ns.trace.dispatch_$Dname_Unkmsg"), algo::Comment());
    }ind_end;

    // alloc
    // Inlary / Tary -- only trace at global level
    ind_beg(amc::_db_field_curs,field,_db) {
        amc::FCtype &parent = *field.p_ctype;
        bool has_alloc = HasAllocQ(field);
        bool regular_trace = has_alloc
            && parent.p_ns->c_globfld
            && field.c_ftrace;
        // Add tracefld for fbuf out egain
        bool fbuf_trace = field.c_fbuf
            && fbufdir_Get(*field.c_fbuf) == dmmeta_Fbufdir_fbufdir_out
            && field.c_ftrace;
        if (regular_trace) {
            Set(R,"$ns"      , ns_Get(parent));
            Set(R,"$partrace", Refname(*field.p_ctype));
            Set(R,"$name"    , name_Get(field));
            Main_Trace_Field(Subst(R,"$ns.trace.alloc_$partrace_$name"), field.p_arg->comment);
            Main_Trace_Field(Subst(R,"$ns.trace.del_$partrace_$name"), field.p_arg->comment);
        }
        if (fbuf_trace) {
            Set(R,"$ns"      , ns_Get(parent));
            Set(R,"$partrace", Refname(*field.p_ctype));
            Set(R,"$name"    , name_Get(field));
            Main_Trace_Field(Subst(R,"$ns.trace.$partrace_$name_n_eagain"), field.p_arg->comment);
        }
        // mark field as being traced
        field.do_trace = regular_trace || fbuf_trace;
    }ind_end;

    ind_beg(amc::_db_ns_curs,ns,_db) if (ns.c_globfld) {
        Set(R,"$ns"      , ns.ns);
    }ind_end;
}
