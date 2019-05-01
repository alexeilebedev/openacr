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

In a given dataset,
The list of all ssim files is provided by "acr ssimfile".
The list of all attrbitutes is provided by "acr field"

Ssim tuples is typically held in data sets, (with one directory per
namespace, and one file per table) or in a single file.  One can use
grep, sed, awk, and other line-oriented tools to access these records.

All amc-supported commands support the -in argument which specifies the input
dataset for the tool -- either a file or a directory. By default it's "data"

### Structured Key Normal Form

There is a lot of literature on how to construct database schema so that it doesn't
have anomalies, and how to create primary keys; There are 6 or 7 'normal forms' -- invariants
that have to hold if you want certain anomalies to be absent from your schema.

Here I will describe the Structured Key Normal Form, or SKNF, which all ssim
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

If we were to rename this element to dmm.Ctype, we'd get an acr -check
error, since dmm is not a valid namespace.

dmmeta.Ctype.ns is an attribute of dmmeta.Ctype, and it's defined as a function of other fields,
or 'fldfunc' for short. The acr option -fldfunc expands all fldfuncs when showing output.
The 'ns' field in the query above is defined like this:

      dmmeta.field  field:dmmeta.Ctype.ns  arg:dmmeta.Ns  reftype:Pkey  dflt:""  comment:"translates to c++ namespace"
        dmmeta.substr  field:dmmeta.Ctype.ns  expr:.RL  srcfield:dmmeta.Ctype.ctype

The substr expression consists of triples of characters, where the first character is
what to look for in the string (in this case '.'), the second character is where to start (in this case
start at the right side of the string, 'R'), and the third character specifies what part of the
string to take after the match is found, or the search stops. If the match is not found, search stops
at the opposite end of the string. So, the expression .RL means "Scan from the right until you find the
character or hit the left side of the string. Then, return everything to the right of the current position".
These triples can be repeated as many times as necessary to "bite off" pieces of the string, for example
.RL.RL removes the 2 trailing dots from a string. The corresponding C++ function that implements this
search is called algo::Pathcomp.

There are no constraints other than foreign key constraints in ssim databases.
(acr -check can detect some errors such as strings that are too long for the underlying specified type,
but that's not a constraint per se)

### History of SKNF

In his seminal paper (https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf)
Codd explained that attaching an access path to a data set
results in anomalies which lie at the root of most data modeling problems,
and so must be avoided. Here for the first time, way after languages
such as Algol were designed and published, we have the most important
simplication -- the factoring of a state space into a set of sets, and a set
of access paths between them. Data structure is replaced with access structure
and a set.

Whenever we talk of "linked list of X" or "hash table of X", we are introducing
invisible anomalies by presupposing that all X are reachable via said access path.
To avoid this anomaly, we analyze X as a set by itself, and figure out what
information must be included in the element of each set in order to distinguish
two elements from each other. Only after we have done that, do we begin to attach
indexing structures, or access paths, so that we can retrieve elements of X
from the data store (in-memory or on-disk) faster.

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

So, surrogate keys don't solve the problem of constructing the set
For that, you have to describe the cardinality of
the set, either being an integer, or a subset of another set, or a cross product of two sets.

Codd was much of in favor of simple domains, where each column (field of a struct)
is either an integer or a string, and the primary key is described as a concatenation
of several such fields. This principle is not scalable, because if you use simple domains,
you get very complicated joins which are very sensitive to all layers of the schema.
And it's violated by SQL's timestamp type itself, which is a complex domain composed of year,
month, day, hour, minute, second and nanosecond. If we followed Codd blindly, we'd use 7
fields every time we needed a timestamp.

Codd was also in favor of the 4-valued boolean logic, where you have values
"yes", "no", "NULL but doesn't matter", and "NULL and it does matter". He had a lot
of trouble convincing people to implement this 4-valued logic, which was necessary
for consistency. Most database implementors ignored the 4-valued logic, sayng
"NULL is NULL, OK?", making Codd very upset. But most database
users heeded the "simple domain" rule. It's possible that if it weren't for
this unfortuante decision, SQL would be a lot more popular today.

If it weren't for the simple domain rule and NULLs, the proliferation of normal forms
wouldn't exist. And so SKNF represents a very simple but stable point in the space of
all possible schema schemas (meta-schemas), where you don't have NULLs and every key is
just a single composite value. It scales indefinitely, and every join takes just 2 values.

SKNF is not a name recognized in the industry. In the clade of DBMS construction philosophies,
the closest analog would be DKNF (Domain Key Normal Form).

