// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/concat.cpp -- Pkey constructor
//

#include "include/amc.h"

static bool CompatiblePatternsQ(amc::FSubstr* substr1, amc::FSubstr* substr2){
    int maxCheck = i32_Min(ch_N(substr1->expr.value), ch_N(substr2->expr.value));
    bool result = true;
    for(int i = 0; i <= maxCheck - 3; i += 3){
        char sep1 = ch_Getary(substr1->expr.value)[i];
        char sep2 = ch_Getary(substr2->expr.value)[i];
        char dirFrom1 = ch_Getary(substr1->expr.value)[i+1];
        char dirFrom2 = ch_Getary(substr2->expr.value)[i+1];
        char dirTo1 = ch_Getary(substr1->expr.value)[i+2];
        char dirTo2 = ch_Getary(substr2->expr.value)[i+2];

        result &= (sep1 == sep2);
        result &= (dirFrom1 == dirFrom2);
        if(dirTo1 != dirTo2 ) {
            break;
        }
    }

    return result;
}


static char FindSepOnRight(amc::FSubstr& substr){
    int num = ch_N(substr.expr.value);
    char result = 0;
    for(int i = 0; i <= num - 3; i += 3){
        char sep = ch_Getary(substr.expr.value)[i];
        char dirTo = ch_Getary(substr.expr.value)[i+2];
        result = dirTo == 'L' ? sep : result;
    }
    return result;
}

static void GenerateForCurrentCoverage(){
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField* sourceField = amc::zd_substr_params_First()->p_srcfield;
    Set(R, "$NS"    , ns_Get(*sourceField));
    Set(R, "$Type"  , name_Get(*sourceField->p_ctype));

    cstring name("Concat");
    cstring argList;
    char comma = ' ';
    ind_beg(amc::_db_zd_substr_params_curs, param, amc::_db){
        amc::FField& field = *param.p_field;
        name << "_"<< amc::name_Get(field);
        cstring ns(ns_Get(*field.p_arg));
        if(ns != ""){
            ns<<"::";
        }
        //argList << comma <<"const "<< ns <<name_Get(*field.p_arg)<<"& "<< name_Get(field)<<" ";
        argList << comma <<Argtype(field)<<" "<< name_Get(field)<<" ";
        comma = ',';
    }ind_end;
    Set(R, "$FName", name);
    Set(R, "$Args", argList);

    Set(R, "$Source", tempstr()<<name_Get(*sourceField)<<"."<<name_Get(*sourceField));
    amc::FFunc& func = amc::ind_func_GetOrCreate(Subst(R, "$NS.$Type..$FName"));
    func.ret = "tempstr";
    func.glob = true;
    Ins(&R, func.proto, "$Type_$FName($Args)", false);
    Ins(&R, func.body, "return tempstr() ", false);
    ind_beg(amc::_db_zd_substr_params_curs, param, amc::_db){
        amc::FField& field = *param.p_field;
        cstring p;
        char sep = FindSepOnRight(param);
        Set(R, "$Param", name_Get(field));
        Ins(&R, func.body, "<< $Param ", false);
        if(sep != 0){
            Ins(&R, func.body, tempstr()<<"<<\'"<<sep<<"\'", false);
        }
    }ind_end;
    Ins(&R, func.body,";");
}

void amc::GenerateSetForCoveredTrees(amc::FField& sourceField, u64 filledRange, int lastIndex){
    if(filledRange == ((u64)1 << 32) - 1){
        GenerateForCurrentCoverage();
    }
    algo::aryptr<amc::FSubstr*> substrs =  amc::c_substr_field_Getary();
    for(int i = lastIndex + 1; i < elems_N(substrs) ; ++i){
        bool compatible = true;
        amc::FSubstr* substr = substrs[i];
        ind_beg(amc::_db_zd_substr_params_curs, param, amc::_db){
            compatible &= CompatiblePatternsQ(substr, &param);
            compatible &= ((filledRange & substr->range) == 0);//no overlaps
            compatible &= (((filledRange + 1) & substr->range) != 0);//advances with no gap
        }ind_end;

        if(compatible){
            zd_substr_params_Insert(*substr);
            filledRange += substr->range;
            GenerateSetForCoveredTrees(sourceField, filledRange, i);
            filledRange -= substr->range;
            zd_substr_params_Remove(*substr);
        }
    }
}
