import gdb
import os

# add command to list inferiors
class Ps(gdb.Command):
    """List all inferiors with their full command lines."""

    def __init__(self):
        super(Ps, self).__init__("ps",gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        for inferior in gdb.inferiors():
            pid = inferior.pid
            if pid is None:
                cmdline = "<no process>"
            else:
                try:
                    with open(f"/proc/{pid}/cmdline", "rb") as f:
                        raw = f.read().decode(errors="replace")
                        # cmdline is null-separated
                        cmdline = " ".join(raw.split("\0")).strip()
                except Exception as e:
                    cmdline = f"<unavailable: {e}>"

            print(f"Inferior {inferior.num} (PID {pid}): {cmdline}")

Ps()
