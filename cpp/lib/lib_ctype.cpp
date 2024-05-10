// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2016-2019 NYSE | Intercontinental Exchange
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
// Target: lib_ctype (lib) -- Helpful library for reading/writing ctypes as text tuples
// Exceptions: yes
// Source: cpp/lib/lib_ctype.cpp
//

#include "include/algo.h"
#include "include/lib_ctype.h"

// -----------------------------------------------------------------------------

// Find ctype from ctype name
// Supports ctype and ssimfile lookups.
lib_ctype::FCtype *lib_ctype::TagToCtype(strptr name) {
    lib_ctype::FSsimfile *ssimfile = lib_ctype::ind_ssimfile_Find(name);
    lib_ctype::FCtype *ctype = ssimfile ? ssimfile->p_ctype : lib_ctype::ind_ctype_Find(name);
    return ctype;
}

// Find ctype from tuple type tag.
// Supports ctype and ssimfile lookups.
lib_ctype::FCtype *lib_ctype::TagToCtype(algo::Tuple &tuple) {
    return lib_ctype::TagToCtype(tuple.head.value);
}

// -----------------------------------------------------------------------------

// Check if specified attribute matches the default value for the field.
bool lib_ctype::EqualDefaultQ(algo::Attr *attr, lib_ctype::FField &field) {
    lib_ctype::FCdflt *cdflt=field.p_arg->c_cdflt;
    return attr != NULL  &&
        (attr->value == field.dflt.value//
         || (!ch_N(field.dflt.value) // no custom field default specified
             && cdflt // cdflt present
             && ch_N(cdflt->ssimdflt)>0 // ssimdflt value specified
             && attr->value == cdflt->ssimdflt));// they are equal
}

// -----------------------------------------------------------------------------

// Print attributes from TUPLE,
// which is known to be of ctype CTYPE, to cstring TEXT.
// Output fields in normalized order, respecting Base.
// If SKIP_DFLT is true, do not print fields which happen to match their default.
// Suports Varlen fields.
// Attributes that are themselves tuples are recursively normalized as well.
void lib_ctype::PrintTupleAttrs(cstring& text, algo::Tuple &tuple, lib_ctype::FCtype &ctype, bool skip_dflt) {
    ind_beg(lib_ctype::ctype_c_field_curs, field, ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Base) {// print base fields (recursive)
            lib_ctype::PrintTupleAttrs(text, tuple, *field.p_arg, skip_dflt);
        } else if (field.c_cppfunc || field.c_substr) {
            // skip fldfuncs!
        } else {
            algo::Attr *attr=algo::attr_Find(tuple,name_Get(field),0);// regular attr
            if (attr && !(skip_dflt && EqualDefaultQ(attr,field))) {
                PrintAttrSpace(text, attr->name, attr->value);
            }
            // #AL# handle varlen fields recursively!
            int i=0;
            // varlen etc.
            while ((attr = algo::attr_Find(tuple,(tempstr()<<name_Get(field)<<"."<<i)))!=NULL) {
                PrintAttrSpace(text, attr->name, attr->value);
                i++;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Normalize a string that's supposed to correspond CTYPE
// If CTYPE is NULL, it is guessed from the type tag
// The following actions are done:
// 1. Print correct type tag back
// 2. Remove any attributes from tuple that don't correspond to fields of CTYPE
// 3. Print back any fields in ctype that don't appear in the string
// 4. Optionally skip printing any field that's already equal to the default value (if SKIP_DFLT is specified)
// 5. Recursively call NormalizeTuple on any field that has Tuple print format
// NOTE:
// Ctype from parent field has been removed, as it is wrong for derived types!
tempstr lib_ctype::NormalizeSsimTuple(strptr str, bool skip_dflt) {
    Tuple tuple;
    tempstr out;
    if (Tuple_ReadStrptrMaybe(tuple,str)) {
        lib_ctype::FCtype *ctype = lib_ctype::TagToCtype(tuple);
        if (ctype) {
            out << ctype->ctype;
            ind_beg(lib_ctype::ctype_c_field_curs, field, *ctype) {
                if (lib_ctype::TupleFieldQ(field)) {
                    if (algo::Attr *attr=algo::attr_Find(tuple,name_Get(field),0)) {
                        attr->value = NormalizeSsimTuple(attr->value, skip_dflt);
                    }
                }
            }ind_end;
            PrintTupleAttrs(out, tuple, *ctype, skip_dflt);
        }
    }
    return out;
}

// -----------------------------------------------------------------------------

// Retrieve base type for the given ctype
lib_ctype::FCtype *lib_ctype::Basetype(lib_ctype::FCtype &ctype) {
    //TODO: this doesn't support 2 level basetype
    lib_ctype::FCtype *retval = &ctype;
    ind_beg(lib_ctype::ctype_c_field_curs,field,ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Base) {
            retval = field.p_arg;
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

// Find field by name, possibly scanning base fields
// Skips dot-suffix first, i.e. attr "abc.4" is looked up as "abc" (used for Varlen fields)
lib_ctype::FField *lib_ctype::FindField(lib_ctype::FCtype &ctype, strptr name) {
    name = Pathcomp(name, ".LL");// quote_side.1 -> quote_side
    lib_ctype::FCtype *parent = &ctype;
    lib_ctype::FField *field = NULL;
    while (parent) {
        field = lib_ctype::ind_field_Find(dmmeta::Field_Concat_ctype_name(parent->ctype, name));
        if (field) {
            break;
        }
        lib_ctype::FCtype *base = Basetype(*parent);
        parent = base==parent ? NULL : base;
    }
    return field;
}

// -----------------------------------------------------------------------------

// Check if field is printed as tuple -- i.e. requires recursive parsing
bool lib_ctype::TupleFieldQ(lib_ctype::FField &field) {
    if (bool_Update(field.istuple_computed,true)) {
        field.istuple = field.c_ftuple != NULL
            || (field.p_arg->printfmt == dmmeta_Printfmt_printfmt_Tuple
                && (field.reftype == dmmeta_Reftype_reftype_Val
                    || field.reftype == dmmeta_Reftype_reftype_Opt
                    || field.reftype == dmmeta_Reftype_reftype_Varlen));
    }
    return field.istuple;
}

// -----------------------------------------------------------------------------

// Check if attribute is a tuple.
// This is basically the same as TupleFieldQ, but returns false for
// empty Opt fields.
bool lib_ctype::TupleAttrQ(lib_ctype::FField &field, strptr val) {
    return
        TupleFieldQ(field)
        && !(field.reftype == dmmeta_Reftype_reftype_Opt && !val.n_elems);
}

// -----------------------------------------------------------------------------

static void PrepPrintfmt() {
    ind_beg(lib_ctype::_db_ctype_curs, ctype, lib_ctype::_db) {
        tempstr key(dmmeta::Cfmt_Concat_ctype_strfmt(ctype.ctype, dmmeta_Strfmt_strfmt_String));
        lib_ctype::FCfmt *cfmt = lib_ctype::ind_cfmt_Find(key);
        ctype.printfmt = cfmt ? cfmt->printfmt : algo::Smallstr50(dmmeta_Printfmt_printfmt_Raw);
    }ind_end;
}

// -----------------------------------------------------------------------------

static void PrepFconst() {
    ind_beg(lib_ctype::_db_fconst_curs, fconst, lib_ctype::_db) {
        fconst.key = tempstr() << field_Get(fconst) << "/" << fconst.value;
        if (!lib_ctype::ind_fconst_key_Find(fconst.key)) {
            ind_fconst_key_InsertMaybe(fconst);
            lib_ctype::FField* field = lib_ctype::ind_field_Find(field_Get(fconst));
            if (field) {
                field->has_fconst=true;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Rewrite field with reftype Pkey by replacing Pkey with Val,
// and arg,p_arg with new type
// This happens recursively as long if the target field is also a Pkey
// The default for the resulting field is then taken to be the default of the target type.
static void RewritePkey() {
    ind_beg(lib_ctype::_db_field_curs,field,lib_ctype::_db) {
        if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
            lib_ctype::FField *parentfield = &field;
            lib_ctype::FCtype *arg = parentfield->p_arg;
            while (parentfield->reftype == dmmeta_Reftype_reftype_Pkey && c_field_N(*arg) > 0) {
                parentfield = c_field_Find(*arg, 0);
                arg = parentfield->p_arg;
            }
            field.p_arg   = arg;
            field.arg     = arg->ctype;
            field.reftype = dmmeta_Reftype_reftype_Val;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Prepare main schema
void lib_ctype::Init() {
    RewritePkey();
    PrepFconst();
    PrepPrintfmt();
    lib_ctype::_db.c_bool = ind_ctype_Find("bool");
}

// -----------------------------------------------------------------------------

// Find fconst from field and ssim attr value.
lib_ctype::FFconst *lib_ctype::FindFconst(lib_ctype::FField *field, cstring& attr_value) {
    lib_ctype::FField *fconst_field = field;
    lib_ctype::FFconst* fconst = NULL;
    while (fconst_field) {
        bool ok = fconst_field->reftype==dmmeta_Reftype_reftype_Val;
        ok &= !fconst_field->has_fconst;
        ok &= c_field_N(*fconst_field->p_arg) == 1;
        if (!ok) {
            break;
        }
        fconst_field = lib_ctype::c_field_Find(*fconst_field->p_arg,0);
    }
    if (fconst_field != NULL && fconst_field->has_fconst) {
        tempstr fconstkey;
        fconstkey << fconst_field->field <<"/"<< attr_value;
        fconst = lib_ctype::ind_fconst_Find(fconstkey);
        if ((fconst == NULL) && (fconst_field->arg == "char")) {
            // add single quote
            fconstkey = tempstr() << fconst_field->field << "/'" << attr_value<<"'";
            fconst = lib_ctype::ind_fconst_key_Find(fconstkey);
        }
    }
    return fconst;
}



// Record results of of comparison of EXPECT and RESULT, which is provided in EQUAL.
// If there is a mismatch2, increment match distance and save unmatched result
static bool Match_Value(lib_ctype::Match &match, Attr &expect, Attr *result, bool equal) {
    match.distance += !equal;
    match.nattr ++;
    if (!equal && match.save_unmatched) {
        attr_Add(match.err, "EXPECTED_ATTR", tempstr() << expect);
        attr_Add(match.err, "FOUND_ATTR", tempstr() << (result ? *result : Attr()));
        match.n_unmatched ++;
    }
    return equal;
}


// -----------------------------------------------------------------------------

// Match separator-formatted strings
// Special value NA in expected string allows skipping values in the output.
static void Match_Sep(lib_ctype::Match& match, lib_ctype::FCfmt* sep, Attr &attr_expect, Attr &attr_result) {
    strptr sepstr = sep->sep;
    strptr expstr = attr_expect.value;
    strptr outstr = attr_result.value;
    frep_(i, sepstr.n_elems) {
        strptr val1, val2;
        NextSep(expstr, sepstr.elems[i], val1);
        NextSep(outstr, sepstr.elems[i], val2);
        Match_Value(match, attr_expect, &attr_result, val1 == "NA" || val1 == val2);
    }
    Match_Value(match, attr_expect, &attr_result, expstr == "NA" || expstr == outstr);
}

// -----------------------------------------------------------------------------

// Compare attributes ATTR1 and ATTR2, knowing that they represent values of
// type FIELD. Adjust MATCH accordingly
// ATTR1 is expected attribute. ATTR2 is actual atribute
static void Match_Attr_Printfmt(lib_ctype::Match &match, lib_ctype::FField *field, Attr &attr1, Attr &attr2) {
    if (lib_ctype::TupleFieldQ(*field)) {
        algo::Tuple tuple1, tuple2;
        (void)Tuple_ReadStrptrMaybe(tuple1, attr1.value);
        (void)Tuple_ReadStrptrMaybe(tuple2, attr2.value);
        lib_ctype::Match_Tuple(match, tuple1, tuple2);
    } else if (field->p_arg == lib_ctype::_db.c_bool) { // compare boolean -- TODO GET RID OF ME
        bool b1      = false;
        bool b2      = false;
        bool parseok = bool_ReadStrptrMaybe(b1, attr1.value) && bool_ReadStrptrMaybe(b2, attr2.value);
        Match_Value(match, attr1, &attr2, parseok && b1==b2);
    } else if (field->p_arg->printfmt == dmmeta_Printfmt_printfmt_Sep) {
        Match_Sep(match, *field->p_arg->c_cfmt_elems, attr1, attr2);
    } else {
        // default to simple string comparison
        Match_Value(match, attr1, &attr2, attr1.value == attr2.value);
    }
}

// -----------------------------------------------------------------------------

// Check if EXPECT is a proper subset of RESULT
// Search exits early if match.distance exceeds match.maxdist.
// match.distance is incremented on every mismatched attribute
static void Match_Tuple(lib_ctype::Match &match, Tuple &expect, Tuple &result, lib_ctype::FCtype &ctype) {
    ind_beg(algo::Tuple_attrs_curs, expected_attr, expect) {
        lib_ctype::FField *field = FindField(ctype, expected_attr.name);
        Attr *attr2 = attr_Find(result, expected_attr.name, 0);
        if (field) {
            if (!attr2) {
                Match_Value(match, expected_attr, attr2, false);
            } else {
                Match_Attr_Printfmt(match, field, expected_attr, *attr2);
            }
        } else {
            // unexpected attribute in source tuple -- doesn't correspond to the schema.
            // do verbatim match
            Match_Value(match, expected_attr, attr2, attr2 && expected_attr.value == attr2->value);
        }
        if (match.distance >= match.maxdist) {
            break;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Match Tuple EXPECT against tuple RESULT.
// EXPECT must be a subset of RESULT for the match to pass.
// Results of comparison, mostly represented as a match distance, are saved in MATCH.
// Search exits early if match.distance exceeds match.maxdist.
void lib_ctype::Match_Tuple(lib_ctype::Match &match, Tuple &expect, Tuple &result) {
    if (expect.head == result.head) {
        match.nattr++; // head counts for 1 matched attribute
        if (lib_ctype::FCtype *ctype = lib_ctype::ind_ctype_Find(expect.head.value)) {
            ::Match_Tuple(match, expect, result, *ctype);
        }
    } else {
        match.distance += 1000;// high penalty
        match.nattr++;
    }
}


// -----------------------------------------------------------------------------

// Remove unstable fields from a tuple encoded in STR, print it back and return
// So, if STR is ams.TraceInfo2Msg  tstamp:... field:xxx
// And ams.TraceInfo2Msg.tstamp appears in dmmeta.unstablefld table,
// this attribute will be stripped from the tuple.
tempstr lib_ctype::StabilizeSsimTuple(strptr str) {
    Tuple tuple;
    tempstr out;
    if (Tuple_ReadStrptrMaybe(tuple,str)) {
        lib_ctype::FCtype *ctype = lib_ctype::TagToCtype(tuple);
        if (ctype) {
            out << tuple.head.value;
            ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
                lib_ctype::FField *field = ind_field_Find(dmmeta::Field_Concat_ctype_name(ctype->ctype,attr.name));
                cstring value = field && field->c_unstablefld ? tempstr("***")
                    : field && TupleFieldQ(*field) ?  StabilizeSsimTuple(attr.value)
                    : attr.value;
                PrintAttrSpace(out, attr.name, value);
            }ind_end;
        }
    }
    if (!ch_N(out)) {
        out = str;
    }
    return out;
}

// -----------------------------------------------------------------------------

// Same as tuple version of FillReplscope, but also parses the tuple from STR
void lib_ctype::FillReplscope(algo_lib::Replscope &R, strptr str) {
    Tuple tuple;
    if (Tuple_ReadStrptrMaybe(tuple,str)) {
        FillReplscope(R,tuple);
    }
}

// Fill Replscope with attribute values (including substrings) of tuple
// So, if STR is dev.targdep targdep:a.b comment:"blah"
// Then R will be filled with variables $targdep, $target, $parent, $comment
// This can be used to perform $-substitution in strings.
void lib_ctype::FillReplscope(algo_lib::Replscope &R, algo::Tuple &tuple) {
    lib_ctype::FCtype *ctype = lib_ctype::TagToCtype(tuple);
    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
        Set(R,tempstr()<<"$"<<attr.name,attr.value);
        lib_ctype::FField *srcfield = ctype
            ? ind_field_Find(dmmeta::Field_Concat_ctype_name(ctype->ctype,attr.name))
            : NULL;
        if (srcfield) {
            ind_beg(field_c_substr_srcfield_curs,substr,*srcfield) {
                Set(R,tempstr()<<"$"<<name_Get(*substr.p_field),Pathcomp(attr.value,strptr(substr.expr.value)));
            }ind_end;
        }
    }ind_end;
}
