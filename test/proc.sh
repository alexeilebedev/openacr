# Process lookups for test scripts, through ps, which Linux and macOS both have,
# and through /proc or lsof for a working directory.  macOS has no /proc, so a
# script that reads only /proc finds nothing there.
# Source this file; each function prints its answer, and prints nothing for a
# process that is gone.

# The pids of the children of process $1, one per line.
proc_children() {
    ps -A -o pid= -o ppid= | awk -v p="$1" '$2 == p {print $1}'
}

# The pids of the descendants of process $1 that run program $2, one per line.
proc_find() {
    local child
    for child in $(proc_children "$1"); do
        if [ "$(proc_name "$child")" = "$2" ]; then
            echo "$child"
        fi
        proc_find "$child" "$2"
    done
}

# The name of the program process $1 runs.  Linux's ps prints the name and
# macOS's the path, so the file name is taken either way.
proc_name() {
    local comm
    comm=$(ps -o comm= -p "$1" 2>/dev/null)
    echo "${comm##*/}"
}

# The state letter of process $1: R running, S sleeping, Z exited and unreaped.
proc_state() {
    ps -o stat= -p "$1" 2>/dev/null | cut -c1
}

# The working directory of process $1: /proc's link where there is one, since a
# Linux runner image may have no lsof, and lsof on macOS.
proc_cwd() {
    if [ -d /proc/self ]; then
        readlink "/proc/$1/cwd" 2>/dev/null
    else
        lsof -a -p "$1" -d cwd -Fn 2>/dev/null | sed -n 's/^n//p'
    fi
}
