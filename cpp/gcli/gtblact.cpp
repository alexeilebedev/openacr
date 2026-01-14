// Copyright (C) 2023-2026 AlgoRND
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
// Target: gcli (exe) -- Gcli - gitlab/github command-line client
// Exceptions: yes
// Source: cpp/gcli/gtblact.cpp
//

#include "include/gcli.h"
// -----------------------------------------------------------------------------
static bool NoteQ(strptr note_key){
    tempstr note(note_key);
    return Replace(note,".",".")
        && gcli::SingleIssueQ(Pathcomp(note,".RL"))
        ;
}
// -----------------------------------------------------------------------------
static bool JobQ(strptr note_key){
    tempstr note(note_key);
    return Replace(note,".",".")
        && gcli::SingleIssueQ(Pathcomp(note,"/RL"))
        ;
}
// -----------------------------------------------------------------------------
void gcli::PropagateGtblactID(strptr id){
    ind_beg(gcli::_db_zd_gtblact_curs,gtblact_next,gcli::_db){
        gtblact_next.id=id;
    }ind_end;
}
// -----------------------------------------------------------------------------
// Parse selector into a table and an id
void gcli::ValidateGtbl(){
    // is separator present in table?
    tempstr table(gcli::_db.cmdline.selector);
    Replace(table,":","\n");
    gcli::_db.p_gtbl=NULL;
    tempstr id;
    ind_beg(Line_curs,line,table) if (line!=""){
        if (gcli::_db.p_gtbl){
            id=line;
        } else if (!gcli::_db.p_gtbl){
            gcli::_db.p_gtbl=gcli::ind_gtbl_Find(line);
            if (!gcli::_db.p_gtbl){
                id=line;
            }
        }
    }ind_end;
    // set table default
    if (gcli::_db.p_gtbl==NULL){
        gcli::_db.p_gtbl=gcli::ind_gtbl_Find(gclidb_Gtbl_gtbl_issue);
        gcli::_db.p_gtbl->set_as_dflt=true;
    }
    // Redefine the table if the arg is a note
    bool tail_dot=false;
    if (NoteQ(id)) {
        // strip part to the right of the dot if present if it is not a digit
        // so mr:123.x (or mr:123.) can be ised as mrnote:mr:123
        u32 note_id;
        tempstr tail(Pathcomp(id,".RR"));
        if (tail=="" || !u32_ReadStrptrMaybe(note_id,tail)){
            id=tempstr()<<Pathcomp(id,".RL");
        }
        tail_dot=tail=="";
        if (gcli::_db.p_gtbl->gtbl == gclidb_Gtbl_gtbl_issue){
            gcli::_db.p_gtbl=gcli::ind_gtbl_Find(gclidb_Gtbl_gtbl_issuenote);
        } else if (gcli::_db.p_gtbl->gtbl == gclidb_Gtbl_gtbl_mr) {
            gcli::_db.p_gtbl=gcli::ind_gtbl_Find(gclidb_Gtbl_gtbl_mrnote);
        }
    }
    // Redefine the table if the arg is a job
    if (JobQ(id)) {
        if (gcli::_db.p_gtbl->gtbl == gclidb_Gtbl_gtbl_mr) {
            gcli::_db.p_gtbl=gcli::ind_gtbl_Find(gclidb_Gtbl_gtbl_mrjob);
        }
    }
    // set all ids to the recoverd token
    gcli::_db.p_gtbl->id=id;
    gcli::_db.p_gtbl->tail_dot=tail_dot;
}

// Pick up table and action from the command line
// -----------------------------------------------------------------------------
static void TableAct(gcli::FGact &gact, bool is_true){
    if (is_true){
        tempstr gtblact_key;
        gtblact_key=
            gclidb::Gtblact_Concat_gtbl_gact(gcli::_db.p_gtbl->gtbl,gact.gact);
        gcli::FGtblact *gtblact=gcli::ind_gtblact_Find(gtblact_key);
        if (!gtblact){
            gcli::_db.cmdline.t=true;
            gcli::ShowHelp(gcli::_db.p_gtbl->gtbl,"%");
            vrfy(gtblact,tempstr()
                 <<Keyval("table",gcli::_db.p_gtbl->gtbl)
                 <<Keyval("action",gact.gact)
                 <<Keyval("comment","selected action is not supported in this table, see supported actions above")
                 );
        }
        gtblact->select|=is_true;
        gtblact->id=gcli::_db.p_gtbl->id;
        // set -e
        // open the editor if the token fields are empty
        // and edit option is present
        gcli::_db.cmdline.e|=!fields_N(gcli::_db.cmdline) && gtblact->e;
    }
}
// -----------------------------------------------------------------------------
static bool LoadTblact(){
    // load selected verbs in the fixed order
    ind_beg(gcli::_db_gtblact_curs,gtblact,gcli::_db) if (gtblact.select) {
        zd_gtblact_Insert(gtblact);
        gcli::_db.need_auth|=gtblact.auth;
    }ind_end;
    return gcli::zd_gtblact_N()!=0;
}
// -----------------------------------------------------------------------------
static bool GoodArg(){
    return !gcli::_db.p_gtbl->set_as_dflt
        || Replace(gcli::_db.p_gtbl->id,"%","%")
        || gcli::IssueArgNumber(gcli::_db.p_gtbl->id,"")!=""
        ;
}
// -----------------------------------------------------------------------------
//void gcli::ArgsToTuple(int argc, char **argv) {
void gcli::ArgsToTuple() {
    ind_beg(gcli::_db_gact_curs,gact,gcli::_db){
        bool arg;
        if (gact.gact==gclidb_Gact_gact_list){
            arg=gcli::_db.cmdline.list;
        } else if (gact.gact==gclidb_Gact_gact_update){
            arg=gcli::_db.cmdline.update;
        } else if (gact.gact==gclidb_Gact_gact_create){
            arg=gcli::_db.cmdline.create;
        } else if (gact.gact==gclidb_Gact_gact_accept){
            arg=gcli::_db.cmdline.accept;
        } else if (gact.gact==gclidb_Gact_gact_start){
            arg=gcli::_db.cmdline.start;
        } else if (gact.gact==gclidb_Gact_gact_approve){
            arg=gcli::_db.cmdline.approve;
        } else if (gact.gact==gclidb_Gact_gact_needs_work){
            arg=gcli::_db.cmdline.needs_work;
        } else if (gact.gact==gclidb_Gact_gact_stop){
            arg=gcli::_db.cmdline.stop;
        } else {
            vrfy(0,tempstr()
                 <<Keyval("gact",gact.gact)
                 <<Keyval("comment","gact doesn't have a cmdline part!")
                 );
        }
        TableAct(gact,arg);
    }ind_end;
    if (!LoadTblact()){
        // set tblact default
        tempstr act_default(gclidb_Gact_gact_list);
        if (GoodArg()){
            if (gcli::_db.cmdline.e){
                act_default= SingleIssueQ(gcli::_db.p_gtbl->id) && !gcli::_db.p_gtbl->tail_dot ?
                    gclidb_Gact_gact_update
                    : gclidb_Gact_gact_create;
            }
        } else {
            gcli::ShowHelp("%","%");
            vrfy(0,tempstr()
                 <<Keyval("selector",_db.cmdline.selector)
                 <<Keyval("comment","selector is not valid, see the valid list above")
                 );
        }
        TableAct(ind_gact_FindX(act_default),true);
        vrfy(LoadTblact(),"impossible condition");
    }
}
// -----------------------------------------------------------------------------
static void SetFieldNameDflt(strptr val){
    ind_beg(gcli::_db_gtblactfld_curs, gtblactfld, gcli::_db) if (gtblactfld.field_name_dflt){
        gtblactfld.uval=val;
        gtblactfld.select=true;
    }ind_end;
}
// -----------------------------------------------------------------------------
static void TokenbufParse(strptr tokenbuf_in){
    gcli::c_gfld_RemoveAll();
    tempstr tokenbuf(tokenbuf_in);
    // First find all fields present in target
    // Replace field separators ':' and ',' with eol
    tempstr target;
    target=tokenbuf;

    Translate(target,",:","\n\n");
    // Identify field names - have to be a single item on the line
    ind_beg(Line_curs,line,target) if (line!=""){
        if (gcli::FGfld *gfld=gcli::ind_gfld_Find(line)){
            c_gfld_InsertMaybe(*gfld);
        }
    }ind_end;

    if (gcli::c_gfld_N()){
        // See if the line starts with the first field
        // if not, set the default and advance
        // cover case 1,state:closed as opposed to id:1,state:closed
        if (!StartsWithQ(tokenbuf,gcli::c_gfld_Find(0)->gfld)){
            SetFieldNameDflt(Pathcomp(tokenbuf,",LL"));
            tokenbuf=tempstr()<<Pathcomp(tokenbuf,",LR");
        }
        // Set values for the present fields
        // Replace field names with eol
        target=tempstr()<<","<<tokenbuf;
        ind_beg(gcli::_db_c_gfld_curs,gfld,gcli::_db){
            Replace(target,tempstr()<<","<<gfld.gfld<<":","\n");
        }ind_end;

        // Recover field values - have to be a single item on the line
        // and line count has to be same as field names count

        u32 gfld_idx(0);
        ind_beg(Line_curs,line,target) if (line!=""){
            gcli::FGfld *gfld=gcli::c_gfld_Find(gfld_idx++);
            vrfy(gfld, tempstr()
                 <<Keyval("target",tokenbuf)
                 <<Keyval("comment","target is malformed or has misspelled fields")
                 );
            ind_beg(gcli::gfld_c_gtblactfld_curs, gtblactfld, *gfld) {
                gtblactfld.uval=line;
                gtblactfld.select=true;
            }ind_end;
        }ind_end;
    } else {
        // if no fields are identified by the input is present
        // set all field_name_default fields to the input
        if (tokenbuf!=""){
            SetFieldNameDflt(tokenbuf);
        }
    }
}
// -----------------------------------------------------------------------------
static void TokenbufValidate(strptr tokenbuf_in){
    tempstr target(tokenbuf_in);
    Translate(target,",","\n");
    tempstr fld;
    ind_beg(Line_curs,line_in,target) if (line_in!=""){
        tempstr line(line_in);
        if (Replace(line,":",":")){
            tempstr fld_key(Pathcomp(line,":LL"));
            if (!gcli::ind_gfld_Find(fld_key)){
                fld=fld_key;
            }
        }
    }ind_end;
    if (fld!=""){
        tempstr id;
        tempstr gact;
        ind_beg(gcli::_db_zd_gtblact_curs,gtblact,gcli::_db) {
            id=gtbl_Get(gtblact);
            gact=gact_Get(gtblact);
        }ind_end;
        gcli::_db.cmdline.t=true;
        gcli::ShowHelp(id,gact);

        vrfy(0,tempstr()
             <<Keyval("table",id)
             <<Keyval("action",gact)
             <<Keyval("field",fld)
             <<Keyval("comment","field is not valid, see supported fields above")
             );
    }
}
// -----------------------------------------------------------------------------
// parse key:value,key:value,... or value - no other mix is covered
void gcli::TokenbufToGtblactflds(){
    // Set all fields to default values
    ind_beg(gcli::_db_gtblactfld_curs, gtblactfld, gcli::_db) {
        if (gtblactfld.dflt!=""){
            gtblactfld.uval=gtblactfld.dflt;
            gtblactfld.select=true;
        }
    }ind_end;

    ind_beg(command::gcli_fields_curs,fields,gcli::_db.cmdline){
        TokenbufValidate(fields);
        TokenbufParse(fields);
    }ind_end;
}
// -----------------------------------------------------------------------------
void gcli::zd_gtblact_Extend(strptr gtblact_key, strptr id){
    if (gcli::FGtblact *gtblact=gcli::ind_gtblact_Find(gtblact_key)){
        gtblact->id=id;
        zd_gtblact_Insert(*gtblact);
    }
}
// -----------------------------------------------------------------------------
gcli::FGtblact & gcli::SetGtblactfld(strptr gtblact_key, strptr field_key, strptr val){
    gcli::FGtblact &gtblact=gcli::ind_gtblact_FindX(gtblact_key);
    tempstr gtblactfld_key;
    gtblactfld_key=gclidb::Gtblactfld_Concat_gtblact_field(gtblact_key,field_key);
    gcli::FGtblactfld *gtblactfld=gcli::ind_gtblactfld_Find(gtblactfld_key);
    if (!gtblactfld){
        gtblactfld=&gtblactfld_Alloc();
        gtblactfld->gtblactfld=gtblactfld_key;
        vrfy(gtblactfld_XrefMaybe(*gtblactfld),algo_lib::_db.errtext);
    }
    gtblactfld->uval=val;
    gtblactfld->select=true;
    return gtblact;
}
// -----------------------------------------------------------------------------
void gcli::SetGtblactfldDflt(strptr gtblact_key, strptr field_key){
    tempstr gtblactfld_key;
    gtblactfld_key=gclidb::Gtblactfld_Concat_gtblact_field(gtblact_key,field_key);
    gcli::FGtblactfld *gtblactfld=gcli::ind_gtblactfld_Find(gtblactfld_key);
    if (gtblactfld){
        gtblactfld->uval=gtblactfld->dflt;
    }
}
// -----------------------------------------------------------------------------
void gcli::VerifyGtblactflds(){
    ind_beg(gcli::_db_zd_gtblact_curs,gtblact,gcli::_db) {
        if (!gcli::_db.cmdline.e
            || (gcli::_db.cmdline.e && !gtblact.e)
            ){
            if (gcli::_db.cmdline.e && !gtblact.e){
                prlog("gcli.warning  "
                      <<Keyval("table",gtbl_Get(gtblact))
                      <<Keyval("action",gact_Get(gtblact))
                      <<Keyval("comment","this combination doesn't support -e option")
                      );
            }
            ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) {
                // verfiy non optional fields are present
                if ((gtblactfld.optional!=true)
                    && (gtblactfld.uval=="")
                    ){
                    gcli::_db.cmdline.t=true;
                    gcli::ShowHelp(gtbl_Get(gtblact),gact_Get(gtblact));

                    vrfy(0,tempstr()
                         <<Keyval("table",gtbl_Get(gtblact))
                         <<Keyval("action",gact_Get(gtblact))
                         <<Keyval("field",field_Get(gtblactfld))
                         <<Keyval("comment","this field must be present, see fields above")
                         );
                }
            }ind_end;
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
tempstr gcli::GetTblactfld(strptr verb, strptr field){
    tempstr ret;
    tempstr gtblactfld_key;
    gtblactfld_key=gclidb::Gtblactfld_Concat_gtblact_field(verb,field);
    if (gcli::FGtblactfld *gtblactfld=gcli::ind_gtblactfld_Find(gtblactfld_key)){
        ret=gtblactfld->uval;
    }
    return ret;
}
// -----------------------------------------------------------------------------
static void MatchState(strptr name,tempstr &val){
    if (name==gclidb_Gfld_gfld_state){
        if (val=="opened"){
            val="open";
        } else if (val=="closed"){
            val="close";
        }
    }
}
// -----------------------------------------------------------------------------
// take an issue and populate gtblactfld uval2 with its values
void gcli::TupleToGtblactflds(gcli::FGtblact &gtblact,strptr str){
    algo::Tuple tuple;
    if (Tuple_ReadStrptrMaybe(tuple,str)){
        ind_beg(algo::Tuple_attrs_curs, attr, tuple){
            tempstr gtblactfld_key;
            gtblactfld_key=gclidb::Gtblactfld_Concat_gtblact_field(gtblact.gtblact,attr.name);
            if (gcli::FGtblactfld *gtblactfld=gcli::ind_gtblactfld_Find(gtblactfld_key)){
                gtblactfld->present=true;
                tempstr val(attr.value);
                MatchState(attr.name,val);
                gtblactfld->uval2=val;
                // preserve cmdline entry of gtblactflds;
                if (!gtblactfld->select){
                    gtblactfld->uval=gtblactfld->uval2;
                }
            }
        }ind_end;
    }
}
// -----------------------------------------------------------------------------
// take text in "gtblact:value" format and populate gtblactfld uval2 with its values
static void TextToGtblactflds(gcli::FGtblact &gtblact,strptr file){
    // clear uval2
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) {
        gtblactfld.uval2="";
    }ind_end;

    // populate uval2 from the file
    cstring text(Trimmed(FileToString(file)));
    gcli::FGtblactfld *gtblactfld=NULL;
    ind_beg(Line_curs,line,text) if (line!="") {
        tempstr field_key(Pathcomp(line,":LL"));
        tempstr gtblactfld_key;
        gtblactfld_key=gclidb::Gtblactfld_Concat_gtblact_field(gtblact.gtblact,field_key);
        if (gcli::FGtblactfld *gtblactfld2=gcli::ind_gtblactfld_Find(gtblactfld_key)){
            gtblactfld=gtblactfld2;
            gtblactfld->uval2=tempstr()<<Trimmed(Pathcomp(line,":LR"));
        } else if (gtblactfld){
            gtblactfld->uval2<<eol<<line;
        }
    }ind_end;
}
// -----------------------------------------------------------------------------
bool gcli::EditGtblactfld(gcli::FGtblact &gtblact,strptr issue_key_in){
    tempstr issue_key(issue_key_in);
    if (issue_key==""){
        issue_key=gcli::_db.grepo_sel.name;
        issue_key<<".NEW_ISSUE";
    }
    Replace(issue_key,"/","_");

    vrfy_(gcli::_db.editor.ch_n);
    //    gcli::_db.edit_file = DirFileJoin(gcli::_db.home,issue_key);
    gcli::_db.edit_file = DirFileJoin("temp",issue_key);
    if (FileQ(gcli::_db.edit_file)){
        // restore file to gtblactfiled uval2
        TextToGtblactflds(gtblact,gcli::_db.edit_file);
    };
    // preserve cmdline entry of gtblactflds;
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) {
        if (!gtblactfld.select && gtblactfld.uval2!=""){
            gtblactfld.uval=gtblactfld.uval2;
        }
    }ind_end;
    // load gtblactflds for editing;
    cstring tpl;
    tpl<<"WIP: "
       <<Keyval("Editing",issue_key)
       <<Keyval("File",gcli::_db.edit_file)
       <<Keyval("Note","Fill out the following lines and remove this line to proceed")
       <<eol;
    tpl<<eol;
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) if (gtblactfld.present){
        tpl<<field_Get(gtblactfld)<<":"<<gtblactfld.uval<<eol;
    }ind_end;
    vrfy_(StringToFile(tpl,gcli::_db.edit_file,algo::FileFlags(),false));

    cstring cmdline;
    cmdline << gcli::_db.editor << " " << gcli::_db.edit_file;
    SysCmd(cmdline, FailokQ(false), DryrunQ(false));
    // Check if the WIP: line is present
    cstring text(Trimmed(FileToString(gcli::_db.edit_file)));
    if (Replace(text,"WIP:","WIP:")) {
        return false;
    }
    // Parse edited result back into gtblactflds
    TextToGtblactflds(gtblact,gcli::_db.edit_file);
    // form new entry
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) {
        if (gtblactfld.uval2!=""){
            gtblactfld.uval=gtblactfld.uval2;
            gtblactfld.uval2="";
            gtblactfld.select=true;
        }
    }ind_end;
    return true;
}
// -----------------------------------------------------------------------------
void gcli::VerifyGtblactfldsUpdate(gcli::FGtblact &gtblact){
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) {
        // set update field
        gtblactfld.update=gtblactfld.uval!="";
        // verfiy non optional fields are present
        vrfy((gtblactfld.optional==true) || (gtblactfld.update==true),tempstr()
             <<Keyval("field",field_Get(gtblactfld))
             <<Keyval("comment","this field must be present.")
             );
    }ind_end;
}
// -----------------------------------------------------------------------------
tempstr gcli::GtblactfieldsToJson(gcli::FGtblact &gtblact){
    tempstr out;
    lib_json::FNode *obj(NULL);
    ind_beg(gcli::gtblact_c_gtblactfld_curs,gtblactfld,gtblact) if (gtblactfld.update){
        if (!obj){
            obj = &lib_json::NewObjectNode(NULL);
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_title){
            lib_json::NewStringNode(obj,"title",gtblactfld.uval);
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_description){
            lib_json::NewStringNode(obj,"description",gtblactfld.uval);
            lib_json::NewStringNode(obj,"body",gtblactfld.uval);
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_assignee){
            u32 user_id(0);
            tempstr user_name;
            if (gtblactfld.uval!="none"){
                gcli::FUser &user=gcli::GetUser(gtblactfld.uval);
                user_id=user.id;
                user_name=user.user;
            };

            lib_json::NewNumberNode(obj,"assignee_id",tempstr()<<user_id);
            lib_json::FNode &ary = lib_json::NewArrayNode(obj,"assignees");
            lib_json::NewStringNode(&ary,"",user_name);
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_reviewer){
            u32 user_id(0);
            tempstr user_name;
            if (gtblactfld.uval!="none"){
                gcli::FUser &user=gcli::GetUser(gtblactfld.uval);
                user_id=user.id;
                user_name=user.user;
            };
            lib_json::FNode &ary = lib_json::NewArrayNode(obj,"reviewer_ids");
            lib_json::NewStringNode(&ary,"",tempstr()<<user_id);
            lib_json::FNode &ary2 = lib_json::NewArrayNode(obj,"reviewers");
            lib_json::NewStringNode(&ary2,"",user_name);
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_milestone){
            gcli::FMilestone &milestone=gcli::GetMilestone(gtblactfld.uval);
            lib_json::NewStringNode(obj,"milestone",iid_Get(milestone));
            lib_json::NewStringNode(obj,"milestone_id",iid_Get(milestone));
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_state){
            tempstr gstate_key;
            gstate_key<<"state_set_"<<gtblactfld.uval;
            tempstr gstate(gcli::Gstate(gstate_key));

            vrfy(gstate!="",tempstr()
                 <<Keyval("state",gtblactfld.uval)
                 <<Keyval("comment","state not valid - use open or close")
                 );

            lib_json::NewStringNode(obj,"state",gstate);
            lib_json::NewStringNode(obj,"state_event",gstate);
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_labels){
            tempstr labels_in;
            if (gtblactfld.uval!="none"){
                labels_in=gtblactfld.uval;
            }
            lib_json::NewStringNode(obj,"add_labels",labels_in);
            tempstr labels(labels_in);
            Replace(labels,",","\n");
            lib_json::FNode &ary = lib_json::NewArrayNode(obj,"labels");
            ind_beg(Line_curs,line,labels){
                lib_json::NewStringNode(&ary,"",line);
            }ind_end;
        }
        if (field_Get(gtblactfld)==gclidb_Gfld_gfld_note){
            lib_json::NewStringNode(obj,"body",gtblactfld.uval);
        }
    }ind_end;
    if (obj){
        lib_json::JsonSerialize(obj,out);
    }
    return out;
}
