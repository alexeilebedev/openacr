// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/prlog.cpp -- prlog macro
//
// -----------------------------------------------------------------------------

#include "include/algo.h"

// Default implementation of prlog handler
// This function is called via algo_lib::_db.Prlog pointer.
// The pointer may be set by the application to replace the function with something else.
//
// Notes on WriteFile use:
// we must use WriteFile (which contains a loop) to write all the bytes out,
// otherwise some terminals push back and refuse the data.
void algo::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) {
    try {
        algo::Fildes fildes(logcat->stdout ? 1:2);
        if (!LogcatFilterQ(*logcat,str)) {
            // filtered out
        }  else if (algo_lib::_db.show_tstamp || (logcat && !logcat->builtin)) {
            // rich and slow case
            tempstr out;
            algo::UnTime time = algo::CurrUnTime() - algo::UnDiffSecs(double(get_cycles() - tstamp) / get_cpu_hz());
            i64 pos(0);
            i64 start(0);
            while(pos<ch_N(str)) {
                if (str[pos++] == '\n') {
                    // get whole line including eol
                    strptr line = ch_GetRegion(str,start,pos-start);
                    start = pos; // advance to next line
                    if (algo_lib::_db.pending_eol) {
                        out << line;
                        algo_lib::_db.pending_eol = false;
                    } else {
                        // format log line
                        // time logcat: text
                        if (algo_lib::_db.show_tstamp) {
                            UnTime_PrintSpec(time,out,algo_lib::_db.tstamp_fmt);
                        }
                        if (logcat && !logcat->builtin) {
                            out << logcat->logcat <<": ";
                        }
                        out << line;
                    }
                }
            }
            algo_lib::_db.pending_eol = start != pos;
            // write out the temp string with additional annotations
            WriteFile(fildes,(u8*)out.ch_elems,out.ch_n);
        } else {
            // simple and fast case
            WriteFile(fildes, (u8*)str.elems, i32(str.n_elems));
        }
    } catch (...) {
        // coverity UNCAUGHT_EXCEPT --
        // do not allow this function to throw exception.
    }
}

// -----------------------------------------------------------------------------

// Enable or disable logcat tracing based on traace expression WHAT
// WHAT is a comma-separated list of logcat regexes, e.g. a,b,c
// Each component can be a key-value pair, e.g. a:<filter>,b,c
// <filter> is an optional regex; Regex can be prefixed with ! to indicate a negative match.
// Timestamps can be controlled with 'timestamps'
// Verbose can be controlled with 'verbose'
// Debug can be controlled with 'debug'
int algo_lib::ApplyTrace(algo::strptr what, bool enable DFLTVAL(true)) {
    int nmatch=0;
    for (; what != ""; what=Pathcomp(what, ",LR")) {
        algo::strptr expr=Pathcomp(what,",LL");
        algo::strptr name=Pathcomp(expr,":LL");
        algo::strptr filter=Pathcomp(expr,":LR");
        nmatch += algo_lib::ApplyTrace(name,filter,enable);
    }
    return nmatch;
}

// Enable/disable log category NAME with filter FILTER.
// FILTER is a regex
// If FILTER starts with !, it is a negative filter (any matching lines are omitted)
// Return number of logcats affected.
// Changed logcats are marked, attribute changed = algo_lib::_db.clock
int algo_lib::ApplyTrace(algo::strptr name, algo::strptr filter, bool enable DFLTVAL(true)) {
    int nmatch=0;
    bool negfilter=false;
    if (StartsWithQ(filter,"!")) {
        filter=RestFrom(filter,1);
        negfilter=true;
    }
    algo_lib::Regx name_regx;
    Regx_ReadSql(name_regx,name,true);
    ind_beg(algo_lib::_db_logcat_curs,logcat,algo_lib::_db) {
        if (!logcat.builtin && algo_lib::Regx_Match(name_regx, logcat.logcat)) {
            nmatch++;
            bool changed = false;
            if (enable) {
                logcat.enabled=true;
                changed=true;
                if (filter!="") {
                    Regx_ReadSql((negfilter ? logcat.negfilter : logcat.filter),filter,false);
                }
            } else {
                changed = logcat.enabled;
                logcat.enabled=false;
                algo::Refurbish(logcat.filter);
                algo::Refurbish(logcat.negfilter);
            }
            if (changed) {
                logcat.changed = algo_lib::_db.clock;// mark as changed
                logcat.nmsg=0;// reset message counter
                logcat.suppress=false;// an operator's decision outranks the window in progress
            }
            // update various internal flags based on logcats being enabled/disabled
            // this allows selecting debug / verbose tracing at runtime,
            // with regex filters
            if (&logcat == &algo_lib_logcat_debug) {
                algo_lib::_db.cmdline.debug = logcat.enabled;
            } else if (&logcat == &algo_lib_logcat_verbose2) {
                u8_UpdateMax(algo_lib::_db.cmdline.verbose, logcat.enabled*2);
            } else if (&logcat == &algo_lib_logcat_verbose) {
                u8_UpdateMax(algo_lib::_db.cmdline.verbose, logcat.enabled);
            } else if (&logcat == &algo_lib_logcat_timestamps) {
                algo_lib::_db.show_tstamp = logcat.enabled;
            }
        }
    }ind_end;
    return nmatch;
}

// End LOGCAT's throttle window: lift the suppression and start a fresh count,
// reporting how many messages the window turned away.
//
// The suppression is its own flag and never the enabled bit, because the two
// facts have different owners: enabled is what the operator asked for, and
// suppress is what the throttle is doing about the current window.  Written to
// one field, an operator's `trace -disable` on a busy category survived only
// until this hook ran, which restored the bit from whether traffic had arrived
// -- so tracing resumed a window later, on a category the operator had turned
// off and a report had confirmed as off.
static void LogcatThrottleCheck(algo_lib::FLogcat &logcat) {
    int extra=logcat.nmsg - logcat.maxmsg;
    logcat.suppress=false;
    logcat.nmsg=0;
    if (extra > 0) {
        prlog_cat(logcat,extra<<" more messages suppressed");
    }
}

// Filter string STR for output on LOGCAT.
// The string must match FILTER and not match NEGFILTER.
// The initial state is that filter is empty (uninitialized), so empty filter
// is interpreted as "match all"
// Filter and negfilter are configured with ApplyTrace which parses a trace expression
// In addition, if throttling is enabled on LOGCAT, block message
// if more than MAXMSG are being printed within WINDOW secs. The counter
// is reset every WINDOW secs.
// The window's count is of messages this category *matched*, not of messages it
// printed, which is what makes the suppressed tally at window end a real number.
// Counting only what printed freezes the count at the cap -- nothing increments
// it once suppression is on -- so the overflow reads as zero and an operator is
// never told the trace they are reading is lossy.  TOTMSG stays the count of
// messages that went out.
bool algo_lib::LogcatFilterQ(algo_lib::FLogcat &logcat, algo::strptr str) {
    bool ret = logcat.enabled
        && (logcat.filter.expr.ch_n==0 || algo_lib::Regx_Match(logcat.filter,str))
        && !algo_lib::Regx_Match(logcat.negfilter,str);
    if (ret && logcat.maxmsg>0) {// check throttle setting
        if (logcat.nmsg==0) {
            // schedule throttle reset
            hook_Set1(logcat.th_throttle, logcat, LogcatThrottleCheck);
            algo_lib::ThScheduleIn(logcat.th_throttle, algo::ToSchedTime(double(logcat.window)));
        }
        logcat.nmsg++;// count number of messages within this window
        // check throttle
        if (logcat.nmsg > logcat.maxmsg) {
            logcat.suppress=true;
        }
    }
    ret = ret && !logcat.suppress;
    if (ret) {
        logcat.totmsg++;// count total messages printed
    }
    return ret;
}
