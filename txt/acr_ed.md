## acr_ed: Acr Editor

`Acr_ed` started its life as a cheat sheet generator, but now it's an indispensable 
tool for common tasks such as creating, deleting, and renaming targets, ctypes, ssimfiles,
xrefs, and source files.

By default, `acr_ed` spits out an executable script to stdout. With
`-write`, the script is executed (this has the same effect as piping output
to sh)

The main commands in acr_ed are:

~~~
-ctype:X -create : Create new ctype. Suboptions: -ssimfile, -cbase
-ctype:X -rename:Y: Rename ctype X to Y
-ctype:X -create -subset:Y -pooltype:Z -indexed: Create a new indexed in-memory table

-ssimfile:X -create : Create new ssimfile. Suboptions: -subset, -subset2, -separator, -ctype
-ssimfile:X -rename:Y: Rename ssimfile (to fully rename an ssimfile, first rename the ssimfile, then the ctype)

-field:X -create: Create field or access path.
  Suboptions: -comment, -arg, -dflt, -reftype, -before, -via, -xref, -fstep, -substr, -anon, -bigend
  Some of these can be guessed by acr_ed. For instance, if program x has tables y and z, then using
    acr_ed -create -field x.FY.zd_z will automatically add a cross-reference ("group-by") of z by y,
    figuring out the projection of z on y and any necessary index through which to find a y record
    given a z.
-finput -create  -ssimfile:X -target:Y: Add ssimfile as finput for a target. Suboptions: -indexed

-target:X -create: Create new target. Issues necessary git commands as well.
-target:X -rename:Y: Rename target. Issues necessary git commands as well.
-target:X -del: Delete a target

-srcfile:X -create -target:Y: Create new source file
-srcfile:X -rename:Y: Rename source file (automatically updates headers)
-srcfile:X -del: Delete source file
~~~

Create new target:

       acr_ed -create -target xyz

Delete target:

       acr_ed -del -target xyz

Create new ssimfile.

	 acr_ed -ssimfile dmmeta.nsref -create

Create new ssimfile as a subset of 2 other ctypes, using separator for the key

	 acr_ed -create -ssimfile dmmeta.nsref -subset dmmeta.Ns -subset2 dmmeta.Reftype -separator /

Create new table for in-memory database (manual).
	 Parameter -reftype Thash is guessed based on field name
	 Parameter -hashfld is guessed based on primary key

	 acr_ed -create -ctype amc.FCtype -cbase dmmeta.Ctype
	 acr_ed -create -field amc.FDb.ctype -arg amc.FCtype  -reftype Lary
	 acr_ed -create -field amc.FDb.ind_ctype -arg amc.FCtype  -xref

Create new table for in-memory database, based on an ssimfile

	 acr_ed -create -finput -target amc -ssimfile dmmeta.ctype

Create new table for in-memory database, based on an ssimfile, add a hash index

	 acr_ed -create -finput -target amc -ssimfile dmmeta.ctype -indexed

Add a pointer from ssimfile -> ctype for target acr_ed
Parameters -reftype, -via are guessed.

Conditional cross-reference

	 acr_ed -create -field acr_ed.FSsimfile.p_ctype -arg acr_ed.FCtype -xref -inscond false

Create new source file:

       acr_ed -create -srcfile cpp/amc/blah.cpp -write

Create new header:

       acr_ed -create -srcfile include/amc/blah.h -target amc -write

