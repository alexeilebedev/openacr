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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/fm.cpp
//

#include "include/atf_unit.h"
#include "include/lib_fm.h"

static int n_raise;
static int n_clear;

static void ResetStat() {
    n_raise = 0;
    n_clear = 0;
}

static void AlarmHook(void *, lib_fm::FAlarm &alarm) {
    n_raise += value_GetEnum(alarm.flag) == fm_Flag_value_raised  ? 1 : 0;
    n_clear += value_GetEnum(alarm.flag) == fm_Flag_value_cleared ? 1 : 0;
}

void atf_unit::unittest_fm() {
    lib_fm::FmInit("data","prefix");
    lib_fm::_db.h_alarm = AlarmHook;
    lib_fm::alarm_RemoveAll();

    algo::UnTime t1,t2,t3,t4,t5;
    algo::UnTime_ReadStrptrMaybe(t1,"2022-02-03T21:04:42.191623");
    algo::UnTime_ReadStrptrMaybe(t2,"2022-02-03T21:04:42.191722");
    algo::UnTime_ReadStrptrMaybe(t3,"2022-02-03T21:04:42.191734");
    algo::UnTime_ReadStrptrMaybe(t4,"2022-02-03T21:04:42.191871");
    algo::UnTime_ReadStrptrMaybe(t5,"2022-02-03T21:04:42.191989");

    vrfy_(t1!=t2);
    vrfy_(t1!=t3);
    vrfy_(t1!=t4);
    vrfy_(t1!=t5);
    vrfy_(t2!=t3);
    vrfy_(t2!=t4);
    vrfy_(t2!=t5);
    vrfy_(t3!=t4);
    vrfy_(t3!=t5);
    vrfy_(t4!=t5);

    cstring code(lib_fm::fmdb_alm_code_TEST_EXER);
    cstring objt(lib_fm::fmdb_alm_objtype_SYS);
    cstring obj("instance");
    cstring alm_id("TEST-EXER@SYS.prefix.instance");
    cstring summary("Test exercise");
    cstring osummary("System");
    cstring desc1("Some condition");
    cstring desc2("Another condition");
    cstring src1(lib_fm::fmdb_alm_source_UnitTest);

    // clear alarm (non-existing)
    prlog("FM T1");
    ResetStat();
    lib_fm::FAlarm *alarm = lib_fm::SetAlarm(false,t1,code,objt,obj,"");
    vrfyeq_(lib_fm::alarm_N(),0);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,0);

    // raise alarm
    prlog("FM T2");
    ResetStat();
    alarm = lib_fm::SetAlarm(true,t1,code,objt,obj,desc1);
    lib_fm::DumpAlarms();
    vrfy_(alarm);
    vrfyeq_(lib_fm::alarm_N(),1);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_raised);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,1);
    vrfyeq_(alarm->first_time,t1);
    vrfyeq_(alarm->last_time,t1);
    vrfyeq_(alarm->clear_time,algo::UnTime());
    vrfyeq_(alarm->update_time,t1);
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,1);
    vrfyeq_(n_clear,0);

    // clear alarm
    prlog("FM T3");
    ResetStat();
    alarm = lib_fm::SetAlarm(false,t2,code,objt,obj,"");
    lib_fm::DumpAlarms();
    vrfy_(alarm);
    vrfyeq_(lib_fm::alarm_N(),1);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_cleared);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,1);
    vrfyeq_(alarm->first_time,t1);
    vrfyeq_(alarm->last_time,t1);
    vrfyeq_(alarm->clear_time,t2);
    vrfyeq_(alarm->update_time,t2);
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,1);

    // re-raise alarm
    prlog("FM T4");
    ResetStat();
    alarm = lib_fm::SetAlarm(true,t3,code,objt,obj,desc1);
    lib_fm::DumpAlarms();
    vrfy_(alarm);
    vrfyeq_(lib_fm::alarm_N(),1);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_raised);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,2);
    vrfyeq_(alarm->first_time,t1);
    vrfyeq_(alarm->last_time,t3);
    vrfyeq_(alarm->clear_time,algo::UnTime());
    vrfyeq_(alarm->update_time,t3);
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,1);
    vrfyeq_(n_clear,0);

    // re-raise alarm - no effect
    prlog("FM T5");
    ResetStat();
    alarm = lib_fm::SetAlarm(true,t4,code,objt,obj,desc1);
    lib_fm::DumpAlarms();
    vrfy_(!alarm);
    vrfyeq_(lib_fm::alarm_N(),1);
    alarm = &lib_fm::alarm_qFind(0);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_raised);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,2);
    vrfyeq_(alarm->first_time,t1);
    vrfyeq_(alarm->last_time,t3);
    vrfyeq_(alarm->clear_time,algo::UnTime());
    vrfyeq_(alarm->update_time,t3);
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,0);

    // change message
    prlog("FM T6");
    ResetStat();
    alarm = lib_fm::SetAlarm(true,t4,code,objt,obj,desc2);
    lib_fm::DumpAlarms();
    vrfy_(alarm);
    vrfyeq_(lib_fm::alarm_N(),1);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_raised);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,2);
    vrfyeq_(alarm->first_time,t1);
    vrfyeq_(alarm->last_time,t3);
    vrfyeq_(alarm->clear_time,algo::UnTime());
    vrfyeq_(alarm->update_time,t4);
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc2);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,1);
    vrfyeq_(n_clear,0);

    // ClearAlarm
    prlog("FM T7");
    ResetStat();
    vrfy_(lib_fm::ClearAlarm(*alarm,t5));
    lib_fm::DumpAlarms();
    vrfyeq_(lib_fm::alarm_N(),1);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_cleared);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,2);
    vrfyeq_(alarm->first_time,t1);
    vrfyeq_(alarm->last_time,t3);
    vrfyeq_(alarm->clear_time,t5);
    vrfyeq_(alarm->update_time,t5);
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc2);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,1);

    // history
    cstring file_prefix("temp/atf_unit.fm.alarm-history-");

    // create history file
    lib_fm::alarm_RemoveAll();
    cstring hist;
    algo::UnTime t;
    algo::UnTime_ReadStrptrMaybe(t,"2022-02-03T07:30:13.0");
    for (int i=0; i<101; ++i) {
        lib_fm::FAlarm *historic_alarm = lib_fm::SetAlarm(!(i%2),t+algo::UnDiffSecs(i*100),code,objt,obj,desc1);
        fmdb::Alarm out;
        alarm_CopyOut(*historic_alarm,out);
        hist << out << '\n';
    }
    algo::StringToFile(hist,lib_fm::LogFile(file_prefix,t));

    // load today's file
    prlog("FM T8");
    lib_fm::alarm_RemoveAll();
    ResetStat();
    lib_fm::GatherAlarmHistory(file_prefix,t+algo::UnDiffSecs(20000),1);
    lib_fm::DumpAlarms();
    vrfyeq_(lib_fm::alarm_N(),1);
    alarm = &lib_fm::alarm_qFind(0);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_cleared);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,51);
    vrfyeq_(alarm->first_time,t);
    vrfyeq_(alarm->last_time,t+algo::UnDiffSecs(100*100));
    vrfyeq_(alarm->clear_time,t+algo::UnDiffSecs(20000));
    vrfyeq_(alarm->update_time,t+algo::UnDiffSecs(20000));
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,1);

    // load yesterday
    prlog("FM T8");
    lib_fm::alarm_RemoveAll();
    ResetStat();
    algo::DeleteFile(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24,0,0)));
    lib_fm::GatherAlarmHistory(file_prefix,t+algo::UnDiffHMS(24,0,0),2);
    lib_fm::DumpAlarms();
    vrfyeq_(lib_fm::alarm_N(),1);
    alarm = &lib_fm::alarm_qFind(0);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_cleared);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,51);
    vrfyeq_(alarm->first_time,t);
    vrfyeq_(alarm->last_time,t+algo::UnDiffSecs(100*100));
    vrfyeq_(alarm->clear_time,t+algo::UnDiffHMS(24,0,0));
    vrfyeq_(alarm->update_time,t+algo::UnDiffHMS(24,0,0));
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,1);
    vrfy_(FileQ(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24,0,0))));

    // load day before yesterday
    prlog("FM T9");
    lib_fm::alarm_RemoveAll();
    ResetStat();
    algo::DeleteFile(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24+24,0,0))); //just in case
    algo::DeleteFile(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24+24+24,0,0)));
    lib_fm::GatherAlarmHistory(file_prefix,t+algo::UnDiffHMS(24+24+24,0,0),3);
    lib_fm::DumpAlarms();
    vrfyeq_(lib_fm::alarm_N(),1);
    alarm = &lib_fm::alarm_qFind(0);
    vrfyeq_(alarm->alarm,alm_id);
    vrfyeq_(value_GetEnum(alarm->flag),fm_Flag_value_cleared);
    vrfyeq_(value_GetEnum(alarm->severity),fm_Severity_value_major);
    vrfyeq_(alarm->n_occurred,51);
    vrfyeq_(alarm->first_time,t);
    vrfyeq_(alarm->last_time,t+algo::UnDiffSecs(100*100));
    vrfyeq_(alarm->clear_time,t+algo::UnDiffHMS(24,0,0));
    vrfyeq_(alarm->update_time,t+algo::UnDiffHMS(24,0,0));
    vrfyeq_(alarm->objtype_summary,osummary);
    vrfyeq_(alarm->summary,summary);
    vrfyeq_(alarm->description,desc1);
    vrfyeq_(alarm->source,src1);
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,1);
    vrfy_(FileQ(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24+24+24,0,0))));

    // purge outdated
    prlog("FM T10");
    lib_fm::alarm_RemoveAll();
    ResetStat();
    algo::DeleteFile(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24,0,0)));
    algo::DeleteFile(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24+24,0,0))); //just in case
    algo::DeleteFile(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24+24+24,0,0)));
    lib_fm::GatherAlarmHistory(file_prefix,t+algo::UnDiffHMS(24+24+24,0,0),2);
    lib_fm::DumpAlarms();
    vrfyeq_(lib_fm::alarm_N(),0);
    alarm = NULL;
    vrfyeq_(n_raise,0);
    vrfyeq_(n_clear,0);
    vrfy_(FileQ(lib_fm::LogFile(file_prefix,t+algo::UnDiffHMS(24+24+24,0,0))));

    // cleanup
    lib_fm::alarm_RemoveAll();
    lib_fm::_db.h_alarm = NULL;
}
