// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/io.cpp -- I/O functions
//
// AMC reflection.

#include "include/amc.h"

void amc::tfunc_Io_InputMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.c_finput) {
        vrfy(!field.c_gstatic
             , tempstr()<<"amc.too_much_gstatic"
             <<Keyval("field",field.field)
             <<Keyval("comment","Finput and gstatic cannot be specified together"));
        amc::FFinput &finput = *field.c_finput;
        bool can_read      = GlobalQ(*field.p_ctype);
        amc::FCtype *basetype  = GetBaseType(*field.p_arg,field.p_arg);// always read base class if possible
        Set(R, "$Basetype", basetype->cpp_type);
        amc::FFunc& input = amc::CreateCurFunc();
        Ins(&R, input.ret, "bool", false);
        Ins(&R, input.proto, "$name_InputMaybe($Parent, $Basetype &elem)", false);
        // extern + nsx -> inputmaybe implemented by user
        // extern + exceptions -> inputmaybe calls Input, Input implemented by user
        // nonextern -> inputmaybe calls InsertMaybe
        if (finput.extrn) {
            if (!GenThrowQ(*field.p_ctype->p_ns)) {
                input.extrn = true;
            } else {
                Ins(&R, input.body, "bool retval = true;");
                Ins(&R, input.body, "try {");
                Ins(&R, input.body, "    $name_Input($pararg, elem);");
                Ins(&R, input.body, "} catch (algo_lib::ErrorX &x) {");
                Ins(&R, input.body, "    algo_lib::SaveBadTag(\"input_error\",x.str);");
                Ins(&R, input.body, "    retval = false;");
                Ins(&R, input.body, "}");
                Ins(&R, input.body, "return retval;");
            }
        } else {
            input.priv = true;
            Ins(&R, input.body, "bool retval = true;");
            if (can_read) {
                if (finput.update) {
                    Ins(&R, input.body, "retval = $name_UpdateMaybe(elem) != nullptr;");// possibly an update, if called twice
                } else {
                    Ins(&R, input.body, "retval = $name_InsertMaybe(elem) != nullptr;");
                }
            } else {
                Ins(&R, input.body, "// $name cannot be read, no cfmt defined.");
            }
            Ins(&R, input.body, "return retval;");
        }
    }
}

void amc::tfunc_Io_Input() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.c_finput && field.c_finput->extrn && GenThrowQ(*field.p_ctype->p_ns)) {
        amc::FCtype *basetype  = GetBaseType(*field.p_arg,field.p_arg);// always read base class if possible
        Set(R, "$Basetype", basetype->cpp_type);
        amc::FFunc& input = amc::CreateCurFunc();
        Ins(&R, input.ret, "void", false);
        Ins(&R, input.proto, "$name_Input($Parent, $Basetype &elem)", false);
        input.extrn = true;
    }
}

void amc::tfunc_Io_SaveSsimfile() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FField *inst = FirstInst(*field.p_arg);

    if (field.c_foutput && !inst) {
        prerr("amc.no_pool_instance"
              <<Keyval("ctype",field.arg)
              <<Keyval("comment","Foutput requires that ctype has an instance"));
        algo_lib::_db.exit_code=1;
    }
    if (field.c_foutput && inst) {// field:jivecli.FDb.zs_sorted_space
        Set(R, "$instname", name_Get(*inst)); // space
        amc::FCtype *tgttype = field.p_arg;
        amc::FCtype *basetype = GetBaseType(*tgttype, tgttype);
        vrfy(basetype->c_ssimfile, tempstr()<<"No ssimfile associated with "<<field.field);
        Set(R, "$Basetype", basetype->cpp_type);
        amc::FFunc& save = amc::CreateCurFunc();
        save.glob = true;
        Ins(&R, save.comment, "Save table to ssimfile");
        Ins(&R, save.ret    , "bool", false);
        Ins(&R, save.proto  , "$name_SaveSsimfile(algo::strptr fname)",false);
        Ins(&R, save.body   , "cstring text;");
        Ins(&R, save.body   , "ind_beg($ns::_db_$name_curs, $name, $ns::_db) {");
        Ins(&R, save.body   , "    $Basetype out;");
        Ins(&R, save.body   , "    $instname_CopyOut($name, out);");
        Ins(&R, save.body   , "    $Basetype_Print(out, text);");
        Ins(&R, save.body   , "    text << eol;");
        Ins(&R, save.body   , "}ind_end;");
        Ins(&R, save.body   , "(void)algo::CreateDirRecurse(algo::GetDirName(fname));");
        Ins(&R, save.body   , "// it is a silent error if the file cannot be saved.");
        Ins(&R, save.body   , "return algo::SafeStringToFile(text, fname);");
    }
}

// -----------------------------------------------------------------------------

void amc::tclass_Io() {
}

// -----------------------------------------------------------------------------

// Generate a name for the user-defined function based on the specified hook
// I.e. amc::tclass_Io (hook name 'step' -- legacy -- all hooks before this feature
//   were called 'step' and function name didn't depend on hook name)
// or amc::tclass_Io_newfield (hook name 'newfield')
tempstr amc::StaticHookName(amc::FField &field, strptr suffix) {
    tempstr ret;
    ret<<ns_Get(field)<<"::"<<Refname(*field.p_ctype)<<"_"<<suffix;
    if (name_Get(field) != "step") {
        ret << "_"<<name_Get(field);
    }
    return ret;
}

// -----------------------------------------------------------------------------

tempstr amc::StaticHookPkey(amc::FField &field, strptr suffix) {
    tempstr ret(StaticHookName(field,suffix));
    Replace(ret,"::","...");
    return ret;
}

// -----------------------------------------------------------------------------

static int NHooks(amc::FField &field) {
    int n=0;
    ind_beg(amc::ctype_c_field_curs,hookfield,*field.p_arg) {
        if (hookfield.reftype == dmmeta_Reftype_reftype_Hook) {
            n++;
        }
    }ind_end;
    return n;
}

// -----------------------------------------------------------------------------

static void Gstatic_DataTable(cstring &out, amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    Ins(&R, out, "static struct _t {");
    Ins(&R, out, "    const char *s;");
    ind_beg(amc::ctype_c_field_curs,hookfield,*field.p_arg) {
        if (hookfield.reftype == dmmeta_Reftype_reftype_Hook) {
            Set(R,"$hookname",name_Get(hookfield));
            Ins(&R, out, "    void (*$hookname)();");
            amc::AddArg(out, amc::Refto(hookfield.p_arg->cpp_type), hookfield.arg != "");
        }
    }ind_end;
    Ins(&R, out, "} data[] = {");
    algo::ListSep ls(",");
    int nhooks=NHooks(field);
    ind_beg(amc::ctype_c_static_curs, static_tuple, *field.p_arg) {
        tempstr str;
        strptr_PrintCpp(tempstr() << static_tuple.tuple, str);
        out << "    "<<ls<<"{ "<<str;
        ind_beg(amc::ctype_c_field_curs,hookfield,*field.p_arg) {
            if (hookfield.reftype == dmmeta_Reftype_reftype_Hook) {
                out << ", "<<StaticHookName(hookfield,amc::PkeyCppident(static_tuple.tuple));
            }
        }ind_end;
        out << " }\n";
    }ind_end;
    out<<"    "<<ls<<"{"<<"NULL";
    for (int i=0; i<nhooks; i++) {
        out<<", NULL";
    }
    out<<"}\n";
    Ins(&R, out, "};");
    Ins(&R, out, "(void)data;");
}

// -----------------------------------------------------------------------------

static void Gstatic_Insert(amc::FFunc &func, amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    Set(R, "$Basetype", GetBaseType(*field.p_arg,field.p_arg)->cpp_type);
    Ins(&R,func.body     , "$Basetype $name;");
    Ins(&R,func.body     , "for (int i=0; data[i].s; i++) {");
    Ins(&R, func.body    , "    (void)$Basetype_ReadStrptrMaybe($name, algo::strptr(data[i].s));");
    int nhooks = NHooks(field);
    Ins(&R, func.body, "$Cpptype *elem = $name_InsertMaybe($name);");
    cstring text;
    bool must_succeed = field.reftype == dmmeta_Reftype_reftype_Inlary;
    ind_beg(amc::ctype_c_field_curs,hookfield,*field.p_arg) {
        if (hookfield.reftype == dmmeta_Reftype_reftype_Hook) {
            must_succeed = true;
        }
    }ind_end;
    if (must_succeed) {
        Ins(&R, text, "vrfy(elem, tempstr(\"$ns.static_insert_fatal_error\")"
            "\n    << Keyval(\"tuple\",algo::strptr(data[i].s))"
            "\n    << Keyval(\"comment\",algo_lib::DetachBadTags())"
            ");");
    }
    if (nhooks) {
        if (!must_succeed) {
            Ins(&R, text, "    if (elem) {");
        }
        ind_beg(amc::ctype_c_field_curs,hookfield,*field.p_arg) {
            if (hookfield.reftype == dmmeta_Reftype_reftype_Hook) {
                Set(R,"$hookname",name_Get(hookfield));
                Ins(&R, text, "elem->$hookname = data[i].$hookname;");
            }
        }ind_end;
        if (!must_succeed) {
            Ins(&R, text, "    }");
        }
    }
    if (FindStr(text, "elem")==-1) {
        Ins(&R, text, "(void)elem;");
    }
    Ins(&R, text    , "}");
    func.body << text;
}

// -----------------------------------------------------------------------------

static void Gstatic_Globalref(amc::FField &field) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    tempstr gsymbol;
    int idx = 0;
    ind_beg(amc::ctype_c_static_curs, static_tuple, *field.p_arg) {
        Set(R, "$ns", ns.ns);
        Set(R, "$key", amc::PkeyCppident(static_tuple.tuple));
        Set(R, "$idx", tempstr()<<idx);
        Ins(&R, gsymbol, "static $ns::$Ctype &$ns_$name_$key \t= (($ns::$Ctype*)$ns::_db.$name_data)[$idx];");
        idx++;
    }ind_end;
    if (ch_N(gsymbol)) {
        *ns.inl << Tabulated(gsymbol,"\t","l",2);
    }
}


// -----------------------------------------------------------------------------

void amc::tfunc_Io_LoadStatic() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (field.c_gstatic) {
        amc::FFunc &func = amc::CreateCurFunc();
        func.ret="void";
        func.priv=true;
        Ins(&R,func.proto,"$name_LoadStatic()",false);

        Set(R, "$hookname", "step");
        // table of function pointers
        Gstatic_DataTable(func.body, field);
        // code to populate the static table and assign up function pointers
        Gstatic_Insert(func, field);
        if (field.reftype == dmmeta_Reftype_reftype_Inlary) {
            Gstatic_Globalref(field);
        }
    }
}
