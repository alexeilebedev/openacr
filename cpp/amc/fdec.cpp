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
// Source: cpp/amc/fdec.cpp
//
// Created By: alexei.lebedev jeffrey.wang
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev jeffrey.wang hayk.mkrtchyan
//

#include "include/amc.h"

void amc::tclass_Dec() {
}

void amc::tfunc_Dec_GetDouble() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$fromdbl", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$pararg"));

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "double",false);
    Ins(&R, func.proto, "$name_GetDouble($Cparent)",false);
    Ins(&R, func.body, "double ret;");
    Ins(&R, func.body, "ret = $fldval / double($fromdbl);");
    Ins(&R, func.body, "return ret;");
}

void amc::tfunc_Dec_GetInt() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$scale", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$pararg"));

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "$Cpptype",false);
    Ins(&R, func.comment  , "Return integer portion (divide number by $scale)",false);
    Ins(&R, func.proto, "$name_GetInt($Cparent)",false);
    Ins(&R, func.body, "$Ctype ret;");
    Ins(&R, func.body, "ret = $fldval / $scale;");
    Ins(&R, func.body, "return ret;");
}

void amc::tfunc_Dec_GetScale() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$scale", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$Name", name_Get(*field.p_ctype));

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "$Cpptype",false);
    Ins(&R, func.comment  , "Return constant $scale",false);
    Ins(&R, func.proto, "$Name_GetScale()",false);
    Ins(&R, func.body, "return $scale;");
}

void amc::tfunc_Dec_qSetDouble() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$fromdbl", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$pararg"));

    amc::FFunc& qsetdbl = amc::CreateCurFunc();
    qsetdbl.inl = true;
    Ins(&R, qsetdbl.comment, "Set value of field $name.");
    Ins(&R, qsetdbl.comment, "The value is rounded to the nearest integer.");
    Ins(&R, qsetdbl.comment, "This ensures that truncation of a near-integer value does not occur.");
    Ins(&R, qsetdbl.comment, "Example: 1.3 cannot be represented exactly as a double, the actual");
    Ins(&R, qsetdbl.comment, "stored value will be 1.29999999. when we apply C truncation,");
    Ins(&R, qsetdbl.comment, "we want to end up with 1.3, not 1.2.");
    Ins(&R, qsetdbl.ret  , "void",false);
    Ins(&R, qsetdbl.proto, "$name_qSetDouble($Parent, double val)",false);
    Ins(&R, qsetdbl.body, "double intval = val * $fromdbl + (val > 0 ? 0.5 : -0.5);");
    // determine min and max value for the field.
    Ins(&R, qsetdbl.body, "// set underlying field.");
    Ins(&R, qsetdbl.body, AssignExpr(field, "$parname", "intval", false)<<";");
}

void amc::tfunc_Dec_SetDoubleMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$fromdbl", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$ctype", field.arg);

    amc::FFunc& setdoublex = amc::CreateCurFunc();
    Ins(&R, setdoublex.comment, "Set value of field $name, using rounding.");
    Ins(&R, setdoublex.comment, "If value is out of range for the target type, return false.");
    Ins(&R, setdoublex.ret  , "bool",false);
    Ins(&R, setdoublex.proto, "$name_SetDoubleMaybe($Parent, double val)",false);
    Ins(&R, setdoublex.body, "double intval = val * $fromdbl;");
    // find expressions for min/max value of the field
    // type of expression is u64
    u64 minval=0;
    u64 maxval=0;
    bool issigned=false;
    amc::GetMinMax(*field.p_arg, minval, maxval, issigned);
    if (issigned) {
        Ins(&R, setdoublex.body, tempstr()<<"$ctype minval = $ctype("<<i64(minval)<<"LL);");
        Ins(&R, setdoublex.body, tempstr()<<"$ctype maxval = $ctype("<<i64(maxval)<<"LL);");
    } else {
        Ins(&R, setdoublex.body, tempstr()<<"$ctype minval = $ctype("<<u64(minval)<<"ULL);");
        Ins(&R, setdoublex.body, tempstr()<<"$ctype maxval = $ctype("<<u64(maxval)<<"ULL);");
    }
    Set(R, "$setexpr", AssignExpr(field, "$parname", "intval", false));
    Ins(&R, setdoublex.body, "bool retval = intval >= minval;");
    Ins(&R, setdoublex.body, "retval &= intval < maxval;");
    Ins(&R, setdoublex.body, "if (retval) {");
    Ins(&R, setdoublex.body, "    intval = intval + (val > 0 ? 0.5 : -0.5);");
    Ins(&R, setdoublex.body, "    $setexpr; // set underlying field.");
    Ins(&R, setdoublex.body, "}");
    Ins(&R, setdoublex.body, "return retval;");
}

void amc::tfunc_Dec_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFdec &fdec = *field.c_fdec;

    u64 minval=0;
    u64 maxval=0;
    bool issigned=false;
    amc::GetMinMax(*field.p_arg, minval, maxval, issigned);
    Set(R, "$minval", (issigned ? (tempstr()<<i64(minval)<<"LL") : (tempstr()<<u64(minval)<<"ULL")));
    Set(R, "$maxval", (issigned ? (tempstr()<<i64(maxval  )<<"LL") : (tempstr()<<u64(maxval)<<"ULL")));
    Set(R, "$nplace", tempstr() << fdec.nplace);

    // default read function
    // this function can be much improved. instead of reading a double, simply parse the dot
    // and adjust number later.
    if (HasReadQ(*field.p_ctype)) {
        amc::FFunc& tryread = amc::CreateCurFunc();
        Ins(&R, tryread.ret  , "bool",false);
        Ins(&R, tryread.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr in)",false);
        Ins(&R, tryread.body, "int index = 0;");
        Ins(&R, tryread.body, "int neg = false;");
        Ins(&R, tryread.body, "// skip leading sign");
        Ins(&R, tryread.body, "if (index < in.n_elems && (in.elems[index] == '+' || in.elems[index] == '-')) {");
        Ins(&R, tryread.body, "    neg = in.elems[index]=='-';");
        Ins(&R, tryread.body, "    index++;");
        Ins(&R, tryread.body, "};");
        Ins(&R, tryread.body, "// parse digits");
        Ins(&R, tryread.body, "u64 val = 0;");
        Ins(&R, tryread.body, "int ndot = 0; // number of . encountered");
        Ins(&R, tryread.body, "int nfrac = 0; // read up to NFRAC digits after .; if not found, fill in");
        Ins(&R, tryread.body, "bool ok = true;// overflow or bad char");
        Ins(&R, tryread.body, "for (; index < in.n_elems; index++) {");
        Ins(&R, tryread.body, "    char c = in.elems[index];");
        Ins(&R, tryread.body, "    if (c == '.') {");
        Ins(&R, tryread.body, "        ok &= ndot == 0; // max 1 dot");
        Ins(&R, tryread.body, "        ndot++;");
        Ins(&R, tryread.body, "    } else if (algo_lib::DigitCharQ(c)) {");
        Ins(&R, tryread.body, "        if (nfrac < $nplace) { // ignore digits after $nplace'th");
        Ins(&R, tryread.body, "            u64 newval = val*10 + (c-'0');");
        Ins(&R, tryread.body, "            ok &= newval >= val;");
        Ins(&R, tryread.body, "            val = newval;");
        Ins(&R, tryread.body, "            nfrac += ndot>0;");
        Ins(&R, tryread.body, "        }");
        Ins(&R, tryread.body, "    } else {");
        Ins(&R, tryread.body, "        ok = false; // unknown char");
        Ins(&R, tryread.body, "    }");
        Ins(&R, tryread.body, "}");
        Ins(&R, tryread.body, "while (nfrac < $nplace) {// insert missing");
        Ins(&R, tryread.body, "    u64 newval = val*10;");
        Ins(&R, tryread.body, "    ok &= newval >= val;");
        Ins(&R, tryread.body, "    val = newval;");
        Ins(&R, tryread.body, "    nfrac++;");
        Ins(&R, tryread.body, "}");
        if (issigned) {
            Ins(&R, tryread.body, "i64 final_val = val;");
            Ins(&R, tryread.body, "ok &= val <= $maxval; // u->i coversion, check limits");
        } else {
            Ins(&R, tryread.body, "u64 final_val = val;");
        }
        if (issigned) {
            Ins(&R, tryread.body, "if (neg) {");
            Ins(&R, tryread.body, "    final_val = -final_val;");
            Ins(&R, tryread.body, "    ok &= final_val >= $minval;");
            Ins(&R, tryread.body, "}");
        } else {
            Ins(&R, tryread.body, "ok &= !neg;");
        }
        //if (maxval < 0xffffffffffffffff) {// for 64-bit, overflow was already detected
        //    Ins(&R, tryread.body, "ok &= final_val <= $maxval; // check limits");
        //}
        Ins(&R, tryread.body, "if (ok) {");
        Ins(&R, tryread.body, tempstr()<<"    "<<AssignExpr(field, "$parname", "final_val", false)<<";// store value");
        Ins(&R, tryread.body, "}");
        Ins(&R, tryread.body, "return ok;");
    }
}

void amc::tfunc_Dec_Print() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFdec &fdec = *field.c_fdec;
    amc::FCtype& valtype = *(field).p_arg;

    u64 min=0,max=0;
    bool issigned=false;
    amc::GetMinMax(valtype,min,max,issigned);

    if (amc::HasStringPrintQ(*field.p_ctype)) {
        amc::FFunc& print = amc::CreateCurFunc();
        Ins(&R, print.ret  , "void",false);
        Ins(&R, print.proto, "$name_Print($Parent, cstring &outstr)",false);
        Set(R, "$nplace", tempstr() << field.c_fdec->nplace);
        Set(R, "$Ftype", valtype.cpp_type);
        Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$pararg"));
        Set(R, "$name", name_Get(field));
        Set(R, "$fixed", fdec.fixedfmt ? "true" : "false");
        Ins(&R, print.body, "$Ftype value = $fldval;");
        Ins(&R, print.body, "ch_Reserve(outstr, 64);");
        if (issigned) {
            Ins(&R, print.body, "if (value < 0) {");
            Ins(&R, print.body, "    ch_Alloc(outstr) = '-';");
            Ins(&R, print.body, "    value = -value;");
            Ins(&R, print.body, "}");
        }
        Ins(&R, print.body, "u8 *$name_start = (u8*)(outstr.ch_elems + outstr.ch_n);");
        Ins(&R, print.body, "outstr.ch_n += algo::u64_FmtBufDec(value, $nplace, $name_start, $fixed);");
    }
}
