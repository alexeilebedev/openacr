// Copyright (C) 2023 AlgoRND
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
    // cpp/abt/abt_md.cpp
    //

    // Load specified readme into memory as FILE_SECTION records
    void LoadReadme(abt_md::FReadme &readme);

    // Print a single section to string
    void PrintSection(abt_md::FileSection &file_section, cstring &out);

    // Print sections from readme matching command line regx
    void PrintReadme();

    // Update/evaluate specified section
    void UpdateSection(abt_md::FileSection &file_section);

    // Update loaded readme file:
    // - generate missing sections
    // - evaluate all commands using sandbox (if specified)
    // - save readme to disk
    void UpdateReadme();
    //     (user-implemented function, prototype is in amc-generated header)
    // void mdsection_Title(abt_md::FileSection &section);
    // void mdsection_Syntax(abt_md::FileSection &section);
    // void mdsection_Description(abt_md::FileSection &);
    // void mdsection_Content(abt_md::FileSection &);
    // void mdsection_Limitations(abt_md::FileSection &);
    // void mdsection_Example(abt_md::FileSection &);
    // void mdsection_Inputs(abt_md::FileSection &section);
    // void mdsection_Tests(abt_md::FileSection &section);
    // void mdsection_Copyright(abt_md::FileSection &);
    void Main();
}
