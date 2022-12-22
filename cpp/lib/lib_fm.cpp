//
// Target: lib_fm (lib) -- Library for dynamic alarm management
// Exceptions: yes
// Source: cpp/lib/lib_fm.cpp
//

#include "include/algo.h"
#include "include/lib_fm.h"

// Initialize FM library
// in - directory to load data
// prefix - automatically add prefix to object instance ID, current service name
void lib_fm::FmInit(algo::strptr in, algo::strptr object_prefix) {
    _db.object_prefix = object_prefix;
    lib_fm::LoadTuplesMaybe(in);
}

// Compose log file name from file_prefix and date:
// file_prefix%Y-%b-%d.ssim
tempstr lib_fm::LogFile(algo::strptr file_prefix, algo::UnTime date) {
    tempstr out;
    out << file_prefix;
    UnTime_PrintSpec(date,out,"%Y-%b-%d");
    out << ".ssim";
    return out;
}

// Find most recent log file, load alarms from it, clears all raised alarms,
// and if today's file does not exist, create it and dump snapshot.
// This function calls h_alarm user hook for all alarms for resutling snapshot,
// could be useful for other subsystems.
void lib_fm::GatherAlarmHistory(algo::strptr file_prefix, algo::UnTime time, int days_old) {
    // assumed that after loading resulting snapshot
    // will be dumped at the beginning of new today's file,
    // so that no need to scan all files
    cstring log_file;
    for (int i = 0; i <= days_old; ++i) {
        log_file = lib_fm::LogFile(file_prefix,time - algo::UnDiffHMS(24*i,0,0));
        if (FileQ(log_file)) {
            break;
        }
    }
    // load file data
    if (FileQ(log_file)) {
        algo::UnTime purge = time - algo::UnDiffHMS(24*days_old,0,0);
        ind_beg(algo::FileLine_curs,line,log_file) {
            fmdb::Alarm alarm;
            if (fmdb::Alarm_ReadStrptrMaybe(alarm,line) && !UnTime_Lt(alarm.update_time,purge)) {
                lib_fm::FAlarm &falarm = lib_fm::ind_alarm_GetOrCreate(alarm.alarm);
                falarm.flag            = alarm.flag;
                falarm.severity        = alarm.severity;
                falarm.n_occurred      = alarm.n_occurred;
                falarm.first_time      = alarm.first_time;
                falarm.last_time       = alarm.last_time;
                falarm.clear_time      = alarm.clear_time;
                falarm.update_time     = alarm.update_time;
                falarm.objtype_summary = alarm.objtype_summary;
                falarm.summary         = alarm.summary;
                falarm.description     = alarm.description;
                falarm.source          = alarm.source;
            }
        }ind_end;
    }
    // clear all raised alarms (or call hook, just in case)
    ind_beg(lib_fm::_db_alarm_curs,alarm,lib_fm::_db) {
        if (!ClearAlarm(alarm,time)) {
            h_alarm_Call(alarm);
        }
    }ind_end;

    // create today's log file, if not already existing
    log_file = lib_fm::LogFile(file_prefix,time);
    if (!FileQ(log_file)) {
        alarm_SaveSsimfile(log_file);
    }
}

// clear existing alarm, return true if cleared, false if not
bool lib_fm::ClearAlarm(lib_fm::FAlarm &alarm, algo::UnTime time) {
    bool ret(false);
    if (value_GetEnum(alarm.flag) != fm_Flag_value_cleared) {
        value_SetEnum(alarm.flag,fm_Flag_value_cleared);
        alarm.update_time = time;
        alarm.clear_time = time;
        ret = true;
        h_alarm_Call(alarm);
    }
    return ret;
}

// Set or clear alarm, return alarm record created or updated, NULL if none
// TIME: current time, e.g. algo::CurrUnTime()
// CODE: alarm code; one of the constants from fmdb.alm_code table, e.g. lib_fm::fmdb_alm_code_MEM_ULK
// OBJTYPE: object type; one of the constants from fmdb.alm_objtype table, e.g. lib_fm::fmdb_alm_objtype_PROC
// OBJINST: object instance; key identifying the object.
// MESSAGE: user-readable message.
lib_fm::FAlarm *lib_fm::SetAlarm(bool raise, algo::UnTime time, algo::strptr code, algo::strptr objtype, algo::strptr objinst, algo::strptr message) {
    lib_fm::FAlarm *ret(NULL);
    cstring objinst_prefixed;
    objinst_prefixed << _db.object_prefix << (ch_N(_db.object_prefix) && ch_N(objinst) ? "." : "" ) << objinst;
    cstring object_id = fmdb::Alarm_Concat_objtype_objinst(objtype,objinst_prefixed);
    cstring alarm_id = fmdb::Alarm_Concat_code_object(code,object_id);
    if (raise) {
        lib_fm::FAlarm &alarm = lib_fm::ind_alarm_GetOrCreate(alarm_id);
        if (value_GetEnum(alarm.flag) == fm_Flag_value_raised) {
            // alarm already raised, just compare and update if needed
            // severity and summary are static at this stage
            if (message != alarm.description) {
                alarm.description = message;
                alarm.update_time = time;
                ret = &alarm;
                h_alarm_Call(alarm);
            }
        } else {
            // new or cleared alarm: assign severity and summary from profile
            value_SetEnum(alarm.flag,fm_Flag_value_raised);
            if (!alarm.n_occurred++) {
                lib_fm::FAlmCode *alm_code = lib_fm::ind_alm_code_Find(code);
                if (alm_code) {
                    alarm.severity = alm_code->severity;
                    alarm.summary = alm_code->summary;
                    alarm.source = alm_code->source;
                }
                lib_fm::FAlmObjtype *alm_objtype = lib_fm::ind_alm_objtype_Find(objtype);
                if (alm_objtype) {
                    alarm.objtype_summary = alm_objtype->summary;
                }
                alarm.first_time = time;
            }
            alarm.last_time = time;
            alarm.clear_time = algo::UnTime();
            alarm.update_time = time;
            alarm.description = message;
            ret = &alarm;
            h_alarm_Call(alarm);
        }
    } else { // clear
        lib_fm::FAlarm *alarm = lib_fm::ind_alarm_Find(alarm_id);
        ret = alarm && ClearAlarm(*alarm,time) ? alarm : NULL;
    }
    return ret;
}

// copy alarm to message
void lib_fm::CopyToMsg(lib_fm::FAlarm &from, fm::AlarmMsg &to) {
    to.code            = code_Get(from);
    to.objtype         = objtype_Get(from);
    to.objinst         = objinst_Get(from);
    to.flag            = from.flag;
    to.severity        = from.severity;
    to.n_occurred      = from.n_occurred;
    to.first_time      = from.first_time;
    to.last_time       = from.last_time;
    to.clear_time      = from.clear_time;
    to.update_time     = from.update_time;
    to.objtype_summary = from.objtype_summary;
    to.summary         = from.summary;
    to.description     = from.description;
    to.source          = from.source;
}

// update from message
void lib_fm::UpdateFromMsg(fm::AlarmMsg &msg) {
    cstring object_id = fmdb::Alarm_Concat_objtype_objinst(msg.objtype,msg.objinst);
    cstring alarm_id = fmdb::Alarm_Concat_code_object(msg.code,object_id);
    lib_fm::FAlarm &alarm = lib_fm::ind_alarm_GetOrCreate(alarm_id);
    alarm.flag            = msg.flag;
    alarm.severity        = msg.severity;
    alarm.n_occurred      = msg.n_occurred;
    alarm.first_time      = msg.first_time;
    alarm.last_time       = msg.last_time;
    alarm.clear_time      = msg.clear_time;
    alarm.update_time     = msg.update_time;
    alarm.objtype_summary = msg.objtype_summary;
    alarm.summary         = msg.summary;
    alarm.description     = msg.description;
    alarm.source          = msg.source;
    h_alarm_Call(alarm);
}

// dump all alarms (for debugging purpose)
void lib_fm::DumpAlarms() {
    ind_beg(lib_fm::_db_alarm_curs,alarm,lib_fm::_db) {
        fmdb::Alarm out;
        alarm_CopyOut(alarm,out);
        prlog((tempstr()<<out));
    }ind_end;
}
