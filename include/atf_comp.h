// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
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
// Target: atf_comp (exe) -- Algo Test Framework - Component test execution
// Exceptions: yes
// Header: include/atf_comp.h
//

#include "include/algo.h"
#include "include/gen/atf_comp_gen.h"
#include "include/gen/atf_comp_gen.inl.h"

namespace atf_comp { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf_comp/edit.cpp -- Edit comptest
    //

    // Open selected testcases in an editor
    // Upon completion, replace all selected testcases with new data
    void Main_EditSelected();

    // -------------------------------------------------------------------
    // cpp/atf_comp/main.cpp
    //

    // Compute component directory depending on config given on command line
    void Main_GuessCompdir();

    // Run abt for build or ood check for config given on command line
    // -cfg, -build, and -ood options are passed directly to abt
    void Main_Ood();

    // Running instrumented executable or under dynamic analyzer
    // may slow down execution greatly. Timeout given on the comptest record is
    // 3 * typical elapsed time for running release executable without any tool.
    // This functios applies known slowdown figures for various configurations and tools.
    int GetTimeout(atf_comp::FComptest &comptest);

    // Initialize COMPTEST.BASH.CMD to the command that will execute
    // If invoked with -memcheck, wrap the command line with valgrind
    // and initialize COMPTEST.FILE_MEMCHECK pathname
    // If invoked with -callgrind, wrap the command line with valgrind
    // and initialize COMPTEST.FILE_CALLGRIND_LOG, COMPTEST.FILE_CALLGRIND_OUT pathname
    // If invoked with -coverage, wrap the command line with atf_cov
    // and initialize COMPTEST.COVDIR
    //
    void SetupCmdline(atf_comp::FComptest &comptest, algo_lib::Replscope &R);

    // Initialize COMPTEST.FILE_TEST_IN
    // Write input messages to file
    void PrepareInput(atf_comp::FComptest &comptest);
    void Comptest_Start(atf_comp::FComptest &comptest);

    // Start next test
    //     (user-implemented function, prototype is in amc-generated header)
    // void zd_sel_comptest_Step();

    // Compare output of current test with the reference file.
    // If tfilt exists, filter output before matching
    // Any difference = error
    // return true for success, false for error
    bool CompareOutput(atf_comp::FComptest &comptest);

    // get number of memory errors for testcase.
    // valgrind memcheck log example:
    // ==664953== ERROR SUMMARY: 7 errors from 6 contexts (suppressed: 0 from 0)
    // ~~~
    // returns number of errors, in this case - 7
    i32 GetNumMemoryErrors(atf_comp::FComptest &comptest);

    // Cleanup test structure after run
    void Comptest_Finish(atf_comp::FComptest &comptest);

    // Attempted whenever we suspect that waitpid()
    // will return something interesting
    // Decreases # of jobs
    //     (user-implemented function, prototype is in amc-generated header)
    // void zd_run_comptest_Step();

    // Set up signal handler to detect process exits
    void Main_SetupSigchild();

    // Print selected comptests using a format compatible with Main_Read
    // to string OUT.
    void Main_Print(cstring &out, bool show_output, bool show_testcase);

    // Merge outputs of multiple atf_cov runs into a single report.
    void Main_Coverage();

    // Rewrite / normalize tags in all tmsgs
    // If a tmsg corresponds to an unknown type, the message is deleted
    void Main_Normalize();

    // Write all testcases from list zd_out_comptest back
    // to where they were read from
    void Main_Write();

    // Select comptests matching regx from command line
    // If using insert mode (-i), deselect any testcases not inserted from stdin
    // If no comptests selected, print warning
    void Main_Select();
    void Main_Debug();
    void Main_RewriteCmdline();
    void Main();

    // -------------------------------------------------------------------
    // cpp/atf_comp/read.cpp -- Read comptest from input
    //

    // Read FD, create testcases based on the following mini-language
    // comptest <name> <args> -- component test name & command-line arguments
    // comment <text>    -- comment
    // exit_code <code>  -- exit code
    // filter <code>     -- output filter
    // expect <msg>      -- expected outputline
    // <anything else>   -- input message
    // New testcases are added to zd_out_comptest, and will be subject
    // to selection in a subsequent step
    void Main_Read(algo::Fildes fd);
}
