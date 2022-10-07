//
// Target: gitlab (exe) -- Gitlab command line interface
// Exceptions: yes
// Source: cpp/gitlab.cpp
//

#include "include/algo.h"
#include "include/gitlab.h"

#include <curl/curl.h>

// -----------------------------------------------------------------------------

// Clear all response fields for request reuse case
static void ClearResponse(gitlab::FHttp &http) {
    http.response_status_line = strptr();
    http.response_status_code = 0;
    http.response_reason_phrase = strptr();
    response_header_RemoveAll(http);
    http.response_content_type = strptr();
    http.response_body = strptr();
    http.response_content_type_json = false;

    // ugly hack as operator= is forbidden,
    // Uninit and Init calls do not deal with child structures
    http.response_json_parser.~FParser(); // call destructor
    new (&http.response_json_parser) lib_json::FParser(); // call placement new
}

// -----------------------------------------------------------------------------

// Curl write callback fuction
static size_t CurlWrite(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    gitlab::FHttp &http = *(gitlab::FHttp *)userdata;
    strptr chunk(ptr,size*nmemb);
    if (http.response_content_type_json) {
        JsonParse(http.response_json_parser,chunk);
    } else {
        http.response_body << chunk;
    }
    return size*nmemb;
}

// -----------------------------------------------------------------------------

// Curl header calback function, also called for status line
static size_t CurlHeader(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    gitlab::FHttp &http = *(gitlab::FHttp *)userdata;
    strptr header(ptr,size*nmemb);
    if (!http.response_status_line.ch_n || http.response_status_code == 100) {
        // store status line
        http.response_status_line = Trimmed(header);
        // parse status line
        algo::StringIter it(http.response_status_line);
        GetWordCharf(it); //skip HTTP-Version
        TryParseU32(it,http.response_status_code); // Status-Code
        http.response_reason_phrase = Trimmed(it.Rest());// Reason-Phrase
    } else {
        // add header
        response_header_Alloc(http) = Trimmed(header);
        // check content-type
        strptr name = Trimmed(Pathcomp(header,":LL"));
        strptr value = Trimmed(Pathcomp(header,":LR"));
        if (!CompareNoCase(name,"Content-Type")) {
            http.response_content_type = value;
            http.response_content_type_json = !CompareNoCase(value,"application/json");
        }
    }
    return size*nmemb;
}

// -----------------------------------------------------------------------------

// Curl header calback function, also called for status line
static size_t CurlRead(char *ptr, size_t size, size_t nmemb, void *userdata) {
    vrfy_(userdata);
    gitlab::FHttp &http = *(gitlab::FHttp *)userdata;
    u32 requested_bytes = size*nmemb;
    u32 available_bytes = http.request_body.ch_n-http.request_body_sent_bytes;
    u32 returned_bytes  = u32_Min(available_bytes,requested_bytes);
    memcpy(ptr,http.request_body.ch_elems+http.request_body_sent_bytes,returned_bytes);
    http.request_body_sent_bytes+=returned_bytes;
    return returned_bytes;
}

// -----------------------------------------------------------------------------

#define curl_vrfy_(res)                                                 \
    do {                                                                \
        CURLcode res_copy(res);                                         \
        vrfy(res_copy==CURLE_OK,tempstr()<<(#res)<<": "<<curl_easy_strerror(res_copy)); \
    } while (0)

// -----------------------------------------------------------------------------

static void CurlExec(gitlab::FHttp &http) {
    ClearResponse(http);

    // Form headers:
    // note, curl does not copy values for itself,
    // so keep this structure unchanged!
    curl_slist *hdr(NULL);
    ind_beg(gitlab::FHttp_request_header_curs,rh,http){
        vrfy_(hdr=curl_slist_append(hdr,Zeroterm(rh)));
    }ind_end;

    CURL *curl;
    vrfy_(curl = curl_easy_init());

    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HTTPHEADER,hdr));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_URL, Zeroterm(http.request_uri)));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L));
    if (algo_lib::_db.cmdline.debug) {
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L));
    }
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &http));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, CurlHeader));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_HEADERDATA, &http));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_READFUNCTION, CurlRead));
    curl_vrfy_(curl_easy_setopt(curl, CURLOPT_READDATA, &http));
    switch (http.request_method) {
    case gitlab_FHttp_request_method_GET:
        // this is the default
        break;
    case gitlab_FHttp_request_method_POST:
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_POST, 1L));
        break;
    case gitlab_FHttp_request_method_PUT:
        curl_vrfy_(curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L));
        break;
    default:
        vrfy(0,tempstr()<<"Unsupported HTTP method: " << http.request_method);
    }

    curl_vrfy_(curl_easy_perform(curl));

    curl_slist_free_all(hdr);
    curl_easy_cleanup(curl);

    // print debug output
    if (algo_lib::_db.cmdline.debug) {
        // params
        dbglog(http);
        // headers
        ind_beg(gitlab::FHttp_response_header_curs,rh,http){
            dbglog(rh);
        }ind_end;
        // json
        if (http.response_json_parser.root_node) {
            cstring out;
            JsonSerialize(http.response_json_parser.root_node,out,true);
            dbglog(out);
        }
    }
}

// -----------------------------------------------------------------------------

static tempstr GetResponseHeaderValue(gitlab::FHttp &http, strptr search_name) {
    tempstr out;
    ind_beg(gitlab::FHttp_response_header_curs,hdr,http) {
        strptr name = Trimmed(Pathcomp(hdr,":LL"));
        if (StrEqual(name,search_name,false)) {
            out = Trimmed(Pathcomp(hdr,":LR"));
            break;
        }
    }ind_end;
    return out;
}

// -----------------------------------------------------------------------------

// Manage environment variables: USER,HOME,EDITOR
static void Main_ManageEnv() {
    gitlab::_db.unix_user = getenv("USER");
    gitlab::_db.home = getenv("HOME");
    gitlab::_db.editor = getenv("EDITOR");

    bool need_editor = gitlab::_db.cmdline.iadd;

    vrfy(gitlab::_db.unix_user.ch_n,"Your USER environment variable has not been set");
    vrfy(gitlab::_db.home.ch_n,"Your HOME environment variable has not been set");
    vrfy(!need_editor || gitlab::_db.editor.ch_n, "Your environment variable EDITOR has not been set");
}

// -----------------------------------------------------------------------------

// Prepare authorization header:
// Manage -auth_token option, $HOME/.auth_token file.

static void Main_ManageAuth() {
    // auth token
    gitlab::_db.auth_file = DirFileJoin(gitlab::_db.home, gitlab::_db.auth_file_name);
    gitlab::_db.auth_token = Trimmed(gitlab::_db.cmdline.auth_token);
    if (gitlab::_db.cmdline.auth_token.ch_n) {
        // case where token is supplied via command line
        vrfy_(StringToFile(gitlab::_db.auth_token,gitlab::_db.auth_file,algo::FileFlags(),false));
        errno_vrfy_(!chmod(Zeroterm(gitlab::_db.auth_file),0600));
    } else if (FileQ(gitlab::_db.auth_file)) {
        // case where token is read from file
        gitlab::_db.auth_token = Trimmed(FileToString(gitlab::_db.auth_file));
    }
    vrfy(gitlab::_db.auth_token.ch_n, gitlab::_db.usrmsg_no_token);
    // prepare http authorization header
    gitlab::_db.http_auth_header << "Authorization: Bearer " << gitlab::_db.auth_token;
}

// -----------------------------------------------------------------------------

// prepare API URI as base to add resource parts:
// process server option
static void Main_ManageServer() {
    gitlab::_db.rest_api << "https://" << gitlab::_db.cmdline.server << "/api/v4";
}

// -----------------------------------------------------------------------------

// Get numeric project id
static void Main_ManageProject() {
    gitlab::FProject *project = gitlab::ind_project_Find(gitlab::_db.cmdline.project);
    vrfy(project, tempstr() << "Can't find project:" << gitlab::_db.cmdline.project);
    gitlab::_db.project_id = project->gitlab_project_id;
}

// -----------------------------------------------------------------------------

static void LoadIssueNotes(gitlab::FIssue &issue) {
    cstring page("1");
    do {
        // prepare REST request
        gitlab::FHttp http;
        http.request_uri << gitlab::_db.rest_api
                         << "/projects/" << gitlab::_db.project_id
                         <<"/issues/" << iid_Get(issue)
                         << "/notes?per_page=100&page="<<page;
        request_header_Alloc(http) << gitlab::_db.http_auth_header;
        request_header_Alloc(http) << "Accept: application/json";

        // execute request
        CurlExec(http);

        // prechecks on output
        vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
             << http.response_status_line);
        vrfy_(http.response_json_parser.root_node);
        vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_array);

        // load into database
        ind_beg(lib_json::node_c_child_curs,issue_obj,*http.response_json_parser.root_node){
            vrfy_(issue_obj.type == lib_json_FNode_type_object);

            gitlab::IssueNote note;
            note.issue_note = gitlab::Issue_Concat_project_iid(issue.issue, u32_Get(&issue_obj,"id"));
            note.body = strptr_Get(&issue_obj,"body");

            vrfy_(gitlab::issue_note_InsertMaybe(note));
        }ind_end;
        page = GetResponseHeaderValue(http,"x-next-page");
    } while (page != "");
}

// -----------------------------------------------------------------------------

// load specific issue
static gitlab::FIssue *LoadIssueMaybe(u32 iid) {
    // prepare REST request
    gitlab::FHttp http;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/issues/" << iid;
    request_header_Alloc(http) << gitlab::_db.http_auth_header;
    request_header_Alloc(http) << "Accept: application/json";

    // execute request
    CurlExec(http);

    if (http.response_status_code == 404) {
        return NULL;
    }

    // prechecks on output
    vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
         << http.response_status_line);
    vrfy_(http.response_json_parser.root_node);
    lib_json::FNode &issue_obj = *http.response_json_parser.root_node;
    vrfy_(issue_obj.type == lib_json_FNode_type_object);

    gitlab::Issue issue;
    issue.issue = tempstr() << gitlab::_db.cmdline.project << "." << u32_Get(&issue_obj,"iid");
    issue.title = strptr_Get(&issue_obj,"title");

    gitlab::FIssue *fissue = gitlab::issue_InsertMaybe(issue);
    vrfy_(fissue);

    gitlab::IssueDescription issue_description;
    issue_description.issue = issue.issue;
    issue_description.description = strptr_Get(&issue_obj,"description");

    vrfy_(gitlab::issue_description_InsertMaybe(issue_description));

    return fissue;
}

// -----------------------------------------------------------------------------

// Read list of issues from gitlab
// and populate _db.issue, _db.issue_desecription tables
static void Main_LoadIssueList() {
    cstring page("1");
    do {
        // prepare REST request
        gitlab::FHttp http;
        http.request_uri << gitlab::_db.rest_api
                         << "/projects/" << gitlab::_db.project_id
                         << "/issues?state=opened&per_page=100&page="<<page;
        request_header_Alloc(http) << gitlab::_db.http_auth_header;
        request_header_Alloc(http) << "Accept: application/json";

        // execute request
        CurlExec(http);

        // prechecks on output
        vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
             << http.response_status_line);
        vrfy_(http.response_json_parser.root_node);
        vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_array);

        // load into database
        ind_beg(lib_json::node_c_child_curs,issue_obj,*http.response_json_parser.root_node){
            vrfy_(issue_obj.type == lib_json_FNode_type_object);

            gitlab::Issue issue;
            issue.issue = gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project, u32_Get(&issue_obj,"iid"));
            issue.assignee = strptr_Get(&issue_obj,"assignee.username");
            issue.title = strptr_Get(&issue_obj,"title");

            vrfy_(gitlab::issue_InsertMaybe(issue));

            gitlab::IssueDescription issue_description;
            issue_description.issue = issue.issue;
            issue_description.description = strptr_Get(&issue_obj,"description");

            vrfy_(gitlab::issue_description_InsertMaybe(issue_description));
        }ind_end;
        page = GetResponseHeaderValue(http,"x-next-page");
    } while (page != "");
}

// -----------------------------------------------------------------------------

// Display contents of _db.issue, _db.issue_description, _db.issue_note tables
// In column view (default) or tree view
static void Main_ShowIssueList() {
    // mark selected issues
    int nmatch=0;
    ind_beg(gitlab::_db_issue_curs, issue, gitlab::_db) {
        issue.select = algo_lib::Regx_Match(gitlab::_db.cmdline.issue, issue.issue)
            || EndsWithQ(issue.issue, gitlab::_db.cmdline.issue.expr);
        // if -assignee and -issue were ommitted,
        // filter issues to include only unassigned ones, and ones assigned to current user
        // otherwise, match assignee as a regular regx
        if (gitlab::_db.cmdline.assignee.expr == "" && gitlab::_db.cmdline.issue.expr == "") {
            issue.select = issue.select && (issue.assignee=="" || issue.assignee==gitlab::_db.unix_user);
        } else if (gitlab::_db.cmdline.assignee.expr != "") {
            issue.select = issue.select &&  Regx_Match(gitlab::_db.cmdline.assignee, issue.assignee);
        }
        nmatch += issue.select;
    }ind_end;

    // load comments for selected issues
    ind_beg(gitlab::_db_issue_curs, issue, gitlab::_db) {
        if (gitlab::_db.cmdline.t && issue.select) {
            LoadIssueNotes(issue);
        }
    }ind_end;

    if (gitlab::_db.cmdline.t) {
        // tree view
        ind_beg(gitlab::_db_issue_curs, issue, gitlab::_db) if (issue.select) {
            gitlab::Issue out;
            issue_CopyOut(issue,out);
            prlog(out);
            cstring str;
            str << "\n";
            InsertIndent(str,issue.c_issue_description->description,1);
            str << "\n";

            ind_beg(gitlab::issue_c_issue_note_curs, note, issue){
                InsertIndent(str,tempstr() << "gitlab.issue_note issue_note:" << note.issue_note,2);
                str << "\n\n";
                InsertIndent(str,note.body,3);
                str << "\n";
            }ind_end;

            prlog(str);
        }ind_end;
    } else {
        cstring out;
        out << "ISSUE\tASSIGNEE\tTITLE\n";
        ind_beg(gitlab::_db_issue_curs, issue, gitlab::_db) if (issue.select) {
            out << issue.issue
                << "\t" << issue.assignee
                << "\t" << issue.title
                << eol;
        }ind_end;
        prlog(Tabulated(out,"\t"));
    }
}

// -----------------------------------------------------------------------------

// Load merge request for particular branch
static u32 CountMrForBranch(strptr branch) {
    u32 count = 0;
    cstring page("1");
    do {
        // prepare REST request
        gitlab::FHttp http;
        http.request_uri << gitlab::_db.rest_api
                         << "/projects/" << gitlab::_db.project_id
                         << "/merge_requests/"
                         << "?source_branch=" << branch
                         << "&state=opened&per_page=100&page="<<page;
        request_header_Alloc(http) << gitlab::_db.http_auth_header;
        request_header_Alloc(http) << "Accept: application/json";

        // execute request
        CurlExec(http);

        // prechecks on output
        vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
             << http.response_status_line);
        vrfy_(http.response_json_parser.root_node);
        vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_array);

        ind_beg(lib_json::node_c_child_curs,mr_obj,*http.response_json_parser.root_node){
            vrfy_(mr_obj.type == lib_json_FNode_type_object);
#if 0 // -- for future
            gitlab::Mr mr;
            mr.mr = tempstr() << gitlab::Issue_Concat_project_iid(_db.cmdline.project, u32_Get(&mr_obj,"iid"));
            mr.title = strptr_Get(&mr_obj,"title");

            vrfy_(gitlab::mr_InsertMaybe(mr));

            gitlab::MrDescription mr_description;
            mr_description.mr = mr.mr;
            mr_description.description = strptr_Get(&mr_obj,"description");

            vrfy_(gitlab::mr_description_InsertMaybe(mr_description));
#endif
            ++count;
        }ind_end;
        page = GetResponseHeaderValue(http,"x-next-page");
    } while (page != "");

    return count;
}

// when getting one mr, there much more information than in getting mr list
static void LoadMrDetails(gitlab::FMr &mr) {
    // prepare REST request
    gitlab::FHttp http;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/merge_requests/" << iid_Get(mr);
    request_header_Alloc(http) << gitlab::_db.http_auth_header;
    request_header_Alloc(http) << "Accept: application/json";

    // execute request
    CurlExec(http);

    // prechecks on output
    vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
         << http.response_status_line);
    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);

    lib_json::FNode &mr_obj = *http.response_json_parser.root_node;
    mr.source_branch = strptr_Get(&mr_obj,"source_branch");
    mr.pipeline_status = strptr_Get(&mr_obj,"head_pipeline.status");
}

// load list of merge requests
static void Main_LoadMrList() {
    cstring page("1");
    do {
        // prepare REST request
        gitlab::FHttp http;
        http.request_uri << gitlab::_db.rest_api
                         << "/projects/" << gitlab::_db.project_id
                         << "/merge_requests?view=simple&state=opened&per_page=100&page="<<page;
        request_header_Alloc(http) << gitlab::_db.http_auth_header;
        request_header_Alloc(http) << "Accept: application/json";

        // execute request
        CurlExec(http);

        // prechecks on output
        vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
             << http.response_status_line);
        vrfy_(http.response_json_parser.root_node);
        vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_array);

        // format output
        ind_beg(lib_json::node_c_child_curs,mr_obj,*http.response_json_parser.root_node){
            vrfy_(mr_obj.type == lib_json_FNode_type_object);
            u32 iid = u32_Get(&mr_obj,"iid");
            strptr title = strptr_Get(&mr_obj,"title");
            gitlab::Mr mr;
            mr.mr = gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project, iid);
            mr.title = title;
            gitlab::mr_InsertMaybe(mr);
        }ind_end;

        // load details
        ind_beg(gitlab::_db_mr_curs, mr, gitlab::_db) {
            LoadMrDetails(mr);
        }ind_end;
        page = GetResponseHeaderValue(http,"x-next-page");
    } while (page != "");
}

// -----------------------------------------------------------------------------

static void Main_DoShowMrList() {
    cstring out;
    out << "MR\tS-BRANCH\tPIPELINE\tTITLE\n";
    ind_beg(gitlab::_db_mr_curs, mr, gitlab::_db) {
        out << mr.mr
            << "\t" << mr.source_branch
            << "\t" << mr.pipeline_status
            << "\t" << mr.title
            << eol;
    }ind_end;
    prlog(Tabulated(out,"\t"));
}

// -----------------------------------------------------------------------------

static u32 GetCurrentUserId() {
    // prepare REST request
    gitlab::FHttp http;
    http.request_uri << gitlab::_db.rest_api
                     << "/user";
    request_header_Alloc(http) << gitlab::_db.http_auth_header;
    request_header_Alloc(http) << "Accept: application/json";

    // execute request
    CurlExec(http);

    // prechecks on output
    vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
         << http.response_status_line);
    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);

    lib_json::FNode &user_obj = *http.response_json_parser.root_node;
    u32 current_user_id = u32_Get(&user_obj,"id");
    vrfy_(current_user_id>0);
    return current_user_id;;
}

// -----------------------------------------------------------------------------

// Function length exception - for now keep it as is --
// it is quite linear and understandable.
// For further, I separete edit, execution, and printing parts
static void Main_DoIssueAdd() {
    u32 current_user_id = GetCurrentUserId();

    cstring file;
    cstring title(gitlab::_db.cmdline.title);
    cstring description(gitlab::_db.cmdline.description);
    if (!title.ch_n) {
        vrfy_(gitlab::_db.editor.ch_n);
        file = DirFileJoin(gitlab::_db.home,".gitlab_issue");
        if (!FileQ(file)) {
            cstring tpl;
            tpl <<
                "WIP: The first line is issue title\n"
                "\n"
                "Subsequent lines are issue description.\n"
                "\n"
                "Keep WIP: if writing has not yet been finished,"
                " you can save, exit, and continue later.\n"
                "\n"
                "Once finished, remove WIP: to submit the issue.\n";

            vrfy_(StringToFile(tpl,file,algo::FileFlags(),false));
        }
        cstring cmdline;

        cmdline << gitlab::_db.editor << " " << file;
        SysCmd(cmdline, FailokQ(false), DryrunQ(false));

        cstring text(Trimmed(FileToString(file)));
        title = Trimmed(Pathcomp(text,"\nLL"));
        description = Trimmed(Pathcomp(text,"\nLR"));
    }
    vrfy(title.ch_n,"Issue title is empty");
    // allow empty description
    //vrfy(description.ch_n,"Issue description is empty");

    if (StartsWithQ(title,"WIP:")) {
        prlog("Issue has not been submitted, you can continue composing it.");
        return;
    }

    // prepare REST request
    gitlab::FHttp http;

    http.request_method = gitlab_FHttp_request_method_POST;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/issues";
    request_header_Alloc(http) << gitlab::_db.http_auth_header;
    request_header_Alloc(http) << "Accept: application/json";
    request_header_Alloc(http) << "Content-Type: application/json";

    lib_json::FNode &obj = lib_json::NewObjectNode(NULL);
    lib_json::NewStringNode(&obj,"title",title);
    lib_json::NewStringNode(&obj,"description",description);
    lib_json::NewNumberNode(&obj,"assignee_id",tempstr()<<current_user_id);
    lib_json::JsonSerialize(&obj,http.request_body);

    // execute request
    CurlExec(http);

    vrfy(http.response_status_code == 201, tempstr() << "Server request has failed:"
         << http.response_status_line);

    // consider operation successful, remove file
    if (file.ch_n) {
        unlink(Zeroterm(file));
    }

    // fill data from response
    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);
    lib_json::FNode &issue_obj = *http.response_json_parser.root_node;

    // print result
    gitlab::Issue issue;
    issue.issue = tempstr() << gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project, u32_Get(&issue_obj,"iid"));
    issue.title = strptr_Get(&issue_obj,"title");
    issue.assignee = strptr_Get(&issue_obj,"assignee.username");
    prlog(issue);
    cstring str;
    str << "\n";
    InsertIndent(str,strptr_Get(&issue_obj,"description"),1);
    str << "\n";
    prlog(str);

    // Assign command line argument to make possible further operations in one shot
    gitlab::_db.cmdline.issue.expr = issue.issue;
}

// -----------------------------------------------------------------------------

static tempstr GitCmd(strptr gitcmd) {
    tempstr cmd("git");
    if (gitlab::_db.cmdline.gitdir.ch_n) {
        cmd << " -C " << gitlab::_db.cmdline.gitdir;
    }
    cmd << " " << gitcmd;
    return cmd;
}

// -----------------------------------------------------------------------------

// Interpret -issue argument as a number, or specific issue id
// gitlab -issue 33           --> return 33
// gitlab -issue myproject.33 --> return 33
// gitlab -issue %            --> return 0
int gitlab::IssueArgNumber() {
    strptr expr = Pathcomp(_db.cmdline.issue.expr, ".RR");
    i32 ret=0;
    i32_ReadStrptrMaybe(ret,expr);
    return ret;
}

// -----------------------------------------------------------------------------

// same for mr
int gitlab::MrNumber(strptr in) {
    strptr expr = Pathcomp(in, ".RR");
    i32 ret=0;
    i32_ReadStrptrMaybe(ret,expr);
    return ret;
}

// -----------------------------------------------------------------------------

u32 gitlab::GetUserId(strptr user) {
    gitlab::FUser *fuser = gitlab::ind_user_Find(user);
    return fuser ? fuser->id : 0;
}

// -----------------------------------------------------------------------------

static void Main_DoIssueCommentAdd() {
    cstring body(gitlab::_db.cmdline.comment);
    bool use_editor = !body.ch_n;
    int issuenum = gitlab::IssueArgNumber();
    cstring file = DirFileJoin(gitlab::_db.home,".gitlab_issue_note");
    if (use_editor) {
        vrfy_(gitlab::_db.editor.ch_n);
        cstring cmdline;
        cmdline << gitlab::_db.editor << " " << file;
        SysCmd(cmdline, FailokQ(false), DryrunQ(false));

        body = Trimmed(FileToString(file,algo::FileFlags()));
        vrfy(body.ch_n,"Issue note is empty");

        if (StartsWithQ(body,"WIP:")) {
            prlog("Issue note has not been submitted, you can continue composing it.");
            return;
        }
    }

    // prepare REST request
    gitlab::FHttp http;

    http.request_method = gitlab_FHttp_request_method_POST;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/issues/" << issuenum
                     << "/notes";
    request_header_Alloc(http) << gitlab::_db.http_auth_header;
    request_header_Alloc(http) << "Accept: application/json";
    request_header_Alloc(http) << "Content-Type: application/json";

    lib_json::FNode &obj = lib_json::NewObjectNode(NULL);
    lib_json::NewStringNode(&obj,"body",body);
    lib_json::JsonSerialize(&obj,http.request_body);

    // execute request
    CurlExec(http);

    vrfy(http.response_status_code == 201, tempstr() << "Server request has failed:"
         << http.response_status_line);

    // consider operation successful, remove file
    if (use_editor) {
        unlink(Zeroterm(file));
    }

    // fill data from response
    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);
    lib_json::FNode &issue_obj = *http.response_json_parser.root_node;

    // print result
    cstring issue_note;
    issue_note << gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project,u32_Get(&issue_obj,"noteable_iid"))
               << "."
               << u32_Get(&issue_obj,"id");
    cstring str;
    InsertIndent(str,tempstr() << "gitlab.issue_note issue_note:" << issue_note,2);
    str << "\n\n";
    InsertIndent(str,strptr_Get(&issue_obj,"body"),3);
    str << "\n";
    prlog(str);
}

// -----------------------------------------------------------------------------

static void Main_DoIssueClose() {
    int issuenum = gitlab::IssueArgNumber();

    // prepare REST request
    gitlab::FHttp http;

    http.request_method = gitlab_FHttp_request_method_PUT;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/issues/" << issuenum
                     << "?state_event=close";
    request_header_Alloc(http) << gitlab::_db.http_auth_header;

    // execute request
    CurlExec(http);
    vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
         << http.response_status_line);

    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);
    lib_json::FNode &issue_obj = *http.response_json_parser.root_node;
    strptr state = strptr_Get(&issue_obj,"state");

    tempstr issue = gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project,issuenum);
    vrfy(state == "closed", tempstr() << Keyval("issue",issue) << Keyval("state",state));
    prlog("gitlab.success" << Keyval("issue",issue) << Keyval("state",state));
}

// -----------------------------------------------------------------------------

static void LoadUsers() {
    cstring page("1");
    do {
        // prepare REST request
        gitlab::FHttp http;
        http.request_uri << gitlab::_db.rest_api
                         << "/users?active=true&per_page=100&page="<<page;
        request_header_Alloc(http) << gitlab::_db.http_auth_header;
        request_header_Alloc(http) << "Accept: application/json";

        // execute request
        CurlExec(http);

        // prechecks on output
        vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
             << http.response_status_line);
        vrfy_(http.response_json_parser.root_node);
        vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_array);

        ind_beg(lib_json::node_c_child_curs,user_obj,*http.response_json_parser.root_node){
            vrfy_(user_obj.type == lib_json_FNode_type_object);
            gitlab::User user;
            user.user = strptr_Get(&user_obj,"username");
            user.id = u32_Get(&user_obj,"id");
            user.name = strptr_Get(&user_obj,"name");
            gitlab::user_InsertMaybe(user);
        }ind_end;
        page = GetResponseHeaderValue(http,"x-next-page");
    } while (page != "");
}

// -----------------------------------------------------------------------------

static void MaybeLoadUserList() {
    if (!gitlab::user_N()) {
        LoadUsers();
    }
}

// -----------------------------------------------------------------------------

static void Main_ShowUserList() {
    cstring out;
    out << "USER\tID\tNAME\n";
    ind_beg(gitlab::_db_user_curs, user, gitlab::_db) {
        out << user.user
            << "\t" << user.id
            << "\t" << user.name
            << eol;
    }ind_end;
    prlog(Tabulated(out,"\t"));
}

// -----------------------------------------------------------------------------

static void Main_DoIssueAssign() {
    int issuenum = gitlab::IssueArgNumber();

    u32 assignee_id = gitlab::GetUserId(gitlab::_db.cmdline.iassignto);
    vrfy(assignee_id, tempstr() << "User " << gitlab::_db.cmdline.iassignto << " does not exist, see -ulist");

    // prepare REST request
    gitlab::FHttp http;

    http.request_method = gitlab_FHttp_request_method_PUT;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/issues/" << issuenum
                     << "?assignee_id=" << assignee_id;
    request_header_Alloc(http) << gitlab::_db.http_auth_header;

    // execute request
    CurlExec(http);
    vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
         << http.response_status_line);

    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);
    lib_json::FNode &issue_obj = *http.response_json_parser.root_node;
    u32 id = u32_Get(&issue_obj,"assignee.id");
    strptr username = strptr_Get(&issue_obj,"assignee.username");

    tempstr issue = gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project,issuenum);
    vrfy(id == assignee_id, tempstr() << Keyval("issue",issue) << Keyval("assignee",username));
    prlog("gitlab.success" << Keyval("issue",issue) << Keyval("assignee",username));
}

// -----------------------------------------------------------------------------

static void AssertGitWorkDirClean() {
    bool clean = !SysCmd(GitCmd("update-index --refresh >/dev/null"), FailokQ(true), DryrunQ(false))
        && !SysCmd(GitCmd("diff-index --quiet HEAD --"), FailokQ(true), DryrunQ(false));
    vrfy(clean,"Working directory is not clean");
}

// -----------------------------------------------------------------------------

static void Main_DoIssueStart() {
    vrfy_(gitlab::_db.unix_user.ch_n);
    vrfy(gitlab::_db.cmdline.issue.expr != "", "Please specify issue ID");
    int issuenum = gitlab::IssueArgNumber();
    tempstr issuekey = gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project, issuenum);
    gitlab::FIssue *issue = LoadIssueMaybe(issuenum);
    vrfy(issue,tempstr() << "Issue does not exist: " << issuekey);

    cstring commit_comment;
    commit_comment << "Issue #" << issuenum << " " << issue->title
                   << "\n\n" << issue->c_issue_description->description;

    AssertGitWorkDirClean();

    SysCmd(GitCmd("fetch"), FailokQ(false), DryrunQ(false), algo_EchoQ_true);
    tempstr co_cmd;
    co_cmd << "checkout --no-track -b " << issuekey << " origin/master";
    SysCmd(GitCmd(co_cmd), FailokQ(false), DryrunQ(false), algo_EchoQ_true);

    // strptr_ToBash is broken, so using temp file
    algo_lib::FTempfile tempfile;
    TempfileInitX(tempfile,"gitlab");
    StringToFile(commit_comment,tempfile.filename);
    tempstr cmt_cmd;
    cmt_cmd << "commit --allow-empty -F " << GetFullPath(tempfile.filename);
    SysCmd(GitCmd(cmt_cmd), FailokQ(true), DryrunQ(false), algo_EchoQ_true);
}

// -----------------------------------------------------------------------------

static void Main_DoMergeReq() {
    AssertGitWorkDirClean();

    // git branch --show-current doesn't work on older gits
    cstring branch(SysEval(GitCmd("rev-parse --symbolic-full-name HEAD"), FailokQ(false), 1024));
    // convert refs/heads/XX -> XX
    // (use temp string to avoid assigning substrin to itself)
    branch = tempstr() << Pathcomp(Trimmed(branch),"/RR");
    vrfy_(branch != "master");
    SysCmd(GitCmd("push origin HEAD -f"), FailokQ(false), DryrunQ(false), algo_EchoQ_true);
    int n_mr = CountMrForBranch(branch);

    if (n_mr) {
        prlog("Merge request is already existing for this branch.");
        prlog("No worry, branch has updated in merge request.");
        return;
    }

    cstring head_meta(SysEval(GitCmd("show --quiet --pretty=medium"), FailokQ(false), 64*1024));

    LineBuf linebuf;
    algo::memptr data=strptr_ToMemptr(head_meta);
    strptr line;
    LinebufBegin(linebuf,data,false);

    vrfy_(LinebufNext(linebuf,line)); // commit
    vrfy_(LinebufNext(linebuf,line)); // author
    vrfy_(LinebufNext(linebuf,line)); // date
    vrfy(LinebufNext(linebuf,line) && !elems_N(line),tempstr() << "{{"<<line<<"}}"); // empty line

    vrfy(LinebufNext(linebuf,line) && elems_N(line), "First line of commit message is empty!");
    vrfy(FindChar(line,'#')!=-1,"First line of commit messsage does not contain issue reference (#NNN)")

        strptr title(Trimmed(line));
    dbglog("TITILE: "<<title);

    // skip empty
    while (LinebufNext(linebuf,line) && !elems_N(Trimmed(line))) {}

    // need to keep format transparent so skip 4 space chars
    cstring description;
    description << RestFrom(line,4) << "\n";
    while (LinebufNext(linebuf,line)) {
        description << RestFrom(line,4) << "\n";
    }
    //    description << "\nReviewers: @alexei"; //
    dbglog("DESCRIPTION:");
    dbglog(description);

    //
    // submit
    //

    // prepare REST request
    gitlab::FHttp http;

    http.request_method = gitlab_FHttp_request_method_POST;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/merge_requests";
    request_header_Alloc(http) << gitlab::_db.http_auth_header;
    request_header_Alloc(http) << "Accept: application/json";
    request_header_Alloc(http) << "Content-Type: application/json";

    lib_json::FNode &obj = lib_json::NewObjectNode(NULL);
    lib_json::NewStringNode(&obj,"source_branch",branch);
    lib_json::NewStringNode(&obj,"target_branch","master");
    lib_json::NewStringNode(&obj,"title",title);
    lib_json::NewStringNode(&obj,"description",description);
    lib_json::JsonSerialize(&obj,http.request_body);

    // execute request
    CurlExec(http);

    vrfy(http.response_status_code == 201, tempstr() << "Server request has failed:"
         << http.response_status_line);

    // fill data from response
    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);
    lib_json::FNode &mr_obj = *http.response_json_parser.root_node;

    // print result
    gitlab::Mr mr;
    mr.mr = gitlab::Issue_Concat_project_iid(gitlab::_db.cmdline.project, u32_Get(&mr_obj,"iid"));
    mr.title = strptr_Get(&mr_obj,"title");
    prlog(mr);
    cstring str;
    str << "\n";
    InsertIndent(str,strptr_Get(&mr_obj,"description"),1);
    str << "\n";
    prlog(str);
}

// -----------------------------------------------------------------------------

static void Main_DoMrAccept() {
    int mrnum = gitlab::MrNumber(gitlab::_db.cmdline.mraccept);

    // prepare REST request
    gitlab::FHttp http;

    http.request_method = gitlab_FHttp_request_method_PUT;
    http.request_uri << gitlab::_db.rest_api
                     << "/projects/" << gitlab::_db.project_id
                     << "/merge_requests/" << mrnum
                     << "/merge";
    request_header_Alloc(http) << gitlab::_db.http_auth_header;

    // execute request
    CurlExec(http);

    tempstr mr = gitlab::Mr_Concat_project_iid(gitlab::_db.cmdline.project,mrnum);

    vrfy(http.response_status_code != 405,  tempstr() << Keyval("mr",mr)
         << Keyval("reason","Merge request can not be accepted"));
    vrfy(http.response_status_code != 406, tempstr() << Keyval("mr",mr)
         << Keyval("reason","Branch can not be merged"));

    vrfy(http.response_status_code == 200, tempstr() << "Server request has failed:"
         << http.response_status_line);

    vrfy_(http.response_json_parser.root_node);
    vrfy_(http.response_json_parser.root_node->type == lib_json_FNode_type_object);
    lib_json::FNode &issue_obj = *http.response_json_parser.root_node;
    strptr state = strptr_Get(&issue_obj,"state");

    vrfy(state == "merged", tempstr() << Keyval("mr",mr) << Keyval("state",state));
    prlog("gitlab.success" << Keyval("mr",mr) << Keyval("state",state));
}

// -----------------------------------------------------------------------------

void gitlab::Main() {
    Main_ManageEnv();
    Main_ManageAuth();
    Main_ManageServer();
    Main_ManageProject();

    // Default -> display issue list
    if (!(_db.cmdline.iadd  || _db.cmdline.ic || _db.cmdline.istart || _db.cmdline.mrlist || _db.cmdline.mergereq
          || _db.cmdline.iclose || _db.cmdline.ulist || _db.cmdline.iassignto != ""
          || _db.cmdline.mraccept != "")) {
        _db.cmdline.ilist = true;
    }

    if (gitlab::_db.cmdline.ilist) {
        Main_LoadIssueList();
        Main_ShowIssueList();
    }

    if (gitlab::_db.cmdline.iadd) {
        MaybeLoadUserList();
        Main_DoIssueAdd();
    }

    if (gitlab::_db.cmdline.ic) {
        Main_DoIssueCommentAdd();
    }

    if (gitlab::_db.cmdline.istart) {
        Main_DoIssueStart();
    }

    if (gitlab::_db.cmdline.mrlist) {
        Main_LoadMrList();
        Main_DoShowMrList();
    }

    if (gitlab::_db.cmdline.mergereq) {
        Main_DoMergeReq();
    }

    if (gitlab::_db.cmdline.iclose) {
        Main_DoIssueClose();
    }

    if (gitlab::_db.cmdline.ulist) {
        MaybeLoadUserList();
        Main_ShowUserList();
    }

    if (gitlab::_db.cmdline.iassignto != "") {
        MaybeLoadUserList();
        Main_DoIssueAssign();
    }

    if (gitlab::_db.cmdline.mraccept != "") {
        Main_DoMrAccept();
    }
}
