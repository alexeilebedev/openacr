//
// Target: atf_fuzz (exe)
// Exceptions: yes
// Source: cpp/atf/atf_fuzz.cpp
//

#include "include/algo.h"
#include "include/atf_fuzz.h"

void atf_fuzz::fuzzstrat_skip_inputs() {
    srandom(_db.cmdline.seed);
    if (_db.cmdline.inputfile == "") {
        _db.cmdline.inputfile = "temp/atf_fuzz.in";
        command::acr_in_proc acr_in;
        acr_in.cmd.ns.expr = _db.cmdline.target;
        acr_in.cmd.data=true;
        acr_in.fstdout << ">"<<_db.cmdline.inputfile;
        vrfy(acr_in_Exec(acr_in)==0, "acr_in failed");
        prlog("atf_fuzz: prepared input file using acr_in, "<<_db.cmdline.inputfile);
    }
    cstring inputs=FileToString(_db.cmdline.inputfile);
    int nline=0;
    ind_beg(Line_curs,line,inputs) {
        (void)line;
        nline++;
    }ind_end;
    prlog("atf_fuzz.skip_inputs.begin"
          <<Keyval("inputfile",_db.cmdline.inputfile)
          <<Keyval("reprofile",_db.cmdline.reprofile)
          <<Keyval("nline",nline));
    DeleteFile(_db.cmdline.reprofile);
    int ncrash=0;
    prlog("atf_fuzz: preparing sandbox");
    strptr sandbox="temp/atf_fuzz.sandbox/";
    command::bash_proc init;
    init.cmd.c << "(grep -v build .gitignore; echo .git) > temp/atf_fuzz.filter" << eol;
    init.cmd.c << "rsync -ac --filter=':- temp/atf_fuzz.filter' ./ "<<sandbox;
    vrfy(bash_Exec(init)==0,tempstr()<<"failed: "<<init.cmd.c);
    int iter=0;
    ind_beg(Line_curs,line,inputs) {
        if (algo::double_WeakRandom(1) <= _db.cmdline.testprob) {
            prlog(iter+1<<" of "<<nline);
            command::bash_proc sh;
            sh.cmd.c<<"cd "<<sandbox
                    <<" && echo "<<strptr_ToBash(tempstr()<<"acr.delete "<<line)
                    <<" | acr -insert -write >/dev/null"
                    <<" && "<<_db.cmdline.target<<" "<<_db.cmdline.args;
            // in verbose mode, see subcommand output
            sh.fstdout=">temp/atf_fuzz.childout";
            sh.fstderr=">&1";
            (void)command::bash_Exec(sh);
            if (algo_lib::_db.cmdline.verbose) {
                tempstr str;
                InsertIndent(str,algo::FileToString("temp/atf_fuzz.childout"),4);
                verblog(str);
            }
            if (WIFSIGNALED(sh.status) && (WTERMSIG(sh.status) == SIGSEGV)) {
                ncrash++;
                prlog("CRASH "<<sh.cmd.c);
                StringToFile(sh.cmd.c,_db.cmdline.reprofile,algo_FileFlags_append);
            }
            // restore sandbox data dir
            command::bash_proc cleanup;
            cleanup.cmd.c<<"rsync -a data/ "<<sandbox<<"/data/";
            vrfy(bash_Exec(cleanup)==0,tempstr()<<"failed: "<<cleanup.cmd.c);
        }
        iter++;
    }ind_end;
    prlog("atf_fuzz.skip_inputs.end"
          <<Keyval("ncases",nline)
          <<Keyval("ncrash",ncrash)
          <<Keyval("success",ncrash==0)
          <<Keyval("reprofile",_db.cmdline.reprofile));
}

void atf_fuzz::Main() {
    _db.c_target=ind_target_Find(_db.cmdline.target);
    vrfy(_db.c_target, tempstr()
         <<"atf_fuzz.badtarget"
         <<Keyval("target",_db.cmdline.target)
         <<Keyval("comment","Target not found"));
    ind_beg(_db_fuzzstrat_curs,fuzzstrat,_db) {
        if (Regx_Match(_db.cmdline.fuzzstrat,fuzzstrat.fuzzstrat)) {
            prlog("Apply strategy "<<fuzzstrat.fuzzstrat<<" to target "<<_db.c_target->target);
            fuzzstrat.step();
        }
    }ind_end;
}
