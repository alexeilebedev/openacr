// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Header: include/src_func.h
//

#include "include/algo.h"
#include "include/gen/src_func_gen.h"
#include "include/gen/src_func_gen.inl.h"

namespace src_func { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/src_func/edit.cpp -- Implementation of -e
    //
    void Main_EditFunc();
    void Main_CreateMissing();

    // -------------------------------------------------------------------
    // cpp/src_func/fileloc.cpp -- Location in file, for each function
    //

    // Remember current file location
    void SaveFileloc(src_func::FTargsrc &targsrc, int lineno);

    // Get current file location in the form 'filename:lineno: '
    tempstr GetFileloc();

    // Get function file location in the form 'filename:lineno: '
    // Second argument specifies line offset within the function (starting with 0)
    tempstr Location(src_func::FFunc &func, int lineoffset);

    // -------------------------------------------------------------------
    // cpp/src_func/header.cpp -- Generate prototypes in header
    //

    // Extract the namespace of the update-hdr section a line like
    // 'namespace xyz { // update-hdr' opens.
    // A marker line may open more than one namespace -- `namespace outer { namespace
    // inner {` -- and the section's body then stands inside the innermost of them,
    // which is where the prototypes are written. The functions written there are the
    // ones whose definition names that namespace, and a definition inside
    // outer::inner names inner, so the innermost name is the one the insertion point
    // and the function selection agree on. Reading the outermost name instead
    // declares outer's functions inside inner, and leaves inner's own functions with
    // no declaration at all.
    // C++17 spells the same nesting as one namespace line, `namespace outer::inner {`,
    // and the innermost name is again the one to read: the section body stands inside
    // outer::inner either way, and a definition there names inner. So the name is read
    // as the last identifier of the definition's head -- the tokens from the keyword to
    // the brace that opens the body -- and every identifier the head carries after the
    // keyword replaces the one before it. That is what carries the attribute C++ admits
    // there: the words of `[[deprecated]]` are read and then replaced by the name that
    // follows them. A brace is what ends the head, since a marker line may carry code
    // of its own, as `namespace foo { void A(); }` does, whose identifiers name no
    // namespace.
    // Which bytes of the line are read is the second half of the rule, and the half a
    // scan over the raw line gets wrong. `namespace foo { // see namespace bar for the
    // rest // update-hdr` is an ordinary marker line carrying a remark, and a scan that
    // reads every identifier byte finds the keyword inside that remark, starts a second
    // head there, and comes back with `rest`. The section then receives the functions of
    // a namespace the header never mentions while foo's own stay undeclared, at exit 0.
    // An attribute carrying an argument is the same mistake read the other way round:
    // the parens of `namespace [[deprecated("use x")]] bar {` are not code standing
    // between the keyword and the name, and a scan that ends the head on a paren never
    // reaches `bar`.
    // The insight is that a namespace head is spelled in code, and a comment or a
    // literal on the line is text the compiler does not read as code. So the walk steps
    // over each span that is not code -- a block comment, a quoted or a raw string
    // literal, and the trailing line comment -- and reads identifiers and braces from
    // what is left. The marker tag needs no test of its own for the same reason: the tag
    // stands in a comment, and the walk stops where that comment begins. A marker line
    // begins outside every such span, which is one of the tests that made it a marker,
    // so the walk starts in code and needs no state carried in.
    // An empty answer means the line names no namespace at all, which the caller
    // refuses rather than fills; a section filled under an empty name receives the
    // functions defined at global scope.
    // NHEAD is set to the number of namespaces the line names, counting the keywords
    // the walk reads as code. It is how deep the section's own braces are meant to
    // reach, which a caller comparing it against the depth the opening line ends at
    // uses to tell a brace the section owns from one it does not.
    // Pinned by src_func.SectionNs and src_func.SectionUnnamed.
    strptr Nsline_GetNamespace(strptr line, int &nhead);

    // Go over all sources (includes & sources!)
    // Look for 'update-hdr' sections and insert function prototypes there.
    void Main_UpdateHeader();

    // -------------------------------------------------------------------
    // cpp/src_func/iffy.cpp -- Check iffy function prototypes
    //

    // Check if the function is iffy and set FUNC.IFFY flag if so.
    // Checks can be disabled with ignore: annotations in function pre-comment
    void ComputeIffy(src_func::FFunc &func);

    // -------------------------------------------------------------------
    // cpp/src_func/list.cpp -- Print list of functions
    //

    // Collect list of global function prototypes
    // in target TARGET with namespace NS
    void PrintGlobalProtos(src_func::FTarget &target, strptr ns, strptr srcfilter, cstring &out);
    void Main_ListFunc();

    // -------------------------------------------------------------------
    // cpp/src_func/main.cpp -- Main file
    //

    // Remove single-line C++ comment from file
    // and return result
    strptr StripComment(strptr line);

    // Return LINE with a leading 'template<...>' parameter list removed.
    // Name extraction locates a function's parameter list as the first paren on
    // the line; a template parameter list can carry parens of its own (e.g. the
    // function-pointer parameter in 'template<typename T, void (*F)(T)>'), which
    // that search would stop at, degenerating the extracted name. Skipping the
    // template list first -- by angle-bracket depth, with angle brackets inside
    // parens ignored so a comparison in a parenthesized default argument (e.g.
    // 'template<bool B = (N > 0)>') does not end the scan early -- leaves a line
    // the paren search handles like any non-template function.
    strptr StripTemplate(strptr line);

    // Get first line of function definition
    // Remove open curly
    // Replace DFLTVAL(x) with =x (for headers)
    // Replace FUNCATTR(x) with x (for headers)
    tempstr GetProto(src_func::FFunc &func);

    // Check if line contains function start
    // Criteria are:
    // - first character nonblank
    // - has parentheses
    // - last nonblank character is {
    // - it's not namespace, enum or struct
    bool FuncstartQ(strptr line, strptr trimmedline);

    // Extract function namespace name from FUNCLINE, a function's first line
    // void *ns::blah(arg1, arg2) -> ns
    // A leading template parameter list is skipped, so a caller may pass the raw
    // line or an already-stripped view (the skip is then a no-op).
    tempstr GetFuncNs(strptr funcline);

    // Filter functions based on parameters provided on command line.
    bool MatchFuncQ(src_func::FFunc &func);

    // The srcfile filter the update-hdr tag in NSCOMMENT names, and `%` when it names
    // none. NSCOMMENT is the comment a marker line ends in, which is where its tag and
    // the tag's attributes stand.
    // A marker line can carry the tag twice: `namespace foo { /* was: // update-hdr
    // srcfile:src/z.cpp */ // update-hdr srcfile:src/y.cpp` quotes an earlier marker in
    // a remark and writes the live one after it, and the two occurrences name different
    // sources. What makes such a line a marker is the tag in the comment it ends in, so
    // the attributes are read from that same occurrence: a search over the whole line
    // reaches the quoted tag first and fills the section from a source the live marker
    // does not name, at exit 0.
    // Pinned by src_func.SectionOpen, whose requoted marker names one source inside the
    // remark and another in the tag that opens its section.
    tempstr Nsline_GetSrcfile(strptr nscomment);
    void RewriteOpts();

    // Calculate a set of prefixes & suffixes (together:affixes)
    // which "look like generated code"
    // We will not generate prototypes for functions that look like generated code
    // (this is a heuristic, not a hard rule, but it saves hours of debugging when it works)
    // The reason is that if something changes in the underlying table where the userfunc
    // no longer gets generated (and thus expected) by amc, the user function should trigger
    // a compile error from lack of prototype.
    // Also, for each prototype that we refused to generate because a function matched
    // a known gen affix, we add a comment to the include file.
    void CalcGenaffix();
    src_func::FGenaffix *FindAffix(strptr cppname);

    // Main
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:src_func

    // -------------------------------------------------------------------
    // cpp/src_func/nextfile.cpp -- Find next file in target
    //

    // Find and print next/previous file
    void Main_Nextfile();

    // -------------------------------------------------------------------
    // cpp/src_func/scan.cpp -- Scan sources to collect functions
    //

    // Read functions from all sources
    void Main_ScanFiles();
}
