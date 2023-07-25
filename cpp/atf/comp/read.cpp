//
// Target: atf_comp (exe)
// Exceptions: yes
// Source: cpp/atf/comp/read.cpp -- Read comptest from input
//

#include "include/atf_comp.h"

// -----------------------------------------------------------------------------

// Read FD, create testcases based on the following mini-language
// comptest <name>
// expect <name>
// <msg> -- input message
// New testcases are added to zd_out_comptest, and will be subject
// to selection in a subsequent step
void atf_comp::Main_Read(algo::Fildes fd) {
    atf_comp::FComptest *cur_test=NULL;
    int rank=0;
    ind_beg(algo::FileLine_curs,line,fd) if (!StartsWithQ(line,"#")) {
        if (StartsWithQ(line,"comptest")) {
            algo::StringIter iter(ch_RestFrom(line,8));
            strptr name = GetWordCharf(iter);
            strptr args;
            strptr filter;
            strptr rest = iter.Ws().Rest();
            if (ch_N(rest)) {
                args = Trimmed(Pathcomp(rest,"|LL"));
                filter = Trimmed(Pathcomp(rest,"|LR"));
            }
            atf_comp::FComptest *comptest = ind_comptest_Find(name);
            if (!comptest) {
                comptest = &comptest_Alloc();
                comptest->comptest=name;
                comptest_XrefMaybe(*comptest);
            } else {
                zd_tmsg_Cascdel(*comptest);// clear our tmsgs under this test
            }
            if (comptest->c_targs) {
                comptest->c_targs->args=args;
            } else {
                atf_comp::FTargs &targs=targs_Alloc();
                targs.comptest=name;
                targs.args=args;
                targs_XrefMaybe(targs);
            }
            if (comptest->c_tfilt) {
                comptest->c_tfilt->filter=filter;
            } else {
                atf_comp::FTfilt &tfilt=tfilt_Alloc();
                tfilt.comptest=name;
                tfilt.filter=filter;
                tfilt_XrefMaybe(tfilt);
            }
            cur_test=comptest;
            cur_test->need_write=true;// mark as modified
            _db.nchange++;
            rank=0;
            _db.report.ninsert++;
        } else {
            if (cur_test) {
                bool expect = StartsWithQ(line,"expect");
                strptr dir = expect ? "exp" : "in";
                strptr msg = algo::Trimmed(expect ? ch_RestFrom(line,6) : strptr(line));
                if (ch_N(msg)) {
                    atf_comp::FTmsg &tmsg=tmsg_Alloc();
                    tmsg.tmsg=atfdb::Tmsg_Concat_comptest_rank_dir(cur_test->comptest,100000+rank*10,dir);
                    tmsg.msg=msg;
                    rank++;
                    (void)tmsg_XrefMaybe(tmsg);// will succeed
                }
            } else {
                prlog("# ignore "<<Keyval("line",line));
            }
        }
    }ind_end;
}
