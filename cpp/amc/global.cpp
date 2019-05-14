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
// Source: cpp/amc/global.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

//
// Re-order ctypes for reading.
// Include only ctypes which have instances in this namespace.
// Is anything else required?
//
static void InsTopoSortVisit(amc::FNs& ns, amc::FCtype& ctype) {
    if (!ctype.ins_visited) {
        ctype.ins_visited = true;
        if (ctype.p_ns == &ns && FirstInst(ctype)) {
            ind_beg(amc::ctype_c_parent_curs, parent, ctype) {
                InsTopoSortVisit(ns, parent);
            }ind_end;
            amc::c_ctype_ins_Insert(ns, ctype);
        }
    }
}


// -----------------------------------------------------------------------------

void amc::tclass_Global() {
    amc::FField         &field = *amc::_db.genfield.p_field;
    amc::FNs            &ns    = *field.p_ctype->p_ns;

    ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
        InsTopoSortVisit(ns, ctype);
    }ind_end;
}

void amc::tfunc_Global_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;

    Ins(&R, *ns.hdr, "extern $Ctype $name;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (ctype_Get(field) == "algo_lib.FDb") {// dirty hack
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "void", "", "");
            func.inl = false;
            Ins(&R, func.body, "algo_lib::_db.last_signal             = 0;");
            Ins(&R, func.body, "ary_beg(cstring, str, algo_lib::temp_strings_Getary()) {");
            Ins(&R, func.body, "    ch_Reserve(str, 256);");
            Ins(&R, func.body, "}ary_end;");
            Ins(&R, func.body, "algo_lib::_db.n_temp = algo_lib::temp_strings_N();");
            Ins(&R, func.body, "algo_lib::bh_timehook_Reserve(32);");
            Ins(&R, func.body, "algo_lib::InitCpuHz();");
            Ins(&R, func.body, "algo_lib::_db.eol          = true;");
        }
    }
}

// -----------------------------------------------------------------------------

static void GenLoadTuples(amc::FFunc &ldt, algo_lib::Replscope &R, amc::FField &field) {
    Ins(&R, ldt.body, "static const char *ssimfiles[] = {");
    ListSep ls;
    int nitem=0;
    ind_beg(amc::ns_c_ctype_ins_curs, ctype, *field.p_ctype->p_ns) if (FirstInst(ctype)) {
        amc::FField &inst = *FirstInst(ctype);
        amc::FCtype *base = GetBaseType(ctype, &ctype);
        if (inst.c_finput && base->c_ssimfile) {
            if (!HasReadQ(*base)) {
                prerr("amc.gen_finput"
                      <<Keyval("field",inst.field)
                      <<Keyval("base",base->ctype)
                      <<Keyval("text","Missing cfmt read:Y for base"));
                algo_lib::_db.exit_code++;
            }
            if (inst.reftype == "Cppstack") {
                prerr("amc.gen_finput_cppstack"
                      <<Keyval("field",inst.field)
                      <<Keyval("text","Finput does not work with reftype:Cppstack"));
                algo_lib::_db.exit_code++;
            }
            ldt.body << ls << "\"" << base->c_ssimfile->ssimfile << "\"";
            if (++nitem % 4 == 0) {
                ldt.body << "\n";
            }
        }
    }ind_end;
    ldt.body << "\n" << ls << "NULL" << "};\n";
    Ins(&R, ldt.body, "retval = algo_lib::DoLoadTuples(root, $ns::InsertStrptrMaybe, ssimfiles, true);");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_LoadTuplesMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFunc& ldt = amc::CreateCurFunc(true); {
        AddProtoArg(ldt, "algo::strptr", "root");
        AddRetval(ldt, "bool", "retval","true");
    }
    if (amc::HasFinputsQ(*field.p_ctype->p_ns)) {
        GenLoadTuples(ldt,R,field);
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_SaveTuples() {
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    algo_lib::Replscope &R = amc::_db.genfield.R;

    if (c_foutput_N(ns)) {
        // function to save all tuples
        amc::FFunc& savetuples = amc::CreateCurFunc(true); {
            AddProtoArg(savetuples,"algo::strptr","root");
            AddRetval(savetuples,"u32","retval","0");
        }
        Ins(&R, savetuples.body   , "u32 nbefore = algo_lib::_db.stringtofile_nwrite;");
        ind_beg(amc::ns_c_ctype_curs, ctype,ns) {
            ind_beg(amc::ctype_c_field_curs, field, ctype) if (field.c_foutput) {
                Set(R, "$tablename", name_Get(field));
                amc::FCtype *tgttype = field.p_arg;
                amc::FCtype *basetype = GetBaseType(*tgttype, tgttype);
                vrfy(basetype->c_ssimfile, tempstr()<<"No ssimfile associated with "<<field.field);
                Set(R, "$ssimfile", basetype->c_ssimfile->ssimfile);
                Ins(&R, savetuples.body, "(void)$tablename_SaveSsimfile(SsimFname(root, \"$ssimfile\"));");
            }ind_end;
        }ind_end;
        Ins(&R, savetuples.body   , "retval = algo_lib::_db.stringtofile_nwrite - nbefore;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_InsertStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    bool has_inputs = amc::HasFinputsQ(*field.p_ctype->p_ns);
    amc::FFunc& fcn = amc::CreateCurFunc(true); {
        AddProtoArg(fcn, "algo::strptr", "str");
        AddRetval(fcn, "bool", "retval", "true");
    }
    Ins(&R, fcn.comment, "Parse strptr into known type and add to database.");
    Ins(&R, fcn.comment, "Return value is true unless an error occurs. If return value is false, algo_lib::_db.errtext has error text");
    if (has_inputs) {
        Ins(&R, fcn.body   , "$ns::TableId table_id(-1);");
        Ins(&R, fcn.body   , "value_SetStrptrMaybe(table_id, algo::GetTypeTag(str));");
        Ins(&R, fcn.body   , "switch (value_GetEnum(table_id)) {");
        ind_beg(amc::ctype_c_field_curs, inst, *field.p_ctype) if (inst.c_finput) {
            vrfy(!inst.c_gstatic, tempstr()<<"Finput and gstatic cannot be specified together. Field: ["<<inst.field<<"]");
            amc::FCtype *base  = GetBaseType(*inst.p_arg,inst.p_arg);// always read base class if possible
            if (HasReadQ(*inst.p_arg) && base != inst.p_arg) {
                prerr("amc.read_base  "
                      <<Keyval("base_ctype",base->ctype)
                      <<Keyval("orig_ctype",inst.arg)
                      <<Keyval("comment","program will read bad type instead of original"));
            }
            Set(R, "$basens" , ns_Get(*base));
            Set(R, "$basename" , name_Get(*base));
            Set(R, "$instname"   , name_Get(inst));
            Set(R, "$finput"   , inst.field);
            Set(R, "$Elemtype"  , amc::NsToCpp(base->ctype));
            Ins(&R, fcn.body    , "case $ns_TableId_$basens_$basename: { // finput:$finput");
            Ins(&R, fcn.body    , "    $Elemtype elem;");
            Ins(&R, fcn.body    , "    retval = $Elemtype_ReadStrptrMaybe(elem, str);");
            Ins(&R, fcn.body    , "    retval = retval && $instname_InputMaybe(elem);");
            // non-strict mode -- always succeed
            if (!inst.c_finput->strict) {
                Ins(&R, fcn.body, "    retval = true; // finput strict:N");
            }
            Ins(&R, fcn.body    , "    break;");
            Ins(&R, fcn.body    , "}");
        }ind_end;
        Ins(&R, fcn.body    , "default:");
        if (ns_Get(*field.p_ctype) != "algo_lib") {
            Ins(&R, fcn.body    , "    retval = algo_lib::InsertStrptrMaybe(str);");
        }
        Ins(&R, fcn.body        , "    break;");
        Ins(&R, fcn.body        , "} //switch");
        Ins(&R, fcn.body    , "if (!retval) {");
        Ins(&R, fcn.body    , "    algo_lib::NoteInsertErr(str); // increment error counter");
        Ins(&R, fcn.body    , "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_InitReflection() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;

    amc::FFunc& initrefl = amc::CreateCurFunc(true); {
        AddRetval(initrefl, "void", "", "");
    }
    initrefl.priv = true;
    initrefl.inl = false;
    int n_finput = c_finput_N(ns);// count inputs/outputs in this ns
    tempstr text;// register own database
    Set(R, "$InsertStrptrMaybe", n_finput > 0 ? "$ns::InsertStrptrMaybe" : "NULL");
    Set(R, "$Step", c_fstep_N(ns) ? "$ns::Step" : "NULL");
    Set(R, "$MainLoop", ns.c_main ? "$ns::MainLoop" : "NULL");
    Ins(&R, initrefl.body,"algo_lib::imdb_InsertMaybe(algo::Imdb(\"$ns\", $InsertStrptrMaybe, $Step, $MainLoop, NULL, algo::Comment()));");
    Ins(&R, initrefl.body,"");
    // register each table in this database in the algo_lib.FImtable table
    // only values are registered.
    ind_beg(amc::ctype_c_field_curs, globfield, *ns.c_globfld->p_arg) if (globfield.reflect) {
        amc::FCtype *base = GetBaseType(*globfield.p_arg,globfield.p_arg);
        amc::FSsimfile *rel = globfield.p_arg->c_ssimfile;
        if (!rel && base) {
            rel = base->c_ssimfile;
        }
        Set(R, "$globname", name_Get(globfield));
        Set(R, "$Ctype"  , globfield.p_arg->cpp_type);
        Set(R, "$ssimfile", rel ? strptr(rel->ssimfile) : strptr());
        tempstr comtstr;
        if (base) {
            strptr_PrintCpp(base->comment, comtstr);
        }
        Set(R, "$comtstr", comtstr);
        amc::FTfunc *f_find  = amc::ind_tfunc_Find(amcdb::Tfunc_Concat_tclass_name(globfield.reftype,"RowidFind"));
        amc::FTfunc *f_n     = amc::ind_tfunc_Find(amcdb::Tfunc_Concat_tclass_name(globfield.reftype,"N"));
        bool has_print      = HasStringPrintQ(*globfield.p_arg);
        Set(R, "$RowidFind", (f_find ? "$globname_RowidFind" : "NULL"));
        Set(R, "$NItems", (f_n ? "$globname_N" : "NULL"));
        Set(R, "$Print", (has_print ? "$Ctype_Print" : "NULL"));// there may be a specific function that prints JUST this field!
        // register with algo_lib database
        ch_RemoveAll(text);
        Ins(&R, text,"algo::Imtable t_$globname;");
        Ins(&R, text,"t_$globname.imtable       \t= \"$ns.$globname\";");
        Ins(&R, text,"t_$globname.ssimfile      \t= \"$ssimfile\";");
        Ins(&R, text,"t_$globname.size          \t= sizeof($Ctype);");
        Ins(&R, text,"t_$globname.comment.value \t= $comtstr;");
        Ins(&R, text,"t_$globname.c_RowidFind   \t= $RowidFind;");
        Ins(&R, text,"t_$globname.NItems        \t= $NItems;");
        Ins(&R, text,"t_$globname.Print         \t= (algo::ImrowPrintFcn)$Print;");
        Ins(&R, text,"algo_lib::imtable_InsertMaybe(t_$globname);");
        Ins(&R, text,"");
        initrefl.body << Tabulated(text, "\t", "ll", 2) << eol;
    }ind_end;

    // dispsig
    if (ind_ctype_Find(Subst(R,"$ns.FDb"))) {
        Ins(&R, initrefl.body, "// -- load signatures of existing dispatches --");
        ind_beg(amc::ns_c_dispsig_curs,dispsig,ns) {
            Set(R,"$dispsig"  ,dispsig.dispsig);
            Set(R,"$signature",tempstr()<<dispsig.signature);
            Ins(&R, initrefl.body, "algo_lib::InsertStrptrMaybe(\"dmmeta.Dispsigcheck  dispsig:'$dispsig'  signature:'$signature'\");");
        }ind_end;
    }
}

void amc::tfunc_Global_LoadSsimfileMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& loadssimfile = amc::CreateCurFunc(true); {
        AddProtoArg(loadssimfile,"algo::strptr","fname");
        AddRetval(loadssimfile,"bool","retval","true");
    }
    // Loading non-existent files is considered a success
    Ins(&R, loadssimfile.body   , "if (FileQ(fname)) {");
    Ins(&R, loadssimfile.body   , "    retval = algo_lib::LoadTuplesFile(fname, $ns::InsertStrptrMaybe, true);");
    Ins(&R, loadssimfile.body   , "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_main() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (amc::c_fstep_N(ns) > 0 && !ns.c_main && ExeQ(ns)) {
        prerr("amc.no_main"
              <<Keyval("ns",ns.ns)
              <<Keyval("comment","Add dmmeta.main record for this namespace"));
        algo_lib::_db.exit_code=1;
    }
    // generate main() function
    if (ns.c_main && ExeQ(ns)) {
        // main
        amc::FFunc& main = amc::ind_func_GetOrCreate(Subst(R, "$ns...main"));
        main.glob = true;
        main.globns = true;
        main.ret = "int";
        Ins(&R, main.proto, "main(int argc, char **argv)", false);
        Ins(&R, main.body    , "try {");
        ind_beg(amc::ns_c_parentns_curs, parentns, ns) if (parentns.c_globfld) {
            Ins(&R, main.body, tempstr()<<"    "<<parentns.c_globfld->p_arg->cpp_type<<"_Init();");
        }ind_end;
        Ins(&R, main.body    , "    algo_lib::_db.argc = argc;");
        Ins(&R, main.body    , "    algo_lib::_db.argv = argv;");
        Ins(&R, main.body    , "    algo_lib::IohookInit();");
        // call last non-module main, followed by all module mains
        for (int i=c_parentns_N(ns)-1; i>=0; i--) {// call first non-module main
            amc::FNs &parentns=*c_parentns_Find(ns,i);
            if (parentns.c_main && !parentns.c_main->ismodule) {
                Set(R, "$parentns", parentns.ns);
                Ins(&R, main.body    , "    $parentns::MainArgs(algo_lib::_db.argc,algo_lib::_db.argv); // dmmeta.main:$parentns");
                break;
            }
        }
        ind_beg(amc::ns_c_parentns_curs,parentns,ns) if (parentns.c_main && parentns.c_main->ismodule) {// call all ismodule mains
            Set(R, "$parentns", parentns.ns);
            Ins(&R, main.body    , "    $parentns::MainArgs(algo_lib::_db.argc,algo_lib::_db.argv); // dmmeta.main:$parentns  ismodule:Y");
        }ind_end;
        for (int i=c_parentns_N(ns)-1; i>=0; i--) {// call main loop of ismodule main
            amc::FNs &parentns=*c_parentns_Find(ns,i);
            if (parentns.c_main && parentns.c_main->ismodule) {
                Set(R, "$parentns", parentns.ns);
                Ins(&R, main.body    , "    $parentns::MainLoop(); // dmmeta.main:$parentns  ismodule:Y");
                break;
            }
        }
        Ins(&R, main.body    , "} catch(algo_lib::ErrorX &x) {");
        Ins(&R, main.body    , "    prerr(\"$ns.error  \" << x); // there may be additional hints in DetachBadTags");
        Ins(&R, main.body    , "    algo_lib::_db.exit_code = 1;");
        Ins(&R, main.body    , "}");
        Ins(&R, main.body    , "if (algo_lib::_db.last_signal) {");
        Ins(&R, main.body    , "    algo_lib::_db.exit_code = 1;");
        Ins(&R, main.body    , "}");
        Ins(&R, main.body    , "try {");

        for (int i=c_parentns_N(ns)-1; i>=0; i--) {
            amc::FNs &parentns = *c_parentns_Find(ns,i);
            if (parentns.c_globfld) {
                Ins(&R, main.body, tempstr()<<"    "<<parentns.c_globfld->p_arg->cpp_type<<"_Uninit();");
            }
        }
        Ins(&R, main.body    , "} catch(algo_lib::ErrorX &x) {");
        Ins(&R, main.body    , "    // don't print anything, might crash");
        Ins(&R, main.body    , "    algo_lib::_db.exit_code = 1;");
        Ins(&R, main.body    , "}");
        Ins(&R, main.body    , "return algo_lib::_db.exit_code;");
    }
}

void amc::tfunc_Global_MainArgs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (ns.c_main) {
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddProtoArg(func, "int", "argc");
            AddProtoArg(func, "char **", "argv");
        }
        Ins(&R, func.ret    , "void",false);
        if (ns.c_fcmdline && ns.c_fcmdline->read) {// read command line
            Set(R,"$cmdlinename", name_Get(*ns.c_fcmdline->p_field));
            Ins(&R, func.body, "Argtuple argtuple;");
            Ins(&R, func.body, "Argtuple_ReadArgv(argtuple, argc,argv,$ns_syntax, $ns_help);");
            Ins(&R, func.body, "vrfy($ns_ReadTupleMaybe($ns::_db.$cmdlinename, argtuple.tuple),\"where:read_cmdline\");");
            amc::FFloadtuples *floadtuples = ns.c_fcmdline->p_field->p_arg->c_floadtuples;
            if (floadtuples) {
                Set(R,"$loadtuplesname",name_Get(*floadtuples->p_field));
                Ins(&R, func.body, "vrfy($ns::LoadTuplesMaybe($ns::_db.$cmdlinename.$loadtuplesname)");
                Ins(&R, func.body, "    ,tempstr()<<\"where:load_input  \"<<algo_lib::DetachBadTags());");
            }
        }
        Ins(&R, func.body, "$ns::Main(); // call through to user-defined main");
    }
}

static int CountDirectSteps(amc::FNs &ns) {
    int ret=0;
    ind_beg(amc::ns_c_fstep_curs, fstep, ns) {
        ret += DirectStepQ(fstep);
    }ind_end;
    return ret;
}

void amc::tfunc_Global_MainLoop() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (ns.c_main) {
        amc::FFunc& mainloop = amc::CreateCurFunc();
        Ins(&R, mainloop.ret    , "void",false);
        Ins(&R, mainloop.proto  , "MainLoop()",false);
        Ins(&R, mainloop.body   , "SchedTime time(get_cycles());");
        Ins(&R, mainloop.body   , "algo_lib::_db.clock          = time;");
        Ins(&R, mainloop.body   , "do {");
        Ins(&R, mainloop.body   , "    algo_lib::_db.next_loop.value = algo_lib::_db.limit;");
        // parent ns
        for (int i = c_parentns_N(ns)-1; i>=0; i--) {
            amc::FNs &parent = *c_parentns_Find(ns,i);
            if (CountDirectSteps(parent) > 0) {
                Set(R, "$parns", parent.ns);
                Ins(&R, mainloop.body, "    $parns::Step(); // dependent namespace specified via (dev.targdep)");
            }
        }
        Ins(&R, mainloop.body   , "} while (algo_lib::_db.next_loop < algo_lib::_db.limit);");
    }
}

void amc::tfunc_Global_Step() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (ns.c_main || c_fstep_N(ns)>0) {// generate main step function from all of its constituents.
        amc::FFunc& stepfunc = amc::CreateCurFunc(true);
        Ins(&R, stepfunc.ret    , "void",false);
        ind_beg(amc::ns_c_fstep_curs, fstep, ns) {
            if (DirectStepQ(fstep)) {
                Ins(&R, stepfunc.body, tempstr()<<name_Get(fstep)<<"_Call();");
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_Main() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (ns.c_main) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        func.extrn = true;
        Ins(&R, func.ret    , "void",false);
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_StaticCheck() {
    amc::tfunc_Protocol_StaticCheck();
}

// -----------------------------------------------------------------------------

static strptr Dash(amc::FField *fld) {
    return fld->c_anonfld ? "" : "-";
}

static tempstr CmdargName(amc::FField *fld) {
    tempstr ret;
    algo::Smallstr50 name = name_Get(*fld);
    if (fld->c_anonfld) ret << "[";
    ret << name;
    if (fld->c_anonfld) ret << "]";
    return ret;
}

// print rhs as a multiline c++ string
// Example:
// FmtCppStr(lhs, "ab\tcd\nefg");
// output:
// "ab\tcd\n"
// "efg"
// Note in the above example '\t' was converted to a tab by c++ compiler,
// and escaped again by FmtCppStr.
static void FmtCppStr(cstring &lhs, strptr rhs) {
    int start = 0;
    do {
        int end = FindFrom(rhs, '\n', start);
        if (end==-1) end=rhs.n_elems;
        else end++;
        strptr_PrintCpp(GetRegion(rhs,start,end-start), lhs);
        start=end;
        lhs << eol;
    } while (start < rhs.n_elems);
    lhs << ";\n";
}

// Generate help and syntax strings for command-line tools.
//
// Example Output:
//
// const char *dmsess_help =
// "Usage: dmsess [sess] [options]\n"
// "  sess          session name\n"
// "  -list         list processes in session"
// "  -cfg:string   (with -create) debug or release. default: debug"
// ;
//
// const char *dmsess_syntax =
// "[sess]:string="
// " -cfg:string=debug"
// ;
//
// generate help string
// "Usage: <stripped name> <non-optional args> <options>\n"
// <non-optional args>: any arg without default value (whether named or not).
//     exclude bool, Bool
// <options> true if any options defined
// <table of non-optional args>
// Options
// <list of optional args>
// table: compute width or largest name
// prepend - to field names which are not bracketed.
// append ". default: XXXX" to options which have a default
//

static tempstr GetCmdArgType(amc::FField& field) {
    tempstr ret("string");// default
    amc::FArgvtype *argvtype = field.p_arg->c_argvtype;
    if (!argvtype && c_field_N(*field.p_arg) == 1) {
        argvtype = c_field_Find(*field.p_arg,0)->p_ctype->c_argvtype;
    }
    if (argvtype) {
        ret=argvtype->argvtype;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// CTYPE: type of command line
// NS: target namespace (actual executable)
static void GenReadArgv(amc::FNs &ns, amc::FCtype &ctype) {
    tempstr name(name_Get(ctype));
    tempstr cmd_format;
    cmd_format << name;
    bool has_opts = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        // determine field type
        bool is_opt  = false;
        is_opt      |= ch_N(field.dflt.value) > 0;
        is_opt      |= (field.p_arg->ctype == "bool" || field.p_arg->ctype == "Bool");
        if (is_opt) {
            has_opts = true;
        } else {
            cmd_format << " "<<name_Get(field);
        }
    }ind_end;
    if (has_opts > 0) {
        cmd_format << " [options]";
    }
    // help message
    *ns.hdr << "extern const char *"<<name<<"_help;" << eol;
    *ns.cpp << "namespace "<<ns.ns<<" {"<<eol;
    *ns.cpp << "const char *"<<name<<"_help =" << eol;
    tempstr usage, table;
    if (ch_N(ns.comment.value)) {
        usage << name_Get(ctype) << ": "<<ns.comment<<eol;
    }
    usage << "Usage: "<< cmd_format<<eol;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        table << "    ";
        table << Dash(&field);
        table << CmdargName(&field);
        table << "\t";
        bool isflag = (field.p_arg->ctype == "bool" || field.p_arg->ctype == "Bool");
        if (!isflag) table << GetCmdArgType(field);
        table << "\t";
        table << field.comment;
        if (field.dflt.value.n_ch > 0) {
            strptr dflt = field.dflt.value;
            if (dflt == "\"\"") dflt=""; // empty string OK
            if (dflt.n_elems > 0) table << ". default: " << dflt;
        }
        table << eol;
    }ind_end;
    // add standard options
    table << "    -verbose\t\tEnable verbose mode"<<eol;
    table << "    -debug\t\tEnable debug mode"<<eol;
    table << "    -version\t\tShow version information"<<eol;
    table << "    -sig\t\tPrint SHA1 signatures for dispatches"<<eol;
    table << "    -help\t\tPrint this screen and exit"<<eol;
    usage << Tabulated(table, "\t", "ll",2);

    tempstr lc;
    lc << name;
    MakeLower(lc);
    FmtCppStr(*ns.cpp, usage);
    *ns.cpp << eol << eol;

    // syntax string
    *ns.hdr << "extern const char *"<<name<<"_syntax;" << eol;
    *ns.cpp << "const char *"<<name<<"_syntax =" << eol;
    tempstr syntax;
    ListSep ls("\n ");
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        syntax << ls;
        syntax << Dash(&field);
        syntax << CmdargName(&field) << ":" << GetCmdArgType(field);
        bool isflag = (field.p_arg->ctype == "bool" || field.p_arg->ctype == "Bool");
        strptr dflt = field.dflt.value;
        if (isflag && dflt == "false") dflt = strptr();
        if (dflt == "\"\"") dflt = " "; // " string x=" -- empty string allowed
        if (dflt.n_elems > 0) syntax << "=" <<  dflt;
    }ind_end;
    FmtCppStr(*ns.cpp, Tabulated(syntax, "\t", "ll",2));
    *ns.cpp << "} // namespace "<<ns.ns << eol;
}

static amc::FCtype *GetCmdline(amc::FCtype &ctype) {
    amc::FCtype *ret=NULL;
    if (ctype.p_ns->c_fcmdline) {
        ret=ctype.p_ns->c_fcmdline->p_field->p_arg;
    }
    return ret;
}

//
// Command-line help string and syntax string for executable command
//
void amc::tfunc_Global_ReadArgv() {
    amc::FCtype *cmdline_ctype = GetCmdline(*amc::_db.genfield.p_field->p_ctype);
    if (cmdline_ctype && HasArgvPrintQ(*cmdline_ctype)) {
        GenReadArgv(*amc::_db.genfield.p_field->p_ctype->p_ns, *cmdline_ctype);
    }
}
