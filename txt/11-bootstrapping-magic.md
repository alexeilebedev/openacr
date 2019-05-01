## Bootstrapping Magic

Let's consider the three main tools in this repository: acr, amc and abt.

The three tools are interdependent:
 acr, abt and amc all use acr to manage and validate their inputs.
 acr, abt and amc's in-memory structures are all generated with amc
 acr, abt and amc are all build using abt

There are several examples of such systems in the world.
The most famous one is the LISP interpreter as expressed in LISP itself.
Another example is gcc after it was compiled by gcc that was compiled by gcc.

Humbly joining this hallowed group is amc, which generates most
of its own source code as C++ and lets you modify this source code with
perl, sed and MariaDB from the bash command line, without introducing
a new language or an interpreter.

Why is this important?
You see, when a LISP interpreter interprets LISP, the new interpreter is
qualitatively different from the old one: it is slower. It can only
run smaller jobs than its parent.

Similarly, the C++ template language is less powerful than the parent language.
If you go back one step and extended C++ itself by changing its grammar,
and update the compiler accordingly, you get a new language that's not C++, and
cannot be mixed with C++.

So, after one cycle of application of these tools (LISP interpretation, C++ templates),
you get to a new and better place, but that place is
temporary, and you don't lock in the gains. Yet *it is* possible to
lock in the gains. For that, we need a tool whose input is about equally readable and writable
by both human and the machine, and where most of the source code is derived
from this input. That way, we skip the slow interpretation layer, and
get to lock in any iterative improvements.
And we want to avoid creating a new language, at all costs --
a conservative subset of C++ is all you need to write anything.

In the world where a lot of source code is generated from a machine-readable
format, you encounter seemingly circular situations:

    'acr field:dmmeta.Field.field' describes its own primary key.
    'acr ctype:dmmeta.Ctype' describes the struct type (C type).

    'amc acr.%' generates from scratch (most of) the source code for acr
    'amc amc.%' generates from scratch the source code itself.

    'src_func abt' shows the hand-written source code of abt.
    'src_func src_func' shows the hand-written source code of itself.

    'acr_in -data amc' shows all of the inputs that amc takes
    'acr_in -data acr_in' shows all of the inputs for itself.

    'acr_compl -line amc' shows bash completions for amc
    'acr_compl -line acr_compl' shows bash completions for acr_compl itself

    'acr ns:abt -t' shows the definitions of all abt structures
    'acr ns:acr -t' shows the definitions of its own structures

    'abt acr' builds acr.
    'abt abt' builds itself.


