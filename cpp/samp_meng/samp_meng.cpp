// Copyright (C) 2024,2026 AlgoRND
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
// Target: samp_meng (exe) -- Sample matching engine
// Exceptions: yes
// Source: cpp/samp_meng/samp_meng.cpp -- Main source
//
// samp_meng matches orders: it folds an order log into a book and emits the
// trades, new orders and cancels that follow.  It is a filter -- orders arrive
// through lib_ams, as binary messages on a shm ring or as ssim text on stdin,
// and its own output leaves the same way.
// A pipe of orders carries no sequencing time, so an order is stamped with the
// wall clock as it is matched.  That makes the matcher a function of when it
// ran as well as of what it read, so two copies fed the same orders agree on
// the trades and disagree on their timestamps.

#include "include/algo.h"
#include "include/samp_meng.h"
#include "include/lib_ams.h"

// Emit a message through lib_ams: binary to the output shm, or ssim text on
// stdout when the matcher is running at the end of a pipe.
template<class T> static void Emit(T &msg) {
    lib_ams::EmitMsg(Castbase(msg));
}

void samp_meng::In_SampMengTextMsg(ams::SampMengTextMsg &) {
}

// Bring SYMBOL into the book with its two order queues, and report whether it
// was not there already.  A symbol's id is its position among the symbols, so
// the order symbols are created in is part of the book's state.
static bool CreateSymbol(ams::SampMengSymbol name) {
    samp_meng::FSymbol &symbol = samp_meng::ind_symbol_GetOrCreate(name);
    bool fresh = symbol.id == 0;
    if (fresh) {
        symbol.id = samp_meng::symbol_N();
        for (int i=0; i<2; i++) {
            samp_meng::FOrdq &ordq=samp_meng::ordq_Alloc();
            ordq.p_symbol=&symbol;
            samp_meng::ordq_XrefMaybe(ordq);
        }
    }
    return fresh;
}

// Bring USER into the book, and report whether it was not there already.
static bool CreateUser(i32 user) {
    int n_user = samp_meng::ind_user_N();
    samp_meng::ind_user_GetOrCreate(user);
    return samp_meng::ind_user_N() > n_user;
}

// Create new symbol
void samp_meng::In_SampMengNewSymbolReqMsg(ams::SampMengNewSymbolReqMsg &msg) {
    if (CreateSymbol(msg.symbol)) {
        ams::SampMengNewSymbolMsg out;
        out.symbol = msg.symbol;
        Emit(out);
    }
}

// Create new user
void samp_meng::In_SampMengNewUserReqMsg(ams::SampMengNewUserReqMsg &msg) {
    if (CreateUser(msg.user)) {
        ams::SampMengNewUserMsg out;
        out.user = msg.user;
        Emit(out);
    }
}

// Add an order to the orderbook.
// Publish any resulting trades; If order is IOC, cancel the remainder back,
// otherwise post it to the book
void samp_meng::In_SampMengNewOrderReqMsg(ams::SampMengNewOrderReqMsg &msg) {
    samp_meng::FUser *user = ind_user_Find(msg.user);
    samp_meng::FSymbol *symbol = ind_symbol_Find(msg.symbol);
    samp_meng::FOrder *order=&order_Alloc();
    if (user && symbol) {
        order->order = _db.next_order_id++;
        order->price = msg.price;
        order->p_ordq = &c_ordq_qFind(*symbol,msg.qty<0);
        order->time = _db.now;
        order->qty = labs(msg.qty);
        order->p_user = user;
        order->ordkey.price = algo::i64_NegateIf(order->price.value, msg.qty>0);
        order->ordkey.time = -order->time.value;
        order_XrefMaybe(*order);
    }
    samp_meng::FOrdq *xside=&c_ordq_qFind(*symbol,msg.qty>=0);
    samp_meng::FOrder *xorder = 0;
    while (order->qty !=0 && (xorder = bh_order_First(*xside))!=0) {
        if (msg.qty>0 ? (order->price.value >= xorder->price.value) : (order->price.value <= xorder->price.value)){
            int matchqty = i32_Min(order->qty,xorder->qty);
            ams::SampMengOrderTradeMsg out;
            out.order = xorder->order;
            out.qty = matchqty;
            out.price = xorder->price;
            Emit(out);
            order->qty -= matchqty;
            xorder->qty -= matchqty;
            if (xorder->qty==0) {
                CancelOrder(xorder,true);
            }
        }
    }
    if (order->qty !=0 && !msg.ioc) {
        ams::SampMengNewOrderMsg out;
        out.time = order->time;
        out.price = order->price;
        out.order = order->order;
        out.symbol = msg.symbol;
        out.qty = order->qty;
        Emit(out);
    } else {
        CancelOrder(order,false);
        order=NULL;
    }
}

// Cancel a single order
void samp_meng::CancelOrder(samp_meng::FOrder *order, bool notify) {
    if (notify) {
        ams::SampMengCancelOrderMsg out;
        out.order = order->order;
        Emit(out);
    }
    order_Delete(*order);
}

void samp_meng::In_SampMengCancelReqMsg(ams::SampMengCancelReqMsg &msg) {
    if (samp_meng::FOrder *order = ind_order_Find(msg.order)) {
        CancelOrder(order,true);
    }
}

void samp_meng::In_SampMengMassCancelReqMsg(ams::SampMengMassCancelReqMsg &msg) {
    if (samp_meng::FUser *user = ind_user_Find(msg.user)) {
        while (!zd_order_EmptyQ(*user)) {
            CancelOrder(zd_order_First(*user),true);
        }
    }
}

// Route an inbound lib_ams message into InDispatch.
// Messages are ams.SampMeng* types (ams.MsgHeader-based).
// In stdio mode, lib_ams parses text into InputLineMsg; re-parse as ams message.
static void OnRecv(lib_ams::FShm &shm, ams::MsgHeader &msg) {
    (void)shm;
    samp_meng::_db.now = algo::CurrUnTime();
    samp_meng::_db.n_in++;
    if (ams::InputLineMsg *ilm = ams::InputLineMsg_Castdown(msg)) {
        algo::strptr line = Trimmed(ams::payload_Getary(*ilm));
        algo::ByteAry buf;
        if (samp_meng::In_ReadStrptrMaybe(line, buf)) {
            samp_meng::InDispatch(*(ams::MsgHeader*)buf.ary_elems);
        }
    } else {
        samp_meng::InDispatch(msg);
    }
}

void samp_meng::Main() {
    lib_ams::Init(_db.cmdline.proc, OnRecv);
    samp_meng::MainLoop();
}
