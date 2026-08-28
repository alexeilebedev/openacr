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
// Source: cpp/doc/msg.cpp
//
// A protocol is a header and the messages that begin with it.  The header carries a
// field saying which message follows -- dmmeta.typefld names that field -- and each
// message embeds the header as its first member, which is reftype Base.  So the two
// tables that describe every protocol in this tree are already here, and the pages
// below are walks over them: from the header to the messages that share it, and from a
// message to the dispatches that receive it.

#include "include/algo.h"
#include "include/doc.h"

// Return the ctype the message header field FIELD belongs to.
static algo::strptr Hdrctype(algo::strptr field) {
    return Pathcomp(field, ".RL");
}

// Return the comment on the ctype NAME, and nothing when the tree holds no such ctype.
static algo::strptr Ctypecomment(algo::strptr name) {
    doc::FCtype *ctype = doc::ind_ctype_Find(name);
    return ctype ? algo::strptr(ctype->comment) : algo::strptr();
}

// Return the comment on the dispatch NAME, which says what that branch reads.
static algo::strptr Dispatchcomment(algo::strptr name) {
    doc::FDispatch *dispatch = doc::ind_dispatch_Find(name);
    return dispatch ? algo::strptr(dispatch->comment) : algo::strptr();
}

// Return how many bytes the message NAME takes on the wire, with a trailing plus when it
// ends in a varlen tail.
//
// A message with no tail is exactly its length, and one with a tail is at least that: the
// fixed part amc measured, and then as many bytes as the framing says.  So the plus is the
// whole of the difference between a size and a floor, and it is one character.
static tempstr Msgsize(algo::strptr name) {
    doc::FMsg *msg = doc::ind_msg_Find(name);
    bool varlen = false;
    tempstr ret;
    if (msg) {
        ind_beg(doc::msg_c_msgfield_curs, msgfield, *msg) {
            varlen = varlen || msgfield.varlen;
        }ind_end;
        ret << msg->len << (varlen ? "+" : "");
    }
    return ret;
}

// Append the wire layout of the message NAME to OUT: every leaf field, where it begins
// and how wide it is.
//
// This is the table somebody reading a packet capture needs, and nothing else on the page
// answers it: the declaration says what the fields are and in what order, and the offsets
// come from how amc laid them out -- alignment, packing, the header cloned in at the
// front.  A varlen tail has no width, since what bounds it is the length the header
// carries rather than anything about the field.
//
// The rows come out in the order the bytes arrive, which the walk gets by taking the
// offsets in turn rather than by sorting: ten messages in the tree put two fields at one
// offset, and stepping the byte counter emits both without needing a rule for which of
// them comes first.
static void PutMsgwire(algo::cstring &out, algo::strptr name) {
    doc::FMsg *msg = doc::ind_msg_Find(name);
    tempstr body;
    int last = 0;
    int n = 0;
    if (msg) {
        ind_beg(doc::msg_c_msgfield_curs, msgfield, *msg) {
            last = i32_Max(last, msgfield.offset);
        }ind_end;
        frep_(at, last + 1) {
            ind_beg(doc::msg_c_msgfield_curs, msgfield, *msg) {
                if (msgfield.offset == int(at)) {
                    n++;
                    body << "|";
                    doc::PutTablecell(body, tempstr() << "`" << doc::name_Get(msgfield) << "`");
                    doc::PutTablecell(body, tempstr() << msgfield.offset);
                    doc::PutTablecell(body, msgfield.varlen ? tempstr() << "to the end" : tempstr() << msgfield.width);
                    doc::PutTablecell(body, tempstr() << "[" << msgfield.arg << "](ctype:" << msgfield.arg << ")"
                                      << (msgfield.bigend ? ", big-endian" : ""));
                    body << eol;
                }
            }ind_end;
        }
    }
    if (n > 0) {
        doc::PutSecthead(out, "wire-layout", "Wire Layout");
        out << "| " << doc::Count(n, "field") << " | offset | bytes | reads as |" << eol
            << "|---|---|---|---|" << eol << body << eol;
    }
}

// Return the type the message NAME is dispatched by, as its author wrote it.
//
// The expression is kept rather than the number amc computes from it, because half of
// what it says is in the spelling: `'A'` is a character the protocol puts on the wire
// and 65 is the same value with that fact taken off.
static tempstr Msgtypeof(algo::strptr name) {
    doc::FMsgtype *msgtype = doc::ind_msgtype_Find(name);
    tempstr ret;
    if (msgtype) {
        ret << msgtype->type;
    }
    return ret;
}

// Return the roots of every protocol in the tree, as markdown.
//
// A root is a ctype carrying a type field, which is the whole of what makes it a header:
// a reader of the wire takes that field and learns which message they are holding.  So
// this table is the list of protocols, and each entry opens the one it names.
tempstr doc::MsgrootText() {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_typefld_curs, typefld, doc::_db) {
        algo::strptr name = Hdrctype(typefld.field);
        n++;
        body << "|";
        doc::PutTablecell(body, tempstr() << "[" << name << "](msg:" << name << ")");
        doc::PutTablecell(body, tempstr() << "`" << Pathcomp(typefld.field, ".RR") << "`");
        doc::PutTablecell(body, Ctypecomment(name));
        body << eol;
    }ind_end;
    tempstr ret;
    ret << "## Messages" << eol << "<a href=\"#messages\"></a>" << eol << eol;
    ret << "A message header and the messages that begin with it are one protocol." << eol << eol;
    ret << "| " << doc::Count(n, "protocol") << " | type field | what it is |" << eol
        << "|---|---|---|" << eol << body << eol;
    return ret;
}

// Append the messages that begin with the header NAME to OUT, each leading to its page.
static void PutMsgset(algo::cstring &out, algo::strptr name) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_field_curs, field, doc::_db) {
        bool base = field.reftype == "Base" && field.arg == name;
        algo::strptr msg = Hdrctype(field.field);
        if (base) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << msg << "](msg:" << msg << ")");
            doc::PutTablecell(body, tempstr() << "`" << Msgtypeof(msg) << "`");
            doc::PutTablecell(body, Msgsize(msg));
            doc::PutTablecell(body, Ctypecomment(msg));
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "messages", "Messages");
        out << "| " << doc::Count(n, "message") << " | type | size | what it carries |" << eol
            << "|---|---|---|---|" << eol << body << eol;
    }
}

// Append the dispatches that receive the message NAME to OUT, each leading to its handler.
//
// A dispatch is a multi-way branch over a header's type field, and a row of
// dmmeta.dispatch_msg says this message is one of its cases.  Following the row reaches
// the handler's source, so a reader walks from the wire to the code that reads it in two
// steps and without knowing the name of either.
static void PutMsgrecv(algo::cstring &out, algo::strptr name) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_dispatch_msg_curs, dispatch_msg, doc::_db) {
        if (doc::ctype_Get(dispatch_msg) == name) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << doc::dispatch_Get(dispatch_msg)
                              << "](dispatch_msg:" << dispatch_msg.dispatch_msg << ")");
            doc::PutTablecell(body, Dispatchcomment(doc::dispatch_Get(dispatch_msg)));
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "received-in", "Received In");
        out << "| " << doc::Count(n, "dispatch") << " | what it reads |" << eol
            << "|---|---|" << eol << body << eol;
    }
}

// Return what the page about the message NAME says beyond its declaration, as markdown.
//
// Two questions, and a message answers one of them or the other.  A header answers what
// begins with it, which is the protocol it heads.  A message answers where it is read,
// which is every dispatch carrying it as a case.  A ctype that is neither -- a message
// nobody dispatches, or a plain record somebody typed the word at -- answers with
// nothing, and the page is then its declaration alone.
tempstr doc::MsgText(algo::strptr name) {
    tempstr ret;
    PutMsgset(ret, name);
    PutMsgwire(ret, name);
    PutMsgrecv(ret, name);
    return ret;
}

// Lay the page about the message location LOC onto the page at the terminal.
void doc::RenderMsg(algo::strptr loc) {
    algo::strptr name = Pathcomp(loc, ":LR");
    if (ch_N(name) == 0) {
        doc::ParseText(doc::MsgrootText());
        doc::RenderDoc();
    } else {
        doc::RenderCtype(name);
        tempstr text(doc::MsgText(name));
        if (ch_N(text) > 0) {
            doc::EndRow();
            doc::ParseText(text);
            doc::RenderDoc();
        }
    }
}
