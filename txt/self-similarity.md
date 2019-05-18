## Self-Similarity

Looking at the three main tools in OpenACR, `acr`, `amc` and `abt`,
we see that they all seem to be circularly dependent on each other.

* acr, abt and amc all use acr to edit the description from which their source code is derived.
* acr, abt and amc's in-memory structures are all generated with amc.
* acr, abt and amc are all build using abt.

This circularity comes from the fact that these tools' outputs
also affect their inputs, and you're looking at a system that underwent 
perhaps a few thousand cycles of evolution, with human programmers completing
the loop.

There are many instances of self-similar systems in the software world,
where the rules created by a system begin to apply to the system itself.
Let's consider three other examples.

* The most famous one is the LISP interpreter as expressed in LISP itself.
* Another example is the [self-compiling compiler](#https://en.wikipedia.org/wiki/Bootstrapping_(compilers)).
All compiled languages have one (and *no* interpreted language has one).
* And a third example is the template meta-programming sublanguage of C++,
using which you can manipulate the very types from which the underlying
C++ program is written.

Joining this list is OpenACR, which is of a different kind:
It not only generates most of its own source code and lets you modify this source code with
plain command-line tools like sed and awk, and even SQL, without introducing
any new language or an interpreter; but it serves as a sort of planter from which
you can grow other applications that share these same properties. 

Why is this important?
Let's go back to our three examples and consider one cycle of application of them.

* When a LISP interpreter written in LISP interprets more LISP, it is
qualitatively different: it is slower. It can only
run smaller jobs than its parent. In order to be the same, the homoiconic interpreter
would have to be vastly different; At the very least it would have to contain a
memory model of the underlying computer and its file system, so it could then target them.
That's why no interpreted language today uses a self-hosting interpreter -- nobody
wants to pay for the slowdown.
* The output of a self-compiling compiler is an object file -- unreadable
for all practical purposes. So even though the compiler can compile itself, and the resulting
compiler can run even faster than the one before (the opposite of what happens in LISP),
this is a one-time gain.
* Finally, the C++ template sublanguage, our third example, is strictly less powerful
than its parent language; you can't loop over the fields of a struct, 
or check how many structs are defined, or if the name of a function contains an uppercase S.
Neither the C++ language, nor its template sublanguage contain words that 
describe themselves.

So, after one cycle of application, you get to a new and better place, but that place is
either inaccessible (e.g. object file), or built at some unmaintainable expense;
in either case, the gains are temporary. Yet *it is* possible to lock them in.
For that, we need tools whose input is readable and writable
by both human and the machine, where source code is derived
from this input, and where the system of names applies equally well to the 
description of itself and the tools. And of course, the tools should be useful for other
purposes, not just generating their own source code; that's merely a byproduct.

Creating a new language is not the answer, because the source code for that
language is a whole new thing. And as you define both the language and its 
debuggers, multi-platform support, you will need new words for referring
to them, words that have to be written somewhere... it becomes a chasing game. 

In the world where data is kept in a machine-readable format, and most of the source code
is generated, any tool works with almost any other tool. 

    'abt acr' builds acr.
    'abt abt' builds itself.

    'acr field:dmmeta.Field.field' describes its own primary key.
    'acr ctype:dmmeta.Ctype' describes the struct type (C type).

    'amc acr.%' generates from scratch (most of) the source code for acr
    'amc amc.%' generates from scratch the source code of itself.

    'src_func abt' shows the hand-written source code of abt.
    'src_func src_func' shows the hand-written source code of itself.

    'acr_in -data amc' shows all of the inputs that amc takes
    'acr_in -data acr_in' shows all of the inputs that it takes.

    'acr_compl -line amc' shows bash completions for amc
    'acr_compl -line acr_compl' shows bash completions for acr_compl itself

    'acr ns:abt -t' shows the definitions of all abt structures
    'acr ns:acr -t' shows the definitions of its own structures

    'mdbg acr' debugs acr
    'mdbg "mdbg acr"' debugs the debugger debugging acr (in principle)
    
And of course, as already mentioned above, even though this is amazingly fun, 
the point of these tools is not to compile themselves; 
The point is to allow the creation of new applications,
using ssim files to describe new domains while continuing
to apply the same small set of tools -- bash, acr, perl, etc. on each cycle.
