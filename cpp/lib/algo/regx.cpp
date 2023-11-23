// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/regx.cpp -- Sql Regx implementation
//
// Regx parser / matcher.
// Supports parentheses, char ranges, ?, *, + (full list of macros below)
// escaped char -> cranges

#include "include/algo.h"

static algo::RegxMacro _macro[]  = {
    { 'n' ,"\n"}
    ,{'r' ,"\r"}
    ,{'t' ,"\t"}
    ,{'a' ,"\a"}
    ,{'e' ,"\x1b"} // standard way to indicate \e
    ,{'v' ,"\v"}
    ,{'f' ,"\f"}
    ,{'[' ,"[" }
    ,{']' ,"]" }
    ,{'\\',"\\"}
    ,{'$' ,"$" }
    ,{'.' ,"." }
    ,{'|' ,"|" }
    ,{'?' ,"?" }
    ,{'*' ,"*" }
    ,{'+' ,"+" }
    ,{'(' ,"(" }
    ,{')' ,")" }
    ,{'{' ,"{" }
    ,{'}' ,"}" }
    ,{'d' ,"0-9" }
    ,{'D' ,"^0-9" }
    ,{'w' ,"a-zA-Z0-9_"}
    ,{'W' ,"^a-zA-Z0-9_"}
    ,{'s' ,"\n\r\t\v \f"}
    ,{'S' ,"^\n\r\t\v \f"}
};

static int RegxState_ReadStrptrChClass(algo_lib::RegxState &state, strptr str);

// -----------------------------------------------------------------------------

// Return true if state has a total (all chars allowed) transition.
static bool TotalQ(algo_lib::RegxState &state) {
    bool ret=false;
    ind_beg(algo_lib::state_ch_class_curs,ch_class,state) {
        if (ch_class == algo::i32_Range(0,INT_MAX)) {
            ret=true;
            break;
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Check if regx accepts all strings.
// This is true if the start state is an accept state, and it contains a total (all chars allowed)
// transition to itself. We don't detect other patterns.
static void CalcAcceptsAllQ(algo_lib::Regx &regx) {
    bool ret=false;
    if (ary_GetBit(regx.start, regx.accept)) {
        ind_beg(algo_lib::Bitset_ary_bitcurs,start,regx.start) {
            if (algo_lib::RegxState *state = state_Find(regx,start)) {
                ret = ary_GetBit(state->out, regx.accept) && TotalQ(*state);
                if (ret) {
                    break;
                }
            }
        }ind_end;
    }
    regx.accepts_all=ret;

    // see if reaching some state implies inevitably reaching accept state
    // Need a local cycle plus an arc to accept.
    // (just an arc to accept would mean the string must end here).
    ind_beg(algo_lib::regx_state_curs,state,regx) {
        state.accept_all = ary_GetBit(state.out, regx.accept)
            && ary_GetBit(state.out, ind_curs(state).index)
            && TotalQ(state);
    }ind_end;
}

// -----------------------------------------------------------------------------

// C           escaped character
// STATE       out char class.
// Parses escaped chars such as d->digit, w->word, s->space, $->$
static bool RegxState_ReadChar(algo_lib::RegxState &state, char c) {
    bool retval = false;
    algo::aryptr<algo::RegxMacro> _g_macro(_macro,sizeof(_macro)/sizeof(_macro[0]));
    ind_beg_aryptr(algo::RegxMacro,macro,_g_macro) {
        if (c == macro.c) {
            RegxState_ReadStrptrChClass(state, macro.crange);
            retval = true;
            break;
        }
    }ind_end_aryptr;
    return retval;
}

// -----------------------------------------------------------------------------

// STR         char range string in the form  \^?(A(-B)?)+
// STATE       output char class
// RETURN      number of chars processed.
// function stops at ] (doesn't skip) or end of string
static int RegxState_ReadStrptrChClass(algo_lib::RegxState &state, strptr str) {
    int i=0;
    bool neg = elems_N(str) && str[0]=='^';
    i += neg;
    for (; i < elems_N(str); i++) {
        int c    = u8(str[i]);
        int next = i < elems_N(str)-1 ? u8(str[i+1]) : 0;
        int c2   = c+1;
        if (i>0 && c==']') {
            break;
        }
        if (next && c=='\\' && RegxState_ReadChar(state, (char)next)) {
            i++;
            continue;
        }
        if (i < elems_N(str)-2 && next=='-') {
            c2=u8(str[i+2])+1;
            i+=2;
        }
        ch_class_Alloc(state) = algo::i32_Range(c,u32_Max(c2,c+1));
    }

    //
    // normalize
    // sort list of ranges
    // remove overlaps
    // coalesce touching ranges
    //
    ch_class_QuickSort(state);
    int prev = -1;
    int end = 0;
    frep_(idx,ch_class_N(state)) {
        algo::i32_Range R = ch_class_qFind(state, idx);
        R.beg = i32_Max(R.beg, end);    // remove overlap
        int new_end = i32_Max(end, R.end);
        if (prev>=0 && R.beg == end) { // coalesce
            ch_class_qFind(state, prev).end = new_end;
            R.end            = 0;
        }
        end = new_end;
        if (R.end > R.beg) {
            prev++;
            ch_class_qFind(state, prev) = R;
        }
    }
    // trim
    state.ch_class_n = prev + 1;

    // negate
    if (neg) {
        // replace list of ranges with a complementary one
        // (over 0..INT_MAX)
        // this may add or remove a state.
        //
        int k = 0;
        int L = 0;
        frep_(idx, ch_class_N(state)) {
            algo::i32_Range R = ch_class_qFind(state, idx);
            if (L < R.beg) {
                ch_class_qFind(state, k) = algo::i32_Range(L,R.beg);
                k++;
            }
            L             = R.end;
        }
        if (L < INT_MAX) {
            ch_class_Alloc(state) = algo::i32_Range(L,INT_MAX);
            k++;
        }
        state.ch_class_n = k; // trim
    }
    return i;
}

// -----------------------------------------------------------------------------

static algo_lib::RegxState& NewState(algo_lib::RegxParse &regxparse) {
    algo_lib::RegxExpr  &E = ary_expr_Alloc(regxparse);
    type_SetEnum(E.type, algo_lib_RegxToken_type_expr);
    int        s = state_N(*regxparse.p_regx);
    E.in         = s;
    algo_lib::RegxState &S = state_Alloc(*regxparse.p_regx);
    ary_AllocBit(E.out, s);
    ary_qSetBit(E.out, s);
    return S;
}

// -----------------------------------------------------------------------------

static void AddOut(algo_lib::Regx &regx, algo_lib::RegxExpr &E, int out) {
    ind_beg(algo_lib::Bitset_ary_bitcurs,id,E.out) {
        algo_lib::RegxState &state = state_qFind(regx, id);
        ary_AllocBit(state.out, out);
        ary_qSetBit(state.out, out);
    }ind_end;
}

// -----------------------------------------------------------------------------

static algo_lib::RegxExpr *RegxExpr_Last(algo_lib::RegxParse &regxparse, algo_lib::RegxToken type) {
    algo_lib::RegxExpr *retval = NULL;
    if (ary_expr_N(regxparse) > 0) {
        algo_lib::RegxExpr &last = ary_expr_qLast(regxparse);
        if (last.type.type <= type.type) {
            retval = &last;
        }
    }
    return retval;
}

// -----------------------------------------------------------------------------

// stack  precedence    action
// A B    >=EXPR        concat
// A|B    >=OR          alternate
static void Reduce(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx, int prec) {
    int n;
    bool reduce;
    do {
        n = ary_expr_N(regxparse);
        if (n<2) {
            break;
        }
        algo_lib::RegxExpr &A = ary_expr_qFind(regxparse, n-2);
        algo_lib::RegxExpr &B = ary_expr_qFind(regxparse, n-1);
        algo_lib::RegxToken t1 = A.type;
        algo_lib::RegxToken t2 = B.type;
        reduce = false;
        if (prec >= algo_lib_RegxToken_type_expr && t1 == algo_lib_RegxToken_type_expr && t2 == algo_lib_RegxToken_type_expr) {
            AddOut(regx, A, B.in);// concat
            algo::TSwap(A.out,B.out);
            reduce=true;
        }
        if (prec >= algo_lib_RegxToken_type_or && t1 == algo_lib_RegxToken_type_or && t2 == algo_lib_RegxToken_type_expr) {
            int    s = state_N(regx);
            algo_lib::RegxState &S = state_Alloc(regx);
            ary_AllocBit(S.out, A.in);
            ary_qSetBit(S.out, A.in);
            ary_AllocBit(S.out, B.in);
            ary_qSetBit(S.out, B.in);


            // copy B to A.
            // first stretch A.
            while (ary_N(A.out) < ary_N(B.out)) {
                ary_Alloc(A.out) = 0;
            }
            ary_OrBits(A.out,B.out);

            A.in = s;
            type_SetEnum(A.type, algo_lib_RegxToken_type_expr);
            reduce=true;
        }
        if (!reduce) {
            break;
        }
        ary_expr_RemoveLast(regxparse);
    } while (true);
}

// -----------------------------------------------------------------------------

// eliminate states with empty char classes,
// adding their outputs to their predecessors' inputs.
static void ClearBubbles(algo_lib::Regx &regx) {
    algo_lib::Bitset &clear = regx.front;
    algo_lib::Bitset &set   = regx.next_front;
    bool changed;
    do {
        changed = false;
        ind_beg_aryptr(algo_lib::RegxState,S,state_Getary(regx)) {
            ary_ClearBitsAll(clear);
            ary_ClearBitsAll(set  );
            ind_beg(algo_lib::Bitset_ary_bitcurs,out,S.out) {
                algo_lib::RegxState &next = state_qFind(regx, out);
                if (ary_qGetBit(S.out, out) && !ch_class_N(next)) {
                    ary_qSetBit (clear, out);
                    ary_OrBits(set  , next.out);
                    changed = true;
                }
            }ind_end;
            ary_OrBits  (S.out, set);
            ary_ClearBits(S.out, clear);
        }ind_end_aryptr;
    } while (changed);
}

// -----------------------------------------------------------------------------

static bool ZeroOrMore(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx) {
    bool processed = false;
    if (algo_lib::RegxExpr  *regx_expr = RegxExpr_Last(regxparse, algo_lib_RegxToken_type_expr)) {
        int    n = state_N(regx);
        algo_lib::RegxState &state = state_Alloc(regx);
        ary_AllocBit(state.out, regx_expr->in);
        ary_qSetBit(state.out, regx_expr->in);
        AddOut(regx, *regx_expr, n);
        ary_AllocBit(regx_expr->out,n);
        ary_qSetBit(regx_expr->out,n);
        regx_expr->in = n;
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool LeftParen(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx) {
    Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
    algo_lib::RegxExpr &regx_expr = ary_expr_Alloc(regxparse);
    regx_expr.type  = algo_lib_RegxToken_type_lparen;
    return true;
}

// -----------------------------------------------------------------------------

static bool RightParen(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx) {
    bool processed = false;
    Reduce(regxparse,regx,algo_lib_RegxToken_type_or);
    int n   = ary_expr_N(regxparse);
    if (n >= 2) {
        algo_lib::RegxExpr &A = ary_expr_qFind(regxparse, n-2);
        algo_lib::RegxExpr &B = ary_expr_qFind(regxparse, n-1);
        if (A.type==algo_lib_RegxToken_type_lparen && B.type==algo_lib_RegxToken_type_expr) {
            ary_expr_Remove(regxparse, n-2);
            processed = true;
        }
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool ZeroOrOne(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx) {
    bool processed = false;
    if (algo_lib::RegxExpr  *regx_expr = RegxExpr_Last(regxparse, algo_lib_RegxToken_type_expr)) {
        int    n = state_N(regx);
        algo_lib::RegxState &state = state_Alloc(regx);
        ary_AllocBit(state.out, regx_expr->in);
        ary_qSetBit(state.out, regx_expr->in);
        ary_AllocBit(regx_expr->out,n);
        ary_qSetBit(regx_expr->out,n);
        regx_expr->in=n;
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool LeftBracket(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx, int &i) {
    Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
    algo_lib::RegxState &state = NewState(regxparse);
    int j = RegxState_ReadStrptrChClass(state, regxparse.input);
    if (j<elems_N(regxparse.input) && regxparse.input[j]==']') {
        i += j;
    } else { // take '[' char verbatim -- ignore built up char class
        ch_class_RemoveAll(state);
        ch_class_Alloc(state) = algo::i32_Range(u8('['),u8('['+1));
    }
    return true;
}

// -----------------------------------------------------------------------------

static bool Or(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx) {
    bool processed = false;
    Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
    if (algo_lib::RegxExpr *regx_expr = RegxExpr_Last(regxparse,algo_lib_RegxToken_type_expr)) {
        regx_expr->type  = algo_lib_RegxToken_type_or;
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool Backslash(algo_lib::RegxParse &regxparse, algo_lib::Regx &regx, int &i) {
    bool processed = false;
    Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
    algo_lib::RegxState &state = NewState(regxparse);
    if (i < elems_N(regxparse.input)-1 && RegxState_ReadChar(state, regxparse.input[i+1])) {
        i++;// read escaped char
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static void RegxReset(algo_lib::Regx &regx) {
    // reset any previous regx state -- actually, leaving it here doesn't hurt
    // except for wasting memory.
    state_RemoveAll(regx);
    ary_RemoveAll(regx.front);
    ary_RemoveAll(regx.next_front);
    ary_RemoveAll(regx.start);
    regx.accept = 0;
}

// -----------------------------------------------------------------------------

// parse regex string
static void RunRegxParse(algo_lib::RegxParse &regxparse) {
    algo_lib::Regx &regx = *regxparse.p_regx;
    RegxReset(regx);
    for (int i=0; i<elems_N(regxparse.input); i++) {
        bool processed = false;
        char c = regxparse.input[i];
        switch(c) {
        case '*':
            processed=ZeroOrMore(regxparse,regx);
            break;
        case '+':
            if (algo_lib::RegxExpr *regx_expr = RegxExpr_Last(regxparse, algo_lib_RegxToken_type_expr)) {
                AddOut(regx, *regx_expr, regx_expr->in);
                processed = true;
            }
            break;
        case '?':
            processed = ZeroOrOne(regxparse,regx);
            break;
        case '|':
            processed = Or(regxparse,regx);
            break;
        case '(':
            processed = LeftParen(regxparse,regx);
            break;
        case ')':
            processed = RightParen(regxparse,regx);
            break;
        case '\\':
            processed = Backslash(regxparse,regx,i);
            break;
        case '.':
            {
                Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
                algo_lib::RegxState &state = NewState(regxparse);
                ch_class_Alloc(state) = algo::i32_Range(0,INT_MAX);
                processed = true;
            }
            break;
        case '[':
            processed=LeftBracket(regxparse,regx,i);
            break;
        case ']':
            break;
        default:
            { // take char as-is
                Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
                algo_lib::RegxState &state = NewState(regxparse);
                ch_class_Alloc(state) = algo::i32_Range(u8(c),u8(c+1));
                processed = true;
            }
            break;
        }
        if (!processed) {// don't know what to do with this char -- take it verbatim
            regx.parseerror = true;// non-fatal
            Reduce(regxparse,regx,algo_lib_RegxToken_type_expr);
            algo_lib::RegxState &state = NewState(regxparse);
            ch_class_Alloc(state) = algo::i32_Range(u8(c),u8(c+1));
        }
    }
    Reduce(regxparse,regx,algo_lib_RegxToken_type_or);
    if (ary_expr_N(regxparse)>=1) {
        algo_lib::RegxExpr &regx_expr = ary_expr_qFind(regxparse, 0);
        ary_AllocBit(regx.front     , state_N(regx));
        ary_AllocBit(regx.next_front, state_N(regx));
        ary_AllocBit(regx.start     , state_N(regx));
        int acc = state_N(regx);
        ch_class_Alloc(state_Alloc(regx)) = algo::i32_Range(0,INT_MAX);
        AddOut(regx, regx_expr, acc);
        int start = regx_expr.in;
        ClearBubbles(regx);
        if (ch_class_N(state_qFind(regx, start)) > 0) {
            ary_qSetBit(regx.start, start);
        } else {
            ary_OrBits(regx.start, state_qFind(regx, start).out);
        }
        regx.accept = acc;
    } else {
        regx.parseerror=true;// non-fatal
    }
    CalcAcceptsAllQ(regx);
}

// -----------------------------------------------------------------------------

// there is not enough information in a regx expression to fully specify it.
// sql vs shell vs classic regx, vs acr, partial vs full.
// we print back the original expression that was read in, but the information
// about what function read it is lost.
void algo_lib::Regx_Print(algo_lib::Regx &regx, algo::cstring &lhs) {
    // copout -- printing regx as it was read
    lhs << regx.expr;
}

// -----------------------------------------------------------------------------

static bool ScanString(algo_lib::Regx &regx, const algo::strptr &text) {
    ary_ClearBitsAll(regx.next_front);
    ary_Setary(regx.front, regx.start);
    ind_beg_aryptr(char,c,text) {// scan the string
        u32 match = 0;
        ind_beg(algo_lib::Bitset_ary_bitcurs,idx,regx.front) {
            algo_lib::RegxState &state  = state_qFind(regx, idx);
            if (state.accept_all) {// we win -- short circuit
                return true;
            }
            // test if char matches char class
            ind_beg_aryptr(algo::i32_Range,r,ch_class_Getary(state)) {
                if (u8(c) >= r.beg && u8(c) < r.end) {
                    match++;
                    ary_OrBits(regx.next_front, state.out);
                    break;
                }
            }ind_end_aryptr;
        }ind_end;
        ary_ClearBitsAll(regx.front);
        // is swapping arrays like this legal?
        algo::TSwap(regx.front,regx.next_front);
        if (!match) {
            break;
        }
    }ind_end_aryptr;
    return ary_qGetBit(regx.front,regx.accept);
}

// -----------------------------------------------------------------------------

// Check if REGX matches S, return result
bool algo_lib::Regx_Match(algo_lib::Regx &regx, algo::strptr text) {
    bool ret = false;
    if (regx.accepts_all) {
        ret = true;// matches all strings
    } else if (regx.literal) {
        ret = regx.expr == text;// matches literally
    } else if (ary_N(regx.front) == 0) {
        ret = text.n_elems == 0; // matches empty string only
    } else {
        ret = ScanString(regx,text);
    }
    //prerr(regx.expr<<" // "<< text << " // "<<ret);
    return ret;
}

// -----------------------------------------------------------------------------

// Parse string INPUT as regex and store in REGX.
// Supported features:
// \n,\r,\t,\a,\e,\v,\f, [..], $, ^,
// ., *, +, (..), {..}, \d,\D,\w,\W,\s,\S
void algo_lib::Regx_ReadDflt(algo_lib::Regx &regx, algo::strptr input) {
    regx.expr = input;
    algo_lib::RegxParse regxparse;
    regxparse.input = input;
    regxparse.p_regx = &regx;
    ::RunRegxParse(regxparse);
}

// -----------------------------------------------------------------------------

// Parse bash-style regx:
// * -> .*
// ? -> .?
// All other regx chars are escaped away
// if FULL is set to false, input is treated as ".*input.*"
void algo_lib::Regx_ReadShell(algo_lib::Regx &regx, algo::strptr input, bool full) {
    regx.expr = input;
    regx.literal=true;
    tempstr regx_str;
    if (!full) {
        regx.literal=false;
        regx_str << ".*";
    }
    frep_(i,elems_N(input)) {
        switch(input[i]) {
        case '*':
            regx_str<<".*";
            regx.literal=false;
            break;
        case '?':
            regx_str<<".?";
            regx.literal=false;
            break;
        case '.':
        case '\\':
        case '[':
        case ']':
        case '(':
        case ')':
            regx.literal=false;
            regx_str<<'\\'<<input[i];
            break;
        default:
            regx_str<<input[i];
        }
    }
    if (!full) {
        regx_str << ".*";
    }
    algo_lib::RegxParse regxparse;
    regxparse.input = regx_str;
    regxparse.p_regx = &regx;
    ::RunRegxParse(regxparse);
}

// -----------------------------------------------------------------------------

// Parse SQL-style regx:
// % is rewritten as .*
// _ is rewritten as .
// (, ), [, ] are passed through
// ., *, ?, + are escaped
// if FULL is set to false, input is treated as ".*input.*"
void algo_lib::Regx_ReadSql(algo_lib::Regx &regx, algo::strptr input, bool full) {
    regx.expr = input;
    regx.literal=true;
    tempstr regx_str;
    if (!full) {
        regx.literal=false;
        regx_str << ".*";
    }
    bool escape = false;
    frep_(i,elems_N(input)) {
        if (escape) {
            escape = false;
            switch(input[i]) {
            case '%' : regx_str<<"%"           ; break; // \% -> %
            case '_' : regx_str<<"_"           ; break; // \_ -> _
            case '\\': regx_str<<'\\'<<input[i]; break; // \\ -> \\ ...
            case '(' : regx_str<<'\\'<<input[i]; break; // \( -> \(
            case ')' : regx_str<<'\\'<<input[i]; break; // \) -> \)
            case '|' : regx_str<<'\\'<<input[i]; break; // \| -> \|
            default  : regx_str<<'\\'<<input[i]; break; // \$X -> \$X
            }
        } else {
            switch(input[i]) {
            case '\\': escape = true           ; regx.literal=false; break; //
            case '%' : regx_str<<".*"          ; regx.literal=false; break; // % -> .*
            case '_' : regx_str<<"."           ; regx.literal=false; break; // _ -> .
            case '.' :
            case '*' :
            case '?' :
            case '+' : regx_str<<'\\'<<input[i]; break; // + -> \+, etc.
            case '|' :
            case '(' :
            case ')' :
            case '[' :
            case ']' : regx_str<<input[i]      ; regx.literal=false; break; // + -> \+
            default  : regx_str<<input[i]      ; break; // $X -> $X
            }
        }
    }
    if (!full) {
        regx_str << ".*";
    }
    algo_lib::RegxParse regxparse;
    regxparse.input = regx_str;
    regxparse.p_regx = &regx;
    ::RunRegxParse(regxparse);
}

// -----------------------------------------------------------------------------

// Parse ACR-style regx:
// % is rewritten as .*
// (, ), [, ], _ are passed through
// ., *, ?, + are escaped
// if FULL is set to false, input is treated as ".*input.*"
// If the input expression can be matched as a string, set REGX.LITERAL to true
void algo_lib::Regx_ReadAcr(algo_lib::Regx &regx, algo::strptr input, bool full) {
    regx.expr = input;
    regx.literal=true;
    tempstr regx_str;
    if (!full) {
        regx.literal=false;
        regx_str << ".*";
    }
    bool escape = false;
    frep_(i,elems_N(input)) {
        if (escape) {
            escape = false;
            switch(input[i]) {
            case '%' : regx_str<<"%"           ; break; // \% -> %
            case '\\':
            case '(' :
            case ')' :
            case '|' :
            default  : regx_str<<'\\'<<input[i]; break; // \$X -> \$X
            }
        } else {
            switch(input[i]) {
            case '\\': escape = true           ; regx.literal=false; break; //
            case '%' : regx_str<<".*"          ; regx.literal=false; break; // % -> .*
            case '.' :
            case '*' :
            case '?' :
            case '+' : regx_str<<'\\'<<input[i]; break; // + -> \+, etc.
            case '|' :
            case '(' :
            case ')' :
            case '[' :
            case ']' : regx_str<<input[i]      ; regx.literal=false; break; // + -> \+
            default  : regx_str<<input[i]      ; break; // $X -> $X
            }
        }
    }
    if (!full) {
        regx_str << ".*";
    }
    algo_lib::RegxParse regxparse;
    regxparse.input = regx_str;
    regxparse.p_regx = &regx;
    ::RunRegxParse(regxparse);
}

// -----------------------------------------------------------------------------

// Set REGX to match string INPUT literally
void algo_lib::Regx_ReadLiteral(algo_lib::Regx &regx, algo::strptr input) {
    RegxReset(regx);
    regx.expr = input;
    regx.literal=true;
}

// -----------------------------------------------------------------------------

bool algo_lib::Regx_ReadStrptrMaybe(algo_lib::Regx &regx, algo::strptr input) {
    Regx_ReadSql(regx,input,true);
    return true;
}
