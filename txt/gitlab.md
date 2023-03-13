## gitlab: Utility to help development workflow around gitlab

### Introduction
`gitlab` is Command Line Interface command to a gitlab instance. It covers basic operations with gitlab, per a project:
listing users, listing issues, 
creating an issue, commenting on an issue, assigning an issue to a user,
starting work on an issue - pulling from project's git repo and creating a git branch with right comments
requesting a merge -  pushing branch to project's git repo
accepting an issue - rebasing issues branch to master and pushing it up to the git repo of the project.

This iteration of `gitlab` works with multiple projects. It identifies the project from an issue name, or, if absent,
it takes the first "active" line from ~/.ssim/dev/gitlab_auth.ssim, created by a project init.
For example `gitlab -iadd` will pick up first active project, and `gitlab -iadd P2.%` will pick up project P2.

Below is a list of all `gitlab` parameters, followed by some use examples.

```
inline-command: gitlab -h


gitlab: Gitlab command line interface
Usage: gitlab [options]
    -in           string  Input directory or filename, - for stdin. default: "data"
    [issue]       string  Issue id (e.g. myproject.33, 33 or %). default: "%"
    -mrlist               (action) Show list of merge requests. default: false
    -mergereq             (action) Push current branch to project git repo, create merge request. default: false
    -ilist                (action) Show list of issues matching regx. default: false
    -istart               (action) Start working on the specified issue. default: false
    -t                    Tree view: expand issue description. default: false
    -iadd                 (action) Add new issue. default: false
    -ic                   (action) Add issue comment. default: false
    -iclose               (action) Close issue. default: false
    -iassignto    string  (action) Assign issue to user
    -title        string  (with -iadd), skip editor and use argument as title
    -description  string  (with -iadd -title), use argument as description
    -comment      string  (with -ic), skip editor and use argument as comment
    -gitdir       string  (setup) Change directory of git repository
    -assignee     string  Filter assignee. Default=current user only
    -ulist                (action) List users. default: false
    -mraccept     string  (action) Accept merge request
    -auth_token   string  (init) GitLab auth token, requires server, adds record to  ~/.ssim/dev/gitlab_auth.ssim
    -server       string  (init) GitLab url, like http://localhost, requires auth_token
    -verbose              Enable verbose mode
    -debug                Enable debug mode
    -version              Show version information
    -sig                  Print SHA1 signatures for dispatches
    -help                 Print this screen and exit

```

Examples: 
Operate between two gitlae projects, P3 and P2:

Create 1st project in gitlab via gitlab GUI: project P3 for example.
Create P3 project access token via GUI, say it is glpat-eaGJw951ig77WbcFvpxg
Switch default branch from main to master if desired
run gitlab init command:
```
command: gitlab -server:http://localhost -auth_token:glpat-eaGJw951ig77WbcFvpxg
```
to check that the settings are installed, run
```
command: acr -in:$HOME/.ssim  gitlab_auth:P3/%
dev.gitlab_auth  gitlab_auth:P3/glpat-eaGJw951ig77WbcFvpxg  url:http://localhost  default_branch:""  id:5  name_with_namespace:"VP / P3"  ssh_url_to_repo:git@localhost:vp/p3.git  web_url:http://localhost/vp/p3  active:Y
report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
```
```
command: git remote -v show P3
* remote P3
  Fetch URL: git@localhost:vp/p3.git
  Push  URL: git@localhost:vp/p3.git
  HEAD branch: master
  Remote branch:
    master new (next fetch will store in remotes/P3)
  Local ref configured for 'git push':
    master pushes to master (local out of date)
```

Create 2nd project in gitlab via gitlab GUI: project P2 for example.
Create P2 project access token via GUI, say it is glpat-ruh2p-YDQzHb5bC2dZa3
Switch default branch from main to master if desired
run gitlab init command:
```
command: gitlab -server:http://localhost -auth_token:glpat-QytvENDH3dYzNs6KTbH2
```
to check that the settings are installed, run
```
command: acr -in:$HOME/.ssim  gitlab_auth:P2/%
dev.gitlab_auth  gitlab_auth:P2/glpat-QytvENDH3dYzNs6KTbH2  url:http://localhost  default_branch:""  id:4  name_with_namespace:"VP / P2"  ssh_url_to_repo:git@localhost:vp/p2.git  web_url:http://localhost/vp/p2  active:Y
report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
```
```
command: git remote -v show P2
* remote P2
  Fetch URL: git@localhost:vp/p2.git
  Push  URL: git@localhost:vp/p2.git
  HEAD branch: master
  Remote branch:
    master new (next fetch will store in remotes/P2)
  Local ref configured for 'git push':
    master pushes to master (local out of date)
```

Create an issue in project P2
```
command: gitlab -iadd P2.%
ISSUE  ASSIGNEE       TITLE
P2.1   project_4_bot  Issue Example
```

Create an issue in project P3
```
command: gitlab -iadd P3.%
ISSUE  ASSIGNEE       TITLE
P3.1   project_4_bot  Issue Example
```

Assign issue P3.1 to a user etc...

