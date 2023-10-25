// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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
// Source: cpp/amc/global.cpp
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

// -----------------------------------------------------------------------------

void amc::tfunc_Global_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (ctype_Get(field) == "algo_lib.FDb") {// dirty hack
        amc::FFunc& func = amc::CreateCurFunc(true); {
            AddRetval(func, "void", "", "");
            func.inl = false;
            Ins(&R, func.body, "algo_lib::_db.last_signal             = 0;");
            Ins(&R, func.body, "ind_beg_aryptr(cstring, str, algo_lib::temp_strings_Getary()) {");
            Ins(&R, func.body, "    ch_Reserve(str, 256);");
            Ins(&R, func.body, "}ind_end_aryptr;");
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
    algo::ListSep ls;
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
        // #AL# this must be cleaned up -- only used for lib_m2
        for (int i=c_parentns_N(ns)-1; i>=0; i--) {// call first non-module main
            amc::FNs &parentns=*c_parentns_Find(ns,i);
            if (parentns.c_main && !parentns.c_main->ismodule) {
                Set(R, "$parentns", parentns.ns);
                if (parentns.c_fcmdline && parentns.c_fcmdline->read) {
                    Ins(&R, main.body, "$parentns::ReadArgv(); // dmmeta.main:$parentns");
                }
                Ins(&R, main.body, "$parentns::Main(); // user-defined main");
                break;
            }
        }

        ind_beg(amc::ns_c_parentns_curs,parentns,ns) if (parentns.c_main && parentns.c_main->ismodule) {// call all ismodule mains
            Set(R, "$parentns", parentns.ns);
            if (parentns.c_fcmdline && parentns.c_fcmdline->read) {
                Ins(&R, main.body, "$parentns::ReadArgv(); // dmmeta.main:$parentns  ismodule:Y");
            }
            Ins(&R, main.body, "$parentns::Main(); // call through to user-defined main");
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
        Ins(&R, main.body    , "} catch(algo_lib::ErrorX &) {");
        Ins(&R, main.body    , "    // don't print anything, might crash");
        Ins(&R, main.body    , "    algo_lib::_db.exit_code = 1;");
        Ins(&R, main.body    , "}");
        Ins(&R, main.body    , "// only the lower 1 byte makes it to the outside world");
        Ins(&R, main.body    , "(void)i32_UpdateMin(algo_lib::_db.exit_code,255);");
        Ins(&R, main.body    , "return algo_lib::_db.exit_code;");
    }
}
// -----------------------------------------------------------------------------

void amc::tfunc_Global_WinMain() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    // generate WinMain() function only if main() exists
    if (amc::ind_func_Find(Subst(R, "$ns...main"))) {
        amc::FFunc& winmain = amc::ind_func_GetOrCreate(Subst(R, "$ns...WinMain"));
        winmain.glob = true;
        winmain.globns = true;
        winmain.prepcond = "defined(WIN32)"; // windows only
        winmain.ret = "int WINAPI";
        Ins(&R, winmain.proto, "WinMain(HINSTANCE,HINSTANCE,LPSTR,int)", false);
        // __argc, __argv are parsed command line args provided by MSVC runtime
        Ins(&R, winmain.body    , "return main(__argc,__argv);");
    }
}

// -----------------------------------------------------------------------------

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
        Ins(&R, mainloop.body   , "algo::SchedTime time(algo::get_cycles());");
        Ins(&R, mainloop.body   , "algo_lib::_db.clock          = time;");
        Ins(&R, mainloop.body   , "do {");
        Ins(&R, mainloop.body   , "    algo_lib::_db.next_loop.value = algo_lib::_db.limit;");
        Set(R, "ns", ns.ns);
        Ins(&R, mainloop.body, "    $ns::Steps();");
        Ins(&R, mainloop.body   , "} while (algo_lib::_db.next_loop < algo_lib::_db.limit);");
    }
}

void amc::tfunc_Global_Steps() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (c_parentns_N(ns) > 0) {
        amc::FFunc& steps = amc::CreateCurFunc();
        Ins(&R, steps.ret    , "void",false);
        Ins(&R, steps.proto  , "Steps()",false);
        // parent ns
        for (int i = c_parentns_N(ns)-1; i>=0; i--) {
            amc::FNs &parent = *c_parentns_Find(ns,i);
            if (CountDirectSteps(parent) > 0) {
                Set(R, "$parns", parent.ns);
                Ins(&R, steps.body, "$parns::Step(); // dependent namespace specified via (dev.targdep)");
            }
        }
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

static tempstr CmdargName(amc::FField *fld) {
    tempstr ret;
    algo::Smallstr50 name = name_Get(*fld);
    if (fld->c_anonfld) ret << "[";
    ret << name;
    if (fld->c_anonfld) ret << "]";
    if (fld->reftype == dmmeta_Reftype_reftype_Tary) {
        ret << "...";
    }
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
        if (end==-1) end=rhs.n_elems; else end++;
        strptr_PrintCpp(GetRegion(rhs,start,end-start), lhs);
        start=end;
        lhs << eol;
    } while (start < rhs.n_elems);
    lhs << ";\n";
}

// Generate help strings for command-line tools.
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
// generate help string
// "Usage: <stripped name> <non-optional args> <options>\n"
// <non-optional args>: any arg without default value (whether named or not), excluding bool
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
    // amc rewrites all pkey fields so they become Vals
    // so this first line can never execute
    if (c_fconst_N(*GetEnumField(field))) {
        ret = "enum";
    } else if (field.c_fflag) {
        ret = "flag";
    } else if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
        ret = "pkey";
    } if (field.reftype == dmmeta_Reftype_reftype_Regx) {
        ret = "regx";
    } else {
        if (!argvtype && c_field_N(*field.p_arg) == 1) {
            argvtype = c_field_Find(*field.p_arg,0)->p_ctype->c_argvtype;
        }
        if (argvtype) {
            ret=argvtype->argvtype;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE if FIELD (in command line context) requires no argument
// This is true for bool fields or fields with "emptyval" provided
bool amc::CmdArgValueRequiredQ(amc::FField &field) {
    return !(field.p_arg->ctype == "bool" || (field.c_fflag && field.c_fflag->emptyval != "\"\""));
}

// -----------------------------------------------------------------------------

// True if field is a required command-line argument
bool amc::CmdArgRequiredQ(amc::FField &field) {
    return field.dflt.value=="" // no default provided...
        && !field.c_tary // not an array
        && !c_fconst_N(*amc::GetEnumField(field)) // not an enum (these are always initialized)
        && CmdArgValueRequiredQ(field); // does require an arg
}

// -----------------------------------------------------------------------------

// Pick a field to extract enums from.
// Handle the case of a single-field ctype with enums in it
amc::FField *amc::GetEnumField(amc::FField &field) {
    return c_field_N(*field.p_arg)==1
        && c_fconst_N(*c_field_Find(*field.p_arg,0))
        ? c_field_Find(*field.p_arg,0)
        : &field;
}

// -----------------------------------------------------------------------------

// Adjust displayed default
// Translate true/false into "Y"/<empty string>
static tempstr GetCmdArgDflt(amc::FField &field) {
    tempstr ret(field.dflt.value);

    if (field.c_fflag) {
        // no default for flags (even if they can take a value)
        ret="";
    } if (field.c_tary) {
        // no default for arrays -- since they can be empty
        ret="";
    } else if (field.arg=="bool") {
        if (field.dflt.value=="true") {
            ret="Y";
        } else if (field.dflt.value=="false") {
            ret="";// do not show
        }
    } else {
        // scan enums for the field and translate back from the
        // numeric value to the symbolic
        tempstr enum_dflt;
        // scan enums...
        ind_beg(amc::field_c_fconst_curs,fconst,*GetEnumField(field)) {
            if (fconst.value.value == field.dflt.value
                || (!ch_N(field.dflt.value) && (fconst.value.value=="0" || fconst.value.value=="'\\0'"))) {
                enum_dflt = name_Get(fconst);
            }
        }ind_end;
        if (ch_N(enum_dflt) && enum_dflt != "\"\"") {
            ret = enum_dflt;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

tempstr amc::GetUsageString(amc::FNs &ns, amc::FFcmdline &cmdline) {
    amc::FCtype &ctype = *cmdline.p_field->p_arg;
    tempstr cmd_format(name_Get(ctype));
    bool has_opts = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) if (!field.c_falias) {
        if (CmdArgRequiredQ(field)) {
            if (CmdArgValueRequiredQ(field)) {
                if (field.c_anonfld) {
                    // [-str:]<string>   -- CmdArgRequired, anon, value required
                    cmd_format << " [-"<<name_Get(field)<<":]<"<<GetCmdArgType(field)<<">";
                } else {
                    // -astr:<string>    -- CmdArgRequired, named, value required
                    cmd_format << " -"<<name_Get(field)<<":<"<<GetCmdArgType(field)<<">";
                }
            }
        } else {
            if (CmdArgValueRequiredQ(field)) {
                if (field.c_anonfld) {
                    // [[-str:]<string>] -- !CmdArgRequired, anon, value required
                    cmd_format << " [[-"<<name_Get(field)<<":]<"<<GetCmdArgType(field)<<">]";
                } else {
                    //                   -- !CmdArgRequired, named, value not required -- skipped
                    has_opts = true;
                }
            }
        }
    }ind_end;
    if (has_opts) {
        cmd_format << " [options]";
    }
    tempstr ret;
    if (ch_N(ns.comment.value)) {
        ret << name_Get(ctype) << ": "<<ns.comment<<eol;
    }
    ret << "Usage: "<< cmd_format<<eol;
    return ret;
}

// -----------------------------------------------------------------------------

// CTYPE: type of command line
// NS: target namespace (actual executable)
static void GenHelpSyntax(amc::FNs &ns, amc::FFcmdline &cmdline) {
    amc::FCtype &ctype = *cmdline.p_field->p_arg;
    tempstr name(name_Get(ctype));
    // help message
    *ns.hdr << "extern const char *"<<name<<"_help;" << eol;
    *ns.cpp << "namespace "<<ns.ns<<" {"<<eol;
    *ns.cpp << "const char *"<<name<<"_help =" << eol;
    tempstr table;
    tempstr usage = GetUsageString(ns,cmdline);
    table << "    OPTION\tTYPE\tDFLT\tCOMMENT\n";
    // loop over fields of this command line, and its base command line
    for (int i=0; i<2; i++) {
        amc::FCtype *thisctype=i==0 ? &ctype : cmdline.p_basecmdline->p_arg;
        if (thisctype) {
            ind_beg(amc::ctype_c_field_curs, field, *thisctype) if (!field.c_falias) {
                table << "    ";
                table << (field.c_anonfld ? "" : "-");
                table << CmdargName(&field);
                table << "\t";
                table << GetCmdArgType(field);
                table << "\t";
                table << GetCmdArgDflt(field);
                table << "\t";
                table << field.comment;
                // list aliases
                ind_beg(amc::ctype_c_field_curs, aliasfield, *thisctype) {
                    if (aliasfield.c_falias && aliasfield.c_falias->p_basefield == &field) {
                        table << "; alias -"<<name_Get(aliasfield);
                    }
                }ind_end;
                // explain cumulative flag
                if (field.c_fflag && field.c_fflag->cumulative) {
                    table << "; cumulative";
                }
                // provide single-line enum choice
                // and explain enum values (with comments)
                tempstr fconst_values;
                tempstr fconst_choice;
                algo::ListSep ls("|");
                if (!field.c_tary) {
                    ind_beg(amc::field_c_fconst_curs,fconst,*GetEnumField(field)) {
                        fconst_choice << ls << name_Get(fconst);
                        if (ch_N(fconst.comment)) {
                            fconst_values << "\t\t\t    " << name_Get(fconst) << "  " << fconst.comment << eol;
                        }
                    }ind_end;
                }
                // fconst choice
                if (ch_N(fconst_choice)) {
                    table << " (" << fconst_choice << ")";
                }
                table << eol;
                table << fconst_values; // fconst values
            }ind_end;
        }
    }
    usage << Tabulated(table, "\t", "lll",2);

    tempstr lc;
    lc << name;
    MakeLower(lc);
    FmtCppStr(*ns.cpp, usage);
    *ns.cpp << eol << eol;
    *ns.cpp << "} // namespace "<<ns.ns << eol;
}

// -----------------------------------------------------------------------------

// Return expression
//   $cpptype &NAME = $ns::$_db.$fieldname
// where
//   $cpptype is the type of FIELD
//   $ns is the namespace of FIELD
//   $_db is the global instance in $ns
//   $fieldname is the name of the field
tempstr amc::VarRefToGlobal(amc::FField &field, strptr name) {
    amc::FField *glob=field.p_ctype->p_ns->c_globfld;
    return tempstr()
        <<field.p_arg->cpp_type<<" &"<<name<<" = "<<ns_Get(field)<<"::"<<name_Get(*glob)<<"."<<name_Get(field);
}

// -----------------------------------------------------------------------------

static void CheckBaseCmdline(amc::FFcmdline &cmdline) {
    amc::FField *basecmdline=cmdline.p_basecmdline;
    vrfy(basecmdline != cmdline.p_field,
         tempstr()<<"amc.circularline"
         <<Keyval("field",cmdline.field)
         <<Keyval("basecmdline",basecmdline->field)
         <<Keyval("comment","Base command line cannot be the same as the command line itself"));
    vrfy(basecmdline->field=="" || GlobalQ(*basecmdline->p_ctype)
         ,tempstr()<<"amc.baseglob"
         <<Keyval("field",basecmdline->field)
         <<Keyval("used_in",cmdline.field)
         <<Keyval("comment","Base command line must be global"));
    // disallow base command line to have anon fields
    ind_beg(amc::ctype_c_field_curs,checkfield,*basecmdline->p_arg) {
        if (checkfield.c_anonfld) {
            prerr("amc.badanon"
                  <<Keyval("field",checkfield.field)
                  <<Keyval("used_in",cmdline.field)
                  <<Keyval("comment","Base commandline cannot have anon fields"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Namespace ReadArgv function to read command line
void amc::tfunc_Global_ReadArgv() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field; // atf_amc_cmd.FDb._db
    amc::FNs &ns = *field.p_ctype->p_ns; // atf_amc_cmd
    amc::FFcmdline *fcmdline = ns.c_fcmdline; // atf_amc_cmd.FDb.cmdline
    if (ns.c_main && fcmdline) {
        amc::FCtype *cmdline_ctype = fcmdline->p_field->p_arg;
        if (cmdline_ctype) {
            GenHelpSyntax(*field.p_ctype->p_ns, *fcmdline);
        }
        amc::FFunc& func = amc::CreateCurFunc(true);
        Ins(&R, func.ret    , "void",false);
        int n_anon = c_anonfld_N(*cmdline_ctype);
        Set(R,"$cmdlinefield"  , fcmdline->field); // atf_amc_cmd.FDb.cmdline
        Set(R,"$cmdlinetypens"  , ns_Get(*fcmdline->p_field->p_arg));// command
        Set(R,"$cmdlinectypename", name_Get(*fcmdline->p_field->p_arg));// atf_amc_cmd
        Set(R,"$cmdlinecpptype", fcmdline->p_field->p_arg->cpp_type);// command::atf_amc_cmd
        amc::FField *basecmdline=fcmdline->p_basecmdline;
        Set(R,"$basecmdlinefield"  , basecmdline->field);// algo_lib.FDb.cmdline
        Set(R,"$basecmdlinetypens"  , ns_Get(*basecmdline->p_arg));// algo_lib
        Set(R,"$basecmdlinectypename", name_Get(*basecmdline->p_arg));//Cmdline
        CheckBaseCmdline(*fcmdline);
        if (basecmdline->field == "") {
            basecmdline = NULL;
        }
        Ins(&R, func.comment, "Read argc,argv directly into the fields of the command line(s)");
        Ins(&R, func.comment, "The following fields are updated:");
        Ins(&R, func.comment, "    $cmdlinefield");
        if (basecmdline) {
            Ins(&R, func.comment, "    $basecmdlinefield");
        }
        Ins(&R, func.body, tempstr()<<VarRefToGlobal(*fcmdline->p_field, "cmd")<<";");
        if (basecmdline) {
            Ins(&R, func.body, tempstr()<<VarRefToGlobal(*basecmdline, "base")<<";");
        }
        Ins(&R, func.body, "    int needarg=-1;// unknown");
        Ins(&R, func.body, "    int argidx=1;// skip process name");
        if (n_anon > 0) {
            Ins(&R, func.body, "int anonidx=0;");
            Ins(&R, func.body, "algo::strptr nextanon = command::$ns_GetAnon(cmd, anonidx);");
        }
        Ins(&R, func.body, "    tempstr err;");
        Ins(&R, func.body, "    algo::strptr attrname;");
        Ins(&R, func.body, "    bool isanon=false; // true if attrname is anonfld (positional)");
        if (basecmdline) {
            Ins(&R, func.body, "$basecmdlinetypens::FieldId baseattrid;");
        }
        Ins(&R, func.body, "    $cmdlinetypens::FieldId attrid;");
        Ins(&R, func.body, "    bool endopt=false;");
        Ins(&R, func.body, "    int whichns=0;// which namespace does the current attribute belong to");
        ind_beg(ctype_c_field_curs,reqfield,*cmdline_ctype) if (CmdArgRequiredQ(reqfield)) {
            Set(R,"$reqfieldname",name_Get(reqfield));
            Ins(&R, func.body, "    bool $reqfieldname_present = false;");
        }ind_end;
        Ins(&R, func.body, "    for (; argidx < algo_lib::_db.argc; argidx++) {");
        Ins(&R, func.body, "        algo::strptr arg = algo_lib::_db.argv[argidx];");
        Ins(&R, func.body, "        algo::strptr attrval;");
        Ins(&R, func.body, "        algo::strptr dfltval;");
        Ins(&R, func.body, "        bool haveval=false;");
        Ins(&R, func.body, "        bool dash=elems_N(arg)>1 && arg.elems[0]=='-'; // a single dash is not an option");
        Ins(&R, func.body, "        // this attribute is a value");
        Ins(&R, func.body, "        if (endopt || needarg>0 || !dash) {");
        Ins(&R, func.body, "            attrval=arg;");
        Ins(&R, func.body, "            haveval=true;");
        Ins(&R, func.body, "        } else {");
        Ins(&R, func.body, "            // this attribute is a field name (with - or --)");
        Ins(&R, func.body, "            // or a -- by itself");
        Ins(&R, func.body, "            bool dashdash = elems_N(arg) >= 2 && arg.elems[1]=='-';");
        Ins(&R, func.body, "            int skip = int(dash) + dashdash;");
        Ins(&R, func.body, "            attrname=ch_RestFrom(arg,skip);");
        Ins(&R, func.body, "            if (skip==2 && elems_N(arg)==2) {");
        Ins(&R, func.body, "                endopt=true;");
        Ins(&R, func.body, "                continue;// nothing else to do here");
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "            // parse \"-a:B\" arg into attrname,attrvalue");
        Ins(&R, func.body, "            algo::i32_Range colon = TFind(attrname,':');");
        Ins(&R, func.body, "            if (colon.beg < colon.end) {");
        Ins(&R, func.body, "                attrval=ch_RestFrom(attrname,colon.end);");
        Ins(&R, func.body, "                attrname=ch_FirstN(attrname,colon.beg);");
        Ins(&R, func.body, "                haveval=true;");
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "            // look up which command (this one or the base) contains the field");
        Ins(&R, func.body, "            whichns=0;");
        Ins(&R, func.body, "            needarg=-1;");
        if (basecmdline) {
            Ins(&R, func.body, "        // look up parameter information in base namespace (needarg will be -1 if lookup fails)");
            Ins(&R, func.body, "        if ($basecmdlinetypens::FieldId_ReadStrptrMaybe(baseattrid,attrname)) {");
            Ins(&R, func.body, "            needarg = $basecmdlinetypens::$basecmdlinectypename_NArgs(baseattrid,dfltval,&isanon);");
            Ins(&R, func.body, "        }");
        }
        Ins(&R, func.body, "            if (needarg<0) {");
        Ins(&R, func.body, "                whichns=1;");
        Ins(&R, func.body, "                // look up parameter information in this namespace (needarg will be -1 if lookup fails)");
        Ins(&R, func.body, "                if ($cmdlinetypens::FieldId_ReadStrptrMaybe(attrid,attrname)) {");
        Ins(&R, func.body, "                    needarg = $cmdlinetypens::$cmdlinectypename_NArgs(attrid,dfltval,&isanon);");
        Ins(&R, func.body, "                }");
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "            if (attrval == \"\" && dfltval != \"\") {");
        Ins(&R, func.body, "                attrval=dfltval;");
        Ins(&R, func.body, "                haveval=true;");
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "            if (needarg<0) {");
        Ins(&R, func.body, "                err<<\"$ns: unknown option \"<<Keyval(\"value\",arg)<<eol;");
        Ins(&R, func.body, "            } else {");
        if (n_anon>0) {
            Ins(&R, func.body, "            if (isanon) {");
            Ins(&R, func.body, "                if (attrname == nextanon) { // treat named anon (positional) argument as unnamed");
            Ins(&R, func.body, "                    attrname = \"\"; // treat it as unnamed");
            Ins(&R, func.body, "                } else if (nextanon != \"\") { // disallow out-of-order anon (positional) args");
            Ins(&R, func.body, "                    err<<\"$ns: error at \"<<algo::strptr_ToSsim(arg)<<\": must be preceded by [-\"<<nextanon<<\"]\"<<eol;");
            Ins(&R, func.body, "                }");
            Ins(&R, func.body, "            }");
        }
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "        }");
        if (n_anon>0) {
            Ins(&R, func.body, "        // look up anon field name based on index");
            Ins(&R, func.body, "        // anon fields are only allowed in the leaf ns, never base");
            Ins(&R, func.body, "        if (ch_N(attrname) == 0) {");
            Ins(&R, func.body, "            attrname = nextanon;");
            Ins(&R, func.body, "            nextanon = command::$ns_GetAnon(cmd, ++anonidx);");
            Ins(&R, func.body, "            $cmdlinetypens::FieldId_ReadStrptrMaybe(attrid,attrname);");
            Ins(&R, func.body, "            whichns=1;");
            Ins(&R, func.body, "        }");
        }
        Ins(&R, func.body, "        if (ch_N(attrname) == 0) {");
        Ins(&R, func.body, "            err << \"$ns: too many arguments. error at \"<<algo::strptr_ToSsim(arg)<<eol;");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "        // read value into currently selected arg");
        Ins(&R, func.body, "        if (haveval) {");
        Ins(&R, func.body, "            bool ret=false;");
        Ins(&R, func.body, "            // it's already known which namespace is consuming the args,");
        Ins(&R, func.body, "            // so directly go there");
        if (basecmdline) {
            Ins(&R, func.body, "        if (whichns == 0) {");
            Ins(&R, func.body, "            ret=$basecmdlinetypens::$basecmdlinectypename_ReadFieldMaybe(base, attrname, attrval);");
            Ins(&R, func.body, "        }");
        }
        // mark required fields as present
        Ins(&R, func.body, "            if (whichns==1) {");
        Ins(&R, func.body, "                ret=$cmdlinetypens::$cmdlinectypename_ReadFieldMaybe(cmd, attrname, attrval);");
        Ins(&R, func.body, "                switch(attrid.value) {");
        ind_beg(ctype_c_field_curs,reqfield,*cmdline_ctype) if (CmdArgRequiredQ(reqfield)) {
            Set(R,"$reqfieldname",name_Get(reqfield));
            Ins(&R, func.body, "                case $cmdlinetypens_FieldId_$reqfieldname: $reqfieldname_present=true; break;");
        }ind_end;
        Ins(&R, func.body, "                    default:break;");
        Ins(&R, func.body, "                }");
        Ins(&R, func.body, "            }");

        Ins(&R, func.body, "            if (!ret) {");
        Ins(&R, func.body, "                err<<\"$ns: error in \"");
        Ins(&R, func.body, "                    <<Keyval(\"option\",attrname)");
        Ins(&R, func.body, "                    <<Keyval(\"value\",attrval)<<eol;");
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "            needarg--;");
        Ins(&R, func.body, "            if (needarg <= 0) {");
        Ins(&R, func.body, "                attrname=\"\";// forget which argument was being filled");
        Ins(&R, func.body, "            }");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "    }");

        Ins(&R, func.body, "bool dohelp = false;");

        Ins(&R, func.body, "bool doexit=false;");

        // post-processing steps: hard-coded handling of algo_lib commandline
        if (basecmdline && basecmdline->field == "algo_lib.FDb.cmdline") {
            Ins(&R, func.body, "if (algo_lib::_db.cmdline.help) {");
            Ins(&R, func.body, "    dohelp = true;");
            Ins(&R, func.body, "    doexit = true;");
            Ins(&R, func.body, "    algo_lib::_db.exit_code = 1; // help exits with non-zero status code");
            Ins(&R, func.body, "} else if (algo_lib::_db.cmdline.version) {");
            Ins(&R, func.body, "    // -ver output goes to stdout");
            Ins(&R, func.body, "    prlog(algo::gitinfo_Get());");
            Ins(&R, func.body, "    doexit = true;");
            Ins(&R, func.body, "} else if (algo_lib::_db.cmdline.signature) {");
            Ins(&R, func.body, "    ind_beg(algo_lib::_db_dispsigcheck_curs,dispsigcheck,algo_lib::_db) {");
            Ins(&R, func.body, "        // dispsig goes to stdout");
            Ins(&R, func.body, "        dmmeta::Dispsigcheck out;");
            Ins(&R, func.body, "        dispsigcheck_CopyOut(dispsigcheck,out);");
            Ins(&R, func.body, "        prlog(out);");
            Ins(&R, func.body, "    }ind_end");
            Ins(&R, func.body, "    doexit = true;");
            Ins(&R, func.body, "}");
        }

        Ins(&R, func.body, "if (!dohelp) {");
        ind_beg(ctype_c_field_curs,reqfield,*cmdline_ctype) if (CmdArgRequiredQ(reqfield)) {
            Set(R,"$reqfieldname",name_Get(reqfield));
            Ins(&R, func.body, "if (!$reqfieldname_present) {");
            Ins(&R, func.body, "    err << \"$ns: Missing value for required argument -$reqfieldname (see -help)\" << eol;");
            Ins(&R, func.body, "    doexit = true;");
            Ins(&R, func.body, "}");
        }ind_end;
        Ins(&R, func.body, "}");

        Ins(&R, func.body, "if (err != \"\") {");
        Ins(&R, func.body, "    algo_lib::_db.exit_code=1;");
        Ins(&R, func.body, "    prerr(err);");
        Ins(&R, func.body, "    doexit=true;");
        Ins(&R, func.body, "}");

        Ins(&R, func.body, "if (dohelp) {");
        Ins(&R, func.body, "    prlog($cmdlinectypename_help);");
        Ins(&R, func.body, "}");

        Ins(&R, func.body, "if (doexit) {");
        Ins(&R, func.body, "    _exit(algo_lib::_db.exit_code);");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "algo_lib::ResetErrtext();");

        // post-processing steps:
        amc::FFloadtuples *floadtuples = fcmdline->p_field->p_arg->c_floadtuples;
        if (floadtuples) {
            Set(R,"$loadtuplesname",name_Get(*floadtuples->p_field));
            Ins(&R, func.body, "vrfy($ns::LoadTuplesMaybe(cmd.$loadtuplesname)");
            Ins(&R, func.body, "    ,tempstr()<<\"where:load_input  \"<<algo_lib::DetachBadTags());");
        }
    }
}
