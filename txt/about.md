## About

The home for this project is at https://github.com/alexeilebedev/openacr

This is OpenACR, an open-source version of acr, which stands for Auto Cross
Reference, and its related tools.
It is the result of over 10 years of development and
production use. The tools were initially created by Alexei Lebedev (me) at AlgoEngineering
with the goal of formalizing construction of low-latency software of higher quality;
but they ended up being suitable for all sorts of other things, and the code generation
part took on a life of its own, eventually generating most of its own source code.

The tools were licensed to Intercontinental Exchange as part
of a project called Pillar to rewrite all of NYSE's electronic exchanges.

As of this writing, there are several national electronic markets
written entirely in acr/amc, from matching engines to development tools,
with amc responsible for over 95% of all executable code (>4 million LOC)
and acr handling all configurations -- from NICs and ip addresses to bit fields and
 priority queues.

Since the tools are domain-agonostic, it was decided that open-sourcing
them would be the best way to ensure their longevity and value, and
also a way to give back to the community. After
all, if it weren't for Linux, gcc and the shell, this project wouldn't have
been possible in the first place. And so here we are.

OpenACR is in an interesting position of having been co-developed with a large
software project, and later extracted back from it. Its usability
on small projects is assumed but not proven. The ideal use case is growing,
in-place, an ecosystem of commands and corresponding configuration data
files around this kernel.

### Contributors

Here are some of the names of git authors who contributed code to this project,
in alphabetical order by first name. 

* Alexey Polovinkin
* Ara Aslyan
* Ezequiel Fernandes
* Hank Wang
* Hayk Mkrtchyan
* Jeffrey Wang
* Jeremy Xue
* John Brzezniak
* Jonathan Joshua
* Luke Huang
* Shreejith Billenahallilokegowda
* Vladimir Parizhsky

There were also many others who helped immensely with
discussions and influenced various design decisions.

Enjoy,

Alexei Lebedev
April 29, 2019

### Testimonials

#### 1

    Alexei has built a tool set and framework that significantly improves
    the software development process with large percentage of the code
    generated with guaranteed consistency and quality. 

        Shreejith Billenahalli
        Director of Technology, Software Architecture, Development & Delivery
        at Intercontinental Exchange

#### 2 

    Alexei's ACR and AMC is a unique, transformative, and extendible tool
    that models and implements the relationships between data,
    configurations, components, and anything else that can be defined. The
    code that it generates leads to reduced development efforts, improved
    quality, excellent performance, and a single coding standard across
    the entire codebase.

        Jon Joshua
        Development Lead at New York Stock Exchange

#### 3

    Alexei has created a paradigm-shifting methodology for systems that
    require extreme stability while having a high-frequency release
    cycle. His model compiler in combination with pre-commit invariant
    checking has enabled a big team of developers to produce very
    high-quality products in very short periods of time while working on a
    single codebase
 
        Hayk Mkrtchyan
        Director at Intercontinental Exchange
 
#### 4

    Alexei envisioned a new HPC programming paradigm, wrote a programming
    language and compiler for it, and invented middleware with a
    blockchain-like transactional algorithm. The resulting unique and
    powerful programming environment, that I have the privilege using at
    AlgoEngineering and NYSE, formed the foundation for NYSE Pillar
    exchange architecture. NYSE Pillar currently powers 4 markets, with
    more on the way.

        Vladimir Parizhsky
        SVP, Systems Architecture at Intercontinental Exchange
