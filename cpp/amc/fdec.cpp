// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Exceptions: yes
// Source: cpp/amc/fdec.cpp -- Decimal types
//

#include "include/amc.h"

// VAL as a C++ decimal literal: the i64 minimum has no negative decimal
// literal (its magnitude does not fit i64), so it is spelled with the
// standard -max-1 idiom
static tempstr I64Literal(i64 val) {
    tempstr retval;
    if (val == i64(0x8000000000000000ULL)) {
        retval << "(-9223372036854775807LL-1)";
    } else {
        retval << val << "LL";
    }
    return retval;
}

// Compute the numeric range the fdec field's storage holds, once per field;
// the Dec tfuncs generate their range checks from the stored bounds. An arg
// with no integer range cannot back those checks, so the run fails naming
// the field and its arg.
// The decimal place count is checked here as well, before any tfunc builds
// arithmetic from it.
void amc::tclass_Dec() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFdec &fdec = *field.c_fdec;
    bool numrange = GetMinMax(*field.p_arg, fdec.nummin, fdec.nummax, fdec.issigned);
    if (!numrange) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numtype"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("comment","fdec arg must resolve to an integer builtin"));
    }
    // Every quantity the Dec tfuncs compute -- the scale factor, the bounds of
    // the range, the integer portion, the digits a text form parses to -- is a
    // number of the integer builtin the arg stands for, and is spelled with
    // that builtin. The arg's own type appears only where the field's storage
    // is touched, and a value is cast to it on the way in. An arg that names
    // the builtin outright spells the two the same way; an arg that stands for
    // one through a wrapper ctype spells them apart, and the value crosses
    // between them through the wrapper's conversions.
    amc::BltinId bltin_id(amc_BltinIdEnum(0));
    bool bltin_known = amc::GetBltinId(*field.p_arg, bltin_id);
    Set(R, "$Numtype", bltin_known ? strptr(value_ToCstr(bltin_id)) : strptr(field.p_arg->cpp_type));
    tempstr badcast = amc::BadBltinCast(*field.p_arg);
    if (numrange && ch_N(badcast)) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numcast"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("comment",badcast));
    }
    // A Bitfld field has no word of its own: it occupies WIDTH bits of another
    // field's word, and its setter masks the store to those bits. Its arg
    // names the type the accessors hand back, not the magnitude the field
    // holds, so a ten-bit slice of a u32 keeps 0..1023 and drops every value
    // above -- a bound built from the arg would let SetDoubleMaybe report
    // success on 42949672.95 and store 10.23. The bounds are therefore taken
    // from the width whenever the width is the narrower of the two, and both
    // ends move together. The getter hands the masked word back through a
    // cast to the arg type, and a value occupying fewer bits than that type
    // never sets its sign bit, so a slice narrower than a signed arg holds no
    // negative value at all and its minimum is zero. A width that spans the
    // whole arg leaves both bounds alone: the cast then reproduces the arg's
    // own range, negative values included. gen_compute_size rejects the
    // narrower signed slice on its own -- through a wrapper arg as well as a
    // direct one, which test/amc/bad_fdec_nplace.ssim carries a row for either
    // way -- and the bounds computed here do not rest on that rejection, so
    // the emitted range checks describe what the field holds regardless.
    // What the store holds is FieldMinStore and FieldMaxStore's question, and it is
    // the same question asked of the same field by the fconst check and by the
    // reader's own range condition, so the answer is read from them rather than
    // computed here a second time: the max is the arg type's clamped to the bits
    // the width names, and the min is the arg type's raised to zero when those bits
    // drop the sign. A width no mask describes -- zero, negative, or past
    // sixty-four -- leaves both bounds the arg type's own, and is reported by the
    // bitfield accessors naming the field. Each half of that has a fixture of its
    // own, because the bitfield accessors end the run at the first width they refuse
    // and a run therefore reaches this rule with one bad width at most:
    // test/amc/bad_fdec_bitwidth.ssim holds a zero width and
    // test/amc/bad_fdec_bitwidth_neg.ssim a negative one, and each pins that
    // the field draws the bitfield diagnostic alone with no count diagnostic
    // beside it.
    if (numrange && field.c_bitfld) {
        (void)amc::FieldMinStore(field, fdec.nummin);
        (void)amc::FieldMaxStore(field, fdec.nummax);
    }
    // A field declared with eighteen decimal places -- the convention for
    // crypto amounts -- ends up with two different scales in the same
    // field. The double accessors substitute the constant
    // algo::I64Power10(nplace), whose table of powers of ten stops at the
    // seventeenth and answers LLONG_MAX (9223372036854775807, 9.2 times
    // the intended 10^18) beyond it, while Print and ReadStrptrMaybe carry
    // nplace into the emitted body and scale by 10^nplace. A scale too
    // large for the field's own arg is the same disagreement reached from
    // the other side: GetScale returns the scale as the arg type, so
    // twelve places on an i32 field emit a return of 1000000000000 from a
    // function whose return type holds nine digits. A count of zero -- the
    // field's own default, so the count a forgotten nplace attr produces
    // -- breaks the reader on its own: its digit test admits a digit only
    // while fewer than nplace have been read, so nothing accumulates and a
    // parse of "123" reports success after storing zero. Past thirty
    // places the printer's own scratch buffer, sized for the widest
    // decimal an i64 holds, overruns. The count is therefore at least one
    // and no more than the places whose scale both the powers of ten
    // reach and the field's own storage holds; any other count fails the
    // run naming the field.
    // A storage whose maximum is below ten holds no scale at all, so no
    // count at all is acceptable for it and the diagnostic says so rather
    // than stating an empty range. A three-bit Bitfld is such a field.
    // An arg with no numeric range is held to the powers-of-ten limit
    // alone, since the bounds handed back for it are the widest signed: the
    // count is still examined, and a field with both faults draws both
    // diagnostics.
    int maxplace = 17;
    while (maxplace > 0 && u64(algo::I64Power10(maxplace)) > fdec.nummax) {
        maxplace--;
    }
    if (fdec.nplace < 1 || fdec.nplace > maxplace) {
        algo_lib::_db.exit_code++;
        tempstr comment;
        if (maxplace == 0) {
            comment << "fdec field holds no decimal place: its range stops below ten";
        } else {
            comment << "fdec nplace must be in 1.." << maxplace;
        }
        prerr("amc.bad_nplace"
              <<Keyval("field",field.field)
              <<Keyval("nplace",fdec.nplace)
              <<Keyval("comment",comment));
    }
}

// Generate $name_GetDouble: return the fixed-point field as a double, dividing by the decimal scale.
void amc::tfunc_Dec_GetDouble() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$fromdbl", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$parname"));

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "double",false);
    Ins(&R, func.proto, "$name_GetDouble($Cparent)",false);
    Ins(&R, func.body, "double ret;");
    Ins(&R, func.body, "ret = $fldval / double($fromdbl);");
    Ins(&R, func.body, "return ret;");
}

// Generate $name_GetInt: return the integer portion of the fixed-point field, dividing by the decimal scale.
void amc::tfunc_Dec_GetInt() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$scale", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$parname"));

    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.ret  , "$Numtype",false);
    Ins(&R, func.comment  , "Return integer portion (divide number by $scale)",false);
    Ins(&R, func.proto, "$name_GetInt($Cparent)",false);
    Ins(&R, func.body, "$Numtype ret;");
    Ins(&R, func.body, "ret = $fldval / $scale;");
    Ins(&R, func.body, "return ret;");
}

// Generate the ctype's GetScale accessor returning the fixed-point scale constant, only when the ctype carries a single fdec field.
void amc::tfunc_Dec_GetScale() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$scale", tempstr() << algo::I64Power10(fdec.nplace));
    Set(R, "$Name", name_Get(*field.p_ctype));

    // the convenience accessor is named after the ctype, so it exists only
    // when the ctype carries a single fdec field; with two fdec fields the
    // second definition would collide with the first
    int nfdec = 0;
    ind_beg(amc::ctype_c_field_curs,fld2,*field.p_ctype) {
        nfdec += fld2.c_fdec != NULL;
    }ind_end;
    if (nfdec == 1) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.ret  , "$Numtype",false);
        Ins(&R, func.comment  , "Return constant $scale",false);
        Ins(&R, func.proto, "$Name_GetScale()",false);
        Ins(&R, func.body, "return $scale;");
    }
}

// Generate $name_qSetDouble: set the fixed-point field from a double, rounding to the nearest representable value.
void amc::tfunc_Dec_qSetDouble() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$fromdbl", tempstr() << algo::I64Power10(fdec.nplace));

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
    Ins(&R, qsetdbl.body, AssignExpr(field, "$parname", "intval", true)<<";");
}

// Generate $name_SetDoubleMaybe: store VAL scaled and rounded to the
// nearest scaled integer, returning false when that integer is outside the
// field's range.
// Both bounds are checked before the rounding, against the field's range
// widened by the half unit the rounding itself moves: maxval must be accepted
// (214748364.7 scales to exactly 2147483647, the top of an i32 field with one
// implied place) while a value that rounds to maxval+1 must not be, and the
// bottom of the range needs the same allowance or a value that rounds up to
// minval is refused while its mirror at the top stores. Half a unit is also
// what keeps the upper bound safe for a field as wide as the double's integer
// reach: converting the type's maximum to a double rounds it up to the next
// power of two, and half a unit disappears into that rounding, leaving the
// bound strict -- so no double at or above the type's magnitude is ever
// stored, which would be undefined behavior. At the bottom of such a field
// the type's minimum is itself a power of two, exactly representable, and the
// half unit disappears the other way: the strict bound then refuses the
// minimum, a double whose neighbors are a thousand units away.
void amc::tfunc_Dec_SetDoubleMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFdec &fdec = *field.c_fdec;
    Set(R, "$fromdbl", tempstr() << algo::I64Power10(fdec.nplace));

    amc::FFunc& setdoublex = amc::CreateCurFunc();
    Ins(&R, setdoublex.comment, "Set value of field $name, using rounding.");
    Ins(&R, setdoublex.comment, "If value is out of range for the target type, return false.");
    Ins(&R, setdoublex.ret  , "bool",false);
    Ins(&R, setdoublex.proto, "$name_SetDoubleMaybe($Parent, double val)",false);
    Ins(&R, setdoublex.body, "double intval = val * $fromdbl;");
    // find expressions for min/max value of the field
    // type of expression is u64
    if (fdec.issigned) {
        Ins(&R, setdoublex.body, tempstr()<<"$Numtype minval = $Numtype("<<I64Literal(i64(fdec.nummin))<<");");
        Ins(&R, setdoublex.body, tempstr()<<"$Numtype maxval = $Numtype("<<I64Literal(i64(fdec.nummax))<<");");
    } else {
        Ins(&R, setdoublex.body, tempstr()<<"$Numtype minval = $Numtype("<<u64(fdec.nummin)<<"ULL);");
        Ins(&R, setdoublex.body, tempstr()<<"$Numtype maxval = $Numtype("<<u64(fdec.nummax)<<"ULL);");
    }
    Set(R, "$setexpr", AssignExpr(field, "$parname", "intval", true));
    Ins(&R, setdoublex.body, "bool retval = intval > double(minval) - 0.5;// minval rounds in, minval-1 rounds out");
    Ins(&R, setdoublex.body, "retval &= intval < double(maxval) + 0.5;// maxval rounds in, maxval+1 rounds out");
    Ins(&R, setdoublex.body, "if (retval) {");
    Ins(&R, setdoublex.body, "    intval = intval + (val > 0 ? 0.5 : -0.5);");
    Ins(&R, setdoublex.body, "    $setexpr; // set underlying field.");
    Ins(&R, setdoublex.body, "}");
    Ins(&R, setdoublex.body, "return retval;");
}

// Generate $name_ReadStrptrMaybe: parse a decimal string into the scaled
// integer the field stores, returning false on a bad character or a value
// the field cannot hold.
void amc::tfunc_Dec_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFdec &fdec = *field.c_fdec;

    u64 posmax = fdec.nummax;
    u64 negmax = fdec.issigned ? u64(0) - fdec.nummin : posmax;
    tempstr limdiv;
    tempstr limmod;
    if (negmax/10 == posmax/10) {
        limdiv << posmax/10 << "ULL";
    } else {
        limdiv << "neg ? " << negmax/10 << "ULL : " << posmax/10 << "ULL";
    }
    if (negmax%10 == posmax%10) {
        limmod << posmax%10 << "ULL";
    } else {
        limmod << "neg ? " << negmax%10 << "ULL : " << posmax%10 << "ULL";
    }
    Set(R, "$limdiv", limdiv);
    Set(R, "$limmod", limmod);
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
        // The digits accumulate into a u64 as a magnitude, and the store then
        // narrows the result to the field's own type: 700.00 read into a
        // u16-backed field with two implied places accumulates 70000, and the
        // store would keep only the low 16 bits (4464) while the read reported
        // success. A field as wide as the accumulator fails the same way one
        // level up -- the accumulator itself wraps -- and comparing the new
        // accumulated value with the previous one does not catch that:
        // val*10+digit modulo 2^64 lands above val whenever val is at least
        // 2^64/9. So a digit is admitted only when it keeps the magnitude
        // within what the field holds, tested before the multiply against that
        // magnitude's quotient and remainder by 10.
        //
        // Which magnitude the field holds depends on the sign, and a signed
        // field reaches one unit further down than up: an i32-backed field
        // with one implied place prints its minimum as -214748364.8, so the
        // reader has to admit magnitude 2147483648 behind a minus and refuse
        // it behind a plus, or a printed record cannot be read back. The
        // limits are therefore picked once from the sign, which is already
        // known when the digits start. The magnitude then never exceeds what
        // the field holds and needs no check after the loop, and the signed
        // store negates in u64, so the minimum's magnitude never has to be
        // representable as a positive value of the field's own type.
        Ins(&R, tryread.body, "u64 limdiv = $limdiv;// largest magnitude the field holds for this sign, over 10");
        Ins(&R, tryread.body, "u64 limmod = $limmod;// and that magnitude's last digit");
        Ins(&R, tryread.body, "for (; index < in.n_elems; index++) {");
        Ins(&R, tryread.body, "    char c = in.elems[index];");
        Ins(&R, tryread.body, "    if (c == '.') {");
        Ins(&R, tryread.body, "        ok &= ndot == 0; // max 1 dot");
        Ins(&R, tryread.body, "        ndot++;");
        Ins(&R, tryread.body, "    } else if (algo_lib::DigitCharQ(c)) {");
        Ins(&R, tryread.body, "        if (nfrac < $nplace) { // ignore digits after $nplace'th");
        Ins(&R, tryread.body, "            u64 digit = c - '0';");
        Ins(&R, tryread.body, "            ok &= val < limdiv || (val == limdiv && digit <= limmod);// val*10+digit must fit the field");
        Ins(&R, tryread.body, "            val = val*10 + digit;");
        Ins(&R, tryread.body, "            nfrac += ndot>0;");
        Ins(&R, tryread.body, "        }");
        Ins(&R, tryread.body, "    } else {");
        Ins(&R, tryread.body, "        ok = false; // unknown char");
        Ins(&R, tryread.body, "    }");
        Ins(&R, tryread.body, "}");
        Ins(&R, tryread.body, "while (nfrac < $nplace) {// insert missing");
        Ins(&R, tryread.body, "    ok &= val <= limdiv;// val*10 must fit the field");
        Ins(&R, tryread.body, "    val = val*10;");
        Ins(&R, tryread.body, "    nfrac++;");
        Ins(&R, tryread.body, "}");
        if (fdec.issigned) {
            Ins(&R, tryread.body, "i64 final_val = i64(neg ? 0 - val : val);// negate in u64: -val at the type minimum has no positive form");
        } else {
            Ins(&R, tryread.body, "u64 final_val = val;");
            Ins(&R, tryread.body, "ok &= !neg;");
        }
        Ins(&R, tryread.body, "if (ok) {");
        Ins(&R, tryread.body, tempstr()<<"    "<<AssignExpr(field, "$parname", "final_val", true)<<";// store value");
        Ins(&R, tryread.body, "}");
        Ins(&R, tryread.body, "return ok;");
    }
}

// Generate $name_Print: format the fixed-point field as a decimal string.
void amc::tfunc_Dec_Print() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFdec &fdec = *field.c_fdec;

    if (amc::HasStringPrintQ(*field.p_ctype)) {
        amc::FFunc& print = amc::CreateCurFunc();
        Ins(&R, print.ret  , "void",false);
        Ins(&R, print.proto, "$name_Print($Parent, cstring &outstr)",false);
        Set(R, "$nplace", tempstr() << field.c_fdec->nplace);
        Set(R, "$fldval", FieldvalExpr(field.p_ctype, field,"$parname"));
        Set(R, "$name", name_Get(field));
        Set(R, "$fixed", fdec.fixedfmt ? "true" : "false");
        Ins(&R, print.body, "$Numtype value = $fldval;");
        Ins(&R, print.body, "ch_Reserve(outstr, 64);");
        if (fdec.issigned) {
            // negate after widening to u64: -value in the field's own type is
            // undefined at the type minimum, and the wrapped value would then
            // sign-extend into a garbage u64 magnitude
            Ins(&R, print.body, "u64 magnitude = u64(value);");
            Ins(&R, print.body, "if (value < 0) {");
            Ins(&R, print.body, "    ch_Alloc(outstr) = '-';");
            Ins(&R, print.body, "    magnitude = 0 - u64(value);");
            Ins(&R, print.body, "}");
        }
        Set(R, "$magnitude", fdec.issigned ? "magnitude" : "value");
        Ins(&R, print.body, "u8 *$name_start = (u8*)(outstr.ch_elems + outstr.ch_n);");
        Ins(&R, print.body, "outstr.ch_n += algo::u64_FmtBufDec($magnitude, $nplace, $name_start, $fixed);");
    }
}
