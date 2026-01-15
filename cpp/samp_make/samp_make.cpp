// Copyright (C) 2023-2024,2026 AlgoRND
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
// Target: samp_make (exe) -- sample program for Makefile management
// Exceptions: yes
// Source: cpp/samp_make/samp_make.cpp
//

#include "include/algo.h"
#include "include/algo.h"
#include "include/samp_make.h"

// -----------------------------------------------------------------------------
static samp_make::FTargdep& ind_targdep_GetOrCreate(strptr targdep_key){
    samp_make::FTargdep *targdep=samp_make::ind_targdep_Find(targdep_key);
    if (!targdep){
        targdep=&samp_make::targdep_Alloc();
        targdep->targdep=targdep_key;
        vrfy(targdep_XrefMaybe(*targdep),algo_lib::_db.errtext);
    }
    return *targdep;
}
// -----------------------------------------------------------------------------
static samp_make::FTargsrc& ind_targsrc_GetOrCreate(strptr targsrc_key){
    samp_make::FTargsrc *targsrc=samp_make::ind_targsrc_Find(targsrc_key);
    if (!targsrc){
        targsrc=&samp_make::targsrc_Alloc();
        targsrc->targsrc=targsrc_key;
        vrfy(targsrc_XrefMaybe(*targsrc),algo_lib::_db.errtext);
    }
    return *targsrc;
}
// -----------------------------------------------------------------------------
static samp_make::FTargrec& ind_targrec_GetOrCreate(strptr targrec_key){
    samp_make::FTargrec *targrec=samp_make::ind_targrec_Find(targrec_key);
    if (!targrec){
        targrec=&samp_make::targrec_Alloc();
        targrec->target=targrec_key;
        vrfy(targrec_XrefMaybe(*targrec),algo_lib::_db.errtext);
    }
    return *targrec;
}
// -----------------------------------------------------------------------------
// Parse recipies
// targets and targsrc can be found by index
// so all that is left in the line are the recipe
static void ParseRecipies(){
    ind_beg(samp_make::_db_target_curs,target,samp_make::_db){
        tempstr recipe;
        algo::ListSep ls_sp(" ");
        ind_beg(algo::Sep_curs,token,target.recipe,' ') if (token!=""){
            if (samp_make::FTarget *parent=samp_make::ind_target_Find(token)){
                if (parent->target!=target.target){
                    ind_targdep_GetOrCreate(sampdb::Targdep_Concat_target_parent(target.target,token)).rec=true;
                } else {
                    recipe<<ls_sp<<token;
                }
            } else if (samp_make::ind_gitfile_Find(token)) {
                ind_targsrc_GetOrCreate(sampdb::Targsrc_Concat_target_src(target.target,token)).rec=true;
            } else {
                recipe<<ls_sp<<token;
            }
        }ind_end;
        if (recipe!=""){
            ind_targrec_GetOrCreate(target.target).recipe=recipe;
            recipe="";
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
// Parse prereqs
// targets can be found by index
// so all that is left in the line are targsrc
static void ParsePrereq(){
    // Parse prereq to collect targdep and targsrc
    ind_beg(samp_make::_db_target_curs,target,samp_make::_db){
        ind_beg(algo::Sep_curs,token,target.prereq,' ') if (token!=""){
            if (samp_make::ind_target_Find(token)){
                ind_targdep_GetOrCreate(sampdb::Targdep_Concat_target_parent(target.target,token)).pre=true;
            } else {
                ind_targsrc_GetOrCreate(sampdb::Targsrc_Concat_target_src(target.target,token)).pre=true;
                (void)samp_make::ind_gitfile_GetOrCreate(token);
            }
        }ind_end;
    }ind_end;
}
// -----------------------------------------------------------------------------
// Parse Makefile to collect indexed targets
// extract recipe and prereq lines into targets
static void ParseTargets(strptr makefile){
    tempstr line_in;
    samp_make::FTarget *target=NULL;
    // make a list of targets
    // collect recipie and prereq into targets
    bool dflt(true);
    ind_beg(algo::FileLine_curs,line,makefile) if (line!=""){
        // unroll continuation lines into new line
        line_in<<line;
        if (Replace(line_in,"\\","")){
            continue;
        }
        tempstr target_in(Pathcomp(line_in,":LL"));
        if (target_in!="" && !StartsWithQ(target_in," ")){
            target=&samp_make::ind_target_GetOrCreate(Trimmed(target_in));
            target->dflt=dflt;
            dflt=false;
        }
        if (StartsWithQ(target_in," ")){
            target->recipe=line_in;
        } else {
            target->prereq=Pathcomp(line_in,":LR");
        }
        line_in="";
    }ind_end;
    ParsePrereq();
    ParseRecipies();
}
// Parse specific make file, extract content, write it out as ssims
// -----------------------------------------------------------------------------
static void Main_ParseMake(strptr makefile){
    ParseTargets(makefile);

    cstring out;
    ind_beg(samp_make::_db_target_curs,target,samp_make::_db){
        sampdb::Target target_out;
        target_CopyOut(target,target_out);
        out<<target_out<<eol;
    }ind_end;

    ind_beg(samp_make::_db_targdep_curs,targdep,samp_make::_db){
        sampdb::Targdep targdep_out;
        targdep_CopyOut(targdep,targdep_out);
        out<<targdep_out<<eol;
    }ind_end;

    ind_beg(samp_make::_db_targsrc_curs,targsrc,samp_make::_db){
        sampdb::Targsrc targsrc_out;
        targsrc_CopyOut(targsrc,targsrc_out);
        out<<targsrc_out<<eol;
    }ind_end;

    ind_beg(samp_make::_db_targrec_curs,targrec,samp_make::_db){
        sampdb::Targrec targrec_out;
        targrec_CopyOut(targrec,targrec_out);
        out<<targrec_out<<eol;
    }ind_end;

    ind_beg(samp_make::_db_gitfile_curs,gitfile,samp_make::_db){
        sampdb::Gitfile gitfile_out;
        gitfile_CopyOut(gitfile,gitfile_out);
        out<<gitfile_out<<eol;
    }ind_end;

    // Create temp file and write out there
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"samp_make");
    StringToFile(out,tempfile.filename);

    // Execute command to print the files or use acr to create them
    tempstr cmd;
    cmd<<"cat "<<tempfile.filename<<" | ";
    if (samp_make::_db.cmdline.write){
        command::acr acr;
        acr.trunc=true;
        acr.write=true;
        acr.insert=true;
        cmd<<acr_ToCmdline(acr);
    } else {
        cmd<<"sort";
    }
    // execute command
    SysCmd(cmd);
}
// -----------------------------------------------------------------------------
static tempstr PrintTarget(samp_make::FTarget &target){
    target.select=true;
    tempstr out;
    // collect prerecs
    tempstr prereq;
    tempstr recipe;
    algo::ListSep ls_pre(" ");
    algo::ListSep ls_rec(" ");
    out<<target.target<<" : ";
    ind_beg(samp_make::target_c_targsrc_curs,targsrc,target) {
        if (targsrc.pre){
            prereq<<ls_pre<<src_Get(targsrc);
        };
        if (targsrc.rec){
            recipe<<ls_pre<<src_Get(targsrc);
        };
    }ind_end;
    ind_beg(samp_make::target_c_targdep_curs,targdep,target){
        if (targdep.pre){
            prereq<<ls_pre<<parent_Get(targdep);
        };
        if (targdep.rec){
            recipe<<ls_pre<<parent_Get(targdep);
        };
    }ind_end;
    if (prereq!=""){
        out<<prereq<<eol;
    } else {
        out<<eol;
    }
    if (recipe!="" && target.c_targrec){
        out<<"\t"<<target.c_targrec->recipe
           <<" "
           <<recipe;
    }
    return out;
}
// -----------------------------------------------------------------------------
static void PrintTargets(samp_make::FTarget &target){
    // gather all targets pointing to default
    prlog(PrintTarget(target));
    if (target.dflt){
        ind_beg(samp_make::target_c_targdep_curs,targdep,target) {
            if (!targdep.p_parent->select){
                prlog(PrintTarget(*targdep.p_parent));
            }
        }ind_end;
    }
}
// -----------------------------------------------------------------------------
void samp_make::Main() {
    if (samp_make::_db.cmdline.parse_make){
        Main_ParseMake(samp_make::_db.cmdline.makefile);
    } else {
        // Print From.I of make
        samp_make::LoadTuplesMaybe(samp_make::_db.cmdline.in,true);
        // Build default target
        // followed by all targets that pount to it
        // followed by the rest

        // Put default target upfront
        ind_beg(samp_make::_db_target_curs,target,samp_make::_db) if (target.dflt) {
            c_target_InsertMaybe(target);
        }ind_end;
        ind_beg(samp_make::_db_target_curs,target,samp_make::_db) if (!target.dflt) {
            c_target_InsertMaybe(target);
        }ind_end;
        ind_beg(_db_c_target_curs, target,_db) if (algo_lib::Regx_Match(_db.cmdline.target,target.target)){
            PrintTargets(target);
        }ind_end;
    }
}
