## atf_unit: Unit Tests

atf\_unit runs single-function tests which are linked into atf\_unit.
The test source code is under cpp/atf/unit, and the names of all defined tests are in the table
atfdb.unittest.

Example: Create new unit test

    $ acr_ed -create -unittest amc.Test1

Example: Create and edit a new unit test

    $ acr_ed -create -unittest amc.Test1 -e

Example: Run a single unit test

    $ atf_unit http_lib.Http09

Example: Run several unit tests

    $ atf_unit http_lib.%

Example: Debug a unit test

    $ atf_unit http_lib.Http09 -debug


