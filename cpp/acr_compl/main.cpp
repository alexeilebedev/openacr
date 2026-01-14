// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: acr_compl (exe) -- ACR shell auto-complete for all targets
// Exceptions: NO
// Source: cpp/acr_compl/main.cpp
//

#include "include/acr_compl.h"

// -----------------------------------------------------------------------------

// unlike algo::GetTokenStrptr() it does not skip separator
static strptr GetTokenStrptrNoSkip(algo::StringIter &iter, const strptr& separator) {
    int i = FindFrom(iter.expr, separator, iter.index);
    strptr result;
    if (i==-1) {
        result=iter.Rest();
        iter.index=elems_N(iter.expr);
    } else {
        result=qGetRegion(iter.expr, iter.index,i-iter.index);
        iter.index=i;
    }
    return result;
}

// -----------------------------------------------------------------------------

// adapted from cpp/amc/global.cpp
tempstr acr_compl::GetCmdArgType(acr_compl::FField& field) {
    tempstr ret("string");// default
    acr_compl::FArgvtype *argvtype = field.p_arg->c_argvtype;
    // amc rewrites all pkey fields so they become Vals
    // so this first line can never execute
    if (c_fconst_N(*GetEnumField(field))) {
        ret = "enum";
    } else if (field.c_fflag) {
        ret = "flag";
    } else if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
        ret = "pkey";
    } else if (field.reftype == dmmeta_Reftype_reftype_RegxSql) {
        ret = "regx";
    } else {
        if (!argvtype && c_field_N(*field.p_arg) == 1) {
            argvtype = c_field_Find(*field.p_arg,0)->p_ctype->c_argvtype;
        }
        if (argvtype) {
            ret=argvtype->argvtype;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// adapted from cpp/amc/global.cpp
bool acr_compl::CmdArgValueRequiredQ(acr_compl::FField &field) {
    return field.c_falias
        ? CmdArgValueRequiredQ(*field.c_falias->p_srcfield)
        : !(field.p_arg->ctype == "bool" || (field.c_fflag && field.c_fflag->emptyval != "\"\""));
}

// -----------------------------------------------------------------------------

// adapted from cpp/amc/global.cpp
bool acr_compl::CmdArgRequiredQ(acr_compl::FField &field) {
    return field.c_falias
        ? CmdArgValueRequiredQ(*field.c_falias->p_srcfield)
        : field.dflt.value=="" // no default provided...
        && field.reftype != dmmeta_Reftype_reftype_Tary // not an array
        && !c_fconst_N(*GetEnumField(field)) // not an enum (these are always initialized)
        && CmdArgValueRequiredQ(field); // does require an arg
}

// -----------------------------------------------------------------------------

// adapted from cpp/amc/global.cpp
acr_compl::FField *acr_compl::GetEnumField(acr_compl::FField &field) {
    return c_field_N(*field.p_arg)==1
        && c_fconst_N(*c_field_Find(*field.p_arg,0))
        ? c_field_Find(*field.p_arg,0)
        : &field;
}

// -----------------------------------------------------------------------------

acr_compl::FCompletion &acr_compl::AddFieldToCompletion(acr_compl::FField &field, acr_compl_BadnessEnum badness) {
    FCompletion &completion = completion_Alloc();
    completion.value << "-" << name_Get(field);
    if (field.arg == "bool" && field.dflt.value == "true") {
        completion.value << ":N";
    } else if (_db.cmdline.type == "63" && CmdArgValueRequiredQ(field)) {
        completion.value << ":<" << GetCmdArgType(field) << '>';
    } else if (_db.exact) {
        completion.value << ":";
        completion.nospace = true;
    }
    completion.field = &field;
    completion.badness.badness = badness;
    completion.badness.strkey = name_Get(field);
    vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
    return completion;
}

// -----------------------------------------------------------------------------

// mark as seen field and linked alias or base field
void acr_compl::MarkSeen(acr_compl::FField &field) {
    field.seen = true;
    if (field.c_falias) {
        field.c_falias->p_srcfield->seen = true;
    }
    ind_beg(field_c_falias_srcfield_curs,falias,field) {
        falias.p_field->seen = true;
    }ind_end;
}

// -----------------------------------------------------------------------------

// whether the field is eligible to be proposed, e.g. not already seen
bool acr_compl::EligibleQ(acr_compl::FField &field) {
    return !field.seen || CumulativeQ(field);
}

// -----------------------------------------------------------------------------

bool acr_compl::CumulativeQ(acr_compl::FField &field) {
    FField *srcfield = field.c_falias ? field.c_falias->p_srcfield : NULL;
    return (field.c_fflag && field.c_fflag->cumulative)
        || (srcfield && CumulativeQ(*srcfield));
}

// -----------------------------------------------------------------------------

// whether the completion is unique at lowest badness -
// get two first elements from the heap,
// and compare their badnesses
bool acr_compl::UniqueCompletionQ() {
    int badness1 = INT_MAX;
    int badness2 = INT_MAX;
    int iter = 0;
    ind_beg(_db_bh_completion_curs, completion, _db) {
        if (iter == 0) {
            badness1 = completion.badness;
        } else {
            badness2 = completion.badness;
            break;
        }
        iter++;
    }ind_end;
    prcat(debug,"iter="<<iter<<" badness1="<<badness1<<" badness2="<<badness2);
    return badness1 < badness2;
}

// -----------------------------------------------------------------------------

// Read source data for completions from file FNAME into global table _db.complsource
void acr_compl::LoadComplsource(strptr fname) {
    if (fname != "-") {
        // fd closed upon destruction
        algo_lib::FFildes in;
        in.fd = OpenRead(fname, algo::FileFlags());
        ind_beg(algo::FileLine_curs,line,in.fd) {
            Tuple_ReadStrptr(complsource_Alloc().tuple, line, false);
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Create completion entries for string VALUE, which is a piece of a key for
// type CTYPE
void acr_compl::Main_Line_Ctype(acr_compl::FCtype *ctype, strptr value, strptr compl_prefix, bool exact) {
    if (ctype) {
        // Read VALUE into a regex.
        algo_lib::Regx value_regx;
        Regx_ReadSql(value_regx, value, false);
        // Process only if ssimfile exists
        if (ctype->c_ssimfile && c_field_N(*ctype)) {
            // look up the name of the first field (primary key)
            tempstr attrname(name_Get(*c_field_Find(*ctype,0)));
            // if input is a directory, reload complsource list for each completion
            // if it's a file (or stdin), the complsource table is loaded once in Main
            // and stays unchanged
            if (_db.is_data_dir) {
                complsource_RemoveAll();
                LoadComplsource(SsimFname(_db.cmdline.data,ctype->c_ssimfile->ssimfile));
            }
            ind_beg(_db_complsource_curs,complsource,_db) {
                Tuple &tuple=complsource.tuple;
                if (tuple.head.value == ctype->c_ssimfile->ssimfile) {// check tuple type
                    // Look up the primary key in the tuple loaded from file
                    algo::Attr *attr = attr_Find(tuple,attrname);
                    tempstr this_val;
                    if (attr) {
                        this_val << attr->value;
                    }
                    bool match = false;
                    // Regx is a substring match.
                    match = Regx_Match(value_regx, this_val);
                    //
                    // Three classes of match:
                    // - first - prefix match - this is ideal match;
                    // - last word - prefix match of the last word;
                    // - substring - all others.
                    //
                    // Note that classes are exclusive and restrictive.
                    //
                    // First, prefix match is tried, and ONLY if no such match,
                    // prefix match of the last word is considered,
                    // if STILL no luck, substring match is considered.
                    //
                    // Last word match case is very special to ACR  query parameter,
                    // where user starts typing ssimfile name without namespace,
                    // thus not exact but prefix match.
                    // Note that this may cause unwanted behavior when applied
                    // to some other parameter.
                    //
                    // These classes are enough to get predictability and
                    // better user experinence.
                    // Their enhancement is not desirabe, and lead to very
                    // aggresive and problematic completion behavior.
                    //
                    // Problems with enhanced classes in previous implementation:
                    // - full exact match causes problem with empty pattern and/or value,
                    // too early completion, although there are other alternatives;
                    // - first word exact match - makes too early completion,
                    // although there are other alternatives;
                    // - postfix match - weird behavior, may cause erasing of already
                    // typed text (and useless, as no one types letters
                    // in reverse order);
                    // - exact last word match - weak use (whole word is required,
                    // in opposite to prefix match where user may just start typing
                    // the last word).
                    //
                    // But in some corner cases exact match is required instead of prefix,
                    // `exact' parameter indicates that.
                    bool first_match      = exact
                        ? this_val == value
                        : StartsWithQ(this_val, value);
                    bool last_word_match  = exact
                        ? Pathcomp(this_val,".RR") == value
                        : StartsWithQ(Pathcomp(this_val,".RR"),value);
                    if (match) {
                        FCompletion &completion = completion_Alloc();
                        completion.value << compl_prefix << this_val;
                        completion.badness.badness =
                            first_match ? acr_compl_Badness_first
                            : last_word_match ? acr_compl_Badness_last_word
                            : acr_compl_Badness_substring;
                        completion.badness.strkey = completion.value;
                        vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
                        prcat(debug,"acr_compl.ctype_completion"
                              << Keyval("value",completion.value)
                              << Keyval("badness",completion.badness));
                    }
                }
            }ind_end;
        }
    }
}

void acr_compl::Main_Line_Ctype(acr_compl::FCtype *ctype, strptr value, strptr compl_prefix) {
    Main_Line_Ctype(ctype,value,compl_prefix,false);
}

//------------------------------------------------------------------------------

// process ACR query attribute
void acr_compl::Main_Line_Acr(acr_compl::FField *, strptr value, cstring &compl_prefix) {
    // whether ssimfile key is a full key, that is no key completion is required
    bool have_key = FindChar(value,':')>=0;
    // get value parts
    tempstr ssimfile_key(Pathcomp(value,":LL"));
    tempstr ssimfile_value(Pathcomp(value,":LR"));
    // further steps are to guess ssim file
    FSsimfile *ssimfile(NULL);
    // first try exact match, only if full key has supplied
    if (have_key) {
        ssimfile = ind_ssimfile_Find(ssimfile_key);
    }
    // if no luck try to do some match from dmmeta.Ssimfile
    if (!ssimfile) {
        Main_Line_Ctype(ind_ctype_Find("dmmeta.Ssimfile"),ssimfile_key,compl_prefix,have_key);
        bool unique = UniqueCompletionQ();
        FCompletion *first_compl = bh_completion_First();
        // bash treats : as a word separator.
        // this means that having : in a word prevents completion of anything to the left.
        // however, acr recognizes bare name for ssimfile (e.g. ctype for dmmeta.ctype)
        // if we find one, we assume the ssimfile is found but don't modify the arg.
        // if the colon is missing, we are OK to complete.
        bool ssimfile_ok = first_compl &&
            (first_compl->badness == acr_compl_Badness_first
             || first_compl->badness == acr_compl_Badness_last_word);

        if (unique && first_compl && have_key && ssimfile_ok) {
            // we found good and unique ssimfile, and now move to value completion
            ssimfile = ind_ssimfile_Find(first_compl->value);
            // we do not need key completions anymore
            completion_RemoveAll();
        }

        // add colon to all found ssimfile keys,
        // note this heap is empty if we moved to value completion
        ind_beg(_db_completion_curs, completion, _db) {
            completion.nospace = true;
            completion.value << ':';
        }ind_end;
    }

    // if ssimfile key is OK, perform value completion
    if (ssimfile) {
        Main_Line_Ctype(ssimfile->p_ctype,ssimfile_value,compl_prefix);
    }
}

//------------------------------------------------------------------------------

// perform completion
void acr_compl::Main_Line() {
    try {
        vrfy(i32_ReadStrptrMaybe(_db.point,_db.cmdline.point), algo_lib::_db.errtext);
    } catch (algo_lib::ErrorX &) {
        prcat(debug,"Bad completion point - can't read");
        return;
    }
    if (_db.point > ch_N(_db.cmdline.line)) {
        prcat(debug,"Bad completion point - exceeds line length");
        return;
    }

    // process cmdline
    Shellqtype qtype;
    i32                   qpoint = 0;
    _db.line = ch_FirstN(_db.cmdline.line,_db.point);
    // split to argv
    if (!Main_SplitLineToArgv()) {
        prcat(debug,"Unfinished io redirect");
        return;
    };

    // perform shell expansion
    ind_beg(_db_word_curs,word,_db) {
        word = Main_ShellExpand(word, &qtype, &qpoint);
    }ind_end;

    // can't do anything without command
    if (word_EmptyQ()) {
        prcat(debug,"Empty line"); // should never happen (command line test only)
        return;
    }
    strptr cmd = StripDirName(word_qFind(0));
    FNs *ns = ind_ns_Find(cmd);
    FFcmdline *fcmdline = ns ? ns->c_fcmdline : NULL;
    FCtype *ctype = fcmdline ? fcmdline->p_field->p_arg : NULL;
    if (!ctype) {
        prcat(debug,"Unknown command"); // may happen if outdated completion exists on bash
        return;
    }

    // prepare zd_cmd_field and ind_cmd_field_name
    ind_beg(ctype_c_field_curs,field,*ctype) {
        zd_cmd_field_Insert(field);
        ind_cmd_field_name_InsertMaybe(field);
        //prcat(debug,field);
    }ind_end;
    ind_beg(ctype_c_field_curs,field,*fcmdline->p_basecmdline->p_arg) {
        zd_cmd_field_Insert(field);
        ind_cmd_field_name_InsertMaybe(field);
        //prcat(debug,field);
    }ind_end;

    // parse command line
    // 1) <TAB>              :  name:""        value:""   exact:N  anon:N
    // 2) val<TAB>           :  name:""        value:val  exact:N  anon:N
    // 3) -<TAB>             :  name:""        value:""   exact:N  anon:N
    // 4) -opt<TAB>          :  name:opt       value:""   exact:N  anon:N
    // 5) -opt:<TAB>         :  name:opt       value:""   exact:Y  anon:N
    // 6) -opt:val<TAB>      :  name:opt       value:val  exact:Y  anon:N
    // 7) -with_arg val<TAB> :  name:with_arg  value:val  exact:Y  anon:N
    // 8) --<TAB>            :  -
    // 9) -- ... <TAB>       :  name:""        value:""   exact:Y  anon:Y
    // 10) --  ... -val<TAB>  :  name:""       value:-val exact:Y  anon:Y

    ind_beg(_db_word_curs,word,_db) if (ind_curs(word).index) { // start from 1
        bool is_last = &word == word_Last();
        bool is_second_to_last = &word == word_Find(word_N()-2);
        if (_db.need_value) { // awaiting value, got it
            _db.need_value = false;
            _db.value = word;
            _db.exact = true;
        } else if (_db.anon) {
            _db.name       = strptr();
            _db.value      = word;
            _db.anon_index += !is_last;
            _db.exact      = true;
        } else if (word == "--") { // end of named options
            _db.anon = true;
            if (is_last) { // do not complete last
                prcat(debug,"Double dash is last");
                return;
            }
        } else {
            algo::StringIter it(word);
            if (SkipChar(it,'-')) { // this is an option
                _db.name  = GetTokenStrptrNoSkip(it,":"); // scan to colon or eol
                _db.exact = SkipChar(it,':'); // skip colon
                _db.value = it.Rest(); // value is the rest if any
                if (!is_last) {
                    tempstr field_key = dmmeta::Field_Concat_ctype_name(ctype->ctype,_db.name);
                    FField *field = ind_cmd_field_name_Find(_db.name);
                    if (!field) {
                        prcat(debug,"Unknown option: "<<_db.name);
                        return;
                    }
                    _db.need_value = !_db.exact && CmdArgValueRequiredQ(*field); // check for extra arg, only if no colon
                    if (!_db.need_value || !is_second_to_last) {
                        MarkSeen(*field);
                    }
                }
            } else { // empty string or anon arg
                _db.name       = strptr();
                _db.value      = it.Rest();
                _db.anon_index += !is_last;
                _db.exact      = false;
            }
        }
        prcat(debug,"atf_compl.parse_word"
              << Keyval("word",word)
              << Keyval("name",_db.name)
              << Keyval("value",_db.value)
              << Keyval("need_value",_db.need_value)
              << Keyval("anon_index",_db.anon_index)
              << Keyval("exact",_db.exact)
              << Keyval("anon",_db.anon));
    }ind_end;

    // mark seen anons below anon_index
    {
        int cnt(_db.anon_index);
        ind_beg(_db_zd_cmd_field_curs,field,_db) if (field.c_anonfld) {
            if (cnt--) {
                field.seen = true;
            } else {
                break;
            }
        }ind_end;
    }

    // add first mandatory (regardless of anon)
    if (!ch_N(_db.name) && !_db.anon) {
        ind_beg(_db_zd_cmd_field_curs,field,_db) {
            if (EligibleQ(field) && CmdArgRequiredQ(field)) {
                prcat(debug,"atf_compl.first_unseen_mandatory"
                      << Keyval("name",name_Get(field)));
                AddFieldToCompletion(field,acr_compl_Badness_first);
                break;
            }
        }ind_end;
    }

    // anon arg:
    // after -- : add by anon_index
    // without --: add first unseen
    if (!ch_N(_db.name) && !completion_N() ) {
        int cnt = 0;
        ind_beg(_db_zd_cmd_field_curs,field,_db) if (field.c_anonfld) {
            bool ok = _db.anon
                ? (field.reftype == dmmeta_Reftype_reftype_Tary && cnt <= _db.anon_index)
                || cnt == _db.anon_index
                : field.reftype != dmmeta_Reftype_reftype_Tary && EligibleQ(field);
            prcat(debug,"atf_compl.anon"
                  <<Keyval("name",name_Get(field))
                  <<Keyval("cnt",cnt)
                  <<Keyval("ok",ok));
            if (ok) {
                AddFieldToCompletion(field,acr_compl_Badness_first);
                break;
            }
            ++cnt;
        }ind_end;
    }

    // add all unseen options
    if (!completion_N() && !_db.anon) {
        ind_beg(_db_zd_cmd_field_curs,field,_db) {
            strptr fname = name_Get(field);
            if (EligibleQ(field) && (!ch_N(_db.name) || FindStr(fname,_db.name) >= 0)) {
                bool first = _db.exact ? fname == _db.name : StartsWithQ(fname,_db.name);
                acr_compl_BadnessEnum badness = first ? acr_compl_Badness_first : acr_compl_Badness_substring;
                acr_compl::FCompletion &completion = AddFieldToCompletion(field,badness);
                prcat(debug,"atf_compl.unseen_option"
                      << Keyval("first",first)
                      << Keyval("badness",completion.badness));
            }
        }ind_end;
    }

    // Only a single option matches? Great, this becomes the prefix for all subsequent completions,
    // and we move on to value completion
    tempstr compl_prefix;
    FField *cur_field = NULL;
    if (completion_N() && UniqueCompletionQ()) {
        prcat(debug,"acr_compl.unique_completion"
              <<Keyval("completion",bh_completion_First()->value)
              <<Keyval("nospace",bh_completion_First()->nospace));
        cur_field = bh_completion_First()->field;
        if (_db.exact || ch_N(_db.value)) {
            completion_RemoveAll();
        } else if (cur_field->arg == "bool") {
            cur_field = NULL;
        } else {
            completion_RemoveAll();
            compl_prefix << '-' << name_Get(*cur_field) << ':';
            prcat(debug,"acr_compl.value_completion"
                  <<Keyval("prefix",compl_prefix));
        }
    }

    // we have field type, try to get possible values
    if (cur_field) {
        // bool is special -- fixed values
        if (cur_field->arg == "bool") {
            if (algo::StartsWithQ("Y",_db.value)) {
                acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
                completion.value << compl_prefix << 'Y';
                vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
            }
            if (algo::StartsWithQ("N",_db.value)) {
                acr_compl::FCompletion &completion = acr_compl::completion_Alloc();
                completion.value << compl_prefix << 'N';
                vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
            }
        }
        // fconst
        if (acr_compl::FField *enum_field = GetEnumField(*cur_field)) {
            ind_beg(field_c_fconst_curs,fconst,*enum_field) {
                if (StartsWithQ(name_Get(fconst),_db.value)) {
                    FCompletion &completion = completion_Alloc();
                    completion.value << compl_prefix << name_Get(fconst);
                    vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
                }
            }ind_end;
        }

        // command.acr.query is special
        if (cur_field->field == "command.acr.query") {
            Main_Line_Acr(cur_field, _db.value, compl_prefix);
        } else if (cur_field->p_arg->c_ssimfile
                   && (cur_field->reftype == dmmeta_Reftype_reftype_Pkey
                       || cur_field->reftype == dmmeta_Reftype_reftype_RegxSql)
                   && c_field_N(*cur_field->p_arg)) {
            // arg ctype is pkey of ssim file
            Main_Line_Ctype(cur_field->p_arg,_db.value,compl_prefix);
        }
    }

    // nothing matches
    if (cur_field && !completion_N()) {
        if (ch_N(compl_prefix)) {
            // if there was prefix, return prefix
            FCompletion &completion = completion_Alloc();
            completion.value << compl_prefix;
            completion.nospace = true;
            vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
        } else if (_db.cmdline.type == "63") {
            // if list is requested, return just value type
            FCompletion &completion = completion_Alloc();
            completion.value << '<' ;
            if (_db.anon) {
                completion.value << name_Get(*cur_field) << ":";
            }
            completion.value << GetCmdArgType(*cur_field) << '>';
            vrfy(completion_XrefMaybe(completion), algo_lib::_db.errtext);
        }
    }

    // yield completion words
    // process only the leading badness class,
    // so if there are completions of badness 0 and 1, only 0 gets reported.
    int badness_limit = bh_completion_N() ? bh_completion_First()->badness+1 : INT_MAX;
    int n_type = 0;
    ind_beg(_db_bh_completion_curs,completion,_db) {
        if (completion.badness >= badness_limit) break;// early escape!
        tempstr out;
        n_type += StartsWithQ(completion.value,"<") && EndsWithQ(completion.value,">") ? 1 : 0;
        if (_db.cmdline.type == "63") {
            if (ch_N(completion.value)) {
                // need to escape spaces if any
                frep_(i,ch_N(completion.value)) {
                    if ((ch_qFind(completion.value, i) == ' '
                         || ch_qFind(completion.value, i) == '\t'
                         || ch_qFind(completion.value, i) == '\n')) {
                        out << '\\';
                    }
                    out << ch_qFind(completion.value, i);
                }
            } else {
                out << "<EMPTY>";
            }
        } else if (ch_N(completion.value)) {
            // escape depending on active quote, from the position where the quote starts
            strptr_PrintEscBash(ch_RestFrom(completion.value,qpoint),out,type_GetEnum(qtype));
            if (!completion.nospace
                && type_GetEnum(qtype) != acr_compl_Shellqtype_squote
                && type_GetEnum(qtype) != acr_compl_Shellqtype_dquote) {
                // add space only if not quoted (bash will automatically close quote)
                out << ' ';
            }
        }
        // Empty values in most cases are useless and thus ignored:
        // For named parameter and unique empty completion, bash on its own add space.
        // For named parameter and non-unique completion (including empty),
        // let the user hit space to choose empty value among others.
        // For unnamed parameter and non-unique completion, let the user add "" explicitly.
        // For unnamed parameter and unique completion, it would be better to add "",
        // but the user still can do it.
        // In most cases, unnamed parameters have regex or very specific structure,
        // so that they have to be specified by the user explicitly,
        // or they have reasonable default, so better to do not to specify them at all.
        prlog(out);
        prcat(debug,out);
    }ind_end;
    // Ugly hack: In some unpredictable situations bash readline treats list completion that yields
    // sole value as normal completion, i.e. completes instead of just display.
    // We add second completion for ambuiguity, so it will not have a chance to complete on its own.
    if (n_type == 1) {
        prlog("value type");
        prcat(debug,"value type");
    }
}

//------------------------------------------------------------------------------

// Generate a script to install completion handler
void acr_compl::Main_Install(strptr prog) {
    tempstr out;
    out<<"complete -o default -o nospace -C "<<prog;
    ind_beg(_db_ns_curs,ns,_db) {
        if (ns.nstype == dmmeta_Nstype_nstype_exe && ns.c_fcmdline) {
            out<<" "<<ns.ns;
        }
    }ind_end;
    prlog(out);
}

// -----------------------------------------------------------------------------

// main routine
void acr_compl::Main() {
    // debug log
    if (!ch_N(_db.cmdline.debug_log)) {
        _db.cmdline.debug_log = getenv("ACR_COMPL_DEBUG_LOG");
    }
    if (ch_N(_db.cmdline.debug_log)) {
        algo_lib::_db.cmdline.debug = true;
        dup2(OpenWrite(_db.cmdline.debug_log,algo_FileFlags_append).value,2);// redirect stderr
    }
    // see how we are invoked
    strptr comp_line = getenv("COMP_LINE");
    if (elems_N(comp_line)) {
        // we're hooked into bash completion
        // this is to avoid syntax error when word or precword starting with dash
        // we no not need any command line parameters, all needed is here:
        _db.cmdline.line  = comp_line;
        _db.cmdline.point = getenv("COMP_POINT");
        _db.cmdline.type  = getenv("COMP_TYPE");
    } else {
        ReadArgv();
        if (!ch_N(_db.cmdline.point)) {
            _db.cmdline.point << ch_N(_db.cmdline.line);
        }
    }
    prcat(debug,_db.cmdline);
    // load schema and/or data from stdin
    if (_db.cmdline.schema == "-" || _db.cmdline.data == "-") {
        // read from stdin
        ind_beg(algo::FileLine_curs,line,algo::Fildes(0)) {
            if (_db.cmdline.schema == "-") {
                algo_lib::InsertStrptrMaybe(line);
                acr_compl::InsertStrptrMaybe(line);
            }
            if (_db.cmdline.data == "-") {
                Tuple_ReadStrptr(complsource_Alloc().tuple, line, false);
            }
        }ind_end;
    }
    // load schema from file or dir
    if (_db.cmdline.schema != "-") {
        vrfy(LoadTuplesMaybe(_db.cmdline.schema,true), algo_lib::_db.errtext);
    }
    // load data from file or dir
    _db.is_data_dir = DirectoryQ(_db.cmdline.data);
    if (!_db.is_data_dir) {
        // load tuples once
        acr_compl::LoadComplsource(_db.cmdline.data);
        // due to the way acr_compl works,
        // the list of ssimfiles from schema dir must be present
        // in the completions list
        if (_db.cmdline.schema != _db.cmdline.data) {
            ind_beg(_db_ssimfile_curs,ssimfile,_db) {
                dmmeta::Ssimfile rec;
                ssimfile_CopyOut(ssimfile,rec);
                Tuple_ReadStrptrMaybe(complsource_Alloc().tuple,tempstr()<<rec);
            }ind_end;
        }
    }
    if (_db.cmdline.install) {
        Main_Install(algo_lib::_db.argv[0]);
    }
    if (ch_N(_db.cmdline.line)) {
        Main_Line();
    }
    if (!_db.cmdline.install && !ch_N(_db.cmdline.line)) {
        prerr("You seem to be calling acr_compl interactively.");
        prerr("Normally, acr_compl is invoked implicitly by bash,");
        prerr("with COMP_LINE or COMP_POINT environment variables set.");
        prerr("Suggestion: Use acr_compl -install to generate an installation script");
    }
}
