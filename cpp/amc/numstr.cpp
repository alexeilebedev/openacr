// (C) AlgoEngineering LLC 2008-2013
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
// Source: cpp/amc/numstr.cpp -- Small strings
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::tclass_Numstr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;
    GetMinMax(*numstr.p_numtype, numstr.nummin, numstr.nummax, numstr.issigned);

    tempstr min_str;
    tempstr max_str;
    u64_PrintHex(numstr.nummin, min_str, 0, true, true);
    u64_PrintHex(numstr.nummax, max_str, 0, true, true);

    Set(R, "$nummin", min_str);
    Set(R, "$nummax", max_str);
    if (numstr.issigned) {
        Set(R, "$nummin", "i64($nummin)");
        Set(R, "$nummax", "i64($nummax)");
    }
    Set(R,"$Rtype", amc::NsToCpp(numstr.numtype));
    // signed type can not have '0' as padding
    if (numstr.issigned && smallstr.pad.value=="0") {
        algo_lib::_db.exit_code=1;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("comment","Warning: Signed numstr type can not be padded with '0'."));
    }
    // Don't support base 1 bacause a) it's silly amd b) we use a buffer of size 32 for
    // formatting numbers.
    vrfy(numstr.base == 256
         || numstr.base == 95
         || (numstr.base >= 2 && numstr.base <= 36), "unsupported base (must be 2..36 or 256)");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Numstr_qGetnum() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;
    double str_max  = pow(numstr.base, smallstr.length)-1;

    amc::FFunc& qgetnum = amc::CreateCurFunc();
    qgetnum.priv = true;
    Set(R, "$base", tempstr() << numstr.base);
    Set(R, "$bthresh", tempstr() << u32_Min(10,numstr.base));
    Ins(&R, qgetnum.comment, "Convert field to numeric value. If the value is too large");
    Ins(&R, qgetnum.comment, "for the target type, the result is undefined.");
    Ins(&R, qgetnum.comment, "The special case of an empty string is evaluated to zero.");
    Ins(&R, qgetnum.ret  , "$Rtype", false);
    Ins(&R, qgetnum.proto, "$name_qGetnum($Parent, u32 &ok)", false);
    Ins(&R, qgetnum.body        , "(void)ok;");
    Ins(&R, qgetnum.body        , "u64 val = 0;");
    Ins(&R, qgetnum.body        , "strptr str = $name_Getary($pararg);");
    bool fast_path = (numstr.base == 10 && smallstr.length <= 16); // fast path for base-10 conversion
    if (numstr.issigned) {
        Ins(&R, qgetnum.body    , "bool is_neg = (str.n_elems > 0) && (str.elems[0] == '-');");
        Ins(&R, qgetnum.body    , "if (is_neg) {");
        Ins(&R, qgetnum.body    , "    str.elems++;");
        Ins(&R, qgetnum.body    , "    str.n_elems--;");
        Ins(&R, qgetnum.body    , "}");
    }
    if (fast_path) {
        Set(R, "$Fcn"
            , smallstr.length <= 4 ? "aParseNum4"
            : smallstr.length <= 8 ? "aParseNum8"
            : "aParseNum16");
        Ins(&R, qgetnum.body    , "if (elems_N(str)>0) { // empty string maps to zero");
        Ins(&R, qgetnum.body    , "    val = $Fcn(str, ok);");
        Ins(&R, qgetnum.body    , "}");
    } else {
        Ins(&R, qgetnum.body    , "for (int i = 0; i < str.n_elems; i++) {");
        Ins(&R, qgetnum.body    , "    char ch = str.elems[i];");
        Ins(&R, qgetnum.body    , "    int digit = ch;");
        if (numstr.base >= 2 && numstr.base <= 36) {
            Ins(&R, qgetnum.body, "    if (ch >= '0' && ch < '0' + $bthresh) {");
            Ins(&R, qgetnum.body, "        digit -= '0';");
        }
        if (numstr.base > 10 && numstr.base <= 36) {
            Ins(&R, qgetnum.body, "    } else if (ch >= 'a' && ch <= 'a' + $base-10) {");
            Ins(&R, qgetnum.body, "        digit = digit - 'a' + 10;");
            Ins(&R, qgetnum.body, "    } else if (ch >= 'A' && ch <= 'A' + $base-10) {");
            Ins(&R, qgetnum.body, "        digit = digit - 'A' + 10;");
        }
        if (numstr.base >= 2 && numstr.base <= 36) {
            Ins(&R, qgetnum.body, "    } else {");
            Ins(&R, qgetnum.body, "        ok = 0;");
            Ins(&R, qgetnum.body, "    }");
        }
        if (numstr.base == 95) {
            Ins(&R, qgetnum.body, "    digit = u8((u8)ch - (u8)' ');");
        }
        if (str_max > 0xffffffffffffffff) {
            Ins(&R, qgetnum.body, "    // Check for 64-bit overflow inside the loop");
            Ins(&R, qgetnum.body, "    u64 r1 = val*$base + digit;");
            Ins(&R, qgetnum.body, "    ok &= (val <= r1);");
            Ins(&R, qgetnum.body, "    val = r1;");
        } else {
            Ins(&R, qgetnum.body, "    val = val*$base + digit;");
        }
        Ins(&R, qgetnum.body    , "}");
    }

    if (numstr.issigned) {
        Ins(&R, qgetnum.body    , "i64 ret = is_neg ? -val : val;");
        if ((str_max >= numstr.nummax) && (str_max < 0xffffffffffffffff)) { // 64-bit overflow is already checked
            Ins(&R, qgetnum.body    , "ok &= ret >= $nummin && ret <= $nummax;");// check for overflow
        }
        Ins(&R, qgetnum.body    , "return $Rtype(ret);");
    } else {
        if ((str_max >= numstr.nummax) && (str_max < 0xffffffffffffffff)) { // 64-bit overflow is already checked
            Ins(&R, qgetnum.body    , "ok &= val <= $nummax;");// check for overflow
        }
        Ins(&R, qgetnum.body    , "return $Rtype(val);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Numstr_GetnumDflt() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;

    amc::FFunc& getnumdflt = amc::CreateCurFunc();
    Set(R, "$base", tempstr() << numstr.base);
    Set(R, "$bthresh", tempstr() << u32_Min(10,numstr.base));
    Ins(&R, getnumdflt.comment, "Convert field to numeric value. If the value is too large");
    Ins(&R, getnumdflt.comment, "for the target type, or the string is invalid, return default value.");
    Ins(&R, getnumdflt.comment, "Empty string is evaluated to zero.");
    Ins(&R, getnumdflt.ret  , "$Rtype", false);
    Ins(&R, getnumdflt.proto, "$name_GetnumDflt($Parent, $Rtype dflt)", false);
    Ins(&R, getnumdflt.body , "u32 ok = 1;");
    Ins(&R, getnumdflt.body , "$Rtype result = $name_qGetnum($parname, ok);");
    Ins(&R, getnumdflt.body , "return ok ? result : dflt;");
}


void amc::tfunc_Numstr_Geti64() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;

    amc::FFunc& geti64 = amc::CreateCurFunc();
    Set(R, "$base", tempstr() << numstr.base);
    Ins(&R, geti64.comment, "Convert field to numeric value. If the value is too large");
    Ins(&R, geti64.comment, "for the target type, or the string is invalid, throw an exception.");
    Ins(&R, geti64.comment, "Empty string is evaluated to zero.");
    Ins(&R, geti64.ret  , "i64", false);
    Ins(&R, geti64.proto, "$name_Geti64($Parent, bool &out_ok)", false);
    Ins(&R, geti64.body , "u32 ok = 1;");
    Ins(&R, geti64.body , "i64 result = $name_qGetnum($parname, ok);");
    Ins(&R, geti64.body , "out_ok = ok != 0;");
    Ins(&R, geti64.body , "return result;");
}

void amc::tfunc_Numstr_SetnumMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;

    // Setnumx: Set string from number.
    // Conversion is performed using an auxiliary buffer.
    //
    amc::FFunc& setnum = amc::CreateCurFunc();
    Set(R, "$base", tempstr() << numstr.base);
    Ins(&R, setnum.comment, "Set string to number specified in RHS performing base-$base conversion.");
    Ins(&R, setnum.comment, "If the number is too large for the string, return false.");
    Ins(&R, setnum.ret  , "bool", false);
    Ins(&R, setnum.proto, "$name_SetnumMaybe($Parent, i64 rhs)", false);
    Ins(&R, setnum.body        , "char buf[128];");
    Ins(&R, setnum.body        , "int length = 0;");
    Ins(&R, setnum.body        , "int charpos = 64;"); // start in the middle
    if (numstr.issigned) {
        Ins(&R, setnum.body    , "u64 val = (rhs < 0 ? -rhs : rhs);");
    } else {
        Ins(&R, setnum.body    , "u64 val = rhs;");
    }

    if ((numstr.base == 10) && ((numstr.numtype == "u16") || (numstr.numtype == "i16"))) {
        Ins(&R, setnum.body    , "length = u16_FmtBuf(val, (u8*)buf + charpos);");
    } else if ((numstr.base == 10) && ((numstr.numtype == "u32") || (numstr.numtype == "i32"))) {
        Ins(&R, setnum.body    , "length = u32_FmtBuf(val, (u8*)buf + charpos);");
    } else if ((numstr.base == 10) && ((numstr.numtype == "u64") || (numstr.numtype == "i64"))) {
        Ins(&R, setnum.body    , "length = u64_FmtBuf(val, (u8*)buf + charpos);");
    } else {
        Ins(&R, setnum.body    , "charpos = sizeof(buf);");
        Ins(&R, setnum.body    , "do {");
        Ins(&R, setnum.body    , "    u32 rem = u32(val % $base);");
        Ins(&R, setnum.body    , "    val = val / $base;");
        Ins(&R, setnum.body    , "    char ch;");
        if (numstr.base == 256) {
            Ins(&R, setnum.body, "    ch = char(rem);");
        } else if (numstr.base <= 10) {
            Ins(&R, setnum.body, "    ch = char('0'+rem);");
        } else if (numstr.base > 10 && numstr.base <= 36) {
            Ins(&R, setnum.body, "    ch = rem < 10 ? char('0' + rem) : char('A' + rem - 10);");
        } else {
            Ins(&R, setnum.body, "    ch = char(rem + ' ');");
        }
        Ins(&R, setnum.body    , "    buf[--charpos] = ch;");
        Ins(&R, setnum.body    , "} while (val != 0);");
        Ins(&R, setnum.body    , "length = sizeof(buf) - charpos;");
    }
    Set(R, "$minlength", tempstr() << numstr.min_len);
    // string is guaranteed to have at least one character written to it
    // because of the do-while above
    if (numstr.min_len > 1) {
        Ins(&R, setnum.body        , "//pad string with 0s up to min_length");
        Ins(&R, setnum.body        , "while (charpos > 0 && length < $minlength) {");
        Ins(&R, setnum.body        , "    buf[--charpos] = '0';");
        Ins(&R, setnum.body        , "    ++length;");
        Ins(&R, setnum.body        , "}");
    }
    if (numstr.issigned) {
        Ins(&R, setnum.body    , "if (rhs < 0) {");
        Ins(&R, setnum.body    , "    if (charpos > 0 && buf[charpos] != '0') {");
        Ins(&R, setnum.body    , "        --charpos;");
        Ins(&R, setnum.body    , "        ++length;");
        Ins(&R, setnum.body    , "    }");
        Ins(&R, setnum.body    , "    buf[charpos] = '-';");
        Ins(&R, setnum.body    , "}");
    }
    Ins(&R, setnum.body        , "bool retval = length <= $max_length;");
    Ins(&R, setnum.body        , "if (retval) {");
    Ins(&R, setnum.body        , "    $name_qSet($pararg, strptr(buf + charpos, length));");
    Ins(&R, setnum.body        , "}");
    Ins(&R, setnum.body        , "return retval;");
}


void amc::tfunc_Numstr_ForAllStrings() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;
    Set(R, "$min_length", tempstr() << numstr.min_len);
    if (numstr.base <= 36) {
        amc::FFunc& forallstr = amc::ind_func_GetOrCreate(Subst(R,"$ns...ForAllStrings"));
        Set(R, "$issigned", numstr.issigned ? "true" : "false");
        Set(R, "$Strtype", tempstr() << "\"" << smallstr.strtype << "\"");

        if (!ch_N(forallstr.proto)) {
            Ins(&R, forallstr.comment, "Test string conversion");
            Ins(&R, forallstr.ret   , "void", false);
            Ins(&R, forallstr.proto , "ForAllStrings(void (*fcn)(algo::StringDesc&) )", false);
            Ins(&R, forallstr.body  , "algo::StringDesc desc;");
            forallstr.glob = true;
        }
        Ins(&R, forallstr.body      , "// fill out descriptor for numstr:$field");

        Ins(&R, forallstr.body      , "{");
        Ins(&R, forallstr.body      , "    desc.Geti64      = "
            "Geti64Fcn(static_cast<i64(*)($Partype&,bool&)>($name_Geti64));");
        Ins(&R, forallstr.body      , "    desc.SetnumMaybe = "
            "SetnumFcn(static_cast<bool(*)($Partype&,i64)>($name_SetnumMaybe));");
        Ins(&R, forallstr.body      , "    desc.Init        = "
            "InitFcn(static_cast<void(*)($Partype&)>($name_Init));");
        Ins(&R, forallstr.body      , "    desc.Getary      = "
            "GetaryFcn(static_cast<algo::aryptr<char>(*)(const $Partype&)>($name_Getary));");
        Ins(&R, forallstr.body      , "    desc.smallstr    = \"$field\";");
        Ins(&R, forallstr.body      , "    desc.strtype     = $Strtype;");
        Ins(&R, forallstr.body      , "    desc.pad         = $pad;");
        Ins(&R, forallstr.body      , "    desc.base        = $base;");
        Ins(&R, forallstr.body      , "    desc.issigned    = $issigned;");
        Ins(&R, forallstr.body      , "    desc.min_length  = $min_length;");
        Ins(&R, forallstr.body      , "    desc.max_length  = $max_length;");
        Ins(&R, forallstr.body      , "    desc.numtype_max = $nummax;");
        Ins(&R, forallstr.body      , "    desc.numtype_min = $nummin;");
        Ins(&R, forallstr.body      , "    fcn(desc); // call test function");
        Ins(&R, forallstr.body      , "}");
    }
}
