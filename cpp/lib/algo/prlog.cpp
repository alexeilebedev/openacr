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
            i32 pos(0);
            i32 start(0);
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
// Each component can be prefixed with + or -, e.g. +a,-b etc.
// Finally, each component can be a key-value pair, e.g. +a:<filter>,-b,+c
// <filter> is an optional regex; Regex can be prefixed with ! to indicate a negative match.
// Timestamps can be enabled with 'timestamps', disabled with '-timestamps'
// Verbose can be enabled with 'verbose', disabled with '-verbose'
// Debug can be enabled with 'debug', disabled with '-debug'
int algo_lib::ApplyTrace(algo::strptr what) {
    int nmatch=0;
    for (; what != ""; what=Pathcomp(what, ",LR")) {
        algo::strptr expr=Pathcomp(what,",LL");
        algo::strptr name=Pathcomp(expr,":LL");
        algo::strptr filter=Pathcomp(expr,":LR");
        nmatch += algo_lib::ApplyTrace(name,filter);
    }
    return nmatch;
}

// Enable/disable log category NAME with filter FILTER.
// If NAME is prefixed with +, logging is enabled
// If NAME is prefixed with -, logging is disabled
// If NAME is an empty string. current state is printed
// FILTER is a regex
// If FILTER starts with !, it is a negative filter (any matching lines are omitted)
// Return number of logcats affected.
int algo_lib::ApplyTrace(algo::strptr name, algo::strptr filter) {
    int change=0;
    int nmatch=0;
    if (StartsWithQ(name,"+")) {
        change=1;
        name=ch_RestFrom(name,1);
    } else if (StartsWithQ(name,"-")) {
        change=-1;
        name=ch_RestFrom(name,1);
    } else {
        change = name != "";
    }
    algo::strptr negfilter;
    if (StartsWithQ(filter,"!")) {
        negfilter=RestFrom(filter,1);
        filter="";
    }
    if (change<=0 && name=="") {
        name="%";// show all, or disable all
    }
    algo_lib::Regx name_regx;
    Regx_ReadSql(name_regx,name,true);
    ind_beg(algo_lib::_db_logcat_curs,logcat,algo_lib::_db) {
        if (!logcat.builtin && algo_lib::Regx_Match(name_regx, logcat.logcat)) {
            nmatch++;
            if (change>0) {
                logcat.enabled=true;
                logcat.nmsg=0;// reset message counter
                if (filter!="") {
                    Regx_ReadSql(logcat.filter,filter,false);
                }
                if (negfilter!="") {
                    Regx_ReadSql(logcat.negfilter,negfilter,false);
                }
            } else if (change<0) {
                logcat.enabled=false;
                logcat.nmsg=0;
                algo::Refurbish(logcat.filter);
                algo::Refurbish(logcat.negfilter);
            }
            if (change) {
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
        }
    }ind_end;
    return nmatch;
}

void algo_lib::ShowTrace() {
    ind_beg(algo_lib::_db_logcat_curs,logcat,algo_lib::_db) if (!logcat.builtin) {
        if (logcat.enabled) {
            prlog("trace"
                  << Keyval("target",logcat.logcat)
                  << Keyval("filter",logcat.filter.expr));
        }
    }ind_end;
}

static void LogcatThrottleCheck(algo_lib::FLogcat &logcat) {
    int extra=logcat.nmsg - logcat.maxmsg;
    logcat.enabled=logcat.nmsg>0;// re-enable if the message counter was non-zero
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
bool algo_lib::LogcatFilterQ(algo_lib::FLogcat &logcat, algo::strptr str) {
    bool ret = logcat.enabled
        && (logcat.filter.expr.ch_n==0 || algo_lib::Regx_Match(logcat.filter,str))
        && !algo_lib::Regx_Match(logcat.negfilter,str);
    if (ret && logcat.maxmsg>0) {
        if (logcat.nmsg==0) {
            // schedule throttle reset
            hook_Set1(logcat.th_throttle, logcat, LogcatThrottleCheck);
            algo_lib::ThScheduleIn(logcat.th_throttle, algo::ToSchedTime(double(logcat.window)));
        }
        logcat.nmsg++;// count number of messages printed
        // check throttle
        if (logcat.nmsg >= logcat.maxmsg) {
            logcat.enabled=false;
            ret=false;
        }
    }
    return ret;
}
