// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: acr_compl (exe) -- ACR shell auto-complete for all targets
// Exceptions: NO
// Source: cpp/acr/compl/shell.cpp
//

#include "include/acr_compl.h"

// Parse _db.line, split to args into _db.word array
// The aim is to be more accurate and have less ambiguity vs. supplied in COMP_WORD (for complete -F),
// or in args: cmd word precword

void acr_compl::Main_SplitLineToArgv() {
    strptr buf = _db.line;
    i32 ind = 0;
    i32 word_start=0;
    i32 paren_level=0;
    enum {normal,space,squote,dquote,bquote,dbquote,paren} state = normal;
    while (ind < elems_N(buf)) {
        switch (state) {
        case normal:
            switch (buf[ind]) {
            case ' ' : // no break
            case '\t': // no break
            case '\n': if (!paren_level) {
                    word_Alloc() = RestFrom(FirstN(buf,ind),word_start);
                    word_start=ind+1;
                }
                state=space;    ++ind; break;
            case '\'': state=squote;   ++ind; break;
            case '"' : state=dquote;   ++ind; break;
            case '`' : state=bquote;   ++ind; break;
            case '\\':                ind+=2; break;
            case '(' : ++paren_level;  ++ind; break;
            case ')' : --paren_level;  ++ind; break;
            default  :                 ++ind; break;
            }
            break;
        case space:
            switch (buf[ind]) {
            case ' ' : // no break
            case '\t': // no break
            case '\n': ++ind;
                if (!paren_level) {
                    word_start=ind;
                }
                break;
            default  : state = normal;        break;
            }
            break;
        case squote:
            switch (buf[ind]) {
            case '\'': state = normal; ++ind; break;
            default  :                 ++ind; break;
            }
            break;
        case dquote:
            switch (buf[ind]) {
            case '\\':                ind+=2; break;
            case '"' : state=normal;   ++ind; break;
            case '`' : state=dbquote;  ++ind; break;
            default  :                 ++ind; break;
            }
            break;
        case bquote:
            switch (buf[ind]) {
            case '\\':                ind+=2; break;
            case '`' : state=normal;   ++ind; break;
            case '(' : ++paren_level;  ++ind; break;
            case ')' : --paren_level;  ++ind; break;
            default  :                 ++ind; break;
            }
            break;
        case dbquote:
            switch (buf[ind]) {
            case '\\':                ind+=2; break;
            case '`' : state=dquote;   ++ind; break;
            default  :                 ++ind; break;
            }
            break;
        default:
            vrfy_(0);
        }
    }
    word_Alloc() = RestFrom(FirstN(buf,ind),word_start);
}

//
// Perform single and double quote shell expansion
// Other expansions are not supported
// qtype  is quote type on which the input string ends (active quoting)
// qpoint is the point in the ___output___ string corresponding to the point
//        in the input string where the active quote starts

tempstr acr_compl::Main_ShellExpand(strptr in, acr_compl::Shellqtype *qtype, i32 *qpoint) {
    tempstr out;
    acr_compl_ShellqtypeEnum state = acr_compl_Shellqtype_none;
    i32 state_point= 0;
    i32 ind = 0;
    while (ind < elems_N(in)) {
        switch (state) {
        case acr_compl_Shellqtype_none:
            switch (in[ind]) {
            case '$' : /* var, ansi, subshell, arithmetic,  expansion is not supported */
            case '{' : /* brace expansion is not supported */
            case '~' : /* tilda expansion is not supported */
            case '`' : /* command subst is not supported*/
            case '(' : /* subshell is not supported */
            case '*' : /* filename expansion is not supported */
            case '?' : /* filename expansion is not supported */
            case '[' : /* filename expansion is not supported */
            case '!' : /* history expansion is not supported */
                state = acr_compl_Shellqtype_error;
                state_point = ch_N(out);
                break;
            case '\\':
                if (++ind < elems_N(in)) {
                    out << in[ind++];
                }
                break;
            case '\'':
                state = acr_compl_Shellqtype_squote;
                state_point = ch_N(out);
                ++ind;
                break;
            case '"':
                state = acr_compl_Shellqtype_dquote;
                state_point = ch_N(out);
                ++ind;
                break;
            default  :
                out << in[ind++];
            }
            break;
        case acr_compl_Shellqtype_squote:
            if (in[ind] == '\'') {
                state = acr_compl_Shellqtype_none;
                state_point = ch_N(out);
                ++ind;
            } else {
                out << in[ind++];
            }
            break;
        case acr_compl_Shellqtype_dquote:
            switch (in[ind]) {
            case '"':
                state = acr_compl_Shellqtype_none;
                state_point = ch_N(out);
                ++ind;
                break;
            case '\\':
                if (++ind < elems_N(in)) {
                    out << in[ind++];
                }
                break;
            case '$':
            case '!':
            case '`':
                state = acr_compl_Shellqtype_error;
                state_point = ch_N(out);
                break;
            default :
                out << in[ind++];
                break;
            }
            break;
        case acr_compl_Shellqtype_error:
            ind = elems_N(in);
        }
    }
    if (state == acr_compl_Shellqtype_error) {
        out = in;
        state_point = 0;
    }
    if (qtype) {
        type_SetEnum(*qtype,state);
    }
    if (qpoint) {
        *qpoint = state_point;
    }
    return out;
}

// properly escape the character for: 1) readline, 2) bash

void acr_compl::char_PrintEscBash(char c, cstring &out, acr_compl_ShellqtypeEnum type) {
    switch (type) {
    case acr_compl_Shellqtype_none:
        switch (c) {
        case '\t':
        case '\n':
        case ' ' :
        case '!' :
        case '"' :
        case '#' :
        case '$' :
        case '&' :
        case '\'':
        case '(' :
        case ')' :
        case '*' :
            //        case ':' : /// i think we can do without this
        case ';' :
        case '<' :
        case '>' :
        case '?' :
        case '[' :
        case '\\':
        case ']' :
        case '`' :
        case '{' :
        case '|' :
        case '}' :
        case '~' :
            out << '\\';
            break;
        default:
            break;
        }
        break;
    case acr_compl_Shellqtype_dquote:
        switch (c) {
        case '$' :
        case '`' :
        case '!' :
        case '\\':
            out << '\\';
            break;
        default:
            break;
        }
        break;
    case acr_compl_Shellqtype_squote:
        if (c=='\'') out <<"'\\'";
        break;
    default:
        break;
    }
    out << c;
}

void acr_compl::strptr_PrintEscBash(strptr str, cstring &out, acr_compl_ShellqtypeEnum type) {
    frep_(i,elems_N(str)) {
        char_PrintEscBash(str[i],out,type);
    }
}
