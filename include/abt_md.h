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
    // cpp/abt_md/file_section.cpp
    //

    // - Check that section doesn't have nested code-blocks (~~~) and preformatted blocks (```).
    // - Check that section doesn't have unbalanced code blocks and preformatted blocks
    // - Check that section doesn't contain headers of higher significance than the title line
    // (i.e. title line is ### but inside there is a ##)
    void CheckSection(abt_md::FFileSection &file_section);

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
    void Marktag(abt_md::FFileSection &section);

    // Execute commands marked by "inline-command: ..." inside backtick blocks,
    // and substitute their output into the section text
    void EvalInlineCommand(abt_md::FFileSection &file_section);

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

    // Load specified readme into memory as FILE_SECTION records
    // the section table is global and is wiped on every readme.
    // Only lines starting with ## or ### are considered FILE_SECTIONS
    void LoadSections(abt_md::FReadmefile &readmefile);

    // Print a single section to string
    void PrintSection(abt_md::FFileSection &file_section, cstring &out);

    // Print all sections to string OUT
    // Sections with empty text are deleted, except for title
    // (Note that contents of generated sections are not considered sections, so they may be empty
    // and will not be deleted)
    void PrintSections(cstring &out);

    // -------------------------------------------------------------------
    // cpp/abt_md/main.cpp
    //

    // Return true if readme file READMEFILE needs section MDSECTION
    bool NeedSectionQ(abt_md::FMdsection &mdsection, abt_md::FReadmefile &readmefile);

    // Extract words from line up until first dash
    // E.g. LineKey("#### Some Text - blah") -> "some-text"
    // This is similar to the algorithm used by other markdown processors
    // to generate anchors, but we allow '.' in anchor name
    tempstr LineKey(algo::strptr line);

    // Translate characters to create a markdown link
    // : is skipped
    // non-identifier characters are replaced with -
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

    // Link to internals documentation for given namespace (could be lib,protocol,exe,ssimdb)
    // The link text is NAME
    // the namespace is NS
    // Optional anchor is ANCHOR
    // For executables, a separate 'internals' file is used
    tempstr LinkToInternals(algo::strptr name, abt_md::FNs &ns, algo::strptr anchor = "");
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
    //     (user-implemented function, prototype is in amc-generated header)
    // void Main(); // main:abt_md

    // -------------------------------------------------------------------
    // cpp/abt_md/mdsection.cpp
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
    // void mdsection_Functions(abt_md::FFileSection &section); // gstatic/dev.mdsection:Functions

    // Update title of document
    // - For namespace, pull namespace name and comment from ns table
    // - For script, use script name and comment from scriptfile table
    // For all other cases, leave title as-is
    // Section contents are user-defined
    // void mdsection_Title(abt_md::FFileSection &section); // gstatic/dev.mdsection:Title

    // Update syntax string
    // Invoke command with -h flag and substitute output into section body
    // void mdsection_Syntax(abt_md::FFileSection &section); // gstatic/dev.mdsection:Syntax

    // Table of contents
    // for README file, create links to subdirectories
    // for non-README file, create links to sections inside file
    // The README.md files must form a tree covering all files.
    // Thus, non-README must not include links to other files in the same directory into ToC,
    // but can include those links outside of ToC
    // README.md must not include a link to internals.md on the same level (this link has to come
    // from above) to avoid contaminating ToC tree with unneeded details
    // void mdsection_Toc(abt_md::FFileSection &section); // gstatic/dev.mdsection:Toc

    // Create links to other files in the same directory
    // void mdsection_Chapters(abt_md::FFileSection &section); // gstatic/dev.mdsection:Chapters
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
    // void mdsection_Reftypes(abt_md::FFileSection &section); // gstatic/dev.mdsection:Reftypes
    // void mdsection_Subsets(abt_md::FFileSection &section); // gstatic/dev.mdsection:Subsets

    // Show related ssimfiles (those that reference this ssimfile NOT through pkey)
    // void mdsection_Related(abt_md::FFileSection &section); // gstatic/dev.mdsection:Related

    // Show related ssimfiles (those that reference this ssimfile NOT through pkey)
    // void mdsection_CmdlineUses(abt_md::FFileSection &section); // gstatic/dev.mdsection:CmdlineUses

    // Show related ssimfiles (those that reference this ssimfile NOT through pkey)
    // void mdsection_ImdbUses(abt_md::FFileSection &section); // gstatic/dev.mdsection:ImdbUses
    // void mdsection_Constants(abt_md::FFileSection &section); // gstatic/dev.mdsection:Constants
}
