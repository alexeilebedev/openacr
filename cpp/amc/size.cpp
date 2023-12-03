// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2023 AlgoRND
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/size.cpp -- Compute struct sizes
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static bool VardeclQ(strptr line) {
    return EndsWithQ(line,";") // split lines won't have ; at the end
        && !EndsWithQ(line,");") // function declaration
        && !EndsWithQ(line," const;") // also function
        && !EndsWithQ(line,"};")// enum
        ;
}

// -----------------------------------------------------------------------------

static int ComputeCtypeSize(amc::FCtype *ctype);

// -----------------------------------------------------------------------------

static int RoundUp(int size, int alignment) {
    return (size+alignment-1) & ~(alignment-1);
}

// -----------------------------------------------------------------------------

static void PadToAlignment(amc::FCtype &ctype, int alignment) {
    u32_UpdateMax(ctype.alignment, alignment);
    int padding = alignment==1 ? 0 : RoundUp(ctype.totsize_byte,alignment)-ctype.totsize_byte;
    ctype.n_padbytes += padding;
    ctype.totsize_byte += padding;
}

// -----------------------------------------------------------------------------

static void CountStructField(amc::FCtype &ctype, strptr name, amc::FCtype *ftype, int multiple) {
    ComputeCtypeSize(ftype);
    if (ftype->size_unknown) {
        // won't know the size!
        ctype.size_unknown=true;
        // allow user to debug this occurence with -v
        prerr("amc.size_unknown_recursive"
              << Keyval("ctype",ctype.ctype)
              << Keyval("field",name)
              << Keyval("fieldtype",ftype->ctype)
              << Keyval("field_is_extern",ftype->c_cextern)
              << Keyval("comment", "size of field unknown, recursively propagated to the type"));
        algo_lib::_db.exit_code=1;
    } else {
        (void)name;
        int alignment = ctype.c_pack ? 1 : ftype->alignment;
        PadToAlignment(ctype, alignment);
        // do we really want offsetof?? too voluminous
        ctype.sizecheck << "algo_assert(_offset_of("<<ctype.cpp_type<<","<<name<<") == "<<ctype.totsize_byte<<");\n";
        // zero-size fields not allowed because addresses of any 2
        // fields must be different.
        // so we gain a byte for every empty field
        ctype.totsize_byte += ftype->totsize_byte == 0 ? 1 : ftype->totsize_byte * multiple;
    }
}

// -----------------------------------------------------------------------------

static amc::FCtype *LookupCppType(strptr s) {
    tempstr fieldtype(s);
    Replace(fieldtype,"::",".");
    // handle pointers as u64
    return EndsWithQ(fieldtype,"*")
        ? amc::_db.c_u64
        : amc::ind_ctype_Find(fieldtype);
}

// -----------------------------------------------------------------------------

// parse a compile time constant, just a few cases to consider:
// a+b
// sizeof(a) * b
// a
static int ParseCompileTimeConst(strptr s) {
    int idx=0;
    int ret=0;
    if ((idx=FindChar(s,'*'))!=-1) {
        strptr left=TrimmedRight(FirstN(s,idx));
        strptr right=TrimmedLeft(RestFrom(s,idx+1));
        ret= ParseCompileTimeConst(left) * ParseCompileTimeConst(right);
    } else if ((idx=FindChar(s,'+'))!=-1) {
        strptr left=TrimmedRight(FirstN(s,idx));
        strptr right=TrimmedLeft(RestFrom(s,idx+1));
        int sizeleft = ParseCompileTimeConst(left);
        int sizeright = ParseCompileTimeConst(right);
        // zero = bad size, respect that
        ret= sizeleft > 0 && sizeright > 0 ? sizeleft + sizeright : 0;
    } else if (StartsWithQ(s,"sizeof(")) {
        strptr arg=Trimmed(Pathcomp(s,"(LR)RL"));
        amc::FCtype *ctype=LookupCppType(arg);
        ret= ctype ? ComputeCtypeSize(ctype) : 0;
    } else if (StartsWithQ(s,"sizeu128(")) {
        strptr args=Trimmed(Pathcomp(s,"(LR)RL"));
        strptr arg1=Trimmed(Pathcomp(args,",LL"));
        strptr arg2=Trimmed(Pathcomp(args,",RR"));
        amc::FCtype *ctype=LookupCppType(arg1);
        int size=ctype ? ComputeCtypeSize(ctype) : 0;
        int n=ParseCompileTimeConst(arg2);
        ret = (size*n+15)/16;
    } else {
        ret = ParseI32(s,0);
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void ParseStructField(amc::FCtype &ctype, strptr line) {
    line=Trimmed(line);// leading whitespace
    int index=FindStr(line,";");
    if (index!=-1) {
        line=FirstN(line,index);
        strptr fieldtype(Pathcomp(line," LL"));
        strptr name(Trimmed(Pathcomp(line," LR[LL")));
        strptr strmultiple = Trimmed(Pathcomp(line,"[LR]RL"));
        int multiple = strmultiple.n_elems ? ParseCompileTimeConst(strmultiple) : 1;
        amc::FCtype *ftype = LookupCppType(fieldtype);
        if (!ftype || multiple==0) {
            verblog("amc.compute_size"
                    << Keyval("ctype",ctype.ctype)
                    << Keyval("fieldtype",fieldtype)
                    << Keyval("multiple",strmultiple)
                    << Keyval("line",line)
                    << Keyval("comment", "can't determine size -- unknown type or size expression is too complicated"));
            ctype.size_unknown=true;// complain
        } else {
            CountStructField(ctype,name,ftype,multiple);
        }
    }
}

// -----------------------------------------------------------------------------

static strptr StripComment(strptr line) {
    int comment=FindStr(line,"//");
    if (comment != -1) {
        line=Trimmed(FirstN(line,comment));
    }
    return line;
}

// -----------------------------------------------------------------------------

static int CountCurly(strptr line, int ncurly) {
    if (EndsWithQ(line,"{")) {
        ncurly++;
    }
    if (EndsWithQ(line,"}")) {
        ncurly--;
    }
    return ncurly;
}

// -----------------------------------------------------------------------------

// Query whether CTYPE fields should be packed (field alignment=1)
bool amc::PackQ(amc::FCtype &ctype) {
    return ctype.c_pack != NULL;
}

// -----------------------------------------------------------------------------

static int ComputeCtypeSize(amc::FCtype *ctype) {
    if (bool_Update(ctype->size_locked,true)) {
        if (ctype->c_csize) {
            ctype->totsize_byte = ctype->c_csize->size;
            ctype->alignment = ctype->c_csize->alignment;
        } else if (ctype->c_cextern) {
            ctype->size_unknown = true;
        } else {
            int ncurly=0;
            ind_beg(Line_curs,line,ctype->body) {
                line=StripComment(line);
                ncurly=CountCurly(line,ncurly);// avoid inline functions
                if (VardeclQ(line) && !ncurly) {
                    ParseStructField(*ctype,line);
                }
            }ind_end;
            // insert padding at the end
            PadToAlignment(*ctype,ctype->alignment);
        }
    }
    return ctype->totsize_byte;
}

// -----------------------------------------------------------------------------

// Recursively compute sizes of all ctypes,
// and generate per-namespace SizeCheck function,
// asserting that amc-computed sizes are the same as gcc-computed sizes.
void amc::gen_compute_size() {
    amc::_db.c_u64 = amc::ind_ctype_Find("u64");
    ind_beg(amc::_db_csize_curs,csize,amc::_db) {
        if (csize.size == 0 || csize.alignment == 0 || csize.size % csize.alignment != 0) {
            prerr("amc.invalid_size"
                  <<Keyval("csize",csize.ctype)
                  <<Keyval("size",csize.size)
                  <<Keyval("alignment",csize.alignment)
                  <<Keyval("comment","Both size and alignment must be >0, and size must be a multiple of alignment"));
            algo_lib::_db.exit_code=1;
        }
    }ind_end;
    ind_beg(amc::_db_ns_curs, ns, amc::_db) if (ns.select && ns.ns != "") {
        amc::FFunc *check = &amc::ind_func_GetOrCreate(tempstr() << ns.ns << "...SizeCheck");
        check->ret = "void";
        check->inl = true;// makes it immune to the 'unused' warning
        check->proto = "SizeCheck()";
        check->priv = true;
        ind_beg(amc::ns_c_ctype_curs, ctype, ns) {
            if (ctype.original) {
                if (ComputeCtypeSize(&ctype)!=-1 && ctype.totsize_byte>0 && !ctype.size_unknown) {
                    dmmeta::Ctypelen ctypelen;
                    ctypelen.ctype = ctype.ctype;
                    ctypelen.len = ctype.totsize_byte;
                    ctypelen.alignment = ctype.alignment;
                    ctypelen.padbytes = ctype.n_padbytes;
                    amc::ctypelen_InsertMaybe(ctypelen);// create it
                    if (ctype.p_ns->c_nsx && ctype.p_ns->c_nsx->pack) {
                        check->body << ctype.sizecheck;
                        check->body << "    algo_assert(sizeof("<<ctype.cpp_type<<") == "<<ctype.totsize_byte<<");\n";
                        Refurbish(ctype.sizecheck);
                    }
                }
            }
        }ind_end;
    }ind_end;
    amc::c_ctypelen_QuickSort();
}

// -----------------------------------------------------------------------------

// Generate symbol corresponding to the size of ctype CTYPE.
// SizeEnums are generated only for occasional ctypes, and are used
// to avoid circular dependencies in headers
tempstr amc::SizeEnum(amc::FCtype &ctype) {
    tempstr ret;
    ret << "sizeof_" << ctype.ctype;
    Translate(ret,".","_");
    return ret;
}

// -----------------------------------------------------------------------------

// Generate compile-time constants for sizes of certain ctypes
// accessed from namespace NS, without having to include corresponding header.
void amc::gen_ns_size_enums() {
    amc::FNs &ns=*amc::_db.c_ns;
    tempstr str;
    algo::ListSep ls(", ");
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            field.p_arg->enum_visited=false;
        }ind_end;
    }ind_end;
    ind_beg(amc::ns_c_ctype_curs,ctype,ns) {
        ind_beg(amc::ctype_c_field_curs,field,ctype) {
            if (field.reftype == dmmeta_Reftype_reftype_Opt && bool_Update(field.p_arg->enum_visited, true)) {
                if (field.p_arg->c_ctypelen) {
                    str << "    " << ls << SizeEnum(*field.p_arg) << " = " << field.p_arg->c_ctypelen->len << eol;
                } else {
                    // code won't compile if we can't use the sizeenum
                    prerr("amc.nolength"
                          <<Keyval("field",field.field)
                          <<Keyval("arg",field.arg)
                          <<Keyval("comment","Ctypelen must be enabled for this type"));
                    algo_lib::_db.exit_code=1;
                }
            }
        }ind_end;
    }ind_end;
    if (ch_N(str)) {
        BeginNsBlock(*ns.hdr, ns, "");
        *ns.hdr << ""<<eol;
        *ns.hdr << "// sizes types appearing as Opt or Varlen; these need to be here"<<eol;
        *ns.hdr << "// to allow inline implementations of _Get functions and avoid"<<eol;
        *ns.hdr << "// a circular dependency between headers"<<eol;
        *ns.hdr << "enum {" << eol;
        *ns.hdr << str;
        *ns.hdr << "};" << eol;
        EndNsBlock(*ns.hdr, ns, "");
    }
}
