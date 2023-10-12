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
// Target: gcli (exe) -- cli utility for gitlab and github
// Exceptions: yes
// Source: cpp/gcli/note.cpp
//

#include "include/gcli.h"

// -----------------------------------------------------------------------------
void gcli::gclicmd_noteadd(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrnoteadd(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_notemod(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::gclicmd_mrnotemod(gcli::FGclicmd&){
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issuenote_create(gcli::FGtblact &gtblact){
    // read the issue we want to update
    gcli::FIssue &issue=ReadSingleIssue(gtblact);
    if (gcli::_db.cmdline.e) {
        // need to mark available fields to edit...

        gclidb::Issuenote issuenote_out;
        TupleToGtblactflds(gtblact,tempstr()<<issuenote_out);

        if (!EditGtblactfld(gtblact,tempstr()<<issue.issue<<".new_note")){
            prlog("Note create has not been submitted, you can continue composing it.");
            return;
        }
    }

    gcli::VerifyGtblactfldsUpdate(gtblact);
    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_noteadd,true,gcli::IssueArgNumber(iid_Get(issue),""));
        gclicmd.body=d;

        gcli::Main_CurlExec();

        gcli::_db.cmdline.t=gtblact.t;
        ReadSingleIssue(gtblact);
        gcli::Main_ShowIssuelist();
    }

    // consider operation successful, remove file
    if (gcli::_db.edit_file!="") {
        unlink(Zeroterm(gcli::_db.edit_file));
        gcli::_db.edit_file="";
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issuenote_update(gcli::FGtblact &gtblact){
    tempstr note_id(gtblact.id);
    // validate note id
    tempstr nid(Pathcomp(note_id,".RR"));
    tempstr new_id(Pathcomp(note_id,".RL"));
    gtblact.id = new_id;

    gcli::_db.cmdline.t=gtblact.t;
    gcli::FIssue &issue=ReadSingleIssue(gtblact);
    note_id=tempstr()<<issue.issue<<"."<<nid;
    // find the note to edit

    gcli::FIssuenote *issuenote=ind_issuenote_Find(note_id);
    vrfy(issuenote,tempstr()
         <<Keyval("issue",issue.issue)
         <<Keyval("note_id",note_id)
         <<Keyval("comment","note with this note_id is not present in this issue")
         );

    gclidb::Issuenote issuenote_out;
    issuenote_CopyOut(*issuenote,issuenote_out);
    TupleToGtblactflds(gtblact,tempstr()<<issuenote_out);

    // update gtblactfld uval with edits
    if (gcli::_db.cmdline.e) {
        if (!EditGtblactfld(gtblact,note_id)){
            prlog("Note update has not been submitted, you can continue editing it.");
            return;
        }
        // re-populate gtblactflds with the issue
        TupleToGtblactflds(gtblact,tempstr()<<issuenote_out);
    }

    // Mark fields for update
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) if (gtblactfld.present){
        gtblactfld.update=(gtblactfld.uval!="")
            && (gtblactfld.uval!=gtblactfld.uval2);
    }ind_end;

    // update marked fields
    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_notemod,true,gcli::IssueArgNumber(issue.issue,""));
        gclicmd.note_id=nid_Get(*issuenote);
        gclicmd.body=d;
        gcli::Main_CurlExec();

        ReadSingleIssue(gtblact);
        gcli::Main_ShowIssuelist();
    } else {
        prlog("Issue has no changes.");
        gcli::Main_ShowIssuelist();
    }
    // consider operation successful, remove file
    if (gcli::_db.edit_file!="") {
        unlink(Zeroterm(gcli::_db.edit_file));
        gcli::_db.edit_file="";
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_issuenote_list(gcli::FGtblact &gtblact){
    tempstr note_id(gtblact.id);
    // validate note id
    tempstr nid(Pathcomp(note_id,".RR"));
    tempstr new_id(Pathcomp(note_id,".RL"));
    gtblact.id = SingleIssueQ(new_id) ? new_id : note_id;

    gcli::_db.cmdline.t=gtblact.t;
    zd_gtblact_Extend(gclidb_Gtblact_gtblact_issue_list,gtblact.id);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mrnote_list(gcli::FGtblact &gtblact){
    tempstr note_mr(gtblact.id);
    tempstr new_mr(Pathcomp(note_mr,".RL"));
    gtblact.id = SingleIssueQ(new_mr) ? new_mr : note_mr;

    gcli::_db.cmdline.t=gtblact.t;
    zd_gtblact_Extend(gclidb_Gtblact_gtblact_mr_list,gtblact.id);
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mrnote_create(gcli::FGtblact &gtblact){
    // read the mr we want to update
    gcli::FMr &mr=ReadSingleMr(gtblact);
    if (gcli::_db.cmdline.e) {
        // need to mark available fields to edit...

        gclidb::Mrnote mrnote_out;
        TupleToGtblactflds(gtblact,tempstr()<<mrnote_out);

        if (!EditGtblactfld(gtblact,tempstr()<<mr.mr<<".new_note")){
            prlog("Mr note create has not been submitted, you can continue composing it.");
            return;
        }
    }

    gcli::VerifyGtblactfldsUpdate(gtblact);

    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_mrnoteadd,true,gcli::IssueArgNumber(iid_Get(mr),""));
        gclicmd.body=d;

        gcli::Main_CurlExec();

        gcli::_db.cmdline.t=gtblact.t;
        ReadSingleMr(gtblact);
        gcli::Main_ShowMrlist();
    }
    // consider operation successful, remove file
    if (gcli::_db.edit_file!="") {
        unlink(Zeroterm(gcli::_db.edit_file));
        gcli::_db.edit_file="";
    }
}
// -----------------------------------------------------------------------------
void gcli::gtblact_mrnote_update(gcli::FGtblact &gtblact){
    tempstr note_mr(gtblact.id);
    // validate note id
    tempstr nid(Pathcomp(note_mr,".RR"));
    gtblact.id=Pathcomp(note_mr,".RL");

    gcli::_db.cmdline.t=gtblact.t;
    gcli::FMr &mr=ReadSingleMr(gtblact);
    note_mr=tempstr()<<mr.mr<<"."<<nid;
    // find the note to edit
    gcli::FMrnote *mrnote=ind_mrnote_Find(note_mr);
    vrfy(mrnote,tempstr()
         <<Keyval("mr",mr.mr)
         <<Keyval("note_mr",note_mr)
         <<Keyval("comment","note with this note_mr is not present in this mr")
         );

    gclidb::Mrnote mrnote_out;
    mrnote_CopyOut(*mrnote,mrnote_out);
    TupleToGtblactflds(gtblact,tempstr()<<mrnote_out);

    // update gtblactfld uval with edits
    if (gcli::_db.cmdline.e) {
        if (!EditGtblactfld(gtblact,note_mr)){
            prlog("Note update has not been submitted, you can continue editing it.");
            return;
        }
        // re-populate gtblactflds with the mr
        TupleToGtblactflds(gtblact,tempstr()<<mrnote_out);
    }

    // Mark fields for update
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) if (gtblactfld.present){
        gtblactfld.update=(gtblactfld.uval!="")
            && (gtblactfld.uval!=gtblactfld.uval2);
    }ind_end;

    // update marked fields
    tempstr d(GtblactfiledsToJson(gtblact));
    if (d!=""){
        gcli::FGclicmd &gclicmd=AddGclicmd(gclidb_Gclicmd_gclicmd_mrnotemod,true,gcli::IssueArgNumber(mr.mr,""));
        gclicmd.note_id=nid_Get(*mrnote);
        gclicmd.body=d;
        gcli::Main_CurlExec();

        ReadSingleMr(gtblact);
        gcli::Main_ShowMrlist();
    } else {
        prlog("Mr has no changes.");
    }
    // consider operation successful, remove file
    if (gcli::_db.edit_file!="") {
        unlink(Zeroterm(gcli::_db.edit_file));
        gcli::_db.edit_file="";
    }
}
