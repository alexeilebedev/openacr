## algo_lib: Rules
<a href="#algo_lib-rules"></a>

`algo_lib` is the support library every executable links, and most of it needs no
document: a function that formats a number or opens a file says what it does
where it is written.  Two of its pieces do not.  `Replscope` substitutes `$`
variables into text and `Regx` matches a subject against a pattern, and both hold
their answer in a data structure chosen for a workload the source never mentions.
This file carries those decisions, and the invariants each structure exists to
hold.

### Invariants
<a href="#invariants"></a>

A Replscope's variables live in a path-compressed trie whose root is node 0 and
stands for the leading dollar sign.  Every node but the root either names a
variable or has a descendant that does.  Every value in a scope lives
concatenated in one string, and the ranges naming them never overlap.

A Regx knows whether its expression reduces to a fixed string, and a reduced
expression is matched by searching for that string rather than by simulating its
states.  The states a matcher walks carry no group-boundary states unless the
caller asked for the groups.  Matching writes nothing into the compiled Regx.

A bitset holds no bit past the words it has, and every operation that combines
two of them respects that.

A time format that names a field matches only by reading one, where a field is
a digit or a month or weekday name.  Every numeric field reads as zero when no
digit stands under it, so without this rule the format `%Y%m%d` matches any
string and `UnTime_ReadStrptrMaybe` answers `null` with a date in 1753.
`UnTime`, `UnDiff` and `UnixTime` all read through `TimeStruct_Read`, so a
string carrying no time is refused by every one of them and the output is left
as it was.

A time string that ends in an ISO 8601 zone designator, a `Z` or a numeric
offset such as `-04:00`, reads to the instant its fields name in that zone, so
every spelling of one instant reads to one `UnTime` whatever the host's TZ.  A
time string with no zone reads as local time.  `UnTime_ReadStrptrMaybe` reads
the zone after the date and time fields, which is where ISO 8601 puts it, and
`UnixTime` reads through it, so a gitlab timestamp dates a job by the instant it
names.

An empty string is the unset value of every scalar an ssim attr holds, and it
reads as the type's zero.  `acr` writes a time field nobody set as `begin:""`,
and the generated loaders hand that to the time readers, so an empty string
reads as the zero time the way it reads as `0` for an integer.  A caller asking
whether a string carries a time at all, such as `lib_gli::UnixTime`, tests the
length before parsing.

A `retry_curs` body runs with the `verbose` log category off, so a command that
the body runs does not echo its command line on every attempt.  The cursor that
turned verbose off turns it back on after the body, and also when it goes out of
scope.  So a body that throws leaves verbose as it found it, and a nested loop
leaves verbose off until the outer body ends.

A run of progress dots that `PrlogDot` writes leaves the stdout log line open,
and `FDb.dotline` records that.  The default `Prlog` ends an open dot line before
it writes any other line, so a logged line never lands after the dots.  A program
that installs its own Prlog handler does not get this.

### A variable is found by walking, not by asking
<a href="#a-variable-is-found-by-walking-not-by-asking"></a>

Consider `amc` generating the tree.  It defines about fifteen variables for each
field of each ctype -- `$ns`, `$Partype`, `$Parname`, `$parname`, `$pararg`,
`$name`, `$cppname` and the rest -- and then substitutes them into a few million
lines of C++.  The names share prefixes heavily, which is not an accident: they
are named after the things they stand for, and the things are related.

Held in a hash, that set answers the wrong question.  A substitution has to find
out which key starts at a given dollar sign, and a hash can only answer whether
some particular string is a key.  So the scanner asked once per candidate length:
it hashed `$`, then `$c`, then `$cp`, then `$cpp`, up to the whole of `$cppname`
and past it to the end of the text when nothing matched.  A nine-character
variable cost nine hashes over prefixes of growing length, nine bucket probes and
nine comparisons.  Defining one cost more: with `strict` set, every proper prefix
of every key had to be registered as a variable of its own so the scanner could
tell "keep going" from "no such key", which is eight more records, allocations
and hash inserts for that same nine-character key.  Two fifths of `amc`'s
instructions went into this.

The insight is that the question is about a position in text rather than about a
string, and the structure that answers it is a trie.  Each node consumes some
characters of a key; a node consuming several is what path compression means, so
a run of characters no key branches on costs one node instead of one per
character.  Substituting walks down from the root one character at a time and
stops at the first node carrying a value, which is by construction the shortest
key matching there -- the rule the old scanner got by probing lengths in
increasing order.

Two things the hash had to be told, the trie states by its shape.  Whether one
key is a prefix of another is the ambiguity `strict` reports, and it is now a
test of two fields: a node naming no variable but having children has variables
below it, which is what makes defining its key ambiguous.  That test is only
sound because of the invariant above -- a node is created either to carry a value
or to split a shared prefix, and none is ever removed on its own, so a node with
no children is a node that names a variable.  And a value owns no string: the
scope keeps every value concatenated in one, a node names its own with a range,
and clearing a scope is a truncation rather than a walk of records with
destructors.

A node's key is an inline `algo.Smallstr10`, so the walk compares against
characters already in hand rather than following a pointer.  A key with more than
ten characters left to spell becomes several nodes, each the only child of the
one before it.  A node remembers its parent and not its own spelling, so reading
a key back -- for an error message, or to print the scope -- is a walk upward.

Re-defining a variable writes the new value where the old one sat when it fits
and appends otherwise, and the scope counts the bytes it orphans.  Once they
outweigh the values still in use the scope rewrites the string, which is what
keeps a long-lived scope from growing by the size of every value it was ever
given.

### Most expressions are not regular
<a href="#most-expressions-are-not-regular"></a>

`Regx` is a Thompson NFA, and simulating one costs a test of every live state
against every character of the subject.  That is the price of matching a regular
expression without backtracking, and it is worth paying for an expression that
uses the generality.  Almost none do.

An acr or sql pattern is a piece of text with `%` where anything may go, so
`%Replscope%` arrives at the engine as `.*Replscope.*`.  The NFA is at its worst
on exactly that shape: a leading `.*` keeps every state alive, so nothing ever
drops out of the front and the cost is the full product of states and characters.
Over 76MB of ssimfiles that expression took 962 ms while `grep` took 8.

So a compiled Regx first asks whether its expression is a fixed string with an
optional `.*` at either end, and records the string when it is.  Where the `.*`
stands decides which search runs: pinned at both ends the subject has to equal
the string, pinned at one end it has to begin or end with it, and pinned at
neither it may contain it anywhere.  The same expression now takes 60 ms, of
which 35 is reading the lines.

This is a different question from the `literal` flag, which they are easy to
confuse.  `literal` says that `Regx.expr` -- the text the caller handed in -- is
itself the string the expression matches, and `acr` reads it to turn a query into
a hash lookup on the primary key.  That has to stay exactly as narrow as it is,
because a caller acts on `expr` when it is set.  The reduction is broader: it
holds for `%Replscope%`, which `literal` must not, and it keeps its own string
rather than pointing at `expr`.

### A state that consumes nothing costs a whole pass
<a href="#a-state-that-consumes-nothing-costs-a-whole-pass"></a>

An lparen or rparen state consumes no character and its test always succeeds.
It exists to record where a capture group opened or closed, and a match that asks
no question about groups still has to walk through it.  Walking through a state
that consumes nothing is not free: the matcher cannot advance the character until
its front is closed under such arcs, so it makes another pass over the whole
front.  One pair of parentheses therefore doubles the cost of matching, and every
expression read with `full:N` is wrapped in one by the reader itself.

So the arcs come in two forms.  `RegxState.next` reaches the same states without
stopping at a boundary, and is what the matcher follows.  `RegxState.nextgroup`
keeps the boundaries, and is followed only when the caller asked for the groups.
Both are produced by one splice, which takes a class of states out of the arcs
and puts each one's own arcs in its place.

That splice is where a bitset's length turns into a defect.  `ary_Setary` copies
a bitset's length along with its contents and `ary_OrBits` ors only the words
both sides have, so a one-word set that receives a two-word set keeps the first
64 bits and drops the rest without saying so.  The splice rebuilds its working
set from a state's arcs on every pass, which is where the length comes from, and
an expression with more than 64 states then lost every successor numbered past
63.  What that looked like was a query naming ten alternatives matching the first
nine, with a threshold sharp enough to move when the prefix in front of the
alternation grew by one character.

The branch splice has always had this shape and never failed, which is an
accident of numbering rather than a property worth relying on: the parser gives a
branch state a higher number than the states it points at, so whoever points at
the branch already has a set wide enough to receive them.  A group's arcs run the
other way.  Anything that ors one bitset into another re-expands the destination
first.

### A group's position belongs to the path, not to the state
<a href="#a-group-s-position-belongs-to-the-path-not-to-the-state"></a>

Where a capture group opened is a fact about the path that reached a state.  One
state stands on as many paths as the expression has ways of reaching it, so a
single remembered position per state is enough only while no two groups are open
at once.  With `(ab(cd)ef)` two are: the outer group opened at 0, the inner at 2,
and one position could hold only the second, so closing the outer group reported
the match as 2..6.

A state now carries one position per group, and an arc copies the whole set
forward, so a group closes on the position its own opening recorded.  The room is
taken only when the caller asked for the groups, and the positions live in the
matching context rather than in the compiled expression, which is why matching
writes nothing into the Regx it matches with.

What this does not reach is two paths arriving at one state having opened a group
in different places.  The last arrival wins, and no arrangement of per-state
positions settles it, because the state is one and the histories are two.
Reaching it means the front becomes a list of threads each carrying its own
positions -- a Pike VM -- which puts a copy per thread per character on every
match rather than on the ones that asked for groups.  Two callers in the tree ask
for groups: `samp_regx` and the regx unit test.

### Why these are two things and not one
<a href="#why-these-are-two-things-and-not-one"></a>

Both matchers answer "which of a set of patterns starts here", and it is tempting
to fold Replscope into Regx by hanging a substitution on each accepting state.
The set is what separates them.

A Regx is compiled once and matched many times, so it can afford to spend on
preparation what it saves per character -- which is the whole reason `grep` wins,
its lazy DFA being preparation carried further than an NFA carries it.  A
Replscope's set changes constantly: `amc` clears a scope and redefines fifteen
variables for every field of every ctype, tens of thousands of times per run.
Each of those would be a recompilation, and compiling an expression costs far
more than inserting into a trie.  The direction that would pay is the reverse
one, and it stops short of a merge: an alternation of fixed strings is a trie
with failure links, so the structure Replscope uses is what Regx wants when it
comes to reduce `(a|b|c)` the way it already reduces a single fixed string.
Sharing that primitive is worth doing; making one engine out of both is not.
