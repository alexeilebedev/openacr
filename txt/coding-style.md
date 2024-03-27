## Coding Style
<a href="#coding-style"></a>

OpenACR is written with a certain uniform coding style. This section 
explains the reasoning behind it. The style is maintained by various tools.
First of all, amc generates all code in this style; 
Second, `indent-recent` and `cpp-indent` automatically indent 
source files, and run automatically on every commit (via CI), enforcing this style.

### Spaces, Indentation
<a href="#spaces--indentation"></a>

Use spaces, use 4 space indent. We use `cpp-indent` to normalize all source files,
so this policy is enforced automatically.

### Variable Names
<a href="#variable-names"></a>

Variable names use `lower_under` style.

### Member Functions
<a href="#member-functions"></a>

There are no member functions. Functions describe actual blocks of instructions for 
CPU to execute; Structs don't even exist -- they are rules for calculating field
offsets and thereby structuring RAM. We maintain this separation.

### Predicate Functions
<a href="#predicate-functions"></a>

Functions that return bool end in Q: VisibleQ, ExistsQ, etc. 
Q means Query or Question mark. This is the Mathematica convention.

### Curly Braces
<a href="#curly-braces"></a>

Curly braces start on the same line both for functions and for control flow operators.
'Else' starts on the same line as the previous closing curly brace. The reason is that
without this rule, the if-block that that ends at page boundary can be
incorrectly perceived as lacking the else clause, inadvertently changing its meaning.

    if (x) {
    }

    if (x) {
    } else if (y) {
    } else {
    }

    void Proc() {
    }

### Split Conditionals
<a href="#split-conditionals"></a>

If you need to split a long line, the connecting operator goes to the new line.

Example:

    if (xyz
        && abc) {
    }

This formatting rule can be derived from principle of incremental construction.

The expression if (xyz) becomes if (xyz && abc)
by adding && abc, so logically the operator groups with the later code.
The side benefit is that commenting out or deleting the additional code is easier.

### Curly Braces around Conditionals are Non-Optional
<a href="#curly-braces-around-conditionals-are-non-optional"></a>

If statements and loops always use curly braces, even if the body is empty.
The reason is again due to incremental construction: adding an extra statement
to an if statement shouldn't entail insertion of three lines. The curly braces
should already be there.

Control structures must be formatted on multiple lines
This rule is important for debugging. A statement like if (x) a=b; requires
going to assembly level to determine if the conditional was taken, or to place a breakpoint
at the assignment.

### Use of semi-colon forces a new line
<a href="#use-of-semi-colon-forces-a-new-line"></a>

    x;
    y;

The reason here is again that debuggers are line-oriented.
All code is versioned in git, so it wouldn't be nice to create spurious
diffs just because someone needed to debug a piece of code.

### Keep code separate from data
<a href="#keep-code-separate-from-data"></a>

Structure space with ssim files and amc.
Structure time with plain C++.

### No Code In Headers
<a href="#no-code-in-headers"></a>

`.h` contain declarations and structure definitions only, never any code.
All inline code codes into `inl.h` headers (after all, it must go somewhere).

### Use query-command separation
<a href="#use-query-command-separation"></a>

Functions can be divided into two types: queries, which are read-only functions that compute
a value; and commands, which are procedures that perform some in-memory update.
Queries are a lot easier to write and debug, since there is no invariant to enforce.
Keep as much code as possible in query form, and keep it separate from command-type functions.
This will reduce debugging time. Bertrand Meyer writes eloquently about this.

### Keep it single-threaded
<a href="#keep-it-single-threaded"></a>

We can easily task thousands of cores with independent tasks, and they will all execute at once.
But let's not task a human with trying to understand two processes simltaneously. Keep programs
single-threaded, and as deterministic (independent of wall clock) as possible,
and your debug times will be finite. Hoare's
[Communicating Sequential Processes](https://www.cs.cmu.edu/~crary/819-f09/Hoare78.pdf) should
serve as an inspiration.

### Use Single Entry, Single Exit (SESE) style
<a href="#use-single-entry--single-exit--sese--style"></a>

Do not use the continue statement (break is OK, it is sometimes necessary to terminate a search).
Use return only at the end of a function. SESE has several sound reasons behind it.
One is the desire to be able to extend the program with minimal effort.
It is natural to replace a program that does X with a program that does X, then Y.
Only SESE style makes this possible by writing ``X; Y''. For this, X cannot have hidden exits.
SESE is also a normal form, i.e. a solution that two developers can agree on without communicating,
so it is scalable.
Finally, SESE dictates that code of the program is simply a text representation of its control
flow graph. All arcs in the graph are made visible, and this facilitates reasoning about program
correctness. Bertrand Meyer is a big proponent of this style.
See [Wiki Entry](https://en.wikipedia.org/wiki/Single-entry_single-exit)

### Single File Static Assignment
<a href="#single-file-static-assignment"></a>

Single Static Assignment (SSA) is when  there is only one location in the source tree
where each variable is assigned to. This is preferable but not always achievable, but it
should be kept in mind.

All assignments to a given variable must be in the same source file.
This is necessary for reasoning about correctness, debugging, and enforcing post-conditions
of given variable assignment. See Andrew Appel's
[SSA Is Functional Programming](https://www.cs.princeton.edu/~appel/papers/ssafun.pdf)

### Document all non-static functions
<a href="#document-all-non-static-functions"></a>

Non-static functions are API functions. Each one needs to be documented, explaining
the reason for its existence, side effects, pre- and post-conditions,
and any other rules for caller/callee not enforceable at the language level.

### All rules allow exceptions
<a href="#all-rules-allow-exceptions"></a>

Given a sufficiently good reason, any rule can be replaced. But let's try to keep a consistent
whole by following these rules whenever possible.

