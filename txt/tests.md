## Tests

There are two kinds of tests in OpenACR, unit tests and normalization tests.
Unit tests just check that some functions do what they should.
Normalization tests enforce various invariants on this repository, such as checking
that all files have an entry in the gitfile table, or round-tripping all data values
through a temporary MariaDB instance to ensure portability. In addition, all of the unit
tests are included here for convenience. You can run all possible tests with

    $ atf_ci
    ...

If you just want to invoke unit tests, you can run `atf_unit` directly.
It is split into a separate executable from `atf_ci` for reasons of manageability.

    $ atf_unit
    ...

To run all tests with a syntax highlighter, use the `normalize` scriptlet:

    $ normalize
    ...

Every commit should satisfy `normalize`.

### Unit Tests

`atf_unit` runs single-function tests which are linked into `atf_unit`.
The test source code is under cpp/atf/unit, and the names of all defined tests are in the table
`atfdb.unittest`. You can see what all the tests are with `acr unittest`.

Let's begin by creating a new unit test:

    $ acr_ed -create -unittest algo_lib.Test1 -write
    ...

This creates a new entry in `atfdb.unittest`, and adds a stub to `cpp/atf/unit/algo_lib.cpp`.

    +// --------------------------------------------------------------------------------
    +
    +void atf_unit::unittest_algo_lib_Test1() {
    +    // test code goes here
    +}

Alternatively, we could proceed manually:

    $ echo atfdb.unittest unittest:algo_lib.Test1 | acr -insert -write
    ...
    $ amc
    ...

Or even:

    $ vim data/atfdb/unittest.ssim

After implementing the test, we can run it with `atf_unit algo_lib.Test`

We can run all unit tests from our namespace with

    $ atf_unit algo_lib.%
    ...

### Normalization Checks

We can create a normalization check with :

    $ acr_ed -create -normcheck <name> -write
    ...

Alternatively, we could do:

    $ echo atfdb.normcheck normcheck:mycheck | acr -insert -write
    $ amc

And then implement the function `atf_ci::citest_mycheck()` somewhere
in a file that links with `atf_ci`.

### Debugging

If a test fails, the easiest way to debug it is to re-run `atf_unit` with
`-debug` flag. It will use `mdbg` and automatically set a breakpoint at the first
line of the test in question.
