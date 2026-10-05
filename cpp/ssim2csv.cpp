// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Target: ssim2csv (exe) -- Ssim -> csv conversion tool
// Exceptions: yes
// Source: cpp/ssim2csv.cpp
//

#include "include/algo.h"
#include "include/gen/ssim2csv_gen.h"
#include "include/gen/ssim2csv_gen.inl.h"

//
// print array of strings as CSV tokens
// escape single quote, \n, \r
// enclose each token in single quotes
//
static void CsvPrint(algo::aryptr<cstring> in, cstring &out) {
    ch_RemoveAll(out);
    algo::ListSep ls(",");
    frep_(i,elems_N(in)) {
        tempstr tok;
        tok<<in[i];
        Translate(tok,"'\n\r","   ");
        if (ssim2csv::_db.cmdline.ignoreQuote) {
            out<<ls<<tok;
        } else {
            out<<ls<<"'"<<tok<<"'"; // ignore:hand_quote
        }
    }
    out<<eol;
}

static void HandleLine(strptr line, int lineno) {
    ssim2csv::name_RemoveAll();
    ssim2csv::value_RemoveAll();

    ssim2csv::FFlatten &flatten = ssim2csv::flatten_Alloc();
    flatten.in = line;
    //
    // attributes for which EXPAND is set are tuples;
    // parse them and add their attributes to the output line, adding dots:
    // input:   a:b c:"d e:f"
    // expand:c
    // output:  a.b c:d c.e:f
    //
    // flatten
    Tuple temp;
    while (ssim2csv::flatten_N()) {
        ssim2csv::FFlatten fl = ssim2csv::flatten_qLast();
        ssim2csv::flatten_RemoveLast();
        Tuple_ReadStrptr(temp,fl.in,true);
        ind_beg(algo::Tuple_attrs_curs, attr, temp) {
            tempstr name(fl.prefix);
            if (ch_N(fl.prefix)) name << ".";
            name << attr.name;
            if (!ssim2csv::ind_expand_Find(name)) {
                ssim2csv::name_Alloc() << name;
                ssim2csv::value_Alloc() << attr.value;
            } else {
                ssim2csv::FFlatten &newfl = ssim2csv::flatten_Alloc();
                newfl.in = attr.value;
                newfl.prefix = name;
            }
        }ind_end;
    }
    if (ssim2csv::value_N() > 0) {
        tempstr out_name;
        tempstr out_value;
        CsvPrint(RestFrom(ssim2csv::name_Getary(),1),out_name);
        CsvPrint(RestFrom(ssim2csv::value_Getary(),1),out_value);
        ssim2csv::FOutfile &outfile = ssim2csv::ind_outfile_GetOrCreate(ssim2csv::value_qFind(0));
        if (!ValidQ(outfile.fildes.fd)) {
            tempstr fname;
            fname << ssim2csv::value_qFind(0) << ".csv";
            prerr("open "<<fname);
            outfile.header = out_name;
            outfile.fildes.fd = OpenWrite(fname);
            WriteFileX(outfile.fildes.fd, strptr_ToMemptr(out_name));
        }
        vrfy(outfile.header == out_name,tempstr()<< lineno<<": header mismatch: was \n"
             <<outfile.header<<"\n now \n"<<out_name<<"");
        WriteFileX(outfile.fildes.fd, strptr_ToMemptr(out_value));
    }
}

void ssim2csv::Main() {
    strptr toks=ssim2csv::_db.cmdline.expand;
    while (elems_N(toks)) {
        algo::i32_Range R=TFind(toks,',');
        ssim2csv::ind_expand_GetOrCreate(FirstN(toks,R.beg));
        toks=RestFrom(toks,R.end);
    }
    ind_beg(algo::FileLine_curs,line,Fildes(0)) {
        if (elems_N(line)) {
            HandleLine(line,ind_curs(line).i+1);
        }
    }ind_end;
}
