// Copyright (C) 2026 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/cmdline.cpp
//

#include "include/algo.h"
#include "include/amc.h"

// -----------------------------------------------------------------------------

// Pick a field to extract enums from.
// Handle the case of a single-field ctype with enums in it
static amc::FField *GetEnumField(amc::FField &field) {
    return c_field_N(*field.p_arg)==1
        && c_fconst_N(*c_field_Find(*field.p_arg,0))
        ? c_field_Find(*field.p_arg,0)
        : &field;
}

// -----------------------------------------------------------------------------

static tempstr CmdargName(amc::FField *fld) {
    tempstr ret;
    algo::Smallstr100 name = name_Get(*fld);
    if (fld->c_anonfld) ret << "[";
    ret << name;
    if (fld->c_anonfld) ret << "]";
    if (fld->reftype == dmmeta_Reftype_reftype_Tary) {
        ret << "...";
    }
    return ret;
}

// -----------------------------------------------------------------------------

static tempstr GetCmdArgType(amc::FField& field) {
    tempstr ret("string");// default
    amc::FArgvtype *argvtype = field.p_arg->c_argvtype;
    // amc rewrites all pkey fields so they become Vals
    // so this first line can never execute
    if (c_fconst_N(*GetEnumField(field))) {
        ret = "enum";
    } else if (field.c_fflag) {
        ret = "flag";
    } else if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
        ret = "pkey";
    } else if (field.reftype == dmmeta_Reftype_reftype_Regx) {
        ret = "regx";
    } else {
        if (!argvtype && c_field_N(*field.p_arg) == 1) {
            argvtype = c_field_Find(*field.p_arg,0)->p_ctype->c_argvtype;
        }
        if (argvtype) {
            ret=argvtype->argvtype;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return TRUE if FIELD (in command line context) requires no argument
// This is true for bool fields or fields with "emptyval" provided
bool amc::CmdArgValueRequiredQ(amc::FField &field) {
    return !(field.p_arg->ctype == "bool" || (field.c_fflag && field.c_fflag->emptyval != "\"\""));
}

// -----------------------------------------------------------------------------

// True if field is a required command-line argument
bool amc::CmdArgRequiredQ(amc::FField &field) {
    return field.dflt.value=="" // no default provided...
        && !(field.arg == "algo.UnTime" || field.arg == "algo.UnDiff") // these can't be mandatory
        && !field.c_tary // not an array
        && !c_fconst_N(*GetEnumField(field)) // not an enum (these are always initialized)
        && CmdArgValueRequiredQ(field); // does require an arg
}

// -----------------------------------------------------------------------------

// Adjust displayed default
// Translate true/false into "Y"/<empty string>
static tempstr GetCmdArgDflt(amc::FField &field) {
    tempstr ret(field.dflt.value);
    if (field.c_fflag) {
        // no default for flags (even if they can take a value)
        ret="";
    } else if (field.c_tary) {
        // no default for arrays -- since they can be empty
        ret="";
    } else if (field.arg=="bool") {
        if (field.dflt.value=="true") {
            ret="Y";
        } else if (field.dflt.value=="false") {
            ret="";// do not show
        }
    } else {
        // scan enums for the field and translate back from the
        // numeric value to the symbolic.
        // an empty dflt on the command field means the member's own default
        // applies: for a wrapper ctype (single value field with fconsts) that
        // is the value field's dflt, which need not be zero -- a state enum
        // defaults to 255/unknown, and assuming zero would display the value-0
        // fconst as the default.  the "" alias fconst is for reading only and
        // never names the displayed default.
        amc::FField *enumfield = GetEnumField(field);
        tempstr dflt(field.dflt.value);
        if (!ch_N(dflt) && enumfield != &field) {
            dflt = enumfield->dflt.value;
        }
        tempstr enum_dflt;
        ind_beg(amc::field_c_fconst_curs,fconst,*enumfield) {
            if ((fconst.value.value == dflt
                 || (!ch_N(dflt) && (fconst.value.value=="0" || fconst.value.value=="'\\0'")))
                && name_Get(fconst) != "\"\"") {
                enum_dflt = name_Get(fconst);
            }
        }ind_end;
        if (ch_N(enum_dflt)) {
            ret = enum_dflt;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Generate a usage string for CTYPE, treating it as a command line
// description.
// Example: Usage: dmsess [sess] [options]
static tempstr GenUsageString(amc::FCtype &ctype, strptr comment) {
    tempstr cmd_format(name_Get(ctype));
    bool has_opts = false;
    ind_beg(amc::ctype_c_field_curs, field, ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Base) {
            // already expanded into fields
        } else if (field.c_falias) {
            // ignore
        } else if (CmdArgRequiredQ(field)) {
            if (CmdArgValueRequiredQ(field)) {
                if (field.c_anonfld) {
                    // [-str:]<string>   -- CmdArgRequired, anon, value required
                    cmd_format << " [-"<<name_Get(field)<<":]<"<<GetCmdArgType(field)<<">";
                } else {
                    // -astr:<string>    -- CmdArgRequired, named, value required
                    cmd_format << " -"<<name_Get(field)<<":<"<<GetCmdArgType(field)<<">";
                }
            }
        } else {
            if (CmdArgValueRequiredQ(field)) {
                if (field.c_anonfld) {
                    // [[-str:]<string>] -- !CmdArgRequired, anon, value required
                    cmd_format << " [[-"<<name_Get(field)<<":]<"<<GetCmdArgType(field)<<">]";
                } else {
                    //                   -- !CmdArgRequired, named, value not required -- skipped
                    has_opts = true;
                }
            }
        }
    }ind_end;
    if (has_opts) {
        cmd_format << " [options]";
    }
    tempstr ret;
    if (ch_N(comment)) {
        ret << name_Get(ctype) << ": "<<comment<<eol;
    }
    ret << "Usage: "<< cmd_format<<eol;
    return ret;
}

// -----------------------------------------------------------------------------

// Generate help string for CTYPE, treating it as command line description.
// If BASE is provided, it is the base command line which is included as well.
// Generate help strings for command-line tools.
//
// Example Output:
// Usage: dmsess [sess] [options]
//   sess          session name
//   -list         list processes in session
//   -cfg:string   (with -create) debug or release. default: debug
algo::tempstr amc::GenHelpString(amc::FCtype &ctype, amc::FCtype *base, strptr comment) {
    tempstr name(name_Get(ctype));
    tempstr table;
    tempstr usage = GenUsageString(ctype, comment);
    table << "    OPTION\tTYPE\tDFLT\tCOMMENT\n";
    // loop over fields of this command line, and its base command line
    int niter = base==NULL ? 1:2;
    for (int i=0; i<niter; i++) {
        amc::FCtype *thisctype=i==0 ? &ctype : base;
        ind_beg(amc::ctype_c_field_curs, field, *thisctype) {
            if (field.c_falias) {
                // don't show help
            } else if (field.reftype == dmmeta_Reftype_reftype_Base) {
            } else {
                table << "    ";
                table << (field.c_anonfld ? "" : "-");
                table << CmdargName(&field);
                table << "\t";
                table << GetCmdArgType(field);
                table << "\t";
                table << GetCmdArgDflt(field);
                table << "\t";
                table << field.comment;
                // list aliases
                ind_beg(amc::ctype_c_field_curs, aliasfield, *thisctype) {
                    if (aliasfield.c_falias && aliasfield.c_falias->p_srcfield == &field) {
                        table << "; alias -"<<name_Get(aliasfield);
                    }
                }ind_end;
                // explain cumulative flag
                if (field.c_fflag && field.c_fflag->cumulative) {
                    table << "; cumulative";
                }
                // enum values rendered inline, pipe-separated
                tempstr fconst_choice;
                algo::ListSep ls("|");
                ind_beg(amc::field_c_fconst_curs,fconst,*GetEnumField(field)) {
                    fconst_choice << ls << name_Get(fconst);
                }ind_end;
                if (ch_N(fconst_choice)) {
                    table << " (" << fconst_choice << ")";
                }
                table << eol;
            }
        }ind_end;
    }
    usage << Tabulated(table, "\t", "lll",2);
    return usage;
}
