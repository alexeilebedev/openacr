## llmtool - Claude Code cost and status reporting
<a href="#llmtool"></a>
Claude Code writes down everything it spends.  Every api request it makes is
recorded, with the model that served it, the tokens it consumed in each of four
categories, and the directory the work happened in.  `llmtool` reads those
records.  It answers two questions: what is happening right now, which is the
status line above the prompt, and where the money went, which is a table of
worktrees.

### Syntax
<a href="#syntax"></a>
```usage
llmtool: Claude Code cost and status reporting
Usage: llmtool [options]
    OPTION       TYPE    DFLT  COMMENT
    -session     string  ""    Session id to report on; default is every session in this directory
    -transcript  string  ""    Transcript file to report on, instead of a session id
    -all                       Report on every directory claude code has recorded, not just this one
    -rate        string  ""    Price list to use; default is the .claude/costrate.json of the tree this binary was built from
    -analyze                   Report cost per worktree and per model, as ssim tuples
    -status                    Print the claude code status line; its json arrives on stdin
    -verbose     flag          Verbosity level (0..255); alias -v; cumulative
    -debug       flag          Debug level (0..255); alias -d; cumulative
    -trace       string  ""    Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                      Print help and exit; alias -h
    -version                   Print version and exit
    -signature                 Show signatures and exit; alias -sig
```

### What a token costs
<a href="#what-a-token-costs"></a>

Claude Code charges by the token, and the rate depends on the model and on
which of four categories the token falls in.  A *fresh input* token is one the
model has not been shown before.  An *output* token is one it produced.  A
*cache read* is a token recovered from the prompt cache, charged at a tenth of
fresh input.  A *cache write* is a token placed into that cache, charged at
more than fresh input, and charged again according to how long the entry is
held -- five minutes or an hour.

Those rates live in `.claude/costrate.json`, keyed by model id, in dollars per
million tokens.  A price change or a new model is an edit to that file and
nothing else.  A model the file does not name is counted but never priced:
its tokens appear in the report and its dollars read zero, and the report says
which model it was with `priced:N`.  Borrowing another model's rate would
produce a confident figure that no reader could tell was invented.  Claude Code
spells some model ids with a release date appended, so a model can need a row
under both spellings.

The file is found by walking up from the directory holding the `llmtool`
binary, not from the current directory: the price list belongs to the tool, and
its shape and the code that parses it change together.  That is what lets
`wt/mybranch/bin/llmtool` be run from anywhere and still price correctly.
`-rate` names a different file outright.

### Where the money went
<a href="#where-the-money-went"></a>

`llmtool -analyze` prints ssim tuples, one row per worktree per qualifier.
Pipe them through `ssimfilt -t` for a table:

```bash
$ llmtool -analyze | ssimfilt -t
STAT              N_REQUEST  INPUT  OUTPUT  CACHE_READ  CACHE_WRITE  USD_OUTPUT  USD_CACHE_READ  USD
.,main            171        319    162354  63693354    836888       4.0589      31.8467         44.2760
.,subagent        305        754    10916   27854309    654616       0.2729      13.9272         18.2952

MODEL          N_REQUEST  INPUT  OUTPUT  CACHE_READ  CACHE_WRITE  USD      PRICED
claude-opus-5  759        1621   293464  118152183   2264573      84.4172  Y
```

The row key is the worktree and the qualifier, comma-separated.  The worktree
is the directory the session ran in, named relative to the directory `llmtool`
runs in, so a sibling worktree reads `../mybranch` and the current one reads
`.`; a directory in another tree entirely is written with a tilde when that
comes out shorter.  The qualifier is `main` or `subagent`, which separates a
session's own spend from the spend of the agents it fanned out to.

The rows partition the spend.  No row contains another, so any subset of them
can be added up, and a total is an `awk` away:

```bash
llmtool -analyze -all | ssimfilt report.llmtool -f:usd | awk '{n+=$1} END {print n}'
```

The model rows are what keep the worktree rows honest.  A model with
`priced:N` contributed tokens and no dollars, so every worktree row it touched
reads low; the model row is where a reader learns that, and by how much.

Four selections decide which transcripts are read.  With no option the report
covers every session of the current directory, which is the question asked
from inside a worktree.  `-all` covers every directory Claude Code has
recorded, which is the question asked about a month's work.  `-session` names
one session by id within this directory, and `-transcript` names one by path,
which is how a session belonging elsewhere is reached.

### Why the transcript and not a collector
<a href="#why-the-transcript-and-not-a-collector"></a>

The transcript already holds every figure the report needs, including which
model served each request and how long each cache write was held.  A telemetry
collector would add a daemon, a configuration, and a latency risk to obtain
strictly less.

Two properties of the transcript are load-bearing, and both are invisible to
someone reading the file for the first time -- which is why an improvised
`jq` sum over it is not merely absent but wrong, and plausible.

A single api request occupies several transcript lines, and every one of them
repeats that request's usage in full.  Adding up the lines therefore counts
most requests several times over; on a measured session that overstated output
tokens by 2.7x.  The lines of one request share a request id, and counting each
id once is what makes the total correct.

A cache write is priced by how long it was held, and the duration varies within
a single session: a main session caches for an hour while its subagents cache
for five minutes.  On a measured session, charging one rate across both would
have overstated the bill by 26%.  Each request records its own durations, so
each is priced at the rate it actually earned.

### What the numbers leave out
<a href="#what-the-numbers-leave-out"></a>

Auxiliary model calls -- the small background traffic Claude Code makes on its
own behalf -- appear in no transcript and are therefore excluded.  They run
around two parts in a thousand, so the report reads slightly *lower* than the
cumulative `cost` on the status line, which does include them.  That gap is
expected.

A cache write whose duration the transcript did not record is priced at the
duration `cache_write_default` nominates, which makes that report's cache-write
dollars an estimate.  A current Claude Code times every cache write, so this
does not arise for a current transcript; when it does, `llmtool` says so on
stderr, naming the token count and the duration it assumed.  Nothing else in
the report is estimated.

### The status line
<a href="#the-status-line"></a>

`llmtool -status` reads the status object Claude Code writes to its stdin and
prints the line displayed above the prompt.  `.claude/settings.json` wires it
up:

```bash
cwd:~/src/wt/llmtool  nmod:38  Opus 5  ctx:148K  limit:200K  cost:$9.28  agents:2  tasks:3
```

`cwd` is the working directory and `nmod` the count of modified tracked files.
`ctx` and `limit` are the context window consumed and its size.  `cost` is what
Claude Code reports the session has cost so far.  `agents` and `tasks` count the
ones currently running.  A field that would read zero is omitted, so the line
stays short when little is happening.

The line carries no breakdown of that cost by category.  Claude Code reports the
cumulative figure as a single number and exposes no cumulative token split, so
the only breakdown computable from the status object describes the most recent
api request by itself.  That is a narrower question than the one a glance at a
status line asks, and the cumulative question is what `-analyze` answers, from
the transcripts rather than from this interface.

### Options
<a href="#options"></a>
#### -session -- Session id to report on; default is every session in this directory
<a href="#-session"></a>

#### -transcript -- Transcript file to report on, instead of a session id
<a href="#-transcript"></a>

#### -all -- Report on every directory claude code has recorded, not just this one
<a href="#-all"></a>

#### -rate -- Price list to use; default is the .claude/costrate.json of the tree this binary was built from
<a href="#-rate"></a>

#### -analyze -- Report cost per worktree and per model, as ssim tuples
<a href="#-analyze"></a>

#### -status -- Print the claude code status line; its json arrives on stdin
<a href="#-status"></a>
