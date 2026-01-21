// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Target: bash2html (exe) -- Convert bash output and colours to html
// Exceptions: yes
// Source: cpp/bash2html.cpp
//

#include "include/algo.h"
#include "include/bash2html.h"

const char esc = '\x1b';

const char * const default_background = "Black";
const char * const default_colour = "White";

static void TestOutput (){
    prlog("Test \x1b[31mRed\x1b[0m Test.");
    prlog("Test \x1b[1;31mBold Red\x1b[0m Test.");
    prlog("spaces                spaces");
    prlog(" <&' \" <&>>");
    prlog("\x1b]0;xterm set window title\x07");
    prlog("Normal \x1b[1mBold\x1b[0m");
    prlog("Normal \x1b[2mLight\x1b[0m");
    prlog("Normal \x1b[3mItalic\x1b[0m");
    prlog("Normal \x1b[4mUnderline\x1b[0m");
    prlog("Normal \x1b[9mline through\x1b[0m");
    prlog("Text \x1b[39mDefault\x1b[0m");
    prlog("Text \x1b[30mBlack\x1b[0m");
    prlog("Text \x1b[31mRed\x1b[0m");
    prlog("Text \x1b[32mGreen\x1b[0m");
    prlog("Text \x1b[33mYellow\x1b[0m");
    prlog("Text \x1b[34mBlue\x1b[0m");
    prlog("Text \x1b[35mMagenta\x1b[0m");
    prlog("Text \x1b[36mCyan\x1b[0m");
    prlog("Text \x1b[37mLight gray\x1b[0m");
    prlog("Text \x1b[90mDark gray\x1b[0m");
    prlog("Text \x1b[91mLight red\x1b[0m");
    prlog("Text \x1b[92mLight green\x1b[0m");
    prlog("Text \x1b[93mLight yellow\x1b[0m");
    prlog("Text \x1b[94mLight blue\x1b[0m");
    prlog("Text \x1b[95mLight magenta\x1b[0m");
    prlog("Text \x1b[96mLight cyan\x1b[0m");
    prlog("Text \x1b[97mWhite\x1b[0m");
    prlog("Background \x1b[49mDefault\x1b[0m");
    prlog("Background \x1b[40mBlack\x1b[0m");
    prlog("Background \x1b[41mRed\x1b[0m");
    prlog("Background \x1b[42mGreen\x1b[0m");
    prlog("Background \x1b[43mYellow\x1b[0m");
    prlog("Background \x1b[44mBlue\x1b[0m");
    prlog("Background \x1b[45mMagenta\x1b[0m");
    prlog("Background \x1b[46mCyan\x1b[0m");
    prlog("Background \x1b[47mLight gray\x1b[0m");
    prlog("Background \x1b[100mDark gray\x1b[0m");
    prlog("Background \x1b[101mLight red\x1b[0m");
    prlog("Background \x1b[102mLight green\x1b[0m");
    prlog("Background \x1b[103mLight yellow\x1b[0m");
    prlog("Background \x1b[104mLight blue\x1b[0m");
    prlog("Background \x1b[105mLight magenta\x1b[0m");
    prlog("Background \x1b[106mLight cyan\x1b[0m");
    prlog("Background \x1b[107mWhite\x1b[0m");
    prlog("\x1b[41m Red background  \x1b[32mGreen\x1b[49m text \x1b[0m");
}

struct BashState {
    struct State {
        enum StateType {
            none,
            color,
            background_color,
            font_weight,
            font_style,
            underlined,
            crossedout,
            NUM_STATES
        };

        StateType type;
        cstring span;

        State ()
            : type(none)
        {}

        State(const StateType &_type, strptr _span): type(_type), span(_span) {}
    };

    State current_states[State::NUM_STATES];

    tempstr ClearAll(){
        tempstr out;
        for (int i = 0; i != State::NUM_STATES; ++i){
            State &s = current_states[i];
            if(s.type != State::none){
                s = State();
                out << "</span>";
            }
        }
        return out;
    }


    tempstr Set(const State& state){
        tempstr out;

        out << Clear(state.type);

        for (int i = 0; i != State::NUM_STATES; ++i){
            State &s = current_states[i];
            if(s.type == State::none){
                s = state;
                out << state.span;
                break;
            }
        }

        return out;
    }

    tempstr Set(const State::StateType & type, strptr span){
        return Set(State(type, span));
    }

    tempstr Clear(const State::StateType& type){
        tempstr out;

        int loc = -1;
        int nones = -1;

        for (int i = 0; i != State::NUM_STATES; ++i){
            State &s = current_states[i];
            if(s.type == type){
                loc = i;
            }
            if(s.type == State::none){
                nones = i;
                if(loc == -1){
                    loc = nones;
                }
                break;
            }
        }

        for(int i = loc; i != nones; ++i){
            out << "</span>";
        }

        for(int i = loc; i != nones; ++i){
            State &s = current_states[i];
            s = current_states[i + 1];
            out << s.span;
        }

        return out;
    }

};

static tempstr ProcessEscapeSequence(const strptr& seq, BashState& bs) {
    tempstr out;

    algo_lib::CsvParse parsecsv;
    parsecsv.sep = ';';
    parsecsv.input = seq;
    RunCsvParse(parsecsv);

    for (int i = 0; i != ary_tok_N(parsecsv); ++i){
        u32 code = 0;
        algo::StringIter si(ary_tok_qFind(parsecsv,i));
        /*const bool passed =*/
        TryParseU32(si, code);

        switch(code){
        case 0: out << bs.ClearAll(); break;
        case 1: out << bs.Set(BashState::State::font_weight, "<span style=\"font-weight: bolder\">"); break;
        case 2: out << bs.Set(BashState::State::font_weight, "<span style=\"font-weight: lighter\">"); break;
        case 3: out << bs.Set(BashState::State::font_style, "<span style=\"font-style: italic\">"); break;
        case 4: out << bs.Set(BashState::State::underlined, "<span style=\"text-decoration-line: underline\">"); break;
        case 9: out << bs.Set(BashState::State::crossedout, "<span style=\"text-decoration-line: line-through\">"); break;
        case 22: out << bs.Clear(BashState::State::font_weight); break;
        case 23: out << bs.Clear(BashState::State::font_style); break;
        case 24: out << bs.Clear(BashState::State::underlined); break;
        case 29: out << bs.Clear(BashState::State::crossedout); break;
        case 30: out << bs.Set(BashState::State::color, "<span style=\"color: Black\">"); break;
        case 31: out << bs.Set(BashState::State::color, "<span style=\"color: Red\">"); break;
        case 32: out << bs.Set(BashState::State::color, "<span style=\"color: Green\">"); break;
        case 33: out << bs.Set(BashState::State::color, "<span style=\"color: Yellow\">"); break;
        case 34: out << bs.Set(BashState::State::color, "<span style=\"color: Blue\">"); break;
        case 35: out << bs.Set(BashState::State::color, "<span style=\"color: Magenta\">"); break;
        case 36: out << bs.Set(BashState::State::color, "<span style=\"color: Cyan\">"); break;
        case 37: out << bs.Set(BashState::State::color, "<span style=\"color: Lightgray\">"); break;
        case 39: out << bs.Clear(BashState::State::color); break;
        case 90: out << bs.Set(BashState::State::color, "<span style=\"color: Darkgray\">"); break;
        case 91: out << bs.Set(BashState::State::color, "<span style=\"color: #ff8080\">"); break;
        case 92: out << bs.Set(BashState::State::color, "<span style=\"color: Lightgreen\">"); break;
        case 93: out << bs.Set(BashState::State::color, "<span style=\"color: Lightyellow\">"); break;
        case 94: out << bs.Set(BashState::State::color, "<span style=\"color: Lightblue\">"); break;
        case 95: out << bs.Set(BashState::State::color, "<span style=\"color: #ff80ff\">"); break;
        case 96: out << bs.Set(BashState::State::color, "<span style=\"color: Lightcyan\">"); break;
        case 97: out << bs.Set(BashState::State::color, "<span style=\"color: White\">"); break;
        case 40:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Black\">"); break;
        case 41:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Red\">"); break;
        case 42:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Green\">"); break;
        case 43:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Yellow\">"); break;
        case 44:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Blue\">"); break;
        case 45:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Magenta\">"); break;
        case 46:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Cyan\">"); break;
        case 47:  out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Lightgray\">"); break;
        case 49:  out << bs.Clear(BashState::State::background_color); break;
        case 100: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Darkgray\">"); break;
        case 101: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: #ff8080\">"); break;
        case 102: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Lightgreen\">"); break;
        case 103: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Lightyellow\">"); break;
        case 104: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Lightblue\">"); break;
        case 105: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: #ff80ff\">"); break;
        case 106: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: Lightcyan\">"); break;
        case 107: out << bs.Set(BashState::State::background_color, "<span style=\"background-color: White\">"); break;
        default: out << "ESC [" << code << "m"; break;
        }
    }
    return out;
}

static tempstr ProcessEscapeSequences(strptr & line, BashState &bs) {
    if (elems_N(line) < 2){
        return tempstr() << "ESC ";
    } else if (line[1] != '[') { // is not the Control string initiator
        line = RestFrom(line, 1);
        return tempstr() << "ESC " << line[0];
    } else {
        const int m_pos = Find(line, 'm');
        if (m_pos == -1){
            return tempstr();
        } else {
            const strptr esc_seq = GetRegion(line, 2, m_pos-2);

            line = RestFrom(line, m_pos);
            return ProcessEscapeSequence(esc_seq, bs);
        }
    }
}

static void BashLineToHtml(strptr line, BashState &bs){
    cstring out;
    for (; 0 != elems_N(line); line = RestFrom(line, 1)){
        const char & c = line[0];
        switch(c) {
        case esc:
            //found bash escape code
            out << ProcessEscapeSequences(line, bs);
            break;
        case '<':
            out << "&lt;";
            break;
        case '>':
            out << "&gt;";
            break;
        case '&':
            out << "&amp;";
            break;
        case '"':
            out << "&quot;";
            break;
        case '\'':
            out << "&apos;";
            break;
        case ' ':
            out << "&nbsp;";
            break;
        case '\r':
            //nothing?
            break;
        case '\n':
            out << "<br>";
            break;
        default:
            out << c;
            break;
        }

    }
    prlog(out);

}

static tempstr Header() {
    return tempstr()
        << "<!DOCTYPE html><html>"
        << "<head><meta charset=\"utf-8\"/></head>"
        << "<body style=\"white-space: pre;"
        <<     "background-color:" << default_background << ";"
        <<     "\"><font face=\"Lucida Concole, Monaco, monospace\">"
        << "<span style=\""
        << "color: " << default_colour << ";"
        << "background-color: " << default_background<< ";"
        << "font-weight: Normal;"
        << "font-style: Normal;"
        << "text-decoration-line: none;"
        << "\">";
}

static tempstr Footer() {
    return tempstr()
        << "</span>"
        << "</font></body></html>"
        ;
}

static void ToHtml(){
    prlog(Header());
    BashState bs;
    ind_beg(algo::FileLine_curs,line,algo::Fildes(0)) {
        BashLineToHtml(line, bs);
    }ind_end;
    prlog(Footer());
}

void bash2html::Main() {
    if (_db.cmdline.test){
        TestOutput();
    } else {
        ToHtml();
    }
}
