// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/cget.cpp -- Getters / Setters
//

#include "include/amc.h"


//returns NULL if the function already exists.
static amc::FFunc* CreateFunc(amc::FCtype& ctype, strptr fconst){
    tempstr name(fconst);
    Translate(name, ".", "_");
    algo_lib::Replscope R;
    Set(R, "$Ns"    , ns_Get(ctype));
    Set(R, "$Ctype" , name_Get(ctype));
    Set(R, "$Fname" , name);
    tempstr fname(Subst(R, "$Ns.$Ctype..$Fname"));
    amc::FFunc* func = amc::ind_func_Find(fname);
    if(!func){
        func = &amc::ind_func_GetOrCreate(fname);
        func->ret = "bool";
        func->inl = true;
        Ins(&R, func->proto, "$Fname(const $Ns::$Ctype& row)", false);
        amc::cd_temp_func_Insert(*func);
    }else{
        if(cd_temp_func_InLlistQ(*func)){
            func->disable = true;
        }
        func = NULL;
    }
    return func;
}

static void GenBoolFunc(amc::FCtype& ctype, amc::FField& field){
    algo_lib::Replscope R;
    Set(R, "$Field" , name_Get(field));
    amc::FFunc* func = CreateFunc(ctype, Subst(R, "$FieldQ"));
    if(func){
        Set(R, "$Field" , FieldvalExpr(NULL, field, "row"));
        Ins(&R, func->body, "return $Field;");
    }
}

static void GenInlineFconstFunc(amc::FCtype& ctype, amc::FField& field, amc::FFconst& fconst){
    algo_lib::Replscope R;
    Set(R, "$Field"    , FieldvalExpr(NULL, field, "row"));
    Set(R, "$Fconst"   , name_Get(fconst));
    Set(R, "$Constval" , fconst.cpp_name);
    amc::FFunc* func = CreateFunc(ctype, Subst(R, "$FconstQ"));
    if(func){
        Ins(&R, func->body, "return $Field == $Constval;");
    }

}


static void GenFarFconstFunc(amc::FCtype& ctype, amc::FField& field, amc::FCtype& arg){
    algo_lib::Replscope R;
    Set(R, "$Ns"       , ns_Get(ctype));
    Set(R, "$Ctype"    , name_Get(ctype));
    Set(R, "$Field"    , FieldvalExpr(NULL, field, "row"));
    ind_beg(amc::ctype_c_field_curs, farfield, arg) {
        ind_beg(amc::field_c_fconst_curs, fconst, farfield){
            Set(R, "$Fconst"   , name_Get(fconst));
            Set(R, "$Farfield" , FieldvalExpr(NULL, farfield, Subst(R, "$Field")));
            Set(R, "$Constval" , fconst.cpp_name);
            amc::FFunc* func = CreateFunc(ctype, Subst(R, "$FconstQ"));
            if(func){
                Ins(&R, func->body, "return $Farfield == $Constval;");
            }
        }ind_end;
    }ind_end;
}

static bool HasFconst(amc::FCtype& ctype){
    (void)ctype;
    bool result = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype){
        result |= c_fconst_N(field) > 0;
        if(result){
            break;
        }
    }ind_end;
    return result;
}

static void GenCtype(amc::FCtype& ctype){
    ind_beg(amc::ctype_c_field_curs, field, ctype) if(!field.synthetic){
        if(field.arg == "bool"){
            GenBoolFunc(ctype, field);
        }
        ind_beg(amc::field_c_fconst_curs, fconst, field){
            GenInlineFconstFunc(ctype, field, fconst);
        }ind_end;

        if(HasFconst(*field.p_arg)){
            GenFarFconstFunc(ctype, field, *field.p_arg);
        }
    }ind_end;
    amc::cd_temp_func_RemoveAll();
}

void amc::gen_cget(){
    ind_beg(amc::_db_cget_curs, cget, amc::_db){
        GenCtype(*cget.p_ctype);
    }ind_end;
}
