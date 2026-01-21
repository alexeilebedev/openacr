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
// Target: jkv (exe) -- JSON <-> key-value mapping tool
// Exceptions: yes
// Source: cpp/jkv/jkv.cpp
//

#include "include/algo.h"
#include "include/algo.h"
#include "include/jkv.h"
#include "include/lib_json.h"

void jkv::PrintKv(lib_json::FNode &node, algo::strptr prefix, cstring &out) {
    verblog("["<<prefix<<"]: "<<lib_json::type_ToCstr(node)<<" "<<node.value);
    switch(node.type) {
    case lib_json_FNode_type_field: {
        if (node.value == "") {
            // nameless field = bad
        } else {
            ind_beg(lib_json::node_c_child_curs,child,node) {
                PrintKv(child, prefix, out);
            }ind_end;
        }
    }break;
    case lib_json_FNode_type_array: {
        if (c_child_N(node) > 0) {
            ind_beg(lib_json::node_c_child_curs,child,node) {
                tempstr new_prefix;
                new_prefix << prefix << "-" << ind_curs(child).index;
                PrintKv(child, new_prefix, out);
            }ind_end;
        } else {
            out<<prefix<<":[]"<<eol;
        }
    }break;
    case lib_json_FNode_type_object: {
        if (c_child_N(node) > 0) {
            ind_beg(lib_json::node_c_child_curs,child,node) {
                tempstr new_prefix(prefix);
                if (new_prefix != "") {
                    new_prefix << ".";
                }
                new_prefix << child.value;
                PrintKv(child, new_prefix, out);
            }ind_end;
        } else {
            out<<prefix<<":{}"<<eol;
        }
    }break;
    case lib_json_FNode_type_string: {
        out<<prefix<<"::";
        tempstr str;
        lib_json::JsonSerializeString(node.value, str);
        out<<ch_GetRegion(str,1,ch_N(str)-2);
        out<<eol;
    }break;
    case lib_json_FNode_type_true:
    case lib_json_FNode_type_false:
    case lib_json_FNode_type_number:
    case lib_json_FNode_type_null: {
        out<<prefix<<":";
        lib_json::JsonSerialize(&node, out);
        out<<eol;
    }break;
    }// switch
}

// -----------------------------------------------------------------------------

void jkv::SetPath(lib_json::FNode *node, algo::strptr key, lib_json::FNode *val) {
    while (key != "") {
        node->type = lib_json_FNode_type_object;
        algo::i32_Range dot=ch_FindFirst(key,'.');
        algo::strptr fieldname=ch_FirstN(key,dot.beg);
        key=ch_RestFrom(key,dot.end);
        algo::i32_Range dash = ch_FindFirst(fieldname,'-');
        algo::strptr num;
        if (dash.end > dash.beg) {
            num=ch_RestFrom(fieldname,dash.end);
            fieldname=ch_FirstN(fieldname,dash.beg);
        }
        if (fieldname != "") {// disallow empty field names
            // find or create field with name 'fieldname'
            lib_json::FNode* fieldnode = lib_json::ind_objfld_Find(lib_json::FldKey(node,fieldname));
            node = fieldnode ? fieldnode : &NewFieldVal(node, lib_json_FNode_type_null, fieldname);
        }
        // handle array index
        if (num != "") {
            node->type = lib_json_FNode_type_array;
            u32 index=0;
            u32_ReadStrptrMaybe(index,num);// read array index
            while (u32(c_child_N(*node)) <= index) {
                lib_json::NewNode(node,lib_json_FNode_type_null);
            }
            node = c_child_Find(*node,index);
        }
    }
    c_child_Cascdel(*node);// delete any children
    // store value
    node->type = val->type;
    node->value = val->value;
    node_Delete(*val);
}

// -----------------------------------------------------------------------------

void jkv::ApplyKv(lib_json::FNode &node, algo::strptr kv) {
    tempstr key(Pathcomp(kv,":LL"));
    strptr val=Pathcomp(kv,":LR");
    if (StartsWithQ(key,".")) {// .blah:val -> start from last key
        key = tempstr() << Pathcomp(_db.last_kv,".RL") << key;
    } else if (StartsWithQ(key,"-")) {// -blah:val -> use last prefix key
        key = tempstr() << Pathcomp(_db.last_kv,"-RL") << key;
    } else if (key == "") {// empty key = last key + 1
        int num=0;
        i32_ReadStrptrMaybe(num,Pathcomp(_db.last_kv,"-RR"));
        strptr prefix = Pathcomp(_db.last_kv,"-RL");
        if (prefix != "") {
            num = num+1;
        } else {
            prefix = _db.last_kv;
        }
        key = tempstr() << prefix << "-" << num;
    }
    lib_json::FParser parser;
    if (StartsWithQ(val,":")) {
        JsonParse(parser,tempstr() << '"' << ch_RestFrom(val,1) << '"');
    } else {
        JsonParse(parser,val);
    }
    JsonParse(parser,"");
    bool good = parser.root_node != NULL;
    verblog("apply "
            <<Keyval("key",key)
            <<Keyval("val",val)
            <<Keyval("comment",(good ? "" : "bad val")));
    if (good) {
        lib_json::FNode *valnode = parser.root_node;
        parser.root_node=NULL;// detach
        valnode->p_parent = NULL;
        vrfy_(c_child_N(*valnode)==0);
        SetPath(&node,key,valnode);
    }
    _db.last_kv=key;
}

// -----------------------------------------------------------------------------

void jkv::Main() {
    // in-memory object
    lib_json::FParser parser;
    // read input
    cstring input;
    if (_db.cmdline.file == "-") {
        ind_beg(algo::FileLine_curs,line,algo::Fildes(0)) {
            input << line << eol;
        }ind_end;
    } else {
        input = FileToString(_db.cmdline.file);
    }
    // input -> object
    if (_db.cmdline.r) {
        parser.root_node = &lib_json::NewNode(NULL,lib_json_FNode_type_object);// start with empty object
        ind_beg(algo::Line_curs,line,input) {
            ApplyKv(*parser.root_node,line);
        }ind_end;
    } else {
        lib_json::JsonParse(parser,input);
        lib_json::JsonParse(parser,"");// finalize
        if (!parser.root_node) {
            parser.root_node = &lib_json::NewNode(NULL,lib_json_FNode_type_object);// recover from failure
        }
    }
    // apply kvs
    ind_beg(command::jkv_kv_curs,kv,_db.cmdline) {
        ApplyKv(*parser.root_node,kv);
    }ind_end;
    if (_db.cmdline.output == command_jkv_output_auto) {
        _db.cmdline.output = _db.cmdline.write
            ? (_db.cmdline.r ? command_jkv_output_kv : command_jkv_output_json)
            : (_db.cmdline.r ? command_jkv_output_json : command_jkv_output_kv);
    }
    cstring out;
    if (_db.cmdline.output == command_jkv_output_json) {
        lib_json::JsonSerialize(parser.root_node, out, _db.cmdline.pretty);
        out << eol;
    } else if (_db.cmdline.output == command_jkv_output_kv) {
        PrintKv(*parser.root_node, "", out);
    }
    if (_db.cmdline.write && _db.cmdline.file != "-") {
        SafeStringToFile(out,_db.cmdline.file);
    } else {
        prlog_(out);
    }
}
