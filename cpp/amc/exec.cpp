// (C) AlgoEngineering LLC 2008-2012
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/exec.cpp -- reftype Exec
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    Set(R, "$cmdname", name_Get(*field.p_arg));
    // allow access to custom fields using $_<fieldname>
    Set(R, "$_", field.c_anonfld ? "$parname." : "$parname.$name_");
}

void amc::tfunc_Exec_Init() {
}

void amc::tfunc_Exec_Uninit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& uninit = amc::CreateCurFunc();
    Ins(&R, uninit.body, "$name_Kill($pararg); // kill child, ensure forward progress");
}

void amc::tfunc_Exec_Wait() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& wait = amc::CreateCurFunc();
    Ins(&R, wait.ret  , "void",false);
    Ins(&R, wait.proto, "$name_Wait($Parent)",false);
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
}


void amc::tfunc_Exec_Kill() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& kill = amc::CreateCurFunc();
    Ins(&R, kill.ret  , "void",false);
    Ins(&R, kill.proto, "$name_Kill($Parent)",false);
    Ins(&R, kill.body, "if ($_pid != 0) {");
    Ins(&R, kill.body, "    kill($_pid,9);");
    Ins(&R, kill.body, "    $name_Wait($pararg);");
    Ins(&R, kill.body, "}");
}

void amc::tfunc_Exec_Start() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
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
    Ins(&R, start.body, "    $_pid = fork();");
    Ins(&R, start.body, "    if ($_pid == 0) { // child");
    Ins(&R, start.body, "        algo_lib::DieWithParent();");
    Ins(&R, start.body, "        if ($_timeout > 0) {");
    Ins(&R, start.body, "            alarm($_timeout);");
    Ins(&R, start.body, "        }");
    // todo: do something smart with ApplyRedirect failures other than cause exec failure?
    Ins(&R, start.body, "        if (retval==0) retval=algo_lib::ApplyRedirect($_fstdin , 0);");
    Ins(&R, start.body, "        if (retval==0) retval=algo_lib::ApplyRedirect($_fstdout, 1);");
    Ins(&R, start.body, "        if (retval==0) retval=algo_lib::ApplyRedirect($_fstderr, 2);");
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
    Ins(&R, start.body, "    }");
    Ins(&R, start.body, "#endif");
    Ins(&R, start.body, "}");
    Ins(&R, start.body, "$_status = $_pid > 0 ? 0 : -1; // if didn't start, set error status");
    Ins(&R, start.body , "return retval;");
}

// // Should this be generated?
// static algo::Fildes bash_StartRead(command::bash_proc &bash, algo_lib::FFildes &read) {
//     int pipefd[2];
//     int rc=pipe(pipefd);
//     (void)rc;
//     read.fd.value = pipefd[0];
//     bash.fstdout  << ">&" << pipefd[1];
//     bash_Start(bash);
//     (void)close(pipefd[1]);
//     return read.fd;
// }
void amc::tfunc_Exec_StartRead() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& exec = amc::CreateCurFunc(true);
    exec.ret = "algo::Fildes";
    AddArg(exec.proto, "algo_lib::FFildes &read");
    Ins(&R, exec.body, "int pipefd[2];");
    Ins(&R, exec.body, "int rc=pipe(pipefd);");
    Ins(&R, exec.body, "(void)rc;");
    Ins(&R, exec.body, "read.fd.value = pipefd[0];");
    Ins(&R, exec.body, "$_fstdout  << \">&\" << pipefd[1];");
    Ins(&R, exec.body, "$name_Start($pararg);");
    Ins(&R, exec.body, "(void)close(pipefd[1]);");
    Ins(&R, exec.body, "return read.fd;");
}

void amc::tfunc_Exec_Exec() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& exec = amc::CreateCurFunc();
    Ins(&R, exec.comment, "Execute subprocess and return exit code");
    Ins(&R, exec.ret  , "int",false);
    Ins(&R, exec.proto, "$name_Exec($Parent)",false);
    Ins(&R, exec.body, "$name_Start($pararg);");
    Ins(&R, exec.body, "$name_Wait($pararg);");
    Ins(&R, exec.body, "return $_status;");
}

void amc::tfunc_Exec_ExecX() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& exec = amc::CreateCurFunc();
    Ins(&R, exec.comment, "Execute subprocess; throw human-readable exception on error");
    Ins(&R, exec.ret  , "void",false);
    Ins(&R, exec.proto, "$name_ExecX($Parent)",false);
    Ins(&R, exec.body, "int rc = $name_Exec($pararg);");
    Ins(&R, exec.body, "vrfy(rc==0, tempstr() << \"algo_lib.exec\" << Keyval(\"cmd\",$name_ToCmdline($pararg))"
        "\n    << Keyval(\"comment\",algo::DescribeWaitStatus($_status)));");
}

void amc::tfunc_Exec_ToCmdline() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &execfield = *amc::_db.genfield.p_field;
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
    Ins(&R, tocmdline.body, "if (ch_N($_fstdin)) {");
    Ins(&R, tocmdline.body, "    retval << \" \" << $_fstdin;");
    Ins(&R, tocmdline.body, "}");
    Ins(&R, tocmdline.body, "if (ch_N($_fstdout)) {");
    Ins(&R, tocmdline.body, "    retval << \" \" << $_fstdout;");
    Ins(&R, tocmdline.body, "}");
    Ins(&R, tocmdline.body, "if (ch_N($_fstderr)) {");
    Ins(&R, tocmdline.body, "    retval << \" 2\" << $_fstderr;");
    Ins(&R, tocmdline.body, "}");
    Ins(&R, tocmdline.body, "return retval;");
}

void amc::tfunc_Exec_Execv() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &execfield = *amc::_db.genfield.p_field;
    bool amc_command = ind_ns_Find(name_Get(*execfield.p_arg)) != NULL;

    amc::FFunc& execv = CreateCurFunc();
    Set(R, "$ndatafld", tempstr()<<amc::c_datafld_N(*amc::_db.genfield.p_field->p_arg) * 2);
    Ins(&R, execv.ret    , "int",false);
    Ins(&R, execv.proto  , "$name_Execv($Parent)",false);
    Ins(&R, execv.comment, "Call execv with specified parameters -- cprint:$Ctype.Argv");
    Ins(&R, execv.body, "char **argv = (char**)alloca(($ndatafld+2+algo_lib::_db.cmdline.verbose)*sizeof(char*));"
        " // start of first arg (future pointer)");
    Ins(&R, execv.body, "algo::tempstr temp;");
    Ins(&R, execv.body, "int n_argv=0;");

    Ins(&R, execv.body,"argv[n_argv++] = (char*)(int_ptr)ch_N(temp);// future pointer");
    Ins(&R, execv.body, "temp << $_path;");
    Ins(&R, execv.body, "ch_Alloc(temp) = 0;// NUL term for pathname");

    ind_beg(amc::ctype_c_field_curs, field, *amc::_db.genfield.p_field->p_arg) if (!FldfuncQ(field)) {
        Set(R, "$fldname", name_Get(field));
        if (ch_N(field.dflt.value) > 0) {
            Set(R, "$dflt", field.dflt.value);
            if (field.reftype==dmmeta_Reftype_reftype_RegxSql) {
                Set(R, "$chkdflt", "$_cmd.$fldname.expr != $dflt");
            } else {
                Set(R, "$chkdflt", "$_cmd.$fldname != $dflt");
            }
        } else {
            Set(R, "$chkdflt", "true");
        }
        Ins(&R, execv.body, "");
        Ins(&R, execv.body, "if ($chkdflt) {");
        if (amc_command) {
            // amc command -- single argument contains key & value
            Ins(&R, execv.body,"    argv[n_argv++] = (char*)(int_ptr)ch_N(temp);// future pointer");
            Ins(&R, execv.body,"    temp << \"-$fldname:\";");
        } else {
            // non-amc command -- can't use key-value format. 2 arguments needed
            Ins(&R, execv.body,"    argv[n_argv++] = (char*)(int_ptr)ch_N(temp);// future pointer");
            Ins(&R, execv.body,"    temp << \"-$fldname\";");
            Ins(&R, execv.body,"    ch_Alloc(temp) = 0;");
            Ins(&R, execv.body,"    argv[n_argv++] = (char*)(int_ptr)ch_N(temp);// future pointer");
        }
        // #AL# this is a hack -- need a field print function here.
        amc::FFunc* func = amc::ind_func_Find(dmmeta::Func_Concat_field_name(field.field,"Print"));
        bool has_field_print = func != NULL;
        has_field_print |= field.reftype == dmmeta_Reftype_reftype_RegxSql;
        if (has_field_print) {
            Set(R, "$fns", ns_Get(*field.p_ctype));
            Ins(&R, execv.body, "    $fns::$fldname_Print($_cmd, temp);");
        } else {
            Set(R, "$Ftype", name_Get(*field.p_arg));
            Ins(&R, execv.body,"    $Ftype_Print($_cmd.$fldname, temp);");
        }
        Ins(&R, execv.body,"    ch_Alloc(temp) = 0;// NUL term for this arg");
        Ins(&R, execv.body,"}");
    }ind_end;

    if (amc_command) {
        // add verbose flags -- one fewer than current process
        Ins(&R, execv.body,"for (int i=0; i+1 < algo_lib::_db.cmdline.verbose; i++) {");
        Ins(&R, execv.body,"    argv[n_argv++] = (char*)(int_ptr)ch_N(temp);// future pointer");
        Ins(&R, execv.body,"    temp << \"-verbose\";");
        Ins(&R, execv.body,"    ch_Alloc(temp) = 0;");
        Ins(&R, execv.body,"}");
    }

    // shift argv pointers
    Ins(&R, execv.body,"argv[n_argv] = NULL; // last pointer");
    Ins(&R, execv.body,"while (n_argv>0) { // shift pointers");
    Ins(&R, execv.body,"    argv[--n_argv] += (u64)temp.ch_elems;");
    Ins(&R, execv.body,"}");

    Ins(&R, execv.body,"// if $_path is relative, search for it in PATH");
    Ins(&R, execv.body,"algo_lib::ResolveExecFname($_path);");
    Ins(&R, execv.body,"return execv(Zeroterm($_path),argv);");
}

// -----------------------------------------------------------------------------

void amc::NewFieldExec() {
    ind_beg(amc::_db_field_curs, field, amc::_db) if (field.reftype == dmmeta_Reftype_reftype_Exec) {
        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "path")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr(DefaultExecPath(field))
                                            , algo::Comment("path for executable")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "cmd")
                                            , field.arg
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr()
                                            , algo::Comment("command line for child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "fstdin")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr()
                                            , algo::Comment("redirect for stdin")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "fstdout")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr()
                                            , algo::Comment("redirect for stdout")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "fstderr")
                                            , "algo.cstring"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr()
                                            , algo::Comment("redirect for stderr")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "pid")
                                            , "pid_t"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr()
                                            , algo::Comment("pid of running child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "timeout")
                                            , "i32"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr("0")
                                            , algo::Comment("optional timeout for child process")));

        Field_AddChild(field, dmmeta::Field(SubfieldName(field, "status")
                                            , "i32"
                                            , dmmeta_Reftype_reftype_Val
                                            , dmmeta::CppExpr()
                                            , algo::Comment("last exit status of child process")));
    }ind_end;
}
