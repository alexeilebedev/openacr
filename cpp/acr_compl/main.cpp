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

// The directory a source-naming option resolves to.  A user completing a line may
// have typed the option already, as in `acr_ed -in:/other -ctype:<TAB>`, and the
// root to read is then /other rather than whatever the schema says.  With nothing
// typed the option's own default stands, and a string default is stored as the C++
// literal the generated ReadArgv initializes from, so the surrounding quotes of
// a quoted default comes off before the value is used as a directory name.
static tempstr RootOfField(acr_compl::FField &field) {
    tempstr ret;
    if (ch_N(field.typed_value)) {
        ret << field.typed_value;
    } else {
        strptr dflt(field.dflt.value);
        if (ch_N(dflt) >= 2 && dflt.elems[0] == '"' && dflt.elems[ch_N(dflt)-1] == '"') {
            dflt = strptr(dflt.elems + 1, ch_N(dflt) - 2);
        }
        ret << dflt;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Add SSIMFILE's rows under ROOT to the completion table, leaving whatever is
// already there.  A root that does not hold the file contributes nothing, which is
// the ordinary state of an inventory layer nobody attached.
static void LoadComplsourceLayer(strptr root, acr_compl::FSsimfile &ssimfile) {
    tempstr fname(SsimFname(root, ssimfile.ssimfile));
    if (FileQ(fname)) {
        acr_compl::LoadComplsource(fname);
    }
}

// -----------------------------------------------------------------------------

// Read SSIMFILE's rows from every root the command being completed declares,
// replacing what the completion table held before.
//
// A tool's rows do not all come from one directory.  A tool may read the source tree
// named by -in and then the inventory layer named by -invdir, and its 413 node keys
// live only in the second, so a completion that reads one root offers nothing for
// -node while offering ctype keys from data/ perfectly well.  A command states its
// sources as dmmeta.floadtuples rows, one per cmdline field naming a root, and the
// rows marked autoscan are the ones whose contents a user may name on a command
// line.  Reading all of them is what makes a moved table completable again.
static void ReloadComplsource(acr_compl::FSsimfile &ssimfile) {
    acr_compl::complsource_RemoveAll();
    LoadComplsourceLayer(acr_compl::_db.cmdline.data, ssimfile);
    if (acr_compl::_db.p_cmd_ctype) {
        ind_beg(acr_compl::ctype_c_floadtuples_curs,floadtuples,*acr_compl::_db.p_cmd_ctype) {
            if (floadtuples.autoscan && floadtuples.p_field) {
                tempstr root(RootOfField(*floadtuples.p_field));
                if (ch_N(root) && root != acr_compl::_db.cmdline.data) {
                    LoadComplsourceLayer(root, ssimfile);
                }
            }
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
                ReloadComplsource(*ctype->c_ssimfile);
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

// -----------------------------------------------------------------------------

// Insert-mode completion for composite-pkey flags annotated via
// dmmeta.finsertwhen.  Walks the pkey's dmmeta.substr decomposition: at the
// cursor position, count the separators already typed and offer values for
// the corresponding component from its own pkey-source ssimfile.
//
// Limitation: handles 2-component pkeys (fcurs <field>/<curstype>,
// dispatch_msg <dispatch>/<msgtype>, fstep <field>/<curstype>).  Pkeys with
// 3+ components fall through to the caller's default lookup.
//
// Returns true if at least one completion candidate was emitted.
bool acr_compl::Main_Line_Insert(acr_compl::FField *cur_field, strptr value, strptr compl_prefix) {
    bool emitted = false;
    acr_compl::FCtype *ctype = cur_field->p_arg;
    if (ctype && c_field_N(*ctype) > 0) {
        acr_compl::FField *pkfield = c_field_Find(*ctype, 0);
        // collect substrs decomposing this pkey
        acr_compl::FSubstr *left  = NULL;
        acr_compl::FSubstr *right = NULL;
        char sep = 0;
        ind_beg(_db_substr_curs, substr, _db) {
            algo::strptr expr(substr.expr.value);
            // direct single-step decompositions only: <sep><LR><LR>
            // (e.g. "/RL", "/RR").  Multi-step exprs ("/RL.RL") name derived
            // components, not positional pkey parts; skip them.  Multiple
            // substrs for the same side (e.g. dmmeta.Ssimfile has both
            // .ns and .ssimns via .LL) are aliases — first one wins.
            if (substr.srcfield == pkfield->field && ch_N(expr) == 3) {
                char this_sep = expr[0];
                char side     = expr[2];
                if (sep == 0) {
                    sep = this_sep;
                }
                if (this_sep == sep && side == 'L' && !left) {
                    left = &substr;
                } else if (this_sep == sep && side == 'R' && !right) {
                    right = &substr;
                }
            }
        }ind_end;
        if (left && right && sep != 0) {
            // pick component based on separator count in `value`
            int idx = algo::FindChar(value, sep);
            acr_compl::FSubstr *cur = idx == -1 ? left : right;
            tempstr val_so_far(idx == -1 ? value : ch_RestFrom(value, idx + 1));
            // commit-prefix: everything up to and including the last separator
            tempstr cprefix(compl_prefix);
            if (idx != -1) {
                cprefix << ch_FirstN(value, idx + 1);
            }
            // the substr's own field.arg names the component's ctype;
            // offer values from its ssimfile
            acr_compl::FField *compfield = ind_field_Find(cur->field);
            if (compfield && compfield->p_arg) {
                int n_before = completion_N();
                Main_Line_Ctype(compfield->p_arg, val_so_far, cprefix);
                // when completing the left half, append the separator and
                // suppress the trailing space so the user can keep typing
                // the right component
                if (cur == left) {
                    ind_beg(_db_completion_curs, c, _db) if (ind_curs(c).index >= n_before) {
                        c.value << sep;
                        c.nospace = true;
                    }ind_end;
                }
                emitted = completion_N() > n_before;
            }
        }
    }
    return emitted;
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

// Clear everything Main_Line derives from the command line it is given, so a
// second call starts from a clean parse.
//
// Batch validation runs Main_Line once per command line, hundreds of times in
// one process. A documentation line such as `gcache -- g++ -c x.cpp` sets the
// anon flag, which tells the word scanner that named options have ended and
// every remaining word is a positional value. Carried into the next call, that
// flag routes every word of every later line down the positional branch, which
// never reaches the unknown-option check, so an entire corpus reports valid
// after one line that carries --.
//
// The command's own option set is per-line for the same reason: it is built
// from the command named by the line's first word. Accumulated over a batch it
// becomes the union of every command seen so far, so `amc -write x` following
// an `acr -write` line finds acr's flag in the index and passes. The
// completion offers are per-line too, and the leading offer of a bare command
// name carries no option field at all -- read as the current line's offer, it
// yields a null field the value-completion step then dereferences.
//
// Every field cleared here is a function of the single command line being
// parsed, so none of it may outlive the call. The clearing runs at entry
// rather than at exit because Main_Line leaves early on a malformed line.
static void ClearLineState() {
    ind_beg(acr_compl::_db_field_curs,field,acr_compl::_db) {
        field.seen = false;
        ch_RemoveAll(field.typed_value);
    }ind_end;
    acr_compl::_db.p_cmd_ctype = NULL;
    ind_beg(acr_compl::_db_zd_cmd_field_curs,field,acr_compl::_db) {
        acr_compl::ind_cmd_field_name_Remove(field);
    }ind_end;
    acr_compl::zd_cmd_field_RemoveAll();
    acr_compl::completion_RemoveAll();
    acr_compl::_db.name       = algo::strptr();
    acr_compl::_db.value      = algo::strptr();
    acr_compl::_db.need_value = false;
    acr_compl::_db.anon_index = 0;
    acr_compl::_db.exact      = false;
    acr_compl::_db.anon       = false;
    acr_compl::word_RemoveAll();
    ch_RemoveAll(acr_compl::_db.parse_error);
    ch_RemoveAll(acr_compl::_db.compl_output);
}

//------------------------------------------------------------------------------

// perform completion
void acr_compl::Main_Line() {
    ClearLineState();
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
        _db.parse_error << "acr_compl.check"<<Keyval("error","unfinished io redirect");
        return;
    };

    // perform shell expansion
    ind_beg(_db_word_curs,word,_db) {
        word = Main_ShellExpand(word, &qtype, &qpoint);
    }ind_end;

    // can't do anything without command
    if (word_EmptyQ()) {
        _db.parse_error << "acr_compl.check"<<Keyval("error","empty line");
        return;
    }
    strptr cmd = StripDirName(word_qFind(0));
    FCmd *fcmd = ind_cmd_Find(cmd);
    FCtype *ctype = fcmd ? fcmd->p_ctype : NULL;
    if (!ctype) {
        _db.parse_error << "acr_compl.check"<<Keyval("error","unknown command")<<Keyval("value",cmd);
        return;
    }

    // the command whose line is being completed; ReloadComplsource reads the roots
    // this command declares, so a key that lives only in a later layer is offered
    _db.p_cmd_ctype = ctype;

    // prepare zd_cmd_field and ind_cmd_field_name
    ind_beg(ctype_c_field_curs,field,*ctype) {
        zd_cmd_field_Insert(field);
        ind_cmd_field_name_InsertMaybe(field);
        //prcat(debug,field);
    }ind_end;
    ind_beg(ctype_c_field_curs,field,*fcmd->p_basectype) {
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
                        _db.parse_error << "acr_compl.check"<<Keyval("error","unknown option")<<Keyval("value",_db.name)<<Keyval("command",cmd);
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
        // Remember the value of any option the user has finished typing, so that an
        // option naming a tuple source is read from where the line says rather than
        // from the schema default.  Both spellings land here: `-invdir:/other` sets
        // name and value in one word, and `-invdir /other` leaves name standing from
        // the previous word while this one supplies the value.
        if (!is_last && ch_N(_db.name)) {
            FField *named_field = ind_cmd_field_name_Find(_db.name);
            if (named_field) {
                named_field->typed_value = _db.value;
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
            // dmmeta.finsertwhen: switch this Pkey flag to insert-completion
            // (walk substrs, complete component-by-component) whenever its
            // companion `when` flag is also seen on the partial cmdline
            // dmmeta.finsertwhen: when the trigger flag is on the cmdline,
            // switch this Pkey flag from "lookup existing rows" to insert
            // mode (walk substr decomposition).  Always suppresses the
            // default lookup — single-component pkeys get free-text input.
            bool insert_ok = false;
            if (acr_compl::FFinsertwhen *iw = ind_finsertwhen_Find(cur_field->field)) {
                acr_compl::FField *when = ind_field_Find(iw->when);
                if (when && when->seen) {
                    Main_Line_Insert(cur_field, _db.value, compl_prefix);
                    insert_ok = true;
                }
            }
            // default: arg ctype is pkey of ssim file — offer existing rows
            if (!insert_ok) {
                Main_Line_Ctype(cur_field->p_arg,_db.value,compl_prefix);
            }
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
        _db.compl_output << out << "\n";
        prcat(debug,out);
    }ind_end;
    // Ugly hack: In some unpredictable situations bash readline treats list completion that yields
    // sole value as normal completion, i.e. completes instead of just display.
    // We add second completion for ambuiguity, so it will not have a chance to complete on its own.
    if (n_type == 1) {
        _db.compl_output << "value type\n";
        prcat(debug,"value type");
    }
    if (!_db.cmdline.check && !_db.cmdline.check_batch) {
        // print each completion on its own line, preserving trailing spaces
        ind_beg(algo::Line_curs, l, _db.compl_output) {
            prlog(l);
        }ind_end;
    }
}

//-----------------------------------------------------------------------------
void acr_compl::LoadCmds() {
    ind_beg(_db_ccmdline_curs,ccmdline,_db) if (ns_Get(*ccmdline.p_ctype)=="command") {
        acr_compl::FCmd &cmd = ind_cmd_GetOrCreate(name_Get(*ccmdline.p_ctype));
        cmd.p_ctype = ccmdline.p_ctype;
        cmd.p_basectype = ccmdline.p_basecmdline ? ccmdline.p_basecmdline->p_arg : NULL;
    }ind_end;
}
//------------------------------------------------------------------------------

// Generate a script to install completion handler
void acr_compl::Main_Install(strptr prog) {
    tempstr out;
    out<<"complete -o default -o nospace -C "<<prog;
    ind_beg(_db_cmd_curs,cmd,_db) {
        out<<" "<<cmd.cmd;
    }ind_end;
    prlog(out);
}

// -----------------------------------------------------------------------------

// Validate a single command line and return the error text (empty on success).
// Main_Line clears its own per-line state, so the function is safe to call
// repeatedly.
// Caller must pass an independent buffer — line_in must not alias _db.cmdline.line.
static algo::tempstr RunOneCheck(algo::strptr line_in) {
    tempstr line_copy;
    line_copy << line_in << " "; // trailing space so the last real word is fully validated
    ch_RemoveAll(acr_compl::_db.cmdline.line);
    acr_compl::_db.cmdline.line << line_copy;
    ch_RemoveAll(acr_compl::_db.cmdline.point);
    acr_compl::_db.cmdline.point << ch_N(acr_compl::_db.cmdline.line);
    acr_compl::Main_Line();
    return tempstr() << acr_compl::_db.parse_error;
}

// Check command line validity.
// Call Main_Line with -check mode; report any error from _db.parse_error.
void acr_compl::Main_Check() {
    tempstr err = RunOneCheck(_db.cmdline.line);
    if (ch_N(err)) {
        prerr(err);
        algo_lib::_db.exit_code = 1;
    }
}

// -----------------------------------------------------------------------------

// Batch mode: read acr_compl.checkreq rows from stdin, validate each line,
// emit acr_compl.checkerr rows for failures only. The id field is echoed
// from request to response so the caller can correlate errors with sources.
// A row this build cannot parse -- a type tag that is not acr_compl.checkreq,
// or an attr a newer producer added -- fails the run instead of disappearing:
// silence on stdout means "every request validated clean", so a dropped
// request would report a documentation corpus as checked when nothing in it
// was. The request carries no usable id at that point, so the report goes to
// stderr rather than to a checkerr row, and the rest of the batch is still
// validated. An empty line carries no request and is not an error.
void acr_compl::Main_CheckBatch() {
    ind_beg(algo::FileLine_curs, line, algo::Fildes(0)) {
        acr_compl::checkreq req;
        if (acr_compl::checkreq_ReadStrptrMaybe(req, line)) {
            tempstr err = RunOneCheck(req.line);
            if (ch_N(err)) {
                acr_compl::checkerr resp;
                resp.id  = req.id;
                resp.err = err;
                cstring buf;
                checkerr_Print(resp, buf);
                prlog(buf);
                algo_lib::_db.exit_code = 1;
            }
        } else if (ch_N(algo::Trimmed(line))) {
            tempstr badtag = algo_lib::DetachBadTags();
            tempstr report;
            report << "acr_compl.badreq" << Keyval("line",line);
            if (ch_N(badtag)) {
                report << badtag;
            } else {
                report << Keyval("comment","not an acr_compl.checkreq tuple");
            }
            prerr(report);
            algo_lib::_db.exit_code = 1;
        }
    }ind_end;
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
    // See how we are invoked.  A shell that exported COMP_LINE while working
    // on completions leaves it in the environment of every process it starts,
    // so `acr_compl -check_batch` run from such a shell arrives with both a
    // command line of its own and a completion line it has no business
    // serving.  Taking the completion entrypoint on the strength of the
    // environment alone dropped the command line unparsed: -check_batch never
    // became true, fd 0 was never read, completion offers went to stdout, and
    // the process exited 0 -- which its caller reads as every queued command
    // validated clean.
    //
    // The two entrypoints are told apart by the argument list.  bash runs a
    // `complete -C` handler with the name of the command being completed as
    // the first argument, followed by the words around the cursor, and those
    // are positional words the tool's own option parser has no field for; a
    // deliberate command line begins with an option instead.  So an argument
    // list that starts with an option is parsed as one, and COMP_LINE is
    // honored only for the handler shape, where nothing else says what to do.
    strptr comp_line = getenv("COMP_LINE");
    bool argv_option = algo_lib::_db.argc > 1 && algo::StartsWithQ(algo_lib::_db.argv[1],"-");
    if (elems_N(comp_line) && !argv_option) {
        // bash completion entrypoint: COMP_LINE/POINT/TYPE carry the line to
        // complete; ReadArgv is skipped because the handler's positional words
        // are not options of this tool.
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
    // -check_batch is the whole job of the process it runs in: it reads
    // request rows from stdin and writes one response row per failing request
    // to stdout. A second mode touching either descriptor does not add to the
    // batch, it breaks it, so each such combination is refused before either
    // descriptor is used.
    //
    // -schema:- and -data:- read stdin, and the loader that reads it first
    // drains it: the batch would then validate zero requests and its caller
    // would read a whole corpus as checked. -install writes the `complete`
    // command to stdout, ahead of the response rows and not a response row,
    // which a consumer parsing the stream as tuples has no way to tell.
    // -check is a second validation mode over the -line argument, and only one
    // mode runs, so the check it asks for would silently not happen.
    bool stdin_ssim = _db.cmdline.schema == "-" || _db.cmdline.data == "-";
    tempstr conflict;
    if (_db.cmdline.check_batch && stdin_ssim) {
        conflict << "-check_batch reads requests from stdin and cannot be combined with -schema:- or -data:-";
    } else if (_db.cmdline.check_batch && _db.cmdline.install) {
        conflict << "-check_batch writes responses to stdout and cannot be combined with -install";
    } else if (_db.cmdline.check_batch && _db.cmdline.check) {
        conflict << "-check_batch and -check are two validation modes and cannot be combined";
    }
    if (ch_N(conflict)) {
        prerr("acr_compl.badcmdline"
              <<Keyval("comment",conflict));
        algo_lib::_db.exit_code = 1;
    } else {
        // load schema and/or data from stdin
        if (stdin_ssim) {
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
        // Prepare cmd selector
        LoadCmds();
        if (_db.cmdline.install) {
            Main_Install(algo_lib::_db.argv[0]);
        }
        if (_db.cmdline.check_batch) {
            Main_CheckBatch();
        } else if (_db.cmdline.check) {
            Main_Check();
        } else if (ch_N(_db.cmdline.line)) {
            Main_Line();
        } else if (!_db.cmdline.install) {
            prerr("You seem to be calling acr_compl interactively.");
            prerr("Normally, acr_compl is invoked implicitly by bash,");
            prerr("with COMP_LINE or COMP_POINT environment variables set.");
            prerr("Suggestion: Use acr_compl -install to generate an installation script");
        }
    }
}
