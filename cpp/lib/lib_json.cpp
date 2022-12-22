// (C) 2014-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: lib_json (lib) -- Full json support library
// Exceptions: NO
// Source: cpp/lib/lib_json.cpp
//
// Created By: alexei.lebedev alexey.polovinkin
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev alexey.polovinkin
//

#include "include/algo.h"

static void JsonParseError(lib_json::FParser &parser, strptr info) {
    if (lib_json::JsonParseOkQ(parser)) {
        parser.state      = lib_json_FParser_state_err;
        parser.err_info   = info;
        parser.err_offset = parser.offset + parser.ind;
    }
}

// Note: this detects duplicate field names.
inline bool NodeParentingRulesOkQ(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr value) {
    bool ok = false;
    bool isfield = type == lib_json_FNode_type_field;
    ok     |= parent == NULL && type != lib_json_FNode_type_field;
    ok     |= (parent && parent->type == lib_json_FNode_type_array  && !isfield);
    ok     |= (parent && parent->type == lib_json_FNode_type_field  && !isfield && !c_child_N(*parent));
    ok     |= (parent && parent->type == lib_json_FNode_type_object && isfield
               && !ind_objfld_Find(lib_json::FldKey(parent->p_parent,value)));
    return ok;
}

// Create new node
// do not check hierarchy rules
static lib_json::FNode &qNewNode(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr value) {
    lib_json::FNode &node = lib_json::node_Alloc();
    node.p_parent           = parent ? parent : &node;
    node.type             = type;
    node.value            = value;
    node_XrefMaybe(node);
    return node;
}

// Create new node
// enforce hierarchy rules
lib_json::FNode &lib_json::NewNode(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr field) {
    bool ok = NodeParentingRulesOkQ(parent,type,field);
    vrfy(ok, tempstr()<<"lib_json.bad_node_nesting"
         <<Keyval("type", ((u32)type))
         <<Keyval("parent",(parent?type_ToCstr(*parent):"NULL"))
         <<Keyval("comment","Bad combination of field types, or duplicate field name under object"));
    return qNewNode(parent,type,field);
}

static lib_json::FNode &NewNodeSmart(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr field) {
    bool is_obj = parent && parent->type == lib_json_FNode_type_object;
    vrfy(is_obj == !!elems_N(field),"wrong use of field name");
    if (is_obj) {
        parent = &qNewNode(parent,lib_json_FNode_type_field,field);
    }
    return qNewNode(parent, type, strptr());
}

lib_json::FNode &lib_json::NewFieldNode(lib_json::FNode *parent, strptr field) {
    // note: not smart!
    return NewNode(parent,lib_json_FNode_type_field,field);
}

lib_json::FNode &lib_json::NewObjectNode(lib_json::FNode *parent, strptr field) {
    return NewNodeSmart(parent, lib_json_FNode_type_object, field);
}

lib_json::FNode &lib_json::NewArrayNode(lib_json::FNode *parent, strptr field) {
    return NewNodeSmart(parent, lib_json_FNode_type_array, field);
}

lib_json::FNode &lib_json::NewStringNode(lib_json::FNode *parent, strptr field, strptr value) {
    lib_json::FNode &node = NewNodeSmart(parent, lib_json_FNode_type_string, field);
    node.value = value;
    return node;
}

lib_json::FNode &lib_json::NewNumberNode(lib_json::FNode *parent, strptr field, strptr value) {
    lib_json::FNode &node = NewNodeSmart(parent, lib_json_FNode_type_number, field);
    node.value = value;
    return node;
}

// add new node as child of current node (or as root node)
// NULL on error
static lib_json::FNode *JsonNodeAdd(lib_json::FParser &parser, lib_json_FNode_type_Enum type, strptr value=strptr()) {
    if (!NodeParentingRulesOkQ(parser.node,type,value)) {
        JsonParseError(parser,"misplaced colon");
        return NULL;
    }
    lib_json::FNode &node = qNewNode(parser.node,type,value);
    if (!parser.root_node) {
        parser.root_node = &node;
        parser.node      = &node;
    }
    return &node;
}

// move one level up in tree
static void JsonUp(lib_json::FParser &parser) {
    if (parser.node) {
        parser.node = (parser.node->p_parent != parser.node ? parser.node->p_parent : NULL);
    } else {
        JsonParseError(parser,"closing curly brace or bracket has no opening match");
    }
}

// Open object
static void JsonObjectOpen(lib_json::FParser &parser) {
    parser.node = JsonNodeAdd(parser,lib_json_FNode_type_object);
    parser.need_comma = false;
    parser.have_comma = false;
}

// Close object
static void JsonObjectClose(lib_json::FParser &parser) {
    if (!parser.need_comma && parser.have_comma) {
        JsonParseError(parser,"value expected");
        return;
    }
    if (parser.node && parser.node->type == lib_json_FNode_type_field) {
        if (!c_child_N(*parser.node)) {
            JsonParseError(parser,"field value expected");
            return;
        }
        JsonUp(parser);
    }
    if (parser.node && parser.node->type == lib_json_FNode_type_object) {
        parser.need_comma = true;
        parser.have_comma = false;
        JsonUp(parser);
    } else {
        JsonParseError(parser,"closing curly brace mismatch");
    }
}

// Open array
static void JsonArrayOpen(lib_json::FParser &parser) {
    parser.node = JsonNodeAdd(parser,lib_json_FNode_type_array);
    parser.need_comma = false;
    parser.have_comma = false;
}

// Close array
static void JsonArrayClose(lib_json::FParser &parser) {
    if (!parser.need_comma && parser.have_comma) {
        JsonParseError(parser,"value expected");
        return;
    }
    if (parser.node && parser.node->type == lib_json_FNode_type_array) {
        parser.need_comma = true;
        parser.have_comma = false;
        JsonUp(parser);
    } else {
        JsonParseError(parser,"closing bracket mismatch");
    }
}

// New field
static void JsonField(lib_json::FParser &parser) {
    if (!parser.need_colon) {
        JsonParseError(parser,"unexpected colon");
    }
    parser.need_colon = false;
    if (!parser.node || !(parser.node->type == lib_json_FNode_type_field)) {
        JsonParseError(parser,"unexpected colon");
    }
}

// Next element
static void JsonNextElem(lib_json::FParser &parser) {
    if (!parser.need_comma) {
        JsonParseError(parser,"unexpected comma");
        return;
    }
    parser.need_comma = false;
    parser.have_comma = true;
    if (parser.node && parser.node->type == lib_json_FNode_type_field) {
        JsonUp(parser);
    }
    bool ok = parser.node
        && (parser.node->type == lib_json_FNode_type_object
            || parser.node->type == lib_json_FNode_type_array);
    if (!ok) {
        JsonParseError(parser,"unexpected comma");
    }
}

// Handle number
static void JsonNumber(lib_json::FParser &parser) {
    JsonNodeAdd(parser,lib_json_FNode_type_number,parser.value);
    parser.need_comma = true;
    parser.have_comma = false;
}

// Handle string
static void JsonString(lib_json::FParser &parser) {
    if (parser.node && parser.node->type == lib_json_FNode_type_object) {
        parser.node = JsonNodeAdd(parser,lib_json_FNode_type_field,parser.value);
        parser.need_colon = true;
    } else {
        JsonNodeAdd(parser,lib_json_FNode_type_string,parser.value);
        parser.need_comma = true;
        parser.have_comma = false;
    }
}

// Handle token
static void JsonToken(lib_json::FParser &parser) {
    if (parser.value == "true") {
        JsonNodeAdd(parser,lib_json_FNode_type_true);
        parser.need_comma = true;
        parser.have_comma = false;
    } else if (parser.value == "false") {
        JsonNodeAdd(parser,lib_json_FNode_type_false);
        parser.need_comma = true;
        parser.have_comma = false;
    } else if (parser.value == "null") {
        JsonNodeAdd(parser,lib_json_FNode_type_null);
        parser.need_comma = true;
        parser.have_comma = false;
    } else if (parser.value == "throw" && !parser.root_node) {
        parser.state = lib_json_FParser_state_sec_line;
    } else {
        JsonParseError(parser,tempstr()<<"bad token: "<<parser.value);
    }
}

// Skip whitespace
static void JsonSkipWs(lib_json::FParser &parser) {
    for (; parser.ind<elems_N(parser.buf); ++parser.ind) {
        if (!algo_lib::WhiteCharQ(parser.buf[parser.ind])) {
            parser.state = lib_json_FParser_state_value;
            break;
        }
    }
}

// Parse number
static void JsonParseNumber(lib_json::FParser &parser) {
    bool done = !elems_N(parser.buf);
    i32  beg  = parser.ind;
    for (;parser.ind<elems_N(parser.buf); ++parser.ind) {
        if (!lib_json::JsonNumCharQ(parser.buf[parser.ind])) {
            done = true;
            break;
        }
    }
    ch_Addary(parser.value,RestFrom(FirstN(parser.buf,parser.ind),beg));
    if (done) {
        parser.state = lib_json_FParser_state_ws;
        JsonNumber(parser);
        ch_RemoveAll(parser.value);
    }
}

// encode as utf-8
// todo check and not encode:
// d800-dfff  : UTF-16 uses d800-dfff to encode it's surrogate pairs, i.e. values that don't fit in to 16 bits.
//              This means UTF-8/UTF-32 are not allowed to encode these values.
// fdd0-fdef  : Non-characters.
// xfffe-xffff: Unassigned: x ranges from 0 to 10 (hex), so the values to check for are fffe-ffff, 1fffe-1ffff, etc.

static void Utf8Encode(u32 c, cstring &out) {
    if (c<0x80) {
        out << char(c);
    } else if (c<0x0800) {
        out << char((c>>6&0x1f)|0xc0)  << char((c&0x3f)|0x80);
    } else if (c<0x010000) {
        out << char((c>>12&0x0f)|0xe0) << char((c>>6&0x3f)|0x80)  << char((c>>0&0x3f)|0x80);
    } else if (c<0x110000) {
        out << char((c>>18&0x07)|0xf0) << char((c>>12&0x3f)|0x80) << char((c>>6&0x3f)|0x80) << char((c>>0&0x3f)|0x80);
    }
    // 10ffff is the highest defined codepoint
}

// expect low surrogate backslash
static void JsonParseStringSurrogate1(lib_json::FParser &parser) {
    if (parser.ind<elems_N(parser.buf)) {
        if (parser.buf[parser.ind++] == '\\') {
            parser.state = lib_json_FParser_state_str_usurr2;
        } else {
            JsonParseError(parser,"UTF-16: low surrogate expected");
        }
    }
}

// expect low surrogate u
static void JsonParseStringSurrogate2(lib_json::FParser &parser) {
    if (parser.ind<elems_N(parser.buf)) {
        if (parser.buf[parser.ind++] == 'u') {
            parser.state = lib_json_FParser_state_str_uesc;
            parser.uesc_need = 4;
        } else {
            JsonParseError(parser,"UTF-16: low surrogate expected");
        }
    }
}

// parse unicode string escape
static void JsonParseStringUnicode(lib_json::FParser &parser) {
    bool ok = true;
    for (;ok && parser.uesc_need && parser.ind<elems_N(parser.buf);--parser.uesc_need,++parser.ind) {
        u8 dig = 0;
        ok &= algo::ParseHex1(parser.buf[parser.ind],dig) == 1;
        parser.uesc_value <<= 4;
        parser.uesc_value  |= dig;
    }
    if (!ok) {
        tempstr info("unrecognized character within unicode char escape: code \\x");
        u64_PrintHex(parser.buf[parser.ind-1],info,2,false,true);
        JsonParseError(parser,info);
        return;
    }
    if (!parser.uesc_need) {
        // decode utf-16
        if (parser.uesc_value<0xd800) {
            // BMP chars
            Utf8Encode(parser.uesc_value,parser.value);
            parser.uesc_value = 0;
            parser.state = lib_json_FParser_state_str;
        } else if (parser.uesc_value<0xdc00) {
            // high surrogate
            parser.state = lib_json_FParser_state_str_usurr1;
        } else if (parser.uesc_value<0xe000) {
            // low surrogate, unexpected
            tempstr info("UTF-16: unexpected low surrogate: \\u");
            u64_PrintHex(parser.uesc_value,info,4,false,true);
            JsonParseError(parser,info);
        } else if (parser.uesc_value<0x10000) {
            // rest BMP chars
            Utf8Encode(parser.uesc_value,parser.value);
            parser.uesc_value = 0;
            parser.state = lib_json_FParser_state_str;
        } else if ((parser.uesc_value&0xfc00) == 0xdc00) {
            // high word - high surrogate, low word - low surrogate
            u32 value = parser.uesc_value>>6&0x03fffc00;
            value    += parser.uesc_value&0xffff;
            value    -= 0x35FDC00;
            Utf8Encode(value,parser.value);
            parser.uesc_value = 0;
            parser.state = lib_json_FParser_state_str;
        } else {
            // error, not a low surrogate
            tempstr info("UTF-16: bad low surrogate: \\u");
            u64_PrintHex(parser.uesc_value&0xffff,info,4,false,true);
            JsonParseError(parser,info);
        }
    }
}

// parse string escape
static void JsonParseStringEscape(lib_json::FParser &parser) {
    if (parser.ind<elems_N(parser.buf)) {
        parser.state = lib_json_FParser_state_str;
        switch (parser.buf[parser.ind]) {
        case '"' :
        case '\\':
        case '/' : parser.value << parser.buf[parser.ind]; break;
        case 'b' : parser.value << '\b';     break;
        case 'f' : parser.value << '\f';     break;
        case 'n' : parser.value << '\n';     break;
        case 'r' : parser.value << '\r';     break;
        case 't' : parser.value << '\t';     break;
        case 'u' : parser.state = lib_json_FParser_state_str_uesc; parser.uesc_need = 4; break;
        default  : {
            tempstr info;
            info << "unrecognized string escape character: code \\x";
            u64_PrintHex(parser.buf[parser.ind],info,2,false,true);
            JsonParseError(parser,info);
            return;
        }
        }
        ++parser.ind;
    }
}

// parse string
static void JsonParseString(lib_json::FParser &parser) {
    bool done = false;
    for (; !done && parser.ind<elems_N(parser.buf); ++parser.ind) {
        switch(parser.buf[parser.ind]) {
        case '\\': parser.state = lib_json_FParser_state_str_esc; ++parser.ind; return; // return immediately
        case '"' : done = true;                            break;
        default  : parser.value << parser.buf[parser.ind]; break;
        }
    }
    if (done) {
        parser.state = lib_json_FParser_state_ws;
        JsonString(parser);
        ch_RemoveAll(parser.value);
    }
}

// parse token
static void JsonParseToken(lib_json::FParser &parser) {
    bool done = !elems_N(parser.buf);
    i32 beg = parser.ind;
    for (;parser.ind<elems_N(parser.buf); ++parser.ind) {
        if (!algo_lib::AlphaCharQ(parser.buf[parser.ind])) {
            done = true;
            break;
        }
    }
    ch_Addary(parser.value,RestFrom(FirstN(parser.buf,parser.ind),beg));
    if (done) {
        parser.state = lib_json_FParser_state_ws;
        JsonToken(parser);
        ch_RemoveAll(parser.value);
    }
}

// parse security line
static void JsonParseSecLine(lib_json::FParser &parser) {
    bool done = !elems_N(parser.buf);
    for (;parser.ind<elems_N(parser.buf); ++parser.ind) {
        if (parser.buf[parser.ind] == ';') {
            ++parser.ind;
            done = true;
            break;
        }
    }
    if (done) {
        parser.state = lib_json_FParser_state_ws;
    }
}

// parse value
static void JsonParseValue(lib_json::FParser &parser) {
    if (parser.ind<elems_N(parser.buf)) {
        char c = parser.buf[parser.ind];
        bool ok = !parser.need_comma || c==',' || c==']' || c=='}';
        if (!ok) {
            JsonParseError(parser,"comma expected");
            return;
        }
        ok = !parser.need_colon || c==':';
        if (!ok) {
            JsonParseError(parser,"colon expected");
            return;
        }
        switch(c) {
        case '{':   ++parser.ind; parser.state = lib_json_FParser_state_ws; JsonObjectOpen(parser);  break;
        case '}':   ++parser.ind; parser.state = lib_json_FParser_state_ws; JsonObjectClose(parser); break;
        case '[':   ++parser.ind; parser.state = lib_json_FParser_state_ws; JsonArrayOpen(parser);   break;
        case ']':   ++parser.ind; parser.state = lib_json_FParser_state_ws; JsonArrayClose(parser);  break;
        case '"':   ++parser.ind; parser.state = lib_json_FParser_state_str;                         break;
        case ':':   ++parser.ind; parser.state = lib_json_FParser_state_ws; JsonField(parser);       break;
        case ',':   ++parser.ind; parser.state = lib_json_FParser_state_ws; JsonNextElem(parser);    break;
        default :
            if (lib_json::JsonFirstNumCharQ(parser.buf[parser.ind])) {
                parser.state = lib_json_FParser_state_number;
            } else                                           { parser.state = lib_json_FParser_state_token; }
            break;
        }
    }
}

// dispatch to parser routine
static void JsonParseDispatch(lib_json::FParser &parser) {
    switch (parser.state) {
    case lib_json_FParser_state_ws        : JsonSkipWs(parser);                break;
    case lib_json_FParser_state_value     : JsonParseValue(parser);            break;
    case lib_json_FParser_state_number    : JsonParseNumber(parser);           break;
    case lib_json_FParser_state_str       : JsonParseString(parser);           break;
    case lib_json_FParser_state_str_esc   : JsonParseStringEscape(parser);     break;
    case lib_json_FParser_state_str_uesc  : JsonParseStringUnicode(parser);    break;
    case lib_json_FParser_state_str_usurr1: JsonParseStringSurrogate1(parser); break;
    case lib_json_FParser_state_str_usurr2: JsonParseStringSurrogate2(parser); break;
    case lib_json_FParser_state_token     : JsonParseToken(parser);            break;
    case lib_json_FParser_state_sec_line  : JsonParseSecLine(parser);          break;
    case lib_json_FParser_state_err       : parser.ind = elems_N(parser.buf);  break; // blackhole
    default: vrfy_(0);
    }
}

inline bool ScalarNodeQ(lib_json::FNode &node) {
    return node.type != lib_json_FNode_type_field
        && node.type != lib_json_FNode_type_array
        && node.type != lib_json_FNode_type_object;
}

// Main parser routine - parse buffer
bool lib_json::JsonParse(lib_json::FParser &parser, strptr buf) {
    parser.buf = buf;
    parser.ind = 0;
    do {
        // at least one time to finalize
        JsonParseDispatch(parser);
    } while (parser.ind<elems_N(parser.buf));
    // check final state
    bool ok = elems_N(parser.buf)
        || (parser.state == lib_json_FParser_state_ws
            && (!parser.node  || (parser.node == parser.root_node && ScalarNodeQ(*parser.node))));
    if (!ok) {
        JsonParseError(parser,"unexpected end of input");
    }
    // advance offset
    parser.offset += elems_N(parser.buf);
    return lib_json::JsonParseOkQ(parser);
}

// AMC cleanup function - automatically delete parsed JSON tree
void lib_json::root_node_Cleanup(lib_json::FParser& parent) {
    if (parent.root_node) {
        node_Delete(*parent.root_node);
        parent.root_node = NULL;
    }
}

// encode json string
// "The representation of strings is similar to conventions used in the C
// family of programming languages.  A string begins and ends with
// quotation marks.  All Unicode characters may be placed within the
// quotation marks except for the characters that must be escaped:
// quotation mark, reverse solidus, and the control characters (U+0000
// through U+001F)."
//    -- this says that solidus need not be escaped when printing -- only when parsing!
static void JsonSerializeString(strptr str, cstring &lhs) {
    lhs << '"';
    frep_(i,elems_N(str)) {
        switch (str[i]) {
        case '"' :  lhs << '\\' << str[i]; break;
        case '\\':  lhs << '\\' << str[i]; break;
        case '\b':  lhs << "\\b";          break;
        case '\f':  lhs << "\\f";          break;
        case '\n':  lhs << "\\n";          break;
        case '\r':  lhs << "\\r";          break;
        case '\t':  lhs << "\\t";          break;
        default  :  if (str[i] & ~0x1f) {
                lhs << str[i];
            } else {
                // escape control chars
                lhs << "\\u";
                u64_PrintHex(str[i],lhs,4,false,true);
            }
            break;
        }
    }
    lhs << '"';
}

// Serialize to string
void lib_json::JsonSerialize(lib_json::FNode* node, cstring &lhs, bool pretty, u32 indent) {
    if (node) {
        switch (node->type) {
        case lib_json_FNode_type_null  :    lhs<<"null";      break;
        case lib_json_FNode_type_true  :    lhs<<"true";      break;
        case lib_json_FNode_type_false :    lhs<<"false";     break;
        case lib_json_FNode_type_number:    lhs<<node->value; break;

        case lib_json_FNode_type_string: {
            JsonSerializeString(node->value,lhs);
            break;
        }

        case lib_json_FNode_type_field : {
            // silently omit malformed fields
            if (c_child_N(*node)==1) {
                JsonSerializeString(node->value,lhs);
                lhs<<':';
                lib_json::JsonSerialize(c_child_Find(*node,0),lhs,pretty,indent);
            }
        } break;

        case lib_json_FNode_type_array:
        case lib_json_FNode_type_object: {
            lhs<< (node->type == lib_json_FNode_type_object ? '{' : '[');
            indent++;
            algo::ListSep ls(",");
            ind_beg(lib_json::node_c_child_curs, child, *node) {
                if (pretty) {
                    lhs<< '\n';
                    char_PrintNTimes(' ',lhs,indent*4);
                }
                lhs<<ls;
                lib_json::JsonSerialize(&child,lhs,pretty,indent);
            }ind_end;
            indent--;
            if (pretty) {
                lhs<<'\n';
                char_PrintNTimes(' ',lhs,indent*4);
            }
            lhs<< (node->type == lib_json_FNode_type_object ? '}' : ']');
        } break;

        default:
            vrfy_(0);
        }
    }
}


// find node in object chain
// path is dot-separated list of field keys
// e.g. path abc.def.ghi and parent {"abc:{"def":{"ghi":value}}}
// yields value node
lib_json::FNode* lib_json::node_Find(lib_json::FNode* parent, strptr path) {
    algo::StringIter it(path);
    while (!it.EofQ()) {
        if (!parent || parent->type!=lib_json_FNode_type_object) {
            return NULL;
        }
        parent = ind_objfld_Find(lib_json::FldKey(parent,GetTokenChar(it,'.')));
    }
    return parent;
}

// get node value as string, empty in case if path is not found, or structural node
// boolean is converted to 0/1 (to be shown as numeric)
// null is empty string
strptr lib_json::strptr_Get(lib_json::FNode* parent, strptr path) {
    lib_json::FNode* node = lib_json::node_Find(parent,path);
    strptr ret;
    if (node) {
        switch (node->type) {
        case lib_json_FNode_type_true  :   ret = "1"           ; break;
        case lib_json_FNode_type_false :   ret = "0"           ; break;
        case lib_json_FNode_type_string:
        case lib_json_FNode_type_number:   ret = node->value   ; break;
        default: break;
        }
    }
    return ret;
}

// get array node, NULL in case of any error
// NULL if not found or not an array
lib_json::FNode* lib_json::node_GetArray(lib_json::FNode* parent, strptr path) {
    lib_json::FNode* node = node_Find(parent,path);
    return (!node || node->type != lib_json_FNode_type_array) ? NULL : node;
}

// get node value as u32
// boolean is converted to 0/1 (to be shown as numeric)
// exception on any error
u32 lib_json::u32_Get(lib_json::FNode* parent, strptr path) {
    lib_json::FNode* node = lib_json::node_Find(parent,path);
    vrfy(node,"u32_Get: path is not found");
    u32 ret=0;
    if (node) {
        switch (node->type) {
        case lib_json_FNode_type_true  :   ret = 1;                             break;
        case lib_json_FNode_type_false :   ret = 0;                             break;
        case lib_json_FNode_type_number:
            vrfy(u32_ReadStrptrMaybe(ret,node->value), algo_lib::_db.errtext);
            break;
        default:                           vrfy(0,"u32_Get: non-numeric node"); break;
        }
    }
    return ret;
}

//
// predefined conversion functions
//

lib_json::FNode *u64_FmtJson(u64 value, lib_json::FNode *parent) {
    lib_json::FNode *node = &lib_json::node_Alloc();
    node->p_parent = parent?parent:node;
    node->type = lib_json_FNode_type_number;
    u64_Print(value,node->value);
    node_XrefMaybe(*node);
    return node;
}

lib_json::FNode *u32_FmtJson(u32 value, lib_json::FNode *parent) {
    return u64_FmtJson(value,parent);
}

lib_json::FNode *u16_FmtJson(u16 value, lib_json::FNode *parent) {
    return u64_FmtJson(value,parent);
}

lib_json::FNode *u8_FmtJson(u8 value, lib_json::FNode *parent) {
    return u64_FmtJson(value,parent);
}


lib_json::FNode *i64_FmtJson(i64 value, lib_json::FNode *parent) {
    lib_json::FNode *node = &lib_json::node_Alloc();
    node->p_parent = parent?parent:node;
    node->type = lib_json_FNode_type_number;
    i64_Print(value,node->value);
    node_XrefMaybe(*node);
    return node;
}

lib_json::FNode *i32_FmtJson(i32 value, lib_json::FNode *parent) {
    return i64_FmtJson(value,parent);
}

lib_json::FNode *i16_FmtJson(i16 value, lib_json::FNode *parent) {
    return i64_FmtJson(value,parent);
}

lib_json::FNode *i8_FmtJson(i8 value, lib_json::FNode *parent) {
    return i64_FmtJson(value,parent);
}


lib_json::FNode *double_FmtJson(double value, lib_json::FNode *parent) {
    lib_json::FNode *node = &lib_json::node_Alloc();
    node->p_parent = parent?parent:node;
    node->type = lib_json_FNode_type_number;
    double_Print(value,node->value);
    node_XrefMaybe(*node);
    return node;
}

lib_json::FNode *float_FmtJson(float value, lib_json::FNode *parent) {
    return double_FmtJson(value,parent);
}

lib_json::FNode *bool_FmtJson(bool value, lib_json::FNode *parent) {
    lib_json::FNode *node = &lib_json::node_Alloc();
    node->p_parent = parent?parent:node;
    node->type = value?lib_json_FNode_type_true:lib_json_FNode_type_false;
    node_XrefMaybe(*node);
    return node;
}

lib_json::FNode *char_FmtJson(char value, lib_json::FNode *parent) {
    lib_json::FNode *node = &lib_json::node_Alloc();
    node->p_parent = parent?parent:node;
    node->type = lib_json_FNode_type_string;
    char_Print(value,node->value);
    node_XrefMaybe(*node);
    return node;
}

lib_json::FNode *algo::strptr_FmtJson(const algo::strptr value, lib_json::FNode *parent) {
    lib_json::FNode *node = &lib_json::node_Alloc();
    node->p_parent = parent?parent:node;
    node->type = lib_json_FNode_type_string;
    strptr_Print(value,node->value);
    node_XrefMaybe(*node);
    return node;
}

lib_json::FNode *algo::cstring_FmtJson(algo::cstring &value, lib_json::FNode *parent) {
    return strptr_FmtJson(value,parent);
}

// -----------------------------------------------------------------------------

lib_json::FldKey lib_json::fldkey_Get(lib_json::FNode &node) {
    lib_json::FldKey ret;
    if (node.p_parent && node.p_parent->type==lib_json_FNode_type_field) {
        ret.object = node.p_parent->p_parent;
        ret.field = node.p_parent->value;
    }
    return ret;
}
