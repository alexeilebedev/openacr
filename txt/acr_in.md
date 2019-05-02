## acr_in: Show input tuples for target

acr_in computes the names and the order of ssimfiles
which constitute target's declared input. Input fields are simply
those fields for which finput records exist, e.g. for target t:

    acr finput:t.%

but taken recursively over all libraries that the target uses.

With -data argument, acr_in also loads the specified ssimfiles in memory
and prints out their contents in the order in which it is safe to load.

The order of ssimfiles is determined as a transitive closure on Pkey
references, and is independent of the target itself. This means that
acr_in can be called with an regex of target names (e.g. %), and the
resulting input can be fed into any one of the targets implied by the
regex, without error.

Example: Create canned input file for a given tool:

    acr_in amc -data > tempfile
    amc -in:tempfile
    # this is exactly the same as running amc -in:data


