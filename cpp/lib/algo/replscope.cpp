// Copyright (C) 2025-2026 AlgoRND
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/replscope.cpp
//
// A Replscope is a set of variables and the machinery that substitutes them
// into text.  A variable's key always begins with a dollar sign, and the text
// being substituted names it by writing that key, so finding a variable means
// answering "which key starts here" for every dollar sign in the text.
// The variables are held in a path-compressed trie, whose root is node 0 and
// stands for the dollar sign itself.  Each node consumes some characters of a
// key, and a node consuming more than one character is what path compression
// means: a run of characters no key branches on costs one node rather than
// one node per character.  A node that names a variable carries the value's
// range within Replscope.value, the single string every value lives in.
// The trie decides the questions a flat table has to be told.  Substituting
// walks it once, one character at a time, and stops at the first node
// carrying a value, which is the shortest key matching at that position.
// Whether one key is a prefix of another -- the ambiguity strict mode
// reports -- is read off the shape of the trie rather than tracked
// separately.
// Invariants:
// - Every node other than the root either names a variable or has a
// descendant that does.  Nodes are created only to carry a value or to
// split a shared prefix, and no node is ever removed on its own, so a
// childless node always names a variable.  This is what lets the prefix
// check be a test of two fields instead of a search.
// - Value ranges never overlap and a node's range is never shared.

#include "include/algo.h"

// -----------------------------------------------------------------------------

static void ReplscopeError(algo_lib::Replscope &scope, algo::strptr err) {
    if (scope.strict) {
        prerr(err);
    }
    vrfy(scope.strict<2, err);
}

// -----------------------------------------------------------------------------

// Write the key of node IDX, dollar sign and all, to OUT.
// A node remembers its parent and not its children's spelling, so the walk
// arrives at the labels in reverse order.  Each label is appended with its
// characters reversed, and reversing the whole appended run afterwards puts
// every character back where it belongs.
static void PrintKey(algo_lib::Replscope &R, int idx, algo::cstring &out) {
    int start = ch_N(out);
    int cur = idx;
    while (cur != -1) {
        algo_lib::Replnode &node = node_qFind(R,cur);
        for (int k = node.key.n_ch-1; k >= 0; k--) {
            out << char(node.key.ch[k]);
        }
        cur = node.up;
    }
    out << '$';
    int end = ch_N(out);
    for (int k = 0; k < (end-start)/2; k++) {
        algo::TSwap(out.ch_elems[start+k], out.ch_elems[end-1-k]);
    }
}

// -----------------------------------------------------------------------------

// True when node IDX sits strictly below node ROOT in the trie.
static bool DescendantQ(algo_lib::Replscope &R, int idx, int root) {
    bool ret = false;
    int cur = node_qFind(R,idx).up;
    while (cur != -1 && !ret) {
        ret = cur == root;
        cur = node_qFind(R,cur).up;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// List to OUT the variables SCOPE defines.  When ROOT is not -1, list only the
// ones sitting below that node, which is the set a newly defined variable is
// reported as intercepting.
static void PrintKeySet(algo_lib::Replscope &R, int root, algo::cstring &out) {
    algo::ListSep ls(",");
    frep_(i,node_N(R)) {
        bool show = node_qFind(R,i).value.beg != -1;
        show = show && (root == -1 || DescendantQ(R,int(i),root));
        if (show) {
            out << ls;
            PrintKey(R,int(i),out);
        }
    }
}

// -----------------------------------------------------------------------------

// Print SCOPE as the variables it defines, since the trie holding them is a
// representation rather than a thing a reader wants to see.
void algo_lib::Replscope_Print(algo_lib::Replscope &row, algo::cstring &str) {
    str << "algo_lib.Replscope";
    str << Keyval("eatcomma",row.eatcomma);
    str << Keyval("strict",row.strict);
    algo::tempstr temp;
    PrintKeySet(row,-1,temp);
    str << Keyval("var",temp);
}

// -----------------------------------------------------------------------------

// Value of the variable node IDX names.
static algo::strptr NodeValue(algo_lib::Replscope &R, int idx) {
    algo::i32_Range range = node_qFind(R,idx).value;
    return algo::qGetRegion(algo::strptr(R.value.ch_elems,R.value.ch_n), range.beg, range.end-range.beg);
}

// -----------------------------------------------------------------------------

// Among the children of PARENT, find the one whose key matches TEXT at POS.
// A node's key may be several characters long, so a child whose first
// character matches while the rest does not is a miss -- and since the
// children's first characters are all distinct, it means no child matches.
// Return the child's index, or -1.
static int MatchChild(algo_lib::Replscope &R, int parent, algo::strptr text, int pos) {
    int ret = -1;
    int cur = node_qFind(R,parent).child;
    while (cur != -1) {
        algo_lib::Replnode &node = node_qFind(R,cur);
        if (pos < text.n_elems && node.key.ch[0] == u8(text.elems[pos])) {
            int n = node.key.n_ch;
            int k = 1;
            while (k < n && pos+k < text.n_elems && node.key.ch[k] == u8(text.elems[pos+k])) {
                k++;
            }
            ret = k == n ? cur : -1;
            cur = -1;
        } else {
            cur = node.next;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Find the shortest variable of SCOPE whose key matches TEXT starting at the
// dollar sign at position POS-1.  Return the node naming it, or -1 when no
// variable matches.  END receives the position just past the matched key, so
// the caller knows where the text resumes.
static int ScanKey(algo_lib::Replscope &R, algo::strptr text, int pos, int &end) {
    int ret = -1;
    int cur = node_N(R) > 0 ? 0 : -1;
    while (cur != -1) {
        if (node_qFind(R,cur).value.beg != -1) {
            ret = cur;
            end = pos;
            cur = -1;
        } else {
            cur = MatchChild(R,cur,text,pos);
            if (cur != -1) {
                pos += node_qFind(R,cur).key.n_ch;
            }
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Find the variable of SCOPE spelled exactly KEY, which excludes the leading
// dollar sign.  Return the node naming it, or -1.  This is what the ${..}
// form needs: the walk has to consume the whole of KEY and land on a node that
// names a variable, rather than stopping at the first one it meets.
static int FindKey(algo_lib::Replscope &R, algo::strptr key) {
    int ret = -1;
    int cur = node_N(R) > 0 ? 0 : -1;
    int pos = 0;
    while (cur != -1) {
        if (pos == key.n_elems) {
            ret = node_qFind(R,cur).value.beg != -1 ? cur : -1;
            cur = -1;
        } else {
            cur = MatchChild(R,cur,key,pos);
            if (cur != -1) {
                pos += node_qFind(R,cur).key.n_ch;
            }
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Give SCOPE its root node, which stands for the dollar sign and names no
// variable of its own until someone defines the variable spelled "$".
static void InitRoot(algo_lib::Replscope &R) {
    if (node_N(R) == 0) {
        algo_lib::Replnode &root = node_Alloc(R);
        root.value = algo::i32_Range(-1,-1);
    }
}

// -----------------------------------------------------------------------------

// Hang a chain of nodes spelling KEY under PARENT, and return the last of them,
// which is the node the caller gives a value to.  A node's key is an inline
// Smallstr10, so a key with more than ten characters left to spell becomes
// several nodes, each the only child of the one before it.
static int AddChain(algo_lib::Replscope &R, int parent, algo::strptr key) {
    int cur = parent;
    int pos = 0;
    while (pos < key.n_elems) {
        int n = i32_Min(key.n_elems-pos, int(algo::Smallstr10::ch_max));
        int idx = int(node_N(R));
        algo_lib::Replnode &node = node_Alloc(R);
        node.key = algo::strptr(key.elems+pos, n);
        node.value = algo::i32_Range(-1,-1);
        node.up = cur;
        algo_lib::Replnode &up = node_qFind(R,cur);
        node.next = up.child;
        up.child = idx;
        cur = idx;
        pos += n;
    }
    return cur;
}

// -----------------------------------------------------------------------------

// Cut node IDX down to the first N characters of its key, moving the rest into
// a new node that takes over the variable IDX named and the children it had.
// IDX is left naming no variable, which is what a shared prefix is.
static void SplitNode(algo_lib::Replscope &R, int idx, int n) {
    int tailidx = int(node_N(R));
    algo_lib::Replnode &tail = node_Alloc(R);
    algo_lib::Replnode &node = node_qFind(R,idx);
    tail.key = algo::strptr((char*)node.key.ch+n, node.key.n_ch-n);
    tail.value = node.value;
    tail.child = node.child;
    tail.next = -1;
    tail.up = idx;
    node.key.n_ch = u8(n);
    node.value = algo::i32_Range(-1,-1);
    node.child = tailidx;
    for (int cur = tail.child; cur != -1; cur = node_qFind(R,cur).next) {
        node_qFind(R,cur).up = tailidx;
    }
}

// -----------------------------------------------------------------------------

// Rewrite SCOPE.value so it holds the values still in use and nothing else,
// moving every node's range onto the new string.
static void Compact(algo_lib::Replscope &R) {
    algo::tempstr temp;
    frep_(i,node_N(R)) {
        algo_lib::Replnode &node = node_qFind(R,i);
        if (node.value.beg != -1) {
            int beg = ch_N(temp);
            temp << NodeValue(R,int(i));
            node.value = algo::i32_Range(beg,ch_N(temp));
        }
    }
    ch_Setary(R.value,temp);
    R.dead = 0;
}

// -----------------------------------------------------------------------------

// True when TO is a view of SCOPE.value itself, which is the one value the
// append below cannot read while it writes.
static bool AliasQ(algo_lib::Replscope &R, algo::strptr to) {
    return to.n_elems > 0 && to.elems >= R.value.ch_elems && to.elems < R.value.ch_elems+R.value.ch_n;
}

// -----------------------------------------------------------------------------

// Make TO the value of the variable node IDX names.
// Every value in the scope lives in one string, so a value that fits where the
// node's previous one sat is written in place and anything else is appended.
// The bytes a replaced value leaves behind are counted, and the scope rewrites
// the string once they outweigh the values still in use -- otherwise a scope
// that is never reset would grow by the size of every value it was ever given.
// A node that named no variable yet has no range to write into, so it takes
// the appending branch whatever the length of TO, and that is what turns it
// into a node naming a variable whose value happens to be empty.
// TO may be a view of the very string being appended to, so the append reserves
// its room first and then re-derives the source from the offset it noted --
// growing the string moves the bytes but not their positions.
static void NodeSetValue(algo_lib::Replscope &R, int idx, algo::strptr to) {
    algo::i32_Range old = node_qFind(R,idx).value;
    int oldlen = old.beg == -1 ? 0 : old.end-old.beg;
    if (old.beg != -1 && to.n_elems <= oldlen) {
        memmove(R.value.ch_elems+old.beg, to.elems, to.n_elems);
        node_qFind(R,idx).value = algo::i32_Range(old.beg, old.beg+to.n_elems);
        R.dead += oldlen - to.n_elems;
    } else {
        int alias = AliasQ(R,to) ? int(to.elems - R.value.ch_elems) : -1;
        ch_Reserve(R.value, to.n_elems);
        int beg = ch_N(R.value);
        ch_Addary(R.value, alias == -1 ? to : algo::strptr(R.value.ch_elems+alias, to.n_elems));
        node_qFind(R,idx).value = algo::i32_Range(beg, ch_N(R.value));
        R.dead += oldlen;
    }
    if (R.dead > 1024 && R.dead > ch_N(R.value)/2) {
        Compact(R);
    }
}

// -----------------------------------------------------------------------------

// Among the children of PARENT, find the one whose key begins with the same
// character KEY has at POS, and report through NSHARE how many characters the
// two keys share.  Return the child's index, or -1 when no child begins with
// that character.  Insertion needs this where substitution needs MatchChild:
// a child whose key only partly matches is where the trie has to be split,
// rather than a miss.
static int SharedChild(algo_lib::Replscope &R, int parent, algo::strptr key, int pos, int &nshare) {
    int ret = -1;
    int cur = node_qFind(R,parent).child;
    nshare = 0;
    while (cur != -1) {
        algo_lib::Replnode &node = node_qFind(R,cur);
        if (node.key.ch[0] == u8(key.elems[pos])) {
            int lim = i32_Min(int(node.key.n_ch), key.n_elems-pos);
            int k = 0;
            while (k < lim && node.key.ch[k] == u8(key.elems[pos+k])) {
                k++;
            }
            nshare = k;
            ret = cur;
            cur = -1;
        } else {
            cur = node.next;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Define the variable spelled FROM, whose leading dollar sign is the trie's
// root, and give it the value TO.
// Walking down to the node that will hold the value meets the two ways a key
// can be ambiguous.  Passing through a node that already names a variable
// means that variable's key is a prefix of FROM, so a substitution written as
// FROM would stop short and take the shorter value.  Landing on a node that
// names nothing but has children means the reverse: FROM is a prefix of
// variables already defined, and defining it intercepts every substitution
// meant for them.  A strict scope reports either one, and the definition goes
// ahead regardless, since a warning-only scope has to end up with the variable
// it asked for.
static void SetVar(algo_lib::Replscope &R, algo::strptr from, algo::strptr to) {
    if (from.n_elems == 0 || from.elems[0] != '$') {
        ReplscopeError(R, tempstr()<<"Replscope"
                       <<Keyval("key",from)
                       <<Keyval("comment","a variable key begins with a dollar sign; this one could never be matched"));
    } else {
        InitRoot(R);
        int cur = 0;
        int pos = 1;
        int target = -1;
        while (target == -1) {
            if (pos == from.n_elems) {
                target = cur;
            } else if (node_qFind(R,cur).value.beg != -1) {
                if (R.strict) {
                    tempstr msg;
                    msg << "Replscope: existing variable ";
                    PrintKey(R,cur,msg);
                    msg << " Is a prefix of newly defined variable:" << from;
                    ReplscopeError(R, msg);
                }
                target = AddChain(R,cur,algo::RestFrom(from,pos));
            } else {
                int nshare = 0;
                int child = SharedChild(R,cur,from,pos,nshare);
                if (child == -1) {
                    target = AddChain(R,cur,algo::RestFrom(from,pos));
                } else {
                    if (nshare < node_qFind(R,child).key.n_ch) {
                        SplitNode(R,child,nshare);
                    }
                    pos += nshare;
                    cur = child;
                }
            }
        }
        algo_lib::Replnode &node = node_qFind(R,target);
        if (node.value.beg == -1 && node.child != -1 && R.strict) {
            tempstr msg;
            msg << "Replscope: new variable " << from << " Is a prefix of previously defined variable(s): ";
            PrintKeySet(R,target,msg);
            ReplscopeError(R, msg);
        }
        NodeSetValue(R,target,to);
    }
}

// -----------------------------------------------------------------------------

// Set value of key KEY value VALUE
// KEY        string to replace
// VALUE      value to replace it with
// SUBST      If set, $-expand the VALUE parameter
// NOTES
// With 'strict' Replscope, avoid expressions like this:
//   Set(R, "$var", field.comment);
// This will trigger an error when field.comment contains a $ sign and the substitution fails.
// Use
//   Set(R, "$var", field.comment, false);
// A value with no dollar sign in it substitutes to itself, so the expansion
// pass runs only when there is a dollar sign to expand.
//
void algo_lib::Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst DFLTVAL(true)) {
    bool expand = subst && to.n_elems > 0 && memchr(to.elems,'$',to.n_elems) != NULL;
    if (expand) {
        tempstr temp;
        Ins(&scope, temp, to, false);
        SetVar(scope,from,temp);
    } else {
        SetVar(scope,from,to);
    }
}

// -----------------------------------------------------------------------------

// Skip the comma or period, and the space after it, that a substitution to the
// empty string leaves stranded in TEXT at position J.
static int EatComma(strptr text, int j) {
    if (j < text.n_elems && (text.elems[j] == ',' || text.elems[j] == '.')) {
        j++;
    }
    if (j < text.n_elems && algo_lib::WhiteCharQ(text.elems[j])) {
        j++;
    }
    return j;
}

// -----------------------------------------------------------------------------

// Read the variable written at position I of TEXT, where a dollar sign stands,
// and return its value, leaving I at the character just past the variable.
// The plain form takes the shortest key that matches, so with $field and
// $fieldval both defined a text reaches the longer one only by writing
// ${fieldval}.  The braced form is read by looking for the closing brace and
// asking whether the name it encloses is defined: a brace closing a name
// nobody defined is not the end of the expression, so the scan goes on to the
// next one, and it gives up when it meets the dollar sign that starts the next
// substitution.
// When nothing matches, the scanner advances one character and returns the
// dollar sign itself, which leaves the text as it was.  A strict scope reports
// the failure first, and a scope with strict at 2 throws.
static algo::strptr ScanVar(algo_lib::Replscope &R, algo::strptr text, int &i) {
    algo::strptr ret;
    int j = i+1;
    int node = -1;
    if (j < text.n_elems-1 && text.elems[j] == '{') {
        j++;
        bool stop = false;
        while (j < text.n_elems && !stop) {
            if (text.elems[j] == '}') {
                node = FindKey(R, algo::strptr(text.elems+(i+2), j-(i+2)));
                stop = node != -1;
                j++;
            } else if (text.elems[j] == '$') {
                stop = true;
            } else {
                j++;
            }
        }
    } else {
        node = ScanKey(R,text,i+1,j);
        if (node == -1) {
            j = i+1;
        }
    }
    if (node != -1) {
        ret = NodeValue(R,node);
        if (ret.n_elems == 0 && R.eatcomma) {
            j = EatComma(text,j);
        }
    } else {
        if (R.strict) {
            tempstr defined;
            PrintKeySet(R,-1,defined);
            ReplscopeError(R,tempstr()<<"Replscope"
                           <<Keyval("text",LimitLengthEllipsis(text,80))
                           <<Keyval("pos",i)
                           <<Keyval("at",LimitLengthEllipsis(algo::RestFrom(text,i),20))
                           <<Keyval("comment",tempstr()<<"substitution pattern not found. defined: "<<defined));
        }
        ret = qGetRegion(text,i,j-i);
    }
    i = j;
    return ret;
}

// -----------------------------------------------------------------------------

// Append TEXT to OUT, performing $-substitution using variables from SCOPE (must be non-NULL)
// if EOL is set, then new line is appended at the end.
void algo_lib::Ins(algo_lib::Replscope *scope, algo::cstring &out, strptr text, bool eol DFLTVAL(true)) {
    int i = 0;
    int lim = text.n_elems;
    int start = 0;
    while (i < lim) {
        const char *dollar = (const char*)memchr(text.elems+i, '$', lim-i);
        if (dollar) {
            i = int(dollar - text.elems);
            out << qGetRegion(text,start,i-start);
            out << ScanVar(*scope,text,i);
            start = i;
        } else {
            i = lim;
        }
    }
    if (i > start) {
        out << qGetRegion(text,start,i-start);
    }
    if (eol) {
        out << '\n';
    }
}

// -----------------------------------------------------------------------------

// Enable comma-eating (default true)
void algo_lib::eatcomma_Set(algo_lib::Replscope &scope, bool enable) {
    scope.eatcomma=enable;
}

// -----------------------------------------------------------------------------

// Perform $-substitutions in TEXT and return new value.
tempstr algo_lib::Subst(algo_lib::Replscope &scope, strptr text) {
    tempstr retval;
    Ins(&scope, retval, text, false);
    return retval;
}

// -----------------------------------------------------------------------------

// Forget every variable SCOPE defines, and the storage their values occupy.
void algo_lib::Reset(algo_lib::Replscope &scope) {
    node_RemoveAll(scope);
    ch_RemoveAll(scope.value);
    scope.dead = 0;
}
