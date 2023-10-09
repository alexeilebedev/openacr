## Gcli - gitlab/github command-line client

Gcli is a client for gitlab/github. It can list, create and update issues,
merge requests (MRs), pull requests, list MR jobs, list users, list remote repos, update local
git config to access those repos, create and update issue & MR notes, list milestones.

### Syntax

```
inline-command: gcli -h
gcli: cli utility for gitlab and github
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
    -help                                         Print help an exit; alias -h
    -version                                      Print version and exit
    -signature                                    Show signatures and exit; alias -sig

```

### Operation

The main command structure is `gcli [selectpr] [fields]`. Selector is a key-value
pair `[table]:[key]` identifying the table on which to perform an action;
`[fields] is an optional list of additional key-value pairs used when updating or filtering the table.

Table is by default `issue`, but can be any table from the list provided by `gcli help`.
Key is a regex. Actions can be `-create`, `-update`, `-start`, `-e`, etc. 

### Getting help (beyond command syntax)

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

### Getting detailed help

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

### Accessing repository

The list of repositories is stored in ~/.ssim/gclidb/grepo.ssim
Access to a new host can be established by running

```
gcli repo -create token:<token> host:<host>
```

This fetches a list of available repositories and adds them all to the list in `grepo.ssim`

### Listing remote repositories

This command uses a token to fetch a list of available repositories from remote service
and display them. It doesn't access local state.

```
gcli reporemote token:<token> [host:<host>]
```

### Listing known repositories

The list of known repositories can be displayed with `gcli repo`
and examined/edited with `acr -in ~/.ssim grepo`

### Setting default remote repo

Gcli addresses all requests to the default repo. The repo can be switched with

```
gcli repo -update <repo>
```

The output of the command lists all known repos, and shows which one is the default

### Editing list of repos

```
gcli repo -update
```

TODO: this should be `gcli repo -e`, `-update` should do nothing

### Listing issues

```
gcli
```

### Listing a single issue with details (notes etc)

```
gcli <issuekey> -t
```

where issuekey is in form of <repo>.<issue number> or just <issue number>.

### Creating a new issue

```
gcli -create
```

### Editing an existing issue

```
gcli <issuekey> -e
```

### Reopening an issue

```
gcli <issuekey> -update status:opened
```

### Changing issue assignee

Multiple fields can be updated with the same command.
For safety, the `-update` command only works with a single issue.

```
gcli <issuekey> -update assignee:<assignee>
```

### Displaing issues with filtering

```
gcli <repo>.% author:<author>
```

### Starting working on an issue

Gcli will ensure that the current directory is clean (no modifications),
fetch remote repo, create a local branch with a name matching issue key,
set issue state to `in_progress`, and switch to the new branch.

If the branch already exists, no local changes are performed

```
gcli <issuekey> -start
```

### Showing multiple issues with filtering

```
gcli <repo>.% assignee:<aignee> state:opened
```

### Listing merge requests

```
gcli mr
```

### Showing status of CI pipelines for merge request

To show details of a merge request, including any of its running pipelines,
use `-t`

```
gcli mr:<mrkey> -t
```

Where mrkey is in form <repo>.<MR nubmer>


### Showing output of a CI pipeline

To show the output of an mrjob, use 

```
gcli mrjob:mr:<mrkey>/<jobname>
```

### Scanning all CI pipelines for errors

You can use a regex with mrjob, e.g. 

```
mr:<mrkey>/%
```

This is useful when searching CI output for a particular error string

```
$ gcli mrjob:mr:<mrkey>/% | grep success:N
```

### Creating merge request

Gcli will push current branch to the remote repo and create or update a merge request for it.
The issue name is determined by looking at the current branch name.

The comment for the most recent git commit must reference the issue and agree
with current branch name. For instance, if you are on a branch `proj.117`, the comment
for HEAD must contain the string `proj#117`. Use `git commit --amend` to edit the commit comment.

If a merge request already exists, it is automatically refreshed.
By default, merge requests are prefixed with `Draft: ` making them unmergeable.
A reviewer must approve the merge request (or remove draft status through gitlab/github browser
interface) to continue.

```
gcli mr -create
```

### Reviewing merge request

The verbs `-start`, `-approve`, `-needs_work` are used with the review workflow.
To begin reviewing a merge request, use

```
gcli mr:<mrkey> -start
```

### Approving merge request

When you approve a merge request, the Draft prefix is lifted from the MR title

```
gcli mr:<mrkey> -approve
```

### List remote users

```
gcli user
```

### Input tables

Gcli inputs the following tables

```
inline-command: acr -report:N ctype:$(acr_in gcli | acr -in:- ssimfile -regxof ctype) | ssimfilt -t
CTYPE                COMMENT
gclidb.Gact          Action that may be performed on a table
gclidb.Gclicmdf2j    Mapping of internal fields to JSON fields for gitlab/github
gclidb.Gclicmdt      Internal test
gclidb.Gfld          Gitlab/github field name
gclidb.Gmethod       HTTP method list
gclidb.Grepo         Repo table (acr grepo -in ~/.ssim)
gclidb.Grepogitport  Default ports for repositories
gclidb.Grepossh      Ssh key table
gclidb.Gstatet       Internal
gclidb.Gtbl          Supported gcli tables
gclidb.Gtblactfld    List of available for each table & action combination
gclidb.Gtype         Platform type (ghp=github; glpat=gitlab)
gclidb.Gtypeh        HTTP heaaderes for user with platform
gclidb.Gtypeprefix   Token prefix mapping to platform

```
