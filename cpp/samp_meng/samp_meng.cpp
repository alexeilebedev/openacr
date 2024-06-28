// Copyright (C) 2024 AlgoRND
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
// Target: samp_meng (exe)
// Exceptions: yes
// Source: cpp/samp_meng/samp_meng.cpp
//

#include "include/algo.h"
#include "include/algo.h"
#include "include/samp_meng.h"

void samp_meng::In_TextMsg(samp_meng::TextMsg &) {
}

// Create new symbol
void samp_meng::In_NewSymbolReqMsg(samp_meng::NewSymbolReqMsg &msg) {
    samp_meng::FSymbol &symbol = ind_symbol_GetOrCreate(msg.symbol);
    if (symbol.id == 0) {
        // key is already populated
        symbol.id = symbol_N(); // first symbol gets 1
        for (int i=0; i<2; i++) {
            samp_meng::FOrdq &ordq=ordq_Alloc();
            ordq.p_symbol=&symbol;
            ordq_XrefMaybe(ordq); // will succeed;
        }
        samp_meng::NewSymbolMsg out(msg.symbol);
        prlog(out);
    }
}

// Create new user.
void samp_meng::In_NewUserReqMsg(samp_meng::NewUserReqMsg &msg) {
    int n_user=ind_user_N();
    samp_meng::FUser &user = ind_user_GetOrCreate(msg.user);
    if (ind_user_N()>n_user) {
        samp_meng::NewUserMsg out(user.user);
        prlog(out);
    }
}

// Add an order to the orderbook.
// Publish any resulting trades; If order is IOC, cancel the remainder back,
// otherwise post it to the book
// USER_ID: owning user
// SYMBOL: symbol associated with order
// PRICE: order price
// QTY: signed quantity; positive=buy, negative=sell
// IOC: if TRUE, order is cancelled immediately after matching
void samp_meng::In_NewOrderReqMsg(samp_meng::NewOrderReqMsg &msg) {
    samp_meng::FUser *user = ind_user_Find(msg.user);
    samp_meng::FSymbol *symbol = ind_symbol_Find(msg.symbol);
    samp_meng::FOrder *order=&order_Alloc();
    if (user && symbol) {
        order->order = _db.next_order_id++;
        order->price = msg.price;
        order->p_ordq = &c_ordq_qFind(*symbol,msg.qty<0);
        order->time = algo::CurrUnTime();
        order->qty = labs(msg.qty);
        order->p_user = user;
        order->ordkey.price = algo::i64_NegateIf(order->price.value, msg.qty>0);
        order->ordkey.time = -order->time.value;// oldest orders first
        order_XrefMaybe(*order);// will succeed;
    }
    samp_meng::FOrdq *xside=&c_ordq_qFind(*symbol,msg.qty>=0);
    samp_meng::FOrder *xorder = 0;
    while (order->qty !=0 && (xorder = bh_order_First(*xside))!=0) {
        if (msg.qty>0 ? (order->price >= xorder->price) : (order->price <= xorder->price)){
            int matchqty = i32_Min(order->qty,xorder->qty);
            samp_meng::OrderTradeMsg out(xorder->order,matchqty,xorder->price);
            prlog(out);
            order->qty -= matchqty;
            xorder->qty -= matchqty;
            if (xorder->qty==0) {
                CancelOrder(xorder,true);
            }
        }
    }
    if (order->qty !=0 && !msg.ioc) {
        samp_meng::NewOrderMsg out(order->time,order->price,order->order,msg.symbol,order->qty);
        prlog(out);
    } else {
        CancelOrder(order,false);
        order=NULL;
    }
}

// Cancel a single order
void samp_meng::CancelOrder(samp_meng::FOrder *order, bool notify) {
    if (notify) {
        samp_meng::CancelOrderMsg out(order->order);
        prlog(out);
    }
    order_Delete(*order);
}

// Cancel a single order
void samp_meng::In_CancelReqMsg(samp_meng::CancelReqMsg &msg) {
    if (samp_meng::FOrder *order = ind_order_Find(msg.order)) {
        CancelOrder(order,true);
    }
}

// Cancel all orders for user
void samp_meng::In_MassCancelReqMsg(samp_meng::MassCancelReqMsg &msg) {
    if (samp_meng::FUser *user = ind_user_Find(msg.user)) {
        while (!zd_order_EmptyQ(*user)) {
            CancelOrder(zd_order_First(*user),true);
        }
    }
}

void samp_meng::cd_fdin_eof_Step() {
    fdin_RemoveAll();
}

// Read next input line from stdin
void samp_meng::cd_fdin_read_Step() {
    samp_meng::FFdin &fdin = *cd_fdin_read_RotateFirst();
    algo::strptr msgstr = in_GetMsg(fdin);
    if (msgstr.elems) {
        algo::ByteAry buf;
        if (In_ReadStrptrMaybe(msgstr,buf)) {
            samp_meng::MsgHeader *msg=(samp_meng::MsgHeader*)buf.ary_elems;
            tempstr out;
            In_Print(out,*msg,msg->length);
            prlog("input: "<<out);
            samp_meng::InDispatch(*msg);
            prlog("");
        } else if (Trimmed(msgstr)!="") {
            prlog("bad input: "<<msgstr);
        }
        in_SkipMsg(fdin);
    }
}

void samp_meng::Main() {
    samp_meng::FFdin &fdin = fdin_Alloc();
    fdin_XrefMaybe(fdin);
    algo::Fildes fd(0);
    algo::SetBlockingMode(fd,false);
    in_BeginRead(fdin,fd);
    samp_meng::MainLoop();
}
