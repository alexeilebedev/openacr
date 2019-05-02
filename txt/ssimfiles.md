## Ssim files

Ssim is a Super-Simple line-oriented text format for
storing configuration data in the form of tables of tuples. Each tuple consists
of a type tag and key-value pairs called attributes. The first
key-value pair is a primary key.

    $ acr ctype:amc.% | head
    dmmeta.ctype  ctype:amc.BltinId       comment:""
    dmmeta.ctype  ctype:amc.CppkeywordId  comment:""
    dmmeta.ctype  ctype:amc.Enumstr       comment:Key
    dmmeta.ctype  ctype:amc.FAnonfld      comment:""
    ^^type tag          ^^primary key

Every line is treated as an element of a set.  There are no headers or
footers or other file markers, although lines can be commented out with #.
Any concatenation, permutation, or subset of two ssim files is a
valid ssim file, just like you would expect with sets.

Both keys and values may be arbitrary byte sequences. A string
containing non-obvious characters and be enclosed either with single
or double quotes (there being no difference between these types of quotes),
and inside the quotes, C++ string rules exactly apply. So "\t":"\001" is a valid
key-value pair.

A ssimfile maps directly to a relational table, and each line corresponds to a record
in a database.

In a given data set,
The list of all ssim files is provided by "acr ssimfile".
The list of all attrbitutes is provided by "acr field"

Ssim tuples are typically held in data sets, (with one directory per
namespace, and one file per table) or in a single file.  One can use
grep, sed, awk, and other line-oriented tools to access, edit,
and multilate these records.

All amc-generated commands support the -in argument which specifies the input
data set for the tool -- either a file or a directory. By default it's "data"

### Structured Key Normal Form

There is a lot of literature on how to construct database schema so that it doesn't
have anomalies, and how to create primary keys. There are 6 or 7 'normal forms' -- invariants
that have to hold if you want certain anomalies to be absent from your schema.
Anomalies are logical inconsistencies that arise as a result of operations such as join, update, insert
or delete.

Here I will describe the Structured Key Normal Form, or SKNF, which all OpenACR
schemas use. SKNF has no anomalies by construction and requires only one normal form.
All it boils down to is this:
a single field, the first field of a table, is the primary key, and it is either a simple type,
or a cross product of two other keys (which is the same thing if you allow for an empty set).

All other columns are non-NULL, and are also either simple types, or must refer to a key of some other
table. acr treats the key as a regular string.
When the primary key is a cross-product of two other sets, for instance dmmeta.Ctype, where dmmeta refers to ns:dmmeta
and ctype Ctype is a string, we use a separator, in this case '.'.

If you need some column to be NULLable, you delete
the column, and create a new ssimfile which is a subset of the original file.
Deleting the rows from this new ssimfile is equivalent to NULLing the original fields.

Let's look at an example:

    $ acr ctype:dmmeta.Ctype
    dmmeta.ctype  ctype:dmmeta.Ctype  comment:"C structure"

Here, the primary key is the string 'dmmeta.Ctype'.

But the key is restricted by a foreign key constraint.
Let's try acr with -fldfunc argument:

    $ acr ctype:dmmeta.Ctype -fldfunc
    dmmeta.ctype  ctype:dmmeta.Ctype  ns:dmmeta  name:Ctype  comment:"C structure"

If we were to rename this element to xyz.Ctype, we'd get an acr -check
error, since xyz is not a valid namespace.

dmmeta.Ctype.ns is an attribute of dmmeta.Ctype, and it's defined as a function of other fields,
or 'fldfunc' for short. The acr option -fldfunc expands all fldfuncs when showing output.
The 'ns' field in the query above is defined like this:

      dmmeta.field  field:dmmeta.Ctype.ns  arg:dmmeta.Ns  reftype:Pkey  dflt:""  comment:"translates to c++ namespace"
        dmmeta.substr  field:dmmeta.Ctype.ns  expr:.RL  srcfield:dmmeta.Ctype.ctype

There are no constraints other than foreign key constraints in ssim databases,
and since they are kept in text files, the only storage type is string (in memory it can be
a different story).

acr -check can detect some errors such as strings that are too long for the underlying specified type,
but that's not really a database constraint.

### History of Database Design

In his seminal paper (https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf)
Codd explained how to decompose a *data structure* into a *set of tables* and *access paths*,
where each access path is also a table.
This was a crucial insight. Attaching an access path to a data set and calling
it a data structure results in anomalies which lie at the root of many, many bugs,
and so must be avoided. An anomaly is basically anything that dangles. 
Any value that was supposed to be updated but wasn't is an anomaly. Anomalies are just bugs.

For the first time, way after languages
such as Algol were designed and published, the world of software construction
acquired one of the most important tools -- the factoring of a state space into a
set of sets, and a set of access paths between them. This decomposition is similar 
in importance, and in fact identical in princple, to the first successful axiomatization
of mathematics, the Zermelo–Fraenkel set theory.
Data structure was replaced by a set and an access path, and the access path
is itself a set.

Whenever we talk of "linked list of X" or "hash table of X", we are introducing
invisible anomalies by presupposing that all X are reachable via said access path.
To avoid this anomaly, we need to first think of X as a thing in itself, and only attach 
indexes later.

### Cardinality Analysis

If we view each element of a set as a struct with several fields, then the set of
fields which can be used to distinguish this element from others is called a key.
Many such keys are possible. For instance, we could generate a globally unique ID (GUID)
or get a sequence number from some service, and attach it to the elements of our
set as a key; This would be called a 'surrogate key'. In fact, most relational databases
blindly use surrogate keys (a field called 'id') for most purposes. There is even
an argument that surrogate keys are good since they protect the user from having
to know the schema. But the problem with surrogate keys is that they are not guessable,
and so two people cooperating on constructing the same table without communicating
with each other will run into a conflict: they will certainly include duplicate elements
into the table, marked with different surrogate keys.

So, surrogate keys don't solve the problem of constructing the set. What does?

Cardinality analysis does. The cardinality of each set is either either an integer, a string,
same as that of another set (i.e. a subset), or a cross product of two sets.

Decomposing your domain into sets based on the cardinality alone has the property of being
replicable -- if two people go into separate rooms and each design a schema for the same domain,
they will arrive at the same decomposition module spelling of names. This is important 
for collaborations as it simplifies merging.

### The Curse Of Simple Domains

Codd was much of in favor of simple domains, where each column (field of a struct)
is either an integer or a string, and the primary key is described as a concatenation
of several such fields. This principle is not scalable, because if you use simple domains,
you get very complicated joins which are very sensitive to all layers of the schema.
And it's violated by SQL's timestamp type itself, which is a complex domain composed of year,
month, day, hour, minute, second and nanosecond. If we followed Codd blindly, we'd use 7
fields every time we needed a timestamp.

### Remember 4-valued logic?

Simple domains weren't the only thing that Codd strongly advocated for.
He was also in favor of the 4-valued boolean logic, where the result of any expression has
"yes", "no", "NULL and doesn't matter", and "NULL and it does matter". He had a lot
of trouble convincing people to implement this 4-valued logic, which was necessary
for logical consistency in presence of NULLable columns. Codd was right. If you have NULLs
and don't use 4-valued logic, you have consistency issues. But 50 years later,
we have some hindsight: why not just throw away NULLs?
When you get rid of NULLs, you are naturally pushed toward columnar storage, 
since you still need to support missing values at various stages of your data set lifetime, 
but your missing values simply become missing rows.

A NULL is nothing more than a missing join!

### Structured Key Normal Form

If it weren't for the simple domain rule and NULLs, the proliferation of normal forms
wouldn't exist. 

And so SKNF represents a very simple but stable point in the space of
all possible schemas of schemas, where you don't have NULLs and every key is
just a single composite value. It scales indefinitely, and every join takes just 2 values.
It's guessable and easy to remember.

SKNF is not a name recognized in the industry. Perhaps in the clade of DBMS construction philosophies,
the closest analog would be DKNF (Domain Key Normal Form).
