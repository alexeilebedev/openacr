// Copyright (C) 2026 AlgoRND
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
// Target: risk_engine (exe)
// Exceptions: yes
// Source: cpp/risk_engine/risk_engine.cpp
//

#include "include/algo.h"
#include "include/risk_engine.h"

static tempstr FmtDollar(double val) {
    tempstr out;
    if (val < 0) {
        out << "-$";
        algo::double_PrintWithCommas(-val, out, 2);
    } else {
        out << "$";
        algo::double_PrintWithCommas(val, out, 2);
    }
    return out;
}

static tempstr FmtPct(double val) {
    tempstr out;
    algo::double_PrintPrec(val * 100, out, 1, false, false);
    out << "%";
    return out;
}

static void PrintHeader(algo::strptr title) {
    prlog("");
    prlog("========================================================================");
    prlog("  " << title);
    prlog("========================================================================");
}

static void PrintSubheader(algo::strptr title) {
    prlog("");
    prlog("  --- " << title << " ---");
}

static void RunRiskReport() {
    PrintHeader("RISK ENGINE REPORT");
    prlog("  Generated: " << algo::CurrUnTime());

    PrintHeader("PORTFOLIO SUMMARY BY COUNTERPARTY");

    ind_beg(risk_engine::_db_zd_cpty_curs, cpty, risk_engine::_db) {
        double gross_exposure = 0;
        double net_exposure = 0;
        double total_daily_pnl = 0;
        int n_positions = 0;

        ind_beg(risk_engine::cpty_zd_position_curs, pos, cpty) {
            risk_engine::FInstrument &instr = *pos.p_instrument;
            double market_value = pos.qty * instr.price;
            double daily_pnl = pos.qty * (instr.price - instr.prev_close);

            gross_exposure += algo::Abs(market_value);
            net_exposure += market_value;
            total_daily_pnl += daily_pnl;
            n_positions++;
        }ind_end;

        PrintSubheader(cpty.cpty);
        prlog("    Positions:      " << n_positions);
        prlog("    Gross Exposure: " << FmtDollar(gross_exposure));
        prlog("    Net Exposure:   " << FmtDollar(net_exposure));
        prlog("    Daily P&L:      " << FmtDollar(total_daily_pnl));

        ind_beg(risk_engine::cpty_zd_position_curs, pos, cpty) {
            risk_engine::FInstrument &instr = *pos.p_instrument;
            double market_value = pos.qty * instr.price;
            double daily_pnl = pos.qty * (instr.price - instr.prev_close);
            prlog("      " << instr.instrument
                  << "  qty:" << pos.qty
                  << "  mkt_val:" << FmtDollar(market_value)
                  << "  daily_pnl:" << FmtDollar(daily_pnl)
                  << "  (" << instr.name << ")");
        }ind_end;
    }ind_end;

    PrintHeader("RISK LIMIT CHECK");

    int n_breaches = 0;
    int n_warnings = 0;

    ind_beg(risk_engine::_db_zd_cpty_curs, cpty, risk_engine::_db) {
        double gross_exposure = 0;
        double net_exposure = 0;
        double daily_pnl_loss = 0;

        ind_beg(risk_engine::cpty_zd_position_curs, pos, cpty) {
            risk_engine::FInstrument &instr = *pos.p_instrument;
            double market_value = pos.qty * instr.price;
            double daily_pnl = pos.qty * (instr.price - instr.prev_close);

            gross_exposure += algo::Abs(market_value);
            net_exposure += market_value;
            if (daily_pnl < 0) {
                daily_pnl_loss += algo::Abs(daily_pnl);
            }
        }ind_end;

        ind_beg(risk_engine::cpty_zd_risklimit_curs, lim, cpty) {
            algo::Smallstr50 ltype = limittype_Get(lim);
            double actual = 0;

            if (ltype == "gross_exposure") {
                actual = gross_exposure;
            } else if (ltype == "net_exposure") {
                actual = algo::Abs(net_exposure);
            } else if (ltype == "daily_pnl_loss") {
                actual = daily_pnl_loss;
            }

            double utilization = lim.limit_value > 0 ? actual / lim.limit_value : 0;

            if (utilization > 1.0) {
                prlog("  ** BREACH **  " << cpty.cpty
                      << "  limit:" << ltype
                      << "  actual:" << FmtDollar(actual)
                      << "  limit:" << FmtDollar(lim.limit_value)
                      << "  util:" << FmtPct(utilization));
                n_breaches++;
            } else if (utilization > lim.warn_pct) {
                prlog("  ! WARNING !   " << cpty.cpty
                      << "  limit:" << ltype
                      << "  actual:" << FmtDollar(actual)
                      << "  limit:" << FmtDollar(lim.limit_value)
                      << "  util:" << FmtPct(utilization));
                n_warnings++;
            } else {
                prlog("    OK          " << cpty.cpty
                      << "  limit:" << ltype
                      << "  actual:" << FmtDollar(actual)
                      << "  limit:" << FmtDollar(lim.limit_value)
                      << "  util:" << FmtPct(utilization));
            }
        }ind_end;
    }ind_end;

    PrintHeader("EXPOSURE BY INSTRUMENT TYPE");

    ind_beg(risk_engine::_db_zd_instrtype_curs, itype, risk_engine::_db) {
        double type_exposure = 0;
        double type_pnl = 0;
        int n_instruments = 0;

        ind_beg(risk_engine::instrtype_zd_instrument_curs, instr, itype) {
            ind_beg(risk_engine::_db_zd_position_curs, pos, risk_engine::_db) {
                if (pos.p_instrument == &instr) {
                    double mkt = pos.qty * instr.price;
                    double dpnl = pos.qty * (instr.price - instr.prev_close);
                    type_exposure += algo::Abs(mkt);
                    type_pnl += dpnl;
                }
            }ind_end;
            n_instruments++;
        }ind_end;

        if (n_instruments > 0) {
            prlog("  " << itype.instrtype
                  << "  instruments:" << n_instruments
                  << "  exposure:" << FmtDollar(type_exposure)
                  << "  daily_pnl:" << FmtDollar(type_pnl));
        }
    }ind_end;

    PrintHeader("SUMMARY");
    prlog("  Counterparties: " << risk_engine::zd_cpty_N());
    prlog("  Instruments:    " << risk_engine::zd_instrument_N());
    prlog("  Positions:      " << risk_engine::zd_position_N());
    prlog("  Risk Limits:    " << risk_engine::zd_risklimit_N());
    prlog("  Breaches:       " << n_breaches);
    prlog("  Warnings:       " << n_warnings);
    prlog("");
}

void risk_engine::Main() {
    RunRiskReport();
}
