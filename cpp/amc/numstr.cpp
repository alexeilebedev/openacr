// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/numstr.cpp -- Small strings
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

void amc::tfunc_Numstr_Getnum() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;
    double str_max  = pow(numstr.base, smallstr.length)-1;
    bool str_max_may_not_fit_in_u64 = ((smallstr.length * log2(numstr.base)) > 8*sizeof(u64));

    amc::FFunc& qgetnum = amc::CreateCurFunc();
    Set(R, "$base", tempstr() << numstr.base);
    Set(R, "$bthresh", tempstr() << u32_Min(10,numstr.base));
    Ins(&R, qgetnum.comment, "Convert field to numeric value. If the value is too large");
    Ins(&R, qgetnum.comment, "for the target type, or the string is invalid, the result");
    Ins(&R, qgetnum.comment, "is undefined, and and_ok is set to false.");
    Ins(&R, qgetnum.comment, "Empty string is evaluated to zero.");
    Ins(&R, qgetnum.ret  , "$Rtype", false);
    Ins(&R, qgetnum.proto, "$name_Getnum($Parent, bool &and_ok)", false);
    Ins(&R, qgetnum.body        , "u64 val = 0;");
    Ins(&R, qgetnum.body        , "algo::strptr str = $name_Getary($pararg);");
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
        Ins(&R, qgetnum.body    , "    u32 ok = 1;");
        Ins(&R, qgetnum.body    , "    val = $Fcn(str, ok);");
        Ins(&R, qgetnum.body    , "    and_ok &= (ok != 0);");
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
            Ins(&R, qgetnum.body, "        and_ok = false;");
            Ins(&R, qgetnum.body, "    }");
        }
        if (numstr.base == 95) {
            Ins(&R, qgetnum.body, "    digit = u8((u8)ch - (u8)' ');");
        }
        if (str_max_may_not_fit_in_u64) {
            Ins(&R, qgetnum.body, "    // Check for 64-bit overflow inside the loop");
            Ins(&R, qgetnum.body, "    u64 r1 = val*$base + digit;");
            Ins(&R, qgetnum.body, "    and_ok &= (val <= r1);");
            Ins(&R, qgetnum.body, "    val = r1;");
        } else {
            Ins(&R, qgetnum.body, "    val = val*$base + digit;");
        }
        Ins(&R, qgetnum.body    , "}");
    }

    if (numstr.issigned) {
        Ins(&R, qgetnum.body    , "i64 ret = is_neg ? -val : val;");
        if ((str_max >= numstr.nummax) && !str_max_may_not_fit_in_u64) { // 64-bit overflow is already checked
            Ins(&R, qgetnum.body    , "and_ok &= ret >= $nummin && ret <= $nummax;");// check for overflow
        }
        Ins(&R, qgetnum.body    , "return $Rtype(ret);");
    } else {
        if ((str_max >= numstr.nummax) && !str_max_may_not_fit_in_u64) { // 64-bit overflow is already checked
            Ins(&R, qgetnum.body    , "and_ok &= val <= $nummax;");// check for overflow
        }
        Ins(&R, qgetnum.body    , "return $Rtype(val);");
    }
}


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
    Ins(&R, getnumdflt.body , "bool ok = true;");
    Ins(&R, getnumdflt.body , "$Rtype result = $name_Getnum($parname, ok);");
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
    Ins(&R, geti64.body , "out_ok = true;");
    Ins(&R, geti64.body , "i64 result = $name_Getnum($parname, out_ok);");
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
        Ins(&R, setnum.body    , "length = algo::u16_FmtBuf(val, (u8*)buf + charpos);");
    } else if ((numstr.base == 10) && ((numstr.numtype == "u32") || (numstr.numtype == "i32"))) {
        Ins(&R, setnum.body    , "length = algo::u32_FmtBuf(val, (u8*)buf + charpos);");
    } else if ((numstr.base == 10) && ((numstr.numtype == "u64") || (numstr.numtype == "i64"))) {
        Ins(&R, setnum.body    , "length = algo::u64_FmtBuf(val, (u8*)buf + charpos);");
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
    Ins(&R, setnum.body        , "    $name_SetStrptr($pararg, algo::strptr(buf + charpos, length));");
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

// -----------------------------------------------------------------------------

// TYPE is the return type
// VALTYPE is the temporary type used for calculations
// ISSIGNED is set if TYPE supports negative numbers
// MAXVAL is the maximum representable value by TYPE (either signed or unsigned)
// CHECK_OVERFLOW is true if VALTYPE is unable to represent MAXVAL*10+9, neccessitating some extra
// logic to check overflow
static void GenParseNum(strptr type, strptr valtype, bool issigned, strptr maxval, bool check_overflow, int maxdig) {
    algo_lib::Replscope R;
    Set(R, "$type", type);
    Set(R, "$valtype", valtype);
    Set(R, "$maxval", maxval);
    Set(R, "$negval", (issigned ? "-num" : "0"));
    Set(R, "$maxdig", tempstr()<<maxdig);
    amc::FFunc& func = amc::ind_func_GetOrCreate(Subst(R,"...$type_ReadStrptrMaybe"));
    Ins(&R, func.comment, "Attempt to parse $type from str");
    Ins(&R, func.comment, "Leading whitespace is silently skipped");
    Ins(&R, func.comment, "Return success value; If false, RESULT is unchanged");
    Ins(&R, func.comment, "String must be non-empty");
    Ins(&R, func.comment, "Number may prefixed with + or - (with no space after)");
    Ins(&R, func.comment, "If the value is outside of valid range for the type, it is clipped to the valid range");
    Ins(&R, func.comment, "Supported bases: 10, 16 (if string starts with 0x or 0X");
    Ins(&R, func.comment, "For hex numbers, there is no overflow (just take last N digits that fit the type)");
    Ins(&R, func.proto, Subst(R,"$type_ReadStrptrMaybe()"), false);
    AddRetval(func, "bool", "retval", "true");
    AddProtoArg(func, Subst(R,"$type&"), "result");
    AddProtoArg(func, "algo::strptr", "str");

    Ins(&R, func.body, "int index = 0;");
    Ins(&R, func.body, "bool neg=false;");
    Ins(&R, func.body, "bool hex=false;");
    Ins(&R, func.body, "$valtype num=0;");
    Ins(&R, func.body, "char c;");
    Ins(&R, func.body, "while (index < str.n_elems && algo_lib::WhiteCharQ(str.elems[index])) {");
    Ins(&R, func.body, "    index++;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (index < str.n_elems) {");
    Ins(&R, func.body, "    c = str.elems[index];");
    Ins(&R, func.body, "    index++;");
    Ins(&R, func.body, "    if (c=='+') {");
    Ins(&R, func.body, "    } else if (c=='-') {");
    Ins(&R, func.body, "        neg=true;");
    Ins(&R, func.body, "    } else if (algo_lib::DigitCharQ(c)) {");
    Ins(&R, func.body, "        num = c-'0';");
    Ins(&R, func.body, "        if (num==0 && index < str.n_elems) {");
    Ins(&R, func.body, "            c = str.elems[index];");
    Ins(&R, func.body, "            if (c == 'x' || c == 'X') {");
    Ins(&R, func.body, "                hex=true;");
    Ins(&R, func.body, "                index++;");
    Ins(&R, func.body, "            }");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "    } else {");
    Ins(&R, func.body, "        retval=false;");// bad char
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    retval= index == 0;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (hex) {");
    Ins(&R, func.body, "    if (index == str.n_elems) {");
    Ins(&R, func.body, "        retval=false;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    for (; index < str.n_elems; index++) {");
    Ins(&R, func.body, "        c = str.elems[index];");
    Ins(&R, func.body, "        u8 val;");
    Ins(&R, func.body, "        if (!algo::ParseHex1(c, val)) {");
    Ins(&R, func.body, "            break;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        num = num*16 + val;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    int lim = u32_Min(index+$maxdig-1, str.n_elems); // 1 digit already in num");
    Ins(&R, func.body, "    for (; index < lim; index++) {");
    Ins(&R, func.body, "        c = str.elems[index];");
    Ins(&R, func.body, "        if (!algo_lib::DigitCharQ(c)) {");
    Ins(&R, func.body, "            break;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        num = num*10 + (c-'0');");
    Ins(&R, func.body, "    }");
    if (check_overflow) {
        Ins(&R, func.body, "// 2nd batch of digits");
        Ins(&R, func.body, "if (index < str.n_elems) {");
        Ins(&R, func.body, "    lim = u32_Min(index+$maxdig, str.n_elems);");
        Ins(&R, func.body, "    $valtype num2 = 0;");
        Ins(&R, func.body, "    $valtype div = 1;");
        Ins(&R, func.body, "    for (; index < lim; index++) {");
        Ins(&R, func.body, "        c = str.elems[index];");
        Ins(&R, func.body, "        if (!algo_lib::DigitCharQ(c)) {");
        Ins(&R, func.body, "            break;");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "        num2 = num2*10 + (c-'0');");
        Ins(&R, func.body, "        div = div*10;");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "    if (num > $maxval/div) {");
        Ins(&R, func.body, "        num = $maxval;");
        Ins(&R, func.body, "    } else {");
        Ins(&R, func.body, "        num = num*div + num2;");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "}");
    } else {
        Ins(&R, func.body, "if (num > $maxval) {");
        Ins(&R, func.body, "    num = $maxval;");
        Ins(&R, func.body, "}");
    }
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (neg) {");
    Ins(&R, func.body, "    num = $negval;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (retval) {");
    Ins(&R, func.body, "    result = num;");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    algo_lib::SaveBadTag(\"comment\", \"$type_ReadStrptrMaybe: bad number\");");
    Ins(&R, func.body, "    algo_lib::SaveBadTag(\"value\",str);");
    Ins(&R, func.body, "}");
}

void amc::gen_parsenum() {
    GenParseNum("i8", "u32", true, "0x7f", false, 4);
    GenParseNum("u8", "u32", false, "0xff", false, 4);

    GenParseNum("i16", "u32", true, "0x7fff", false, 6);
    GenParseNum("u16", "u32", false, "0xffff", false, 6);

    GenParseNum("i32", "u64", true, "0x7fffffff", false, 12);
    GenParseNum("u32", "u64", false, "0xffffffff", false, 12);

    GenParseNum("i64", "u64", true, "0x7fffffffffffffffULL", true, 14);
    GenParseNum("u64", "u64", false, "0xffffffffffffffffULL", true, 14);

    GenParseNum("u128", "u128", false, "((u128(0xffffffffffffffffULL) << 64) | 0xffffffffffffffffULL)", true, 25);
}
