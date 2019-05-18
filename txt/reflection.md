## Reflection

`amc` bakes some information about a process into the process itself.
For each each namespace linked into a process, possessing an in-memory database, an `algo_lib.FImdb`
entry is defined, and added to the `algo_lib.FDb.imdb` table. This table is indexed
with `algo_lib.FDb.ind_imdb`.

Without loading any ssimfiles, a process can access the `imdb` records.
Each record has the following fields:

* `imdb`: primary key. (e.g. `algo_lib` or `sample`)
* `InsertStrptrMaybe`: pointer to a function that takes a strptr and inserts it into this namespace's in-memory database.
* `Step`: pointer to a function that executes a single scheduler cycle.
* `MainLoop`: pointer to the main loop function, if defined
* `GetTrace`: pointer to a function that reads all trace variables from the process

In addition, the `algo_lib.FDb.imtable` table contains the list of all in-memory tables.
`Imtable` is also indexed, with `ind_imtable`, and has the followin fields (`amc -e algo.Imtable`):

* `imtable`: primary key
* `elem_type`: A string holding the ctype pkey of the records in this table
* `c_RowidFind`: Optional pointer to a function to find an element by rowid
* `XrefX`: Optional function to x-reference the record
* `NItems`: Optional function that returns the number of records in the table
* `Print`: Optional function to print one element as a string
* `size`: With of each record
* `ssimfile`: Pkey of ssimfile, if one is associated with the record.

Reflection is generally considered a very powerful mechanism, but in the OpenACR world
it's not used that frequently. While the ability to insert records dynamically (i.e. outside
of a pre-declared code-path) and knowing the list of namespace linked into a given program is useful,
it's not as useful as simply loading the ssimtables that describe *all* processes in the 
given universe, and doing something with them. An analogy would be surgery on yourself,
especially brain surgery. Powerful? Yes. Best practice? Hardly.

