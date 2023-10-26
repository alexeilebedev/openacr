## Tutorial 1: Parts And Projects Tutorial

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

Step 1: Create new target

```
inline-command: acr_ed -create -target tut1 -write
acr_ed.create_target  target:tut1
report.acr_check  records:38  errors:0
report.acr  ***
report.amc  ***
report.acr  ***
please execute $(acr_compl -install) to add completions support for new target
report.amc  ***
```

Step 2: Define data structures for tut1

```
inline-command: acr_ed -create -ctype tut1.Proj -pooltype Tpool -indexed -write
report.acr_check  records:13  errors:0
report.acr  ***
report.amc  ***
```

```
inline-command: acr_ed -create -ctype tut1.Part -pooltype Tpool -indexed -write
report.acr_check  records:13  errors:0
report.acr  ***
report.amc  ***
```

```
inline-command: acr_ed -create -ctype tut1.Partproj -pooltype Tpool  -write
report.acr_check  records:5  errors:0
report.acr  ***
report.amc  ***
```

Step 3: Create reference fields.
The first field is a pointer from Partproj to Part

```
inline-command: acr_ed -create -field tut1.Partproj.p_part -arg tut1.Part -reftype Upptr -write
acr_ed.create_field  field:tut1.Partproj.p_part
report.acr_check  records:5  errors:0
report.acr  ***
report.amc  ***
```

The second field is a pointer from Partproj to Proj

```
inline-command: acr_ed -create -field tut1.Partproj.p_proj -arg tut1.Proj -reftype Upptr -write
acr_ed.create_field  field:tut1.Partproj.p_proj
report.acr_check  records:5  errors:0
report.acr  ***
report.amc  ***
```

Step 4: Create x-reference (cross reference) fields.

An x-reference field is maintained in response to some other insertion. 
A cross reference is a synonym for an index.
First, let's create two global lists so we can scan projects and parts:

```
inline-command: acr_ed -create -field tut1.FDb.zd_part -write -comment "List of all parts"
acr_ed.create_field  field:tut1.FDb.zd_part
tut1.FDb.zd_part
tut1.FDb.part
acr_ed.guess_arg  arg:tut1.Part
report.acr_check  records:9  errors:0
report.acr  ***
report.amc  ***
```

```
inline-command: acr_ed -create -field tut1.FDb.zd_proj -write -comment "List of all projects"
acr_ed.create_field  field:tut1.FDb.zd_proj
tut1.FDb.zd_proj
tut1.FDb.proj
acr_ed.guess_arg  arg:tut1.Proj
report.acr_check  records:9  errors:0
report.acr  ***
report.amc  ***
```

Second, let's add group-bys. The first is a pointer from Part to Partproj that is initialized
in response to a new Partproj being created. A part can only be used in one project, so it's a pointer.

```
inline-command: acr_ed -create -field tut1.Part.c_partproj -reftype Ptr -cascdel -write -comment "List of projects by part"
acr_ed.create_field  field:tut1.Part.c_partproj
tut1.FDb.c_partproj
tut1.FDb.partproj
acr_ed.guess_arg  arg:tut1.Partproj
acr_ed.guess_xref  field:tut1.Partproj.p_part  parentbase:tut1.Part
acr_ed.via_match1  child:tut1.Partproj.p_part  comment:"This child field is a possible via key candidate"
keyfield is tut1.Partproj.p_part, child is tut1.Partproj, is pkey?Y
report.acr_check  records:8  errors:0
report.acr  ***
report.amc  ***
```

The second cross-reference is a doubly linked, zero-terminated list of Partprojs by Proj.

```
inline-command: acr_ed -create -field tut1.Proj.zd_partproj -cascdel -write -comment "List of parts by project"
acr_ed.create_field  field:tut1.Proj.zd_partproj
tut1.FDb.zd_partproj
tut1.FDb.partproj
acr_ed.guess_arg  arg:tut1.Partproj
acr_ed.guess_xref  field:tut1.Partproj.p_proj  parentbase:tut1.Proj
acr_ed.via_match1  child:tut1.Partproj.p_proj  comment:"This child field is a possible via key candidate"
keyfield is tut1.Partproj.p_proj, child is tut1.Partproj, is pkey?N
report.acr_check  records:10  errors:0
report.acr  ***
report.amc  ***
```

Step 5: Enter the program text in cpp/tut1.cpp:

### Program Source

This sample program source comes from `conf/tut1.txt`:

```
inline-command: cat conf/tut1.txt
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
// Target: tut1 (exe)
// Exceptions: yes
// Source: cpp/tut1.cpp
//

#include "include/algo.h"
#include "include/tut1.h"

tut1::Partproj *tut1::CreatePartProj(tut1::Part &part, tut1::Proj &proj) {
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
    ind_beg(tut1::_db_zd_proj_curs,proj,tut1::_db) {
        prlog("  project "<<proj.proj);
        ind_beg(tut1::proj_zd_partproj_curs,partproj,proj) {
            prlog("    has part "<<partproj.p_part->part);
        }ind_end
    }ind_end;
    prlog("list of parts and their projects");
    ind_beg(tut1::_db_zd_part_curs,part,tut1::_db) {
        prlog("  part "<<part.part);
        if (tut1::Partproj *partproj = part.c_partproj) {
            prlog("    has project "<<partproj->p_proj->proj);
        }
    }ind_end;
    prlog("");
}

void tut1::Main() {
    tut1::Proj &proj1 = tut1::ind_proj_GetOrCreate("proj1");
    tut1::Proj &proj2 = tut1::ind_proj_GetOrCreate("proj2");

    tut1::Part &part1 = tut1::ind_part_GetOrCreate("part1");
    tut1::Part &part2 = tut1::ind_part_GetOrCreate("part2");
    tut1::Part &part3 = tut1::ind_part_GetOrCreate("part3");

    // assign parts to projects
    tut1::CreatePartProj(part1,proj1);
    tut1::CreatePartProj(part2,proj1);
    tut1::CreatePartProj(part3,proj2);

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
inline-command: cp conf/tut1.txt cpp/tut1.cpp
```

And build tut1:

```
inline-command: ai tut1
abt.config  config:Linux-g++.release-x86_64  cache:***  out_dir:build/release
abt.outofdate  ***
report.abt  n_target:5  time:***  n_warn:0  n_err:0  n_install:0
```

### Running The Program

When we run tut1, we see the following output:

```
inline-command: tut1
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

Let's use `amc_vis` to visualize the various access paths that we created. 
In the diagram below, every ctype is indicated by a veritcal line:

Fields are shown as horizontal arrows, either left-pointing (references)
Or right-pointing (cross-references, or index fields). This a map of all "access paths"
in the in-memory database of `tut1`

```
inline-command: amc_vis tut1.%
                                                                            
                                                                            
    / tut1.FDb                                                              
    |                                                                       
    |Tpool proj------>/ tut1.Proj                                           
    |Thash ind_proj-->|                                                     
    |Llist zd_proj--->|                                                     
    |                 |                                                     
    |Tpool part-------|------------------->/ tut1.Part                      
    |Thash ind_part---|------------------->|                                
    |Llist zd_part----|------------------->|                                
    |                 |                    |                                
    |Tpool partproj---|--------------------|---------------->/ tut1.Partproj
    -                 |                    |                 |              
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

The code generated by `amc` for this tutorial can be examined interactively,
with `amc tut1.%`, or by studying files `cpp/gen/tut1_gen.cpp`, `include/gen/tut1_gen.h`
and `include/gen/tut1_gen.inl.h` (this last file is where all of the inline
functions are placed).

### Summary

This tutorial has demonstrated the following automatically generated features:

* The shell as an IDE. Creating a new executable, all of its data structures, and all of its indexing
  structures from a command line.
* Automatic insertion of a record into a global list.
* Automation removal of a record from a global list.
* Automatic group-by, by following a reference.
* Automatic deletion of a record when a reference to it would become invalid.
* Custom allocation and deallocation for a record

When creating this tutorial, we accidentally introduced unnecessary (and unused) hash indexes
`tut1.FDb.ind_proj` and `tut1.FDb.ind_part`. We can easily delete them:

```
inline-command: acr_ed -del -field tut1.FDb.ind_proj -write
acr_ed.delete_field  field:tut1.FDb.ind_proj
report.acr_check  records:0  errors:0
report.acr  ***
acr.delete  dmmeta.field  field:tut1.FDb.ind_proj  arg:tut1.Proj  reftype:Thash  dflt:""  comment:""

acr.delete  dmmeta.thash  field:tut1.FDb.ind_proj  hashfld:tut1.Proj.proj  unique:Y  comment:""

acr.delete  dmmeta.xref  field:tut1.FDb.ind_proj  inscond:true  via:""
report.acr  ***
report.amc  ***
```

```
inline-command: acr_ed -del -field tut1.FDb.ind_part -write
acr_ed.delete_field  field:tut1.FDb.ind_part
report.acr_check  records:0  errors:0
report.acr  ***
acr.delete  dmmeta.field  field:tut1.FDb.ind_part  arg:tut1.Part  reftype:Thash  dflt:""  comment:""

acr.delete  dmmeta.thash  field:tut1.FDb.ind_part  hashfld:tut1.Part.part  unique:Y  comment:""

acr.delete  dmmeta.xref  field:tut1.FDb.ind_part  inscond:true  via:""
report.acr  ***
report.amc  ***
```

And rebuild the tutorial. These indexes can be easily introduced later if necessary.

