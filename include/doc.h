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
// Header: include/doc.h
//

#include "include/gen/doc_gen.h"
#include "include/gen/doc_gen.inl.h"

namespace doc { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/doc/acr.cpp -- Rendering an acr query as a page
    //

    // Return true when LOC is answered from the ssim database rather than from a
    // document of the tree.
    //
    // A location the reader can be taken to is one of two things, and a colon is
    // what tells them apart.  A query is written ssimfile:key -- the shape
    // dmmeta.ctype:dmmeta.Ctype has -- while no document in this tree carries a
    // colon in its path.  A web address carries one too, and NewLink refuses those
    // before they can become a location at all.
    bool QueryQ(algo::strptr loc);

    // Return the word LOC opens with when doc synthesizes a page for that word, and the
    // empty string when it does not.
    //
    // A location carrying a colon is answered from the ssim database, and the word before
    // that colon says which answer.  A table's full name is a namespace and a name with a
    // dot between them, so a word with no dot in it is not a table, and that is the whole
    // of the discrimination: func:dmmeta.ctype:lib_x2.FStream is the page doc builds about
    // those records, while dmmeta.func:x is the row acr answers with.
    //
    // The vocabulary is closed and every word in it has a page.  It is dev.docpage that says
    // which words those are, so a page is a row and a function and nothing else -- a list
    // repeated here would be a second place to edit, and the two would part company on the
    // day somebody added a page and read only one of them.  A bare word doc does not know is
    // a table's short name instead, which ExpandQuery turns into the full one, so nothing a
    // reader can type is left without an answer.
    tempstr ViewWord(algo::strptr loc);

    // Return the query the location LOC means when its word was handed a pattern, and the
    // empty string when it was not.
    //
    // A word of the vocabulary answers about one thing: `ns:acr` is one namespace and the page
    // doc writes about it, gathering that namespace's ctypes and sources and tests.  A pattern
    // names a set instead, and there is no such page for a set -- what a reader who typed
    // `ns:%` wants is every namespace, which is the rows of `dmmeta.ns` and an ordinary page of
    // records.  Without this a pattern is looked up as though it were a name, and `ns:%`
    // answers that the tree holds no namespace called `%`.
    //
    // Which words those are is `bytable` on their `dev.docpage` rows, so the vocabulary states
    // its own rule.  A word's name is its table's short name where it has one, which is what
    // `ExpandQuery` already knows, so the row says only whether that expansion is what a
    // pattern means -- and it is not always: `func` expands to `dmmeta.func`, a table that has
    // nothing to do with the functions `func:` is about.  A word with the mark off keeps its
    // page whatever it is handed, which is what leaves `accesspath:acr.%` taking the regx it
    // always took.
    //
    // A word that is a location on its own already answers about all of them, and `msg` is
    // one: the bare word is every message header in the tree.  So `msg:%` is that word, and
    // the summary has one name rather than two.
    tempstr Wordpattern(algo::strptr loc);

    // Return QUERY with a table's short name expanded to the full one, or QUERY as it
    // stands when no single table answers to that name.
    //
    // acr takes a table's short name as well as its full one, so `acr ssimfile:dmmeta.ctype`
    // answers.  A page has one canonical text, though, and two spellings of one location
    // would give a reader two histories of the same walk -- so the short name is expanded
    // here, and every location that reaches a link, a history or a URL carries the full one.
    //
    // Ten short names in the tree name two tables each, gitfile and target and user among
    // them, and one of those is left exactly as the reader wrote it.  Choosing between two
    // tables is acr's business rather than this tool's.
    tempstr ExpandQuery(algo::strptr query);
    tempstr Recfunc(algo::strptr loc);

    // Return the transcript the page at LOC is about, and the empty string when the page is
    // about no single comptest.
    //
    // A comptest is a row of atfdb.comptest and its transcript is the file it is checked
    // against, so a page selecting that one row is a page a reader wants the transcript on --
    // whichever spelling they reached it by.  Reading it off the records rather than off the
    // word in the location is what makes `comptest:doc.Fold` and `atfdb.comptest:doc.Fold` one
    // page, and it is how the function section beside it already works.
    //
    // A selection matching a pattern answers with the empty string.  A page of forty tests is
    // about forty transcripts, and showing one of them would be picking a row out of the page
    // for no reason the reader gave.
    tempstr Rectranscript(algo::strptr loc);

    // Return the path of the transcript the comptest COMPTEST is checked against.
    tempstr ComptestFile(algo::strptr comptest);

    // Return how many lines TEXT holds.
    int NLine(algo::strptr text);

    // Return the byte offset just past the word at POS in LINE, where a word is what a name in
    // this tree is made of: letters, digits, underscores and the dot between a namespace and a
    // type.
    int WordEnd(algo::strptr line, int pos);

    // Return the location the text of a code span names, or the empty string when it names
    // nothing this tool can open.
    //
    // A word in backticks is the name of something more often than not, and in this tree those
    // names are recognizable: `dmmeta.thash` is a table, `lib_x2.FStream` is a ctype, `amc` is a
    // namespace, and `dmmeta.ctype:doc.FSpan` is a selection of records.  A table leads to its
    // rows rather than to the page about it: a reader who wrote a table's name in a sentence is
    // pointing at what is in it.  Each becomes a link
    // wherever it is written, so a document carries them without an author spelling out a target
    // -- which is most of what makes a reference page worth reading through this tool rather
    // than reading as a file.
    //
    // A directory of the documentation is the fifth: `txt/rule` is where the rule files are,
    // and a sentence that names it leads there.  It is tried last, so a name that is also a
    // table or a namespace stays what it was.
    //
    // Everything else in backticks names nothing here and stays plain text: a command line, a
    // C++ expression, an option, a file, a field of a struct.  Three tests are the whole of
    // what keeps that true.  A span carrying a space is prose or a command line rather than a
    // name.  A span with no colon has to be a table, a ctype or a namespace outright, and a
    // ctype carries its namespace -- which is what keeps `u32`, a ctype and a word in half the
    // sentences of this tree, from linking to the page that says it is builtin.  A span with a colon has to
    // name a table or one of doc's own words before it, which is what leaves `arg:u32` and
    // `-color:N` alone.
    //
    // Where a word of the vocabulary and a table answer to the same name, the word wins, as it
    // does in a location a reader types.  `msg` is both -- doc's page about the protocols and
    // the short name of `dmmeta.msg` -- and a span expanded to the table led to the row rather
    // than to the page the same text opens at the command line.
    tempstr Codeloc(algo::strptr text);

    // Record every directory the documentation has a document in, so that a code span naming
    // one can be recognized as naming it.
    //
    // The list is the directories of dev.readmefile with the file taken off, each kept once,
    // and it is built after the inputs are read because that is when the rows are all there.
    // Reading it off that table rather than off the tree is the same rule the listings follow:
    // what the reader is offered is what git tracks.
    void ReadmedirRead();

    // Return true when LOC names the function view of a query: the C++ functions the
    // query's records are about, rather than the records themselves.
    //
    // Everything that carries a location therefore carries a view for free: the history
    // pushes and pops one, so Backspace leaves the function view, and -links reaches its
    // links without a terminal.
    bool FuncQ(algo::strptr loc);

    // Return the file LOC names when LOC names a function inside a particular file, and
    // the empty string otherwise.
    //
    // A source file and a function name with a colon between them is how a reader asks for
    // one function of a file they are already reading, and it is the spelling a link out of
    // that file's own listing carries.  The test is that the text before the colon is a
    // file of the tree, which no query and no other location can be.
    tempstr FuncsrcSrc(algo::strptr loc);

    // Return the function LOC names, and the empty string when LOC names none.
    //
    // `func:` addresses functions, and what follows it says which.  A selection of records
    // carries a colon; a ctype's name is a ctype this tool has a record of; and what is
    // neither is a function's own key.  So `func:dmmeta.ctype:dmmeta.Ctype` and
    // `func:dmmeta.Ctype` are both every function about that ctype, while
    // `func:doc.HtmlNav` is that one function's source.  One word serves all three because
    // all three are functions, and a file and a name is a second spelling of the last.
    //
    // Two keys name a set however singular they look, and both have to be refused here.  A
    // key carrying `%` is a pattern, and `%` means the same thing in this tool that it means
    // everywhere else in the tree.  A key that is a namespace outright names that
    // namespace's functions.  Without those two tests `algo_lib.%` is read as one function's
    // key, and since a key is looked up by running src_func and taking the first row it
    // answered, the page shows one arbitrary function out of the 2,331 that matched.
    tempstr FuncsrcName(algo::strptr loc);

    // Return true when KEY is an amc key naming one function.
    //
    // amc keys a ctype by a namespace and a name, a field by a ctype and a name, and a
    // function by a field and a verb -- so the count of dots is what says which of the three
    // a key is, and three of them is a function.  `doc.FDb.anchor.Alloc` is one, and so is
    // `doc.FAnchor..Init`, whose empty middle is the ctype's own constructor.
    bool AmckeyQ(algo::strptr key);

    // Return the amc key the marker line LINE names, and the empty string when LINE is not a
    // marker.
    //
    // amc opens the comment above each thing it generates with `// --- <key>`, so the marker
    // is what says which key a chunk of generated code came from -- and a page showing that
    // code has the key on it already, needing only to be made a link.
    tempstr Funckey(algo::strptr line);

    // Return the location the amc key KEY leads to, and the empty string when it leads
    // nowhere.
    //
    // A key of three dots is a function and a key of one is a ctype; amc writes markers for
    // neither a field nor an enum name, so those two are the whole of it.
    tempstr Funckeyloc(algo::strptr key);

    // Return true when LOC names the source of one function.
    bool FuncsrcQ(algo::strptr loc);

    // Return true when LOC is a `func:` whose argument is a regex over function names.
    //
    // A function's name is arbitrary.  It may belong to a ctype, and then amc keys it by that
    // ctype and a verb; it may be global, and then the namespace is all it shares with its
    // neighbours.  So neither a ctype's name nor a namespace's constrains what a reader can
    // ask for, and an argument carrying `%` is handed to src_func as it stands rather than
    // resolved to a record first.  `algo.Smallstr100%` is the case that says so: it is a
    // ctype's name and a prefix of thirty-one function keys, and resolving it either way --
    // to the ctype, or to the namespace its first component names -- answers a question the
    // reader did not ask.
    //
    // A colon means the argument is a selection of records instead, and those name their own
    // subjects.
    bool FuncregxQ(algo::strptr loc);

    // Return true when LOC names the generated-code view of a query: what amc emitted
    // for the ctypes the query's records are about.
    bool CodeQ(algo::strptr loc);

    // Return true when LOC names one ctype's own page.
    bool CtypeQ(algo::strptr loc);

    // Return the acr query that selects the ctype NAME.
    tempstr CtypeQuery(algo::strptr name);

    // Return the ctype page QUERY has one of, and the empty string when it selects
    // anything but a single ctype.
    //
    // The entry exists on a records page so that a reader looking at one ctype's row can
    // reach the page about that ctype, and it is absent from a page selecting many because
    // there is no one ctype for it to lead to.
    tempstr CtypeLoc(algo::strptr query);

    // Return true when the page at LOC selects records from the ssim database.
    //
    // A document resolves to itself through LocQuery the way a query does, so the query
    // alone does not say which of the two a location is; both tests together do.  Whatever
    // selects records can be asked for its schema and drawn as access paths, and whatever
    // does not must not carry those flags in its URL.
    bool RecordsQ(algo::strptr loc);

    // Return the namespace whose ctypes the sections of the page at LOC are about, and the
    // empty string when the page is about no whole namespace.
    //
    // `ns:<name>` is the one page that is.  What makes the code section of such a page the
    // namespace's own functions rather than one type's accessors is that the subject read off
    // it is a namespace.
    //
    // A namespace's README is not one of them.  It is the writing about the namespace, and
    // what a reader opens it for is that writing; the machinery is what the internals link at
    // its foot leads to, and putting the same two sections under the prose would answer a
    // question nobody asked on the page they asked it on.
    tempstr Sectionns(algo::strptr loc);

    // Return true when the page at LOC carries the two sections that open under a triangle:
    // the access paths of what it is about, and the code.
    //
    // A ctype's page and a namespace's page carry them, and nothing else does.  Both are about
    // ctypes, which is what amc_vis draws and what amc generated code for; a page of arbitrary
    // records is about whatever was typed, and asking those two questions of it answers with a
    // complaint as often as with anything.
    //
    // A view of a query -- the functions, the generated code, one function's source -- is
    // already one of those sections, on a page of its own so that a link can name it, and
    // offering the same section under a triangle beneath it would be the same thing twice.
    bool SectionQ(algo::strptr loc);

    // Return true when the page at LOC is a page of records: acr answered it, so the pane
    // maps its rows and whoever laid the records out is who lays the sections under them.
    //
    // A namespace's README carries the same sections and is not one of these: it is a
    // document, its pane is the documents of its directory, and its sections go under the
    // text somebody wrote rather than under a table acr produced.
    bool RecpageQ(algo::strptr loc);

    // Return true when the page at LOC carries any section that opens under a heading of its
    // own: the access paths and the code, the rows of a table, the function behind a record.
    bool FoldQ(algo::strptr loc);

    // Return the table whose rows the data section of the page at LOC shows, and the empty
    // string when the page is about no single table.
    //
    // A page selecting one row of dmmeta.ssimfile is a page about a table, and what a reader
    // looking at it wants next is what is in that table -- so the rows are a section of that
    // page rather than another location to walk to.  The section is closed until asked for,
    // since a table of ten thousand rows is not what somebody reading the declaration asked
    // for.
    tempstr Datassimfile(algo::strptr loc);

    // Return the selection the sections of the page at LOC are about.
    //
    // A page of records is about what it selected.  A namespace's page is about the namespace,
    // which is the one record of dmmeta.ns naming it -- and that is what makes the code section
    // of such a page the namespace's own functions rather than one type's accessors, since the
    // subject read off that record is a namespace.
    tempstr SectionQuery(algo::strptr loc);

    // Return true when LOC names an access path drawing.
    bool AccesspathQ(algo::strptr loc);

    // Return true when LOC names one namespace's own page.
    bool NsQ(algo::strptr loc);

    // Return true when LOC names one comptest's own page.
    bool ComptestQ(algo::strptr loc);

    // Return true when LOC is the page about one message.
    bool MsgQ(algo::strptr loc);

    // Return true when LOC names the page of keys rather than anything to read.
    //
    // It is a location like any other, so the help arrives by the same Enter that a
    // citation does and leaves by the same Backspace, and `doc help` writes the table
    // out at a pipe.  The word carries no colon, so nothing takes it for a query, and
    // no document of the tree is named help.
    bool HelpQ(algo::strptr loc);

    // Return true when the bare key LOC is the name of a page doc writes.
    //
    // Most pages are addressed by a word and a colon, because they are about something and
    // the argument says what.  A page that is about the whole tree has nothing to put after
    // the colon: the keys the reader has, and the roots of every protocol.  So the word
    // alone is the location, and dev.docpage is what says which words those are.
    bool BarepageQ(algo::strptr loc);

    // Return the acr query LOC selects its records with, and the empty string when LOC
    // selects no records at all.
    //
    // A view of a query carries that query after its word.  A ctype page carries a ctype
    // name instead, and the records it is about are that ctype's row and everything hanging
    // off it, so the word expands to the table it is the short name of.  A page showing one
    // function and a page showing a drawing select nothing, and the empty string is what
    // says so -- their locations carry a colon, so nothing else would.
    tempstr LocQuery(algo::strptr loc);

    // Return the table QUERY selects the whole of, and the empty string when it selects
    // anything less.
    //
    // `dmmeta.ns:%` is the whole of that table and `dmmeta.ns:dmmeta` is one row of it, which
    // is a difference the closure makes enormous: the closure of a record is its neighbours,
    // and the closure of every record is the database.  Asking acr for `dmmeta.ns:%` with
    // -xref answers with 53,071 records, where the table has 199.
    tempstr Wholetable(algo::strptr query);

    // Return true when the query QUERY is answered with its records and nothing around them.
    //
    // How much of a record to fetch is not a decision the reader makes, and this is the whole
    // of the rule that settles it.  A selection naming one record is fetched as its
    // cross-reference closure, descended through pkeys and drawn as a tree: a record read
    // alone says little, since the row for one field names its ctype and its arg, and the
    // closure is what puts those on the page beside it.  The descent is held to pkeys because
    // the full closure of a common ctype reaches every row that mentions it and runs to
    // several hundred, and the tree is what says which row hangs off which.
    //
    // A selection naming a set is fetched flat, and `%` is what says it names one.  The
    // closure of one record is its neighbours and the closure of a set is the database:
    // `acr dmmeta.ns:% -xref` answers with 53,071 records where the table holds 199, and even
    // the five rows of `dmmeta.ns:abt%` reach 629 that way.  A set of records read together is
    // read for what they have in common, which is the columns, and a closure is what buries
    // those.
    bool FlatQ(algo::strptr query);

    // Return the flags acr answers the query QUERY under, spelled the way acr takes them.
    //
    // They follow from the selection rather than from anything a reader set, so this is the
    // line the page ran and the line a reader can paste into a shell.
    tempstr AcrFlag(algo::strptr query);

    // Return the options the page at LOC carries: the sections it has open, and the tab.
    //
    // doc takes every one of these under the name it is printed by, and that is the whole
    // reason the flags are spelled acr's way here rather than shortened: the text of the box
    // is a command line, and a reader who copies it out of the box has to be able to run it.
    // What acr was asked is not among them -- the closure follows from the selection, so it
    // is a fact about the query rather than a decision the reader made.
    //
    // A location carries only the flags it honours, and each of the three tests here is the
    // one the emitter itself branches on: the tabs belong to a page acr answered, the two
    // folds to a ctype's page and a namespace's page, and the data fold to a page about one
    // table.  Testing them any other way is how `ns:abt` came to draw an Access Paths fold
    // whose link carried no `-vis`, so following it redrew the page with the fold still shut.
    tempstr PageFlag(algo::strptr loc);

    // Return the canonical text of the page showing LOC: the flags in force, then the
    // location.
    //
    // Every page has one text that says what it is, and that text says the whole of it --
    // which is what lets a reader see the state rather than infer it, and lets them type
    // it back and arrive at the same page.  The flags come first because what follows
    // them is the thing being asked about, and they are spelled the way acr takes them,
    // since that is the spelling a reader of this tree already knows.
    //
    // It is the same text on both devices: the reader's status line and the browser's
    // search box show it, so a page has one name and not one per screen it appears on.
    tempstr PageText(algo::strptr loc);

    // Take the flags the canonical text TEXT spells and return the location that follows
    // them.
    //
    // The text lists exactly what is in force, so a flag absent from it is off.  acr's own
    // `-meta:N` spelling is accepted as well, because a reader who knows acr will reach for
    // it, and it says the same thing as leaving the flag out.  A leading word this tool does
    // not take is stepped over rather than read as part of the location, so a line carrying an
    // option doc has no use for still arrives at the page it names.
    tempstr ReadText(algo::strptr text);

    // Return the acr command line that answers QUERY in the shape its selection asks for,
    // which is what `FlatQ` decides.  With EDIT the line carries -e, which is what asks acr to
    // put the records it selected into an editor and write back whatever comes out.
    tempstr AcrCmdline(algo::strptr query, bool edit = false);

    // Return the byte offset just past the token of LINE that begins at POS.
    //
    // A token runs to the next space, except that acr quotes a value carrying
    // spaces: comment:"File with ssim tuples" is one attribute and not three.  So a
    // quote suspends the rule until its partner arrives, and both quote characters
    // occur -- a comment comes back in double quotes, a c++ default such as
    // dflt:'"data"' in single ones.
    int TokenEnd(algo::strptr line, int pos);

    // Return TEXT without the pair of quotes acr wrapped it in, or TEXT itself when
    // it carries none.  What a query needs is the value, and the quotes belong to
    // the printed form rather than to it.
    algo::strptr Unquote(algo::strptr text);

    // Return the query that retrieves what the attribute NAME:VALUE of a record of
    // SSIMFILE points at, or the empty string when the attribute points at nothing.
    //
    // An attribute names a field of the record's ctype, and that field's arg names
    // a second ctype.  When the second ctype has a table of its own, the value is a
    // key into it, so the table and the value together are the query that retrieves
    // what the attribute refers to.  An attribute whose arg is a plain scalar -- a
    // comment, a boolean, a length -- refers to no record, and gets no query.
    // Return the location that shows the record VALUE of the table SSIMFILE.
    //
    // A ctype has a page of its own -- its declaration, the C++ that acts on it, and its
    // access paths -- so an attribute naming one leads there rather than to the bare row.
    // The two are the same selection: `ctype` is the short name of `dmmeta.ctype`, and the
    // page is what doc makes of it.
    tempstr TableLoc(algo::strptr ssimfile, algo::strptr value);

    // Return the query the attribute TEXT of a record of SSIMFILE leads to, or the
    // empty string when it leads nowhere.  PKEY says TEXT is the record's own primary
    // key, which is the first attribute of its line.
    //
    // Two attributes can be followed and they lead to different places.  One refers to
    // another record, and AttrQuery says which; standing on `ns:dmmeta` of a ctype
    // leads to the namespace record.  The other is the record's own key, whose arg is a
    // plain string with no table behind it -- and following that narrows the page to
    // the one record, which is what a reader wants when they are looking at a listing
    // of two hundred.  The first sense wins where a key is also a reference, since a
    // page already showing the record has nothing to narrow to.
    //
    // This is the rule rather than the drawing of it, which is why it is here and not
    // inside the painter: a browser hangs an anchor on the same attribute a terminal
    // underlines, and the two must agree about where it goes.
    tempstr AttrTarget(algo::strptr ssimfile, algo::strptr text, bool pkey);

    // Return the noun NOUN with the count N after it in parentheses.
    //
    // The noun stays singular, which is the rule the whole tree names things by: a word names
    // one element of a set and the same word names the set, so there is no plural to form.
    // Putting the count second follows from that -- what a heading is about is the noun, and
    // how many there happen to be is a fact about this page rather than part of the name.
    tempstr Count(int n, algo::strptr noun);

    // Return the number of records the acr answer ANSWER carries.
    //
    // A record is a line opening with the name of a table, which is also what makes the
    // count right under a closure: every record the closure reached is a line of its own.
    // acr's own report says how many it selected, but it says so at the foot -- and the
    // count belongs at the head, where it tells a reader that an empty page is an empty
    // table rather than a link that went nowhere.
    int NRecord(algo::strptr answer);

    // Return the acr command line that answers with the declaration of the ctype NAME.
    //
    // The flags are the reader's, as they are on any page answered by acr.  A ctype page
    // asks a narrower question than a raw query page -- it selects one row rather than
    // whatever was typed -- but what a reader wants to see of the answer is the same
    // decision either way, and a page that ignored the boxes in the pane would be a page
    // whose canonical text did not describe it.
    tempstr CtypeCmdline(algo::strptr name);

    // Lay the ctype NAME onto the page: what it was declared to be, what amc made of it,
    // what acts on it, and where it sits.
    //
    // A ctype's declaration is its own row together with the field rows beneath it, which
    // is exactly what the pkey closure of that one row selects -- so the page opens with
    // the fields, their types, their defaults and what each is for.  The generated code
    // and the functions follow when their flags are set, in that order: what amc made of
    // the declaration is nearer to it than what hand-written code does with the result.
    // Then where the ctype sits: which tables build their key out of it, which merely
    // mention it, which command lines take it as an option, and what named values its
    // fields carry.  A declaration says what a record holds, and none of that is visible in
    // the rows of the ctype itself.
    void RenderCtype(algo::strptr name);

    // Run acr for QUERY and lay its answer on the page.
    //
    // The page opens with how many records it holds and the command line that produced
    // them.  The count is what keeps an empty table from reading as a broken link, and the
    // command line is there because a key does not say which flags were in force when it
    // was answered -- and because the line can then be pasted into a shell.  What follows is acr's own output, one page row
    // per line of it.  acr's diagnostics come back on the same stream, so a query
    // naming a table that does not exist shows the complaint where the records would
    // have been, rather than an empty page.
    void RenderQuery(algo::strptr query);

    // Lay the records QUERY selects onto the page, with no closure around them.
    //
    // The note above them is the plain command line, so what the box says it ran is what it
    // ran.  A reader looking at a table's rows reaches the ssimfile record this way, and one
    // record is the whole of what they came for.
    void RenderFlat(algo::strptr query);

    // Return everything the command line CMD wrote, its diagnostics with it.
    //
    // Every answer this tool shows comes from running another one -- acr for records, src_func
    // for functions, amc_vis for access paths -- and each of them writes its complaints to
    // the error stream.  A page that shows the complaint where the records would have been is
    // better than a page that shows nothing, so the two streams are joined and a non-zero
    // exit is not an error here.
    //
    // The cap is one size for every caller.  The largest answer any page asks for is a
    // namespace's generated code, and a caller-by-caller guess at a smaller one buys nothing
    // but a page silently cut off.
    tempstr ToolOut(algo::strptr cmd);

    // Return the command line that selects QUERY and nothing around it.
    //
    // The flags in force say how much of the closure the page's own records show, and they are
    // a statement about the query the reader asked.  A box holding some other query answers a
    // different question, so it is asked plainly: what a reader wants of the ssimfile record
    // under a table's rows is that record, not the schema its closure would drag in.
    //
    // The same plain answer is what a section beside the records is computed from.  A record's
    // closure runs to the schema around it -- a field's ctype, its reftype, the namespace, the
    // license -- and asking src_func or amc_vis about all of that is a scan nobody asked for
    // and a page nobody can read.
    tempstr AcrFlatCmdline(algo::strptr query);

    // Run acr for QUERY with no flags at all and return everything it wrote.
    tempstr AcrFlat(algo::strptr query);

    // Run acr for QUERY under the view now in force and return everything it wrote.
    //
    // The diagnostics come back on the same stream as the records, so a query naming
    // a table that does not exist answers with the complaint rather than with
    // nothing, and the page shows it where the records would have been.
    tempstr AcrOut(algo::strptr query);

    // Lay one dim row of plain text onto the page: the command line the rows below it
    // came from, or a note saying why there are none.
    //
    // A page opens with the command that produced it because a key does not say which
    // flags were in force when it was answered, and because the line can then be
    // pasted into a shell.
    // Return what to show in place of the command line CMD: the line itself, or the word
    // `query` when it is too long to read.
    //
    // A command line naming twenty subjects runs to several hundred characters, and at the
    // top of every page that is five rows of the screen spent on something a reader wants
    // once in twenty pages.  Eighty characters is the measure of a sentence, so a line past
    // it is put away behind the word for what it is.  A browser opens the word on a click;
    // the terminal has no click and shows the word alone, where a reader who wants to run
    // the line again has `e` for it.
    tempstr Querytext(algo::strptr cmd);

    // Lay one dim row of plain text onto the page: the command line the rows below it came
    // from, or a note saying why there are none.
    void PutNote(algo::strptr text);

    // Lay the heading of a section that opens and closes onto the page: NAME, anchored at SLUG,
    // drawn open when ON, and turning the option OPT the other way when it is followed.
    //
    // The heading is drawn whether or not the section is open, because a reader who cannot see
    // that the code is there has no reason to open it.  What opens it is the heading itself:
    // it is a link like every other thing on the page, and following it flips OPT, which is
    // the option spelled as doc's own command line spells it.  The link carries the section's
    // anchor as well, so the reader who asked for the code is left standing on it rather than
    // wherever the row they were on has moved to.
    //
    // A section that cannot close names no option.  It is then a heading with no triangle and
    // no link, since a triangle on something that never turns says nothing.
    void PutFoldhead(algo::strptr name, algo::strptr slug, algo::strptr opt, bool on);

    // -------------------------------------------------------------------
    // cpp/doc/browse.cpp
    //

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
    tempstr HelpText();

    // Lay the page of keys onto the page.
    void RenderHelp();

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
    void Browse();

    // -------------------------------------------------------------------
    // cpp/doc/canvas.cpp
    //

    // Drop the page: its cells, its rows and its links.
    void CanvasClear();

    // Append one cell to the row being built.
    void PutCell(u32 ch, doc::Attr attr, doc::Color color, int link);

    // Close the row being built and start the next one.
    //
    // Trailing blanks are dropped as the row closes.  Padding is emitted freely
    // while a table row is laid out, and a row that kept it would end every line
    // with spaces that show up as trailing whitespace in a capture and as a wide
    // selection on the screen.
    void EndRow();

    // Return the number of cells already in the row being built.
    int RowWidth();

    // Append N blank cells to the row being built.
    void PutSpace(int n);

    // Append blanks until the row being built is COL cells wide.
    void PadTo(int col);

    // Decode the utf-8 sequence of TEXT at POS into CH and return the offset just
    // past it.  A byte that starts no valid sequence is passed through as itself,
    // so malformed input costs one garbled character rather than a desynchronized
    // scan.
    int Utf8Get(algo::strptr text, int pos, u32 &ch);

    // Append the characters of TEXT to RUN, all carrying the same style.
    void PutRunText(doc::FRun &run, algo::strptr text, doc::Attr attr, doc::Color color, int link);

    // Append cells BEGIN through BEGIN+N of RUN to the row being built.
    void PutRun(doc::FRun &run, int begin, int n);

    // Encode the code point CH as utf-8 and append it to OUT.
    void PrintUtf8(u32 ch, algo::cstring &out);

    // Append the characters of TEXT to the row being built, all carrying the same
    // style and all belonging to LINK, which defaults to no link at all.
    void PutText(algo::strptr text, doc::Attr attr, doc::Color color, int link = 0);

    // Record where each link's first cell landed, so the reader can be taken to a
    // link without the page being searched for it.
    //
    // Position is not known while a link is rendered.  Wrapping moves text after
    // the fact, and half the links in this corpus sit in table cells, which are
    // wrapped one at a time and then printed column by column -- so the order links
    // reach the page is not the order they were rendered in.  Reading the finished
    // page settles it for both cases at once.
    void LocateLink();

    // Return true when an occurrence of the text being searched for begins exactly
    // at column ICOL of row IROW.
    //
    // This is the test the reader steps by, where HitQ is the test the painter
    // highlights by: stepping wants one position per occurrence, while painting
    // wants every cell the occurrence covers.
    bool HitStartQ(int irow, int icol);

    // Return true when the cell at column ICOL of row IROW falls inside an
    // occurrence of the text the reader is searching for.
    //
    // The comparison is over the row's characters rather than over the painted
    // output, so a hit is found the same way whether or not the run it lands in is
    // coloured, and a match that straddles two styles is still one match.
    bool HitQ(int irow, int icol);

    // Paint row IROW into OUT, giving every cell whose link is SELECT, and every
    // cell inside a hit recorded by the search, the reverse style on the way past.
    // NCOL is the number of cells there is room for, or zero for all of them.
    //
    // SELECT is the one-based index of the selected link, or zero when nothing is
    // selected.  Every hit of the search is drawn in reverse and the one the reader
    // is standing on is drawn bold as well, since a screen with four identical
    // highlights on it does not say which one the arrows will move away from.  The
    // overlay is applied here and nowhere else: no cell is modified, so moving the
    // selection is a repaint rather than a re-render.
    //
    // NCOL is what stops a row wider than the window from wrapping onto a second
    // display line.  A record copied from acr runs to a hundred and forty columns,
    // and a screenful of those on an eighty-column terminal asks it to draw
    // thirty-one lines in a twenty-four line window: the terminal scrolls seven
    // lines, every frame, and the reader sees the whole page jump.  Cutting each row
    // at the edge of the window is what a pager does with long lines, and it makes
    // the frame exactly as tall as the screen.  The plain-text path passes zero,
    // since its output is somebody's file rather than a window.
    void PaintRow(int irow, int select, algo::cstring &out, int ncol = 0);

    // Paint every row of the page into OUT, one line each.
    void PaintPage(algo::cstring &out);

    // -------------------------------------------------------------------
    // cpp/doc/doc.cpp
    //

    // Fill _db.c_match with the documents KEY names, and return true when KEY
    // matched one of the path conventions rather than being searched for.
    //
    // The conventions are probed in order, best first, and the order they are
    // probed in is the order the matches come out in.  That ranking is what makes
    // the common case land: `acr` names two documents in the tree -- the tool's README and
    // the rule file beside it -- and a reader who types the tool's name wants its README,
    // every time.  The tiers below the
    // first are still reachable, because they follow the winner in the list and
    // the renderer names them at the end of what it shows.
    //
    // A key that matches no convention becomes a substring search over every
    // document path.  The two cases are told apart by the return value, and the
    // caller uses it to decide between showing a document and listing candidates:
    // a name resolves, a guess offers.
    bool ResolveKey(algo::strptr key);

    // Return the query locations a key that named no document could have meant, one per
    // line, and set NFOUND to how many there were even when NMAX cuts the list.
    //
    // A document is half of what this tool reads and the database is the other half, so
    // a key that matched no document has only been half looked for.  A table whose name
    // carries the key leads to the whole of that table, and a ctype whose name carries it
    // leads to its record -- which is what a reader typing `FStream` is after, since no
    // document is named that and eight ctypes are.
    //
    // A dotted key whose namespace exists offers that namespace's ctypes as well.  The
    // key that prompts this is the near miss: `lib_x2.FCtype` names nothing at all, and
    // the useful answer is not silence but the list its author was picking from.
    tempstr GuessLoc(algo::strptr key, int nmax, int &nfound);

    // Return the acr query the key KEY selects records with, or nothing when it selects
    // none.
    //
    // A key that is already a query is one.  Otherwise the key may name a ctype, whose
    // record it is; a namespace, whose record it is; or a table, in which case the whole
    // of that table is the selection.
    //
    // A pattern handed to `code:` selects ctypes, since that view is about ctypes and nothing
    // else.  `func:` is not resolved here at all when its argument is a regex -- a function's
    // name is arbitrary, so the regex goes to src_func as it stands.
    //
    // This is what a view of a query resolves its key with, and it prefers records where
    // resolving a key on its own prefers documents.  `func:lib_x2` asks about the
    // functions of that library, where a bare `lib_x2` opens the library's README --
    // which is right both times, because a view is a view of records and there is nothing
    // else for it to be a view of.
    tempstr KeyQuery(algo::strptr key);

    // Return the location the key KEY names, or the empty string when it names no one
    // thing.  The documents it could have meant are left in c_match either way.
    //
    // This is the whole of what a key means, and it is one function because it is asked
    // twice: once by the command line, and once by the search box on the page, whose
    // visitor typed a key into it and expects the same answer.  A key that is already a
    // path, a query or the page of keys is that location outright; a key matching a path
    // convention is the document it names; a key matching no document may still name a
    // ctype, and then it is that ctype's record.  Naming nothing at all is the
    // documentation index, which is the page to start from.
    //
    // A leading slash comes off first.  Everywhere else in this tool a target beginning
    // with one is a path from the top of the repository -- that is how the documents
    // under txt/ cite each other -- so a key reads the same way, and `/cpp/doc/html.cpp`
    // means what `cpp/doc/html.cpp` means.  One consequence is worth naming: an absolute
    // path of the filesystem is then not a key at all, so nothing reaches a file test
    // with a leading slash still on it.
    //
    // A fragment comes off with the leading slash and goes back onto whatever resolved, so
    // `txt/rule/doc.md#invariants` names that heading of that document.  It is the one part
    // of a page's state the server never learns on its own, because a browser keeps a
    // fragment to itself -- so the box carries it back here when a reader submits.
    //
    // A directory resolves to its README, with the slash or without it, because that is the
    // page somebody opening a directory wants: a README says what the directory is about and
    // ends with the listing of what it holds, so it answers both questions where the bare
    // listing answers one.  A directory holding no README resolves to itself, its page is the
    // listing, and the location it resolves to carries the slash -- which is the whole of
    // what tells a directory from a file everywhere below here.
    //
    // A table's name is that table's rows.  `doc dmmeta.ns` is a question about what is in
    // that table, and it is answered with the records; the schema and the table's own row
    // follow them as sections of the same page, which is the order the reader asked in.  The
    // file under txt/ssimdb that holds the prose is still a document and still opens when it
    // is named by its path.
    //
    // A word doc does not synthesize a page for is a table's short name, and it is expanded
    // to the full one here.  So `ssimfile:dmmeta.ctype` and `dmmeta.ssimfile:dmmeta.ctype`
    // are one page rather than two, and the location a link or a history carries is the
    // full name whichever of them the reader typed.
    //
    // A key of three dots is an amc key naming one function, and it opens that function's
    // source.  amc keys a ctype by a namespace and a name and a field by a ctype and a name,
    // so the count of dots is what tells the three apart, and `x2ui.FCtype.c_cstr.Remove`
    // needs no word in front of it to say which it is.
    //
    // A word whose argument is a pattern is the query over that word's own table.  `ns:acr`
    // is one namespace and the page doc writes about it; `ns:%` is every namespace, which is
    // no such page and is the rows of `dmmeta.ns`.  Which table a word is about is the
    // `ssimfile` of its `dev.docpage` row, so a word with no table keeps its own page
    // whatever it is handed.
    //
    // A key naming a view of a query carries a key of its own, and that inner key is
    // resolved to records rather than to a document.  So `func:lib_x2.FStream` is the
    // functions of that ctype and `code:lib_x2` the generated code of that namespace,
    // while what comes back is spelled out in full -- `func:dmmeta.ctype:lib_x2.FStream`
    // -- because one page has one canonical text and the short form is a way of typing
    // it rather than a second name for it.
    tempstr ResolveLoc(algo::strptr want);

    // Return the location a link TARGET written on the page at BASE leads to, or
    // the empty string when it leads nowhere this tool can open.
    //
    // Five forms occur.  A link into a namespace's ssimdb documentation, anchored at a
    // table's own name, is that table's page: a file in the tree can spell a synthesized
    // page no other way, since a markdown link may name a path and a fragment and nothing
    // else, and the generated field tables carry thousands of them.  A target carrying a
    // colon is a query over the ssim database, and it is its own location -- an attribute of
    // a record names the record it refers to, not a path, so nothing is resolved against
    // anything.  A target beginning with a slash is a path from the top of the repository,
    // which is how the documents under txt/ cite each other.  A target beginning with a hash
    // is a heading inside the document being read, and names no file at all.  Anything else
    // is a path relative to the directory the citing document lives in.
    tempstr ResolveTarget(algo::strptr base, algo::strptr target);

    // Flip the display option the link target TARGET names, and return true when it names
    // none of them.  A fold heading is the link that carries one.
    //
    // Following such a link is not a navigation.  The location does not change, the option
    // rides along every link on the page after it, and nothing goes onto the history -- a
    // reader who opened the code and then followed an attribute expects Backspace to bring
    // back the page with the code still open, not to close it.
    //
    // The target is the option as doc's own command line spells it, which is what lets the
    // heading say what it does: the page a reader gets by following `-code` is the page
    // `doc -code` would have printed.
    bool Foldflip(algo::strptr target);

    // Show one document, answer one query, or say which documents the key could have
    // meant.
    //
    // A key carrying a colon is a query over the ssim database rather than the name
    // of a document, so `doc dmmeta.ctype:dmmeta.Ctype` puts that record on the page
    // and every attribute of it becomes a link.  No document in the tree has a colon
    // in its path, which is what lets the one argument mean both things.
    //
    // A terminal gets the reader, which owns the screen so that links can be
    // selected and followed.  Anything else -- a pipe, a file, a capture -- gets the
    // page written out once as plain text, which is what makes `doc acr | grep`
    // and a comptest see the same thing.
    //
    // -html asks for the same location as an HTML page instead, and then the cell grid
    // is never built: a browser wants the structure a grid has already thrown away, so
    // the HTML comes from the blocks.  The grid is still built when -links or -follow
    // is asked for, because those report what the grid decided.
    //
    // -bind turns the whole thing around.  Instead of one location shown once, the
    // process answers a location per request until it is killed, and the key on the
    // command line is not used at all -- every request brings its own.
    //
    // Naming nothing opens the documentation index, which is a page of links into
    // every part of the tree and so the one page a reader who does not yet know
    // what to ask for can start from.  It is txt/README.md rather than the copy at
    // the top of the repository, because that copy is generated from this one and
    // opens with two lines telling the reader not to edit it.  `-list` with no key
    // still lists every document.
    //
    // A key that names no document at all may still name a ctype, and `doc
    // lib_x2.FStream` then opens that ctype's record.  Most of the tree's types have
    // no document of their own, and the record is what there is to read about one:
    // its fields, and from there the functions that take it.
    //
    // The one key that names neither a document nor a record is `help`, which is the
    // page of keys the reader's ? opens.  It is a location like the others, so it can
    // be written out at a pipe as well as read on the screen.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:doc

    // -------------------------------------------------------------------
    // cpp/doc/docpage.cpp
    //

    // Lay the access paths of the ctypes the location names onto the page.
    //     (user-implemented function, prototype is in amc-generated header)
    // void docpage_accesspath(); // gstatic/dev.docpage:accesspath

    // Lay what amc generated for the ctypes of a selection onto the page.
    // void docpage_code(); // gstatic/dev.docpage:code

    // Lay one ctype onto the page: its declaration, the prose about its table, where it sits.
    // void docpage_ctype(); // gstatic/dev.docpage:ctype

    // Lay the functions a selection is about, or one function's source, onto the page.
    // void docpage_func(); // gstatic/dev.docpage:func

    // Lay the page of keys onto the page.
    // void docpage_help(); // gstatic/dev.docpage:help

    // Lay one namespace onto the page: its ctypes, sources, dependencies, inputs, tests.
    // void docpage_ns(); // gstatic/dev.docpage:ns

    // Lay the message roots, or one message, onto the page.
    // void docpage_msg(); // gstatic/dev.docpage:msg

    // Lay the records a query selects onto the page.
    // void docpage_query(); // gstatic/dev.docpage:query

    // Return the page the location LOC asks for, or NULL when no generated page answers for it.
    //
    // The word before the first colon names the page, and a location carrying no such word is
    // the records of a query -- which is the one page addressed by what it selects rather than
    // by a name.
    //
    // A bare key is the other case that carries no word: a page about the whole tree has
    // nothing to put after a colon, so its own name is the location.
    //
    // A function's source is the case that carries no word at all: `cpp/doc/ns.cpp:NavText` is
    // a file and a name, so the word before its colon is a path.  It answers to the same row as
    // `func:`, because both are questions about functions and the argument's own shape is what
    // says which of the two was asked.
    doc::FDocpage *Docpage(algo::strptr loc);

    // -------------------------------------------------------------------
    // cpp/doc/func.cpp -- The functions and the generated code of a selection
    //

    // Return the offset of the first byte of LINE at or after POS that is not blank.
    int SkipSpace(algo::strptr line, int pos);

    // Make the ctype NAME the one thing the page is about.
    //
    // A page about one ctype is about that ctype and nothing else.  Reading the subjects off
    // the records instead would widen the question, because the closure of a ctype row
    // reaches the FDb field that holds a pool of it -- and answering "what acts on this
    // type" with the two thousand functions of its module is not an answer.
    void SubjectOne(algo::strptr name);

    // Read off the records of the acr answer OUT the things the page is about.
    //
    // A page names either ctypes or namespaces and never a mixture, because the
    // namespaces are collected only when no ctype was found.  A record page opened on
    // a ctype carries the ctype's own namespace row in its closure, so a page whose
    // subject is one ctype would otherwise also be a page whose subject is the whole
    // library that declares it.  What is left for the namespace pass is the page that
    // names no ctype at all, which is what a page of dmmeta.ns records is.
    void SubjectRead(algo::strptr out);

    // Return the src_func command line that lists the functions of the page's subjects,
    // spelled so that a reader can paste it into a shell.
    //
    // A ctype's functions are the ones whose prototype names it, which is what `-matchproto`
    // asks.  Nothing narrows by a function's own name: an accessor amc wrote for a field is
    // named after the field, and such a filter cut the list to a fraction of itself.
    //
    // Nothing narrows by target either.  Doing so assumed a namespace's sources sit in a
    // target of the same name, and that is false wherever the code lives elsewhere -- `algo`
    // is written in `cpp/lib/algo` under another target, so `-targsrc:algo/%` matched nothing
    // and both the namespace page and every algo ctype page reported no functions at all.
    // src_func answers off an index either way, and the scan of the tree costs a tenth of a
    // second where the scan of one target cost a hundredth.
    //
    // The comment above each function comes back as well, since that is where this repo says
    // what calling it does.
    //
    // A ctype's generated functions are what a reader of the code section came for -- the
    // accessors amc wrote are the interface of that type -- so the scan includes them.  A
    // namespace's functions are the other question, and there the generated code is fifteen
    // thousand functions nobody asked to read, so it stays out.
    //
    // A regex needs none of that.  The reader wrote what to match, so it is passed as it
    // stands, and the generated sources are scanned along with the rest: the accessors amc
    // wrote for a type are as much its functions as the hand-written ones, and a regex such as
    // `algo.Smallstr100%` matches nothing outside them at all.
    //
    // Either way the scan is confined to the subject's own namespace.  A prototype match alone
    // answers with every function anywhere that takes the type as a parameter, and for a
    // message header that is six hundred functions across a dozen namespaces where the reader
    // asked about one -- twenty-five hundred lines of code section under a page about a
    // twelve-byte struct.  The box carries no selector to widen it back, and a section a
    // reader cannot narrow had better not be the widest reading of the question.
    tempstr FuncCmdline(algo::strptr query);

    // Return the byte offset of the colon that closes the file position src_func's
    // answer LINE opens with -- the second colon of `path:line: prototype` -- or -1
    // when the line carries no position at all.
    //
    // A line src_func wrote for some other reason, a complaint or a report, carries
    // none.  Both emitters cut the line at the same place, so a browser hangs its
    // anchor on exactly the text a terminal underlines.
    //
    // A path of this tree carries no space, and that is the whole of what keeps a comment
    // out of the answer: `// in algo_lib::_db.errtext` has two colons and is a sentence, and
    // without the test it was read as the file `// in algo_lib` and offered as a link.
    int LocEnd(algo::strptr line);

    // Return the number of functions the src_func answer ANSWER lists.  A line carrying a
    // file position is one function; anything else src_func said is not.
    int NFunc(algo::strptr answer);

    // Run acr for QUERY, read off what its records are about, and lay src_func's
    // answer for those subjects on the page.
    //
    // The page opens with the src_func line that produced it, and that line names
    // every subject in its regx -- so what the page is about is on the page, and the
    // reader can paste the line into a shell to ask src_func something else about the
    // same set.
    //
    // The selection is asked plainly, with no closure around it.  A reader who names one
    // ctype is asking about that ctype's functions, and the closure of a ctype row reaches
    // its namespace, its table and the arg of every field -- so the closure would answer
    // with the functions of twenty types the reader did not name.
    void RenderFunc(algo::strptr query);

    // Lay src_func's answer about the subjects already recorded onto the page.  QUERY names
    // them, and is on the page only so that a page about nothing can say what it asked.
    void RenderFuncset(algo::strptr query);

    // Return the chunk of a generated header that declares CTYPE, with PATH set to
    // the header it came from and AT to the line it begins at.  The chunk is empty
    // when the header declares no such ctype, and PATH is empty as well when CTYPE is
    // a builtin type, which belongs to no namespace and so to no generated header.
    //
    // amc divides a generated header into sections and opens each one with a comment naming
    // what the section is for, so the span from `// --- lib_x2.FStream` onward is that ctype's
    // generated code: where the records live, the struct itself, and then the accessor of every
    // field of it.
    //
    // The chunk ends where the struct does.  What follows it is functions, and functions belong
    // in the list src_func builds, where each is a line a reader can follow to its source --
    // rather than a wall of text nothing in which can be clicked.
    tempstr CodeChunk(algo::strptr ctype, algo::cstring &path, int &at);

    // Lay the generated declaration of CTYPE onto the page, or one row saying that
    // its namespace's generated header carries none.
    //
    // amc divides a generated header into sections and opens each one with a comment
    // naming what the section is for, so the span from `// --- lib_x2.FStream` to the
    // next such comment is exactly that ctype's generated code: where the records
    // live, the struct itself, and the accessor of every field of it.  Reading amc's
    // own boundary is what keeps the extraction one rule, where matching the struct
    // and guessing at where its accessors stop would be a list of the shapes amc
    // emits and would fall behind the next shape it learns.
    void RenderCodeOne(algo::strptr ctype);

    // Run acr for QUERY, read off which ctypes its records are about, and lay what amc
    // generated for each of them onto the page.
    //
    // A record page says what a field was declared as; what a reader wants next is
    // the accessor amc made of it, and that is a chunk of one generated header rather
    // than anything acr can answer.  The chunks follow one another with a blank row
    // between, and each opens with the file and line it was taken from, as a link --
    // so a reader who wants the surroundings can have the whole header instead.
    void RenderCode(algo::strptr query);

    // Ask src_func where the function NAME lives and read its source into _db.funcsrc.
    //
    // NAME is either what the function is called or the line it starts on, and a link built
    // from a listing carries the second, since a line is a fact src_func already reported
    // while a name would have to be parsed back out of a prototype.  SRC narrows the
    // question to one file and is empty when the location named the function alone: a key
    // is a target and a name with a dot between them and is unique in the tree, while a
    // bare name is not, so the spelling carrying a file is what says which of two static
    // helpers of one name is meant.
    //
    // The record ends up empty when no function answers, and `src` is what says so.  What
    // it holds otherwise is the comment that introduces the function, its prototype and its
    // body, and none of the rest of the file: a prototype says what to call, a body says
    // what it does, and the file around them is thousands of lines nobody asked for.
    void ReadFuncsrc(algo::strptr name, algo::strptr src);

    // Lay the source of the function NAME onto the page, under a link to the file at the
    // line the function starts on.
    //
    // The listing carries the file's own line numbers, so a link naming L and a number
    // lands on the line it names here exactly as it does on the page showing the whole
    // file.  The head is a link, so a reader who wants the surroundings after all is one
    // Enter from them.
    void RenderFuncsrc(algo::strptr name, algo::strptr src);

    // Read what the page at LOC is about and return amc_vis's drawing of the access paths of
    // those ctypes, with CMD set to the command line that produced it.
    //
    // A word of the drawing that names a ctype is drawn as a link to that ctype's page: what
    // amc_vis draws is a graph of ctypes, so nearly every word in it is one a reader wants to
    // open, and a drawing nobody can follow is a picture of the thing rather than a way in.
    //
    // The drawing is empty when the records name no ctype and no namespace, and that is
    // what says the page has no access paths to show rather than none being drawn.
    tempstr VisOut(algo::strptr loc, algo::cstring &cmd);

    // Lay the access paths of the ctypes the page at LOC is about onto the page.
    //
    // What amc_vis draws is preformatted: every column of it is placed, so it is poured
    // line for line the way a listing is and nothing wraps it.  A page that is about no
    // ctype says so instead, since an empty drawing and a drawing nobody asked for look
    // the same on the glass.
    void RenderVis(algo::strptr loc);

    // Lay the two sections every page of records carries onto the page: the access paths of
    // the ctypes it is about, and the code amc generated for them.
    //
    // Both are about whatever the page selected rather than about one kind of page, so they
    // are laid out here, once, after every body that selects records.  A heading is drawn
    // whether or not the section is open, because a reader who cannot see that the code is
    // there has no reason to open it, and the heading is the link that opens it.
    //
    // A comptest's row brings its transcript, which is the file that test is checked against.
    // Three things a reader of a comptest wants and each lives somewhere else: the row says
    // when it runs and what it may cost, the transcript is what it asserted the last time
    // somebody captured it, and the function is what it does.
    //
    // A page showing a whole table ends with two more: the table itself, and then the ctype it
    // is an instance of.  A reader who typed a table's name asked what is in it, so the records
    // come first and the schema is what they reach for after -- where the table sits is one
    // line and the schema is the shape of every line above it.
    //
    // The prose somebody wrote about the table opens the first of the two.  Most of what there
    // is to say about a table is computed, but not all of it, and what a person wrote is the
    // half a reader cannot get anywhere else.
    void RenderSection(algo::strptr loc);

    // Lay the access paths of the ctypes REGX names onto the page.
    //
    // A page of its own, rather than the section a query page grows under the vis flag,
    // because a reader sometimes wants the drawing and nothing else -- a whole namespace's
    // paths at once, say, which no single record page is about.
    void RenderAccesspath(algo::strptr regx);

    // -------------------------------------------------------------------
    // cpp/doc/hilite.cpp
    //

    // Colour the fenced block TEXT, whose fence named the language LANG, into RUN.
    // A language with no lexer here prints uncoloured, so nothing is claimed about
    // code the tool cannot read.
    //
    // `usage` is a program's own help, and it reaches the shell lexer because of the four
    // things that lexer marks -- a comment, a string, a variable and a flag -- a usage block
    // holds only flags, and the flag is the whole of what a reader is looking at.
    void Highlight(algo::strptr lang, algo::strptr text, doc::FRun &run);

    // -------------------------------------------------------------------
    // cpp/doc/html.cpp -- The same locations, laid out for a browser
    //

    // Return the URL the prefix itself names, which is where a visitor who typed the
    // host and nothing else belongs.
    tempstr UrlRoot();

    // Return the path a request for the URL prefix carries, which is what the server
    // matches an arriving path against.
    //
    // A prefix names a host as well as a path -- http://localhost:8080/doc -- and only
    // the path reaches the server, so the host has to come off.  A prefix that names
    // no host is already a path.
    tempstr UrlPath();

    // Return the URL of location LOC under the tab TAB names, which is the empty string for
    // the records themselves.
    //
    // A tab is the one part of a page's state that a link does not carry, so it cannot come
    // from the flags in force -- the strip names the tab it leads to, and this joins that
    // name to the sticky flags the location already carries.
    tempstr Tabhref(algo::strptr loc, algo::strptr tab);

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
    tempstr Href(algo::strptr loc);

    // Return the location the URL path PATH names, the prefix already off it.
    // Naming nothing is the documentation index, which is what the home button leads
    // to.
    tempstr UrlLoc(algo::strptr path);

    // Return true when LOC names a directory of the tree.
    //
    // A breadcrumb makes every component of a path a link, so each of them has to
    // answer with something, and what a directory answers with is what it holds.  The
    // trailing slash is the whole of the test, and it is what a browser sends for a
    // crumb.
    bool DirQ(algo::strptr loc);

    // Append TEXT to OUT with the characters HTML reserves written as entities.
    //
    // Everything that reaches a page goes through here: a document's own words, a
    // record acr printed, a prototype, and the key a visitor typed into the search
    // box.  A `<` that arrived as data and left as markup is the whole of how a page
    // becomes something its author did not write, and an include line in a C++ listing
    // carries one on every file.
    void HtmlEsc(algo::cstring &out, algo::strptr text);

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
    void HtmlInline(algo::cstring &out, algo::strptr in, algo::strptr base);

    // Append the file at PATH to OUT: a markdown document as its blocks, anything else
    // as one coloured listing.
    void PutFile(algo::cstring &out, algo::strptr path);

    // Return the tab the page is showing: the schema, the command line, or the records,
    // which is the tab a location carries no parameter for.
    tempstr Curtab();

    // Append the records QUERY selects to OUT.
    void PutQuery(algo::cstring &out, algo::strptr query);

    // Append the functions the records of QUERY are about to OUT.
    void PutFunc(algo::cstring &out, algo::strptr query);

    // Append what amc generated for the ctypes the records of QUERY are about to OUT,
    // one chunk of one generated header each.
    void PutCodeHtml(algo::cstring &out, algo::strptr query);

    // Append the listing of the directory DIR to OUT.
    //
    // The text is markdown and goes through the parser a document goes through, so the
    // listing is a real table here and the same table on the glass.
    void PutDir(algo::cstring &out, algo::strptr dir);

    // Append the page of keys to OUT.
    //
    // The keys belong to the reader at the terminal, and the browser has none of its
    // own -- but the page that describes them is a document, and a visitor reading the
    // manual in a browser is who wants to know what the terminal does.  It is the same
    // markdown either way, so its tables become HTML tables here without anything
    // being written twice.
    void PutHelp(algo::cstring &out);

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
    void PutNs(algo::cstring &out, algo::strptr ns);

    // Append the page about the message location LOC to OUT: the roots of every protocol when
    // LOC names none, and otherwise one message -- its declaration, then what begins with it
    // or where it is received.
    void PutMsg(algo::cstring &out, algo::strptr loc);

    // Append the ctype NAME to OUT: what it was declared to be, what somebody wrote about the
    // table it holds, and where it sits.
    //
    // The two sections a reader can open -- the access paths and the code -- are not written
    // here.  They are about whatever records a page selects, and this page is one of those, so
    // they follow every such body from one place.
    void PutCtype(algo::cstring &out, algo::strptr name);

    // Append the source of the function LOC names to OUT, under the two tabs such a page
    // carries: the source itself, and the command line that found it.
    //
    // The listing carries the file's own line numbers rather than starting again at one, so
    // a link naming L and a number lands on the line it names here exactly as it does on
    // the page showing the whole file.
    void PutFuncsrc(algo::cstring &out, algo::strptr loc);

    // Append the access path drawing of the ctypes REGX names to OUT.
    //
    // Every column of what amc_vis draws is placed, so it goes in a pre and nothing
    // reformats it -- the same rule that holds acr's answer, for the same reason.
    void PutAccesspath(algo::cstring &out, algo::strptr regx);

    // content comes from what a visitor typed is reached by its own name instead, which
    // is what HtmlFind is for: it takes a key rather than a location, so no request text
    // is ever dispatched on.  Falling through to the candidate list instead is how
    // `?q=/etc/passwd` served that file twice, once through resolution and once through
    // this file test.
    void HtmlBody(algo::cstring &out, algo::strptr loc);

    // Return the page for location LOC.
    tempstr HtmlPage(algo::strptr loc);

    // Return the page for a location LOC this server does not answer for.
    //
    // A page that has moved or gone leaves links behind it, and a reader following one
    // arrives here.  What they get is the page they expected -- the header, the search box,
    // the home and help buttons -- with the body saying that this particular thing is not
    // there, so the next thing they try is one keystroke away rather than a browser error.
    // The location is taken as text and dispatched on by nothing, which is the same rule the
    // candidate page follows: what a visitor typed reaches the page as words.
    tempstr HtmlMiss(algo::strptr loc);

    // Return the page offering what the key KEY could have meant.
    //
    // It takes a key and not a location, which is the whole point: a key is what a
    // visitor typed, and every branch of the body dispatch reads something.  So the one
    // page whose content comes from a request is reached by name, its body is the
    // candidate list and can be nothing else, and its breadcrumb is the key as text.
    tempstr HtmlFind(algo::strptr key);

    // -------------------------------------------------------------------
    // cpp/doc/msg.cpp
    //

    // Return the roots of every protocol in the tree, as markdown.
    //
    // A root is a ctype carrying a type field, which is the whole of what makes it a header:
    // a reader of the wire takes that field and learns which message they are holding.  So
    // this table is the list of protocols, and each entry opens the one it names.
    tempstr MsgrootText();

    // Return what the page about the message NAME says beyond its declaration, as markdown.
    //
    // Two questions, and a message answers one of them or the other.  A header answers what
    // begins with it, which is the protocol it heads.  A message answers where it is read,
    // which is every dispatch carrying it as a case.  A ctype that is neither -- a message
    // nobody dispatches, or a plain record somebody typed the word at -- answers with
    // nothing, and the page is then its declaration alone.
    tempstr MsgText(algo::strptr name);

    // Lay the page about the message location LOC onto the page at the terminal.
    void RenderMsg(algo::strptr loc);

    // -------------------------------------------------------------------
    // cpp/doc/ns.cpp
    //

    // Append a heading reading TEXT, anchored at SLUG, to OUT.
    //
    // A heading is the name of a section and nothing else, because it is what the pane lists
    // and what a link to the section carries.  How many rows the section holds goes into the
    // table under it, in the corner where a table's first column is named -- the same place a
    // box of records says how many records it holds.
    //
    // The anchor is what a browser scrolls to and what the pane hangs its entries on, and it
    // is written here for the same reason abt_md writes one under every heading it
    // generates: a heading with no anchor is a place in the page that nothing can name.  The
    // slug is fixed rather than made from the text, so a link to a section survives the
    // count in that section's heading changing.
    void PutSecthead(algo::cstring &out, algo::strptr slug, algo::strptr text);

    // Append TEXT to OUT as the contents of one cell of a markdown table.
    //
    // A pipe inside a cell would end it, and a comment is free text that may hold one, so
    // the pipes are escaped here rather than hoped about.  An empty cell is written as a
    // space, since a table row of nothing but separators reads as a rule.
    void PutTablecell(algo::cstring &out, algo::strptr text);

    // Return the page about the namespace NS, as markdown.
    //
    // It opens with what the namespace is for, in its own words from dmmeta.ns, and with
    // the three answers that are pages of their own rather than sections here -- its
    // records, its functions, its access paths.  Then the sections, each one a table, and
    // each absent when the namespace has none of that kind: a library has no comptests, a
    // ssimdb namespace has no sources, and a heading over an empty table says only that the
    // generator did not check.
    tempstr NsText(algo::strptr ns);

    // Lay the page about the namespace NS onto the page.
    void RenderNs(algo::strptr ns);

    // Return the namespace whose tables the directory DIR holds the writing about, and the
    // empty string when DIR is not one of those directories.
    //
    // txt/ssimdb/<ns>/ is where the prose about the tables of one namespace is kept, and the
    // directory's own name is the whole of what says which namespace that is.  Reading it
    // off the path rather than out of a file is what lets a namespace nobody has written a
    // word about have a page all the same.
    tempstr Ssimdbns(algo::strptr dir);

    // Return the page of what the key KEY could have meant, as markdown.
    //
    // A key that resolves to nothing has still been half looked for.  The documents whose path
    // carries it are one answer and the tables and ctypes whose name carries it are another,
    // and a key is often genuinely both -- `x2test` names two tables of this tree, and choosing
    // between them is the reader's business rather than this tool's.
    //
    // The second list is headed Disambiguation, since that is the question the page is asking.
    // Every entry is a link, so a reader picks one where they would otherwise type the name a
    // second time.
    tempstr MatchText(algo::strptr key);

    // Return the page listing what the directory DIR holds, as markdown.
    //
    // What a directory holds is usually files, and a document among them is named by its
    // title rather than by its path, since the path is what the author had to call the file
    // and the title is what the file is about.  A subdirectory has no title and is named by
    // itself.
    //
    // Under txt/ssimdb it is tables instead.  That tree is the writing about the ssim
    // database, one directory per namespace, and the tables of a namespace are what a reader
    // arriving there is looking for -- so the rows come from the database, which knows every
    // table whether or not anybody wrote a line about it.
    //
    // SKIP is the one child to leave out, and it says which of two pages this is.  Empty,
    // the listing is the directory's own page and opens with the directory's name.  Named,
    // the listing is being appended to a document of that directory -- a README, whose
    // subject is what the directory is about where the listing is what it holds -- and the
    // document it is appended to is the one child there is no point offering.
    tempstr DirText(algo::strptr dir, algo::strptr skip);

    // Lay the listing of the directory DIR onto the page, leaving out the child SKIP.
    void RenderDir(algo::strptr dir, algo::strptr skip);

    // Return TEXT with INTRO inserted after its title.
    //
    // A document opens by saying what it is, and what doc generates about the document --
    // the tables of an ssimdb namespace, the link to the internals of the namespace it is
    // about -- belongs under that rather than above it, since a reader who has not yet been
    // told what they are looking at has nothing to hang either on.  So the insertion goes
    // after the first heading, and after the anchor line under it when there is one, since an
    // anchor belongs to the heading above it and would otherwise be read as belonging to what
    // follows.
    tempstr Withintro(algo::strptr text, algo::strptr intro);

    // Return true when the page for the document at PATH ends with the listing of its own
    // directory.
    //
    // A README does, since what its directory holds is the other half of what a reader
    // arriving there is asking about.  A README under txt/ssimdb does not: what that
    // directory holds is the namespace's tables, and they have already gone under the title.
    bool FootdirQ(algo::strptr path);

    // Return what doc generates about the document at PATH to go under its title, as
    // markdown.
    //
    // A document under txt/ssimdb is about a namespace's tables, and the tables go here, under
    // the title: they are what a reader arriving there is looking for, and the page has
    // nothing else to say first.  A document under txt/protocol is about a namespace's
    // messages, and its ctypes go here for the same reason -- one convention, two kinds of
    // namespace, and in both the list is computed from the database rather than pasted into
    // the file by a generator.  Every other document has nothing above its own first
    // paragraph.
    //
    // The headings of the document are not here.  They are the pane, which is where a map
    // belongs on both devices -- a list of them in the body scrolls away with the text and
    // says exactly what the pane says.
    tempstr IntroText(algo::strptr path);

    // Return the link to the internals of the namespace the document at PATH is about, as
    // markdown, and the empty string when the document is about no namespace.
    //
    // A README under txt/exe, txt/lib, txt/protocol or txt/ssimdb sits in a directory named
    // after a namespace, and that is the whole of what says which document is about which
    // namespace.  What the link opens is the namespace page doc computes, so it says what
    // the tree holds now rather than what it held when a generator last ran.
    //
    // It goes at the foot of the page, under the listing of the directory and above the see
    // also.  What a reader arrives at a README for is the writing in it, and the machinery
    // behind the namespace is where they go once they have read it.
    tempstr InternalsText(algo::strptr path);

    // Return the namespace the document at PATH is about, and the empty string when it is
    // about none.
    //
    // A README under txt/exe, txt/lib, txt/protocol or txt/ssimdb sits in a directory named
    // after a namespace, and that path is the whole of what says which document is about
    // which one.  Reading it off the path rather than out of the file is what lets a
    // namespace nobody has written a word about carry the same page as one they have.
    tempstr Docns(algo::strptr path);

    // Return the ssimfile whose prose the heading anchored at FRAG opens, or the empty
    // string when FRAG anchors no such heading.
    //
    // A table's prose is anchored at the table's own name, which is the one spelling of a
    // synthesized page a file in the tree can carry: a markdown link may name a path and a
    // fragment and nothing else, so `txt/ssimdb/dmmeta/README.md#dmmeta-bitfld` is how a
    // generated field table points at a table.  The anchor is matched by building it from
    // each ssimfile rather than by taking it apart, since the transformation that made it
    // throws characters away.
    tempstr Ssimanchor(algo::strptr frag);

    // Return the prose somebody wrote about the ssimfile SSIMFILE, as markdown, and the
    // empty string when nobody wrote any.
    //
    // Most of what there is to say about a table is in the database and is computed, but not
    // all of it: what a bitfield is for, and the example that makes it clear, is writing.  A
    // namespace's tables are written about in one file, txt/ssimdb/<ns>/README.md, and each
    // table's prose sits under a heading naming that table -- so the heading is the whole of
    // what says which prose belongs to which table, and one file holds a namespace's
    // documentation instead of one file per table holding a title and little else.
    //
    // The heading itself is left behind, since the page it is being put on already says
    // which table this is, and so is the anchor line under it.  What is taken is everything
    // down to the next heading of the same level or shallower.
    tempstr Ssimdoc(algo::strptr ssimfile);

    // Return the ssimfile that holds the records of the ctype NAME, or the empty string
    // when the ctype is not a table.
    tempstr CtypeSsimfile(algo::strptr name);

    // Return what the ssim database says about the ctype NAME beyond its own declaration,
    // as markdown.
    //
    // A declaration says what a record holds; these sections say where it sits.  Which
    // tables build their key out of this one, which merely mention it, which command lines
    // take it as an option, and what named values its fields carry -- each of those is a
    // walk of a table doc already holds, and none of them is visible in the rows of the
    // ctype itself.
    tempstr CtypeText(algo::strptr name);

    // Return the name of the function the prototype PROTO declares, or nothing when PROTO
    // declares none.
    //
    // The name is the identifier just before the argument list, and the namespace in front
    // of it comes off -- a pane full of `lib_x2::` says nothing, since every entry on a
    // file's page shares it.
    tempstr Funcname(algo::strptr proto);

    // Return the pane of the page at LOC, as markdown, and the empty string when the page has
    // no map worth drawing.
    //
    // The pane is where a reader sees the shape of what they are reading: the headings of a
    // document, the sections of a page of records, the functions of a listing.  It is markdown
    // like every other page this tool generates, which is what lets one text be drawn above
    // the page at the terminal and beside it in a browser -- two placements of one map rather
    // than two maps.
    //
    // The headings are not written into the page as a list of chapters.  A list in the body
    // scrolls away with the text and cannot be collapsed, and it says exactly what the pane
    // says, so the pane is where it belongs on both devices.
    //
    // Every page's pane is its own anchors, whatever the page turned out to be, so nothing here
    // asks what kind of page it is except to know what to add: the sections of a page of
    // records, the functions of a listing, the other documents beside a document.  A page whose
    // body holds no anchor at all -- the listing of a directory, one function's source -- has
    // nothing to map and carries no pane.
    //
    // A fourth-level heading keeps its anchor in the file, since a document elsewhere may link
    // to it; what it does not get is an entry here, where it would bury the sections it sits
    // under.
    tempstr NavText(algo::strptr loc);

    // -------------------------------------------------------------------
    // cpp/doc/parse.cpp
    //

    // Return the language the file at PATH is written in, spelled the way a fence
    // spells it, or the empty string when the extension names nothing this tool has
    // a lexer for.
    //
    // The empty string is what a bare fence carries, so a file in a language the
    // tool cannot read is coloured exactly as an unlabelled listing is: what cannot
    // be prose is marked, and nothing is claimed about the rest.
    algo::strptr FileLang(algo::strptr path);

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
    void ParseText(algo::strptr text);

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
    tempstr Doctitle(algo::strptr path);

    // Fill the blocks with one listing holding TEXT, coloured as the language LANG names.
    //
    // The lines of a function's source mean themselves at the column they were written in,
    // exactly as a whole file's do, so they reach the page as the block a file becomes and
    // the renderer pours them the same way.
    void ParseListingText(algo::strptr text, algo::strptr lang);

    // Read the file at PATH and fill _db.block with its blocks in document order.
    //
    // The extension decides how the file is read.  Markdown is parsed into the
    // blocks its markers name; anything else is a listing, whose every line means
    // itself and is carried through unexamined.
    void ParseFile(algo::strptr path);

    // Read the markdown inline markup of IN into _db.span, one span per run of text that
    // carries one style.
    //
    // This is the whole of what the inline syntax is, and it is read once for both devices:
    // the terminal makes cells of a span and the browser wraps it in tags, and neither knows
    // anything about the syntax that produced it.
    //
    // The spans live on _db and every call replaces them, so a caller walks what it asked
    // for before asking for the next string.
    void ReadInline(algo::strptr in);

    // -------------------------------------------------------------------
    // cpp/doc/render.cpp
    //

    // Return the substring of TEXT beginning at byte POS.
    algo::strptr RestFrom(algo::strptr text, int pos);

    // Return the byte offset of the next MARK in IN at or after POS, or the length
    // of IN when there is none.  An unterminated span then renders as plain text
    // to the end of the block rather than swallowing the rest of the document.
    int FindClose(algo::strptr in, int pos, algo::strptr mark);

    // Record a link on TARGET and return its one-based index, or zero when the
    // target is one a reader at a terminal cannot be taken to.
    //
    // An http target is not recorded.  Following it would mean launching a browser
    // from a documentation reader, and a link that highlights and then refuses to
    // open is worse than one that never offered.
    int NewLink(algo::strptr target);

    // Render the markdown inline markup of IN into RUN, every cell carrying ATTR, COLOR and
    // LINK unless the markup changes them.
    //
    // The markup is read into spans first, which is where the syntax lives, so what is left
    // here is the terminal's own vocabulary: bold and emphasis are attributes, a code span is
    // cyan, and a link is underlined text pointing at a record the reader can follow.
    //
    // A link becomes one record however many spans its text turned out to be, since the
    // reader selects and follows a link rather than a run of cells.  The span's own number
    // says when a new link has started, which two adjacent links to the same target would
    // not.
    void RenderInline(algo::strptr in, doc::Attr attr, doc::Color color, int link, doc::FRun &run);

    // Work out where RUN breaks into lines of at most WIDTH cells, recording each
    // line's first cell in RUN's break array.
    //
    // A newline cell is a break the author asked for and is always taken.  Other
    // breaks go at a blank, and a word longer than the measure -- a path, a long
    // option -- is left to overhang rather than being cut, because a cut path is
    // no longer a path a reader can copy.
    void WrapCompute(doc::FRun &run, int width);

    // Lay RUN onto the page wrapped to WIDTH, with MARKER at INDENT on the first
    // line and the lines after it starting under the marker's text.  With REPEAT
    // the marker is drawn on every line instead.
    //
    // The two forms are the two things a marker can mean.  A list bullet names the
    // item once and its continuation lines hang under the item's words, which is
    // what keeps a wrapped item reading as one item.  A quotation's bar is not a
    // name but an edge, and an edge that stopped after the first line would leave
    // the rest of the quotation looking like the prose around it.
    void WrapRun(doc::FRun &run, int indent, algo::strptr marker, int width, bool repeat = false);

    // Put the plain text MARKER onto the page as unstyled cells.
    void PutRunOwn(algo::strptr marker);

    // Return the number of cells the plain text MARKER occupies.
    int MarkerWidth(algo::strptr marker);

    // Lay RUN onto the page at column INDENT, breaking only where the run itself
    // carries a newline.  A code listing is poured rather than wrapped: its lines
    // were laid out by whoever wrote them, and rewrapping them would change what
    // the code means.
    // Return the target of the heading of this page anchored at NAME, and the empty string
    // when no heading of it is.
    //
    // The blocks of the page being drawn are what is asked, so this answers about the document
    // a listing sits in rather than about the tree.  A page that documents its flags under
    // headings anchored by the flag therefore links its usage block to them, and a page that
    // does not says nothing about a word that merely looks like one.
    tempstr Headanchor(algo::strptr name);
    void PourRun(doc::FRun &run, int indent);

    // Return the byte offset just past the pipe table row LINE's leading delimiter.
    // Return the byte offset of the first cell of the table row LINE, past the pipe
    // that opens it when the author wrote one.
    int RowBegin(algo::strptr line);

    // Return the byte offset of the pipe table row LINE's trailing delimiter.
    // Return the byte offset just past the last cell of the table row LINE, before the
    // pipe that closes it when the author wrote one.
    int RowEnd(algo::strptr line);

    // Return true when every cell of the pipe table row LINE is a separator, which
    // is how markdown marks the row above as the header.
    // Return true when LINE is the separator row markdown uses to mark the row above
    // it as the header.  It carries no content, so it is dropped once it has served
    // that purpose -- on the terminal and in the browser alike.
    bool SepRowQ(algo::strptr line);

    // Lay the pipe table TEXT onto the page, indented by INDENT and fitted to the
    // resolved measure.  The separator row that markdown uses to mark the header
    // carries no content, so it is dropped once it has served that purpose.
    void RenderTable(algo::strptr text, int indent);

    // Lay every parsed block onto the page.
    //
    // The indentation follows a man page.  A top-level heading sits at the left
    // margin where the eye finds it while scrolling, a deeper heading is inset by
    // two, and body text is inset by three under all of them.  A listing goes one
    // step further in, which is what separates it from the prose around it even
    // with colour turned off.
    //
    // Ordered items are numbered from the running count of adjacent items at the
    // same depth rather than from the digits the author typed.  Markdown files are
    // edited by inserting lines, so those digits are wrong as often as not.
    void RenderDoc();

    // Return the row the target NAME leads to, or -1 when this page carries no such
    // row.
    //
    // The anchors were recorded as the page was laid out, from the markers abt_md
    // plants under its headings, so this is an exact match against the same string a
    // link's target names.  Searching the page text for the name instead can never
    // work: a target is a slug -- `reading-a-document` -- while what is printed is
    // the heading it was made from.
    //
    // A target spelling `L` and a number names a line of a file rather than a heading,
    // which is how a function link reaches the line its prototype was declared on.  A page
    // that is one file listing shows one line per row, so the row follows from the row the
    // listing began at and the file line that row shows; a page that is not one listing has
    // no lines to name and answers -1.  A whole file begins at line one and a function's
    // source begins wherever the function does, which is what `listbase` carries.
    // Record the anchor SLUG at the row about to be laid out, so that a link naming it can be
    // followed to here.
    //
    // A heading of a document carries the anchor abt_md planted under it; a section doc lays
    // out itself carries the one the pane spells.  Both arrive here, so an entry of the pane
    // leads somewhere on either kind of page.
    void PutAnchor(algo::strptr slug);

    // Return the row the anchor NAME was laid out on, or -1 when the page carries no such
    // anchor.
    int AnchorRow(algo::strptr name);

    // Lay the page of what the key KEY could have meant onto the page, and record the key as
    // the location the reader is on.
    //
    // It is a page like any other once it is drawn, which is what lets a key that resolved to
    // nothing be read on the screen, paged, searched and followed out of -- the reader picks
    // one of the candidates with Enter instead of typing a name a second time.
    void RenderMatch(algo::strptr key);

    // Lay the location LOC onto the page, and record it as the one the reader is on.
    //
    // A location is either a page this tool generates, a directory of the tree, or a file --
    // and dev.docpage is the list of the first kind, each row bound to the function that draws
    // it.  So the dispatch here is a lookup: the word before the colon names the page, and what
    // it names is a row rather than a case.  Access paths are the one thing added to a
    // page rather than laid out as one: they are drawn about whatever records the page
    // selects, so the drawing follows every body that selects any, in one place.  What the two share is this frame: the
    // old page goes, the location is recorded so that the reader has a name to come
    // back to, and the links are located once the page stands still.  Where the
    // links are cannot be known before that, because wrapping and table layout both
    // move text after it has been rendered.
    void RenderPage(algo::strptr loc);

    // -------------------------------------------------------------------
    // cpp/doc/serve.cpp -- Answering one location per request
    //

    // Bind the address -bind names and answer requests there until the process is
    // killed.
    //
    // The address it is listening on is printed before the first request can arrive,
    // because that is the only way a caller learns the server is up -- a test that
    // starts one and connects to it has nothing else to wait for.
    void Serve();
}
