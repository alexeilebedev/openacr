// Copyright (C) 2023-2024 AlgoRND
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
// Target: ssimfilt (exe) -- Tuple utility
// Exceptions: yes
// Source: cpp/ssimfilt.cpp
//

#include "include/algo.h"
#include "include/lib_json.h"
#include "include/ssimfilt.h"
#include "include/lib_ctype.h"

// -----------------------------------------------------------------------------

void ssimfilt::PrintCmd(algo::Tuple &tuple) {
    tempstr cmd;

    // show whole line
    cmd << "tuple=";
    tempstr temp;
    Tuple_Print(tuple, temp);
    strptr_PrintBash(temp, cmd);
    cmd<< eol;

    // show tuple head
    cmd << "head=";
    strptr_PrintBash(tuple.head.value, cmd);
    cmd<<eol;

    // print the command
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        cmd << attr.name << "=";
        strptr_PrintBash(attr.value, cmd);
        cmd << eol;
    }ind_end;

    cmd << _db.cmdline.cmd<<eol;
    prlog(cmd);
}

// -----------------------------------------------------------------------------

bool ssimfilt::MatchOutputAttr(algo::Attr &attr) {
    bool ret=false;
    if (selfield_N()) {
        ind_beg(ssimfilt::_db_selfield_curs,selfield,_db) {
            if (Regx_Match(selfield.key, attr.name)) {
                ret=true;
                break;
            }
        }ind_end;
    }else {
        ret=true;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Select attrs to display and print as ssim
void ssimfilt::PrintSsim(algo::Tuple &tuple) {
    algo::Tuple out;
    out.head=tuple.head;
    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
        if (MatchOutputAttr(attr)) {
            attrs_Alloc(out)=attr;
        }
    }ind_end;
    prlog(out);
}

// -----------------------------------------------------------------------------

// Print selected fields one by one
void ssimfilt::PrintField(algo::Tuple &tuple) {
    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
        if (MatchOutputAttr(attr)) {
            prlog(attr.value);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Check if input tuple matches filters
bool ssimfilt::MatchInputTuple(algo::Tuple &tuple) {
    // determine typetag from first line of input
    bool ret=true;
    if (_db.cmdline.typetag.expr == "^") {
        Regx_ReadSql(_db.cmdline.typetag, tuple.head.value, true);
    } else {
        ret = Regx_Match(_db.cmdline.typetag, tuple.head.value);
    }
    if (ret) {
        ind_beg(_db_matchfield_curs,match,_db) {
            ind_beg(algo::Tuple_attrs_curs,attr,tuple){
                if (Regx_Match(match.key,attr.name)) {
                    ret=Regx_Match(match.value,attr.value);
                    if (!ret) {
                        break;
                    }
                }
            }ind_end;
            if (!ret) {
                break;
            }
        }ind_end;
    }
    return ret;
}

// -----------------------------------------------------------------------------

void ssimfilt::PrintJson(algo::Tuple &tuple, cstring &out, bool toplevel) {
    algo::ListSep ls(",");
    out<<"{"<<ls;
    out<<"\"@type\":";
    lib_ctype::FCtype *ctype = lib_ctype::TagToCtype(tuple);
    lib_json::JsonSerializeString(tuple.head.value,out);
    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
        if (!toplevel || MatchOutputAttr(attr)) {
            out<<ls;
            lib_json::JsonSerializeString(attr.name,out);
            out<<":";
            auto quoted=true;
            auto isbool=false;
            auto istuple=false;
            // for unknown schema, print fields using quotes
            if (ctype) {
                lib_ctype::FField *field = lib_ctype::FindField(*ctype,attr.name);
                if (field) {
                    istuple = TupleFieldQ(*field);
                    // TODO: create a table
                    quoted = !(field->p_arg->ctype == "double" ||
                               field->p_arg->ctype == "float" ||
                               field->p_arg->ctype == "i8" ||
                               field->p_arg->ctype == "u8" ||
                               field->p_arg->ctype == "i16" ||
                               field->p_arg->ctype == "u16" ||
                               field->p_arg->ctype == "i32" ||
                               field->p_arg->ctype == "u32" ||
                               field->p_arg->ctype == "i64" ||
                               field->p_arg->ctype == "u64" ||
                               field->p_arg->ctype == "u128");
                    isbool = field->p_arg->ctype == "bool";
                }
            }
            if (istuple) {// print recursive json object
                algo::Tuple fieldtuple;
                Tuple_ReadStrptrMaybe(fieldtuple,attr.value);
                PrintJson(fieldtuple,out,false);
            } else if (isbool) {
                bool b=false;
                (void)bool_ReadStrptrMaybe(b,attr.value);
                out<<(b?"true":"false");
            } else if (quoted) {
                lib_json::JsonSerializeString(attr.value,out);
            } else {
                out<<attr.value; // matches json
            }
        }
    }ind_end;
    out<<"}";
}

// Print tuple as a JSON object
// if schema is available (ctype found), determine if the field is a bool, print numeric types
// without quotes.
// if no schema is available, all field values are quoted
void ssimfilt::PrintJson(algo::Tuple &tuple) {
    tempstr out;
    PrintJson(tuple,out,true);
    prlog(out);
}

// -----------------------------------------------------------------------------

void ssimfilt::PrintCsv(algo::Tuple &tuple) {
    // lock CSV header on first match
    tempstr out;
    algo::ListSep ls(",");
    if (bool_Update(_db.csv_locked,true)) {
        verblog("#csv locked to typetag "<<tuple.head.value);
        Regx_ReadSql(_db.cmdline.typetag,tuple.head.value,true);
        if (selfield_N()==0) {
            ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
                KVRegx &kvregx=selfield_Alloc();
                Regx_ReadSql(kvregx.key,attr.name,true);
            }ind_end;
        }
        ind_beg(ssimfilt::_db_selfield_curs,selfield,_db) {
            out<<ls;
            strptr_PrintCsv(selfield.key.expr,out);
        }ind_end;
        out<<eol;
        ls.iter=0;
    }
    ind_beg(ssimfilt::_db_selfield_curs,selfield,_db) {
        out<<ls;
        ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
            if (selfield.key.expr==attr.name) {
                strptr_PrintCsv(attr.value,out);
                break;
            }
        }ind_end;
    }ind_end;
    prlog(out);
}

// -----------------------------------------------------------------------------

void ssimfilt::Table_Save(algo::Tuple &tuple) {
    // flush when input type changes
    if (tuple_N()>0) {
        auto& header = tuple_qFind(0);
        auto flush = header.head.name == ""
            ? header.head.value != tuple.head.value
            : header.head.name != tuple.head.name;
        flush |= attrs_N(tuple) != attrs_N(header);
        if (flush) {
            Table_Flush();
        }
    }
    // save tuple
    algo::Tuple &saved=tuple_Alloc();
    saved.head=tuple.head;
    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
        attrs_Alloc(saved)=attr;
    }ind_end;
}

// -----------------------------------------------------------------------------

static void AddCol(algo_lib::FTxttbl& txttbl, algo::Attr& attr) {
    if (attr.name != "" && ssimfilt::MatchOutputAttr(attr)) {
        tempstr name(attr.name);
        ind_beg(algo::cstring_ch_curs,ch,name) {
            ch=algo::ToUpper(ch);
        }ind_end;
        AddCol(txttbl,name);
    }
}

static void AddCell(algo_lib::FTxttbl& txttbl, algo::Attr& attr) {
    if (attr.name != "" && ssimfilt::MatchOutputAttr(attr)) {
        AddCell(txttbl)=attr.value;
    }
}

void ssimfilt::Table_Flush() {
    algo_lib::FTxttbl txttbl;
    ind_beg(_db_tuple_curs,tuple,_db) {
        if (ind_curs(tuple).index==0) {
            AddCol(txttbl, tuple.head);
            ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
                AddCol(txttbl, attr);
            }ind_end;
        }
        AddRow(txttbl);
        AddCell(txttbl, tuple.head);
        ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
            AddCell(txttbl, attr);
        }ind_end;
    }ind_end;
    tempstr out;
    FTxttbl_Print(txttbl,out);
    prlog(out);
    tuple_RemoveAll();
}

// -----------------------------------------------------------------------------

void ssimfilt::Main() {
    lib_ctype::Init();
    // Populate input filter
    ind_beg(command::ssimfilt_match_curs,match,_db.cmdline) {
        strptr key=Pathcomp(match,":LL");
        strptr value=Pathcomp(match,":LR");
        KVRegx &kvregx=matchfield_Alloc();
        Regx_ReadSql(kvregx.key,key,true);
        Regx_ReadSql(kvregx.value,value,true);
    }ind_end;

    // Populate output select
    ind_beg(command::ssimfilt_field_curs,match,_db.cmdline) {
        KVRegx &kvregx=selfield_Alloc();
        Regx_ReadSql(kvregx.key,match,true);
    }ind_end;

    if (_db.cmdline.t) {
        _db.cmdline.format = command_ssimfilt_format_table;
    }
    if (_db.cmdline.cmd != "") {
        _db.cmdline.format = command_ssimfilt_format_cmd;
    }
    ind_beg(algo::FileLine_curs,line,algo::Fildes(0)) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptr(tuple,line,false) && (attrs_N(tuple) || tuple.head.value != "")) {
            if (MatchInputTuple(tuple)) {
                switch(_db.cmdline.format) {
                    break; case command_ssimfilt_format_ssim: PrintSsim(tuple);
                    break; case command_ssimfilt_format_csv: PrintCsv(tuple);
                    break; case command_ssimfilt_format_field: PrintField(tuple);
                    break; case command_ssimfilt_format_table: Table_Save(tuple);
                    break; case command_ssimfilt_format_cmd: PrintCmd(tuple);
                    break; case command_ssimfilt_format_json: PrintJson(tuple);
                }
            }
        }
    }ind_end;
    if (_db.cmdline.format == command_ssimfilt_format_table) {
        Table_Flush();
    }
}
