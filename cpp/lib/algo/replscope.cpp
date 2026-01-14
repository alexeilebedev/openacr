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

#include "include/algo.h"

// -----------------------------------------------------------------------------

static void ReplscopeError(algo_lib::Replscope &scope, algo::strptr err) {
    if (scope.strict) {
        prerr(err);
    }
    vrfy(scope.strict<2, err);
}

// -----------------------------------------------------------------------------

// Add all prefixes of FROM as partial Replvars to R
// (For warnings)
static void AddPrefixes(algo_lib::Replscope &R, algo::strptr from) {
    for (int i=from.n_elems-1; i>1; i--) {
        algo::strptr prefix=ch_FirstN(from,i);
        algo_lib::FReplvar *oldvar = algo_lib::ind_replvar_Find(R,prefix);
        if (oldvar) {
            if (!oldvar->partial && R.strict) {
                tempstr msg = tempstr()<<"Replscope: existing variable "<<prefix<<" Is a prefix of newly defined variable:"<<from;
                ReplscopeError(R, msg);
            }
            break;
        } else {
            // add prefix
            algo_lib::FReplvar &newvar = algo_lib::replvar_Alloc();
            newvar.p_replscope = &R;
            newvar.partial=true;
            newvar.key = prefix;
            replvar_XrefMaybe(newvar);
        }
    }
}

// -----------------------------------------------------------------------------

static void SetVar(algo_lib::Replscope &R, strptr from, strptr to) {
    // make sure the new variable is not a suffix of any existin variable
    algo_lib::FReplvar *replvar = algo_lib::ind_replvar_Find(R, from);
    bool isnew=replvar == NULL;
    if (isnew) {
        replvar              = &algo_lib::replvar_Alloc();
        replvar->p_replscope = &R;
        replvar->key         = from;
        (void)replvar_XrefMaybe(*replvar);
    } else {
        // Check if FROM is a prefix of another variable
        // If so, it can intercept substitutions ment for the other variable
        if (replvar->partial && R.strict) {
            tempstr msg = tempstr()<<"Replscope: new variable "<<from<<" Is a prefix of previously defined variable(s): ";
            algo::ListSep ls(",");
            ind_beg(algo_lib::replscope_ind_replvar_curs,var,R) {
                if (!var.partial && StartsWithQ(var.key,from)) {
                    msg<<ls<<var.key;
                }
            }ind_end;
            ReplscopeError(R, msg);
        }
        replvar->partial = false; // no longer partial
    }
    replvar->value = to;
    if (R.strict && isnew) {
        AddPrefixes(R,from);
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
//
void algo_lib::Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst DFLTVAL(true)) {
    if (subst) {
        tempstr temp;
        Ins(&scope, temp, to, false);
        SetVar(scope,from,temp);
    } else {
        SetVar(scope,from,to);
    }
}

// -----------------------------------------------------------------------------

//
// An empty substitution followed by
static int EatComma(strptr text, int j) {
    if (j < text.n_elems && (text.elems[j] == ',' || text.elems[j] == '.')) {
        j++;
    }
    if (j < text.n_elems && algo_lib::WhiteCharQ(text.elems[j])) {
        j++;
    }
    return j;
}

// Scan TEXT starting at position I for a variable that is defined in R
// If the variable is found, I is set to the character position right after
// the variable, and the variable value is returned.
// Initially I points to the '$' character.
// If the variable is not found because end-of-string is found the substitution fails,
// and the scanner advances one character.
// In this case, if R.FATAL is set, an exception is thrown; otherwise
// I is advanced 1 character and that character is returned as the substitution,
// leaving the original string untouched.
// If one variable is a prefix another (e.g. $field, $fieldval), then
// one must use ${fieldval} to avoid the shortest substitution being used
static strptr ScanVar(algo_lib::Replscope &R, strptr text, int &i) {
    strptr ret;
    int j=i+1;// right after the $
    algo_lib::FReplvar *replvar=NULL;
    if (j<text.n_elems-1 && text.elems[j]=='{') {    // scan ${....} expression
        for (j++; j<text.n_elems; j++) {
            if (text.elems[j]=='}') {
                tempstr key = tempstr()<<'$'<<strptr(text.elems+(i+2),j-(i+2));// '$' + variable name
                replvar = algo_lib::ind_replvar_Find(R,key);
                if (replvar && !replvar->partial) {
                    j++;// skip '}'
                    break;
                }
            } else if (text.elems[j]=='$') {
                break;// exit on start of next substitution
            }
        }
    } else { // scan character-by-character
        for (; j<=text.n_elems; j++) {
            replvar = algo_lib::ind_replvar_Find(R,strptr(text.elems+i, j-i));
            if (replvar && !replvar->partial) {
                break;
            }
        }
        if (j > text.n_elems) {
            j=i+1;
        }
    }
    if (replvar && !replvar->partial) {
        ret=replvar->value;
        if (ret.n_elems==0 && R.eatcomma) {
            j = EatComma(text,j);
        }
    } else {
        if (R.strict) {
            tempstr defined;
            algo::ListSep ls;
            ind_beg(algo_lib::replscope_ind_replvar_curs,var,R) if (!var.partial) {
                defined<<ls<<var.key;
            }ind_end;
            ReplscopeError(R,tempstr()<<"Replscope"
                           <<Keyval("text",LimitLengthEllipsis(text,80))
                           <<Keyval("pos",i)
                           <<Keyval("at",LimitLengthEllipsis(strptr(text.elems+i, text.n_elems-i),20))
                           <<Keyval("comment",tempstr()<<"substitution pattern not found. defined: "<<defined));
        }
        // leave text intact
        ret=strptr(text.elems+i, j-i);
    }
    i=j;// whatever we grabbed so far
    return ret;
}

// -----------------------------------------------------------------------------

// Append TEXT to OUT, performing $-substitution using variables from SCOPE (must be non-NULL)
// if EOL is set, then new line is appended at the end.
void algo_lib::Ins(algo_lib::Replscope *scope, algo::cstring &out, strptr text, bool eol DFLTVAL(true)) {
    int i=0;
    int lim = text.n_elems;
    int start=0;
    strptr var;
    while (i < lim) {
        int ch = text.elems[i];
        if (ch=='$') {
            out << qGetRegion(text,start,i-start);// previous stuff
            out << ScanVar(*scope,text,i);// updates i
            start=i;
        } else {
            i++;
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
