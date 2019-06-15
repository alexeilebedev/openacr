## Tutorial 1: Parts And Projects Tutorial

In the tutorial that follows, we'll implement a parts-and-project application, focusing
on automatic group-by and cascade delete operations. In subsequent chapters, we'll examine
how amc inputs are specified using ssim files, and how amc builds its own data
structures using these same inputs.

This tutorial is based on the example from Codd classical 1970 paper
[A Relational Model of Data For Large Shared Data Banks](https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf).
The second tutorial deals with this paper and its implications in more detail; Here,
we just create an in-memory database using these record types, and demonstrate a few useful
`amc` features on it.

### Creating The Program

Step 1: Create new target

    $ acr_ed -create -target tut1 -write

Step 2: Create in-memory tables

    $ acr_ed -create -ctype tut1.Proj -pooltype Tpool -indexed -write
    $ acr_ed -create -ctype tut1.Part -pooltype Tpool -indexed -write
    $ acr_ed -create -ctype tut1.Partproj -pooltype Tpool  -write

Step 3: Create reference fields.

    $ acr_ed -create -field tut1.Partproj.p_part -arg tut1.Part -reftype Upptr -write
    $ acr_ed -create -field tut1.Partproj.p_proj -arg tut1.Proj -reftype Upptr -write

Step 4: Create x-reference (cross reference) fields.
An x-reference field is maintained in response to some other insertion. 
A cross reference is a synonym for an index.
First, let's create two global lists so we can scan projects and parts:

    $ acr_ed -create -field tut1.FDb.zd_part -write -comment "List of all parts"
    $ acr_ed -create -field tut1.FDb.zd_proj -write -comment "List of all projects"

Second, let's add group-bys:

    $ acr_ed -create -field tut1.Part.c_partproj -reftype Ptr -cascdel -write -comment "List of projects by part"
    $ acr_ed -create -field tut1.Proj.zd_partproj -cascdel -write -comment "List of parts by project"

Step 5: Enter the following program text in cpp/tut1.cpp:

### Program Source

```
cat > cpp/tut1.cpp << EOF
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
EOF
```

### Running The Program

When we run tut1, we see the following output:

```
$ tut1
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

    / tut1.FDb
    |
    |
    -

Fields are shown as horizontal arrows, either left-pointing (references)
Or right-pointing (cross-references, or index fields).

    $ amc_vis tut1.%

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

### Generated Code

The code generated by `amc` for this tutorial can be examined interactively,
with `amc tut1.%`, or by studying files `cpp/gen/tut1_gen.cpp`, `include/gen/tut1_gen.h`
and `include/gen/tut1_gen.inl.h` (this last file is where all of the inline
functions are placed).

### Summary

This tutorial has demonstrated the following automatically generated features:

* The shell as an IDE. Creating a new executable, all of its data structures, and all of its indexing
  structures from a command-line.
* Automatic insertion of a record into a global list.
* Automation removal of a record from a global list
* Automatic group-by, by following a reference
* Automatic deletion of a record when a reference to it would become invalid.
* Custom allocation and deallocation for a record

When creating this tutorial, we accidentally introduced unnecessary (and unused) hash indexes
`tut1.FDb.ind_proj` and `tut1.FDb.ind_part`. We can easily delete them:

    acr_ed -del -field tut1.FDb.ind_proj -write
    acr_ed -del -field tut1.FDb.ind_part -write

And rebuild the tutorial. These indexes can be easily introduced later if necessary.
    
### Cleanup

Let's delete this tutorial.

    $ acr_ed -del -target tut1 -write
