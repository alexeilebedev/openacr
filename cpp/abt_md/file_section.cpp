// Copyright (C) 2024,2026 AlgoRND
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
// Target: abt_md (exe) -- Tool to generate markdown documentation
// Exceptions: yes
// Source: cpp/abt_md/file_section.cpp -- Read and rewrite a file's marked sections
//

#include "include/algo.h"
#include "include/abt_md.h"

// -----------------------------------------------------------------------------

// - Check that section doesn't have nested code-blocks (~~~) and preformatted blocks (```).
// - Check that section doesn't have unbalanced code blocks and preformatted blocks
// - Check that section doesn't contain headers of higher significance than the title line
// (i.e. title line is ### but inside there is a ##)
void abt_md::CheckSection(abt_md::FFileSection &file_section) {
    int title_level=GetHeaderLevel(file_section.title);
    int backticks_start_line=0;
    int codeblock_start_line=0;
    ind_beg(algo::Line_curs,line,file_section.text) {
        int lineno = file_section.firstline + ind_curs(line).i;
        vrfy_(lineno>0);
        if (StartsWithQ(line,"```")) {
            backticks_start_line = backticks_start_line ? 0 : lineno;
        } else if (StartsWithQ(line,"~~~")) {
            codeblock_start_line = codeblock_start_line ? 0 : lineno;
        }
        if (codeblock_start_line && backticks_start_line) {
            prlog(_db.c_readmefile->gitfile<<":"<<lineno
                  <<": "<<"Interleaved code block & performatted block");
            prlog("    "<<line);
        }
        if (!backticks_start_line && !codeblock_start_line) {
            int level=GetHeaderLevel(line);
            if (level > 0 && level <= title_level) {
                prlog(_db.c_readmefile->gitfile<<":"<<lineno
                      <<": "<<"Bad title indentation "<<level<<"; header has "<<title_level);
                prlog("    "<<line);
                algo_lib::_db.exit_code++;
            }
        }
    }ind_end;
    if (backticks_start_line) {
        prlog(_db.c_readmefile->gitfile<<":"<<backticks_start_line<<": "<<"Unterminated preformatted block (```)");
        algo_lib::_db.exit_code++;
    }
    if (codeblock_start_line) {
        prlog(_db.c_readmefile->gitfile<<":"<<codeblock_start_line<<": "<<"Unterminated code block (~~~)");
        algo_lib::_db.exit_code++;
    }
}

// -----------------------------------------------------------------------------

void abt_md::RemoveHttpComments(abt_md::FFileSection &file_section) {
    tempstr text;
    ind_beg(Line_curs,line,file_section.text) {
        if (StartsWithQ(line,"<!-- dev.mdmark")){
            // old style, delete
        } else if (StartsWithQ(line,"<!-- abt_md.")){
            // new style, delete also
        } else {
            text<<line<<eol;
        }
    }ind_end;
    file_section.text=text;
}

// -----------------------------------------------------------------------------

// Scan section and save human-entered text into hash ind_human_text
// The sections where human text begins needs preserving are given by attribute dev.mdsection.genlist
// Any line that matches genlist header string is the beginning of new human text
// The key for the entry is given by Pathcomp(line,<key pathcomp>).
void abt_md::SaveHumanText(abt_md::FFileSection &file_section) {
    abt_md::FMdsection &mdsection = *file_section.p_mdsection;
    ind_human_text_Cascdel();
    if (mdsection.genlist != "") {
        abt_md::FHumanText *cur_human_text=&abt_md::ind_human_text_GetOrCreate("");// initial text
        ind_beg(Line_curs,line,file_section.text) {
            if (StartsWithQ(line,mdsection.genlist)) {
                tempstr name=LineKey(line);
                abt_md::FHumanText &ht=abt_md::ind_human_text_GetOrCreate(name);
                cur_human_text=&ht;
            } else {
                cur_human_text->text << line << eol;
            }
        }ind_end;
    }
}

// -----------------------------------------------------------------------------

// Scan section and restore any human text that has been saved before
// This allows abt_md to easily re-generate section content headings
// and re-apply human-written annotations from before.
void abt_md::RestoreHumanText(abt_md::FFileSection &file_section) {
    abt_md::FMdsection &mdsection = *file_section.p_mdsection;
    if (mdsection.genlist != "") {
        cstring newtext;
        if (abt_md::FHumanText *front=abt_md::ind_human_text_Find("")) {
            newtext << Trimmed(front->text) << eol;
        }
        ind_beg(Line_curs,line,file_section.text) {
            newtext << line << eol;
            if (StartsWithQ(line,mdsection.genlist)) {
                tempstr name=LineKey(line);
                if (abt_md::FHumanText *ht=abt_md::ind_human_text_Find(name)) {
                    newtext << Trimmed(ht->text) << eol << eol;
                    ht->used=true;
                }
            }
        }ind_end;
        file_section.text = newtext;
    }
}

// -----------------------------------------------------------------------------

// Insert anchors in FILE_SECTION, one for each section / subsection
// (The anchors are stripped when loading the section for updating)
// Delete any double blank links outside of code/preformatted blocks
// In the end, if the section body doesn't contain any non-empty lines except anchors,
// leave the section body empty (it will not be printed back to the file).
void abt_md::RewriteAnchors(abt_md::FFileSection &file_section) {
    cstring out;
    int nanchor=0;
    AddAnchor(LineKey(file_section.title),out);
    nanchor++;
    bool backticks=false;
    bool codeblock=false;
    int nlines=0;
    int n_conseq_empty=0;// run of empty lines
    ind_beg(algo::Line_curs,line,file_section.text) {
        int nbefore=out.ch_n;
        bool empty=Trimmed(line)=="";
        n_conseq_empty = !empty ? 0 : n_conseq_empty+1;// count empty lines
        out << line << eol;
        nlines += !empty;
        if (StartsWithQ(line,"```")) {
            backticks=!backticks;
        } else if (StartsWithQ(line,"~~~")) {
            codeblock=!codeblock;
        } else if (!backticks && !codeblock) {
            if (n_conseq_empty>1 || StartsWithQ(line, "<a href=\"#") || StartsWithQ(line,"<a name=\"")) {
                out.ch_n=nbefore;
                nlines -= !empty;
                // strip anchors -- we are regenerating them
            } else {
                int level=GetHeaderLevel(line);
                if (level>=1 && level<=4) {
                    AddAnchor(tempstr()<<LineKey(line), out);
                    nanchor++;
                }
            }
        }
    }ind_end;
    verblog("abt_md.rewrite_anchors"
            <<Keyval("title",file_section.title)
            <<Keyval("nlines",nlines)
            <<Keyval("nanchor",nanchor));
    file_section.text = nlines>0 ? out : cstring();
}

// -----------------------------------------------------------------------------

// Scan for links and anchors in section SECTION
void abt_md::ScanLinksAnchors() {
    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        bool backticks=false;
        bool codeblock=false;
        ind_beg(algo::Line_curs,line,file_section.text) {
            if (StartsWithQ(line,"```")) {
                backticks=!backticks;
            } else if (StartsWithQ(line,"~~~")) {
                codeblock=!codeblock;
            } else if (!backticks && !codeblock) {
                tempstr link_text;
                tempstr link_target;
                u8 state(0);
                for (int i=0; i<line.n_elems; i++) {
                    char ch=line.elems[i];
                    switch (state) {
                    case 0: // start
                        if (ch == '[') {
                            state=1; // open bracket
                        }
                        break;
                    case 1: // open bracket
                        if (ch != ']') {
                            link_text<<ch;
                        } else {
                            state = 2; // close bracket
                        }
                        break;
                    case 2: // close bracket
                        if (ch == '(') {
                            state = 3; // open parenthesis
                        } else {
                            // reset, ]( condition is not met
                            state = 0; // start
                            link_text="";
                        }
                        break;
                    case 3: // open parenthesis
                        if (ch != ')' && i<line.n_elems-1) {
                            link_target << ch;
                        } else {
                            // Take action
                            abt_md::FLink &link = link_Alloc();
                            link.location << _db.c_readmefile->gitfile<<":"<<(file_section.firstline + 1 + ind_curs(line).i);
                            link.text=link_text;
                            link.target=link_target;
                            link_text="";
                            link_target="";
                            state = 0; // start again
                        }
                        break;
                    }
                }
                if (StartsWithQ(line,"<a href=\"")) {
                    strptr name=Pathcomp(line,"\"LR#LR\"LL");
                    abt_md::FAnchor &anchor=anchor_Alloc();
                    anchor.anchor << _db.c_readmefile->gitfile<<"#"<<name;
                    if (!anchor_XrefMaybe(anchor)) {
                        anchor_RemoveLast();
                    }
                }
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Execute commands marked by "inline-command: ..." inside backtick blocks,
// and substitute their output into the section text
void abt_md::EvalInlineCommand(abt_md::FFileSection &file_section) {
    abt_md::FReadmefile &readmefile = *_db.c_readmefile;
    cstring out;
    bool inlinecommand=false;
    bool backticks=false;// inside backticks block
    bool codeblock=false;// inside code block
    bool leadempty=true;
    // expand inline-commands
    ind_beg(algo::Line_curs,line,file_section.text) {
        int lineno = file_section.firstline + ind_curs(line).i + 1/*skip title*/;
        if (line=="" && leadempty) {
            // ignore leading empty line
        } else if ((codeblock || backticks) && StartsWithQ(line,"inline-command: ")) {
            out << line << eol;
            verblog(readmefile.gitfile<<": eval "<<line);
            tempstr cmd(Pathcomp(line," LR"));
            int rc=0;
            algo::strptr tempfile("temp/abt_md.out");
            if (readmefile.sandbox) {
                command::wt_proc wt;
                wt.cmd.name.expr = dev_Sandbox_sandbox_abt_md;
                wt.cmd.q = true;
                cmd_Alloc(wt.cmd) << cmd;
                wt.fstdout << ">" << tempfile;
                wt.fstderr = ">&1";
                rc = wt_Exec(wt);
            } else {
                command::bash_proc bash;
                bash.cmd.c = cmd;
                bash.fstdout << ">" << tempfile;
                bash.fstderr = ">&1";
                rc = bash_Exec(bash);
            }
            if (rc) {
                // rc is the child's raw wait status, not an exit code: a
                // command that exited 1 yields 256, and one killed by a
                // signal yields the signal number.  Neither is a count of
                // errors, which is what every other diagnostic in abt_md adds
                // one to and what the process finally exits with, so the
                // failure is counted here rather than stored.
                prerr(readmefile.gitfile<<":"<<lineno<<": command failed ("<<algo::DescribeWaitStatus(rc)<<"): "<<cmd);
                algo_lib::_db.exit_code++;
            }
            // an empty filter means the output is taken as it comes, which is what cat
            // does.  It is read as a default here and not written onto the record: the
            // record is the file's own dev.readmefile row, and this tool saves that row
            // to carry the document's title, so a default assigned to it would land in
            // the ssimfile as though somebody had asked for it.
            algo::strptr filter(ch_N(readmefile.filter) > 0 ? algo::strptr(readmefile.filter) : algo::strptr("cat"));
            out << SysEval(tempstr()<<filter<<" <"<<tempfile,FailokQ(true),1024*1024*10);
            inlinecommand=true;
        } else if (StartsWithQ(line, "```")) {
            out << line << eol;
            inlinecommand=false;
            backticks = !backticks;
        } else if (StartsWithQ(line, "~~~")) {
            out << line << eol;
            inlinecommand=false;
            codeblock = !codeblock;
        } else {
            // regular line : output unless it's part of command line output
            // that's being replaced
            if (!inlinecommand) {
                out << line << eol;
            }
        }
        if (line != "") {
            leadempty =false;
        }
    }ind_end;
    file_section.text=out;
}

// Index of the first character of LINE at which the first shell command
// ends -- an unquoted separator (|, ;, >, &) or an unquoted word-initial
// # comment -- or -1 when the whole line is one command. A separator
// inside quotes is argument text, not command composition. Quote state
// follows bash lexing: outside quotes a backslash escapes the next
// character (\| is argument text, \" does not open a region), inside
// double quotes it escapes the next character (\" does not close the
// region), and inside single quotes it is literal -- a scanner without
// escapes would desynchronize from bash at the first \" and misclassify
// every separator after it. Bash strips a comment before command
// composition, so everything from an unquoted word-initial # on is prose:
// scanning it would let an apostrophe (don't) open a quote region that
// hides every later separator and ships the comment's words as command
// text. Mid-word, # is argument text (ctype:a#b), not a comment -- and
// word-initial is a lexing fact, not a raw-byte fact: whitespace consumed
// by an escape (ctype:a\ #b) or inside quotes joins the word, so a # after
// it continues that word and does not start a comment.
static int FindCommandEnd(algo::strptr line) {
    int ret = -1;
    char quote = 0;
    bool escape = false;
    bool wordstart = true;// at line start and after unquoted, unescaped whitespace
    for (int i = 0; i < line.n_elems && ret == -1; i++) {
        char c = line.elems[i];
        bool boundary = false;
        if (escape) {
            escape = false;
        } else if (quote == '\'') {
            if (c == '\'') {
                quote = 0;
            }
        } else if (quote == '"') {
            if (c == '\\') {
                escape = true;
            } else if (c == '"') {
                quote = 0;
            }
        } else if (c == '\\') {
            escape = true;
        } else if (c == '\'' || c == '"') {
            quote = c;
        } else if (c == '|' || c == ';' || c == '>' || c == '&') {
            ret = i;
        } else if (c == '#' && wordstart) {
            ret = i;
        } else if (c == ' ' || c == '\t') {
            boundary = true;
        }
        wordstart = boundary;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Replace each <placeholder> in TEXT with a value, so validation reaches the
// whole command line.
// Documentation writes a value the reader has to supply as `<ns>`, `<regex>`,
// `<path>`, and a placeholder written where an option expects its value hides
// every flag behind it. `acr_ed -create -ctype <ns> -pooltype Lary` reported
// nothing, though `-pooltype` is not an acr_ed option; the same line with a
// plain value in place of the placeholder reports it.
// Two mechanisms put the words out of reach, and a placeholder is the one text
// that trips both. A shell lexer reads the `<` as an input redirect and takes
// the word after it as the redirect target, so the value the option was
// waiting for is consumed as a filename and the line reads as one still being
// typed -- validation of what follows stops there. The `>` that closes the
// placeholder is separately an end-of-command character, so FindCommandEnd
// would cut the request at it even if the lexer had carried on. Neither
// mechanism needs the other: `-ctype <ns` with no `>` is equally unchecked,
// and a redirect after a flag that takes no value (`acr -check <file
// -blahblah`) reports the bad flag normally.
// A quarter of the command lines the docs carry have a placeholder in them, so
// a quarter went unchecked past their first one.
// The insight is that a placeholder stands for a value, and this validation
// reads option names only -- it accepts `-mode:x` exactly as it accepts
// `-mode:run` -- so supplying any value in the placeholder's place cannot
// turn a correct command line into a reported one, and it puts the words
// behind the placeholder back in view.
// A placeholder is `<`, a run of characters that are not whitespace or
// brackets, then `>`. A genuine redirect (`cmd < file`, `cmd <file`) does not
// match that shape and keeps its meaning.
static tempstr SubstPlaceholder(algo::strptr text) {
    tempstr ret;
    int i = 0;
    while (i < text.n_elems) {
        int end = -1;
        if (text.elems[i] == '<') {
            for (int j = i + 1; j < text.n_elems && end == -1; j++) {
                char c = text.elems[j];
                if (c == '>') {
                    end = j;
                } else if (c == ' ' || c == '\t' || c == '<') {
                    end = -2;// not a placeholder; stop looking
                }
            }
        }
        if (end >= 0) {
            ret << "x";
            i = end + 1;
        } else {
            ret << text.elems[i];
            i++;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Queue TEXT for batch validation if it reads as a command line, recording
// LINENO so a later failure names the line the reader would have copied.
// TEXT reads as a command line when its first word names an executable
// namespace, which is how `bin/acr a:b` is accepted alongside `acr a:b`:
// only the basename is looked up. Text carrying a shell variable, a comment,
// a prompt marker or a line continuation is a fragment rather than a
// command, and a fragment cannot be validated as one.
static void QueueCommandLine(algo::strptr text, u32 lineno) {
    abt_md::FReadmefile &readmefile = *abt_md::_db.c_readmefile;
    char first = ch_N(text) > 0 ? text.elems[0] : 0;
    bool skip = first == '#' || first == '$' || first == '>'
        || first == ' ' || first == '\t'
        || algo::FindStr(text, "$") != -1
        || StartsWithQ(text, "inline-command:")
        || EndsWithQ(text, "\\");
    if (ch_N(text) > 0 && !skip) {
        // Placeholders are substituted before the command end is found, so a
        // `<regex>` is one value by then: the `>` that closes a placeholder is
        // not the redirect that ends a command line, and cutting there would
        // drop every word behind it from the request.
        tempstr subst_buf = SubstPlaceholder(text);
        algo::strptr subst = subst_buf;
        // truncate at pipe/redirect/semicolon/ampersand or at a
        // trailing # comment -- only check the first command. A
        // separator inside a quoted region is argument text
        // (acr 'ctype:a|b'), not command composition: truncating
        // there would strip the rest of the command from the check
        // request, shipping anything after the quote (a bad flag,
        // a typo) unvalidated.
        int end = FindCommandEnd(subst);
        if (end == -1) {
            end = ch_N(subst);
        }
        strptr check_line = Trimmed(FirstN(subst, end));
        strptr first_word = Pathcomp(check_line, " LL");
        strptr basename = Pathcomp(first_word, "/RR");
        abt_md::FNs *ns = abt_md::ind_ns_Find(basename);
        if (ns && ns->nstype == dmmeta_Nstype_nstype_exe) {
            abt_md::FCheckReq &req = abt_md::checkreq_Alloc();
            req.id      = ++abt_md::_db.next_checkid;
            req.gitfile = readmefile.gitfile;
            req.lineno  = lineno;
            req.line    = check_line;
            (void)abt_md::ind_checkreq_InsertMaybe(req);
        }
    }
}

// -----------------------------------------------------------------------------

// Queue TEXT for existence checking if it reads as an acr key, recording
// LINENO so a later failure names the line the reader would have looked up.
// An acr key is written `<ns>.<ssimfile>:<pkey>`, and only the qualified form
// is read as one. The bare leaf form is how an attribute appears inside a
// tuple, so `cascdel:Y` in prose says the field carries that attribute and
// names no record at all -- and `dmmeta.cascdel:Y` selects nothing, as does a
// homonym like `target:/sys/cmd@57`, which is a stream path and not a
// `dev.target`. Requiring the namespace tells the two apart with no
// ambiguity left to guess at: an attribute inside a tuple is never
// namespace-qualified, so a false report is not merely unlikely here, it
// cannot be constructed.
// A value carrying `%` is a query rather than a key -- it names a set, and a
// set that happens to be empty is not an error -- and one carrying `<` is a
// template the reader fills in. Neither is looked up.
//
// A table's own short name is read as one further form, because that is how a
// document names a table: `ssimfile:x2db.product` says which table, where the
// qualified `dmmeta.ssimfile:x2db.product` says which row of the catalog and
// reads sideways in a sentence.
//
// The short name is admitted only where it cannot be anything else, and the
// test for that is what the table is keyed by. A table whose primary key is a
// table name is a table about tables, so a span naming one is a reference to a
// table and the value either names a table or names none. Every other short
// name stays refused, and each refusal is a span some document already writes:
// `cascdel:Y` is an attribute inside a tuple, `cfmt:Argv` and `sandbox:Y` are
// values rather than keys, `field:dmmeta.Field.zs_fcb` is a key a sentence
// calls nonsense on purpose, `command::x2sup.trace` is C++, and
// `func:doc.NavText` is a location doc answers rather than a row of
// `dmmeta.func`. Requiring the namespace, as the qualified form does, tells
// none of those apart -- what does is that none of their leaves is a catalog.
static bool ShortkeyQ(algo::strptr ssimfile) {
    abt_md::FSsimfile *only = NULL;
    int nfound = 0;
    ind_beg(abt_md::_db_ssimfile_curs, cand, abt_md::_db) {
        if (Pathcomp(cand.ssimfile, ".RR") == ssimfile) {
            only = &cand;
            nfound++;
        }
    }ind_end;
    abt_md::FCtype *p_ctype = nfound == 1 ? only->p_ctype : NULL;
    abt_md::FField *p_field = p_ctype ? c_field_Find(*p_ctype, 0) : NULL;
    bool catalog = p_field
        && (only->ssimfile == "dmmeta.ssimfile" || p_field->arg == "dmmeta.Ssimfile");
    return catalog;
}

static void QueueAcrKey(algo::strptr text, u32 lineno) {
    abt_md::FReadmefile &readmefile = *abt_md::_db.c_readmefile;
    int colon = algo::FindStr(text, ":");
    strptr ssimfile = colon == -1 ? strptr() : algo::FirstN(text, colon);
    strptr value = colon == -1 ? strptr() : algo::RestFrom(text, colon + 1);
    bool named = colon > 0 && ch_N(value) > 0
        && (algo::FindStr(ssimfile, ".") != -1 || ShortkeyQ(ssimfile));
    bool keyshape = named
        && algo::FindStr(value, "%") == -1
        && algo::FindStr(value, "<") == -1;
    // The key is queued only when the tuple that would select it can be
    // composed. A span naming no ssimfile is prose, not a key; and one whose
    // pkey cannot be determined has no tuple to send, so queuing it would
    // report it missing on the strength of a request never made.
    if (keyshape && ch_N(abt_md::AcrKeyTuple(text)) > 0) {
        abt_md::FCheckKey &checkkey = abt_md::ind_checkkey_GetOrCreate(text);
        if (!ch_N(checkkey.gitfile)) {
            checkkey.gitfile = readmefile.gitfile;
            checkkey.lineno  = lineno;
        }
    }
}

// -----------------------------------------------------------------------------

// Queue each inline `...` span on LINE for whichever check it answers to.
// Prose puts both kinds of thing in single backticks: a command line, which is
// the same kind of candidate a fenced-block line is and is judged by the same
// rule, and an acr key, which appears nowhere else. A span is offered to each
// check and each decides for itself, so a span that is neither is queued by
// neither. Spans are taken in pairs, so a line mentioning two of them yields
// two candidates and the text between them is not scanned.
static void QueueInlineSpan(algo::strptr line, u32 lineno) {
    int open = -1;
    for (int i = 0; i < line.n_elems; i++) {
        if (line.elems[i] == '`') {
            if (open == -1) {
                open = i;
            } else {
                strptr span(line.elems + open + 1, i - open - 1);
                QueueCommandLine(Trimmed(span), lineno);
                QueueAcrKey(Trimmed(span), lineno);
                open = -1;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Queue every command line in the current readme's sections for batch
// validation by Main_RunBatchCheck.
// A command line reaches a reader two ways: as a line of a bash or unlabeled
// fenced block, and as an inline `...` span in prose. Both are collected --
// a wrong flag misleads the reader wherever it is printed -- and a labeled
// fenced block of some other language is not shell and is skipped whole.
void abt_md::CheckCommandLines() {
    ind_beg(abt_md::_db_file_section_curs,file_section,_db) {
        bool backticks = false;
        bool skip_block = false;
        ind_beg(algo::Line_curs, line, file_section.text) {
            int lineno = file_section.firstline + ind_curs(line).i + 1;
            if (StartsWithQ(line, "```")) {
                backticks = !backticks;
                if (backticks) {
                    strptr lang = Trimmed(RestFrom(line, 3));
                    skip_block = ch_N(lang) > 0
                        && lang != "bash"
                        && lang != "sh";
                }
            } else if (backticks) {
                if (!skip_block) {
                    QueueCommandLine(line, lineno);
                }
            } else {
                QueueInlineSpan(line, lineno);
            }
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Update/evaluate specified section
// The algorithm is as follows:
// - First, check if the generator needs to be run. This is true if the template's (mdsection's) regx
// matches the readme pathname. If so, run the generator.
// The generator typically replaces the entire section contents with something computed,
// but a special case is allowed where the generator produces a list of subsections (e.g. one entry
// per file in some directory, or one entry per row in some table), and these subsections
// have human-entered text. The human-entered text is saved into a side table and later restored.
// See functions SaveHumanText / RestoreHumanText
// - Then, evaluate inline-command inside the section by running the command (possibly in sandbox)
// - Then, scan the resulting section text again and generate anchors for the main
//   section and for any subsection. Save the anchor to a global anchor table.
//   Find any links ([link text](link target)) and save them to the global link table
// - Finally, check that the resulting section body doesn't have unbalanced backticks
// (it's possible since the inline-command output might contain backticks).
void abt_md::UpdateSection(abt_md::FFileSection &file_section) {
    // evaluate section content if it's not fully generated
    abt_md::FReadmefile &readmefile = *_db.c_readmefile;
    abt_md::FMdsection &mdsection = *file_section.p_mdsection;
    verblog("abt_md.update_section_begin"
            <<Keyval("mdsection",mdsection.mdsection)
            <<Keyval("title",file_section.title)
            <<Keyval("sortkey",file_section.sortkey)
            <<Keyval("length",ch_N(file_section.text)));
    RemoveHttpComments(file_section);
    if (GenSectionQ(mdsection,readmefile)) {
        // run generator function on section --
        // this may generate content and/or replace file section fields
        // is section has 'genlist' defined, scan for human-entered text and save it in a hash
        SaveHumanText(file_section);
        file_section.p_mdsection->step(file_section);
        // restore human-entered text after section has been re-generated
        RestoreHumanText(file_section);
    }
    if (_db.cmdline.evalcmd) {
        EvalInlineCommand(file_section);
    }
    RewriteAnchors(file_section);
    CheckSection(file_section);
    verblog("abt_md.update_section_end"
            <<Keyval("mdsection",mdsection.mdsection)
            <<Keyval("title",file_section.title)
            <<Keyval("length",ch_N(file_section.text)));
}

// -----------------------------------------------------------------------------

bool abt_md::TitleQ(abt_md::FFileSection &section) {
    return section.p_mdsection && section.p_mdsection->mdsection == abt_md_mdsection_Title.mdsection;
}

// -----------------------------------------------------------------------------

// Load specified readme into memory as FILE_SECTION records.
// The section table is global and is wiped on every readme.
// Only lines starting with ## or ### (outside code fences) are sections.
//
// The same pass detects git conflict markers (<<<<<<<, |||||||, =======,
// >>>>>>>) left baked in by a botched merge.  Malformed structure -- a
// separator or closer with no open <<<<<<<, a nested <<<<<<<, or an
// unterminated region -- is reported as <gitfile>:<lineno>: and fails
// -check.  A complete, well-ordered block is left alone, so
// acr_dm's README, which documents its own conflict output with one such
// block, still passes.  When a region straddles a real section header the
// section split can no longer be trusted, so the function returns false
// and the caller refuses to rewrite the file.
bool abt_md::LoadSections(abt_md::FReadmefile &readmefile) {
    algo_lib::Reset(_db.R);// clean up
    if (readmefile.p_ns) {
        Set(_db.R,"$ns",readmefile.p_ns->ns);
    }
    file_section_RemoveAll();
    abt_md::FFileSection *cur_section=NULL;
    bool backticks=false;// inside backticks block
    bool codeblock=false;// inside code block
    bool in_conflict=false;// between <<<<<<< and >>>>>>>
    bool region_has_header=false;// a real section header seen in the open region
    bool spans_header=false;// some region straddled a section header
    int open_lineno=0;// line of the unclosed <<<<<<<
    ind_beg(algo::FileLine_curs,line,readmefile.gitfile) {
        int lineno=ind_curs(line).i+1;
        bool is_begin = StartsWithQ(line,"<<<<<<< ") || line=="<<<<<<<";
        bool is_sep   = line=="=======" || StartsWithQ(line,"||||||| ") || line=="|||||||";
        bool is_end   = StartsWithQ(line,">>>>>>> ") || line==">>>>>>>";
        if ((is_begin && in_conflict) || ((is_sep || is_end) && !in_conflict)) {
            prerr(readmefile.gitfile<<":"<<lineno<<": "
                  <<Keyval("comment","orphaned git conflict marker baked into committed file; resolve by hand"));
            algo_lib::_db.exit_code++;
        }
        if (is_begin && !in_conflict) {
            in_conflict=true;
            region_has_header=false;
            open_lineno=lineno;
        } else if (is_end && in_conflict) {
            spans_header = spans_header || region_has_header;
            in_conflict=false;
        }
        bool skip=false;
        if (StartsWithQ(line,"```")) {
            backticks=!backticks;
        } else if (StartsWithQ(line,"~~~")) {
            codeblock=!codeblock;
        } else if (!backticks && !codeblock) {
            int level=GetHeaderLevel(line);
            if (level>=1 && level<=3) {
                if (in_conflict) {
                    region_has_header=true;
                }
                cur_section=&file_section_Alloc();
                cur_section->title=line;
                cur_section->p_mdsection = &abt_md_mdsection_Content;// generic content
                cur_section->firstline = lineno;
                ind_beg(_db_mdsection_curs,mdsection,_db) {
                    // don't match title unless it's the first ##
                    if (&mdsection == &abt_md_mdsection_Title && zd_file_section_N(abt_md_mdsection_Title)>0) {
                        continue;
                    }
                    if (Regx_Match(mdsection.regx_match,cur_section->title)) {
                        cur_section->p_mdsection=&mdsection;
                    }
                }ind_end;
                verblog(readmefile.gitfile<<":"<<cur_section->firstline
                        <<Keyval("section",line)
                        <<Keyval("mdsection",cur_section->p_mdsection->mdsection)
                        <<Keyval("gen",GenSectionQ(*cur_section->p_mdsection,readmefile))
                        );
                cur_section->sortkey=Sortkey(*cur_section->p_mdsection, file_section_N());
                cur_section->select=Regx_Match(_db.cmdline.section,line);
                vrfy_(file_section_XrefMaybe(*cur_section));// index by mdsection
                skip=true;
            }
        }
        if (!skip) {
            if (cur_section) {
                cur_section->text << line << eol;
            } else {
                verblog("# discard "<<line);
            }
        }
    }ind_end;
    if (in_conflict) {
        prerr(readmefile.gitfile<<":"<<open_lineno<<": "
              <<Keyval("comment","unterminated git conflict marker (no >>>>>>>); resolve by hand"));
        algo_lib::_db.exit_code++;
        spans_header=true;
    }
    return !spans_header;
}

// -----------------------------------------------------------------------------

// Print a single section to string
void abt_md::PrintSection(abt_md::FFileSection &file_section, cstring &out) {
    out << TrimmedRight(file_section.title) << eol;
    out << TrimmedRight(file_section.text) << eol;
}

// -----------------------------------------------------------------------------

// Print all sections to string OUT
// Sections with empty text are deleted, except for title
// (Note that contents of generated sections are not considered sections, so they may be empty
//   and will not be deleted)
// A blank line separates sections; the file ends with a single newline (no trailing blank line).
void abt_md::PrintSections(cstring &out) {
    algo::ListSep sep("\n");
    ind_beg(abt_md::_db_bh_file_section_curs,file_section,_db) {
        bool select = file_section.select || !_db.cmdline.dry_run;
        select = select && (file_section.p_mdsection == &abt_md_mdsection_Title || Trimmed(file_section.text) != "");
        if (select) {
            out << sep;
            PrintSection(file_section,out);
        }
    }ind_end;
}
