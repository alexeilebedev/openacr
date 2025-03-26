## Tutorial 1: Parts And Projects Tutorial
<a href="#tutorial-1-parts-and-projects-tutorial"></a>
In the tutorial that follows we create a new program and its
runtime data structures through shell commands.

The program is a simple parts-and-project application, with
automatic group-by and cascade delete operations.

The data structure is described entirely through ssim tables, and the code for it
is generated with `amc`.

The tutorial is based on the example from Codd classical 1970 paper
[A Relational Model of Data For Large Shared Data Banks](https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf).

This is a live tutorial. All of the code here has been evaluated during
CI by running it in a sandbox

### Creating The Program
<a href="#creating-the-program"></a>
Step 1: Create new target.

```
inline-command: acr_ed -create -target samp_tut1 -write
acr_ed.create_target  target:samp_tut1
report.acr_check  records:***  errors:0
acr.insert  dev.gitfile  gitfile:bin/samp_tut1
acr.insert  dev.gitfile  gitfile:cpp/gen/samp_tut1_gen.cpp
acr.insert  dev.gitfile  gitfile:cpp/samp_tut1/samp_tut1.cpp
acr.insert  dev.gitfile  gitfile:include/gen/samp_tut1_gen.h
acr.insert  dev.gitfile  gitfile:include/gen/samp_tut1_gen.inl.h
acr.insert  dev.gitfile  gitfile:include/samp_tut1.h
acr.insert  dev.gitfile  gitfile:txt/exe/samp_tut1/README.md
  acr.insert  dev.readme  gitfile:txt/exe/samp_tut1/README.md      inl:N   sandbox:N   filter:""  comment:""

acr.insert  dev.gitfile  gitfile:txt/exe/samp_tut1/internals.md
  acr.insert  dev.readme  gitfile:txt/exe/samp_tut1/internals.md  inl:N   sandbox:N   filter:""  comment:""

acr.insert  dmmeta.ns  ns:samp_tut1  nstype:exe  license:GPL  comment:""
  acr.insert  dev.target  target:samp_tut1
    acr.insert  dev.targdep  targdep:samp_tut1.algo_lib  comment:""
    acr.insert  dev.targdep  targdep:samp_tut1.lib_prot  comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut1/cpp/gen/samp_tut1_gen.cpp        comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut1/cpp/samp_tut1/samp_tut1.cpp      comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut1/include/gen/samp_tut1_gen.h      comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut1/include/gen/samp_tut1_gen.inl.h  comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut1/include/samp_tut1.h              comment:""

  acr.insert  dmmeta.ctype  ctype:samp_tut1.FDb  comment:""
    acr.insert  dmmeta.field  field:samp_tut1.FDb._db      arg:samp_tut1.FDb      reftype:Global  dflt:""  comment:""
    acr.insert  dmmeta.field  field:samp_tut1.FDb.cmdline  arg:command.samp_tut1  reftype:Val     dflt:""  comment:""
      acr.insert  dmmeta.fcmdline  field:samp_tut1.FDb.cmdline  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:""

  acr.insert  dmmeta.main  ns:samp_tut1  ismodule:N
  acr.insert  dmmeta.nscpp  ns:samp_tut1  comment:""
  acr.insert  dmmeta.nsx  ns:samp_tut1  genthrow:Y  correct_getorcreate:Y  pool:algo_lib.FDb.malloc  sortxref:Y  pack:N  comment:""

acr.insert  dmmeta.ctype  ctype:command.samp_tut1  comment:""
  acr.insert  dmmeta.field  field:command.samp_tut1.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"
    acr.insert  dmmeta.floadtuples  field:command.samp_tut1.in  comment:""

  acr.insert  dmmeta.cfmt  cfmt:command.samp_tut1.Argv  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
abt.config  builddir:***  ood_src:***  ood_target:***  cache:***
report.abt  n_target:***  time:***  hitrate:***  pch_hitrate:***  n_warn:0  n_err:0  n_install:***
please execute $(acr_compl -install) to add completions support for new target
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

Step 2: Define data structures for samp_tut1

```
inline-command: acr_ed -create -ctype samp_tut1.Proj -pooltype Tpool -indexed -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.ctype  ctype:samp_tut1.Proj  comment:""
  acr.insert  dmmeta.field  field:samp_tut1.Proj.proj  arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""

acr.insert  dmmeta.field  field:samp_tut1.FDb.proj      arg:samp_tut1.Proj  reftype:Tpool  dflt:""  comment:""
acr.insert  dmmeta.field  field:samp_tut1.FDb.ind_proj  arg:samp_tut1.Proj  reftype:Thash  dflt:""  comment:""
  acr.insert  dmmeta.thash  field:samp_tut1.FDb.ind_proj  hashfld:samp_tut1.Proj.proj  unique:Y  comment:""
  acr.insert  dmmeta.xref  field:samp_tut1.FDb.ind_proj  inscond:true  via:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

```
inline-command: acr_ed -create -ctype samp_tut1.Part -pooltype Tpool -indexed -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.ctype  ctype:samp_tut1.Part  comment:""
  acr.insert  dmmeta.field  field:samp_tut1.Part.part  arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""

acr.insert  dmmeta.field  field:samp_tut1.FDb.part      arg:samp_tut1.Part  reftype:Tpool  dflt:""  comment:""
acr.insert  dmmeta.field  field:samp_tut1.FDb.ind_part  arg:samp_tut1.Part  reftype:Thash  dflt:""  comment:""
  acr.insert  dmmeta.thash  field:samp_tut1.FDb.ind_part  hashfld:samp_tut1.Part.part  unique:Y  comment:""
  acr.insert  dmmeta.xref  field:samp_tut1.FDb.ind_part  inscond:true  via:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

```
inline-command: acr_ed -create -ctype samp_tut1.Partproj -pooltype Tpool  -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.ctype  ctype:samp_tut1.Partproj  comment:""
acr.insert  dmmeta.field  field:samp_tut1.FDb.partproj  arg:samp_tut1.Partproj  reftype:Tpool  dflt:""  comment:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

Step 3: Create reference fields.
The first field is a pointer from Partproj to Part

```
inline-command: acr_ed -create -field samp_tut1.Partproj.p_part -arg samp_tut1.Part -reftype Upptr -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut1.Partproj.p_part  arg:samp_tut1.Part  reftype:Upptr  dflt:""  comment:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

The second field is a pointer from Partproj to Proj

```
inline-command: acr_ed -create -field samp_tut1.Partproj.p_proj -arg samp_tut1.Proj -reftype Upptr -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut1.Partproj.p_proj  arg:samp_tut1.Proj  reftype:Upptr  dflt:""  comment:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

Step 4: Create x-reference (cross reference) fields.

An x-reference field is maintained in response to some other insertion. 
A cross reference is a synonym for an index.
First, let's create two global lists so we can scan projects and parts:

```
inline-command: acr_ed -create -field samp_tut1.FDb.zd_part -write -comment "List of all parts"
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut1.FDb.zd_part  arg:samp_tut1.Part  reftype:Llist  dflt:""  comment:"List of all parts"
  acr.insert  dmmeta.llist  field:samp_tut1.FDb.zd_part  havetail:Y  havecount:Y  comment:""
  acr.insert  dmmeta.xref  field:samp_tut1.FDb.zd_part  inscond:true  via:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

```
inline-command: acr_ed -create -field samp_tut1.FDb.zd_proj -write -comment "List of all projects"
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut1.FDb.zd_proj  arg:samp_tut1.Proj  reftype:Llist  dflt:""  comment:"List of all projects"
  acr.insert  dmmeta.llist  field:samp_tut1.FDb.zd_proj  havetail:Y  havecount:Y  comment:""
  acr.insert  dmmeta.xref  field:samp_tut1.FDb.zd_proj  inscond:true  via:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

Second, let's add group-bys. The first is a pointer from Part to Partproj that is initialized
in response to a new Partproj being created. A part can only be used in one project, so it's a pointer.

```
inline-command: acr_ed -create -field samp_tut1.Part.c_partproj -reftype Ptr -cascdel -write -comment "List of projects by part"
acr_ed.via_match1  child:samp_tut1.Partproj.p_part  comment:"This child field is a possible via key candidate"
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut1.Part.c_partproj  arg:samp_tut1.Partproj  reftype:Ptr  dflt:""  comment:"List of projects by part"
  acr.insert  dmmeta.cascdel  field:samp_tut1.Part.c_partproj  comment:""
  acr.insert  dmmeta.xref  field:samp_tut1.Part.c_partproj  inscond:true  via:samp_tut1.Partproj.p_part
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

The second cross-reference is a doubly linked, zero-terminated list of Partprojs by Proj.

```
inline-command: acr_ed -create -field samp_tut1.Proj.zd_partproj -cascdel -write -comment "List of parts by project"
acr_ed.via_match1  child:samp_tut1.Partproj.p_proj  comment:"This child field is a possible via key candidate"
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut1.Proj.zd_partproj  arg:samp_tut1.Partproj  reftype:Llist  dflt:""  comment:"List of parts by project"
  acr.insert  dmmeta.cascdel  field:samp_tut1.Proj.zd_partproj  comment:""
  acr.insert  dmmeta.llist  field:samp_tut1.Proj.zd_partproj  havetail:Y  havecount:Y  comment:""
  acr.insert  dmmeta.xref  field:samp_tut1.Proj.zd_partproj  inscond:true  via:samp_tut1.Partproj.p_proj
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

Step 5: Enter the program text in cpp/samp_tut1.cpp:

### Program Source
<a href="#program-source"></a>
This samp_tut1 program source comes from `conf/samp_tut1.txt`:

```
inline-command: cat conf/samp_tut1.txt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: samp_tut1 (exe)
// Exceptions: yes
// Source: cpp/samp_tut1.cpp
//

#include "include/algo.h"
#include "include/samp_tut1.h"

samp_tut1::Partproj *samp_tut1::CreatePartProj(samp_tut1::Part &part, samp_tut1::Proj &proj) {
    Partproj *pp = &partproj_Alloc();
    pp->p_part = &part;
    pp->p_proj = &proj;
    if (!partproj_XrefMaybe(*pp)) {
        partproj_Delete(*pp);
        pp=NULL;
    }
    return pp;
}

static void Show() {
    prlog("list of projects and their parts");
    ind_beg(samp_tut1::_db_zd_proj_curs,proj,samp_tut1::_db) {
        prlog("  project "<<proj.proj);
        ind_beg(samp_tut1::proj_zd_partproj_curs,partproj,proj) {
            prlog("    has part "<<partproj.p_part->part);
        }ind_end
    }ind_end;
    prlog("list of parts and their projects");
    ind_beg(samp_tut1::_db_zd_part_curs,part,samp_tut1::_db) {
        prlog("  part "<<part.part);
        if (samp_tut1::Partproj *partproj = part.c_partproj) {
            prlog("    has project "<<partproj->p_proj->proj);
        }
    }ind_end;
    prlog("");
}

void samp_tut1::Main() {
    samp_tut1::Proj &proj1 = samp_tut1::ind_proj_GetOrCreate("proj1");
    samp_tut1::Proj &proj2 = samp_tut1::ind_proj_GetOrCreate("proj2");

    samp_tut1::Part &part1 = samp_tut1::ind_part_GetOrCreate("part1");
    samp_tut1::Part &part2 = samp_tut1::ind_part_GetOrCreate("part2");
    samp_tut1::Part &part3 = samp_tut1::ind_part_GetOrCreate("part3");

    // assign parts to projects
    samp_tut1::CreatePartProj(part1,proj1);
    samp_tut1::CreatePartProj(part2,proj1);
    samp_tut1::CreatePartProj(part3,proj2);

    // list parts
    prlog("initial setup");
    Show();

    // delete a part -- this will remove it from any projects
    // that reference the part
    prlog("deleting part1");
    part_Delete(part1);
    Show();
    // let's not reference part1 beyond this point

    // delete a project
    prlog("deleting proj2");
    proj_Delete(proj2);
    // now part3 will become unassigned
    Show();

    prlog("deleting proj1");
    proj_Delete(proj1);
    // at this point, part2 and part3 remain,
    // but they aren't attached to any project
    Show();
}
```

Let's copy it into place

```
inline-command: cp conf/samp_tut1.txt cpp/samp_tut1/samp_tut1.cpp
```

And build samp_tut1:

```
inline-command: ai samp_tut1
abt.config  builddir:***  ood_src:***  ood_target:***  cache:***
report.abt  n_target:***  time:***  hitrate:***  pch_hitrate:***  n_warn:0  n_err:0  n_install:***
```

### Running The Program
<a href="#running-the-program"></a>
When we run samp_tut1, we see the following output:

```
inline-command: samp_tut1
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
list of parts and their projects
  part part2
    has project proj1
  part part3
    has project proj2

deleting proj2
list of projects and their parts
  project proj1
    has part part2
list of parts and their projects
  part part2
    has project proj1
  part part3

deleting proj1
list of projects and their parts
list of parts and their projects
  part part2
  part part3

```

### Visualizing the In-Memory Database
<a href="#visualizing-the-in-memory-database"></a>
Let's use `amc_vis` to visualize the various access paths that we created. 
In the diagram below, every ctype is indicated by a veritcal line:

Fields are shown as horizontal arrows, either left-pointing (references)
Or right-pointing (cross-references, or index fields). This a map of all "access paths"
in the in-memory database of `samp_tut1`

```
inline-command: amc_vis samp_tut1.%
                                                                                     
                                                                                     
    / samp_tut1.FDb                                                                  
    |                                                                                
    |Tpool proj---------->/ samp_tut1.Proj                                           
    |Thash ind_proj------>|                                                          
    |Llist zd_proj------->|                                                          
    |                     |                                                          
    |Tpool part-----------|------------------->/ samp_tut1.Part                      
    |Thash ind_part-------|------------------->|                                     
    |Llist zd_part--------|------------------->|                                     
    |                     |                    |                                     
    |Tpool partproj-------|--------------------|---------------->/ samp_tut1.Partproj
    -                     |                    |                 |                   
                          |                    |                 |                   
                          |<-------------------|-----------------|Upptr p_proj       
                          |                    |<----------------|Upptr p_part       
                          |Llist zd_partproj---|---------------->|                   
                          -                    |                 |                   
                                               |                 |                   
                                               |Ptr c_partproj-->|                   
                                               -                 |                   
                                                                 |                   
                                                                 -                   
```

### Generated Code
<a href="#generated-code"></a>
The code generated by `amc` for this tutorial can be examined interactively,
with `amc samp_tut1.%`, or by studying files `cpp/gen/samp_tut1_gen.cpp`, `include/gen/samp_tut1_gen.h`
and `include/gen/samp_tut1_gen.inl.h` (this last file is where all of the inline
functions are placed).

### Summary
<a href="#summary"></a>
This tutorial has demonstrated the following automatically generated features:

* The shell as an IDE. Creating a new executable, all of its data structures, and all of its indexing
  structures from a command line.
* Automatic insertion of a record into a global list.
* Automation removal of a record from a global list.
* Automatic group-by, by following a reference.
* Automatic deletion of a record when a reference to it would become invalid.
* Custom allocation and deallocation for a record

When creating this tutorial, we accidentally introduced unnecessary (and unused) hash indexes
`samp_tut1.FDb.ind_proj` and `samp_tut1.FDb.ind_part`. We can easily delete them:

```
inline-command: acr_ed -del -field samp_tut1.FDb.ind_proj -write
report.acr_check  records:***  errors:0
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
acr.delete  dmmeta.field  field:samp_tut1.FDb.ind_proj  arg:samp_tut1.Proj  reftype:Thash  dflt:""  comment:""

acr.delete  dmmeta.thash  field:samp_tut1.FDb.ind_proj  hashfld:samp_tut1.Proj.proj  unique:Y  comment:""

acr.delete  dmmeta.xref  field:samp_tut1.FDb.ind_proj  inscond:true  via:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

```
inline-command: acr_ed -del -field samp_tut1.FDb.ind_part -write
report.acr_check  records:***  errors:0
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
acr.delete  dmmeta.field  field:samp_tut1.FDb.ind_part  arg:samp_tut1.Part  reftype:Thash  dflt:""  comment:""

acr.delete  dmmeta.thash  field:samp_tut1.FDb.ind_part  hashfld:samp_tut1.Part.part  unique:Y  comment:""

acr.delete  dmmeta.xref  field:samp_tut1.FDb.ind_part  inscond:true  via:""
report.acr  n_select:***  n_insert:***  n_delete:***  n_ignore:***  n_update:***  n_file_mod:***
report.amc  n_cppfile:***  n_cppline:***  n_ctype:***  n_func:***  n_xref:***  n_filemod:***
```

And rebuild the tutorial. These indexes can be easily introduced later if necessary.

