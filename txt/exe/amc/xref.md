## amc Cross-References (xrefs)
<a href="#amc-cross-references--xrefs-"></a>

Cross-reference or an `xref` is an index formed to keep track of all references to a given record.
Every time a new reference appears or disappears (because a record is added or removed),
the cross-reference is updated.

In database terms, a cross-reference is called a group-by or a partitioned index.
In AMC in-memory databases, a cross-reference is implemented with a pointer, pointer array, hash,
heap, tree, linked list, or count and corresponds to field reftype of
Ptr, Ptrary, Thash, Bheap, Atree, Llist and Count.

A cross-reference is described by use of an xref record. Here is a random example from amc, which
needs to keep track of targets and their dependencies.

    dmmeta.field  field:amc.FTarget.c_targdep  arg:amc.FTargdep  reftype:Ptrary  dflt:"  comment:"
      dmmeta.ptrary  field:amc.FTarget.c_targdep  unique:Y
        dmmeta.xref  field:amc.FTarget.c_targdep  inscond:true  via:amc.FDb.ind_target/dev.Targdep.target

This says: Dear amc, whenever a `targdep` record is inserted in my program, find an instance
of `target` by using global index `ind_target` with key `dev.Targdep.target` as the key, and
insert `targdep` into an array of pointers rooted in `target`.
Whenever a `targdep` record is deleted, automatically remove it from the list.

Xrefs can be added and removed either by hand (with `acr -e` or by editing ssimfiles
directly), or using `acr_ed`. In the example above, `acr_ed -create -field amc.FTarget.c_targdep -write`
would be enough to establish the xref.

There can be any number of xrefs between any two tables.
A special case is when the parent record is a global object (FDb). In this case, `via` need only contain
one field, because no key is needed to find it.

Another special case is when the `via` field is a Upptr. In this case, a second field is also unnecessary
since an Upptr is sufficient to find the target record.

AMC generates a function called `XrefMaybe`, which establishes all cross-references for a given record.
This function is called automatically when inserting a record into an in-memory database, or it can be called
by hand. If the function returns `false`, the x-reference operation failed. It may have failed because
the parent record was not found, a duplicate hash key was used on a unique hash, or the parent index is full
(for instance, if the parent field is just a Ptr,
it can keep track of only one child record). In this case, the child record should be deleted.

Xrefs are exactly analogous to database indexes; except xrefs can be established between any two
tables, so they are also `partitioned indexes` and incremental `group by`s at the same time.
Those two concepts refer to the same thing.

To visualize xrefs, it may be useful to use [amc_vis](/txt/exe/amc_vis/README.md).
To visualize the example above, we would use
```
inline-command: amc_vis amc.FTarget\|amc.FTargdep
                                      / amc.FTargdep  
                  / amc.FTarget       |               
                  |<------------------|Upptr p_target 
                  |Ptrary c_targdep-->|               
                  |<------------------|Upptr p_parent 
                  |                   -               
                  -                                   
```

