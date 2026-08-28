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
// Source: cpp/doc/browse.cpp
//
// Reading the page: scrolling, following a link, searching, going back.
// The tool pages its own output rather than handing it to less, because a link
// cannot be selected from outside the program that owns the screen.  less has
// no way to say which row is showing or which run the reader is pointing at, so
// there is no arrangement in which it keeps the paging and this file keeps the
// links.
// Nothing here re-renders.  The page is a grid of cells laid out once, and
// moving the selection or typing into a search is a repaint of at most a
// screenful, with the highlight applied on the way out by the painter.  That is
// what makes a search that highlights as you type affordable on a document of
// several thousand rows.

#include "include/algo.h"
#include "include/doc.h"

#include <sys/ioctl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

// Put the terminal into the state a full-screen reader needs, and remember
// enough to undo it.
//
// Three things change together.  Characters stop being echoed and stop being
// held until a newline, so a keystroke arrives as it is typed.  The alternate
// screen is switched on, which is what lets the shell's scrollback come back
// untouched when the reader quits.  And the cursor is hidden, since it would
// otherwise sit wherever the last row was painted.
static void RawBegin(struct termios &saved) {
    struct termios raw;
    bool ok = tcgetattr(STDIN_FILENO, &saved) == 0;
    raw = saved;
    raw.c_lflag &= ~tcflag_t(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    ok = ok && tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0;
    if (ok) {
        prlog_("\033[?1049h\033[?25l\033[2J");
    }
    doc::screenrow_RemoveAll();
    doc::_db.quit = !ok;
}

// Undo RawBegin: leave the alternate screen, show the cursor, and put the
// terminal back the way it was found.
static void RawEnd(struct termios &saved) {
    prlog_("\033[?25h\033[?1049l");
    (void)tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
}

// Read the size of the terminal into _db, leaving the previous values when the
// terminal does not answer.
//
// A window that changed size takes the remembered screen with it.  What the
// painter skips is a line the screen already carries, and a resize reflows or
// discards every line the terminal was holding -- so a memory of it kept across
// the change would let the painter skip lines that are no longer there.
static void ReadSize() {
    struct winsize winsize;
    winsize.ws_row = 0;
    winsize.ws_col = 0;
    (void)ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);
    int rows = winsize.ws_row > 2 ? winsize.ws_row : doc::_db.rows;
    int cols = winsize.ws_col > 0 ? winsize.ws_col : doc::_db.cols;
    bool resized = rows != doc::_db.rows || cols != doc::_db.cols;
    doc::_db.rows = rows;
    doc::_db.cols = cols;
    doc::_db.width = winsize.ws_col > 0 ? i32_Min(100, winsize.ws_col) : doc::_db.width;
    if (resized) {
        doc::screenrow_RemoveAll();
    }
}

// Return the number of page rows that fit above the status line.
static int PageRows() {
    return i32_Max(1, doc::_db.rows - 1);
}

// Scroll so that row WANT is the first one showing, refusing to go past either
// end of the page.
static void ScrollTo(int want) {
    int last = i32_Max(0, i32(doc::row_N()) - PageRows());
    doc::_db.top = i32_Max(0, i32_Min(want, last));
}

// Scroll the least amount that brings row WANT onto the screen.  A row already
// showing leaves the page where it is, which is what keeps following a link
// near the middle of the screen from throwing the reader to the top.
static void ScrollOnto(int want) {
    bool above = want < doc::_db.top;
    bool below = want >= doc::_db.top + PageRows();
    if (above) {
        ScrollTo(want);
    } else if (below) {
        ScrollTo(want - PageRows() + 1);
    }
}

// Return true when link ISEL is one of those now on the screen.
static bool ShowingQ(int isel) {
    doc::FLink *link = isel > 0 && isel <= i32(doc::link_N()) ? doc::link_Find(isel - 1) : NULL;
    return link && link->row >= doc::_db.top && link->row < doc::_db.top + PageRows();
}

// Select a link on the screen, walking from wherever the selection was in the
// direction the page last moved, and select nothing at all when the screen
// carries no link.
//
// The selection is either a link the reader can see or nothing.  That invariant
// is what this exists to hold, and it is worth more than it looks: Enter opens
// the selection, so a selection somewhere else in the document is an Enter that
// opens a page the reader did not choose, with nothing on the screen to say why.
// A reader presses space, reads the screenful that arrives, and presses Enter on
// what they see -- and without this they would be pressing it on what they saw a
// screen ago.
//
// The direction matters because both ends of a screen are equally good answers
// and only one of them is the one the reader is moving toward.  Coming down a
// page, the first link on the new screen is what they are heading for; going
// back up, it is the last.
//
// A selection already showing is not moved, which is what keeps a one-line
// scroll from dragging it off the link the reader was aiming at.
static void SelectShowing(int delta) {
    int step = delta < 0 ? -1 : 1;
    int want = doc::_db.sel;
    while (!ShowingQ(want) && want + step >= 1 && want + step <= i32(doc::link_N())) {
        want += step;
    }
    doc::_db.sel = ShowingQ(want) ? want : 0;
}

// Select the first link on the screen, or nothing when the screen carries none.
// This is where a fresh page and a jump to a heading start from.
static void SelectVisible() {
    doc::_db.sel = 0;
    SelectShowing(1);
}

// Return the link n or p moves from when nothing is selected: the first one at
// or after the row showing when moving on, the last one before it when moving
// back.
//
// Nothing is selected exactly when the screen carries no link, and the reader is
// then in the middle of a document rather than at its start.  Moving from the
// beginning of the document would throw them back to the top; moving from the
// edge of what they are looking at continues the reading they were doing.
static int NearLink(int delta) {
    int ret = 0;
    frep_(i, doc::link_N()) {
        bool below = doc::link_qFind(i).row >= doc::_db.top;
        bool take = delta > 0 ? (ret == 0 && below) : !below;
        ret = take ? i32(i) + 1 : ret;
    }
    return ret;
}

// Move the selection DELTA links along the page and scroll to whatever it lands
// on.  The ends do not wrap: a reader holding n down should stop at the last
// link rather than silently start again from the top.
static void SelectLink(int delta) {
    int want = doc::_db.sel > 0 ? doc::_db.sel + delta : NearLink(delta);
    doc::_db.sel = i32_Max(1, i32_Min(want, i32(doc::link_N())));
    if (doc::link_N() > 0) {
        ScrollOnto(doc::link_qFind(doc::_db.sel - 1).row);
    }
}

// Move the selection to a link on the nearest row DELTA-wards that carries one,
// keeping as close to the column it was in as that row allows, and bring the
// page to it.
//
// The links of a record page are laid out in a grid: one row per record, one
// link per attribute of it.  Reaching the third attribute of the row below by
// stepping links in document order means walking off the end of the row you are
// on and back in along the next, which on a wide record is a dozen keystrokes
// for a move the eye made instantly.  So the vertical arrows move vertically:
// down goes to the next row carrying a link and lands on the one nearest the
// column just left, which is what keeps a walk down a column of `arg:` values in
// that column.
//
// A direction with no link left in it scrolls the page a line instead, which is
// what makes the arrows still do something at the ends of a document and on a
// page that carries no links at all.
static void SelectRow(int delta) {
    doc::FLink *at = doc::_db.sel > 0 ? doc::link_Find(doc::_db.sel - 1) : NULL;
    int row = at ? at->row : doc::_db.top;
    int col = at ? at->col : 0;
    int want_row = -1;
    ind_beg(doc::_db_link_curs, link, doc::_db) {
        bool ahead = delta > 0 ? link.row > row : link.row < row;
        bool nearer = want_row < 0 || (delta > 0 ? link.row < want_row : link.row > want_row);
        if (ahead && nearer) {
            want_row = link.row;
        }
    }ind_end;
    int want = 0;
    int best = 0;
    int i = 0;
    ind_beg(doc::_db_link_curs, link, doc::_db) {
        i++;
        int dist = link.col > col ? link.col - col : col - link.col;
        if (link.row == want_row && (want == 0 || dist < best)) {
            want = i;
            best = dist;
        }
    }ind_end;
    doc::_db.sel = want > 0 ? want : doc::_db.sel;
    if (want > 0) {
        ScrollOnto(doc::link_qFind(want - 1).row);
    } else {
        ScrollTo(doc::_db.top + delta);
    }
}

// Put the selected link halfway down the screen, and paint every line of it
// again.
//
// Two things a reader wants from one key.  Centring answers "where am I": a link
// reached by searching, or by following one, lands wherever the scroll left it,
// which is often hard against the top or bottom edge with the text that explains
// it off the screen.  Repainting answers "what is actually on my screen": the
// painter writes only the lines it believes have changed, so anything else that
// wrote to the terminal -- a stray message, a resize the tool did not see --
// leaves it believing something false, and forgetting the screen is how a reader
// says so.  Emacs spells both of these Ctrl-L.
static void Recenter() {
    doc::FLink *link = doc::_db.sel > 0 ? doc::link_Find(doc::_db.sel - 1) : NULL;
    if (link) {
        ScrollTo(link->row - PageRows() / 2);
    }
    doc::screenrow_RemoveAll();
}

// Move to the hit DELTA places along from the one the reader is standing on,
// scroll it onto the screen, and select the link it falls inside when there is
// one.
//
// Selecting the link is what makes searching a way of reaching one.  A document
// of this corpus cites its neighbours from the middle of a paragraph, and
// stepping to those links with n means passing every other link on the way;
// typing part of the title and stepping the hits goes straight there.
static void NextHit(int delta) {
    int atrow = doc::_db.hitrow;
    int atcol = doc::_db.hitcol + delta;
    bool found = false;
    while (!found && atrow >= 0 && atrow < doc::row_N()) {
        doc::Row &row = doc::row_qFind(atrow);
        while (!found && atcol >= 0 && atcol < row.n) {
            found = doc::HitStartQ(atrow, atcol);
            atcol += found ? 0 : delta;
        }
        if (!found) {
            atrow += delta;
            atcol = delta > 0 ? 0 : (atrow >= 0 && atrow < doc::row_N() ? doc::row_qFind(atrow).n - 1 : 0);
        }
    }
    if (found) {
        doc::_db.hitrow = atrow;
        doc::_db.hitcol = atcol;
        doc::Cell &cell = doc::cell_qFind(doc::row_qFind(atrow).begin + atcol);
        doc::_db.sel = cell.link > 0 ? cell.link : doc::_db.sel;
        ScrollOnto(atrow);
    }
}

// Move to the first hit at or after the row the search started from, so that
// each character typed lands the reader on the nearest match rather than
// throwing them to the top of the document.
static void FirstHit() {
    doc::_db.hitcol = -1;
    NextHit(1);
}

// Paint the screen: the visible rows of the page, then the status line.
//
// The whole screen is written in one go and sent as a single write, so a
// scroll or a keystroke redraws without the reader seeing the screen build up
// row by row.
//
// Ask the terminal to scroll the page area by DELTA lines, and move the
// remembered screen with it, so that only the lines the scroll exposed count as
// damaged.
//
// Comparing what was painted against what the screen carries finds every line
// changed after a scroll, because a scroll moves the content of all of them --
// which makes the cheapest keystroke in the reader the most expensive one to
// draw.  A terminal can perform that move itself, and far more cheaply than by
// being sent the text again.  So the page area is made a scrolling region, told
// to scroll, and the region is dropped again; the status line sits outside it
// and does not move.
//
// The memory of the screen is then rotated to match what the terminal now holds,
// including the blank lines the scroll exposed, because the compare that follows
// trusts it exactly.  A memory that disagreed with the glass by one line would
// skip a line that needed writing, and that line would stay wrong until
// something else happened to change it.
static void ScrollScreen(int delta, algo::cstring &out) {
    int n = delta > 0 ? delta : -delta;
    out << "\033[1;" << PageRows() << "r";
    out << "\033[" << n << (delta > 0 ? "S" : "T");
    out << "\033[r";
    frep_(i, PageRows()) {
        int to = delta > 0 ? i32(i) : PageRows() - 1 - i32(i);
        int from = to + delta;
        tempstr text;
        if (from >= 0 && from < PageRows()) {
            text = doc::screenrow_qFind(from).text;
        }
        doc::screenrow_qFind(to).text = text;
    }
}

// Put LINE on screen row IROW, writing nothing at all when that row already
// carries it.
//
// This is the whole of the damage tracking, and it is derived rather than
// declared: the comparison is against what was last written to that row, so no
// caller has to know which rows its action disturbed and none can get it wrong.
// Moving the selection changes two rows and the status line, and narrowing a
// query to one record changes the first few and blanks the rest -- in both cases
// what reaches the terminal is those rows and nothing else.
static void PutScreenrow(int irow, algo::strptr line, algo::cstring &out) {
    while (i32(doc::screenrow_N()) <= irow) {
        doc::FScreenrow &blank = doc::screenrow_Alloc();
        blank.text = "";
    }
    doc::FScreenrow &screenrow = doc::screenrow_qFind(irow);
    if (screenrow.text != line) {
        screenrow.text = line;
        out << "\033[" << irow + 1 << ";1H\033[K" << line;
    }
}

// Paint the screen: the visible rows of the page, then the status line.
//
// A frame is as tall as the screen and no taller, and that is what stops it
// flickering.  A terminal that is asked to draw more lines than it has scrolls,
// and a frame that scrolls the window every time it is painted is a page that
// jumps under the reader.  Two things can make a frame too tall: a row wider
// than the window, which wraps onto a second display line -- a record copied
// from acr runs to a hundred and forty columns -- and a status line that reaches
// the last column.  So every row is cut at the window's edge, the status line is
// cut with it, and automatic wrapping is turned off for the duration in case
// anything reaches the last column anyway.
//
// Only the rows that changed are written.  Every row is painted, since painting
// is how the tool learns what a row should look like, but a row the screen
// already carries produces no output -- so a keystroke that moves the selection
// costs three lines rather than a screenful.
//
// A scroll is handed to the terminal first.  Scrolling changes the content of
// every line, so the compare alone would find the whole screen damaged; asking
// the terminal to move the lines and rotating the memory of the screen to match
// leaves only the lines the scroll exposed different.  That is what makes j cost
// one line.  It is done only when the page and the window are the ones the
// remembered screen was painted for -- otherwise there is nothing to move, and
// the compare is going to rewrite everything regardless.
//
// Whatever does go out goes in one call, bracketed by the synchronized-update
// mode, which asks the terminal to hold what it composes until the frame closes.
// That covers what is left: a frame arrives as an erase of each line followed by
// that line's text, and a terminal is free to put the erased state on the glass
// before the text lands.  A terminal that does not know the mode ignores both
// sequences.
static void Draw() {
    algo::tempstr out;
    out << "\033[?2026h\033[?7l";
    int delta = doc::_db.top - doc::_db.screentop;
    int moved = delta > 0 ? delta : -delta;
    bool same = doc::_db.screenpage == algo::strptr(doc::_db.page) && i32(doc::screenrow_N()) == PageRows() + 1;
    if (same && moved > 0 && moved < PageRows()) {
        ScrollScreen(delta, out);
    }
    doc::_db.screentop = doc::_db.top;
    doc::_db.screenpage = doc::_db.page;
    frep_(i, PageRows()) {
        algo::tempstr line;
        int irow = doc::_db.top + i32(i);
        if (irow < doc::row_N()) {
            doc::PaintRow(irow, doc::_db.sel, line, doc::_db.cols);
        }
        PutScreenrow(i32(i), line, out);
    }
    algo::tempstr bar;
    if (doc::_db.searching) {
        bar << "/" << doc::_db.find;
    } else {
        int pct = doc::row_N() > 0 ? i32((doc::_db.top + PageRows()) * 100 / doc::row_N()) : 100;
        bar << doc::PageText(doc::_db.page);
        bar << "  " << i32_Min(100, pct) << "%";
        if (doc::_db.sel > 0) {
            bar << "  link " << doc::_db.sel << " of " << doc::link_N();
        } else if (doc::link_N() > 0) {
            bar << "  " << doc::link_N() << " links";
        }
        bar << "   ? for keys";
    }
    int nbar = i32_Min(ch_N(bar), i32_Max(0, doc::_db.cols - 2));
    algo::tempstr status;
    status << "\033[7m " << ch_GetRegion(bar, 0, nbar) << " \033[0m";
    PutScreenrow(PageRows(), status, out);
    out << "\033[?7h\033[?2026l";
    prlog_(out);
}

// Record where the reader is standing, so Back can bring them to it.
//
// The stack holds a display location -- a document and the row it was scrolled
// to -- rather than a document alone.  A jump to a heading moves the reader as
// surely as opening a citation does, and one that left no mark would make
// Backspace step over the section they had been reading and out of the document
// entirely.
static void Push() {
    doc::FHist &hist = doc::hist_Alloc();
    hist.page = doc::_db.page;
    hist.top = doc::_db.top;
    hist.sel = doc::_db.sel;
}

// Lay the location the reader is on out again, with the query flags as they now
// stand, and leave them looking at the row they were looking at.
//
// A toggle asks acr a different question about the same records, so the answer
// is a different page of different length.  Keeping the row rather than the
// selection is what makes t and l feel like a change of view: the records the
// reader was reading are still under the cursor, with their closure now around
// them.
static void Reload() {
    int top = doc::_db.top;
    doc::RenderPage(doc::_db.page);
    ScrollTo(top);
    SelectVisible();
}

static void Enter(algo::strptr path) {
    doc::RenderPage(path);
    doc::_db.top = 0;
    SelectVisible();
}

// Return to the document the reader came from, at the row and the link they
// left it on.  Going back from the first document does nothing, rather than
// leaving the reader on a blank page.
static void Back() {
    doc::FHist *hist = doc::hist_N() > 0 ? &doc::hist_qFind(doc::hist_N() - 1) : NULL;
    if (hist) {
        tempstr page(hist->page);
        int top = hist->top;
        int sel = hist->sel;
        bool same = page == algo::strptr(doc::_db.page);
        doc::hist_RemoveLast();
        if (!same) {
            doc::RenderPage(page);
        }
        doc::_db.top = top;
        doc::_db.sel = sel;
    }
}

// Follow the selected link, when it is one that names a file.
// Scroll to the heading NAME anchors, and return whether the page carried one.
static bool JumpAnchor(algo::strptr name) {
    int row = doc::AnchorRow(name);
    if (row >= 0) {
        ScrollTo(row);
        doc::_db.sel = 0;
        SelectVisible();
    }
    return row >= 0;
}

// Select the link whose target is TARGET, and bring the page onto it.
//
// A fold heading is opened by following it, and the page laid out afterwards carries that
// same heading with that same target on it.  Landing the reader back there is what lets a
// second Enter close what the first one opened, which is what the triangle promises.
//
// The pane names the section by that same link, and its entry is the earlier of the two on
// the page, so the target alone does not say which one was meant.  The heading is the link
// standing on the row its own anchor names, and that is the one to land on: a reader who
// opened a section wants to be looking at it, and a first-match search leaves them at the
// top of the page with the section they just opened below the screen.
static void SelectTarget(algo::strptr target) {
    int row = doc::AnchorRow(Pathcomp(target, "#LR"));
    int want = 0;
    int first = 0;
    int i = 0;
    ind_beg(doc::_db_link_curs, link, doc::_db) {
        i++;
        bool same = link.target == target;
        first = first == 0 && same ? i : first;
        want = want == 0 && same && link.row == row ? i : want;
    }ind_end;
    want = want > 0 ? want : first;
    if (want > 0) {
        doc::_db.sel = want;
        ScrollOnto(doc::link_qFind(want - 1).row);
    }
}

// Open whatever the selected link points at.
//
// A target has two halves, either of which may be missing: a path, and an
// anchor naming a heading inside it.  A table of contents entry is all anchor,
// and following it scrolls this page.  A bare path opens that document at its
// top.  A path with an anchor opens the document and then scrolls it, which is
// why the jump happens after Enter rather than before.
//
// An anchor the page does not carry falls back to searching for its text.  The
// documents here also anchor by hand, without abt_md's marker, and a search is
// the only thing left that can find one of those.
//
// The location is pushed once, by whichever of the two halves is going to move
// the reader, so one Enter costs one Backspace however much work it did.
//
// The target is copied out of the link before anything else happens, because
// opening the document it names lays out a new page and a new page has new links.
// A strptr into the link record would then point into freed memory, and the anchor
// read out of it afterwards would be whatever the renderer had since written
// there -- which is a jump to a row nobody asked for, or no jump at all.
static void Follow() {
    doc::FLink *link = doc::_db.sel > 0 ? doc::link_Find(doc::_db.sel - 1) : NULL;
    tempstr target(link ? algo::strptr(link->target) : algo::strptr());
    bool fold = doc::Foldflip(Pathcomp(target, "#LL"));
    tempstr next(fold ? tempstr() : doc::ResolveTarget(doc::_db.page, target));
    tempstr name(Pathcomp(target, "#LR"));
    bool named = link && !fold && name != target && ch_N(name) > 0;
    bool open = ch_N(next) > 0 && (doc::QueryQ(next) || algo::FileQ(next));
    bool here = named && ch_N(next) == 0;
    if (fold) {
        Reload();
        SelectTarget(target);
    }
    if (open || here) {
        Push();
    }
    if (open) {
        Enter(next);
    }
    if (named && (open || here)) {
        bool jumped = JumpAnchor(name);
        if (!jumped) {
            doc::_db.find = name;
            doc::_db.hitrow = 0;
            doc::_db.hitcol = -1;
            NextHit(1);
            doc::_db.find = "";
        }
    }
}

// Return true when a byte is already waiting to be read, allowing MSEC for it
// to arrive.
//
// This is what tells a bare Esc from the first byte of an arrow key.  A
// terminal sends the bytes of a real escape sequence in one go, so a wait that
// finds nothing means the reader pressed Esc itself.  Without the test, the
// read of the rest of the sequence blocks, and Esc appears to do nothing at all
// until two further keys are typed.
static bool PendingQ(int msec) {
    struct pollfd pfd;
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    pfd.revents = 0;
    return poll(&pfd, 1, msec) > 0;
}

// Read the tail of a control sequence, the escape and the bracket that opened it
// already gone, and return the key it names or a bare Esc when it names none.
static int ReadCsi() {
    char code = 0;
    int got = read(STDIN_FILENO, &code, 1);
    int ret = 27;
    if (got == 1 && code == 'A') {
        ret = doc_Key_up;
    } else if (got == 1 && code == 'B') {
        ret = doc_Key_down;
    } else if (got == 1 && code == 'C') {
        ret = doc_Key_right;
    } else if (got == 1 && code == 'D') {
        ret = doc_Key_left;
    } else if (got == 1 && (code == '5' || code == '6')) {
        char tilde = 0;
        int eaten = read(STDIN_FILENO, &tilde, 1);
        ret = eaten == 1 ? (code == '5' ? doc_Key_pgup : doc_Key_pgdn) : doc_Key_esc;
    } else if (got == 1 && code == 'H') {
        ret = doc_Key_home;
    } else if (got == 1 && code == 'F') {
        ret = doc_Key_end;
    }
    return ret;
}

// Read one keystroke, decoding what a terminal sends for the arrow and page keys
// and for a meta key into the single values the rest of this file uses.
//
// An escape that begins no recognized sequence is returned as itself, which is
// how a bare Esc goes back a document and cancels a search.  Telling that from
// the escape that opens a sequence is PendingQ's job.
//
// What follows an escape is read one byte at a time.  A terminal sends an arrow
// as escape, bracket, letter, and it sends M-v as escape, v -- so a read of two
// bytes that assumed the bracket would swallow the letter of every meta key and
// hand back a bare Esc, having eaten the keystroke that said what the reader
// meant.
static int ReadKey() {
    char ch = 0;
    int n = read(STDIN_FILENO, &ch, 1);
    int ret = n == 1 ? u8(ch) : 'q';
    if (ret == doc_Key_esc && PendingQ(50)) {
        char lead = 0;
        int got = read(STDIN_FILENO, &lead, 1);
        ret = got == 1 && lead == 'v' ? doc_Key_pgup : ret;
        if (got == 1 && lead == '[') {
            ret = ReadCsi();
        }
    }
    return ret;
}

// Take one keystroke while the search prompt is open.
//
// The search is incremental: every character typed re-runs the match and the
// page jumps to the first row carrying it, with every hit on screen highlighted
// by the painter.  Enter leaves the prompt and keeps the highlight, so that the
// next `.` continues from where the reader stopped; Esc leaves and drops it.
static void SearchKey(int key) {
    if (key == doc_Key_right) {
        NextHit(1);
    } else if (key == doc_Key_left) {
        NextHit(-1);
    } else if (key == doc_Key_up) {
        ScrollTo(doc::_db.top - 1);
    } else if (key == doc_Key_down) {
        ScrollTo(doc::_db.top + 1);
    } else if (key == doc_Key_esc) {
        doc::_db.find = "";
        doc::_db.searching = false;
    } else if (key == doc_Key_cr || key == doc_Key_lf) {
        doc::_db.searching = false;
    } else if ((key == doc_Key_del || key == doc_Key_bs) && ch_N(doc::_db.find) > 0) {
        doc::_db.find.ch_n--;
        FirstHit();
    } else if (key >= ' ' && key < 127) {
        doc::_db.find << char(key);
        FirstHit();
    }
}

// Return the page of keys, as markdown.
//
// It is a document this tool writes for itself rather than one it reads, and it
// reaches both devices through the same parser: the terminal lays it out with the
// renderer every other document gets, and the browser turns its tables into HTML
// tables.  Written twice instead -- once per device -- the two would drift, and a
// key documented in one place would be missing from the other.
//
// It lives beside PageKey, which is what decides what a keystroke does, so a key
// added to the reader and not added here shows up as one half of a diff.  A table
// kept in the README instead goes quietly out of date, and nothing but a reader's
// surprise says so.
tempstr doc::HelpText() {
    tempstr ret;
    ret << "# doc -- the keys" << eol << eol;
    ret << "## Moving" << eol << eol;
    ret << "| key | effect |" << eol << "|---|---|" << eol;
    ret << "| `j` `k` | scroll a line |" << eol;
    ret << "| `Space` `b` `PgDn` `PgUp` | scroll a screen |" << eol;
    ret << "| `Ctrl-F` `Ctrl-B` `Ctrl-V` `Meta-V` `Ctrl-N` `Ctrl-P` | scroll a screen |" << eol;
    ret << "| `g` `^` `Home` | first row |" << eol;
    ret << "| `G` `End` | last row |" << eol;
    ret << "| `Ctrl-L` | put the selection halfway down the screen, and repaint it |" << eol << eol;
    ret << "## Following a link" << eol << eol;
    ret << "| key | effect |" << eol << "|---|---|" << eol;
    ret << "| `n` `p` `Right` `Left` | select the next or previous link |" << eol;
    ret << "| `Up` `Down` | select the link on the row above or below |" << eol;
    ret << "| `Enter` | follow the selected link |" << eol;
    ret << "| `Backspace` `Esc` `u` | back to where you came from |" << eol << eol;
    ret << "## Searching" << eol << eol;
    ret << "| key | effect |" << eol << "|---|---|" << eol;
    ret << "| `/` | search, with the page following what you type |" << eol;
    ret << "| `Right` `Left` | step between the hits, selecting a link a hit falls in |" << eol;
    ret << "| `Enter` | accept the search and keep its highlight |" << eol;
    ret << "| `Esc` | drop the search; with none showing, go back a document |" << eol;
    ret << "| `.` `,` | step the hits again once the search is accepted |" << eol << eol;
    ret << "## On a page of records" << eol << eol;
    ret << "Each of these is an option of the command line the page carries, and pressing"
        << " it puts that option on the line or takes it off." << eol << eol;
    ret << "| key | effect |" << eol << "|---|---|" << eol;
    ret << "| `m` | the schema of the selection rather than its records (`-meta`) |" << eol;
    ret << "| `v` | the access paths of the ctypes the page is about (`-vis`) |" << eol;
    ret << "| `c` | what `amc` generated for those ctypes, and their functions (`-code`) |" << eol << eol;
    ret << "A section that opens and closes is a link on its own heading, and following"
        << " that heading puts the same option on the line.  The rows of a table are"
        << " reached that way and no other, since `-data` has no key." << eol << eol;
    ret << "## And" << eol << eol;
    ret << "| key | effect |" << eol << "|---|---|" << eol;
    ret << "| `e` | open what is on the page in an editor |" << eol;
    ret << "| `?` | this page |" << eol;
    ret << "| `q` | quit |" << eol;
    return ret;
}

// Lay the page of keys onto the page.
void doc::RenderHelp() {
    doc::ParseText(doc::HelpText());
    doc::RenderDoc();
}

// Take one keystroke while the page is being read.
//
// A screen moves under five spellings, because a reader arrives with the habits
// of whichever pager taught them: space and b as less has them, Ctrl-F and
// Ctrl-B likewise, Ctrl-V and Meta-V for the emacs hand along with Ctrl-N and
// Ctrl-P, and the Page keys for everyone else.  The file's two ends are g and G, Home and End, and ^ for the
// top.  A line at a time is j and k.  Ctrl-L puts the selection halfway down the
// screen and paints the whole of it again.
//
// The four arrows move the selection rather than the page, and the page follows
// wherever the selection lands.  Left and right step the links in the order they
// were written; up and down step rows, landing on the link nearest the column
// just left.  That split is what makes a page of records navigable: a record is
// a row of links, so reaching the row below in document order would mean walking
// through every attribute of the row you are on.
//
// The keys are documented on a page of their own, which ? opens.  The status line
// therefore names only that one key rather than reciting the rest, which leaves
// its width to the location the reader is standing on.
//
// Esc unwinds one thing at a time.  A search that has been accepted leaves its
// highlight on the page, and the first Esc clears that; only once nothing is
// highlighted does Esc go back a document.  Pressing it once to be rid of a
// search and finding oneself in a different document instead is the behaviour
// this ordering exists to prevent.
//
// Whatever the key did to the page, the selection is brought back onto the
// screen afterwards, in the direction the page moved.  That is done here, once,
// rather than in each branch that scrolls, so no way of moving the page can
// leave the reader able to press Enter on a link they cannot see.
//
// A page showing one function carries a colon and is not a query: it selects no records,
// so x, t, l, m, v and c have nothing to ask about and are left alone.
//
// Each of those keys is a section or a question about the same selection rather than a
// page of its own, so pressing one reloads where the reader is standing instead of
// navigating.  That is why none of them pushes onto the history: a reader who opened the
// code and then followed a link expects Backspace to bring back the page with the code
// still open, not to close it.
static void PageKey(int key) {
    int top = doc::_db.top;
    bool query = doc::QueryQ(doc::_db.page) && !doc::FuncsrcQ(doc::_db.page);
    bool toggle = query && (key == 'm' || key == 'v' || key == 'c');
    bool help = key == '?' && !doc::HelpQ(doc::_db.page);
    if (help) {
        Push();
        Enter("help");
    } else if (toggle) {
        doc::_db.meta = key == 'm' ? !doc::_db.meta : doc::_db.meta;
        doc::_db.vis = key == 'v' ? !doc::_db.vis : doc::_db.vis;
        doc::_db.code = key == 'c' ? !doc::_db.code : doc::_db.code;
        Reload();
    } else if (key == 'q') {
        doc::_db.quit = true;
    } else if (key == 'j') {
        ScrollTo(doc::_db.top + 1);
    } else if (key == 'k') {
        ScrollTo(doc::_db.top - 1);
    } else if (key == doc_Key_down) {
        SelectRow(1);
    } else if (key == doc_Key_up) {
        SelectRow(-1);
    } else if (key == ' ' || key == doc_Key_ctrlf || key == doc_Key_ctrln || key == doc_Key_ctrlv || key == doc_Key_pgdn) {
        ScrollTo(doc::_db.top + PageRows());
    } else if (key == 'b' || key == doc_Key_ctrlb || key == doc_Key_ctrlp || key == doc_Key_pgup) {
        ScrollTo(doc::_db.top - PageRows());
    } else if (key == doc_Key_ctrll) {
        Recenter();
    } else if (key == 'g' || key == '^' || key == doc_Key_home) {
        ScrollTo(0);
    } else if (key == 'G' || key == doc_Key_end) {
        ScrollTo(i32(doc::row_N()));
    } else if (key == 'n' || key == doc_Key_right) {
        SelectLink(1);
    } else if (key == 'p' || key == doc_Key_left) {
        SelectLink(-1);
    } else if (key == doc_Key_cr || key == doc_Key_lf) {
        Follow();
    } else if (key == doc_Key_esc && ch_N(doc::_db.find) > 0) {
        doc::_db.find = "";
        doc::_db.hitcol = -1;
    } else if (key == doc_Key_del || key == doc_Key_bs || key == doc_Key_esc || key == 'u') {
        Back();
    } else if (key == '/') {
        doc::_db.searching = true;
        doc::_db.find = "";
        doc::_db.hitrow = doc::_db.top;
        doc::_db.hitcol = -1;
    } else if (key == '.') {
        NextHit(1);
    } else if (key == ',') {
        NextHit(-1);
    }
    SelectShowing(doc::_db.top - top);
}

// Open what is on the page for editing, and return once the editor has left.
//
// A document is handed to the editor by name.  A query is handed to acr instead,
// with -e added to the line the page was answered by: acr writes the records it
// selected into a file, runs the editor on that, and puts back whatever comes
// out -- so editing a record from the reader edits the ssimfile the record lives
// in, in the form the page is showing it.
//
// The editor is named by VISUAL, then by EDITOR, and vi is what is left when
// neither says anything.  A failure is not fatal here: an editor that will not
// start leaves the page as it was, which is better than losing the reader's
// place to it.
static void EditPage() {
    tempstr cmd;
    if (doc::QueryQ(doc::_db.page)) {
        cmd << doc::AcrCmdline(doc::LocQuery(doc::_db.page), true);
    } else {
        tempstr editor(getenv("VISUAL") ? strptr(getenv("VISUAL"))
                       : getenv("EDITOR") ? strptr(getenv("EDITOR")) : strptr("vi"));
        cmd << editor << " " << strptr_ToBash(doc::_db.page);
    }
    SysCmd(cmd, FailokQ(true));
}

// Read the page on screen until the reader quits.
//
// The terminal is put back to the way it was found on every path out of here,
// including the one where setting it up failed in the first place, because a
// terminal left in raw mode with no cursor is a shell the user has to reset by
// hand.
//
// It is given back on the way through, too.  An editor expects to own the screen
// it starts on and cannot be handed one already in the alternate screen with the
// cursor hidden and echo off, so e leaves raw mode, runs the editor, comes back,
// and lays the page out again over whatever the editing changed.
void doc::Browse() {
    struct termios saved;
    ReadSize();
    doc::RenderPage(doc::_db.page);
    SelectVisible();
    RawBegin(saved);
    while (!doc::_db.quit) {
        ReadSize();
        Draw();
        int key = ReadKey();
        bool edit = key == 'e' && !doc::_db.searching;
        if (edit) {
            RawEnd(saved);
            EditPage();
            RawBegin(saved);
            Reload();
        } else if (doc::_db.searching) {
            SearchKey(key);
        } else {
            PageKey(key);
        }
    }
    RawEnd(saved);
}
