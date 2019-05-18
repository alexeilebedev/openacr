## acr_in: Show input tuples for target

acr_in computes the names and the order of ssimfiles
which constitute target's declared input.

To illustrate, let's create a new program and make it read a table.

    $ acr_ed -create -target sample -write
    ...
    $ acr_ed -create -finput -target sample -ssimfile dmmeta.ctype -write
    ...
    $ acr finput:sample.%
    dmmeta.finput  field:sample.FDb.gitfile  extrn:N  update:N  strict:Y  comment:""
    ...
    
For this target, finput is `dmmeta.ctype`. Let's see if acr_in knows that:

    $ acr_in sample
    dmmeta.Dispsigcheck  dispsig:sample.Input  signature:f162f70f9895c41909c2192722172e6d21fe5679
    dmmeta.Dispsigcheck  dispsig:algo_lib.Input  signature:ddc07e859e7056e1a824df1ad0e6d08e12e89849
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype

The output is the list of ssimfiles needed by the target, plus any signatures
used by the target. We can ignore the signatures for now, but they can be used 
to detect schema changes between the compiled version of a program and the version 
of data set on which `acr_in` operates. The `-sigcheck` option can be used to omit these.

Let's now add, as an `finput` for `sample`, the `dmmeta.ns` table, which is lexicographically
after `dmmeta.ctype`, but logically before (since ctype depends on ns).

    $ acr_ed -create -finput -target sample -ssimfile dmmeta.ns -write
    
    $ acr_in sample -sigcheck:N
    dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype
    
We see that `acr_in` has printed ns and ctype in the order of Pkey dependencies between them.
`acr_in` operates recursively over all libraries that are target uses.

The order of ssimfiles is determined as a transitive closure on Pkey
references, and is independent of the target itself. This means that
`acr_in` can be called with an regex of target names (e.g. %), and the
resulting input can be fed into any one of the targets implied by the
regex, without error.

### The -data option

With -data argument, acr_in also loads the specified ssimfiles in memory
and prints out their contents.

This can be used to create canned input files:

    acr_in sample -data > tempfile
    sample -in:tempfile
    # this is exactly the same as running sample -in:data

### The -checkable option

If we take the data from ssmifiles `ns` and `ctype`, they are now sufficient
to serve as inputs to the newly created `sample`. However, `acr -check` will fail on
this resulting dataset, because `ns` also depends on `nstype`, and in general there
may be unresolved Pkey references in the resulting output.

To recursively include any dependent ssimfiles, specify `-checkable`:

    $ acr_in sample -checkable -sigcheck:N
    dmmeta.ssimfile  ssimfile:dmmeta.nstype  ctype:dmmeta.Nstype
    dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
    dmmeta.ssimfile  ssimfile:dmmeta.dispsig  ctype:dmmeta.Dispsig
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype

### The -related option

`acr_in` can optionally include only those tuples which are transitively
reachable from a certain set. For this, specify `-related`.
Here is an example where we constrain `sample`'s input to the `sample` namespace itself.

    $ acr_in sample -data -related:dmmeta.ns:sample -sigcheck:N
    dmmeta.ns  ns:sample  nstype:exe  comment:""
    dmmeta.ctype  ctype:sample.FCtype  comment:""
    dmmeta.ctype  ctype:sample.FDb  comment:""
    dmmeta.ctype  ctype:sample.FGitfile  comment:""
    dmmeta.ctype  ctype:sample.FNs  comment:""

In contrast, if we didn't specify `-related`, `-data` would fetch all records:

    $ acr_in sample -data | wc -l
    864
