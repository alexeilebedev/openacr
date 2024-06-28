## gcli - Gcli - gitlab/github command-line client
<a href="#gcli"></a>

Gcli is a client for gitlab/github. It can list, create and update issues,
merge requests (MRs), pull requests, list MR jobs, list users, list remote repos, update local
git config to access those repos, create and update issue & MR notes, list milestones.
Examples are provided below

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Operation](#operation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: Get help (beyond command syntax)](#example-get-help--beyond-command-syntax-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: Get detailed help](#example-get-detailed-help)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: create access to repository](#example-create-access-to-repository)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list remote repositories](#example-list-remote-repositories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list known repositories](#example-list-known-repositories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: set default remote repo](#example-set-default-remote-repo)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: edit list of repos](#example-edit-list-of-repos)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list issues](#example-list-issues)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list a single issue with details (notes etc)](#example-list-a-single-issue-with-details--notes-etc-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: create a new issue](#example-create-a-new-issue)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: edit an existing issue](#example-edit-an-existing-issue)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: reopen an issue](#example-reopen-an-issue)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: change issue assignee](#example-change-issue-assignee)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: display issues with filtering](#example-display-issues-with-filtering)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: start working on an issue](#example-start-working-on-an-issue)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: show multiple issues with filtering](#example-show-multiple-issues-with-filtering)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list merge requests](#example-list-merge-requests)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: show status of CI pipelines for merge request](#example-show-status-of-ci-pipelines-for-merge-request)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: show output of a CI pipeline](#example-show-output-of-a-ci-pipeline)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: scan all CI pipelines for errors](#example-scan-all-ci-pipelines-for-errors)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: create merge request](#example-create-merge-request)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: start reviewing merge request](#example-start-reviewing-merge-request)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: approve merge request](#example-approve-merge-request)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list remote users](#example-list-remote-users)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&#128196; [gcli - Internals](/txt/exe/gcli/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
gcli: Gcli - gitlab/github command-line client
Usage: gcli [[-selector:]<string>] [[-fields:]<string>] [options]
    OPTION                     TYPE    DFLT       COMMENT
    -in                        string  "data"     Input directory or filename, - for stdin
    [selector]                 string  "issue:%"  table:key, where table is issue,repo,mr, etc. and key is a regex.
    [fields]...                string             additional key:value pairs for use with -create, -list, -update
    -accept                                       (action) accept a slected merge request
    -start                                        (action) start working on a selected issue
    -list                                         (action) show selected table
    -create                                       (action) create a table record(s)
    -update                                       (action) update fields of a selected issue or mergereq
    -approve                                      (action) remove draft desiognation from mergereq
    -needs_work                                   (action) reopen an issue or put a draft designation on mergereq
    -stop                                         (action) closes an issue, or remove mr branch after review
    -t                                            Tree view: expand issue description
    -e                                            edit the input
    -authdir                   string  ".ssim"    (setup) Input directory for auth data
    -dry_run                                      Print actions, do not perform
    -gitdir                    string  ""         (setup) Change directory of git repository
    -show_gitlab_system_notes                     (misc) Show issue and mr notes created by gitlab
    -verbose                   int                Verbosity level (0..255); alias -v; cumulative
    -debug                     int                Debug level (0..255); alias -d; cumulative
    -help                                         Print help and exit; alias -h
    -version                                      Print version and exit
    -signature                                    Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Operation
<a href="#operation"></a>

The main command structure is `gcli [selectpr] [fields]`. Selector is a key-value
pair `[table]:[key]` identifying the table on which to perform an action;
`[fields] is an optional list of additional key-value pairs used when updating or filtering the table.

Table is by default `issue`, but can be any table from the list provided by `gcli help`.
Key is a regex. Actions can be `-create`, `-update`, `-start`, `-e`, etc.

### Example: Get help (beyond command syntax)
<a href="#example-get-help--beyond-command-syntax-"></a>

```
inline-command: gcli help
TABLE       USE/DFLT  COMMENT
gitconfig             git remote configuration
help                  this help screen
issue                 issues in github/gitlab
issuenote             issues notes/comments
milestone             milestone in github/gitlab
mr                    merge requests/pulls in github/gitlab
mrjob                 lists CI jobs triggered by mr, shows jobs "trace"
mrnote                mr notes/comments
repo                  repositories configured locally in github/gitlab
reporemote            repositories in github/gitlab visible for access tokens
user                  users in github/gitlab

use help[:table] -t for more detailed help, ex "help:issue -t" or "help:help -t"
shortcuts: "." selector represents current branch, ".." selector represents current branch with "." to use for notes
```

### Example: Get detailed help
<a href="#example-get-detailed-help"></a>

One of the tables gcli supports is `help`. The value is table name:

```
inline-command: gcli help:mr
TABLE                     USE/DFLT  COMMENT
mr                                  merge requests/pulls in github/gitlab
  -list                             [-fields:][reviewer:value],[author:value],[labels:value],[milestone:value],<state:value>,[title:value] [-t]
     [reviewer:value]               show mrs for an reviewer, 'none' for unassigned mrs
     [author:value]                 show mr created by this author
     [labels:value]                 show mr with the labels
     [milestone:value]              show mr for a milestone, 'none' for mr_list w/o milestone
     <state:value>        open      show mr for a given state, default 'open'
     [title:value]                  show mr with the pattern in the title
  -create                           [-fields:][reviewer:value] [-t]
     [reviewer:value]               create mr for an reviewer, 'none' for unassigned mr
  -start
  -approve
  -accept
  -update                           [-fields:][reviewer:value],[title:value],[description:value] [-t] [-e]
     [reviewer:value]               update mr for an reviewer, 'none' to clear
     [title:value]                  update mr title
     [description:value]            update mr description
  -needs_work
  -stop
mrjob                               lists CI jobs triggered by mr, shows jobs "trace"
  -list                             [-t]
mrnote                              mr notes/comments
  -create                           [-fields:]<[note:]value> [-t] [-e]
     <[note:]value>                 note for the specified mr
  -update                           [-fields:][[note:]value] [-t] [-e]
     [[note:]value]                 note to overwrite the existing note
  -list                             [-t]

```

### Example: create access to repository
<a href="#example-create-access-to-repository"></a>

The list of repositories is stored in ~/.ssim/gclidb/grepo.ssim
Access to a new host can be established by running

```
gcli repo -create token:<token> host:<host>
```

This fetches a list of available repositories and adds them all to the list in `grepo.ssim`

### Example: list remote repositories
<a href="#example-list-remote-repositories"></a>

This command uses a token to fetch a list of available repositories from remote service
and display them. It doesn't access local state.

```
gcli reporemote token:<token> [host:<host>]
```

### Example: list known repositories
<a href="#example-list-known-repositories"></a>

The list of known repositories can be displayed with `gcli repo`
and examined/edited with `acr -in ~/.ssim grepo`

### Example: set default remote repo
<a href="#example-set-default-remote-repo"></a>

Gcli addresses all requests to the default repo. The repo can be switched with

```
gcli repo -update arnd
```

The output of the command lists all known repos, and shows which one is the default

### Example: edit list of repos
<a href="#example-edit-list-of-repos"></a>

```
gcli repo -update
```

TODO: this should be `gcli repo -e`, `-update` should do nothing

### Example: list issues
<a href="#example-list-issues"></a>

```
gcli
```

### Example: list a single issue with details (notes etc)
<a href="#example-list-a-single-issue-with-details--notes-etc-"></a>

```
gcli <issuekey> -t
```

### Example: create a new issue
<a href="#example-create-a-new-issue"></a>

```
gcli -create
```

### Example: edit an existing issue
<a href="#example-edit-an-existing-issue"></a>

```
gcli arnd.117 -e
```

### Example: reopen an issue
<a href="#example-reopen-an-issue"></a>

```
gcli arnd.117 -update status:opened
```

### Example: change issue assignee
<a href="#example-change-issue-assignee"></a>

Multiple fields can be updated with the same command.
For safety, the `-update` command only works with a single issue.

```
gcli arnd.117 -update assignee:alexei
```

### Example: display issues with filtering
<a href="#example-display-issues-with-filtering"></a>

```
gcli arnd.% author:apolov
```

### Example: start working on an issue
<a href="#example-start-working-on-an-issue"></a>

Gcli will ensure that the current directory is clean (no modifications),
fetch remote repo, create a local branch with a name matching issue key,
set issue state to `in_progress`, and switch to the new branch.

If the branch already exists, no local changes are performed

```
gcli 117 -start
```

### Example: show multiple issues with filtering
<a href="#example-show-multiple-issues-with-filtering"></a>

```
gcli arnd.% assignee:alexei state:opened
```

### Example: list merge requests
<a href="#example-list-merge-requests"></a>

```
$ gcli mr
MR          ISSUE     AUTHOR  REVIEWER  PIPELINE  STATE   TITLE
mr:arnd.91  arnd.119  apolov            success   opened  Issue arnd#119
mr:arnd.84  arnd.126  alexei            failed    opened  issue #126
mr:arnd.77  arnd.121  alexei  vparizhs  success   opened  Issue #arnd.121
```

### Example: show status of CI pipelines for merge request
<a href="#example-show-status-of-ci-pipelines-for-merge-request"></a>

To show details of a merge request, including any of its running pipelines,
use `-t`

```
gcli mr:arnd.91 -t
MR                    ISSUE     AUTHOR           REVIEWER  PIPELINE  STATE   TITLE
mr:arnd.91            arnd.119  apolov                     success   opened  Issue arnd#119

DESCRIPTION
create patch/thash-dl.diff: makes collision list doubly linked
performance doesn't justify inclusion
add atf_amc tests for thash
closes arnd#119

MRNOTE                AUTHOR
mr:arnd.91.3630       apolov
![image](/uploads/c3fbc5adaf145ce178862d0033970d91/image.png)

Performance of Thash Remove DLL is certainly worse than SLL.

NOT RECOMMENDED TO BE MERGED:
- no any benefit;
- rewrites table thash, which makes impossible to merge/rebase normally.

Only Thash SLL tests could be taken those were implemented in scope of this issue.


MRJOB                 STATUS    RUNNER
mr:arnd.91/memcheck   success   gitlab-runner-2
mr:arnd.91/comp       success   gitlab-runner-4
mr:arnd.91/normalize  success   gitlab-runner-2
mr:arnd.91/build      success   gitlab-runner-4
```

### Example: show output of a CI pipeline
<a href="#example-show-output-of-a-ci-pipeline"></a>

To show the output of an mrjob, use `gcli mrjob:...`

```
$ gcli mrjob:mr:arnd.91/memcheck | head
Running with gitlab-runner 16.4.0 (4e724e03)
  on gitlab-runner-2 TDXuZ4xYU, system ID: s_ca1266e3726f
  Preparing the "shell" executor
  Using Shell (bash) executor...
  Preparing environment
  Running on gitlab-runner-2.ctwsl1.vovaco.com...
  Getting source from Git repository
  Fetching changes with git depth set to 20...
  Reinitialized existing Git repository in /home/gitlab-runner/builds/TDXuZ4xYU/0/algornd/arnd/.git/
  Checking out a49a3f69 as detached HEAD (ref is arnd.119)...
```

### Example: scan all CI pipelines for errors
<a href="#example-scan-all-ci-pipelines-for-errors"></a>

You can use a regex with mrjob, e.g. `mr:arnd.91/%`
This is useful when searching CI output for a particular error string

```
$ gcli mrjob:mr:arnd.91/% | grep success:N
```

### Example: create merge request
<a href="#example-create-merge-request"></a>

Gcli will push current branch to the remote repo and create or update a merge request for it.
The issue name is determined by looking at the current branch name.

The comment for the most recent git commit must reference the issue and agree
with current branch name. For instance, if you are on a branch `arnd.117`, the comment
for HEAD must contain the string `#arnd.117`. Use `git commit --amend` to edit the commit comment.

If a merge request already exists, it is automatically refreshed.
By default, merge requests are prefixed with `Draft: ` making them unmergeable.
A reviewer must approve the merge request (or remove draft status through gitlab/github browser
interface) to continue.

```
gcli mr -create
```

### Example: start reviewing merge request
<a href="#example-start-reviewing-merge-request"></a>

The verbs `-start`, `-approve`, `-needs_work` are used with the review workflow.
To begin reviewing a merge request, use

```
gcli mr:arnd.121 -start
```

### Example: approve merge request
<a href="#example-approve-merge-request"></a>

When you approve a merge request, the Draft prefix is lifted from the MR title
```
gcli mr:arnd.121 -approve
```

### Example: list remote users
<a href="#example-list-remote-users"></a>

```
gcli user
```

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -selector -- table:key, where table is issue,repo,mr, etc. and key is a regex.
<a href="#-selector"></a>

#### -fields -- additional key:value pairs for use with -create, -list, -update
<a href="#-fields"></a>

#### -accept -- (action) accept a slected merge request
<a href="#-accept"></a>

#### -start -- (action) start working on a selected issue
<a href="#-start"></a>

#### -list -- (action) show selected table
<a href="#-list"></a>

#### -create -- (action) create a table record(s)
<a href="#-create"></a>

#### -update -- (action) update fields of a selected issue or mergereq
<a href="#-update"></a>

#### -approve -- (action) remove draft desiognation from mergereq
<a href="#-approve"></a>

#### -needs_work -- (action) reopen an issue or put a draft designation on mergereq
<a href="#-needs_work"></a>

#### -stop -- (action) closes an issue, or remove mr branch after review
<a href="#-stop"></a>

#### -t -- Tree view: expand issue description
<a href="#-t"></a>

#### -e -- edit the input
<a href="#-e"></a>

#### -authdir -- (setup) Input directory for auth data
<a href="#-authdir"></a>

#### -dry_run -- Print actions, do not perform
<a href="#-dry_run"></a>

#### -gitdir -- (setup) Change directory of git repository
<a href="#-gitdir"></a>

#### -show_gitlab_system_notes -- (misc) Show issue and mr notes created by gitlab
<a href="#-show_gitlab_system_notes"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`gcli` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[gclidb.gtype](/txt/ssimdb/gclidb/gtype.md)|Platform type (ghp=github; glpat=gitlab)|
|[gclidb.gtypeprefix](/txt/ssimdb/gclidb/gtypeprefix.md)|Token prefix mapping to platform|
|[gclidb.gtypeh](/txt/ssimdb/gclidb/gtypeh.md)|HTTP heaaderes for user with platform|
|[gclidb.gact](/txt/ssimdb/gclidb/gact.md)|Action that may be performed on a table|
|[gclidb.gtbl](/txt/ssimdb/gclidb/gtbl.md)|Supported gcli tables|
|[gclidb.gfld](/txt/ssimdb/gclidb/gfld.md)|Gitlab/github field name|
|[gclidb.gtblactfld](/txt/ssimdb/gclidb/gtblactfld.md)|List of available for each table & action combination|
|[gclidb.gstatet](/txt/ssimdb/gclidb/gstatet.md)|Internal|
|[gclidb.grepossh](/txt/ssimdb/gclidb/grepossh.md)|Ssh key table|
|[gclidb.grepogitport](/txt/ssimdb/gclidb/grepogitport.md)|Default ports for repositories|
|[gclidb.grepo](/txt/ssimdb/gclidb/grepo.md)|Repo table (acr grepo -in ~/.ssim)|
|[gclidb.gmethod](/txt/ssimdb/gclidb/gmethod.md)|HTTP method list|
|[gclidb.gclicmdt](/txt/ssimdb/gclidb/gclicmdt.md)|Internal test|
|[gclidb.gclicmdf2j](/txt/ssimdb/gclidb/gclicmdf2j.md)|Mapping of internal fields to JSON fields for gitlab/github|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gcli/git.cpp](/cpp/gcli/git.cpp)||
|[cpp/gcli/gtblact.cpp](/cpp/gcli/gtblact.cpp)||
|[cpp/gcli/help.cpp](/cpp/gcli/help.cpp)||
|[cpp/gcli/issue.cpp](/cpp/gcli/issue.cpp)||
|[cpp/gcli/main.cpp](/cpp/gcli/main.cpp)||
|[cpp/gcli/milestone.cpp](/cpp/gcli/milestone.cpp)||
|[cpp/gcli/mr.cpp](/cpp/gcli/mr.cpp)||
|[cpp/gcli/note.cpp](/cpp/gcli/note.cpp)||
|[cpp/gcli/repo.cpp](/cpp/gcli/repo.cpp)||
|[cpp/gcli/rest.cpp](/cpp/gcli/rest.cpp)||
|[cpp/gcli/search.cpp](/cpp/gcli/search.cpp)||
|[cpp/gcli/user.cpp](/cpp/gcli/user.cpp)||
|[cpp/gen/gcli_gen.cpp](/cpp/gen/gcli_gen.cpp)||
|[include/gcli.h](/include/gcli.h)||
|[include/gen/gcli_gen.h](/include/gen/gcli_gen.h)||
|[include/gen/gcli_gen.inl.h](/include/gen/gcli_gen.inl.h)||

