// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

// Run src_func -updateproto against a header in a directory that denies the write, and
// check that the update-hdr block rewrite fails the run and names the path.  The seed
// puts junk inside the update-hdr block, so the rewrite has a change to make.  An exit
// code of 0 would leave the header stale with no word of it.
void atf_comp::comptest_src_func_FileWriteFail() {
    atf_comp::ProcStart(DenyWriteCmd("printf \"namespace xx { // update-hdr\\njunk\\n}\\n\" > ro/x.h",
                                     "$$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/writefail.ssim -updateproto"));
}

// Run src_func over a targsrc source file that does not exist, and check that the scan
// fails the run and names the path.  A scan that indexed the file as empty and exited
// 0 would drop its functions from every listing and header with no word of it.
void atf_comp::comptest_src_func_FileReadFail() {
    atf_comp::ProcStart("$bindir/src_func -in:test/src_func/readfail.ssim");
}

// Run src_func -updateproto against a targsrc header that does not exist, and check
// that the rewrite pass fails the run and names the path.  An exit code of 0 would
// leave the header's update-hdr block stale with no word of it.
void atf_comp::comptest_src_func_UpdateprotoReadFail() {
    atf_comp::ProcStart("$bindir/src_func -in:test/src_func/hdr_readfail.ssim -updateproto");
}

// Run src_func -createmissing with a targsrc source file that does not exist, and
// check that it skips the stub-creation pass entirely:
// 1. Write src/y.cpp with xx::ExistingFunc, and copy it aside.
// 2. Run src_func -createmissing over a universe that declares userfunc xx.NewFunc
//    with no scanned definition.
// 3. Check that the run exits with code 1 and that src/y.cpp stays byte-unchanged
//    (cmp).
// The failed scan leaves the function index incomplete.  An ungated pass would append
// a stub for xx.NewFunc to the healthy source, duplicating a definition that the scan
// never saw.
void atf_comp::comptest_src_func_CreatemissingScanFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"void xx::ExistingFunc() {\\n}\\n\" > src/y.cpp"
                        " && cp src/y.cpp orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/createmissing_readfail.ssim -createmissing;"
                        " test $$? = 1 && cmp src/y.cpp orig && echo source_unchanged'");
}

// Run src_func -updateproto against a header whose second update-hdr section is
// unterminated after the first one closed.  Check that the run names the file and
// exits with code 1, and that the header stays byte-unchanged (cmp).  A rewrite would
// drop everything from the unterminated marker to EOF and write the truncation back.
void atf_comp::comptest_src_func_UpdateprotoUnterminated() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && printf \"namespace xx { // update-hdr\\n}\\nnamespace yy { // update-hdr\\nvoid Dropped();\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unterminated.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}

// Run src_func -updateproto over a targsrc source that ends inside a function, with no
// closing brace at column 0.  Check that the run exits with code 1 and that the header
// stays byte-unchanged (cmp).  The scan is incomplete, so src_func must fail the run
// and skip the header rewrite.  A rewrite would drop the truncated file's functions
// from the header.
void atf_comp::comptest_src_func_UnfinishedFunc() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src hdr"
                        " && printf \"void xx::Foo() {\\n    body();\\n\" > src/y.cpp"
                        " && printf \"namespace xx { // update-hdr\\nvoid OldFunc();\\n}\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unfinished.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}

// Scan a function whose template parameter list holds the parenthesized default
// argument 'bool B = (4 > 2)' next to a function-pointer parameter, beside a plain
// fn-ptr template.  Print the index rows, and check that they key the functions as
// xx.Paren and xx.Fnptr.  If the '>' inside the parens ended the template-list strip
// early, the name extraction would fall into the fn-ptr paren and key the function as
// the bare namespace.
void atf_comp::comptest_src_func_TemplateParenDefault() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"template<bool B = (4 > 2), void (*F)(int)> inline void xx::Paren(int a) {\\n}\\ntemplate<typename T, void (*F)(T)> inline void xx::Fnptr(T t) {\\n}\\n\" > src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/template.ssim % -printssim'");
}

// Scan one function per return-type shape with src_func -iffy -list, and check which
// functions the iffy 'bigret' check reports, as the table below says.  The check
// reports a function that returns cstring by value.  It reads the return type as
// whatever stands left of the parameter list, and finds the parameter list as the
// first paren on the line.  A template parameter list can carry parens of its own, as
// the function-pointer parameter in 'template<typename T, void (*F)(T)>' does.  So the
// paren search has to run on the line with the template list stripped.  Read from the
// raw line, the type comes out of the template list.  The check then fires on sound
// functions and stays silent on faulty ones.  The fixture holds the whole table of
// return-type shapes, controls next to rejects, in source order:
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

// Run src_func -updateproto with a targsrc source file that does not exist, and check
// that the run exits with code 1 and leaves the header byte-unchanged (cmp).  The seed
// puts a prototype inside the header's update-hdr block.  The failed scan leaves the
// function index incomplete, so src_func must skip the header rewrite entirely.  A
// rewrite from the incomplete index would strip the prototype.
void atf_comp::comptest_src_func_UpdateprotoScanFail() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && printf \"namespace xx { // update-hdr\\nvoid OldFunc();\\n}\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/scan_readfail.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}

// Load gendb.cppsym keys across the whole 250-character width, and check that each
// key stays its own row:
// 1. Write four functions to src/y.cpp.
// 2. Run src_func -printssim over cppsym_width.ssim, which holds a short key, two keys
//    that agree for their first fifty characters and differ past it, and a key at the
//    250-character limit.  Check that all four load as distinct rows, and that each
//    binds to its own scanned function and prints with its own acrkey.
// 3. Run src_func over cppsym_toolong.ssim, whose key runs one character over the
//    limit.  Check that src_func rejects the key naming the limit, and exits with
//    code 1.
// A gendb.cppsym key is a C++ name and a record key, <cppname>/<acrkey>, and the field
// is 250 characters wide.  Two user functions whose names agree for a long stretch must
// stay two rows.  Otherwise one registration disappears, and its function loses the
// record it binds to.
void atf_comp::comptest_src_func_CppsymKeyWidth() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir src"
                        " && printf \"void xx::ShortName() {\\n}\\nvoid xx::NearTwinKeyThatAgreesUpToTheFiftiethCharacterThenAlpha() {\\n}\\nvoid xx::NearTwinKeyThatAgreesUpToTheFiftiethCharacterThenBravo() {\\n}\\nvoid xx::CppsymKeyExactlyAtTheLimitPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadPadP() {\\n}\\n\" > src/y.cpp"
                        " && cp $$OLDPWD/test/src_func/cppsym_width.ssim $$OLDPWD/test/src_func/cppsym_toolong.ssim ."
                        " && $$OLDPWD/$bindir/src_func -in:cppsym_width.ssim % -printssim"
                        " && $$OLDPWD/$bindir/src_func -in:cppsym_toolong.ssim % -printssim;"
                        " test $$? = 1 && echo over_limit_rejected'");
}

// Scan one function per comment-line shape with src_func -updateproto, print the
// header, and check which of each function's comment lines the header mirrors, as the
// table below says.  The fixture holds the shapes in source order.
// The comment lines above a function belong to that function.  The header's update-hdr
// block mirrors them.  The divider line this repo puts between functions belongs to
// no function, so it ends the comment accumulated above it.  A comment line qualifies
// as a divider by its whole form, and a substring of it never suffices.  An ascii
// table inside a leading comment draws a rule line out of dashes.  Read as a divider,
// that rule drops everything above the table from the mirrored header with no word of
// it: the paragraph saying what the function does, and the table's own header row.  So
// a rule line breaks its dashes with some other character, as every table in the tree
// does.  The last function of the fixture pins the boundary, because a rule line of
// dashes alone still reads as a divider.
//
// A banner of slashes is the same rule read over the other divider this repo draws.
// The fixture writes it both as a bare run of slashes and as a comment that holds one,
// `// ` and then the run.  The two banners in the tree use the second form.  A run of
// slashes with a word among them is a section title.  It is comment text for the
// reason a rule line broken by a word is.
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
//   a table rule of dashes alone             | below the rule
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
// writes the prototypes again, at exit 0.
// No section body here carries a brace, since a body that does holds hand-written
// code and is refused (src_func.SectionNested). Three sections hold braces that
// nest no C++ and must not be counted: one inside a line comment, one inside a
// one-line block comment, one inside a block comment that spans lines, one inside
// a line comment standing after a declaration, and braces inside a string
// literal, a character literal and a literal carrying an escaped quote. The last
// of them stands on the same line as a digit-separated constant, whose quotes are
// not literal quotes.
// Three more sections hold the braces of a character literal that carries an
// encoding prefix -- L, u, U and u8 -- because the prefix is an identifier byte
// standing where a number's digit would stand, and the last of them stands after
// a string literal on the same line. A body line whose literal brace is counted
// either refuses the whole file or closes the section on a line that does not
// begin with a brace, which refuses it too, so a run that miscounts leaves this
// golden with nothing rewritten.
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

// Rewrite a header whose update-hdr sections cover the shapes a marker line can take,
// and check which namespace's functions each section receives:
// 1. Copy sectionns.hdr to hdr/x.h.  Write src/y.cpp with one function in every
//    namespace the markers name, one in each namespace that a reader of the raw bytes
//    would name, and one at global scope.
// 2. Run src_func -updateproto, and print hdr/x.h.
// 3. Check that four sections each come back holding exactly one prototype: one that
//    opens one namespace, one that opens two, one that opens three, and one whose
//    marker line opens none because its brace stands on the line below.  The function
//    in each section says which name src_func read the section as.
// 4. Check the same for five sections that name their namespace in a spelling of its
//    own.  The spellings are the C++17 `namespace scope1::scope2 {`, whose innermost
//    component is the name; an attribute between the keyword and the name; an
//    attribute that carries a string argument; an attribute whose own name is
//    qualified; and a comment between the keyword and the name.
// 5. Check the same for five sections that carry comment or literal text after the
//    name.  The texts are a comment that spells a call and a semicolon; a line comment
//    that mentions another namespace; a line comment whose last word is the keyword
//    itself; and a quoted and a raw string literal, each of which spells a namespace
//    line of its own.  The name each line gives its namespace stands outside that
//    text.
// 6. Check that no section declares the functions in `deprecated`, `visibility`,
//    `run`, `rest` and `bar`, the namespaces a reader of the raw bytes names, or the
//    function at global scope.
// A marker line may open more than one namespace, as in
// `namespace outer { namespace inner {`.  The section's body then stands inside the
// innermost of them.  The prototypes written there are the ones whose definition names
// that namespace.  A definition inside outer::inner names inner.  So a run that reads
// the marker's outermost name selects outer's functions and declares them inside
// inner, while inner's own functions stay undeclared.  The header that the next build
// reads then holds a prototype for a function defined nowhere.
// The six functions of step 6 are the controls the spellings need.  A name read out of
// an attribute or a remark selects that namespace's functions.  The section then
// declares them inside a namespace they are not defined in.  A name the reader fails
// to extract comes back empty.  An empty name selects the functions defined at global
// scope, so the global function is a control for the same reason.
void atf_comp::comptest_src_func_SectionNs() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr src"
                        " && cp $$OLDPWD/test/src_func/sectionns.hdr hdr/x.h"
                        " && printf \"void solo::Solo() {\\n}\\nvoid outer::Outer() {\\n}\\nvoid inner::Inner() {\\n}\\nvoid deep1::Deep1() {\\n}\\nvoid deep2::Deep2() {\\n}\\nvoid deep3::Deep3() {\\n}\\nvoid twoline::Twoline() {\\n}\\nvoid scope2::Scope2() {\\n}\\nvoid attr::Attr() {\\n}\\nvoid cmt::Cmt() {\\n}\\nvoid attrarg::Attrarg() {\\n}\\nvoid attrqual::Attrqual() {\\n}\\nvoid cmtparen::Cmtparen() {\\n}\\nvoid cmtword::Cmtword() {\\n}\\nvoid lastword::Lastword() {\\n}\\nvoid litword::Litword() {\\n}\\nvoid rawword::Rawword() {\\n}\\nvoid deprecated::Deprecated() {\\n}\\nvoid visibility::Visibility() {\\n}\\nvoid run::Run() {\\n}\\nvoid rest::Rest() {\\n}\\nvoid bar::Bar() {\\n}\\nvoid Global() {\\n}\\n\" > src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionns.ssim -updateproto"
                        " && cat hdr/x.h'");
}

// Rewrite a header whose update-hdr marker line names no namespace, and check that
// src_func refuses to fill it:
// 1. Write src/y.cpp with a function Global at global scope and named::Named.
// 2. In stage anon, write `namespace { // update-hdr` to hdr/x.h, and run
//    src_func -updateproto.  Check that the run exits with code 1 and leaves the
//    header as it stands.
// 3. In stage named, the control, write `namespace named { // update-hdr` to hdr/x.h,
//    and run src_func -updateproto.  Check that the run rewrites the header at exit 0,
//    and print it.
// The prototypes a section receives are the functions whose definition names the
// section's namespace.  A marker line whose name src_func fails to read has no such
// set.  An empty name selects the functions defined at global scope.  The section
// would then come back declaring those inside whatever the marker opened.  An
// anonymous namespace is the shape of legal C++ that reaches this case.
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

// Rewrite headers whose update-hdr marker line closes its own section or leaves open a
// brace that none of its namespaces opened, and check that src_func refuses each:
// 1. In stage selfclose, run src_func -updateproto on
//    `namespace foo { void A(); } // update-hdr`.  Check that the run exits with
//    code 1 and leaves the header byte-unchanged (cmp).
// 2. In stage closeouter, do the same for a marker line that closes the namespace
//    enclosing it, `namespace inner { void A(); }} // update-hdr`.  This line ends one
//    level below where it began.
// 3. In stage structopen, do the same for `namespace foo { struct S { // update-hdr`.
// 4. In stage innerclosed, do the same for
//    `namespace one { namespace two { } // update-hdr`.
// 5. In stage control, run src_func -updateproto on an ordinary section, and check
//    that src_func rewrites it at exit 0.
// 6. In stage innerbrace, run it on `namespace foo { struct S {}; // update-hdr`, and
//    check that src_func rewrites the header at exit 0.  Print the header, and check
//    that the prototypes stand inside the section and the lines below it come back.
// A section's prototypes go above the line that closes it.  A section that closes on
// the line it opened leaves them no place, so src_func refuses to rewrite the header.
// The shape earns a diagnostic of its own because of what a run does with a section
// whose opening it never located.  The depth the opening reaches tells section body
// from the lines below the section.  Without it, the run takes the first brace
// anywhere further down the file for the section's own.  It drops the lines that nest
// under that brace as body and writes the prototypes at its close.  For the shape of
// step 2, the header comes back at exit 0 with a struct's field deleted and a
// prototype block inside the struct.
// src_func refuses the marker lines of steps 3 and 4 for a related reason, because
// each leaves open a brace that none of its namespaces opened.  In step 3 the
// section's depth is the struct's, so a run would locate the close on the struct's
// `};`.  It would write the prototypes among the members and drop the members.  In
// step 4 the line closes the inner namespace it names again.  The depth then stands
// one level below the namespace whose functions the section receives.
// The marker line of step 6 opens a brace pair and closes it again.  It is a section
// one level deep with a body below it.  A rule that reads the marker line's deepest
// point as its end refuses this shape, so the test prints the last stage's header.
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
// counts is not always a brace the compiler reads as nesting -- `#define OPENBLOCK
// {` spells a block opener that a macro's user closes somewhere else -- so the
// line's own text is the second description of the same event. A section is closed
// by the brace of the namespace its marker opened, and that brace is the first
// thing on its line. Where the two descriptions disagree the file is refused rather
// than rewritten, and the brace count reaching zero at end of file is the
// whole-file form of the same test.
// The fixture holds a stage per disagreement. A declaration sharing its line with
// the namespace's brace is the shape that leaves a hand-written declaration inside
// the region whose banner says it was scanned from sources, and a stray brace
// standing after a section that was read correctly is refused too, which is what a
// test on the whole file costs. A macro's brace inside the section is refused
// before either test sees it, because a body line carrying a brace is hand-written
// code (src_func.SectionNested). The last stage is the control: an ordinary section
// with a hand-written line below it, rewritten at exit 0 and printed, so the golden
// pins the prototype banner inside the section and the line below it standing.
// Each refused stage pins the exit code and the header byte-unchanged (cmp).
void atf_comp::comptest_src_func_SectionUneven() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && try(){ printf \"$$2\" > hdr/x.h; cp hdr/x.h orig;"
                        " $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unterminated.ssim -updateproto;"
                        " echo $$1_code:$$?;"
                        " cmp -s hdr/x.h orig && echo $$1_header:unchanged || echo $$1_header:rewritten; };"
                        " try sharedline \"namespace foo { // update-hdr\\nvoid B(); }\\nvoid KeepMe();\\n\";"
                        " try strayafter \"namespace foo { // update-hdr\\nvoid A();\\n}\\n#define OPENBLOCK {\\n\";"
                        " try control \"namespace foo { // update-hdr\\nvoid Old();\\n}\\nvoid KeepMe();\\n\"; cat hdr/x.h'");
}

// A section whose body holds hand-written code that carries a brace, which the rewrite
// would drop and no later run could write again.
// The body of an update-hdr section is replaced by the banner and the prototypes,
// and neither carries a brace. So a body line that carries one is hand-written,
// and the section is refused with the file left as it stands. The stages are an
// inline definition on one line, the same definition with its body on three lines
// and its closing brace on a line of its own, a struct, a pair of macros that spell
// a block's braces, a namespace the body opens, and a line that closes the marker's
// namespace and opens another one, which ends level with where it began. Each
// refused stage pins the exit code and the header byte-unchanged (cmp), since an
// exit code alone says nothing about what was written. The last stage is the
// control: the same header with the definition moved below the section, rewritten
// at exit 0 and printed, so the golden shows the definition standing.
void atf_comp::comptest_src_func_SectionNested() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && try(){ printf \"$$2\" > hdr/x.h; cp hdr/x.h orig;"
                        " $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/unterminated.ssim -updateproto;"
                        " echo $$1_code:$$?;"
                        " cmp -s hdr/x.h orig && echo $$1_header:unchanged || echo $$1_header:rewritten; };"
                        " try oneline \"namespace xx { // update-hdr\\ninline int Bar() { return 1; }\\n}\\n\";"
                        " try threeline \"namespace xx { // update-hdr\\ninline int Bar() {\\n    return 1;\\n}\\n}\\n\";"
                        " try struct \"namespace xx { // update-hdr\\nstruct S {\\n    int a;\\n};\\nvoid Old();\\n}\\n\";"
                        " try macro \"namespace xx { // update-hdr\\n#define BEGIN {\\n#define END }\\nvoid Old();\\n}\\n\";"
                        " try namespace \"namespace xx { // update-hdr\\nnamespace body {\\nvoid Old();\\n}\\n}\\n\";"
                        " try reopen \"namespace xx { // update-hdr\\n} namespace yy {\\nvoid Old();\\n}\\n\";"
                        " try control \"namespace xx { // update-hdr\\n}\\nnamespace xx {\\ninline int Bar() { return 1; }\\n}\\n\"; cat hdr/x.h'");
}

// Rewrite one header that holds every shape the update-hdr section-open rule has to
// judge, and check that only the real markers open a section:
// 1. Copy sectionopen.hdr to hdr/x.h, and write src/y.cpp with requoted::Requoted.
// 2. Run src_func -updateproto, and print hdr/x.h.
// 3. Check that four look-alike markers open no section.  One stands inside a block
//    comment that spans lines, as a hand edit leaves it when it comments a section
//    out.  One has its tag inside a block comment on the namespace line itself.  One
//    opens a block comment that it never closes and carries the tag inside it, the
//    same hand edit written on one line.  One has its tag inside a string literal.  A
//    fifth shape, a marker behind a `//`, is a look-alike that the rule rejects too.
// 4. Check that the lines with the tag inside a raw string open no section: one
//    closed on its own line, one closed on its own line with a delimiter, one that
//    spans lines and whose text holds a whole `namespace zz { // update-hdr` line,
//    and one whose namespace line opens a raw string and puts the tag inside it.
// 5. Check that two look-alike markers after a `//` comment that ends in a backslash
//    open no section.  One stands one line below such a comment, and the other two
//    lines below one.
// 6. Check that src_func rewrites the section of
//    `namespace requoted { /* was: // update-hdr */ // update-hdr`, which quotes the
//    marker in a remark and then writes the real one.  Check that the section holds
//    the prototype of requoted::Requoted under the banner of src/y.cpp.
// 7. Check that src_func rewrites the section of a real marker whose own trailing
//    comment ends in a backslash.
// 8. Check that every other line comes back byte for byte, including the lines that
//    the look-alikes enclose and the struct below them.
// A line opens a section only when the compiler reads the namespace and the marker tag
// as code.  Read as a marker, a look-alike opens a section that nobody meant to open.
// The rewrite then drops every line from there to the next lone brace.
// In step 4, a line-at-a-time scan reads the spanning raw string as a marker, because
// the line that holds the tag begins inside the literal and looks like code by itself.
// The tag's own test rejects the last raw-string shape, because a tag counts only in
// the comment that ends its line.  A raw string is code, so the tag inside it stays
// below the code that the line carries.
// The marker of step 6 is the accepting side of the same rule, because its tag stands
// in the trailing comment.  A rule that reads the first occurrence of the tag anywhere
// on the line refuses this marker.  The section then stays stale at exit 0 with no
// word of it.  Each of the two tags names a source, because the attributes come from
// the same occurrence that the rule picks.  The remark names the header itself, which
// defines no function.  The live tag names src/y.cpp, which defines one.  A run that
// read the attributes from the quoted tag would come back with an empty section.
// A `//` comment whose line ends in a backslash is the fourth kind of span that a line
// can begin inside, because C++ splices lines before it recognizes comments.  The line
// below such a comment belongs to it entire.  So does the line after that one, when
// the line below also ends in a backslash.  The marker of step 7 is the accepting side
// of this rule, because its namespace and tag are code.
void atf_comp::comptest_src_func_SectionOpen() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr src"
                        " && cp $$OLDPWD/test/src_func/sectionopen.hdr hdr/x.h"
                        " && printf \"void requoted::Requoted() {\\n}\\n\" > src/y.cpp"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionns.ssim -updateproto"
                        " && cat hdr/x.h'");
}

// Run src_func -updateproto on a header whose first update-hdr section never closes
// and whose second one does.  Check that the run exits with code 1 and leaves the file
// byte-unchanged (cmp).  The run ends with no section open and one section updated.  A
// write gated on those two facts alone would save a copy that lost every line from the
// first marker to the second, the second marker included.  The diagnostic printed
// above it would still say that src_func left the file as it was.
void atf_comp::comptest_src_func_SectionOpenTwice() {
    atf_comp::ProcStart("bash -c 'cd $tempdir && mkdir hdr"
                        " && printf \"namespace c1 { // update-hdr\\nvoid Dropped();\\nnamespace c2 { // update-hdr\\n}\\n\" > hdr/x.h"
                        " && cp hdr/x.h orig"
                        " && $$OLDPWD/$bindir/src_func -in:$$OLDPWD/test/src_func/sectionclose.ssim -updateproto;"
                        " test $$? = 1 && cmp hdr/x.h orig && echo header_unchanged'");
}
