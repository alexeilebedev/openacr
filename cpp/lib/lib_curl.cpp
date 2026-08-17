// Copyright (C) 2025-2026 AlgoRND
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
// Target: lib_curl (lib) -- covers curl_easy
// Exceptions: yes
// Source: cpp/lib/lib_curl.cpp
//
// Two ways to make an HTTP request over one description of a request.  Curl()
// returns when the exchange is over; CurlBegin() returns at once and fires the
// call's done hook when the response arrives.  Both build the handle through
// CurlSetopt, so an option added for one serves both.

#include "include/algo.h"
#include "include/algo.h"
#include "include/lib_curl.h"
#include <curl/curl.h>

// -----------------------------------------------------------------------------
#define curl_vrfy_(res)                                                 \
    do {                                                                \
        CURLcode res_copy(res);                                         \
        vrfy(res_copy==CURLE_OK,tempstr()<<(#res)<<": "<<curl_easy_strerror(res_copy)); \
    } while (0)

#define curl_vrfy_rc(res, opt, val, strname)                            \
    do {                                                                \
        CURLcode res_copy(res);                                         \
        vrfy(res_copy==CURLE_OK,tempstr()<<(#res)<<": "<<curl_easy_strerror(res_copy)); \
        strname<<(opt) << " = \"" << (val)<<"\""<<eol;                  \
    } while (0)


// -----------------------------------------------------------------------------
// ---- write callback (accumulate body) ----
static size_t WriteCb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    // auto* out = static_cast<std::string*>(userdata);
    // out->append(ptr, size * nmemb);
    // return size * nmemb;
    vrfy_(userdata);
    lib_curl::FResponse &resp = *(lib_curl::FResponse *)userdata;
    strptr chunk(ptr,size*nmemb);
    resp.body << chunk;
    return size*nmemb;
}
// -----------------------------------------------------------------------------
// Curl header calback function, also called for status line
static size_t HeaderCb(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    lib_curl::FResponse &resp = *(lib_curl::FResponse *)userdata;
    strptr chunk(ptr,size*nmemb);
    tempstr header(chunk);
    header = tempstr()<<Trimmed(header);
    if (header!=""){
        // Parse header for status etc
        // Parse reason phrase (HTTP/<v> <code> <reason>)
        headers_Alloc(resp)=header;

        if (algo::StartsWithQ(header, "HTTP/")){
            resp.status_line=header;
            u16_ReadStrptrMaybe(resp.code,Trimmed(Pathcomp(header," LR LL")));
            resp.reason = algo::Trimmed(Pathcomp(header," LR LR"));
        } else if (algo::StartsWithQ(header, "Content-Type")){ // Parse Content-Type: <type>; <??>
            resp.content_type=Trimmed(Pathcomp(header,":LR;LL"));
        }
    }
    return size*nmemb;
}

// -----------------------------------------------------------------------------
// Apply REQ to the easy handle CURL and hand back the header list the exchange
// owns; the caller frees it when the exchange is over.  OUT_RESP is bound as
// the sink for the reply.
static struct curl_slist *CurlSetopt(CURL *curl, lib_curl::FRequest &req, lib_curl::FResponse &out_resp){
    bool knownmethod = req.method=="GET" || req.method=="POST" || req.method=="PUT" || req.method=="PATCH" || req.method=="DELETE";
    vrfy(knownmethod, tempstr()<<"lib_curl.bad_method"<<Keyval("method",req.method));

    // Set Headers
    struct curl_slist* hdrs=NULL;
    tempstr data_brkt("\"");
    ind_beg(lib_curl::FRequest_headers_curs,header,req){
        if (header=="Content-Type: application/json"){
            data_brkt="";
        }
        hdrs = curl_slist_append(hdrs, Zeroterm(header));
        out_resp.curlrc<<"header = \""<<header<<"\""<<eol;
    }ind_end;

    // Core options
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_URL,Zeroterm(req.url)),"url",req.url,out_resp.curlrc);
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HTTPHEADER,hdrs));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCb));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_resp));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &HeaderCb));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &out_resp));
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_USERAGENT, "lib_curl/1.0"),"user-agent","lib_curl/1.0",out_resp.curlrc);

    // TLS (secure by default)
    i64 host_tls=req.insecure_tls ? 0L : 2L;
    i64 peer_tls=req.insecure_tls ? 0L : 1L;
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, host_tls));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, peer_tls));
    if (req.insecure_tls){
        out_resp.curlrc<<"insecure"<<eol;
    }

    // AWS SigV4: the same mechanism curl's own --aws-sigv4 flag drives --
    // libcurl computes the Authorization header (and the payload hash it
    // covers) from the scope string plus the username/password pair, so
    // the caller never assembles the signature by hand.  A caller with a
    // temporary STS credential adds its session token as an ordinary
    // header (X-Amz-Security-Token) via req.headers; SigV4 itself has no
    // separate slot for one.
    if (req.aws_sigv4 != ""){
        curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_AWS_SIGV4,Zeroterm(req.aws_sigv4)),"aws-sigv4",req.aws_sigv4,out_resp.curlrc);
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_USERNAME,Zeroterm(req.username)));
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_PASSWORD,Zeroterm(req.password)));
    }

    // Timeouts
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, req.connect_timeout_sec),"connect-timeout",req.connect_timeout_sec,out_resp.curlrc);
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_TIMEOUT, req.total_timeout_sec),"max-time",req.total_timeout_sec,out_resp.curlrc);
    curl_vrfy_rc(curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L),"keepalive-time", 1, out_resp.curlrc);

    // Method + body
    out_resp.curlrc<<"request = "<<req.method<<eol;
    if (req.method == "GET") {
        // default
    } else if (req.method == "POST") {
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        if (req.body.ch_n) {
            out_resp.curlrc<<"data = "<<data_brkt<<req.body<<data_brkt<<eol;
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.ch_elems));
        }
        // A POST with no POSTFIELDS makes libcurl pull the body through the
        // read callback, whose default reads stdin and blocks indefinitely on
        // a terminal (CURLOPT_TIMEOUT cannot fire inside the callback).  An
        // explicit size covers the empty body: zero bytes to read, so the
        // callback is never consulted and a Content-Length: 0 POST goes out.
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.body.ch_n));
    } else {
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, Zeroterm(tempstr()<<req.method)));
        if (req.body.ch_n) {
            out_resp.curlrc<<"data = "<<data_brkt<<req.body<<data_brkt<<eol;
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.ch_elems));
            curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.body.ch_n));
        }
    }
    return hdrs;
}

// -----------------------------------------------------------------------------
// Internal: non-throwing attempt that returns CURLcode in out param.
// We keep this internal so HttpJson can still throw on transport errors,
// while retry wrapper can inspect and retry on transport failures.
static CURLcode HttpJsonAttempt(lib_curl::FRequest &req, lib_curl::FResponse &out_resp){
    CURLcode out_cc(CURLE_OK);

    // One handle lives across requests: a fresh handle pays TCP+TLS setup
    // (~70ms against a local gitlab) on every call, which dominates any
    // tool that walks an API list.  Reset clears the options but keeps the
    // handle's connection cache, so consecutive requests to the same host
    // ride one connection.
    CURL *curl = (CURL*)(uintptr_t)lib_curl::_db.curl;
    if (curl) {
        curl_easy_reset(curl);
    } else {
        vrfy(curl = curl_easy_init(),"curl_easy_init failed");
        lib_curl::_db.curl = (u64)(uintptr_t)curl;
    }
    struct curl_slist *hdrs = CurlSetopt(curl, req, out_resp);

    // Perform
    if (req.curlrc_only){
        out_cc = CURLE_OK;
    } else {
        out_cc = curl_easy_perform(curl);
        vrfy(out_cc == CURLE_OK,tempstr()
             <<Keyval("error",out_cc)
             <<Keyval("comment",curl_easy_strerror(out_cc))
             );
        long response_code=0;// getinfo writes a long; a u16* would be overrun
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        out_resp.code = (u16)response_code;
    }
    curl_slist_free_all(hdrs);
    return out_cc;
}
// ----------------- Public API -----------------
bool lib_curl::Curl(lib_curl::FRequest &req, lib_curl::FResponse &out_resp){
    CURLcode out_cc(HttpJsonAttempt(req, out_resp));
    if (out_cc) {
        algo::Throw(curl_easy_strerror(out_cc),(Errcode)out_cc);
    }
    return out_cc;
}

// -----------------------------------------------------------------------------
tempstr lib_curl::PrintCurlResp(lib_curl::FResponse &resp, bool nodate DFLTVAL(false)){
    tempstr out;
    out<<"# "<<Keyval("code",resp.code)<<eol;
    out<<"# "<<Keyval("reason",resp.reason)<<eol;
    out<<"# "<<Keyval("content",resp.content_type)<<eol;
    out<<"# "<<Keyval("body",resp.body)<<eol;
    out<<"# "<<Keyval("status_line",resp.status_line)<<eol;
    ind_beg(lib_curl::FResponse_headers_curs,header,resp) if (!(StartsWithQ(header,"Date:") && nodate)) {
        out<<"# "<<Keyval("header",header)<<eol;
    }ind_end;
    out<<"#---curlrc---"<<eol;
    out<<resp.curlrc<<eol;
    return out;
}

// -----------------------------------------------------------------------------
// Settle every finished exchange, firing each one's done hook as it goes.  A
// hook may delete its own call and start the next exchange.
static void ReapCall() {
    CURLM *multi = (CURLM*)(uintptr_t)lib_curl::_db.curlm;
    bool drained = false;
    while (!drained) {
        int nleft = 0;
        CURLMsg *msg = curl_multi_info_read(multi, &nleft);
        if (!msg) {
            drained = true;
        } else if (msg->msg == CURLMSG_DONE) {
            CURL *easy = msg->easy_handle;
            CURLcode cc = msg->data.result;
            lib_curl::FCall *call = NULL;
            curl_easy_getinfo(easy, CURLINFO_PRIVATE, (char**)&call);
            long response_code = 0;// getinfo writes a long; a u16* would be overrun
            curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response_code);
            call->resp.code = (u16)response_code;
            if (cc != CURLE_OK) {
                call->err = curl_easy_strerror(cc);
            }
            curl_multi_remove_handle(multi, easy);
            curl_easy_cleanup(easy);
            curl_slist_free_all((struct curl_slist*)(uintptr_t)call->hdrs);
            call->hdrs = 0;
            done_Call(*call, *call);
        }
    }
}

// -----------------------------------------------------------------------------
// Advance every transfer waiting on FD, then settle whatever that finished.
// FD is CURL_SOCKET_TIMEOUT when the caller is curl's timer.
static void SockAction(curl_socket_t fd, int ev_bitmask) {
    int nrunning = 0;
    CURLMcode mc = curl_multi_socket_action((CURLM*)(uintptr_t)lib_curl::_db.curlm, fd, ev_bitmask, &nrunning);
    vrfy(mc == CURLM_OK, tempstr()<<"lib_curl.multi_failed"<<Keyval("comment",curl_multi_strerror(mc)));
    ReapCall();
}

// -----------------------------------------------------------------------------
// An epoll event on one of curl's sockets: name the directions it is ready in
// and let curl move its transfer along.
//
// The re-arm is not optional: this epoll is edge-triggered, but curl writes a
// large body one chunk per call and waits to be called again while the socket
// stays ready.  Without a fresh edge the transfer stops for good.
static void OnSockEvt(lib_curl::FSock &sock, algo_lib::FIohook &iohook) {
    int ev_bitmask = 0;
    if (read_Get(iohook.flags)) {
        ev_bitmask |= CURL_CSELECT_IN;
    }
    if (write_Get(iohook.flags)) {
        ev_bitmask |= CURL_CSELECT_OUT;
    }
    if (err_Get(iohook.flags)) {
        ev_bitmask |= CURL_CSELECT_ERR;
    }
    algo::Fildes fildes = sock.iohook.fildes;
    SockAction(fildes.value, ev_bitmask);
    if (sock.iohook.in_epoll && sock.iohook.fildes == fildes) {
        algo_lib::IohookAdd(sock.iohook, sock.iohook.evt_flags);
    }
}

// -----------------------------------------------------------------------------
// curl's request to watch FD, change the directions it is watched in, or stop.
//
// A row is never freed here.  This runs in the middle of the scheduler's walk
// over a batch of epoll events, and a later event in that batch can still name
// the row; released rows go on a free list instead.
static int SockCb(CURL *easy, curl_socket_t fd, int what, void *userp, void *socketp) {
    (void)easy;
    CURLM *multi = (CURLM*)userp;
    lib_curl::FSock *sock = (lib_curl::FSock*)socketp;
    if (what == CURL_POLL_REMOVE) {
        if (sock) {
            algo_lib::IohookRemove(sock->iohook);
            sock->iohook.fildes = algo::Fildes();
            lib_curl::zd_sock_free_Insert(*sock);
            curl_multi_assign(multi, fd, NULL);
        }
    } else {
        if (!sock) {
            sock = lib_curl::zd_sock_free_RemoveFirst();
            if (!sock) {
                sock = &lib_curl::sock_Alloc();
            }
            // curl opened this descriptor and curl closes it; the hook is
            // only watching it.
            sock->iohook.nodelete = true;
            callback_Set2(sock->iohook, *sock, OnSockEvt);
            curl_multi_assign(multi, fd, sock);
        }
        sock->iohook.fildes = algo::Fildes(fd);
        algo::IOEvtFlags flags;
        read_Set(flags, what == CURL_POLL_IN || what == CURL_POLL_INOUT);
        write_Set(flags, what == CURL_POLL_OUT || what == CURL_POLL_INOUT);
        algo_lib::IohookAdd(sock->iohook, flags);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// curl's request to be called back in TIMEOUT_MS, or -1 for nothing timed.
// The timehook is stood down so an idle process is free to sleep.
static int TimerCb(CURLM *multi, long timeout_ms, void *userp) {
    (void)multi;
    (void)userp;
    if (timeout_ms < 0) {
        algo_lib::bh_timehook_Remove(lib_curl::_db.th_curl);
    } else {
        algo_lib::ThScheduleIn(lib_curl::_db.th_curl, algo::ToSchedTime(double(timeout_ms) / 1000.0));
    }
    return 0;
}

// -----------------------------------------------------------------------------
// curl's timer expired: let it time out whatever it was waiting on.
static void CurlTimerStep() {
    SockAction(CURL_SOCKET_TIMEOUT, 0);
}

// -----------------------------------------------------------------------------
// The multi handle every asynchronous exchange runs on, created on first use.
// One handle per process is one connection cache.
static CURLM *Curlm() {
    CURLM *ret = (CURLM*)(uintptr_t)lib_curl::_db.curlm;
    if (!ret) {
        vrfy(ret = curl_multi_init(),"curl_multi_init failed");
        lib_curl::_db.curlm = (u64)(uintptr_t)ret;
        curl_multi_setopt(ret, CURLMOPT_SOCKETFUNCTION, &SockCb);
        curl_multi_setopt(ret, CURLMOPT_SOCKETDATA, ret);
        curl_multi_setopt(ret, CURLMOPT_TIMERFUNCTION, &TimerCb);
        hook_Set0(lib_curl::_db.th_curl, CurlTimerStep);
    }
    return ret;
}

// -----------------------------------------------------------------------------
// Start CALL's exchange and return without waiting.  The done hook fires when
// the response is complete or the transport gave up; that hook reads resp and
// err and disposes of the row.
//
// Until it fires, CALL must stay where it is: curl holds the body by address
// rather than copying it.
void lib_curl::CurlBegin(lib_curl::FCall &call) {
    CURLM *multi = Curlm();
    // A call is reused to retry, so the previous answer is cleared rather
    // than accumulated onto.
    call.resp.code = 0;
    call.resp.body = "";
    call.resp.content_type = "";
    call.resp.status_line = "";
    call.resp.reason = "";
    call.resp.curlrc = "";
    lib_curl::headers_RemoveAll(call.resp);
    call.err = "";
    CURL *easy = NULL;
    vrfy(easy = curl_easy_init(),"curl_easy_init failed");
    call.hdrs = (u64)(uintptr_t)CurlSetopt(easy, call.req, call.resp);
    curl_easy_setopt(easy, CURLOPT_PRIVATE, (void*)&call);
    CURLMcode mc = curl_multi_add_handle(multi, easy);
    vrfy(mc == CURLM_OK, tempstr()<<"lib_curl.multi_add_failed"<<Keyval("comment",curl_multi_strerror(mc)));
}
