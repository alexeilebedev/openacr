# OpenACR Tutorial Walkthrough

A hands-on record of working through all 9 OpenACR tutorials, from environment
setup through advanced data modeling. Each section documents the commands run,
the expected output, and key concepts learned.

---

## Table of Contents

- [Phase 0: Environment Setup](#phase-0-environment-setup)
- [Phase 1: Foundational Reading](#phase-1-foundational-reading)
- [Tutorial 2: Parts and Projects in SQL](#tutorial-2-parts-and-projects-in-sql)
- [Tutorial 8: ACR Live Tutorial](#tutorial-8-acr-live-tutorial)
- [Tutorial 1: Parts and Projects](#tutorial-1-parts-and-projects)
- [Tutorial 4: Command Lines](#tutorial-4-command-lines)
- [Tutorial 3: Scheduling and Main Loop](#tutorial-3-scheduling-and-main-loop)
- [Tutorial 6: Iohook and Fbuf](#tutorial-6-iohook-and-fbuf)
- [Tutorial 5: Running Multiple Subprocesses](#tutorial-5-running-multiple-subprocesses)
- [Tutorial 7: Tests](#tutorial-7-tests)
- [Tutorial 9: Makefile to OpenACR](#tutorial-9-makefile-to-openacr)
- [Key Takeaways](#key-takeaways)

---

## Phase 0: Environment Setup

OpenACR only actively supports **g++ on Linux**. macOS support is listed as
"no longer maintained and most likely broken," and the bootstrap binaries
only exist for x86_64. If you are on Apple Silicon (aarch64), you need
x86_64 emulation.

### Prerequisites

- **Docker Desktop** installed and running. Verify with:

```bash
docker --version
# Docker version 27.x.x or similar

docker info > /dev/null 2>&1 && echo "Docker daemon is running"
```

- **The OpenACR repo** cloned somewhere on your host machine:

```bash
git clone https://github.com/alexeilebedev/openacr.git
cd openacr
```

Note the full path to this directory -- you will need it below. In these
instructions we use `/path/to/openacr` as a placeholder; replace it with
your actual path (e.g., `$HOME/openacr`).

### Step 1: Create the Container

We need `--platform linux/amd64` because the bootstrap binaries are x86_64
only. The repo is mounted read-only so we can copy it into the container.
`sleep infinity` keeps the container alive in the background.

```bash
docker run -d --name openacr-tut \
  --platform linux/amd64 \
  -v /path/to/openacr:/host-openacr:ro \
  -w /root/openacr \
  ubuntu:22.04 \
  sleep infinity
```

Verify it is running:

```bash
docker ps --filter name=openacr-tut
# Should show the container with status "Up"
```

### Step 2: Install Dependencies

```bash
docker exec openacr-tut bash -c "apt-get update -qq && \
  DEBIAN_FRONTEND=noninteractive apt-get install -y -qq \
  g++ make git perl \
  mariadb-server mariadb-client libmariadb-dev libmariadbd-dev \
  libssl-dev libcurl4-openssl-dev liblz4-dev cppcheck \
  uuid-dev libsqlite3-dev libreadline-dev"
```

This takes 1-2 minutes. All packages are needed either for compilation or
for specific OpenACR tools (e.g., MariaDB for `acr_my`, sqlite for `aqlite`).

### Step 3: Copy the Repo Into the Container

We copy rather than mount read-write so tutorial commands can freely create
files, modify ssimfiles, and generate code without affecting your host repo:

```bash
docker exec openacr-tut bash -c "cp -a /host-openacr /root/openacr"
```

Verify the copy:

```bash
docker exec openacr-tut ls /root/openacr/
# Should show: LICENSE Makefile README.md bin build conf cpp data ...
```

### Step 4: Bootstrap the Build

```bash
docker exec -w /root/openacr openacr-tut bash -c \
  "export PATH=\$PATH:\$PWD/bin && perl bin/ai"
```

The `ai` script bootstraps the build system: it compiles `abt` (the build
tool) from a pre-compiled bootstrap, then `abt` builds everything else.
Expect this to take **5-10 minutes** under x86_64 emulation on Apple Silicon,
or 1-2 minutes on a native x86_64 Linux host.

You should see output ending with something like:

```
report.abt  n_target:64  time:00:06:06  ...  n_warn:0  n_err:0  n_install:64
```

The key thing is `n_err:0`.

### Step 5: Verify

```bash
docker exec -w /root/openacr openacr-tut bash -c \
  "export PATH=\$PATH:\$PWD/bin && acr -h"
```

Expected: the `acr` help screen starting with
`acr: Algo Cross-Reference - ssimfile database & update tool`.

```bash
docker exec -w /root/openacr openacr-tut bash -c \
  "export PATH=\$PATH:\$PWD/bin && amc -h"
```

Expected: the `amc` help screen starting with
`amc: Algo Model Compiler: generate code under include/gen and cpp/gen`.

### Shell Access

For all tutorial work, open an interactive shell inside the container:

```bash
docker exec -it -w /root/openacr openacr-tut bash
export PATH=$PATH:$PWD/bin
```

You are now in a Linux environment at `/root/openacr` with all tools on
your PATH. All tutorial commands below assume you are in this shell.

To exit the container shell, type `exit` or press Ctrl-D. The container
keeps running in the background.

### Stopping and Restarting the Container

```bash
# Stop (preserves all state)
docker stop openacr-tut

# Restart later
docker start openacr-tut

# Shell back in
docker exec -it -w /root/openacr openacr-tut bash
export PATH=$PATH:$PWD/bin

# Remove completely (destroys all state)
docker rm -f openacr-tut
```

### Branch Strategy

Each tutorial modifies the repo (creates targets, ssimfiles, etc.). Work on
a dedicated branch per tutorial so you can reset cleanly:

```bash
git checkout -b tut1-work
# ... do the tutorial ...
git checkout -f master
git branch -D tut1-work
```

---

## Phase 1: Foundational Reading

Before starting tutorials, read these two documents:

1. **`txt/ssim.md`** -- The ssim tuple format, how ssimfiles map to SQL
   tables, fldfunc (substring) fields, and cross-references. This is the
   single most important concept in OpenACR.

2. **`txt/setup.md`** -- Directory structure (`bin/`, `cpp/`, `data/`,
   `include/`, `txt/`), how binaries and builds work, and the ssimfile
   format overview.

### Key Concepts from ssim.md

- **ssim tuple**: `<namespace>.<table>  <primary_key>  [<key>:<value> ...]`
- **ssimfile**: text file at `data/<namespace>/<table>.ssim`, one tuple per line
- **fldfunc**: computed fields extracted from a primary key via expressions
  like `.RL` (scan Right, take Left)
- **cross-references**: any field can be a foreign key to another ssimfile's
  primary key, enabling `acr -check` for referential integrity

---

## Tutorial 2: Parts and Projects in SQL

**Source**: `txt/tut/tut2.md`
**Compiles C++**: No
**Theme**: Conceptual foundation -- ssim vs SQL, the dmmeta schema

This tutorial is primarily a reading exercise. It walks through Codd's 1970
relational model paper, shows how ssim tuples map to SQL tables, and builds
up the self-describing schema (`dmmeta.ctype`, `dmmeta.field`,
`dmmeta.ssimfile`, etc.) from scratch.

### Key Commands to Verify Understanding

```bash
# acr describing its own primary key field
acr field:dmmeta.Field.field
# Output:
# dmmeta.field  field:dmmeta.Field.field  arg:algo.Smallstr100  reftype:Val  ...

# acr describing the Ctype concept
acr ctype:dmmeta.Ctype
# Output:
# dmmeta.ctype  ctype:dmmeta.Ctype  comment:Struct

# Show all of acr's own structure definitions
acr ns:acr -t | head -30
```

### What You Learn

- ssim tuples are the text equivalent of SQL rows
- The `dmmeta` namespace is the schema that describes all other schemas
- Tools like `acr`, `amc`, `abt`, `acr_ed` are all described using ssim
  tuples in the `dmmeta` and `dev` namespaces
- OpenACR is self-describing: `acr field:dmmeta.Field.field` describes
  its own primary key

---

## Tutorial 8: ACR Live Tutorial

**Source**: `txt/tut/tut8.md`
**Compiles C++**: No
**Theme**: Hands-on `acr` -- the core data manipulation tool

This is the most important "learn by doing" tutorial. Work on a branch since
it creates and modifies ssimfiles under `data/`.

### Creating Tables

```bash
# Create a new ssimfile (table) called dev.a
acr_ed -create -ssimfile dev.a -write -amc:N
```

This creates:
- `data/dev/a.ssim` (empty data file)
- Schema entries in `dmmeta.ctype`, `dmmeta.field`, `dmmeta.ssimfile`

### Inserting Data

```bash
# Insert 11 records
for X in {0..10}; do echo dev.a a:a$X; done | acr -insert -write
```

### Querying

```bash
# SQL-style regex query
acr a:a'(2|3|4)'
# Output:
# dev.a  a:a2  comment:""
# dev.a  a:a3  comment:""
# dev.a  a:a4  comment:""

# Extract just the field values
acr a:a'(2|3|4)' -field:a
# Output:
# a2
# a3
# a4

# Get a regex of matching values (useful for piping)
acr a:a'(2|3|4)' -regxof:a
# Output:
# (a2|a3|a4)
```

### Insert vs Replace vs Merge

```bash
# -insert: fails on duplicate keys (ignores them)
echo 'dev.t t:ggg' | acr -insert -write   # ignored if ggg exists

# -replace: overwrites entire record, unspecified fields get defaults
echo 'dev.t t:ggg' | acr -replace -write  # val resets to 0

# -merge (upsert): updates only specified fields, preserves the rest
echo 'dev.t t:ggg val2:4' | acr -merge -write  # val stays at 3, val2 becomes 4

# -trunc: wipes the table before inserting
echo 'dev.t t:hhh' | acr -insert -trunc -write
```

### Subset Tables and References

```bash
# Create table b as a subset of a (b.a is a foreign key to a)
acr_ed -create -ssimfile:dev.b -subset dev.A -write -amc:N

# Populate b with a subset of a's values
for X in {3..7}; do echo dev.b a:a$X; done | acr -insert -write

# Follow references upward: from b, find the parent a record
acr b:a3 -nup 1
# Output:
# dev.a  a:a3  b:0  comment:""
#
# dev.b  a:a3  comment:""

# Follow references downward: from a, find dependent b records
acr a:a4 -ndown 1

# Full cross-reference with tree display
acr b -xref -tree
# Shortcut:
acr b:a5 -t
```

### Cascade Delete

```bash
# Deleting a record also deletes dependents
acr a:a4 -del -write
# Output:
# acr.delete  dev.a  a:a4  b:0  comment:""
# acr.delete  dev.b  a:a4  comment:""
```

### Rename with Dependency Tracking

```bash
acr a:a3 -rename a99 -write
# Renames a3 -> a99 in both dev.a AND dev.b
```

### Cross-Product Types

```bash
# Create independent table c (colors)
acr_ed -create -ssimfile:dev.c -write -amc:N
for X in red green blue; do echo dev.c c:$X; done | acr -insert -write

# Create table d as cross-product of b and c, with '.' separator
acr_ed -create -ssimfile dev.d -subset dev.B -subset2 dev.C -separator . -write -amc:N

# Populate d
for B in a5 a6 a7; do
  for C in red green blue; do
    echo dev.d d:$B.$C
  done
done | acr -insert -write

# View the tree structure
acr d -t
# Output shows d grouped under b, which groups under a, with c referenced
```

### Fldfunc (Computed Fields)

```bash
# Show the computed substring fields
acr d -fldfunc
# Output:
# dev.d  d:a5.blue   b:a5  c:blue   comment:""
# dev.d  d:a5.green  b:a5  c:green  comment:""
# ...

# Fields b and c are extracted from d using expressions .RL and .RR
acr field:dev.D.b
# dmmeta.field  field:dev.D.b  arg:dev.B  reftype:Pkey  ...
#   dmmeta.substr  field:dev.D.b  expr:.RL  srcfield:dev.D.d
```

### Referential Integrity Checking

```bash
# Insert a record with a bad reference
echo 'dev.b a:xyz' | acr -insert -write

# Check for violations
acr -check b
# Output:
# data/dev/b.ssim:5: Invalid value a:xyz
# ...
# acr.badrefs  ctype:dev.B  nbad:1
```

### Other Useful Flags

```bash
acr d -where c:blue       # query by non-primary column
acr c -meta                # show the type definition instead of data
acr c -cmt                 # show data with field comments
acr c -rowid               # show row ordering IDs
echo '...' | acr -in:- %   # read from stdin as the data source
```

---

## Tutorial 1: Parts and Projects

**Source**: `txt/tut/tut1.md`
**Compiles C++**: Yes
**Theme**: First C++ target with amc code generation

### Step 1: Create the Target

```bash
acr_ed -create -target samp_tut1 -write -amc:N
```

This creates a complete executable project: source files, generated headers,
build configuration, and namespace registration.

### Step 2: Define Data Structures

```bash
# Project pool with hash index
acr_ed -create -ctype samp_tut1.Proj -pooltype Tpool -indexed -write -amc:N

# Part pool with hash index
acr_ed -create -ctype samp_tut1.Part -pooltype Tpool -indexed -write -amc:N

# Partproj (the relationship) pool -- no index
acr_ed -create -ctype samp_tut1.Partproj -pooltype Tpool -write -amc:N
```

### Step 3: Create Reference Fields

```bash
# Upward pointers from Partproj to Part and Proj
acr_ed -create -field samp_tut1.Partproj.p_part -arg samp_tut1.Part -reftype Upptr -write -amc:N
acr_ed -create -field samp_tut1.Partproj.p_proj -arg samp_tut1.Proj -reftype Upptr -write -amc:N
```

### Step 4: Create Cross-References (Indexes)

```bash
# Global linked lists for iterating all parts/projects
acr_ed -create -field samp_tut1.FDb.zd_part -write -comment "List of all parts" -amc:N
acr_ed -create -field samp_tut1.FDb.zd_proj -write -comment "List of all projects" -amc:N

# Group-by: Part -> its Partproj (cascade delete)
acr_ed -create -field samp_tut1.Part.c_partproj -reftype Ptr -cascdel -write \
  -comment "List of projects by part" -amc:N

# Group-by: Proj -> list of Partprojs (cascade delete)
acr_ed -create -field samp_tut1.Proj.zd_partproj -cascdel -write \
  -comment "List of parts by project" -amc:N
```

### Step 5: Copy Source and Build

```bash
cp conf/samp_tut1.txt cpp/samp_tut1/samp_tut1.cpp
amc && ai samp_tut1
```

### Step 6: Run

```bash
samp_tut1
```

Expected output:

```
initial setup
list of projects and their parts
  project proj1
    has part part1
    has part part2
  project proj2
    has part part3
list of parts and their projects
  part part1
    has project proj1
  part part2
    has project proj1
  part part3
    has project proj2

deleting part1
list of projects and their parts
  project proj1
    has part part2
  project proj2
    has part part3
...
```

### Visualize the In-Memory Database

```bash
amc_vis samp_tut1.%
```

```
/ samp_tut1.FDb
|Tpool proj------>/ samp_tut1.Proj
|Thash ind_proj-->|
|Llist zd_proj--->|
|Tpool part-------|------------------->/ samp_tut1.Part
|Thash ind_part---|------------------->|
|Llist zd_part----|------------------->|
|Tpool partproj---|--------------------|---------------->/ samp_tut1.Partproj
-                 |                    |                 |
                  |<-------------------|-----------------|Upptr p_proj
                  |                    |<----------------|Upptr p_part
                  |Llist zd_partproj---|---------------->|
                  |                    |Ptr c_partproj-->|
```

### What You Learn

- `acr_ed -create -target` scaffolds a complete executable
- `acr_ed -create -ctype` defines data structures via ssim tables
- `amc` generates all C++ code from the schema (pools, indexes, xrefs)
- Cascade delete and automatic cross-referencing happen at the generated
  code level -- you don't write that logic yourself

---

## Tutorial 4: Command Lines

**Source**: `txt/tut/tut4.md`
**Compiles C++**: Yes
**Theme**: amc-generated CLI support

### Create a Target and Add Options

```bash
acr_ed -create -target sample -write

# Add a boolean flag
acr_ed -create -field command.sample.flag -arg bool -write -comment "An important flag"

# Add an integer option with default
acr_ed -create -field command.sample.val -dflt 0 -arg u32 -write -comment "A value"

# Make val an anonymous (positional) argument
echo 'dmmeta.anonfld field:command.sample.val' | acr -insert -write
amc && abt -install sample
```

### Source Code

```cpp
void sample::Main() {
    prlog(Keyval("flag",_db.cmdline.flag));
    prlog(Keyval("val",_db.cmdline.val));
}
```

### Test

```bash
sample -flag
# flag:Y
# val:0

sample 5
# flag:N
# val:5

sample -h
# Usage: sample [[-val:]<int>] [options]
#     -in       string  "data"  Input directory or filename, - for stdin
#     -flag                     An important flag
#     [val]     int     0       A value
#     ...
```

### Recursive Subprocess Invocation

amc generates `command::sample_proc` for typed subprocess calls:

```cpp
#include "include/gen/command_gen.h"
#include "include/gen/command_gen.inl.h"

void sample::Main() {
    prlog(_db.cmdline.val);
    if (_db.cmdline.val > 0) {
        command::sample_proc sample;
        sample.cmd.val = _db.cmdline.val - 1;
        sample_Exec(sample);
    }
}
```

```bash
sample 3
# 3
# 2
# 1
# 0
```

### What You Learn

- All command-line options are described as fields of `command.<ns>`
- `amc` generates parsing, help screens, and shell completion
- `_ToCmdline` converts a command struct back to a bash command string
- `*_proc` structs provide typed, safe subprocess invocation (no `system()`)

---

## Tutorial 3: Scheduling and Main Loop

**Source**: `txt/tut/tut3.md`
**Compiles C++**: Yes
**Theme**: Async engine programming with fsteps

### Core Concept

The main loop is controlled by three variables:
- `algo_lib::_db.clock` -- current CPU time
- `algo_lib::_db.limit` -- when to exit
- `algo_lib::_db.next_loop` -- soonest time any step needs CPU

If no step revises `next_loop`, the loop exits (program has nothing to do).

### Step 1: Inline Step (fires every cycle while list is non-empty)

```bash
acr_ed -create -target samp_tut3 -write
acr_ed -create -ctype samp_tut3.Value -subset i32 -pooltype Tpool -write -amc:N
acr_ed -create -field samp_tut3.FDb.zd_value -fstep Inline -write -amc:N
```

Source (`conf/samp_tut3.txt`):

```cpp
void samp_tut3::zd_value_Step() {
    Value &value = *zd_value_First();
    prlog(value.value);
    value_Delete(value);
}

void samp_tut3::Main() {
    for (int i = 0; i < 10; i++) {
        Value &value = value_Alloc();
        value.value = i;
        value_XrefMaybe(value);
    }
    samp_tut3::MainLoop();
}
```

```bash
cp conf/samp_tut3.txt cpp/samp_tut3/samp_tut3.cpp
amc && ai samp_tut3
samp_tut3
# 0 1 2 3 4 5 6 7 8 9
```

### Step 2: InlineRecur with Delay (fires once per second)

```bash
echo 'dmmeta.fstep fstep:samp_tut3.FDb.zd_value steptype:InlineRecur' | acr -merge -write
echo 'dmmeta.fdelay fstep:samp_tut3.FDb.zd_value delay:1.000000000 scale:N' | acr -insert -write
```

Update source to print timestamps:

```cpp
void samp_tut3::zd_value_Step() {
    Value &value = *zd_value_First();
    prlog(algo::CurrUnTime() << " " << value.value);
    value_Delete(value);
}
```

Output now prints one number per second with ISO 8601 timestamps.

### Step 3: Interleaved Steps (two async activities)

Add a second step that randomly deletes values at 60 Hz:

```bash
acr_ed -create -field samp_tut3.FDb.ind_value -write
acr_ed -create -field samp_tut3.FDb.update -arg bool -fstep InlineRecur -dflt true -write
echo 'dmmeta.fdelay fstep:samp_tut3.FDb.update delay:0.018 scale:N' | acr -insert -write
```

```cpp
void samp_tut3::update_Step() {
    int num = algo::get_cycles() % 10;
    if (Value *value = ind_value_Find(num)) {
        prlog("deleted " << value->value);
        value_Delete(*value);
    }
    if (ind_value_N() == 0) {
        _db.update = false;
    }
}
```

Output shows interleaved deletion and printing:

```
2026-03-16T21:25:55.668583425 0
deleted 8
deleted 1
deleted 6
deleted 3
deleted 5
2026-03-16T21:25:58.171059176 2
2026-03-16T21:26:00.673166552 4
2026-03-16T21:26:03.175560595 7
2026-03-16T21:26:05.677987305 9
```

### What You Learn

- `fstep` turns a data structure into a cooperatively scheduled thread
- `Inline` fires on every cycle; `InlineRecur` fires after a delay
- Deleting records automatically removes them from all indexes (Auto Unref)
- Multiple steps interleave naturally without manual synchronization

---

## Tutorial 6: Iohook and Fbuf

**Source**: `txt/tut/tut6.md`
**Compiles C++**: Yes
**Theme**: Non-blocking I/O with file descriptor polling

### Setup

```bash
acr_ed -create -target sample -write
acr_ed -create -field sample.FDb.read -arg algo_lib.FIohook -write
```

### Source

```cpp
static void DoRead() {
    char buf;
    int rc = 0;
    do {
        rc = read(sample::_db.read.fildes.value, &buf, 1);
        if (rc == 1) {
            tempstr out;
            char_PrintCppSingleQuote(buf, out);
            prlog(algo::CurrUnTime() << " " << out);
        } else if (rc == -1 && errno == EAGAIN) {
        } else {
            IohookRemove(sample::_db.read);
        }
    } while (rc > 0);
}

void sample::Main() {
    _db.read.fildes = algo::Fildes(0);
    algo::SetBlockingMode(_db.read.fildes, false);
    callback_Set0(_db.read, DoRead);
    IOEvtFlags flags;
    read_Set(flags, true);
    IohookAdd(_db.read, flags);
    MainLoop();
}
```

### Test

```bash
echo 'hello' | sample
# 2026-03-16T21:27:42.078235168 'h'
# 2026-03-16T21:27:42.079708668 'e'
# 2026-03-16T21:27:42.07971046 'l'
# 2026-03-16T21:27:42.079711293 'l'
# 2026-03-16T21:27:42.079712168 'o'
# 2026-03-16T21:27:42.079713335 '\n'
```

### What You Learn

- `FIohook` wraps `epoll` (Linux) or `kevent` (BSD) into the main loop
- Non-blocking I/O integrates with fsteps seamlessly
- `IohookRemove` detaches the hook, causing the main loop to exit when
  no other work remains

---

## Tutorial 5: Running Multiple Subprocesses

**Source**: `txt/tut/tut5.md`
**Compiles C++**: No (reads existing code)
**Theme**: Parallel subprocess orchestration

This tutorial examines the existing `atf_nrun` program, which runs N parallel
child jobs using two data structures:

- `zd_todo` -- linked list of jobs waiting to be spawned
- `ind_running` -- hash table of currently running jobs, keyed by PID

### Visualize

```bash
amc_vis atf_nrun.%
# / atf_nrun.FDb
# |Lary fentry-------->/ atf_nrun.FEntry
# |Thash ind_running-->|
# |Llist zd_todo------>|
```

### Run

```bash
atf_nrun 3 -maxjobs:2
# atf_nrun.spawn  command:"echo 0; sleep 1"  pid:8999  ntodo:2  nrunning:1
# atf_nrun.spawn  command:"echo 1; sleep 1"  pid:9000  ntodo:1  nrunning:2
# 0
# 1
# SIGCHLD
# atf_nrun.sigchild  pid:8999  status:0
# atf_nrun.spawn  command:"echo 2; sleep 1"  pid:9001  ntodo:0  nrunning:2
# 2
# SIGCHLD
# atf_nrun.sigchild  pid:9001  status:0
```

### What You Learn

- `SIGCHLD` triggers immediate rescheduling of the `ind_running` step
- `zd_todo_SetDelay` and `ind_running_SetDelay` dynamically adjust timing
- The pattern of "todo list + running set" is a general recipe for job
  scheduling in the main loop

---

## Tutorial 7: Tests

**Source**: `txt/tut/tut7.md`
**Compiles C++**: Yes
**Theme**: Unit tests and normalization checks

### List All Tests

```bash
acr unittest | head -20
```

### Run Tests for a Namespace

```bash
atf_unit algo_lib.%
# Runs ~111 tests, reports pass/fail for each
```

### Create a New Unit Test

```bash
acr_ed -create -unittest algo_lib.Test1 -write
```

This inserts a record into `atfdb.unittest` and generates a stub:

```cpp
void atf_unit::unittest_algo_lib_Test1() {
    // test code goes here
}
```

After implementing, build and run:

```bash
ai atf_unit
atf_unit algo_lib.Test1
# atf_unit.unittest  unittest:algo_lib.Test1  success:Y  comment:""
```

### Normalization Checks

```bash
# Create a normalization check
acr_ed -create -normcheck mycheck -write
# Implement atf_ci::citest_mycheck() in a file linked with atf_ci

# Run all checks
atf_ci
```

### What You Learn

- `atf_unit` runs individual function-level tests
- `atf_ci` runs normalization checks (referential integrity, round-trip, etc.)
- `acr_ed -create -unittest` scaffolds a test automatically
- `-debug` flag on `atf_unit` launches `mdbg` with a breakpoint at the test

---

## Tutorial 9: Makefile to OpenACR

**Source**: `txt/tut/tut9_make.md`
**Compiles C++**: Yes (uses existing `samp_make`)
**Theme**: Modeling build systems as relational data

This tutorial models a GNU Makefile as a set of ssim tables:

| Table | Purpose | Primary Key Format |
|-------|---------|-------------------|
| `sampdb.target` | Build targets | `target` |
| `sampdb.gitfile` | Source files | `gitfile` |
| `sampdb.targsrc` | Target-to-source deps | `target/source` (composite) |
| `sampdb.targdep` | Target-to-target deps | `target.parent` (composite) |
| `sampdb.targrec` | Build recipes | `target` |

### Query the Data

```bash
acr sampdb.target:edit -t
# sampdb.target  target:edit  dflt:Y  comment:""
#   sampdb.targdep  targdep:edit.command.o  rec:Y  pre:Y  comment:""
#   sampdb.targdep  targdep:edit.display.o  rec:Y  pre:Y  comment:""
#   ...
#   sampdb.targrec  target:edit  recipe:"cc -o edit"  comment:""
```

### Generate the Makefile

```bash
samp_make -target:%
# edit : command.o display.o files.o insert.o kbd.o main.o search.o utils.o
#         cc -o edit  command.o display.o files.o ...
# command.o : command.c command.h defs.h
#         cc -c  command.c
# ...

# Generate for specific targets using regex
samp_make -target:'clean|insert.o'
# clean :
#         rm command.o display.o edit ...
# insert.o : buffer.h defs.h insert.c
#         cc -c  insert.c
```

### What You Learn

- Composite keys with fldfunc (e.g., `targsrc:command.o/command.c` splits
  into `target:command.o` via `/LL` and `src:command.c` via `/LR`)
- Foreign keys between tables enforce that targets and sources must exist
- A C++ program reads these tables and reconstructs the Makefile
- Adding a new build target means inserting rows, not editing Makefile syntax

---

## Key Takeaways

1. **Everything is data.** Programs, their command lines, data structures,
   build configurations, tests -- all described as ssim tuples in text files.

2. **acr is the Swiss Army knife.** Query, insert, delete, rename, check
   referential integrity, follow references up/down, generate shell scripts.

3. **amc eliminates boilerplate.** Pools, hash tables, linked lists,
   cross-references, cascade delete, command-line parsing -- all generated
   from schema descriptions.

4. **The main loop is cooperative.** Steps are data-driven scheduled
   functions. Multiple async activities interleave naturally without threads.

5. **Self-description is the superpower.** `acr` can query its own schema.
   `amc` generates its own source code. `abt` builds itself. This
   reflexivity means the tools compose and scale.

### Quick Reference: Most-Used Commands

| Command | Purpose |
|---------|---------|
| `acr <query>` | Query ssim data |
| `acr -insert -write` | Insert tuples from stdin |
| `acr -del -write` | Delete matching records |
| `acr -check` | Verify referential integrity |
| `acr <query> -t` | Show with cross-refs as tree |
| `acr_ed -create -target` | Create a new executable |
| `acr_ed -create -ssimfile` | Create a new data table |
| `acr_ed -create -field` | Add a field to a ctype |
| `acr_ed -create -ctype` | Create a new C++ struct |
| `amc` | Regenerate all C++ code from schema |
| `abt -install <target>` | Build and install a target |
| `ai <target>` | Shortcut for amc + abt |
| `amc_vis <ns>.%` | Visualize in-memory database |
| `src_func <ns>.<func>` | Show source of a function |
| `atf_unit <pattern>` | Run matching unit tests |
