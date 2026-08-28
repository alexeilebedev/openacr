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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/regx.cpp -- Sql Regx implementation
//
// Regx parser / matcher.
// Runs "breadth first" instead of the "depth first" (like PRCE / perl), so
// avoids any exponential blowups that come from backtracking.
// There is no backtracking because it matches simultaneously on all paths.
// The main idea: we next state we evaluate is not the most advanced (along the string) state,
// but the least advanced. With .* expressions, this results in minimal matches (usually with regx
// you see maximal matches).
// Supports char ranges, wildcards, anchors, capture groups (LIMITED)
// Has some optimizations, such as matching "a.*" against an infinite string will exit as soon as
// 'a' is matched.
// The idea is described in depth in this article
// https://swtch.com/~rsc/regexp/regexp1.html
// and is called "Thompson NFA"
// Main types:
// Regx          - Compiled expression, used to evaluate.
// RegxStyle     - One of supported syntax styles: default, shell, sql, acr, literal
// RegxState     - an NFA state with assertion bitset, a set of char ranges to match, and a set of output states
// RegxOp        - instruction executed by the NFA
// During parsing:
// RegxExpr      - Used during parsing; describes an expression as a single input state (in) and a set of output states (out)
// RegxParse     - Used during parsing, holds expression stack and other temporary structures

#include "include/algo.h"
#include <algorithm>

static int RegxState_ReadStrptrChClass(algo_lib::RegxState &state, strptr str, int i); // ignore:static_fwd_decl
static void RewriteRegx(algo::strptr input, algo_lib::RegxStyle style, algo::cstring &out); // ignore:static_fwd_decl
static int SingleOutput(algo_lib::Bitset &bitset); // ignore:static_fwd_decl

// -----------------------------------------------------------------------------

static inline u16 ChRange(u16 beg, u16 end) {
    return (beg<<8) | (end-1);
}
static inline u16 ChRangeBeg(u16 ch_range) {
    return (ch_range>>8)&0xff;
}
static inline u16 ChRangeEnd(u16 ch_range) {
    return (ch_range & 0xff)+1;
}
static inline bool ChRangeContains(u16 ch_range, u16 ch) {
    return ch >= ChRangeBeg(ch_range) && ch < ChRangeEnd(ch_range);
}

static void ary_AllocSetBit(algo_lib::Bitset &bitset, int n) {
    ary_AllocBit(bitset, n);
    ary_qSetBit(bitset, n);
}
static void TraceParse(algo_lib::RegxParse &regxparse, int i, int phase) {
    if (phase==0) { //parse
        prlog("parse: "
              <<Keyval("expr",regxparse.input)
              <<Keyval("index",i)
              <<Keyval("ch",regxparse.input[i])
              <<Keyval("n",ary_expr_N(regxparse))
              <<Keyval("lastexpr",ToDbgString(ary_expr_Last(regxparse))));
    } else if (phase==1) {//finalize
        prlog("parse: finalize: ");
        prlog(ToDbgString(regxparse));
    }
}

// -----------------------------------------------------------------------------

static algo_lib::RegxState &NewState(algo_lib::Regx &regx, algo_lib::RegxOp op) {
    algo_lib::RegxState &ret = state_Alloc(regx);
    ret.op = op;
    return ret;
}

// Create a new regx state
// Push to the expression stack an expression describing this state
// Return the state
static algo_lib::RegxState& NewExprState(algo_lib::RegxParse &regxparse, algo_lib::RegxOp op = algo_lib::RegxOp()) {
    algo_lib::RegxExpr  &expr = ary_expr_Alloc(regxparse);
    type_SetEnum(expr.type, algo_lib_RegxToken_type_expr);
    int state_idx = state_N(*regxparse.p_regx);
    expr.first = state_idx;
    ary_AllocSetBit(expr.last, state_idx);
    algo_lib::RegxState &state=state_Alloc(*regxparse.p_regx);
    state.op=op;
    return state;
}

// -----------------------------------------------------------------------------

static inline bool AcceptAllQ(const algo_lib::RegxOp &op) {
    return op.op == algo_lib_RegxOp_accept && op.imm==1;
}
static bool NextAcceptQ(algo_lib::Regx &regx, algo_lib::RegxState &state) {
    ind_beg(algo_lib::Bitset_ary_bitcurs,id,state.next) {
        algo_lib::RegxState &other=state_qFind(regx,id);
        if (other.op.op == algo_lib_RegxOp_accept) {
            return true;
        }
    }ind_end;
    return false;
}

// Check if regx accepts all strings.
static void CalcAcceptsAllQ(algo_lib::Regx &regx) {
    // for each state, calculate if reaching this state means reaching accept state.
    // this is true IF:
    // - state has a true arc to another state that has a true arc to accept, and back to this state. between the two states, 1 character is consumed
    if (state_N(regx)>0) {
        int nchange=0;
        do {
            nchange=0;
            ind_beg(algo_lib::regx_state_curs,state,regx) if (!AcceptAllQ(state.op) && (state.op.op == algo_lib_RegxOp_true)) {
                ind_beg(algo_lib::Bitset_ary_bitcurs,id,state.next) {
                    algo_lib::RegxState &other = state_qFind(regx, id);
                    bool x2 = AcceptAllQ(other.op) ||
                        ((other.op.op == algo_lib_RegxOp_true)
                         && (state.op.consume + other.op.consume == 1)
                         && NextAcceptQ(regx,other)
                         && ary_GetBit(other.next, ind_curs(state).index));
                    if (x2) {
                        state.op=algo_lib::RegxOp(algo_lib_RegxOp_accept,1,1);
                        other.op=algo_lib::RegxOp(algo_lib_RegxOp_accept,1,1);
                        nchange++;
                        break;
                    }
                }ind_end;
            }ind_end;
        } while (nchange>0);
        accepts_all_Set(regx.flags,AcceptAllQ(state_qFind(regx,0).op));
    }
}

// -----------------------------------------------------------------------------

// True when OP only forwards the match: it consumes nothing and its test always
// succeeds, so a matcher walking through it learns nothing about the subject.
static bool BranchQ(const algo_lib::RegxOp &op) {
    return op.op == algo_lib_RegxOp_true && op.consume == 0;
}

// True when OP marks where a capture group opened or closed.  It forwards the
// match like a branch does, and differs only in that a caller asking for the
// groups needs to see it.
static bool GroupQ(const algo_lib::RegxOp &op) {
    return op.op == algo_lib_RegxOp_lparen || op.op == algo_lib_RegxOp_rparen;
}

// -----------------------------------------------------------------------------

// Take every state that TESTQ accepts out of the arcs, splicing each one's own
// arcs into its place, so the matcher reaches the same states without stopping
// at a state that tells it nothing.
// A regex such as "a**" or "(a*)*" builds a cycle of such states pointing back
// at one another; without a guard the splice would re-discover the same state
// forever and never terminate.  EXPANDED records the states already spliced for
// the state being rewritten, so each is spliced at most once and a cycle
// collapses in a finite number of passes.
// NEXT is re-expanded on every pass because copying a state's arcs into it also
// copies their length, and a bitset holds no bit past the words it has: a set
// whose bits all fall below 64 is one word long, and or-ing a two-word set into
// it would drop every state numbered 64 or above without saying so.
static void Splice(algo_lib::Regx &regx, bool (*testq)(const algo_lib::RegxOp&)) {
    algo_lib::Bitset next;
    algo_lib::Bitset expanded;
    ary_ExpandBits(expanded,state_N(regx));
    ind_beg(algo_lib::regx_state_curs,state,regx) {
        int nmatch=0;
        ary_ClearBitsAll(expanded);
        do {
            ary_Setary(next,state.next);
            ary_ExpandBits(next,state_N(regx));
            nmatch=0;
            ind_beg(algo_lib::Bitset_ary_bitcurs,id,state.next) {
                algo_lib::RegxState &other = state_qFind(regx, id);
                if (testq(other.op)) {
                    ary_ClearBit(next, id);
                    if (!ary_GetBit(expanded, id)) {
                        ary_SetBit(expanded, id);
                        ary_OrBits(next, other.next);
                    }
                    nmatch++;
                }
            }ind_end;
            if (nmatch) {
                ary_Setary(state.next,next);
            }
        }while(nmatch>0);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Keep each state's arcs as they stand under NEXTGROUP, then splice the group
// boundaries out of NEXT.
// An lparen or rparen state consumes nothing and its test always succeeds, so
// it exists only to record where a capture group opened or closed.  A match
// that asks no question about groups still has to walk through it, and walking
// through a state that consumes nothing costs the matcher an extra pass over
// its whole front before the character advances -- so a single pair of
// parentheses doubles the work of matching.  Every expression read with full:N
// is wrapped in one, which makes this the common case rather than an unusual
// one.
// The matcher therefore follows NEXT, which reaches the same states without
// stopping at the boundaries, and follows NEXTGROUP only when the caller asked
// for the groups.
static void KeepGroupArc(algo_lib::Regx &regx) {
    ind_beg(algo_lib::regx_state_curs,state,regx) {
        ary_Setary(state.nextgroup,ary_Getary(state.next));
    }ind_end;
}

// -----------------------------------------------------------------------------

// Negate the character ranges matched by STATE.CH_CLASS
// For instance, if CH_CLASS matches a..z, then after this function
// it will match anything BUT a..z
static void Negate(algo::U16Ary &ch_class) {
    // replace list of ranges with a complementary one over 0..255
    // this may add or remove a state.
    int k = 0;
    int L = 0;
    frep_(idx, ary_N(ch_class)) {
        u16 R = ary_qFind(ch_class, idx);
        if (L < ChRangeBeg(R)) {
            ary_qFind(ch_class, k) = ChRange(L,ChRangeBeg(R));
            k++;
        }
        L = ChRangeEnd(R);
    }
    if (L < 256) {
        ary_Alloc(ch_class) = ChRange(L,256);
        k++;
    }
    ch_class.ary_n = k; // trim
}

// -----------------------------------------------------------------------------

static void OptimizeChClass(algo_lib::RegxState &state) {
    // sort list of ranges
    // remove overlaps
    // coalesce touching ranges
    if (state.op.op == algo_lib_RegxOp_charrange) {
        std::sort(state.ch_class.ary_elems, state.ch_class.ary_elems + state.ch_class.ary_n);
        //ary_QuickSort(ch_class);
        int prev = -1;
        int end = 0;
        frep_(idx,ary_N(state.ch_class)) {
            u16 R = ary_qFind(state.ch_class, idx);
            u16 Rbeg = ChRangeBeg(R);
            u16 Rend = ChRangeEnd(R);
            Rbeg = i32_Max(Rbeg, end);    // remove overlap
            int new_end = i32_Max(end, Rend);
            if (prev>=0 && Rbeg == end) { // coalesce
                u16 &Rprev=ary_qFind(state.ch_class, prev);
                Rprev=ChRange(ChRangeBeg(Rprev), new_end);
                Rend = 0;
            }
            end = new_end;
            if (Rend > Rbeg) {
                prev++;
                ary_qFind(state.ch_class, prev) = ChRange(Rbeg,Rend);
            }
        }
        state.ch_class.ary_n = prev + 1;
        // rewrite single-character ranges as a simpler test
        // rewrite a full char range range as true
        if (ary_N(state.ch_class)==1) {
            u16 R=ary_qFind(state.ch_class,0);
            u16 Rbeg =ChRangeBeg(R);
            u16 Rend =ChRangeEnd(R);
            if (Rend==Rbeg+1) {
                ary_RemoveAll(state.ch_class);
                state.op.op=algo_lib_RegxOp_char;
                state.op.imm=Rbeg;
            } else if (Rbeg==0 && Rend==256) {
                ary_RemoveAll(state.ch_class);
                state.op.op=algo_lib_RegxOp_true;
                state.op.imm=0;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Add character range BEG..END (INCLUSIVE!)
// to STATE.
static void AddCharRange(algo::U16Ary &ch_class, int beg, int end=0) {
    // this range is EXCLUSIVE
    if (end==0) {
        end=beg;
    }
    ary_Alloc(ch_class) = ChRange(beg,end+1);
}

// C           escaped character
// STATE       out char class.
// Parses escaped chars such as
//    n->newline
//    d->digit
//    w->word
//    s->space etc.
// Non-macro characters are read verbatim (i.e. the leading backslash is ignored).
static bool RegxState_ReadMacro(algo_lib::RegxState &state, char c) {
    algo::U16Ary temp;
    bool neg=false;
    switch (c) {
    case 'n': AddCharRange(temp,'\n'); break;
    case 'r': AddCharRange(temp,'\r'); break;
    case 't': AddCharRange(temp,'\t'); break;
    case 'a': AddCharRange(temp,'\a'); break;
    case 'e': AddCharRange(temp,'\x1b'); break;
    case 'v': AddCharRange(temp,'\v'); break;
    case 'f': AddCharRange(temp,'\f'); break;
    case 'D':
        neg=true;
        AddCharRange(temp,'0','9');
        break;
    case 'd':
        AddCharRange(temp,'0','9');
        break;
    case 'W':
        neg=true;
        AddCharRange(temp,'a','z');
        AddCharRange(temp,'A','Z');
        AddCharRange(temp,'0','9');
        AddCharRange(temp,'_');
        break;
    case 'w':
        AddCharRange(temp,'a','z');
        AddCharRange(temp,'A','Z');
        AddCharRange(temp,'0','9');
        AddCharRange(temp,'_');
        break;
    case 'S':
        neg=true;
        AddCharRange(temp,'\n');
        AddCharRange(temp,'\r');
        AddCharRange(temp,'\t');
        AddCharRange(temp,'\v');
        AddCharRange(temp,' ');
        AddCharRange(temp,'\f');
        break;
    case 's':
        AddCharRange(temp,'\n');
        AddCharRange(temp,'\r');
        AddCharRange(temp,'\t');
        AddCharRange(temp,'\v');
        AddCharRange(temp,' ');
        AddCharRange(temp,'\f');
        break;
    default:
        AddCharRange(temp,c);
    }
    if (neg) {
        Negate(temp);
    }
    ary_Addary(state.ch_class,ary_Getary(temp));
    return true;
}

// -----------------------------------------------------------------------------

// STR         char range string in the form  \^?(A(-B)?)+
// STATE       output char class
// I           starting char index
// RETURN      ending char index
// function stops at ] (DOESN'T SKIP) or end of string
static int RegxState_ReadStrptrChClass(algo_lib::RegxState &state, strptr str, int i) {
    bool neg = false;
    if (i<str.n_elems && str[i]=='^') {
        neg=true;
        i++;
    }
    algo::U16Ary &ch_class=state.ch_class;
    for (; i < elems_N(str); i++) {
        int c    = u8(str[i]);
        int next = i < elems_N(str)-1 ? u8(str[i+1]) : 0;
        int c2   = c+1;
        if (c==']') {
            break;
        }
        if (next && c=='\\' && RegxState_ReadMacro(state, (char)next)) {
            i++;
            continue;
        }
        if (i < elems_N(str)-2 && next=='-') {
            c2=u8(str[i+2])+1;
            i+=2;
        }
        c2=u32_Max(c2,c+1);
        ary_Alloc(ch_class) = ChRange(c,c2);
    }
    if (neg) {
        Negate(ch_class);
    }
    return i;
}

// -----------------------------------------------------------------------------

// Add TO as a next for all of E's next states.
static void Connect(algo_lib::Regx &regx, algo_lib::RegxExpr &E, int to) {
    ind_beg(algo_lib::Bitset_ary_bitcurs,id,E.last) {
        algo_lib::RegxState &state = state_qFind(regx, id);
        ary_AllocSetBit(state.next, to);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Look at the expression stack.
// If the last iten is an expression with precedence <= type, return the top expression.
// Otherwise return NULL
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
static void Reduce(algo_lib::RegxParse &regxparse, int prec) {
    algo_lib::Regx &regx = *regxparse.p_regx;
    int n;
    bool reduce;
    do {
        n = ary_expr_N(regxparse);
        if (n<2) {
            break;
        }
        ary_expr_Reserve(regxparse,1);// prevent crash later
        algo_lib::RegxExpr &A = ary_expr_qFind(regxparse, n-2);
        algo_lib::RegxExpr &B = ary_expr_qFind(regxparse, n-1);
        reduce = false;
        if ((prec >= algo_lib_RegxToken_type_start && A.type == algo_lib_RegxToken_type_start && B.type == algo_lib_RegxToken_type_expr)
            || (prec >= algo_lib_RegxToken_type_expr && A.type == algo_lib_RegxToken_type_expr && B.type == algo_lib_RegxToken_type_expr)) {
            Connect(regx, A, B.first);// connect A->B
            algo::TSwap(A.last,B.last);// B's outputs are new expression's outputs
            ary_expr_RemoveLast(regxparse);// delete B
            reduce=true;
        } else if (prec >= algo_lib_RegxToken_type_or && A.type == algo_lib_RegxToken_type_or && B.type == algo_lib_RegxToken_type_expr) {
            // A=expression to the left of |
            // B=expression to the right of |
            // new state branching into A and B
            algo_lib::RegxState &state=NewState(regx,algo_lib::RegxOp(algo_lib_RegxOp_true,0,0));
            // branch
            ary_AllocSetBit(state.next, A.first);
            ary_AllocSetBit(state.next, B.first);
            A.first=state_N(regx)-1;
            A.type = algo_lib_RegxToken_type_expr;
            ary_ExpandBits(A.last,state_N(regx));
            ary_OrBits(A.last,B.last);
            ary_expr_Remove(regxparse,n-1);// delete B
            reduce=true;
        }
    } while (reduce);
}

// -----------------------------------------------------------------------------

static bool ZeroOrMore(algo_lib::RegxParse &regxparse) {
    algo_lib::Regx &regx = *regxparse.p_regx;
    bool processed = false;
    if (algo_lib::RegxExpr  *regx_expr = RegxExpr_Last(regxparse, algo_lib_RegxToken_type_expr)) {
        int    n = state_N(regx);
        algo_lib::RegxState &state = NewState(regx,algo_lib::RegxOp(algo_lib_RegxOp_true,0,0));
        ary_AllocSetBit(state.next, regx_expr->first);// leads to expression
        Connect(regx, *regx_expr, n);// expr goes back here

        ary_AllocSetBit(regx_expr->last,n);
        regx_expr->first = n;
        processed = true;
    }
    return processed;
}

static bool OneOrMore(algo_lib::RegxParse &regxparse) {
    bool processed=false;
    algo_lib::Regx &regx = *regxparse.p_regx;
    if (algo_lib::RegxExpr *regx_expr = RegxExpr_Last(regxparse, algo_lib_RegxToken_type_expr)) {
        Connect(regx, *regx_expr, regx_expr->first);// jump to beginning
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

// Push left paren to the parsing stack
static bool LeftParen(algo_lib::RegxParse &regxparse) {
    NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_lparen,0,regxparse.nextgroup++));
    ary_expr_qLast(regxparse).type = algo_lib_RegxToken_type_lparen;
    return true;
}

// -----------------------------------------------------------------------------

static bool RightParen(algo_lib::RegxParse &regxparse) {
    algo_lib::Regx &regx=*regxparse.p_regx;
    bool processed=false;
    algo_lib::RegxExpr *A = ary_expr_Find(regxparse, ary_expr_N(regxparse)-2);
    algo_lib::RegxExpr *B = ary_expr_Find(regxparse, ary_expr_N(regxparse)-1);
    if (B && B->type == algo_lib_RegxToken_type_lparen) {
        // empty parentheses
        int n=state_N(regx);
        algo_lib::RegxState &state=NewState(regx,algo_lib::RegxOp(algo_lib_RegxOp_rparen,0,0));
        state.op.imm=state_qFind(regx,A->first).op.imm;// copy capture group
        Connect(regx, *B, n);
        B->type = algo_lib_RegxToken_type_expr;
        ary_ClearBitsAll(B->last);
        ary_AllocSetBit(B->last, n);
        processed=true;
    } else if (A && B && A->type == algo_lib_RegxToken_type_lparen && B->type == algo_lib_RegxToken_type_expr) {
        // create rparen state, needed for the capturegroup
        int n=state_N(regx);
        algo_lib::RegxState &state=NewState(regx,algo_lib::RegxOp(algo_lib_RegxOp_rparen,0,0));
        state.op.imm=state_qFind(regx,A->first).op.imm;// copy capture group
        Connect(regx, *A, B->first);
        Connect(regx, *B, n);
        A->type = algo_lib_RegxToken_type_expr;
        ary_ClearBitsAll(A->last);
        ary_AllocSetBit(A->last, n);
        ary_expr_RemoveLast(regxparse);
        processed=true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool ZeroOrOne(algo_lib::RegxParse &regxparse) {
    algo_lib::Regx &regx = *regxparse.p_regx;
    bool processed = false;
    // find expression on the top of the stack
    // it has a single "first" state and multiple "last" (exit) states.
    // these are all the states that lead to a match
    if (algo_lib::RegxExpr  *regx_expr = RegxExpr_Last(regxparse, algo_lib_RegxToken_type_expr)) {
        // create a new state, this will fork into
        // the existing expression and straight through to exit
        // set the entry point of the expression to the new state
        int n = state_N(regx);
        algo_lib::RegxState &state=NewState(regx,algo_lib::RegxOp(algo_lib_RegxOp_true,0,0));
        // new state -> existing expression
        ary_AllocSetBit(state.next,regx_expr->first);
        regx_expr->first = n;
        // add new state to the list of exit states
        ary_AllocSetBit(regx_expr->last,n);
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool LeftBracket(algo_lib::RegxParse &regxparse, int &i) {
    algo_lib::RegxState &state = NewExprState(regxparse);
    int j = RegxState_ReadStrptrChClass(state, regxparse.input, i+1);
    if (j<elems_N(regxparse.input) && regxparse.input[j]==']') {
        // accept string so far
        i = j;
    } else { // take '[' char verbatim -- ignore built up char class
        ary_RemoveAll(state.ch_class);
        state.op = algo_lib::RegxOp(algo_lib_RegxOp_char,1,'[');
    }
    return true;
}

// -----------------------------------------------------------------------------

// Take the last expression on the stack and set its type to 'or'
static bool Or(algo_lib::RegxParse &regxparse) {
    bool processed = false;
    if (algo_lib::RegxExpr *regx_expr = RegxExpr_Last(regxparse,algo_lib_RegxToken_type_expr)) {
        regx_expr->type  = algo_lib_RegxToken_type_or;
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

static bool Backslash(algo_lib::RegxParse &regxparse, int &i) {
    bool processed = false;
    algo_lib::RegxState &state = NewExprState(regxparse);
    if (i < elems_N(regxparse.input)-1 && RegxState_ReadMacro(state, regxparse.input[i+1])) {
        if(trace_Get(regxparse.p_regx->flags)) prlog("processed escape character... result: "<<algo_lib::ToDbgString(state,state_N(*regxparse.p_regx)-1));
        i++;// read escaped char
        processed = true;
    }
    return processed;
}

// -----------------------------------------------------------------------------

// True when the escape \C stands for exactly one character, which is returned
// through OUT.  The question is put to the macro reader itself rather than to a
// second list of escapes kept beside it, so an escape that gains or loses a
// meaning cannot answer differently in the two places.
static bool SingleCharEscapeQ(char c, char &out) {
    algo_lib::RegxState state;
    bool ret = false;
    RegxState_ReadMacro(state,c);
    if (ary_N(state.ch_class) == 1) {
        u16 R = ary_qFind(state.ch_class,0);
        ret = ChRangeEnd(R) == ChRangeBeg(R)+1;
        out = char(ChRangeBeg(R));
    }
    return ret;
}

// -----------------------------------------------------------------------------

// True when C is a character the default style reads as an operator rather than
// as itself.
static bool RegxMetaQ(char c) {
    return c=='.' || c=='*' || c=='+' || c=='?' || c=='|'
        || c=='(' || c==')' || c=='[' || c==']'
        || c=='^' || c=='$' || c=='\\';
}

// -----------------------------------------------------------------------------

// Read BODY -- an expression already translated to the default style -- as a
// fixed string with an optional ".*" at either end, and return the string
// through LIT.  BEG and END report whether a ".*" stood at that end.
// Most expressions this codebase compiles have this shape: an acr or sql
// pattern is a piece of text with % where anything may go, so "%Regx%" arrives
// here as ".*Regx.*" and reduces to a search for four characters.  Recognizing
// that is worth doing because the NFA's cost for the same answer is a test of
// every state against every character of the subject, and a leading ".*" keeps
// every state alive, so nothing ever drops out of the front.
// Return false when BODY holds anything else, and the NFA does the matching.
static bool FixedStringQ(algo::strptr body, algo::cstring &lit, bool &beg, bool &end) {
    int n = body.n_elems;
    bool ret = true;
    ch_RemoveAll(lit);
    beg = n >= 2 && body[0]=='.' && body[1]=='*';
    end = false;
    int i = beg ? 2 : 0;
    while (i < n && ret) {
        char c = body[i];
        char single = 0;
        if (c=='.' && i+2==n && body[i+1]=='*') {
            end = true;
            i = n;
        } else if (c=='\\' && i+1 < n && SingleCharEscapeQ(body[i+1],single)) {
            lit << single;
            i += 2;
        } else if (RegxMetaQ(c)) {
            ret = false;
        } else {
            lit << c;
            i++;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Record on REGX that its expression reduces to a search for the fixed string
// LIT, which BEG and END place: each says a ".*" stood at that end of the
// expression, so the string may sit anywhere from there.  FULL says the
// expression was read as a whole-subject match; a search instead lets the
// string sit anywhere at both ends whatever the expression said.
static void SetFixed(algo_lib::Regx &regx, algo::strptr lit, bool beg, bool end, bool full) {
    fixed_Set(regx.flags,true);
    anchorbeg_Set(regx.flags,full && !beg);
    anchorend_Set(regx.flags,full && !end);
    regx.lit = lit;
}

// -----------------------------------------------------------------------------

// If BITSET has exactly one bit equal to 1,
// return its index.
// Otherwise, return -1.n
static int SingleOutput(algo_lib::Bitset &bitset) {
    int ret=-1;
    int nnext=0;
    ind_beg(algo_lib::Bitset_ary_bitcurs,next,bitset) {
        nnext++;
        ret=next;
    }ind_end;
    return nnext==1 ? ret : -1;
}

// Calculate if REGX matches a literal string:
// - Each state matches a single character
// - Each state has a single output arc
// True states are allowed as long as they don't consume.
// In addition, the original expression must match rewritten expression.
// This is to prevent surprises when the raw string handed to Regx is not the
// one matching.
static void CalcLiteral(algo_lib::Regx &regx) {
    int i=0;//start state
    if (state_N(regx)>0) {
        tempstr newstr;
        while (i!=-1) {
            algo_lib::RegxState &state=state_qFind(regx,i);
            if (state.op.op == algo_lib_RegxOp_char) {
                newstr << char(state.op.imm);
            } else if (state.op.op==algo_lib_RegxOp_true && state.op.consume==0) {
                // allow -- simple branch
            } else if (state.op.op==algo_lib_RegxOp_accept && state.op.imm==0) {// end-of-string accept
                // expr could be "[a]" but we don't want to rewrite it
                literal_Set(regx.flags, newstr==regx.expr);
                break;
            } else {
                return;// not literal -- char range etc.
            }
            i = SingleOutput(state.next);
        }
    }
}

// -----------------------------------------------------------------------------

// parse regex string
static void Parse(algo_lib::RegxParse &regxparse) {
    algo_lib::Regx &regx = *regxparse.p_regx;
    // create start state, #0 by convention
    NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_true,0,0));
    ary_expr_qFind(regxparse,0).type = algo_lib_RegxToken_type_start;
    for (int i=0; i<elems_N(regxparse.input); i++) {
        bool processed = false;
        char c = regxparse.input[i];
        switch(c) {
        case '*':
            processed = ZeroOrMore(regxparse);
            break;
        case '+':
            processed = OneOrMore(regxparse);
            break;
        case '?':
            processed = ZeroOrOne(regxparse);
            break;
        case '|':
            Reduce(regxparse,algo_lib_RegxToken_type_expr);
            processed = Or(regxparse);
            break;
        case '(':
            Reduce(regxparse,algo_lib_RegxToken_type_expr);
            processed = LeftParen(regxparse);
            break;
        case ')':
            Reduce(regxparse,algo_lib_RegxToken_type_or);
            processed = RightParen(regxparse);
            break;
        case '\\':
            Reduce(regxparse,algo_lib_RegxToken_type_expr);
            processed = Backslash(regxparse,i);
            break;
        case '.':
            {
                Reduce(regxparse,algo_lib_RegxToken_type_expr);
                NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_true,1,0));
                processed = true;
            }
            break;
        case '^':
            {
                Reduce(regxparse,algo_lib_RegxToken_type_expr);
                NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_stringbeg,0,0));
                processed=true;
            }
            break;
        case '$':
            {
                Reduce(regxparse,algo_lib_RegxToken_type_expr);
                NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_stringend,0,0));
                processed=true;
            }
            break;
        case '[':
            Reduce(regxparse,algo_lib_RegxToken_type_expr);
            processed=LeftBracket(regxparse,i);// adjusts i
            break;
        case ']':
            // real right bracket was eaten by LeftBracket
            // this is a sole character
            break;
        default:
            break;
        }
        if (!processed) {// don't know what to do with this char -- take it verbatim
            Reduce(regxparse,algo_lib_RegxToken_type_expr);
            NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_char,1,c));
        }
        if (trace_Get(regx.flags)) {
            TraceParse(regxparse,i,0);
        }
    }
    Reduce(regxparse,algo_lib_RegxToken_type_or);
    NewExprState(regxparse,algo_lib::RegxOp(algo_lib_RegxOp_accept,0,0));
    Reduce(regxparse,algo_lib_RegxToken_type_start);// ultimate reduce
    if (ary_expr_N(regxparse)>1) {
        // couldn't reduce expression stack.
        // probably unmatched parenthesis...
        valid_Set(regx.flags,false);
    }
    ind_beg(algo_lib::regx_state_curs,state,regx) {
        OptimizeChClass(state);
    }ind_end;
    CalcLiteral(regx);
    CalcAcceptsAllQ(regx);
    regx.ngroup = regxparse.nextgroup;
    Splice(regx,BranchQ);
    KeepGroupArc(regx);
    Splice(regx,GroupQ);
    if (trace_Get(regx.flags)) {
        TraceParse(regxparse,0,1);
    }
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

void algo_lib::RegxState_Print(algo_lib::RegxState &state, algo::cstring &lhs) {
    lhs << ToDbgString(state,-1);
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// The positions at which each group opened, on the path that reached state IDX.
// A group's opening position is a property of the path taken to a state, not of
// the state, and one state stands on as many paths as the expression has ways
// of reaching it.  A single position per state is therefore enough only while
// no two groups are open at once: an outer group that opened before an inner
// one has its position overwritten by the inner one, and closing it reports the
// inner group's start.  One position per group per state is what an expression
// with nested groups needs, and the room for it is taken only when the caller
// asked for the groups.
static i32 *SlotAry(algo_lib::RegxM &regxm, int ngroup, int idx) {
    return &slot_qFind(regxm,idx*ngroup);
}

// -----------------------------------------------------------------------------

// Carry the group positions of state FROM along an arc to state TO, so the
// state at the far end of the arc remembers the path that reached it.
static void CopySlot(algo_lib::RegxM &regxm, int ngroup, int from, int to) {
    i32 *src = SlotAry(regxm,ngroup,from);
    i32 *dst = SlotAry(regxm,ngroup,to);
    frep_(k,ngroup) {
        dst[k] = src[k];
    }
}

// -----------------------------------------------------------------------------

static bool ScanString(algo_lib::Regx &regx, const algo::strptr &text) {
    algo_lib::RegxM &regxm = algo_lib::_db.regxm;// temporary matching context
    bool capture = capture_Get(regx.flags);
    ary_RemoveAll(regxm.matchrange);
    ary_RemoveAll(regxm.front);
    ary_RemoveAll(regxm.next_char);
    ary_RemoveAll(regxm.this_char);
    ary_RemoveAll(regxm.visited);
    ary_ExpandBits(regxm.front,regx.state_n);
    ary_ExpandBits(regxm.next_char,regx.state_n);
    ary_ExpandBits(regxm.this_char,regx.state_n);
    ary_ExpandBits(regxm.visited,regx.state_n);
    int ngroup = capture ? regx.ngroup : 0;
    slot_RemoveAll(regxm);
    if (ngroup > 0) {
        slot_AllocNVal(regxm, i64(regx.state_n)*ngroup, -1);
    }
    ary_qSetBit(regxm.front, 0);
    int ch_index=0;
    while (true) {
        int nthis=0;
        u16 ch=ch_index < text.n_elems ? u8(text.elems[ch_index]) : 0xffff;
        if (trace_Get(regx.flags)) {
            prlog("scanning"
                  <<Keyval("ch_index",ch_index)
                  <<Keyval("ch",ch));
        }
        int ntest=0;
        ind_beg(algo_lib::Bitset_ary_bitcurs,idx,regxm.front) {
            ntest++;
            ary_SetBit(regxm.visited, idx);// this state is now closed for this char
            algo_lib::RegxState &state  = state_qFind(regx, idx);
            bool test = false;
            switch (state.op.op) {
            case algo_lib_RegxOp_charrange: {
                ind_beg(algo::U16Ary_ary_curs,r,state.ch_class) {
                    if (ChRangeContains(r,ch)) {
                        test=true;
                        break;
                    }
                }ind_end;
            }break;
            case algo_lib_RegxOp_char: {
                test = ch == state.op.imm;
            }break;
            case algo_lib_RegxOp_true: {
                test = true;
            }break;
            case algo_lib_RegxOp_accept: {
                // imm = always accept?
                test = state.op.imm==1 ? true : ch_index==text.n_elems;
                if (test) {
                    return true;
                }
            }break;
            case algo_lib_RegxOp_lparen: {
                test = true;
                if (ngroup > 0) {
                    SlotAry(regxm,ngroup,idx)[state.op.imm]=ch_index;
                }
            }break;
            case algo_lib_RegxOp_rparen: {
                test = true;
                if (ngroup > 0) {
                    while (ary_N(regxm.matchrange)<=state.op.imm) {
                        ary_Alloc(regxm.matchrange)=algo::i32_Range();
                    }
                    ary_qFind(regxm.matchrange,state.op.imm).beg=SlotAry(regxm,ngroup,idx)[state.op.imm];
                    ary_qFind(regxm.matchrange,state.op.imm).end=ch_index;
                }
            }break;
            case algo_lib_RegxOp_stringbeg: {
                test = ch_index==0;
            }break;
            case algo_lib_RegxOp_stringend: {
                test = ch_index==text.n_elems;
            }break;
            }
            if (trace_Get(regx.flags)) {
                prlog("  "
                      <<ToDbgString(state,idx)
                      <<Keyval("test",test));
            }
            if (test) {
                algo_lib::Bitset &next = capture ? state.nextgroup : state.next;
                // carry the group positions forward along every arc
                if (ngroup > 0) {
                    ind_beg(algo_lib::Bitset_ary_bitcurs,id,next) {
                        CopySlot(regxm,ngroup,idx,id);
                    }ind_end;
                }
                if (state.op.consume) {
                    ary_OrBits(regxm.next_char, next);
                } else {
                    // Follow a zero-consume arc to states tested against the
                    // same char.  A group closed under repetition, such as
                    // "()*" or "(a*)*", forms a cycle of lparen/rparen/true
                    // states that the splice leaves in place when the caller
                    // asked for the captures; forwarding it unguarded would
                    // keep re-adding the same states and never advance the
                    // char.
                    // Only states not yet closed for this char count as
                    // progress, so a cycle drains after one pass.
                    ind_beg(algo_lib::Bitset_ary_bitcurs,nx,next) {
                        if (!ary_GetBit(regxm.visited, nx)) {
                            ary_SetBit(regxm.this_char, nx);
                            nthis++;
                        }
                    }ind_end;
                }
            }
        }ind_end;
        if (ntest==0) {// empty front -> exit
            break;
        }
        if (trace_Get(regx.flags)) {
            prlog("  result"
                  <<Keyval("this",ToDbgString(regxm.this_char))
                  <<Keyval("next",ToDbgString(regxm.next_char))
                  <<Keyval("advance",nthis==0));
        }
        if (nthis>0) {
            ary_ClearBitsAll(regxm.front);
            algo::TSwap(regxm.front,regxm.this_char);
        } else {
            if (ch_index >= text.n_elems) {
                break;
            }
            ary_ClearBitsAll(regxm.front);
            ary_ClearBitsAll(regxm.visited);// new char -> reopen every state
            algo::TSwap(regxm.front,regxm.next_char);
            ch_index++;
            // keep processing non-consuming states...
        }
    }
    return false;
}

// -----------------------------------------------------------------------------

// Locate in TEXT the fixed string REGX reduces to, and return the range it
// occupies, or an empty range beginning at -1 when it is not there.
// Which of the four searches runs is decided by where the string is anchored:
// pinned at both ends the subject has to equal it, pinned at one end the
// subject has to begin or end with it, and pinned at neither it may sit
// anywhere.
static algo::i32_Range FixedMatchQ(algo_lib::Regx &regx, algo::strptr text) {
    algo::i32_Range ret(-1,-1);
    algo::strptr lit = regx.lit;
    bool beg = anchorbeg_Get(regx.flags);
    bool end = anchorend_Get(regx.flags);
    if (beg && end) {
        if (lit == text) {
            ret = algo::i32_Range(0,text.n_elems);
        }
    } else if (beg) {
        if (StartsWithQ(text,lit)) {
            ret = algo::i32_Range(0,lit.n_elems);
        }
    } else if (end) {
        if (EndsWithQ(text,lit)) {
            ret = algo::i32_Range(text.n_elems-lit.n_elems,text.n_elems);
        }
    } else if (lit.n_elems == 0) {
        ret = algo::i32_Range(0,0);
    } else {
        i64 at = FindFrom(text,lit,0,true);
        if (at != -1) {
            ret = algo::i32_Range(int(at),int(at)+lit.n_elems);
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Check if REGX matches TEXT, return result
bool algo_lib::Regx_Match(algo_lib::Regx &regx, algo::strptr text) {
    bool ret = false;
    if (fixed_Get(regx.flags)) {
        algo::i32_Range at = FixedMatchQ(regx,text);
        ret = at.beg != -1;
        if (ret && capture_Get(regx.flags)) {
            ary_RemoveAll(_db.regxm.matchrange);
            ary_Alloc(_db.regxm.matchrange) = at;
        }
    } else if (accepts_all_Get(regx.flags)) {
        ret = true;
    } else if (regx.state_n==0) {
        ret = text.n_elems==0;// can only match empty stringh
    } else {
        ret = ScanString(regx,text);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Find REGX in TEXT and return range of chars which match.
// If the regx is not found, return empty range.
// For this to work, REGX must be parsed with "full:N".
algo::i32_Range algo_lib::Regx_Find(algo_lib::Regx &regx, algo::strptr text, int start DFLTVAL(0)) {
    algo::i32_Range ret;
    capture_Set(regx.flags,true);
    algo::strptr suffix = RestFrom(text,start);
    if (fixed_Get(regx.flags)) {
        algo::i32_Range at = FixedMatchQ(regx,suffix);
        if (at.beg != -1) {
            ret = algo::i32_Range(at.beg+start, at.end+start);
        }
    } else if (Regx_Match(regx,suffix)) {
        // accepts_all skips regular scanning so add a match manually
        if (accepts_all_Get(regx.flags)) {
            ary_RemoveAll(_db.regxm.matchrange);
            ary_Alloc(_db.regxm.matchrange) = algo::i32_Range(0,suffix.n_elems);
        }
        // shifit the match by START
        if (ary_N(_db.regxm.matchrange)>0) {
            ret=ary_qFind(_db.regxm.matchrange,0);
            ret.beg += start;
            ret.end += start;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

void algo_lib::Regx_ReadStyle(algo_lib::Regx &regx, algo::strptr input, algo_lib::RegxStyle style, bool full) {
    valid_Set(regx.flags,true);
    literal_Set(regx.flags,false);
    accepts_all_Set(regx.flags,false);
    fixed_Set(regx.flags,false);
    fullmatch_Set(regx.flags,full);
    state_RemoveAll(regx);
    ch_RemoveAll(regx.lit);
    regx.style = style;
    regx.expr = input;
    if (regx.style.value == algo_lib_RegxStyle_literal) {
        literal_Set(regx.flags,full);
        SetFixed(regx,input,false,false,full);
    } else {
        tempstr body;
        RewriteRegx(input,style,body);
        tempstr lit;
        bool beg = false;
        bool end = false;
        if (FixedStringQ(body,lit,beg,end)) {
            SetFixed(regx,lit,beg,end,full);
        }
        tempstr regx_str;
        if (!full) {
            regx_str << ".*(";
        }
        regx_str << body;
        if (!full) {
            regx_str << ").*";
        }
        algo_lib::RegxParse regxparse;
        regxparse.input = regx_str;
        regxparse.p_regx = &regx;
        ::Parse(regxparse);
    }
}

// -----------------------------------------------------------------------------

// Parse string INPUT as regex and store in REGX.
// Supported features:
// \n,\r,\t,\a,\e,\v,\f     regular escaped chars
// [..]                     char range, possibly with negation
// $, ^                     anchors
// .                        any char
// *                        repetition
// +                        one or more
// (..)                     precedence grouping and capture groups
// \d,\D,\w,\W,\s,\S        macros
void algo_lib::Regx_ReadDflt(algo_lib::Regx &regx, algo::strptr input) {
    Regx_ReadStyle(regx, input, algo_lib_RegxStyle_default, true);
}
void algo_lib::Regx_ReadShell(algo_lib::Regx &regx, algo::strptr input, bool full) {
    Regx_ReadStyle(regx,input,algo_lib_RegxStyle_shell,full);
}
void algo_lib::Regx_ReadSql(algo_lib::Regx &regx, algo::strptr input, bool full) {
    Regx_ReadStyle(regx,input,algo_lib_RegxStyle_sql,full);
}
void algo_lib::Regx_ReadAcr(algo_lib::Regx &regx, algo::strptr input, bool full) {
    Regx_ReadStyle(regx,input,algo_lib_RegxStyle_acr,full);
}
// Set REGX to match string INPUT literally
void algo_lib::Regx_ReadLiteral(algo_lib::Regx &regx, algo::strptr input) {
    Regx_ReadStyle(regx,input,algo_lib_RegxStyle_literal,true);
}
bool algo_lib::Regx_ReadStrptrMaybe(algo_lib::Regx &regx, algo::strptr input) {
    Regx_ReadSql(regx,input,true);
    return true;
}

// -----------------------------------------------------------------------------

algo::tempstr algo_lib::ToDbgString(algo_lib::Bitset &bitset) {
    algo::tempstr ret;
    algo::ListSep ls(",");
    ind_beg(algo_lib::Bitset_ary_bitcurs,state,bitset) {
        ret<<ls<<state;
    }ind_end;
    return ret;
}

static void ShowChar(int ch, cstring &out) {
    out << int(ch);
    out << "(";
    algo::char_PrintCppSingleQuote(ch, out);
    out << ")";
}

algo::tempstr algo_lib::ToDbgString(algo_lib::RegxState &state, int index) {
    algo::tempstr out;
    if (index!=-1) {
        out << "state #"<<index<<" ";
    }
    if (state.op.op==algo_lib_RegxOp_char) {
        ShowChar(state.op.imm,out);
    } else if (state.op.op==algo_lib_RegxOp_charrange) {
        algo::ListSep ls(", ");
        ind_beg(algo::U16Ary_ary_curs,R,state.ch_class) {
            int beg=ChRangeBeg(R);
            int end=ChRangeEnd(R)-1;// inclusive
            out<<ls;
            ShowChar(beg,out);
            if (end>beg) {
                out<<"..";
                ShowChar(end,out);
            }
        }ind_end;
    }
    out << Keyval("op",state.op);
    out << Keyval("next",ToDbgString(state.next));
    out << Keyval("nextgroup",ToDbgString(state.nextgroup));
    return out;
}

algo::tempstr algo_lib::ToDbgString(algo_lib::Regx &regx) {
    return tempstr("Regx")
        <<Keyval("expr",regx.expr)
        <<Keyval("style",regx.style)
        <<Keyval("expr",regx.expr)
        <<Keyval("n_state",state_N(regx))
        <<Keyval("literal",literal_Get(regx.flags))
        <<Keyval("fixed",fixed_Get(regx.flags))
        <<Keyval("lit",regx.lit);
}

tempstr algo_lib::ToDbgString(algo_lib::RegxExpr *expr) {
    tempstr ret;
    if (expr) {
        ret << Keyval("type",expr->type)
            <<Keyval("first",expr->first)
            <<Keyval("last",ToDbgString(expr->last));
    } else {
        ret << "";
    }
    return ret;
}

tempstr algo_lib::ToDbgString(algo_lib::RegxParse &regxparse) {
    algo_lib::Regx &regx=*regxparse.p_regx;
    tempstr out;
    out<<"regx:"<<ToDbgString(regx)<<eol;
    ind_beg(algo_lib::regx_state_curs,state,regx) {
        out<<algo_lib::ToDbgString(state,ind_curs(state).index)<<eol;
    }ind_end;
    ind_beg(algo_lib::regxparse_ary_expr_curs,expr,regxparse) {
        out<<"expr #"<<ind_curs(expr).index<<": "<<algo_lib::ToDbgString(&expr)<<eol;
    }ind_end;
    return out;
}

// -----------------------------------------------------------------------------

// Rewrite string INPUT into OUT (appending to OUT)
// translating from STYLE to default style.
static void RewriteRegx(algo::strptr input, algo_lib::RegxStyle style, algo::cstring &out) {
    switch (style.value) {
    case algo_lib_RegxStyle_acr: {
        // % is rewritten as .*
        // (, ), [, ], _ are passed through
        // ., *, ?, + are escaped
        bool escape = false;
        frep_(i,elems_N(input)) {
            if (escape) {
                escape = false;
                switch(input[i]) {
                case '%' : out<<"%"           ; break; // \% -> %
                case '\\':
                case '(' :
                case ')' :
                case '|' :
                default  : out<<'\\'<<input[i]; break; // \$X -> \$X
                }
            } else {
                switch(input[i]) {
                case '\\': escape = true           ; break; //
                case '%' : out<<".*"          ; break; // % -> .*
                case '.' :
                case '*' :
                case '?' :
                case '^' :
                case '$' :
                case '+' : out<<'\\'<<input[i]; break; // + -> \+, etc.
                case '|' :
                case '(' :
                case ')' :
                case '[' :
                case ']' : out<<input[i]      ; break; // + -> \+
                default  : out<<input[i]      ; break; // $X -> $X
                }
            }
        }
    }break;
    case algo_lib_RegxStyle_sql: {
        // % is rewritten as .*
        // _ is rewritten as .
        // (, ), [, ] are passed through
        // ., *, ?, + are escaped
        bool escape = false;
        frep_(i,elems_N(input)) {
            if (escape) {
                escape = false;
                switch(input[i]) {
                case '%' : out<<"%"           ; break; // \% -> %
                case '_' : out<<"_"           ; break; // \_ -> _
                case '\\': out<<'\\'<<input[i]; break; // \\ -> \\ ...
                case '(' : out<<'\\'<<input[i]; break; // \( -> \(
                case ')' : out<<'\\'<<input[i]; break; // \) -> \)
                case '|' : out<<'\\'<<input[i]; break; // \| -> \|
                default  : out<<'\\'<<input[i]; break; // \$X -> \$X
                }
            } else {
                switch(input[i]) {
                case '\\': escape = true           ; break; //
                case '%' : out<<".*"          ; break; // % -> .*
                case '_' : out<<"."           ; break; // _ -> .
                case '.' :
                case '*' :
                case '?' :
                case '^' :
                case '$' :
                case '+' : out<<'\\'<<input[i]; break; // + -> \+, etc.
                case '|' :
                case '(' :
                case ')' :
                case '[' :
                case ']' : out<<input[i]      ; break; // + -> \+
                default  : out<<input[i]      ; break; // $X -> $X
                }
            }
        }
    }break;
    case algo_lib_RegxStyle_shell: {
        // * -> .*
        // ? -> .?
        // All other regx chars are escaped away
        frep_(i,elems_N(input)) {
            switch(input[i]) {
            case '*':
                out<<".*";
                break;
            case '?':
                out<<".?";
                break;
            case '.':
            case '\\':
            case '[':
            case ']':
            case '(':
            case ')':
            case '^':// anchors
            case '$':
                out<<'\\'<<input[i];
                break;
            default:
                out<<input[i];
            }
        }
    }break;
    case algo_lib_RegxStyle_literal: {
        out << input;
    }break;
    case algo_lib_RegxStyle_default: {
        out << input;
    }break;
    }
}
