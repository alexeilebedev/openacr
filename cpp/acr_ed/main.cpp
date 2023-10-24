// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr_ed/main.cpp
//
// ACR editor
// Without -write, commands are printed to stdout.
// With -write, they are piped to acr, then amc + abt are used.
// See tex/acr/acr_ed.tex for more info

#include "include/acr_ed.h"
#include "include/gen/dev_gen.h"
#include "include/gen/dev_gen.inl.h"

// Request that amc runs after the current script
void acr_ed::NeedAmc() {
    acr_ed::_db.need_amc = true;
}

// -----------------------------------------------------------------------------

acr_ed::FCtype *acr_ed::Basetype(acr_ed::FCtype &ctype) {
    acr_ed::FCtype *retval = &ctype;
    ind_beg(acr_ed::ctype_c_field_curs,field,ctype) {
        if (field.reftype == dmmeta_Reftype_reftype_Base) {
            retval = field.p_arg;
        }
    }ind_end;
    return retval;
}

// -----------------------------------------------------------------------------

acr_ed::FField *acr_ed::PkeyField(strptr ctype) {
    acr_ed::FCtype &fctype = acr_ed::ind_ctype_FindX(ctype);
    acr_ed::FCtype *basetype = acr_ed::Basetype(fctype);
    vrfy(c_field_N(*basetype) > 0, tempstr() << "no fields in "<<ctype);
    return c_field_Find(*basetype, 0);
}

// -----------------------------------------------------------------------------

tempstr acr_ed::ToLowerUnder(strptr str) {
    tempstr lower;
    algo::strptr_PrintLowerUnder(str,lower);
    return lower;
}

// -----------------------------------------------------------------------------

void acr_ed::InsertSrcfileInclude(algo_lib::Replscope &R, bool mainheader) {
    if (mainheader) {
        Ins(&R, acr_ed::_db.script, "#include \"include/gen/$target_gen.h\"");
        Ins(&R, acr_ed::_db.script, "#include \"include/gen/$target_gen.inl.h\"");
    } else {
        Ins(&R, acr_ed::_db.script, "#include \"include/$target.h\"");
    }
    Ins(&R, acr_ed::_db.script, "");
    if (GetFileExt(acr_ed::_db.cmdline.srcfile) == ".h") {
        Ins(&R, acr_ed::_db.script, "namespace $target { // update-hdr");
        Ins(&R, acr_ed::_db.script, "}");
    }
}

// -----------------------------------------------------------------------------

static void Main_Vis() {
    algo::ListSep ls("|");
    // command for acr_in
    // feed both existing data and our proposed output to amc_vis
    tempstr cmd;
    verblog(acr_ed::_db.out_ssim);
    StringToFile(acr_ed::_db.out_ssim, "temp/acr_ed.ssim");
    cmd << "(acr_in -data amc_vis -sigcheck:N; cat temp/acr_ed.ssim) | amc_vis -xref:N -in:- '(";
    ind_beg(acr_ed::_db_vis_curs, vis, acr_ed::_db) {
        cmd << ls << vis;
    }ind_end;
    cmd << ")'";

    // total output
    tempstr out;
    out << "#  Proposed change" << eol;
    tempstr text(SysEval(cmd, FailokQ(false), 1024*1024));
    ind_beg(Line_curs, line, text) {
        out << "# "<<line<<eol;
    }ind_end;
    // prepend
    acr_ed::_db.out_ssim << out;
}

// -----------------------------------------------------------------------------

static void SelectCreate() {
    if (ch_N(acr_ed::_db.cmdline.unittest)) {
        acr_ed_edaction_Create_Unittest.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.citest)) {
        acr_ed_edaction_Create_Citest.select=true;
    }
    if (acr_ed::_db.cmdline.finput || acr_ed::_db.cmdline.gstatic) {
        acr_ed_edaction_Create_Finput.select=true;
    } else if (ch_N(acr_ed::_db.cmdline.ssimfile) > 0) {
        acr_ed_edaction_Create_Ssimfile.select=true;
    } else if (ch_N(acr_ed::_db.cmdline.target) > 0) {
        acr_ed_edaction_Create_Target.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.srcfile) > 0) {
        acr_ed_edaction_Create_Srcfile.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        acr_ed_edaction_Create_Ctype.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.field) > 0) {
        acr_ed_edaction_Create_Field.select=true;
    }
}

// -----------------------------------------------------------------------------

static void SelectDelete() {
    if (ch_N(acr_ed::_db.cmdline.field) > 0) {
        acr_ed_edaction_Delete_Field.select=true;
    }
    // if -del -ctype is specified, delete associated ssimfile (if exists)
    if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        if (acr_ed::FCtype *ctype = acr_ed::ind_ctype_Find(acr_ed::_db.cmdline.ctype)) {
            if (ctype->c_ssimfile) {
                acr_ed::_db.cmdline.ssimfile = ctype->c_ssimfile->ssimfile;
                acr_ed::_db.cmdline.ctype = "";// will be handled
            }
        }
    }
    if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        acr_ed_edaction_Delete_Ctype.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.ssimfile) > 0) {
        acr_ed_edaction_Delete_Ssimfile.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.srcfile) > 0) {
        acr_ed_edaction_Delete_Srcfile.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.target) > 0) {
        acr_ed_edaction_Delete_Target.select=true;
    }
}

// -----------------------------------------------------------------------------

static void SelectRename() {
    if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        acr_ed_edaction_Rename_Ctype.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.srcfile) > 0) {
        acr_ed_edaction_Rename_Srcfile.select=true;
    } else if (ch_N(acr_ed::_db.cmdline.target) > 0) {
        acr_ed_edaction_Rename_Target.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.ssimfile) > 0) {
        acr_ed_edaction_Rename_Ssimfile.select=true;
    }
    if (ch_N(acr_ed::_db.cmdline.field) > 0) {
        acr_ed_edaction_Rename_Field.select=true;
    }
}

// -----------------------------------------------------------------------------

static void ExecuteTransaction() {
    cstring final_script;
    if (algo_lib::_db.cmdline.verbose > 0) {
        final_script << "set -x" << eol;// echo all output
    }
    {
        command::acr acr;
        acr.t       = true;
        acr.replace = true;
        acr.print   = true;
        acr.report  = true;
        acr.check  = true;
        acr.selerr  = false;// do not select error records
        acr.write   = true;
        final_script << acr_ToCmdline(acr) << " << EOF" << eol;
        final_script << acr_ed::_db.out_ssim;
        final_script << "EOF" << eol;
        final_script << eol;
    }
    // inside sandbox, always build & run amc
    // because it may contain changes to be tested
    if (acr_ed::_db.cmdline.sandbox) {
        final_script << "ai amc && amc" << eol;
    }
    // append accumulated script to 'script'
    final_script << acr_ed::_db.script;
    if (acr_ed::_db.need_amc) {
        final_script <<  "bin/amc" << eol;
    }
    // highlight proposed change
    if (!acr_ed::_db.cmdline.write) {
        StringToFile(final_script, "temp/acr_ed.ssim", algo_FileFlags__throw);
        final_script = "cat temp/acr_ed.ssim";
        if (algo::SaneTerminalQ()) {
            algo_lib::Replscope R;
            Set(R, "$field" , dmmeta::Field_name_Get(acr_ed::_db.cmdline.field));
            Set(R, "$arg"   , acr_ed::_db.cmdline.arg);
            Set(R, "$target", acr_ed::_db.cmdline.target);
            final_script << Subst(R," | hilite acr.insert $arg $field $target");
        }
    }
    // perform transaction
    int rc=SysCmd(final_script, FailokQ(true));
    if (rc!=0) {
        algo_lib::_db.exit_code++;
    }
    (void)unlink("temp/acr_ed.ssim");
}

// -----------------------------------------------------------------------------

void acr_ed::BuildTest() {
    command::abt_proc abt;
    abt.cmd.install = true;
    abt.cmd.cfg = dev_Cfg_cfg_debug;
    abt.cmd.target.expr = "atf%|amc%|abt%|acr%";
    if (abt_Exec(abt)!=0) {
        algo_lib::_db.exit_code=1;
    }
    command::atf_amc_proc atf_amc;
    if (atf_amc_Exec(atf_amc)!=0) {
        algo_lib::_db.exit_code=1;
    }
}

// -----------------------------------------------------------------------------

void acr_ed::ScriptEditFile(algo_lib::Replscope &R, strptr fname) {
    tempstr test_file = FileToString(Subst(R,fname),algo::FileFlags());
    int nline=0;
    ind_beg(Line_curs,line,test_file) {
        nline++;
        (void)line;
    }ind_end;
    Set(R, "$prefix", acr_ed::_db.cmdline.e ? "" : "# ");
    Set(R, "$EDITOR", "$EDITOR", false);
    Ins(&R, acr_ed::_db.script, tempstr()<<"$prefix $EDITOR +"<<nline+4<<" "<<fname);
}

// -----------------------------------------------------------------------------

void acr_ed::ProcessAction() {
    int nsel=0;
    ind_beg(_db_edaction_curs,edaction,_db) {
        if (edaction.select) {
            nsel++;
        }
    }ind_end;
    if (nsel==0) {
        prerr("acr_ed: No action selected");
        algo_lib::_db.exit_code=1;
    } else if (nsel>1) {
        prerr("acr_ed: More than one action selected:");
        ind_beg(_db_edaction_curs,edaction,_db) {
            if (edaction.select) {
                prerr("    "<<edaction.edaction);
            }
        }ind_end;
        algo_lib::_db.exit_code=1;
    }
    if (algo_lib::_db.exit_code==0) {
        ind_beg(_db_edaction_curs,edaction,_db) {
            if (edaction.select) {
                edaction.step();
                if (edaction.needamc) {
                    NeedAmc();
                }
            }
        }ind_end;
    } else {
        SysCmd("acr edaction -report:N | ssimfilt -t -field:edaction -field:comment");
        prerr("see 'acr_ed -h' for more help");
    }
}

// -----------------------------------------------------------------------------

void acr_ed::Main() {
    algo_lib::FLockfile lockfile;
    LockFileInit(lockfile, "lock/acr_ed");

    acr_ed::_db.script << "set -e" << eol;

    if (acr_ed::_db.cmdline.create) {
        SelectCreate();
    }

    if (acr_ed::_db.cmdline.del) {
        SelectDelete();
    }

    if (ch_N(acr_ed::_db.cmdline.rename) > 0) {
        SelectRename();
    }

    // execute selected action
    ProcessAction();

    if (algo_lib::_db.exit_code==0) {
        // Run amc_vis on old dataset, include proposed changes,
        // producing visualization
        if (acr_ed::vis_N() > 0) {
            Main_Vis();
        }

        // BEGIN SANDBOX (if requested)
        if (acr_ed::_db.cmdline.sandbox) {
            command::sandbox_proc sandbox;
            sandbox.cmd.name.expr = dev_Sandbox_sandbox_acr_ed;
            sandbox.cmd.reset = true;
            sandbox_ExecX(sandbox);
            algo_lib::SandboxEnter(dev_Sandbox_sandbox_acr_ed);
            acr_ed::_db.cmdline.write = true;// !!! enable write mode from now on
            NeedAmc();
        }

        // execute/show proposed transaction
        ExecuteTransaction();
        if (acr_ed::_db.cmdline.test) {
            BuildTest();
        }

        // END SANDBOX (if requested)
        if (acr_ed::_db.cmdline.sandbox) {
            algo_lib::SandboxExit();
            command::sandbox_proc sandbox;
            sandbox.cmd.name.expr = dev_Sandbox_sandbox_acr_ed;
            sandbox.cmd.diff = true;
            sandbox_ExecX(sandbox);
        }
    }
}
