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
// Source: cpp/doc/html.cpp -- The same locations, laid out for a browser
//
// The second device this tool draws on.
// A terminal is a grid of cells, and the renderer for it flattens a table into
// padded columns because that is the only table a terminal has.  A browser wants
// the structure back: a heading is an h2, a pipe table is a table, a link is an
// anchor.  So HTML is not made from the finished grid but from the same blocks the
// grid is made from, and the two emitters part company exactly where layout does.
// Everything upstream of layout is shared -- which document a key resolves to,
// what the parser found in it, what a page's records are about, where an attribute
// leads, and which bytes of a listing are a comment.
// Syntax colour is the clearest case of that sharing.  Highlight fills a run of
// cells each carrying a doc.Color, and this file groups adjacent cells of one
// colour into a span whose class names the colour.  One lexer, two devices, and a
// fenced block reads the same in both.
// What does not become a table is acr's answer.  Its columns are aligned by acr
// and a -t tree carries its depth in its indentation, so the invariant that
// nothing reformats that answer holds here exactly as it holds on the glass: the
// answer goes into a pre, and what the emitter adds is an anchor on each attribute.
// A URL is a location.  The path after the prefix is the location string itself,
// with one substitution -- a markdown document is spelled .html rather than .md --
// so a link is a roundtrip that names what it is asking for, and the browser's own
// history is the reader's Backspace.

#include "include/algo.h"
#include "include/doc.h"

// Return the URL prefix every generated link carries, without a trailing slash so
// that one join rule covers every location.
static tempstr UrlBase() {
    algo::strptr server = doc::_db.cmdline.server;
    bool slash = ch_N(server) > 0 && server.elems[ch_N(server) - 1] == '/';
    tempstr ret;
    ret << (slash ? algo::strptr(server.elems, ch_N(server) - 1) : server);
    return ret;
}

// Return the URL the prefix itself names, which is where a visitor who typed the
// host and nothing else belongs.
tempstr doc::UrlRoot() {
    tempstr ret;
    ret << UrlBase() << "/";
    return ret;
}

// Return the path a request for the URL prefix carries, which is what the server
// matches an arriving path against.
//
// A prefix names a host as well as a path -- http://localhost:8080/doc -- and only
// the path reaches the server, so the host has to come off.  A prefix that names
// no host is already a path.
tempstr doc::UrlPath() {
    tempstr base(UrlBase());
    int host = algo::FindStr(base, "://");
    int slash = host >= 0 ? algo::FindChar(doc::RestFrom(base, host + 3), '/') : -1;
    tempstr ret;
    if (host < 0) {
        ret << base;
    } else if (slash >= 0) {
        ret << doc::RestFrom(base, host + 3 + slash);
    }
    return ret;
}

// Return the flags now in force, spelled as a URL query string with its leading `?`, or
// nothing when the page is the one a bare location opens.
//
// A server keeps no reader between requests, so the flags cannot live on FDb the way
// they do on the terminal: what a link opens has to be written into the link.  They
// ride in the query string rather than in the path because they are not part of what
// is being asked for -- the same record under two display modes is one location seen
// two ways, and a URL says so by keeping the path and changing the question.
//
// A parameter spells the deviation from the default, and every section is closed by
// default, as it is on the terminal.  So a parameter names a section to open, and a page
// nobody has opened one on has a URL with no question mark in it.
//
// What a link carries is what stays in force as a reader walks: the access paths, the
// code and the data.  Each of those is a standing decision about how much of a record to
// be shown, so a reader who opened the code once means it for the next page too.
//
// Each is written only where the page draws that fold, and the test is the one the
// emitter branches on.  A fold whose link left its own option out redraws the page with
// the fold still shut, which is what a namespace page did.
//
// The tabs are the other kind, and this function writes neither -- the tab strip spells
// its own.  Meta shows the schema of a selection in place of its records and cmd shows
// the command line in place of either, so a link carrying one would answer a question
// nobody asked about the thing they clicked.
static tempstr UrlFlag(algo::strptr loc) {
    bool section = doc::SectionQ(loc);
    bool data = ch_N(doc::Datassimfile(loc)) > 0;
    tempstr ret;
    algo::ListSep ls("&");
    if (section && doc::_db.vis) {
        ret << ls << "vis";
    }
    if (section && doc::_db.code) {
        ret << ls << "code";
    }
    if (data && doc::_db.data) {
        ret << ls << "data";
    }
    tempstr out;
    if (ch_N(ret) > 0) {
        out << "?" << ret;
    }
    return out;
}

// Return the URL of location LOC under the tab TAB names, which is the empty string for
// the records themselves.
//
// A tab is the one part of a page's state that a link does not carry, so it cannot come
// from the flags in force -- the strip names the tab it leads to, and this joins that
// name to the sticky flags the location already carries.
tempstr doc::Tabhref(algo::strptr loc, algo::strptr tab) {
    tempstr flag(UrlFlag(loc));
    tempstr ret;
    ret << UrlBase() << "/" << (EndsWithQ(loc, ".md") ? algo::strptr(algo::ReplaceExt(loc, ".html")) : loc);
    ret << flag;
    if (ch_N(tab) > 0) {
        ret << (ch_N(flag) > 0 ? "&" : "?") << tab;
    }
    return ret;
}

// Return the URL of location LOC.
//
// A markdown document is spelled .html, which is what a browser expects of a page
// and what makes the home button README.html.  Every other location goes through
// unchanged: a path to a source file is already what it is, and a query, a view of
// a query and help carry no character a URL path cannot.
//
// A fragment stays last, after the flags.  It names a heading inside the page rather
// than part of what is being asked for, which is also why the server never sees one --
// a browser keeps it -- and why the script is what puts it into the box.
tempstr doc::Href(algo::strptr loc) {
    algo::strptr path = Pathcomp(loc, "#LL");
    algo::strptr frag = Pathcomp(loc, "#LR");
    bool named = frag != loc && ch_N(frag) > 0;
    bool markdown = EndsWithQ(path, ".md");
    tempstr ret;
    ret << UrlBase() << "/";
    ret << (markdown ? algo::strptr(algo::ReplaceExt(path, ".html")) : path);
    ret << UrlFlag(path);
    if (named) {
        ret << "#" << frag;
    }
    return ret;
}

// Return the location the URL path PATH names, the prefix already off it.
// Naming nothing is the documentation index, which is what the home button leads
// to.
tempstr doc::UrlLoc(algo::strptr path) {
    tempstr ret;
    if (ch_N(path) == 0) {
        ret << "txt/README.md";
    } else if (EndsWithQ(path, ".html")) {
        ret << algo::ReplaceExt(path, ".md");
    } else {
        ret << path;
    }
    return ret;
}

// Return true when LOC names a directory of the tree.
//
// A breadcrumb makes every component of a path a link, so each of them has to
// answer with something, and what a directory answers with is what it holds.  The
// trailing slash is the whole of the test, and it is what a browser sends for a
// crumb.
bool doc::DirQ(algo::strptr loc) {
    return ch_N(loc) > 0 && loc.elems[ch_N(loc) - 1] == '/';
}

// Append TEXT to OUT with the characters HTML reserves written as entities.
//
// Everything that reaches a page goes through here: a document's own words, a
// record acr printed, a prototype, and the key a visitor typed into the search
// box.  A `<` that arrived as data and left as markup is the whole of how a page
// becomes something its author did not write, and an include line in a C++ listing
// carries one on every file.
void doc::HtmlEsc(algo::cstring &out, algo::strptr text) {
    frep_(i, ch_N(text)) {
        char ch = text.elems[i];
        if (ch == '&') {
            out << "&amp;";
        } else if (ch == '<') {
            out << "&lt;";
        } else if (ch == '>') {
            out << "&gt;";
        } else if (ch == '"') {
            out << "&quot;";
        } else if (ch == '\'') {
            out << "&#39;";
        } else {
            out << ch;
        }
    }
}

// Return the URL of location LOC, or nothing when LOC names nothing this tool can
// open.  An attribute that refers to no record gets nothing, and its text then
// reaches the page unlinked.
static tempstr LocHref(algo::strptr loc) {
    tempstr ret;
    if (ch_N(loc) > 0) {
        ret << doc::Href(loc);
    }
    return ret;
}

// Return the URL of the page at LOC with the fold option OPT the other way, or nothing
// when OPT is not one of doc's fold options.
//
// Opening a section is not a navigation: the location is the same page, and what changes
// is a view flag that rides along in the URL.  So the answer is this page's own URL built
// with the flag flipped, which is what `doc <loc> -code` would print.  Foldflip is asked
// to do the flipping, and asked a second time to put it back, so the one list of which
// options are folds stays where it already is.
static tempstr Foldhref(algo::strptr loc, algo::strptr opt) {
    tempstr ret;
    bool fold = doc::Foldflip(opt);
    if (fold) {
        ret << doc::Tabhref(loc, doc::Curtab());
        doc::Foldflip(opt);
    }
    return ret;
}

// Return the URL a link TARGET written in the document at BASE leads to.
//
// A target has two halves, either of which may be missing: a path, and an anchor
// naming a heading inside it.  A table of contents entry is all anchor, and it
// names this very page, so what comes back is the fragment alone and the browser
// needs no roundtrip at all for it.  A target naming both resolves the path and
// keeps the fragment after it, which is the jump the terminal makes in two steps
// and a browser makes in one.
//
// A web address comes back as itself.  The terminal declines one, because
// following it there would mean launching a browser out of a documentation reader
// -- and a browser is already where this page is being read.
static tempstr HtmlHref(algo::strptr base, algo::strptr target) {
    bool web = StartsWithQ(target, "http:") || StartsWithQ(target, "https:") || StartsWithQ(target, "mailto:");
    algo::strptr frag = Pathcomp(target, "#LR");
    bool named = !web && frag != target && ch_N(frag) > 0;
    tempstr fold(web ? tempstr() : Foldhref(base, Pathcomp(target, "#LL")));
    tempstr loc(web || ch_N(fold) > 0 ? tempstr() : doc::ResolveTarget(base, target));
    tempstr ret;
    if (web) {
        ret << target;
    } else if (ch_N(fold) > 0) {
        ret << fold;
    } else {
        ret << LocHref(loc);
    }
    if (named) {
        ret << "#" << frag;
    }
    return ret;
}

// Append an anchor to OUT with HREF for its target, TEXT for its text and CLASS
// for its class when one is given.  An empty href leaves the text unlinked.
static void PutHref(algo::cstring &out, algo::strptr href, algo::strptr text, algo::strptr klass) {
    if (ch_N(href) == 0) {
        doc::HtmlEsc(out, text);
    } else {
        out << "<a href=\"";
        doc::HtmlEsc(out, href);
        out << "\"";
        if (ch_N(klass) > 0) {
            out << " class=\"" << klass << "\"";
        }
        out << ">";
        doc::HtmlEsc(out, text);
        out << "</a>";
    }
}

// Append the cells of RUN to OUT, adjacent cells of one style inside one span.  With
// LINE0 nonzero each line is wrapped in a span whose id is its line number, counting
// from LINE0.
//
// The lexer has already decided what every byte of a listing is, and it recorded
// that decision on each cell as a colour and a set of style bits.  So the browser
// needs no lexer of its own: it needs the runs where those agree, and the
// stylesheet names each of them.
//
// The line ids are what make a listing navigable.  A function link, a chunk link and
// the pane's map of a file all name a line, and a browser scrolls to a line only if
// something on the page carries its name.  The style span closes and reopens at each
// break so that the line wrapper is always the outer element -- a colour run that
// crossed a newline would otherwise interleave with it.
static void PutRunHtml(algo::cstring &out, doc::FRun &run, int line0) {
    doc::Attr attr(doc_Attr_plain);
    doc::Color color(doc_Color_default);
    bool inspan = false;
    int link = 0;
    int line = line0;
    out << "<span class=\"ln\"";
    if (line0 > 0) {
        out << " id=\"L" << line << "\"";
    }
    out << ">";
    frep_(i, ary_N(run)) {
        doc::Cell &cell = ary_qFind(run, i);
        bool same = inspan && cell.color == color && cell.attr == attr && cell.link == link;
        tempstr klass;
        algo::ListSep ls(" ");
        if (cell.color.value != doc_Color_default) {
            klass << ls << "c-" << value_ToCstr(cell.color);
        }
        if (cell.attr.value & doc_Attr_dim) {
            klass << ls << "a-dim";
        }
        if (cell.attr.value & doc_Attr_bold) {
            klass << ls << "a-bold";
        }
        if (cell.attr.value & doc_Attr_italic) {
            klass << ls << "a-italic";
        }
        if (inspan && !same) {
            out << "</span>";
            inspan = false;
        }
        if (link > 0 && cell.link != link) {
            out << "</a>";
        }
        if (cell.link > 0 && cell.link != link) {
            doc::FLink *rec = doc::link_Find(cell.link - 1);
            out << "<a href=\"";
            doc::HtmlEsc(out, HtmlHref(doc::_db.page, rec ? algo::strptr(rec->target) : algo::strptr()));
            out << "\">";
        }
        if (!inspan && ch_N(klass) > 0) {
            out << "<span class=\"" << klass << "\">";
            inspan = true;
        }
        attr = cell.attr;
        color = cell.color;
        link = cell.link;
        tempstr text;
        doc::PrintUtf8(cell.ch, text);
        doc::HtmlEsc(out, text);
        if (cell.ch == '\n' && i + 1 < ary_N(run)) {
            if (inspan) {
                out << "</span>";
                inspan = false;
            }
            if (link > 0) {
                out << "</a>";
                link = 0;
            }
            line++;
            out << "</span><span class=\"ln\"";
            if (line0 > 0) {
                out << " id=\"L" << line << "\"";
            }
            out << ">";
        }
    }
    if (inspan) {
        out << "</span>";
    }
    if (link > 0) {
        out << "</a>";
    }
    out << "</span>";
}

// Append the fenced block TEXT, whose fence named the language LANG, to OUT as a
// coloured listing.  LINE0 is the file line its first line came from, or zero for a
// block that is not a file and so has no lines to name.
// Append the line numbers of a box to OUT: NLINE of them, the first being LINE0.
//
// They are one element of their own, laid in the padding to the left of the text and
// marked unselectable, so that a reader dragging across the code drags across the code
// alone.  Numbers written in front of each line -- as text or as generated content -- are
// inside the selection whatever the stylesheet says, and land in the paste.
//
// The column lines up because the lines of a box do not wrap: a box that is too wide
// scrolls sideways, so the nth number is beside the nth line.
static void PutGutter(algo::cstring &out, int nline, int line0) {
    out << "<span class=\"gut\" aria-hidden=\"true\">";
    frep_(i, nline) {
        out << (i > 0 ? "\n" : "") << (line0 + i);
    }
    out << "</span>";
}

// Append TEXT to OUT as the lines of a box, each line a span so that the stylesheet can
// number it in a column the mouse does not select.
//
// A word that names a ctype becomes a link to that ctype's page.  What amc_vis draws is a
// graph of ctypes, so nearly every word in it is one a reader wants to open -- and a
// drawing whose boxes are not clickable is a picture of the thing instead of a way into it.
static void PutTextline(algo::cstring &out, algo::strptr text) {
    out << "<pre>";
    PutGutter(out, doc::NLine(text), 1);
    out << "<code class=\"num\">";
    int nline = 0;
    ind_beg(algo::Line_curs, line, text) {
        out << (nline > 0 ? "\n" : "") << "<span class=\"ln\">";
        nline++;
        int i = 0;
        while (i < ch_N(line)) {
            int end = doc::WordEnd(line, i);
            algo::strptr word(line.elems + i, i32_Max(0, end - i));
            doc::FCtype *ctype = end > i ? doc::ind_ctype_Find(word) : NULL;
            if (ctype) {
                PutHref(out, LocHref(tempstr() << "ctype:" << ctype->ctype), word, "");
                i = end;
            } else if (end > i) {
                doc::HtmlEsc(out, word);
                i = end;
            } else {
                doc::HtmlEsc(out, algo::strptr(line.elems + i, 1));
                i++;
            }
        }
        out << "</span>";
    }ind_end;
    out << "</code></pre>\n";
}

// Append the header of a box to OUT: the format FMT of what is in it, then AT -- what the
// box holds or where it came from -- as a link to HREF when there is one, and the glyph
// that copies the box.
//
// A box of text on this page is one of half a dozen things -- ssim records, a generated
// header, a shell command, a drawing -- and which one it is decides how a reader reads it.
// So the box says: `ssim: 14 records`, `c++: include/gen/dmmeta_gen.h:2052`, `shell`.  The
// location is a link, because a chunk of a generated header is worth opening where it
// lives.
//
// The copy glyph is an ordinary element and the script at the foot of the page makes it
// work.  Nothing is lost without the script: the text is selectable, which is how it was
// copied before.
static void PutCap(algo::cstring &out, algo::strptr fmt, algo::strptr at, algo::strptr href) {
    out << "<div class=\"cap\"><span class=\"fmt\">";
    doc::HtmlEsc(out, ch_N(fmt) > 0 ? fmt : algo::strptr("text"));
    if (ch_N(at) > 0) {
        out << ": ";
        if (ch_N(href) > 0) {
            PutHref(out, href, at, "");
        } else {
            doc::HtmlEsc(out, at);
        }
    }
    out << "</span>";
    out << "<span class=\"copy\" title=\"copy\">"
        << "<svg viewBox=\"0 0 18 18\" width=\"16\" height=\"16\" aria-hidden=\"true\">"
        << "<rect x=\"1\" y=\"1\" width=\"11\" height=\"13\" rx=\"1.6\"/>"
        << "<rect x=\"5.6\" y=\"4\" width=\"11\" height=\"13\" rx=\"1.6\"/>"
        << "</svg></span></div>\n";
}

// Append the mermaid source TEXT to OUT as a diagram for the browser to draw.
//
// A flowchart is a picture, and its source is not one: four documents of this tree carry
// one, and read as text they are a list of arrows nobody follows.  So the block goes out
// as the element mermaid looks for, and the page's own script fetches the library when a
// page carries at least one -- which is four pages out of twelve hundred, and none of the
// rest pays anything for it.
//
// The source is left in the element as it was written.  A browser with no network, or one
// the library fails to load in, shows exactly what the terminal shows, which is the source
// -- no worse than what a reader had before and never a blank where a picture should be.
static void PutDiagram(algo::cstring &out, algo::strptr text) {
    out << "<div class=\"box\">\n";
    PutCap(out, "mermaid", "", "");
    out << "<pre class=\"mermaid\">";
    doc::HtmlEsc(out, text);
    out << "</pre>\n</div>\n";
}

// Append the text TEXT to OUT as a coloured listing in a box of its own, whose header says
// its language LANG and, when there is one, the place AT it came from -- a link to HREF.
// LINE0 is the file line its first line came from, or zero for a block that is not a file.
static void PutListing(algo::cstring &out, algo::strptr lang, algo::strptr text, int line0, algo::strptr at, algo::strptr href) {
    if (lang == "mermaid") {
        PutDiagram(out, text);
    } else {
        doc::run_RemoveAll();
        doc::FRun &run = doc::run_Alloc();
        doc::Highlight(lang, text, run);
        out << "<div class=\"box\">\n";
        PutCap(out, lang, at, href);
        out << "<pre>";
        PutGutter(out, doc::NLine(text), i32_Max(1, line0));
        out << "<code class=\"num\">";
        PutRunHtml(out, run, line0);
        out << "</code></pre>\n</div>\n";
        doc::run_RemoveAll();
    }
}

// Append the markdown inline markup of IN to OUT as HTML, resolving every link
// against the document at BASE.
//
// The markup is read into spans first, so what is left here is the browser's own
// vocabulary: a span's style is the tags around its text, and a link is an anchor.
// The text is escaped on its way out, and a span's own text is all that reaches the
// page -- an html tag written into the document was dropped when it was read.
//
// A link whose target this tool will not open is rendered as its text alone.  The
// alternative is an anchor that leads nowhere, and a reader clicking one learns less
// than a reader who was never offered it.
//
// One anchor covers a link however many spans its text turned out to be, and the
// span's own number is what says when a new link has started -- two adjacent links to
// one target would otherwise read as a single anchor.
void doc::HtmlInline(algo::cstring &out, algo::strptr in, algo::strptr base) {
    doc::ReadInline(in);
    int ilink = 0;
    tempstr href;
    ind_beg(doc::_db_span_curs, span, doc::_db) {
        if (span.ilink != ilink) {
            if (ch_N(href) > 0) {
                out << "</a>";
            }
            ilink = span.ilink;
            ch_RemoveAll(href);
            if (ilink > 0) {
                href << HtmlHref(base, span.target);
            }
            if (ch_N(href) > 0) {
                out << "<a href=\"";
                doc::HtmlEsc(out, href);
                out << "\">";
            }
        }
        if (span.bold) {
            out << "<strong>";
        }
        if (span.italic) {
            out << "<em>";
        }
        if (span.code) {
            out << "<code>";
        }
        if (span.br) {
            out << "<br/>";
        } else {
            doc::HtmlEsc(out, span.text);
        }
        if (span.code) {
            out << "</code>";
        }
        if (span.italic) {
            out << "</em>";
        }
        if (span.bold) {
            out << "</strong>";
        }
    }ind_end;
    if (ch_N(href) > 0) {
        out << "</a>";
    }
}

// Append the pipe table TEXT to OUT as an HTML table, wrapped in the box that
// carries it sideways on a narrow screen rather than widening the page.
//
// The header row decides how many columns the table has, which is the rule the
// terminal follows and for the same reason: a generated table named four columns and
// then gave a ctype with several access paths a cell for each, so one row ran to nine.
// A renderer that widened the table to the widest row would leave the four that were
// named too narrow to read.
static void PutTable(algo::cstring &out, algo::strptr text, algo::strptr base) {
    int ncol = 0;
    int nrow = 0;
    out << "<div class=\"tablebox\"><table>\n";
    ind_beg(algo::Line_curs, line, text) {
        algo::strptr body = algo::Trimmed(line);
        if (!doc::SepRowQ(body)) {
            int i = doc::RowBegin(body);
            int cell = i;
            int icol = 0;
            algo::strptr tag(nrow == 0 ? "th" : "td");
            out << (nrow == 0 ? "<thead>" : (nrow == 1 ? "<tbody>" : "")) << "<tr>";
            while (i <= doc::RowEnd(body)) {
                bool escaped = i > 0 && body.elems[i - 1] == '\\';
                bool split = (i == doc::RowEnd(body) || body.elems[i] == '|') && !escaped;
                if (split && (nrow == 0 || icol < ncol)) {
                    out << "<" << tag << ">";
                    doc::HtmlInline(out, algo::Trimmed(algo::strptr(body.elems + cell, i - cell)), base);
                    out << "</" << tag << ">";
                }
                icol += split ? 1 : 0;
                cell = split ? i + 1 : cell;
                i++;
            }
            while (nrow > 0 && icol < ncol) {
                out << "<td></td>";
                icol++;
            }
            out << "</tr>" << (nrow == 0 ? "</thead>\n" : "\n");
            ncol = nrow == 0 ? icol : ncol;
            nrow++;
        }
    }ind_end;
    out << (nrow > 1 ? "</tbody>" : "") << "</table></div>\n";
}

// Append every parsed block to OUT as HTML, resolving links against the document
// at BASE.
//
// A heading carries the anchor abt_md planted under it as its id, so a link naming
// a heading of this page is one the browser resolves without a roundtrip.  List
// items are gathered into one list as long as they keep coming, which is what
// makes a wrapped item read as one item.
//
// An item carries its nesting depth as a class rather than as a list inside a list,
// because what the parser recorded is a depth per item and nothing in it says where
// the inner list began.  The stylesheet indents by that class, and the indentation is
// the one the terminal draws from the same number.
static void PutBlock(algo::cstring &out, algo::strptr base) {
    algo_lib::Regx section;
    algo_lib::Regx_ReadSql(section, doc::_db.cmdline.section, true);
    bool filter = ch_N(doc::_db.cmdline.section) > 0;
    bool show = !filter;
    algo::strptr open;
    ind_beg(doc::_db_block_curs, block, doc::_db) {
        bool heading = block.blocktype == doc_Blocktype_heading;
        bool bullet = block.blocktype == doc_Blocktype_bullet;
        bool number = block.blocktype == doc_Blocktype_number;
        algo::strptr want(bullet ? "ul" : (number ? "ol" : ""));
        if (heading && filter) {
            tempstr flat;
            doc::HtmlInline(flat, block.text, base);
            show = algo_lib::Regx_Match(section, flat);
        }
        if (show && open != want) {
            if (ch_N(open) > 0) {
                out << "</" << open << ">\n";
            }
            if (ch_N(want) > 0) {
                out << "<" << want << ">\n";
            }
            open = want;
        }
        if (show && heading) {
            int level = i32_Min(4, i32_Max(1, block.level));
            out << "<h" << level;
            if (ch_N(block.anchor) > 0) {
                out << " id=\"";
                doc::HtmlEsc(out, block.anchor);
                out << "\"";
            }
            out << ">";
            doc::HtmlInline(out, block.text, base);
            out << "</h" << level << ">\n";
        } else if (show && block.blocktype == doc_Blocktype_code) {
            PutListing(out, block.lang, block.text, 0, block.cmd, "");
        } else if (show && block.blocktype == doc_Blocktype_table) {
            PutTable(out, block.text, base);
        } else if (show && block.blocktype == doc_Blocktype_rule) {
            out << "<hr/>\n";
        } else if (show && block.blocktype == doc_Blocktype_quote) {
            out << "<blockquote><p>";
            doc::HtmlInline(out, block.text, base);
            out << "</p></blockquote>\n";
        } else if (show && ch_N(want) > 0) {
            out << "<li class=\"l" << i32_Min(4, i32(block.level) + 1) << "\">";
            doc::HtmlInline(out, block.text, base);
            out << "</li>\n";
        } else if (show) {
            out << "<p>";
            doc::HtmlInline(out, block.text, base);
            out << "</p>\n";
        }
    }ind_end;
    if (ch_N(open) > 0) {
        out << "</" << open << ">\n";
    }
}

// Append the file at PATH to OUT: a markdown document as its blocks, anything else
// as one coloured listing.
void doc::PutFile(algo::cstring &out, algo::strptr path) {
    doc::ParseFile(path);
    bool listing = doc::block_N() == 1 && doc::block_qFind(0).blocktype == doc_Blocktype_code;
    tempstr intro(doc::IntroText(path));
    if (ch_N(intro) > 0) {
        doc::ParseText(doc::Withintro(algo::FileToString(path, algo::FileFlags()), intro));
    }
    if (listing) {
        PutListing(out, doc::block_qFind(0).lang, doc::block_qFind(0).text, 1, path, "");
    } else {
        PutBlock(out, path);
    }
    if (doc::FootdirQ(path)) {
        doc::ParseText(doc::DirText(tempstr() << Pathcomp(path, "/RL") << "/", path));
        PutBlock(out, path);
    }
}

// Append the foot of the page about the document at PATH to OUT: the link to the
// internals of the namespace it is about, and the other documents the key matched.
//
// It comes after the sections rather than before them, because both of these lead the
// reader somewhere else and the sections are still the page they are on.
static void PutFoot(algo::cstring &out, algo::strptr path) {
    tempstr foot(doc::InternalsText(path));
    if (ch_N(foot) > 0) {
        doc::ParseText(foot);
        PutBlock(out, path);
    }
    if (doc::c_match_N() > 1 && path == algo::strptr(doc::c_match_qFind(0).gitfile)) {
        int i = 0;
        out << "<h3>see also</h3>\n<ul>\n";
        ind_beg(doc::_db_c_match_curs, readmefile, doc::_db) {
            if (i > 0) {
                out << "<li>";
                PutHref(out, LocHref(readmefile.gitfile), readmefile.gitfile, "");
                out << "</li>\n";
            }
            i++;
        }ind_end;
        out << "</ul>\n";
    }
}
// Append the command line CMD to OUT, put away behind the word `query` when it is long
// enough to be in the way.
//
// A disclosure and not a script: the summary is the word, the line is inside it, and a
// browser opens it on a click with nothing loaded to make that happen.  It is a div and
// not a p, because a paragraph may hold only phrasing content and a details element is
// not that.
static void PutQuerytext(algo::cstring &out, algo::strptr cmd) {
    if (doc::Querytext(cmd) == cmd) {
        doc::HtmlEsc(out, cmd);
    } else {
        out << "<details class=\"query\"><summary>query</summary>";
        doc::HtmlEsc(out, cmd);
        out << "</details>";
    }
}


// Return the tab the page is showing: the schema, the command line, or the records,
// which is the tab a location carries no parameter for.
tempstr doc::Curtab() {
    tempstr ret;
    if (doc::_db.meta) {
        ret << "meta";
    } else if (doc::_db.showcmd) {
        ret << "cmd";
    }
    return ret;
}

// Append one entry of the tab strip to OUT: the word TEXT, leading to LOC under the tab
// TAB, and marked when that is the tab now showing.
static void PutTabitem(algo::cstring &out, algo::strptr loc, algo::strptr tab, algo::strptr text) {
    bool on = tab == algo::strptr(doc::Curtab());
    out << "<a class=\"tab" << (on ? " on" : "") << "\" href=\"";
    doc::HtmlEsc(out, doc::Tabhref(loc, tab));
    out << "\">";
    doc::HtmlEsc(out, text);
    out << "</a>\n";
}

// Append the tab strip of the page at LOC to OUT.
//
// Questions about one thing, of which a reader is looking at exactly one: for a selection
// the records, the schema those records are instances of, or the command line that fetched
// them; for one function its source or the command line that found it.  They are drawn as
// tabs and not as boxes because they are alternatives, and none of them travels along a link
// -- a reader who clicks an attribute wants that attribute's records, not its schema.
static void PutTab(algo::cstring &out, algo::strptr loc) {
    out << "<nav class=\"tabs\">\n";
    if (doc::FuncsrcQ(loc)) {
        PutTabitem(out, loc, "", "Code");
    } else {
        PutTabitem(out, loc, "", "Records");
        PutTabitem(out, loc, "meta", "Meta");
    }
    PutTabitem(out, loc, "cmd", "Query");
    out << "</nav>\n";
}

// Append one attribute of a record to OUT: the text TEXT, coloured by what it is and
// linked when it names something this tool can open.  SSIMFILE is the table the record
// belongs to, and PKEY says the attribute is the record's own key.
//
// The colour says what the token is and the underline says it is a link, which are two
// facts and want two channels: a name is sky blue, a value amber, a quoted value pale
// green, exactly as the terminal paints them.  Colouring the links instead leaves a page
// of records one wall of blue that says nothing about which token is a name and which a
// value -- on a page where nearly every token is a link, the colour has to carry the
// other fact.
static void PutRecattr(algo::cstring &out, algo::strptr ssimfile, algo::strptr text, bool pkey) {
    int colon = algo::FindChar(text, ':');
    algo::strptr name = colon >= 0 ? algo::strptr(text.elems, colon) : text;
    algo::strptr shown = colon >= 0 ? doc::RestFrom(text, colon + 1) : algo::strptr();
    bool quoted = ch_N(shown) > ch_N(doc::Unquote(shown));
    tempstr href(LocHref(doc::AttrTarget(ssimfile, text, pkey)));
    bool linked = ch_N(href) > 0;
    if (linked) {
        out << "<a class=\"rec\" href=\"";
        doc::HtmlEsc(out, href);
        out << "\">";
    }
    out << (colon >= 0 ? "<span class=\"c-skyblue\">" : "<span>");
    doc::HtmlEsc(out, name);
    out << "</span>";
    if (colon >= 0) {
        out << ":<span class=\"" << (quoted ? "c-palegreen" : "c-amber") << "\">";
        doc::HtmlEsc(out, shown);
        out << "</span>";
    }
    if (linked) {
        out << "</a>";
    }
}

// Append the answer ANSWER to OUT as records, one line each, under a header carrying the
// count of them.
//
// The lines are copied over as acr wrote them, spaces and all, so the columns acr
// aligned stay aligned and a tree keeps its indentation.  That is why the answer goes
// into a pre rather than becoming a table: a record dump is aligned text whose alignment
// means something, and a -t closure carries its depth in that indentation.  The walk
// exists to find where each token starts and stops, so an anchor can be hung on an
// attribute without the text being rebuilt.
static void PutRecline(algo::cstring &out, algo::strptr answer) {
    out << "<div class=\"box\">\n";
    PutCap(out, "ssim", doc::Count(doc::NRecord(answer), "record"), "");
    out << "<pre>";
    PutGutter(out, doc::NLine(answer), 1);
    out << "<code class=\"num\">";
    int nline = 0;
    ind_beg(algo::Line_curs, line, answer) {
        tempstr ssimfile;
        out << (nline > 0 ? "\n" : "") << "<span class=\"ln\">";
        nline++;
        int i = 0;
        int itok = 0;
        while (i < ch_N(line)) {
            bool space = line.elems[i] == ' ';
            int end = space ? i + 1 : doc::TokenEnd(line, i);
            algo::strptr text(line.elems + i, end - i);
            if (space) {
                doc::HtmlEsc(out, text);
            } else if (itok == 0) {
                doc::FSsimfile *file = doc::ind_ssimfile_Find(text);
                out << (file ? "<span class=\"c-paleblue\">" : "<span class=\"a-dim\">");
                doc::HtmlEsc(out, text);
                out << "</span>";
                ch_RemoveAll(ssimfile);
                ssimfile << (file ? text : algo::strptr());
            } else {
                PutRecattr(out, ssimfile, text, itok == 1);
            }
            itok += space ? 0 : 1;
            i = end;
        }
        out << "</span>";
    }ind_end;
    out << "</code></pre>\n</div>\n";
}

// Append one box of records to OUT: the answer to the command line CMD.
//
// Records and Meta are one option apart -- acr answers about the schema of the same
// selection instead of its rows -- so both arrive here as a command line to run.  The
// Query tab runs nothing: the command line is the answer there, and it is shown whole
// rather than put away behind a word, since a reader on that tab asked to read it.  Every
// box on the page follows the tab, so a page shows one kind of thing throughout.
static void PutRecbox(algo::cstring &out, algo::strptr cmd) {
    if (doc::_db.showcmd) {
        PutListing(out, "shell", cmd, 0, "", "");
    } else {
        PutRecline(out, doc::ToolOut(cmd));
    }
}

// Append the records section of the page at LOC to OUT, CMD being the command line that
// answers it.
//
// The tabs sit on the top edge of the box they name, so a reader sees the three of them
// naming what is inside that box rather than the page below it.  Nothing wraps the two: a
// border around a border says there is a third thing there, and there is not.
static void PutRecsect(algo::cstring &out, algo::strptr loc, algo::strptr cmd) {
    out << "<section id=\"records\">\n";
    PutTab(out, loc);
    PutRecbox(out, cmd);
    out << "</section>\n";
}

// Append the records QUERY selects to OUT.
void doc::PutQuery(algo::cstring &out, algo::strptr query) {
    PutRecsect(out, query, doc::AcrCmdline(query));
}


// Append src_func's answer about the subjects already recorded to OUT, the file
// position of each a link to that one function's source.  QUERY names the subjects, and
// is on the page only so that a page about nothing can say what it asked.
static void PutFuncset(algo::cstring &out, algo::strptr query) {
    bool have = doc::FuncregxQ(tempstr() << "func:" << query) || doc::subject_N() > 0;
    tempstr cmd(have ? doc::FuncCmdline(query) : tempstr());
    tempstr answer(have ? doc::ToolOut(cmd) : tempstr());
    out << "<div class=\"note\">";
    if (have) {
        PutQuerytext(out, cmd);
    } else {
        out << "<span class=\"miss\">acr ";
        doc::HtmlEsc(out, query);
        out << " selects no ctype and no namespace</span>";
    }
    out << "</div>\n<div class=\"box\">\n";
    PutCap(out, "c++", doc::Count(doc::NFunc(answer), "function"), "");
    out << "<pre>";
    PutGutter(out, doc::NLine(answer), 1);
    out << "<code class=\"num\">";
    int nline = 0;
    bool keyed = false;
    ind_beg(algo::Line_curs, line, answer) {
        out << (nline > 0 ? "\n" : "") << "<span class=\"ln\">";
        nline++;
        int end = doc::LocEnd(line);
        int colon = algo::FindChar(line, ':');
        algo::strptr proto = end >= 0 ? doc::RestFrom(line, doc::SkipSpace(line, end + 1)) : algo::strptr();
        tempstr name(end >= 0 ? doc::Funcname(proto) : tempstr());
        tempstr key(end < 0 ? doc::Funckey(line) : tempstr());
        tempstr keyloc(ch_N(key) > 0 ? doc::Funckeyloc(key) : tempstr());
        if (end >= 0) {
            tempstr target;
            target << algo::strptr(line.elems, colon) << ":" << algo::strptr(line.elems + colon + 1, end - colon - 1);
            out << "<span id=\"fn-";
            doc::HtmlEsc(out, name);
            out << "\">";
            if (keyed) {
                out << "<span class=\"a-dim\">";
                doc::HtmlEsc(out, algo::strptr(line.elems, end));
                out << "</span>";
            } else {
                PutHref(out, LocHref(target), algo::strptr(line.elems, end), "");
            }
            out << "  ";
            doc::run_RemoveAll();
            doc::FRun &run = doc::run_Alloc();
            doc::Highlight("c++", proto, run);
            PutRunHtml(out, run, 0);
            doc::run_RemoveAll();
            out << "</span>";
            doc::_db.navfunc << name << eol;
            keyed = false;
        } else if (ch_N(keyloc) > 0) {
            out << "<span class=\"a-dim\">// --- </span>";
            PutHref(out, LocHref(keyloc), key, "");
            keyed = true;
        } else {
            out << "<span class=\"a-dim\">";
            doc::HtmlEsc(out, line);
            out << "</span>";
        }
        out << "</span>";
    }ind_end;
    out << "</code></pre>\n</div>\n";
}

// Append the functions the records of QUERY are about to OUT.
void doc::PutFunc(algo::cstring &out, algo::strptr query) {
    doc::SubjectRead(doc::AcrFlat(query));
    PutFuncset(out, query);
}

// Append what amc generated for one subject to OUT: the chunk of the generated header
// that declares CTYPE, or, when the subject is a namespace, the header itself as a link.
static void PutCodeOne(algo::cstring &out, algo::strptr ctype) {
    tempstr path;
    int at = 0;
    tempstr chunk(doc::_db.subjectns ? tempstr() : doc::CodeChunk(ctype, path, at));
    if (doc::_db.subjectns) {
        tempstr file;
        file << "include/gen/" << ctype << "_gen.h";
        out << "<p class=\"note\">the generated header of ";
        doc::HtmlEsc(out, ctype);
        out << ", whole: ";
        PutHref(out, LocHref(file), file, "");
        out << "</p>\n";
    } else if (ch_N(chunk) > 0) {
        PutListing(out, "c++", chunk, at, tempstr() << path << ":" << at,
                   LocHref(tempstr() << path << "#L" << at));
    } else if (ch_N(path) > 0) {
        out << "<p class=\"note\">";
        doc::HtmlEsc(out, path);
        out << " declares no ";
        doc::HtmlEsc(out, ctype);
        out << "</p>\n";
    } else {
        out << "<p class=\"note\">";
        doc::HtmlEsc(out, ctype);
        out << " is a builtin type, and amc generates nothing for one</p>\n";
    }
}

// Append what amc generated for the ctypes the records of QUERY are about to OUT,
// one chunk of one generated header each.
void doc::PutCodeHtml(algo::cstring &out, algo::strptr query) {
    doc::SubjectRead(doc::AcrFlat(query));
    if (doc::subject_N() == 0) {
        out << "<p class=\"note\"><span class=\"miss\">acr ";
        doc::HtmlEsc(out, query);
        out << " selects no ctype and no namespace</span></p>\n";
    }
    ind_beg(doc::_db_subject_curs, subject, doc::_db) {
        PutCodeOne(out, subject.subject);
    }ind_end;
}
// Append the listing of the directory DIR to OUT.
//
// The text is markdown and goes through the parser a document goes through, so the
// listing is a real table here and the same table on the glass.
void doc::PutDir(algo::cstring &out, algo::strptr dir) {
    doc::ParseText(doc::DirText(dir, ""));
    PutBlock(out, "");
}

// Append the page of keys to OUT.
//
// The keys belong to the reader at the terminal, and the browser has none of its
// own -- but the page that describes them is a document, and a visitor reading the
// manual in a browser is who wants to know what the terminal does.  It is the same
// markdown either way, so its tables become HTML tables here without anything
// being written twice.
void doc::PutHelp(algo::cstring &out) {
    doc::ParseText(doc::HelpText());
    PutBlock(out, "help");
}

// Append what the key KEY could have meant to OUT: the documents whose path carries it,
// then the tables and ctypes whose name does.
//
// One text draws it on both devices, the way every other page this tool writes is drawn
// once and laid out twice.
static void PutMatch(algo::cstring &out, algo::strptr key) {
    doc::ParseText(doc::MatchText(key));
    PutBlock(out, key);
}

// Append the location LOC to OUT as the body of a page.
//
// The dispatch is the one the terminal renderer makes, because it is the same
// question: a location is a document, a query, a view of a query, a directory or
// the page of keys, and each has its own layout.
//
// Every branch here reads something -- a file, a table, a subprocess -- so what
// reaches this must be a location that has already been allowed.  A page whose
// Append the page about the namespace NS to OUT.
//
// The text is markdown and goes through the parser a document goes through, so a table
// written for the namespace page is a real table here and a link in it is followed like
// any other.  Nothing about the page knows which device is asking.
void doc::PutNs(algo::cstring &out, algo::strptr ns) {
    tempstr text(doc::NsText(ns));
    if (ch_N(text) > 0) {
        doc::ParseText(text);
        PutBlock(out, "");
    } else {
        out << "<p class=\"note\"><span class=\"miss\">the tree holds no namespace ";
        doc::HtmlEsc(out, ns);
        out << "</span></p>\n";
    }
}

// Append the page about the message location LOC to OUT: the roots of every protocol when
// LOC names none, and otherwise one message -- its declaration, then what begins with it
// or where it is received.
void doc::PutMsg(algo::cstring &out, algo::strptr loc) {
    algo::strptr name = Pathcomp(loc, ":LR");
    if (ch_N(name) == 0) {
        doc::ParseText(doc::MsgrootText());
        PutBlock(out, "");
    } else {
        PutRecsect(out, loc, doc::CtypeCmdline(name));
        tempstr text(doc::MsgText(name));
        if (ch_N(text) > 0) {
            doc::ParseText(text);
            PutBlock(out, "");
        }
    }
}

// Append the ctype NAME to OUT: what it was declared to be, what somebody wrote about the
// table it holds, and where it sits.
//
// The two sections a reader can open -- the access paths and the code -- are not written
// here.  They are about whatever records a page selects, and this page is one of those, so
// they follow every such body from one place.
void doc::PutCtype(algo::cstring &out, algo::strptr name) {
    PutRecsect(out, tempstr() << "ctype:" << name, doc::CtypeCmdline(name));
    doc::SubjectOne(name);
    tempstr prose(doc::Ssimdoc(doc::CtypeSsimfile(name)));
    if (ch_N(prose) > 0) {
        doc::ParseText(prose);
        PutBlock(out, "");
    }
    tempstr text(doc::CtypeText(name));
    if (ch_N(text) > 0) {
        doc::ParseText(text);
        PutBlock(out, "");
    }
}

// Append the source of the function LOC names to OUT, under the two tabs such a page
// carries: the source itself, and the command line that found it.
//
// The listing carries the file's own line numbers rather than starting again at one, so
// a link naming L and a number lands on the line it names here exactly as it does on
// the page showing the whole file.
void doc::PutFuncsrc(algo::cstring &out, algo::strptr loc) {
    doc::ReadFuncsrc(doc::FuncsrcName(loc), doc::FuncsrcSrc(loc));
    doc::Funcsrc &funcsrc = doc::_db.funcsrc;
    tempstr at;
    at << funcsrc.src << ":" << funcsrc.line;
    out << "<section id=\"code\">\n";
    PutTab(out, loc);
    if (doc::_db.showcmd) {
        PutListing(out, "bash", funcsrc.cmd, 0, "", "");
    } else if (ch_N(funcsrc.src) > 0) {
        PutListing(out, doc::FileLang(funcsrc.src), funcsrc.text, i32(funcsrc.first), at,
                   LocHref(tempstr() << funcsrc.src << "#L" << funcsrc.line));
    } else {
        out << "<p class=\"note\"><span class=\"miss\">the tree holds no such function</span></p>\n";
    }
    out << "</section>\n";
}

// Append the access path drawing of the ctypes REGX names to OUT.
//
// Every column of what amc_vis draws is placed, so it goes in a pre and nothing
// reformats it -- the same rule that holds acr's answer, for the same reason.
void doc::PutAccesspath(algo::cstring &out, algo::strptr regx) {
    command::amc_vis amc_vis;
    amc_vis.in = doc::_db.cmdline.in;
    (void)command::ctype_ReadStrptrMaybe(amc_vis, regx);
    tempstr cmd(command::amc_vis_ToCmdline(amc_vis));
    out << "<div class=\"note\">";
    PutQuerytext(out, cmd);
    out << "</div>\n<div class=\"box\">\n";
    PutCap(out, "text", "", "");
    PutTextline(out, doc::ToolOut(cmd));
    out << "</div>\n";
}

// Append the access paths of the ctypes the page at LOC is about to OUT.
static void PutVis(algo::cstring &out, algo::strptr loc) {
    tempstr cmd;
    tempstr drawing(doc::VisOut(loc, cmd));
    if (ch_N(drawing) == 0) {
        out << "<p class=\"note\"><span class=\"miss\">the records on this page name no ctype"
            << " and no namespace</span></p>\n";
    } else if (doc::_db.showcmd) {
        PutListing(out, "bash", cmd, 0, "", "");
    } else {
        out << "<div class=\"box\">\n";
        PutCap(out, "text", "", "");
        PutTextline(out, drawing);
        out << "</div>\n";
    }
}

// Append a section heading that opens and closes to OUT: the name NAME, anchored at SLUG,
// leading to the page at LOC with the flag FLAG the other way.
//
// A closed section is a page the reader has not asked for yet, so the heading is a link
// and the triangle says which way following it goes.  Opening one is not a navigation:
// the flag rides along every link below it, so a reader who opens the code and then
// follows an attribute arrives with the code still open.
//
// The link carries the section's own anchor, because opening a section is a fresh request
// for the page and a browser lands such a request at the top -- with the thing the reader
// just asked to see below a screenful of records.
static void PutFold(algo::cstring &out, algo::strptr loc, algo::strptr name, algo::strptr slug, algo::strptr opt, bool on) {
    tempstr href(Foldhref(loc, opt));
    href << "#" << slug;
    out << "<h3 id=\"" << slug << "\" class=\"fold\"><a href=\"";
    doc::HtmlEsc(out, href);
    out << "\"><span class=\"tri\">" << (on ? "&#9662;" : "&#9656;") << "</span> ";
    doc::HtmlEsc(out, name);
    out << "</a></h3>\n";
}

// Append the two sections every page of records carries to OUT: the access paths of the
// ctypes it is about, and the code amc generated for them.
//
// Both are about whatever the page selected rather than about one kind of page, so they
// are written here, once, after every body that selects records.  Both are closed until
// asked for: a reader arriving at a record wants the record, and amc_vis and src_func are
// the two questions on this page that cost real time to answer.
static void PutSection(algo::cstring &out, algo::strptr loc) {
    tempstr query(doc::SectionQuery(loc));
    tempstr table(doc::Datassimfile(loc));
    if (doc::SectionQ(loc)) {
        PutFold(out, loc, "Access Paths", "access-paths", "-vis", doc::_db.vis);
        if (doc::_db.vis) {
            PutVis(out, loc);
        }
        PutFold(out, loc, ch_N(doc::Sectionns(loc)) > 0 ? "Functions" : "Code", "code", "-code", doc::_db.code);
        if (doc::_db.code) {
            doc::SubjectRead(doc::AcrFlat(query));
            ind_beg(doc::_db_subject_curs, subject, doc::_db) {
                PutCodeOne(out, subject.subject);
            }ind_end;
            PutFuncset(out, query);
        }
    }
    if (ch_N(table) > 0) {
        PutFold(out, loc, "Data", "data", "-data", doc::_db.data);
        if (doc::_db.data) {
            PutRecbox(out, doc::AcrCmdline(tempstr() << table << ":%"));
        }
    }
    tempstr transcript(doc::Rectranscript(loc));
    if (ch_N(transcript) > 0) {
        out << "<h3 id=\"transcript\">Transcript</h3>\n";
        if (algo::FileQ(transcript)) {
            PutListing(out, "text", algo::FileToString(transcript, algo::FileFlags()), 0,
                       transcript, LocHref(transcript));
        } else {
            out << "<p class=\"note\"><span class=\"miss\">";
            doc::HtmlEsc(out, transcript);
            out << " has not been captured</span></p>\n";
        }
    }
    tempstr func(doc::Recfunc(loc));
    if (ch_N(func) > 0) {
        out << "<h3 id=\"as-function\">As Function</h3>\n";
        doc::PutFuncsrc(out, tempstr() << "func:" << func);
    }
    tempstr whole(doc::Wholetable(doc::LocQuery(loc)));
    if (ch_N(whole) > 0) {
        tempstr prose(doc::Ssimdoc(whole));
        out << "<h3 id=\"ssimfile\">Ssimfile</h3>\n";
        if (ch_N(prose) > 0) {
            doc::ParseText(prose);
            PutBlock(out, loc);
        }
        PutRecbox(out, doc::AcrFlatCmdline(tempstr() << "dmmeta.ssimfile:" << whole));
    }
}


// content comes from what a visitor typed is reached by its own name instead, which
// is what HtmlFind is for: it takes a key rather than a location, so no request text
// is ever dispatched on.  Falling through to the candidate list instead is how
// `?q=/etc/passwd` served that file twice, once through resolution and once through
// this file test.
void doc::HtmlBody(algo::cstring &out, algo::strptr loc) {
    doc::FDocpage *docpage = doc::Docpage(loc);
    doc::_db.page = loc;
    ch_RemoveAll(doc::_db.navfunc);
    doc::_db.p_out = &out;
    bool file = !docpage && !doc::DirQ(loc) && algo::FileQ(loc);
    if (docpage) {
        (*docpage->step)();
    } else if (doc::DirQ(loc)) {
        doc::PutDir(out, loc);
    } else if (file) {
        doc::PutFile(out, loc);
    } else {
        out << "<p class=\"note\"><span class=\"miss\">the tree holds no ";
        doc::HtmlEsc(out, loc);
        out << "</span></p>\n";
    }
    doc::_db.p_out = NULL;
    if (doc::FoldQ(loc)) {
        PutSection(out, loc);
    }
    if (file) {
        PutFoot(out, loc);
    }
}

// Append the location LOC to OUT with its words linked, and nothing else.
//
// A path's components are the directories above it, and each has a page of its own --
// what it holds.  The last component is the page itself and carries no link; a
// directory has no last component, since its text ends in the separator, so every one
// of its components is a link and the trailing slash falls out of the walk.  A query's components are the table it selects from, which leads to
// the whole of that table, and the key, which is where the reader is.  A view of a
// query opens with the view's own name, leading back to the records it is a view of.
static void PutShownloc(algo::cstring &out, algo::strptr loc) {
    bool view = !doc::FuncsrcQ(loc) && (doc::FuncQ(loc) || doc::CodeQ(loc));
    if (doc::FuncsrcQ(loc)) {
        PutHref(out, LocHref(Pathcomp(loc, ":LL")), Pathcomp(loc, ":LL"), "");
        out << ":";
        doc::HtmlEsc(out, Pathcomp(loc, ":LR"));
    } else if (view) {
        PutHref(out, LocHref(doc::LocQuery(loc)), Pathcomp(loc, ":LL"), "");
        out << ":";
        PutShownloc(out, doc::LocQuery(loc));
    } else if (doc::QueryQ(loc)) {
        tempstr table;
        table << Pathcomp(loc, ":LL") << ":%";
        PutHref(out, LocHref(table), Pathcomp(loc, ":LL"), "");
        out << ":";
        doc::HtmlEsc(out, Pathcomp(loc, ":LR"));
    } else {
        tempstr at;
        ind_beg(algo::Sep_curs, part, loc, '/') if (ch_N(part) > 0) {
            bool last = ch_N(at) + ch_N(part) == ch_N(loc);
            at << part << "/";
            if (last) {
                doc::HtmlEsc(out, part);
            } else {
                PutHref(out, LocHref(at), part, "");
                out << "/";
            }
        }ind_end;
    }
}

// Return the canonical text of the page showing LOC, with its words linked.
//
// The words are the ones the breadcrumb used to carry, and the text is the one the
// search box holds -- shown once rather than twice, because they were always the same
// thing said two ways.  What comes out here must read as exactly that text with the
// markup taken off, so that a reader who clicks into the line to edit it finds what
// they were looking at.  The flags go through as plain words: they are part of the
// state and the navigation pane is what changes them.
//
// So the flags here are the page's own flags, spelled by the one function that spells
// them.  A shorter list would make the two faces of the box two different strings, and
// clicking the line would rewrite it in front of the reader -- which is the one thing a
// line claiming to say where they are may not do.
static tempstr HtmlShown(algo::strptr loc) {
    tempstr flag(doc::PageFlag(loc));
    tempstr ret;
    if (ch_N(flag) > 0) {
        ret << "<span class=\"flag\">";
        doc::HtmlEsc(ret, algo::Trimmed(flag));
        ret << "</span> ";
    }
    PutShownloc(ret, loc);
    return ret;
}



// Return the navigation pane for location LOC, as HTML.
//
// The pane is one markdown text that both devices draw, so what is left here is where the
// browser puts it: an aside beside the page, which the stylesheet makes a column that
// scrolls with the reader.  A page with no map carries no element at all rather than an
// empty column.
static tempstr HtmlNav(algo::strptr loc) {
    tempstr text(doc::NavText(loc));
    tempstr ret;
    if (ch_N(text) > 0) {
        doc::ParseText(text);
        ret << "<aside class=\"side\">\n";
        PutBlock(ret, loc);
        ret << "</aside>\n";
    }
    return ret;
}

// Return the two things the page's footer says: the moment of the commit this binary was
// built from, and that commit's ref.
//
// The build stamps both into the binary, so the page says what the process running it was
// built from rather than what the checkout beside it holds -- which is the honest answer,
// and the reason nothing here runs git.  A tree unpacked without .git reports itself
// unversioned, and then the footer is empty.
//
// The documentation claims no copyright, the tree being under the GPL, so the footer names
// nobody: the license travels with the source and says what a reader may do with it.
static void ReadFoot(tempstr &commit, tempstr &gitref) {
    dev::Gitinfo gitinfo;
    (void)dev::Gitinfo_ReadStrptrMaybe(gitinfo, algo::gitinfo_Get());
    if (gitinfo.commitdate.value != 0) {
        algo::UnTime_Print(gitinfo.commitdate, commit);
    }
    gitref << gitinfo.gitref;
}

// Return the static template filled in with TITLE, TEXT, CRUMB, NAV and BODY.
//
// Every replacement is made in one pass over the template, so nothing that goes
// into the page is scanned for placeholders afterwards.  That is not tidiness: the
// template is a file of the tree like any other, so a reader can open it through
// this very tool, and a second pass would substitute the stylesheet into the
// listing of the template that names it.
static tempstr HtmlFill(algo::strptr title, algo::strptr text, algo::strptr crumb, algo::strptr nav, algo::strptr body) {
    vrfy(algo::FileQ("www/doc/page.html") && algo::FileQ("www/doc/doc.css"),
         "doc.notemplate  comment:'www/doc/page.html and www/doc/doc.css are read from the"
         " top of the checkout; run this from there'");
    tempstr home(doc::Href("txt/README.md"));
    tempstr help(doc::Href("txt/exe/doc/README.md"));
    tempstr action(doc::UrlRoot());
    tempstr commit;
    tempstr gitref;
    ReadFoot(commit, gitref);
    tempstr css(algo::FileToString("www/doc/doc.css", algo::FileFlags()));
    tempstr templ(algo::FileToString("www/doc/page.html", algo::FileFlags()));
    tempstr ret;
    int i = 0;
    while (i < ch_N(templ)) {
        int open = algo::FindStr(doc::RestFrom(templ, i), "{{");
        int at = open < 0 ? ch_N(templ) : i + open;
        int close = open < 0 ? -1 : algo::FindStr(doc::RestFrom(templ, at), "}}");
        algo::strptr name = close < 0 ? algo::strptr() : algo::strptr(templ.ch_elems + at + 2, close - 2);
        ret << algo::strptr(templ.ch_elems + i, at - i);
        if (name == "title") {
            doc::HtmlEsc(ret, title);
        } else if (name == "css") {
            ret << css;
        } else if (name == "home") {
            doc::HtmlEsc(ret, home);
        } else if (name == "help") {
            doc::HtmlEsc(ret, help);
        } else if (name == "action") {
            doc::HtmlEsc(ret, action);
        } else if (name == "query") {
            doc::HtmlEsc(ret, text);
        } else if (name == "shown") {
            ret << crumb;
        } else if (name == "nav") {
            ret << nav;
        } else if (name == "body") {
            ret << body;
        } else if (name == "commit") {
            doc::HtmlEsc(ret, commit);
        } else if (name == "gitref") {
            doc::HtmlEsc(ret, gitref);
        } else if (close >= 0) {
            ret << algo::strptr(templ.ch_elems + at, close + 2);
        }
        i = close < 0 ? ch_N(templ) : at + close + 2;
    }
    return ret;
}

// Return the page for location LOC.
tempstr doc::HtmlPage(algo::strptr loc) {
    tempstr body;
    doc::HtmlBody(body, loc);
    return HtmlFill(loc, doc::PageText(loc), HtmlShown(loc), HtmlNav(loc), body);
}

// Return the page for a location LOC this server does not answer for.
//
// A page that has moved or gone leaves links behind it, and a reader following one
// arrives here.  What they get is the page they expected -- the header, the search box,
// the home and help buttons -- with the body saying that this particular thing is not
// there, so the next thing they try is one keystroke away rather than a browser error.
// The location is taken as text and dispatched on by nothing, which is the same rule the
// candidate page follows: what a visitor typed reaches the page as words.
tempstr doc::HtmlMiss(algo::strptr loc) {
    tempstr shown;
    tempstr body;
    doc::HtmlEsc(shown, loc);
    body << "<p class=\"note\"><span class=\"miss\">404 &mdash; this page doesn't exist</span></p>\n";
    return HtmlFill(loc, loc, shown, HtmlNav("miss"), body);
}

// Return the page offering what the key KEY could have meant.
//
// It takes a key and not a location, which is the whole point: a key is what a
// visitor typed, and every branch of the body dispatch reads something.  So the one
// page whose content comes from a request is reached by name, its body is the
// candidate list and can be nothing else, and its breadcrumb is the key as text.
tempstr doc::HtmlFind(algo::strptr key) {
    tempstr shown;
    tempstr body;
    doc::HtmlEsc(shown, key);
    PutMatch(body, key);
    return HtmlFill(key, key, shown, HtmlNav("find"), body);
}
