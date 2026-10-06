// Copyright (C) 2024,2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2014-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: lib_json (lib) -- Full json support library
// Exceptions: NO
// Header: include/lib_json.h
//
// JSON library
// predefined conversions from basic types to json nodes

#pragma once

#include "include/gen/lib_json_gen.h"
lib_json::FNode *u64_FmtJson(u64 value, lib_json::FNode *parent);
lib_json::FNode *u32_FmtJson(u32 value, lib_json::FNode *parent);
lib_json::FNode *u16_FmtJson(u16 value, lib_json::FNode *parent);
lib_json::FNode *u8_FmtJson(u8 value, lib_json::FNode *parent);
lib_json::FNode *i64_FmtJson(i64 value, lib_json::FNode *parent);
lib_json::FNode *i32_FmtJson(i32 value, lib_json::FNode *parent);
lib_json::FNode *i16_FmtJson(i16 value, lib_json::FNode *parent);
lib_json::FNode *i8_FmtJson(i8 value, lib_json::FNode *parent);
lib_json::FNode *double_FmtJson(double value, lib_json::FNode *parent);
lib_json::FNode *float_FmtJson(float value, lib_json::FNode *parent);
lib_json::FNode *bool_FmtJson(bool value, lib_json::FNode *parent);
lib_json::FNode *char_FmtJson(char value, lib_json::FNode *parent);

namespace lib_json { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_json.cpp
    //

    // Create new node
    // enforce hierarchy rules
    lib_json::FNode &NewNode(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr field);

    // PARENT    parent node or NULL
    // TYPE      node type
    // VALUE     node value where applicable (only for field, string, number)
    lib_json::FNode &NewNode(lib_json::FNode *parent, lib_json_FNode_type_Enum type);

    // PARENT    parent node or NULL
    // TYPE      node type
    // VALUE     node value where applicable (only for field, string, number)
    // Construct 2 nodes, a "field" node which has the name FIELD, and its value
    // of type TYPE. Return the VALUE
    lib_json::FNode &NewFieldVal(lib_json::FNode *parent, lib_json_FNode_type_Enum type, strptr field);
    lib_json::FNode &NewFieldNode(lib_json::FNode *parent, strptr field);
    lib_json::FNode &NewObjectNode(lib_json::FNode *parent, strptr field = strptr());
    lib_json::FNode &NewArrayNode(lib_json::FNode *parent, strptr field = strptr());
    lib_json::FNode &NewStringNode(lib_json::FNode *parent, strptr field = strptr(), strptr value = strptr());
    lib_json::FNode &NewNumberNode(lib_json::FNode *parent, strptr field = strptr(), strptr value = strptr("0"));
    lib_json::FNode &NewBoolNode(lib_json::FNode *parent, strptr field = strptr(), bool value = true);

    // Parses JSON text into tree structure according to ECMA-404
    // can be invoked repeadetly with new data portions (can be used directly as read hook)
    //
    // PARSER    parser handle
    // BUF       input buffer to process
    // non-zero-length - parse new data portion
    // zero-length     - to finalize and check completeness
    // RETURN    true for OK, false for parse error
    //
    // after finalization, root node pointer is available at: parser.root_node
    // whole tree is automatically deleted upon parser destruction,
    // to detach tree from parser do parser.root_node=NULL
    bool JsonParse(lib_json::FParser &parser, strptr buf);

    // AMC cleanup function - automatically delete parsed JSON tree
    //     (user-implemented function, prototype is in amc-generated header)
    // void root_node_Cleanup(lib_json::FParser& parent); // dmmeta.ffunc:lib_json.FParser.root_node.Cleanup

    // Write STR to OUT as a JSON string, quotes included.
    // "The representation of strings is similar to conventions used in the C
    // family of programming languages.  A string begins and ends with
    // quotation marks.  All Unicode characters may be placed within the
    // quotation marks except for the characters that must be escaped:
    // quotation mark, reverse solidus, and the control characters (U+0000
    // through U+001F)."
    // -- this says that solidus need not be escaped when printing -- only when parsing!
    // JSON text is Unicode, so a byte of STR that begins no valid UTF-8 sequence
    // cannot be carried at all.  Copying it through would make OUT a string no
    // JSON parser accepts, and one binary value would spoil a whole response.
    // Each such byte is written as U+FFFD, the replacement character, and a valid
    // UTF-8 sequence is copied as it is.
    void JsonSerializeString(algo::strptr str, algo::cstring &out);

    // Serialize to string
    // Serialize to JSON tree to text
    // NODE      root node to start from
    // OUT       target string
    // PRETTY    pretty printer setting:
    // 0 = no pretty printer (compact output)
    // 1 = algo style pretty printer
    // 2 = standard (jq) style) pretty printer
    // INDENT    level of indenting (for pretty-formatting)
    void JsonSerialize(lib_json::FNode* node, cstring &out, u32 pretty = 0, u32 indent = 0);

    // Find node in object chain
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    // e.g. path abc.def.ghi and parent pointing to {"abc:{"def":{"ghi":value}}}
    // yields value node
    lib_json::FNode* node_Find(lib_json::FNode* parent, strptr path);

    // Get node value as string
    // Empty string is returned in case of any error or null value
    // true/false is converted to 0/1
    //
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    strptr strptr_Get(lib_json::FNode* parent, strptr path);

    // Get array node, NULL in case of any error
    //
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    lib_json::FNode* node_GetArray(lib_json::FNode* parent, strptr path);

    // The values of the array node at PATH, joined with commas; empty when
    // there is no array there.  A list of labels or tags travels this way:
    // gitlab answers an array, and every filter and every record field holds
    // the comma list.
    //
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    tempstr csv_Get(lib_json::FNode* parent, strptr path);

    // Get node value as u32
    // If the path is not found, or the value is malformatted, DFLT is returned.
    // true/false is converted to 0/1
    // A string node holding digits reads as the number it spells.
    //
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    u32 u32_Get(lib_json::FNode* parent, strptr path, int dflt = 0);

    // Get node value as i32
    // If the path is not found, or the value is malformatted, DFLT is returned.
    // true/false is converted to 0/1
    // A string node holding digits reads as the number it spells.
    //
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    i32 i32_Get(lib_json::FNode* parent, strptr path, int dflt = 0);

    // Get node value as bool
    // number is converted: zero - false, nonzero true
    // empty string - false, non-empty string true
    // On any error, DFLT is returned.
    //
    // PARENT    node to start from
    // PATH      dot-separated list of field keys
    u32 bool_Get(lib_json::FNode* parent, strptr path, int dflt = false);
    //     (user-implemented function, prototype is in amc-generated header)
    // lib_json::FldKey fldkey_Get(lib_json::FNode &node);

    // -------------------------------------------------------------------
    // include/lib_json.inl.h
    //

    // Check for parse error
    //
    // PARSER    parser handle
    // RETURN    true for OK, false for parse error
    inline bool JsonParseOkQ(lib_json::FParser &parser);
}

#include "include/lib_json.inl.h"
#include "include/gen/lib_json_gen.inl.h"
