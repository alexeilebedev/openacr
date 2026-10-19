// Copyright (C) 2026 AlgoX2 Corp
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/regxstyle.cpp -- Regx style translators, written as C++ and as TypeScript
//
// A regx pattern is read in two passes.  The first rewrites a pattern of one style
// (acr, sql, shell) into the default syntax, one token at a time, as the
// amcdb.regxrule rows of that style say.  The second normalizes the default syntax:
// it escapes every character that means itself, so that the only bare punctuation
// left is an operator, and it settles each malformed shape the way its
// amcdb.regxlenient row says.  The result is a pattern that algo_lib's parser and a
// JavaScript RegExp read the same way.
// algo_lib runs the passes in C++, and a browser runs them in TypeScript.  Both are
// written here from one template text.  The two languages share the syntax of
// expressions, switch statements and loops, so the template differs between them
// only in the words a Replscope substitutes: how a variable is declared, how a
// string is appended to, what the length of the input is called.  A change to the
// syntax is a change to the rows or to the template, and it reaches both languages
// in the same amc run.

#include "include/amc.h"

// -----------------------------------------------------------------------------

// C, written as a character literal that both C++ and TypeScript read as C.
static tempstr RegxCharLit(u8 c) {
    tempstr ret("'");
    if (c == '\\' || c == '\'') {
        ret << '\\' << char(c);
    } else if (c >= 0x20 && c < 0x7f) {
        ret << char(c);
    } else {
        ret << "\\x" << "0123456789abcdef"[c >> 4] << "0123456789abcdef"[c & 15];
    }
    ret << "'"; // ignore:hand_quote
    return ret;
}

// -----------------------------------------------------------------------------

// STR, written as a string literal that both C++ and TypeScript read as STR.
// A regxrule's text is printable, and the generator refuses it otherwise.
static tempstr RegxStrLit(strptr str) {
    tempstr ret("\"");
    frep_(i,elems_N(str)) {
        u8 c = str[i];
        vrfy(c >= 0x20 && c < 0x7f, tempstr()<<"amc.regx_unprintable"
             <<Keyval("text",str)
             <<Keyval("comment","regx rule text must be printable ASCII"));
        if (c == '\\' || c == '"') {
            ret << '\\';
        }
        ret << char(c);
    }
    ret << "\"";
    return ret;
}

// -----------------------------------------------------------------------------

// Read CHCLASS, a regxmacro's characters, into SET.  The text is single
// characters and ranges a-b, and a backslash names a control character the way C
// does (\n \r \t \v \f \a \e).
static void RegxReadChclass(strptr chclass, algo::Charset &set) {
    int i = 0;
    int n = elems_N(chclass);
    while (i < n) {
        u8 lo = chclass[i];
        int step = 1;
        if (lo == '\\' && i + 1 < n) {
            u8 e = chclass[i+1];
            lo = e == 'n' ? '\n' : e == 'r' ? '\r' : e == 't' ? '\t' : e == 'v' ? '\v'
                : e == 'f' ? '\f' : e == 'a' ? '\a' : e == 'e' ? 0x1b : e;
            step = 2;
        }
        u8 hi = lo;
        if (step == 1 && i + 2 < n && chclass[i+1] == '-') {
            hi = chclass[i+2];
            step = 3;
        }
        for (int k = lo; k <= hi; k++) {
            ch_qSetBit(set, k);
        }
        i += step;
    }
}

// -----------------------------------------------------------------------------

// The ranges of SET as pairs of low and high characters, in increasing order.
static tempstr RegxRangePairs(algo::Charset &set) {
    tempstr ret;
    int k = 0;
    while (k < 256) {
        if (ch_GetBit(set, k)) {
            int lo = k;
            while (k + 1 < 256 && ch_GetBit(set, k + 1)) {
                k++;
            }
            ret << char(lo) << char(k);
        }
        k++;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// One character of a JavaScript class: \xHH below 256, \uHHHH above.
static tempstr RegxJsClassChar(int c) {
    tempstr ret;
    const char *hex = "0123456789abcdef";
    if (c < 256) {
        ret << "\\x" << hex[(c >> 4) & 15] << hex[c & 15];
    } else {
        ret << "\\u" << hex[(c >> 12) & 15] << hex[(c >> 8) & 15] << hex[(c >> 4) & 15] << hex[c & 15];
    }
    return ret;
}

// -----------------------------------------------------------------------------

// The body of a JavaScript class matching what MACRO matches.  A negated macro
// runs up to U+FFFF, which covers U+E080..U+E0FF, the code units that the
// TypeScript side reads the bytes 0x80..0xff as.
static tempstr RegxJsClass(amc::FRegxmacro &macro) {
    algo::Charset set;
    RegxReadChclass(macro.chclass, set);
    tempstr ret;
    int k = 0;
    while (k < 0x10000) {
        bool in = (k < 256 && ch_GetBit(set, k)) != macro.neg;
        if (in) {
            int lo = k;
            while (k + 1 < 0x10000 && ((k + 1 < 256 && ch_GetBit(set, k + 1)) != macro.neg)) {
                k++;
            }
            ret << RegxJsClassChar(lo);
            if (k > lo) {
                ret << "-" << RegxJsClassChar(k);
            }
        }
        k++;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Set the words of the template that differ between the two languages.  TS
// selects TypeScript, and C++ otherwise.
static void RegxSetLang(algo_lib::Replscope &R, bool ts) {
    Set(R, "$int"     , ts ? "let" : "int");
    Set(R, "$bool"    , ts ? "let" : "bool");
    Set(R, "$chr"     , ts ? "let" : "char");
    Set(R, "$cchr"    , ts ? "const" : "char");
    Set(R, "$len"     , ts ? "str.length" : "str.n_elems");
    Set(R, "$app"     , ts ? "+=" : "<<");
    Set(R, "$nochar"  , ts ? "''" : "0");
    Set(R, "$droplast", ts ? "out = out.slice(0, -1);" : "out.ch_n--;");
    Set(R, "$macroq"  , ts ? "regxMacroQ" : "algo_lib::RegxMacroQ");
    Set(R, "$punctq"  , ts ? "regxPunctQ" : "algo_lib::RegxPunctQ");
    Set(R, "$putmacro", ts ? "out += regxMacroJs(n, false);" : "out << '\\\\'; out << n;");
    Set(R, "$classmacro", ts ? "out += regxMacroJs(n, true);" : "out << '\\\\'; out << n;");
    Set(R, "$reversed", ts ? "hi < lo" : "u8(hi) < u8(lo)");
    Set(R, "$dollar"  , "'$'");
}

// -----------------------------------------------------------------------------

// Append to BODY the switch case of the rewrite for TOKEN, a single character
// or the empty catch-all.  The case appends RULE's out to the output, and the
// catch-all appends the character after it.
static void RegxAppendCase(algo_lib::Replscope &R, cstring &body, amc::FRegxrule &rule, strptr token) {
    Set(R, "$label", ch_N(token) ? tempstr() << "case " << RegxCharLit(token[0]) : tempstr("default"));
    Set(R, "$out", RegxStrLit(rule.out));
    Ins(&R, body, "$label: {");
    if (ch_N(rule.out) > 0) {
        Ins(&R, body, "out $app $out;");
    }
    if (!ch_N(token)) {
        Ins(&R, body, "out $app c;");
    }
    Ins(&R, body, "} break;");
}

// -----------------------------------------------------------------------------

// The body of the rewrite of STYLE, which reads the input STR and appends to OUT.
// A style that escapes reads a backslash as a request for the next character's
// escaped rule; one that does not reads it as a character like any other.
static tempstr RegxRewriteBody(algo_lib::Replscope &R, amc::FRegxstyle &style) {
    tempstr plaincase;
    tempstr esccase;
    amc::FRegxrule *any = NULL;
    amc::FRegxrule *anyesc = NULL;
    ind_beg(amc::regxstyle_c_regxrule_curs, rule, style) {
        strptr token = token_Get(rule);
        bool esc = style.esc && StartsWithQ(token, "\\");
        strptr ch = esc ? RestFrom(token, 1) : token;
        vrfy(elems_N(ch) <= 1, tempstr()<<"amc.regx_token"
             <<Keyval("regxrule",rule.regxrule)
             <<Keyval("comment","a token is one character, or a backslash and one character"));
        if (elems_N(ch) == 0 && esc) {
            anyesc = &rule;
        } else if (elems_N(ch) == 0) {
            any = &rule;
        } else {
            RegxAppendCase(R, esc ? esccase : plaincase, rule, ch);
        }
    }ind_end;
    vrfy(any, tempstr()<<"amc.regx_nocatchall"
         <<Keyval("regxstyle",style.regxstyle)
         <<Keyval("comment","a style needs a rule for the empty token"));
    vrfy(!style.esc || anyesc, tempstr()<<"amc.regx_nocatchall"
         <<Keyval("regxstyle",style.regxstyle)
         <<Keyval("comment","a style that escapes needs a rule for a lone backslash"));
    RegxAppendCase(R, plaincase, *any, "");
    if (anyesc) {
        RegxAppendCase(R, esccase, *anyesc, "");
    }
    tempstr text;
    if (style.esc) {
        Ins(&R, text, "$bool esc = false;");
    }
    Ins(&R, text, "for ($int i = 0; i < $len; i++) {");
    Ins(&R, text, "    $cchr c = str[i];");
    if (style.esc) {
        Ins(&R, text, "    if (esc) {");
        Ins(&R, text, "        esc = false;");
        Ins(&R, text, "        switch (c) {");
        text << esccase;
        Ins(&R, text, "        }");
        Ins(&R, text, "    } else if (c == '\\\\') {");
        Ins(&R, text, "        esc = true;");
        Ins(&R, text, "    } else {");
        Ins(&R, text, "        switch (c) {");
        text << plaincase;
        Ins(&R, text, "        }");
        Ins(&R, text, "    }");
    } else {
        Ins(&R, text, "    switch (c) {");
        text << plaincase;
        Ins(&R, text, "    }");
    }
    Ins(&R, text, "}");
    return text;
}

// -----------------------------------------------------------------------------

// Append to TEXT, at INDENT, the statement that marks the pattern invalid when
// the regxlenient row SHAPE says the shape makes it so, and nothing otherwise.
// Every shape the template tests needs a row.
static void RegxLenientIns(algo_lib::Replscope &R, cstring &text, strptr shape, strptr indent) {
    amc::FRegxlenient *lenient = amc::ind_regxlenient_Find(shape);
    vrfy(lenient, tempstr()<<"amc.regx_nolenient"
         <<Keyval("regxlenient",shape)
         <<Keyval("comment","the normalizer tests this shape, and it needs a row"));
    if (lenient->invalid) {
        Ins(&R, text, tempstr() << indent << "valid = false;");
    }
}

// -----------------------------------------------------------------------------

// The body of the normalizer, which reads STR in the default syntax and appends
// its canonical form to OUT, leaving VALID false when a malformed shape makes the
// pattern invalid.  In the canonical form every character that means itself and
// is punctuation stands behind a backslash, so the only bare punctuation left is
// an operator, every class is closed, and no quantifier follows another.
static tempstr RegxNormalizeBody(algo_lib::Replscope &R) {
    vrfy(amc::ind_regxlenient_FindX("open_noclose").invalid, tempstr()<<"amc.regx_open_noclose"
         <<Keyval("comment","an unclosed ( can only make the pattern invalid"));
    tempstr text;
    // TERM says that something stands to the left within the current alternative,
    // ATOM that it can take a quantifier, and QUANT which quantifier ended it.
    Ins(&R, text, "$bool valid = true;");
    Ins(&R, text, "$int depth = 0;");
    Ins(&R, text, "$bool term = false;");
    Ins(&R, text, "$bool atom = false;");
    Ins(&R, text, "$chr quant = $nochar;");
    Ins(&R, text, "$int i = 0;");
    Ins(&R, text, "while (i < $len) {");
    Ins(&R, text, "    $cchr c = str[i];");
    Ins(&R, text, "    $int step = 1;");
    Ins(&R, text, "    $bool lit = false;");
    Ins(&R, text, "    if (c == '\\\\' && i + 1 < $len) {");
    Ins(&R, text, "        $cchr n = str[i + 1];");
    Ins(&R, text, "        if ($macroq(n)) {");
    Ins(&R, text, "            $putmacro");
    Ins(&R, text, "        } else {");
    Ins(&R, text, "            if ($punctq(n)) {");
    Ins(&R, text, "                out $app '\\\\';");
    Ins(&R, text, "            }");
    Ins(&R, text, "            out $app n;");
    Ins(&R, text, "        }");
    Ins(&R, text, "        step = 2;");
    Ins(&R, text, "        term = true;");
    Ins(&R, text, "        atom = true;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    } else if (c == '[') {");
    Ins(&R, text, "        $int j = i + 1;");
    Ins(&R, text, "        if (j < $len && str[j] == '^') {");
    Ins(&R, text, "            j++;");
    Ins(&R, text, "        }");
    Ins(&R, text, "        $bool closed = false;");
    Ins(&R, text, "        while (j < $len && !closed) {");
    Ins(&R, text, "            if (str[j] == '\\\\') {");
    Ins(&R, text, "                j += 2;");
    Ins(&R, text, "            } else if (str[j] == ']') {");
    Ins(&R, text, "                closed = true;");
    Ins(&R, text, "            } else {");
    Ins(&R, text, "                j++;");
    Ins(&R, text, "            }");
    Ins(&R, text, "        }");
    Ins(&R, text, "        if (closed) {");
    Ins(&R, text, "            out $app '[';");
    Ins(&R, text, "            $int k = i + 1;");
    Ins(&R, text, "            if (str[k] == '^') {");
    Ins(&R, text, "                out $app '^';");
    Ins(&R, text, "                k++;");
    Ins(&R, text, "            }");
    Ins(&R, text, "            while (k < j) {");
    Ins(&R, text, "                $cchr m = str[k];");
    Ins(&R, text, "                if (m == '\\\\' && $macroq(str[k + 1])) {");
    Ins(&R, text, "                    $cchr n = str[k + 1];");
    Ins(&R, text, "                    $classmacro");
    Ins(&R, text, "                    k += 2;");
    Ins(&R, text, "                } else {");
    Ins(&R, text, "                    $chr lo = m;");
    Ins(&R, text, "                    $int kk = k + 1;");
    Ins(&R, text, "                    if (m == '\\\\') {");
    Ins(&R, text, "                        lo = str[k + 1];");
    Ins(&R, text, "                        kk = k + 2;");
    Ins(&R, text, "                    }");
    Ins(&R, text, "                    $chr hi = lo;");
    Ins(&R, text, "                    if (kk + 1 < j && str[kk] == '-') {");
    Ins(&R, text, "                        $cchr e = str[kk + 1];");
    Ins(&R, text, "                        if (e != '\\\\') {");
    Ins(&R, text, "                            hi = e;");
    Ins(&R, text, "                            kk = kk + 2;");
    Ins(&R, text, "                        } else if (!$macroq(str[kk + 2])) {");
    Ins(&R, text, "                            hi = str[kk + 2];");
    Ins(&R, text, "                            kk = kk + 3;");
    Ins(&R, text, "                        }");
    Ins(&R, text, "                    }");
    Ins(&R, text, "                    if ($reversed) {");
    Ins(&R, text, "                        hi = lo;");
    Ins(&R, text, "                    }");
    Ins(&R, text, "                    if ($punctq(lo)) {");
    Ins(&R, text, "                        out $app '\\\\';");
    Ins(&R, text, "                    }");
    Ins(&R, text, "                    out $app lo;");
    Ins(&R, text, "                    if (hi != lo) {");
    Ins(&R, text, "                        out $app '-';");
    Ins(&R, text, "                        if ($punctq(hi)) {");
    Ins(&R, text, "                            out $app '\\\\';");
    Ins(&R, text, "                        }");
    Ins(&R, text, "                        out $app hi;");
    Ins(&R, text, "                    }");
    Ins(&R, text, "                    k = kk;");
    Ins(&R, text, "                }");
    Ins(&R, text, "            }");
    Ins(&R, text, "            out $app ']';");
    Ins(&R, text, "            step = j + 1 - i;");
    Ins(&R, text, "            term = true;");
    Ins(&R, text, "            atom = true;");
    Ins(&R, text, "            quant = $nochar;");
    Ins(&R, text, "        } else {");
    RegxLenientIns(R, text, "class_noclose", "            ");
    Ins(&R, text, "            lit = true;");
    Ins(&R, text, "        }");
    Ins(&R, text, "    } else if (c == '(') {");
    Ins(&R, text, "        out $app c;");
    Ins(&R, text, "        depth++;");
    Ins(&R, text, "        term = false;");
    Ins(&R, text, "        atom = false;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    } else if (c == ')' && depth > 0) {");
    Ins(&R, text, "        out $app c;");
    Ins(&R, text, "        depth--;");
    Ins(&R, text, "        term = true;");
    Ins(&R, text, "        atom = true;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    } else if (c == ')') {");
    RegxLenientIns(R, text, "close_noopen", "        ");
    Ins(&R, text, "        lit = true;");
    Ins(&R, text, "    } else if (c == '|') {");
    // An empty alternative matches the empty string, and the canonical form
    // writes it as an empty group, which both engines read that way.
    Ins(&R, text, "        if (!term) {");
    Ins(&R, text, "            out $app \"()\";");
    Ins(&R, text, "        }");
    Ins(&R, text, "        out $app c;");
    Ins(&R, text, "        if (i + 1 == $len || (str[i + 1] == ')' && depth > 0)) {");
    Ins(&R, text, "            out $app \"()\";");
    Ins(&R, text, "        }");
    Ins(&R, text, "        term = false;");
    Ins(&R, text, "        atom = false;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    } else if ((c == '*' || c == '+' || c == '?') && !atom) {");
    RegxLenientIns(R, text, "quant_noatom", "        ");
    Ins(&R, text, "        lit = true;");
    Ins(&R, text, "    } else if (c == '*' || c == '+' || c == '?') {");
    Ins(&R, text, "        if (quant == $nochar) {");
    Ins(&R, text, "            quant = c;");
    Ins(&R, text, "        } else {");
    Ins(&R, text, "            $droplast");
    Ins(&R, text, "            if (quant != c) {");
    Ins(&R, text, "                quant = '*';");
    Ins(&R, text, "            }");
    Ins(&R, text, "        }");
    Ins(&R, text, "        out $app quant;");
    Ins(&R, text, "    } else if (c == '.') {");
    Ins(&R, text, "        out $app c;");
    Ins(&R, text, "        term = true;");
    Ins(&R, text, "        atom = true;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    } else if (c == '^' || c == $dollar) {");
    Ins(&R, text, "        out $app c;");
    Ins(&R, text, "        term = true;");
    Ins(&R, text, "        atom = false;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    } else {");
    Ins(&R, text, "        lit = true;");
    Ins(&R, text, "    }");
    Ins(&R, text, "    if (lit) {");
    Ins(&R, text, "        if ($punctq(c)) {");
    Ins(&R, text, "            out $app '\\\\';");
    Ins(&R, text, "        }");
    Ins(&R, text, "        out $app c;");
    Ins(&R, text, "        term = true;");
    Ins(&R, text, "        atom = true;");
    Ins(&R, text, "        quant = $nochar;");
    Ins(&R, text, "    }");
    Ins(&R, text, "    i = i + step;");
    Ins(&R, text, "}");
    Ins(&R, text, "valid = valid && depth == 0;");
    return text;
}

// -----------------------------------------------------------------------------

// The body of a predicate that is true for the escape letters of amcdb.regxmacro.
static tempstr RegxMacroQBody(algo_lib::Replscope &R) {
    tempstr text;
    Ins(&R, text, "$bool ret = false;");
    Ins(&R, text, "switch (c) {");
    ind_beg(amc::_db_regxmacro_curs, macro, amc::_db) {
        Set(R, "$mch", RegxCharLit(macro.regxmacro.ch[0]));
        Ins(&R, text, "case $mch:");
    }ind_end;
    Ins(&R, text, "    ret = true;");
    Ins(&R, text, "    break;");
    Ins(&R, text, "default:");
    Ins(&R, text, "    break;");
    Ins(&R, text, "}");
    return text;
}

// -----------------------------------------------------------------------------

// The expression that is true when C is ASCII punctuation.
static strptr RegxPunctExpr() {
    return "(c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~')";
}

// -----------------------------------------------------------------------------

// The condition, on a character C, that C is an operator of STYLE: a character
// that a pattern of STYLE must escape to mean itself.  EQ is the language's
// equality operator.  A character with a rule of
// its own is an operator unless the rule escapes it.  Every other character takes
// the catch-all rule, which passes it to the default syntax, where it is an
// operator when it is one of the characters the normalizer reads as one outside a
// class.
static tempstr RegxQuoteExpr(amc::FRegxstyle &style, strptr eq) {
    strptr defaultop = "\\[()|*+?.^$";
    algo::Charset op;
    amc::FRegxrule *any = NULL;
    ind_beg(amc::regxstyle_c_regxrule_curs, rule, style) {
        strptr token = token_Get(rule);
        if (elems_N(token) == 0) {
            any = &rule;
        }
    }ind_end;
    for (int c = 0x21; c < 0x7f; c++) {
        amc::FRegxrule *own = NULL;
        ind_beg(amc::regxstyle_c_regxrule_curs, rule, style) {
            strptr token = token_Get(rule);
            bool esc = style.esc && StartsWithQ(token, "\\");
            if (!esc && elems_N(token) == 1 && u8(token[0]) == c) {
                own = &rule;
            }
        }ind_end;
        bool isop = own
            ? strptr(own->out) != strptr(tempstr() << '\\' << char(c))
            : (any && ch_N(any->out) > 0) || FindChar(defaultop, char(c)) != -1;
        if (isop) {
            ch_SetBit(op, c);
        }
    }
    tempstr ret;
    algo::ListSep ls(" || ");
    for (int c = 0; c < 256; c++) {
        if (ch_GetBit(op, c)) {
            ret << ls << "c " << eq << " " << RegxCharLit(u8(c));
        }
    }
    if (!ch_N(ret)) {
        ret << "false";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Write the C++ half into the namespace NS: the declarations into its header and
// the definitions into its source.
static void RegxGenCpp(amc::FNs &ns) {
    algo_lib::Replscope R;
    RegxSetLang(R, false);
    Set(R, "$ns", ns.ns);
    cstring &hdr = *ns.hdr;
    cstring &cpp = *ns.cpp;
    amc::BeginNsBlock(hdr, ns, "");
    Ins(&R, hdr, "    // True when C is ASCII punctuation, which the canonical regx syntax escapes when it means itself");
    Ins(&R, hdr, "    bool RegxPunctQ(char c);");
    Ins(&R, hdr, "    // True when \\C is an escape that stands for a class of characters (amcdb.regxmacro)");
    Ins(&R, hdr, "    bool RegxMacroQ(char c);");
    Ins(&R, hdr, "    // The characters escape \\C stands for, as pairs of low and high characters; NEG says it means every other one");
    Ins(&R, hdr, "    algo::strptr RegxMacroClass(char c, bool &neg);");
    Ins(&R, hdr, "    // Append to OUT the default-syntax form of STR, a pattern of regx style STYLE (amcdb.regxrule)");
    Ins(&R, hdr, "    void RegxRewrite(algo::strptr str, $ns::RegxStyle style, algo::cstring &out);");
    Ins(&R, hdr, "    // Append to OUT the canonical form of STR, a default-syntax pattern; return false when the pattern is invalid");
    Ins(&R, hdr, "    bool RegxNormalize(algo::strptr str, algo::cstring &out);");
    Ins(&R, hdr, "    // True when a pattern of regx style STYLE must escape C to mean C (amcdb.regxrule)");
    Ins(&R, hdr, "    bool RegxQuoteQ(char c, $ns::RegxStyle style);");
    amc::EndNsBlock(hdr, ns, "");

    Ins(&R, cpp, "");
    Ins(&R, cpp, "// --- $ns.RegxPunctQ");
    Ins(&R, cpp, "bool $ns::RegxPunctQ(char c) {");
    cpp << "    return " << RegxPunctExpr() << ";" << eol;
    Ins(&R, cpp, "}");
    Ins(&R, cpp, "");
    Ins(&R, cpp, "// --- $ns.RegxMacroQ");
    Ins(&R, cpp, "bool $ns::RegxMacroQ(char c) {");
    InsertIndent(cpp, RegxMacroQBody(R), 1);
    Ins(&R, cpp, "    return ret;");
    Ins(&R, cpp, "}");
    Ins(&R, cpp, "");
    Ins(&R, cpp, "// --- $ns.RegxMacroClass");
    Ins(&R, cpp, "algo::strptr $ns::RegxMacroClass(char c, bool &neg) {");
    Ins(&R, cpp, "    algo::strptr ret;");
    Ins(&R, cpp, "    neg = false;");
    Ins(&R, cpp, "    switch (c) {");
    ind_beg(amc::_db_regxmacro_curs, macro, amc::_db) {
        algo::Charset set;
        RegxReadChclass(macro.chclass, set);
        tempstr pairs(RegxRangePairs(set));
        tempstr pairlit;
        strptr_PrintCppQuoted(pairs, pairlit, '"');
        Set(R, "$mch", RegxCharLit(macro.regxmacro.ch[0]));
        Set(R, "$pairs", pairlit);
        Set(R, "$npair", tempstr() << ch_N(pairs));
        Set(R, "$neg", macro.neg ? "true" : "false");
        Ins(&R, cpp, "    case $mch:");
        Ins(&R, cpp, "        ret = algo::strptr($pairs, $npair);");
        Ins(&R, cpp, "        neg = $neg;");
        Ins(&R, cpp, "        break;");
    }ind_end;
    Ins(&R, cpp, "    default:");
    Ins(&R, cpp, "        break;");
    Ins(&R, cpp, "    }");
    Ins(&R, cpp, "    return ret;");
    Ins(&R, cpp, "}");
    Ins(&R, cpp, "");
    Ins(&R, cpp, "// --- $ns.RegxRewrite");
    Ins(&R, cpp, "void $ns::RegxRewrite(algo::strptr str, $ns::RegxStyle style, algo::cstring &out) {");
    Ins(&R, cpp, "    switch (style.value) {");
    ind_beg(amc::_db_regxstyle_curs, style, amc::_db) {
        Set(R, "$style", style.regxstyle);
        Ins(&R, cpp, "    case $ns_RegxStyle_$style: {");
        InsertIndent(cpp, RegxRewriteBody(R, style), 2);
        Ins(&R, cpp, "    } break;");
    }ind_end;
    Ins(&R, cpp, "    default: {");
    Ins(&R, cpp, "        out << str;");
    Ins(&R, cpp, "    } break;");
    Ins(&R, cpp, "    }");
    Ins(&R, cpp, "}");
    Ins(&R, cpp, "");
    Ins(&R, cpp, "// --- $ns.RegxNormalize");
    Ins(&R, cpp, "bool $ns::RegxNormalize(algo::strptr str, algo::cstring &out) {");
    InsertIndent(cpp, RegxNormalizeBody(R), 1);
    Ins(&R, cpp, "    return valid;");
    Ins(&R, cpp, "}");
    Ins(&R, cpp, "");
    Ins(&R, cpp, "// --- $ns.RegxQuoteQ");
    Ins(&R, cpp, "bool $ns::RegxQuoteQ(char c, $ns::RegxStyle style) {");
    Ins(&R, cpp, "    bool ret = false;");
    Ins(&R, cpp, "    switch (style.value) {");
    ind_beg(amc::_db_regxstyle_curs, style, amc::_db) if (style.esc) {
        Set(R, "$style", style.regxstyle);
        Set(R, "$opexpr", RegxQuoteExpr(style, "=="));
        Ins(&R, cpp, "    case $ns_RegxStyle_$style:");
        Ins(&R, cpp, "        ret = $opexpr;");
        Ins(&R, cpp, "        break;");
    }ind_end;
    Ins(&R, cpp, "    default:");
    Ins(&R, cpp, "        break;");
    Ins(&R, cpp, "    }");
    Ins(&R, cpp, "    return ret;");
    Ins(&R, cpp, "}");
}

// -----------------------------------------------------------------------------

// Write the TypeScript half as ts/gen/algo_lib_regx.ts.
static void RegxGenTs() {
    algo_lib::Replscope R;
    RegxSetLang(R, true);
    cstring &out = amc::outfile_Create("ts/gen/algo_lib_regx.ts").text;
    tempstr styles;
    algo::ListSep ls(" | ");
    ind_beg(amc::_db_regxstyle_curs, style, amc::_db) {
        styles << ls << "'" << style.regxstyle << "'"; // ignore:hand_quote
    }ind_end;
    Set(R, "$union", styles);
    tempstr quotestyles;
    algo::ListSep qs(" | ");
    ind_beg(amc::_db_regxstyle_curs, style, amc::_db) if (style.esc) {
        quotestyles << qs << "'" << style.regxstyle << "'"; // ignore:hand_quote
    }ind_end;
    Set(R, "$quoteunion", quotestyles);
    Ins(&R, out, "// The regx styles a pattern can be written in (amcdb.regxstyle).");
    Ins(&R, out, "export type RegxStyle = $union");
    Ins(&R, out, "");
    Ins(&R, out, "// True when C is ASCII punctuation, which the canonical regx syntax escapes");
    Ins(&R, out, "// when it means itself.");
    Ins(&R, out, "export function regxPunctQ(c: string): boolean {");
    out << "    return " << RegxPunctExpr() << eol;
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// True when \\C is an escape that stands for a class of characters");
    Ins(&R, out, "// (amcdb.regxmacro).");
    Ins(&R, out, "export function regxMacroQ(c: string): boolean {");
    InsertIndent(out, RegxMacroQBody(R), 1);
    Ins(&R, out, "    return ret");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// The JavaScript class escape \\C stands for: its body when INCLASS, and a");
    Ins(&R, out, "// bracketed class otherwise.  An empty string when \\C is no such escape.");
    Ins(&R, out, "export function regxMacroJs(c: string, inclass: boolean): string {");
    Ins(&R, out, "    let ret = ''");
    Ins(&R, out, "    switch (c) {");
    ind_beg(amc::_db_regxmacro_curs, macro, amc::_db) {
        Set(R, "$mch", RegxCharLit(macro.regxmacro.ch[0]));
        tempstr js;
        strptr_PrintCppQuoted(RegxJsClass(macro), js, '\'');
        Set(R, "$js", js);
        Ins(&R, out, "    case $mch:");
        Ins(&R, out, "        ret = $js");
        Ins(&R, out, "        break");
    }ind_end;
    Ins(&R, out, "    }");
    Ins(&R, out, "    return inclass || ret === '' ? ret : '[' + ret + ']'");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// STR, a pattern of regx style STYLE, in the default syntax (amcdb.regxrule).");
    Ins(&R, out, "export function regxRewrite(str: string, style: RegxStyle): string {");
    Ins(&R, out, "    let out = ''");
    Ins(&R, out, "    switch (style) {");
    ind_beg(amc::_db_regxstyle_curs, style, amc::_db) {
        Set(R, "$style", style.regxstyle);
        Ins(&R, out, "    case '$style': {");
        InsertIndent(out, RegxRewriteBody(R, style), 2);
        Ins(&R, out, "    } break;");
    }ind_end;
    Ins(&R, out, "    }");
    Ins(&R, out, "    return out");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// STR, a default-syntax pattern, in the canonical form that algo_lib's parser");
    Ins(&R, out, "// and a JavaScript RegExp read alike, or null when the pattern is invalid.");
    Ins(&R, out, "export function regxNormalize(str: string): string | null {");
    Ins(&R, out, "    let out = ''");
    InsertIndent(out, RegxNormalizeBody(R), 1);
    Ins(&R, out, "    return valid ? out : null");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// STR as the bytes algo_lib's Regx reads, one code unit per byte of its UTF-8");
    Ins(&R, out, "// encoding.  A byte of 0x80 or more becomes U+E080..U+E0FF, private-use code");
    Ins(&R, out, "// units that keep the order of the bytes and have no case.");
    Ins(&R, out, "export function regxBytes(str: string): string {");
    Ins(&R, out, "    let out = str");
    Ins(&R, out, "    if (/[^\\x00-\\x7f]/.test(str)) {");
    Ins(&R, out, "        out = ''");
    Ins(&R, out, "        for (const b of new TextEncoder().encode(str)) {");
    Ins(&R, out, "            out += String.fromCharCode(b < 0x80 ? b : 0xe000 + b)");
    Ins(&R, out, "        }");
    Ins(&R, out, "    }");
    Ins(&R, out, "    return out");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// A compiled regx.  TEST is true when the regx matches SUBJECT.");
    Ins(&R, out, "export interface Regx {");
    Ins(&R, out, "    test(subject: string): boolean");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// PATTERN, a regx of style STYLE, compiled to match what algo_lib's Regx");
    Ins(&R, out, "// matches: the whole subject when FULL, and any part of it otherwise.  Both");
    Ins(&R, out, "// sides read bytes, so a character outside ASCII counts as its UTF-8 length.");
    Ins(&R, out, "// FOLD ignores the case of ASCII letters, which algo_lib's Regx never does.");
    Ins(&R, out, "// Null when the pattern is invalid, which matches nothing.");
    Ins(&R, out, "export function regxCompile(pattern: string, style: RegxStyle, full: boolean, fold = false): Regx | null {");
    Ins(&R, out, "    const body = regxNormalize(regxRewrite(regxBytes(pattern), style))");
    out << "    const re = body === null ? null : new RegExp(full ? '^(?:' + body + ')$' : body, fold ? 'si' : 's')" << eol;
    Ins(&R, out, "    return re === null ? null : { test: (subject: string) => re.test(regxBytes(subject)) }");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// The literal start of STR, a pattern in the canonical form regxNormalize");
    Ins(&R, out, "// writes: the characters every match begins with.  The start ends at the first");
    Ins(&R, out, "// operator, and a literal that a * or ? makes optional is left out.  A pattern");
    Ins(&R, out, "// with an alternative outside every group has no common start, so its start");
    Ins(&R, out, "// is empty.");
    Ins(&R, out, "export function regxPrefix(str: string): string {");
    Ins(&R, out, "    let out = ''");
    Ins(&R, out, "    let depth = 0");
    Ins(&R, out, "    let alt = false");
    Ins(&R, out, "    let stop = false");
    Ins(&R, out, "    let i = 0");
    Ins(&R, out, "    while (i < str.length) {");
    Ins(&R, out, "        const c = str[i]");
    Ins(&R, out, "        let step = 1");
    Ins(&R, out, "        if (c == '\\\\' && i + 1 < str.length) {");
    Ins(&R, out, "            if (!stop) {");
    Ins(&R, out, "                out += str[i + 1]");
    Ins(&R, out, "            }");
    Ins(&R, out, "            step = 2");
    Ins(&R, out, "        } else if (c == '[') {");
    Ins(&R, out, "            stop = true");
    Ins(&R, out, "            let j = i + 1");
    Ins(&R, out, "            while (j < str.length && str[j] != ']') {");
    Ins(&R, out, "                j += str[j] == '\\\\' ? 2 : 1");
    Ins(&R, out, "            }");
    Ins(&R, out, "            step = j + 1 - i");
    Ins(&R, out, "        } else if (c == '(') {");
    Ins(&R, out, "            stop = true");
    Ins(&R, out, "            depth++");
    Ins(&R, out, "        } else if (c == ')') {");
    Ins(&R, out, "            depth--");
    Ins(&R, out, "        } else if (c == '|') {");
    Ins(&R, out, "            stop = true");
    Ins(&R, out, "            alt = alt || depth == 0");
    Ins(&R, out, "        } else if (c == '*' || c == '?') {");
    Ins(&R, out, "            if (!stop) {");
    Ins(&R, out, "                out = out.slice(0, -1)");
    Ins(&R, out, "            }");
    Ins(&R, out, "            stop = true");
    Ins(&R, out, "        } else if (c == '+' || c == '.' || c == '^' || c == $dollar) {");
    Ins(&R, out, "            stop = true");
    Ins(&R, out, "        } else if (!stop) {");
    Ins(&R, out, "            out += c");
    Ins(&R, out, "        }");
    Ins(&R, out, "        i = i + step");
    Ins(&R, out, "    }");
    Ins(&R, out, "    return alt ? '' : out");
    Ins(&R, out, "}");
    Ins(&R, out, "");
    Ins(&R, out, "// The regx styles a backslash escapes in (amcdb.regxstyle esc:Y).");
    Ins(&R, out, "export type RegxQuoteStyle = $quoteunion");
    Ins(&R, out, "");
    Ins(&R, out, "// STR as a pattern of regx style STYLE that matches STR alone.  A backslash");
    Ins(&R, out, "// goes before each character that the style reads as an operator, and the");
    Ins(&R, out, "// rest stays as written.");
    Ins(&R, out, "export function regxQuote(str: string, style: RegxQuoteStyle): string {");
    Ins(&R, out, "    let out = ''");
    Ins(&R, out, "    for (const c of str) {");
    Ins(&R, out, "        let op = false");
    Ins(&R, out, "        switch (style) {");
    ind_beg(amc::_db_regxstyle_curs, style, amc::_db) if (style.esc) {
        Set(R, "$style", style.regxstyle);
        Set(R, "$opexpr", RegxQuoteExpr(style, "==="));
        Ins(&R, out, "        case '$style':");
        Ins(&R, out, "            op = $opexpr");
        Ins(&R, out, "            break");
    }ind_end;
    Ins(&R, out, "        }");
    Ins(&R, out, "        out += op ? '\\\\' + c : c");
    Ins(&R, out, "    }");
    Ins(&R, out, "    return out");
    Ins(&R, out, "}");
}

// -----------------------------------------------------------------------------

// Write the regx style translators into the namespace that defines the RegxStyle
// enum, which is algo_lib, and into ts/gen/algo_lib_regx.ts.  Each amcdb.regxstyle row
// needs a value of that enum, which the C++ switch names.
void amc::gen_ns_regxstyle() {
    amc::FNs &ns = *amc::_db.c_ns;
    amc::FCtype *ctype = amc::ind_ctype_Find("algo_lib.RegxStyle");
    ind_beg(amc::_db_regxmacro_curs, macro, amc::_db) {
        vrfy(ch_N(macro.regxmacro) == 1, tempstr()<<"amc.regx_macrokey"
             <<Keyval("regxmacro",macro.regxmacro)
             <<Keyval("comment","a regxmacro key is the one character after the backslash"));
    }ind_end;
    if (ctype && ctype->p_ns == &ns && amc::regxstyle_N() > 0) {
        RegxGenCpp(ns);
        RegxGenTs();
    }
}
