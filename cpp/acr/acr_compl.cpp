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
// Target: acr_compl (exe) -- ACR shell auto-complete for all targets
// Exceptions: NO
// Source: cpp/acr/acr_compl.cpp
//
// Created By: alexei.lebedev jeffrey.wang
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev jeffrey.wang alexey.polovinkin
//

#include "include/gen/acr_compl_gen.h"
#include "include/gen/acr_compl_gen.inl.h"

// Parse _db.line, split to args into _db.word array
// The aim is to be more accurate and have less ambiguity vs. supplied in COMP_WORD (for complete -F),
// or in args: cmd word precword

static void Main_SplitLineToArgv() {
    strptr buf = acr_compl::_db.line;
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
                    acr_compl::word_Alloc() = RestFrom(FirstN(buf,ind),word_start);
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
    acr_compl::word_Alloc() = RestFrom(FirstN(buf,ind),word_start);

    //ind_beg(acr_compl::_db_word_curs,word,acr_compl::_db) {
    //    prlog(">"<<word<<"<");
    //}ind_end;
}

//
// Perform single and double quote shell expansion
// Other expansions are not supported
// qtype  is quote type on which the input string ends (active quoting)
// qpoint is the point in the ___output___ string corresponding to the point
//        in the input string where the active quote starts

static tempstr Main_ShellExpand(strptr in, acr_compl::Shellqtype *qtype, i32 *qpoint) {
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

static void char_PrintEscBash(char c, cstring &out, acr_compl_ShellqtypeEnum type) {
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

static void strptr_PrintEscBash(strptr str, cstring &out, acr_compl_ShellqtypeEnum type) {
    frep_(i,elems_N(str)) {
        char_PrintEscBash(str[i],out,type);
    }
}

// Generate a script to install completion handler
//
static void Main_Install(strptr prog) {
    prlog("# This is a script suitable for inclusion in .bash_profile");
    tempstr out;
    out<<"complete -o default -o nospace -C "<<prog;
    ind_beg(acr_compl::_db_ns_curs,ns,acr_compl::_db) {
        if (ns.nstype == dmmeta_Nstype_nstype_exe) {
            out<<" "<<ns.ns;
        }
    }ind_end;
    prlog(out);
}

// -----------------------------------------------------------------------------
//
// Create completion entries for string VALUE, which is a piece of a key for
// type CTYPE
//

static void Main_Line_Ctype(acr_compl::FCtype *ctype, strptr value, strptr compl_prefix) {
    if (ctype) {
        // Read VALUE into a regex.
        algo_lib::Regx value_regx;
        Regx_ReadSql(value_regx, value, false);
        // Process only if ssimfile exists
        if (ctype->c_ssimfile && c_field_N(*ctype)) {
            // look up the name of the first field (primary key)
            tempstr attrname(name_Get(*c_field_Find(*ctype,0)));
            // Load ssimfile!
            tempstr fname = SsimFname("data",ctype->c_ssimfile->ssimfile);
            cstring lines(FileToString(fname,algo_FileFlags_throw));
            // Read all lines from ssimfile.
            Tuple tuple;
            ind_beg(Line_curs,line,lines) {
                Tuple_ReadStrptr(tuple, line, false);
                if (attrs_N(tuple) > 0 || ch_N(tuple.head.value)) {
                    // Look up the primary key in the tuple loaded from file
                    algo::Attr *attr = attr_Find(tuple,attrname);
                    tempstr this_val;
                    if (attr) {
                        this_val << attr->value;
                    }
                    bool match = false;
                    // Regx is a substring match.
                    match = Regx_Match(value_regx, this_val);
                    bool full_match       = this_val == value;
                    bool first_word_match = Pathcomp(this_val,".LL") == value;
                    bool last_word_match  = Pathcomp(this_val,".RR") == value;
                    bool first_match      = StartsWithQ(this_val, value);
                    bool last_match       = EndsWithQ(this_val, value);
                    if (match) {
                        acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
                        completion.value << compl_prefix << this_val;
                        completion.badness =
                            full_match ? acr_compl_Badness_full_match
                            : first_word_match ? acr_compl_Badness_first_word
                            : last_word_match ? acr_compl_Badness_last_word
                            : first_match ? acr_compl_Badness_first
                            : last_match ? acr_compl_Badness_last
                            : acr_compl_Badness_substring;
                        vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
                    }
                }
            }ind_end;
        }
    }
}

// -----------------------------------------------------------------------------

static char const *std_opt[] = {
    "verbose"
    ,"debug"
    ,"trace"
    ,"version"
    ,"help"
    ,"sig"
};

static bool StdOptQ(strptr opt) {
    frep_(i,_array_count(std_opt)) {
        if (opt == std_opt[i]) return true;
    }
    return false;
}


static char const *FieldToOptionArgtype (strptr arg) {
    // the code is stolen from amc, better to do it there
    char const *ret = NULL;
    if (arg == "bool") {
        ret = "flag";
    } else if (arg == "float" || arg == "double") {
        ret= "double";
    } else if (arg == "u8"
               || arg == "i8"
               || arg == "u16"
               || arg == "i16"
               || arg == "u32"
               || arg == "i32"
               || arg == "u64"
               || arg == "i64") {
        ret= "int";
    } else {
        ret = "string";
    }
    return ret;
}

static bool UniqueCompletionQ(int badness_limit=INT_MAX) {
    int badness1 = 0;
    int badness2 = 0;
    int iter = 0;
    ind_beg(acr_compl::_db_bh_completion_curs, completion, acr_compl::_db) {
        if (iter == 0) {
            badness1 = completion.badness;
        } else {
            badness2 = completion.badness;
        }
        iter++;
    }ind_end;
    return badness1 < badness2 && badness1 < badness_limit;
}

static void Main_Line_Acr(acr_compl::FField *, strptr value, cstring &compl_prefix) {
    tempstr ssimfile_key(Pathcomp(value,":LL"));
    tempstr ssimfile_value(Pathcomp(value,":LR"));
    acr_compl::FSsimfile *ssimfile = acr_compl::ind_ssimfile_Find(ssimfile_key);
    if (!ssimfile) {
        Main_Line_Ctype(acr_compl::ind_ctype_Find("dmmeta.Ssimfile"),ssimfile_key,compl_prefix);
        bool unique = UniqueCompletionQ();
        acr_compl::FCompletion *first_compl = acr_compl::bh_completion_First();
        // bash treats : as a word separator.
        // this means that having : in a word prevents completion of anything to the left.
        // however, acr recognizes bare name for ssimfile (e.g. ctype for dmmeta.ctype)
        // if we find one, we assume the ssimfile is found but don't modify the arg.
        // if the colon is missing, we are OK to complete.
        bool ssimfile_ok = first_compl &&
            (first_compl->badness == acr_compl_Badness_full_match || first_compl->badness == acr_compl_Badness_last_word);
        if (unique && first_compl && ssimfile_key != value && ssimfile_ok) {
            ssimfile = acr_compl::ind_ssimfile_Find(first_compl->value);
            acr_compl::completion_RemoveAll();
        }
        ind_beg(acr_compl::_db_completion_curs, completion, acr_compl::_db) {
            completion.nospace = true;
            completion.value << ':';
        }ind_end;
    }

    if (ssimfile) {
        Main_Line_Ctype(ssimfile->p_ctype,ssimfile_value,compl_prefix);
    }
}

// perform completion

static void Main_Line() {
    try {
        vrfy(i32_ReadStrptrMaybe(acr_compl::_db.point,acr_compl::_db.cmdline.point), algo_lib::_db.errtext);
    } catch (algo_lib::ErrorX &) {
        return;
    }
    if (acr_compl::_db.point > ch_N(acr_compl::_db.cmdline.line)) {
        return;
    }

    // process cmdline
    acr_compl::Shellqtype qtype;
    i32                   qpoint = 0;
    acr_compl::_db.line = ch_FirstN(acr_compl::_db.cmdline.line,acr_compl::_db.point);
    // split to argv
    Main_SplitLineToArgv();

    // do not complete after one of the special characters is encountered.
    bool has_specials = false;
    ind_beg(acr_compl::_db_word_curs,word,acr_compl::_db) {
        has_specials |= word == "|";
        has_specials |= word == "&";
        has_specials |= word == ">";
        has_specials |= word == ">>";
        has_specials |= word == ">&";
        if (has_specials) {
            break;
        }
    }ind_end;
    if (has_specials) {
        return;
    }

    ind_beg(acr_compl::_db_word_curs,word,acr_compl::_db) {
        word = Main_ShellExpand(word, &qtype, &qpoint);
    }ind_end;
    // derive needed words
    strptr arg0     = acr_compl::word_N() > 0 ? acr_compl::word_qFind(0)                     : strptr();
    strptr word     = acr_compl::word_N() > 1 ? acr_compl::word_qFind(acr_compl::word_N()-1) : strptr();
    strptr precword = acr_compl::word_N() > 2 ? acr_compl::word_qFind(acr_compl::word_N()-2) : strptr();
    strptr cmd      = StripDirName(arg0);

    // USE CASES:
    //
    // cmd <TAB>            -- list options
    // cmd -<TAB>           -- list options
    // cmd -op<TAB>         -- complete option name
    // cmd -opt <TAB>       -- list values
    // cmd -opt:<TAB>       -- list values
    // cmd -opt: <TAB>      -- list options
    // cmd -opt abc<TAB>    -- complete value
    // cmd -opt:abc<TAB>    -- complete value
    // cmd -opt abc <TAB>   -- list options
    // cmd -opt:abc <TAB>   -- list options
    // cmd val <TAB>

    // process words
    tempstr option;
    tempstr value;
    bool have_option        = false;
    bool have_colon         = false;
    bool word_is_option     = StartsWithQ(word,"-");
    bool precword_is_option = StartsWithQ(precword,"-");
    i32_Range r;
    if (elems_N(word)) {
        if (word_is_option) {
            // -abc[:def]<TAB>
            r = TFind(word,':');
            option = RestFrom(FirstN(word,r.beg),1);
            have_option = r.end>r.beg;
            have_colon  = have_option;
            if (have_option) {
                // -abc:[def]<TAB>
                value  = RestFrom(word,r.end);
            }
        } else if (precword_is_option) {
            // -abc[:def] ghi<TAB>
            r =  TFind(precword,':');
            have_option = r.end==r.beg;
            if (have_option) {
                // -abc ghi<TAB>
                option = RestFrom(precword,1);
                value  = word;
            }
        } else {
            // abc def<TAB>
            value = word;
        }
    } else {
        if (precword_is_option) {
            // -abc[:def] <TAB>
            r =  TFind(precword,':');
            have_option = r.end==r.beg;
            if (have_option) {
                // -abc <TAB>
                option = RestFrom(precword,1);
            }
        }
    }
    // prepare ctype and field if possible
    acr_compl::FCtype *cmd_ctype = acr_compl::ind_ctype_Find(tempstr()<<"command."<<cmd);
    acr_compl::FField *field = have_option ? acr_compl::ind_field_Find(tempstr()<<"command."<<cmd<<"."<<option) : NULL;
    // bool arg does not have space form, only colon
    if (have_option && !have_colon && field && field->arg == "bool") {
        field       = NULL;
        have_option = false;
        option      = strptr();
    }

    // standard options do not take argument
    if (have_option && StdOptQ(option)) {
        field       = NULL;
        have_option = false;
        option      = strptr();
    }

    // first anon arg
    if (!word_is_option && acr_compl::word_N() == 2) {
        field = cmd_ctype && c_field_N(*cmd_ctype) ? c_field_Find(*cmd_ctype,0) : NULL;
        if (field && field->c_anonfld) {
            option      = name_Get(*field);
            have_option = true;
        } else {
            field   = NULL;
        }
    }

    // match and complete option
    if (!have_option && !ch_N(value) && cmd_ctype) {
        ind_beg(acr_compl::ctype_c_field_curs,f,*cmd_ctype) if (StartsWithQ(name_Get(f),option)) {
            bool already_seen = false;
            ind_beg(acr_compl::_db_word_curs,w,acr_compl::_db) {
                already_seen = StartsWithQ(w,"-") && name_Get(f) == Pathcomp(ch_RestFrom(w,1),":LL") && name_Get(f) != option;
                already_seen |= !ind_curs(f).index && ind_curs(w).index==1 && f.c_anonfld && !StartsWithQ(w,"-");
                if (already_seen) {
                    break;
                }
            }ind_end;
            if (!already_seen) {
                field = &f;
                acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
                completion.value << "-" << name_Get(f);
                // expand with argument type, only if list is requested: <TAB><TAB>
                if (acr_compl::_db.cmdline.type == "63") {
                    completion.value << ":<" << FieldToOptionArgtype(f.arg) << '>';
                } else if (f.arg != "bool" || f.dflt.value == "true") {
                    completion.value << ":";
                    completion.nospace = true;
                } else {
                    field = NULL;
                }
                vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
            }
        }ind_end;
        frep_(i,_array_count(std_opt)) if (StartsWithQ(std_opt[i],option)) {
            bool already_seen = false;
            ind_beg(acr_compl::_db_word_curs,w,acr_compl::_db) {
                already_seen = StartsWithQ(w,"-") && std_opt[i] == ch_RestFrom(w,1) && std_opt[i] != option;
                if (already_seen) {
                    break;
                }
            }ind_end;
            if (!already_seen){
                acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
                completion.value << '-' << std_opt[i];
                vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
            }
        }
    }

    // Only a single option matches? Great, this becomes the prefix for all subsequent completions,
    // and we move on to value completion
    tempstr compl_prefix;
    if (acr_compl::completion_N()) {
        if (UniqueCompletionQ() && field) {
            acr_compl::completion_RemoveAll();
            compl_prefix << '-' << name_Get(*field) << ':';
        } else {
            field = NULL;
        }
    }

    // bool is special -- fixed values
    if (field && field->arg == "bool") {
        if (StartsWithQ("Y",value)) {
            acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
            completion.value << compl_prefix << 'Y';
            vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
        }
        if (StartsWithQ("N",value)) {
            acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
            completion.value << compl_prefix << 'N';
            vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
        }
    }

    if (field) {
        // command.acr.query is special
        if (field->field == "command.acr.query") {
            Main_Line_Acr(field, value, compl_prefix);
        } else if (field->p_arg->c_ssimfile && (field->reftype == dmmeta_Reftype_reftype_Pkey
                                                || field->reftype == dmmeta_Reftype_reftype_RegxSql) && c_field_N(*field->p_arg)) {
            // arg ctype is pkey of ssim file
            Main_Line_Ctype(field->p_arg,value,compl_prefix);
        }
    }

    // nothing matches
    if (field && !acr_compl::completion_N()) {
        if (ch_N(compl_prefix)) {
            // if there was prefix, return prefix
            acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
            completion.value << compl_prefix;
            completion.nospace = true;
            vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
        } else if (acr_compl::_db.cmdline.type == "63") {
            // if list is requested, return just value type
            acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
            completion.value << '<' << FieldToOptionArgtype(field->arg) << '>';
            vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
        }
    }

    // yield completion words
    // process only the leading badness class,
    // so if there are completions of badness 0 and 1, only 0 gets reported.
    int badness_limit = acr_compl::bh_completion_N() ? acr_compl::bh_completion_First()->badness+1 : INT_MAX;
    ind_beg(acr_compl::_db_bh_completion_curs,completion,acr_compl::_db) {
        if (completion.badness >= badness_limit) break;// early escape!
        tempstr out;
        if (acr_compl::_db.cmdline.type == "63") {
            // need to escape spaces if any
            frep_(i,ch_N(completion.value)) {
                if ((ch_qFind(completion.value, i) == ' ' || ch_qFind(completion.value, i) == '\t' || ch_qFind(completion.value, i) == '\n')) {
                    out << '\\';
                }
                out << ch_qFind(completion.value, i);
            }
        } else {
            // escape depending on active quote, from the position where the quote starts
            strptr_PrintEscBash(ch_RestFrom(completion.value,qpoint),out,type_GetEnum(qtype));
            if (!completion.nospace
                && type_GetEnum(qtype) != acr_compl_Shellqtype_squote
                && type_GetEnum(qtype) != acr_compl_Shellqtype_dquote) {
                // add space only if not quoted (bash will automatically close quote)
                out << ' ';
            }
        }
        prlog(out);
    }ind_end;
}

void acr_compl::Main() {
    // see how we are invoked
    strptr comp_line = getenv("COMP_LINE");
    if (elems_N(comp_line)) {
        // we're hooked into bash completion
        // this is to avoid syntax error when word or precword starting with dash
        // we no not need any command line parameters, all needed is here:
        acr_compl::_db.cmdline.line  = comp_line;
        acr_compl::_db.cmdline.point = getenv("COMP_POINT");
        acr_compl::_db.cmdline.type  = getenv("COMP_TYPE");
    } else {
        // normally
        Argtuple args;
        Argtuple_ReadArgv(args, algo_lib::_db.argc,algo_lib::_db.argv,acr_compl_syntax, acr_compl_help);
        vrfy(acr_compl_ReadTupleMaybe(acr_compl::_db.cmdline, args.tuple), algo_lib::_db.errtext);
        if (!ch_N(acr_compl::_db.cmdline.point)) {
            acr_compl::_db.cmdline.point << ch_N(acr_compl::_db.cmdline.line);
        }
    }
    vrfy(acr_compl::LoadTuplesMaybe("data"), algo_lib::_db.errtext);
    if (acr_compl::_db.cmdline.install) {
        Main_Install(algo_lib::_db.argv[0]);
    }
    if (ch_N(acr_compl::_db.cmdline.line)) {
        Main_Line();
    }
    if (!acr_compl::_db.cmdline.install && !ch_N(acr_compl::_db.cmdline.line)) {
        prerr("You seem to be calling acr_compl interactively.");
        prerr("Normally, acr_compl is implicitly by bash, with COMP_LINE or COMP_POINT environment variables set.");
        prerr("Suggestion: Use acr_compl -install to generate an installation script");
    }
}
