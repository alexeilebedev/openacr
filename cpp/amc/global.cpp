// Copyright (C) 2023-2026 AlgoRND
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
// Source: cpp/amc/global.cpp -- Global (FDb)
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Look up the conventional <ns>.FDb.cmdline field. Returns NULL if absent.
static amc::FField *FindCmdlineField(amc::FNs &ns) {
    return amc::ind_field_Find(
                               dmmeta::Field_Concat_ctype_name(
                                                               dmmeta::Ctype_Concat_ns_name(ns.ns, "FDb"), "cmdline"));
}

// Resolve the ccmdline for a given namespace via its cmdline field.
// Returns NULL if the namespace has no cmdline.
static amc::FCcmdline *FindCcmdline(amc::FNs &ns) {
    amc::FField *cmdfield = FindCmdlineField(ns);
    amc::FCcmdline *ret = NULL;
    if (cmdfield && cmdfield->p_arg) {
        ret = cmdfield->p_arg->c_ccmdline;
    }
    return ret;
}

// True if the namespace has a cmdline that should be auto-parsed at startup.
static bool CmdlineQ(amc::FNs &ns) {
    amc::FCcmdline *cm = FindCcmdline(ns);
    return cm && cm->read;
}

// -----------------------------------------------------------------------------

void amc::tclass_Global() {
}

void amc::tfunc_Global_Init() {
}

int amc::c_parentns_FindIndex(amc::FNs& ns, amc::FNs *val) {
    int index=-1;
    ind_beg(ns_c_parentns_curs,parentns,ns) {
        if (&parentns == val) {
            index=ind_curs(parentns).index;
            break;
        }
    }ind_end;
    return index;
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_LoadTuplesMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FFunc& ldt = amc::CreateCurFunc(true); {
        AddProtoArg(ldt, "algo::strptr", "root");
        AddProtoArg(ldt, "bool", "recursive");
        AddRetval(ldt, "bool", "retval","true");
    }
    ind_beg(amc::_db_finput_curs, finput, amc::_db) {
        amc::FCtype *arg = finput.p_field->p_arg;
        amc::FNs *finput_ns = finput.p_field->p_ctype->p_ns;
        amc::FCtype *ctype = amc::GetBaseType(*arg,arg);// prefer base
        if (ctype->c_ssimfile && (finput_ns == &ns || c_parentns_FindIndex(ns,finput_ns)!=-1)) {
            ctype->c_ssimfile->input_select=true;
        }
    }ind_end;
    Ins(&R, ldt.body, "if (FileQ(root)) {");
    Ins(&R, ldt.body, "    retval = $ns::LoadTuplesFile(root, recursive);");
    Ins(&R, ldt.body, "} else if (root == \"-\") {");
    Ins(&R, ldt.body, "    retval = $ns::LoadTuplesFd(algo::Fildes(0),\"(stdin)\",recursive);");
    Ins(&R, ldt.body, "} else if (DirectoryQ(root)) {");
    ind_beg(_db_c_ssimfile_sorted_curs,ssimfile,_db) {
        if (bool_Update(ssimfile.input_select,false)) {
            Set(R,"$ssimfile",ssimfile.ssimfile);
            Ins(&R, ldt.body, "    retval = retval && $ns::LoadTuplesFile(algo::SsimFname(root,\"$ssimfile\"),recursive);");
        }
    }ind_end;
    Ins(&R, ldt.body, "} else {");
    Ins(&R, ldt.body, "    algo_lib::AppendErrtext(\"path\", root);");
    Ins(&R, ldt.body, "    algo_lib::AppendErrtext(\"comment\", \"Wrong working directory?\");");
    Ins(&R, ldt.body, "    retval = false;");
    Ins(&R, ldt.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_LoadTuplesFile() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& ldt = amc::CreateCurFunc(true);
    Ins(&R, ldt.comment, "Read tuples from file FNAME into this namespace's in-memory database.");
    Ins(&R, ldt.comment, "If RECURSIVE is TRUE, then also load these tuples into any parent namespaces");
    Ins(&R, ldt.comment, "It a file referred to by FNAME is missing, no error is reported (it's considered an empty set).");
    Ins(&R, ldt.comment, "Function returns TRUE if all records were parsed and inserted without error.");
    Ins(&R, ldt.comment, "If the function returns FALSE, use algo_lib::DetachBadTags() for error description");
    AddProtoArg(ldt, "algo::strptr", "fname");
    AddProtoArg(ldt, "bool", "recursive");
    AddRetval(ldt, "bool", "retval","true");
    Ins(&R, ldt.body, "algo_lib::FFildes fildes;");
    Ins(&R, ldt.body, "// missing files are not an error");
    Ins(&R, ldt.body, "fildes.fd = OpenRead(fname,algo::FileFlags());");
    Ins(&R, ldt.body, "if (ValidQ(fildes.fd)) {");
    Ins(&R, ldt.body, "    retval = LoadTuplesFd(fildes.fd, fname, recursive);");
    Ins(&R, ldt.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_LoadTuplesFd() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    amc::FFunc& ldt = amc::CreateCurFunc(true);
    AddProtoArg(ldt, "algo::Fildes", "fd");
    AddProtoArg(ldt, "algo::strptr", "fname");
    AddProtoArg(ldt, "bool", "recursive");
    AddRetval(ldt, "bool", "retval","true");
    Ins(&R, ldt.body, "ind_beg(algo::FileLine_curs,line,fd) {");
    Ins(&R, ldt.body, "    if (recursive) {");
    ind_beg(amc::ns_c_parentns_curs, parentns, ns) {
        if (&parentns != &ns && amc::HasFinputsQ(parentns)) {
            Set(R,"$parentns", parentns.ns);
            Ins(&R, ldt.body, "         retval = retval && $parentns::InsertStrptrMaybe(line);");
        }
    }ind_end;
    Ins(&R, ldt.body, "    }");
    if (amc::HasFinputsQ(ns)) {
        Ins(&R, ldt.body, "    retval = retval && $ns::InsertStrptrMaybe(line);");
    }
    Ins(&R, ldt.body, "    if (!retval) {");
    Ins(&R, ldt.body, "        algo_lib::_db.errtext << eol");
    Ins(&R, ldt.body, "            << fname << \":\"");
    Ins(&R, ldt.body, "            << (ind_curs(line).i+1)");
    Ins(&R, ldt.body, "            << \": \" << line << eol;");
    Ins(&R, ldt.body, "        break;");
    Ins(&R, ldt.body, "    }");
    Ins(&R, ldt.body, "}ind_end;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_SaveTuples() {
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    algo_lib::Replscope &R = amc::_db.genctx.R;

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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

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
            // always read base class if possible
            amc::FCtype *base  = GetBaseType(*inst.p_arg,inst.p_arg);
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
        Ins(&R, fcn.body        , "    break;");
        Ins(&R, fcn.body        , "} //switch");
        Ins(&R, fcn.body    , "if (!retval) {");
        Ins(&R, fcn.body    , "    algo_lib::NoteInsertErr(str); // increment error counter");
        Ins(&R, fcn.body    , "}");
    }
}

// -----------------------------------------------------------------------------

// Look up the Thash on the FDb pool field whose hashfld matches the basetype's
// pkey.  Returns NULL if no such Thash exists.
static amc::FField *PkeyIndex(amc::FField &finput, amc::FCtype &basetype) {
    amc::FField *ret = NULL;
    amc::FField *pkey = c_field_Find(basetype, 0);
    if (pkey) {
        ind_beg(amc::ctype_c_field_curs, hashfld, *finput.p_ctype) {
            if (hashfld.reftype == dmmeta_Reftype_reftype_Thash
                && hashfld.p_arg == finput.p_arg
                && hashfld.c_thash
                && hashfld.c_thash->hashfld == pkey->field) {
                ret = &hashfld;
                break;
            }
        }ind_end;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// RemoveStrptrMaybe can generate a delete branch for this finput when:
//  - the FDb pool's reftype supports random delete (Tpool/Lpool/Malloc — del:Y);
//    Lary/Tary/Inlary lack <pool>_Delete and can't participate.
//  - the basetype has a Thash on its pkey, so the parsed pkey can be looked up
//    in O(1) and the matching record deleted.
static bool CanRemoveStrptrMaybeQ(amc::FField &finput, amc::FCtype &basetype) {
    return finput.p_reftype->del && PkeyIndex(finput, basetype) != NULL;
}

// -----------------------------------------------------------------------------

// Generate <ns>::RemoveStrptrMaybe(strptr str): mirror of InsertStrptrMaybe
// for the delete branch of Syscmd_SsimMsg.  Switches on the type-tag, parses
// the tuple, finds the record by pkey via the basetype's Thash, calls
// <finput-name>_Delete.  Finputs without a pkey-Thash are skipped (the
// generated case logs a verblog and returns true so unrelated tables don't
// cause errors).
void amc::tfunc_Global_RemoveStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    bool has_inputs = amc::HasFinputsQ(*field.p_ctype->p_ns);
    amc::FFunc& fcn = amc::CreateCurFunc(true); {
        AddProtoArg(fcn, "algo::strptr", "str");
        AddRetval(fcn, "bool", "retval", "true");
    }
    Ins(&R, fcn.comment, "Parse strptr into known type and remove matching record from database.");
    Ins(&R, fcn.comment, "Return value is true if the record was found and removed, false otherwise.");
    if (has_inputs) {
        Ins(&R, fcn.body   , "$ns::TableId table_id(-1);");
        Ins(&R, fcn.body   , "value_SetStrptrMaybe(table_id, algo::GetTypeTag(str));");
        Ins(&R, fcn.body   , "switch (value_GetEnum(table_id)) {");
        ind_beg(amc::ctype_c_field_curs, inst, *field.p_ctype) if (inst.c_finput) {
            amc::FCtype *base = GetBaseType(*inst.p_arg, inst.p_arg);
            Set(R, "$basens"   , ns_Get(*base));
            Set(R, "$basename" , name_Get(*base));
            Set(R, "$instname" , name_Get(inst));
            Set(R, "$finput"   , inst.field);
            Set(R, "$Elemtype" , amc::NsToCpp(base->ctype));
            Ins(&R, fcn.body, "case $ns_TableId_$basens_$basename: { // finput:$finput");
            if (CanRemoveStrptrMaybeQ(inst, *base)) {
                amc::FField *thash = PkeyIndex(inst, *base);
                amc::FField *pkey  = c_field_Find(*base, 0);
                Set(R, "$thashname", name_Get(*thash));
                Set(R, "$pkeyname" , name_Get(*pkey));
                Ins(&R, fcn.body, "    $Elemtype elem;");
                Ins(&R, fcn.body, "    retval = $Elemtype_ReadStrptrMaybe(elem, str);");
                Ins(&R, fcn.body, "    if (retval) {");
                Ins(&R, fcn.body, "        if (auto *rec = $thashname_Find(elem.$pkeyname)) {");
                Ins(&R, fcn.body, "            $instname_Delete(*rec);");
                Ins(&R, fcn.body, "        } else {");
                Ins(&R, fcn.body, "            retval = false; // not found");
                Ins(&R, fcn.body, "        }");
                Ins(&R, fcn.body, "    }");
            } else {
                Ins(&R, fcn.body, "    // finput $finput: random delete unsupported");
                Ins(&R, fcn.body, "    // (need reftype del:Y plus a Thash on the pkey)");
                Ins(&R, fcn.body, "    retval = false;");
            }
            Ins(&R, fcn.body, "    break;");
            Ins(&R, fcn.body, "}");
        }ind_end;
        Ins(&R, fcn.body, "default:");
        Ins(&R, fcn.body, "    retval = false;");
        Ins(&R, fcn.body, "    break;");
        Ins(&R, fcn.body, "} //switch");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_InitReflection() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;

    amc::FFunc& initrefl = amc::CreateCurFunc(true); {
        AddRetval(initrefl, "void", "", "");
    }
    initrefl.priv = true;
    initrefl.inl = false;
    bool has_inputs = amc::HasFinputsQ(*field.p_ctype->p_ns);
    tempstr text;// register own database
    Set(R, "$InsertStrptrMaybe", has_inputs ? "$ns::InsertStrptrMaybe" : "NULL");
    Set(R, "$RemoveStrptrMaybe", has_inputs ? "$ns::RemoveStrptrMaybe" : "NULL");
    Set(R, "$Step", c_fstep_N(ns) ? "$ns::Step" : "NULL");
    Set(R, "$MainLoop", ns.c_main ? "$ns::MainLoop" : "NULL");
    Ins(&R, initrefl.body,"algo_lib::FImdb &row = algo_lib::imdb_Alloc();");
    Ins(&R, initrefl.body,"row.imdb               = \"$ns\";");
    Ins(&R, initrefl.body,"row.InsertStrptrMaybe  = $InsertStrptrMaybe;");
    Ins(&R, initrefl.body,"row.RemoveStrptrMaybe  = $RemoveStrptrMaybe;");
    Ins(&R, initrefl.body,"row.Step               = $Step;");
    Ins(&R, initrefl.body,"row.MainLoop           = $MainLoop;");
    Ins(&R, initrefl.body,"algo_lib::imdb_XrefMaybe(row);");
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& loadssimfile = amc::CreateCurFunc(true); {
        AddProtoArg(loadssimfile,"algo::strptr","fname");
        AddProtoArg(loadssimfile,"bool","recursive");
        AddRetval(loadssimfile,"bool","retval","true");
    }
    // Loading non-existent files is considered a success
    Ins(&R, loadssimfile.body   , "if (FileQ(fname)) {");
    Ins(&R, loadssimfile.body   , "    retval = $ns::LoadTuplesFile(fname, recursive);");
    Ins(&R, loadssimfile.body   , "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Global_main() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (amc::c_fstep_N(ns) > 0 && !ns.c_main && ExeQ(ns)) {
        prerr("amc.no_main"
              <<Keyval("ns",ns.ns)
              <<Keyval("comment","Add dmmeta.main record for this namespace"));
        algo_lib::_db.exit_code++;
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
        Ins(&R, main.body    , "    algo_lib::_db.clock = algo::CurrSchedTime(); // initialize clock");

        // call last non-module main, followed by all module mains
        for (int i=c_parentns_N(ns)-1; i>=0; i--) {// call first non-module main
            amc::FNs &parentns=*c_parentns_Find(ns,i);
            if (parentns.c_main && !parentns.c_main->ismodule) {
                Set(R, "$parentns", parentns.ns);
                if (CmdlineQ(parentns)) {
                    Ins(&R, main.body, "$parentns::ReadArgv(); // dmmeta.main:$parentns");
                }
                Ins(&R, main.body, "$parentns::Main(); // user-defined main");
                break;
            }
        }

        ind_beg(amc::ns_c_parentns_curs,parentns,ns) if (parentns.c_main && parentns.c_main->ismodule) {// call all ismodule mains
            Set(R, "$parentns", parentns.ns);
            if (CmdlineQ(parentns)) {
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FNs &ns = *field.p_ctype->p_ns;
    if (ns.c_main) {
        amc::FFunc& func = amc::CreateCurFunc(true);
        func.acrkey << "main:"<<ns.ns;
        func.extrn = true;
        Ins(&R, func.ret    , "void",false);
    }
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

static void CheckBaseCmdline(amc::FCcmdline &cmdline, amc::FField &cmdfield) {
    amc::FField *basecmdline=cmdline.p_basecmdline;
    vrfy(basecmdline != &cmdfield,
         tempstr()<<"amc.circularline"
         <<Keyval("ctype",cmdline.ctype)
         <<Keyval("basecmdline",basecmdline->field)
         <<Keyval("comment","Base command line cannot be the same as the command line itself"));
    vrfy(basecmdline->field=="" || GlobalQ(*basecmdline->p_ctype)
         ,tempstr()<<"amc.baseglob"
         <<Keyval("field",basecmdline->field)
         <<Keyval("used_in",cmdline.ctype)
         <<Keyval("comment","Base command line must be global"));
    // disallow base command line to have anon fields
    ind_beg(amc::ctype_c_field_curs,checkfield,*basecmdline->p_arg) {
        if (checkfield.c_anonfld) {
            prerr("amc.badanon"
                  <<Keyval("field",checkfield.field)
                  <<Keyval("used_in",cmdline.ctype)
                  <<Keyval("comment","Base commandline cannot have anon fields"));
            algo_lib::_db.exit_code++;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Per-ctype field-aware command-line reader.  Parses ARGS -- a word array
// already split (from real argv, or from a tokenized command string) -- into
// the fields of PARENT, accumulating diagnostics in ERR.  This is the single
// intelligent parser: it consults $Name_NArgs so a bare "-opt" consumes the
// following word as its value, and it splits "-opt:value".  When the ctype
// carries a basecmdline (a tool's algo_lib.Cmdline), those base options are
// read into that global in the same pass.
void amc::GenReadArgvFunc(amc::FCtype &ctype) {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    int n_anon = c_anonfld_N(ctype);
    Set(R, "$Cpptype", ctype.cpp_type);
    amc::FCcmdline *ccmdline = ctype.c_ccmdline;
    amc::FField *basecmdline = ccmdline ? ccmdline->p_basecmdline : NULL;
    if (basecmdline && basecmdline->field == "") {
        basecmdline = NULL;
    }
    amc::FFunc &func = amc::CreateCurFunc();
    AddRetval(func, "bool", "retval", "true");
    Ins(&R, func.proto, "$Name_ReadArgv()", false);
    AddProtoArg(func, Subst(R,"$Cpptype &"), "parent");
    AddProtoArg(func, "algo::StringAry &", "args");
    AddProtoArg(func, "algo::cstring &", "err");
    func.glob = true;
    Ins(&R, func.comment, "Read command-line ARGS (already split into words) into the fields of PARENT.");
    Ins(&R, func.comment, "Field-aware: a value-taking option consumes the next word; errors go to ERR.");
    if (basecmdline) {
        Set(R,"$basecmdlinetypens", ns_Get(*basecmdline->p_arg));
        Set(R,"$basecmdlinectypename", name_Get(*basecmdline->p_arg));
        Ins(&R, func.body, tempstr()<<VarRefToGlobal(*basecmdline, "base")<<";");
    }
    Ins(&R, func.body, "int needarg=-1;// how many args the current option still wants");
    if (n_anon > 0) {
        Ins(&R, func.body, "int anonidx=0;");
        Ins(&R, func.body, "algo::strptr nextanon = $ns::$Name_GetAnon(parent, anonidx);");
    }
    Ins(&R, func.body, "algo::strptr attrname;");
    Ins(&R, func.body, "bool isanon=false; // true if attrname is anonfld (positional)");
    if (basecmdline) {
        Ins(&R, func.body, "$basecmdlinetypens::FieldId baseattrid;");
    }
    Ins(&R, func.body, "$ns::FieldId attrid;");
    Ins(&R, func.body, "bool endopt=false;");
    Ins(&R, func.body, "int whichns=0;// 0=base, 1=leaf");
    // Required-arg tracking is emitted only for tools (basecmdline); cluster
    // commands are lenient (see the required-check gate below).
    if (basecmdline) {
        ind_beg(ctype_c_field_curs,reqfield,ctype) if (CmdArgRequiredQ(reqfield)) {
            Set(R,"$reqfieldname",name_Get(reqfield));
            Ins(&R, func.body, "bool $reqfieldname_present = false;");
        }ind_end;
    }
    Ins(&R, func.body, "for (int argidx=0; argidx < ary_N(args); argidx++) {");
    Ins(&R, func.body, "    algo::strptr arg = ary_qFind(args, argidx);");
    Ins(&R, func.body, "    algo::strptr attrval;");
    Ins(&R, func.body, "    algo::strptr dfltval;");
    Ins(&R, func.body, "    bool haveval=false;");
    Ins(&R, func.body, "    bool dash=elems_N(arg)>1 && arg.elems[0]=='-'; // a single dash is not an option");
    Ins(&R, func.body, "    if (endopt || needarg>0 || !dash) {");
    Ins(&R, func.body, "        attrval=arg;");
    Ins(&R, func.body, "        haveval=true;");
    Ins(&R, func.body, "    } else {");
    Ins(&R, func.body, "        bool dashdash = elems_N(arg) >= 2 && arg.elems[1]=='-';");
    Ins(&R, func.body, "        int skip = int(dash) + dashdash;");
    Ins(&R, func.body, "        attrname=ch_RestFrom(arg,skip);");
    Ins(&R, func.body, "        if (skip==2 && elems_N(arg)==2) {");
    Ins(&R, func.body, "            endopt=true;");
    Ins(&R, func.body, "            continue;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        algo::i32_Range colon = TFind(attrname,':');");
    Ins(&R, func.body, "        if (colon.beg < colon.end) {");
    Ins(&R, func.body, "            attrval=ch_RestFrom(attrname,colon.end);");
    Ins(&R, func.body, "            attrname=ch_FirstN(attrname,colon.beg);");
    Ins(&R, func.body, "            haveval=true;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        whichns=0;");
    Ins(&R, func.body, "        needarg=-1;");
    if (basecmdline) {
        Ins(&R, func.body, "        if ($basecmdlinetypens::FieldId_ReadStrptrMaybe(baseattrid,attrname)) {");
        Ins(&R, func.body, "            needarg = $basecmdlinetypens::$basecmdlinectypename_NArgs(baseattrid,dfltval,&isanon);");
        Ins(&R, func.body, "        }");
    }
    Ins(&R, func.body, "        if (needarg<0) {");
    Ins(&R, func.body, "            whichns=1;");
    Ins(&R, func.body, "            if ($ns::FieldId_ReadStrptrMaybe(attrid,attrname)) {");
    Ins(&R, func.body, "                needarg = $ns::$Name_NArgs(attrid,dfltval,&isanon);");
    Ins(&R, func.body, "            }");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        if (attrval == \"\" && dfltval != \"\") {");
    Ins(&R, func.body, "            attrval=dfltval;");
    Ins(&R, func.body, "            haveval=true;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        if (needarg<0) {");
    Ins(&R, func.body, "            err<<\"$Name: unknown option \"<<Keyval(\"value\",arg)<<eol;");
    Ins(&R, func.body, "        } else {");
    if (n_anon > 0) {
        Ins(&R, func.body, "            if (isanon) {");
        Ins(&R, func.body, "                if (attrname == nextanon) { // named positional given as -name: treat as unnamed");
        Ins(&R, func.body, "                    attrname = \"\";");
        Ins(&R, func.body, "                } else if (nextanon != \"\") { // disallow out-of-order positional");
        Ins(&R, func.body, "                    err<<\"$Name: error at \"<<algo::strptr_ToSsim(arg)<<\": must be preceded by [-\"<<nextanon<<\"]\"<<eol;");
        Ins(&R, func.body, "                }");
        Ins(&R, func.body, "            }");
    }
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    if (n_anon > 0) {
        Ins(&R, func.body, "    if (ch_N(attrname) == 0) { // positional: assign to the next anonfld");
        Ins(&R, func.body, "        attrname = nextanon;");
        Ins(&R, func.body, "        nextanon = $ns::$Name_GetAnon(parent, ++anonidx);");
        Ins(&R, func.body, "        $ns::FieldId_ReadStrptrMaybe(attrid,attrname);");
        Ins(&R, func.body, "        whichns=1;");
        Ins(&R, func.body, "    }");
    }
    Ins(&R, func.body, "    if (ch_N(attrname) == 0) {");
    Ins(&R, func.body, "        err << \"$Name: too many arguments. error at \"<<algo::strptr_ToSsim(arg)<<eol;");
    Ins(&R, func.body, "    } else if (haveval) {");
    Ins(&R, func.body, "        bool ret=false;");
    if (basecmdline) {
        Ins(&R, func.body, "        if (whichns == 0) {");
        Ins(&R, func.body, "            ret=$basecmdlinetypens::$basecmdlinectypename_ReadFieldMaybe(base, attrname, attrval);");
        Ins(&R, func.body, "        }");
    }
    Ins(&R, func.body, "        if (whichns==1) {");
    Ins(&R, func.body, "            ret=$ns::$Name_ReadFieldMaybe(parent, attrname, attrval);");
    if (basecmdline) {
        Ins(&R, func.body, "            switch(attrid.value) {");
        ind_beg(ctype_c_field_curs,reqfield,ctype) if (CmdArgRequiredQ(reqfield)) {
            Set(R,"$reqfieldname",name_Get(reqfield));
            Ins(&R, func.body, "            case $ns_FieldId_$reqfieldname: $reqfieldname_present=true; break;");
        }ind_end;
        Ins(&R, func.body, "                default:break;");
        Ins(&R, func.body, "            }");
    }
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        if (!ret) {");
    Ins(&R, func.body, "            err<<\"$Name: error in \"<<Keyval(\"option\",attrname)<<Keyval(\"value\",attrval)<<eol;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        needarg--;");
    Ins(&R, func.body, "        if (needarg <= 0) {");
    Ins(&R, func.body, "            attrname=\"\";// forget which argument was being filled");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
    bool has_required = false;
    bool has_help = false;
    ind_beg(ctype_c_field_curs,reqfield,ctype) {
        if (CmdArgRequiredQ(reqfield)) {
            has_required=true;
        }
        if (name_Get(reqfield) == "help") {
            has_help=true;
        }
    }ind_end;
    // Required-arg enforcement applies only to tools (which carry a basecmdline);
    // cluster commands are lenient -- their optional fields use dflt:"" and the
    // handler treats an empty value as "not given", matching the prior behavior.
    if (has_required && basecmdline) {
        // -help suppresses the missing-required diagnostic, where a help flag exists
        bool gate = basecmdline || has_help;
        if (gate) {
            Set(R, "$helpexpr", basecmdline ? "base.help" : "parent.help");
            Ins(&R, func.body, "if (!$helpexpr) {");
        }
        ind_beg(ctype_c_field_curs,reqfield,ctype) if (CmdArgRequiredQ(reqfield)) {
            Set(R,"$reqfieldname",name_Get(reqfield));
            Ins(&R, func.body, "    if (!$reqfieldname_present) {");
            Ins(&R, func.body, "        err << \"$Name: Missing value for required argument -$reqfieldname (see -help)\" << eol;");
            Ins(&R, func.body, "    }");
        }ind_end;
        if (gate) {
            Ins(&R, func.body, "}");
        }
    }
    Ins(&R, func.body, "retval = (ch_N(err) == 0);");
}

// Report one function the generated argv reader READER calls that amc does not
// emit, and fail the run.
//
// A reader is assembled from companion functions -- $Name_NArgs for how many
// words an option takes, $Name_GetAnon for the next positional, $Name_ReadFieldMaybe
// for the store, and the base command line's NArgs and ReadFieldMaybe when the
// ctype carries a basecmdline -- and the namespace-level ReadArgv is a call to
// the cmdline ctype's reader.  Each of those has an emit gate of its own, and
// the gates read different rows: a reader and its NArgs come from a read cfmt
// with strfmt:Argv, while ReadFieldMaybe and GetAnon come from a read cfmt whose
// printfmt is neither Raw nor Extern.  A ctype that satisfies one gate and not
// the other yields a reader calling a function amc never emits, which nothing
// but the C++ link reports.  This is that report.
static void ReportArgvCall(strptr reader, strptr callee, strptr comment) {
    prerr("amc.badargvread"
          <<Keyval("reader",reader)
          <<Keyval("callee",callee)
          <<Keyval("comment",comment));
    algo_lib::_db.exit_code++;
}

// True when CTYPE has a field-name reader ($Name_ReadFieldMaybe, and
// $Name_GetAnon with it).  A printfmt of Raw or Extern says the ctype's read is
// written by hand, so neither function is generated for it.  This is narrower
// than amc::HasReadQ, which answers only whether some cfmt reads at all.
static bool HasFieldReadQ(amc::FCtype &ctype) {
    bool ret = false;
    ind_beg(amc::ctype_zs_cfmt_curs, cfmt, ctype) if (cfmt.read) {
        if (cfmt.printfmt != dmmeta_Printfmt_printfmt_Raw
            && cfmt.printfmt != dmmeta_Printfmt_printfmt_Extern) {
            ret = true;
            break;
        }
    }ind_end;
    return ret;
}

// Report every companion of CTYPE's argv reader that amc does not emit.
static void CheckArgvRead(amc::FCtype &ctype) {
    amc::FCcmdline *ccmdline = ctype.c_ccmdline;
    amc::FField *basecmdline = ccmdline ? ccmdline->p_basecmdline : NULL;
    amc::FCtype *basectype = (basecmdline && basecmdline->field != "") ? basecmdline->p_arg : NULL;
    bool fieldread = HasFieldReadQ(ctype);
    tempstr reader;
    reader << ns_Get(ctype) << "::" << name_Get(ctype) << "_ReadArgv";
    if (!fieldread && c_anonfld_N(ctype) > 0) {
        ReportArgvCall(reader, tempstr()<<name_Get(ctype)<<"_GetAnon"
                       ,"ctype needs a read cfmt whose printfmt is neither Raw nor Extern");
    }
    if (!fieldread) {
        ReportArgvCall(reader, tempstr()<<name_Get(ctype)<<"_ReadFieldMaybe"
                       ,"ctype needs a read cfmt whose printfmt is neither Raw nor Extern");
    }
    if (basectype && !amc::HasArgvReadQ(*basectype)) {
        ReportArgvCall(reader, tempstr()<<name_Get(*basectype)<<"_NArgs"
                       ,"basecmdline ctype needs a cfmt with strfmt:Argv and read:Y");
    }
    if (basectype && !HasFieldReadQ(*basectype)) {
        ReportArgvCall(reader, tempstr()<<name_Get(*basectype)<<"_ReadFieldMaybe"
                       ,"basecmdline ctype needs a read cfmt whose printfmt is neither Raw nor Extern");
    }
}

// Emit $Name_ReadArgv only for ctypes that declare a readable strfmt:Argv
// cfmt (command lines), the same gate tfunc_Ctype_NArgs uses.
void amc::tfunc_Ctype_ReadArgv() {
    amc::FCtype &ctype = *amc::_db.genctx.p_ctype;
    if (amc::HasArgvReadQ(ctype)) {
        CheckArgvRead(ctype);
        GenReadArgvFunc(ctype);
    }
}

// Emit the load of one tuple source into the command's ReadArgv function.
//
// A command names its sources as dmmeta.floadtuples rows, and each row becomes one
// LoadTuplesMaybe call here.  A source that names nothing is a hard error for the primary
// root: a command run from the wrong directory finds no data/ and has to say so rather
// than run against an empty database.  A layer is the opposite case, because a checkout
// with no inventory attached names a directory that is not there and is meant to proceed
// with the rows the primary root already gave it.  That is what the row's optional flag
// selects, by putting an existence test in front of the load.
static void GenLoadTuples(algo_lib::Replscope &R, amc::FFunc &func, amc::FFloadtuples &floadtuples) {
    Set(R,"$loadtuplesfield",floadtuples.field);
    Set(R,"$loadtuplesname",name_Get(*floadtuples.p_field));
    tempstr cond("!dohelp && err==\"\"");
    if (floadtuples.optional) {
        cond << Subst(R," && (DirectoryQ(cmd.$loadtuplesname) || FileQ(cmd.$loadtuplesname))");
    }
    Set(R,"$loadtuplescond",cond);
    Ins(&R, func.body, "// dmmeta.floadtuples:$loadtuplesfield");
    Ins(&R, func.body, "if ($loadtuplescond) {");
    Ins(&R, func.body, "    algo_lib::ResetErrtext();");
    Ins(&R, func.body, "    if (!$ns::LoadTuplesMaybe(cmd.$loadtuplesname,true)) {");
    Ins(&R, func.body, "        err << \"$ns.load_input  \"<<algo_lib::DetachBadTags()<<eol;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "}");
}

// -----------------------------------------------------------------------------

// Namespace ReadArgv function to read command line
void amc::tfunc_Global_ReadArgv() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field; // atf_amc_cmd.FDb._db
    amc::FNs &ns = *field.p_ctype->p_ns; // atf_amc_cmd
    amc::FField *cmdfield = FindCmdlineField(ns); // atf_amc_cmd.FDb.cmdline
    amc::FCcmdline *ccmdline = FindCcmdline(ns);
    if (ns.c_main && ccmdline) {
        amc::FCtype *cmdline_ctype = ccmdline->p_ctype;
        amc::FFunc& func = amc::CreateCurFunc(true);
        Ins(&R, func.ret    , "void",false);
        Set(R,"$cmdlinefield"  , cmdfield->field);
        Set(R,"$cmdlinetypens"  , ns_Get(*cmdline_ctype));
        Set(R,"$cmdlinectypename", name_Get(*cmdline_ctype));
        Set(R,"$cmdlinecpptype", cmdline_ctype->cpp_type);
        amc::FField *basecmdline=ccmdline->p_basecmdline;
        Set(R,"$basecmdlinefield"  , basecmdline->field);
        CheckBaseCmdline(*ccmdline, *cmdfield);
        if (!amc::HasArgvReadQ(*cmdline_ctype)) {
            ReportArgvCall(Subst(R,"$ns::ReadArgv"), Subst(R,"$cmdlinectypename_ReadArgv")
                           ,"cmdline ctype needs a cfmt with strfmt:Argv and read:Y");
        }
        if (basecmdline->field == "") {
            basecmdline = NULL;
        }
        Ins(&R, func.comment, "Read argc,argv into the fields of $cmdlinefield (and any base command line)");
        Ins(&R, func.comment, "via $cmdlinectypename_ReadArgv; then apply -help/-version and load floadtuples input.");
        Ins(&R, func.body, tempstr()<<VarRefToGlobal(*cmdfield, "cmd")<<";");
        Ins(&R, func.body, "algo::cstring err;");
        Ins(&R, func.body, "algo::StringAry args;");
        Ins(&R, func.body, "for (int argidx=1; argidx < algo_lib::_db.argc; argidx++) {// skip process name");
        Ins(&R, func.body, "    ary_Alloc(args) = algo_lib::_db.argv[argidx];");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "$cmdlinetypens::$cmdlinectypename_ReadArgv(cmd, args, err);");
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
            Ins(&R, func.body, "algo_lib_logcat_debug.enabled = algo_lib::_db.cmdline.debug;");
            Ins(&R, func.body, "algo_lib_logcat_verbose.enabled = algo_lib::_db.cmdline.verbose > 0;");
            Ins(&R, func.body, "algo_lib_logcat_verbose2.enabled = algo_lib::_db.cmdline.verbose > 1;");
        }

        // missing-required diagnostics are emitted inside $cmdlinectypename_ReadArgv

        // post-processing steps: one load per tuple source the command declares.
        // Pass 0 emits the primary roots and pass 1 the layers, so a row a layer supplies
        // resolves its references against what the primary root has already inserted.  The
        // reverse order fails on the first child whose parent has not arrived yet.
        for (int pass = 0; pass <= 1; pass++) {
            bool layer = pass == 1;
            ind_beg(amc::ctype_c_floadtuples_curs, floadtuples, *cmdline_ctype) {
                if (floadtuples.optional == layer) {
                    GenLoadTuples(R, func, floadtuples);
                }
            }ind_end;
        }

        Ins(&R, func.body, "if (err != \"\") {");
        Ins(&R, func.body, "    algo_lib::_db.exit_code=1;");
        Ins(&R, func.body, "    prerr_(err); // already has eol");
        Ins(&R, func.body, "    doexit=true;");
        Ins(&R, func.body, "}");

        Ins(&R, func.body, "if (dohelp) {");
        Ins(&R, func.body, "    prlog($cmdlinecpptype_help);");
        Ins(&R, func.body, "}");

        Ins(&R, func.body, "if (doexit) {");
        Ins(&R, func.body, "    _exit(algo_lib::_db.exit_code);");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "algo_lib::ResetErrtext();");
    }
}
