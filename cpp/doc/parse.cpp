// Copyright (C) 2026 AlgoRND
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
// Target: doc (exe) -- Render a markdown document to the terminal
// Exceptions: yes
// Source: cpp/doc/parse.cpp
//
// Read markdown, at both of its levels, and hand the result to whichever device
// asked.
// A document is a sequence of blocks -- a heading, a paragraph, a fenced listing,
// a table -- and every one of them is recognized from the first character or two
// of a line.  A block's text is carried through untouched, and the second half of
// this file is what reads the markup inside it: the spans of bold, emphasis, code
// and links, each one a record carrying its own style.  Both levels are here
// because both are the syntax rather than the drawing, and the two devices differ
// only in what they make of a block and a span.
// A file that is not markdown is a listing, and becomes a single block of one.

#include "include/algo.h"
#include "include/doc.h"

// Return the number of leading spaces of LINE, counting a tab as four.
static int Indent(algo::strptr line) {
    int ret = 0;
    int i = 0;
    bool done = false;
    while (i < ch_N(line) && !done) {
        if (line.elems[i] == ' ') {
            ret += 1;
        } else if (line.elems[i] == '\t') {
            ret += 4;
        } else {
            done = true;
        }
        i += (done ? 0 : 1);
    }
    return ret;
}

// Return the heading level of LINE, or zero when LINE is not a heading.
// A heading is one to six hashes followed by a space, which is the only form
// the documents under txt/ use.
static int HeadingLevel(algo::strptr line) {
    int ret = 0;
    int i = 0;
    while (i < ch_N(line) && line.elems[i] == '#') {
        i++;
    }
    if (i >= 1 && i <= 6 && i < ch_N(line) && line.elems[i] == ' ') {
        ret = i;
    }
    return ret;
}

// Return the length of the fence marker opening or closing LINE, or zero when
// LINE is not a fence.  Both spellings abt_md recognizes count: three
// backticks for a listing, three tildes for a preformatted block.
static int FenceLen(algo::strptr line) {
    int ret = 0;
    algo::strptr trim = algo::TrimmedLeft(line);
    char ch = ch_N(trim) ? trim.elems[0] : ' ';
    if (ch == '`' || ch == '~') {
        int i = 0;
        while (i < ch_N(trim) && trim.elems[i] == ch) {
            i++;
        }
        if (i >= 3) {
            ret = i;
        }
    }
    return ret;
}

// Return true when LINE is a horizontal rule: three or more dashes, asterisks
// or underscores, alone on the line.
static bool RuleQ(algo::strptr line) {
    algo::strptr trim = algo::Trimmed(line);
    char ch = ch_N(trim) ? trim.elems[0] : ' ';
    bool ret = false;
    if (ch_N(trim) >= 3 && (ch == '-' || ch == '*' || ch == '_')) {
        int i = 0;
        while (i < ch_N(trim) && trim.elems[i] == ch) {
            i++;
        }
        ret = i == ch_N(trim);
    }
    return ret;
}

// Return true when LINE is a row of a pipe table.
static bool TableLineQ(algo::strptr line) {
    algo::strptr trim = algo::TrimmedLeft(line);
    return ch_N(trim) > 0 && trim.elems[0] == '|';
}

// Return true when LINE carries nothing a reader needs: an anchor abt_md
// planted for the web rendering, or an html comment marking a generated
// region.  Both are invisible on a web page and would be noise here.
static bool NoiseQ(algo::strptr line) {
    algo::strptr trim = algo::Trimmed(line);
    bool anchor = StartsWithQ(trim, "<a ") && EndsWithQ(trim, "</a>");
    bool comment = StartsWithQ(trim, "<!--");
    return anchor || comment;
}

// Return the anchor LINE plants, without its leading hash, or the empty string
// when LINE plants none.
//
// abt_md writes one of these under every heading it generates, as
// `<a href="#reading-a-document"></a>`, and the name inside is the exact target
// the table of contents links to.  Reading it is what lets a link be followed
// without reimplementing the rule abt_md used to turn a heading into a slug --
// a rule with enough punctuation cases in it that a second copy would drift.
static algo::strptr AnchorName(algo::strptr line) {
    algo::strptr trim = algo::Trimmed(line);
    int open = doc::FindClose(trim, 0, "\"#");
    int close = open + 2 < ch_N(trim) ? doc::FindClose(trim, open + 2, "\"") : 0;
    algo::strptr ret;
    if (NoiseQ(line) && open + 2 <= close && close < ch_N(trim)) {
        ret = algo::strptr(trim.elems + open + 2, close - open - 2);
    }
    return ret;
}

// Return the byte offset just past the bullet marker of LINE, or zero when
// LINE does not begin a bullet item.
static int BulletLen(algo::strptr line) {
    int ret = 0;
    int i = Indent(line);
    char ch = i < ch_N(line) ? line.elems[i] : ' ';
    bool marker = ch == '-' || ch == '*' || ch == '+';
    if (marker && i + 1 < ch_N(line) && line.elems[i + 1] == ' ') {
        ret = i + 2;
    }
    return ret;
}

// Return the byte offset just past the number marker of LINE, or zero when
// LINE does not begin a numbered item.
static int NumberLen(algo::strptr line) {
    int ret = 0;
    int i = Indent(line);
    int digit = i;
    while (digit < ch_N(line) && algo_lib::DigitCharQ(line.elems[digit])) {
        digit++;
    }
    bool dot = digit > i && digit < ch_N(line) && (line.elems[digit] == '.' || line.elems[digit] == ')');
    if (dot && digit + 1 < ch_N(line) && line.elems[digit + 1] == ' ') {
        ret = digit + 2;
    }
    return ret;
}

// Start a new block of kind BLOCKTYPE at the end of the document.
static doc::FBlock &NewBlock(doc_BlocktypeEnum blocktype) {
    doc::FBlock &block = doc::block_Alloc();
    block.blocktype = blocktype;
    return block;
}

// Return the language the file at PATH is written in, spelled the way a fence
// spells it, or the empty string when the extension names nothing this tool has
// a lexer for.
//
// The empty string is what a bare fence carries, so a file in a language the
// tool cannot read is coloured exactly as an unlabelled listing is: what cannot
// be prose is marked, and nothing is claimed about the rest.
algo::strptr doc::FileLang(algo::strptr path) {
    algo::strptr ext = Pathcomp(path, ".RR");
    algo::strptr ret = "";
    if (ext == "cpp" || ext == "cc" || ext == "cxx" || ext == "c" || ext == "h" || ext == "hpp" || ext == "inl") {
        ret = "c++";
    } else if (ext == "sh" || ext == "bash") {
        ret = "bash";
    }
    return ret;
}

// Return true when PATH names a markdown document, the one kind of file whose
// lines mean something other than themselves.
static bool MarkdownQ(algo::strptr path) {
    return Pathcomp(path, ".RR") == "md";
}

// Read the file at PATH into a single listing block, coloured as the language
// its extension names.
//
// The documents in this tree cite sources: a rule file names the module it
// governs, AGENTS.md points at the file a convention lives in, and a reader
// following such a link is asking to read that file.  Its lines are
// preformatted -- each one means itself, at the column it was written in.
// Handed to the markdown parser instead, a c++ file loses its line breaks to
// paragraph filling, the underscore in a name to emphasis, and an angle-
// bracketed include to an html tag, so what arrives on screen is a wall of
// prose that no longer says what the file says.
//
// A listing is the block that already survives that: the renderer pours one
// line for line, and the highlighter colours it by the language named here.
static void ParseListing(algo::strptr path) {
    doc::FBlock &block = NewBlock(doc_Blocktype_code);
    block.lang = doc::FileLang(path);
    block.text << algo::FileToString(path, algo::FileFlags());
}

// Read one line of a markdown document into the blocks.
//
// The parser's whole memory is three values on FDb: whether the block last added may
// be continued, which heading an anchor line would belong to, and the fence still
// open.  It lives there rather than in the caller so that the same walk reads a
// document off the disk and a document this tool built for itself.
//
// Neither block is remembered as an address.  A pointer held from one line to the
// next is the shape that goes stale, so the heading is named by its index and the
// block being continued is named by the fact that it is the last one added -- which
// it always is, since a block is continued only until something else starts.  The
// check at the foot says so rather than trusting it.
//
// Every block keeps its markdown inline markup -- the asterisks, the
// backticks, the links -- because the renderer is the part that decides how
// those look.  What does not survive is the markup that exists only for the
// web rendering: the anchors abt_md plants under each heading, and the html
// comments that bracket its generated regions.
//
// A fenced block is captured whole and unexamined.  That is what keeps a
// document that shows a markdown table inside a listing from having that
// table rendered as one.
static void ParseLine(algo::strptr line) {
    doc::FBlock *cur = doc::_db.parsecont ? doc::block_Last() : NULL;
    doc::FBlock *head = doc::block_Find(doc::_db.parsehead);
    int fence = doc::_db.parsefence;
    {
        int fencelen = FenceLen(line);
        int level = HeadingLevel(line);
        int bullet = BulletLen(line);
        int number = NumberLen(line);
        bool blank = ch_N(algo::Trimmed(line)) == 0;
        bool para = cur && (cur->blocktype == doc_Blocktype_para
                            || cur->blocktype == doc_Blocktype_bullet
                            || cur->blocktype == doc_Blocktype_number
                            || cur->blocktype == doc_Blocktype_quote);
        doc::FBlock *cont = para ? cur : NULL;
        bool incmd = fence > 0 && cur && StartsWithQ(algo::TrimmedLeft(line), "inline-command:");
        if (incmd) {
            cur->cmd << algo::Trimmed(doc::RestFrom(algo::TrimmedLeft(line), 15));
        } else if (fence > 0) {
            bool close = fencelen >= fence;
            fence = close ? 0 : fence;
            if (close) {
                cur = NULL;
            } else if (cur) {
                cur->text << line << eol;
            }
        } else if (fencelen > 0) {
            fence = fencelen;
            cur = &NewBlock(doc_Blocktype_code);
            cur->lang = algo::Trimmed(doc::RestFrom(algo::TrimmedLeft(line), fencelen));
        } else if (NoiseQ(line)) {
            // An anchor or a generated-region marker, neither of which a reader
            // sees.  The anchor is still worth keeping: it names the target the
            // table of contents links to, and it sits under the heading it
            // names, so it belongs to the block just built.
            if (ch_N(AnchorName(line)) > 0 && head) {
                head->anchor = AnchorName(line);
            }
        } else if (blank) {
            cur = NULL;
        } else if (Indent(line) >= 4 && !cont && bullet == 0 && number == 0) {
            // A run of lines indented four spaces is a listing, which is what markdown
            // says and what a reader of this tree writes: the tutorials put their shell
            // transcripts that way, and so do thirty-odd other documents.  Read as prose
            // the lines are joined into a paragraph and the box drawing of a MySQL result
            // is taken for a table, which is how a transcript arrives shredded.
            //
            // A block only opens where no paragraph is being continued, since an indented
            // line under one is that paragraph wrapped.  A bullet is asked about first
            // because a nested one is indented too, and it is a list rather than a
            // listing.
            if (!cur || cur->blocktype != doc_Blocktype_code) {
                cur = &NewBlock(doc_Blocktype_code);
            }
            cur->text << doc::RestFrom(line, 4) << eol;
        } else if (level > 0) {
            doc::FBlock &block = NewBlock(doc_Blocktype_heading);
            block.level = level;
            block.text << algo::Trimmed(doc::RestFrom(line, level + 1));
            doc::_db.parsehead = i32(doc::block_N()) - 1;
            cur = NULL;
        } else if (RuleQ(line)) {
            NewBlock(doc_Blocktype_rule);
            cur = NULL;
        } else if (TableLineQ(line)) {
            if (!cur || cur->blocktype != doc_Blocktype_table) {
                cur = &NewBlock(doc_Blocktype_table);
            }
            cur->text << algo::Trimmed(line) << eol;
        } else if (StartsWithQ(algo::TrimmedLeft(line), ">")) {
            if (!cur || cur->blocktype != doc_Blocktype_quote) {
                cur = &NewBlock(doc_Blocktype_quote);
            }
            cur->text << algo::Trimmed(doc::RestFrom(algo::TrimmedLeft(line), 1)) << " ";
        } else if (bullet > 0) {
            cur = &NewBlock(doc_Blocktype_bullet);
            cur->level = Indent(line) / 2;
            cur->text << doc::RestFrom(line, bullet) << " ";
        } else if (number > 0) {
            cur = &NewBlock(doc_Blocktype_number);
            cur->level = Indent(line) / 2;
            cur->text << doc::RestFrom(line, number) << " ";
        } else if (cont) {
            cont->text << algo::Trimmed(line) << " ";
        } else {
            cur = &NewBlock(doc_Blocktype_para);
            cur->text << algo::Trimmed(line) << " ";
        }
    }
    vrfy(!cur || cur == doc::block_Last(),
         "doc.parsecont  comment:'a block being continued is always the last one added'");
    doc::_db.parsecont = cur != NULL;
    doc::_db.parsefence = fence;
}

// Parse the markdown TEXT and fill _db.block with its blocks in document order.
// Any blocks left from a previous document are dropped first, so the pool always
// describes exactly one document.
//
// Every block keeps its markdown inline markup -- the asterisks, the backticks,
// the links -- because the renderer is the part that decides how those look.  What
// does not survive is the markup that exists only for the web rendering: the
// anchors abt_md plants under each heading, and the html comments that bracket its
// generated regions.
//
// A fenced block is captured whole and unexamined.  That is what keeps a document
// that shows a markdown table inside a listing from having that table rendered as
// one.
//
// The text need not have come from a file.  The page of keys is a markdown document
// this tool writes for itself, and it reaches both devices through here, which is
// what keeps the keys from being laid out twice by two different pieces of code.
void doc::ParseText(algo::strptr text) {
    doc::block_RemoveAll();
    doc::_db.parsecont = false;
    doc::_db.parsehead = -1;
    doc::_db.parsefence = 0;
    ind_beg(algo::Line_curs, line, text) {
        ParseLine(line);
    }ind_end;
}

// Return the title of the markdown document at PATH -- the text of its first heading --
// and the empty string when the file is not markdown or carries no heading.
//
// A listing that names documents by their paths names them by what the author had to
// call the file, which is not what the file is about.  `txt/openacr/build.md` is "Build
// and Test", and the second is what a reader is looking for.
//
// The title is an attribute of the document's own `dev.readmefile` row, put there by
// `abt_md`, so listing sixty files costs no opens at all.  A row without one is read
// from the file instead: a document added since the last `abt_md` run still has a title,
// and it is the same title the row will carry once that run happens.
tempstr doc::Doctitle(algo::strptr path) {
    doc::FReadmefile *readmefile = doc::ind_readmefile_Find(path);
    tempstr ret;
    if (readmefile) {
        ret << readmefile->comment;
    }
    if (ch_N(ret) == 0 && EndsWithQ(path, ".md") && algo::FileQ(path)) {
        ind_beg(algo::FileLine_curs, line, path) {
            int at = doc::SkipSpace(line, 0);
            int hash = at;
            while (hash < ch_N(line) && line.elems[hash] == '#') {
                hash++;
            }
            if (ch_N(ret) == 0 && hash > at) {
                ret << algo::Trimmed(doc::RestFrom(line, hash));
            }
        }ind_end;
    }
    return ret;
}

// Fill the blocks with one listing holding TEXT, coloured as the language LANG names.
//
// The lines of a function's source mean themselves at the column they were written in,
// exactly as a whole file's do, so they reach the page as the block a file becomes and
// the renderer pours them the same way.
void doc::ParseListingText(algo::strptr text, algo::strptr lang) {
    doc::block_RemoveAll();
    doc::FBlock &block = NewBlock(doc_Blocktype_code);
    block.lang = lang;
    block.text << text;
}

// Read the file at PATH and fill _db.block with its blocks in document order.
//
// The extension decides how the file is read.  Markdown is parsed into the
// blocks its markers name; anything else is a listing, whose every line means
// itself and is carried through unexamined.
void doc::ParseFile(algo::strptr path) {
    if (MarkdownQ(path)) {
        doc::ParseText(algo::FileToString(path, algo::FileFlags()));
    } else {
        doc::block_RemoveAll();
        ParseListing(path);
    }
}

// Return true when the character before IN[POS] cannot be part of an identifier.
//
// Markdown gives underscore and asterisk the same job, and this is the one place they
// differ: an underscore only opens emphasis at a word boundary.  The distinction is
// what keeps a repository full of names such as acr_ed and x2_gw readable -- treated as
// emphasis, the underscores vanish and the reader is handed a tool name that does not
// exist.
static bool WordEdgeQ(algo::strptr in, int pos) {
    char prev = pos > 0 ? in.elems[pos - 1] : ' ';
    return !(algo_lib::AlphaCharQ(prev) || algo_lib::DigitCharQ(prev) || prev == '_');
}

// Return the text the html entity starting at IN[POS] stands for, and set END to the
// byte offset just past it -- to POS when what is there is not an entity this knows.
//
// The generated tables of contents are written for a web page, so they arrive full of
// &nbsp; and &bull; and the odd numeric reference such as &#128196;.  Left alone those
// read as literal ampersand-soup, which is what makes a contents block the ugliest
// thing in an unrendered document.
static tempstr EntityText(algo::strptr in, int pos, int &end) {
    int stop = pos + 1;
    while (stop < ch_N(in) && stop < pos + 12 && in.elems[stop] != ';') {
        stop++;
    }
    tempstr ret;
    end = pos;
    if (stop < ch_N(in) && in.elems[stop] == ';') {
        algo::strptr name(in.elems + pos + 1, stop - pos - 1);
        u32 code = 0;
        bool numeric = ch_N(name) > 1 && name.elems[0] == '#';
        bool decoded = numeric && u32_ReadStrptrMaybe(code, algo::strptr(name.elems + 1, ch_N(name) - 1));
        if (name == "nbsp") {
            ret << " ";
        } else if (name == "amp") {
            ret << "&";
        } else if (name == "lt") {
            ret << "<";
        } else if (name == "gt") {
            ret << ">";
        } else if (name == "quot") {
            ret << "\"";
        } else if (name == "bull") {
            ret << "\u2022";
        } else if (name == "mdash") {
            ret << "\u2014";
        } else if (decoded && code > 0 && code < 0x110000) {
            doc::PrintUtf8(code, ret);
        }
        end = ch_N(ret) > 0 ? stop + 1 : pos;
    }
    return ret;
}

// Skip the html tag that starts at IN[POS] and return the byte offset just past it, or
// POS when what follows is not a tag.  Every tag these documents carry is decoration
// for the web rendering -- the anchors, the <br/> line breaks handled by the caller --
// so dropping them loses nothing.
static int TagEnd(algo::strptr in, int pos) {
    int end = pos + 1;
    while (end < ch_N(in) && in.elems[end] != '>' && in.elems[end] != '\n') {
        end++;
    }
    bool tagged = end < ch_N(in) && in.elems[end] == '>';
    return tagged ? end + 1 : pos;
}

// Return true when CH can open a markdown inline span.  Everything else is literal text
// and is copied a run at a time, which is also what keeps a multi-byte character
// together on its way to the decoder.
static bool SpecialQ(char ch) {
    return ch == '\\' || ch == '&' || ch == '<' || ch == '`' || ch == '*' || ch == '_' || ch == '[';
}

// Return how many links the spans read so far belong to.
//
// It is read off the spans rather than counted in a variable, so a link's number is a
// fact about what has been read and cannot disagree with it.  Markdown does not nest
// links, so the largest number seen is the number of links.
static int NSpanlink() {
    int ret = 0;
    ind_beg(doc::_db_span_curs, span, doc::_db) {
        ret = i32_Max(ret, span.ilink);
    }ind_end;
    return ret;
}

// Append TEXT to _db.span as a run carrying BOLD, ITALIC, CODE and the link TARGET,
// which is link ILINK of the string.  BREAK says the run is a line break rather than
// text.
//
// A run whose style matches the one before it extends that run instead of starting
// another, since the split between them says nothing about the text: the scanner reaches
// an escape or an entity one character at a time and would otherwise leave a span per
// character.
static void PutSpan(algo::strptr text, bool bold, bool italic, bool code, algo::strptr target, int ilink, bool brk) {
    doc::FSpan *last = doc::span_N() > 0 ? &doc::span_qFind(u64(doc::span_N() - 1)) : NULL;
    bool same = last && !brk && !last->br && last->bold == bold && last->italic == italic
        && last->code == code && last->ilink == ilink;
    doc::FSpan &span = same ? *last : doc::span_Alloc();
    span.bold = bold;
    span.italic = italic;
    span.code = code;
    span.br = brk;
    span.ilink = ilink;
    span.target = target;
    span.text << text;
}

// Read the markdown inline markup of IN into _db.span, under the style this level was
// entered with: BOLD, ITALIC, and the link TARGET, which is link ILINK of the string.
//
// The spans that matter here are the four the documents under txt/ actually use: a code
// span in backticks, bold in double asterisks, emphasis in single asterisks or
// underscores, and a link in brackets.  A span carries its style outright rather than
// opening and closing it, so a style inside a link is one span that is both, and nothing
// downstream works out what closing an inner span should leave behind.
//
// A link keeps its text and drops its target from the run, the way a man page prints the
// name of a cross-reference rather than a path.  The target rides along on the span, and
// what each device makes of it is its own business -- a record the reader can follow, or
// an anchor.
//
// A code span that names something this tool can open becomes a link to it, with no author
// having written a target: a table, a ctype, a query.  What names what is `Codeloc`, and a
// span already inside a link is left as it is, since the author's target is the one that
// was meant.
static void ReadSpan(algo::strptr in, bool bold, bool italic, algo::strptr target, int ilink) {
    int i = 0;
    while (i < ch_N(in)) {
        char ch = in.elems[i];
        char next = i + 1 < ch_N(in) ? in.elems[i + 1] : '\0';
        int entity = i;
        tempstr text;
        if (ch == '&') {
            text << EntityText(in, i, entity);
        }
        if (ch == '\\' && next != '\0') {
            PutSpan(algo::strptr(in.elems + i + 1, 1), bold, italic, false, target, ilink, false);
            i += 2;
        } else if (entity > i) {
            PutSpan(text, bold, italic, false, target, ilink, false);
            i = entity;
        } else if (ch == '<' && (StartsWithQ(doc::RestFrom(in, i), "<br>") || StartsWithQ(doc::RestFrom(in, i), "<br/>"))) {
            PutSpan("", bold, italic, false, target, ilink, true);
            i = TagEnd(in, i);
        } else if (ch == '<' && TagEnd(in, i) > i) {
            i = TagEnd(in, i);
        } else if (ch == '`') {
            int end = doc::FindClose(in, i + 1, "`");
            algo::strptr code(in.elems + i + 1, i32_Max(0, end - i - 1));
            tempstr named(ch_N(target) > 0 ? tempstr() : doc::Codeloc(code));
            bool link = ch_N(named) > 0;
            PutSpan(code, bold, italic, true, link ? algo::strptr(named) : target,
                    link ? NSpanlink() + 1 : ilink, false);
            i = end < ch_N(in) ? end + 1 : ch_N(in);
        } else if (ch == '*' && next == '*') {
            int end = doc::FindClose(in, i + 2, "**");
            ReadSpan(algo::strptr(in.elems + i + 2, end - i - 2), true, italic, target, ilink);
            i = end + 2;
        } else if (ch == '_' && WordEdgeQ(in, i) && next != ' ' && next != '\0') {
            int end = doc::FindClose(in, i + 1, "_");
            ReadSpan(algo::strptr(in.elems + i + 1, end - i - 1), bold, true, target, ilink);
            i = end + 1;
        } else if (ch == '*' && next != ' ' && next != '\0') {
            int end = doc::FindClose(in, i + 1, "*");
            ReadSpan(algo::strptr(in.elems + i + 1, end - i - 1), bold, true, target, ilink);
            i = end + 1;
        } else if (ch == '[') {
            int close = doc::FindClose(in, i + 1, "]");
            bool linked = close + 1 < ch_N(in) && in.elems[close + 1] == '(';
            int paren = linked ? doc::FindClose(in, close + 2, ")") : 0;
            if (linked) {
                ReadSpan(algo::strptr(in.elems + i + 1, close - i - 1), bold, italic,
                         algo::strptr(in.elems + close + 2, paren - close - 2), NSpanlink() + 1);
                i = paren + 1;
            } else {
                PutSpan(algo::strptr(in.elems + i, 1), bold, italic, false, target, ilink, false);
                i++;
            }
        } else {
            int end = i + 1;
            while (end < ch_N(in) && !SpecialQ(in.elems[end])) {
                end++;
            }
            PutSpan(algo::strptr(in.elems + i, end - i), bold, italic, false, target, ilink, false);
            i = end;
        }
    }
}

// Read the markdown inline markup of IN into _db.span, one span per run of text that
// carries one style.
//
// This is the whole of what the inline syntax is, and it is read once for both devices:
// the terminal makes cells of a span and the browser wraps it in tags, and neither knows
// anything about the syntax that produced it.
//
// The spans live on _db and every call replaces them, so a caller walks what it asked
// for before asking for the next string.
void doc::ReadInline(algo::strptr in) {
    doc::span_RemoveAll();
    ReadSpan(in, false, false, "", 0);
}
