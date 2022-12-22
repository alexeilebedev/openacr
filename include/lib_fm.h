//
// Target: lib_fm (lib) -- Library for dynamic alarm management
// Exceptions: yes
// Header: include/lib_fm.h
//

#include "include/gen/lib_fm_gen.h"
#include "include/gen/lib_fm_gen.inl.h"

namespace lib_fm { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_fm.cpp
    //

    // Initialize FM library
    // in - directory to load data
    // prefix - automatically add prefix to object instance ID, current service name
    void FmInit(algo::strptr in, algo::strptr object_prefix);

    // Compose log file name from file_prefix and date:
    // file_prefix%Y-%b-%d.ssim
    tempstr LogFile(algo::strptr file_prefix, algo::UnTime date);

    // Find most recent log file, load alarms from it, clears all raised alarms,
    // and if today's file does not exist, create it and dump snapshot.
    // This function calls h_alarm user hook for all alarms for resutling snapshot,
    // could be useful for other subsystems.
    void GatherAlarmHistory(algo::strptr file_prefix, algo::UnTime time, int days_old);

    // clear existing alarm, return true if cleared, false if not
    bool ClearAlarm(lib_fm::FAlarm &alarm, algo::UnTime time);

    // Set or clear alarm, return alarm record created or updated, NULL if none
    // TIME: current time, e.g. algo::CurrUnTime()
    // CODE: alarm code; one of the constants from fmdb.alm_code table, e.g. lib_fm::fmdb_alm_code_MEM_ULK
    // OBJTYPE: object type; one of the constants from fmdb.alm_objtype table, e.g. lib_fm::fmdb_alm_objtype_PROC
    // OBJINST: object instance; key identifying the object.
    // MESSAGE: user-readable message.
    lib_fm::FAlarm *SetAlarm(bool raise, algo::UnTime time, algo::strptr code, algo::strptr objtype, algo::strptr objinst, algo::strptr message);

    // copy alarm to message
    void CopyToMsg(lib_fm::FAlarm &from, fm::AlarmMsg &to);

    // update from message
    void UpdateFromMsg(fm::AlarmMsg &msg);

    // dump all alarms (for debugging purpose)
    void DumpAlarms();
}
