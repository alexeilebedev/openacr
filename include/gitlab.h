//
// Target: gitlab (exe) -- Gitlab command line interface
// Exceptions: yes
// Header: include/gitlab.h
//

#include "include/gen/gitlab_gen.h"
#include "include/gen/gitlab_gen.inl.h"

namespace gitlab { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/gitlab.cpp
    //

    // Interpret -issue argument as a number, or specific issue id
    // gitlab -issue 33        --> return 33
    // gitlab -issue algouk.33 --> return 33
    // gitlab -issue %         --> return 0
    int IssueArgNumber();

    // same for mr
    int MrNumber(strptr in);
    u32 GetUserId(strptr user);
    u32 GetMilestoneId(strptr milestone);
    void Main();
}
