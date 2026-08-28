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
// Target: doc (exe) -- Render a markdown document to the terminal
// Exceptions: yes
// Source: cpp/doc/serve.cpp -- Answering one location per request
//
// The socket half of reading the tree in a browser.
// One request is one location, and the answer is that location rendered.  Nothing
// is cached and no page is written to disk: a link is a roundtrip, so the page a
// visitor gets was made from the tree as it stands when they asked for it, which
// is what makes this worth running against a checkout somebody is editing.
// The reader on the terminal keeps its state on FDb -- which flags are in force,
// where it has been -- and a server has no reader to keep it in.  So the flags
// arrive in the query string and are set from it on every request, and the
// browser's own history is the Backspace.
// A table decides what may be served.  The security
// domain is the one a source browser has: the tree's contents are not secret, and
// nothing outside the tree may leave the process.  A path is answered only when it
// is a row of dev.gitfile, so `..`, an absolute path, a symlink pointing out of the
// checkout and a file nobody committed are all refused by construction -- there is
// no sanitizing rule to get subtly wrong.  A path outside the URL prefix is refused
// before anything looks at it at all.

#include "include/algo.h"
#include "include/doc.h"
#include "include/lib_http.h"
#include "include/lib_netio.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Read one request from FD into REQ, and return whether one arrived whole.
//
// The blank line is part of the head the decoder reads: it is what tells the
// decoder the headers have ended, and the decoder refuses a head it has not
// consumed to the last byte.  A body is read and dropped, because every request
// this answers is a GET.
static bool ReadRequest(algo::Fildes fd, http::Request &req) {
    char chunk[4096];
    cstring buf;
    int headend = -1;
    bool eof = false;
    while (headend < 0 && !eof && ch_N(buf) < 64 * 1024) {
        int nread = read(fd.value, chunk, sizeof(chunk));
        if (nread > 0) {
            buf << strptr(chunk, nread);
            headend = algo::FindStr(buf, "\r\n\r\n");
        } else {
            eof = true;
        }
    }
    bool ok = headend >= 0;
    if (ok) {
        strptr head = FirstN(strptr(buf), headend + 4);
        ok = lib_http::DecodeRequest(head, req);
    }
    return ok;
}

// Write RSP to FD.  Every reply says the connection ends with it, so a browser
// opens a new one for its next request and this never has to hold two requests
// apart on one socket.
static void WriteResponse(algo::Fildes fd, http::Response &rsp) {
    rsp.version = "HTTP/1.1";
    rsp.connection = "close";
    rsp.content_length = ch_N(rsp.body);
    cstring out;
    lib_http::EncodeResponse(out, rsp);
    int nleft = ch_N(out);
    while (nleft > 0) {
        int nwrite = write(fd.value, out.ch_elems + ch_N(out) - nleft, nleft);
        if (nwrite > 0) {
            nleft -= nwrite;
        } else {
            nleft = 0;
        }
    }
}

// Return true when QUERY carries the bare parameter NAME.
//
// The display flags are spelled as their own presence -- `?t&l` -- because that is
// what they are: three switches, each either in force or not, and a value would
// only invite the question of what `t=0` means.
static bool ParamQ(algo::strptr query, algo::strptr name) {
    bool ret = false;
    ind_beg(algo::Sep_curs, pair, query, '&') {
        ret = ret || Pathcomp(pair, "=LL") == name;
    }ind_end;
    return ret;
}

// Return the value of NAME among the parameters QUERY carries, decoded, or nothing
// when QUERY does not carry it.
static tempstr ParamVal(algo::strptr query, algo::strptr name) {
    tempstr ret;
    ind_beg(algo::Sep_curs, pair, query, '&') {
        if (Pathcomp(pair, "=LL") == name && ch_N(ret) == 0) {
            ret << algo::UrlDecode(Pathcomp(pair, "=LR"), true);
        }
    }ind_end;
    return ret;
}

// Take the display flags for this request from QUERY.
//
// They are set from the request and from nowhere else, so one visitor's choice cannot
// reach another's page, and a link naming no flag opens the same page however the
// previous request left the process.
//
// Each parameter names a section to open or a tab to land on, and its absence is the
// closed section and the records tab, exactly as it is on the terminal.  How much of a
// record comes back is no parameter at all, since it follows from the selection.
static void ReadFlag(algo::strptr query) {
    doc::_db.meta = ParamQ(query, "meta");
    doc::_db.vis = ParamQ(query, "vis");
    doc::_db.code = ParamQ(query, "code");
    doc::_db.data = ParamQ(query, "data");
    doc::_db.showcmd = ParamQ(query, "cmd");
}

// Return true when the location LOC is one this server may answer.
//
// A fragment names a heading inside a page and comes off before the gate looks at
// anything, so what is tested is the page.
//
// A query, a view of a query and the page of keys name no file at all, and a
// directory names a prefix.  A path names a file, and the
// files this answers with are the ones git tracks: the lookup is the whole of the
// gate, so a path climbing out of the checkout with `..`, an absolute path, and a
// file that exists but nobody committed are each refused because none of them is a
// row of that table.  A rule about which characters a path may carry would have to
// be right about every spelling of the same escape; a table has no spellings.
static bool ServableQ(algo::strptr loc) {
    algo::strptr path = Pathcomp(loc, "#LL");
    bool named = doc::QueryQ(path) || doc::BarepageQ(path) || doc::DirQ(path);
    return named || doc::ind_gitfile_Find(path) != NULL;
}

// Fill RSP with the page for the request whose path is REST and whose query string
// is QUERY, the URL prefix already off the path.
//
// A search is answered with a redirect.  The box
// submits the canonical text of a page, which is the flags in force and then a key,
// so the flags are taken from it first and the key is resolved the way the command
// line resolves one.  The visitor is then sent to the location it named, under those
// flags.  The address bar then holds a location like any other, the back button
// works, and the page can be linked to.
//
// A key that named nothing is answered in place with the candidates, which is the
// distinction the command line makes: a name resolves, a guess offers.  A key that
// named something this server may not serve is refused instead.
//
// Both redirects carry the tab, where a link on a page carries none.  A reader who typed
// a command line into the box asked for exactly that line, and the tab is one of its
// options -- so submitting `-meta <table>` and landing on the records would be the box
// answering a question other than the one it was handed.
//
// A path that named a location other than itself is redirected to that location
// `/func:lib_x2.FStream` and `/acr` are both real ways to ask
// for a page, and answering them where they were asked would leave the page's text
// disagreeing with the page -- so the address bar is corrected first, and every page
// that gets rendered is one whose location is already canonical.
//
// The candidate page takes the key rather than a location, and that is what keeps
// the two apart safely.  `?q=/etc/passwd` is the attack, and it has two doors:
// resolution answers with any path that exists, and the body dispatch file-tests
// whatever location it is handed.  Refusing the resolved case shuts the first door
// only, and the second stays open for as long as a key can be passed off as a
// location -- so it cannot be.
static void AnswerLoc(algo::strptr rest, algo::strptr query, http::Response &rsp) {
    tempstr text(ParamVal(query, "q"));
    bool searching = ch_N(text) > 0;
    tempstr search(searching ? doc::ReadText(text) : tempstr());
    tempstr want(searching ? tempstr() : doc::UrlLoc(rest));
    tempstr loc(searching ? doc::ResolveLoc(search) : doc::ResolveLoc(want));
    bool resolved = ch_N(loc) > 0;
    bool servable = resolved && ServableQ(loc);
    bool moved = !searching && ch_N(rest) > 0 && servable && loc != want;
    rsp.content_type = "text/html; charset=utf-8";
    if (moved) {
        rsp.status = "302 Found";
        rsp.location = doc::Tabhref(loc, doc::Curtab());
        rsp.body << "<!DOCTYPE html><p>";
        doc::HtmlEsc(rsp.body, loc);
        rsp.body << "</p>";
    } else if (searching && servable) {
        rsp.status = "302 Found";
        rsp.location = doc::Tabhref(loc, doc::Curtab());
        rsp.body << "<!DOCTYPE html><p>";
        doc::HtmlEsc(rsp.body, search);
        rsp.body << "</p>";
    } else if (searching && !resolved) {
        rsp.status = "200 OK";
        rsp.body << doc::HtmlFind(text);
    } else if (!searching && servable) {
        rsp.status = "200 OK";
        rsp.body << doc::HtmlPage(loc);
    } else {
        rsp.status = "404 Not Found";
        rsp.body << doc::HtmlMiss(searching ? algo::strptr(search) : algo::strptr(want));
    }
}

// Fill RSP with the answer to REQ.
//
// The URL prefix is the outer gate and it runs first: a path that does not begin
// with it is refused without anything else in this program seeing it.  That is what
// "nothing outside the prefix is served" means -- one server may hold several
// things, and this one answers for its own subtree and declines to have an opinion
// about the rest.
//
// The root of the host is the one exception, and it redirects.  Typing a host name
// is how a person arrives at a server they were told about, and a bare refusal there
// is a wall: they have the address they were given and no way to learn the one path
// it answers for.  So `/` is sent to the prefix,
// which discloses only what the operator chose when they set it.  Nothing else
// outside the prefix moves, and a refusal carries a link to the prefix for the same
// reason -- a reader who guessed a path wrong should be one click from the index.
static void Answer(http::Request &req, http::Response &rsp) {
    tempstr base(doc::UrlPath());
    tempstr path(algo::UrlDecode(Pathcomp(req.uri, "?LL"), true));
    algo::strptr query(Pathcomp(req.uri, "?LR"));
    bool inside = StartsWithQ(path, base);
    bool root = !inside && path == "/";
    algo::strptr rest = inside ? doc::RestFrom(path, ch_N(base)) : algo::strptr();
    bool slash = ch_N(rest) > 0 && rest.elems[0] == '/';
    prlog("doc.request" << Keyval("path", path) << Keyval("param", query) << Keyval("inside", inside));
    ReadFlag(query);
    if (inside) {
        AnswerLoc(slash ? doc::RestFrom(rest, 1) : rest, query, rsp);
    } else {
        rsp.content_type = "text/html; charset=utf-8";
        rsp.status = root ? "302 Found" : "404 Not Found";
        rsp.location << (root ? doc::UrlRoot() : tempstr());
        rsp.body << "<!DOCTYPE html><title>doc</title><p>nothing is served outside <a href=\"";
        doc::HtmlEsc(rsp.body, doc::UrlRoot());
        rsp.body << "\">";
        doc::HtmlEsc(rsp.body, doc::UrlRoot());
        rsp.body << "</a></p>";
    }
}

// Give FD a deadline for the bytes of a request, so that a client which connects
// and then says nothing cannot hold the server.
//
// One connection is served at a time, and a blocking read on a silent socket is
// therefore the whole process stopped: a browser tab that opened a connection and
// went away, or one client deliberately holding it, and every other visitor waits
// forever with nothing to say why.  Five seconds is far longer than a request on a
// loopback takes and far shorter than a person will wait.
static void SetDeadline(algo::Fildes fd) {
    struct timeval tv;
    algo::ZeroBytes(tv);
    tv.tv_sec = 5;
    (void)setsockopt(fd.value, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

// Answer requests arriving on LISTEN_FD until the process is killed.
//
// One connection is served at a time and each is closed after its reply.  A page
// costs one acr run and sometimes one src_func run, which is tens of milliseconds,
// and a documentation browser has one visitor at a time -- so nothing is bought by
// overlapping them, and the request log stays in the order the browser made them.
static void ServeLoop(algo::Fildes listen_fd) {
    while (true) {
        ietf::Ipport peer;
        algo::Fildes fd = lib_netio::Accept(listen_fd, peer);
        if (ValidQ(fd)) {
            http::Request req;
            SetDeadline(fd);
            if (ReadRequest(fd, req)) {
                http::Response rsp;
                Answer(req, rsp);
                WriteResponse(fd, rsp);
            }
            close(fd.value);
        }
    }
}

// Bind the address -bind names and answer requests there until the process is
// killed.
//
// The address it is listening on is printed before the first request can arrive,
// because that is the only way a caller learns the server is up -- a test that
// starts one and connects to it has nothing else to wait for.
void doc::Serve() {
    vrfy(ch_N(doc::_db.cmdline.server) > 0,
         "doc.noserver  comment:'-bind needs -server, which is the URL prefix every generated"
         " link carries and the only path this answers for'");
    algo::Fildes listen_fd = lib_netio::CreateTcpSocket();
    vrfy(ValidQ(listen_fd), "doc.nosocket");
    lib_netio::SetReuseAddress(listen_fd);
    vrfy(lib_netio::Bind(listen_fd, doc::_db.cmdline.bind),
         tempstr() << "doc.nobind" << Keyval("bind", doc::_db.cmdline.bind));
    vrfy(lib_netio::Listen(listen_fd, 16), "doc.nolisten");
    ietf::Ipport ipport;
    lib_netio::GetIpportLocal(listen_fd, ipport);
    prlog("doc.listen" << Keyval("bind", ipport) << Keyval("url", doc::_db.cmdline.server));
    ServeLoop(listen_fd);
}
