## amc_gc: AMC garbage collector

amc_gc is a tool for removing unused records from the dmmeta database.

amc_gc takes a target regex and an acr query as an argument. It finds all records
matching the query, then it tries to remove them one by one, and rebuilds the targets
(using abt) in a sandbox dir. If build succeeds with one of the records deleted
it means it wasn't needed in the first place.

Eliminate all ctypes in amc without which amc can be built

    $ amc_gc -target:amc -key:ctype:amc.%

