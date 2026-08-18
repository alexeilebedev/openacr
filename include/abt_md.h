// Copyright (C) 2023-2024,2026 AlgoRND
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
// Header: include/abt_md.h
//

#include "include/gen/abt_md_gen.h"
#include "include/gen/abt_md_gen.inl.h"

namespace abt_md { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/abt_md/file_section.cpp -- Read and rewrite a file's marked sections
    //

    // - Check that section doesn't have nested code-blocks (~~~) and preformatted blocks (```).
    // - Check that section doesn't have unbalanced code blocks and preformatted blocks
    // - Check that section doesn't contain headers of higher significance than the title line
    // (i.e. title line is ### but inside there is a ##)
    void CheckSection(abt_md::FFileSection &file_section);
    void RemoveHttpComments(abt_md::FFileSection &file_section);

    // Scan section and save human-entered text into hash ind_human_text
    // The sections where human text begins needs preserving are given by attribute dev.mdsection.genlist
    // Any line that matches genlist header string is the beginning of new human text
    // The key for the entry is given by Pathcomp(line,<key pathcomp>).
    void SaveHumanText(abt_md::FFileSection &file_section);

    // Scan section and restore any human text that has been saved before
    // This allows abt_md to easily re-generate section content headings
    // and re-apply human-written annotations from before.
    void RestoreHumanText(abt_md::FFileSection &file_section);

    // Insert anchors in FILE_SECTION, one for each section / subsection
    // (The anchors are stripped when loading the section for updating)
    // Delete any double blank links outside of code/preformatted blocks
    // In the end, if the section body doesn't contain any non-empty lines except anchors,
    // leave the section body empty (it will not be printed back to the file).
    void RewriteAnchors(abt_md::FFileSection &file_section);

    // Scan for links and anchors in section SECTION
    void ScanLinksAnchors();

    // Execute commands marked by "inline-command: ..." inside backtick blocks,
    // and substitute their output into the section text
    void EvalInlineCommand(abt_md::FFileSection &file_section);

    // Queue every command line in the current readme's sections for batch
    // validation by Main_RunBatchCheck.
    // A command line reaches a reader two ways: as a line of a bash or unlabeled
    // fenced block, and as an inline `...` span in prose. Both are collected --
    // a wrong flag misleads the reader wherever it is printed -- and a labeled
    // fenced block of some other language is not shell and is skipped whole.
    void CheckCommandLines();

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
    // section and for any subsection. Save the anchor to a global anchor table.
    // Find any links ([link text](link target)) and save them to the global link table
    // - Finally, check that the resulting section body doesn't have unbalanced backticks
    // (it's possible since the inline-command output might contain backticks).
    void UpdateSection(abt_md::FFileSection &file_section);
    bool TocQ(abt_md::FFileSection &section);
    bool TitleQ(abt_md::FFileSection &section);

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
    bool LoadSections(abt_md::FReadmefile &readmefile);

    // Print a single section to string
    void PrintSection(abt_md::FFileSection &file_section, cstring &out);

    // Print all sections to string OUT
    // Sections with empty text are deleted, except for title
    // (Note that contents of generated sections are not considered sections, so they may be empty
    // and will not be deleted)
    // A blank line separates sections; the file ends with a single newline (no trailing blank line).
    void PrintSections(cstring &out);

    // -------------------------------------------------------------------
    // cpp/abt_md/main.cpp -- Markdown linter and regenerator - main file
    //

    // Return true if readme file READMEFILE auto-generates section MDSECTION
    // (Any section is allowed but not all will be auto-updated)
    bool GenSectionQ(abt_md::FMdsection &mdsection, abt_md::FReadmefile &readmefile);

    // Extract words from line up until first dash
    // E.g. LineKey("#### Some Text - blah") -> "some-text"
    // This is similar to the algorithm used by other markdown processors
    // to generate anchors, but we allow '.' in anchor name
    tempstr LineKey(algo::strptr line);

    // Translate characters to create a markdown link
    // : is skipped
    // non-identifier characters are replaced with -
    // internal runs of - are collapsed to one
    // (so "A & B" and "A: B" both anchor as "a-b"; "-link" stays as "-link")
    // All characters are lowercased
    tempstr MdAnchor(algo::strptr str);
    algo::strptr FileIcon();
    algo::strptr FolderIcon();

    // Print anchor to OUT and add it to a global table
    void AddAnchor(algo::strptr name, cstring &out);
    tempstr Backticks(algo::strptr text);
    tempstr Preformatted(algo::strptr text);
    tempstr CodeBlock(algo::strptr text);

    // Return markdown link pointing to page URL and optional anchor ANCHOR
    // The displayed string is NAME
    tempstr Link(algo::strptr name, algo::strptr url, algo::strptr anchor = "");

    // Construct a link to file FNAME
    // using the first line of FNAME as the link text
    tempstr LinkToMd(strptr fname);

    // Link within repo to a given file
    // The URL is given as a relative pathname with respect to the root
    tempstr LinkToFileAbs(algo::strptr name, algo::strptr url, algo::strptr anchor = "");

    // Link to the md file for given ssimfile
    tempstr LinkToSsimfile(algo::strptr name, algo::strptr ssimfile, algo::strptr anchor = "");

    // Link to documentation for given namespace (could be lib,protocol,exe,ssimdb)
    tempstr LinkToNs(strptr ns, algo::strptr anchor = "");

    // Link to gen documentation for given namespace (could be lib,protocol,exe,ssimdb)
    // The link text is NAME
    // the namespace is NS
    // Optional anchor is ANCHOR
    // For executables and libraries, a separate gen file is used
    tempstr LinkToGen(algo::strptr name, abt_md::FNs &ns, algo::strptr anchor = "");
    tempstr LinkToReftype(algo::strptr reftype);
    tempstr LinkToCtype(abt_md::FCtype &ctype);
    tempstr TypeComment(abt_md::FCtype &ctype);

    // Compute base type, or return CTYPE if none
    abt_md::FCtype *GetBaseType(abt_md::FCtype &ctype);

    // Create an HTML comment
    tempstr MdComment(algo::strptr str);

    // Create table of contents link from string
    // The string should be a section heading for which markdown generates a link target;
    // We just repeat its algorithm and generate a link to that target
    tempstr LinkToSection(strptr str);

    // Populate global table DIRENT with a directory listing
    // matching PATTERN, that can be read in sorted order
    // (Dir_curs does not provide sorted order)
    void PopulateDirent(abt_md::FDirscan &dirscan, strptr pattern);
    int Sortkey(abt_md::FMdsection &mdsection, int i);

    // Determine header depth level of current line by counting leading #'s
    int GetHeaderLevel(strptr line);

    // Update loaded readme file:
    // - generate missing sections
    // - evaluate all commands using sandbox (if specified)
    // - save readme to disk
    void UpdateReadme();

    // Enforce structure of files under txt:
    // - txt/{nstype}/xyz.md implies that namespace 'xyz' must exist
    // - txt/script/xyz.md implies that file bin/xyz must exist
    // For namespaces, set FReadme.p_ns to point to the namespace,
    // and FNamespace.c_readme to point to the readme.
    // Skip files named README.md since they are index files
    // The same check is partially implemented via ssimreq, but ssimreq
    // canot handle exceptions like README.md
    void Main_XrefNs();
    void CheckLinks();
    void ProcessReadme(abt_md::FReadmefile& readmefile);

    // Flush queued FCheckReq rows through `acr_compl -check_batch` in a single
    // fork.  Requests are streamed to acr_compl via a temp file; acr_compl emits
    // one `acr_compl.checkerr` tuple per failing request on its stdout.  Each
    // failure is mapped back to its source location and reported as
    // `<gitfile>:<lineno>: <err>`, and the global exit code is set.  Any other
    // line the subprocess writes passes through to stderr unchanged.
    // A subprocess that exits abnormally has emitted no checkerr line for the
    // validations it never ran, so its wait status fails the run too.
    void Main_RunBatchCheck();

    // Report every dev.mdsection that claims to generate a section it can never
    // reach, and fail the run for each one.
    //
    // A row's path is a SQL-style regex over dev.readmefile.gitfile, and a
    // non-empty path is how the table says abt_md writes the section itself; the
    // idiom for a hand-written section is an empty path, which GenSectionQ tests
    // for. So a row whose path matches no readme at all claims generation and
    // delivers none, and nothing notices: the handler simply never runs, and
    // acr -check, abt_md -check and normalize all pass with the row in place,
    // while a reader of `acr mdsection` concludes the section is generated and
    // either hand-edits it expecting abt_md to overwrite the edit, or leaves a gap
    // expecting abt_md to fill it.
    //
    // The general fact is that a regex matching nothing looks exactly like a
    // regex whose matches are all fine, so the table can misdescribe the doc set
    // indefinitely. Requiring one match turns that silence into a failure at the
    // normalize gate, where the next such row is cheap to fix.
    //
    // The comparison needs a complete readmefile pool to mean anything.
    // dev.mdsection is gstatic, so every row is compiled in and present on every
    // run, while dev.readmefile is a finput holding whatever -in supplied. A run
    // pointed at a single fixture file loads a handful of readmes against the full
    // section table, so each row carrying a path reads as dead and the run fails
    // on the input rather than on the table. A directory input is a whole ssim
    // database and carries the tree's readmefile set, which is the only input this
    // check can draw a conclusion from.
    void Main_CheckMdsection();

    // The ssim tuple that selects the record KEY names, or empty when KEY names no
    // ssimfile this build knows or one whose pkey cannot be determined.
    // A key is written `<ssimfile>:<pkey>`, and `acr -sel` reads tuples, so the
    // key has to be turned into `<ssimfile>  <attr>:<pkey>`. The attribute is the
    // name of the ctype's first field, which is not always the ssimfile's own last
    // component: `x2db.gwproto` is keyed by `netproto`, so composing the attribute
    // from the ssimfile name sends a tuple with no primary key and acr answers
    // with nothing -- which reads as "no record has this key" and reports a
    // correct reference as broken.
    tempstr AcrKeyTuple(algo::strptr key);

    // Resolve the queued FCheckKey rows against the ssim database in a single fork
    // and report each key that names no record.
    // `acr -sel` reads tuples from stdin and prints the records it found, so one
    // fork answers "which of these keys exist" for the whole queue: a key is sent
    // as the tuple its ssimfile takes, and the reply reconstructs the key it
    // answers -- the ssimfile is the type tag and the pkey is the first attribute
    // -- so the rows that came back need no correlating id. What is left over is
    // the answer: a queued key acr did not return is a key no record has.
    // Reading the reply is what makes the check trustworthy, because `acr` reports
    // a key that resolves to nothing exactly as it reports one that resolves --
    // exit code 0, an empty selection -- so a check built on its status would pass
    // every key ever written.
    void Main_RunKeyCheck();

    // Select readme files by regex or namespace, process each one (generate
    // sections, evaluate inline commands, save), then check links and flush
    // the batched acr_compl validations; failed readme writes fail the run.
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:abt_md

    // -------------------------------------------------------------------
    // cpp/abt_md/mdsection.cpp -- Markdown section handlers - toc, inline command, links
    //
    void DescribeCtype(abt_md::FCtype *ctype, cstring &out);
    //     (user-implemented function, prototype is in amc-generated header)
    // void mdsection_Tables(abt_md::FFileSection &section); // gstatic/dev.mdsection:Tables
    // void mdsection_Attributes(abt_md::FFileSection &section); // gstatic/dev.mdsection:Attributes
    abt_md::FSsimfile *FieldSsimfile(abt_md::FCtype &ctype);
    void PopulateScanNs(abt_md::FNs &ns);
    //     (user-implemented function, prototype is in amc-generated header)
    // void mdsection_Inputs(abt_md::FFileSection &section); // gstatic/dev.mdsection:Inputs
    // void mdsection_InputMessages(abt_md::FFileSection &section); // gstatic/dev.mdsection:InputMessages
    abt_md::FCtype *GenerateFieldsTable(abt_md::FCtype &ctype, cstring &text_out, cstring &base_note);

    // Extract generated info and combine into a table
    //     (user-implemented function, prototype is in amc-generated header)
    // void mdsection_Imdb(abt_md::FFileSection &section); // gstatic/dev.mdsection:Imdb
    // void mdsection_Options(abt_md::FFileSection &section); // gstatic/dev.mdsection:Options
    // void mdsection_Ctypes(abt_md::FFileSection &section); // gstatic/dev.mdsection:Ctypes

    // Regenerate the Functions section by forking src_func.  This is one
    // fork per executable readme — for a full repo regen that's ~50.
    // evalcmd:N disables it so fast "refresh doc structure" runs don't
    // pay the fork tax; the previous Functions text is left in place.
    // void mdsection_Functions(abt_md::FFileSection &section); // gstatic/dev.mdsection:Functions

    // Update title of document
    // - For namespace, pull namespace name and comment from ns table
    // - For script, use script name and comment from scriptfile table
    // For all other cases, leave title as-is
    // Section contents are user-defined
    // void mdsection_Title(abt_md::FFileSection &section); // gstatic/dev.mdsection:Title

    // Update syntax section from FNs.help (pre-populated by
    // LoadHelpStrings on the first call).  No fork; no built binary
    // required.  evalcmd:N now only gates inline `cmd:…` blocks
    // elsewhere — the Syntax section is always refreshed because its
    // source is the generated file, not the binary.
    // void mdsection_Syntax(abt_md::FFileSection &section); // gstatic/dev.mdsection:Syntax

    // Table of contents
    // for README file, create links to subdirectories
    // for non-README file, create links to sections inside file
    // The README.md files must form a tree covering all files.
    // Thus, non-README must not include links to other files in the same directory into ToC,
    // but can include those links outside of ToC
    // void mdsection_Toc(abt_md::FFileSection &section); // gstatic/dev.mdsection:Toc

    // Create links to other files in the same directory
    // void mdsection_Chapters(abt_md::FFileSection &section); // gstatic/dev.mdsection:Chapters

    // Create links to other files in the same directory
    // void mdsection_Internals(abt_md::FFileSection &section); // gstatic/dev.mdsection:Internals
    // void mdsection_Sources(abt_md::FFileSection &section); // gstatic/dev.mdsection:Sources
    // void mdsection_Dependencies(abt_md::FFileSection &section); // gstatic/dev.mdsection:Dependencies
    // void mdsection_Description(abt_md::FFileSection &section); // gstatic/dev.mdsection:Description
    // void mdsection_Content(abt_md::FFileSection &); // gstatic/dev.mdsection:Content
    // void mdsection_Limitations(abt_md::FFileSection &); // gstatic/dev.mdsection:Limitations
    // void mdsection_Example(abt_md::FFileSection &); // gstatic/dev.mdsection:Example

    // Update tests section
    // Scan component tests for this namespace and print a table
    // void mdsection_Tests(abt_md::FFileSection &section); // gstatic/dev.mdsection:Tests

    // Update copyright section
    // void mdsection_Copyright(abt_md::FFileSection &); // gstatic/dev.mdsection:Copyright
    // void mdsection_Subsets(abt_md::FFileSection &section); // gstatic/dev.mdsection:Subsets

    // Show related ssimfiles (those that reference this ssimfile NOT through pkey)
    // void mdsection_Related(abt_md::FFileSection &section); // gstatic/dev.mdsection:Related

    // Show related ssimfiles (those that reference this ssimfile NOT through pkey)
    // void mdsection_CmdlineUses(abt_md::FFileSection &section); // gstatic/dev.mdsection:CmdlineUses

    // Show related ssimfiles (those that reference this ssimfile NOT through pkey)
    // void mdsection_ImdbUses(abt_md::FFileSection &section); // gstatic/dev.mdsection:ImdbUses
    // void mdsection_Constants(abt_md::FFileSection &section); // gstatic/dev.mdsection:Constants
}
