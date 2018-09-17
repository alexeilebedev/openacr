// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/arg.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

namespace algo { // update-hdr srcfile:%/algo/arg.%
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/arg.cpp -- Parse command-line
    //

    // Retrieve gitinfo string, e.g.
    // dev.gitinfo  gitinfo:2014-10-06.afa3edc.abt  author:alebedev@nyx.com  cfg:g++/release.Linux-x86_64  compver:4.4.6
    // This symbol is populated by abt using loader commands when building the executable
    // and reflects the most recent commit on top of which the build is being done
    strptr gitinfo_Get();

    // Scan argc/argv array, validate provided options,
    // and populate argtuple.attrs -- this is the Tuple that
    // will be ultimately scanned by <somecommand>_ReadStrptrMaybe
    // to read the command line
    // both -a:b and -a b syntaxes are supported
    // In the syntax string, [x]:y means x is anonymous
    // if there is no value, and proto-arg has default value, it is taken
    // for a flag, -a alone means value of 'true'.
    // There is an ambiguity here,
    // in that command line '-a true' will be interpreted as flag '-a' followed by
    // positional arg with value 'true'.
    void Argtuple_ReadArgv(algo::Argtuple &argtuple, int argc, char **argv, strptr syntax, strptr help_string);
}
