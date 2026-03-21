#!/usr/bin/env bash
# build_risk_system.sh
#
# Builds the OpenACR risk engine from scratch.
# Run inside an OpenACR container with all tools built.
#
# Usage:
#   docker exec -it -w /root/openacr openacr-risk bash
#   export PATH=$PATH:$PWD/bin
#   bash build_risk_system.sh
#
# Or from the host:
#   docker exec -w /root/openacr openacr-risk bash -c \
#     "export PATH=\$PATH:\$PWD/bin && bash build_risk_system.sh"

set -euo pipefail

echo "============================================"
echo "  STEP 1: Create ssimfile tables"
echo "============================================"

acr_ed -create -ssimfile dev.instrtype  -write -amc:N
acr_ed -create -ssimfile dev.cpty       -write -amc:N
acr_ed -create -ssimfile dev.limittype  -write -amc:N

acr_ed -create -ssimfile dev.instrument -write -amc:N
acr_ed -create -field dev.Instrument.instrtype   -arg dev.Instrtype    -reftype Pkey -write -amc:N
acr_ed -create -field dev.Instrument.name        -arg algo.Smallstr50  -write -comment "Display name" -amc:N
acr_ed -create -field dev.Instrument.price       -arg double -dflt 0.0 -write -comment "Current market price" -amc:N
acr_ed -create -field dev.Instrument.prev_close  -arg double -dflt 0.0 -write -comment "Previous close price" -amc:N

acr_ed -create -ssimfile dev.position -subset dev.Cpty -subset2 dev.Instrument -separator . -write -amc:N
acr_ed -create -field dev.Position.qty      -arg double -dflt 0.0 -write -comment "Number of units held" -amc:N
acr_ed -create -field dev.Position.avg_cost -arg double -dflt 0.0 -write -comment "Average cost per unit" -amc:N

acr_ed -create -ssimfile dev.risklimit -subset dev.Cpty -subset2 dev.Limittype -separator . -write -amc:N
acr_ed -create -field dev.Risklimit.limit_value -arg double -dflt 0.0 -write -comment "Maximum allowed value" -amc:N
acr_ed -create -field dev.Risklimit.warn_pct    -arg double -dflt 0.8 -write -comment "Warning threshold as fraction of limit" -amc:N

echo ""
echo "============================================"
echo "  STEP 2: Populate reference data"
echo "============================================"

cat <<EOF | acr -insert -write
dev.instrtype instrtype:equity comment:"Common stock and ETFs"
dev.instrtype instrtype:bond comment:"Fixed income securities"
dev.instrtype instrtype:fx comment:"Foreign exchange"
dev.instrtype instrtype:commodity comment:"Physical commodities and futures"
EOF

cat <<EOF | acr -insert -write
dev.cpty cpty:desk_a comment:"Equities Trading Desk A"
dev.cpty cpty:desk_b comment:"Fixed Income Desk B"
dev.cpty cpty:desk_c comment:"Macro Trading Desk C"
dev.cpty cpty:client_x comment:"Hedge Fund Client X"
dev.cpty cpty:client_y comment:"Pension Fund Client Y"
EOF

cat <<EOF | acr -insert -write
dev.limittype limittype:gross_exposure comment:"Maximum gross market value of positions"
dev.limittype limittype:net_exposure comment:"Maximum net market value of positions"
dev.limittype limittype:daily_pnl_loss comment:"Maximum daily PnL loss (absolute value)"
dev.limittype limittype:concentration comment:"Maximum percentage in single instrument"
EOF

cat <<EOF | acr -insert -write
dev.instrument instrument:AAPL instrtype:equity name:"Apple Inc" price:178.50 prev_close:175.20
dev.instrument instrument:TSLA instrtype:equity name:"Tesla Inc" price:245.30 prev_close:252.10
dev.instrument instrument:MSFT instrtype:equity name:"Microsoft Corp" price:415.80 prev_close:412.00
dev.instrument instrument:AMZN instrtype:equity name:"Amazon.com" price:185.60 prev_close:183.90
dev.instrument instrument:UST10Y instrtype:bond name:"US Treasury 10Y" price:96.25 prev_close:96.50
dev.instrument instrument:CORP_AA instrtype:bond name:"AA Corporate Bond" price:98.10 prev_close:98.30
dev.instrument instrument:EURUSD instrtype:fx name:"Euro/US Dollar" price:1.0845 prev_close:1.0820
dev.instrument instrument:GBPUSD instrtype:fx name:"British Pound/USD" price:1.2630 prev_close:1.2685
dev.instrument instrument:GOLD instrtype:commodity name:"Gold Spot" price:2185.40 prev_close:2172.00
dev.instrument instrument:OIL_WTI instrtype:commodity name:"WTI Crude Oil" price:78.25 prev_close:79.10
EOF

cat <<EOF | acr -insert -write
dev.position position:desk_a.AAPL qty:5000 avg_cost:170.00
dev.position position:desk_a.TSLA qty:-2000 avg_cost:248.00
dev.position position:desk_a.MSFT qty:3000 avg_cost:400.00
dev.position position:desk_b.UST10Y qty:10000 avg_cost:97.00
dev.position position:desk_b.CORP_AA qty:5000 avg_cost:99.00
dev.position position:desk_b.MSFT qty:1000 avg_cost:410.00
dev.position position:desk_b.AAPL qty:4000 avg_cost:170.00
dev.position position:desk_b.TSLA qty:2000 avg_cost:240.00
dev.position position:desk_c.EURUSD qty:1000000 avg_cost:1.0800
dev.position position:desk_c.GBPUSD qty:-500000 avg_cost:1.2700
dev.position position:desk_c.GOLD qty:100 avg_cost:2150.00
dev.position position:desk_c.OIL_WTI qty:5000 avg_cost:76.00
dev.position position:client_x.AAPL qty:10000 avg_cost:165.00
dev.position position:client_x.AMZN qty:8000 avg_cost:180.00
dev.position position:client_x.TSLA qty:3000 avg_cost:260.00
dev.position position:client_y.UST10Y qty:50000 avg_cost:95.50
dev.position position:client_y.CORP_AA qty:20000 avg_cost:97.50
dev.position position:client_y.GOLD qty:200 avg_cost:2100.00
EOF

cat <<EOF | acr -insert -write
dev.risklimit risklimit:desk_a.gross_exposure limit_value:5000000 warn_pct:0.8
dev.risklimit risklimit:desk_a.daily_pnl_loss limit_value:200000 warn_pct:0.75
dev.risklimit risklimit:desk_b.gross_exposure limit_value:3000000 warn_pct:0.8
dev.risklimit risklimit:desk_b.daily_pnl_loss limit_value:100000 warn_pct:0.75
dev.risklimit risklimit:desk_c.gross_exposure limit_value:10000000 warn_pct:0.8
dev.risklimit risklimit:desk_c.daily_pnl_loss limit_value:500000 warn_pct:0.75
dev.risklimit risklimit:desk_c.net_exposure limit_value:5000000 warn_pct:0.8
dev.risklimit risklimit:client_x.gross_exposure limit_value:8000000 warn_pct:0.85
dev.risklimit risklimit:client_x.daily_pnl_loss limit_value:300000 warn_pct:0.8
dev.risklimit risklimit:client_y.gross_exposure limit_value:15000000 warn_pct:0.85
dev.risklimit risklimit:client_y.daily_pnl_loss limit_value:400000 warn_pct:0.8
EOF

echo ""
echo "Checking referential integrity..."
acr -check

echo ""
echo "============================================"
echo "  STEP 3: Create executable target"
echo "============================================"

acr_ed -create -target risk_engine -write -amc:N

echo ""
echo "============================================"
echo "  STEP 4: Create in-memory pools + indexes"
echo "============================================"

acr_ed -create -ctype risk_engine.FInstrtype  -pooltype Tpool -indexed -write -amc:N
acr_ed -create -ctype risk_engine.FCpty       -pooltype Tpool -indexed -write -amc:N
acr_ed -create -ctype risk_engine.FLimittype  -pooltype Tpool -indexed -write -amc:N
acr_ed -create -ctype risk_engine.FInstrument -pooltype Tpool -indexed -write -amc:N
acr_ed -create -ctype risk_engine.FPosition   -pooltype Tpool -indexed -write -amc:N
acr_ed -create -ctype risk_engine.FRisklimit  -pooltype Tpool -indexed -write -amc:N

echo ""
echo "============================================"
echo "  STEP 5: Add Base fields (inherit ssimfile fields)"
echo "============================================"

# Delete auto-created primary key fields, add Base (must be first field)
for pair in \
  "risk_engine.FInstrtype.instrtype:dev.Instrtype" \
  "risk_engine.FCpty.cpty:dev.Cpty" \
  "risk_engine.FLimittype.limittype:dev.Limittype" \
  "risk_engine.FInstrument.instrument:dev.Instrument" \
  "risk_engine.FPosition.position:dev.Position" \
  "risk_engine.FRisklimit.risklimit:dev.Risklimit"; do
  field="${pair%%:*}"
  base_arg="${pair##*:}"
  ctype="${field%.*}"
  acr "field:$field" -del -write
  echo "dmmeta.field field:${ctype}.base arg:${base_arg} reftype:Base dflt:\"\" comment:\"\"" | acr -insert -write
done

# Re-add thash records (cascade-deleted when primary key fields were removed)
cat <<EOF | acr -insert -write
dmmeta.thash field:risk_engine.FDb.ind_instrtype hashfld:dev.Instrtype.instrtype unique:Y comment:""
dmmeta.thash field:risk_engine.FDb.ind_cpty hashfld:dev.Cpty.cpty unique:Y comment:""
dmmeta.thash field:risk_engine.FDb.ind_limittype hashfld:dev.Limittype.limittype unique:Y comment:""
dmmeta.thash field:risk_engine.FDb.ind_instrument hashfld:dev.Instrument.instrument unique:Y comment:""
dmmeta.thash field:risk_engine.FDb.ind_position hashfld:dev.Position.position unique:Y comment:""
dmmeta.thash field:risk_engine.FDb.ind_risklimit hashfld:dev.Risklimit.risklimit unique:Y comment:""
EOF

echo ""
echo "============================================"
echo "  STEP 6: Add Upptr fields + xrefs"
echo "============================================"

cat <<EOF | acr -insert -write
dmmeta.field field:risk_engine.FInstrument.p_instrtype arg:risk_engine.FInstrtype reftype:Upptr dflt:"" comment:""
dmmeta.xref field:risk_engine.FInstrument.p_instrtype inscond:true via:risk_engine.FDb.ind_instrtype/dev.Instrument.instrtype
dmmeta.field field:risk_engine.FPosition.p_cpty arg:risk_engine.FCpty reftype:Upptr dflt:"" comment:""
dmmeta.xref field:risk_engine.FPosition.p_cpty inscond:true via:risk_engine.FDb.ind_cpty/dev.Position.cpty
dmmeta.field field:risk_engine.FPosition.p_instrument arg:risk_engine.FInstrument reftype:Upptr dflt:"" comment:""
dmmeta.xref field:risk_engine.FPosition.p_instrument inscond:true via:risk_engine.FDb.ind_instrument/dev.Position.instrument
dmmeta.field field:risk_engine.FRisklimit.p_cpty arg:risk_engine.FCpty reftype:Upptr dflt:"" comment:""
dmmeta.xref field:risk_engine.FRisklimit.p_cpty inscond:true via:risk_engine.FDb.ind_cpty/dev.Risklimit.cpty
EOF

echo ""
echo "============================================"
echo "  STEP 7: Add group-by Llist fields"
echo "============================================"

cat <<EOF | acr -insert -write
dmmeta.field field:risk_engine.FCpty.zd_position arg:risk_engine.FPosition reftype:Llist dflt:"" comment:"Positions held by this counterparty"
dmmeta.llist field:risk_engine.FCpty.zd_position havetail:Y havecount:Y comment:""
dmmeta.xref field:risk_engine.FCpty.zd_position inscond:true via:risk_engine.FPosition.p_cpty
dmmeta.cascdel field:risk_engine.FCpty.zd_position comment:""
dmmeta.field field:risk_engine.FCpty.zd_risklimit arg:risk_engine.FRisklimit reftype:Llist dflt:"" comment:"Risk limits for this counterparty"
dmmeta.llist field:risk_engine.FCpty.zd_risklimit havetail:Y havecount:Y comment:""
dmmeta.xref field:risk_engine.FCpty.zd_risklimit inscond:true via:risk_engine.FRisklimit.p_cpty
dmmeta.cascdel field:risk_engine.FCpty.zd_risklimit comment:""
dmmeta.field field:risk_engine.FInstrtype.zd_instrument arg:risk_engine.FInstrument reftype:Llist dflt:"" comment:"Instruments of this type"
dmmeta.llist field:risk_engine.FInstrtype.zd_instrument havetail:Y havecount:Y comment:""
dmmeta.xref field:risk_engine.FInstrtype.zd_instrument inscond:true via:risk_engine.FInstrument.p_instrtype
dmmeta.cascdel field:risk_engine.FInstrtype.zd_instrument comment:""
EOF

echo ""
echo "============================================"
echo "  STEP 8: Add global iteration lists"
echo "============================================"

acr_ed -create -field risk_engine.FDb.zd_cpty       -write -comment "All counterparties"    -amc:N
acr_ed -create -field risk_engine.FDb.zd_instrument -write -comment "All instruments"       -amc:N
acr_ed -create -field risk_engine.FDb.zd_position   -write -comment "All positions"         -amc:N
acr_ed -create -field risk_engine.FDb.zd_risklimit  -write -comment "All risk limits"       -amc:N
acr_ed -create -field risk_engine.FDb.zd_instrtype  -write -comment "All instrument types"  -amc:N

echo ""
echo "============================================"
echo "  STEP 9: Configure automatic data loading"
echo "============================================"

cat <<EOF | acr -insert -write
dmmeta.finput field:risk_engine.FDb.instrtype extrn:N update:N strict:Y comment:""
dmmeta.finput field:risk_engine.FDb.cpty extrn:N update:N strict:Y comment:""
dmmeta.finput field:risk_engine.FDb.limittype extrn:N update:N strict:Y comment:""
dmmeta.finput field:risk_engine.FDb.instrument extrn:N update:N strict:Y comment:""
dmmeta.finput field:risk_engine.FDb.position extrn:N update:N strict:Y comment:""
dmmeta.finput field:risk_engine.FDb.risklimit extrn:N update:N strict:Y comment:""
EOF

echo ""
echo "============================================"
echo "  STEP 10: Write C++ source"
echo "============================================"

cat > cpp/risk_engine/risk_engine.cpp << 'CPPEOF'
// Copyright (C) 2026 AlgoRND
// License: GPL
// Target: risk_engine (exe)
// Source: cpp/risk_engine/risk_engine.cpp

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
CPPEOF

echo ""
echo "============================================"
echo "  STEP 11: Generate code + build"
echo "============================================"

amc
ai risk_engine

echo ""
echo "============================================"
echo "  STEP 12: Run the risk engine"
echo "============================================"

risk_engine

echo ""
echo "============================================"
echo "  BUILD COMPLETE"
echo "============================================"
echo "Run 'risk_engine' to see the report."
echo "Edit data with 'acr -merge -write' and re-run."
