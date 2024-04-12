// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/amc/fcmap.cpp -- Fcmap generation
//

#include "include/amc.h"

// ignore:ptr_byref
static void SwapPtrMaybe(bool dir, amc::FField*& one, amc::FField*& two){
    if(dir){
        amc::FField* tmp = one;
        one = two;
        two = tmp;
    }
}

static bool DependentQ(amc::FField* left, amc::FField* right){
    bool result = false;
    ind_beg(amc::ns_c_cppincl_curs, usedNs, *left->p_ctype->p_ns){
        result |= (&usedNs == right->p_ctype->p_ns);
    }ind_end;
    ind_beg(amc::ns_c_hdrincl_curs, usedNs, *left->p_ctype->p_ns){
        result |= (&usedNs == right->p_ctype->p_ns);
    }ind_end;
    ind_beg(amc::ns_c_parentns_curs, usedNs, *left->p_ctype->p_ns){
        result |= (&usedNs == right->p_ctype->p_ns);
    }ind_end;
    return result;
}



static tempstr GetCppNameForVal(amc::FField* field, const algo::Smallstr40& val, bool booltoint){
    tempstr result(val);
    tempstr fconstName(field->field);
    fconstName <<'/'<< val;
    amc::FFconst* fconst = amc::ind_fconst_Find(fconstName);
    amc::FCtype* boolType = amc::ind_ctype_Find("bool");
    if(fconst == NULL && boolType != field->p_arg){
        prerr("amc.gen_fcmap"
              <<Keyval("val", val)
              <<Keyval("comment","unknown constant used"));
        algo_lib::_db.exit_code++;
    }
    bool boolValQ = (val == "true" || val == "false");
    //clang compiler complains about switch(bool), so we substitute it with
    //switch(int) by assigning 0 to false and 1 to true.
    if(boolValQ && booltoint){
        result = cstring(val=="true" ? "1" : "0");
    }
    if(boolType == field->p_arg && !boolValQ){
        prerr("amc.gen_fcmap"
              <<Keyval("val", val)
              <<Keyval("comment", "bool can only be true/false"));
        algo_lib::_db.exit_code++;
    }

    if(fconst != NULL)
        result = fconst->cpp_name;

    return result;
}

static void CheckFieldVal(amc::FField* field, amc::FField* otherField, const dmmeta::FieldPkey& val){
    int match = 0;
    int regexpMatch = 0;
    ind_beg(amc::field_zs_fcmap_curs, fcmap, *field) if (fcmap.p_rightField == otherField){
        match       += (leftVal_Get(fcmap) == val);
        regexpMatch += (leftVal_Get(fcmap) == "*");
    }ind_end;
    if( i32_Max(match, regexpMatch) != 1){
        prerr("amc.gen_fcmap"
              <<Keyval("field1", field->field)
              <<Keyval("field2", otherField->field)
              <<Keyval("const", val)
              <<Keyval("matchN", match)
              <<Keyval("regexpN", regexpMatch)
              <<Keyval("comment","must be present exactly once")
              );
        algo_lib::_db.exit_code++;
    }
}

static void GenerateFcmapSwitch(algo_lib::Replscope& R ,amc::FFunc& func, amc::FField* left, amc::FField* right){
    ind_beg(amc::field_zs_fcmap_curs, fcmap, *right){
        if(fcmap.p_rightField == left){
            Set(R, "$caseName", "default");
            if(leftVal_Get(fcmap) != "*"){
                Set(R, "$caseName", tempstr()<<"case "<<GetCppNameForVal(right, leftVal_Get(fcmap), true));
            }
            Set(R,"$assignment","" );
            if(algo::ch_N(rightVal_Get(fcmap))>0){
                Set(R,"$assignment", AssignExpr(*left, "lhs" , GetCppNameForVal(left, rightVal_Get(fcmap), false), false) );
            }
            Ins(&R, func.body,"    $caseName\t: $assignment;\tbreak;");
        }
    }ind_end;

    amc::FCtype* boolType = amc::ind_ctype_Find("bool");
    if( boolType == right->p_arg){
        CheckFieldVal(right, left, dmmeta::FieldPkey("true"));
        CheckFieldVal(right, left, dmmeta::FieldPkey("false"));
    }
    ind_beg(amc::field_c_fconst_curs, rightConst, *right){
        CheckFieldVal(right, left, name_Get(rightConst));
    }ind_end;
}


static void GenFCmap(amc::FField* left, amc::FField* right, bool dir){
    algo_lib::Replscope R;
    Set(R, "$NSleft"    , ns_Get(*left));
    Set(R, "$Typeleft"  , name_Get(*left->p_ctype));
    Set(R, "$NSright"   , ns_Get(*right));
    Set(R, "$Typeright" , name_Get(*right->p_ctype));
    Set(R, "$Dir"       , dir ? "To" : "From");
    Set(R, "$ConstRight", dir ? "" : "const");
    Set(R, "$ConstLeft" , dir ? "const" : "");
    Set(R, "$FirstArg"  , dir ? "rhs" : "lhs");
    Set(R, "$SecondArg" , dir ? "lhs" : "rhs");
    Set(R, "$RBitfld"   , "");
    Set(R, "$LBitfld"   , "");
    if(right->c_bitfld != NULL){
        Set(R, "$RBitfld", tempstr()<<'_'<<name_Get(*right));
    }
    if(left->c_bitfld != NULL){
        Set(R, "$LBitfld", tempstr()<<'_'<<name_Get(*left));
    }

    cstring funcName(     Subst(R, "$NSleft.$Typeleft..$Dir_$NSright_$Typeright$RBitfld$LBitfld"));
    cstring otherFuncName(Subst(R, "$NSright.$Typeright..$Dir_$NSleft_$Typeleft$LBitfld$RBitfld"));
    bool midwaySwap = amc::ind_func_Find(otherFuncName) != NULL;
    if( midwaySwap ) {
        prerr("amc.gen_fcmap"
              <<Keyval("field1" , left->field)
              <<Keyval("field2" , right->field)
              <<Keyval("comment","Cannot change the order of fields midway"));
        algo_lib::_db.exit_code++;
    }

    if(!midwaySwap && amc::ind_func_Find(funcName) == NULL){
        amc::FFunc &func = amc::ind_func_GetOrCreate(funcName);
        Ins(&R, func.comment, "map fields of rhs into lhs");
        func.glob = true;
        func.ret = "void";
        Ins(&R, func.proto,"$Typeleft$LBitfld_$Dir$RBitfld($ConstLeft $NSleft::$Typeleft& $FirstArg"
            ", $ConstRight $NSright::$Typeright& $SecondArg)", false);
        SwapPtrMaybe(dir,left, right);
        Set(R,"$right", FieldvalExpr(NULL, *right, "rhs"));
        //clang related: We substitute bool expression by an int expression.
        if(right->arg == "bool"){
            Set(R,"$right", "$right ? 1 : 0");
        }
        Ins(&R, func.body,    "switch($right){");
        GenerateFcmapSwitch(R, func, left, right);
        Ins(&R, func.body,    "}");
        func.body = Tabulated(func.body,"\t","ll",2);
    }
}

// Get the conversion function if it exists
amc::FFunc* amc::ConvertFuncMaybe(amc::FCtype& from, amc::FCtype& to) {
    amc::FFunc* ret = NULL;
    ind_beg(amc::_db_fcmap_curs, fcmap, amc::_db) {
        if(fcmap.p_leftField->p_ctype == &from && fcmap.p_rightField->p_ctype == &to) {
            ret = fcmap.c_convfunc;
            break;
        }
    } ind_end;

    return ret;
}

// Generate
static amc::FFunc* GenCreateFrom(amc::FField* left, amc::FField* right, bool dir){
    amc::FFunc* func = NULL;
    algo_lib::Replscope R;
    Set(R, "$NSleft"    , ns_Get(*left));
    Set(R, "$Typeleft"  , name_Get(*left->p_ctype));
    Set(R, "$NSright"   , ns_Get(*right));
    Set(R, "$Typeright" , name_Get(*right->p_ctype));
    Set(R, "$Dir"       , dir ? "To" : "From");
    cstring funcName(Subst(R, "$NSleft.$Typeleft..CreateFrom_$NSright_$Typeright$Dir"));
    bool bitfields = right->c_bitfld || left->c_bitfld;
    func = amc::ind_func_Find(funcName);
    if(func == NULL && !bitfields){
        func = &amc::ind_func_GetOrCreate(funcName);
        func->glob = true;

        Set(R, "$Ret", Subst(R, dir ? "$NSright::$Typeright" : "$NSleft::$Typeleft"));
        func->ret = Subst(R, "$Ret");
        Set(R, "$FuncName", Subst(R, dir ? "$NSright_$Typeright_CreateFrom" : "$NSleft_$Typeleft_CreateFrom"));
        Ins(&R, func->proto, dir ? "$FuncName(const $NSleft::$Typeleft& lhs)"
            : "$FuncName(const $NSright::$Typeright& lhs)", false);
        Ins(&R, func->body,"$Ret result;");
        Ins(&R, func->body, dir ? "$Typeleft_$Dir(lhs, result);" : "$Typeleft_$Dir(result, lhs);");
        Ins(&R, func->body,"return result;");
    }

    return func;
}

//Swap left with right in fcmap rows with bidir:Y and add them to fcmap
static void ProcessBidir(){
    ind_beg(amc::_db_fcmap_curs, fcmap, amc::_db){
        dmmeta::Fcmap reflected;
        reflected.fcmap = dmmeta::Fcmap_Concat_leftField_leftVal_rightField_rightVal(rightField_Get(fcmap),
                                                                                     rightVal_Get(fcmap),
                                                                                     leftField_Get(fcmap),
                                                                                     leftVal_Get(fcmap));
        if(fcmap.bidir){
            //Reflected entries are not bidirectional, would cause infinite copying.
            reflected.bidir = false;
            amc::fcmap_InsertMaybe(reflected);
        }else{
            amc::FFcmap* other = amc::ind_fcmap_Find(reflected.fcmap);
            //Recommend reflection if my reflection exists and I am not a reflection.
            if( other != NULL && !other->bidir){
                prlog("amc.fcmap_bidir_candidate"<<
                      Keyval("fcmap", fcmap.fcmap) <<
                      Keyval("bidir", fcmap.bidir));
            }
        }
    }ind_end;
}

//TODO::warn about the following case:
//A.b = A.c, A.d = A.e. That setup yields only one function A_FromA() instead of 2 (and a compile error).
void amc::gen_fcmap(){
    ProcessBidir();
    ind_beg(amc::_db_fcmap_curs, fcmap, amc::_db){
        //prlog("dmmeta.fcmap  fcmap:\""<<fcmap.fcmap<<'\"');
        amc::FField* left = fcmap.p_leftField;
        amc::FField* right = fcmap.p_rightField;
        if(!DependentQ(left,right) && !DependentQ(right,left)){
            prerr("amc.fcmap_independent_ns"
                  <<Keyval("ctype1",left->p_ctype->ctype)
                  <<Keyval("ctype2",right->p_ctype->ctype)
                  <<Keyval("comment","Specify correct dependency by targdep or variable relations"));
            algo_lib::_db.exit_code++;
        }

        bool dir = DependentQ(left, right);
        SwapPtrMaybe(!dir, left , right);
        GenFCmap(left, right, dir);
        fcmap.c_convfunc = GenCreateFrom(left, right, dir);
    }ind_end;
}
