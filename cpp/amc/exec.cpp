// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/exec.cpp -- Exec reftype
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static tempstr DefaultExecPath(amc::FField &field) {
    tempstr ret;
    if (field.dflt.value != "") {
        ret = field.dflt.value;
    } else {
        ret << "\"bin/"<<name_Get(*field.p_arg)<<"\"";
    }
    return ret;
}

// -----------------------------------------------------------------------------

static tempstr SubfieldName(amc::FField &field, strptr name) {
    tempstr ret;
    if (field.c_anonfld) {
        ret << ctype_Get(field) << "." << name;
    } else {
        ret << field.field << "_" << name;
    }
    return ret;
}

// -----------------------------------------------------------------------------

void amc::tclass_Exec() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    Set(R, "$cmdname", name_Get(*field.p_arg));
    // allow access to custom fields using $_<fieldname>
    Set(R, "$_", field.c_anonfld ? "$parname." : "$parname.$name_");
}

void amc::tfunc_Exec_Init() {
}

void amc::tfunc_Exec_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& uninit = amc::CreateCurFunc();
    Ins(&R, uninit.body, "$name_Kill($pararg); // kill child, ensure forward progress");
}

void amc::tfunc_Exec_Wait() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& wait = amc::CreateCurFunc();
    Ins(&R, wait.ret  , "void",false);
    Ins(&R, wait.proto, "$name_Wait($Parent)",false);
    // close the stdin pipe first so the child sees EOF and can exit
    Ins(&R, wait.body, "algo_lib::Close($_to_stdin);");
    Ins(&R, wait.body, "if ($_pid > 0) {");
    Ins(&R, wait.body, "    int wait_flags = 0;");
    Ins(&R, wait.body, "    int wait_status = 0;");
    Ins(&R, wait.body, "    int rc = -1;");
    Ins(&R, wait.body, "    do {");
    Ins(&R, wait.body, "        // really wait for subprocess to exit");
    Ins(&R, wait.body, "        rc = waitpid($_pid,&wait_status,wait_flags);");
    Ins(&R, wait.body, "    } while (rc==-1 && errno==EINTR);");
    Ins(&R, wait.body, "    if (rc == $_pid) {");
    Ins(&R, wait.body, "        $_status = wait_status;");
    Ins(&R, wait.body, "        $_pid = 0;");
    Ins(&R, wait.body, "    }");
    Ins(&R, wait.body, "}");
    // close the read ends; caller must drain them before _Wait to avoid deadlock
    Ins(&R, wait.body, "algo_lib::Close($_from_stdout);");
    Ins(&R, wait.body, "algo_lib::Close($_from_stderr);");
}


void amc::tfunc_Exec_Kill() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& kill = amc::CreateCurFunc();
    Ins(&R, kill.ret  , "void",false);
    Ins(&R, kill.proto, "$name_Kill($Parent)",false);
    Ins(&R, kill.body, "if ($_pid > 0) {");
    Ins(&R, kill.body, "    kill($_pgroup ? -$_pid : $_pid,9); // pgroup child dies as a whole group");
    Ins(&R, kill.body, "    $name_Wait($pararg);");
    Ins(&R, kill.body, "}");
}

void amc::tfunc_Exec_Start() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    (void)field;

    amc::FFunc& start = amc::CreateCurFunc();
    Ins(&R, start.comment, "If subprocess already running, do nothing. Otherwise, start it");
    Ins(&R, start.ret  , "int",false);
    Ins(&R, start.proto, "$name_Start($Parent)",false);
    Ins(&R, start.body, "int retval = 0;");
    Ins(&R, start.body, "if ($_pid == 0) {");
    Ins(&R, start.body, "    verblog($name_ToCmdline($pararg)); // maybe print command");
    // todo: use posix_spawn instead of this mess and
    // implement a portable posix spawn for both windows and unix
    Ins(&R, start.body, "#ifdef WIN32");
    Ins(&R, start.body, "    algo_lib::ResolveExecFname($_path);");
    Ins(&R, start.body, "    tempstr cmdline($name_ToCmdline($pararg));");
    Ins(&R, start.body, "    $_pid = dospawn(Zeroterm($_path),Zeroterm(cmdline),$_timeout,$_fstdin,$_fstdout,$_fstderr);");
    Ins(&R, start.body, "#else");
    // Create up to 3 pipes before fork (parent keeps one end, child gets the other).
    // To merge stderr into stdout, set fstdout="|" and fstderr=">&1": stdout is
    // applied before stderr below, so >&1 duplicates the already-redirected pipe.
    Ins(&R, start.body, "    int in_pipe[2]  = {-1,-1}; // [0]=child stdin (read), [1]=$_to_stdin (write)");
    Ins(&R, start.body, "    int out_pipe[2] = {-1,-1}; // [0]=$_from_stdout (read), [1]=child stdout (write)");
    Ins(&R, start.body, "    int err_pipe[2] = {-1,-1}; // [0]=$_from_stderr (read), [1]=child stderr (write)");
    Ins(&R, start.body, "    if ($_fstdin  == \"|\" && pipe(in_pipe)  == 0) { $_to_stdin.value    = in_pipe[1];  }");
    Ins(&R, start.body, "    if ($_fstdout == \"|\" && pipe(out_pipe) == 0) { $_from_stdout.value = out_pipe[0]; }");
    Ins(&R, start.body, "    if ($_fstderr == \"|\" && pipe(err_pipe) == 0) { $_from_stderr.value = err_pipe[0]; }");
    Ins(&R, start.body, "    $_pid = fork();");
    Ins(&R, start.body, "    if ($_pid == 0) { // child");
    Ins(&R, start.body, "        algo_lib::DieWithParent();");
    Ins(&R, start.body, "        // inherited signal handlers stay live until exec, so a kill aimed at");
    Ins(&R, start.body, "        // the child in the fork-to-exec window would run the parent's handler");
    Ins(&R, start.body, "        // in the child and be consumed instead of killing; restore the default");
    Ins(&R, start.body, "        // dispositions so the signal does what the sender means");
    Ins(&R, start.body, "        (void)signal(SIGTERM, SIG_DFL);");
    Ins(&R, start.body, "        (void)signal(SIGINT , SIG_DFL);");
    Ins(&R, start.body, "        (void)signal(SIGHUP , SIG_DFL);");
    Ins(&R, start.body, "        (void)signal(SIGQUIT, SIG_DFL);");
    Ins(&R, start.body, "        (void)signal(SIGALRM, SIG_DFL);");
    Ins(&R, start.body, "        if ($_pgroup) {");
    Ins(&R, start.body, "            // own process group: a kill by the child's pid alone would");
    Ins(&R, start.body, "            // orphan its descendants alive; the group is one killable unit");
    Ins(&R, start.body, "            (void)setpgid(0, 0);");
    Ins(&R, start.body, "        }");
    Ins(&R, start.body, "        if ($_timeout > 0) {");
    Ins(&R, start.body, "            alarm($_timeout);");
    Ins(&R, start.body, "        }");
    Ins(&R, start.body, "        if ($_memlimitmb > 0) {");
    Ins(&R, start.body, "            // memory ceiling: soft and hard, so a child that drops");
    Ins(&R, start.body, "            // privileges cannot raise it; the child sees allocation");
    Ins(&R, start.body, "            // failure at the limit instead of inviting the OOM killer");
    Ins(&R, start.body, "            struct rlimit rlim;");
    Ins(&R, start.body, "            rlim.rlim_cur = rlim_t($_memlimitmb) * 1000000;");
    Ins(&R, start.body, "            rlim.rlim_max = rlim.rlim_cur;");
    Ins(&R, start.body, "            (void)setrlimit(RLIMIT_AS, &rlim);");
    Ins(&R, start.body, "        }");
    // todo: do something smart with ApplyRedirect failures other than cause exec failure?
    Ins(&R, start.body, "        if (retval==0) retval=algo_lib::ApplyRedirect($_fstdin , 0, in_pipe[0]);");
    Ins(&R, start.body, "        if (retval==0) retval=algo_lib::ApplyRedirect($_fstdout, 1, out_pipe[1]);");
    Ins(&R, start.body, "        if (retval==0) retval=algo_lib::ApplyRedirect($_fstderr, 2, err_pipe[1]);");
    // close every pipe fd in the child; fds 0/1/2 already alias the right ends.
    // out_pipe[1] may back both fd 1 and fd 2 -- dup2 copied it, so closing it is safe.
    Ins(&R, start.body, "        if (in_pipe[0]  >= 0) (void)close(in_pipe[0]);");
    Ins(&R, start.body, "        if (in_pipe[1]  >= 0) (void)close(in_pipe[1]);");
    Ins(&R, start.body, "        if (out_pipe[0] >= 0) (void)close(out_pipe[0]);");
    Ins(&R, start.body, "        if (out_pipe[1] >= 0) (void)close(out_pipe[1]);");
    Ins(&R, start.body, "        if (err_pipe[0] >= 0) (void)close(err_pipe[0]);");
    Ins(&R, start.body, "        if (err_pipe[1] >= 0) (void)close(err_pipe[1]);");
    Ins(&R, start.body, "        if (retval==0) retval= $name_Execv($pararg);");
    Ins(&R, start.body, "        if (retval != 0) { // if start fails, print error");
    Ins(&R, start.body, "            int err=errno;");
    Ins(&R, start.body, "            prerr(\"$ns.$name_execv\"");
    Ins(&R, start.body, "                  <<Keyval(\"errno\",err)");
    Ins(&R, start.body, "                  <<Keyval(\"errstr\",strerror(err))");
    Ins(&R, start.body, "                  <<Keyval(\"comment\",\"Execv failed\"));");
    Ins(&R, start.body, "        }");
    Ins(&R, start.body, "        _exit(127); // if failed to start, exit anyway");
    Ins(&R, start.body, "    } else if ($_pid == -1) {");
    Ins(&R, start.body, "        retval = errno; // failed to fork");
    Ins(&R, start.body, "    } else if ($_pgroup) {");
    Ins(&R, start.body, "        // mirror the child's setpgid: the group must exist the moment fork");
    Ins(&R, start.body, "        // returns, or a group kill racing the child's first quantum finds no");
    Ins(&R, start.body, "        // group, loses the signal, and the unkilled child boots into whatever");
    Ins(&R, start.body, "        // the killer already tore down.  EACCES -- the child exec'd first, its");
    Ins(&R, start.body, "        // own setpgid won -- is the benign side of the race.");
    Ins(&R, start.body, "        (void)setpgid($_pid, $_pid);");
    Ins(&R, start.body, "    }");
    // parent: close the child-side ends so we hold only our end of each pipe
    Ins(&R, start.body, "    if (in_pipe[0]  >= 0) (void)close(in_pipe[0]);  // parent keeps write end (to_stdin)");
    Ins(&R, start.body, "    if (out_pipe[1] >= 0) (void)close(out_pipe[1]); // parent keeps read end (from_stdout)");
    Ins(&R, start.body, "    if (err_pipe[1] >= 0) (void)close(err_pipe[1]); // parent keeps read end (from_stderr)");
    Ins(&R, start.body, "#endif");
    Ins(&R, start.body, "}");
    Ins(&R, start.body, "$_status = $_pid > 0 ? 0 : -1; // if didn't start, set error status");
    Ins(&R, start.body , "return retval;");
}

void amc::tfunc_Exec_Exec() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& exec = amc::CreateCurFunc();
    Ins(&R, exec.comment, "Execute subprocess and return its wait() status; decode with algo::WaitStatusToExitCode");
    Ins(&R, exec.ret  , "int",false);
    Ins(&R, exec.proto, "$name_Exec($Parent)",false);
    Ins(&R, exec.body, "$name_Start($pararg);");
    Ins(&R, exec.body, "$name_Wait($pararg);");
    Ins(&R, exec.body, "return $_status;");
}

void amc::tfunc_Exec_ExecX() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& exec = amc::CreateCurFunc();
    Ins(&R, exec.comment, "Execute subprocess; throw human-readable exception on error");
    Ins(&R, exec.ret  , "void",false);
    Ins(&R, exec.proto, "$name_ExecX($Parent)",false);
    Ins(&R, exec.body, "int rc = $name_Exec($pararg);");
    Ins(&R, exec.body, "vrfy(rc==0, tempstr() << \"algo_lib.exec\" << Keyval(\"cmd\",$name_ToCmdline($pararg))"
        "\n    << Keyval(\"comment\",algo::DescribeWaitStatus($_status)));");
}

void amc::tfunc_Exec_ToCmdline() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &execfield = *amc::_db.genctx.p_field;
    if (!HasArgvPrintQ(*execfield.p_arg)) {
        prlog("amc.noargv"
              <<Keyval("exec_field",execfield.field)
              <<Keyval("ctype_arg",execfield.arg)
              <<Keyval("comment","ctype needs to have a cfmt with strfmt:Argv and print:Y"));
        algo_lib::_db.exit_code=1;
    }
    amc::FFunc& tocmdline = amc::CreateCurFunc();
    Ins(&R, tocmdline.ret  , "algo::tempstr",false);
    Ins(&R, tocmdline.proto, "$name_ToCmdline($Parent)",false);
    Ins(&R, tocmdline.body, "algo::tempstr retval;");
    Ins(&R, tocmdline.body, "retval << $_path << \" \";");
    Ins(&R, tocmdline.body, "command::$cmdname_PrintArgv($_cmd,retval);");
    // Only show file redirects; "|" (pipe) and "<&N"/">&N" (fd dup) are internal
    // plumbing, not a useful part of the displayed command line.
    Ins(&R, tocmdline.body, "if (algo_lib::RedirectFileQ($_fstdin)) {");
    Ins(&R, tocmdline.body, "    retval << \" \" << $_fstdin;");
    Ins(&R, tocmdline.body, "}");
    Ins(&R, tocmdline.body, "if (algo_lib::RedirectFileQ($_fstdout)) {");
    Ins(&R, tocmdline.body, "    retval << \" \" << $_fstdout;");
    Ins(&R, tocmdline.body, "}");
    Ins(&R, tocmdline.body, "if (algo_lib::RedirectFileQ($_fstderr)) {");
    Ins(&R, tocmdline.body, "    retval << \" 2\" << $_fstderr;");
    Ins(&R, tocmdline.body, "}");
    Ins(&R, tocmdline.body, "return retval;");
}

void amc::tfunc_Exec_ToArgv() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FCtype &cmdtype = *amc::_db.genctx.p_field->p_arg;
    // True iff the wrapped command struct has a ccmdline record, meaning the
    // target binary is amc-generated and its ReadArgv understands -name:value.
    // Otherwise fall back to two-token -name value, which any conventional CLI parses.
    bool amc_command = cmdtype.c_ccmdline != NULL;
    amc::FFunc& func = CreateCurFunc(true);
    AddRetval(func, "void", "", "");
    AddProtoArg(func, "algo::StringAry&", "args", "");
    Set(R,"$cmdtypens",ns_Get(cmdtype));
    Ins(&R, func.comment, "Form array from the command line");
    Ins(&R, func.body, "ary_RemoveAll(args);");
    Ins(&R, func.body, "ary_Alloc(args) << $_path;");
    ind_beg(amc::ctype_c_field_curs, field, cmdtype) if (!FldfuncQ(field)) {
        Set(R, "$fldname", name_Get(field));
        if (field.reftype == dmmeta_Reftype_reftype_Tary) {
            Set(R, "$Ftype", name_Get(*field.p_arg));
            Ins(&R, func.body, "ind_beg($cmdtypens::$name_$fldname_curs,value,$_cmd) {");
            if (amc_command) {
                // amc command -- single argument contains key & value
                Ins(&R, func.body,"    cstring *arg = &ary_Alloc(args);");
                Ins(&R, func.body,"    *arg << \"-$fldname:\";");
            } else {
                // non-amc command -- can't use key-value format. 2 arguments needed
                Ins(&R, func.body,"    ary_Alloc(args) << \"-$fldname\";");
                Ins(&R, func.body,"    cstring *arg = &ary_Alloc(args);");
            }
            Set(R, "$Ftype", name_Get(*field.p_arg));
            Ins(&R, func.body,"    $Ftype_Print(value, *arg);");
            Ins(&R, func.body, "}ind_end;");
        } else {
            if (ch_N(field.dflt.value) > 0) {
                Set(R, "$dflt", field.dflt.value);
                if (field.reftype==dmmeta_Reftype_reftype_Regx) {
                    Set(R, "$chkdflt", "$_cmd.$fldname.expr != $dflt");
                } else {
                    Set(R, "$chkdflt", "$_cmd.$fldname != $dflt");
                }
            } else {
                Set(R, "$chkdflt", "true");
            }
            Ins(&R, func.body, "");
            Ins(&R, func.body, "if ($chkdflt) {");
            if (amc_command) {
                // amc command -- single argument contains key & value
                Ins(&R, func.body,"    cstring *arg = &ary_Alloc(args);");
                Ins(&R, func.body,"    *arg << \"-$fldname:\";");
            } else {
                // non-amc command -- can't use key-value format. 2 arguments needed
                Ins(&R, func.body,"    ary_Alloc(args) << \"-$fldname\";");
                Ins(&R, func.body,"    cstring *arg = &ary_Alloc(args);");
            }
            // #AL# this is a hack -- need a field print function here.
            amc::FFunc* field_print = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"));
            bool has_field_print = field_print != NULL;
            has_field_print |= field.reftype == dmmeta_Reftype_reftype_Regx;
            if (has_field_print) {
                Set(R, "$fns", ns_Get(*field.p_ctype));
                Ins(&R, func.body, "    $fns::$fldname_Print($_cmd, *arg);");
            } else {
                Set(R, "$Ftype", name_Get(*field.p_arg));
                Ins(&R, func.body,"    $Ftype_Print($_cmd.$fldname, *arg);");
            }
            Ins(&R, func.body,"}");
        }
    }ind_end;

    if (amc_command) {
        // add verbose, debug flags -- one fewer than current process
        Ins(&R, func.body,"for (int i=1; i < algo_lib::_db.cmdline.verbose; ++i) {");
        Ins(&R, func.body,"    ary_Alloc(args) << \"-verbose\";");
        Ins(&R, func.body,"}");
        Ins(&R, func.body,"for (int i=1; i < algo_lib::_db.cmdline.debug; ++i) {");
        Ins(&R, func.body,"    ary_Alloc(args) << \"-debug\";");
        Ins(&R, func.body,"}");
    }
}

void amc::tfunc_Exec_Execv() {
    // form args array
    amc::FFunc& execv = CreateCurFunc(true);
    algo_lib::Replscope &R = amc::_db.genctx.R;
    AddRetval(execv, "int", "ret", "0");
    Ins(&R, execv.comment, "Call execv with specified parameters");
    Ins(&R, execv.body,"algo::StringAry args;");
    Ins(&R, execv.body,"$name_ToArgv($pararg, args);");
    // form argv
    Ins(&R, execv.body,"char **argv = (char**)alloca((ary_N(args)+1)*sizeof(*argv));");
    Ins(&R, execv.body,"ind_beg(algo::StringAry_ary_curs,arg,args) {");
    Ins(&R, execv.body,"    argv[ind_curs(arg).index] = Zeroterm(arg);");
    Ins(&R, execv.body,"}ind_end;");
    Ins(&R, execv.body,"argv[ary_N(args)] = NULL;");

    Ins(&R, execv.body,"// if $_path is relative, search for it in PATH");
    Ins(&R, execv.body,"algo_lib::ResolveExecFname($_path);");
    Ins(&R, execv.body,"ret = execv(Zeroterm($_path),argv);");
}

// -----------------------------------------------------------------------------

void amc::NewFieldExec() {
    ind_beg(amc::_db_field_curs, field, amc::_db) if (field.reftype == dmmeta_Reftype_reftype_Exec) {
        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "path")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr(DefaultExecPath(field))
                                            , algo::Comment("path for executable")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "cmd")
                                            , field.arg
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("command line for child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "fstdin")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("redirect for stdin")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "fstdout")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("redirect for stdout")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "fstderr")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("redirect for stderr")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "to_stdin")
                                            , "algo.Fildes"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("write end of stdin pipe when fstdin==\"|\"; closed by _Wait")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "from_stdout")
                                            , "algo.Fildes"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("read end of stdout pipe when fstdout==\"|\"; closed by _Wait")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "from_stderr")
                                            , "algo.Fildes"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("read end of stderr pipe when fstderr==\"|\"; closed by _Wait")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "pid")
                                            , "pid_t"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("pid of running child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "timeout")
                                            , "i32"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr("0")
                                            , algo::Comment("optional timeout for child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "memlimitmb")
                                            , "u32"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr("0")
                                            , algo::Comment("optional child memory ceiling MB (10^6): RLIMIT_AS before exec; 0 = leave inherited")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "status")
                                            , "i32"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("last exit status of child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "pgroup")
                                            , "bool"
                                            , dmmeta_Reftype_reftype_Val
                                            , algo::CppExpr()
                                            , algo::Comment("run child in its own process group; _Kill targets the group")));
    }ind_end;
}
