// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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

static int ComputeCtypeSize(amc::FCtype *ctype); // ignore:static_fwd_decl

// -----------------------------------------------------------------------------

// Largest alignment a csize row may declare; gen_compute_size rejects more.
// RoundUp adds alignment-1 to a size before masking, so NarrowSize keeps
// every accepted size MaxAlignment-1 bytes below i32 max, and the sum cannot
// overflow for any accepted alignment.
static const u32 MaxAlignment = 16;

// -----------------------------------------------------------------------------

// SIZE stays below the NarrowSize cap, which leaves headroom for
// alignments up to MaxAlignment, so size+alignment-1 cannot overflow i64.
// ALIGNMENT is a power of two at most MaxAlignment: gen_compute_size rejects
// any csize row outside that range, and computed alignments only propagate
// csize values.
static i64 RoundUp(i64 size, int alignment) {
    return (size+alignment-1) & ~i64(alignment-1);
}

// -----------------------------------------------------------------------------

// Narrow a computed byte size or array bound to i32.
// Compile-time size arithmetic multiplies i32 values (element size times
// array multiple), so a valid schema can produce a product beyond i32 --
// e.g. a 65536-byte struct in an Inlary of 65537 wrapped to 65536 and
// shipped as a bogus Ctypelen with exit code 0. Such a size is not
// representable in the generated code: report an error and return 0, the
// established bad-size marker (callers mark the ctype size_unknown).
// The cap leaves headroom for RoundUp/PadToAlignment (alignment <= MaxAlignment).
static int NarrowSize(i64 size, const strptr &src) {
    if (size < 0 || size > 0x7fffffff-i64(MaxAlignment-1)) {
        prerr("amc.size_overflow"
              << Keyval("src",src)
              << Keyval("size",size)
              << Keyval("comment","computed size does not fit in i32"));
        algo_lib::_db.exit_code=1;
        size = 0;
    }
    return int(size);
}

// -----------------------------------------------------------------------------

static void PadToAlignment(amc::FCtype &ctype, int alignment) {
    u32_UpdateMax(ctype.alignment, alignment);
    int padding = alignment==1 ? 0 : int(RoundUp(ctype.totsize_byte,alignment))-ctype.totsize_byte;
    ctype.n_padbytes += padding;
    ctype.totsize_byte += padding;
}

// -----------------------------------------------------------------------------

static void CountStructField(amc::FCtype &ctype, strptr name, amc::FCtype *ftype, int multiple, int alignment) {
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
        PadToAlignment(ctype, ctype.c_pack ? 1 : alignment);
        // do we really want offsetof?? too voluminous
        ctype.sizecheck << "algo_assert(_offset_of("<<ctype.cpp_type<<","<<name<<") == "<<ctype.totsize_byte<<");\n";
        // zero-size fields not allowed because addresses of any 2
        // fields must be different.
        // so we gain a byte for every empty field
        i64 size = ctype.totsize_byte + (ftype->totsize_byte == 0 ? i64(1) : i64(ftype->totsize_byte) * multiple);
        int newsize = NarrowSize(size, tempstr()<<ctype.ctype<<"."<<name);
        if (newsize == 0) {
            ctype.size_unknown = true;
        } else {
            ctype.totsize_byte = newsize;
        }
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
        ret= NarrowSize(i64(ParseCompileTimeConst(left)) * ParseCompileTimeConst(right), s);
    } else if ((idx=FindChar(s,'+'))!=-1) {
        strptr left=TrimmedRight(FirstN(s,idx));
        strptr right=TrimmedLeft(RestFrom(s,idx+1));
        int sizeleft = ParseCompileTimeConst(left);
        int sizeright = ParseCompileTimeConst(right);
        // zero = bad size, respect that
        ret= sizeleft > 0 && sizeright > 0 ? NarrowSize(i64(sizeleft) + sizeright, s) : 0;
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
        ret = NarrowSize((i64(size)*n+15)/16, s);
    } else {
        ret = NarrowSize(ParseI64(s,0), s);
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void ParseStructField(amc::FCtype &ctype, strptr line) {
    line=Trimmed(line);// leading whitespace
    int index=FindStr(line,";");
    if (index!=-1) {
        line=FirstN(line,index);
        // A field declared as raw storage states the alignment it needs, because
        // its element type is a character and a character aligns to one.  The
        // type named in that statement supplies the alignment; the element type
        // still supplies the size, so the prefix comes off before the parse.
        amc::FCtype *aligntype = NULL;
        if (StartsWithQ(line,"alignas(")) {
            aligntype = LookupCppType(Trimmed(Pathcomp(line,"(LR)LL")));
            line = TrimmedLeft(RestFrom(line, FindChar(line,')')+1));
        }
        strptr fieldtype(Pathcomp(line," LL"));
        strptr name(Trimmed(Pathcomp(line," LR[LL")));
        strptr strmultiple = Trimmed(Pathcomp(line,"[LR]RL"));
        int multiple = strmultiple.n_elems ? ParseCompileTimeConst(strmultiple) : 1;
        amc::FCtype *ftype = LookupCppType(fieldtype);
        if (aligntype != NULL) {
            ComputeCtypeSize(aligntype);
        }
        if (!ftype || multiple==0) {
            verblog("amc.compute_size"
                    << Keyval("ctype",ctype.ctype)
                    << Keyval("fieldtype",fieldtype)
                    << Keyval("multiple",strmultiple)
                    << Keyval("line",line)
                    << Keyval("comment", "can't determine size -- unknown type or size expression is too complicated"));
            ctype.size_unknown=true;// complain
        } else {
            CountStructField(ctype,name,ftype,multiple,aligntype ? aligntype->alignment : ftype->alignment);
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

// A plaindata ctype of at most this many bytes is passed by value; a larger one
// is passed by const reference.
// Sixteen bytes is two machine registers, which is also where the platform ABI
// puts the boundary: a trivially copyable aggregate of at most this size is
// passed in registers, and a larger one is passed in memory, at which point the
// caller has built the very copy a reference would have avoided. algo::strptr
// and algo::memptr sit exactly on this boundary, and the whole codebase passes
// them by value.
static const int MaxCheapCopySize = 16;

// -----------------------------------------------------------------------------

// Compute the byte size and alignment the schema declares for CTYPE, and store
// both on the ctype. declared_size stays 0 when the schema does not determine
// the layout.
//
// gen_compute_size below answers the same question exactly, by parsing the
// struct amc emitted, and it therefore cannot run until every field tclass has
// emitted its member declarations. Cheap-copy has to be settled before that,
// because Argtype consults it while emitting each function signature. This walk
// is the part of the answer that the schema alone supports, which is enough to
// decide cheap-copy: a csize row states the size of a type amc does not declare
// and grounds the recursion at every builtin, and otherwise the size is the sum
// of the members' sizes with alignment padding between them, exactly as a C++
// compiler lays out a plain aggregate.
//
// A Val member contributes its own type's declared size and a Ptr member
// contributes a pointer. A Base member contributes nothing, since gen_newfield_cbase
// has already cloned the base's fields into this ctype and they are counted in
// their own right. A Bitfld member is stored inside another field and a fldfunc
// field is a function, so neither occupies storage.
//
// Every other reftype is laid out by its own generator, and the field record
// alone does not say how: an Inlary's element count lives in dmmeta.inlary, a
// Smallstr's capacity is encoded in its ctype name. Such a member leaves the
// size undetermined rather than guessed, so the containing type loses its
// cheap-copy status. Undetermined never claims a type is smaller than it is,
// which is the direction that matters -- a 200-byte string passed by value is
// the defect being removed here.
//
// A ctype reached twice returns the size already stored, so the walk is linear.
// A ctype that contains itself by value is reached while its own size is still
// 0 and reads as undetermined; gen_compute_size reports that cycle as an error.
static void DeclareSize(amc::FCtype &ctype) {
    if (!amc::zs_sig_visit_InLlistQ(ctype)) {
        amc::zs_sig_visit_Insert(ctype);
        if (ctype.c_csize) {
            ctype.declared_size = ctype.c_csize->size;
            ctype.declared_alignment = ctype.c_csize->alignment;
        } else if (!ctype.c_cextern) {
            i64 size = 0;
            int alignment = 1;
            bool determined = true;
            ind_beg(amc::ctype_c_field_curs,field,ctype) {
                amc::FCtype *argtype = field.reftype == dmmeta_Reftype_reftype_Ptr
                    ? amc::_db.c_u64
                    : field.p_arg;
                bool nostorage = amc::FldfuncQ(field)
                    || field.c_bitfld
                    || field.reftype == dmmeta_Reftype_reftype_Base;
                bool counted = field.reftype == dmmeta_Reftype_reftype_Val
                    || field.reftype == dmmeta_Reftype_reftype_Ptr;
                if (nostorage) {
                    // stored elsewhere, or not stored at all
                } else if (!counted || !argtype) {
                    determined = false;
                } else {
                    DeclareSize(*argtype);
                    int fldalign = ctype.c_pack ? 1 : argtype->declared_alignment;
                    determined = determined && argtype->declared_size > 0;
                    size = RoundUp(size,fldalign) + argtype->declared_size;
                    i32_UpdateMax(alignment,fldalign);
                }
            }ind_end;
            size = RoundUp(size,alignment);
            if (determined && size > 0 && size <= 0x7fffffff) {
                ctype.declared_size = int(size);
                ctype.declared_alignment = alignment;
            }
        }
    }
}

// -----------------------------------------------------------------------------

// Determine which ctypes are cheap copy, i.e. passed to a function by value
// rather than by const reference.
// A ctype is cheap copy when copying it is a memcpy -- that is what plaindata
// means -- and when it is small enough that the copy costs no more than the
// reference would. Both halves are computed: the property is a fact about the
// ctype's contents, so nothing asserts it by hand and nothing can assert it
// wrongly.
void amc::gen_cheapcopy() {
    amc::_db.c_u64 = amc::ind_ctype_Find("u64");
    amc::zs_sig_visit_RemoveAll();
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        DeclareSize(ctype);
        ctype.cheap_copy = ctype.plaindata
            && ctype.declared_size > 0
            && ctype.declared_size <= MaxCheapCopySize;
    }ind_end;
}

// -----------------------------------------------------------------------------

static int ComputeCtypeSize(amc::FCtype *ctype) {
    if (bool_Update(ctype->size_locked,true)) {
        ctype->in_compute_size = true;
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
        ctype->in_compute_size = false;
    } else if (ctype->in_compute_size) {
        // Re-entry while this ctype's own body is still being walked: the
        // ctype contains itself by value (directly or through a chain of
        // by-value fields), so no finite size exists. Without this check the
        // call would return the not-yet-computed totsize_byte (0), the field
        // would be counted as 1 byte (the zero-size rule above), and a bogus
        // size would ship with exit code 0, leaving the C++ compiler to
        // reject the generated struct. Mark the size unknown instead:
        // CountStructField then reports size_unknown_recursive for each
        // by-value edge on the stack, tracing the cycle.
        ctype->size_unknown = true;
        prerr("amc.size_cycle"
              << Keyval("ctype",ctype->ctype)
              << Keyval("comment","ctype contains itself by value; size cannot be computed"));
        algo_lib::_db.exit_code=1;
    }
    return ctype->totsize_byte;
}

// -----------------------------------------------------------------------------

// Bit width of a ctype's storage: builtins carry an explicit csize; other
// types carry a computed totsize_byte. Returns 0 when the size is not known
// in this generation run: neither value is set, or the size computation
// failed (size_unknown), in which case totsize_byte holds only the partial
// sum of the members counted before the failure and must not be reported
// as a width.
// The result is i64: the size cap (NarrowSize) and the uncapped csize.size
// both permit byte sizes whose bit count exceeds i32, and a wrapped width
// would misfire every >0-gated width check downstream.
i64 amc::Ctype_Nbit(amc::FCtype &ctype) {
    i64 size = ctype.c_csize ? i64(ctype.c_csize->size) : ctype.totsize_byte;
    return ctype.size_unknown ? 0 : 8*size;
}

// -----------------------------------------------------------------------------

// Recursively compute sizes of all ctypes,
// and generate per-namespace SizeCheck function,
// asserting that amc-computed sizes are the same as gcc-computed sizes.
// The sizes are computed by scanning the actual generated struct for each ctype.
void amc::gen_compute_size() {
    ind_beg(amc::_db_csize_curs,csize,amc::_db) {
        if (csize.size == 0 || csize.alignment == 0 || csize.size % csize.alignment != 0) {
            prerr("amc.invalid_size"
                  <<Keyval("csize",csize.ctype)
                  <<Keyval("size",csize.size)
                  <<Keyval("alignment",csize.alignment)
                  <<Keyval("comment","Both size and alignment must be >0, and size must be a multiple of alignment"));
            algo_lib::_db.exit_code=1;
        } else if (!algo::u32_Pow2Q(csize.alignment) || csize.alignment > MaxAlignment) {
            // RoundUp computes the rounding mask as ~(alignment-1), which is
            // only correct for a power of two, and NarrowSize's headroom
            // only covers size+alignment-1 for alignments up to MaxAlignment.
            prerr("amc.invalid_size"
                  <<Keyval("csize",csize.ctype)
                  <<Keyval("size",csize.size)
                  <<Keyval("alignment",csize.alignment)
                  <<Keyval("comment","alignment must be a power of 2, at most 16; a larger alignment needs wider NarrowSize headroom"));
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
                    ctypelen.plaindata = ctype.plaindata;// save this, useful
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
    // A bitfield wider than its accessor arg truncates on read: the getter casts
    // the masked value back to that arg type. Ctype_Nbit is 0 when the arg size
    // is unknown (its namespace is not generated this run), so skip those.
    // Signedness is read from the type the arg resolves to, not from the arg
    // itself: a wrapper ctype -- one Val field over an integer -- carries no
    // amcdb.bltin row of its own, and a narrow signed slice reached through
    // such a wrapper truncates exactly as a direct one does.
    ind_beg(amc::_db_field_curs, field, amc::_db) {
        if (field.c_bitfld) {
            amc::FCtype &argtype = *field.p_arg;
            amc::FCtype &argbase = *amc::StripWrappers(argtype, &argtype);
            i64 argbits = Ctype_Nbit(argtype);
            bool issigned = argbase.c_bltin && argbase.c_bltin->issigned;
            if (argbits > 0 && field.c_bitfld->width > argbits) {
                prerr("amc.bitfield_width_type"
                      <<Keyval("field",field.field)
                      <<Keyval("width",field.c_bitfld->width)
                      <<Keyval("argbits",argbits)
                      <<Keyval("comment","bitfield is wider than its arg type and would truncate on read; widen arg"));
                algo_lib::_db.exit_code++;
            } else if (argbits > 0 && issigned && field.c_bitfld->width < argbits) {
                prerr("amc.bitfield_signed_narrow"
                      <<Keyval("field",field.field)
                      <<Keyval("width",field.c_bitfld->width)
                      <<Keyval("argbits",argbits)
                      <<Keyval("comment","signed bitfield narrower than its arg type does not sign-extend on read; use an unsigned arg or make width span the full arg type"));
                algo_lib::_db.exit_code++;
            }
        }
    }ind_end;
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
