#!/bin/bash
# Fixture for comptest apm.UpdateFate.
# An update is a chain of steps, and each step runs a child process whose output
# the next step reads back from a file: two sandbox checkouts, two collections
# of the package's records, one three-way merge of those records, and three
# collections of the package's file list. A child that fails leaves its output
# file empty, and an empty output file cannot be told apart from a side of the
# merge that legitimately holds nothing. So the run has to stop at the step that
# failed rather than compose a plan out of what the step did not produce.
# This script drives the whole chain once per step, failing that one step while
# every other step succeeds. Each run prints the diagnostic the failing step
# produced, and then one line naming the step, apm's exit code, and how many
# records the printed plan carried.
#
# $1 is the checkout root and $2 the directory apm was built into. The current
# directory is the test's own tempdir: data/ is symlinked in from the checkout
# because apm reads the schema from there, bin/ holds the stubs standing in for
# the children, and temp/ and wt/ are named relative to the current directory,
# so no run here reaches the checkout.
root="$1"
bindir="$2"
export fatedir="$PWD"
export fateroot="$root"

ln -sfn "$root/data" data
mkdir -p bin

# apm finds a child either at bin/ under the current directory or through PATH,
# so the relative bin entry the repo puts on PATH is what routes the children
# reached that second way -- acr_dm and wt -- to the stubs below. The entry is
# relative on purpose: it follows the current directory, which is the tempdir
# for every run this script makes.
export PATH="bin:$PATH"

# The record database. Every apm action starts by reading the whole ssim
# database out of a child acr into a temporary file, so a failure here leaves
# apm with no records at all rather than with the ones it was asked about. Only
# that first call is a candidate for failure; every other call reaches the real
# acr, which the plan is composed of.
cat > bin/acr <<'STUB'
#!/bin/bash
n=$(( $(cat "$fatedir/n_acr" 2>/dev/null || echo 0) + 1 ))
echo $n > "$fatedir/n_acr"
if [ "$apm_fate" = "loadrec" ] && [ "$n" = 1 ]; then exit 1; fi
exec "$fateroot/bin/acr" "$@"
STUB

# The sandbox checkout. apm asks for two, base first, so the call number picks
# which of the two this run fails. A successful call creates the directory the
# sandbox is named after, which is all apm needs from it.
cat > bin/wt <<'STUB'
#!/bin/bash
n=$(( $(cat "$fatedir/n_wt" 2>/dev/null || echo 0) + 1 ))
echo $n > "$fatedir/n_wt"
if [ "$apm_fate" = "sandbox$n" ]; then exit 1; fi
name=""
for arg in "$@"; do
    case $arg in -name:*) name=${arg#-name:};; esac
done
mkdir -p "$fatedir/wt/$name"
exit 0
STUB

# The child apm, which the run starts inside a sandbox to read that sandbox's
# copy of the package. -showrec reads the package's records and -showfile its
# file list; both write to a redirect the run reads back, so a stub that prints
# nothing and exits nonzero leaves behind exactly what a real failure leaves.
# Each kind of call is counted separately, in the order the run makes them.
cat > bin/apm <<'STUB'
#!/bin/bash
case " $* " in
    *" -showrec"*)
        n=$(( $(cat "$fatedir/n_showrec" 2>/dev/null || echo 0) + 1 ))
        echo $n > "$fatedir/n_showrec"
        if [ "$apm_fate" = "showrec$n" ]; then exit 1; fi
        echo 'dev.package  package:apm  baseref:HEAD  origin:.  comment:""'
        echo 'dmmeta.ns  ns:apm  nstype:exe  license:GPL  comment:""'
        ;;
    *" -showfile"*)
        n=$(( $(cat "$fatedir/n_showfile" 2>/dev/null || echo 0) + 1 ))
        echo $n > "$fatedir/n_showfile"
        if [ "$apm_fate" = "showfile$n" ]; then exit 1; fi
        ;;
esac
exit 0
STUB

# The three-way merge of the records, whose output is the incoming side of the
# plan. Every fate but its own reaches the real acr_dm, so the plan a run prints
# is the one the real merge rules produce from the three sides the run collected.
cat > bin/acr_dm <<'STUB'
#!/bin/bash
if [ "$apm_fate" = "merge" ]; then exit 1; fi
exec "$fateroot/bin/acr_dm" "$@"
STUB

chmod +x bin/acr bin/wt bin/apm bin/acr_dm

# none is the control: every step succeeds, and the plan comes out two records
# long because the one package in one_package.ssim names two keys. The rest fail
# one step each, listed in the order the run performs them.
for fate in none loadrec sandbox1 showrec1 sandbox2 showrec2 merge showfile1 showfile2 showfile3; do
    rm -rf temp wt n_acr n_wt n_showrec n_showfile
    mkdir -p temp
    export apm_fate="$fate"
    out=$("$root/$bindir/apm" -in:data -pkgdata:"$root/test/apm/one_package.ssim" apm -update -dry_run 2>&1)
    rc=$?
    printf '%s\n' "$out" | grep -E '^apm\.(sandbox_create|pkgrec_read|pkgfile_read|recmerge)  '
    ndelete=$(printf '%s\n' "$out" | grep -c '^acr\.delete ')
    nreplace=$(printf '%s\n' "$out" | grep -c '^acr\.replace ')
    echo "apm.update_fate  step:$fate  exit:$rc  delete:$ndelete  replace:$nreplace"
done
