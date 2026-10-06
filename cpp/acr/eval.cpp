// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/eval.cpp -- Evaluate attributes
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

void acr::Evalattr_Step(acr::FEvalattr &evalattr, algo::Tuple &tuple) {
    ch_RemoveAll(evalattr.value);
    strptr substr_expr;

    acr::FSubstr *substr = evalattr.field->c_substr;
    if (substr) {
        evalattr.attr = attr_Find(tuple, StripNs("",substr->srcfield));
        substr_expr = substr->expr.value;
    } else {
        evalattr.attr = attr_Find(tuple, name_Get(*evalattr.field), 0);
    }

    // evaluate substring expression
    if (evalattr.attr) {
        strptr str = evalattr.attr->value;
        algo::i32_Range R;
        R.beg = 0;
        R.end = elems_N(str);
        for (int i = 0; i <= elems_N(substr_expr)-3; i += 3) {
            char sep = substr_expr[i];
            char dir = substr_expr[i+1];
            char side = substr_expr[i+2];
            int l = R.beg;
            int r = R.end;
            if (dir == 'L') {
                for (; l < R.end; l++) {
                    if (str[l] == sep) {
                        r=l+1;
                        break;
                    }
                }
            } else {
                for (; r > R.beg; r--) {
                    if (str[r-1] == sep) {
                        l=r-1;
                        break;
                    }
                }
            }
            if (side=='L') {
                R.end = l;
            } else {
                R.beg = r;
            }
        }
        evalattr.val_range = R;
        evalattr.value = GetRegion(str,R.beg,R.end-R.beg);
    }

    // pick default
    // try field default. if not found, try ctype's default
    //
    // both dflt fields are c++ expressions, so this is just a heuristic.
    // heuristic works for strings (cstring_ReadCmdarg),
    // and for bools (false=>N, true=>Y)
    if (!evalattr.attr && evalattr.pick_dflt) {
        if (ch_N(evalattr.field->dflt.value) > 0) {
            algo::StringIter iter(evalattr.field->dflt.value);
            cstring_ReadCmdarg(evalattr.value, iter, true);
        } else if (evalattr.field->p_arg && evalattr.field->p_arg->c_cdflt) {
            evalattr.value = evalattr.field->p_arg->c_cdflt->dflt.value;
        }
    }

    // normalize value.
    // this involves reading the value into a known type and writing it back.
    // currently only bool requires frequent rewriting.
    if (evalattr.normalize) {
        if (evalattr.field->arg == strptr("bool",4)) {
            bool b = false;
            evalattr.value = bool_ReadStrptrMaybe(b, evalattr.value) && b ? "Y" : "N";
        }
    }
}

// -----------------------------------------------------------------------------

// Retrieve attribute of TUPLE corresponding to FIELD.
// Supports SUBSTR expressions.
void acr::EvalAttrDflt(Tuple &tuple, acr::FField &field, cstring &ret) {
    acr::FEvalattr evalattr;
    evalattr.field = &field;
    evalattr.pick_dflt = true;
    evalattr.normalize = true;
    acr::Evalattr_Step(evalattr, tuple);
    ret = evalattr.value;
}

// -----------------------------------------------------------------------------

// Locate attribute in TUPLE whose name is FIELD.NAME
// If FIELD is a fldfunc, locate the source attribute
// and apply substring expression to retrieve the value.
// The result is a tempstr.
// can this function be shared?
// I see it is being implemented in three different places
tempstr acr::EvalAttr(Tuple &tuple, acr::FField &field) {
    acr::FEvalattr evalattr;
    evalattr.field = &field;
    Evalattr_Step(evalattr, tuple);
    tempstr ret;
    ret = evalattr.value;
    return ret;
}
