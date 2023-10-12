## acr: Auto Cross Reference

Acr is a query and editing tool for ssim (super-simple) databases.
These databases can be proper directories, or files, or stdin.
Acr is not concerned with big data; it's about small but highly cross-referenced data,
essentially configurations and source code. A rule of thumb is that acr will
process 1 million records/sec and the data sets must fit in memory.

### Querying

The default data directory "data", can be specified with -in option.
Assuming you've successfully built acr, you can type

    $ acr %
    amcdb.curs  curs:curs       comment:""
    amcdb.curs  curs:unordcurs  comment:""
    < about 10,000 more records are printed >

This will dump all records in the data directory, about 10,000 of them.
Of course, once you start growing a real project around this initial deposit,
3,000 fields will seem like nothing.
A large project may easily get 10,000 ctypes, 1,000 ssimfiles and 30,000 fields,
and a few million records.

The query parameter for acr is a SQL regular expression.
SQL regexes are very convenient to use on the command line;
the whilecard character is `%` instead of `*`, and thus quoting is mostly unnecessary.
ACR also supports characters '|', '(', ')' in regex, and that's it.

You could achieve a similar result to `acr %` by running

    $ find data -name "*.ssim" | xargs cat
    ...

### Creating A New Table

To show the rest of the commands, let's start with a couple of fresh tables
and proceed interactively. We will choose the `dev` namespace; this choice is quite arbitrary.

```
inline-command: acr_ed -create -ssimfile dev.a -write
report.acr_check  records:13  errors:0
report.acr  ***
report.acr  ***
report.amc  ***
```

This will create a new empty table data/dev/a.ssim. The ctype for `a` has a single string field
named 'a'. The fact that this ssimfile was created was recorded in the dmmeta database.
The dmmeta database is how acr knows anything.

```
inline-command: acr ssimfile:dev.a
dmmeta.ssimfile  ssimfile:dev.a  ctype:dev.A
report.acr  ***
```

### Inserting Data

We can now populate this table with some data. `acr -insert -write` reads values from standard
input, inserts them into the database, and saves everything to disk when done. Duplicate keys
will cause errors. If we need to ignore duplicates, we use `acr -replace -write`.

```
inline-command: for X in {0..10}; do echo dev.a a:a$X; done | acr -insert -write
acr.insert  dev.a  a:a0   comment:""
acr.insert  dev.a  a:a1   comment:""
acr.insert  dev.a  a:a2   comment:""
acr.insert  dev.a  a:a3   comment:""
acr.insert  dev.a  a:a4   comment:""
acr.insert  dev.a  a:a5   comment:""
acr.insert  dev.a  a:a6   comment:""
acr.insert  dev.a  a:a7   comment:""
acr.insert  dev.a  a:a8   comment:""
acr.insert  dev.a  a:a9   comment:""
acr.insert  dev.a  a:a10  comment:""
report.acr  ***
```

Now let's query this table. Here, we use a SQL regex and ask for values a2, a3 and a4.

```
inline-command: acr a:a'(2|3|4)'
dev.a  a:a2  comment:""
dev.a  a:a3  comment:""
dev.a  a:a4  comment:""
report.acr  ***
```

If we wanted to use these values in a shell script, there are a few flags that help with that.
First, the `-field` option:

```
inline-command: acr a:a'(2|3|4)' -field:a
a2
a3
a4
```

Alternatively, we could ask for a regex of the values. This can be used to save the regex
and use it in subsequent expressions

```
inline-command: acr a:a'(2|3|4)' -regxof:a
(a2|a3|a4)
```

#### The -replace option

When `-replace` is specified, each new tuple completely overrides the previous tuple. 
Any fields that aren't specified are assigned default values. Let's illustrate by first
creating a temporary table:

```
inline-command: acr_ed -create -ssimfile dev.t -write
report.acr_check  records:13  errors:0
report.acr  ***
report.acr  ***
report.amc  ***
```

```
inline-command: acr_ed -create -field dev.T.val -arg u32 -write
acr_ed.create_field  field:dev.T.val
report.acr_check  records:5  errors:0
report.acr  ***
report.amc  ***
```

```
inline-command: echo 'dev.t t:ggg val:3' | acr -insert -write
acr.insert  dev.t  t:ggg  val:3  comment:""
report.acr  ***
```

Now let's try to insert another record with the value `ggg`:

```
inline-command: echo 'dev.t t:ggg' | acr -insert -write
acr.duplicate_key  key:dev.T:ggg
report.acr  ***
```

That failed. Now let's replace instead. This will succeed, and the value `val`
will go back to the default:

```
inline-command: echo 'dev.t t:ggg' | acr -replace -write
report.acr  ***
```

#### The -merge option

The `-merge` option is like replace on a per-field basis. When a record being inserted
exists in the dataset, any attributes from the new record replace attributes in the original records.
Let's illustrate using the same `t` table. We'll need another column, call it `val2`, and
set `val` back to 3:

```
inline-command: acr_ed -create -field dev.T.val2 -arg u32 -write
acr_ed.create_field  field:dev.T.val2
report.acr_check  records:5  errors:0
report.acr  ***
report.amc  ***
```

```
inline-command: echo 'dev.t t:ggg val:3' | acr -replace -write
report.acr  ***
```

So far so good. Now let's use -merge:

```
inline-command: echo 'dev.t t:ggg val2:4' | acr -merge -write
report.acr  ***
```

As as we can see, `val` has retained the value of 3, while `val2` was set to 4.

#### The -trunc option

With this option, when the first change is made to the table, the table is first wiped.

```
inline-command: echo 'dev.t t:hhh' | acr -insert -trunc -write
acr.insert  dev.t  t:hhh  val:0   val2:0   comment:""
report.acr  ***
```

### Generating Shell Scripts

Finally, the `-cmd` option produces an executable shell script which can be piped to `sh`. 
For each record in the final selection, acr outputs variable assignment statements, giving the
shell script access to the values of all field attributes, the fldfuncs, the tuple itsef (`acr_tuple`)
the type tag (`acr_head`) and the rowid (`acr_rowid`). The script can then use whatever 
other Unix tools it needs to.

```
inline-command: acr a:a1 -cmd 'echo ==== $a ===='
acr_tuple=$'dev.a  a:a1  comment:""'
acr_head=dev.a
acr_rowid=1
a=a1
comment=''
echo ==== $a ====

# report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
```

Piping through bash produces the desired result:

```
inline-command: acr a:a1 -cmd 'echo ==== $a ====' | bash
==== a1 ====
```

The beauty of `-cmd` is that it just outputs a script, which can be consumed with a single process.
one command per output row would have been a lot slower

### Deleting Records From Stdin

The options `-insert`, `-replace`, `-merge` all enable reading of stdin
for a list of tuples. This input stream can contain special lines.

Whenever a line starts with `acr.delete`, the corresponding record is deleted.

Whenever a line starts with `acr.insert`, the corresponding record is inserted (subject to replace/merge semantics).

### Inserting a Column

We now might want to add a column to the `a` table.

```
inline-command: acr_ed -create -field dev.A.b -arg u32 -write
acr_ed.create_field  field:dev.A.b
report.acr_check  records:5  errors:0
report.acr  ***
report.amc  ***
```

Let's update a few values with `acr -merge`:

```
inline-command: echo 'dev.a a:a1 b:55' | acr -merge -write
report.acr  ***
```

Let's check if the b column is there:

```
inline-command: acr a
dev.a  a:a0   b:0   comment:""
dev.a  a:a1   b:55  comment:""
dev.a  a:a10  b:0   comment:""
dev.a  a:a2   b:0   comment:""
dev.a  a:a3   b:0   comment:""
dev.a  a:a4   b:0   comment:""
dev.a  a:a5   b:0   comment:""
dev.a  a:a6   b:0   comment:""
dev.a  a:a7   b:0   comment:""
dev.a  a:a8   b:0   comment:""
dev.a  a:a9   b:0   comment:""
report.acr  ***
```

Yes, it is. What if we just look in the file itself?

```
inline-command: head -3 data/dev/a.ssim
dev.a  a:a0  b:0  comment:""
dev.a  a:a1  b:55  comment:""
dev.a  a:a10  b:0  comment:""
```

We see largely the same result, except the columns are not aligned. By default,
acr flag `-pretty` is set to true, which aligns columns in groups of 25 records at a time.

#### The -before option

When creating a new field with `acr_ed`, it can be inserted before another field
by specifying `-before <field>`. Alternatively, you can always call up `acr -t -e ctype:<ctype>`
and make the adjustment by hand, or just go into the `data/dmmeta/field.ssim` table
and swap two lines in a text editor. These are all equally valid approaches.

### Creating a Subset Table

Now let's create another table which will be a subset of the first, and populate it.
When we specify the -subset parameter, we must include the ctype (not ssimfile).

```
inline-command: acr_ed -create -ssimfile:dev.b -subset dev.A -write
report.acr_check  records:14  errors:0
report.acr  ***
report.acr  ***
report.amc  ***
```

Let's quickly check how `B`'s fields were defined:

```
inline-command: acr field:dev.B.%
dmmeta.field  field:dev.B.a        arg:dev.A         reftype:Pkey  dflt:""  comment:""
dmmeta.field  field:dev.B.comment  arg:algo.Comment  reftype:Val   dflt:""  comment:""
report.acr  ***
```

We see that the field dev.B.a was created, with `arg:dev.A` and `reftype:Pkey`.
`reftype:Pkey` means that the type of the field in question is the same as the type
of the first field of the arg. In this case, `algo.Smallstr50`. It also means, as far as acr
is concerned, that the value of this field must correspond to some existing value in `a`.
Notice that the new table's primary key has the name `a`, not `b`. This
is because of the convention that a foreign key reference uses the name of the target table.
We'll use values a3 through a7.

```
inline-command: for X in {3..7}; do echo dev.b a:a$X; done | acr -insert -write
acr.insert  dev.b  a:a3  comment:""
acr.insert  dev.b  a:a4  comment:""
acr.insert  dev.b  a:a5  comment:""
acr.insert  dev.b  a:a6  comment:""
acr.insert  dev.b  a:a7  comment:""
report.acr  ***
```

### Following References Up

Now let's see if acr recognizes that these tables are related. We'll use the `-nup 1`
argument to ask acr the following: after locating all records that match the initial query,
follow any of the pkey 1 level up, extending the selection. At the end, print the result.

```
inline-command: acr b:a3 -nup 1
dev.a  a:a3  b:0  comment:""

dev.b  a:a3  comment:""
report.acr  ***
```

Yes! It worked. What if we try the same thing when querying a?

```
inline-command: acr a:a3 -nup 1
dev.a  a:a3  b:0  comment:""
report.acr  ***
```

Since `a` doesn't refer to anything, there is nothing to discover.

### Following References Down

But we *can* discover the b record by following from a:

```
inline-command: acr a:a4 -ndown  1
dev.a  a:a4  b:0  comment:""

dev.b  a:a4  comment:""
report.acr  ***
```

Notice that in the output, `a` is always printed out before `b`. That's because
acr sorts the output so that when it's scanned linearly, referential integrity is
maintained; the output is topologically sorted by references.
Acr has a useful shortcut `-xref`, which stands for `-nup 100 -ndown 100`.
And we'll use the `-tree` option, which groups any dependent records underneath the
nearest parent, using indentation. For ssimfiles, indentation is irrelevant when parsing.
Leading whitespace is ignored. So the leading whitespace is purely
for human comprehension:

```
inline-command: acr b -xref -tree
dev.a  a:a3  b:0  comment:""
  dev.b  a:a3  comment:""

dev.a  a:a4  b:0  comment:""
  dev.b  a:a4  comment:""

dev.a  a:a5  b:0  comment:""
  dev.b  a:a5  comment:""

dev.a  a:a6  b:0  comment:""
  dev.b  a:a6  comment:""

dev.a  a:a7  b:0  comment:""
  dev.b  a:a7  comment:""
report.acr  ***
```

Since `-xref -tree` is very frequently used, command line option `-t` is an alias for it.

```
inline-command: acr b:a5 -t
dev.a  a:a5  b:0  comment:""
  dev.b  a:a5  comment:""
report.acr  ***
```

### Deleting Records

What about deleting records? Let's delete something.

```
inline-command: acr a:a4 -del -write
acr.delete  dev.a  a:a4  b:0  comment:""

acr.delete  dev.b  a:a4  comment:""
report.acr  ***
```

As you can see, acr followed the reference and deleted the dependent `b` record as well as the
matching `a` record.

What if we wanted to see what `-del` does without modifying the database?
We could omit the `-write` option. Notice that acr prints the records that *would*
be deleted, but in the final report, `n_file_mod:0` so we know that nothing was written back.

```
inline-command: acr a:a5 -del
acr.delete  dev.a  a:a5  b:0  comment:""

acr.delete  dev.b  a:a5  comment:""
report.acr  ***
```

`-del` works with any number of records. You could delete the entire database with
`acr % -del -write`.

When deleting a field, acr automatically opens and rewrites the corresponding ssimfile
so that the deleted column disappears.

### Manual Editing

The option -e launches an editor (as specified with the environment
variable EDITOR), allowing you to edit the selected set. When the editor
exits, whatever tuples were in the original selection are deleted from
the ssim files and replaced with the edited output. To abort editing,
kill the editor and remove temporary file `temp/acr.ssim`.

### Checking Referential Integrity

Let's insert a record into the `b` table which has no corresponding record in the `a` table.

```
inline-command: echo 'dev.b a:xyz' | acr -insert -write
acr.insert  dev.b  a:xyz  comment:""
report.acr  ***
```

Now we have a record referring to a non-existing `xyz`. This is clearly a foreign
key constraint violation, but because we've been running acr with `-check:N` (the default),
the violation was allowed. The reason `acr -check` is off by default is that it's
virtually impossible to interactively build a nice data set without it being `broken` at
one point or another. It's like if your editor disallowed syntactically incorrect programs --
you wouln't be able to even type up "hello world". Usually, it's applications that load
and cross-reference records are the ones that care about constraint violation.

```
inline-command: acr -check b
data/dev/b.ssim:5: Invalid value a:xyz
dev.b  a:xyz  comment:""

Type               dev.A
Valid values       a0, a1, a10, a2, a3, a5, a6, a7, a8, a9

acr.badrefs  ctype:dev.B  nbad:1

report.acr_check  records:9  errors:3
report.acr  ***
```

We wee that acr has detected the error and given us some suggestions on how to fix it.
With `-check`, the final selection consists only of records with errors in them. This means
we can run acr with `-check -e` to edit these records (and delete the record in question)
But we could also delete the offending record, rename it, or insert a missing `a` record.

Notice that `-check -del` doesn't work as you would expect (this might be fixed later).

### Renaming

Speaking of renaming, we can rename whole clusters of records with one command:

```
inline-command: acr a:a3 -rename a99 -write
acr.update  dev.a  a:a99  b:0  comment:""

acr.update  dev.b  a:a99  comment:""
report.acr  ***
```

We can check that the dependencies were followed during the rename.

```
inline-command: acr a:a99 -t
dev.a  a:a99  b:0  comment:""
  dev.b  a:a99  comment:""
report.acr  ***
```

### Cross-Product Types

Now it's time to do something more interesting. We'll create a table `c`, which will be
an independent table; we will populate it with a few colors. Then,
we'll let's create a table `d`, whose key is a cross product of `b` and `c`.

```
inline-command: acr_ed -create -ssimfile:dev.c -write
report.acr_check  records:13  errors:0
report.acr  ***
report.acr  ***
report.amc  ***
```

```
inline-command: for X in red green blue; do echo dev.c c:$X; done | acr -insert -write
acr.insert  dev.c  c:red    comment:""
acr.insert  dev.c  c:green  comment:""
acr.insert  dev.c  c:blue   comment:""
report.acr  ***
```


```
inline-command: acr_ed -create -ssimfile dev.d -subset dev.B -subset2 dev.C -separator . -write
report.acr_check  records:20  errors:0
report.acr  ***
report.acr  ***
report.amc  ***
```

```
inline-command: for B in a5 a6 a7; do for C in red green blue; do echo dev.d d:$B.$C; done; done | acr -insert -write
acr.insert  dev.d  d:a5.red    comment:""
acr.insert  dev.d  d:a5.green  comment:""
acr.insert  dev.d  d:a5.blue   comment:""
acr.insert  dev.d  d:a6.red    comment:""
acr.insert  dev.d  d:a6.green  comment:""
acr.insert  dev.d  d:a6.blue   comment:""
acr.insert  dev.d  d:a7.red    comment:""
acr.insert  dev.d  d:a7.green  comment:""
acr.insert  dev.d  d:a7.blue   comment:""
report.acr  ***
```

Now the most interesting part is, how does acr see the resulting structure?
Below we see that acr has followed the schema and discovered all the necessary dependencies,
grouping `d` under `b`, which groups under `a`. All the referenced colors from `c` were
included in the output, but didn't become part of the tree, because only the leftmost
parent is the preferred one.

```
inline-command: acr d -t
dev.c  c:blue   comment:""
dev.c  c:green  comment:""
dev.c  c:red    comment:""
dev.a  a:a5  b:0  comment:""
  dev.b  a:a5  comment:""
    dev.d  d:a5.blue   comment:""
    dev.d  d:a5.green  comment:""
    dev.d  d:a5.red    comment:""

dev.a  a:a6  b:0  comment:""
  dev.b  a:a6  comment:""
    dev.d  d:a6.blue   comment:""
    dev.d  d:a6.green  comment:""
    dev.d  d:a6.red    comment:""

dev.a  a:a7  b:0  comment:""
  dev.b  a:a7  comment:""
    dev.d  d:a7.blue   comment:""
    dev.d  d:a7.green  comment:""
    dev.d  d:a7.red    comment:""
report.acr  ***
```

Let's look at the definition of `dev.D` type. The reason I have been holding off
on showing the definitions of the types `dev.A`, `dev.B`, and `dev.C` is that 
this tutorial is about creating tables and records in these tables,
and type definitions are exactly that -- they're just records inserted by `acr_ed`
into some appropriate tables using `acr -insert -write`.
So it would have been circular to start with them, and I had to hide them.
Seemingly circular definitions occur all the time in acr.

```
inline-command: acr ctype:dev.D -t
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:ssimdb  comment:"Ssim database (not a target)"
  dmmeta.ns  ns:dev  nstype:ssimdb  license:GPL  comment:"Tables supporting compilation of programs"
    dmmeta.ctype  ctype:dev.D  comment:""
      dmmeta.field  field:dev.D.d  arg:algo.Smallstr50  reftype:Val   dflt:""  comment:""
      dmmeta.field  field:dev.D.b  arg:dev.B            reftype:Pkey  dflt:""  comment:""
        dmmeta.substr  field:dev.D.b  expr:.RL  srcfield:dev.D.d

      dmmeta.field  field:dev.D.c  arg:dev.C  reftype:Pkey  dflt:""  comment:""
        dmmeta.substr  field:dev.D.c  expr:.RR  srcfield:dev.D.d

      dmmeta.field  field:dev.D.comment  arg:algo.Comment  reftype:Val  dflt:""  comment:""
      dmmeta.cfmt  cfmt:dev.D.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:dev.D  len:204  alignment:1  padbytes:0

dmmeta.ssimfile  ssimfile:dev.d  ctype:dev.D
  dmmeta.ssimsort  ssimfile:dev.d  sortfld:dev.D.d  comment:""
report.acr  ***
```

### Fldfunc fields

One thing we see in the print-out above, is that the fields `dev.D.b` and `deb.D.c` are not data
fields. They are defined as substrings of `dev.D.d` (the primary key), using expressions `.RL`
and `.RR`. Such computed fields are called fldfuncs. Acr treats fldfunc fields in the same way
as all other fields, but doesn't display them by default unless `-fldfunc` has been specified.
Let's ask acr to show us the fldfunc values:

```
inline-command: acr d -fldfunc
dev.d  d:a5.blue   b:a5  c:blue   comment:""
dev.d  d:a5.green  b:a5  c:green  comment:""
dev.d  d:a5.red    b:a5  c:red    comment:""
dev.d  d:a6.blue   b:a6  c:blue   comment:""
dev.d  d:a6.green  b:a6  c:green  comment:""
dev.d  d:a6.red    b:a6  c:red    comment:""
dev.d  d:a7.blue   b:a7  c:blue   comment:""
dev.d  d:a7.green  b:a7  c:green  comment:""
dev.d  d:a7.red    b:a7  c:red    comment:""
report.acr  ***
```

We can also use `-regx` to quickly confirm which values are present:

```
inline-command: acr d -regxof:c
(blue|green|red)
```

There are three other types of fldfuncs in addition to substr: bitfields, externally-defined values (cppfunc)
and aliases. These will be described later.

The expression such as '.RL' is not limited to 3 characters. 

This substr expression consists of triplets of characters, where the first character is
what to look for in the string (in this case '.'), the second character is where to start (in this case
start at the right side of the string, 'R'), and the third character specifies what part of the
string to take after the match is found, or the search stops. If the match is not found, search stops
at the opposite end of the string. So, the expression .RL means "Scan from the right until you find the
character or hit the left side of the string. Then, return everything to the right of the current position".
These triples can be repeated as many times as necessary to "bite off" pieces of the string, for example
.RL.RL removes the 2 trailing dots from a string. The corresponding C++ function that implements this
search is called algo::Pathcomp.

The funciton that parses these expressions is called Pathcomp, and we can view its source code:

```
inline-command: src_func algo_lib Pathcomp
// S         source string
// EXPR      string in the form (XYZ)*
// - X is the character to search
// - Y is L to search from left, R to search from right
// - Z is which site to pick: L=left, R=right
// If the character to search is not found, it assumed to be found
// at the far end of the string with respect to search direction.
// Function processes characters from expr in groups of 3.
// Example:
// s = "a.b.c.d"
// expr = ".LR.LL"
// first, search for . from left; pick right hand side (b.c.d)
// then, search for . from left. pick left hand side (b)
// result: b
// More examples:
// s = "abcd"; expr = ".LL"; result = "abcd"
// s = "abcd"; expr = ".LR"; result = ""
cpp/lib/algo/string.cpp:44: 
strptr algo::Pathcomp(strptr s, strptr expr) {
    int start  = 0;
    int end    = s.n_elems;
    for (int i=0; i <= elems_N(expr)-3; i+=3) {
        char c     = expr[i];
        int idx;
        bool match = false;
        switch ((expr[i+1]=='R')*2 + (expr[i+2]=='R')) {
        case 0: // LL
            for (idx=start; idx<end && !match; idx++) { match=s[idx]==c; }
            end = idx - match;
            break;
        case 1: // LR
            for (idx=start; idx<end && !match; idx++) { match=s[idx]==c; }
            start = idx;
            break;
        case 2: // RL
            for (idx=end; idx>start && !match; idx--) { match=s[idx-1]==c; }
            end = idx;
            break;
        case 3: // RR
            for (idx=end; idx>start && !match; idx--) { match=s[idx-1]==c; }
            start = idx + match;
            break;
        }
    }
    return strptr(s.elems + start, end-start);
}

```

### Querying On Non-Primary Key

It is possible to ask acr to select by a non-primary column. For that, we have to include the full
field name:

```
inline-command: acr dev.D.c:blue
report.acr  ***
```

Alternatively, we can use `-where`:

```
inline-command: acr d -where c:blue
dev.d  d:a5.blue  comment:""
dev.d  d:a6.blue  comment:""
dev.d  d:a7.blue  comment:""
report.acr  ***
```

The query string you pass to acr is a regular expression that extends to the name of the ssimfile as well
So, if we wanted to search both `b` and `c` tables, we'd write:

```
inline-command: acr '(c|b)'
dev.c  c:blue   comment:""
dev.c  c:green  comment:""
dev.c  c:red    comment:""

dev.b  a:a99  comment:""
dev.b  a:a5   comment:""
dev.b  a:a6   comment:""
dev.b  a:a7   comment:""
dev.b  a:xyz  comment:""
report.acr  ***
```

In fact, we can search for any value in the entire dataset with `acr %:%<value>%`

### Sorting & RowIDs

Acr always saves files in sorted order. Sorting is controlled by the `ssimsort`
table, which is a subset of `ssimfile`. Sorting is optional. If `ssimsort` is missing,
the set is *order-dependent*. When sorting is enabled, it can be done on any fields,
including a fldfunc.

Let's take another look at our color table `c`:

```
inline-command: acr c
dev.c  c:blue   comment:""
dev.c  c:green  comment:""
dev.c  c:red    comment:""
report.acr  ***
```

The colors are sorted by name. Let's look at the ctype:

```
inline-command: acr ctype:dev.C -t
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:ssimdb  comment:"Ssim database (not a target)"
  dmmeta.ns  ns:dev  nstype:ssimdb  license:GPL  comment:"Tables supporting compilation of programs"
    dmmeta.ctype  ctype:dev.C  comment:""
      dmmeta.field  field:dev.C.c        arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""
      dmmeta.field  field:dev.C.comment  arg:algo.Comment     reftype:Val  dflt:""  comment:""
      dmmeta.cfmt  cfmt:dev.C.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:dev.C  len:204  alignment:1  padbytes:0

dmmeta.field  field:dev.D.c  arg:dev.C  reftype:Pkey  dflt:""  comment:""
  dmmeta.substr  field:dev.D.c  expr:.RR  srcfield:dev.D.d

dmmeta.ssimfile  ssimfile:dev.c  ctype:dev.C
  dmmeta.ssimsort  ssimfile:dev.c  sortfld:dev.C.c  comment:""
report.acr  ***
```

Now let's delete the ssimsort line.

```
inline-command: acr ssimsort:dev.c -del -write
acr.delete  dmmeta.ssimsort  ssimfile:dev.c  sortfld:dev.C.c  comment:""
report.acr  ***
```

If we now use `acr -e` to open `c` in an editor, we will see that each line now has an additional
attribute called `acr.rowid`. This is attribute ensures that the line will be saved back to the 
file in the same position. We can't invoke an interactive editor within a readme file,
so we will manually use the `-rowid` option

```
inline-command: acr c -rowid
dev.c  c:blue   acr.rowid:0       comment:""
dev.c  c:green  acr.rowid:1       comment:""
dev.c  c:red    acr.rowid:2       comment:""
report.acr  ***
```

If we swap the lines for `blue` and `green`, and exit from the editor, no change will 
occur in the table. That's because the order of the records is not determined by their visual order
on the screen. It's determined by the rowid. To swap two records, we would need to swap their rowids.
The rowids are temporary, and exist only for the duration of the editing session. They are not 
saved to the ssimfile.

The type of this rowid is conveniently a `float`, which allows you to insert new lines at fractional positions,
something like this:

    dev.c  c:cyan  acr.rowid:0.5 ...
    ...
    
The very important `dmmeta.field` table is partially positional. It is sorted on the `ctype`
attribute, but not sorted by name. That's because the user needs to control the order of fields
in a structure. Most other ssimfiles are sorted on the primary key.

### Creating A New Data Set

So far, we have been using the default data set called `data`. Acr is not limited to it.
We can work with tuples in any data set, and that data set can be either a directory or a file.

To create a new directory-based data set, we just create an empty directory and then use the `-in`
option:

```
inline-command: mkdir x; acr dmmeta.% | acr -in:x -insert -write -print:N
report.acr  ***
```

File `x` now contains ssim tuples, one per line, and it can be queried or edited
just like any other data set. Just by dumping some tuples in a file, we get a data set.

### Using stdin as the source

By specifying `-in:-`, you can have acr read the input from stdin. This can be useful
with bash pipelines. 

```
inline-command: echo 'dmmeta.ctype ctype:xyz' | acr -in:- ctype:%
dmmeta.ctype  ctype:xyz  comment:""
report.acr  ***
```

This shows a couple of interesting things. First, acr always displays tuples 
according to the known schema. When printing a tuple, `acr` loops over the 
fields of the associated `ctype`, and prints the fields in the order specified by the schema.
This means that the input tuple can provide attributes in any order, or even omit them, and acr will
fill in the defaults. You see this because acr added the `comment` field which is present in the schema
but not in the input. acr normalizes the data whenever the data is printed.

### The -schema option

The location from which acr loads the `ctype` and `field` tables (and a few others) can be specified
with `-schema`. By default, these tables are loaded from `"data"` directory.

If we pass an empty data set as schema, we expect nothing to be printed:

```
inline-command: rm -r x; touch x; echo 'dmmeta.ctype ctype:xyz' | acr -in:- ctype:% -schema:x
report.acr  ***
```

Sure enough, acr doesn't recognize the input as a known tuple, so the line is silently
skipped.

### The -meta option

Instead of writing `acr ctype:dev.C -t` we can write

```
inline-command: acr c -meta
dmmeta.ctype  ctype:dev.C  comment:""
  dmmeta.field  field:dev.C.c        arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""
  dmmeta.field  field:dev.C.comment  arg:algo.Comment     reftype:Val  dflt:""  comment:""
  dmmeta.cfmt  cfmt:dev.C.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
  dmmeta.ctypelen  ctype:dev.C  len:204  alignment:1  padbytes:0

dmmeta.field  field:dev.D.c  arg:dev.C  reftype:Pkey  dflt:""  comment:""
  dmmeta.substr  field:dev.D.c  expr:.RR  srcfield:dev.D.d

dmmeta.ssimfile  ssimfile:dev.c  ctype:dev.C
report.acr  ***
```

The `-meta` option looks at the selected records, and de-selects them, and selects their
ctypes instead. `-meta` implies `-t`.

### The -cmt option

The -cmt option displays any comments associated with the current selection, which includes comments attached to the displayed
fields. Let's add a comment to the dev.C.c field:

```
inline-command: echo 'dmmeta.field field:dev.C.c comment:"Name of the color (primary key)"' | acr -merge -write
report.acr  ***
```

Now when we display the `c` table with the `-cmt` option, this comment shows up. `-cmt` is the default whenever
`-e` is used -- it helps the user understand what they're looking at.

```
inline-command: acr c -cmt
# Ctype  Comment

# e.g. dev.c  c:""  comment:""

dev.c  c:blue   comment:""
dev.c  c:green  comment:""
dev.c  c:red    comment:""

# Field    Arg              Reftype  Comment
# dev.C.c  algo.Smallstr50  Val      Name of the color (primary key)
report.acr  ***
```

### MySQL Integration

The option -my launches an instance of mariadb and imports the selection as databases into mariadb.
It then drops you into a mariadb shell, where you can apply needed transformations.
Upon exit, the data is downloaded from the database back to disk. -my implies -write.
The option `-my` implies `-fldfunc`, so that all the computed fields become available inside MariaDB
as regular columns (but they won't be saved back).

All ssim tables are compatible with MariaDB. The namespace (ns) corresponds to a database in MariaDB,
and ssimfile corresponds to a table.

Here is an example of running a SQL query against an acr dataset from bash:

```
inline-command: echo "select * from dev.c where c ='red'" | acr -my dev.%
waiting for server init...
c	comment
red	
```

### MySQL Integration: tools

Tools mysql2ssim and ssim2mysql perform conversion to/from ssim schema to SQL, and
acr_my can set up a local MariaDB server process and iteract with
it via a socket.

Here are some useful commands:
Start a local MariaDB server:

    $ acr_my -start dmmeta

Connect to the local server on the command line:

    $ acr_my -shell

From here, you can issue SQL commands:

    > select count(*) from dmmeta.field where arg ='u8';

When you exit from the shell, the server keeps running.
You can stop the server and save changes:

    $ acr_my -stop

Or stop the server, discarding changes:

    $ acr_my -abort

The table `dmmeta.sqltype` allows ssim2mysql to map ctypes to SQL types so that
round tripping can occur without loss. We check in CI that all acr tables 
can be round tripped through MariaDB with no change.
