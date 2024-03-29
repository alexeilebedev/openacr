## atf_cov - Line coverage


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Description](#description)
* [Options](#options)
* [Test coverage](#test-coverage)
* [Code coverage](#code-coverage)
* [Line coverage](#line-coverage)
* [One-liner](#one-liner)
* [Changing directory to store coverage data](#changing-directory-to-store-coverage-data)
* [Running instrumented executable](#running-instrumented-executable)
* [Redirecting log to a file](#redirecting-log-to-a-file)
* [Running Gcov](#running-gcov)
* [Treating source files as infeasible](#treating-source-files-as-infeasible)
* [Generating coverage reports](#generating-coverage-reports)
* [Writing out in-memory coverage database to files](#writing-out-in-memory-coverage-database-to-files)
* [Merging data from multiple runs](#merging-data-from-multiple-runs)
* [Loading ssim coverage data from prevous run](#loading-ssim-coverage-data-from-prevous-run)
* [Checking and capturing coverage](#checking-and-capturing-coverage)
* [Viewing coverage summary](#viewing-coverage-summary)
* [Output files](#output-files)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
atf_cov: Line coverage
Usage: atf_cov [options]
    OPTION      TYPE    DFLT                              COMMENT
    -in         string  "data"                            Input directory or filename, - for stdin
    -covdir     string  "temp/covdata"                    Output directory to save coverage data
    -logfile    string  ""                                Log file
    -runcmd     string  ""                                command to run
    -exclude    regx    "(extern|include/gen|cpp/gen)/%"  Exclude gitfiles (external, generated)
    -mergepath  string  ""                                colon-separated dir list to load .cov.ssim files from
    -gcov                                                 run gcov
    -ssim                                                 write out ssim files
    -report                                               write out all reports
    -capture                                              Write coverage information into tgtcov table
    -xmlpretty                                            Generate pretty-formatted XML
    -summary            Y                                 Show summary figures
    -check                                                Check coverage information against tgtcov table
    -verbose    int                                       Verbosity level (0..255); alias -v; cumulative
    -debug      int                                       Debug level (0..255); alias -d; cumulative
    -help                                                 Print help and exit; alias -h
    -version                                              Print version and exit
    -signature                                            Show signatures and exit; alias -sig

```

### Description
<a href="#description"></a>

Atf_cov is a coverage measurement tool, which is aimed at measuring test coverage for source code lines.
It runs instrumented executable, gathers and processes coverage data, and produces coverage reports.
Atf_cov provides a utility for GNU Gcov-based code coverage measurement.

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -covdir -- Output directory to save coverage data
<a href="#-covdir"></a>

#### -logfile -- Log file
<a href="#-logfile"></a>

#### -runcmd -- command to run
<a href="#-runcmd"></a>

#### -exclude -- Exclude gitfiles (external, generated)
<a href="#-exclude"></a>

#### -mergepath -- colon-separated dir list to load .cov.ssim files from
<a href="#-mergepath"></a>

#### -gcov -- run gcov
<a href="#-gcov"></a>

#### -ssim -- write out ssim files
<a href="#-ssim"></a>

#### -report -- write out all reports
<a href="#-report"></a>

#### -capture -- Write coverage information into tgtcov table
<a href="#-capture"></a>

#### -xmlpretty -- Generate pretty-formatted XML
<a href="#-xmlpretty"></a>

#### -summary -- Show summary figures
<a href="#-summary"></a>

#### -check -- Check coverage information against tgtcov table
<a href="#-check"></a>

### Test coverage
<a href="#test-coverage"></a>

ISO/IEC/IEEE 29119-2 defines **test coverage** as degree expressed as a percentage,
to which specified *coverage items* have been exercised by a test case or test cases.

**Test coverage item** is an attribute or combination of attributes that is derived
from one or more test conditions by using a test design technique
that enables the measurement of thoroughness of test execution.

ISO/IEC/IEEE 29119-4 defines **test coverage measures** as based on different degrees
of coverage achievable by test design techniques.
Test coverage levels can range from 0% to 100%.

When calculating coverage for any test design technique,
the following formula shall be used:

```
         N
    C = --- * 100%
         T
```

where:
- *C* - coverage achieved by specific test design technique;
- *N* - number of test coverage items covered by executed test cases;
- *T* - total number of test coverage items identified by test design technique.

In each coverage calculation, a number of coverage items may be infeasible.
A test coverage item shall be defined to be infeasible if it can be shown
to not to be executable or impossible to be covered by a test case.
The coverage calculation shall be defined as either counting or discounting
infeasible items.

For theory in deep, refer to ISO/IEC/IEEE 29119-4.

### Code coverage
<a href="#code-coverage"></a>

**Code coverage** is a structure-based test design technique,
which defines test coverage items as a number of source code constructs:
- executable statements;
- branches;
- decision outcomes;
- branch conditions (Boolean values within decisions + total decision outcomes);
- branch condition combination (unique combinations of Boolean values of conditions within decisions).

Code coverage addresses test design from the point of of source code quality.

Code coverage measurement helps to:
- ensure that tested program parts are actually exercised;
- detect untested program parts;
- detect dead code.

## Line coverage
<a href="#line-coverage"></a>

Line coverage defines coverage item as executable line of source code.
Depending on coding style used, executable line of code  more or less
closely relates to executable statement.
For better results, one-statement-per-line style should be preferred.

Line coverage measurement may be performed on automated manner
by instrumentation of source code by the compiler.

After running of instrumented code under the test,
hit counters for all source lines are available.

Not all source lines are executable, e.g. comment lines, blank lines,
declarations, POD type definitions etc.
Never instantiated templates and never called inline functions may also be marked as non-executable.

Exercised lines may be marked as 'partially exercised' if one line contains several branches,
examples are one-line conditionals, like `&&`- or `||`-chains `if`, ternary and other constructs.
Partially executed lines are counted as executed.

Lines within automatically generated files, system and third-party libraries,
although may be hit or not are typically treated as infeasible, and thus discounted.

### One-liner
<a href="#one-liner"></a>

One liner to perform whole processing at once:

```
abt % -cfg coverage && \
atf_cov -covdir temp/covdata -runcmd build/coverage/acr -gcov -ssim -report
```

Here first subcommand builds instrumented executable,
second subcommand runs it and processes coverage results.

`-runcmd build/coverage/acr` tells to run instrumented executable **build/coverage/acr**.

`-gcov` tells to invoke Gcov to get GNU GCC coverage data in Gcov format.

`-ssim` tells to save aggregate coverage data in ssim format.

`-report` asks to produce reports.

### Changing directory to store coverage data
<a href="#changing-directory-to-store-coverage-data"></a>

By default, all temporary and output files are stored on **temp/covdata** directory.

Each run of new command rewrites all the results from previous run.

Use `-covdir` to change this directory.

When running multiple testcases, each testcase must be run on own directory,
and then it is possible to merge all the results.

**WARNING:** Coverage directory has temporary nature, thus it is not recommended
to use any non-empty directory such as project root, or home directory for that purpose.
Atf_cov may produce huge amount of files with extremely long names.
It may not be so easy to clean up these files manually.
Removing whole directory with all the files inside is much easier.

### Running instrumented executable
<a href="#running-instrumented-executable"></a>

Use `-runcmd` option to specify which command to run, with command line arguments if needed.

Instrumented executable for the command must be compiled and linked
with options appropriate to the compiler to produce GNU GCC coverage database files (.gcda)
written on  directory specified on environment variable `GCC_PROFILE_DIR`.

GCC options are:
```
-ftest-coverage -fprofile-arcs -fprofile-dir=%q{GCC_PROFILE_DIR} -coverage
```

Prior to running command, atf_cov sets `GCC_PROFILE_DIR` to the value given in `-covdir` option,
and searches coverage database files (.gcda) files there.

The motivation of such solution is in avoiding of overwriting coverage database
during parallel test run.

### Redirecting log to a file
<a href="#redirecting-log-to-a-file"></a>

Use `-logfile` to redirect atf_cov program log to a file.
This is useful when wrapping `-runcmd` in the pipeline, in order to not garble command output.

Example:

Original pipeline:
```
producer | command | consumer
```

Wrong wrapping:
```
producer | atf_cov -runcmd command | consumer
```

Right wrapping:
```
producer | atf_cov -runcmd command -logfile atf_cov.log | consumer
```

### Running Gcov
<a href="#running-gcov"></a>

Use `-gcov` to run Gcov.

Gcov requires program graph files (.gcno)
with the same name as corresponding coverage database file (.gcda)
placed in the same directory.
Atf_cov prepares them as symbolic links to files generated by GCC, then runs Gcov.

Gcov produces text coverage files (.gcov) on the current directory.

Atf_cov moves relevant .gcov files into directory specified by `-covdata`,
parses these files into in-memory database, merging line hit counts if needed.

Source files, those are not a part of any target (`dev.targsrc` table)
are automatically treated as infeasible, this behavior can not be changed.

For more info on GNU Gcov, refer to (https://gcc.gnu.org/onlinedocs/gcc/Gcov.html).

### Treating source files as infeasible
<a href="#treating-source-files-as-infeasible"></a>

In order to filter-out source files as infeasible,
use `-exclude` option, which accepts SQL regular expression
for file path within git repository.
By default, it is initialized with pattern `(extern|include/gen|cpp/gen)/%`,
which excludes external and AMC-generated files.

### Generating coverage reports
<a href="#generating-coverage-reports"></a>

Use `-report` to generate coverage reports in txt, ssim, and cobertura XML formats.

Reports contains summary figures for files, targets, total.
Total figures are broken down by targets, which in turn are broken down by files.

Total- and target-based data format is defined as `dev.covtarget` table:
```
inline-command: acr dmmeta.field:dev.Covtarget.%
dmmeta.field  field:dev.Covtarget.covtarget  arg:dev.Target    reftype:Pkey  dflt:""  comment:Target
dmmeta.field  field:dev.Covtarget.total      arg:u32           reftype:Val   dflt:""  comment:"Total lines"
dmmeta.field  field:dev.Covtarget.nonexe     arg:u32           reftype:Val   dflt:""  comment:"Non-executable lines"
dmmeta.field  field:dev.Covtarget.exe        arg:u32           reftype:Val   dflt:""  comment:"Executable lines"
dmmeta.field  field:dev.Covtarget.exer       arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Percentage of executable lines"
dmmeta.field  field:dev.Covtarget.hit        arg:u32           reftype:Val   dflt:""  comment:"Exercised lines"
dmmeta.field  field:dev.Covtarget.cov        arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Line coverage"
report.acr  n_select:7  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```
For total figures hard-coded keyword 'TOTAL' is used as target name.

File-based data format is defined as `dev.covfile` table:

```
inline-command: acr dmmeta.field:dev.Covfile.%
dmmeta.field  field:dev.Covfile.covfile  arg:dev.Gitfile   reftype:Pkey  dflt:""  comment:"Source file"
dmmeta.field  field:dev.Covfile.total    arg:u32           reftype:Val   dflt:""  comment:"Total lines"
dmmeta.field  field:dev.Covfile.nonexe   arg:u32           reftype:Val   dflt:""  comment:"Non-executable lines"
dmmeta.field  field:dev.Covfile.exe      arg:u32           reftype:Val   dflt:""  comment:"Executable lines"
dmmeta.field  field:dev.Covfile.exer     arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Percentage of executable lines"
dmmeta.field  field:dev.Covfile.hit      arg:u32           reftype:Val   dflt:""  comment:"Exercised lines"
dmmeta.field  field:dev.Covfile.cov      arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Line coverage"
report.acr  n_select:7  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

Txt format is human-friendly tabulated version of ssim data.

Cobertura XML format is utilized by GitLab CI,
refer to DTD http://cobertura.sourceforge.net/xml/coverage-04.dtd

### Writing out in-memory coverage database to files
<a href="#writing-out-in-memory-coverage-database-to-files"></a>

For manual analysis, sometimes necessary to see aggregated by-line hit counts.
To write out aggregated in-memory database to `-covdata` directory, use '-ssim' option.
This option is not enabled by default.
For each source file, atf_cov writes .ssim coverage file.
By Gcov convention, path separator character '/' in original source file name
is replaced by '#' in order to avoid creating deep directory structure.

Ssim file format is described as 'dev.covline' table:

```
inline-command: acr dmmeta.field:dev.Covline.%
dmmeta.field  field:dev.Covline.covline  arg:algo.Smallstr200  reftype:Val   dflt:""     comment:file:line
dmmeta.field  field:dev.Covline.src      arg:dev.Gitfile       reftype:Pkey  dflt:""     comment:"Source file"
dmmeta.field  field:dev.Covline.line     arg:u32               reftype:Val   dflt:""     comment:"Source line"
dmmeta.field  field:dev.Covline.flag     arg:char              reftype:Val   dflt:"'N'"  comment:Flag
dmmeta.field  field:dev.Covline.hit      arg:u32               reftype:Val   dflt:""     comment:"Number of hits"
dmmeta.field  field:dev.Covline.text     arg:algo.cstring      reftype:Val   dflt:""     comment:"Line text"
report.acr  n_select:6  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

Key of this table is colon-separated pair (source file, source line).

Line is flagged as follows:
```
inline-command: acr dmmeta.fconst:dev.Covline.%
dmmeta.fconst  fconst:dev.Covline.flag/N  value:"'N'"  comment:Non-executable
dmmeta.fconst  fconst:dev.Covline.flag/E  value:"'E'"  comment:Executable
dmmeta.fconst  fconst:dev.Covline.flag/P  value:"'P'"  comment:"Executable, partially executed"
report.acr  n_select:3  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

### Merging data from multiple runs
<a href="#merging-data-from-multiple-runs"></a>

In order to merge data from multiple runs, use '-mergepath' option,
argument is colon-separated list of directories where coverage data being merged reside.
This option loads and aggregates coverage database from all these directories.

Coverage data being loaded may be represented either as bare .gcda files (unprocessed),
or as ssim coverage database (already processed by `-gcov` and written out with `-ssim`)

To merge **ssim** coverage data from *dir1* and *dir2*, and generate reports on *dir3*:
```
atf_cov -mergepath dir1:dir2 -covdir dir3 -report
```

To merge **gcda** coverage data from *dir1* and *dir2*, and generate reports on *dir3*:
```
atf_cov -mergepath dir1:dir2 -covdir dir3 -report -gcov
```

Unlike former command, latter one runs Gcov on each `-mergepath` directory.

### Loading ssim coverage data from prevous run
<a href="#loading-ssim-coverage-data-from-prevous-run"></a>

Unless `-gcov` or `-mergepath` is specified, atf_cov automatically loads saved ssim coverage
data from `-covdir` directory. In such a way, needed reports could be generated later
from ssim files written with `-ssim` option, without rerunning command and gcov..

### Checking and capturing coverage
<a href="#checking-and-capturing-coverage"></a>

To save target figures in order to be checked against later, run:

```
atf_cov -capture
```

To check figures against saved, run:

```
atf_cov -check
```

If measured coverage falls lower than saved one, error message is displayed, and check fails.

### Viewing coverage summary
<a href="#viewing-coverage-summary"></a>

Short summary is automatcally displayed on the tool output, to suppress it use `-summary:N`.
This summary report is generated in file with `-report`.

### Output files
<a href="#output-files"></a>

Atf_cov generates output files on `-covdata` directory;
- \*.gcda - coverage database files written by GCC-instrumented executable;
- \*.gcno - symbolic links GCC-generated program graph files;
- \*.gcov - coverage data in Gcov format;
- \*.ssim - aggregated line coverage in-memory database written by atf_cov
- report.ssim - coverage report in ssim format
- report.txt - coverage report in txt format
- summary.txt - summary figures in txt format
- cobertura.xml - coverage report in cobertura xml format

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf/atf_cov.cpp](/cpp/atf/atf_cov.cpp)||
|[cpp/gen/atf_cov_gen.cpp](/cpp/gen/atf_cov_gen.cpp)||
|[include/atf_cov.h](/include/atf_cov.h)||
|[include/gen/atf_cov_gen.h](/include/gen/atf_cov_gen.h)||
|[include/gen/atf_cov_gen.inl.h](/include/gen/atf_cov_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`atf_cov` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.tgtcov](/txt/ssimdb/dev/tgtcov.md)|Captured line coverate information by target|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dev.covtarget](/txt/ssimdb/dev/covtarget.md)||
|[dev.covline](/txt/ssimdb/dev/covline.md)||
|[dev.covfile](/txt/ssimdb/dev/covfile.md)||
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

