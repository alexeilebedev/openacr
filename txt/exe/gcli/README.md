## gcli - Gcli - gitlab/github command-line client
<a href="#gcli"></a>

Gcli is a client for gitlab/github. It can list, create and update issues,
merge requests (MRs), pull requests, list MR jobs, list users, list remote repos, update local
git config to access those repos, create and update issue & MR notes, list milestones.
Examples are provided below

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Operation](#operation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Starting](#starting)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: Get help (beyond command syntax)](#example-get-help-beyond-command-syntax-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: Get detailed help](#example-get-detailed-help)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: create access to repository](#example-create-access-to-repository)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list remote repositories](#example-list-remote-repositories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list known repositories](#example-list-known-repositories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: set default remote repo](#example-set-default-remote-repo)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: edit list of repos](#example-edit-list-of-repos)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list issues](#example-list-issues)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: list a single issue with details (notes etc)](#example-list-a-single-issue-with-details-notes-etc-)<br/>
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
&nbsp;&nbsp;&bull;&nbsp;  [Example: babysit a merge request](#example-babysit-a-merge-request)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: manage temporary project access tokens](#example-manage-temporary-project-access-tokens)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [gcli - Internals](/txt/exe/gcli/internals.md)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [gcli - Internals](/txt/gen/gcli/gcli.md)<br/>

### Syntax
<a href="#syntax"></a>
```
gcli: Gcli - gitlab/github command-line client
Usage: gcli [[-selector:]<string>] [[-fields:]<string>] [options]
    OPTION                     TYPE    DFLT       COMMENT
    -in                        string  "data"     Input directory or filename, - for stdin
    [selector]                 string  "issue:%"  table:key, where table is issue,repo,mr, etc. and key is a regex.
    [fields]...                string             additional key:value pairs for use with -create, -list, -update
    -accept                                       (action) accept a selected merge request
    -start                                        (action) start working on a selected issue
    -list                                         (action) show selected table
    -create                                       (action) create a table record(s)
    -update                                       (action) update fields of a selected issue or mergereq
    -approve                                      (action) remove draft desiognation from mergereq
    -needs_work                                   (action) reopen an issue or put a draft designation on mergereq
    -stop                                         (action) closes an issue, or remove mr branch after review
    -retry                                        (action) retry failed CI jobs of a selected mergereq (gitlab)
    -limit                     int     0          For mrjob traces: tail this many lines per job (0 = full trace)
    -t                                            Tree view: expand issue description
    -e                                            edit the input
    -authdir                   string  ".ssim"    (setup) Input directory for auth data
    -dry_run                                      Print actions, do not perform
    -gitdir                    string  ""         (setup) Change directory of git repository
    -show_gitlab_system_notes                     (misc) Show issue and mr notes created by gitlab
    -verbose                   flag               Verbosity level (0..255); alias -v; cumulative
    -debug                     flag               Debug level (0..255); alias -d; cumulative
    -help                                         Print help and exit; alias -h
    -version                                      Print version and exit
    -signature                                    Show signatures and exit; alias -sig
```

### Operation
<a href="#operation"></a>

The main command structure is `gcli [selectpr] [fields]`. Selector is a key-value
pair `[table]:[key]` identifying the table on which to perform an action;
`[fields] is an optional list of additional key-value pairs used when updating or filtering the table.

Table is by default `issue`, but can be any table from the list provided by `gcli help`.
Key is a regex. Actions can be `-create`, `-update`, `-start`, `-e`, etc.

### Starting
<a href="#starting"></a>
First a user have to create access token and ssh key on user's (not prject!) account and run gcli command repo -create to populate grepo table.
`gcli`  is governed by grepo table that is set up by executing gcli repo -create host:xxx token:yyy command.
After the command is successfully executed, gcli gitconfig -create will populate git remote with the right hosts and ports for accessing gitlab or github

execute these commands with -dry_run option to see what they do.

### Example: Get help (beyond command syntax)
<a href="#example-get-help-beyond-command-syntax-"></a>

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
token                 project access tokens (gitlab only)
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
  -list                             [-fields:][errors:value] [-t]
     [errors:value]                 show only error-looking trace lines
  -retry
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
<a href="#example-list-a-single-issue-with-details-notes-etc-"></a>

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
fetch remote repo, create a local branch named by the git-safe issue key
(`/` flattened to `_`, e.g. issue `algornd/arnd.117` gets branch
`algornd_arnd.117`), set issue state to `in_progress`, and switch to the
new branch.  The branch is seeded with an empty commit carrying the issue
title and description and the footer `closes #<iid> (<branch>)`; the same
footer shape gli writes, so either tool recognizes the branch, and the
server closes the issue when the merge request merges.

If a local branch bound to the issue already exists -- named by the issue
key in either form, or carrying the footer -- gcli switches to it instead
of creating a new one.

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
The issue is determined from the branch: the trailing `.<iid>` of an
issue-key-shaped branch name (either form), or the commit footer
`closes #<iid> (<branch>)` for branches named some other way (e.g. by gli).

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

### Example: babysit a merge request
<a href="#example-babysit-a-merge-request"></a>

The MR display carries the fields a babysitting loop needs: the `MERGE`
column shows gitlab's detailed merge status (`mergeable`, `need_rebase`,
`ci_still_running`, ...), and the `BEHIND` column shows how many commits the
source branch is behind the target (single-MR fetch).  The note table of
`mrnote` marks each review thread `resolved` or `unresolved` in the
`RESOLVED` column, so pending review feedback is visible.  Failed CI jobs
are retried with `-retry` (gitlab only): with a bare mr it retries the
failed and canceled jobs; with `mrjob:<mr>/<regex>` the matching jobs
regardless of status.  A job trace prints in full by default;
`-limit:<n>` tails it to the last n lines, and `errors:Y` keeps only
error-looking lines.

```
gcli mr:algornd/arnd.117               # MERGE/BEHIND columns
gcli mrjob:mr:algornd/arnd.117 -retry  # re-run failed jobs
gcli mrjob:mr:algornd/arnd.117/% -limit:50   # tail each job trace
gcli mrjob:mr:algornd/arnd.117/% errors:Y    # error lines only
```

### Example: manage temporary project access tokens
<a href="#example-manage-temporary-project-access-tokens"></a>

GitLab only.  `gcli token` lists the project access tokens of the selected
repo.  `gcli token:<repo>.% -create name:<name>` mints a token with scope
`api` that expires in two days, and prints the created record as an ssim
tuple -- the create response is the only place the secret appears.
`gcli token:<repo>.<id> -stop` revokes a token.  Actions performed with such
a token are attributed to a per-token bot account (`project_<n>_bot_<hash>`),
not to a user.  The gcli comptests (`atf_comp gcli.%`) use these verbs to mint a credential for
the test repo at the start of a run and revoke it at the end, so no token is
stored in the source tree.

```
gcli token:algornd/glpat.% -create name:mykey
gcli token
gcli token:algornd/glpat.50 -stop
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -selector -- table:key, where table is issue,repo,mr, etc. and key is a regex.
<a href="#-selector"></a>

#### -fields -- additional key:value pairs for use with -create, -list, -update
<a href="#-fields"></a>

#### -accept -- (action) accept a selected merge request
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

#### -retry -- (action) retry failed CI jobs of a selected mergereq (gitlab)
<a href="#-retry"></a>

#### -limit -- For mrjob traces: tail this many lines per job (0 = full trace)
<a href="#-limit"></a>

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

### Inputs
<a href="#inputs"></a>
`gcli` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[gclidb.gact](/txt/ssimdb/gclidb/gact.md)|Action that may be performed on a table|
|[gclidb.gclicmdf2j](/txt/ssimdb/gclidb/gclicmdf2j.md)|Mapping of internal fields to JSON fields for gitlab/github|
|[gclidb.gclicmdt](/txt/ssimdb/gclidb/gclicmdt.md)|Internal test|
|[gclidb.gfld](/txt/ssimdb/gclidb/gfld.md)|Gitlab/github field name|
|[gclidb.gmethod](/txt/ssimdb/gclidb/gmethod.md)|HTTP method list|
|[gclidb.grepo](/txt/ssimdb/gclidb/grepo.md)|Repo table (acr grepo -in ~/.ssim)|
|[gclidb.gstatet](/txt/ssimdb/gclidb/gstatet.md)|Internal|
|[gclidb.gtbl](/txt/ssimdb/gclidb/gtbl.md)|Supported gcli tables|
|[gclidb.gtblactfld](/txt/ssimdb/gclidb/gtblactfld.md)|List of available for each table & action combination|
|[gclidb.gtype](/txt/ssimdb/gclidb/gtype.md)|Platform type (ghp=github; glpat=gitlab)|
|[gclidb.gtypeh](/txt/ssimdb/gclidb/gtypeh.md)|HTTP heaaderes for user with platform|
|[gclidb.gtypeprefix](/txt/ssimdb/gclidb/gtypeprefix.md)|Token prefix mapping to platform|
