## acr_ed: Acr Editor

`Acr_ed` started its life as a cheat sheet generator, but now it's an indispensable 
tool for common tasks such as creating, deleting, and renaming targets, ctypes, ssimfiles,
xrefs, and source files.

By default, `acr_ed` spits out an executable script to stdout. With
`-write`, the script is executed (this has the same effect as piping output
to sh)

~~~

-field:X -create: Create field or access path.
  Suboptions: -comment, -arg, -dflt, -reftype, -before, -via, -xref, -fstep, -substr, -anon, -bigend
  Some of these can be guessed by acr_ed. For instance, if program x has tables y and z, then using
    acr_ed -create -field x.FY.zd_z will automatically add a cross-reference ("group-by") of z by y,
    figuring out the projection of z on y and any necessary index through which to find a y record
    given a z.
-finput -create  -ssimfile:X -target:Y: Add ssimfile as finput for a target. Suboptions: -indexed

~~~

### Targets

#### Create Target

    $ acr_ed -create -target:X
    ...

#### Rename Target

    $ acr_ed -target:X -rename:Y
    ...

#### Delete Target

    $ acr_ed -del -target:X
    ...

### Ssimfiles

#### Create Ssimfile

    $ acr_ed -create -ssimfile ns.x 
    ...

#### Create Ssimfile as a Subset of Another Ssimfile

    $ acr_ed -create -ssimfile ns.x -subset ns.Y
    ...

#### Create Ssimfile as a Cross Product of 2 Other Ssimfiles

    $ acr_ed -create -ssimfile ns.x -subset ns.Y -subset2 ns.Z -separator .
    ...

Note that ssimfiles user lowercase names but ctypes use uppercase names.
Options `-subset`, `-subset2` take ctype names.

#### Rename Ssimfile

    $ acr_ed -ssimfile ns.x -rename ns2.y
    ...

#### Delete Ssimfile

    $ acr_ed -del -ssimfile ns.x
    ...

### Source Files

#### Create Source File
When creating a source file, `acr_ed` automatically which target
this file will belong to based on the the other files in the same directory.
If there is ambiguity, specify `-target ...` argument
Headers are considered source files.

    $ acr_ed -create -srcfile cpp/...path.cpp
    ...
    $ acr_ed -create -srcfile include/path.h

#### Rename Source File

    $ acr_ed -srcfile cpp/path.cpp -rename cpp/path2.cpp
    ...

#### Delete Source File

    $ acr_ed -del -srcfile cpp/path.cpp
    ...

### Ctypes

#### Create A Ctype

    $ acr_ed -create -ctype ns.Name
    ...

#### Rename A Ctype

    $ acr_ed -ctype ns.Name -rename ns2.Name2
    ...

#### Delete A Ctype

    $ acr_ed -del -ctype ns.Name
    ...
    
### Add An Input To Program

    $ acr_ed -create -finput -target ns -ssimfile ns2.name
    acr_ed.create_finput  target:sample  ssimfile:dev.gitfile
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:sample.FGitfile  comment:""
    dmmeta.field  field:sample.FGitfile.base  arg:dev.Gitfile  reftype:Base  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.gitfile  arg:sample.FGitfile  reftype:Lary  dflt:""  comment:""
    dmmeta.finput  field:sample.FDb.gitfile  extrn:N  update:N  strict:Y  comment:""
    #  Proposed change
    #
    #
    #     / sample.FDb
    #     |
    #     |Lary gitfile-->/ sample.FGitfile
    #     -               |
    #                     |
    #                     -
    EOF
    ...
    
With `-indexed` option, a hash index is thrown in. This is pretty common.

### Create A New In-Memory Table

To create a new table in a program, use `-create -ctype ... -subset ... -pooltype`.
This adds a global pool of a given type (typically `Tpool` or `Lary`).
To throw in a hash index, specify `-indexed`

    $ acr_ed -create -ctype sample.FTable -subset u32 -pooltype Tpool -indexed
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:sample.FTable  comment:""
    dmmeta.field  field:sample.FTable.table  arg:u32  reftype:Val  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.table  arg:sample.FTable  reftype:Tpool  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.ind_table  arg:sample.FTable  reftype:Thash  dflt:""  comment:""
    dmmeta.thash  field:sample.FDb.ind_table  hashfld:sample.FTable.table  unique:Y  comment:""
    dmmeta.xref  field:sample.FDb.ind_table  inscond:true  via:""
    EOF

### Create An Index

#### Create A Hash Table

    $ acr_ed -create -field sample.FDb.ind_table
    $ acr_ed -create -field sample.FDb.ind_table -hashfld <fieldname>

#### Create A Binary Heap

    $ acr_ed -create -field sample.FDb.bh_table
    $ acr_ed -create -field sample.FDb.bh_table -sortfld <fieldname>

#### Create An AVL Tree

    $ acr_ed -create -field sample.FDb.tr_table
    $ acr_ed -create -field sample.FDb.tr_table -sortfld <fieldname>

### Conditional X-Ref

    $ acr_ed -create -field sample.FDb.ind_table -inscond false
