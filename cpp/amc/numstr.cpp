// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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

// The pad budget of the generated SetnumMaybe: its on-stack format buffer is
// two budgets long, and the digits never occupy more than the top budget (a
// u64 in base 2 is 64 digits), so at least one budget of pad characters fits
// below the digits.  A min_len above the budget could need more pad than the
// buffer holds, silently under-padding -- and the sign, prepended below the
// pad, would then overwrite a pad character -- so tclass_Numstr rejects it.
static const i32 numstr_pad_budget = 64;

// -----------------------------------------------------------------------------

// The character encoding digit zero in numstr base BASE: the NUL byte in
// base 256, ' ' in base 95, '0' in every base with a character alphabet
static char ZeroDigit(i32 base) {
    return base == 256 ? '\0' : base == 95 ? ' ' : '0';
}

// True if CH encodes a digit of numstr base BASE: any byte in base 256,
// any printable character in base 95, and in bases 2..36 the digit and
// letter characters Getnum accepts (letters in either case)
static bool BaseDigitQ(i32 base, char ch) {
    bool retval = false;
    if (base == 256) {
        retval = true;
    } else if (base == 95) {
        retval = ch >= ' ' && ch <= '~';
    } else {
        retval = ch >= '0' && ch < '0' + i32_Min(base,10);
        retval |= base > 10 && ch >= 'a' && ch < 'a' + base - 10;
        retval |= base > 10 && ch >= 'A' && ch < 'A' + base - 10;
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Set up the numeric-range substitution variables shared by the numstr
// tfuncs, and fail the amc run on a spec whose stored strings could not
// parse back to the stored value: a pad or a base the sign cannot be told
// apart from, a pad that reads as a digit, or a min_len wider than the
// string, the pad budget, or the room the sign needs
void amc::tclass_Numstr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;
    bool numrange = GetMinMax(*numstr.p_numtype, numstr.nummin, numstr.nummax, numstr.issigned);
    if (!numrange) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numtype"
              <<Keyval("field",smallstr.field)
              <<Keyval("numtype",numstr.numtype)
              <<Keyval("comment","numstr numtype must resolve to an integer builtin"));
    }
    // Getnum hands the parsed digits back as the numtype and Geti64 reads that
    // result as an i64, so a numtype that stands for the builtin through a
    // wrapper ctype has the value constructed on the way out of the parse and
    // read on the way into the i64. A wrapper carrying only one of the two
    // conversions leaves one of those statements without a way to compile, so
    // the field is reported here rather than in cpp/gen.
    tempstr badcast = amc::BadBltinCast(*numstr.p_numtype);
    if (numrange && ch_N(badcast)) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numcast"
              <<Keyval("field",smallstr.field)
              <<Keyval("numtype",numstr.numtype)
              <<Keyval("comment",badcast));
    }

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
    // Every pad rule below judges the byte the generated code pads with, which
    // is what reading strips, and never the spelling the attr carries. The two
    // are not the same thing: tclass_Smallstr substitutes the literal 0 for an
    // empty pad value, so pad:"" and pad:0 both pad with NUL and emit the same
    // Init, N and SetStrptr, and an escaped spelling such as pad:"'\x30'"
    // names the digit zero rather than a backslash. A pad written two ways is
    // one pad and draws one verdict.
    // Only a padded string has a byte to strip. An rpascal string carries its
    // own length and reads back exactly the characters that were stored, so
    // no pad rule applies to it
    char pad = PadChar(smallstr);
    bool leftpad = smallstr.strtype == dmmeta_Strtype_strtype_leftpad;
    bool rightpad = smallstr.strtype == dmmeta_Strtype_strtype_rightpad;
    // the NUL pad is refused on a signed numstr, in the LnullStr style and the
    // RnullStr style alike
    if (numstr.issigned && (leftpad || rightpad) && pad == '\0') {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("comment","signed numstr cannot use the NUL pad"));
    }
    // in base 95 and base 256 the character '-' is itself a digit (13 and 45),
    // so a stored digit string can begin with it: SetnumMaybe(13) in base 95
    // writes the single character '-', which Getnum would strip as a sign and
    // read back as 0. The encoding is ambiguous, so a signed numstr cannot
    // use these bases
    if (numstr.issigned && (numstr.base == 95 || numstr.base == 256)) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("base",numstr.base)
              <<Keyval("comment","signed numstr cannot use a base where '-' is a digit"));
    }
    // reading strips the strtype pad from the stored string, so a pad that
    // is itself a digit of the base makes the strip ambiguous. On the left
    // only the zero digit is safe: leading zero digits carry no value. On
    // the right no digit is safe: a trailing zero digit stripped as pad
    // divides the value by the base, and a nonzero pad digit strips real
    // trailing digits
    if (leftpad && BaseDigitQ(numstr.base, pad) && pad != ZeroDigit(numstr.base)) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("pad",smallstr.pad)
              <<Keyval("base",numstr.base)
              <<Keyval("comment","left-pad that is a nonzero digit of the base corrupts values on read"));
    }
    if (rightpad && BaseDigitQ(numstr.base, pad)) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("pad",smallstr.pad)
              <<Keyval("base",numstr.base)
              <<Keyval("comment","right-pad that is a digit of the base corrupts values on read"));
    }
    // a negative value carries its sign as a leading '-', and the left pad is
    // stripped from that same end: SetnumMaybe(-12) into a five-character
    // string dash-padded on the left stores "---12", the strip takes all
    // three leading dashes, and Getnum reads back +12. The right pad is
    // stripped from the other end and leaves the leading sign alone, and an
    // unsigned numstr writes no sign for a dash to be confused with
    if (numstr.issigned && leftpad && pad == '-') {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("pad",smallstr.pad)
              <<Keyval("comment","signed numstr cannot use the sign character as a left pad"));
    }
    // a min_len wider than the string can never be stored: the padded digits
    // always fail SetnumMaybe's length gate
    if (numstr.min_len > smallstr.length) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("min_len",numstr.min_len)
              <<Keyval("length",smallstr.length)
              <<Keyval("comment","numstr min_len cannot exceed the string length"));
    }
    // a signed numstr prepends the sign in front of min_len digits: with
    // min_len == length the sign has no slot and every negative value
    // fails SetnumMaybe's length gate
    if (numstr.issigned && numstr.min_len == smallstr.length) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("min_len",numstr.min_len)
              <<Keyval("length",smallstr.length)
              <<Keyval("comment","signed numstr min_len leaves no room for the sign"));
    }
    // SetnumMaybe pads within a fixed buffer: one pad budget below the digits.
    // A min_len needing more pad than that silently under-pads, and a sign
    // would then overwrite the first pad character
    if (numstr.min_len > numstr_pad_budget) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("min_len",numstr.min_len)
              <<Keyval("pad_budget",numstr_pad_budget)
              <<Keyval("comment","numstr min_len exceeds the pad budget"));
    }
    // Base 1 is not supported: a unary numeral needs one digit per unit of
    // value, and SetnumMaybe formats into a fixed buffer -- one pad budget's
    // worth of digits (64 covers a u64 even in base 2) plus the pad region
    // checked above.
    if (!(numstr.base == 256 || numstr.base == 95 || (numstr.base >= 2 && numstr.base <= 36))) {
        algo_lib::_db.exit_code++;
        prerr("amc.bad_numstr"
              <<Keyval("field",smallstr.field)
              <<Keyval("base",numstr.base)
              <<Keyval("comment","unsupported base (must be 2..36, 95, or 256)"));
    }
}

// Generate $name_Getnum: parse the stored digit string back to the numeric
// type, clearing and_ok on an invalid digit or a value outside the numtype
// range; the empty string reads as zero
void amc::tfunc_Numstr_Getnum() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
        if (numstr.base >= 2 && numstr.base <= 36) {
            Ins(&R, qgetnum.body, "    int digit = ch;");
            Ins(&R, qgetnum.body, "    if (ch >= '0' && ch < '0' + $bthresh) {");
            Ins(&R, qgetnum.body, "        digit -= '0';");
        }
        if (numstr.base > 10 && numstr.base <= 36) {
            Ins(&R, qgetnum.body, "    } else if (ch >= 'a' && ch < 'a' + $base-10) {");
            Ins(&R, qgetnum.body, "        digit = digit - 'a' + 10;");
            Ins(&R, qgetnum.body, "    } else if (ch >= 'A' && ch < 'A' + $base-10) {");
            Ins(&R, qgetnum.body, "        digit = digit - 'A' + 10;");
        }
        if (numstr.base >= 2 && numstr.base <= 36) {
            Ins(&R, qgetnum.body, "    } else {");
            Ins(&R, qgetnum.body, "        and_ok = false;");
            Ins(&R, qgetnum.body, "    }");
        }
        if (numstr.base == 95) {
            // ' ' is digit zero; the u8 wraparound maps every char below ' ' past 94,
            // so one comparison rejects everything outside the printable range ' '..'~'
            Ins(&R, qgetnum.body, "    int digit = u8((u8)ch - (u8)' ');");
            Ins(&R, qgetnum.body, "    and_ok &= digit < $base;");
        }
        if (numstr.base == 256) {
            // plain char may be signed: a byte >= 0x80 is a large digit, not a negative one
            Ins(&R, qgetnum.body, "    int digit = u8(ch);");
        }
        if (str_max_may_not_fit_in_u64) {
            Ins(&R, qgetnum.body, "    // Check for 64-bit overflow before multiplying: val*$base + digit fits");
            Ins(&R, qgetnum.body, "    // in 64 bits exactly when val < q, or val == q and digit <= r, where");
            Ins(&R, qgetnum.body, "    // q,r split 2^64-1 by $base; the constant divisions fold at compile time");
            Ins(&R, qgetnum.body, "    and_ok &= (val < 0xffffffffffffffffULL/$base || (val == 0xffffffffffffffffULL/$base && u64(digit) <= 0xffffffffffffffffULL%$base));");
            Ins(&R, qgetnum.body, "    val = val*$base + digit;");
        } else {
            Ins(&R, qgetnum.body, "    val = val*$base + digit;");
        }
        Ins(&R, qgetnum.body    , "}");
    }

    if (numstr.issigned) {
        // the check is on the unsigned magnitude: a check on the i64 result cannot
        // see val > i64max (the cast wraps), and the negative bound is one larger
        if (str_max >= numstr.nummax) {
            Ins(&R, qgetnum.body    , "and_ok &= val <= u64($nummax) + u64(is_neg);");// check for overflow
        }
        Ins(&R, qgetnum.body    , "i64 ret = is_neg ? -val : val;");
        Ins(&R, qgetnum.body    , "return $Rtype(ret);");
    } else {
        // for a u64 numtype the in-loop 64-bit overflow check is the range check
        if ((str_max >= numstr.nummax) && numstr.nummax < 0xffffffffffffffffULL) {
            Ins(&R, qgetnum.body    , "and_ok &= val <= $nummax;");// check for overflow
        }
        Ins(&R, qgetnum.body    , "return $Rtype(val);");
    }
}


// Generate $name_GetnumDflt: read the stored value through Getnum,
// returning DFLT when the string does not parse
void amc::tfunc_Numstr_GetnumDflt() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    Ins(&R, getnumdflt.body , "$Rtype result = $name_Getnum($pararg, ok);");
    Ins(&R, getnumdflt.body , "return ok ? result : dflt;");
}


// Generate $name_Geti64: read the stored value as an i64, failing a value
// above i64max rather than wrapping it negative through the cast
void amc::tfunc_Numstr_Geti64() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
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
    // a u64 numtype holds values above i64max; the plain cast wraps them to
    // a negative i64, so the value must fail instead
    if (!numstr.issigned && numstr.nummax > 0x7fffffffffffffffULL) {
        Ins(&R, geti64.body , "$Rtype num = $name_Getnum($pararg, out_ok);");
        Ins(&R, geti64.body , "out_ok &= num <= 0x7fffffffffffffffULL;");
        Ins(&R, geti64.body , "return i64(num);");
    } else {
        Ins(&R, geti64.body , "i64 result = $name_Getnum($pararg, out_ok);");
        Ins(&R, geti64.body , "return result;");
    }
}

// Generate $name_SetnumMaybe: format the number into the string through an
// auxiliary buffer, refusing -- with the stored string left unchanged -- a
// value outside the numtype range or digits that do not fit the string
void amc::tfunc_Numstr_SetnumMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;

    amc::FFunc& setnum = amc::CreateCurFunc();
    Set(R, "$base", tempstr() << numstr.base);
    Set(R, "$budget", tempstr() << numstr_pad_budget);
    Set(R, "$bufsize", tempstr() << 2*numstr_pad_budget);
    Ins(&R, setnum.comment, "Set string to number specified in RHS performing base-$base conversion.");
    Ins(&R, setnum.comment, "If the number is out of range for the numeric type,");
    Ins(&R, setnum.comment, "or too large for the string, return false.");
    Ins(&R, setnum.ret  , "bool", false);
    Ins(&R, setnum.proto, "$name_SetnumMaybe($Parent, i64 rhs)", false);
    // values outside the numtype range must fail rather than store digits of a
    // different number: the base-10 fast path formats through a u32-parameter
    // FmtBuf, which wraps rhs mod 2^32 before the string-length gate can see it.
    // An i64 numtype needs no check (rhs spans it exactly); u64 needs only rhs>=0.
    tempstr inrange_expr;
    if (numstr.issigned) {
        if (numstr.nummax < 0x7fffffffffffffffULL) {
            inrange_expr << "rhs >= $nummin && rhs <= $nummax";
        }
    } else {
        inrange_expr << "rhs >= 0";
        if (numstr.nummax < 0xffffffffffffffffULL) {
            inrange_expr << " && rhs <= $nummax";
        }
    }
    bool gated = ch_N(inrange_expr) > 0;
    // the conversion, built separately so it can be spliced under the range
    // gate when one exists: an out-of-range rhs then performs no conversion
    // and cannot store the digits of a different number
    cstring conv;
    Ins(&R, conv        , "char buf[$bufsize];");
    if (numstr.issigned) {
        // negate after widening to u64: -rhs in i64 is undefined for i64min,
        // whose magnitude has no i64 representation
        Ins(&R, conv    , "u64 val = (rhs < 0 ? 0 - u64(rhs) : u64(rhs));");
    } else {
        Ins(&R, conv    , "u64 val = rhs;");
    }

    if ((numstr.base == 10) && ((numstr.numtype == "u16") || (numstr.numtype == "i16"))) {
        Ins(&R, conv    , "int charpos = $budget;");
        Ins(&R, conv    , "int length = algo::u16_FmtBuf(val, (u8*)buf + charpos);");
    } else if ((numstr.base == 10) && ((numstr.numtype == "u32") || (numstr.numtype == "i32"))) {
        Ins(&R, conv    , "int charpos = $budget;");
        Ins(&R, conv    , "int length = algo::u32_FmtBuf(val, (u8*)buf + charpos);");
    } else if ((numstr.base == 10) && ((numstr.numtype == "u64") || (numstr.numtype == "i64"))) {
        Ins(&R, conv    , "int charpos = $budget;");
        Ins(&R, conv    , "int length = algo::u64_FmtBuf(val, (u8*)buf + charpos);");
    } else {
        Ins(&R, conv    , "int charpos = sizeof(buf);");
        Ins(&R, conv    , "do {");
        Ins(&R, conv    , "    u32 rem = u32(val % $base);");
        Ins(&R, conv    , "    val = val / $base;");
        Ins(&R, conv    , "    char ch;");
        if (numstr.base == 256) {
            Ins(&R, conv, "    ch = char(rem);");
        } else if (numstr.base <= 10) {
            Ins(&R, conv, "    ch = char('0'+rem);");
        } else if (numstr.base > 10 && numstr.base <= 36) {
            Ins(&R, conv, "    ch = rem < 10 ? char('0' + rem) : char('A' + rem - 10);");
        } else {
            Ins(&R, conv, "    ch = char(rem + ' ');");
        }
        Ins(&R, conv    , "    buf[--charpos] = ch;");
        Ins(&R, conv    , "} while (val != 0);");
        Ins(&R, conv    , "int length = sizeof(buf) - charpos;");
    }
    Set(R, "$minlength", tempstr() << numstr.min_len);
    // string is guaranteed to have at least one character written to it
    // because of the do-while above
    if (numstr.min_len > 1) {
        // the pad is the base's zero digit, so the padded string parses back
        // to the same value: '0' is digit 16 in base 95 and digit 48 in base 256
        Set(R, "$zerodigit", numstr.base == 256 ? "'\\0'" : numstr.base == 95 ? "' '" : "'0'");
        Ins(&R, conv        , "//pad string with zero digits up to min_length");
        Ins(&R, conv        , "while (charpos > 0 && length < $minlength) {");
        Ins(&R, conv        , "    buf[--charpos] = $zerodigit;");
        Ins(&R, conv        , "    ++length;");
        Ins(&R, conv        , "}");
    }
    // the sign is prepended in front of the digits (including any pad zeros),
    // so the printed string parses back to the same value
    if (numstr.issigned) {
        Ins(&R, conv    , "if (rhs < 0) {");
        Ins(&R, conv    , "    if (charpos > 0) {");
        Ins(&R, conv    , "        --charpos;");
        Ins(&R, conv    , "        ++length;");
        Ins(&R, conv    , "    }");
        Ins(&R, conv    , "    buf[charpos] = '-';");
        Ins(&R, conv    , "}");
    }
    Set(R, "$retdecl", gated ? "retval" : "bool retval");
    Ins(&R, conv        , "$retdecl = length <= $max_length;");
    Ins(&R, conv        , "if (retval) {");
    Ins(&R, conv        , "    $name_SetStrptr($pararg, algo::strptr(buf + charpos, length));");
    Ins(&R, conv        , "}");
    if (gated) {
        Ins(&R, setnum.body, "bool retval = false;");
        Ins(&R, setnum.body, tempstr() << "if (" << inrange_expr << ") {");
    }
    setnum.body << conv;// emission re-indents the body from its brace structure
    if (gated) {
        Ins(&R, setnum.body, "}");
    }
    Ins(&R, setnum.body, "return retval;");
}


// Generate $ns::ForAllStrings: each numstr field appends a block filling
// a descriptor -- accessor pointers and the shape facts -- and calling
// the test function on it
void amc::tfunc_Numstr_ForAllStrings() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSmallstr &smallstr = *field.c_smallstr;
    amc::FNumstr &numstr = *smallstr.c_numstr;
    Set(R, "$min_length", tempstr() << numstr.min_len);
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
    // Two's-complement negative range is one larger than positive: when the
    // sign is negative, allow magnitude up to MAX+1 so INT_MIN is reachable.
    Set(R, "$cap", issigned ? "($maxval + $valtype(neg))" : "$maxval");
    Set(R, "$maxdig", tempstr()<<maxdig);
    amc::FFunc *_func = amc::ind_func_Find(Subst(R,"$type..ReadStrptrMaybe"));
    vrfy(_func,tempstr()<<"amc: function "<<Subst(R,"$type..ReadStrptrMaybe")<<" not found, make sure cfmt:$type.String exists");
    amc::FFunc& func = *_func;
    func.extrn=false;
    Ins(&R, func.comment, "Attempt to parse $type from in_str");
    Ins(&R, func.comment, "Leading whitespace is silently skipped");
    Ins(&R, func.comment, "Return success value; if false, PARENT is unchanged");
    Ins(&R, func.comment, "An empty string parses as 0; a whitespace-only string fails");
    Ins(&R, func.comment, "Number may be prefixed with + or - (with no space after)");
    Ins(&R, func.comment, "Parsing stops at the first non-digit character; trailing text is silently ignored");
    Ins(&R, func.comment, "If the value is outside of valid range for the type, it is clipped to the valid range");
    Ins(&R, func.comment, "Supported bases: 10, 16 (if string starts with 0x or 0X)");
    Ins(&R, func.comment, "For hex numbers, there is no overflow (just take last N digits that fit the type)");

    Ins(&R, func.body, "int index = 0;");
    Ins(&R, func.body, "bool neg=false;");
    Ins(&R, func.body, "bool hex=false;");
    Ins(&R, func.body, "$valtype num=0;");
    Ins(&R, func.body, "char c;");
    Ins(&R, func.body, "while (index < in_str.n_elems && algo_lib::WhiteCharQ(in_str.elems[index])) {");
    Ins(&R, func.body, "    index++;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (index < in_str.n_elems) {");
    Ins(&R, func.body, "    c = in_str.elems[index];");
    Ins(&R, func.body, "    index++;");
    Ins(&R, func.body, "    if (c=='+') {");
    Ins(&R, func.body, "    } else if (c=='-') {");
    Ins(&R, func.body, "        neg=true;");
    Ins(&R, func.body, "    } else if (algo_lib::DigitCharQ(c)) {");
    Ins(&R, func.body, "        num = c-'0';");
    Ins(&R, func.body, "        if (num==0 && index < in_str.n_elems) {");
    Ins(&R, func.body, "            c = in_str.elems[index];");
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
    Ins(&R, func.body, "    if (index == in_str.n_elems) {");
    Ins(&R, func.body, "        retval=false;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    for (; index < in_str.n_elems; index++) {");
    Ins(&R, func.body, "        c = in_str.elems[index];");
    Ins(&R, func.body, "        u8 val;");
    Ins(&R, func.body, "        if (!algo::ParseHex1(c, val)) {");
    Ins(&R, func.body, "            break;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        num = num*16 + val;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    // leading zeros carry no magnitude: skip them so the digit window that");
    Ins(&R, func.body, "    // bounds overflow detection counts significant digits only");
    Ins(&R, func.body, "    while (num == 0 && index < in_str.n_elems && in_str.elems[index] == '0') {");
    Ins(&R, func.body, "        index++;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, "    int lim = u32_Min(index + $maxdig - (num != 0 ? 1 : 0), in_str.n_elems); // count the digit already in num");
    Ins(&R, func.body, "    for (; index < lim; index++) {");
    Ins(&R, func.body, "        c = in_str.elems[index];");
    Ins(&R, func.body, "        if (!algo_lib::DigitCharQ(c)) {");
    Ins(&R, func.body, "            break;");
    Ins(&R, func.body, "        }");
    Ins(&R, func.body, "        num = num*10 + (c-'0');");
    Ins(&R, func.body, "    }");
    if (check_overflow) {
        Ins(&R, func.body, "// 2nd batch of digits");
        Ins(&R, func.body, "if (index < in_str.n_elems) {");
        Ins(&R, func.body, "    lim = u32_Min(index+$maxdig, in_str.n_elems);");
        Ins(&R, func.body, "    $valtype num2 = 0;");
        Ins(&R, func.body, "    $valtype div = 1;");
        Ins(&R, func.body, "    for (; index < lim; index++) {");
        Ins(&R, func.body, "        c = in_str.elems[index];");
        Ins(&R, func.body, "        if (!algo_lib::DigitCharQ(c)) {");
        Ins(&R, func.body, "            break;");
        Ins(&R, func.body, "        }");
        Ins(&R, func.body, "        num2 = num2*10 + (c-'0');");
        Ins(&R, func.body, "        div = div*10;");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "    // clip: num > cap/div alone misses the boundary batch, where");
        Ins(&R, func.body, "    // num == cap/div but the second batch pushes past cap");
        Ins(&R, func.body, "    $valtype cap = $cap;");
        Ins(&R, func.body, "    if (num > cap/div || (num == cap/div && num2 > cap - num*div)) {");
        Ins(&R, func.body, "        num = cap;");
        Ins(&R, func.body, "    } else {");
        Ins(&R, func.body, "        num = num*div + num2;");
        Ins(&R, func.body, "    }");
        Ins(&R, func.body, "}");
    } else {
        Ins(&R, func.body, "if (num > $cap) {");
        Ins(&R, func.body, "    num = $cap;");
        Ins(&R, func.body, "}");
    }
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (neg) {");
    Ins(&R, func.body, "    num = $negval;");
    Ins(&R, func.body, "}");
    Ins(&R, func.body, "if (retval) {");
    Ins(&R, func.body, "    parent = num;");
    Ins(&R, func.body, "} else {");
    Ins(&R, func.body, "    algo_lib::AppendErrtext(\"comment\", \"$type_ReadStrptrMaybe: bad number\");");
    Ins(&R, func.body, "    algo_lib::AppendErrtext(\"value\",in_str);");
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
