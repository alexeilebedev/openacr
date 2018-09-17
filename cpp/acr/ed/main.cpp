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
// Target: acr_ed (exe) -- ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
// Exceptions: yes
// Source: cpp/acr/ed/main.cpp
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
    ListSep ls("|");
    // command for acr_in
    // feed both existing data and our proposed output to amc_vis
    tempstr cmd;
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

static void Create() {
    int n_action = 0;
    if (ch_N(acr_ed::_db.cmdline.unittest)) {
        acr_ed::Main_CreateUnittest();
        acr_ed::NeedAmc();
        n_action++;
    } else if (acr_ed::_db.cmdline.finput || acr_ed::_db.cmdline.gstatic) {
        acr_ed::Main_CreateFinput();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.srcfile) > 0) {
        acr_ed::Main_CreateSrcfile();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.target) > 0) {
        acr_ed::Main_CreateTarget();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.ssimfile) > 0) {
        acr_ed::Main_CreateSsimfile();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        acr_ed::Main_CreateCtype();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.field) > 0) {
        acr_ed::Main_CreateField();
        acr_ed::NeedAmc();
        n_action++;
    }
    vrfy(n_action<2, "Not sure what to create.");
}

// -----------------------------------------------------------------------------

static void Delete() {
    int n_action = 0;
    if (ch_N(acr_ed::_db.cmdline.srcfile) > 0) {
        acr_ed::Main_DeleteSrcfile();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.target) > 0) {
        acr_ed::Main_DeleteTarget();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.ssimfile) > 0) {
        acr_ed::Main_DeleteSsimfile();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        acr_ed::Main_DeleteCtype();
        acr_ed::NeedAmc();
        n_action++;
    } else if (ch_N(acr_ed::_db.cmdline.field) > 0) {
        acr_ed::Main_DeleteField();
        acr_ed::NeedAmc();
        n_action++;
    }
    vrfy(n_action<2, "Not sure what to delete.");
}

// -----------------------------------------------------------------------------

static void Rename() {
    int n_action = 0;
    if (ch_N(acr_ed::_db.cmdline.ctype) > 0) {
        acr_ed::Main_RenameCtype();
        acr_ed::NeedAmc();
        n_action++;
    }
    if (ch_N(acr_ed::_db.cmdline.srcfile) > 0) {
        acr_ed::Main_RenameSrcfile();
        n_action++;
    }
    if (ch_N(acr_ed::_db.cmdline.ssimfile) > 0) {
        acr_ed::Main_RenameSsimfile();
        acr_ed::NeedAmc();
        n_action++;
    }
    if (ch_N(acr_ed::_db.cmdline.target) > 0) {
        acr_ed::Main_RenameTarget();
        acr_ed::NeedAmc();
        n_action++;
    }
    vrfy(n_action<2, "Not sure what to rename.");
}

// -----------------------------------------------------------------------------

static void ExecuteTransaction() {
    cstring final_script;
    if (algo_lib::_db.cmdline.verbose > 0) {
        final_script << "set -x" << eol;// echo all output
    }
    {
        command::acr acr;
        acr.rowid   = true;
        acr.t       = true;
        acr.insert  = true;
        acr.print   = true;
        acr.report  = true;
        acr.check   = true;
        acr.write   = true;
        acr.replace = acr_ed::_db.cmdline.replace;
        final_script << acr_ToCmdline(acr) << " << EOF" << eol;
        final_script << acr_ed::_db.out_ssim;
        final_script << "EOF" << eol;
        final_script << eol;
    }
    // append accumulated script to 'script'
    ind_beg(Line_curs,line,acr_ed::_db.script) {
        if (acr_ed::_db.cmdline.showcpp && StartsWithQ(line,"git")) {
            final_script << "#";
        }
        final_script << line << eol;
    }ind_end;
    if (acr_ed::_db.need_amc) {
        final_script <<  "bin/amc" << eol;
    }
    StringToFile(final_script, "temp/acr_ed.ssim", algo_FileFlags_throw);
    DryrunQ dry_run(!acr_ed::_db.cmdline.write);
    tempstr cmd = tempstr() << (acr_ed::_db.cmdline.write ? "sh" : "cat") << " < temp/acr_ed.ssim";
    // highlight proposed change
    if (SaneTerminalQ()) {
        algo_lib::Replscope R;
        Set(R, "$field" , dmmeta::Field_name_Get(acr_ed::_db.cmdline.field));
        Set(R, "$arg"   , acr_ed::_db.cmdline.arg);
        Set(R, "$target", acr_ed::_db.cmdline.target);
        cmd << Subst(R, "| hilite acr.insert $arg $field $target");
    }
    // perform transaction
    int rc=SysCmd(cmd, FailokQ(true), DryrunQ(false), EchoQ(false));
    if (rc!=0) {
        algo_lib::_db.exit_code++;
    }
    unlink("temp/acr_ed.ssim");
}

// -----------------------------------------------------------------------------

void acr_ed::Main() {
    algo_lib::FLockfile lockfile;
    LockFileInit(lockfile, "lock/acr_ed");

    if (acr_ed::_db.cmdline.create) {
        Create();
    }

    if (acr_ed::_db.cmdline.del) {
        Delete();
    }

    if (ch_N(acr_ed::_db.cmdline.rename) > 0) {
        Rename();
    }

    // Run amc_vis on old dataset, include proposed changes,
    // producing visualization
    if (acr_ed::vis_N() > 0) {
        Main_Vis();
    }

    // BEGIN SANDBOX (if showcpp specified)

    bool sandbox = acr_ed::_db.cmdline.showcpp || acr_ed::_db.cmdline.sandbox;
    if (sandbox) {
        BeginSandbox();
        acr_ed::_db.cmdline.write = true;// !!! enable write mode from now on
        NeedAmc();
    }

    // execute/show proposed transaction
    ExecuteTransaction();
    if (acr_ed::_db.cmdline.sandbox) {
        BuildX("amc|acr%|abt%");
    }
    ExitSandbox();
    // not in sandbox anymore...

    if (acr_ed::_db.cmdline.showcpp) {
        SysCmd("diff -I signature -U 8 -r ./cpp/gen .testgen/cpp/gen", FailokQ(true));
        SysCmd("diff -I signature -U 8 -r ./include/gen .testgen/include/gen", FailokQ(true));
    }
}
