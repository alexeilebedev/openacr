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
// Target: atf_comp (exe) -- Component test runner: spawn processes and diff the log against a reference
// Exceptions: yes
// Source: cpp/atf_comp/src_func.cpp -- Comptests for src_func
//
// Comptests for src_func, the function indexer and update-hdr rewriter.
// Each test runs src_func against a single-file dataset under
// test/src_func/ whose targsrc row names the file the test breaks.
// Write-denial tests seed the file under ro/ in the test's tempdir and
// compose their command with DenyWriteCmd (cpp/atf_comp/proc.cpp);
// read-failure tests name a file that does not exist.

#include "include/algo.h"
#include "include/atf_comp.h"

// src_func -updateproto against a header whose directory denies the write:
// the update-hdr block rewrite fails, and the run must fail naming the path
// instead of exiting 0 with the header silently stale. The seed puts junk
// inside the update-hdr block so the rewrite has something to change.
void atf_comp::comptest_src_func_FileWriteFail() {
    atf_comp::ProcStart(DenyWriteCmd("printf \"namespace xx { // update-hdr\\njunk\\n}\\n\" > ro/x.h",
                                     "$$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/writefail.ssim -updateproto"));
}

// A targsrc source file that does not exist: the scan must fail the run
// naming the path instead of indexing the file as empty with exit 0, which
// would silently drop its functions from every listing and header.
void atf_comp::comptest_src_func_FileReadFail() {
    atf_comp::ProcStart("$bindir/src_func -in:test/src_func/readfail.ssim");
}

// src_func -updateproto against a targsrc header that does not exist: the
// rewrite pass must fail the run naming the path instead of exiting 0 with
// the header's update-hdr block silently left stale.
void atf_comp::comptest_src_func_UpdateprotoReadFail() {
    atf_comp::ProcStart("$bindir/src_func -in:test/src_func/hdr_readfail.ssim -updateproto");
}

// src_func -createmissing with a targsrc source file that does not exist:
// the failed scan leaves the function index incomplete, so the stub-creation
// pass must be skipped entirely. The universe declares userfunc xx.NewFunc
// with no scanned definition; an ungated pass would append a stub for it to
// the healthy source, duplicating a definition the scan never saw, so the
// test pins the source byte-unchanged (cmp) next to exit code 1.
void atf_comp::comptest_src_func_CreatemissingScanFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"void xx::ExistingFunc() {\\n}\\n\" > src/y.cpp"
                        " && cp src/y.cpp orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/createmissing_readfail.ssim -createmissing;"
                        " test $$? = 1 && cmp src/y.cpp orig && echo source_unchanged'");
}

// src_func -updateproto against a header whose update-hdr section is
// unterminated after an earlier one closed: the rewrite would drop everything
// from the unterminated marker to EOF and write the truncation back, so the
// run must fail naming the file, pinned byte-unchanged (cmp) next to exit
// code 1.
void atf_comp::comptest_src_func_UpdateprotoUnterminated() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && printf \"namespace xx { // update-hdr\\n}\\nnamespace yy { // update-hdr\\nvoid Dropped();\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unterminated.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}

// A targsrc source that ends inside a function (no closing brace at column
// 0): the scan is incomplete, so the run must fail, and the header rewrite
// must be skipped -- the truncated file's functions would otherwise vanish
// from the header. The test pins the header byte-unchanged (cmp) next to
// exit code 1.
void atf_comp::comptest_src_func_UnfinishedFunc() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src hdr"
                        " && printf \"void xx::Foo() {\\n    body();\\n\" > src/y.cpp"
                        " && printf \"namespace xx { // update-hdr\\nvoid OldFunc();\\n}\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unfinished.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}

// A template parameter list can carry a parenthesized default argument whose
// expression contains '>' (e.g. 'bool B = (4 > 2)') next to a function-pointer
// parameter: if the '>' inside the parens ended the template-list strip early,
// the name extraction would fall into the fn-ptr paren and key the function as
// the bare namespace. The test scans one such function next to a plain fn-ptr
// template and pins the printed index rows.
void atf_comp::comptest_src_func_TemplateParenDefault() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"template<bool B = (4 > 2), void (*F)(int)> inline void xx::Paren(int a) {\\n}\\ntemplate<typename T, void (*F)(T)> inline void xx::Fnptr(T t) {\\n}\\n\" > src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/template.ssim % -printssim'");
}

// The iffy 'bigret' check reports a function that returns cstring by value.
// It reads the return type as whatever stands left of the parameter list, and
// finds the parameter list as the first paren on the line. A template
// parameter list can carry parens of its own -- the function-pointer parameter
// in 'template<typename T, void (*F)(T)>' -- so the paren search has to run on
// the line with the template list stripped. Read from the raw line, the type
// comes out of the template list instead, and the check both fires on
// functions that are fine and stays silent on functions that are not. The
// fixture holds the whole table of return-type shapes, controls next to
// rejects, in source order:
//
//   signature shape                                | expected
//   ------------------------------------------------ --------
//   cstring ret                                    | bigret
//   tempstr ret                                    | silent
//   cstring & ret                                  | silent
//   cstring * ret                                  | silent
//   template<class T> cstring ret                  | bigret
//   template<typename T, void (*F)(T)> cstring ret  | bigret
//   template<cstring (*F)(int)> void ret           | silent
//   template<class T, int *P> cstring ret          | bigret
//   template<typename T, void (*F)(T)> cstring & ret | silent
void atf_comp::comptest_src_func_TemplateIffyRet() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"cstring xx::PlainRet() {\\n}\\n\" > src/y.cpp"
                        " && printf \"tempstr xx::PlainTempstr() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"cstring &xx::PlainRef() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"cstring *xx::PlainPtr() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"template<class T> cstring xx::TmplRet(T t) {\\n}\\n\" >> src/y.cpp"
                        " && printf \"template<typename T, void (*F)(T)> cstring xx::FnptrRet(T t) {\\n}\\n\" >> src/y.cpp"
                        " && printf \"template<cstring (*F)(int)> void xx::FnptrCstringParam(int a) {\\n}\\n\" >> src/y.cpp"
                        " && printf \"template<class T, int *P> cstring xx::TmplPtrParam(T t) {\\n}\\n\" >> src/y.cpp"
                        " && printf \"template<typename T, void (*F)(T)> cstring &xx::FnptrRetRef(T t) {\\n}\\n\" >> src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/template.ssim % -iffy -list'");
}

// src_func -updateproto with a targsrc source file that does not exist: the
// failed scan leaves the function index incomplete, so the header rewrite
// must be skipped entirely. The seed puts a prototype inside the header's
// update-hdr block; a rewrite from the incomplete index would strip it, so
// the test pins the header byte-unchanged (cmp) next to exit code 1.
void atf_comp::comptest_src_func_UpdateprotoScanFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && printf \"namespace xx { // update-hdr\\nvoid OldFunc();\\n}\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/scan_readfail.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}

// The dmmeta.userfunc primary key holds the amc function key it is copied
// from, which is a hundred characters wide. When the key was narrower, amc
// clipped every longer name on the way out and two functions whose names
// agreed up to the cut wrote the same row, so one registration disappeared.
// The accepting universe pins the whole width: a short key, two keys that
// agree for their first fifty characters and differ past it, and a key at
// the hundred-character limit -- all four load as distinct rows and each
// binds to its own scanned function, printed with its own acrkey. The
// second universe carries a key one character over the limit and must be
// rejected naming the limit, with the run exiting 1.
void atf_comp::comptest_src_func_UserfuncKeyWidth() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"void xx::ShortName() {\\n}\\nvoid xx::NearTwinKeyThatAgreesUpToTheFiftiethCharacterThenAlpha() {\\n}\\nvoid xx::NearTwinKeyThatAgreesUpToTheFiftiethCharacterThenBravo() {\\n}\\nvoid xx::UserfuncKeyExactlyAtTheHundredCharacterLimitOfTheWidenedPrimaryKeyPaddedOutWithExtraWordsToHere() {\\n}\\n\" > src/y.cpp"
                        " && cp $$OLDPWD/test/src_func/userfunc_width.ssim $$OLDPWD/test/src_func/userfunc_toolong.ssim ."
                        " && $$OLDPWD/$bindir/src_func -in:userfunc_width.ssim % -printssim"
                        " && $$OLDPWD/$bindir/src_func -in:userfunc_toolong.ssim % -printssim;"
                        " test $$? = 1 && echo over_limit_rejected'");
}

// The comment lines above a function belong to that function, and the header's
// update-hdr block mirrors them. The divider line this repo puts between
// functions belongs to no function, so it ends the comment accumulated above
// it. A comment line qualifies as a divider by its whole form and not by a
// substring of it: an ascii table inside a leading comment draws a rule line
// out of dashes, and reading that rule as a divider drops everything above the
// table -- the paragraph saying what the function does, and the table's own
// header row -- from the mirrored header, silently. So a rule line breaks its
// dashes with something, which every table in the tree does, and the last
// function of the fixture pins the boundary: a rule line of nothing but dashes
// still reads as a divider. The fixture holds one function per line shape, in
// source order, and prints the header the scan produced.
//
// A banner of slashes is the same rule read over the other divider this repo
// draws. It is written both as a bare run of slashes and as a comment holding
// one, `// ` and then the run, and the second form is what the two banners in
// the tree are written as; a run of slashes with a word among them is a section
// title, which is comment text for the reason a rule line broken by a word is.
//
//   comment line in the middle of a comment  | mirrored
//   ------------------------------------------ ------------------
//   none, a one-line comment                 | the line
//   a divider of dashes                      | below the divider
//   a banner of slashes                      | below the banner
//   a banner of slashes behind a `//`        | below the banner
//   a slash run under the seventeen bound    | every line
//   a slash run around a word                | every line
//   a table rule of two dash runs            | every line
//   a section marker, dashes around a word   | every line
//   a dash run under the twenty-dash bound   | every line
//   a table rule broken by a pipe            | every line
//   a table rule of nothing but dashes       | below the rule
void atf_comp::comptest_src_func_Precomment() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"namespace xx { // update-hdr\\n}\\n\" > src/x.h"
                        " && printf \"// plain leading comment\\nvoid xx::Plain() {\\n}\\n\" > src/y.cpp"
                        " && printf \"// above the divider\\n// ----------------------------------------\\n// below the divider\\nvoid xx::Divider() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the banner\\n/////////////////////////////////////////\\n// below the banner\\nvoid xx::Banner() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the comment banner\\n// //////////////////////////////////////\\n// below the comment banner\\nvoid xx::CommentBanner() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the short slash run\\n// //////////\\n// below the short slash run\\nvoid xx::ShortSlashRun() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the slash title\\n// //////// Section ////////////////////\\n// below the slash title\\nvoid xx::SlashTitle() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// what TableRule does\\n//\\n//   shape                  | expected\\n//   ---------------------- ----------\\n//   a row                  | a result\\nvoid xx::TableRule() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the section marker\\n// ---- Section ------------------------\\n// below the section marker\\nvoid xx::Section() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the short run\\n// ----------\\n// below the short run\\nvoid xx::ShortRun() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the pipe rule\\n//   ----------------------|----------------------\\n// below the pipe rule\\nvoid xx::PipeRule() {\\n}\\n\" >> src/y.cpp"
                        " && printf \"// above the bare rule\\n//   ----------------------------------------\\n// below the bare rule\\nvoid xx::BareRule() {\\n}\\n\" >> src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/precomment.ssim -updateproto"
                        " && cat src/x.h'");
}

// The rule that locates the closing brace of an update-hdr section, over one
// header carrying every shape it has to place. A section closes on the lines
// that return the body to the nesting level the marker opened, whatever else
// those lines carry, so the fixture closes six sections that way: a bare brace,
// a brace with a trailing line comment, a brace with a trailing block comment,
// a brace indented with a tab where the marker was not indented at all, a `};`
// -- the namespace brace followed by an empty declaration, which is legal C++
// -- and a marker opening two namespaces whose two braces stand on one line.
// A seventh opens two namespaces and closes them on two separate lines, so the
// golden pins that both come back: a rule keeping only the line that reaches
// the marker's level returns a header one brace short of compiling. Two further
// sections open several namespaces and stand hand-written lines between the
// closes -- a struct after the first of two closes, and a declaration followed by
// a struct after the first of three. Those lines are below the section, so the
// golden pins them coming back verbatim next to a single prototype block: a
// section that stayed open past its first close reads the struct's opening brace
// as body and drops it, then takes the struct's own `};` for a second close and
// writes the prototypes again, at exit 0. The first
// section holds the shapes that must not be taken for its close -- a struct's
// `};`, an `} else {`, and the braces of a block the body itself opened, which
// stand deeper than the marker -- and an eighth section holds the same case for
// a namespace, which the body opens and closes inside the replaced text. Reading
// any of those as the close writes the fresh prototypes there and drops the
// hand-written lines above it, silently and at exit 0, which is what this test
// exists to catch. Two further sections
// hold braces that nest no C++ and must not be counted: one inside a line
// comment, one inside a one-line block comment, one inside a block comment that
// spans lines, and braces inside a string literal, a character literal and a
// literal carrying an escaped quote. The last of them closes a function on the
// same line as a digit-separated constant, whose quotes are not literal quotes.
// Three more sections hold the braces of a character literal that carries an
// encoding prefix -- L, u, U and u8 -- because the prefix is an identifier byte
// standing where a number's digit would stand, and the last of them holds one
// inside a block the body opened, where a miscounted brace makes the block's own
// close look like the section's. A section whose brace count is off by one either
// closes early, dropping the lines from there to the real close, or never closes
// at all; the fixture arranges one of each so a run that counts a literal's brace
// leaves this golden with nothing rewritten.
// Two more sections hold raw strings, whose braces nest no C++ either and among
// whose bytes no backslash escapes anything. One carries them on single lines: a
// brace inside a raw string, a raw string whose text holds an odd number of
// quotes next to a `/*`, a delimited raw string whose text holds the undelimited
// terminator, a raw string whose text holds a bare paren, each of the four
// encoding prefixes, and -- as the control -- an ordinary string whose text
// spells a raw string's opening. Reading the odd quote as the string's close
// leaves the `/*` outside every literal, the block comment then never ends, and
// the section never closes, so the whole file is refused at exit 1. The other
// section opens a raw string that spans lines and holds an unpaired brace, which
// is counted by any scan that does not carry the open literal from line to line.
// Three more sections hold an ordinary quoted literal, which spans lines when a
// backslash at the end of one escapes the newline. Two of them put an unpaired
// brace in the continuation: an opening one, which leaves the section unable to
// close and refuses the whole file, and a closing one, which ends the section in
// the middle of a string and writes the prototypes there at exit 0. The third is
// the control for the other half of the same rule -- a literal that reaches the
// end of its line with no backslash is a header cut mid-string and closes there,
// so the line below it is code.
// One section is written over two lines, the namespace and marker on one
// and the opening brace on the next, so the golden pins that the brace line comes
// back: it is not part of what the section replaces, and a rewrite that dropped
// it would return a namespace that is never opened at exit 0.
// Everything below the last section is the control: it is outside every
// section, so each of its lines must come back unchanged. The blank line
// standing there is outside what the golden can see, because the component
// harness logs no empty line, so whitespace below the last section is not
// pinned and a regression that only moved a blank line would still pass.
void atf_comp::comptest_src_func_SectionClose() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && cp $$OLDPWD/test/src_func/sectionclose.hdr hdr/x.h"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionclose.ssim -updateproto"
                        " && cat hdr/x.h'");
}

// Which namespace's functions an update-hdr section receives, over the shapes a
// marker line can take.
// A marker line may open more than one namespace -- `namespace outer { namespace
// inner {` -- and the section's body then stands inside the innermost of them.
// The prototypes written there are the ones whose definition names that
// namespace, and a definition inside outer::inner names inner, so reading the
// marker's outermost name selects outer's functions and declares them inside
// inner while inner's own functions stay undeclared. The header the next build
// reads then holds a prototype for a function that is defined nowhere.
// The fixture holds a section opening one namespace, one opening two, one opening
// three, and one whose marker line opens none because its brace stands on the
// line below; the source defines one function in every namespace those markers
// name. Each section therefore comes back holding exactly one prototype, and
// which function it is says which name the section was read as.
// Five further sections name their namespace in a spelling of its own: the C++17
// `namespace scope1::scope2 {`, whose innermost component is the name; an attribute
// standing between the keyword and the name; an attribute carrying a string
// argument; an attribute whose own name is qualified; and a comment standing between
// the keyword and the name. Five more carry text after the name that the compiler
// does not read as code -- a comment spelling a call and a semicolon, a line comment
// mentioning another namespace, a line comment whose last word is the keyword
// itself, and a quoted and a raw string literal each spelling a namespace line of
// their own -- and the name each of those lines gives its namespace stands outside
// that text.
// The source defines a function in each of those namespaces too, one in each of
// the namespaces a reader of the raw bytes names instead -- `deprecated`,
// `visibility`, `run`, `rest` and `bar` -- and one at global scope. Those six are
// the controls the spellings need. A name read out of an attribute or a remark
// selects that namespace's functions, and the section then declares them inside a
// namespace they are not defined in, so the golden pins each of the five as
// declared nowhere. A name the reader fails to extract comes back empty, an empty
// name selects the functions defined at global scope, and the golden pins the
// global function as declared nowhere for the same reason.
void atf_comp::comptest_src_func_SectionNs() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr src"
                        " && cp $$OLDPWD/test/src_func/sectionns.hdr hdr/x.h"
                        " && printf \"void solo::Solo() {\\n}\\nvoid outer::Outer() {\\n}\\nvoid inner::Inner() {\\n}\\nvoid deep1::Deep1() {\\n}\\nvoid deep2::Deep2() {\\n}\\nvoid deep3::Deep3() {\\n}\\nvoid twoline::Twoline() {\\n}\\nvoid scope2::Scope2() {\\n}\\nvoid attr::Attr() {\\n}\\nvoid cmt::Cmt() {\\n}\\nvoid attrarg::Attrarg() {\\n}\\nvoid attrqual::Attrqual() {\\n}\\nvoid cmtparen::Cmtparen() {\\n}\\nvoid cmtword::Cmtword() {\\n}\\nvoid lastword::Lastword() {\\n}\\nvoid litword::Litword() {\\n}\\nvoid rawword::Rawword() {\\n}\\nvoid deprecated::Deprecated() {\\n}\\nvoid visibility::Visibility() {\\n}\\nvoid run::Run() {\\n}\\nvoid rest::Rest() {\\n}\\nvoid bar::Bar() {\\n}\\nvoid Global() {\\n}\\n\" > src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionns.ssim -updateproto"
                        " && cat hdr/x.h'");
}

// An update-hdr marker line that names no namespace, which the run refuses rather
// than fill.
// The prototypes a section receives are the functions whose definition names the
// section's namespace, so a marker line whose name could not be read has no such
// set: an empty name selects the functions defined at global scope, and the
// section comes back declaring those inside whatever the marker opened. An
// anonymous namespace is the shape that reaches this on legal C++, and the file is
// left as it stands at exit 1. The second stage is the control, a named section
// that is rewritten at exit 0.
void atf_comp::comptest_src_func_SectionUnnamed() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr src"
                        " && printf \"void Global() {\\n}\\nvoid named::Named() {\\n}\\n\" > src/y.cpp"
                        " && try(){ printf \"$$2\" > hdr/x.h; cp hdr/x.h orig;"
                        " $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionns.ssim -updateproto;"
                        " echo $$1_code:$$?;"
                        " cmp -s hdr/x.h orig && echo $$1_header:unchanged || echo $$1_header:rewritten; };"
                        " try anon \"namespace { // update-hdr\\nvoid Old();\\n}\\n\";"
                        " try named \"namespace named { // update-hdr\\nvoid Old();\\n}\\n\";"
                        " cat hdr/x.h'");
}

// An update-hdr section whose closing brace stands on its own marker line, in the
// two shapes that produces.
// The prototypes a section receives go above the line that closes it, so a
// section that closes on the line it opened has nowhere to put them and the
// header cannot be rewritten. `namespace foo { void A(); } // update-hdr` is one
// such line, and a marker line that closes the namespace enclosing it --
// `namespace inner { void A(); }} // update-hdr` -- is the other; the second ends
// one level below where it began.
// What makes the shape worth a diagnostic of its own is what a run does with a
// section it never located the opening of: the depth the opening reaches is what
// tells section body from the lines standing below the section, so the first
// brace anywhere further down the file is taken for the section's own. The lines
// that nest under that brace are then dropped as body and the prototypes are
// written at its close, which for the second shape is a header that comes back
// with a struct's field deleted and a prototype block inside the struct, at exit
// 0. Each stage pins the exit code and the header byte-unchanged (cmp).
// A marker line that leaves a brace open which none of its namespaces opened is
// refused for a related reason, and the fixture carries the two shapes of it. In
// `namespace foo { struct S { // update-hdr` the section's depth is the struct's, so
// its close is located on the struct's `};` and the prototypes are written among the
// members while the members are dropped; in `namespace one { namespace two { } //
// update-hdr` the inner namespace the line names is closed again, leaving the depth
// one level below the namespace whose functions the section receives.
// Two stages are the controls, and both are rewritten at exit 0: one ordinary
// section, and one whose marker line opens a brace pair and closes it again --
// `namespace foo { struct S {}; // update-hdr` -- which is a section one level deep
// with a body below it, not a section that closed itself and not one holding a brace
// it does not own. That is the shape reading the marker line's deepest point as its
// end refuses, so the last stage's header is printed: the prototypes stand inside
// the section and the lines below it come back.
void atf_comp::comptest_src_func_SectionBodyless() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && try(){ printf \"$$2\" > hdr/x.h; cp hdr/x.h orig;"
                        " $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unterminated.ssim -updateproto;"
                        " echo $$1_code:$$?;"
                        " cmp -s hdr/x.h orig && echo $$1_header:unchanged || echo $$1_header:rewritten; };"
                        " try selfclose \"namespace foo { void A(); } // update-hdr\\nvoid KeepMe();\\n\";"
                        " try closeouter \"namespace outer {\\nnamespace inner { void A(); }} // update-hdr\\nnamespace zz {\\nstruct S {\\n    int a;\\n};\\n}\\n\";"
                        " try structopen \"namespace foo { struct S { // update-hdr\\n    int a;\\n};\\nvoid Old();\\n}\\n\";"
                        " try innerclosed \"namespace one { namespace two { } // update-hdr\\nvoid Old();\\n}\\n\";"
                        " try control \"namespace foo { // update-hdr\\nvoid A();\\n}\\n\";"
                        " try innerbrace \"namespace foo { struct S {}; // update-hdr\\nvoid A();\\n}\\nvoid KeepMe();\\n\";"
                        " cat hdr/x.h'");
}

// The shapes where the depth an update-hdr section reaches and the text of the
// line that reaches it disagree about where the section ends.
// A section's lines are dropped as the file is copied, and which lines those are is
// decided by one model of the file: the depth its braces reach. A brace the scan
// counts is not always a brace the compiler reads as nesting. `#define OPENBLOCK {`
// spells a block opener a macro's user will close somewhere else, and the two arms
// of a preprocessor conditional need not balance, so a section holding either one
// stands one level deeper than the count says. Its own closing brace then only
// takes the count back to the depth the marker opened, which is section body, and
// the copy goes on dropping lines until something further down takes the count
// lower. The hand-written lines in between are gone, and the header is saved at
// exit 0.
// The line's own text is the second description of the same event, and it is the
// one a miscount cannot move: a section is closed by the brace of the namespace its
// marker opened, and that brace is the first thing on its line. Where the two
// descriptions disagree the file is refused rather than rewritten, and the brace
// count reaching zero at end of file is the whole-file form of the same test.
// The fixture holds a stage per disagreement. A macro's brace inside a section
// whose enclosing namespace supplies the closing brace the count settles on is the
// shape that deletes hand-written lines below the section; a declaration sharing
// its line with the namespace's brace is the shape that leaves a hand-written
// declaration inside the region whose banner says it was scanned from sources; a
// macro's brace undone by a second macro below the section is the shape whose file
// balances and whose section still ends on the wrong line; and a stray brace
// standing after a section that was read correctly is refused too, which is what a
// test on the whole file costs. Two stages are the controls, and both are rewritten
// at exit 0: a macro's brace undone inside the section it stands in, where the
// count is wrong in the middle and right again at the close, and a struct declared
// in a section, whose `};` is a closing brace standing deeper than the marker and
// is body rather than the close. Each stage pins the exit code and, for the refused
// ones, the header byte-unchanged (cmp), and each control's file is printed: an exit
// code and the word `rewritten` say a rewrite happened and nothing about what it
// produced, so a run that swallowed the line below the section or wrote the
// prototypes at the wrong depth would satisfy them. Both controls come back with the
// section's hand-written lines replaced by the prototype banner, the section's own
// closing brace in place, and the hand-written line below the section standing.
void atf_comp::comptest_src_func_SectionUneven() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && try(){ printf \"$$2\" > hdr/x.h; cp hdr/x.h orig;"
                        " $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unterminated.ssim -updateproto;"
                        " echo $$1_code:$$?;"
                        " cmp -s hdr/x.h orig && echo $$1_header:unchanged || echo $$1_header:rewritten; };"
                        " try macro \"namespace outer {\\nnamespace foo { // update-hdr\\n#define OPENBLOCK {\\nvoid B();\\n}\\nvoid KeepMe();\\n}\\n\";"
                        " try sharedline \"namespace foo { // update-hdr\\nvoid B(); }\\nvoid KeepMe();\\n\";"
                        " try macroclose \"namespace foo { // update-hdr\\n#define OPENBLOCK {\\nvoid B();\\n}\\n#define CLOSEBLOCK }\\n\";"
                        " try strayafter \"namespace foo { // update-hdr\\nvoid A();\\n}\\n#define OPENBLOCK {\\n\";"
                        " try undone \"namespace foo { // update-hdr\\n#define BEGIN {\\n#define END }\\nvoid Old();\\n}\\nvoid KeepMe();\\n\"; cat hdr/x.h;"
                        " try nested \"namespace foo { // update-hdr\\nstruct S {\\n    int a;\\n};\\nvoid Old();\\n}\\nvoid KeepMe();\\n\";"
                        " cat hdr/x.h'");
}

// The rule that decides which lines open an update-hdr section, over one header
// carrying every shape it has to judge. A line opens a section only when the
// compiler reads the namespace and the marker tag as code, so the fixture holds
// one real marker and four look-alikes: a marker line inside a block comment
// that spans lines, which is what a hand edit produces when it comments a
// section out; a namespace line whose tag stands inside a block comment on the
// line itself; a namespace line that opens a block comment it never closes and
// carries the tag inside it, which is the same hand edit written on one line; and
// a namespace line whose tag stands inside a string literal. A fifth shape, a
// marker behind a `//`, is a look-alike the rule has always rejected. Three more
// hold the tag inside a raw string: one closed on its own line, one closed on its
// own line with a delimiter, one spanning lines whose text holds a whole
// `namespace zz { // update-hdr` line of its own, and one whose namespace line
// opens a raw string and puts the tag inside it. The third is the shape a
// line-at-a-time scan reads as a marker, because the line the tag stands on
// begins inside the literal and by itself looks like code; the fourth is the
// shape the tag's own test rejects, since a raw string is not the comment a line
// ends in and leaves the tag below the code the line carries. Reading any of them as a marker opens a section that was never
// meant to exist and drops every line from there to the next lone brace, so the
// golden pins the whole header: only the two real sections come back rewritten,
// and every other line, including the ones the look-alikes enclose and the
// struct below them, comes back byte for byte.
// One line carries the tag twice: `namespace requoted { /* was: // update-hdr */
// // update-hdr` quotes the marker in a remark and then writes the real one.
// That is the other side of the same rule, and it is the accepting side -- the
// line is a marker, since the tag stands in the trailing comment -- so the
// golden pins its section rewritten. A rule reading the first occurrence of the
// tag anywhere on the line refuses it instead, and refusing a marker leaves that
// section stale at exit 0 with nothing printed.
// A `//` comment whose line ends in a backslash is the fourth kind of span a line
// can begin inside, because C++ splices lines before it recognizes comments: the
// line below such a comment belongs to it entire, and so does the line below that
// one when it ends in a backslash too. The fixture holds a look-alike marker one
// line below such a comment and another two lines below one, and it holds the
// accepting side as well -- a real marker whose own trailing comment ends in a
// backslash, whose namespace and tag are code and whose section is rewritten.
// Which occurrence a line's attributes come from is the same question asked of the
// same line, so the requoted marker names a source in each of its two tags: the
// remark names the header itself, which defines no function, and the live tag names
// src/y.cpp, which defines one. The golden pins that function's prototype in the
// section under the banner of the source it came from, so a run that read the
// attributes from the quoted tag comes back with an empty section instead.
void atf_comp::comptest_src_func_SectionOpen() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr src"
                        " && cp $$OLDPWD/test/src_func/sectionopen.hdr hdr/x.h"
                        " && printf \"void requoted::Requoted() {\\n}\\n\" > src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionns.ssim -updateproto"
                        " && cat hdr/x.h'");
}

// A header whose first update-hdr section never closes and whose second one
// does. The run ends with no section open and one section updated, so a write
// gated on those two alone would save a copy that lost every line from the
// first marker to the second, including the second marker itself, while the
// diagnostic printed above it said the file was not rewritten. The test pins
// the file byte-unchanged (cmp) next to exit code 1.
void atf_comp::comptest_src_func_SectionOpenTwice() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && printf \"namespace c1 { // update-hdr\\nvoid Dropped();\\nnamespace c2 { // update-hdr\\n}\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionclose.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}
