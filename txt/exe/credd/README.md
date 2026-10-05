## credd - Hold a session's credentials unlocked in memory and hand them to the tools that authenticate
<a href="#credd"></a>

credd keeps the credentials you use from this box, API tokens and ssh keys, in
one store under `~/.ssh/credd`, sealed under one master password.  Unlock the
store once per login with `credd -start`.  From then on a tool that needs a
token asks the daemon for it by name, and every ssh you run finds the keys in
an agent the daemon started.

### Syntax
<a href="#syntax"></a>
```usage
credd: Hold a session's credentials unlocked in memory and hand them to the tools that authenticate
Usage: credd [options]
    OPTION      TYPE    DFLT  COMMENT
    -dir        string  ""    Store directory; empty = ~/.ssh/credd
    -start                    (action) Start the daemon unless one answers: ask the master password, fork, load the ssh agent
    -nostore                  hold no store on disk; the daemon keeps what -seed gives it
    -seed       string  ""    (action) give the running daemon a credential of this name, read per -src
    -stop                     (action) Stop the daemon: empty and kill its ssh agent, forget the key
    -status                   (action) Report whether a daemon answers and what it holds
    -list                     (action) List every credential with its metadata; needs no daemon
    -show       string  ""    (action) Print the named credential's secret, fetched from the daemon
    -check                    (action) List credentials that expire within -window days
    -window     int     14    (with -check) Days ahead
    -add        string  ""    (action) Add a credential: a name takes its secret per -src, % offers each token and key beside the store, a path offers that file
    -linkto     string  ""    (with -add:<name>) Make the new name a link to this credential, so a tool that asks for the new name gets that one's secret
    -kind       string  ""    (with -add:<name>) Kind of credential, a row of creddb.kind: a token provider, sshkey, or link
    -server     string  ""    (with -add:<name>) Server URL of a token
    -account    string  ""    (with -add:<name>) Account a token belongs to
    -perm       string  ""    (with -add:<name>) Permissions a token was issued with, comma-separated
    -expires    string  ""    (with -add:<name>, -keygen, -renew) Expiry as YYYY-MM-DD; empty = never
    -comment    string  ""    (with -add:<name>, -keygen) Comment on the record
    -src        string  "-"   (with -add:<name>, -renew) Where the secret is read: - the terminal, @file a file
    -keygen     string  ""    (action) Make an ed25519 key under this name and add it sealed
    -pubfile    string  ""    (with -keygen, -show) Write the public key to this file, for installing where the key logs in; -show then shows no secret
    -renew      string  ""    (action) Print where the named token is reissued, then take its new value per -src
    -remove     string  ""    (action) Remove the named credential
    -passwd                   (action) Change the master password, resealing every secret
    -dump                     (action) Print the store, secrets still sealed
    -format     enum    auto  (with -list, -dump) Output format; auto is text for -list, ssim for -dump (auto|ssim|text|json)
    -daemon                   Serve the socket in the foreground; -start runs this in a forked child
    -verbose    flag          Verbosity level (0..255); alias -v; cumulative
    -debug      flag          Debug level (0..255); alias -d; cumulative
    -trace      string  ""    Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                     Print help and exit; alias -h
    -version                  Print version and exit
    -signature                Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### The store
<a href="#the-store"></a>

The store is a directory of three ssimfiles under `~/.ssh/credd/creddb`:
`master.ssim`, `cred.ssim` and `credperm.ssim`.  `-dir` names another store
directory.  A credential is a row of [creddb.cred](/txt/ssimdb/creddb/README.md)
holding a name, a kind, the server and account, when it was added and when it
expires, a comment, and the secret.  The secret is the one sealed column, so
`-list`, `-check` and `-dump` read the store without a daemon.

The kinds are the rows of `creddb.kind`.  A token kind is recognized in a file
by the prefix its row states.  An `sshkey` holds a private key as its secret
and the public half in the `pubkey` column.  A `link` is a name that stands for
another credential.

```ssim
inline-command: acr creddb.kind -report:N | ssimfilt -t -field:kind -field:prefix -field:comment
KIND        PREFIX            COMMENT
aws                           AWS access key id, secret and session token, held as one shell fragment
cloudflare  cfut_ cfat_       Cloudflare API token, a user's or an account's
github      ghp_ github_pat_  GitHub classic personal access token
gitlab      glpat-            GitLab personal access token
latitude                      Latitude.sh API key, held as one shell fragment
link                          A name that stands for another credential; the linkto column names it, and there is no secret
openai                        OpenAI organization, project, project key and admin key, held as one shell fragment
sshkey                        An ssh key pair; the public half is the pubkey column, the private half the secret

```

A token's permissions are rows of `creddb.credperm`, and `creddb.perm` lists
the permissions each kind offers.  The master password is stored nowhere.  The
first change to an empty store asks for it twice, and every later change asks
for it once.

#### Adding and changing credentials
<a href="#adding-and-changing-credentials"></a>

`-add:<name>` adds one credential.  At a terminal, credd asks for every option
you did not give; under a pipe it takes what was given.  The secret comes from
where `-src` says, the terminal with echo off or `@file`.  credd refuses a
secret on the command line.  The secret of a multiline kind, an ssh key or
a shell fragment such as aws or openai, is pasted lines that end with an empty
line.  For an ssh key, credd derives its public half.

`-add:%` walks the directory above the store, `~/.ssh` for the default store.
It offers each gitlab token in `gli.ssim`, each `*.token` file the aws tools
source, and each private key file, and asks before adding every one.  After
adding a gitlab token it offers to add a binding line to `~/.ssh/gli.ssim` for
each record, so gli fetches the token from credd, and then asks whether to
blank the tokens in those records.  `-add:<path>`, a value with a slash in it,
offers the one file it names the same way.  Neither form deletes a file.

`-keygen` makes an ed25519 key in the store and prints its public half.
`-renew` prints the steps for reissuing a token at its provider and takes the
new value.  `-remove` drops a credential, and `-passwd` reseals every secret
under a new master password.  Each change is written to the store, and a running
daemon serves it at once.  `-passwd` is the exception: it stops the daemon,
which holds the old key.

#### The daemon
<a href="#the-daemon"></a>

`credd -start` asks the master password and forks a daemon that holds the key
in memory.  The daemon listens on `~/.ssh/credd/credd.sock`, answers your own
user only, and logs to `~/.ssh/credd/credd.log`.  When a daemon already
answers, `-start` says so and exits, so put these two lines in a shell rc and
the password is asked once per login:

```bash
credd -start
export SSH_AUTH_SOCK=$HOME/.ssh/ssh_auth_sock
```

The daemon runs an ssh agent on `~/.ssh/ssh_auth_sock`, beside the store,
holding every key of the store.  A key you add or remove reaches the agent at
once.  A program links [lib_cred](/txt/lib/lib_cred/README.md) and fetches a
secret the way `-show` does, and a link's name answers with the secret of the
credential it stands for.

#### A host with no person on it
<a href="#a-host-with-no-person-on-it"></a>

A CI runner or a service host has nobody to type a master password.  Run credd
there with `-nostore`: the daemon holds what `-seed` gives it for as long as it
lives, and writes nothing.  What stays under the directory afterwards is the
daemon's socket and its log.

### Examples
<a href="#examples"></a>

```bash
credd -add:gl -kind:gitlab -server:https://gitlab.example.com -account:me -perm:api,read_api -expires:2026-12-31 -src:@tok
credd -add:gl                              # at a terminal, asks every option not given
credd -add:gli-mr -linkto:gl               # a second name for the same token
credd -keygen:id_work -comment:"work laptop"   # make an ed25519 key and store it sealed
credd -keygen:id_ci -pubfile:id_ci.pub     # the same, and write the public key to a file
credd -renew:gl                            # the reissue steps, then the new value
credd -remove:gli-mr                       # remove a link, then the credential it stood for
credd -passwd                              # reseal every secret under a new password
credd -add:%                               # offer each token and key already under ~/.ssh
credd -add:$HOME/.ssh/work.token           # offer that one file
credd -list                                # cred, kind, server, comment
credd -list -format:ssim                   # the rows, with permissions under each
credd -check -window:30                    # what expires within 30 days
credd -dump > store.ssim                   # the store, secrets sealed
credd -dump | acr -in:$HOME/.ssh/credd -insert -replace -write   # restore a dump
acr -in:$HOME/.ssh/credd creddb.cred       # the same rows through acr
credd                                      # is a daemon answering, and what it holds
credd -show:gl                             # the secret, bare, so $(credd -show:gl) is the value
gli -auth -cred:gl                         # gli fetches its token from credd
CREDD_PASSWORD=pw credd -start             # answer the prompt from the environment
credd -stop                                # empty the agent and forget the key
```

On a host with no person, one job's credentials live as long as its daemon:

```bash
credd -dir:temp/credd -nostore -start                                   # no store, no password
credd -dir:temp/credd -nostore -seed:ci -kind:sshkey -src:@"$KEYFILE"   # hand it the key
export SSH_AUTH_SOCK=$PWD/temp/ssh_auth_sock                            # ssh finds the agent here
credd -dir:temp/credd -nostore -stop                                    # the key goes with the process
```

### Caveats
<a href="#caveats"></a>

- A daemon outlives every rebuild of credd.  When `-status`, `-start` or a
  change prints `credd.oldbuild`, the daemon runs another build and may not see
  what the new command line writes.  Run `credd -stop`, then `credd -start`.
- `-passwd` stops a running daemon, since it holds the old key.  Start it
  again under the new password.
- Answer no to blanking gli's tokens while any gli built without credd still
  reads `~/.ssh/gli.ssim`.  Such a gli has only the token in the record.
- A bare name given to `-add` is always a credential's name, even when a file
  of that name sits in the current directory.  Give a path with a slash to
  offer a file.
- `-remove` refuses a credential that a link stands for.  Remove the links
  first.
- An ssh key is not renewed.  Make a new one with `-keygen` and `-remove` the
  old one.
- A key behind a passphrase goes in through `-add:<path>`, which asks for the
  passphrase.  `-add:<name>` refuses it.
- Under `-nostore`, every verb that changes a store refuses with
  `credd.nostore`, so nothing is sealed to disk by accident.  A daemon with a
  store refuses `-seed`; add to its store with `-add`.

### Options
<a href="#options"></a>
#### -dir -- Store directory; empty = ~/.ssh/credd
<a href="#-dir"></a>

The store directory, `~/.ssh/credd` when empty.  The daemon's socket and log
live in it, and its ssh agent's socket `ssh_auth_sock` sits beside it, so a
store named here never touches your own agent.  Give the same `-dir` to every
command that talks to that daemon.

#### -start -- (action) Start the daemon unless one answers: ask the master password, fork, load the ssh agent
<a href="#-start"></a>

Start the daemon unless one already answers, in which case credd reports it and
exits.  It asks the master password, or reads `CREDD_PASSWORD`, forks the daemon
and loads its ssh agent.  With `-nostore` it asks nothing and the daemon starts
empty.

#### -nostore -- hold no store on disk; the daemon keeps what -seed gives it
<a href="#-nostore"></a>

Run with no store on disk.  The daemon seals what `-seed` gives it under a
random key and writes nothing, and every verb that would change a store refuses.
Give it on every command aimed at that daemon, with the same `-dir`.

#### -seed -- (action) give the running daemon a credential of this name, read per -src
<a href="#-seed"></a>

Give a running `-nostore` daemon a credential of this name, with its secret read
per `-src` and its kind from `-kind`.  Seeding the same name again replaces the
secret, and seeding it under a different kind is refused.  A seeded ssh key goes
into the agent at once.

#### -stop -- (action) Stop the daemon: empty and kill its ssh agent, forget the key
<a href="#-stop"></a>

Stop the daemon: it empties and kills its ssh agent and forgets the key.  A
`-nostore` daemon's credentials go with it.

#### -status -- (action) Report whether a daemon answers and what it holds
<a href="#-status"></a>

Report whether a daemon answers, where its agent is, how many credentials it
holds and which build it runs.  credd with no action does the same, and prints
`credd.oldbuild` when the daemon is from another build.

#### -list -- (action) List every credential with its metadata; needs no daemon
<a href="#-list"></a>

List every credential with its kind, server and comment, reading the files with
no daemon.  `-format:ssim` prints the rows with the secret written as `sealed`
and each token's permissions under it, and `-format:json` prints the rows as one
array.

#### -show -- (action) Print the named credential's secret, fetched from the daemon
<a href="#-show"></a>

Print the named credential's secret, fetched from the daemon, with no
decoration, so `$(credd -show:name)` is the value.  A link answers with the
secret of the credential it stands for.  With `-pubfile` it writes an ssh key's
public half to that file and shows no secret.

#### -check -- (action) List credentials that expire within -window days
<a href="#-check"></a>

List the credentials whose expiry falls within `-window` days, with the days
left.  A credential that never expires is not listed.  Run it from a shell rc to
hear about a token before it stops working.

#### -window -- (with -check) Days ahead
<a href="#-window"></a>

How many days ahead `-check` looks, 14 by default.

#### -add -- (action) Add a credential: a name takes its secret per -src, % offers each token and key beside the store, a path offers that file
<a href="#-add"></a>

Add a credential.  A name adds one credential and asks at a terminal for what is
missing.  `%` walks the directory above the store and offers each token and key
it finds, and a path with a slash offers that one file.  The options that
describe a credential go with a name only, and the file forms refuse them.

#### -linkto -- (with -add:<name>) Make the new name a link to this credential, so a tool that asks for the new name gets that one's secret
<a href="#-linkto"></a>

With `-add:<name>`, make the new name a link to this credential, so a tool
asking for the new name gets that one's secret.  The target must exist and must
not itself be a link.

#### -kind -- (with -add:<name>) Kind of credential, a row of creddb.kind: a token provider, sshkey, or link
<a href="#-kind"></a>

With `-add:<name>` or `-seed`, the kind of credential, a row of `creddb.kind`.
At a terminal `-add` asks for it when missing, and `-seed` requires it.

#### -server -- (with -add:<name>) Server URL of a token
<a href="#-server"></a>

With `-add:<name>`, the server URL of a token.  `-renew` fills it into the
reissue steps.

#### -account -- (with -add:<name>) Account a token belongs to
<a href="#-account"></a>

With `-add:<name>`, the account a token belongs to.  `-renew` fills it into the
reissue steps.

#### -perm -- (with -add:<name>) Permissions a token was issued with, comma-separated
<a href="#-perm"></a>

With `-add:<name>`, the permissions the token was issued with, comma-separated.
Each must be a permission `creddb.perm` lists for the kind, and a terminal shows
the list when you leave this out.

#### -expires -- (with -add:<name>, -keygen, -renew) Expiry as YYYY-MM-DD; empty = never
<a href="#-expires"></a>

The expiry date as `YYYY-MM-DD`, or never when empty, for `-add:<name>` and
`-keygen`.  With `-renew` a new date replaces the old one, and without it the
old date stands.

#### -comment -- (with -add:<name>, -keygen) Comment on the record
<a href="#-comment"></a>

A comment on the record, for `-add:<name>` and `-keygen`.  `-keygen` also uses
it as the key's ssh comment, defaulting to the name.

#### -src -- (with -add:<name>, -renew) Where the secret is read: - the terminal, @file a file
<a href="#-src"></a>

Where the secret is read, for `-add:<name>`, `-seed` and `-renew`: `-` is the
terminal with echo off or a pipe, and `@file` is a file.  Anything else is
refused, since a secret on a command line is visible in the process list.

#### -keygen -- (action) Make an ed25519 key under this name and add it sealed
<a href="#-keygen"></a>

Make an ed25519 key under this name, add it sealed, and print the public key for
installing where the key logs in.  The private key exists as a file only while
`ssh-keygen` runs, in the store's private temp directory.

#### -pubfile -- (with -keygen, -show) Write the public key to this file, for installing where the key logs in; -show then shows no secret
<a href="#-pubfile"></a>

Write the public key to this file, as `authorized_keys` takes it.  With
`-keygen` it is written besides being printed; with `-show` it is read from the
store with no daemon and no password, and no secret is shown.

#### -renew -- (action) Print where the named token is reissued, then take its new value per -src
<a href="#-renew"></a>

Renew the named token.  credd prints the provider's reissue steps with the name,
account, server and permissions filled in, reads the new value per `-src`, asks
the master password and reseals it.  Given a link, it renews the credential the
link stands for; an ssh key is refused.

#### -remove -- (action) Remove the named credential
<a href="#-remove"></a>

Remove the named credential, after asking the master password.  A removed key
leaves the agent at once.  A credential that links stand for is refused until
the links are removed.

#### -passwd -- (action) Change the master password, resealing every secret
<a href="#-passwd"></a>

Change the master password: credd asks the old one, the new one twice, and
reseals every secret under the new key.  A running daemon holds the old key, so
`-passwd` stops it, and you start it again.

#### -dump -- (action) Print the store, secrets still sealed
<a href="#-dump"></a>

Print the whole store, secrets still sealed, as ssim by default or json with
`-format:json`.  The ssim form piped to `acr -in:<store> -insert -replace
-write` restores the store.

#### -format -- (with -list, -dump) Output format; auto is text for -list, ssim for -dump
<a href="#-format"></a>

The output format of `-list` and `-dump`.  `auto` is text for `-list` and ssim
for `-dump`; `-dump` refuses `text`.

#### -daemon -- Serve the socket in the foreground; -start runs this in a forked child
<a href="#-daemon"></a>

Serve the socket in the foreground, asking the master password first, or with
`-nostore` starting empty.  `-start` runs this in a forked child; use it
directly under a supervisor that wants the process in front.
