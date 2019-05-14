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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/arg.cpp -- Parse command-line
//
// Created By: alexei.lebedev mark.page
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev mark.page
//
// Retrieve gitinfo string, e.g.
// dev.gitinfo  gitinfo:2014-10-06.afa3edc.abt  author:alebedev@nyx.com  cfg:g++/release.Linux-x86_64  compver:4.4.6

strptr algo::gitinfo_Get() {
    // remove trailing \n
    return Trimmed("dev.gitinfo  comment:'unversioned'");
}

// -----------------------------------------------------------------------------

// Populate argtuple.vararg, argtuple.proto
// Where each prototype arg has name, type, value, optional.
//
// SYNTAX              source string
// PROTO               out: array of ArgProtos
// S     -> {arg} ['...']
// arg   -> name ':' type ['=' [dflt]]
// arg   -> word
// type  -> word
// dflt  -> word
// word  -> identifier | cpp_string
// If dflt is omitted, it is considered to be an empty string.
// All flags are optional, and default to false unless specified otherwise
// If '...' appears in the string, parsing stops and syntax is marked as var-arg.
// both arrays must be non-zero or one of them will go into the bss section, far away
// from the other one (if not linked with git info)
static void ScanSyntax(algo::Argtuple &argtuple, strptr syntax) {
    StringIter iter(syntax);
    while (!iter.Ws().EofQ()) {
        strptr name   = GetTokenChar(iter, ':');
        if (name == "...") {
            argtuple.vararg = true;
            break;
        }
        ArgProto &arg = proto_Alloc(argtuple);
        arg.name      = name;
        arg.anon    = ch_N(arg.name)>0 && ch_qFind(arg.name, 0) == '[';

        // scan identifier...
        int start=iter.index;
        while (iter.index < elems_N(iter.expr) && algo_lib::ArgvIdentQ(iter.expr[iter.index])) {
            iter.index++;
        }
        arg.type = GetRegion(iter.expr,start,iter.index-start);

        bool      eq  = SkipChar(iter, '=');
        tempstr value;
        if (eq && iter.Peek()!=' ' && !iter.EofQ()) {
            cstring_ReadCmdarg(value, iter, true);
        }
        bool is_flag    = arg.type=="flag";
        bool empty_flag = is_flag && !ch_N(value);
        arg.value       = empty_flag ? "false" : strptr(value);
        arg.optional    = eq || is_flag;
    }
}

// -----------------------------------------------------------------------------

static bool CheckBuiltinFlag(algo::Argtuple &argtuple, strptr name) {
    bool found = false;
    if (name == "-debug" || name == "-d") {
        algo_lib::_db.cmdline.debug++;
        found = true;
    } else if (name == "-help" || name == "-h") {
        argtuple.help = true;
        argtuple.doexit = true;
        found = true;
    } else if (name == "-version" || name == "-ver") {
        argtuple.version = true;
        found = true;
    } else if (name == "-sig" || name == "-signature") {
        argtuple.showsigs = true;
        found = true;
    } else if (name == "-console") {
        argtuple.console = true;
        found = true;
    } else if (name == "-verbose" || name == "-v") {
        algo_lib::_db.cmdline.verbose++;
        found = true;
    }
    return found;
}

// -----------------------------------------------------------------------------

static void DumpAnonAttrs(algo::Argtuple &argtuple) {
    // append to attrs values of all optional attr which weren't
    // specified on the command line.
    // check that every non-optional attr has been provided.
    argtuple.anon_idx = 0;
    ind_beg(Argtuple_proto_curs, arg, argtuple) {
        int this_idx = ch_N(arg.name) == 0 ? argtuple.anon_idx : 0;
        bool   found = attr_Find(argtuple.tuple, arg.name, this_idx) != NULL;
        if (UNLIKELY(!(arg.optional || found))) {
            cstring &text = error_Alloc(argtuple);
            text << arg.type<<" value required for "<<arg.type<<" argument ";
            if (arg.anon) {
                text<<"#"<<this_idx<<" ("<<arg.name<<")";
            } else {
                text<<arg.name;
            }
        }
        argtuple.anon_idx += arg.anon;
        if (!found) {
            attr_Add(argtuple.tuple, arg.name, arg.value);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// scan argtuple and find an argument with matching name.
static algo::ArgProto *FindArgProto(algo::Argtuple &argtuple, strptr name) {
    algo::ArgProto *arg_proto =NULL;
    int arg_index = elems_N(name) ? 0 : argtuple.anon_idx;
    ind_beg(algo::Argtuple_proto_curs, arg, argtuple) {
        bool match = arg.name == name;
        if (arg.anon) {
            match |= name == strptr()
                || name == (tempstr() << "-" << ch_GetRegion(arg.name, 1, ch_N(arg.name)-2));
        }
        if (match) {
            --arg_index;
        }
        if (match && arg_index < 0) {
            arg_proto = &arg;
            break;
        }
    }ind_end;
    return arg_proto;
}

// -----------------------------------------------------------------------------

// Perform actions scheduled during scan
// (print version, show help screen, exit program)
static void FinalActions(algo::Argtuple &argtuple, strptr help_string) {
    if (argtuple.showsigs) {
        ind_beg(algo_lib::_db_dispsigcheck_curs,dispsigcheck,algo_lib::_db) {
            // dispsig goes to stdout
            dmmeta::Dispsigcheck out;
            dispsigcheck_CopyOut(dispsigcheck,out);
            prlog(out);
        }ind_end;
        argtuple.doexit = true;
    }

    if (argtuple.version) {
        // -ver output goes to stdout
        prlog(algo::gitinfo_Get());
        argtuple.doexit = true;
    }

    if (argtuple.help) {
        prerr("\n\n"<<help_string);
        argtuple.doexit = true;
    }

    if (error_N(argtuple)) {
        prerr("");
        argtuple.exitcode = 1;
        ind_beg(algo::Argtuple_error_curs, error, argtuple) {
            prerr(error);
        }ind_end;
        argtuple.help = true;
        argtuple.doexit = true;
    }

    if (argtuple.doexit) {
        _exit(argtuple.exitcode);
    }
}

// -----------------------------------------------------------------------------

static int ReadOption(algo::Argtuple &argtuple, int i, int argc, char **argv) {
    strptr option = argv[i];
    bool dashdash       = elems_N(option) >= 2 && option[0]=='-' && option[1]=='-';
    option              = RestFrom(option,dashdash ? 1 : 0); // convert -- to -
    // note, single - is not an option
    bool isopt          = !argtuple.endopt && elems_N(option)>1 && option[0]=='-';
    i32_Range  R        = isopt ? TFind(option,':') : i32_Range(0,0);
    strptr name         = FirstN(option, R.beg);
    strptr value        = RestFrom(option, R.end);
    ArgProto *arg_proto = FindArgProto(argtuple,name);

    bool need_arg       = arg_proto && R.beg>0 && R.beg == elems_N(option);
    if (need_arg) {
        bool read_arg   = arg_proto->type != "flag";
        i              += read_arg;
        if (UNLIKELY(!(i < argc))) {
            error_Alloc(argtuple) << arg_proto->type<<" value required for option "<<Keyval("",name);
        }
        value           = read_arg ? argv[i] : "Y";
    }

    // check built-in options
    bool found = arg_proto != NULL;
    if (found) {
        argtuple.anon_idx += !elems_N(name);
    } else {
        found = CheckBuiltinFlag(argtuple,name);
    }
    if (!found && !argtuple.vararg) {
        if (!elems_N(name)) {
            error_Alloc(argtuple) << argv[0]<<".cmdline  too many arguments. error at "<<Keyval("",argv[i]);
        } else {
            error_Alloc(argtuple) << argv[0]<<".cmdline  unknown option "<<Keyval("",name);
        }
    }
    strptr eff_name    = arg_proto ? strptr(arg_proto->name) : name;
    attr_Add(argtuple.tuple, eff_name, value);
    return i;
}

// -----------------------------------------------------------------------------

// Scan argc/argv array, validate provided options,
// and populate argtuple.attrs -- this is the Tuple that
// will be ultimately scanned by <somecommand>_ReadStrptrMaybe
// to read the command line
// both -a:b and -a b syntaxes are supported
// In the syntax string, [x]:y means x is anonymous
// if there is no value, and proto-arg has default value, it is taken
// for a flag, -a alone means value of 'true'.
// There is an ambiguity here,
// in that command line '-a true' will be interpreted as flag '-a' followed by
// positional arg with value 'true'.
void algo::Argtuple_ReadArgv(algo::Argtuple &argtuple, int argc, char **argv, strptr syntax, strptr help_string) {
    ScanSyntax(argtuple, syntax);
    frep1_(i,argc) {
        strptr option       = argv[i];
        if (option == "--") {
            argtuple.endopt = true;
        } else {
            i=ReadOption(argtuple,i,argc,argv);
        }
    }

    DumpAnonAttrs(argtuple);

    ind_beg(algo::Tuple_attrs_curs, attr, argtuple.tuple) {
        strptr name = attr.name;
        if (elems_N(name) && name[0]=='[') name=strptr();
        if (elems_N(name) && name[0]=='-') name=RestFrom(name,1);
        attr.name = tempstr() << name;
    }ind_end;

    FinalActions(argtuple, help_string);
}
