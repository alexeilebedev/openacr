// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf/unit/lib_json.cpp
//
// Created By: alexei.lebedev alexey.polovinkin
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev alexey.polovinkin
//

#include "include/atf_unit.h"
#include "include/lib/lib_json.h"

// auxiliary function to invoke parser
// one by one char or whole buffer

static void InvokeParser(lib_json::FParser &parser, strptr buf, bool by_one_char) {
    u32 incr = by_one_char ? 1 : elems_N(buf);
    for (i32 i=0; i<elems_N(buf); i += incr) {
        JsonParse(parser,FirstN(RestFrom(buf,i),incr));
    }
    // always finalize
    JsonParse(parser,"");
}

void atf_unit::unittest_lib_json_Empty() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n" : "",by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node == NULL;
        TESTCMP(ok,true);
    }
}

void atf_unit::unittest_lib_json_TokenNull() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n null\n \t \r " : "null", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_null);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_TokenFalse() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n false\n \t \r " : "false", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_false);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_TokenTrue() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n true\n \t \r " : "true", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_true);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_SimpleNumber() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n 1\n \t \r " : "1", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"1");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_SimpleString() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n \"abc\"\n \t \r " : "\"abc\"", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_string);
            TESTCMP(parser.root_node->value,"abc");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_EmptyArray() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n [ \n \t \r ]\n \t \r " : "[]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_Emptyobject() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \t \r \n { \n \t \r }\n \t \r " : "{}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_SecString() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? "  \n \t \rthrow 'allowIllegalResourceCall is false.';   \n \t \r " : "throw 'allowIllegalResourceCall is false.';", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node == NULL;
        TESTCMP(ok,true);
    }
}

void atf_unit::unittest_lib_json_NumberZero() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? "  0  " :  "0" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"0");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_NumberManyDigits() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? "  1234567890  " :  "1234567890" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"1234567890");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_NumberDecimal() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? "  12345.67890  " :  "12345.67890" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"12345.67890");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_NumberExponent1() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? "  12345678E90  " :  "12345678E90" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"12345678E90");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_NumberExponent2() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? "  12345678e90  " :  "12345678e90" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"12345678e90");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_NumberCombined1() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " -12345.678E+90 " :  "-12345.678E+90" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"-12345.678E+90");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_NumberCombined2() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " 12345.678e-90 " :  "12345.678e-90" , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_number);
            TESTCMP(parser.root_node->value,"12345.678e-90");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_StringEmpty() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \"\" " : "\"\"", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_string);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_StringWithEscapes() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " \"\\\\\\\"\\/\\b\\f\\n\\r\\t\\u0001\\u001f\\uD852\\udF62'\" " : "\"\\\\\\\"\\/\\b\\f\\n\\r\\t\\u0001\\u001f\\uD852\\udF62'\"", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_string);
            TESTCMP(parser.root_node->value,"\\\"/\b\f\n\r\t\x01\x1f\xf0\xa4\xad\xa2'"); // last letter is U+24B62 utf-8 four bytes
            TESTCMP(c_child_N(*parser.root_node),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldTokenNull() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : null } " : "{\"field\":null}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_null);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldTokenFalse() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : false }" : "{\"field\":false}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_false);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldTokenTrue() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : true }" : "{\"field\":true}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_true);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldSimpleNumber() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : 1 } " : "{\"field\":1}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_number);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"1");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldSimpleString() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : \"abc\" } " : "{\"field\":\"abc\"}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_string);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"abc");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldEmptyArray() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : [ ] } " : "{\"field\":[]}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_array);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldEmptyObject() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " { \"field\" : { } } " : "{\"field\":{}}", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 1 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_field);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"field");
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->type,lib_json_FNode_type_object);
            TESTCMP(c_child_Find(*c_child_Find(*parser.root_node,0),0)->value,"");
            TESTCMP(c_child_N(*c_child_Find(*c_child_Find(*parser.root_node,0),0)),0);
        }
    }
}

void atf_unit::unittest_lib_json_ObjFieldAll() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser
                     , with_blanks ? " { \"field1\" : null , \"field2\" : false , \"field3\" : true , \"field4\" : 1 , \"field5\" : \"abc\" , \"field6\" : [ ] , \"field7\" : { } } "
                     : "{\"field1\":null,\"field2\":false,\"field3\":true,\"field4\":1,\"field5\":\"abc\",\"field6\":[],\"field7\":{}}"
                     , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 7;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_object);
            TESTCMP(parser.root_node->value,"");
            frep_(i,7) {
                lib_json::FNode *field = c_child_Find(*parser.root_node,i);
                bool ok2 = c_child_N(*field) == 1;
                TESTCMP(ok2,true);
                if (ok2) {
                    TESTCMP(field->type,lib_json_FNode_type_field);
                    TESTCMP(field->value,tempstr()<<"field"<<(i+1));
                    u32 type;
                    const char *value;
                    switch (i) {
                    case 1:  type = lib_json_FNode_type_false  ; value = ""    ; break;
                    case 2:  type = lib_json_FNode_type_true   ; value = ""    ; break;
                    case 3:  type = lib_json_FNode_type_number ; value = "1"   ; break;
                    case 4:  type = lib_json_FNode_type_string ; value = "abc" ; break;
                    case 5:  type = lib_json_FNode_type_array  ; value = ""    ; break;
                    case 6:  type = lib_json_FNode_type_object ; value = ""    ; break;
                    default: type = lib_json_FNode_type_null   ; value = ""    ; // case 0
                    }
                    TESTCMP(c_child_Find(*field,0)->type,type);
                    TESTCMP(c_child_Find(*field,0)->value,value);
                    TESTCMP(c_child_N(*c_child_Find(*field,0)),0);
                }
            }
        }
    }
}

void atf_unit::unittest_lib_json_ArrayTokenNull() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ null ] " : "[null]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_null);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"");
        }
    }
}

void atf_unit::unittest_lib_json_ArrayTokenFalse() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ false ] " : "[false]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_false);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"");
        }
    }
}

void atf_unit::unittest_lib_json_ArrayTokenTrue() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ true ] " : "[true]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_true);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"");
        }
    }
}

void atf_unit::unittest_lib_json_ArraySimpleNumber() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ 1 ] " : "[1]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_number);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"1");
        }
    }
}

void atf_unit::unittest_lib_json_ArraySimpleString() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ \"abc\" ] " : "[\"abc\"]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_string);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"abc");
        }
    }
}

void atf_unit::unittest_lib_json_ArrayEmptyArray() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ [ ] ] " : "[[]]", by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_array);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"");
        }
    }
}

void atf_unit::unittest_lib_json_ArrayEmptyObject() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser, with_blanks ? " [ { } ] " : "[{}]", by_one_char);

        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 1 && c_child_N(*c_child_Find(*parser.root_node,0)) == 0 ;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            TESTCMP(c_child_Find(*parser.root_node,0)->type,lib_json_FNode_type_object);
            TESTCMP(c_child_Find(*parser.root_node,0)->value,"");
        }
    }
}

void atf_unit::unittest_lib_json_ArrayAll() {
    frep_(by_one_char,2) frep_(with_blanks,2) {
        lib_json::FParser parser;
        InvokeParser(parser
                     , with_blanks ? " [ null , false , true , 1 , \"abc\" , [ ] , { } ] "
                     : "[null,false,true,1,\"abc\",[],{}]"
                     , by_one_char);
        TESTCMP(JsonParseOkQ(parser),true);
        bool ok = parser.root_node && c_child_N(*parser.root_node) == 7;
        TESTCMP(ok,true);
        if (ok) {
            TESTCMP(parser.root_node->type,lib_json_FNode_type_array);
            TESTCMP(parser.root_node->value,"");
            frep_(i,7) {
                u32 type;
                const char *value;
                switch (i) {
                case 1:  type = lib_json_FNode_type_false  ; value = ""    ; break;
                case 2:  type = lib_json_FNode_type_true   ; value = ""    ; break;
                case 3:  type = lib_json_FNode_type_number ; value = "1"   ; break;
                case 4:  type = lib_json_FNode_type_string ; value = "abc" ; break;
                case 5:  type = lib_json_FNode_type_array  ; value = ""    ; break;
                case 6:  type = lib_json_FNode_type_object ; value = ""    ; break;
                default: type = lib_json_FNode_type_null   ; value = ""    ; // case 0
                }
                TESTCMP(c_child_Find(*parser.root_node,i)->type,type);
                TESTCMP(c_child_Find(*parser.root_node,i)->value,value);
                TESTCMP(c_child_N(*c_child_Find(*parser.root_node,i)),0);
            }
        }
    }
}

void atf_unit::unittest_lib_json_Typical() {
    lib_json::FParser parser;
    static const char *secstr = "throw 'allowIllegalResourceCall is false.';";
    static const char *in = "{"
        "\"id\":1000,"
        "\"name\":\"Object\","
        "\"methods\":[\"GET\",\"PUT\"],"
        "\"collections\":[],"
        "\"fields\":{"
        "\"field1\":null,"
        "\"field2\":false,"
        "\"field3\":true"
        "}"

        "}";
    tempstr parser_in;
    parser_in << secstr << in;
    JsonParse(parser,parser_in);
    JsonParse(parser,"");
    TESTCMP(JsonParseOkQ(parser),true);
    // basically check what we get
    TESTCMP(strptr_Get(parser.root_node,"id"),"1000");
    TESTCMP(strptr_Get(parser.root_node,"name"),"Object");
    TESTCMP(strptr_Get(parser.root_node,"fields.field1"),"");
    TESTCMP(strptr_Get(parser.root_node,"fields.field2"),"0");
    TESTCMP(strptr_Get(parser.root_node,"fields.field3"),"1");
    lib_json::FNode *methods = node_GetArray(parser.root_node,"methods");
    bool ok = methods && c_child_N(*methods) == 2;
    TESTCMP(ok,true);
    if (ok) {
        TESTCMP(c_child_Find(*methods,0)->value,"GET");
        TESTCMP(c_child_Find(*methods,1)->value,"PUT");
    }
    lib_json::FNode *collections = node_GetArray(parser.root_node,"collections");
    ok = collections && c_child_N(*collections) == 0;
    // and now loop over serialization
    tempstr text;
    JsonSerialize(parser.root_node,text);
    TESTCMP(text,in);// should be the same as input
}

void atf_unit::unittest_lib_json_CtrlCharEscape() {// Control char escape
    lib_json::FParser parser;
    static const char value[] = "\0\001\002\003\004\x1f";
    lib_json::FNode *s = &lib_json::NewStringNode(NULL,"",strptr(value,_array_count(value)-1));
    tempstr text;
    JsonSerialize(s,text);
    TESTCMP(text,"\"\\u0000\\u0001\\u0002\\u0003\\u0004\\u001F\"");
    JsonParse(parser,text);
    JsonParse(parser,"");
    TESTCMP(JsonParseOkQ(parser),true);
    bool ok = parser.root_node && parser.root_node->type == lib_json_FNode_type_string;
    if (ok) {
        TESTCMP(parser.root_node->value,strptr(value,_array_count(value)-1));
    }
}

void atf_unit::unittest_lib_json_ErrorBadToken1() {
    lib_json::FParser parser;
    JsonParse(parser,"x");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadToken2() {
    lib_json::FParser parser;
    JsonParse(parser,"nullx");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadNumber() {
    lib_json::FParser parser;
    JsonParse(parser,"123x");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadString1() {
    lib_json::FParser parser;
    JsonParse(parser,"\"ab");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadString2() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadString3() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\z\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString1() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\u\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString2() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\u1\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString3() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\u100x\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString4() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\ud800x\\udc00\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString5() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\ud800\\ufc00\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString6() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\ud800\\ufc00\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBadUString7() {
    lib_json::FParser parser;
    JsonParse(parser,"\"\\ud800\\x\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch1() {
    lib_json::FParser parser;
    JsonParse(parser,"{");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch2() {
    lib_json::FParser parser;
    JsonParse(parser,"}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch3() {
    lib_json::FParser parser;
    JsonParse(parser,"[");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch4() {
    lib_json::FParser parser;
    JsonParse(parser,"]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch5() {
    lib_json::FParser parser;
    JsonParse(parser,"{]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch6() {
    lib_json::FParser parser;
    JsonParse(parser,"{]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch7() {
    lib_json::FParser parser;
    JsonParse(parser,"{{{}}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch8() {
    lib_json::FParser parser;
    JsonParse(parser,"{{}}}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch9() {
    lib_json::FParser parser;
    JsonParse(parser,"[[[]]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch10() {
    lib_json::FParser parser;
    JsonParse(parser,"[[]]]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch11() {
    lib_json::FParser parser;
    JsonParse(parser,"{[{]}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch12() {
    lib_json::FParser parser;
    JsonParse(parser,"{[}]}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch13() {
    lib_json::FParser parser;
    JsonParse(parser,"[{[}]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBrMismatch14() {
    lib_json::FParser parser;
    JsonParse(parser,"[{]}]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorArrayComma1() {
    lib_json::FParser parser;
    JsonParse(parser,"[null null]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorArrayComma2() {
    lib_json::FParser parser;
    JsonParse(parser,"[null,,null]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorArrayComma3() {
    lib_json::FParser parser;
    JsonParse(parser,"[null,null,]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorArrayComma4() {
    lib_json::FParser parser;
    JsonParse(parser,"[,null,null]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectComma1() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\":null\"b\":null]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectComma2() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\":null,,\"b\":null]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectComma3() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\":null,\"b\":null,]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectComma4() {
    lib_json::FParser parser;
    JsonParse(parser,"{,\"a\":null,\"b\":null]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBareComma() {
    lib_json::FParser parser;
    JsonParse(parser,",");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBareValuesWithComma() {
    lib_json::FParser parser;
    JsonParse(parser,"\"a\",\"b\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectNoValue() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\"}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectColon1() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\"\"b\"}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectColon2() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\"::\"b\"}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectColon3() {
    lib_json::FParser parser;
    JsonParse(parser,"{:\"a\":\"\b\"}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectColon4() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\":\"\b\":}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectColon5() {
    lib_json::FParser parser;
    JsonParse(parser,"{:}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBareColon() {
    lib_json::FParser parser;
    JsonParse(parser,":");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorArrayColon() {
    lib_json::FParser parser;
    JsonParse(parser,"[\"a\":\"b\"]");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorBareValuesWithColon() {
    lib_json::FParser parser;
    JsonParse(parser,"\"a\":\"b\"");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_ErrorObjectDupField() {
    lib_json::FParser parser;
    JsonParse(parser,"{\"a\":\"b\",\"a\":\"b\"}");
    TESTCMP(JsonParse(parser,""),false); // finalize and check
}

void atf_unit::unittest_lib_json_FmtJson_u64_0() {
    lib_json::FNode *node = u64_FmtJson(u64(0),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"0");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u64_max() {
    lib_json::FNode *node = u64_FmtJson(u64(18446744073709551615ULL),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"18446744073709551615");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u32_0() {
    lib_json::FNode *node = u32_FmtJson(u32(0),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"0");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u32_max() {
    lib_json::FNode *node = u32_FmtJson(u32(4294967295),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"4294967295");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u16_0() {
    lib_json::FNode *node = u16_FmtJson(u16(0),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"0");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u16_max() {
    lib_json::FNode *node = u16_FmtJson(u16(65535),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"65535");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u8_0() {
    lib_json::FNode *node = u8_FmtJson(u8(0),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"0");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_u8_max() {
    lib_json::FNode *node = u8_FmtJson(u8(255),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"255");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i64_min() {
    lib_json::FNode *node = i64_FmtJson(i64(0x8000000000000000ULL),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"-9223372036854775808");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i64_max() {
    lib_json::FNode *node = i64_FmtJson(i64(9223372036854775807),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"9223372036854775807");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i32_min() {
    lib_json::FNode *node = i32_FmtJson(i32(-2147483648),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"-2147483648");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i32_max() {
    lib_json::FNode *node = i32_FmtJson(i32(2147483647),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"2147483647");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i16_min() {
    lib_json::FNode *node = i16_FmtJson(i16(-32768),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"-32768");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i16_max() {
    lib_json::FNode *node = i16_FmtJson(i16(32767),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"32767");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i8_min() {
    lib_json::FNode *node = i8_FmtJson(i8(-128),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"-128");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_i8_max() {
    lib_json::FNode *node = i8_FmtJson(i8(127),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"127");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_double_prec() {
    lib_json::FNode *node = double_FmtJson(double(1.00001e-10),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"1.00001e-10");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_float_prec() {
    lib_json::FNode *node = float_FmtJson(float(1.00001e-10),NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"1.00001e-10");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_bool_true() {
    lib_json::FNode *node = bool_FmtJson(true,NULL);
    TESTCMP(node->type,lib_json_FNode_type_true);
    TESTCMP(node->value,"");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_bool_false() {
    lib_json::FNode *node = bool_FmtJson(false,NULL);
    TESTCMP(node->type,lib_json_FNode_type_false);
    TESTCMP(node->value,"");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_char() {
    {
        lib_json::FNode *node = strptr_FmtJson(strptr("abc"),NULL);
        TESTCMP(node->type,lib_json_FNode_type_string);
        TESTCMP(node->value,"abc");
        node_Delete(*node);
    }
    {
        lib_json::FNode *node = char_FmtJson(char('X'),NULL);
        TESTCMP(node->type,lib_json_FNode_type_string);
        TESTCMP(node->value,"X");
        node_Delete(*node);
    }
    {
        cstring temp("abc");
        lib_json::FNode *node = cstring_FmtJson(temp,NULL);
        TESTCMP(node->type,lib_json_FNode_type_string);
        TESTCMP(node->value,"abc");
        node_Delete(*node);
    }
}

void atf_unit::unittest_lib_json_FmtJson_TypeA() {
    atf_unit::TypeA x;
    x.typea = 100;
    lib_json::FNode *node = TypeA_FmtJson(x,NULL);
    TESTCMP(node->type,lib_json_FNode_type_number);
    TESTCMP(node->value,"100");
    node_Delete(*node);
}

void atf_unit::unittest_lib_json_FmtJson_Object() {
    atf_unit::TestJson test_json;
    test_json.fld_bool           = false;
    test_json.fld_u8             = 1;
    test_json.fld_i8             = -1;
    test_json.fld_u16            = 2;
    test_json.fld_i16            = -2;
    test_json.fld_u32            = 3;
    test_json.fld_i32            = -3;
    test_json.fld_u64            = 4 ;
    test_json.fld_i64            = -4;
    test_json.fld_float          = 1.00001e-10;
    test_json.fld_double         = 1.00002e-10;
    test_json.fld_char           = 'x';
    test_json.fld_strptr         = "abc";
    test_json.fld_cstring        = "def";
    test_json.fld_atftypea.typea = 5;
    test_json.fld_atfcstr.val    = "ghi";
    test_json.fld_atfdbl.val     = 1.00003e-10;
    test_json.fld_atftypeb.typea = 6;
    test_json.fld_atftypeb.j     = 7;
    lib_json::FNode *node = TestJson_FmtJson(test_json,NULL);
    TESTCMP(node->type, lib_json_FNode_type_object);
    TESTCMP(c_child_N(*node),18);

    verblog("obj: "<<test_json);
    tempstr str;
    JsonSerialize(node,str);
    verblog("json equivalent: "<<str);

    frep_(i,c_child_N(*node)) {
        lib_json::FNode *field = c_child_Find(*node,i);
        TESTCMP(field->type, lib_json_FNode_type_field);
        const char *name="";
        u32 type=0;
        const char *value="";
        switch (i) {
        case 0:  name = "fld_bool"     ; type = lib_json_FNode_type_false  ; value = ""            ; break;
        case 1:  name = "fld_u8"       ; type = lib_json_FNode_type_number ; value = "1"           ; break;
        case 2:  name = "fld_i8"       ; type = lib_json_FNode_type_number ; value = "-1"          ; break;
        case 3:  name = "fld_u16"      ; type = lib_json_FNode_type_number ; value = "2"           ; break;
        case 4:  name = "fld_i16"      ; type = lib_json_FNode_type_number ; value = "-2"          ; break;
        case 5:  name = "fld_u32"      ; type = lib_json_FNode_type_number ; value = "3"           ; break;
        case 6:  name = "fld_i32"      ; type = lib_json_FNode_type_number ; value = "-3"          ; break;
        case 7:  name = "fld_u64"      ; type = lib_json_FNode_type_number ; value = "4"           ; break;
        case 8:  name = "fld_i64"      ; type = lib_json_FNode_type_number ; value = "-4"          ; break;
        case 9:  name = "fld_float"    ; type = lib_json_FNode_type_number ; value = "1.00001e-10" ; break;
        case 10: name = "fld_double"   ; type = lib_json_FNode_type_number ; value = "1.00002e-10" ; break;
        case 11: name = "fld_char"     ; type = lib_json_FNode_type_string ; value = "x"           ; break;
        case 12: name = "fld_strptr"   ; type = lib_json_FNode_type_string ; value = "abc"         ; break;
        case 13: name = "fld_cstring"  ; type = lib_json_FNode_type_string ; value = "def"         ; break;
        case 14: name = "fld_atftypea" ; type = lib_json_FNode_type_number ; value = "5"           ; break;
        case 15: name = "fld_atfcstr"  ; type = lib_json_FNode_type_string ; value = "ghi"         ; break;
        case 16: name = "fld_atfdbl"   ; type = lib_json_FNode_type_number ; value = "1.00003e-10" ; break;
        case 17: name = "fld_atftypeb" ; type = lib_json_FNode_type_object ; value = ""            ; break;
        default: vrfy_(0);
        }

        lib_json::FNode &child = *c_child_Find(*field,0);
        TESTCMP(field->value,name);
        TESTCMP(child.type,type);
        TESTCMP(child.value,value);
        TESTCMP(c_child_N(child),(i!=17?0:2));
    }
    lib_json::FNode *typeb = c_child_Find(*c_child_Find(*node,17),0);
    TESTCMP(c_child_N(*typeb),2);
    frep_(i,c_child_N(*typeb)) {
        lib_json::FNode *field = c_child_Find(*typeb,i);
        TESTCMP(field->type, lib_json_FNode_type_field);
        const char *name="";
        u32 type=0;
        const char *value="";
        switch (i) {
        case 0:  name = "typea"   ; type = lib_json_FNode_type_number ; value = "6"            ; break;
        case 1:  name = "j"       ; type = lib_json_FNode_type_number ; value = "7"            ; break;
        default: vrfy_(0);
        }
        TESTCMP(field->value,name);
        TESTCMP(c_child_Find(*field,0)->type,type);
        TESTCMP(c_child_Find(*field,0)->value,value);
        TESTCMP(c_child_N(*c_child_Find(*field,0)),0);
    }
}
