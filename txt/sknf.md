## Structured Keys

This chapter looks at ssimfiles and some theory behind them.

### Ssimfiles

`Ssim`, which stands for *super-simple*, is a line-oriented text format for
describing data in the form of tables of tuples. Each tuple consists
of a type tag and key-value pairs called attributes. The first
key-value pair is a primary key.

    $ acr ctype:amc.% | head
    dmmeta.ctype  ctype:amc.BltinId       comment:""
    dmmeta.ctype  ctype:amc.CppkeywordId  comment:""
    dmmeta.ctype  ctype:amc.Enumstr       comment:Key
    dmmeta.ctype  ctype:amc.FAnonfld      comment:""
    ^^^^^^^^^^^^        ^^^^^^^^^^^^
        |                    |
        type tag             primary key

Every line is treated as an element of a set.  There are no headers or
footers or other file markers, although lines can be commented out with `#`.
Any concatenation, permutation, or subset of two ssim files is a
valid ssim file, just like you would expect with sets.

Leading and trailing whitespace, and whitespace between key-value pairs
is ignored, and may be used to aid legibility.

Both keys and values may encode arbitrary byte sequences. A string
containing non-obvious characters and be enclosed either with single
or double quotes (there being no difference between these types of quotes),
and inside the quotes, C++ string rules exactly apply. So "\t":"\001" is a valid
key-value pair. The list of ssim characters not requiring quotes is given by
`acr charset:%.SsimQuotesafe`.

A single file can contain tuples of any type.

### Ssim Data Sets

A ssim file can be a part of a data set, or stand-alone.
Both are called `ssimfiles` and use the extension `.ssim`.
There is one data set in this project, it is in the directory "data". 
In it, there is one directory per namespace, and physical file for tuples of each kind.

In this data set, there is both data (such as the list of supported compilers)
and *data about data*, such as the list of ssimfiles, or the descriptions of 
columns. Meta-data is in the directory `data/dmmeta`, where `dmmeta` stands
for "data model meta". 

In the database world, it is customary to separate `data` from `schema`. 
I find this distinction artificial, as it depends on the point of view.
Depending on the tool you're writing, these labels may change. We simply say that
there exist various sets, grouped into namespaces for convenience.

The list of all ssimfiles is provided by "acr ssimfile".
The list of all attribtutes is provided by "acr field".

Ssim tuples can also be stored together in a file. Acr can read and write those
tuples. One can also use grep, sed, awk, and other line-oriented tools to access, edit,
and mutilate these records.

All amc-generated programs support the `-in` argument which specifies the input
data set for the tool -- either a file or a directory. By default it's `"data"`.

Acr additionally supports the `-schema` argument, specify the location from which it 
loads the schema. But default it's also `"data"`. This allows having many data-oriented
data sets which all use the same schema, and keeping the schema in the default location.

### Structured Key Normal Form

There is a lot of literature on how to construct database schema so that it doesn't
have anomalies, and how to create primary keys. There are 6 or 7 'normal forms' -- invariants
that have to hold if you want certain anomalies to be absent from your schema.
Anomalies are logical inconsistencies that arise as a result of operations such as join, update, insert
or delete.

Here I will describe the Structured Key Normal Form, or SKNF, which is widely used throughout OpenACR.
SKNF has no anomalies by construction and requires only one normal form.
All it boils down to is this:
a single field, the first field of a table, is the primary key, and it is either a simple type,
or a cross product of two other keys (which is the same thing if you allow for an empty set).

All other columns are non-NULL, and are also either simple types, or must refer to a key in some other table.
When the primary key is a cross-product of two other sets, for instance dmmeta.Ctype, 
where `dmmeta` refers to `ns:dmmeta` and ctype Ctype is a string, we use a separator, in this case '.'.

To decompose a domain into ssimfiles, perform cardinality analysis, meaning that you break the domain up
into sets, where each set has 1 or more values attached to a key, and the key is a structured
one as described above. 

If you need some column to be NULLable, there is no action available. Instead, delete
the column, and create a new table which is a subset of the original table.
Deleting the rows from this new table is equivalent to NULLing the original fields.

There are no constraints other than foreign key constraints in ssim databases,
and since they are kept in text files, the only storage type is string (for acr, all values
are just strings).

### An example of a ssim namespace

Let's consider the domain of programs, such as the one found here in OpenACR.
Since we need to attach various properties to these programs in order to do stuff with them,
we create a number of tables to describe them.

First, we have the set of all binaries. We can call it `target`, meaning "build target". 
Then, we have the set of all files; We'll call it `gitfile`.
When naming a set, we don't use plurals; we always use singular.

Finally, to specify that a source file belongs to some target, we create a table `targsrc`
as a cross product of `target` and `gitfile`.
We make `target` a subset of `ns`, since all targets have namespaces describing them,
but not all namespaces become build targets (i.e. `dmmeta` doesn't have a target).

The resulting decomposition is shown below with `amc_vis`. 

    $ amc_vis dev.Target\|dmmeta.Ns\|dev.Targsrc\|dev.Gitfile

                    / dev.Gitfile
                    |
                / dmmeta.Ns
                |   |
                |   |              / dev.Targsrc
                |   |<-------------|Pkey src
                |   -              |
                |                  |
                |                  |
                |   / dev.Target   |
                |<--|Pkey target   |
                -   |              |
                    |              |
                    |<-------------|Pkey target
                    |              -
                    |
                    -

Notice that all arrows point left.
This is important. Left-pointing arrows are *references*.
When visualizing ssim databases, you will never find right-pointing arrows, 
because these databases do not support indexes. Indexes are built on-the-fly by
acr by inverting the references.

Indexes appear whenever we need to quickly and repeatedly answer the question
"which records point to this record?". Another name for an index is a cross-reference,
since it is computed by taking the refefence as an input. 
Here is an example of an in-memory database built specifically
for abt in accordance with the above schema.
The various linked lists, hashes, and pointer arrays, are all indexes that 
are appropriate for this application.

    amc_vis abt.FDb\|abt.FTarget\|abt.FTargsrc

    / abt.FDb
    |
    |Lary targsrc---------------------------------->/ abt.FTargsrc
    |Thash ind_targsrc----------------------------->|
    |                                               |
    |Lary target--------------->/ abt.FTarget       |
    |Thash ind_target---------->|                   |
    |Llist zs_sel_target------->|                   |
    |Llist zsl_libdep_visited-->|                   |
    |Llist zsl_libdep---------->|                   |
    |Llist zs_origsel_target--->|                   |
    -                           |                   |
                                |                   |
                                |Ptrary c_targsrc-->|
                                |<------------------|Upptr p_target
                                |                   -
                                |
                                -

### Cardinality Analysis

If we view each element of a set as a struct with several fields, then the set of
fields which can be used to distinguish one element from another is called a key.

Many such keys are possible. For instance, we could generate a globally unique ID (GUID)
or get a sequence number from some service, and attach it to the elements of our
set as a key; We could lay our records end-to-end in RAM and use the start address
of each record as its key. All of these are valid keys.
Keys that cannot be guessed by examining the elements of a set are called
*surrogate key*. 

It should be obvious that a surrogate key is not a good thing 
from the modeling standpoint. It requires that you extend your set of elements
with some arbitrary data. But surrogate keys are not just prevalent, they are 
pretty much *the only* keying mechanism used by many database designers. 
Any time you see a table with a field called `id`, which is usually some large integer,
you are looking at a surrogate key.
 
There is even an argument that surrogate keys are good since they protect the user from having
to know the schema. But the problem with surrogate keys is that, as pointed out above,
they are not guessable,
and so two people cooperating on constructing the same table without communicating
with each other will run into a conflict: they will certainly include duplicate elements
into the table, marked with different surrogate keys. To me, this is a disqualifying
argument. A cooperative, scalable software ecosystem must use keys that don't depend
on who's creating them.
Surely, a method for creating keys that doesn't depend on who's applying it must exist.

If surrogate keys don't solve the problem of constructing the set. What does?

Cardinality analysis does. The cardinality of each set is either either an empty value,
an integer, a string, or a cross product of two other sets. 

Decomposing your domain into sets based on *cardinality alone* has the property of being
replicable -- if two people go into separate rooms and each design a schema for the same domain,
they will likely arrive at the same decomposition except perhaps for the spelling of names. 

### Structured Keys Are Complex Domains

In database terms, a domain is what in type theory would be considered a type. It's just 
some set, like integer, string, or a cross-product of two sets.

Codd was much of keeping each column of a table a simple domain.
The primary key would be described as a concatenation of several such fields. 

He arrived at the simple domain rule through his procedure of removing access path dependence
by adding all of the components of all access paths as columns to a table.
It seemed like a great idea at the time, and it *was* a great idea.

Many years later, it became clear that having removed access path dependence from the data, 
simple domains retain a different access path -- that of key structure. Every join statement begins to 
reflect the structure of the keys, and the more information-rich your sets, the more complicated the joins.
Changing the structure of any key now results in waves of changes across any code that 
uses these keys.

Let's look at SQL's timestamp type itself: it is a complex domain composed of year,
month, day, hour, minute, second and nanosecond. But the simple domain rule would
require 7 columns to be used every time a timestamp field is needed, and using 7-term joins
in order to join two tables on a timestamp field.
Thankfully, SQL ignores the simple domain rule in this instance, and defines special functions for
projecting this complex domain onto its different components.

It is simply more natural to view the SQL timestamp as both a single item with some canonic
string representation, and a structure comprised of data fields and computed fields such as 
week-of-year. A URL, with its multitude of components, is another candidate. A 2D point, a 3D point,
a complex number, are all useful complex domains and we absolutely need them to remain that 
way.

Structuring allows us to replace two things with one thing, and while all the procedures that
we defined for the simple parts continue to work on more complex part. It is self-similar
at different scales.

To summarize, OpenACR is squarely in the structured camp, if such a camp even exists,
and definitely opposed to the simple domain camp in all its forms: surrogate keys, id columns, etc.

### No nulls

Simple domains weren't the only thing that Codd strongly advocated for.
He was also in favor of the 4-valued boolean logic, where the result of any expression has
"yes", "no", "NULL and doesn't matter", and "NULL and it does matter". He had a lot
of trouble convincing engineers to implement this 4-valued logic, which was necessary
for logical consistency in presence of NULLable columns. Codd was right. If you have NULLs
and don't use 4-valued logic, you have consistency issues. 

But 50 years later, with some hindsight, we can suggest a different solution: 
why not just throw away NULLs? 
When you get rid of NULLs, you are naturally pushed toward columnar storage, 
since you still need to support missing values at various stages of your data set lifetime, 
but your missing values simply become missing rows.
And it's OK for NULLs to occur, but only for in-memory purposes. 
If can think of a pointer as a NULL-terminated inline array of cross-references, then 
checking for NULL is just checking whether the 1-element array is empty.

### Summary

If it weren't for the simple domain rule and NULLs, the proliferation of normal forms
wouldn't exist. 

And so SKNF represents a very simple but stable point in the space of
all possible schemas of schemas, where you don't have NULLs and every key is
just a single composite value. It scales indefinitely, and every join takes just 2 values.

Perhaps in the clade of DBMS construction philosophies,
the closest analog to SKNF would be DKNF (Domain Key Normal Form).
